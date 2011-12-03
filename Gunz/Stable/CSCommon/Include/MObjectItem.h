#ifndef MOBJECTITEM_H
#define MOBJECTITEM_H

#include "MXml.h"
#include "MObject.h"
#include "MObjectTypes.h"
#include <list>
using namespace std;

#define FILENAME_ITEM_DESC		"item.xml"

/// ���� ������ ������ Ÿ��
enum MSlotType
{
	MET_INVENTORY = 0,
	MET_HEAD,
	MET_CHEST,
	MET_SHOULDER,
	MET_HANDS,
	MET_WAIST,
	MET_LEGS,
	MET_FEET,
	MET_FINGER,
	MET_NECK,
	MET_BACK,
	MET_PRIMARY,
	MET_SECONDARY,
	MET_AMMO,
	MET_KEY,
	MET_CONTAINER
};
typedef unsigned long int MSlotTypes;
#define SetSlotType(v, f)		(v |= (1 << f))
#define DelSlotType(v, f)		(v &= -(1 << f))
#define CheckSlotType(v, f)		(v & (1 << f))

struct MItemDesc
{
	// �ĺ�
	unsigned long int	nID;
	unsigned long int	nSerial;
	char				szName[64];
	unsigned char		nCreator;

	// �з�
	unsigned char		nMagic;
	unsigned char		nUnique;
	unsigned char		nTradable;

	// ����
	MSlotTypes			SlotTypes;	///< ���� ������ ���Ե�

	// ����
	unsigned char		nAtkDmg;		// ����
	unsigned char		nAtkDelay;
	unsigned char		nAtkBonus;
	unsigned char		nAtkTarget;
	unsigned char		nAtkRangeMin;
	unsigned char		nAtkRangeMax;
	unsigned char		nAtkEffective;
	unsigned char		nAtkType;

	// ���
	unsigned char		nAC;

	// STAT ����ġ
	char				nSTR;		
	char				nCON;
	char				nDEX;
	char				nAGI;
	char				nINT;
	char				nCHA;
	char				nHP;
	char				nEN;
	char				nFR;
	char				nCR;
	char				nPR;
	char				nLR;
	char				nMR;

	// ����Ʈ
	unsigned long int	nEffect;
	unsigned char		nEffectType;
	char				nRandomRate;
	char				nCharges;

	// ����
	unsigned int		nWeight;
	bool				bStackable;
	unsigned char		nStackCount;

	// �䱸����
	MCharacterClasses	nClasses;
	MCharacterRaces		nRaces;
	unsigned char		nMinLevel;
	unsigned char		nMinSTR;
	unsigned char		nMinCON;
	unsigned char		nMinDEX;
	unsigned char		nMinAGI;
	unsigned char		nMinINT;
	unsigned char		nMinCHA;

	// �ð�, ����
	unsigned char		nIcon;
	bool				bHasLook;
	unsigned char		nLookType;
	unsigned char		nLookTexture;
	unsigned char		nDropLook;
	unsigned int		nBlendColor;
	unsigned char		nParticle;
	unsigned char		nSound;

	// ��Ŀ��
	char				nMcritical;
	char				nRCritical;
	char				nMAspd;
	char				nRAspd;
	char				nMAccu;
	char				nRAccu;
	char				nMFlee;
	char				nRFlee;
	char				nSFlee;
	char				nHPRegen;
	char				nENRegen;
	char				nDSight;
	char				nNSight;
	char				nSpd;
	char				nOCS;
	char				nDCS;
	char				nRCS;
	char				nECS;
	char				nOMS;
	char				nDMS;
	char				nEMS;
};

class MItemCatalog : public map<int, MItemDesc*>
{
private:
protected:
public:
	MItemCatalog();
	virtual ~MItemCatalog();
	bool Read(const char* szFileName);
	void Clear();
	MItemDesc* Find(int nDescID);
};

/// ������ ������Ʈ
class MObjectItem : public MObject {
protected:
	MObject*		m_pOwner;			///< ������ ������
	MObjectItem*	m_pParent;
	MItemDesc*		m_pItemDesc;
	long			m_nQuantity;		///< ������ ����
	rvector	m_Pos;
public:
	MObjectItem();
	MObjectItem(MUID& uid, MMap* pMap, rvector& Pos);
	MObjectItem(MUID& uid, int nDescID, MMap* pMap, rvector& Pos);
	virtual ~MObjectItem(void){}
	const rvector& GetPos() { return m_Pos; }
	void SetPos(const rvector& Pos) { m_Pos = Pos; }

	
	void SetDescID(int nDescID);
	void SetDesc(MItemDesc* pItemDesc) { m_pItemDesc = pItemDesc; }
	int  GetDescID() { return m_pItemDesc->nID; }
	MItemDesc*	GetDesc() { return m_pItemDesc; }
	void SetOwner(MObject* pOwner) { m_pOwner = pOwner; }
	MObject* GetOwner() { return m_pOwner; }

	DECLARE_RTTI()
};

class MObjectItemVector : public vector<MObjectItem*> {};
class MObjectItemList : public list<MObjectItem*> {};
class MObjectItemMap : public map<MUID, MObjectItem*> { };


/// ����� �� �ִ� ����
enum MEquipmentSlot
{
	MES_HEAD = 0,
	MES_CHEST,
	MES_SHOULDER,
	MES_HANDS,
	MES_WAIST,
	MES_LEGS,
	MES_FEET,
	MES_LFINGER,
	MES_RFINGER,
	MES_NECK,
	MES_BACK,
	MES_PRIMARY,
	MES_SECONDARY,
	MES_AMMO,
	MES_PRIMARY2,
	MES_SECONDARY2,
	MES_AMMO2,
	MES_END				// enum ����� ������ �˱� ����
};


class MEquipContainer
{
private:
protected:
	
public:
	MEquipContainer();
	virtual ~MEquipContainer();
	MObjectItemVector		m_EquipItemVector;
};


/// �������� ���� �� �ִ� �����̳�
class MItemContainer : public MObjectItem
{
private:
protected:
	int						m_nMaxSpace;		///< ���� ������ ����
public:
	MObjectItemVector		m_ObjectItemVector;

	MItemContainer(MUID& uid, MMap* pMap, rvector& Pos);
	MItemContainer();
	virtual ~MItemContainer();

	bool Create(int nMaxSpace);
	void Destroy();
	void Clear();

	bool Push(int nIndex, MObjectItem* pObjectItem);
	void ForcePush(int nIndex, MObjectItem* pObjectItem, MObjectItem* pOutObjectItem);
	MObjectItem* Pop(int nIndex);
	MObjectItem* GetItem(int nIndex);

	DECLARE_RTTI()
};


extern MItemCatalog	g_ItemCatalog;

// TAG
#define MICTOK_ITEM		"ITEM"
#define MICTOK_IDENT	"IDENT"
#define MICTOK_CLASS	"CLASS"
#define MICTOK_slot		"SLOT"
#define MICTOK_ATTACK	"ATTACK"
#define MICTOK_DEFENSE	"DEFENSE"
#define MICTOK_STAT		"STAT"
#define MICTOK_EFFECT	"EFFECT"
#define MICTOK_VISUAL	"VISUAL"
#define MICTOK_FOCUS	"FOCUS"

// Attribute
#define MICTOK_ID		"id"
#define MICTOK_NAME		"name"
#define MICTOK_SLOTTYPE	"type"

#endif