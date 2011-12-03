#include "stdafx.h"
#include "ZScreenDebugger.h"
#include "MDrawContext.h"
#include "ZGame.h"
#include "ZGameInterface.h"
#include "ZGlobal.h"
#include "ZScreenEffectManager.h"
#include "ZStencilLight.h"
#include "ZCamera.h"
#include "ZMyCharacter.h"
#include "RBspObject.h"
#include "ZModule_Skills.h"

int g_debug_tex_update_cnt;

_NAMESPACE_REALSPACE2_BEGIN
//extern int g_nCheckWallPolygons,g_nRealCheckWallPolygons;
//extern int g_nCheckFloorPolygons,g_nRealCheckFloorPolygons;
extern int g_nPoly,g_nCall;
extern int g_nPickCheckPolygon,g_nRealPickCheckPolygon;
_NAMESPACE_REALSPACE2_END

#define DEFAULT_SCREEN_DEBUGGER_LINE		120

ZScreenDebugger::ZScreenDebugger() : m_nShowDebugInfo(0), m_nY(DEFAULT_SCREEN_DEBUGGER_LINE)
{

}

ZScreenDebugger::~ZScreenDebugger()
{

}

extern MFontR2*		g_pDefFont;

void ZScreenDebugger::DrawDebugInfo(MDrawContext *pDC)
{
#ifdef _PUBLISH
	if (m_nShowDebugInfo != 0) {
		int sy=100;
		pDC->SetColor(MCOLOR(0xFFffffff));
		char buffer[256];

		int nSendTraffic, nRecvTraffic;
		ZGetGameClient()->GetUDPTraffic(&nSendTraffic, &nRecvTraffic);

		float fMs = 1000.f/g_fFPS;
		float fScore = 100-(fMs-(1000.f/60.f))*2;

		int zcr_cnt = ZGetObjectManager()->GetRenderedCount();
		int zco_cnt = ZGetObjectManager()->GetDrawCount();

		sprintf(buffer, "FPS : %3.3f , ms %4.1f,score %4.1f    Send:%d CPS, Recv:%d CPS    CmdQueue: %d RenderCharCnt = %d/%d", g_fFPS, fMs, fScore, nSendTraffic, nRecvTraffic,
			ZGetGameClient()->GetCommandManager()->GetCommandQueueCount(),zcr_cnt,zco_cnt );

		pDC->BeginFont();
		pDC->Text(0,sy,buffer);
		pDC->EndFont();
	}
#endif _PUBLISH

#ifndef _PUBLISH
	if (m_nShowDebugInfo == 0) return;

	m_nY = DEFAULT_SCREEN_DEBUGGER_LINE;

	if (m_nShowDebugInfo==3)
	{
		OnDrawAIDebugInfo(pDC);
		return;
	}

	ZCamera* pCamera = ZApplication::GetGameInterface()->GetCamera();

	MFont *pFont=g_pDefFont;
	pDC->SetFont(pFont);

	RGetDevice()->SetTexture(0,NULL);
	RGetDevice()->SetTexture(1,NULL);
	RGetDevice()->SetRenderState( D3DRS_LIGHTING, FALSE );
	RGetDevice()->SetRenderState(D3DRS_ZENABLE, false );
	RGetDevice()->SetFVF( D3DFVF_XYZ | D3DFVF_DIFFUSE );

	rmatrix birdinitmat;
	D3DXMatrixIdentity(&birdinitmat);
	RGetDevice()->SetTransform( D3DTS_WORLD, &birdinitmat );
	RSetCamera(RCameraPosition,RCameraDirection+RCameraPosition,rvector(0,0,1));
	
	RDrawLine(rvector(0.0f,0.0f,0.0f), rvector(100.0f,0.0f,0.0f), 0xFFFF0000);
	RDrawLine(rvector(0.0f,0.0f,0.0f), rvector(0.0f,100.0f,0.0f), 0xFF00FF00);
	RDrawLine(rvector(0.0f,0.0f,0.0f), rvector(0.0f,0.0f,100.0f), 0xFF0000FF);


	int sy=100;
	pDC->SetColor(MCOLOR(0xFFffffff));
	char buffer[256];

#define OUTTEXT() { pDC->Text(0,sy,buffer); }
#define NEXTLINE() { sy+=15; }

	pDC->BeginFont();

	if(m_nShowDebugInfo==2) {
		int nSendTraffic, nRecvTraffic;
		ZGetGameClient()->GetUDPTraffic(&nSendTraffic, &nRecvTraffic);

		float fMs = 1000.f/g_fFPS;
		float fScore = 100-(fMs-(1000.f/60.f))*2;

		int zcr_cnt = ZGetObjectManager()->GetRenderedCount();
		int zco_cnt = ZGetObjectManager()->GetDrawCount();

		sprintf(buffer, "FPS : %3.3f , ms %4.1f,score %4.1f ��    Send:%d CPS, Recv:%d CPS    CmdQueue: %d RenderCharCnt = %d/%d", g_fFPS, fMs, fScore, nSendTraffic, nRecvTraffic,
			ZGetGameClient()->GetCommandManager()->GetCommandQueueCount(),zcr_cnt,zco_cnt );
		OUTTEXT();
		NEXTLINE();

		sprintf(buffer, "observer cmd q=%d , effect 0=%d ,1=%d, 2=%d   seffect = %d, dynlight = %d",ZGetGame()->GetObserverCommandListCount(),
			ZGetEffectManager()->GetEffectCount(0),ZGetEffectManager()->GetEffectCount(1),ZGetEffectManager()->GetEffectCount(2),
			ZGetScreenEffectManager()->GetCount(),ZGetStencilLight()->GetCount());
		OUTTEXT();
		NEXTLINE();

		sprintf(buffer, "mapobject = %d , frustum culled = %d , occlusion culled = %d",
			ZGetGame()->GetWorld()->GetBsp()->GetMapObjectList()->size(),ZGetGame()->GetWorld()->GetBsp()->GetDebugInfo()->nMapObjectFrustumCulled,ZGetGame()->GetWorld()->GetBsp()->GetDebugInfo()->nMapObjectOcclusionCulled);
		OUTTEXT();
		NEXTLINE();

#ifndef _PUBLISH
		sprintf(buffer, "Sound Channels: %d", FSOUND_GetChannelsPlaying());
		OUTTEXT();
		NEXTLINE();
#endif
		//NEXTLINE();
		//{
		//	sprintf( buffer, "2D Sound Effect Number : %d 3D Sound Effect Number : %d", ZGetSoundEngine()->GetMySoundEffectPlayer()->GetSoundEffectNum(), ZGetSoundEngine()->GetSoundEffectPlayer()->GetSoundEffectNum() );
		//	pDC->SetColor( MCOLOR(0xFFFFFFFF) );
		//	OUTTEXT();
		//	NEXTLINE();
		//}


		sprintf(buffer, "%d textures , %d bytes ", RGetTextureManager()->CalcUsedCount(), RGetTextureManager()->CalcUsedSize() );
		OUTTEXT();
		pDC->EndFont();

		return;
	}

	for (int i = 0; i < 4; i++)
	{
		if (pCamera->m_bCollision[i]) sprintf(buffer, "True");
		else sprintf(buffer, "False");

		OUTTEXT();
		NEXTLINE();
	}
	sprintf(buffer, "AngleX : %3.3f , AngleZ : %3.3f , Dist : %3.3f", pCamera->m_fAngleX, pCamera->m_fAngleZ,
		pCamera->m_fCurrentDist);
	OUTTEXT();
	NEXTLINE();

	// �׽�Ʈ �ڵ�.. ����� ����..
	ZCharacter* pCharacter = ZGetGame()->m_pMyCharacter;
	ZCombatInterface* pCombatInterface = ZApplication::GetInstance()->GetGameInterface()->GetCombatInterface();

	// �������ϰ��
	if (pCombatInterface->GetObserver()->IsVisible()) 
	{
		pCharacter = pCombatInterface->GetObserver()->GetTargetCharacter();
		if (pCharacter == NULL) pCharacter = ZGetGame()->m_pMyCharacter;
	}

	float fMs = 1000.f/g_fFPS;
	float fScore = 100-(fMs-(1000.f/60.f))*2;

	sprintf(buffer, "FPS : %3.3f , %d polygon , %d calls , %4.1f ms,score %4.1f ��, CmdQueue: %d",g_fFPS,g_nPoly,g_nCall,fMs,fScore,
		ZGetGameClient()->GetCommandManager()->GetCommandQueueCount());
	OUTTEXT();
	NEXTLINE();

	sprintf(buffer,"pos ( %3.1f %3.1f %3.1f ) dir ( %3.1f %3.1f %3.1f )    speed %3.1f ( %3.1f %3.1f %3.1f ) , camera ( %3.1f %3.1f %3.1f )"
		//,pCharacter->m_Position.x,pCharacter->m_Position.y,pCharacter->m_Position.z
		, pCharacter->GetPosition().x, pCharacter->GetPosition().y, pCharacter->GetPosition().z	//mmemory proxy
		,pCharacter->m_Direction.x,pCharacter->m_Direction.y,pCharacter->m_Direction.z
		,Magnitude(rvector(pCharacter->GetVelocity().x,pCharacter->GetVelocity().y,0))
		,pCharacter->GetVelocity().x,pCharacter->GetVelocity().y,pCharacter->GetVelocity().z
		,pCamera->GetPosition().x,pCamera->GetPosition().y,pCamera->GetPosition().z
		);

	OUTTEXT();
	NEXTLINE();

	buffer[0]=0;
	
	ZCharaterStatusBitPacking & uStatus = ZGetGame()->m_pMyCharacter->m_dwStatusBitPackingValue.Ref();
	ZMyCharaterStatusBitPacking & zStatus = ZGetGame()->m_pMyCharacter->m_statusFlags.Ref();

//	if(g_pGame->m_pMyCharacter->m_bWallJump) strcat(buffer,"wall ");
//	if(g_pGame->m_pMyCharacter->m_bWallJump2) strcat(buffer,"wall2 ");
//	if(g_pGame->m_pMyCharacter->m_bTumble) strcat(buffer,"Tumble ");
//	if(!g_pGame->m_pMyCharacter->m_bLand) strcat(buffer,"jump ");

	if(uStatus.m_bWallJump) strcat(buffer,"wall ");
	if(uStatus.m_bWallJump2) strcat(buffer,"wall2 ");
	if(uStatus.m_bTumble) strcat(buffer,"Tumble ");
	if(!uStatus.m_bLand) strcat(buffer,"jump ");

	/*
	if(g_pGame->m_pMyCharacter->m_bClimb)
		strcat(buffer,"climbing ");
	*/
	if(zStatus.m_bJumpQueued) strcat(buffer,"jumpq ");
	if(zStatus.m_bGuard)
	{
		strcat(buffer,"guard-");
		if(ZGetGame()->m_pMyCharacter->m_nGuardBlock.Ref())
		{
			char buf[256];
			sprintf(buf,"bl%d",ZGetGame()->m_pMyCharacter->m_nGuardBlock.Ref());
			strcat(buffer,buf);
		}
		if(zStatus.m_bGuardStart) strcat(buffer,"start");
		if(zStatus.m_bGuardCancel) strcat(buffer,"cancel");
		strcat(buffer," ");
	}
	if(zStatus.m_bWallHang) strcat(buffer,"hang ");
	if(zStatus.m_bSkill) strcat(buffer,"skill ");
	if(uStatus.m_bBlast ) strcat(buffer,"blast ");
	if(uStatus.m_bBlastFall ) strcat(buffer,"blastfall ");
	if(zStatus.m_bShot ) strcat(buffer,"shot ");
	
	OUTTEXT();
	NEXTLINE();

	RVisualMesh* pVMesh = pCharacter->m_pVMesh;

	if(pVMesh) {

		AniFrameInfo* pAniInfo[2];

		pAniInfo[0] = pVMesh->GetFrameInfo(ani_mode_lower);
		pAniInfo[1] = pVMesh->GetFrameInfo(ani_mode_upper);

		if( pAniInfo[0]->m_pAniSet ) {

			for(int i=0;i<2;i++) {
			
				int speed = (int)(( pAniInfo[i]->m_fSpeed / 4.8f) * 100);

				int max_frame = 0;
				int org_time = 0;
				int delay_time = 0;

				max_frame = pVMesh->GetMaxFrame((RAniMode)i);

				if(max_frame==0)
					max_frame = pVMesh->GetMaxFrame( ani_mode_lower );

				if( max_frame ) {
					org_time = (int)(max_frame / 4.8f);
					delay_time = org_time * (1.f/( pAniInfo[i]->m_fSpeed / 4.8f ));
				}

				sprintf(buffer,"speed %d = %d �ۼ�Ʈ ( %d/%d ) ms (delay = %d) ",i, speed, org_time , delay_time,delay_time-org_time);
				pDC->SetColor(MCOLOR(0xFFffffff));
				OUTTEXT();
				NEXTLINE();

			}

			sprintf(buffer,"poly_cnt:%d,low_ani_state %d,%s frame : weapon type %d : down %d / %d CAFactor = %.2f",
				ZGetGame()->m_render_poly_cnt,ZGetGame()->m_pMyCharacter->m_AniState_Lower.Ref(),
				pAniInfo[0]->m_pAniSet->GetName(),
				pVMesh->GetSetectedWeaponMotionID() ,
				pAniInfo[0]->m_nFrame,
				pVMesh->GetMaxFrame( ani_mode_lower ),
				ZGetGame()->m_pMyCharacter->GetCAFactor());

			pDC->SetColor(MCOLOR(0xFFffffff));
			OUTTEXT();
			NEXTLINE();

			if( pAniInfo[1]->m_pAniSet ) {
				sprintf(buffer,"%s frame : weapon type %d : up %d / %d ",
					pAniInfo[1]->m_pAniSet->GetName(), 
					pVMesh->GetSetectedWeaponMotionID() ,
					pAniInfo[1]->m_nFrame,
					pVMesh->GetMaxFrame( ani_mode_upper ));

				pDC->SetColor(MCOLOR(0xFFffffff));
				OUTTEXT();
				NEXTLINE();
			}else
				NEXTLINE();
		}



		rvector rfoot = pVMesh->GetRFootPosition();
		rvector lfoot = pVMesh->GetLFootPosition();

		sprintf(buffer,"rfoot:(%.2f,%.2f,%.2f)lfoot:(%.2f,%.2f,%.2f)",rfoot.x,rfoot.y,rfoot.z,lfoot.x,lfoot.y,lfoot.z);
		OUTTEXT();
		NEXTLINE();

		float _fsize = RGetTextureManager()->CalcUsedSize() / (1024.f*1024.f);

		sprintf(buffer,"texture used size : %f M,update size : %d" , _fsize , g_debug_tex_update_cnt);
		OUTTEXT();
		NEXTLINE();
	}

	NEXTLINE();
	int x = 30, y = 240;
	for (ZCharacterManager::iterator itor = ZGetGame()->m_CharacterManager.begin();
		itor != ZGetGame()->m_CharacterManager.end(); ++itor)
	{
		ZCharacter* pCharacter = (*itor).second;
		char szVisible[64];
		if (pCharacter->IsVisible()) strcpy(szVisible, "visible");
		else strcpy(szVisible, "nonVisible");

		char szAlive[256];
		if (pCharacter->IsDie()) strcpy(szAlive, "Dead"); else strcpy(szAlive, "Alive");

		sprintf(buffer,"%s(%s) : %s pos ( %3.1f %3.1f %3.1f ) dir ( %3.1f %3.1f %3.1f )    speed %3.1f ( %3.1f %3.1f %3.1f )"
			,pCharacter->GetProperty()->GetName()
			, szAlive
			, szVisible
			//,pCharacter->m_Position.x,pCharacter->m_Position.y,pCharacter->m_Position.z
			, pCharacter->GetPosition().x, pCharacter->GetPosition().y, pCharacter->GetPosition().z	//mmemory proxy
			,pCharacter->m_Direction.x,pCharacter->m_Direction.y,pCharacter->m_Direction.z
			,Magnitude(rvector(pCharacter->GetVelocity().x,pCharacter->GetVelocity().y,0))
			,pCharacter->GetVelocity().x,pCharacter->GetVelocity().y,pCharacter->GetVelocity().z
			);

		pDC->SetColor(MCOLOR(0xFFFFFFFF));
		OUTTEXT();
		NEXTLINE();
	}

	// npc ���
	for (ZObjectManager::iterator itor = ZGetGame()->m_ObjectManager.begin();
		itor != ZGetGame()->m_ObjectManager.end(); ++itor)
	{
		ZObject* pCharacter = (*itor).second;

		if(!pCharacter->IsNPC()) continue;

		char szVisible[64];
		if (pCharacter->IsVisible()) strcpy(szVisible, "visible");
		else strcpy(szVisible, "nonVisible");

		char szAlive[256];
		if (pCharacter->IsDie()) strcpy(szAlive, "Dead"); else strcpy(szAlive, "Alive");

		sprintf(buffer,"%s(%s) : %s pos ( %3.1f %3.1f %3.1f ) dir ( %3.1f %3.1f %3.1f )    speed %3.1f ( %3.1f %3.1f %3.1f )"
			,"npc"
			, szAlive
			, szVisible
			//,pCharacter->m_Position.x,pCharacter->m_Position.y,pCharacter->m_Position.z
			, pCharacter->GetPosition().x, pCharacter->GetPosition().y, pCharacter->GetPosition().z	//mmemory proxy
			,pCharacter->m_Direction.x,pCharacter->m_Direction.y,pCharacter->m_Direction.z
			,Magnitude(rvector(pCharacter->GetVelocity().x,pCharacter->GetVelocity().y,0))
			,pCharacter->GetVelocity().x,pCharacter->GetVelocity().y,pCharacter->GetVelocity().z
			);

		pDC->SetColor(MCOLOR(0xFFFFFFFF));
		OUTTEXT();
		NEXTLINE();
	}

	RGetDevice()->SetTexture(0,NULL);
	RGetDevice()->SetRenderState( D3DRS_LIGHTING, FALSE );
	RGetDevice()->SetFVF( D3DFVF_XYZ | D3DFVF_DIFFUSE );

	rmatrix initmat;
	D3DXMatrixIdentity(&initmat);
	RGetDevice()->SetTransform( D3DTS_WORLD, &initmat );
	RSetCamera(RCameraPosition,RCameraDirection+RCameraPosition,rvector(0,0,1));

	if(ZApplication::GetGameInterface()->IsCursorEnable())
	{
		POINT pt;
		GetCursorPos(&pt);
		ScreenToClient(g_hWnd,&pt);


		RBSPPICKINFO bpi;
		rvector pos,dir;
		RGetScreenLine(pt.x,pt.y,&pos,&dir);
		bool bPicked=ZGetGame()->GetWorld()->GetBsp()->PickOcTree(pos,dir,&bpi);
		if(bPicked)
		{
			rplane *pPlane=&bpi.pInfo->plane;

			BSPVERTEX *v=bpi.pNode->pInfo[bpi.nIndex].pVertices;

			sprintf(buffer,"( %3.1f %3.1f %3.1f ) ( %3.1f %3.1f %3.1f ) ( %3.1f %3.1f %3.1f ) ",
				v->x,v->y,v->z,
				(v+1)->x,(v+1)->y,(v+1)->z,
				(v+2)->x,(v+2)->y,(v+2)->z);

			pDC->SetColor(MCOLOR(0xFFffffff));
			OUTTEXT();
			NEXTLINE();

			RGetDevice()->SetTexture(0,NULL);
			RGetDevice()->SetTexture(1,NULL);
			RGetDevice()->SetRenderState(D3DRS_ZENABLE, false );
			RGetDevice()->SetFVF( D3DFVF_XYZ | D3DFVF_DIFFUSE );

			for(int i=0;i<3;i++)
			{
				RDrawLine(*v[i].Coord(),*v[(i+1)%3].Coord(),0xffff0000);
			}
		}
	}

	ZGetGame()->GetWorld()->GetBsp()->DrawOcclusions();


	{
		// ĳ���� �浹üũ
		RDrawCylinder(ZGetGame()->m_pMyCharacter->GetPosition()+rvector(0,0,120),CHARACTER_RADIUS,60,10);	//mmemory proxy

		rmatrix birdinitmat;
		D3DXMatrixIdentity(&birdinitmat);
		RGetDevice()->SetTransform( D3DTS_WORLD, &birdinitmat );

		for (ZObjectManager::iterator itor = ZGetGame()->m_ObjectManager.begin();
			itor != ZGetGame()->m_ObjectManager.end(); ++itor)
		{
			ZObject* pNPC = (*itor).second;
			if(pNPC->IsNPC())
			{
				RDrawCylinder(pNPC->GetPosition()+rvector(0,0,pNPC->GetCollHeight()/2.0f),pNPC->GetCollRadius(),pNPC->GetCollHeight()/2.0f,10);

				ZItem *pItem = pNPC->GetItems()->GetItem(MMCIP_MELEE);
				if(pItem)
				{
					MMatchItemDesc* pDesc = pItem->GetDesc();
					float fMeleeRange = pDesc->m_nRange.Ref();

					rvector p1 = pNPC->GetPosition();
					rvector p2 = pNPC->GetPosition() + pNPC->GetDirection() * fMeleeRange;
					RDrawLine(p1, p2, 0xFF0000FF);
				}

			}
		}

		ZGetGame()->GetWorld()->GetBsp()->DrawSolidNode();

//		ZGetGame()->GetWorld()->GetBsp()->DrawAdjPoltyongs();

		/*
		float fDist=ZGetGame()->GetWorld()->GetBsp()->CheckFloor(pCharacter->m_Position);
		float fDistCeiling=ZGetGame()->GetWorld()->GetBsp()->CheckCeiling(pCharacter->m_Position+rvector(0,0,180));
		*/

		rvector checkpos=pCharacter->GetPosition() +rvector(0,0,90);
		sprintf(buffer,"dist to floor = %6.3f ", ZGetGame()->m_pMyCharacter->GetDistToFloor());
		pDC->SetColor(MCOLOR(0xFFffffff));
		OUTTEXT();
		NEXTLINE();

		g_nPickCheckPolygon=0;
		g_nRealPickCheckPolygon=0;
	}

	ZEffectManager* pZEM = ZGetEffectManager();

	if(pZEM) {
		sprintf(buffer,"effectmgr -> skip cnt : %d ,rendered :%d / %d", pZEM->m__skip_cnt , pZEM->m__rendered , pZEM->m__cnt );
		pDC->SetColor(MCOLOR(0xFFffffff));
		OUTTEXT();
		NEXTLINE();
	}	

	ZScreenEffectManager* pZSEM = ZGetScreenEffectManager();

	if(pZEM) {
		sprintf(buffer,"screeneffectmgr -> %d ", pZSEM->GetCount() );
		pDC->SetColor(MCOLOR(0xFFffffff));
		OUTTEXT();
		NEXTLINE();
	}	

	//{
	//	sprintf( buffer, "2D Sound Effect Number : %d 3D Sound Effect Number : %d", ZGetSoundEngine()->GetMySoundEffectPlayer()->GetSoundEffectNum(), ZGetSoundEngine()->GetSoundEffectPlayer()->GetSoundEffectNum() );
	//	pDC->SetColor( MCOLOR(0xFFFFFFFF) );
	//	OUTTEXT();
	//	NEXTLINE();
	//}
	pDC->EndFont();

#endif
}


static void DrawDebugInfo_NPCArc(rvector pos, rvector dir, float fRange, float fAngle, DWORD color)
{
#ifndef _PUBLISH
	rvector p1 = pos;
	rvector p2 = pos + dir * fRange;
	RDrawLine(p1, p2, color);

	rvector orig = rvector(1.0f, 0.0f, 0.0f);
	float OriginAngle = GetAngleOfVectors(orig, dir);

	// �ٰŸ� ���� ����
	float src_angle = fAngle;
	float start_angle = OriginAngle - src_angle / 2.0f;
	float end_angle = start_angle + src_angle;

	RDrawArc(p1, fRange, start_angle, end_angle, 100, color);
	rvector a = fRange * rvector(cos(start_angle),sin(start_angle),0) + p1;
	rvector b = fRange * rvector(cos(end_angle),sin(end_angle),0) + p1;
	RDrawLine(p1, a, color);
	RDrawLine(p1, b, color);
#endif
}


void ZScreenDebugger::OnDrawAIDebugInfo(MDrawContext *pDC)
{
#ifndef _PUBLISH
	MFont *pFont=g_pDefFont;
	pDC->SetFont(pFont);

	RGetDevice()->SetTexture(0,NULL);
	RGetDevice()->SetTexture(1,NULL);
	RGetDevice()->SetRenderState( D3DRS_LIGHTING, FALSE );
	RGetDevice()->SetRenderState(D3DRS_ZENABLE, false );
	RGetDevice()->SetFVF( D3DFVF_XYZ | D3DFVF_DIFFUSE );

	rmatrix birdinitmat;
	D3DXMatrixIdentity(&birdinitmat);
	RGetDevice()->SetTransform( D3DTS_WORLD, &birdinitmat );
	RSetCamera(RCameraPosition,RCameraDirection+RCameraPosition,rvector(0,0,1));

	ZGetGame()->GetWorld()->GetBsp()->DrawNavi_Polygon();
//	ZGetGame()->GetWorld()->GetBsp()->DrawNavi_Links();

	ZObjectManager *pcm=&ZGetGame()->m_ObjectManager;


	// �浹üũ
	{
		RDrawCylinder(ZGetGame()->m_pMyCharacter->GetPosition()+rvector(0,0,120),CHARACTER_RADIUS,60,10);

		rmatrix birdinitmat;
		D3DXMatrixIdentity(&birdinitmat);
		RGetDevice()->SetTransform( D3DTS_WORLD, &birdinitmat );

		for (ZObjectManager::iterator itor = ZGetGame()->m_ObjectManager.begin();
			itor != ZGetGame()->m_ObjectManager.end(); ++itor)
		{
			ZObject* pObj = (*itor).second;
			if(pObj->IsNPC())
			{
				ZActor* pNPC = (ZActor*)pObj;
				// �浹üũ
				RDrawCylinder(pNPC->GetPosition()+rvector(0,0,pNPC->GetCollHeight()/2.0f),pNPC->GetCollRadius(),pNPC->GetCollHeight()/2.0f,30);

				// �ٰŸ� ����
				ZItem *pItem = pNPC->GetItems()->GetItem(MMCIP_MELEE);
				if(pItem)
				{
					MMatchItemDesc* pDesc = pItem->GetDesc();
					float fMeleeRange = pDesc->m_nRange.Ref();
					float fMelleAngle = ToRadian( (float)pDesc->m_nAngle.Ref());

					DrawDebugInfo_NPCArc(pNPC->GetPosition(), pNPC->GetDirection(), fMeleeRange, fMelleAngle, 0xFF0000FF);
				}

				// �ٰŸ� ��ų ����
				if(pNPC->GetNPCInfo()->nNPCAttackTypes & NPC_ATTACK_MAGIC)
				{
					ZModule_Skills *pmod = (ZModule_Skills *)pNPC->GetModule(ZMID_SKILLS);
					if(pmod) 
					{
						for(int i=0;i<pmod->GetSkillCount();i++)
						{
							ZSkill *pSkill = pmod->GetSkill(i);
							ZSkillDesc *pDesc = pmod->GetSkill(i)->GetDesc();
							if (pDesc)
							{
								if (pDesc->nEffectType == ZSE_SLASH_AREA)
								{
									float fMinRange = pDesc->fEffectAreaMin * 100.0f;
									if (fMinRange != 0.0f)
									{
										DrawDebugInfo_NPCArc(pNPC->GetPosition(), 
											                 pNPC->GetDirection(), 
															 fMinRange, 
															 pDesc->fEffectAngle, 
															 0xFFFFBF2F);
									}

									float fMaxRange = pDesc->fEffectArea * 100.0f;
									DrawDebugInfo_NPCArc(pNPC->GetPosition(), 
											                pNPC->GetDirection(), 
															fMaxRange, 
															pDesc->fEffectAngle, 
															0xFFFFBF2F);
									


								}
							}
						}

					}
				}
			}
		}
	}


	// hud���
	int n=0;
	for (ZObjectManager::iterator itor = pcm->begin(); itor != pcm->end(); ++itor)
	{
		ZObject* pObject = (*itor).second;
		if (pObject->IsNPC())
		{
			ZActor* pActor = (ZActor*)pObject;
			pActor->DebugTest();

			DrawActorInfo(++n, pDC, pActor);
		}
	}
#endif
}

void ZScreenDebugger::DrawActorInfo(int num, MDrawContext *pDC, ZActor* pActor)
{
#ifndef _PUBLISH
	char buf[256];

	rvector pos = pActor->GetPosition();
	rvector dir = pActor->GetDirection();
	rvector vel = pActor->GetVelocity();

	MQuestNPCInfo* pNPCInfo = pActor->GetNPCInfo();
	int nTaskCount = pActor->GetTaskManager()->GetCount();
	ZTask* pCurrTask = pActor->GetTaskManager()->GetCurrTask();
	char szTaskName[256] = "None";
	if (pCurrTask)
	{
		strcpy(szTaskName, pCurrTask->GetTaskName());
	}

	sprintf(buf, "[%02d], %s, HP(%02d)AP(%2d) pos(%.2f %.2f %.2f), dir(%.2f %.2f %.2f), vel(%.2f %.2f %.2f), Task: %d, CurrTask: %s",
		num, pNPCInfo->szName, 
		pActor->GetHP(), pActor->GetAP(),
		pos.x, pos.y, pos.z, dir.x, dir.y, dir.z, vel.x, vel.y, vel.z,
		nTaskCount, szTaskName);
	PrintText(pDC, buf);

	char szFlagLand[8] = "��";
	char szFlagBlast[8] = "��";
	char szFlagBlastDagger[8] = "��";
	char szFlagMoving[8] = "��";
	char szFlagDead[8] = "��";
	char szFlagRequestedDead[8] = "��";
	char szFlagMyControl[8] = "��";

	if (pActor->CheckFlag(AF_LAND)) strcpy(szFlagLand, "��");
	if (pActor->CheckFlag(AF_BLAST)) strcpy(szFlagBlast, "��");
	if (pActor->CheckFlag(AF_BLAST_DAGGER)) strcpy(szFlagBlastDagger, "��");
	if (pActor->CheckFlag(AF_MOVING)) strcpy(szFlagMoving, "��");
	if (pActor->CheckFlag(AF_DEAD)) strcpy(szFlagDead, "��");
	if (pActor->CheckFlag(AF_REQUESTED_DEAD)) strcpy(szFlagRequestedDead, "��");
	if (pActor->CheckFlag(AF_MY_CONTROL)) strcpy(szFlagMyControl, "��");

	sprintf(buf, "     LAND(%s), BLAST(%s), BLASTDAGGER(%s), MOVING(%s), DEAD(%s), REQUESTED_DEAD(%s), MY_CONTROL(%s), DistFloor(%.2f)",
		szFlagLand, szFlagBlast, szFlagBlastDagger ,szFlagMoving, szFlagDead, szFlagRequestedDead, szFlagMyControl, pActor->GetDistToFloor());
	PrintText(pDC, buf);
#endif
}

void ZScreenDebugger::PrintText(MDrawContext* pDC, const char* buffer)
{
#ifndef _PUBLISH
	pDC->Text(0, m_nY,buffer);
	m_nY += 15;
#endif
}

void ZScreenDebugger::SwitchDebugInfo()
{
//#ifndef _PUBLISH
	switch(m_nShowDebugInfo)
	{
	case 0: m_nShowDebugInfo=1; break;
	case 1: m_nShowDebugInfo=2; break;
	case 2: m_nShowDebugInfo=3; break;
	case 3: m_nShowDebugInfo=0; break;
	}
//#endif
}