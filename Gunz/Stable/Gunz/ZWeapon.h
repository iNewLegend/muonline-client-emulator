#ifndef _ZWEAPON_H
#define _ZWEAPON_H

//#pragma	once

#include "RTypes.h"
#include "RMesh.h"
#include "RVisualMeshMgr.h"
#include "MRTTI.h"

class RealSoundEffectPlay;

_USING_NAMESPACE_REALSPACE2

class ZObject;

// rtti ������ �ʿ����..�ܼ��� �����Ұ͵�

enum ZWeaponType {
	ZWeaponType_None = 0,

	ZWeaponType_MovingWeapon,

	ZWeaponType_End
};

class ZWeapon {
public:
	MDeclareRootRTTI;

	ZWeapon();
	virtual ~ZWeapon();

	virtual void Create(RMesh* pMesh);
	virtual void Render();
	virtual bool Update(float fElapsedTime);

	int	 GetWeaponType()		{ return m_WeaponType; }
	void SetItemUID(int nUID)	{ m_nItemUID = nUID; }
	int  GetItemUID()			{ return m_nItemUID; }

public:
	RVisualMesh* m_pVMesh;

	int			m_nWorldItemID;

protected:
	int			m_WeaponType;
	MUID		m_uidOwner;
	MMatchTeam	m_nTeamID;
	float		m_fDamage;
	int			m_SLSid; // ���ٽ� ����Ʈ ���̵�
	int			m_nItemUID;
};

class ZMovingWeapon : public ZWeapon {
public:
	MDeclareRTTI;

	ZMovingWeapon();
	~ZMovingWeapon();

	//void Create(RMesh* pMesh,rvector &pos, rvector &dir,ZObject* pOwner);

	virtual void Explosion();

public:
	rvector m_Position;
	rvector m_Velocity;
	rvector m_Dir,m_Up;
	rvector	m_PostPos;
};

class ZEffectRocket;

class ZWeaponRocket : public ZMovingWeapon {
public:
	MDeclareRTTI;

	ZWeaponRocket();
	~ZWeaponRocket();

	void Create(RMesh* pMesh,rvector &pos, rvector &dir,ZObject* pOwner);
	void Render();
	void Explosion();
	
	bool Update(float fElapsedTime);

	float	m_fStartTime;
	float	m_fLastAddTime;
};

class ZWeaponItemKit : public ZMovingWeapon { 
public:
	MDeclareRTTI;

	ZWeaponItemKit();
	~ZWeaponItemKit();

	void Create(RMesh* pMesh,rvector &pos, rvector &velocity,ZObject* pOwner);

	virtual void Explosion();
	
	virtual bool Update(float fElapsedTime);
	virtual void Render();

private:
	void UpdateFirstPos();
	void UpdatePost(DWORD dwPickPassFlag, float fDropDelayTime);
	void UpdatePos(float fElapsedTime,DWORD dwPickPassFlag);
	void UpdateWorldItemFloorDrop(DWORD dwPickPassFlag);

public:
	bool	m_bInit;
	bool	m_bSendMsg;
	bool	m_bWorldItemFloorDrop;			// ȭ�鿡 ���̴� ������ �������� �ٴڿ� ������....������ ���� �����ʹ� �������
	int		m_nWorldItemID;
	float	m_fStartTime;
	float	m_fDelayTime;

	float	m_fDeathTime;
	bool	m_bDeath;
	rvector m_RotAxis;
//	int		m_nSpItemID;
	
//	static int m_nMySpItemID;
};

class ZWeaponGrenade : public ZMovingWeapon { 
public:
	MDeclareRTTI;

	ZWeaponGrenade() : ZMovingWeapon() { }

	void Create(RMesh* pMesh,rvector &pos, rvector &velocity,ZObject* pOwner);

	virtual void Explosion();

	virtual bool Update(float fElapsedTime);

public:

	int		m_nSoundCount;
	float	m_fStartTime;
	
	rvector m_RotAxis;
};

class ZWeaponFlashBang : public ZWeaponGrenade 
{
public:
	MDeclareRTTI;

	ZWeaponFlashBang::ZWeaponFlashBang() : ZWeaponGrenade(), mRotVelocity(0.0f), mbLand(false), mbIsExplosion(false)
	{
		D3DXMatrixIdentity( &mRotMatrix );
	}


	bool	mbIsExplosion;
	bool	mbIsLineOfSight;
	bool	mbLand;
	float	mRotVelocity;
	rmatrix	mRotMatrix;	
	
public:
	bool	Update( float fElapsedTime );
	void	Explosion();
};

#define NUM_SMOKE	7
class ZWeaponSmokeGrenade : public ZWeaponGrenade
{
protected:
	MDeclareRTTI;

	ZWeaponSmokeGrenade() : ZWeaponGrenade(), miSmokeIndex(0), mRotVelocity(0.0f), mbLand(false) 
	{
		D3DXMatrixIdentity( &mRotMatrix );
	};

	static const float mcfTrigerTimeList[NUM_SMOKE];
	int		miSmokeIndex;
	bool	mbLand;
	float	mRotVelocity;
	rmatrix	mRotMatrix;					
public:
	bool	Update( float fElapsedTime );
	void	Explosion();
};

class ZSkill;
class ZSkillDesc;

enum WeaponMagicExplosionType
{
	WMET_MAP = 0,
	WMET_OBJECT,
	WMET_END
};

class ZWeaponMagic : public ZMovingWeapon {
	MDeclareRTTI
private:
	MUID		m_uidTarget;
	bool		m_bGuide;
	ZSkillDesc	*m_pSkillDesc;
	float		m_fMagicScale;
	bool		m_bThroughNPC;
	bool		m_bSurfaceMount;
public:
	ZWeaponMagic() : ZMovingWeapon() { m_fMagicScale = 1.f; }

	void Create(RMesh* pMesh, ZSkill* pSkill, const rvector &pos, const rvector &dir,float fMagicScale, ZObject* pOwner);

	void Render();
	void ExplosionThrow(WeaponMagicExplosionType type, ZObject* pVictim,rvector& vDir);
	void Explosion(WeaponMagicExplosionType type, ZObject* pVictim,rvector& vDir);

	bool Update(float fElapsedTime);

	const MUID& GetTarget() { return m_uidTarget; }
	ZSkillDesc *GetDesc() { return m_pSkillDesc; }

	float	m_fStartTime;
	float	m_fLastAddTime;
};

class ZWeaponDynamite : public ZMovingWeapon { 
public:
	MDeclareRTTI;

	ZWeaponDynamite() : ZMovingWeapon() { }

	void Create(RMesh* pMesh, rvector &pos, rvector &velocity, ZObject* pOwner);
	virtual void Explosion();
	virtual bool Update(float fElapsedTime);

public:
	int		m_nSoundCount;
	float	m_fStartTime;

	rvector m_RotAxis;

	bool	m_bCollided;
};

//// �� Ʈ���� ����ź�� ���� �������� ���ư� �� ���� �ƴ� '����'�� ����� �� �ߵ��ȴ�
class ZWeaponTrap : public ZMovingWeapon {
public:
	MDeclareRTTI;
private:
	rvector m_RotAxis;
	int		m_nItemId;
	float	m_fLandedTime;
	float	m_fThrowedTime;
	float	m_fActivatedTime;
	float	m_fNextEffectTime;
	MMatchDamageType m_nDamageType;
	bool	m_bLanded;
	bool	m_bActivated;
	bool	m_bFriendly;

	bool Init(RMesh* pMesh, rvector& pos, rvector& velocity, int nItemId, ZObject* pOwner);

public:

	ZWeaponTrap() : ZMovingWeapon() {
		m_nItemId = 0;
		m_fThrowedTime = 0;
		m_fActivatedTime = 0;
		m_fNextEffectTime = 0;
		m_nDamageType=MMDT_NORMAL;
		m_bActivated = false;
		m_bFriendly = true;
	}

	// �� �ڽ� Ȥ�� peer�� ��ô�Ҷ� �����ϱ�
	bool Create(RMesh* pMesh, rvector& pos, rvector& velocity, int nItemId, ZObject* pOwner);
	// �̹� �ߵ� ���̰ų� ���ư��� �ִ� ���� Ʈ���� �����ϱ�(���������� Ʈ�� ����� �޾Ƽ� ����)
	bool CreateActivated(RMesh* pMesh, rvector& pos, float fActivatedTime, int nItemId, ZObject* pOwner);
	virtual bool Update( float fElapsedTime );

	void Activate();	// Ʈ���۵�
	bool UpdateOnActivated(float fElapsedTime);	// Ʈ���� �۵� ���϶��� ������Ʈ ��ƾ

	void AddEffectActivating(int nElapsedIfActivated, bool bFriendly);
};

#endif