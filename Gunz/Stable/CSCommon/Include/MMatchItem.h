#ifndef _MMATCHITEM_H
#define _MMATCHITEM_H

#include "winsock2.h"
#include "MXml.h"
#include "MUID.h"
#include "MSync.h"
#include "MBaseItem.h"
#include "MMemoryProxy.h"
#include <map>
#include <list>
#include <vector>
#include <algorithm>
using namespace std;

class MZFileSystem;
class MMatchCRC32XORCache;


/// ������ Ÿ��
enum MMatchItemType
{
	MMIT_MELEE = 0,
	MMIT_RANGE,
	MMIT_EQUIPMENT,
	MMIT_CUSTOM,
	MMIT_TICKET,
	MMIT_AVATAR,
	MMIT_COMMUNITY,
	MMIT_LONGBUFF,
	MMIT_END
};

/// ������ ���� Ÿ��
enum MMatchItemSlotType
{
	MMIST_NONE = 0,
	MMIST_MELEE,
	MMIST_RANGE,
	MMIST_CUSTOM,
	MMIST_HEAD,
	MMIST_CHEST,
	MMIST_HANDS,
	MMIST_LEGS,
	MMIST_FEET,
	MMIST_FINGER,
	MMIST_EXTRA,
	MMIST_AVATAR,
	MMIST_COMMUNITY,
	MMIST_LONGBUFF,
	MMIST_END
};

//�� ������.....
#define MMATCH_PARTS_ITEM		100
#define MMATCH_PARTS_PRIMARY	108
#define MMATCH_PARTS_SECONDARY	109
#define MMATCH_PARTS_CUSTOM1	110
#define MMATCH_PARTS_CUSTOM2	111

/// ������ 
// MMCIP_END�� ���� �� enum�� ������ ���� �ٲ�� ���÷��� �ε��� ������ ��ġ�Ƿ� �����Ϸ��� ���ǰ� �ʿ���..
// Ư�� MMCIP_END�� MTD_CharInfo�� ũ�⸦ ��ȭ��Ű�Ƿ� ���÷��� �ε� �ڵ带 �������־�� �մϴ�.
// �׸��� �߰� �Ҷ��� ����� �� ������ �ؾ��մϴ�. �߰��� ������ MMatchCharItemParts ����ϴ� Ŀ�ǵ� �ڵ带 ���� ���÷��̿����� ����� �� ���� ��
enum MMatchCharItemParts
{
	MMCIP_HEAD		= 0,
	MMCIP_CHEST		= 1,
	MMCIP_HANDS  	= 2,
	MMCIP_LEGS		= 3,
	MMCIP_FEET		= 4,
	MMCIP_FINGERL	= 5,
	MMCIP_FINGERR	= 6,
	MMCIP_MELEE		= 7,
	MMCIP_PRIMARY	= 8,
	MMCIP_SECONDARY	= 9,
	MMCIP_CUSTOM1	= 10,	
	MMCIP_CUSTOM2	= 11,
	MMCIP_AVATAR	= 12,
	MMCIP_COMMUNITY1	= 13,
	MMCIP_COMMUNITY2	= 14,
	MMCIP_LONGBUFF1	= 15,
	MMCIP_LONGBUFF2	= 16,
	MMCIP_END
};

// Ŀ���� ������ Ÿ��
enum MMatchCustomItemType
{
	MMCIT_MED_KIT		= 0,
	MMCIT_REPAIR_KIT,
	MMCIT_BULLET_KIT,
	MMCIT_DYNAMITE,
	MMCIT_REMOTE_CHARGE,
	MMCIT_DEMOLITION_TOOLS,
	MMCIT_FLASH_BANG,
	MMCIT_FRAGMENTATION,
	MMCIT_SMOKE_GRENADE,
	MMCIT_TEARGAS_GRENADE,
	MMCIT_FOOD,

	MMCIT_ENCHANT_FIRE,			// ��æƮ ������ - fire
	MMCIT_ENCHANT_COLD,			// ��æƮ ������ - cold
	MMCIT_ENCHANT_LIGHTNING,	// ��æƮ ������ - lightning
	MMCIT_ENCHANT_POISON,		// ��æƮ ������ - poison

	MMCIT_POTION,				// �Ҹ� ������ �� ȸ�� �迭 + ���� ����

	MMCIT_END
};

enum MMatchMeleeItemType
{
	MIT_DAGGER			= 0,
	MIT_DUAL_DAGGER		= 1,
	MIT_KATANA			= 2,
	MIT_GREAT_SWORD		= 3,
	MIT_DOUBLE_KATANA	= 4,

	MIT_END
};

enum MMatchRangeItemType
{
	RIT_PISTOL			= 0,
	RIT_PISTOLx2		= 1,
	RIT_REVOLVER		= 2,
	RIT_REVOLVERx2		= 3,
	RIT_SMG				= 4,
	RIT_SMGx2			= 5,
	RIT_SHOTGUN			= 6,
	RIT_SAWED_SHOTGUN	= 7,
	RIT_RIFLE			= 8,
	RIT_MACHINEGUN		= 9,
	RIT_ROCKET			= 10,
	RIT_SNIFER			= 11,

	RIT_END
};

// Melee, Range, Custom�� ��ģ ���� Ÿ�� .. �Ѽ� ����� ������ ����..
enum MMatchWeaponType
{
	MWT_NONE	= 0,

	// melee
	MWT_DAGGER,
	MWT_DUAL_DAGGER,
	MWT_KATANA,
	MWT_GREAT_SWORD,
	MWT_DOUBLE_KATANA,

	// range
	MWT_PISTOL,
	MWT_PISTOLx2,
	MWT_REVOLVER,
	MWT_REVOLVERx2,
	MWT_SMG,
	MWT_SMGx2,
	MWT_SHOTGUN,
	MWT_SAWED_SHOTGUN,
	MWT_RIFLE,
	MWT_MACHINEGUN,
	MWT_ROCKET,
	MWT_SNIFER,

	// custom
	MWT_MED_KIT,
	MWT_REPAIR_KIT,
	MWT_BULLET_KIT,
	MWT_FLASH_BANG,
	MWT_FRAGMENTATION,
	MWT_SMOKE_GRENADE,
	MWT_FOOD,
	MWT_SKILL,				// NPC�� - skill.xml�� ����Ǿ� �ִ� �ɷ� �����Ѵ�.

	// custom - enchant
	MWT_ENCHANT_FIRE,			
	MWT_ENCHANT_COLD,
	MWT_ENCHANT_LIGHTNING,
	MWT_ENCHANT_POISON,

	// custom
	MWT_POTION,
	MWT_TRAP,
	MWT_DYNAMITYE,

	MWT_END
};

enum MMatchSpendType
{
	MMCT_NONE,
	MMCT_NORMAL,	// �Ϲ� ��� ������(���̳�����Ʈ. ����)
	MMCT_LONGBUFF,	// �� ���� ��� ������
	MMCT_SHORTBUFF,	// �� ���� ��� ������
	MMCT_COMMUNITY,	// Ŀ�´�Ƽ ��� ������
	MMCT_END
};

enum MMatchDamageType
{
	MMDT_NORMAL,
	MMDT_FIRE,
	MMDT_COLD,
	MMDT_POISION,
	MMDT_LIGHTING,
	MMDT_HEAL,
	MMDT_REPAIR,
	MMDT_HASTE,
	MMDT_END
};

enum MMatchTicketType
{
	MMTT_NONE = 0,
	MMTT_ADMISSION,
	MMIT_CHANGEHEAD,

	MMTT_END,
};

enum MMatchItemEffectId
{
	// zitem.xml���� �Ʒ� ����� ����ϹǷ� ������ ��ġ�� ���Ƿ� �����ϸ� �ȵ˴ϴ�
	MMIEI_NONE = 0,
	
	// �Ƿ�ĸ��, �Ƿ����, ����ĸ��, �������� ����Ʈ ����
	MMIEI_POTION_HEAL_INSTANT		= 100,
	MMIEI_POTION_REPAIR_INSTANT		= 101,
	MMIEI_POTION_HEAL_OVERTIME		= 102,
	MMIEI_POTION_REPAIR_OVERTIME	= 103,

	// ����ĸ��
	MMIEI_POTION_HASTE				= 130,
};

struct MMatchItemEffectDesc
{
	unsigned long int	m_nID;
	char				m_szName[128];
	int					m_nArea;
	unsigned long int	m_nTime;
	int					m_nModHP;
	int					m_nModAP;
	int					m_nModMaxWT;
	int					m_nModSF;
	int					m_nModFR;
	int					m_nModCR;
	int					m_nModPR;
	int					m_nModLR;
	int					m_nResAP;
	int					m_nResFR;
	int					m_nResCR;
	int					m_nResPR;
	int					m_nResLR;
	int					m_nStun;
	int					m_nKnockBack;
	int					m_nSmoke;
	int					m_nFlash;
	int					m_nTear;
	int					m_nFlame;
};

enum MMatchItemBonusType
{
	MIBT_SOLO = 0,
	MIBT_TEAM = 1,
	MIBT_QUEST = 2
};

struct MMatchItemBonus
{
	float				m_fXP_SoloBonus;
	float				m_fXP_TeamBonus;
	float				m_fXP_QuestBonus;

	float				m_fBP_SoloBonus;
	float				m_fBP_TeamBonus;
	float				m_fBP_QuestBonus;
};

struct MMatchItemName
{
	char m_szItemName[128];
	char m_szMeshName[128];
};

struct MMatchAvatarMeshName
{
	char m_szHeadMeshName[128];
	char m_szChestMeshName[128];
	char m_szHandMeshName[128];
	char m_szLegsMeshName[128];
	char m_szFeetMeshName[128];	
};

struct MMatchItemDesc
{
	unsigned long int		m_nID;
//	char					m_szItemName[128];
	MProtectValue<MMatchItemName>* m_pMItemName;
	MProtectValue<int>		m_nTotalPoint;
	MProtectValue<MMatchWeaponType>	m_nWeaponType;
	MProtectValue<MMatchItemType>	m_nType;
	MProtectValue<int>		m_nResSex;
	MProtectValue<int>		m_nResLevel;
	MMatchItemSlotType		m_nSlot;
	
	MProtectValue<int>		m_nWeight;
	MProtectValue<int>		m_nBountyPrice;
	bool					m_bIsCashItem;	
	MProtectValue<int>		m_nDelay;
	MMatchItemEffectDesc*	m_pEffect;
	MMatchItemEffectId		m_nEffectId;
	MProtectValue<int>		m_nControllability;
	MProtectValue<int>		m_nMagazine;
	MProtectValue<int>		m_nMaxBullet;
	MProtectValue<int>		m_nReloadTime;
	bool					m_bSlugOutput;
	MProtectValue<int>		m_nGadgetID;
	MProtectValue<int>		m_nHP;
	MProtectValue<int>		m_nAP;
	MProtectValue<int>		m_nMaxWT;
	MProtectValue<int>		m_nSF;
	MProtectValue<int>		m_nFR;
	MProtectValue<int>		m_nCR;
	MProtectValue<int>		m_nPR;
	MProtectValue<int>		m_nLR;
	MProtectValue<int>		m_nLimitSpeed;
	MProtectValue<int>		m_nLimitJump;
	MProtectValue<int>		m_nLimitTumble;
	MProtectValue<int>		m_nLimitWall;
	MProtectValue<int>		m_nRange;		// melee���� ���ݹ���
	MProtectValue<int>		m_nAngle;		// melee���� ���ݰ���
	MProtectValue<int>		m_nEffectLevel;


	MProtectValue<int>				m_nDamage;			// ������� ���ݷ�
	MProtectValue<int>				m_nItemPower;		// �Ҹ� �������� ������ �Ǵ� ȸ����
	MProtectValue<int>				m_nDamageTime;		// ��Ʈ ������ �������� ȸ���� ���� �ð�(0�� ���, �� �游 ����)
	MProtectValue<MMatchDamageType>	m_nDamageType;		// �Ҹ� �������� ������ Ÿ�� (������ �Ӽ� Ȥ�� ȸ�� �Ӽ�)
	MProtectValue<int>		m_nLifeTime;				// �۵� �ð� (����� Ʈ���� �۵��ð����� ���)

	char					m_szDesc[8192];	// ����

	bool								m_bIsSpendableItem;
	MProtectValue<MMatchSpendType>		m_nSpendType;

	MProtectValue<MMatchAvatarMeshName>*		m_pAvatarMeshName;

	unsigned long int		m_nColor;
	int						m_nImageID;
	int						m_nBulletImageID;
	int						m_nMagazineImageID;
	char					m_szReloadSndName[256];
	char					m_szFireSndName[256];
	char					m_szDryfireSndName[256];
	char					m_szWeaponByFiber[256];		// melee ������ ������ ���� �Ҹ�

	MMatchItemBonus			m_Bonus;
	MMatchTicketType		m_TicketType;
	MProtectValue<int>		m_nMaxRentPeriod;	// �Ⱓ�� �ٿ�Ƽ �������� �ִ� �Ⱓ ���� (��¥����)

	bool					m_bIsEnableMoveToAccountItem;

	MMatchItemDesc();
	~MMatchItemDesc();

	int GetSellBountyValue(int nCnt = 1) { return int(m_nBountyPrice.Ref() * 0.25) * nCnt; }

	bool IsCashItem()		{ if ((m_nID>=500000) || (m_bIsCashItem)) return true; return false; }
	bool IsEnchantItem()	{ if (m_nWeaponType.Ref() >= MWT_ENCHANT_FIRE && m_nWeaponType.Ref() <= MWT_ENCHANT_POISON) return true; 
								return false; }
	bool IsUnLimitItem()		{ return RENT_PERIOD_UNLIMITED == m_nMaxRentPeriod.Ref(); }	
	bool IsSpendableItem()		{ return m_bIsSpendableItem; }


	void CacheCRC32( MMatchCRC32XORCache& crc );

	void ShiftFugitiveValues();

	void DumpBinary(FILE* fp);
	void LoadBinary(FILE* fp);
};


struct MMatchItemDescForDatabase
{
	unsigned long int m_nID;

	int m_nResSex;
	int	m_nResLevel;

	MMatchItemSlotType	m_nSlot;

	int m_nWeight;
	int m_nBountyPrice;
	int m_nDamage;
	int m_nDelay;

	int m_nControllability;
	int m_nMaxBullet;
	int m_nMagazine;
	int m_nReloadTime;	
	
	int m_nHP;
	int m_nAP;

	bool m_bIsCashItem;
	bool m_bIsSpendableItem;
};

// ���԰� ������ �������� üũ
bool IsSuitableItemSlot(MMatchItemSlotType nSlotType, MMatchCharItemParts nParts);
MMatchCharItemParts GetSuitableItemParts(MMatchItemSlotType nSlotType);
MMatchItemSlotType	GetSuitableItemSlot(MMatchCharItemParts nParts);
bool IsWeaponItemSlotType(MMatchItemSlotType nSlotType);
bool IsWeaponCharItemParts(MMatchCharItemParts nParts);

char* GetItemSlotTypeStr(MMatchItemSlotType nSlotType);
char* GetCharItemPartsStr(MMatchCharItemParts nParts);

// ����Ÿ�� �˾Ƴ���
MMatchWeaponType GetWeaponType(MMatchMeleeItemType nMeleeItemType);
MMatchWeaponType GetWeaponType(MMatchRangeItemType nRangeItemType);
MMatchWeaponType GetWeaponType(MMatchCustomItemType nCustomItemType);

// ��æƮ���������� Ȯ��
bool IsEnchantItem(MMatchItemDesc* pItemDesc);

/*
class MMatchItemEffectDescMgr : public map<int, MMatchItemEffectDesc*>
{
protected:
	void ParseEffect(MXmlElement& element);
public:
	MMatchItemEffectDescMgr();
	virtual ~MMatchItemEffectDescMgr();
	bool ReadXml(const char* szFileName);
	bool ReadXml(MZFileSystem* pFileSystem, const char* szFileName);
	void Clear();
	MMatchItemEffectDesc* GetEffectDesc(int nID);
	static MMatchItemEffectDescMgr* GetInstance();
};

inline MMatchItemEffectDescMgr* MGetMatchItemEffectDescMgr() { return MMatchItemEffectDescMgr::GetInstance(); }
*/

class MMatchItemDescMgr : public map<int, MMatchItemDesc*>
{
	unsigned long m_nChecksum;

	int m_nextItemIdToMemoryShift;
	
protected:
	bool ParseItem(MXmlElement& element);
public:
	MMatchItemDescMgr();
	virtual ~MMatchItemDescMgr();
	bool ReadXml(const char* szFileName);
	bool ReadXml(MZFileSystem* pFileSystem, const char* szFileName);
	void Clear();
	MMatchItemDesc* GetItemDesc(unsigned long int nID);
	static MMatchItemDescMgr* GetInstance();

	unsigned long GetChecksum() { return m_nChecksum; }

	void ShiftMemoryGradually();

	// ���� ���õ� Validation Check
	bool ValidateItemBuff();

	bool ReadCache();
	void WriteCache();
};
inline MMatchItemDescMgr* MGetMatchItemDescMgr() { return MMatchItemDescMgr::GetInstance(); }



typedef struct _DBCharItemCachingData
{
	bool	bNeedDBUpdate;
	int		nAddedItemCount;

	void Reset() { 
		nAddedItemCount = 0; 
		bNeedDBUpdate = false;
	}

	void IncCnt(int nVal) { 
		nAddedItemCount += nVal; 
		bNeedDBUpdate = true; 
	}

	void DecCnt(int nVal) {
		nAddedItemCount -= nVal; 
		bNeedDBUpdate = true; 
	}
	
	bool IsNeedDBUpdate() { return bNeedDBUpdate; }
} DBCharItemCachingData;


class MMatchItem : public MBaseItem
{
private:
protected:
	MUID				m_uidItem;	
	MMatchItemDesc*		m_pDesc;					///< ��ũ����
	bool				m_bEquiped;					///< ����ϰ� �ִ��� ����
	unsigned long int	m_nRentItemRegTime;			///< �Ⱓ�� �������� ��� ����� �ð�. �� �ð��� ������ ��ϵǴ� �ð���.

	// ����ϴ� �߰��� description�� NULL�� �Ǵ��� �˻��ϱ� ���ؼ�.
	unsigned int		m_nItemID;

	WORD				m_nCountOfNonDestroyItem;		///< ����ϸ� ������ ������ ��������� �ʴ� �������� �ִ� ��� ����(ex ����Ŷ).
	WORD				m_nUseCountOfNonDestroyItem;	///< ����ϸ� ������ ������ ��������� �ʴ� �������� ���� ����(ex ����Ŷ).


	unsigned long int	m_nCIID;					///< DB�� ���� CIID

	DBCharItemCachingData m_CharItemCachingData;

protected:
	void SetDesc(MMatchItemDesc* pDesc) { m_pDesc = pDesc; }

public:
	MMatchItem();
	virtual ~MMatchItem();

	bool Create( const MUID& uid, MMatchItemDesc* pDesc, const WORD nCountOfNonDesctroyItem, int nCount = 1);
	void Destroy();

	MUID				GetUID() const								{ return m_uidItem; }
	unsigned long int	GetCIID() const								{ return m_nCIID; }
	unsigned long int	GetDescID() const;
	
	unsigned long int	GetRentItemRegTime() const					{ return m_nRentItemRegTime; }
	MMatchItemDesc*		GetDesc() const;
	MMatchItemType		GetItemType();
	
	void				SetCIID(unsigned long int nCIID)			{ m_nCIID = nCIID; }	
	void				SetEquiped(bool bIsEquiped)					{ m_bEquiped = bIsEquiped; }
	void				SetRentItemRegTime(unsigned long int nTime)	{ m_nRentItemRegTime = nTime; }	

	bool				IsEmpty()									{ return (((m_pDesc == NULL) || (m_nCount <= 0)) ? true : false); }
	bool				IsEquiped() const							{ return m_bEquiped; }			

	const WORD			GetCountOfNonDestroyItem()					{ return m_nCountOfNonDestroyItem; }
	const WORD			GetUseCountOfNonDestroyItem()				{ return m_nUseCountOfNonDestroyItem; }
	void				ResetUseCountOfNonDestroyItem()				{ m_nUseCountOfNonDestroyItem = 0; }
	void				IncreaseUseCountOfNonDestroyItem();

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// DB Caching�� ���Ͽ� ������� ��� �Լ���..	
	void				IncreaseCount(int nVal);
	void				DecreaseCount(int nVal);
	void				DecreaseCountWithCaching(int nVal);

	bool				IsNeedDBUpdate()							{ return m_CharItemCachingData.IsNeedDBUpdate(); }
	void				DBUpdateDone()								{ m_CharItemCachingData.Reset(); }	
	DBCharItemCachingData* GetItemCachingData()						{ return &m_CharItemCachingData; }
};


class MMatchItemMap;
class MMatchCharInfo;

typedef struct _EquipedParts
{
	MUID	uidParts;
	int		nItemCount;
} EquipedParts;

/// ����ϰ��ִ� ������
class MMatchEquipedItem
{
protected:
	// MMatchItem*		m_pParts[MMCIP_END];
	//EquipedParts	m_Parts[MMCIP_END];
	MUID			m_uidParts[ MMCIP_END ];	
	
	MMatchCharInfo*	m_pOwner;

public:
	MMatchEquipedItem() 
	{ 
		// memset(m_pParts, 0, sizeof(m_pParts)); 
		memset( m_uidParts, 0, sizeof(m_uidParts) );
		// memset(m_Parts, 0, sizeof(EquipedParts) * MMCIP_END);


		m_pOwner = NULL;
	}

	virtual ~MMatchEquipedItem() 
	{ 
		m_pOwner = NULL;
	}
public:
	// bool SetItem(MMatchCharItemParts parts, MMatchItem* pMatchItem);
	bool SetItem( MMatchCharItemParts parts, const MUID& uidItem, MMatchItem* pItem );

	MMatchItem* GetItem(MMatchCharItemParts parts); //  { return m_pParts[parts]; }
	void Remove(MMatchCharItemParts parts);
	void Remove(const MUID& uidParts);
	// bool IsEmpty(MMatchCharItemParts parts) { if (m_pParts[parts] != NULL) return false; return true; }
	bool IsEmpty(MMatchCharItemParts parts); //  { if (m_pParts[parts] != NULL) return false; return true; }
	void GetTotalWeight(int* poutWeight, int* poutMaxWeight);
	// bool IsEquipedItem(MMatchItem* pCheckItem, MMatchCharItemParts& outParts); // �ش� �������� ��������� üũ
	bool IsEquipedItem(const MUID& uidItem, MMatchCharItemParts& outParts); // �ش� �������� ��������� üũ
	void Clear();
	void SetOwner( MMatchCharInfo* pOwner ) { _ASSERT( NULL != pOwner ); m_pOwner = pOwner; }


private :
	// MMatchItem* GetMyItem( MMatchCharItemParts parts );
};

/// ĳ���Ͱ� ���� �ִ� �����۵�
class MMatchItemMap : public map<MUID, MMatchItem*>
{
private:
protected:
	static MUID				m_uidGenerate;
	static MCriticalSection	m_csUIDGenerateLock;
	bool					m_bDoneDbAccess;		// ��񿡼� ������ �����Ծ����� ����

	/// �Ⱓ�� �������� �ϳ��� �ִ��� ����
	/// ���� - �Ⱓ�� �������� �ϳ��� ������ true�� ���� �ִ�. 
	bool					m_bHasRentItem;			

public:
	MMatchItemMap();
	virtual ~MMatchItemMap();
	bool IsEmpty() const { return empty(); }
	int GetCount() const { return (int)size(); }
	virtual bool CreateItem( const MUID& uid
		, int nCIID
		, int nItemDescID
		, bool bRentItem = false
		, DWORD dwRentMinutePeriodRemainder = RENT_MINUTE_PERIOD_UNLIMITED
		, const WORD wRentHourPeriod = RENT_PERIOD_UNLIMITED
		, int nCount = 1);
	bool			RemoveItem(const MUID& uidItem);

	virtual void	Clear();
	MMatchItem*		GetItem(const MUID& uidItem) const;
	MMatchItem*		GetItemByItemID(const DWORD dwItemID) const;		//< �������� ItemID�� �־, 1���� �����ش�(Spendable ������ ����)
																		//< �� ��ȣ�� ���� ������, �׷��� �������..

	MMatchItem*		GetItemByCIID(const DWORD dwCIID) const;


	bool			IsDoneDbAccess() const			{ return m_bDoneDbAccess; }
	void			SetDbAccess(bool bVal)			{ m_bDoneDbAccess = bVal; }
	bool			HasRentItem() const				{ return m_bHasRentItem; }
	const bool		IsHave( const DWORD dwItemID ) const;

	int	 GetItemCount(const MUID& uidItem);

public:
	static MUID UseUID() {
		m_csUIDGenerateLock.Lock();
			m_uidGenerate.Increase();	
		m_csUIDGenerateLock.Unlock();
		return m_uidGenerate;
	}
};


bool IsExpiredRentItem( const MMatchItem* pMItem, const DWORD dwTick );

#include "../StringLiteral/cxr_MMatchItem.h"

#endif