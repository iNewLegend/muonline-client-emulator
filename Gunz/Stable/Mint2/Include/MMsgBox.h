#ifndef _MMSGBOX_H
#define _MMSGBOX_H

#include "MFrame.h"
#include "MButton.h"
#include "MLabel.h"
#include "MTextArea.h"

enum MMsgBoxType{
	MT_OK,
	MT_CANCEL,
	MT_OKCANCEL,
	MT_YESNO,

	MT_NOTDECIDED,	// �̸� �������� �ʱ� ����
};

class MMsgBox : public MFrame{
protected:
//	MLabel*		m_pMessage;
	MTextArea*	m_pMessage;
	MButton*	m_pOK;
	MButton*	m_pCancel;
	MMsgBoxType	m_nType;
	//char		m_szMessage[256];

protected:
	/*
	virtual void OnDrawText(MDrawContext* pDC);
	virtual void OnDraw(MDrawContext* pDC);
	*/
	virtual bool OnShow(void);
	virtual void OnSize(int w, int h);

	virtual bool OnCommand(MWidget* pWindow, const char* szMessage);

public:
	MMsgBox(const char* szMessage, MWidget* pParent, MListener* pListener=NULL, MMsgBoxType nType=MT_NOTDECIDED );
	virtual ~MMsgBox(void);

	void SetType(MMsgBoxType nType);
	void SetTitle(const char* szTitle);
	const char* GetTitle(void);
	void SetMessage(const char* szMessage);
	const char* GetMessage(void);
	virtual void SetText(const char* szText);
	virtual const char* GetText(void);

	virtual void MultiplySize(float byIDLWidth, float byIDLHeight, float byCurrWidth, float byCurrHeight);
	void AdjustSize();

#define MINT_MSGBOX	"MsgBox"
	virtual const char* GetClassName(void){ return MINT_MSGBOX; }
};

// Listener���� ���޵Ǵ� �޼���
#define MMSGBOX_OK		"OK"
#define MMSGBOX_CANCEL	"Cancel"
#define MMSGBOX_YES		"Yes"
#define MMSGBOX_NO		"No"

#endif
