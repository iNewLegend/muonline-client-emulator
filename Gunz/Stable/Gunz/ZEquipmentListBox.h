#ifndef ZEQUIPMENTLISTBOX_H
#define ZEQUIPMENTLISTBOX_H

//#include "ZPrerequisites.h"
//#include "MMultiColListBox.h"
//
//bool ZGetIsCashItem(unsigned long nItemID);
//
//
//
//class ZEquipmentListItem_OLD : public MMultiColListItem{
//protected:
//	MBitmap*			m_pBitmap;
//
//	int					m_nAIID;		// �߾����࿡�� ����Ѵ�
//	unsigned long		m_nItemID;
//	bool				m_bLevelResticted;	// ���� �� �������� ���� ������
//public:
//	MUID				m_UID;
//public:
//	char	m_szName[256];
//	char	m_szLevel[256];
//	char	m_szPrice[256];
//public:
//	ZEquipmentListItem_OLD(const MUID& uidItem, const unsigned long nItemID, MBitmap* pBitmap, const char* szName, const char* szLevel, const char* szPrice, bool bLeveRestricted)
//	{
//		m_nAIID = 0;
//		m_nItemID = nItemID;
//
//		m_pBitmap = pBitmap;
//		m_UID = uidItem;
//		strcpy(m_szName, szName);
//		strcpy(m_szLevel, szLevel);
//		strcpy(m_szPrice, szPrice);
//		m_bLevelResticted = bLeveRestricted;
//	}
//	ZEquipmentListItem_OLD(const int nAIID, const unsigned long nItemID, MBitmap* pBitmap, const char* szName, const char* szLevel, const char* szPrice, bool bLeveRestricted)
//	{
//		m_nAIID = nAIID;
//		m_nItemID = nItemID;
//
//		m_pBitmap = pBitmap;
//		m_UID = MUID(0,0);
//		strcpy(m_szName, szName);
//		strcpy(m_szLevel, szLevel);
//		strcpy(m_szPrice, szPrice);
//		m_bLevelResticted = bLeveRestricted;
//	}
//
//	ZEquipmentListItem_OLD(void)
//	{
//		m_nAIID = 0;
//		m_nItemID = 0;
//
//		m_pBitmap = NULL;
//		m_UID = MUID(0,0);
//		m_szName[0] = 0;
//		m_szLevel[0] = 0;
//		m_szPrice[0] = 0;
//		m_bLevelResticted = false;
//	}
//	virtual const char* GetString(void)
//	{
//		return m_szName;
//	}
//
//	virtual bool GetDragItem(MBitmap** ppDragBitmap, char* szDragString, char* szDragItemString)
//	{
//		*ppDragBitmap = GetBitmap(0);
//		strcpy(szDragString, m_szName);
//		strcpy(szDragItemString, m_szName);
//
//		return true;
//	}
//	virtual MBitmap* GetBitmap(int i)
//	{
//		if (i == 0) return m_pBitmap;
//		return NULL;
//	}
//	MUID& GetUID() { return m_UID; }
//	int GetAIID() { return m_nAIID; }
//
//	int GetMSID() { return 0; }	/// �ӽ÷� �߰��ص�
//
//	unsigned long	GetItemID()		{ return m_nItemID; }
//
//	virtual void OnDraw(MRECT& r, MDrawContext* pDC, bool bSelected, bool bMouseOver);
//
//	// Ŀ�� ��ġ�� ������ ���� ������ ������ ��ġ�ΰ� �Ǵ��Ѵ� - ���� ������ �������� ���� ������
//	// ���ڴ� ��� ����Ʈ ������ ���� ��ǥ��� �־����
//	bool IsPtInRectToShowToolTip(MRECT& rcItem, MPOINT& pt);
//	
//	// ������ ������ rect �˾Ƴ��� (�׸� ��ǥ��)
//	void GetIconRect(MRECT& out, const MRECT& rcItem);
//
//	virtual int GetSortHint();
//};
//
//class ZItemMenu;
////typedef void (*ZCB_ONDROP)(void* pSelf, MWidget* pSender, MBitmap* pBitmap, const char* szString, const char* szItemString);
//
//class ZEquipmentListBox_OLD : public MMultiColListBox
//{
//protected:
//	virtual bool IsDropable(MWidget* pSender);
////	virtual bool OnDrop(MWidget* pSender, MBitmap* pBitmap, const char* szString, const char* szItemString);
//	virtual bool OnEvent(MEvent* pEvent, MListener* pListener);
//
//protected:
////	ZCB_ONDROP			m_pOnDropFunc;
//	MWidget*			m_pDescFrame;
//	int					m_idxItemLastTooltip;
//protected:
//	ZItemMenu*			m_pItemMenu;	// ZEquipmentList�� Exclusive�� Popup�� Child�϶��� Show()�����ϴ�
//	ZItemMenu* GetItemMenu()	{ return m_pItemMenu; }
//
//public:
//	ZEquipmentListBox_OLD(const char* szName, MWidget* pParent=NULL, MListener* pListener=NULL);
//	virtual ~ZEquipmentListBox_OLD(void);
//	void AttachMenu(ZItemMenu* pMenu);
//
//	char* GetItemDescriptionWidgetName();
//	void SetupItemDescTooltip();
//
////	void Add(const MUID& uidItem, MBitmap* pIconBitmap, const char* szName, const char* szWeight, const char* szSlot, const char* szPrice);
////	void Add(const MUID& uidItem, MBitmap* pIconBitmap, const char* szName, int nWeight, MMatchItemSlotType nSlot, int nBountyPrice);
//
////	void SetOnDropCallback(ZCB_ONDROP pCallback) { m_pOnDropFunc = pCallback; }
//	void SetDescriptionWidget(MWidget *pWidget)	{ m_pDescFrame = pWidget; }
//
//public:
//	#define MINT_EQUIPMENTLISTBOX	"EquipmentListBox"
//	virtual const char* GetClassName(void){ return MINT_EQUIPMENTLISTBOX; }
//	//DWORD	m_dwLastMouseMove;
//	//int		m_nLastItem;
//};
//
////MListener* ZGetShopAllEquipmentFilterListener(void);
////MListener* ZGetEquipAllEquipmentFilterListener(void);
//
////MListener* ZGetShopSaleItemListBoxListener(void);
////MListener* ZGetCashShopItemListBoxListener(void);
////MListener* ZGetShopPurchaseItemListBoxListener(void);
////MListener* ZGetEquipmentItemListBoxListener(void);
////MListener* ZGetAccountItemListBoxListener(void);
#endif