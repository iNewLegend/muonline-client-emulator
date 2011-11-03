// ------------------------------
// Designed by RPGSky
// Date : 2010-07-30
// ------------------------------
// 
#pragma once
#include "UIListBox.h"

class CUIComboBox : public CUICombo
{
public:
	CUIComboBox();
	virtual ~CUIComboBox();
	virtual void OnControlRegister();
	void OnSelectionChanged();

	virtual bool ContainsPoint(POINT pt); 

	virtual bool HandleKeyboard(UINT uMsg, WPARAM wParam, LPARAM lParam);
	// ¥¶¿Ì Û±Í
	virtual void OnMouseMove(POINT point);
	virtual void OnLButtonDown(POINT point);
	virtual void OnLButtonUp(POINT point);
	virtual void OnMouseWheel(POINT point,short wheelDelta);
	//
	virtual void OnHotkey();

	virtual bool CanHaveFocus() { return (m_bVisible && m_bEnabled); }
	virtual void OnFocusOut();
	virtual void OnFrameRender(const Matrix& mTransform, double fTime, float fElapsedTime);

	bool	AddItem(const wchar_t* wcsText, void *pData=NULL);
	//void    SetDropHeight(UINT nHeight) { m_nDropHeight = nHeight; UpdateRects(); }
	virtual const wchar_t* GetText() const;
	CUIListBox& getListBox();
protected:
	CUIListBox	m_ListBox;
};