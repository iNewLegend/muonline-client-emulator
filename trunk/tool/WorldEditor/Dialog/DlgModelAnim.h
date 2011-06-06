#pragma once
#include "UIDialog.h"

class CDlgModelAnim :public CUIDialog
{
public:
	virtual void OnControlRegister();
	virtual bool OnInitDialog();
	virtual void OnFrameMove(double fTime, float fElapsedTime);
	void ResetAnim();
	void OnAnimChanged();
	void OnSpeedChanged();
	void OnFrameChanged();
protected:
private:
	void OnPlay();
	void OnPrev();
	void OnNext();

	CUIComboBox m_ComboBoxAnim;
	CUIStatic	m_StaticSpeed;
	CUISlider	m_SliderSpeed;
	CUIStatic	m_StaticFrame;
	CUISlider	m_SliderFrame;
	CUIButton	m_BtnPlay;
	CUIButton	m_BtnPrev;
	CUIButton	m_BtnNext;
};