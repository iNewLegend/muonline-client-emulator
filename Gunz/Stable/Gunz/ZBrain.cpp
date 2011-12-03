#include "stdafx.h"
#include "ZBrain.h"
#include "ZActor.h"
#include "ZGame.h"
#include "ZObject.h"
#include "ZMyCharacter.h"
#include "ZRangeWeaponHitDice.h"
#include "ZModule_Skills.h"
#include "MMath.h"
#include "ZGame.h"
#include "RNavigationNode.h"



ZBrain::ZBrain() : m_pBody(NULL), m_uidTarget( MUID( 0, 0))
{
	ResetStuckInState();
	ResetStuckInStateForWarp();
}


ZBrain::~ZBrain()
{
}


ZBrain* ZBrain::CreateBrain( MQUEST_NPC nNPCType)
{
	return new ZBrain();
}


ZObject* ZBrain::GetTarget()
{
#ifdef _DEBUG
	// ȥ�ڼ� AI �׽�Ʈ�� ���
	if ( (ZApplication::GetInstance()->GetLaunchMode() == ZApplication::ZLAUNCH_MODE_STANDALONE_QUEST) || 
		 (ZApplication::GetInstance()->GetLaunchMode() == ZApplication::ZLAUNCH_MODE_STANDALONE_AI))
	{
		return (ZObject*)ZGetGame()->m_pMyCharacter;
	}
#endif

	if ( ZGetObjectManager())
	{
		ZObject* pObject = ZGetObjectManager()->GetObject( m_uidTarget);
		return pObject;
	}

	return NULL;
}


float ZBrain::MakePathFindingUpdateTime( char nIntelligence)
{
	MQuestNPCGlobalAIValue* pGlobalAIValue = ZGetQuest()->GetNPCCatalogue()->GetGlobalAIValue();
	float fShakingRatio = pGlobalAIValue->m_fPathFinding_ShakingRatio;
	float fTime = pGlobalAIValue->m_fPathFindingUpdateTime[ nIntelligence - 1];
	float fExtraValue = fTime * fShakingRatio;
	float fMinTime = fTime - fExtraValue;
	if ( fMinTime < 0.0f)
		fMinTime = 0.0f;
	float fMaxTime = fTime + fExtraValue;

	return RandomNumber( fMinTime, fMaxTime);
}

float ZBrain::MakeAttackUpdateTime( char nAgility)
{
	MQuestNPCGlobalAIValue* pGlobalAIValue = ZGetQuest()->GetNPCCatalogue()->GetGlobalAIValue();
	float fShakingRatio = pGlobalAIValue->m_fAttack_ShakingRatio;
	float fTime = pGlobalAIValue->m_fAttackUpdateTime[ nAgility - 1];
	float fExtraValue = fTime * fShakingRatio;
	float fMinTime = fTime - fExtraValue;
	if ( fMinTime < 0.0f)
		fMinTime = 0.0f;
	float fMaxTime = fTime + fExtraValue;

	return RandomNumber( fMinTime, fMaxTime);
}


float ZBrain::MakeDefaultAttackCoolTime()
{
	if ( !m_pBody->GetNPCInfo())
		return 0.0f;

	float fShakingRatio = 0.3f;
	float fCoolTime = m_pBody->GetNPCInfo()->fAttackCoolTime;
	float fExtraValue = fCoolTime * fShakingRatio;
	float fMinCoolTime = max( (fCoolTime - fExtraValue), 0.01f);
	float fMaxCoolTime = fCoolTime + fExtraValue;

	return RandomNumber( fMinCoolTime, fMaxCoolTime);

}


float ZBrain::MakeSpeed( float fSpeed)
{
	MQuestNPCGlobalAIValue* pGlobalAIValue = ZGetQuest()->GetNPCCatalogue()->GetGlobalAIValue();
	float fShakingRatio = pGlobalAIValue->m_fSpeed_ShakingRatio;
	float fExtraValue = fSpeed * fShakingRatio;
	float fMinSpeed = max( (fSpeed - fExtraValue), 0.0f);
	float fMaxSpeed = fSpeed + fExtraValue;

	return RandomNumber( fMinSpeed, fMaxSpeed);
}


void ZBrain::MakeNeglectUpdateTime()
{
	m_dwNeglectTimer = timeGetTime() + 5500;
}


void ZBrain::Init( ZActor* pBody)
{
	m_pBody = pBody;
	m_Behavior.Init( this);

	if ( m_pBody->GetNPCInfo())
	{
		float fDefaultPathFindingUpdateTime		= ZBrain::MakePathFindingUpdateTime( m_pBody->GetNPCInfo()->nIntelligence);
		float fAttackUpdateTime					= ZBrain::MakeAttackUpdateTime( m_pBody->GetNPCInfo()->nAgility);
		float fDefaultAttackUpdateTime			= m_pBody->GetNPCInfo()->fAttackCoolTime;

		m_PathFindingTimer.Init( fDefaultPathFindingUpdateTime);
		m_AttackTimer.Init( fAttackUpdateTime);
		m_DefaultAttackTimer.Init( fDefaultAttackUpdateTime);
	}

	// ÷ �����ǰ� ���� �� �ð������� ��ų�� ������� �ʴ´�.
	m_dwNoSkillTimer = timeGetTime() + RandomNumber( 1000, 5000);

	// Neglect timer
	MakeNeglectUpdateTime();

	// Set distance
	m_fDistForcedIn		= DIST_FORCEDIN + RandomNumber( 1.0f, (DIST_FORCEDIN * 0.6f)) - (DIST_FORCEDIN * 0.6f / 2.0f);
	m_fDistIn			= DIST_IN       + RandomNumber( 1.0f, (DIST_IN       * 0.6f)) - (DIST_IN       * 0.6f / 2.0f);
	m_fDistOut			= DIST_OUT      + RandomNumber( 1.0f, (DIST_OUT      * 0.6f)) - (DIST_OUT      * 0.6f / 2.0f);
}


void ZBrain::Think( float fDelta)
{
	if ( m_pBody->isThinkAble())
	{
		MUID prevTarget = m_uidTarget;


		// Ÿ�� ã��
		bool bFind = FindTarget();


		// Ÿ���� ������...
		if ( bFind)
		{
			// ��ã��
			ProcessBuildPath( fDelta);


			// ����
			ProcessAttack( fDelta);
		}

		// Ÿ���� ������...
		else if ( prevTarget != MUID( 0, 0))
		{
			m_pBody->Stop();
			m_pBody->m_TaskManager.Clear();

			MakeNeglectUpdateTime();
	
			m_pBody->OnNeglect( 1);
		}
	}


	// Check neglect
	DWORD dwCurrTime = timeGetTime();
	if ( !m_pBody->m_TaskManager.IsEmpty())
		MakeNeglectUpdateTime();

	else if ( dwCurrTime > m_dwNeglectTimer)
	{
		m_pBody->OnNeglect( 1);

		MakeNeglectUpdateTime();
	}
}


bool ZBrain::FindTarget()
{
	MUID uidTarget	= MUID(0,0);
	float fDist		= FLT_MAX;

	for ( ZCharacterManager::iterator itor = ZGetCharacterManager()->begin();  itor != ZGetCharacterManager()->end();  ++itor)
	{
		// ���� ���� ���ɾ���.
		ZCharacter* pCharacter = (*itor).second;
		if ( pCharacter->IsDie())
			continue;

		// ���� ��Ͽ� �� ĳ���ʹ� �ǳʶ�
		if ( ZGetGame()->IsExceptedFromNpcTargetting( pCharacter))
			continue;

		// �Ÿ��� ���Ѵ�.
		float dist = MagnitudeSq( pCharacter->GetPosition() - m_pBody->GetPosition());


		// �� ����� ���̸� �̳��� Ÿ������ ���Ѵ�.
		if ( dist < fDist)
		{
			fDist = dist;
			uidTarget = pCharacter->GetUID();
		}
	}

	m_uidTarget = uidTarget;

	if ( uidTarget == MUID(0,0))
		return false;

	return true;
}


void ZBrain::ProcessAttack( float fDelta)
{
	bool bDefaultAttackEnabled = true;

	// Update time
	if ( m_pBody->GetNPCInfo() && (m_pBody->GetNPCInfo()->fAttackCoolTime != 0.0f))
		bDefaultAttackEnabled = m_DefaultAttackTimer.Update( fDelta);

	if ( !m_AttackTimer.Update(fDelta) && !bDefaultAttackEnabled)
		return;

	// Skip if friendly NPC
	if ( m_Behavior.IsFriendly())
		return;

	// Check attackable status
	if ( !m_pBody->IsAttackable())
		return;

	// Use default attack
	if ( bDefaultAttackEnabled && m_pBody->CanAttackMelee( GetTarget()) && !ZGetGame()->CheckWall(m_pBody, GetTarget(), true))
	{	// (���� ����Ÿ�� �����Ҷ� ��üũ�ϴ� �Լ�) CheckWall�� Ÿ�ٰ� �� ���̿� ��ֹ��� ������ Ȯ��- �ȱ׷��� ��� �ڿ��� ��� ���ģ��

		float fNextCoolTime = MakeDefaultAttackCoolTime();
		m_DefaultAttackTimer.Init( fNextCoolTime);

		ZTask* pNew = ZTaskManager::CreateAttackMelee( m_pBody);
		m_pBody->m_TaskManager.PushFront( pNew);

		return;
	}


	// Check skill useable
	if ( (m_pBody->GetNPCInfo()->nNPCAttackTypes & NPC_ATTACK_MAGIC) == NPC_ATTACK_NONE)
		return;

	ZTASK_ID nTaskID = m_pBody->m_TaskManager.GetCurrTaskID();
	if ( (nTaskID == ZTID_SKILL) || (nTaskID == ZTID_ROTATE_TO_DIR))
		return;

	if ( timeGetTime() < m_dwNoSkillTimer)
		return;


	// Get skill
	int nSkill;
	MUID uidTarget;
	rvector targetPosition;
	if ( GetUseableSkill( &nSkill, &uidTarget, &targetPosition)) 
	{
		// Use skill
		if ( m_pBody->CanSee( GetTarget()))
		{
			m_pBody->m_TaskManager.Clear();

			ZTask* pNew = ZTaskManager::CreateSkill( m_pBody, nSkill, uidTarget, targetPosition);
			m_pBody->m_TaskManager.Push( pNew);
		}
	}
}


bool ZBrain::GetUseableSkill( int *pnSkill, MUID *puidTarget, rvector *pTargetPosition)
{
	// Get skill module
	ZModule_Skills *pmod = (ZModule_Skills *)m_pBody->GetModule(ZMID_SKILLS);
	if ( !pmod)
		return false;

	// Set value
	if ( puidTarget)
		(*puidTarget) = MUID(0,0);

	if (pTargetPosition)
		(*pTargetPosition) = rvector(0.0f,0.0f,0.0f);


	// Check skills
	for ( int i = 0;  i < pmod->GetSkillCount();  i++)
	{
		ZSkill *pSkill = pmod->GetSkill( i);

		// Check cool time
		if ( !pSkill->IsReady())
			continue;

		// Get skill description
		ZSkillDesc *pDesc = pmod->GetSkill( i)->GetDesc();


		// ��ų�� ���� ����� �Ʊ��� ���...
		if ( pDesc->IsAlliedTarget())
		{
			// ȿ���� �ִ� ����� ������ �ִ� �� ã�´�.
			float fDist = DIST_OUT;
			ZObject *pAlliedTarget = NULL;


			for ( ZObjectManager::iterator itor = ZGetObjectManager()->begin();  itor != ZGetObjectManager()->end();  ++itor)
			{
				ZObject *pObject = itor->second;

				// ���� ���� �Ѿ��
				if ( pObject->IsDie())
					continue;

				// ���̸� �Ѿ��
				if ( ZGetGame()->CanAttack(m_pBody,pObject))
					continue;

				// �ڱ� �ڽ��̸� �Ѿ��
				if ( pObject == m_pBody)
					continue;


				// Get distance
				float dist = MagnitudeSq( pObject->GetPosition() - m_pBody->GetPosition());
				if ( pSkill->IsUsable( pObject) && ( dist < fDist))
				{
					fDist = dist;
					pAlliedTarget = pObject;
				}
			}	

			// ���� ����� ������ �ڱ� �ڽ����׶� ��ų�� �Ǵ�.
			if ( ( pAlliedTarget == NULL) && ( pSkill->IsUsable( m_pBody)))
				pAlliedTarget = m_pBody;

			if (pAlliedTarget)
			{
				if ( pnSkill)
					*pnSkill = i;
				
				if ( puidTarget)
					*puidTarget = pAlliedTarget->GetUID();

				if ( pTargetPosition)
					*pTargetPosition = pAlliedTarget->GetCenterPos();

				return true;
			}
		}

		// ��ų�� ���� ����� ������ ���...
		else
		{
			ZObject* pTarget = GetTarget();
			if ( pTarget == NULL)
				continue;

			// Check useable
			if ( !pSkill->IsUsable( pTarget))
				continue;

			// Get pick info
			ZPICKINFO pickinfo;
			memset( &pickinfo, 0, sizeof( ZPICKINFO));


			// Check picking
			rvector pos, tarpos, dir;
			// ���� ���� ���� �Ǹ������� ���� ������ ���� �������� ��ŷ ������..
			pos = m_pBody->GetPosition() + rvector( 0, 0, m_pBody->GetCollHeight()*0.5f*0.8f);		// �������� �����ַ��� *0.8
			tarpos = pTarget->GetPosition() + rvector( 0, 0, pTarget->GetCollHeight()*0.5f*0.8f);
			dir = tarpos - pos;
			Normalize( dir);

			const DWORD dwPickPassFlag = RM_FLAG_ADDITIVE | RM_FLAG_HIDE | RM_FLAG_PASSROCKET | RM_FLAG_PASSBULLET;
			if ( ZGetGame()->Pick( m_pBody, pos, dir, &pickinfo, dwPickPassFlag))
			{
				if ( pickinfo.pObject)
				{
					if ( pnSkill)
						*pnSkill = i;

					if ( puidTarget)
						*puidTarget = pTarget->GetUID();

					if ( pTargetPosition)
						*pTargetPosition = pTarget->GetCenterPos();

					return true;
				}
			}
		}
	}

	return false;
}


void ZBrain::ProcessBuildPath( float fDelta)
{
	// Update timer
	if ( !m_PathFindingTimer.Update( fDelta))
		return;
	
	// Check status
	ZTASK_ID nTaskID = m_pBody->m_TaskManager.GetCurrTaskID();
	if ( (nTaskID == ZTID_ATTACK_MELEE) || (nTaskID == ZTID_ATTACK_RANGE) || (nTaskID == ZTID_ROTATE_TO_DIR) || (nTaskID == ZTID_SKILL))
		return;

	// �ʿ� �����ٸ� �����
	// �����ؼ� ������ Ż���ϴ� �� �����̹��϶������� �����Ѵ� (������ ���� ũ�� ��� ������ �ذ���̴�. ���� ����Ʈ���� ���� �������� �Ͼ ���� �Ҹ��� ����)
	if (ZGetGameTypeManager()->IsSurvivalOnly( ZGetGame()->GetMatch()->GetMatchType()))
	{
		if (EscapeFromStuckIn(m_WayPointList))
			return;
	}

	// Get target
	ZObject* pTarget = GetTarget();
	if ( !pTarget)
	{
		m_pBody->m_TaskManager.Clear();
		m_pBody->Stop();

		return;
	}


	// ���Ÿ� �����̰ų� ��ȣ���̸� �� ������ �ٰ����� �ʰ� �ٶ� ����.
	if ( ( m_Behavior.GetOffenseType() == ZOFFENSETYPE_RANGE) || m_Behavior.IsFriendly())
	{
		// �Ÿ��� ���Ѵ�.
		float dist = MagnitudeSq( pTarget->GetPosition() - m_pBody->GetPosition());

		bool bStop = false;

		// Friendly type
		if ( m_Behavior.IsFriendly())
		{
			if ( dist < m_fDistForcedIn)
				bStop = true;
		}
		// Else type
		else
		{
			// ���� �Ÿ��� ����.
			if ( ( dist > DIST_FORCEDIN) && (dist < m_fDistIn))
			{
				// ���� �Ÿ��� ���� ���̰� ���� ���̰� ������ ����.
				dist = pTarget->GetPosition().z - m_pBody->GetPosition().z;

				// ���̰� �� ���� ���� �ȳ��� ����
				if ( (dist > -DIST_HEIGHT) && (dist < DIST_HEIGHT))
					bStop = true;
			}
		}

		// Stop
		if ( bStop)
		{
			// �� �� �ִ� ��ġ������ ������ �����ϴ�. ���� �Ⱥ��δٸ� �پ�� ���������� �ϵ��� �Ѵ�
			if ( m_pBody->CanSee( pTarget) && m_pBody->CanAttackRange( pTarget))
			{
				m_pBody->Stop();
				m_pBody->m_TaskManager.Clear();

				return;
			}
		}
	}


	// Make path
	RNavigationMesh* pNavMesh = ZGetGame()->GetWorld()->GetBsp()->GetNavigationMesh();
	if ( pNavMesh == NULL)
		return;

	// Make navigation path
	rvector tarpos = pTarget->GetPosition();
	if ( !pNavMesh->BuildNavigationPath( m_pBody->GetPosition(), tarpos))
		return;

	m_WayPointList.clear();
	for ( list<rvector>::iterator itor = pNavMesh->GetWaypointList().begin();  itor != pNavMesh->GetWaypointList().end();  ++itor)
		m_WayPointList.push_back( (*itor));


	AdjustWayPointWithBound(m_WayPointList, pNavMesh);

	PushWayPointsToTask();
}

void ZBrain::PushWayPointsToTask()
{
	// Push task
	if ( m_WayPointList.empty())
		return;

	m_pBody->m_TaskManager.Clear();
	int nTotal	= (int)m_WayPointList.size();
	int cnt		= 0;
	for ( list<rvector>::iterator itor = m_WayPointList.begin();  itor != m_WayPointList.end();  ++itor)
	{
		bool bChained = !( (nTotal-1) == cnt);

		ZTask* pNew = ZTaskManager::CreateMoveToPos( m_pBody, (*itor), bChained);
		m_pBody->m_TaskManager.Push( pNew);

		++cnt;
	}
}

void ZBrain::AdjustWayPointWithBound(list<rvector>& wayPointList, RNavigationMesh* pNavMesh)
{
	// ���� �ڳ��� ���𼭸� �տ���, ���� ������ �÷��̾ �����ϸ� waypoint 1���� ��ã��ȴ�, �׷��� �ڳ� ���̹Ƿ�
	// �ٿ�庼�������� ���𼭸��� �ɷ��� ���ڸ� ������ �ϰ� �ȴ�. �̸� �����ϱ� ����, �ڱ� �ٿ�� �¿��� �������κ���
	// ��ǥ���� ��ã�⸦ �غ��� waypoint 1�� �̻����� ������(�� ���� ���θ�������) ���̵� ������ ���� �ؼ� ���ڳʷκ��� ���� ����߸���
	// * waypoint�� ������ ������ ���� ù��° waypoint�� ������ �׽�Ʈ�Ѵ�

	if (wayPointList.empty()) return;

	const rvector& targetpos = *(wayPointList.begin());

	// �� ����~ù��ǥ���� �̿��� ���� �������ϴ� �������κ��� �ٿ���Ǹ����� �¿����� ���� ������
	rvector center = m_pBody->GetPosition();
	rvector dir = targetpos - center;
	dir.z = 0;
	Normalize(dir);

	dir *= m_pBody->GetCollRadius() * 0.8f;	// �ڱ� �浹 ���������� �ణ �۰� *0.8

	rvector side1 = center + rvector(-dir.y,  dir.x, 0);
	rvector side2 = center + rvector( dir.y, -dir.x, 0);

	// �¿��� ���������� ù������ ���� ��ã�� �غ���. 2���̻��� ������ ���� �Ϻΰ� ��տ� ���� �������̹Ƿ�
	// �׳� �ݴ������� �ѹ�¦ ���� ����������
	if ( pNavMesh->BuildNavigationPath( side1, targetpos) && pNavMesh->GetWaypointList().size() > 1)
		m_WayPointList.push_front(side2);
	else if ( pNavMesh->BuildNavigationPath( side2, targetpos) && pNavMesh->GetWaypointList().size() > 1)
		m_WayPointList.push_front(side1);
}

bool ZBrain::EscapeFromStuckIn(list<rvector>& wayPointList)
{
	// ��ã�� �ڵ��� ������ �巯���� �� �������� �ִ�.. �׷� �������� ���� �̵������� ���Ѵ�
	// �׷����� Ż���ϱ� ���� ������ �Ѵ�. true �����ϸ� ��������Ʈ�� ���⼭ �����ߴٴ� �ǹ�.
	DWORD currTime = timeGetTime();
	// �����ð� ���� ���� �����ִٸ� �ƿ� �����ع�����
	if (currTime - m_dwExPositionTimeForWarp > 2000)
	{
		rvector diff = m_exPositionForWarp - m_pBody->GetPosition();

		ResetStuckInStateForWarp();

		if (MagnitudeSq(diff) < 100)
		{
			OutputDebugString("NPC NEED WARP....\n");
			RNavigationMesh* pNavMesh = ZGetGame()->GetWorld()->GetBsp()->GetNavigationMesh();
			if (pNavMesh) {
				// �ٹ��� ���������� ���Ѵ�

				// ���� ���� ���
				float angle = (rand() % (314*2)) * 0.01f;
				D3DXMATRIX matRot;
				D3DXMatrixRotationZ(&matRot, angle);

				rvector dir(200, 0, 0);	// �̵��� �Ÿ�
				dir = dir * matRot;
				rvector newpos = m_pBody->GetPosition() + dir;

				// ���� ����� �׺���̼ǳ���� ���ͷ� �ű�� (�׺���̼ǳ�尡 ũ�� ���� �ִ� �ʿ��� ������ ���ϰ� ���� ��� ����..)
				RNavigationNode* pNavNode = pNavMesh->FindClosestNode(newpos);
				if (pNavNode) {
					m_pBody->SetPosition( pNavNode->CenterVertex());
					OutputDebugString("NPC WARP DONE!\n");
					return false;
				}
			}
		}
	}

	// ª���ð� ���� ���� �����ִٸ� ������ �ѹ�¦ ���� �������� Ż��õ�
	if (currTime - m_dwExPositionTime > 1000)
	{
		rvector diff = m_exPosition - m_pBody->GetPosition();

		ResetStuckInState();

		if (MagnitudeSq(diff) < 100)
		{
			wayPointList.clear();

			// �⺻������ �������� ������ ��� �¿�� �����ϰ� ������ �ش�
			rvector dir = m_pBody->GetDirection();
			rmatrix matRot;
			D3DXMatrixRotationZ(&matRot, (rand()%314 - 157) * 0.01f);	// 3.14 �� �ݹ��� ���� ������ ������ Ʋ�� ��
			Normalize(dir);

			dir *= m_pBody->GetCollRadius() * 0.8f;
			wayPointList.push_back(m_pBody->GetPosition() + dir);

			PushWayPointsToTask();

			return true;
		}
	}

	return false;
}

void ZBrain::ResetStuckInState()
{
	rvector pos;
	if (m_pBody) pos = m_pBody->GetPosition();
	else	pos.x = pos.y = pos.z = 0;

	DWORD currTime = timeGetTime();

	m_dwExPositionTime = currTime;
	m_exPosition = pos;
}

void ZBrain::ResetStuckInStateForWarp()
{
	rvector pos;
	if (m_pBody) pos = m_pBody->GetPosition();
	else	pos.x = pos.y = pos.z = 0;

	DWORD currTime = timeGetTime();

	m_dwExPositionTimeForWarp = currTime;
	m_exPositionForWarp = pos;
}


void ZBrain::OnDamaged()
{
	// ��ȣ�� ������ ��쿡 ���� ���ϸ� ���� �������� �ٲ��.
	if ( m_Behavior.IsFriendly())
	{
		// Stop
		m_pBody->Stop();
		m_pBody->m_TaskManager.Clear();

		// Change friendly type
		m_Behavior.SetFriendly( false);
	}
}


void ZBrain::OnBody_AnimEnter(ZA_ANIM_STATE nAnimState)
{
}


void ZBrain::OnBody_AnimExit(ZA_ANIM_STATE nAnimState)
{
}


void ZBrain::OnBody_CollisionWall()
{
}


void ZBrain::OnBody_OnTaskFinished( ZTASK_ID nLastID)
{
	// ��ã���� ���� �� �������� ���ο� ��ã�⸦ �ٷ� �ϵ��� �����Ѵ�.
	if ( (nLastID == ZTID_MOVE_TO_POS) || (nLastID == ZTID_MOVE_TO_DIR) || (nLastID == ZTID_MOVE_TO_TARGET))
	{
		if ( GetTarget())
			m_PathFindingTimer.Force();
	}

	// �̵� ���� ������ �Ͼ���� �󸶰� ��ġ�� ������� �ʾ����״� ��а��� �ʿ� �������� �˻����� �ʵ��� �Ѵ�.
	if ( nLastID==ZTID_ATTACK_MELEE || nLastID==ZTID_ATTACK_RANGE || nLastID==ZTID_DELAY || nLastID==ZTID_SKILL)
	{
		ResetStuckInState();
		ResetStuckInStateForWarp();
	}
}
