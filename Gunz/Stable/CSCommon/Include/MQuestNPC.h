#ifndef _MQUEST_NPC_H
#define _MQUEST_NPC_H

#include "MQuestConst.h"
#include "MTypes.h"

/// NPC ����
enum MQUEST_NPC
{
	// ������� �ٲٸ� ���� �ȵȴ�. xml�� �Բ� �ٲ����.

	NPC_NONE					= 0,	///< N/A

	NPC_GOBLIN					= 11,	///< ���� ���
	NPC_GOBLIN_GUNNER			= 12,	///< ��� �ų�
	NPC_GOBLIN_WIZARD			= 13,	///< ��� ������
	NPC_GOBLIN_COMMANDER		= 14,	///< ���� ���
	NPC_GOBLIN_CHIEF			= 15,	///< ��� ����
	NPC_GOBLIN_KING				= 16,	///< ��� ŷ

	NPC_KOBOLD					= 21,	///< �ں�Ʈ
	NPC_KOBOLD_SHAMAN			= 22,	///< �ں�Ʈ ����
	NPC_KOBOLD_COMMANDER		= 23,	///< ���� �ں�Ʈ
	NPC_KOBOLD_KING				= 24,	///< �ں�Ʈ ��
	NPC_KOBOLD_KING_BOSS		= 25,	///< �ں�Ʈ ��(����)
	NPC_BROKEN_GOLEM			= 26,	///< ���峭 ��

	NPC_SKELETON				= 31,	///< ���̷���
	NPC_SKELETON_MAGE			= 32,	///< ���̷��� ������
	NPC_SKELETON_COMMANDER		= 33,	///< ���� ���̷���
	NPC_GIANT_SKELETON			= 34,	///< �Ŵ� ���̷���
	NPC_THE_UNHOLY				= 35,	///< ���ֹ��� �ý�
	NPC_LICH_BABY				= 36,	///< ��ġ ���̺�
	NPC_GIANT_SKELETON_BOSS		= 37,	///< �Ŵ� ���̷���(����)
	NPC_THE_UNHOLY_BOSS			= 38,	///< ���ֹ��� �ý�(����)
	NPC_LICH					= 39,	///< ��ġ

	NPC_PALMPOW_BABY			= 41,	///< ������ ���̺�
	NPC_PALMPOA					= 42,	///< ������
	NPC_PALMPOA_COMMANDER		= 43,	///< ���� ������
	NPC_PALMPOW					= 44,	///< ������
	NPC_CURSED_PALMPOW			= 45,	///< ���ֹ��� ������

	NPC_SCRIDER					= 51,	///< ��ũ���̴�


	//�̺�Ʈ���� ���� ����� ��� �Ѵ�.
	NPC_GOBLIN_EVENT			= 111,	///< �̺�Ʈ�� ���� ���.
	NPC_SKELETON_EVENT			= 131,	///< �̺�Ʈ�� ���̷���.

	// �����̹���
	NPCS_GOBLIN					= 151,	///< ���� ���
	NPCS_GOBLIN_GUNNER			= 152,	///< ��� �ų�
	NPCS_GOBLIN_WIZARD			= 153,	///< ��� ������
	NPCS_GOBLIN_COMMANDER		= 154,	///< ���� ���
	NPCS_GOBLIN_CHIEF			= 155,	///< ��� ����
	NPCS_GOBLIN_KING			= 156,	///< ��� ŷ
	NPCS_LITTLE_GOBLIN_KING_JR	= 157, ///< ������ ���ŷ �ִϾ�
	NPCS_THUNDER_GOBLIN_KING_JR	= 158, ///< ��� ���ŷ �ִϾ�
	NPCS_THUNDER_GOBLIN_KING	= 159, ///< ��� ���ŷ

	NPCS_KOBOLD					= 161,	///< �ں�Ʈ
	NPCS_KOBOLD_SHAMAN			= 162,	///< �ں�Ʈ ����
	NPCS_KOBOLD_COMMANDER		= 163,	///< ���� �ں�Ʈ
	NPCS_KOBOLD_KING			= 164,	///< �ں�Ʈ ��
	NPCS_KOBOLD_KING_BOSS		= 165,	///< �ں�Ʈ ��(����)
	NPCS_BROKEN_GOLEM			= 166,	///< ���峭 ��

	NPCS_SKELETON				= 171,	///< ���̷���
	NPCS_SKELETON_MAGE			= 172,	///< ���̷��� ������
	NPCS_SKELETON_COMMANDER		= 173,	///< ���� ���̷���
	NPCS_GIANT_SKELETON			= 174,	///< �Ŵ� ���̷���
	NPCS_THE_UNHOLY				= 175,	///< ���ֹ��� �ý�
	NPCS_LICH_BABY				= 176,	///< ��ġ ��
	NPCS_GIANT_SKELETON_BOSS	= 177,	///< ���丮��
	NPCS_THE_UNHOLY_BOSS		= 178,	///< �Ƴ׶��(����)
	NPCS_LICH					= 179,	///< ��ġ(����)
};

/// NPC ���
enum MQUEST_NPC_GRADE
{
	NPC_GRADE_REGULAR = 0,			///< �Ϲ�
	NPC_GRADE_VETERAN,				///< ���׶�
	NPC_GRADE_ELITE,				///< ����Ʈ
	NPC_GRADE_BOSS,					///< ������
	NPC_GRADE_LEGENDARY,			///< Ư�� ������
	NPC_GRADE_END
};

/// NPC ���� Ÿ�� 1Byte���� Ŀ���ٸ� MQuestNPCInfo.nNPCAttackTypes�� ������ ��� �Ѵ�.
enum MQUEST_NPC_ATTACK
{
	NPC_ATTACK_NONE			= 0,		///< N/A
	NPC_ATTACK_MELEE		= 0x1,		///< ���� ����
	NPC_ATTACK_RANGE		= 0x2,		///< ���� ����
	NPC_ATTACK_MAGIC		= 0x4,		///< ���� ����
};

/// NPC �ൿ Ÿ��
enum MQUEST_NPC_BEHAVIOR
{

};


/// NPC ����
enum MQUEST_NPC_SOUND
{
	NPC_SOUND_ATTACK = 0,		// �����Ҷ�
	NPC_SOUND_WOUND,			// �¾�����
	NPC_SOUND_DEATH,			// �׾�����
	NPC_SOUND_END
};

/// ����Ʈ NPC ����
struct MQuestNPCInfo
{
	// �⺻ ����
	MQUEST_NPC			nID;				///< id
	MQUEST_NPC_GRADE	nGrade;				///< ���
	char				szName[256];		///< �̸�
	char				szDesc[256];		///< ����
	BYTE				nNPCAttackTypes;	///< ����Ÿ��	- MQUEST_NPC_ATTACK���� SET
	float				fSpeed;				///< �ӵ�
	float				fWeight;			///< ������
	int					nMaxHP;				///< �ִ� HP. Ŭ���̾�Ʈ�� ������� �ʴ´�. �������� �����ش�.
	int					nMaxAP;				///< �ִ� AP. Ŭ���̾�Ʈ�� ������� �ʴ´�. �������� �����ش�.
	int					nDC;				///< ���̵� ���
	char				szMeshName[256];	///< �޽��̸�
	int					nWeaponDamage;		///< ���� �ִ� ���� ���ݷ�
	float				fRotateSpeed;		///< ���� ��ȯ �ӵ� (�ʴ� ȸ�� ������, ���� ��� 3.14159�̸� �ʴ� �ݹ��� ȸ����. -���̸� �ٷ� ȸ��)
	float				fCollRadius;		///< �浹 ������
	float				fCollHeight;		///< �浹 ����
	bool				bColPick;			///< ���Ÿ� �ǰݽ� ���������� ��ŷ���η� �ǰ�üũ���� ����(�ַ� ������ true)
	MVector				vScale;				///< ũ��
	MVector				vColor;				///< ��
	unsigned char		nSpawnWeight;		///< ���� ����ġ
	unsigned long int	nWeaponItemID;		///< ����ϰ� �ִ� ���� ID(Zitem�� �ִ°�)
	float				fDyingTime;			///< �״� �ִϸ��̼� �ð�(�⺻�� 5��)
	float				fTremble;			///< �ǰݽ� �� ���� ����(�⺻�� 30)
	int					nOffenseType;		///< ���� Ư��( 1:melee, 2:range)
	bool				bFriendly;			///< ��ȣ������...
	// int					nDBIndex;			///< DB�� NPC�� Matching�Ǵ� �ε���.

	// Sound
	char				szSoundName[NPC_SOUND_END][128];	///< ���� ���� �̸�

	// AI ���� �Ķ��Ÿ
	char				nIntelligence;			///< ����(1 ~ 5�ܰ�) 1�� ���� ����. - ��ã�� ������Ʈ �ӵ��� �����ִ�.
	char				nAgility;				///< ��ø��(1 ~ 5�ܰ�) 1�� ���� ����. - ���� ������Ʈ �ӵ��� �����ִ�.

	float				fAttackRange;			///< �⺻ ���ݹ���
	float				fAttackRangeAngle;		///< �⺻ ���� �þ� ����
	float				fAttackHitRate;			///< �⺻ ���� ���߷�(Ư�� ���Ÿ�)
	float				fAttackCoolTime;		///< �⺻ ���� ��Ÿ��(Ư�� ���Ÿ�)
	float				fCollisionCheckSpeed;	///< �浹üũ �ӵ�
	float				fViewAngle;				///< �þ� ����(����)
	float				fViewDistance;			///< �þ� �Ÿ�
	float				fViewDistance360;		///< ������ �þ߰Ÿ�
	
	// �÷���
	bool				bNeverBlasted;				///< Į�� ����ø��� �� ���� ����
	bool				bMeleeWeaponNeverDamaged;	///< ���� ���ݿ� �´��� ����
	bool				bRangeWeaponNeverDamaged;	///< ���� ���ݿ� �´��� ����
	bool				bShadow;					///< �׸��ڰ� �ִ��� ����
	bool				bNeverPushed;				///< ���� �޾����� �и����ΰ�?
	bool				bNeverAttackCancel;			///< ���� �޾����� �ϴ� ������ ��ҵǴ°�?

	int					nSkills;				///< ���� ��ų ����
	int					nSkillIDs[MAX_SKILL];	///< �������ִ� ��ų

	// ������ ����ϴ� �Ķ��Ÿ
	int					nDropTableID;			///< drop table index - ������ ����Ѵ�.
	char				szDropTableName[8];		///< drop table name

	/////////////////////////////////////////////

	/// �ʱ�ȭ
	void SetDefault()
	{
		nID					= NPC_GOBLIN;
		nGrade				= NPC_GRADE_REGULAR;
		strcpy(szName, "Noname");
		szDesc[0]			= 0;
		nNPCAttackTypes		= NPC_ATTACK_MELEE;
		fSpeed				= 300.0f;
		fWeight				= 1.0f;
		nMaxHP				= 100;
		nMaxAP				= 0;
		nDC					= 5;
		szMeshName[0]		= 0;
		nWeaponDamage		= 5;
		fRotateSpeed		= 6.28318f;
		fCollRadius			= 35.0f;
		fCollHeight			= 180.0f;
		bColPick			= false;
		vScale				= MVector(1.0f,1.0f,1.0f);
		vColor				= MVector(0.6f,0.6f,0.6f);
		nSpawnWeight		= 100;
		nWeaponItemID		= 300000;		// ��� �ܰ�
		fTremble			= 30.0f;		// �⺻�� ĳ���Ͱ��̶� ����
		
		nIntelligence		= 3;
		nAgility			= 3;
		fDyingTime			= 5.0f;

		fAttackRange		= 130.0f;
		fAttackRangeAngle	= 1.570796f;		// 90��
		fAttackHitRate		= 1.0f;			// �⺻�� 100% ���� (���� ���� NPC�� �� ��ġ�� ���)
		fAttackCoolTime		= 0.0f;
		fCollisionCheckSpeed = 0.0f;


		fViewAngle			= 3.14159f;		// 180��
		fViewDistance		= 800.0f;
		fViewDistance360	= 800.0f;

		bNeverBlasted				= false;
		bMeleeWeaponNeverDamaged	= false;
		bRangeWeaponNeverDamaged	= false;
		bShadow						= true;
		bNeverPushed				= false;
		bNeverAttackCancel			= false;

		nSkills				= 0;

		nDropTableID		= 0;
		szDropTableName[0]	= 0;

		nOffenseType		= 1;
		bFriendly			= false;

		for (int i = 0; i < NPC_SOUND_END; i++) szSoundName[i][0] = 0;
	}

	MQuestNPCSpawnType GetSpawnType();		///< ���� Ÿ�� ��ȯ
};


#define NPC_INTELLIGENCE_STEPS		5
#define NPC_AGILITY_STEPS			5

struct MQuestNPCGlobalAIValue
{
	// shaking ratio
	float		m_fPathFinding_ShakingRatio;
	float		m_fAttack_ShakingRatio;
	float		m_fSpeed_ShakingRatio;

	// update time
	float		m_fPathFindingUpdateTime[NPC_INTELLIGENCE_STEPS];
	float		m_fAttackUpdateTime[NPC_AGILITY_STEPS];
};

/// NPC ���� ������ Ŭ����
class MQuestNPCCatalogue : public map<MQUEST_NPC, MQuestNPCInfo*>
{
private:
	MQuestNPCGlobalAIValue			m_GlobalAIValue;

	// �Լ�
	void ParseNPC(MXmlElement& element);
	void Insert(MQuestNPCInfo* pNPCInfo);
	void ParseGlobalAIValue(MXmlElement& element);

	// ���� ����.
	map< int, MQUEST_NPC > m_MonsterBibleCatalogue;

public :
	MQuestNPCInfo* GetIndexInfo( int nIndex );

public:
	MQuestNPCCatalogue();													///< ������
	~MQuestNPCCatalogue();													///< �Ҹ���

	bool ReadXml(const char* szFileName);									///< xml�κ��� npc������ �д´�.
	bool ReadXml(MZFileSystem* pFileSystem,const char* szFileName);			///< xml�κ��� npc������ �д´�.
	void Clear();															///< �ʱ�ȭ

	MQuestNPCInfo* GetInfo(MQUEST_NPC nNpc);								///< NPC ���� ��ȯ
	MQuestNPCInfo* GetPageInfo( int nPage);									///< �������κ��� NPC ���� ��ȯ

	MQuestNPCGlobalAIValue* GetGlobalAIValue() { return &m_GlobalAIValue; }
};

/////////////////////////////////////////////////////////////////////////////////////////////

/// NPC Set�� NPC����
struct MNPCSetNPC
{
	MQUEST_NPC		nNPC;					///< NPC ����
	int				nMinRate;				///< ������ �ּ� ����(�ۼ�Ʈ)
	int				nMaxRate;				///< ������ �ִ� ����
	int				nMaxSpawnCount;			///< ������ �� �ִ� �ִ� ����

	/// ������
	MNPCSetNPC()
	{
		nNPC = NPC_NONE;
		nMinRate = 0;
		nMaxRate = 0;
		nMaxSpawnCount = 0;
	}
};

/// NPC Set
struct MQuestNPCSetInfo
{
	int					nID;				///< ID
	char				szName[16];			///< �̸�
	MQUEST_NPC			nBaseNPC;			///< �⺻ ���̽� NPC
	vector<MNPCSetNPC>	vecNPCs;			///< ������ NPC
};

/// NPC Set ���� ������ Ŭ����
class MQuestNPCSetCatalogue : public map<int, MQuestNPCSetInfo*>
{
private:
	map<string, MQuestNPCSetInfo*>		m_NameMap;
	// �Լ�
	void Clear();
	void ParseNPCSet(MXmlElement& element);
	void Insert(MQuestNPCSetInfo* pNPCSetInfo);
public:
	MQuestNPCSetCatalogue();											///< ������
	~MQuestNPCSetCatalogue();											///< �Ҹ���

	bool ReadXml(const char* szFileName);								///< xml�κ��� npc������ �д´�.
	bool ReadXml(MZFileSystem* pFileSystem,const char* szFileName);		///< xml�κ��� npc������ �д´�.

	MQuestNPCSetInfo* GetInfo(int nID);									///< NPC Set ���� ��ȯ
	MQuestNPCSetInfo* GetInfo(const char* szName);						///< NPC Set ���� ��ȯ
};


#endif