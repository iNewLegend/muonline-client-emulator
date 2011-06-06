#include "DlgModelController.h"

CDlgModelController::CDlgModelController()
{
}

CDlgModelController::~CDlgModelController()
{	
}

void CDlgModelController::OnControlRegister()
{
	CUIDialog::OnControlRegister();
	RegisterControl("IDC_TAB_ANIM",	m_RadioButtonAnim);

	m_DlgModelAnim.Create("IDD_MODEL_ANIM", this);

	RegisterControlEvent("IDC_TAB_ANIM",		(PEVENT)&CDlgModelController::OnTabChanged);

	RegisterControlEvent("IDC_BTN_CLOSE",		(PEVENT)&CDlgModelController::OnClose);
}

bool CDlgModelController::OnInitDialog()
{
	OnTabChanged();
	return true;
}

void CDlgModelController::OnUpdate()
{
	m_DlgModelAnim.ResetAnim();
}

void CDlgModelController::OnTabChanged()
{
	m_DlgModelAnim.SetVisible(m_RadioButtonAnim.IsChecked());
}

void CDlgModelController::OnClose()
{
	SetVisible(false);
}