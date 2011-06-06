#pragma once
#include "DlgModelAnim.h"
#include "DlgModelPlayer.h"

class CDlgModelController :public CUIDialog
{
public:
	CDlgModelController();
	~CDlgModelController();
public:
	virtual void OnControlRegister();
	virtual bool OnInitDialog();
	void OnUpdate();
protected:
	void OnTabChanged();
	void OnClose();
private:
	CUIRadioButton m_RadioButtonAnim;
	CDlgModelAnim		m_DlgModelAnim;
};