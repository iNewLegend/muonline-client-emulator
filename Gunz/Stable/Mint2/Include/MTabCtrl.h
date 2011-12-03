#ifndef _MTABCTRL_H
#define _MTABCTRL_H
//#pragma once

#include <list>
using namespace std;

#include "MWidget.h"
#include "MLookNFeel.h"


class MTabCtrl;
class MDrawContext;
class MButton;


/// MTabCtrl�� Draw �ڵ尡 �ִ� Ŭ����, �� Ŭ������ ��ӹ޾Ƽ� Ŀ���ҷ��� ���� �� �ִ�.
class MTabCtrlLook{
public:
	virtual void OnDraw(MTabCtrl* pTabCtrl, MDrawContext* pDC);
	virtual MRECT GetClientRect(MTabCtrl* pTabCtrl, MRECT& r);
};


//// MTabItem
class MTabItem {
public:
	MWidget	*pFrame;
	MButton *pButton;
	MListener *pButtonListener;		// ��ư�� ���� ������
	MTabItem(MWidget *pFrame,MButton *pButton,MListener *pListener);
};

class MTabList : public list<MTabItem*> {};


//// MTabCtrl
class MTabCtrl : public MWidget {
protected:
	int			m_nCurrentSel;
	MTabList	m_TabList;

public:
	MTabCtrl(const char* szName, MWidget* pParent=NULL, MListener* pListener=NULL);
	MTabCtrl(MWidget* pParent=NULL, MListener* pListener=NULL);
	virtual ~MTabCtrl(void);
		
	void Add(MButton *pButton, MWidget *pFrame);		// pButton �� ������ pFrame �� ��������
	void RemoveAll();

	int GetCount(void);
	int GetSelIndex(void);
	bool SetSelIndex(int i);

	void UpdateListeners();			// ���� ��ư�� �����ʰ� �ٲ�ٸ� ������� �ҷ��־�� �Ѵ�.

	virtual bool OnCommand(MWidget* pWidget, const char* szMessage);
public:
	DECLARE_LOOK(MTabCtrlLook)
	DECLARE_LOOK_CLIENT()

	#define MINT_TABCTRL	"TabCtrl"
	virtual const char* GetClassName(void){ return MINT_TABCTRL; }

public:
	
};

#endif