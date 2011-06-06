#include "DlgModelAnim.h"
#include "ModelDisplay.h"
#include "..\MainRoot.h"

void CDlgModelAnim::OnControlRegister()
{
	CUIDialog::OnControlRegister();

	RegisterControl("IDC_COMBOBOX_ANIM",m_ComboBoxAnim);
	RegisterControl("IDC_STATIC_SPEED",	m_StaticSpeed);
	RegisterControl("IDC_SLIDER_SPEED",	m_SliderSpeed);
	RegisterControl("IDC_STATIC_FRAME",	m_StaticFrame);
	RegisterControl("IDC_SLIDER_FRAME",	m_SliderFrame);
	RegisterControl("IDC_BTN_PLAY",		m_BtnPlay);
	RegisterControl("IDC_BTN_PREV",		m_BtnPrev);
	RegisterControl("IDC_BTN_NEXT",		m_BtnNext);

	RegisterControlEvent("IDC_COMBOBOX_ANIM",	(PEVENT)&CDlgModelAnim::OnAnimChanged);
	RegisterControlEvent("IDC_SLIDER_SPEED",	(PEVENT)&CDlgModelAnim::OnSpeedChanged);
	RegisterControlEvent("IDC_SLIDER_FRAME",	(PEVENT)&CDlgModelAnim::OnFrameChanged);

	RegisterControlEvent("IDC_BTN_PLAY",	(PEVENT)&CDlgModelAnim::OnPlay);
	RegisterControlEvent("IDC_BTN_PREV",	(PEVENT)&CDlgModelAnim::OnPrev);
	RegisterControlEvent("IDC_BTN_NEXT",	(PEVENT)&CDlgModelAnim::OnNext);
}

bool CDlgModelAnim::OnInitDialog()
{
	m_SliderSpeed.SetRange(1, 50);
	m_SliderSpeed.SetValue(10);
	//OnUpdateShow();
	return true;
}

void CDlgModelAnim::OnFrameMove(double fTime, float fElapsedTime)
{
	/*if (CMainRoot::getInstance().getMainDialog().getModelDisplay().getModelObject())
	{
		m_SliderFrame.SetValue(CMainRoot::getInstance().getMainDialog().getModelDisplay().getModelObject()->m_AnimMgr.uFrame);
		std::wstring wstr = FormatW(L"%d/%d", m_SliderFrame.GetValue(),m_SliderFrame.getMax());
		m_StaticFrame.SetText(wstr.c_str());
	}*/
	CUIDialog::OnFrameMove(fTime,fElapsedTime);
}

void CDlgModelAnim::ResetAnim()
{
	/*int nSelected = m_ComboBoxAnim.getListBox().GetSelectedIndex();
	m_ComboBoxAnim.getListBox().RemoveAllItems();
	if (CMainRoot::getInstance().getMainDialog().getModelDisplay().getModelData())
	{
		iSkeleton& skeleton=CMainRoot::getInstance().getMainDialog().getModelDisplay().getModelData()->getSkeleton();
		size_t uAnimSize = skeleton.getAnimationCount();
		if (uAnimSize>0)
		{
			for (size_t i=0; i<uAnimSize; ++i)
			{
				std::string strAnimName;
				long timeCount;
				skeleton.getAnimation(i,strAnimName,timeCount);
				m_ComboBoxAnim.AddItem(s2ws(strAnimName).c_str());
			}
			if (nSelected>=uAnimSize)
			{
				nSelected = uAnimSize-1;
			}
			m_ComboBoxAnim.getListBox().SelectItem(nSelected);
		}
		OnSpeedChanged();
	}
	//OnUpdateShow();*/
}

void CDlgModelAnim::OnPlay()
{
	//if (CMainRoot::getInstance().getMainDialog().getModelDisplay().getModelAnimManager())
	//{
	//	CMainRoot::getInstance().getMainDialog().getModelDisplay().getModelAnimManager()->Pause();
	//}
}

void CDlgModelAnim::OnPrev()
{
	//if (CMainRoot::getInstance().getMainDialog().getModelDisplay().getModelAnimManager())
	//{
	//	CMainRoot::getInstance().getMainDialog().getModelDisplay().getModelAnimManager()->PrevFrame();
	//}
}

void CDlgModelAnim::OnNext()
{
	//if (CMainRoot::getInstance().getMainDialog().getModelDisplay().getModelAnimManager())
	//{
	//	CMainRoot::getInstance().getMainDialog().getModelDisplay().getModelAnimManager()->NextFrame();
	//}
}

void CDlgModelAnim::OnAnimChanged()
{
	/*if (CMainRoot::getInstance().getMainDialog().getModelDisplay().getModelData())
	{
		std::string strAnimName=ws2s(m_ComboBoxAnim.GetText());
		long timeCount=0;
		CMainRoot::getInstance().getMainDialog().getModelDisplay().getModelObject()->setAnim(strAnimName.c_str());
		if (CMainRoot::getInstance().getMainDialog().getModelDisplay().getModelData()->getSkeleton().getAnimation(strAnimName,timeCount))
		{
			m_SliderFrame.SetRange(0, timeCount);
		}
	}*/
}

void CDlgModelAnim::OnSpeedChanged()
{
	/*float fSpeed = m_SliderSpeed.GetValue()*0.1f;
	if (CMainRoot::getInstance().getMainDialog().getModelDisplay().getModelObject())
	{
		CMainRoot::getInstance().getMainDialog().getModelDisplay().getModelObject()->m_AnimMgr.fSpeed=fSpeed;
	}
	m_StaticSpeed.SetFloat(fSpeed,0,1);*/
}

void CDlgModelAnim::OnFrameChanged()
{
	/*if (CMainRoot::getInstance().getMainDialog().getModelDisplay().getModelObject())
	{
		int nFrame = m_SliderFrame.GetValue();
		CMainRoot::getInstance().getMainDialog().getModelDisplay().getModelObject()->m_AnimMgr.uFrame=nFrame;
	}*/
}