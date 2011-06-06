// ------------------------------
// Designed by RPGSky
// Date : 2010-07-30
// ------------------------------
// 
#pragma once

#include "UIControl.h"

#define BEGIN_CONTROL_REGISTER(_class)\
{\
	typedef _class ThisClass;

#define ON_BTN_CLICKED(_id, _fun)\
	RegisterControlEvent(#_id, (PEVENT)&ThisClass::_fun);

#define END_CONTROL_REGISTER()\
}

void UISetDialogRes(const char* szDialogRes);
const std::string& UIGetDialogRes();

class CUIButton;
class CUIStatic;
class CUIImage;
class CUICheckBox;
class CUIRadioButton;
class CUIComboBox;
class CUISlider;
class CUIEditBox;
class CUIIMEEditBox;
class CUIListBox;
class CUIScrollBar;

class CUICombo;
typedef void (__thiscall CUICombo::*PEVENT)(void);

typedef struct lua_State lua_State;

class CUICombo : public CUIControl
{
public:
	CUICombo();
	~CUICombo();
public:
	virtual bool Create(const char* szID, CUICombo* pParent=NULL);
	virtual bool create(const TiXmlElement* pRootElement);
	bool LoadXml(const char* szFilename, const char* szDialog);
	//bool loadFromMemory(const unsigned char* pBufferSize,  const char* szDialog);

	virtual bool isCombo(){return true;}
	virtual void loadString(const char* szFilename);
	virtual void XMLParse(const TiXmlElement* pControlElement);
	virtual void SetStyle(const std::string& strStyleName);
	virtual void OnChildSize(const CRect<int>& rc);
	virtual void OnSize(const CRect<int>& rc);
	virtual void UpdateRects();
	void XMLParseControls(const TiXmlElement* pElement);

	// my message
	virtual bool postMsg(const std::string& strMsg);

	// Windows消息处理
	virtual bool postMsg(WPARAM wParam, LPARAM lParam);
	virtual bool MsgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

	// Methods called by controls
	bool progressEvent(const std::string& strMsg);

	// Control creation
	void AddControl(CUIControl* pControl);
	void RegisterControl(const char* szID, CUIControl& control);
	void LuaRegisterControl(const char* szID, CUIControl* pControl);

	// Control retrieval
	CUIStatic*      GetStatic(const char* szID) { return (CUIStatic*) GetControl(szID, UI_CONTROL_STATIC); }
	CUIImage*		GetImage(const char* szID) { return (CUIImage*) GetControl(szID, UI_CONTROL_IMAGE); }
	CUIButton*      GetButton(const char* szID) { return (CUIButton*) GetControl(szID, UI_CONTROL_BUTTON); }
	CUICheckBox*    GetCheckBox(const char* szID) { return (CUICheckBox*) GetControl(szID, UI_CONTROL_CHECKBOX); }
	CUIRadioButton* GetRadioButton(const char* szID) { return (CUIRadioButton*) GetControl(szID, UI_CONTROL_RADIOBUTTON); }
	CUIComboBox*    GetComboBox(const char* szID) { return (CUIComboBox*) GetControl(szID, UI_CONTROL_COMBOBOX); }
	CUISlider*      GetSlider(const char* szID) { return (CUISlider*) GetControl(szID, UI_CONTROL_SLIDER); }
	CUIEditBox*     GetEditBox(const char* szID) { return (CUIEditBox*) GetControl(szID, UI_CONTROL_EDITBOX); }
	CUIIMEEditBox*  GetIMEEditBox(const char* szID) { return (CUIIMEEditBox*) GetControl(szID, UI_CONTROL_IMEEDITBOX); }
	CUIListBox*     GetListBox(const char* szID) { return (CUIListBox*) GetControl(szID, UI_CONTROL_LISTBOX); }

	CUIControl* getControl(const char* szID);
	CUIControl* GetControl(const char* szID, unsigned int nControlType);

	CUIControl* GetControlAtPoint(POINT pt);

	bool GetControlEnabled(const char* szID);
	void SetControlEnabled(const char* szID, bool bEnabled);

	bool isControlVisible(const char* szID);
	void setControlVisible(const char* szID, bool bEnabled);

	virtual void SetVisible(bool bVisible);

	void ClearRadioButtonGroup(unsigned int nGroup);

	virtual void ClientToScreen(RECT& rc);
	virtual void ScreenToClient(RECT& rc);

	// Attributes
//	static void SetRefreshTime(float fTime){ s_fTimeRefresh = fTime; }

	CUIControl* getFirstControl();
	CUIControl* getLastControl();
	CUIControl* GetNextControl(CUIControl* pControl);
	CUIControl* GetPrevControl(CUIControl* pControl);

	void RemoveControl(const char* szID);
	void RemoveAllControls();

	// Sets the callback used to notify the app of control events
	void EnableKeyboardInput(bool bEnable) { m_bKeyboardInput = bEnable; }
	bool IsKeyboardInputEnabled() const { return m_bKeyboardInput; }

	// Device state notification
	void Refresh();
	virtual void OnFrameMove(double fTime, float fElapsedTime);
	virtual void OnFrameRender(const Matrix& mTransform, double fTime, float fElapsedTime);

	// 鼠标响应
	virtual void OnMouseMove(POINT pt);
	virtual void OnMouseWheel(POINT point,short wheelDelta);
	virtual void OnLButtonDblClk(POINT point);
	virtual void OnLButtonDown(POINT point);
	virtual void OnLButtonUp(POINT point);
	virtual void OnRButtonDblClk(POINT point);
	virtual void OnRButtonDown(POINT point);
	virtual void OnRButtonUp(POINT point);
	virtual void OnMButtonDblClk(POINT point);
	virtual void OnMButtonDown(POINT point);
	virtual void OnMButtonUp(POINT point);
	// Shared resource access. Indexed fonts and textures are shared among
	// all the controls.

	virtual bool IsFocus();
	virtual void OnFocusOut();

	void FocusDefaultControl();

	bool m_bKeyboardInput;

	void LuaRegisterControlEvent(const char* szID, const char* szLuafun, unsigned int uEvent);
	void LuaRegisterEvent(const char* szEvent, const char* szLuafun);

	void RegisterControlEvent(const char* szID, PEVENT pfn, unsigned int uEvent=EVENT_DEFAULT);
	void RegisterEvent(const char* szEvent, PEVENT pfn);
protected:
	std::map<std::string,std::string>		m_mapLuaEvent;
	std::map<std::string,PEVENT>			m_mapStrEvent;
	std::map<unsigned int,PEVENT>			m_mapEvent;

//	static double s_fTimeRefresh;
	double m_fTimeLastRefresh;

	virtual void OnControlRegister();

	// Control events
	bool OnCycleFocus(bool bForward);

	std::vector<CUIControl*>	m_Controls;
	std::vector<CUIControl*>	m_AssignControls;
public:
	static lua_State* m_pLuaState;
};