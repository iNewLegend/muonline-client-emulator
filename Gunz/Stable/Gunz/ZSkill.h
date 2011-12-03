#pragma once

#include <map>
#include <vector>
#include "ZTimer.h"
using namespace std;

class ZObject;

enum ZSKILLRESISTTYPE {
	ZSR_NONE		= 0,
	ZSR_FIRE		= 1,
	ZSR_COLD		= 2,
	ZSR_LIGHTNING	= 3,
	ZSR_POISON		= 4
};

enum ZSKILLEFFECTTYPE {		// � ��󿡰� ���Ǵ��� ����
	ZSE_ENEMY		= 0,		// �Ϲ����� ��
	ZSE_ENEMY_AREA	= 1,		// �� ����
	ZSE_OWNER_AREA	= 2,		// ĳ���� �ֺ� ����
	ZSE_WHOLE_AREA	= 3,		// ��ü
	ZSE_SLASH_AREA	= 4,		// Į �ֵθ��� ����

	ZSE_ALLIED		= 5,		// �Ʊ�
	ZSE_ALLIED_AREA	= 6,		// �Ʊ� ����
};

enum ZSKILLEFFECTTRAILTYPE {
	ZSTE_NONE	= 0,

	ZSTE_FIRE	= 1,
	ZSTE_COLD	= 2,
	ZSTE_MAGIC	= 3,

	ZSTE_END,
};

enum ZSKILLEFFECTTARGETPOSTYPE {
	ZSTP_NONE		= 0,

	ZSTP_SOURCE		= 1,
	ZSTP_TARGET		= 2,
	ZSTP_TARGETHEAD = 3,

	ZSTP_EFFECT		= 4,
//	ZSTP_LHEALING	= 4,
//	ZSTP_HEALING	= 5,
	ZSTP_SPINE1		= 5,
	
	ZSTP_HEAD		= 6,
	ZSTP_LHAND		= 7,			
	ZSTP_RHAND		= 8,
	ZSTP_LFOOT		= 9,
	ZSTP_RFOOT		= 10,

	ZSTP_END,
};

struct ZSkillRepeat
{
	float	fDelay;
	rvector	vAngle;
};

// ��ȹ���� �ִ°��̿ܿ� ȿ�����۽ð�, ����� �ִ� ��ų�ΰ�� ĳ���ð����� �Ÿ��� �ʿ��ҵ�.
// �� ����Ʈ�� ���� ����� �ʿ��ҵ�.
class ZSkillDesc {
public:
	ZSkillDesc();
	~ZSkillDesc();

public:

	int					nID;
	char				szName[128];
	ZSKILLEFFECTTYPE	nEffectType;
	bool				bHitCheck;
	bool				bGuidable;
	bool				bThroughNPC;
	bool				bSurfaceMount;
	float				fVelocity;			// �߻�Ǵ� ������ �ӵ�
	int					nDelay;
	int					nLifeTime;
	int					nEffectStartTime;	// �߰� : ȿ�� ���۽ð�
	int					nEffectTime;		// ȿ�� ���ӽð�
	float				fEffectArea;
	float				fEffectAreaMin;		// ȿ�� �ּҰŸ� - ������ Melee�� Skill���� ����Ѵ�.
	float				fEffectAngle;		// ȿ�� ���� ���� - ������ Melee�� Skill���� ����Ѵ�. �⺻�� 90��
	ZSKILLRESISTTYPE	ResistType;
	int					nDifficulty;
	bool				bCameraShock;
	float				fCameraPower;
	float				fCameraDuration;
	float				fCameraRange;

	int					nModDamage;
	int					nModLastDamage;
	int					nModDoT;
	int					nModCriticalRate;	// �����(%) ��ü �������� HP�������� ����
	int					nModSpeed;
	bool				bModAntiMotion;
	bool				bModRoot;
	int					nModHeal;
	int					nModRepair;
	float				fModKnockback;
	char				szMessage[256];

	int					nCastingAnimation;	// �߰� : ĳ���� �ִϸ��̼�
	int					nTargetAnimation;

	char				szEffectSound[64];		// ȿ�� ���۵ɶ� ������ ����
	char				szExplosionSound[64];	// �����Ҷ� ������ ����

	ZSKILLEFFECTTARGETPOSTYPE	nCastingEffectType;
	ZSKILLEFFECTTARGETPOSTYPE	nCastingPreEffectType;
	ZSKILLEFFECTTARGETPOSTYPE	nEffectStartPosType;

	rvector				vCastingEffectAddPos;
	char				szCastingEffect[64];
	char				szCastingEffectSp[64];//Ư���� ����Ʈ��..�̸����� Ư���� �Լ��� �����ϰ� �������...
	char				szCastingPreEffect[64];// ��ų �ߵ��� ���ÿ� ��������...
	int					nCastingEffectSpCount;
	
	char				szTrailEffect[64];
	char				szTargetEffect[64];

	ZSKILLEFFECTTRAILTYPE nTrailEffectType;
	float				fTrailEffectScale;
	bool				bDrawTrack;				// �̻��Ϸ� Track �׸��� ����
	bool				bOffDLight;				// ���� ����Ʈ�� ������ ����
	float				fColRadius;				// �̻��Ϸ� �浹 ���� ������
	vector<ZSkillRepeat>*		pRepeatList;		// �ѽ�ų�� ������ �߻��Ϸ��� �� �� ���

	bool IsAlliedTarget() {		// �Ʊ��� ����ΰ�
		if(nEffectType==ZSE_ALLIED || nEffectType==ZSE_ALLIED_AREA) return true;
		return false;
	}

	bool IsAreaTarget() {		// ���������ΰ� ?
		if(nEffectType==ZSE_ENEMY || nEffectType==ZSE_ALLIED) return false;
		return true;
	}

	bool IsEffectiveTo(ZObject *pTarget);	// ��ų�� �������� ��󿡰� ȿ���� �ִ���..

	bool CheckResist(ZObject *pCurrent,float *pfDamage);	// ��������

};

class ZSkillManager : public map < int , ZSkillDesc* > {
public:
	virtual ~ZSkillManager();
	bool Create();
	void Destroy();
};

class ZSkill {
private:
	ZSkillDesc *m_pDesc;
	float		m_fLastBeginTime;
	float		m_fNextDamageTime;	// ������ �������� ���� �ð�
	bool		m_bEnable;
	ZObject		*m_pOwner;

	rvector			m_TargetPos;
	MUID			m_uidTarget;
	int				m_nUseNum;
	ZUpdateTimer	m_RepeatUTimer;

	int				m_nDelay;

public:
	ZSkill();
	virtual ~ZSkill();

	bool Init(int nID, ZObject *pOwner);

	bool IsReady();	// ��Ÿ���� ���� ��밡������..
	bool IsEnable();

	void InitStatus();
	bool Update(float fElapsed);

	ZSkillDesc *GetDesc()	{ return m_pDesc; }
	MUID		GetTarget()	{ return m_uidTarget; }
	const rvector&	GetTarGetPosition() { return m_TargetPos; }

	void Execute(MUID& uidTarget, rvector& targetPos );
	void PreExecute(MUID& uidTarget, rvector& targetPos );
	void LastExecute(MUID& uidTarget, rvector& targetPos );
	void Cancel(MUID& uidTarget);

	bool IsUsable(ZObject *pTarget);	// (��������ؼ�)�� ��ų�� ��󿡰� ��������

protected:

	bool GetPartsTypePos(ZObject* pTargetObject, ZSKILLEFFECTTARGETPOSTYPE nSkillEffectPosType, 
						 RMeshPartsPosInfoType& type,rvector& vPos,rvector& vDir);
	bool GetPartsTypePos(RMeshPartsPosInfoType& type,MUID& uid,MUID& uidTarget);
	bool CheckRange(const rvector& center, ZObject *pCurrent);	// ����� ������ �޴��� üũ
	void Use(MUID& uidTarget, rvector& targetPos);
	void Repeat();
	void GetMissilePosDir(rvector& outDir, rvector& outPos, rvector& TargetPos);
};