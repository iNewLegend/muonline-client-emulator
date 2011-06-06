// ------------------------------
// Designed by RPGSky
// Date : 2010-07-30
// ------------------------------
// 
#pragma once
#include "UIControl.h"

class CUIDisplay: public CUIControl//, public CDisplay
{
public:
	CUIDisplay();
	virtual void	XMLParse(const TiXmlElement* pControlElement);
	virtual void	OnFrameRender(const Matrix& mTransform, double fTime, float fElapsedTime);
	CRect<int>		getViewport();
};
