#pragma once

#include "MMsgBox.h"

class ZShopEquipInterface;
class ZShopEquipListItem;


////////////////////////////////////////////////////////////////////////////////////
//		������ ���� �Է� ��ȭ����
////////////////////////////////////////////////////////////////////////////////////

struct IItemCountDlgDoneHandler
{
	virtual void OnDone(int nCount) = 0;
};

// ZItemCountDlg�� ��� �뵵
enum ZITEMCOUNTDLG_MODE {
	ZICD_SELL,			// �Ǹ�
	ZICD_BUY,			// ����
	ZICD_SENDACCOUNT,	// �߾����࿡ ������
	ZICD_BRINGACCOUNT,	// �߾����࿡�� ��������
};

// ����,�Ǹ�,�����ⳳ �������� �������� ������ �Է��� �� �ִ� ��ȭ����
class ZItemCountDlg
{
	ZITEMCOUNTDLG_MODE	m_mode;
	string		m_strItemName;
	MBitmap*	m_pItemIcon;
	int			m_nPrice;
	int			m_nMin, m_nMax;
	IItemCountDlgDoneHandler* m_pDoneHandler;

	int			m_nCurrCount;

public:
	void Open(ZITEMCOUNTDLG_MODE mode, const char* szItemName, MBitmap* pIcon, int price, int nMin, int nMax, IItemCountDlgDoneHandler* pHandler);
	void Close();
	void UpdateDlg();
	void AddCount(int n);
	void OnEditBoxChanged();
	void OnDlgDone();
};



////////////////////////////////////////////////////////////////////////////////////
//		������ Ȯ�ο� ��ȭ���� ( �޽����ڽ� )
////////////////////////////////////////////////////////////////////////////////////

///// ����,�Ǹ� Ȯ�� �޽��� �ڽ��� �Ϸ� �ڵ鷯
struct ISimpleConfirmDlgDoneHandler
{
	virtual void OnDone(bool bOk) = 0;
};

// ����,�ǸŽ� �����ϰ� ���� Ȯ���� �޴� �޽����ڽ�
class ZSimpleConfirmDlg : public MListener
{
	MMsgBox* m_pMsgbox;
	ISimpleConfirmDlgDoneHandler* m_pDoneHandler;

public:
	ZSimpleConfirmDlg();
	~ZSimpleConfirmDlg();

	virtual bool OnCommand(MWidget* pWidget, const char* szMessage);

	void Open(const char* szMsg, ISimpleConfirmDlgDoneHandler* pHandler);
};



////////////////////////////////////////////////////////////////////////////////////
//		ĳ�� ������ �ȱ� Ȯ�ο� ��ȭ����
////////////////////////////////////////////////////////////////////////////////////

///// Ȯ�� ��ȭ������ �Ϸ� �ڵ鷯
struct ISellCashItemConfirmDlgDoneHandler
{
	virtual void OnConfirmSellCashItem(bool bOk) = 0;
};

// ĳ�� �������� �ٿ�Ƽ�� �ް� �Ǹ��� ���� ��� ��ȭ����
class ZSellCashItemConfirmDlg
{
	ISellCashItemConfirmDlgDoneHandler* m_pDoneHandler;
	DWORD m_nWaitActivatingOkBtnBeginTime;
	bool m_bWaitActivatingOkBtn;

public:
	ZSellCashItemConfirmDlg();
	~ZSellCashItemConfirmDlg();

	void Open(const char* szItemName, MBitmap* pIcon, int price, int count, ISellCashItemConfirmDlgDoneHandler* pHandler);
	void Update();

	void Close();
	void OnOkButton();
};



////////////////////////////////////////////////////////////////////////////////////
//		ĳ�� ������ ���� Ȯ�ο� ��ȭ���� (�Ⱓ�̳� ���� ���� ����)
////////////////////////////////////////////////////////////////////////////////////

// �Ϸ� �ڵ鷯
struct ICashItemConfirmDlgDoneHandler
{
	virtual void OnDone(bool bOk) = 0;
};

class ZCashItemConfirmDlg
{
public:
	ZCashItemConfirmDlg();
	~ZCashItemConfirmDlg();

	void Open(MBitmap* pItemIcon, ICashItemConfirmDlgDoneHandler* pHandler);
};