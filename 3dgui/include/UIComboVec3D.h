// ------------------------------
// Designed by RPGSky
// Date : 2010-07-30
// ------------------------------
// 
#pragma once
#include "UIComboNumber.h"

class CUIComboVec3D : public CUICombo
{
public:
	CUIComboVec3D();
	~CUIComboVec3D();
public:
	virtual void XMLParse(const TiXmlElement* pControlElement);
	void setExtentSize(float fExtentSize);
	virtual void OnControlRegister();
	MY3DGUI_VEC3D	getVec3D();
	void	setVec3D(const MY3DGUI_VEC3D& vec);
	void	OnNumChanged();
private:
	CUIComboNumber	m_NumX;
	CUIComboNumber	m_NumY;
	CUIComboNumber	m_NumZ;
};