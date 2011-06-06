#pragma once
#include "Manager.h"
#include "UIDialog.h"
#include "UIEditBox.h"

class CDlgRegister : public CUIDialog
{
public:
	CDlgRegister();
	~CDlgRegister();
public:
	virtual bool MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
	virtual void OnControlRegister();
	virtual bool OnInitDialog();
	void OnBtnOK();
	void OnBtnCancel();
protected:
private:
	CUIEditBox		m_EditBoxEmail;
	CUIEditBox		m_EditBoxHardwareID;
	CUIEditBox		m_EditBoxKey;
};