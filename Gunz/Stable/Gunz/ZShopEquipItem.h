#pragma once

#include "ZShopEquipItemConfirm.h"

class MShopSetItem;
class MShopPriceMap;

class IShopEquipItemHandle_Purchase;
class IShopEquipItemHandle_Sell;
class IShopEquipItemHandle_SendAccount;
class IShopEquipItemHandle_BringAccount;

enum ZShopEquipItemType
{
	ZSEIT_GAMBLE,
	ZSEIT_MATCH,
	ZSEIT_QUEST,
	ZSEIT_SET,
};

// �ܺο��� ��� �������� ����(����,��ġ������,����Ʈ������ ��)�� ������� �ٷ� �� �ֵ��� �ϱ� ���� ���� �������̽�
class ZShopEquipItem
{
protected:
	IShopEquipItemHandle_Purchase*		m_pHandlerPurchase;
	IShopEquipItemHandle_Sell*			m_pHandlerSell;
	IShopEquipItemHandle_SendAccount*	m_pHandlerSendAccount;
	IShopEquipItemHandle_BringAccount*	m_pHandlerBringAccount;

	MBitmap* m_pIconBitmap;

public:
	ZShopEquipItem() 
		: m_pHandlerPurchase(NULL)
		, m_pHandlerSell(NULL)
		, m_pHandlerSendAccount(NULL)
		, m_pHandlerBringAccount(NULL)
		, m_pIconBitmap(NULL) {}
	virtual ~ZShopEquipItem();

	void SetHandlePurchase(IShopEquipItemHandle_Purchase* p)		{ m_pHandlerPurchase = p; }
	void SetHandleSell(IShopEquipItemHandle_Sell* p)				{ m_pHandlerSell = p; }
	void SetHandleSendAccount(IShopEquipItemHandle_SendAccount* p)	{ m_pHandlerSendAccount= p; }
	void SetHandleBringAccount(IShopEquipItemHandle_BringAccount* p){ m_pHandlerBringAccount= p; }

	IShopEquipItemHandle_Purchase* GetHandlePurchase()			{ return m_pHandlerPurchase; }
	IShopEquipItemHandle_Sell* GetHandleSell()					{ return m_pHandlerSell; }
	IShopEquipItemHandle_SendAccount* GetHandleSendAccount()	{ return m_pHandlerSendAccount; }
	IShopEquipItemHandle_BringAccount* GetHandleBringAccount()	{ return m_pHandlerBringAccount; }

	virtual ZShopEquipItemType GetType() = 0;

	virtual MMatchItemSlotType GetSlotType() = 0;
	virtual bool CanEquip() = 0;
	virtual bool CanSendAccount() = 0;
	virtual bool CanSell() = 0;
	virtual MBitmap* GetIcon() { return m_pIconBitmap; }
	virtual const char* GetName(char* szBuf) = 0;
	virtual const char* GetPriceText(char* szBuf);
	virtual const char* GetLevelResText(char* szBuf) = 0;

	virtual int GetPrice(bool& out_bCash);
	virtual int GetLevelRes() = 0;

	virtual void FillItemDesc(MTextArea* pTextArea) = 0;
	virtual void UpdateCharInfoText() = 0;
	virtual void UpdateCharacterView(ZCharacterView* pCharacterView) {}
};

// ����,���â�� ���� ��������� ����
class ZShopEquipItem_Gamble : public ZShopEquipItem
{
	const ZGambleItemDefine* m_pItemDesc;

public:
	ZShopEquipItem_Gamble(const ZGambleItemDefine* pDesc);
	ZShopEquipItemType GetType() { return ZSEIT_GAMBLE; }

	const ZGambleItemDefine* GetDesc() { return m_pItemDesc; }

	virtual MMatchItemSlotType GetSlotType() { return MMIST_NONE; }
	virtual bool CanEquip() { return true; } // ����������� equip�� use�� ���� �ǹ�
	virtual bool CanSendAccount() { return m_pItemDesc->IsCash(); }
	virtual bool CanSell() {
#ifndef _SELL_CASHITEM
		return (GetDesc() && !GetDesc()->IsCash());
#else
		return GetDesc() ? true : false;
#endif
	}

	virtual const char* GetName(char* szBuf);
	virtual const char* GetLevelResText(char* szBuf);

	virtual int GetLevelRes();

	virtual void FillItemDesc(MTextArea* pTextArea);
	virtual void UpdateCharInfoText();
};

// ����,���â�� ���� ��ġ������ ����
class ZShopEquipItem_Match : public ZShopEquipItem
{
	MMatchItemDesc* m_pItemDesc;
public:
	ZShopEquipItem_Match(MMatchItemDesc* pDesc);
	ZShopEquipItemType GetType() { return ZSEIT_MATCH; }

	MMatchItemDesc* GetDesc() { return m_pItemDesc; }

	virtual MMatchItemSlotType GetSlotType();
	virtual bool CanEquip() { return true; }
	virtual bool CanSendAccount() { return m_pItemDesc->IsCashItem(); }
	virtual bool CanSell() { 
#ifndef _SELL_CASHITEM
		return (GetDesc() && !GetDesc()->IsCashItem());
#else
		return GetDesc() ? true : false;
#endif
	}

	virtual const char* GetName(char* szBuf);
	virtual const char* GetLevelResText(char* szBuf);

	virtual int GetLevelRes();

	virtual void FillItemDesc(MTextArea* pTextArea);
	static void FillItemDesc(MMatchItemDesc* pItemDesc, MTextArea* pTextArea, ZMyItemNode* pRentalNode);
	virtual void UpdateCharInfoText();
	virtual void UpdateCharacterView(ZCharacterView* pCharacterView);
};

// ����,���â�� ���� ����Ʈ������ ����
class ZShopEquipItem_Quest : public ZShopEquipItem
{
	MQuestItemDesc* m_pItemDesc;
public:
	ZShopEquipItem_Quest(MQuestItemDesc* pDesc);
	ZShopEquipItemType GetType() { return ZSEIT_QUEST; }

	MQuestItemDesc* GetDesc() { return m_pItemDesc; }

	virtual MMatchItemSlotType GetSlotType() { return MMIST_NONE; }
	virtual bool CanEquip() { return false; }
	virtual bool CanSendAccount() { return false; }
	virtual bool CanSell() { return GetDesc() ? true : false; }

	virtual const char* GetName(char* szBuf);
	virtual const char* GetLevelResText(char* szBuf);

	virtual int GetLevelRes();

	virtual void FillItemDesc(MTextArea* pTextArea);
	virtual void UpdateCharInfoText();
};

// ������ ���� ��Ʈ������ ����
class ZShopEquipItem_Set : public ZShopEquipItem
{
	// ��Ʈ�� ������ ���������� �����ϴ� ���̶� MShopSetItem�� ������ �ְ���
	MShopSetItem* m_pShopSetItem;
	int m_nLevelRes;
	int m_nSexRes;
	MMatchItemDesc m_sumSetSpec;	// ��Ʈ ����ǰ���� �ɷ� �ջ�ġ
public:
	ZShopEquipItem_Set(MShopSetItem* pSetItem);
	ZShopEquipItemType GetType() { return ZSEIT_SET; }

	virtual MMatchItemSlotType GetSlotType() { return MMIST_NONE; }
	virtual bool CanEquip() { return false; }		// ��Ʈ�� ���������� �����ϴ� ���̹Ƿ�
	virtual bool CanSendAccount() { return false; }
	virtual bool CanSell() { return false; }

	virtual const char* GetName(char* szBuf);
	virtual const char* GetLevelResText(char* szBuf);

	virtual int GetLevelRes();

	virtual void FillItemDesc(MTextArea* pTextArea);
	virtual void UpdateCharInfoText();
	virtual void UpdateCharacterView(ZCharacterView* pCharacterView);
};


// �������� ���� ������ ���� �ڵ鷯 �������̽�
class IShopEquipItemHandle_Purchase
{
public:
	virtual ~IShopEquipItemHandle_Purchase() {}
	virtual bool GetPrice(bool& out_bCash, int& out_nRentalHour, int& out_nPrice) = 0;
	virtual const char* GetPriceText(char* szBuf);

	void OpenSimpleConfirmDlg(ISimpleConfirmDlgDoneHandler* pDoneHandler);
	void OpenCountableConfirmDlg(const char* szItemName, MBitmap* pIconBmp, int nPrice, int nMax, IItemCountDlgDoneHandler* pDoneHandler);

	virtual void Buy() = 0;
};

class ZShopEquipItemHandle_PurchaseGamble : public IShopEquipItemHandle_Purchase, public ISimpleConfirmDlgDoneHandler
{
	ZShopEquipItem_Gamble* m_pItem;
public:
	ZShopEquipItemHandle_PurchaseGamble(ZShopEquipItem_Gamble* pItem);

	bool GetPrice( bool& out_bCash, int& out_nRentalHour, int& out_nPrice );
	virtual void Buy();

	virtual void OnDone(bool bOk);
};

class ZShopEquipItemHandle_PurchaseMatch : public IShopEquipItemHandle_Purchase, public ISimpleConfirmDlgDoneHandler, public IItemCountDlgDoneHandler
{
	ZShopEquipItem_Match* m_pItem;
public:
	ZShopEquipItemHandle_PurchaseMatch(ZShopEquipItem_Match* pItem);

	bool GetPrice( bool& out_bCash, int& out_nRentalHour, int& out_nPrice );
	virtual void Buy();

	virtual void OnDone(bool bOk);
	virtual void OnDone(int nCount);
};

class ZShopEquipItemHandle_PurchaseQuest : public IShopEquipItemHandle_Purchase, public IItemCountDlgDoneHandler
{
	ZShopEquipItem_Quest* m_pItem;
public:
	ZShopEquipItemHandle_PurchaseQuest(ZShopEquipItem_Quest* pItem);

	bool GetPrice( bool& out_bCash, int& out_nRentalHour, int& out_nPrice );
	virtual void Buy();

	virtual void OnDone(int nCount);
};

class ZShopEquipItemHandle_PurchaseSet : public IShopEquipItemHandle_Purchase
{
	ZShopEquipItem_Set* m_pItem;
public:
	ZShopEquipItemHandle_PurchaseSet(ZShopEquipItem_Set* pItem);

	bool GetPrice( bool& out_bCash, int& out_nRentalHour, int& out_nPrice ) { return false; }
	virtual void Buy();
};


// �������� �Ǹ� ������ ���� �ڵ鷯 �������̽�
class IShopEquipItemHandle_Sell
{
	MUID m_uidItem;
public:
	IShopEquipItemHandle_Sell() : m_uidItem(0,0) {}
	virtual ~IShopEquipItemHandle_Sell() {}
	void SetItemUID(const MUID& uid) { m_uidItem = uid; }
	MUID GetItemUID() { return m_uidItem; }
	virtual bool GetPrice(int& out_nPrice) = 0;
	virtual const char* GetPriceText(char* szBuf);

	void OpenSimpleConfirmDlg(ISimpleConfirmDlgDoneHandler* pDoneHandler);

	void OpenCountableConfirmDlg(const char* szItemName, MBitmap* pIconBmp, int nPrice, int nMax, IItemCountDlgDoneHandler* pDoneHandler);

	void OpenSellCashItemConfirmDlg(const char* szItemName, MBitmap* pIcon, int price, int count, ISellCashItemConfirmDlgDoneHandler* pHandler);

	virtual void Sell() = 0;
};

class ZShopEquipItemHandle_SellGamble : public IShopEquipItemHandle_Sell, public IItemCountDlgDoneHandler
{
	ZShopEquipItem_Gamble* m_pItem;
public:
	ZShopEquipItemHandle_SellGamble(ZShopEquipItem_Gamble* pItem);
	virtual bool GetPrice(int& out_nPrice);
	virtual void Sell();

	virtual void OnDone(int nCount);
};

class ZShopEquipItemHandle_SellMatch : public IShopEquipItemHandle_Sell, public ISimpleConfirmDlgDoneHandler, public IItemCountDlgDoneHandler, public ISellCashItemConfirmDlgDoneHandler
{
	ZShopEquipItem_Match* m_pItem;
public:
	ZShopEquipItemHandle_SellMatch(ZShopEquipItem_Match* pItem);
	virtual bool GetPrice(int& out_nPrice);
	virtual void Sell();

	virtual void OnDone(bool bOk);
	virtual void OnDone(int nCount);
	virtual void OnConfirmSellCashItem(bool bOk);
};

class ZShopEquipItemHandle_SellQuest : public IShopEquipItemHandle_Sell, public IItemCountDlgDoneHandler
{
	ZShopEquipItem_Quest* m_pItem;
public:
	ZShopEquipItemHandle_SellQuest(ZShopEquipItem_Quest* pItem);
	virtual bool GetPrice(int& out_nPrice);
	virtual void Sell();

	virtual void OnDone(int nCount);
};


// �������� ����ֱ� ������ ���� �ڵ鷯 �������̽�
class IShopEquipItemHandle_SendAccount
{
	MUID m_uidItem;
public:
	IShopEquipItemHandle_SendAccount() :  m_uidItem(0,0) {}
	virtual ~IShopEquipItemHandle_SendAccount() {}
	void SetItemUID(const MUID& uid) { m_uidItem = uid; }
	MUID GetItemUID() { return m_uidItem; }

	void OpenCountableConfirmDlg(const char* szItemName, MBitmap* pIconBmp, int nPrice, int nMax, IItemCountDlgDoneHandler* pDoneHandler);

	virtual void Send() = 0;
};

class ZShopEquipItemHandle_SendAccountGamble : public IShopEquipItemHandle_SendAccount, public IItemCountDlgDoneHandler
{
	ZShopEquipItem_Gamble* m_pItem;
public:
	ZShopEquipItemHandle_SendAccountGamble(ZShopEquipItem_Gamble* pItem);

	virtual void Send();
	virtual void OnDone(int nCount);
};

class ZShopEquipItemHandle_SendAccountMatch : public IShopEquipItemHandle_SendAccount, public IItemCountDlgDoneHandler
{
	ZShopEquipItem_Match* m_pItem;
public:
	ZShopEquipItemHandle_SendAccountMatch(ZShopEquipItem_Match* pItem);

	virtual void Send();
	virtual void OnDone(int nCount);
};



// �������� ���ನ������ ������ ���� �ڵ鷯 �������̽�
class IShopEquipItemHandle_BringAccount
{
	int m_nAIID;
	ZMyItemNode* m_pMyItemNode;
public:
	IShopEquipItemHandle_BringAccount() : m_nAIID(0), m_pMyItemNode(NULL) {}
	virtual ~IShopEquipItemHandle_BringAccount() {}
	void SetAIID(int aiid) { m_nAIID = aiid; }
	int GetAIID() { return m_nAIID; }

	void SetMyItemNode(ZMyItemNode* pMyItemNode) { m_pMyItemNode = pMyItemNode; }
	ZMyItemNode* GetMyItemNode() { return m_pMyItemNode; }

	void OpenSimpleConfirmDlg(ISimpleConfirmDlgDoneHandler* pDoneHandler);
	void OpenCountableConfirmDlg(const char* szItemName, MBitmap* pIconBmp, int nPrice, int nMax, IItemCountDlgDoneHandler* pDoneHandler);

	virtual void Bring() = 0;

	// ���࿡ �ִ� �� ĳ��������, ĳ���������� ������ ǥ������ �ʴ´� GetPriceText() �ʿ����
};

class ZShopEquipItemHandle_BringAccountGamble : public IShopEquipItemHandle_BringAccount, public IItemCountDlgDoneHandler
{
	ZShopEquipItem_Gamble* m_pItem;
public:
	ZShopEquipItemHandle_BringAccountGamble(ZShopEquipItem_Gamble* pItem);
	
	virtual void Bring();

	virtual void OnDone(int nCount);
};

class ZShopEquipItemHandle_BringAccountMatch : public IShopEquipItemHandle_BringAccount, public ISimpleConfirmDlgDoneHandler, public IItemCountDlgDoneHandler
{
	ZShopEquipItem_Match* m_pItem;
	ZMyItemNode* m_pMyItemNode;
public:
	ZShopEquipItemHandle_BringAccountMatch(ZShopEquipItem_Match* pItem);
	ZMyItemNode* GetMyItemNode() { return m_pMyItemNode; }
	virtual void Bring();

	virtual void OnDone(bool bOk);
	virtual void OnDone( int nCount );
};



