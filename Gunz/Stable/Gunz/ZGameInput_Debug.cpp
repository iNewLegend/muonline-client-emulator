#include "stdafx.h"
#include "ZGameInput.h"
#include "ZGameInterface.h"
#include "ZApplication.h"
#include "ZPost.h"
#include "ZPostLocal.h"
#include "ZModule_FireDamage.h"
#include "ZModule_ColdDamage.h"
#include "ZModule_LightningDamage.h"
#include "ZModule_PoisonDamage.h"
#include "ZModule_Skills.h"
#include "ZWorldItem.h"
#include "ZMapDesc.h"
#include "ZGlobal.h"
#include "ZScreenEffectManager.h"
#include "ZInput.h"

extern bool g_debug_rot;
extern int  g_debug_render_mode;
extern bool g_bVertex_Soft; 


bool ZGameInput::OnDebugEvent(MEvent* pEvent)
{
#ifdef _PUBLISH

	return false;
#endif

	static bool bMusicMute = false;

	switch(pEvent->nMessage){
	case MWM_KEYDOWN:
		{
			switch (pEvent->nKey)
			{
			case VK_END:
				{
					ZGetGameInterface()->m_bTeenVersion = !ZGetGameInterface()->m_bTeenVersion;
				}
				return true;

			case VK_INSERT:
				{
					g_debug_render_mode++;
					if(g_debug_render_mode > 3)
						g_debug_render_mode = 0;
				}
				return true;

			case VK_DELETE:
				{
					g_bVertex_Soft = !g_bVertex_Soft;
				}
				return true;

			
			//---------------------------------------------------------------------------------------------------
			// VK_F1 ���� VK_F8 ������ ���ο� Ű������ ����, Ctrl�� ������ �� �����ϵ��� ����!
			// Added By ȫ����
			case VK_F1:
				{
					if( pEvent->bCtrl ) {
						ZGetGame()->m_pMyCharacter->SetVisible( !ZGetGame()->m_pMyCharacter->IsVisible() );
					}

					return false;					
				}				
				

			case VK_F2: 
				{
					if( pEvent->bCtrl ) {
						ZGetGameInterface()->ShowInterface(	!ZGetGameInterface()->IsShowInterface() );
					}
					
					return false;
				}
				

			case VK_F3:
				{
					if( pEvent->bCtrl ) {
						if (ZIsLaunchDevelop()) {
							ZApplication::GetGameInterface()->GetScreenDebugger()->SwitchDebugInfo();
							return true;
						}						
					}

					return false;
				}
				

			case VK_F4:
				{
					if( pEvent->bCtrl ) {
						ZGetGame()->m_bShowWireframe=!ZGetGame()->m_bShowWireframe;
						return true;
					}

					return false;
				}

			case VK_F5: 
				{
					if( pEvent->bCtrl ) {
						m_bCTOff = !m_bCTOff;
						RMesh::SetTextureRenderOnOff(m_bCTOff);
						return true;
					}

					return false;
				}
				

			case VK_F6:
				{
					if( pEvent->bCtrl ) {
						g_debug_rot =!g_debug_rot;
						return true;
					}

					return false;
				}

			case VK_F7: 
				{	
					if( pEvent->bCtrl ) {
						// �������ϸ� ����/��
						extern bool g_bProfile;
						if(g_bProfile) {
							ZPOSTCMD0(ZC_END_PROFILE);
							ZChatOutput("Profile saved.");
						} else {
							ZPOSTCMD0(ZC_BEGIN_PROFILE);
							ZChatOutput("Profile started.");
						}
						return true;
					}

					return false;
				}


			case VK_F8:
				{
					if( pEvent->bCtrl ) {
						RSolidBspNode::m_bTracePath = !RSolidBspNode::m_bTracePath;
					}
					return false;
				}

			case VK_F9:
				{
					static int nIndex = 0;
					nIndex++; if (nIndex >= 2) nIndex = 0;

					if (nIndex == 0)
					{
						ZGetGameInterface()->m_bViewUI = true;
						ZGetGameInterface()->GetGame()->m_pMyCharacter->SetVisible(true);

					}
					else if (nIndex == 1)
					{
						ZGetGameInterface()->m_bViewUI = false;
						ZGetGame()->m_pMyCharacter->SetVisible(false);
						ZGetGameInterface()->GetCombatInterface()->ShowCrossHair(false);
					}
				}
				return false;

			case 'U': {

				ZC_ENCHANT zctype;
//				int Module = 20;

				if(ZGetGame() && ZGetGame()->m_pMyCharacter ) {

					zctype = ZGetGame()->m_pMyCharacter->GetEnchantType();

//						 if(zctype==ZC_ENCHANT_FIRE)		Module = ZMID_FIREDAMAGE;
//					else if(zctype==ZC_ENCHANT_COLD)		Module = ZMID_COLDDAMAGE;
//					else if(zctype==ZC_ENCHANT_LIGHTNING)	Module = ZMID_LIGHTNINGDAMAGE;
//					else if(zctype==ZC_ENCHANT_POISON)		Module = ZMID_POISONDAMAGE;

					if(zctype==ZC_ENCHANT_FIRE) {
						ZModule_FireDamage *pModule = (ZModule_FireDamage*)ZGetGame()->m_pMyCharacter->GetModule(ZMID_FIREDAMAGE);
						pModule->BeginDamage(ZGetGame()->m_pMyCharacter,5,10);
					}
					else if(zctype==ZC_ENCHANT_COLD) {
						ZModule_ColdDamage *pModule = (ZModule_ColdDamage*)ZGetGame()->m_pMyCharacter->GetModule(ZMID_COLDDAMAGE);
						pModule->BeginDamage(10,50);
					}
					else if(zctype==ZC_ENCHANT_LIGHTNING) {
						ZModule_LightningDamage *pModule = (ZModule_LightningDamage*)ZGetGame()->m_pMyCharacter->GetModule(ZMID_LIGHTNINGDAMAGE);
						pModule->BeginDamage(ZGetGame()->m_pMyCharacter,5,10);
					}
					else if(zctype==ZC_ENCHANT_POISON) {
						ZModule_PoisonDamage *pModule = (ZModule_PoisonDamage*)ZGetGame()->m_pMyCharacter->GetModule(ZMID_POISONDAMAGE);
						pModule->BeginDamage(ZGetGame()->m_pMyCharacter,5,10);
					}
				}

				//g_pGame->m_pMyCharacter->ShotBlocked();
//				ZApplication::GetSoundEngine()->StopMusic();
				//ZApplication::GetSoundEngine()->load_preset();
					  }break;

			case 'M':
				{
					if (ZApplication::GetInstance()->GetLaunchMode()==ZApplication::ZLAUNCH_MODE_STANDALONE_GAME)
					{

						// ȥ���׽�Ʈ�Ҷ� �ǻ�Ƴ���
						if (ZGetGame()->GetMatch()->IsTeamPlay())
						{
							ZCharacter* pCharacter = ZGetGame()->m_pMyCharacter;
							pCharacter->InitStatus();
							rvector pos=rvector(0,0,0), dir=rvector(0,1,0);

							static int nTeamIndex = 0;
							static int nSpawnIndex = 0;

							ZMapSpawnData* pSpawnData = ZGetGame()->GetMapDesc()->GetSpawnManager()->GetTeamData(nTeamIndex, nSpawnIndex);
							if (pSpawnData != NULL)
							{
								pos = pSpawnData->m_Pos;
								dir = pSpawnData->m_Dir;
							}

							pCharacter->SetPosition(pos);
							pCharacter->SetDirection(dir);

							nSpawnIndex++;
							if (nSpawnIndex >= 16) 
							{
								nSpawnIndex = 0;
								nTeamIndex++;
								if (nTeamIndex >= 2) nTeamIndex=0;
							}
						}
						else
						{
							if(ZGetGame()->m_CharacterManager.size()==1)
								ZGetGameInterface()->RespawnMyCharacter();
						}
					}

				}break;

			case 'C' : {
//				ZModule_Skills *pmod = (ZModule_Skills *)g_pGame->m_pMyCharacter->GetModule(ZMID_SKILLS);
//				pmod->Excute(0,MUID(0,0),rvector(0,0,0));
					
//				g_pGame->UpdateCombo(true);


					   }break;

#ifdef _DEBUG
			case 'G' : {
//						MNewMemories::Dump();
//						g_pGame->m_pMyCharacter->m_bGuardTest=!g_pGame->m_pMyCharacter->m_bGuardTest;
					   }break;
#endif

				// �׽�Ʈ��..
			case 'I' : ZGetGame()->m_pMyCharacter->AddIcon(rand()%5); return true;
				// �׽�Ʈ^^

			case 'F' : DumpFontTexture(); return true;
//			case 'F' : 
//				{
//					static bool toggle = false;
//					if(toggle)
//						ZGetInput()->SetDeviceForcesXY(1,1);
//					else
//						ZGetInput()->SetDeviceForcesXY(0,0);
//					toggle=!toggle;
//					return true;
//				}
			case 'L' :
				{
//					rvector pos = g_pGame->m_pMyCharacter->GetPosition();
//					pos.x += 1000.0f;
//					ZApplication::GetSoundEngine()->PlayNPCSound(NPC_GOBLIN, NPC_SOUND_WOUND, pos, true);

					ZApplication::GetGameInterface()->FinishGame();
//					ZGetScreenEffectManager()->AddScreenEffect("teamredwin");

					/*
					g_pGame->m_pMyCharacter->m_Position = rvector( 2712.99805 , -1691.46191 , 2649.13403 );
					g_pGame->m_pMyCharacter->Move(rvector( 2.561 , -7.040 , -6.471 ));
					*/

					/*
					g_pGame->m_pMyCharacter->m_Position = rvector( 2713.05347 , -1691.56250 , 2929.06738 );
					g_pGame->m_pMyCharacter->Move(rvector( 0.00000000 , 0.00000000 , -2.07031250 ));
					*/

					/*
					g_pGame->m_pMyCharacter->m_Position = rvector( 1648.73877 ,8691.30176 ,1501.03381 -120);
					g_pGame->m_pMyCharacter->Move(rvector( -0.134,0.004, -0.986 ));
					g_pGame->m_pMyCharacter->m_Velocity = rvector( -450,0,0);
					
					ZPostLocalMessage(MSG_HACKING_DETECTED);
					

					ZGetEffectManager()->AddShotgunEffect(rvector(0,0,100),rvector(0,0,100),rvector(0,1,0),g_pGame->m_pMyCharacter);
					*/
				}break;

			case 'K':
				{
//					rvector pos = g_pGame->m_pMyCharacter->GetPosition();
//					pos.x += 1000.0f;
//					ZApplication::GetSoundEngine()->PlayNPCSound(NPC_GOBLIN, NPC_SOUND_WOUND, pos, false);

//					ZGetGameInterface()->GetCamera()->Shock(2000.f, .5f, rvector(0.0f, 0.0f, -1.0f));
					ZGetScreenEffectManager()->ShockBossGauge(35.0f);

					static int n = 0; n++;
					ZGetScreenEffectManager()->AddKO(n);

//					g_pGame->m_pMyCharacter->OnBlast(rvector(1,0,0));
//					ZGetGameInterface()->GetCamera()->Shock(500.f, .5f, rvector(0.0f, 0.0f, -1.0f));
				}break;
			case 'B':
				{
					// test
					ZCombatInterface* pCombatInterface = ZGetGameInterface()->GetCombatInterface();
					pCombatInterface->SetObserverMode(!pCombatInterface->GetObserver()->IsVisible());
				}
				return true;
			case 'N':
				{
					ZCombatInterface* pCombatInterface = ZGetGameInterface()->GetCombatInterface();
					if(pCombatInterface->GetObserverMode())
						pCombatInterface->GetObserver()->ChangeToNextTarget();
				}
				return true;
			case 'P':
				{
					// test
//					g_pGame->m_pMyCharacter->OnKnockback(rvector(1.0f, 0.0f, 0.0f),200.f);
					ZGetEffectManager()->AddMethorEffect(rvector(0,0,0) , 1 );

				}
				return true;
			case 'O':
				{
					if (ZApplication::GetInstance()->GetLaunchMode()==ZApplication::ZLAUNCH_MODE_STANDALONE_AI)
					{
						ZGetObjectManager()->ClearNPC();
						// npc ���� test
						MUID uidNPC = MUID(0,0);
						uidNPC.High = rand() % RAND_MAX;
						uidNPC.Low = rand() % RAND_MAX;

						int nNPCType = rand() % NPC_GOBLIN_KING+1;

						nNPCType = NPC_GOBLIN_GUNNER;
						rvector ranpos = rvector(0, 0, 0);

						MQuestNPCInfo* pNPCInfo = NULL;

						if(ZGetQuest())
							pNPCInfo = 	ZGetQuest()->GetNPCInfo(MQUEST_NPC(nNPCType));

						ZActor* pNewActor = ZActor::CreateActor(MQUEST_NPC(nNPCType), 1.0f, 0);
						if (pNewActor)
						{
							pNewActor->SetUID(uidNPC);
							pNewActor->SetPosition(ranpos);
							pNewActor->SetMyControl(true);

							if(pNewActor->m_pVMesh && pNPCInfo) {

								D3DCOLORVALUE color;

								color.r = pNPCInfo->vColor.x;
								color.g = pNPCInfo->vColor.y;
								color.b = pNPCInfo->vColor.z;
								color.a = 1.f;

								pNewActor->m_pVMesh->SetNPCBlendColor(color);//���� ������ ���..
							}

							ZGetObjectManager()->Add(pNewActor);
							ZGetEffectManager()->AddReBirthEffect(ranpos);
						}
					}
				}
				return true;
			
/*
			case VK_HOME:

				if(ZGetGameInterface()->GetGame()) {
					RFrameTime* ft = &ZGetGameInterface()->GetGame()->m_pMyCharacter->m_pVMesh->m_FrameTime;

					if( ft->m_bActive ) {
						ft->Stop();
					}
					else {
						ft->Start(20,300);
					}
				}
				return true;
*/
			// �����δ� NUMPAD�� ��ũ�η� ���� ���̴�. ����!
			// �׷��Ƿ�, NUMPAD1~NUMPAD9�� Return false�� ����!
			case VK_NUMPAD0: ZGetGameInterface()->TestToggleCharacter();return true;
			case VK_NUMPAD1: ZGetGameInterface()->TestChangeParts(0);	return false;
			case VK_NUMPAD2: ZGetGameInterface()->TestChangeParts(1);	return false;
			case VK_NUMPAD3: ZGetGameInterface()->TestChangeParts(2);	return false;
			case VK_NUMPAD4: ZGetGameInterface()->TestChangeParts(3);	return false;
			case VK_NUMPAD5: ZGetGameInterface()->TestChangeParts(4);	return false;
			case VK_NUMPAD6: ZGetGameInterface()->TestChangeParts(5);	return false;
			case VK_NUMPAD7: ZGetGameInterface()->TestChangeWeapon();	return false;			
			case VK_NUMPAD9:
				bMusicMute = !bMusicMute;
				ZGetSoundEngine()->SetMusicMute(bMusicMute);
				return false;

			/* �̰� ���� ? -_-
			case VK_OEM_3:	// `
				{
					ZGetGameInterface()->SetCursorEnable(
						!ZGetGameInterface()->IsCursorEnable());
				}
				return true;
			*/

#ifdef USING_VERTEX_SHADER
			case 'V':
				{
					//RShaderMgr::shader_enabled = !RShaderMgr::shader_enabled;
					RShaderMgr::mbUsingShader = !RShaderMgr::mbUsingShader;
				}
				return false;
#endif			

			}

		}
		break;

	} // switch

	return false;
}

