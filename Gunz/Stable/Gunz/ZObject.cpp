#include "stdafx.h"
#include "ZObject.h"
#include "ZMyCharacter.h"
#include "ZNetCharacter.h"
#include "RCollisionDetection.h"
#include "ZGame.h"
#include "CheckReturnCallStack.h"
#include "ZAniEventHandler.h"

MImplementRTTI(ZObject, ZModuleContainer);

ZObject::ZObject() : m_Direction(1,0,0), m_bInitialized(false), m_UID(MUID(0,0)),
					 m_fSpawnTime(0.0f), m_fDeadTime(0.0f), m_pVMesh(NULL), m_bVisible(false),
					 m_bIsNPC(false)
//					 ,m_Velocity(m_Module_Movable.m_Velocity)
{ 
	//mmemory proxy
	m_Position.Set_MakeCrc(rvector(0,0,0));

	m_pModule_Movable = new ZModule_Movable;
	AddModule(m_pModule_Movable,true);
}

ZObject::~ZObject()
{
	RemoveModule(m_pModule_Movable);
	delete m_pModule_Movable;
}


//void ZObject::SetPosition(const rvector& pos)
//{
//	if(m_MPosition == NULL)
//		m_Position = pos;
//	else
//	{
//		rvector vPos = pos;
//		m_MPosition->SetData(vPos);
//	}
//}

void ZObject::OnDraw()
{
}
void ZObject::OnUpdate(float fDelta)
{
	
}


void ZObject::Draw()
{
	OnDraw();
}


void ZObject::Update(float fDelta)
{
	OnUpdate(fDelta);
}


bool ZObject::Pick(int x,int y,RPickInfo* pInfo)
{
	if(m_pVMesh) 
	{
		return m_pVMesh->Pick(x,y,pInfo);
	}

	return false;
}


bool ZObject::Pick(int x,int y,rvector* v,float* f)
{
	RPickInfo info;
	bool hr = Pick(x,y,&info);
	*v = info.vOut;
	*f = info.t;
	return hr;
}


bool ZObject::Pick(rvector& pos,rvector& dir, RPickInfo* pInfo)
{
	if(m_pVMesh) 
	{
		return m_pVMesh->Pick(pos, dir, pInfo);
	}

	return false;
}

bool ZObject::GetHistory(rvector *pos,rvector *direction,float fTime)
{
	if(m_pVMesh == NULL) return false;
	
	*pos=GetPosition();
	*direction=m_Direction;
	return true;
}

void ZObject::SetDirection(rvector& dir)
{
	m_Direction = dir;
}


void ZObject::SetSpawnTime(float fTime)
{
	m_fSpawnTime=fTime;
}

void ZObject::SetDeadTime(float fTime)
{
	m_fDeadTime = fTime;
}

//////////////////////////////////////////////////////////////////////
bool IsPlayerObject(ZObject* pObject)
{
	return ((MIsExactlyClass(ZNetCharacter, pObject)) || ((MIsExactlyClass(ZMyCharacter, pObject))));
}

ZOBJECTHITTEST ZObject::HitTest( const rvector& origin, const rvector& to,float fTime,rvector *pOutPos)
{
	return ZOH_NONE;
}

void ZObject::Tremble(float fValue, DWORD nMaxTime, DWORD nReturnMaxTime)
{
	if(m_pVMesh)
	{
		RFrameTime* ft = &m_pVMesh->m_FrameTime;
		if(ft && !ft->m_bActive)
			ft->Start(fValue,nMaxTime,nReturnMaxTime);// ���� , �ִ�ð� , ���ͽð�...
	}

}

void ZObject::OnDamaged(ZObject* pAttacker, rvector srcPos, ZDAMAGETYPE damageType, MMatchWeaponType weaponType, float fDamage, float fPiercingRatio, int nMeleeType)
{
	ZModule_HPAP *pModule = (ZModule_HPAP*)GetModule(ZMID_HPAP);

	//jintriple3 ����� �������� ��ŷ ��� �ڵ�
	bool bDebugRegisterValue = !pModule;
	if(!pModule) {
		PROTECT_DEBUG_REGISTER(bDebugRegisterValue) {
			return;
		}
	}

	pModule->OnDamage(pAttacker ? pAttacker->GetUID() : MUID(0,0), fDamage, fPiercingRatio);
}

void ZObject::OnDamagedSkill(ZObject* pAttacker, rvector srcPos, ZDAMAGETYPE damageType, MMatchWeaponType weaponType, float fDamage, float fPiercingRatio, int nMeleeType)
{
	ZModule_HPAP *pModule = (ZModule_HPAP*)GetModule(ZMID_HPAP);
	if(!pModule) return;

	pModule->OnDamage(pAttacker ? pAttacker->GetUID() : MUID(0,0), fDamage, fPiercingRatio);
}

void ZObject::OnSimpleDamaged(ZObject* pAttacker, float fDamage, float fPiercingRatio)
{
	ZModule_HPAP *pModule = (ZModule_HPAP*)GetModule(ZMID_HPAP);
	if(!pModule) return;

	pModule->OnDamage(pAttacker ? pAttacker->GetUID() : MUID(0,0), fDamage, fPiercingRatio);
}

//// HP/AP�� ȸ���Ѵ�
//void ZObject::OnHealing(ZObject* pOwner,int nHP,int nAP)
//{
//	// ����
//	ZModule_HPAP *pModule = (ZModule_HPAP*)GetModule(ZMID_HPAP);
//	if(!pModule) 
//	{
//		CHECK_RETURN_CALLSTACK(OnHealing);
//		return;
//	}
//
//	pModule->SetHP( min( pModule->GetHP() + nHP, pModule->GetMaxHP() ) );
//	pModule->SetAP( min( pModule->GetAP() + nAP, pModule->GetMaxAP() ) );
//
//
//	// TODO: ����Ʈ �߰�. �ӽ÷� �޾���
//	ZGetEffectManager()->AddHealEffect(GetPosition(),this);
//
//    CHECK_RETURN_CALLSTACK(OnHealing);
//}

bool ZObject::ColTest(const rvector& p1, rvector& p2, float radius, float fTime)
{
	rvector p, d;
	if (GetHistory(&p, &d, fTime))
	{
		float fCollHeight = m_Collision.GetHeight();
		float fCollRadius = m_Collision.GetRadius();

		rvector a1 = p + rvector(0,0, (min(fCollHeight, fCollRadius)/2.0f));
		rvector a2 = p + rvector(0,0, fCollHeight - (min(fCollHeight, fCollRadius)/2.0f));

		rvector ap,cp;
		float dist = GetDistanceBetweenLineSegment(p1, p2 , a1, a2, &ap, &cp);

		if (dist < (radius + fCollRadius)) return true;
	}

	return false;
}


//rvector & ZObject::GetPosition() 
//{ 
//	if( m_MPosition == NULL)
//	{
//		return m_Position;
//	}
//	else
//	{
//		return m_MPosition->GetData();
//	}
//}

void ZObject::SetVisualMesh(RVisualMesh* pVMesh)
{ 
	m_pVMesh = pVMesh; 

	// �ִϸ��̼� �ݹ� ���
	for(int i=0;i<ani_mode_end;i++)
	{
		m_pVMesh->GetFrameInfo((RAniMode)i)->SetAnimEventHandler(ZAniEventHandler::ZAniEventHandlerCB);
	}
}

void ZObject::AddHealEffect()
{
	ZGetEffectManager()->AddHealEffect(GetPosition(),this);
}

void ZObject::ShiftFugitiveValues()
{
	// m_pVMesh�� ���� AniFramInfo�� �ִϸ��̼� �Ϸ� bool�̳� ���� �����ӹ�ȣ ���� ���� �ִ�. �̰��� �������� �ִ� ������¿� �����ϴ�
	// �����̸� ��ȿȭ�ϴ� ��ŷ�� �õ��� (Į ������Ⱑ ���� �ǰų�, ���� �߻�,���ε� �ִϸ��̼��� ���� ���������ų�)
	// - �������� ���Ҹ� �����ϱ� �������� ���⼭ ���� ���Ҵ��Ͽ� ����ġ�� �Ű��ش�
	if (m_pVMesh) 
	{
		// AniFramInfo
		{
			AniFrameInfo* old = m_pVMesh->m_FrameInfo;
			AniFrameInfo* New = new AniFrameInfo[ani_mode_end];

			for (int i=0; i<ani_mode_end; ++i)
				New[i] = old[i];
			delete [] old;

			m_pVMesh->m_FrameInfo = New;
		}

		// m_SelectWeaponMotionType �� �ܰ����� �����ϸ� �ӽŰ��� ��� ������ �ƴ� �뽬�� �ϸ� �ӽŰ� �������� Į���� �����ϴ�
		{
			if (m_pVMesh->m_SelectWeaponMotionType !=
				*m_pVMesh->m_pSelectWeaponMotionType_AntiHack)
				g_fpOnCrcFail();

			RWeaponMotionType* old = m_pVMesh->m_pSelectWeaponMotionType_AntiHack;
			RWeaponMotionType* New = new RWeaponMotionType;

			*New = *old;
			delete old;

			m_pVMesh->m_pSelectWeaponMotionType_AntiHack = New;
		}
	}

	if (m_pModule_Movable)
		m_pModule_Movable->ShiftFugitiveValues();

	m_Items.ShiftFugitiveValues();

	m_Position.ShiftHeapPos_CheckCrc();

	// ���ձ� �� ���
	m_Collision.ShiftFugitiveValues();
}
