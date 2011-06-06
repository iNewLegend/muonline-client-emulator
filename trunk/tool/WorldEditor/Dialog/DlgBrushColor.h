#pragma once
#include "DlgBrush.h"

class CDlgBrushColor : public CDlgBrush
{
public:
	CDlgBrushColor();
	~CDlgBrushColor();
public:
	virtual void OnControlRegister();
	virtual bool OnInitDialog();
	virtual void SetVisible(bool bVisible);

	void OnColorPaintChanged();
private:
	CUIComboColor	m_ColorPaint;
};