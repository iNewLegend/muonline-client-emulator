// ------------------------------
// Designed by RPGSky
// Date : 2010-07-30
// ------------------------------
// 
#pragma once

#include "UIControl.h"

//-----------------------------------------------------------------------------
// Í¼Æ¬¿Ø¼þ
//-----------------------------------------------------------------------------
class CUIImage : public CUIControl
{
public:
	CUIImage();
	virtual void XMLParse(const TiXmlElement* pControlElement);
	virtual void OnFrameRender(const Matrix& mTransform, double fTime, float fElapsedTime);

	std::string& GetFilename() { return m_strFilename; }
	void SetFilename(const char* szFilename);
protected:
	RECT m_rcTex;
	std::string m_strFilename;
};
