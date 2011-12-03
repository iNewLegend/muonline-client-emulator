#include "stdafx.h"
#include "ZGameAction.h"
#include "ZGame.h"
#include "ZGameClient.h"
#include "ZEffectManager.h"
#include "ZApplication.h"
#include "ZSoundEngine.h"
#include "ZMyCharacter.h"
#include "ZPost.h"
#include "ZModule_FireDamage.h"
#include "ZModule_ColdDamage.h"
#include "ZModule_LightningDamage.h"
#include "ZModule_PoisonDamage.h"

#define MAX_ENCHANT_DURATION	10.f

bool ZGameAction::OnCommand(MCommand* pCommand)
{
	switch (pCommand->GetID())
	{
		HANDLE_COMMAND(MC_PEER_ENCHANT_DAMAGE,		OnEnchantDamage)
		HANDLE_COMMAND(MC_PEER_REACTION,			OnReaction)
		HANDLE_COMMAND(MC_PEER_SKILL,				OnPeerSkill)
	}

	return false;
}

bool ZGameAction::OnReaction(MCommand* pCommand)
{
	float fTime;
	int nReactionID;

	pCommand->GetParameter(&fTime,			0, MPT_FLOAT);		// �ð�
	pCommand->GetParameter(&nReactionID,	1, MPT_INT);

	ZCharacter *pChar=ZGetCharacterManager()->Find(pCommand->GetSenderUID());
	if(!pChar) return true;

	switch(nReactionID)
	{
		case ZR_CHARGING	: {
			//pChar->m_bCharging=true;
			pChar->m_bCharging->Set_CheckCrc(true);	//mmemory proxy
			if(!pChar->IsHero())
				pChar->SetAnimationLower(ZC_STATE_CHARGE);
			ZGetEffectManager()->AddChargingEffect(pChar);

			if ( pChar->GetProperty()->nSex == MMS_MALE)
				ZGetSoundEngine()->PlaySound( "fx2/MAL05", pChar->GetPosition());
			else
				ZGetSoundEngine()->PlaySound( "fx2/FEM05", pChar->GetPosition());
		}break;
		case ZR_CHARGED		: {
			//pChar->m_bCharged=true;	
			pChar->m_bCharged->Set_CheckCrc(true);	//mmemory proxy
			pChar->m_fChargedFreeTime.Set_CheckCrc( ZGetGame()->GetTime() + fTime);
			ZGetEffectManager()->AddChargedEffect(pChar);

			ZGetSoundEngine()->PlaySoundChargeComplete(pChar->GetSelectItemDesc(), pChar->GetPosition());
		}break;
		case ZR_BE_UPPERCUT	: {
			rvector tpos = pChar->GetPosition();
			tpos.z += 130.f;
			ZGetEffectManager()->AddSwordUppercutDamageEffect(tpos,pChar->GetUID());
			ZGetSoundEngine()->PlaySound("uppercut", tpos);
		}break;
		case ZR_DISCHARGED	: {
			//pChar->m_bCharged=false;
			pChar->m_bCharged->Set_CheckCrc(false);
		}break;
	}

	return true;
}

bool ZGameAction::OnPeerSkill(MCommand* pCommand)
{
	float fTime;
	int nSkill,sel_type;

	pCommand->GetParameter(&fTime, 0, MPT_FLOAT);
	pCommand->GetParameter(&nSkill, 1, MPT_INT);
	pCommand->GetParameter(&sel_type, 2, MPT_INT);

	ZCharacter* pOwnerCharacter = ZGetCharacterManager()->Find(pCommand->GetSenderUID());
	if (pOwnerCharacter == NULL) return true;

	switch(nSkill)	{
		// ���� ��ų
		case ZC_SKILL_UPPERCUT		: OnPeerSkill_Uppercut(pOwnerCharacter);break;
			// ������ ���÷���
		case ZC_SKILL_SPLASHSHOT	: OnPeerSkill_LastShot(fTime,pOwnerCharacter);break;
			// �ܰ� Ư������
		case ZC_SKILL_DASH			: OnPeerSkill_Dash(pOwnerCharacter);break;
	}

	return true;
}

// ������ ó���Ѵ�. ���� �¾Ҵ����� �˻��Ѵ�
void ZGameAction::OnPeerSkill_LastShot(float fShotTime,ZCharacter *pOwnerCharacter)	// Į ������ �� ���÷���
{
	//jintriple3 ����� �������� �� ���� �Ҹ��� ��. ������ ��忡�� ���� if���� code optimize�������� �����ع����Ƿ�...
	bool bReturnValue;
	//jintriple3 ����� �������� �� ��� �ڵ�...
	bReturnValue = pOwnerCharacter == NULL;
	if( pOwnerCharacter == NULL )
		PROTECT_DEBUG_REGISTER( bReturnValue )
			return;
	ZItem *pItem = pOwnerCharacter->GetItems()->GetItem(MMCIP_MELEE);
	bReturnValue = !pItem;
	if(!pItem) 
		PROTECT_DEBUG_REGISTER( bReturnValue )
			return;

	MMatchItemDesc* pDesc = pItem->GetDesc();
	bReturnValue = !pDesc;
	if(!pDesc) 
		PROTECT_DEBUG_REGISTER( bReturnValue )
			return;


	// fShotTime �� �� ĳ������ ���� �ð��̹Ƿ� �� �ð����� ��ȯ���ش�
	fShotTime -= pOwnerCharacter->m_fTimeOffset;
	/*
	float fCurrentTime = g_pGame->GetTime();

	if( abs(fCurrentTime - fShotTime ) > TIME_ERROR_BETWEEN_RECIEVEDTIME_MYTIME )
	{
#ifdef _DEBUG
		mlog("!!!!������ �� ���!!!!ĳ���� ����: %s      fShotTime : %f     fCurrentTime : %f \n", 
			pOwnerCharacter->GetUserName(), fShotTime - pOwnerCharacter->m_fTimeOffset , fCurrentTime);
#endif
		return;
	}
	�� �κ��� �ٿ��� shot�� �� �ð��� �����Ͽ� ������ ���� �����Ͽ� ���� ���� �ڵ忴�µ� �޴� �ʿ��� �ð� �˻縦 ���� ���� 
	������ �ʿ��� �˻縦 �ؼ� shot�� �� �ð��� �ش� ĳ������ lacal time�� ���� ������ �ƿ� ��Ŷ�� ������ �ʵ��� �ٲ��. 
	���� �ش� �ڵ尡 �ʿ� ���� ��. ���� localtime�� ������ ��츦 ����� �ּ�ó���� ���ܵ�..
	*/

		// ���������� �߻�ӵ��� �����Ѵ�.
	//jintriple3 ����� �������� �� ���...�̰� ������ �� ��� ����..
	//bReturnValue = pOwnerCharacter->CheckValidShotTime(pItem->GetDescID(), fShotTime, pItem);

	if (pOwnerCharacter->CheckValidShotTime(pItem->GetDescID(), fShotTime, pItem))
	{
		PROTECT_DEBUG_REGISTER( pOwnerCharacter->m_dwIsValidTime == FOR_DEBUG_REGISTER )
		{
			pOwnerCharacter->UpdateValidShotTime(pItem->GetDescID(), fShotTime);
		}

	}
	else 
	{
		pOwnerCharacter->UpdateValidShotTime(pItem->GetDescID(), fShotTime);
		// _ASSERT(FALSE);	//�� ����ڵ�...���Ʈ�� �����ؼ� ���� �ȵǴ� �ӵ��� �� �Լ��� �Ҹ��� ������ ����������� �Ѵ�. 
		return;
	}


	/////////////////////////////////////////////////////////////////////////////////////
//#endif

	const fRange = 300.f;			// ������ 4����

//	if(pOwnerCharacter->m_AniState_Lower>=ZC_STATE_LOWER_ATTACK3 && pOwnerCharacter->m_AniState_Lower<=ZC_STATE_LOWER_ATTACK5)
	{
		// fShotTime �� �� ĳ������ ���� �ð��̹Ƿ� �� �ð����� ��ȯ���ش�
//		fShotTime-=pOwnerCharacter->m_fTimeOffset;

		rvector OwnerPosition,OwnerDir;
		bReturnValue = !pOwnerCharacter->GetHistory(&OwnerPosition,&OwnerDir,fShotTime);
		if(!pOwnerCharacter->GetHistory(&OwnerPosition,&OwnerDir,fShotTime))
			PROTECT_DEBUG_REGISTER( bReturnValue )
				return;


		rvector waveCenter = OwnerPosition; // ������ �߽�

		rvector _vdir = OwnerDir;
		_vdir.z = 0;
//		Normalize(_vdir);
//		waveCenter += _vdir * 180.f;

		ZC_ENCHANT zc_en_type = pOwnerCharacter->GetEnchantType();

		// ����
		ZGetSoundEngine()->PlaySoundSmash(pDesc, waveCenter, pOwnerCharacter->IsObserverTarget());

		// �ٴ��� wave ����Ʈ
		{
			ZGetEffectManager()->AddSwordWaveEffect(waveCenter,0,pOwnerCharacter);
		}

		for (ZObjectManager::iterator itor = ZGetObjectManager()->begin();
			itor != ZGetObjectManager()->end(); ++itor)
		{
			ZObject* pTar = (*itor).second;
			//jintriple3 ����� �������� ��ŷ ��� �ڵ�
			bReturnValue = pTar==NULL;
			if (pTar==NULL) 
				PROTECT_DEBUG_REGISTER( bReturnValue )
					continue;
			bReturnValue = pOwnerCharacter == pTar;
			if (pOwnerCharacter == pTar) 
				PROTECT_DEBUG_REGISTER(bReturnValue)
					continue;
			bReturnValue = pTar!=ZGetGame()->m_pMyCharacter && (!pTar->IsNPC() || !((ZActor*)pTar)->IsMyControl());
			if(pTar!=ZGetGame()->m_pMyCharacter &&	// �� ĳ���ͳ� ���� �����ϴ� npc �� üũ�Ѵ�
				(!pTar->IsNPC() || !((ZActor*)pTar)->IsMyControl()))
				PROTECT_DEBUG_REGISTER(bReturnValue)
					continue;

			bReturnValue = !ZGetGame()->CanAttack(pOwnerCharacter, pTar);
			if(!ZGetGame()->CanAttack(pOwnerCharacter,pTar)) 
				PROTECT_DEBUG_REGISTER(bReturnValue)
					continue;
			//// ���÷��̰� ���� ���̰� ��ų �Ұ��� �Ǿ������� �Ѿ��
			//if(ZGetGame()->GetMatch()->IsTeamPlay() &&
			//	pOwnerCharacter->IsTeam(pTar) && !g_pGame->GetMatch()->GetTeamKillEnabled()) return;

			rvector TargetPosition,TargetDir;
			
			bReturnValue = pTar->IsDie();
			if(pTar->IsDie()) 
				PROTECT_DEBUG_REGISTER(bReturnValue)
					continue;
			// ������ ��ġ�� ���� ������ ��������~
			bReturnValue = !pTar->GetHistory(&TargetPosition, &TargetDir, fShotTime);
			if( !pTar->GetHistory(&TargetPosition,&TargetDir,fShotTime)) 
				PROTECT_DEBUG_REGISTER(bReturnValue)
					continue;

			rvector checkPosition = TargetPosition + rvector(0,0,80);
			float fDist = Magnitude(waveCenter - checkPosition);
			//jintriple3 ����� �������� ��ŷ ��� �ڵ�
			bReturnValue = fDist >= fRange;
			if( fDist >= fRange)
				PROTECT_DEBUG_REGISTER(bReturnValue)
					continue;

			bReturnValue = (!pTar) || (pTar == pOwnerCharacter);
			if( (!pTar) || (pTar == pOwnerCharacter) )
				PROTECT_DEBUG_REGISTER(bReturnValue)
					continue;

			// �߰��� ���� ���� �ִ°�?
			bReturnValue = ZGetGame()->CheckWall( pOwnerCharacter, pTar) == true;
			if( ZGetGame()->CheckWall( pOwnerCharacter,pTar ) == true)
				PROTECT_DEBUG_REGISTER(bReturnValue)
					continue;
			// ���������� �������� �ȹ޴´�
			bReturnValue = pTar->IsGuard() && DotProduct(pTar->m_Direction,OwnerDir)<0;
			if( pTar->IsGuard() && DotProduct(pTar->m_Direction,OwnerDir)<0 )
			{
				PROTECT_DEBUG_REGISTER(bReturnValue)
				{
					rvector addVel = pTar->GetPosition() - waveCenter;
					Normalize(addVel);
					addVel = 500.f*addVel;
					addVel.z = 200.f;
					pTar->AddVelocity(addVel);
					continue;
				}
			}
			//��� ������ ���������
			rvector tpos = pTar->GetPosition();

			tpos.z += 130.f;
			if( zc_en_type == ZC_ENCHANT_NONE ) 
				ZGetEffectManager()->AddSwordUppercutDamageEffect(tpos,pTar->GetUID());
			else 
				ZGetEffectManager()->AddSwordEnchantEffect(zc_en_type,pTar->GetPosition(),20);

			tpos -= pOwnerCharacter->m_Direction * 50.f;
			rvector fTarDir = pTar->GetPosition() - pOwnerCharacter->GetPosition();
			Normalize(fTarDir);

#define MAX_DMG_RANGE	50.f	// �ݰ��̸�ŭ ������ �ִ� �������� �� �Դ´�
#define MIN_DMG			0.3f	// �ּ� �⺻ �������� ������.

			float fDamageRange = 1.f - (1.f-MIN_DMG)*( max(fDist-MAX_DMG_RANGE,0) / (fRange-MAX_DMG_RANGE));
//							pTar->OnDamagedKatanaSplash( pOwnerCharacter, fDamageRange );
#define SPLASH_DAMAGE_RATIO	.4f		// ���÷��� ������ �����
#define SLASH_DAMAGE	3		// �����ⵥ���� = �Ϲݰ����� x SLASH_DAMAGE

			int damage = (int) pDesc->m_nDamage.Ref()* fDamageRange;

			// ��æƮ �Ӽ��� �������� 1�� �������� �Դ´�. 2005.1.14
			if(zc_en_type == ZC_ENCHANT_NONE)
				damage *=  SLASH_DAMAGE;

			pTar->OnDamaged(pOwnerCharacter,pOwnerCharacter->GetPosition(),ZD_KATANA_SPLASH,MWT_KATANA,damage,SPLASH_DAMAGE_RATIO);
			pTar->OnDamagedAnimation(pOwnerCharacter,SEM_WomanSlash5);

			ZPostPeerEnchantDamage(pOwnerCharacter->GetUID(), pTar->GetUID());

/*
			if (fDist < fRange) 
			{
				if ((pTar) && (pTar != pOwnerCharacter)) 
				{
					if(g_pGame->CheckWall( pOwnerCharacter,pTar )==false) // �߰��� ���� ���� �ִ°�?
					{
						// ���������� �������� �ȹ޴´�
						if(pTar->IsGuard() && DotProduct(pTar->m_Direction,OwnerDir)<0 )
						{
							rvector addVel = pTar->GetPosition() - waveCenter;
							Normalize(addVel);
							addVel = 500.f*addVel;
							addVel.z = 200.f;
							pTar->AddVelocity(addVel);
						}else
						{
							rvector tpos = pTar->GetPosition();

							tpos.z += 130.f;
							if( zc_en_type == ZC_ENCHANT_NONE ) {
								ZGetEffectManager()->AddSwordUppercutDamageEffect(tpos,pTar->GetUID());
							}
							else {
								ZGetEffectManager()->AddSwordEnchantEffect(zc_en_type,pTar->GetPosition(),20);
							}
							tpos -= pOwnerCharacter->m_Direction * 50.f;
							rvector fTarDir = pTar->GetPosition() - pOwnerCharacter->GetPosition();
							Normalize(fTarDir);
#define MAX_DMG_RANGE	50.f	// �ݰ��̸�ŭ ������ �ִ� �������� �� �Դ´�
#define MIN_DMG			0.3f	// �ּ� �⺻ �������� ������.
							float fDamageRange = 1.f - (1.f-MIN_DMG)*( max(fDist-MAX_DMG_RANGE,0) / (fRange-MAX_DMG_RANGE));
//							pTar->OnDamagedKatanaSplash( pOwnerCharacter, fDamageRange );
#define SPLASH_DAMAGE_RATIO	.4f		// ���÷��� ������ �����
#define SLASH_DAMAGE	3		// �����ⵥ���� = �Ϲݰ����� x SLASH_DAMAGE
							int damage = (int) pDesc->m_nDamage * fDamageRange;
							// ��æƮ �Ӽ��� �������� 1�� �������� �Դ´�. 2005.1.14
							if(zc_en_type == ZC_ENCHANT_NONE)
								damage *=  SLASH_DAMAGE;
							pTar->OnDamaged(pOwnerCharacter,pOwnerCharacter->GetPosition(),ZD_KATANA_SPLASH,MWT_KATANA,damage,SPLASH_DAMAGE_RATIO);
							pTar->OnDamagedAnimation(pOwnerCharacter,SEM_WomanSlash5);
							ZPostPeerEnchantDamage(pOwnerCharacter->GetUID(), pTar->GetUID());
						} // �������� �Դ´�
					}
				}
			}*/
		}
#define KATANA_SHOCK_RANGE		1000.f			// 10���ͱ��� ��鸰��

		float fPower= (KATANA_SHOCK_RANGE-Magnitude(ZGetGame()->m_pMyCharacter->GetPosition()+rvector(0,0,50) - OwnerPosition))/KATANA_SHOCK_RANGE;
		if(fPower>0)
			ZGetGameInterface()->GetCamera()->Shock(fPower*500.f, .5f, rvector(0.0f, 0.0f, -1.0f));

	}
	/*
	else{

#ifndef _PUBLISH

		// �̰� Į�� ����� ���ѳ��̴�. �����ϴ�.
		char szTemp[256];
		sprintf(szTemp, "%s ġƮ ?", pOwnerCharacter->GetProperty()->szName);
		ZChatOutput(MCOLOR(0xFFFF0000), szTemp);

		mlog("anistate %d\n",pOwnerCharacter->m_AniState_Lower);

#endif//_PUBLISH

	}
	*/
}

void ZGameAction::OnPeerSkill_Uppercut(ZCharacter *pOwnerCharacter)
{
	float fShotTime=ZGetGame()->GetTime();
	rvector OwnerPosition,OwnerDir;
	OwnerPosition = pOwnerCharacter->GetPosition();
	OwnerDir = pOwnerCharacter->m_Direction;
	OwnerDir.z=0; 
	Normalize(OwnerDir);


	if ( !pOwnerCharacter->IsNPC())
	{
		if ( pOwnerCharacter->GetProperty()->nSex == MMS_MALE)
			ZGetSoundEngine()->PlaySound( "fx2/MAL_shot_01", pOwnerCharacter->GetPosition());
		else
			ZGetSoundEngine()->PlaySound( "fx2/FEM_shot_01", pOwnerCharacter->GetPosition());
	}


	for (ZObjectManager::iterator itor = ZGetObjectManager()->begin();
		itor != ZGetObjectManager()->end(); ++itor)
	{
		ZObject* pTar = (*itor).second;
		if (pOwnerCharacter == pTar) continue;

		rvector TargetPosition,TargetDir;

		if(pTar->IsDie()) continue;
		// ������ ��ġ�� ���� ������ ��������~
		if( !pTar->GetHistory(&TargetPosition,&TargetDir,fShotTime)) continue;

		float fDist = Magnitude(OwnerPosition + OwnerDir*10.f - TargetPosition);

		if (fDist < 200.0f) {

			if ((pTar) && (pTar != pOwnerCharacter))
			{
				bool bCheck = false;

				if (ZGetGame()->GetMatch()->IsTeamPlay())
				{
					if (IsPlayerObject(pTar)) {
						if( pOwnerCharacter->IsTeam( (ZCharacter*)pTar ) == false){
							bCheck = true;
						}
					}
					else {
						bCheck = true;
					}
				}
				else if (ZGetGame()->GetMatch()->IsQuestDrived())
				{
					if (!IsPlayerObject(pTar)) bCheck = true;
				}
				else {
					bCheck = true;
				}

				if(ZGetGame()->CheckWall(pOwnerCharacter,pTar)==true) //�߰��� ���� ���� �ִ°�?
					bCheck = false;

				if( bCheck) {//���̾ƴѰ�츸

					rvector fTarDir = pTar->GetPosition() - (pOwnerCharacter->GetPosition() - 50.f*OwnerDir);
					Normalize(fTarDir);
					float fDot = D3DXVec3Dot(&OwnerDir, &fTarDir);
					if (fDot>0)
					{
						int cm = ZGetGame()->SelectSlashEffectMotion(pOwnerCharacter);//���� Į �ֵθ��� ����

						rvector tpos = pTar->GetPosition();

						tpos.z += 130.f;

						/*
						if (IsPlayerObject(pTar))
						{
							// �켱 �÷��̾ ����Ʈ�� ���´�. - effect �� �����ϰ� NPC�� ������ �ٲ��� �� -bird
							ZGetEffectManager()->AddSwordUppercutDamageEffect(tpos,(ZCharacter*)pTar);
						}
						*/

						tpos -= pOwnerCharacter->m_Direction * 50.f;

						ZGetEffectManager()->AddBloodEffect( tpos , -fTarDir);
						ZGetEffectManager()->AddSlashEffect( tpos , -fTarDir , cm );

						ZGetGame()->CheckCombo(pOwnerCharacter, pTar , true);
						if (pTar == ZGetGame()->m_pMyCharacter) 
						{
							ZGetGame()->m_pMyCharacter->SetLastThrower(pOwnerCharacter->GetUID(), ZGetGame()->GetTime()+1.0f);
							ZPostReaction(ZGetGame()->GetTime(),ZR_BE_UPPERCUT);
						}
						pTar->OnBlast(OwnerDir);

						ZCharacter* pTarCharacter = (ZCharacter*)pTar;
						if ( !pTarCharacter->IsNPC())
						{
							if ( ((ZCharacter*)pTar)->GetProperty()->nSex == MMS_MALE)
								ZGetSoundEngine()->PlaySound( "fx2/MAL07", pTar->GetPosition());
							else
								ZGetSoundEngine()->PlaySound( "fx2/FEM07", pTar->GetPosition());
						}
					}
				}
			}
		}
	}
}

void ZGameAction::OnPeerSkill_Dash(ZCharacter *pOwnerCharacter)
{
	if(pOwnerCharacter->m_AniState_Lower.Ref()!=ZC_STATE_LOWER_UPPERCUT) return;

	float fShotTime=ZGetGame()->GetTime();
	rvector OwnerPosition,OwnerDir;
	OwnerPosition = pOwnerCharacter->GetPosition();
	OwnerDir = pOwnerCharacter->m_Direction;
	OwnerDir.z=0; 
	Normalize(OwnerDir);

	ZItem *pItem = pOwnerCharacter->GetItems()->GetItem(MMCIP_MELEE);
	if(!pItem) return;
	MMatchItemDesc *pDesc = pItem->GetDesc();
	if(!pDesc) { _ASSERT(FALSE); return; }

//	ZGetEffectManager()->AddSkillDashEffect(pOwnerCharacter->GetPosition(),pOwnerCharacter->m_Direction,pOwnerCharacter);

//	for (ZCharacterManager::iterator itor = ZGetCharacterManager()->begin();
//		itor != ZGetCharacterManager()->end(); ++itor)
	for (ZObjectManager::iterator itor = ZGetObjectManager()->begin();
		itor != ZGetObjectManager()->end(); ++itor)
	{
//		ZCharacter* pTar = (*itor).second;
		ZObject* pTar = (*itor).second;

		if (pOwnerCharacter == pTar) continue;

		rvector TargetPosition,TargetDir;

		if(pTar->IsDie()) continue;

		// ������ ��ġ�� ���� ������ ��������~
		if( !pTar->GetHistory(&TargetPosition,&TargetDir,fShotTime)) continue;

		float fDist = Magnitude(OwnerPosition + OwnerDir*10.f - TargetPosition);

		if (fDist < 600.0f) {// 6m

			if ((pTar) && (pTar != pOwnerCharacter)) {

				bool bCheck = false;
/*
				if (ZGetGame()->GetMatch()->IsTeamPlay()){
					if( pOwnerCharacter->IsTeam( pTar ) == false){
						bCheck = true;
					}
				}
				else {
					bCheck = true;
				}
*/
				if (ZGetGame()->GetMatch()->IsTeamPlay()){
					if (IsPlayerObject(pTar)) {
						if( pOwnerCharacter->IsTeam( (ZCharacter*)pTar ) == false){
							bCheck = true;
						}
					}
					else {
						bCheck = true;
					}
				}
				else {
					bCheck = true;
				}

				if(ZGetGame()->CheckWall(pOwnerCharacter,pTar)==true) //�߰��� ���� ���� �ִ°�?
					bCheck = false;

				if( bCheck) {//���̾ƴѰ�츸
					//				if( pOwnerCharacter->IsTeam( pTar ) == false) {//���̾ƴѰ�츸

					rvector fTarDir = pTar->GetPosition() - pOwnerCharacter->GetPosition();
					Normalize(fTarDir);

					float fDot = D3DXVec3Dot(&OwnerDir, &fTarDir);

					bool bDamage = false;

					if( fDist < 100.f) { // 1m ������ �տ��� �־..
						if(fDot > 0.f) {
							bDamage = true;
						}
					}
					else if(fDist < 300.f) {
						if(fDot > 0.5f) {
							bDamage = true;
						}
					}
					else {// 2m ~ 6m
						if(fDot > 0.96f) {
							bDamage = true;
						}
					}

					if ( bDamage ) {

						int cm = ZGetGame()->SelectSlashEffectMotion(pOwnerCharacter);//���� Į �ֵθ��� ����

						float add_time = 0.3f * (fDist / 600.f);
						float time = ZGetGame()->GetTime() + add_time;			// �Ÿ��� ���� �ð��� �޸��ϵ��� �����ϱ�..

						rvector tpos = pTar->GetPosition();

						tpos.z += 180.f;//���� ����

						ZGetEffectManager()->AddSwordUppercutDamageEffect(tpos,pTar->GetUID(),(DWORD)(add_time*1000));
//						ZGetEffectManager()->AddSwordUppercutDamageEffect(tpos,pTar);

						tpos -= pOwnerCharacter->m_Direction * 50.f;

//						ZGetEffectManager()->AddBloodEffect( tpos , -fTarDir);
//						ZGetEffectManager()->AddSlashEffect( tpos , -fTarDir , cm );
						// �Ҹ��� Ư�� �ð� �ڿ�
						ZGetSoundEngine()->PlaySound("uppercut", tpos );

						if (pTar == ZGetGame()->m_pMyCharacter) {
							rvector _dir = pTar->GetPosition() - pOwnerCharacter->GetPosition();
							_dir.z = 0.f;

//							m_pMyCharacter->OnDashAttacked(_dir);
							ZGetGame()->m_pMyCharacter->ReserveDashAttacked( pOwnerCharacter->GetUID(), time,_dir );
						}
						pTar->OnBlastDagger(OwnerDir,OwnerPosition);

						float fDamage = pDesc->m_nDamage.Ref() * 1.5f;// �⺻ ���ⵥ���� * 150 %
						float fRatio = pItem->GetPiercingRatio( pDesc->m_nWeaponType.Ref() , eq_parts_chest );

						if(ZGetGame()->CanAttack(pOwnerCharacter,pTar))//���� ������ ��츸.. ����Ʈ�� ��� �������� ����..
							pTar->OnDamagedSkill(pOwnerCharacter,pOwnerCharacter->GetPosition(),ZD_MELEE,MWT_DAGGER,fDamage,fRatio);

						ZGetGame()->CheckCombo(pOwnerCharacter, pTar,true);
					}

				}//IsTeam
			}
		}
	}
}


bool ZGameAction::OnEnchantDamage(MCommand* pCommand)
{
	MUID ownerUID;
	MUID targetUID;
	pCommand->GetParameter(&ownerUID,	0, MPT_UID);
	pCommand->GetParameter(&targetUID,	1, MPT_UID);

	ZCharacter* pOwnerCharacter = ZGetCharacterManager()->Find(ownerUID);
	ZObject* pTarget= ZGetObjectManager()->GetObject(targetUID);

	if (pOwnerCharacter == NULL || pTarget == NULL ) return true;

	MMatchItemDesc* pDesc = pOwnerCharacter->GetEnchantItemDesc();
	if(pDesc)
	{
		switch(pOwnerCharacter->GetEnchantType())
		{
			case ZC_ENCHANT_FIRE :
				ApplyFireEnchantDamage(pTarget, pOwnerCharacter, pDesc->m_nDamage.Ref(), pDesc->m_nDelay.Ref());
				break;
			case ZC_ENCHANT_COLD :
				ApplyColdEnchantDamage(pTarget, pDesc->m_nLimitSpeed.Ref(), pDesc->m_nDelay.Ref());
				break;
			case ZC_ENCHANT_POISON :
				ApplyPoisonEnchantDamage(pTarget, pOwnerCharacter, pDesc->m_nDamage.Ref(), pDesc->m_nDelay.Ref());
				break;
			case ZC_ENCHANT_LIGHTNING : 
				ApplyLightningEnchantDamage(pTarget, pOwnerCharacter, pDesc->m_nDamage.Ref(), pDesc->m_nDelay.Ref());
				break;
		};
	}

	return true;
}

bool ZGameAction::ApplyFireEnchantDamage(ZObject* pTarget, ZObject* pOwner, int nDamage, int nDuration)
{
	if (!pTarget) return false;

	ZModule_FireDamage *pMod = (ZModule_FireDamage*)pTarget->GetModule(ZMID_FIREDAMAGE);
	if(!pMod) return false;

	// �ѹ� �ɸ��� ���� �ð� ������ �ٽ� �ɸ��� �ʰ�
	if (pMod->IsOnDamage() && (ZGetGame()->GetTime() - pMod->GetDamageBeginTime() < 1.f))
		return false;

	rvector soundPos = pTarget->GetPosition();
	bool bObserverTarget = pTarget->GetUID()==ZGetCombatInterface()->GetTargetUID();
	char* szSoundName = (bObserverTarget) ? "we_enfire_2d" : "we_enfire";
	ZGetSoundEngine()->PlaySound(szSoundName, soundPos);

	pMod->BeginDamage( pOwner, nDamage, nDuration * 0.001f);
	return true;
}

bool ZGameAction::ApplyColdEnchantDamage(ZObject* pTarget, int nLimitSpeed, int nDuration)
{
	if (!pTarget) return false;

	ZModule_ColdDamage *pMod = (ZModule_ColdDamage*)pTarget->GetModule(ZMID_COLDDAMAGE);
	if(!pMod) return false;

	// �ѹ� �ɸ��� ���� �ð� ������ �ٽ� �ɸ��� �ʰ�
	if (pMod->IsOnDamage() && (ZGetGame()->GetTime() - pMod->GetDamageBeginTime() < 1.f))
		return false;

	rvector soundPos = pTarget->GetPosition();
	bool bObserverTarget = pTarget->GetUID()==ZGetCombatInterface()->GetTargetUID();
	char* szSoundName = (bObserverTarget) ? "we_enice_2d" : "we_enice";
	ZGetSoundEngine()->PlaySound(szSoundName, soundPos);

	if (nLimitSpeed < 0 || 100 < nLimitSpeed) return false;
	pMod->BeginDamage( (float)nLimitSpeed*0.01f, nDuration * 0.001f);
	return true;
}

bool ZGameAction::ApplyPoisonEnchantDamage(ZObject* pTarget, ZObject* pOwner, int nDamage, int nDuration)
{
	if (!pTarget) return false;

	ZModule_PoisonDamage *pMod = (ZModule_PoisonDamage*)pTarget->GetModule(ZMID_POISONDAMAGE);
	if(!pMod) return false;

	// �ѹ� �ɸ��� ���� �ð� ������ �ٽ� �ɸ��� �ʰ�
	if (pMod->IsOnDamage() && (ZGetGame()->GetTime() - pMod->GetDamageBeginTime() < 1.f))
		return false;

	rvector soundPos = pTarget->GetPosition();
	bool bObserverTarget = pTarget->GetUID()==ZGetCombatInterface()->GetTargetUID();
	char* szSoundName = (bObserverTarget) ? "we_enpoison_2d" : "we_enpoison";
	ZGetSoundEngine()->PlaySound(szSoundName, soundPos);

	pMod->BeginDamage( pOwner, nDamage, nDuration * 0.001f);
	return true;
}

bool ZGameAction::ApplyLightningEnchantDamage(ZObject* pTarget, ZObject* pOwner, int nDamage, int nDuration)
{
	if (!pTarget) return false;

	ZModule_LightningDamage *pMod = (ZModule_LightningDamage*)pTarget->GetModule(ZMID_LIGHTNINGDAMAGE);
	if(!pMod) return false;

	// �ѹ� �ɸ��� ���� �ð� ������ �ٽ� �ɸ��� �ʰ�
	if (pMod->IsOnDamage() && (ZGetGame()->GetTime() - pMod->GetDamageBeginTime() < 1.f))
		return false;

	rvector soundPos = pTarget->GetPosition();
	bool bObserverTarget = pTarget->GetUID()==ZGetCombatInterface()->GetTargetUID();
	char* szSoundName = (bObserverTarget) ? "we_enlight_2d" : "we_enlight";
	ZGetSoundEngine()->PlaySound(szSoundName, soundPos);

	pMod->BeginDamage( pOwner, nDamage, nDuration * 0.001f);
	return true;
}