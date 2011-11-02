// ------------------------------
// Designed by RPGSky
// Date : 2010-07-30
// ------------------------------
// 
#pragma once
#include "UIControl.h"
#include "UIScrollBar.h"

//-----------------------------------------------------------------------------
// ListBox control
//-----------------------------------------------------------------------------
class CUIDialog;
struct UIRollupPage
{
	CUIDialog* pDialog;
	CUIButton* pButton;
	std::wstring wstrName;
	void*  pData;

	RECT  rcActive;
	bool  bSelected;
};

class CUIRollup : public CUIControl
{
public:
	CUIRollup();
	virtual ~CUIRollup();
	virtual void	SetParent(CUICombo *pControl);
	virtual void	XMLParse(const TiXmlElement* pControlElement);
	virtual bool    HandleKeyboard(UINT uMsg, WPARAM wParam, LPARAM lParam);
	virtual bool    HandleMouse(UINT uMsg, POINT pt, WPARAM wParam, LPARAM lParam);

	virtual void    OnFrameRender(const Matrix& mTransform, double fTime, float fElapsedTime);
	virtual void    UpdateRects();

	DWORD	GetListBoxStyle() const { return m_dwStyle; }
	int		GetSize() const { return m_Pages.size(); }
	void	SetListBoxStyle(DWORD dwStyle) { m_dwStyle = dwStyle; }
	int		GetScrollBarWidth() const { return m_nSBWidth; }
	void	SetScrollBarWidth(int nWidth) { m_nSBWidth = nWidth; UpdateRects(); }
	void	SetBorder(int nBorder, int nMargin) { m_nBorder = nBorder; m_nMargin = nMargin; }
	HRESULT	AddPage(const WCHAR *wszText, CUIDialog* pDialog, void *pData);
	HRESULT	InsertPage(int nIndex, const WCHAR *wszText, CUIDialog* pDialog, void *pData);
	void	RemovePage(int nIndex);
	void	RemovePageByText(WCHAR *wszText);
	void	RemovePageByData(void *pData);
	void	RemoveAllPages();

	UIRollupPage*	GetPage(int nIndex);
	int				GetSelectedIndex(int nPreviousSelected = -1);
	UIRollupPage*	GetSelectedPage(int nPreviousSelected = -1) { return GetPage(GetSelectedIndex(nPreviousSelected)); }
	void			SelectPage(int nNewIndex);

	CUIScrollBar&	GetScrollBar(){return m_ScrollBar;}
	enum STYLE { MULTISELECTION = 1 };

protected:
	CRect<int>	m_rcText;      // Text rendering bound
	CRect<int>	m_rcSelection; // Selection box bound
	CUIScrollBar	m_ScrollBar;
	int		m_nSBWidth;
	int		m_nBorder;
	int		m_nMargin;
	int		m_nTextHeight;  // Height of a single line of text
	DWORD	m_dwStyle;    // List box style
	int		m_nSelected;    // Index of the selected Page for single selection list box
	int		m_nSelStart;    // Index of the Page where selection starts (for handling multi-selection)
	bool	m_bDrag;       // Whether the user is dragging the mouse to select

	std::vector<UIRollupPage*> m_Pages;
};