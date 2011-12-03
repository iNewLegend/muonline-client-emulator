#ifndef _ZOBJECT_H
#define _ZOBJECT_H

#include "ZPrerequisites.h"
#include "MUID.h"
#include "RTypes.h"
#include "MRTTI.h"
#include "ZModule.h"
#include "ZModule_Movable.h"
#include "ZModule_HPAP.h"
#include "ZCharacterItem.h"

#include "MMemoryProxy.h"

#include <list>
#include <string>

using namespace std;

_USING_NAMESPACE_REALSPACE2

enum ZOBJECTHITTEST {
	ZOH_NONE = 0,
	ZOH_BODY = 1,
	ZOH_HEAD = 2,
	ZOH_LEGS = 3
};

enum ZDAMAGETYPE {
	ZD_NONE=-1,
	ZD_BULLET,
	ZD_MELEE,
	ZD_FALLING,
	ZD_EXPLOSION,
	ZD_BULLET_HEADSHOT,
	ZD_KATANA_SPLASH,
	ZD_HEAL,
	ZD_REPAIR,
	ZD_MAGIC,
	ZD_FIRE,
	ZD_COLD,
	ZD_LIGHTNING,
	ZD_POISON,

	ZD_END
};

enum ZC_ENCHANT {
	ZC_ENCHANT_NONE = 0,
	ZC_ENCHANT_FIRE,
	ZC_ENCHANT_COLD,
	ZC_ENCHANT_LIGHTNING,
	ZC_ENCHANT_POISON,

	ZC_ENCHANT_END
};

/// ������Ʈ �浹�� �ʿ��� ��
struct ZObjectCollision
{
private:
	MProtectValue<bool>		bCollideable;		///< �浹����
	MProtectValue<float>	fRadius;			///< ������
	MProtectValue<float>	fHeight;			///< ����
public:
	ZObjectCollision() {
		bCollideable.Set_MakeCrc(true);
		fRadius.Set_MakeCrc(0);
		fHeight.Set_MakeCrc(0);
	}

	float GetRadius()		{ return fRadius.Ref(); }
	float GetHeight()		{ return fHeight.Ref(); }
	
	void SetRadius(float r)		{ fRadius.Set_CheckCrc(r); }
	void SetHeight(float h)		{ fHeight.Set_CheckCrc(h); }

	bool IsCollideable()		{ return bCollideable.Ref(); }
	void SetCollideable(bool b)	{ bCollideable.Set_CheckCrc(b); }

	void ShiftFugitiveValues() {
		bCollideable.ShiftHeapPos_CheckCrc();
		fRadius.ShiftHeapPos_CheckCrc();
		fHeight.ShiftHeapPos_CheckCrc();
	}
};

/// ���ӻ��� ��� ������Ʈ �θ� Ŭ����
class ZObject : public ZModuleContainer
{
	MDeclareRTTI;

public:
	// �� ������ �����ϱ� ���� ��������� ��ġ�� ���嶧���� �ڼ��� ���� �ּ���ũ��(runtime/ShuffleCode.bat ����)
	// m_Position�� m_Direction�� ���� Ŀ�ǵ带 �����, ���� ZPostShot ���� ���� ���� �� �̿�ȴ�
	/* [[SHUFFLE_LINE]] ZObject */	ZModule_Movable			*m_pModule_Movable;	// �ӵ��� �������� �����ϴ� ���

	/* [[SHUFFLE_LINE]] ZObject */	bool					m_bRendered;		///< ȭ�鿡 �׷������� ����

	/* [[SHUFFLE_LINE]] ZObject */	ZCharacterItem			m_Items;			///< ����ϰ� �ִ� ������

	/* [[SHUFFLE_LINE]] ZObject */	MProtectValue<rvector>	m_Position;			//mmemory proxy

	/* [[SHUFFLE_LINE]] ZObject */	RVisualMesh*			m_pVMesh;

	/* [[SHUFFLE_LINE]] ZObject */	rvector					m_Direction;

private:
	bool					m_bVisible;			///< ȭ�鿡 �׸��� ����
	float					m_fSpawnTime;		///< �¾ �ð�
	float					m_fDeadTime;		///< �����ð�
protected:
	MUID					m_UID;				///< �������� �ο��� ����ũ ID(�߿�)
	bool					m_bIsNPC;			///< NPC���� ����
	bool					m_bInitialized;		///< �ʱ�ȭ�Ǿ����� ����
	bool					m_bInitialized_DebugRegister;	//jintriple3 ����� �������� ����..��

	ZObjectCollision		m_Collision;

	virtual void OnDraw();
	virtual void OnUpdate(float fDelta);
	virtual void OnDie() {} // ������

public:
	ZObject();
	virtual ~ZObject();

	__forceinline const rvector& GetPosition() const
	{ 
		return m_Position.Ref();
	}
	
	//�浹�� �����ִ� �͵�..�޸� ���Ͻ�....
	float GetCollRadius()
	{
		return m_Collision.GetRadius(); 
	}
	float GetCollHeight()				
	{
		return m_Collision.GetHeight(); 
	}
	rvector GetCenterPos()				
	{ 
		return GetPosition() + rvector(0.0f, 0.0f, m_Collision.GetHeight()*0.5f);
	}
	virtual bool IsCollideable() 
	{ 
		return m_Collision.IsCollideable();
	}		

	__forceinline void SetPosition(const rvector& pos)
	{
		m_Position.Set_CheckCrc(pos);
	}

	void Draw();
	void Update(float fDelta);
	virtual bool Pick(int x,int y,RPickInfo* pInfo);
	virtual bool Pick(int x,int y,rvector* v,float* f);
	virtual bool Pick(rvector& pos,rvector& dir, RPickInfo* pInfo = NULL);
	virtual bool GetHistory(rvector *pos,rvector *direction,float fTime); // ����/�������� ������ �����͸� ��´�

	// interface
	void SetVisualMesh(RVisualMesh* pVMesh);
	RVisualMesh* GetVisualMesh()					{ return m_pVMesh; }
	bool IsVisible()								{ return m_bVisible; }
	void SetVisible(bool bVisible)					{ m_bVisible = bVisible; }
	bool GetInitialized()							{ return m_bInitialized; }
	MUID& GetUID()									{ return m_UID; }
	void SetUID(MUID& uid)							{ m_UID = uid; }
	void SetSpawnTime(float fTime);
	float GetSpawnTime()							{ return m_fSpawnTime; }
	void SetDeadTime(float fTime);
	float GetDeadTime()								{ return m_fDeadTime; }
	bool IsNPC()									{ return m_bIsNPC; }
	const rvector& GetDirection()					{ return m_Direction; }

	// ������ ���� ����Ȱ͵�
	const rvector& GetVelocity()		{ return m_pModule_Movable->GetVelocity(); }
	void SetVelocity(rvector& vel)		{ m_pModule_Movable->SetVelocity(vel); }
	void SetVelocity(float x,float y,float z) { SetVelocity(rvector(x,y,z)); }
	void AddVelocity(rvector& add)		{ SetVelocity(GetVelocity()+add); }

	float GetDistToFloor()				{ return m_pModule_Movable->GetDistToFloor(); }
	
	ZCharacterItem* GetItems()		{ return &m_Items; }

public:
	// �����ִ� �͵��� ��ӹ޴� ������Ʈ���� �ڽſ� �°� �������̵��ؼ� ����Ѵ�.
	virtual float ColTest(const rvector& pos, const rvector& vec, float radius, rplane* out=0) { return 1.0f; }
	virtual bool ColTest(const rvector& p1, rvector& p2, float radius, float fTime);		// ������� �̿��Ͽ� �����ϰ� ���� �浹�׽�Ʈ
	virtual bool IsAttackable()	 { return true; }
	virtual bool IsDie() { return false; }
	virtual void SetDirection(rvector& dir);
	virtual bool IsGuard()	{ return false; }
	virtual MMatchTeam GetTeamID() { return MMT_ALL; }
	
	// Ư�������� hit test �� ��������� �Ѵ�, pOutPos �� ������ hit�� ��ġ�� ��������� �Ѵ�
	virtual ZOBJECTHITTEST HitTest( const rvector& origin, const rvector& to, float fTime, rvector *pOutPos=NULL );

	// �׼ǿ� ���� �̺�Ʈ

	// ��� ������ ����
	virtual void OnBlast(rvector &dir)	{ }	
	virtual void OnBlastDagger(rvector &dir,rvector& pos) { }

	// �����̳� �̺�Ʈ�� ���� �͵�.
	virtual void OnGuardSuccess() { }
	virtual void OnMeleeGuardSuccess() { }
	
	// knockback�� ����޾ƾ��Ѵ�
	virtual void OnKnockback(rvector& dir, float fForce) { }

	// ������ ���� ���� �Ѵ�.
	// fValue : ����, MaxTime : �ִ�ð� , nReturnMaxTime : ���ͽð�
	void Tremble(float fValue, DWORD nMaxTime, DWORD nReturnMaxTime);

	// ������ �Ǹ� ���ش�
//	virtual void OnDamage(int damage, float fRatio = 1.0f);

	// �������� �Ծ���.
	virtual void OnDamaged(ZObject* pAttacker, rvector srcPos, ZDAMAGETYPE damageType, MMatchWeaponType weaponType, float fDamage, float fPiercingRatio=1.f, int nMeleeType=-1);

	// ��ǵ��� ������� �ʰ� �Ǹ� ���̱�����..
	virtual void OnDamagedSkill(ZObject* pAttacker, rvector srcPos, ZDAMAGETYPE damageType, MMatchWeaponType weaponType, float fDamage, float fPiercingRatio=1.f, int nMeleeType=-1);

	// �����⸦ �¾Ҵ�
	virtual void OnDamagedAnimation(ZObject *pAttacker,int type) { }

	// ����� ������
	virtual void OnScream() { }

	// HP/AP�� ȸ���Ѵ�
	__forceinline void OnHealing(ZObject* pOwner,int nHP,int nAP);

	void OnSimpleDamaged(ZObject* pAttacker, float fDamage, float fPiercingRatio);

	// stun �ȴ�.
	virtual void OnStun(float fTime)	{ }

public:
	// ������ ���� ������ virtual �Լ� - ���� ��������
	virtual bool IsRendered() { return m_bRendered; }		/// ���� �����ӿ��� ȭ�鿡 �׷ȴ��� ����

	// �޸����� ����� �Ǵ� �������� ����ġ�� �ű��
	void ShiftFugitiveValues();

protected:
	bool Move(rvector &diff)	{ return m_pModule_Movable->Move(diff); }

private:
	// �����ζ����� ���� ����� �ű� �Լ� �� �ܺ� Ÿ���� �ʿ���ϴ� �ڵ常 ���� ����
	void AddHealEffect();
};


/// �÷��̾� ������Ʈ���� ����
bool IsPlayerObject(ZObject* pObject);


// HP/AP�� ȸ���Ѵ�
__forceinline void ZObject::OnHealing(ZObject* pOwner,int nHP, int nAP)
{
	if( nHP <= 0 && nAP <= 0 ) return;

	ZModule_HPAP *pModule = (ZModule_HPAP*)GetModule(ZMID_HPAP);
	if(!pModule) return;

	pModule->SetHP( min( pModule->GetHP() + nHP, pModule->GetMaxHP() ) );
	pModule->SetAP( min( pModule->GetAP() + nAP, pModule->GetMaxAP() ) );

	// TODO: ����Ʈ �߰�. �ӽ÷� �޾���
	AddHealEffect();
}

#endif