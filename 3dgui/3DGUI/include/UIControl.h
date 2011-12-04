// ------------------------------
// Designed by RPGSky
// Date : 2010-07-30
// ------------------------------
// 
#pragma once
#include "UIGraph.h"
#include <usp10.h>
#include <dimm.h>
#include <winuser.h>
#include <vector>

class CUICombo;

HWND UIGetHWND();
void UISetHWND(HWND hWnd);

#define UITRACE	(__noop)

#ifndef SPI_GETWHEELSCROLLLINES
#define SPI_GETWHEELSCROLLLINES    0x0068
#endif

#ifndef WM_MOUSEWHEEL
#define WM_MOUSEWHEEL 0x020A // (not always defined)
#endif
#ifndef WHEEL_DELTA
#define WHEEL_DELTA 120 // (not always defined)
#endif

// Defines and macros 
#define EVENT_DEFAULT						"0"
#define EVENT_BUTTON_CLICKED				"0"
#define EVENT_BUTTON_DBLCLK				    "_DBLCLK"

#define EVENT_RADIOBUTTON_CHANGED			"0"
#define EVENT_CHECKBOX_CHANGED				"0"
#define EVENT_LISTBOX_SELECTION_END         "0"
#define EVENT_SLIDER_VALUE_CHANGED          "0"
#define EVENT_COMBOBOX_SELECTION_CHANGED    "0"


#define EVENT_EDITBOX_STRING                "_STRING"
// EVENT_EDITBOX_CHANGE is sent when the listbox content changes
// due to user input.
#define EVENT_EDITBOX_CHANGE                "_CHANGE"
#define EVENT_LISTBOX_ITEM_DBLCLK           "_DBLCLK"
// EVENT_LISTBOX_SELECTION is fired off when the selection changes in
// a single selection list box.
#define EVENT_LISTBOX_SELECTION             "_SELECTION"

#define EVENT_SETFOCUS						"_SETFOCUS"
#define EVENT_KILLFOCUS						"_KILLFOCUS"
#define EVENT_VISIBLE						"_VISIBLE"
// Enums for pre-defined control types

enum UI_CONTROL_TYPE 
{
	UI_CONTROL_DIALOG,
	UI_CONTROL_COMBO,		
	UI_CONTROL_BUTTON,
	UI_CONTROL_STATIC,
	UI_CONTROL_IMAGE,
	UI_CONTROL_CHECKBOX,
	UI_CONTROL_RADIOBUTTON,
	UI_CONTROL_COMBOBOX,
	UI_CONTROL_SLIDER,
	UI_CONTROL_PROGRESS,
	UI_CONTROL_EDITBOX,
	UI_CONTROL_IMEEDITBOX,
	UI_CONTROL_LISTBOX,
	UI_CONTROL_GRID,
	UI_CONTROL_SCROLLBAR,
	UI_CONTROL_DISPLAY,
	UI_CONTROL_MAX,
};

#define MAX_CONTROL_STATES 6

class TiXmlElement;
class CUIControl
{
public:
	static CUIStyle s_TipStyle;
	CUIControl();
	virtual ~CUIControl();

	virtual void SetParent(CUICombo *pControl);
	virtual CUICombo* GetParentDialog() { return m_pParentDialog; }

	virtual bool isCombo(){return false;}

	virtual void XMLParse(const TiXmlElement* pControlElement);
	virtual void SetText(const wchar_t* wcsText){}
	virtual void SetInt(int val);
	virtual void SetFloat(float val,const int slen,const int alen);
	virtual const wchar_t* GetText()const{return NULL;}
	virtual int GetInt();
	virtual float GetFloat();


	virtual void OnInit() { }
	virtual void Refresh();

	virtual void OnFrameMove(double fTime, float fElapsedTime) {}
	virtual void OnFrameRender(const Matrix& mTransform, double fTime, float fElapsedTime);

	virtual void updateUIMatrix(const Matrix& mTransform, double fTime, float fElapsedTime);

	// Windows message handler
	virtual bool MsgProc(UINT uMsg, WPARAM wParam, LPARAM lParam) { return false; }

	virtual bool HandleKeyboard(UINT uMsg, WPARAM wParam, LPARAM lParam) { return false; }
	//响应鼠标的事件
	//除了MouseMove、MouseWheel其他都得通过ContainsPoint的条件才能响应
	virtual void OnMouseMove(POINT point){;}
	virtual void OnMouseWheel(POINT point,short wheelDelta){;}
	virtual void OnLButtonDblClk(POINT point){;}
	virtual void OnLButtonDown(POINT point){;}
	virtual void OnLButtonUp(POINT point){;}
	virtual void OnRButtonDblClk(POINT point){;}
	virtual void OnRButtonDown(POINT point){;}
	virtual void OnRButtonUp(POINT point){;}
	virtual void OnMButtonDblClk(POINT point){;}
	virtual void OnMButtonDown(POINT point){;}
	virtual void OnMButtonUp(POINT point){;}

	virtual bool CanHaveFocus() { return false; }
	virtual void SetFocus(bool bFocus=true);
	virtual void OnFocusIn() { sendEvent(EVENT_SETFOCUS);}
	virtual void OnFocusOut() { sendEvent(EVENT_KILLFOCUS); }
	virtual void OnMouseEnter() { m_bMouseOver = true; }
	virtual void OnMouseLeave() { m_bMouseOver = false; }
	virtual void OnHotkey() {}
	virtual void sendEvent(const char* szEvent);

	virtual bool ContainsPoint(POINT pt);

	virtual void SetEnabled(bool bEnabled) { m_bEnabled = bEnabled; }
	virtual bool GetEnabled() { return m_bEnabled; }
	virtual void SetVisible(bool bVisible) { m_bVisible = bVisible; sendEvent(EVENT_VISIBLE);}
	virtual bool IsVisible() { return m_bVisible; }
	virtual bool isStyleVisible(){return m_Style.isVisible();}

	virtual void SetTip(const std::wstring& wstrTip) { m_wstrTip = wstrTip; }
	virtual void drawTip(const Matrix& mTransform,const CRect<int>& rc, double fTime, float fElapsedTime);
	
	virtual CUIStyle& getStyle() {return m_Style;}
	virtual void SetStyle(const std::string& strStyleName);

	UI_CONTROL_TYPE GetType() const { return m_Type; }

	const std::string&  GetID()const{ return m_strID; }
	void SetID(const char* szID){m_strID = szID;}

	virtual void OnMove(int x, int y);
	virtual void OnSize(const CRect<int>& rc);

	const CRect<int>& getOffset();
	const CRect<int>& getScale();
	void setOffset(const CRect<int>& rc);
	void setScale(const CRect<int>& rc);

	void SetHotkey(std::string& strHotkey);
	void SetHotkey(UINT nHotkey) { m_nHotkey = nHotkey; }
	UINT GetHotkey() { return m_nHotkey; }

	void SetUserData(void *pUserData) { m_pUserData = pUserData; }
	void *GetUserData() const { return m_pUserData; }

	void SetDefault(bool bIsDefault) { m_bIsDefault = bIsDefault; }
	bool IsDefault() const { return m_bIsDefault; }

	CRect<int>& GetBoundingBox(){return m_rcBoundingBox;}

	virtual bool IsPressed(){return this==s_pControlPressed;}
	void SetPressed(bool bPressed);
	virtual bool IsFocus(){return this==s_pControlFocus;}
	virtual bool IsMouseOver(){return this==s_pControlMouseOver;}
	virtual bool IsEvent(){return this==s_pControlEvent;}
	static void ClearFocus();
	static void clearFocus();

	static CUIControl* s_pControlFocus;        // The control which has focus
	static CUIControl* s_pControlPressed;      // The control currently pressed
	static CUIControl* s_pControlMouseOver;    // The control which is hovered over
	static CUIControl* s_pControlEvent;			// The control which is event

	static CUIControl* getControlFocus()	{return s_pControlFocus;}
	static CUIControl* getControlPressed()	{return s_pControlPressed;}
	static CUIControl* getControlMouseOver(){return s_pControlMouseOver;}
	static CUIControl* getControlEvent()	{return s_pControlEvent;}
	virtual void ClientToScreen(RECT& rc);
	void screenToClient(POINT& pt);
	CRect<int> screenToClient(const CRect<int>& rc);
	virtual void UpdateRects();
protected:
	virtual CONTROL_STATE GetState();
protected:
	bool m_bVisible;                // Shown/hidden flag
	bool m_bMouseOver;              // Mouse pointer is above control
	bool m_bIsDefault;              // Is the default control

	CUIStyle m_Style;

	CUICombo*			m_pParentDialog;    // Parent container

	std::string			m_strID;			// ID string
	std::wstring		m_wstrTip;
	UI_CONTROL_TYPE		m_Type;				// Control type, set once in constructor  
	UINT				m_nHotkey;			// Virtual key code for this control's hotkey
	void				*m_pUserData;		// Data associated with this control that is set by user.

	bool				m_bEnabled;			// Enabled/disabled flag

	CRect<int>			m_rcBoundingBox;	// Rectangle defining the active region of the control
	CRect<int>			m_rcRelativeBox;	// Rectangle defining the active region of the control

	CRect<int>			m_rcScale;
	CRect<int>			m_rcOffset;

	float				m_setBlendRate[CONTROL_STATE_MAX];
	MY3DGUI_VEC3D		m_setTranslation[CONTROL_STATE_MAX];
	MY3DGUI_VEC3D		m_setRotate[CONTROL_STATE_MAX];
	float				m_fRate;
	MY3DGUI_VEC3D		m_vTranslation;
	MY3DGUI_VEC3D		m_vRotate;
	Matrix				m_mWorld;

};