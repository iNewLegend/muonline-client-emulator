#pragma once
#include "ZShopEquipItemConfirm.h"


class ZShopEquipListItem;

void WidgetEnableShow(const char* szWidget, bool bEnable, bool bShow);


// ���� ZGameInterface�� �ִ� ������ ���â�� ������ �ڵ带 ����� �ű�ϴ�

class ZShopEquipInterface
{
	int					m_nShopTabNum;
	int					m_nEquipTabNum;

	int					m_nSellQuestItemCount;
	int					m_nSellSpendableItemCount;
	int					m_nBuySpendableItemCount;

	ZItemCountDlg*				m_pItemCountDlg;
	ZSimpleConfirmDlg*			m_pSimpleConfirmDlg;
	ZSellCashItemConfirmDlg*	m_pSellCashItemConfirmDlg;

public:
	ZShopEquipInterface();
	~ZShopEquipInterface();

	void ConfirmBuySimple();

	ZShopEquipListItem* GetListCurSelItem(const char* szListWidget);

	ZItemCountDlg* GetItemCountDlg() { return m_pItemCountDlg; }
	ZSimpleConfirmDlg* GetSimpleConfirmDlg() { return m_pSimpleConfirmDlg; }
	ZSellCashItemConfirmDlg* GetSellCashItemConfirmDlg() { return m_pSellCashItemConfirmDlg; }

	void OnSellButton(void);

	void OnBuyButton(void);

	MMatchCharItemParts RecommendEquipParts(MMatchItemSlotType slot);

	bool Equip(void);
	bool Equip(MMatchCharItemParts parts, MUID& uidItem);

	void SetKindableItem( MMatchItemSlotType nSlotType);
	bool IsKindableItem(MMatchCharItemParts nParts, MMatchItemSlotType nSlotType);

	int _CheckRestrictBringAccountItem();
	bool CheckRestrictBringAccountItem();

	void OnSendAccountButton();
	void OnBringAccountButton();

	void SelectShopTab(int nTabIndex);
	void SelectEquipmentTab(int nTabIndex);

	void Update();

	// ����/�����â���� ������ �� ���� ��� ��Ÿ���� ������
public:
	void SelectEquipmentFrameList( const char* szName, bool bOpen);
	int GetArmorWeaponTabIndexContainItemParts(MMatchCharItemParts parts);
	bool IsEquipmentFrameListOpened( const char* szName="Shop");

	// �� ���� ��� ����� �� �ε��� (���� ����Ʈ ��/�� ����Ʈ ��)
private:
	int m_idxArmorWeaponTab;
public:
	void SetArmorWeaponTabIndex(int idx) { if(idx < 0 || idx > 2) return; else m_idxArmorWeaponTab = idx;}
	int GetArmorWeaponTabIndex()	{ return m_idxArmorWeaponTab; }
	void SelectArmorWeaponTabWithSlotType(MMatchItemSlotType slotType);	// ������ ����Ÿ�Կ� �´� ���� ���� ���·�
	void OnArmorWeaponTabButtonClicked(int nTab);

public:
	void ShowItemDescription(bool bShow, MTextArea* pTextArea, void* pCaller);
	
	// ����/���â���� ĳ������ �⺻ ������, Ư�� ��� ��ü���� ���� ���� ��ȭ�� �ؽ�Ʈ�� ǥ���Ѵ�	
	void DrawCharInfoText();
	void DrawCharInfoText(char* szShopOrEquip, int nReqLevel, int nNewWT, int nNewMaxWT, int nNewHP, int nNewAP, int nReqBounty, int nReqCash);
private:
	// ������ �������⼭ ����ϱ� ������ ���� ������� ������ �ڴ�� hide���� ���ϵ��� �ϱ� ���ؼ� ���� ����ڸ� �����
	void* m_pItemDescriptionClient;
};
