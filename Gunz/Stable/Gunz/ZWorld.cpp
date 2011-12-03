#include "stdafx.h"
#include "ZWorld.h"
#include "ZMap.h"
#include "ZMapDesc.h"
#include "ZSkyBox.h"
#include "ZInitialLoading.h"

ZWorld::ZWorld() : m_pBsp(NULL), m_pMapDesc(NULL), m_pSkyBox(NULL), m_nRefCount(1), m_bCreated(false)
{
	m_szName[0]=0;

	m_bFog = false;
	m_dwFogColor = 0xFFFFFFFF;
	m_fFogNear = 0;
	m_fFogFar = 0;

	m_bWaterMap = false;
	m_fWaterHeight = 0.f;

}

ZWorld::~ZWorld()
{
	Destroy();
}

void ZWorld::Update(float fDelta)
{
	m_waters.Update();
	m_flags.Update();
}

namespace RealSpace2 {
extern void ComputeZPlane(rplane *plane,float z,int sign);
}


void ZWorld::Draw()
{
	if( m_pSkyBox != NULL )
	{
 		m_pSkyBox->Render();
	}

	// farz clipping �� ���� farz plane �� �ٽ� ������ش�
	if(m_bFog) {
		ComputeZPlane(RGetViewFrustum()+5,m_fFogFar,-1);
	}

	m_pBsp->Draw();

	//RGetDynamicLightManager()->Update();

	RealSpace2::g_poly_render_cnt = 0;

	__BP(16,"ZGame::Draw::flags");

	m_flags.Draw();

	__EP(16);
}

void ZWorldProgressCallBack(void *pUserParam,float fProgress)
{
	ZLoadingProgress *pLoadingProgress = (ZLoadingProgress*)pUserParam;
	pLoadingProgress->UpdateAndDraw(fProgress);
}

bool ZWorld::Create(ZLoadingProgress *pLoading )
{
	if(m_bCreated) return true;
	
	m_pBsp = new RBspObject;
// #ifndef _DEBUG
// 	const char* descExt = "xml.mef";
// #else
	const char* descExt = "xml";
/*#endif*/
	if(!m_pBsp->Open(m_szBspName, descExt, RBspObject::ROF_RUNTIME,ZWorldProgressCallBack,pLoading))
	{
		MLog("error while loading %s \n",m_szName);
		// �ʷε��� �����ϸ� �����ص� �������� ��� �����ش�.(���׼���:�ε��� �����Ұ�� ���е� �������� �׳� ���� �߾���)
		ZGetWorldManager()->Clear();
		return false;
	}

	m_pBsp->OptimizeBoundingBox();


	char szMapPath[64] = "";
	ZGetCurrMapPath(szMapPath);

	ZWater*		water_instance;
	RMapObjectList* map_object_list		= m_pBsp->GetMapObjectList();
	RMeshMgr* mesh_mgr					= m_pBsp->GetMeshManager();

	for( RMapObjectList::iterator iter = map_object_list->begin(); iter != map_object_list->end(); )
	{
		ROBJECTINFO* object_info		= *iter;
		RMesh* pMesh						= mesh_mgr->GetFast( object_info->nMeshID );
		if( pMesh->m_data_num <= 0 )
		{
			++iter;
			continue;
		}
		RMeshNode* pMeshNode		= pMesh->m_data[0];

		char* object_name = (char*)object_info->name.c_str();

//		m_pBsp->m_filename.c_str()
		int len = int(strlen(m_szName)+1);
		object_name += len;
/*
		while(1) {
			if( object_name[0] == '_' ) {
				++object_name;
				break;
			}
			++object_name;
		}
*/
		if( pMeshNode->m_point_color_num > 0 ) // �ʿ��� Į���� �ϳ� �̻� ���� ���� ����̴�.. ����..
		{
			ZClothEmblem* new_instance	= new ZClothEmblem;
			new_instance->CreateFromMeshNode( pMeshNode , this );
			m_flags.Add( new_instance, object_name );
			iter	= map_object_list->Delete( iter );
			continue;
		}

		// Sky Box...
		if( m_pSkyBox == NULL )
		{
			if( strncmp(object_name,"obj_sky_", 8) == 0  || 
				strncmp(object_name,"obj_ef_sky", 10) == 0 )
//			if( strncmp(object_name,"obj_sky_", 8) == 0 ) 
			{
				m_pSkyBox	= new ZSkyBox;
				m_pSkyBox->Create( pMesh->m_pVisualMesh );
//				iter = map_object_list->Delete( iter );
				ROBJECTINFO *info=*iter;
				delete info;
				iter = map_object_list->erase( iter );
				continue;
			}
		}

		// Water
//		if( g_pGame->m_bReflection )
		{
			// �ӽ�....
			int nWater = 0;

			if( !strncmp( object_name, "obj_water", 9 ) )	nWater = 1;
			if( !strncmp( object_name, "obj_water2", 10 ) )	nWater = 3;//�Ϸ����� ����..
			if( !strncmp( object_name, "obj_sea", 7 ) )		nWater = 2;

			if( nWater ) {
				m_bWaterMap = true;
				m_fWaterHeight = pMeshNode->m_mat_base._42;
			}
			else {
				m_bWaterMap = false;
				m_fWaterHeight = 0.f;
			}

			if(nWater)	
//			if( !strncmp( object_name, "obj_water", 9 ) )
			{
				int id = object_info->nMeshID;

				RMesh* mesh = mesh_mgr->GetFast(id);
				RMeshNode* node = mesh->m_data[0];

				water_instance = new ZWater;

				water_instance->SetMesh(node);
				m_waters.push_back( water_instance );

					 if(nWater==1) water_instance->m_nWaterType = WaterType1;
//				else if(nWater==2) water_instance->m_nWaterType = WaterType1;
				else if(nWater==3) water_instance->m_nWaterType = WaterType2;
	

				if(nWater==2) 
				{ // �� ���� ó�� �����Ҷ� ����..
//					water_instance->m_nWaterType = 1;//sea
					water_instance->m_isRender = false; // �� �׸��� ������Ʈ�� �׸���..
					pMesh->m_LitVertexModel = true;		// �ٴٴ� ����Ʈ ������ �ȹ޴´�..
					++iter;
				}
				else 
				{
					iter = map_object_list->Delete( iter );
				}

//				iter = map_object_list->Delete( iter );

				continue;
			}
		}

		++iter;
	}

	char szBuf[128];
	
	if(  m_flags.size() > 0 )
	{
		sprintf( szBuf, "%s%s/flag.xml", szMapPath, ZGetGameClient()->GetMatchStageSetting()->GetMapName());
		m_flags.InitEnv(szBuf);

		// mlog("ZGame::Create() m_flags.InitEnv \n");
		mlog( "create game enrvironment.\n" );
	}

	m_pMapDesc = new ZMapDesc;
	m_pMapDesc->Open(m_pBsp);

	sprintf( szBuf, "%s%s/smoke.xml", szMapPath, ZGetGameClient()->GetMatchStageSetting()->GetMapName());
	m_pMapDesc->LoadSmokeDesc(szBuf);

	// mlog("ZGame::Create() pMapDesc->LoadSmokeDesc \n");

	// �ʷ����� ������ ����
	FogInfo finfo = GetBsp()->GetFogInfo();
	m_bFog = finfo.bFogEnable;
	m_fFogNear = finfo.fNear;
	m_fFogFar = finfo.fFar;
	m_dwFogColor = finfo.dwFogColor;

	m_bCreated = true;

	mlog( "game world create success.\n" );

	return true;
}

void ZWorld::Destroy()
{
	SAFE_DELETE(m_pBsp);
	SAFE_DELETE(m_pMapDesc);
	m_flags.Clear();
	m_flags.OnInvalidate();
	m_waters.Clear();
	SAFE_DELETE(m_pSkyBox);
}

void ZWorld::OnInvalidate()
{
	m_pBsp->OnInvalidate();
//	m_waters.OnInvalidate();
	m_flags.OnInvalidate();
}

void ZWorld::OnRestore()
{
	m_pBsp->OnRestore();
//	m_waters.OnRestore();
	m_flags.OnRestore();
}

void ZWorld::SetFog(bool bFog)
{
	if(bFog) {
		RSetFog( m_bFog, m_fFogNear, m_fFogFar, m_dwFogColor );
	}
	else {
		RSetFog( FALSE );
	}
}