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
	DWORD GetColor(){ return m_dwColor; }

	void SetFilename(const char* szFilename);
	void SetColor(DWORD dwColor){ m_dwColor = dwColor; }


protected:
	RECT m_rcTex;
	DWORD m_dwColor;
	std::string m_strFilename;
	void* m_pFilename; 
};
