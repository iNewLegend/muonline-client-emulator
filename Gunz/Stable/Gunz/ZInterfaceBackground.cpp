#include "stdafx.h"

#include "ZApplication.h"
#include "ZInterfaceBackground.h"
#include "RealSpace2.h"
#include "ZMapDesc.h"
#include "MDebug.h"
#include "ZScreenEffectManager.h"

#define DIR_LOGIN	"interface/login"


ZInterfaceBackground::ZInterfaceBackground( void)
{
	RealSpace2::RSetFileSystem( ZApplication::GetFileSystem());

	m_pLogin = NULL;
	m_pMapDesc = NULL;
//	m_bShowMaietLogo = false;
}


ZInterfaceBackground::~ZInterfaceBackground( void)
{
	if ( m_pLogin != NULL)
		delete m_pLogin;
	m_pLogin = NULL;

	if ( m_pMapDesc != NULL)
		delete m_pMapDesc;
	m_pMapDesc = NULL;
}

void ZInterfaceBackground::LoadMesh( void)
{
#ifdef _FASTDEBUG
	return;
#endif

	if ( m_pLogin == NULL)
	{
		m_pLogin = new RBspObject();

//#ifndef _DEBUG
//		const char* descExt = "xml.mef";
//#else
		const char* descExt = "xml";
//#endif
		m_pLogin->Open( DIR_LOGIN"/login.rs", descExt);

		RDummyList* pDummyList = m_pLogin->GetDummyList();

		for ( RDummyList::iterator itor = pDummyList->begin();  itor != pDummyList->end();  ++itor)
		{
			RDummy* pDummy = *itor;

			if( stricmp( pDummy->szName.c_str(), "camera_pos 01" ) == 0 )
			{
				m_vCamPosSt = pDummy->Position;
				m_vCamDirSt = pDummy->Direction;
			}
			else if( stricmp( pDummy->szName.c_str(), "camera_pos 02" ) == 0 )
			{
				m_vCamPosEd = pDummy->Position;
				m_vCamDirEd = pDummy->Direction;
			}
			else if( stricmp( pDummy->szName.c_str(), "spawn_solo_101" ) == 0 )
			{
				m_vCharPos = pDummy->Position;
				m_vCharDir = pDummy->Direction;
			}
		}

		RMapObjectList* pMapObject = m_pLogin->GetMapObjectList();

		list<ROBJECTINFO*>::iterator it;

		ROBJECTINFO* pInfo = NULL;

		for(it = pMapObject->begin();it!=pMapObject->end();it++ ) {

			pInfo = (*it);

			if(!pInfo->pVisualMesh) continue;

			if(pInfo) {// �ʿ��ϸ� xml �� ����..
				string str = "login_obj_ef_sky02.elu";
				if( pInfo->name == str ) {
					pInfo->pVisualMesh->SetUVAnimation(-0.055f,0.f);
				}
			}
		}
			

		// Map description
		if ( m_pMapDesc == NULL)
		{
			m_pMapDesc = new ZMapDesc();
			m_pMapDesc->Open( m_pLogin);

			if( m_pMapDesc ) {

				char szBuf[128];
				sprintf( szBuf, "%s/smoke.xml", DIR_LOGIN );
				m_pMapDesc->LoadSmokeDesc(szBuf);
				mlog("Load smoke description mesh.\n");
			}

		}

		m_pLogin->SetMapObjectOcclusion( true );
	}
}

void ZInterfaceBackground::Free( void)
{
	if ( m_pLogin != NULL)
		delete m_pLogin;
	m_pLogin = NULL;

	if ( m_pMapDesc != NULL)
		delete m_pMapDesc;
	m_pMapDesc = NULL;
}

void ZInterfaceBackground::OnUpdate(float fElapsed)
{
	ZGetEffectManager()->Update( fElapsed);
	ZGetScreenEffectManager()->UpdateEffects();
}

void ZInterfaceBackground::Draw(void)
{
#ifdef _FASTDEBUG
	MWidget* pWidget = (MWidget*)ZApplication::GetGameInterface()->GetIDLResource()->FindWidget( "CharSelection");
	if ( pWidget)
	{
		if ( ZApplication::GetGameInterface()->GetState() == GUNZ_CHARSELECTION)
			pWidget->Show( true);
	}

	MPicture* pPicture = (MPicture*)ZApplication::GetGameInterface()->GetIDLResource()->FindWidget( "CharSel_TopScreen");
	if ( pPicture)
		pPicture->SetOpacity( 0);

	return;
#endif

	// Set rendering state
	RSetViewport( 0, 0, RGetScreenWidth(), RGetScreenHeight());
	RGetDevice()->SetRenderState( D3DRS_CULLMODE  ,D3DCULL_CW);					// from rbs rendering
	RGetDevice()->SetRenderState( D3DRS_NORMALIZENORMALS , TRUE );
	RGetDevice()->SetSamplerState( 0, D3DSAMP_MINFILTER , D3DTEXF_LINEAR);		// Texture filtering
	RGetDevice()->SetSamplerState( 0, D3DSAMP_MAGFILTER , D3DTEXF_LINEAR);
	RGetDevice()->SetSamplerState( 0, D3DSAMP_MIPFILTER , D3DTEXF_LINEAR);

	// Set camera position and direction
    rvector vCamPos, vCamDir;
	float fForgDensity;

	switch ( m_nSceneNumber)
	{
		case  LOGIN_SCENE_FIXEDSKY :
		{
			// Fixed camera
			vCamPos = m_vCamPosSt;
			vCamDir = m_vCamDirSt;

			// Fixed fog -> black screen
			fForgDensity = 0.0f;

			break;
		}

		case  LOGIN_SCENE_FALLDOWN :
		{
			// Get current clock
			DWORD dwClock = ( timeGetTime() - m_dwClock);

			/*
			// Show maiet logo
			if ( !m_bShowMaietLogo)
			{
				m_bShowMaietLogo = true;
				ZGetScreenEffectManager()->AddScreenEffect( "maiet_logo");
			}
			*/

			// Set fog density
			fForgDensity = dwClock * 15.0f;

			// Set wait time
			if ( dwClock < 500)
				dwClock = 0;
			else
				dwClock -= 500;

			// Play BGM music
			if ( dwClock > 2000)
				ZApplication::GetSoundEngine()->PlayMusic( true);


			// End of scroll camera
			float fSeed = dwClock * 0.00035f;			// ī�޶� ���� �� �������� ���� �ɸ��� �ð�( dwClock�� �����ִ� ����
														// �������� ���� ������)
			if ( fSeed > 3.14)
			{
				m_nSceneNumber = LOGIN_SCENE_FIXEDCHAR;
//				m_bShowMaietLogo = false;
			}


			// Move camera position & direction
			float fGain = ( cos( fSeed) + 1.0f) / 2.0f;			// 0 < fGain < 1.0
			vCamPos = m_vCamPosEd + ( m_vCamPosSt - m_vCamPosEd) * fGain;
			vCamDir = m_vCamDirEd + ( m_vCamDirSt - m_vCamDirEd) * fGain;

			//if (m_bShowMaietLogo)
			//{
			//	ZGetScreenEffectManager()->UpdateEffects();
				ZGetScreenEffectManager()->DrawEffects();
			//}

			break;
		}

		case  LOGIN_SCENE_FIXEDCHAR :
		{
			// Show menu UI
			MWidget* pWidget = (MWidget*)ZApplication::GetGameInterface()->GetIDLResource()->FindWidget( "CharSelection");
			if ( pWidget)
			{
				if ( ZApplication::GetGameInterface()->GetState() == GUNZ_CHARSELECTION)
					pWidget->Show( true);
			}
			MPicture* pPicture = (MPicture*)ZApplication::GetGameInterface()->GetIDLResource()->FindWidget( "CharSel_TopScreen");
			if ( pPicture)
				pPicture->SetOpacity( 0);

			// Play BGM music
			ZApplication::GetSoundEngine()->PlayMusic( true);

			// Fixed camera
			vCamPos = m_vCamPosEd;
			vCamDir = m_vCamDirEd;

			// Cleared fog
			fForgDensity = 50000.0f;
			break;
		}

		case  LOGIN_SCENE_SELECTCHAR :
		{
			// Get current clock
			DWORD dwClock = ( timeGetTime() - m_dwClock);
			float fGain = ( cos( dwClock * 0.0012f) + 1.0f) / 2.0f;
			vCamPos = m_vCamPosEd + ( m_vCamPosSt - m_vCamPosEd) * fGain;
			vCamDir = m_vCamDirEd + ( m_vCamDirSt - m_vCamDirEd) * fGain;

			MPicture* pPicture = (MPicture*)ZApplication::GetGameInterface()->GetIDLResource()->FindWidget( "CharSel_TopScreen");
			if ( pPicture)
			{
				// Set wait time
				if ( dwClock < 2000)
					dwClock = 0;
				else
					dwClock -= 2000;

				// Set opacity of black screen
				int nOpacity = (int)( dwClock / 3.0f);
				if ( nOpacity > 255)
					nOpacity = 255;

				pPicture->SetOpacity( nOpacity);
			}

			// Cleared fog
			fForgDensity = 50000.0f;
			break;
		}
	}

	// Set camera
	RSetCamera( vCamPos, (vCamPos + vCamDir), rvector( 0, 0, 1));

	// Draw
	RSetProjection( D3DX_PI * 70 / 180, RGetScreenWidth() / (float)RGetScreenHeight(), 10.0f, 15000.0f);
	SetFogState( fForgDensity-10000.0f, fForgDensity, 0x00000000);
	D3DXMatrixTranslation( &m_matWorld, 0, 0, 0);
	RGetDevice()->SetTransform( D3DTS_WORLD, &m_matWorld);

	// Draw background
	if ( m_pLogin)
	{
		m_pLogin->Draw();
		m_pLogin->DrawObjects();
	}

	if ( m_pMapDesc)
		m_pMapDesc->DrawMapDesc();

	// Draw effects(smoke, cloud)
//	ZGetEffectManager()->Draw( timeGetTime());

	// Draw maiet logo effect
//	ZGetScreenEffectManager()->DrawEffects();
}


void ZInterfaceBackground::OnInvalidate()
{
#ifdef _FASTDEBUG
	return;
#endif

	if( m_pLogin)
		m_pLogin->OnInvalidate();
}


void ZInterfaceBackground::OnRestore()
{
#ifdef _FASTDEBUG
	return;
#endif

	if( m_pLogin)
		m_pLogin->OnRestore();
}


void ZInterfaceBackground::SetScene( int nSceneNumber)
{
	if ( nSceneNumber == m_nSceneNumber)
		return;

	if ( (nSceneNumber == LOGIN_SCENE_FALLDOWN) || (nSceneNumber == LOGIN_SCENE_SELECTCHAR))
		m_dwClock = timeGetTime();

	// �ΰ� �ѷ��ش�
	if( nSceneNumber == LOGIN_SCENE_FALLDOWN)
		ZGetScreenEffectManager()->AddScreenEffect( "maiet_logo");

	if ( nSceneNumber == LOGIN_SCENE_SELECTCHAR)
	{
		m_vCamPosSt = m_vCamPosEd;
		m_vCamPosEd.y = -780;
		m_vCamPosEd.z = 60;

		m_vCamDirSt = m_vCamDirEd;
		m_vCamDirEd.x = -0.55f;
		m_vCamDirEd.z = 0.40f;
	}

	m_nSceneNumber = nSceneNumber;
}


void ZInterfaceBackground::SetFogState( float fStart, float fEnd, unsigned long int color)
{
	RGetDevice()->SetRenderState( D3DRS_FOGENABLE, TRUE);
	RGetDevice()->SetRenderState( D3DRS_FOGCOLOR, color);

	RGetDevice()->SetRenderState( D3DRS_FOGTABLEMODE, D3DFOG_LINEAR);
	RGetDevice()->SetRenderState( D3DRS_FOGSTART, *(DWORD *)(&fStart));
	RGetDevice()->SetRenderState( D3DRS_FOGEND,   *(DWORD *)(&fEnd));
}
