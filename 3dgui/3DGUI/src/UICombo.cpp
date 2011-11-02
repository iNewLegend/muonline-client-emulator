#include "UICombo.h"

#include "UIStatic.h"
#include "UIButton.h"
#include "UICheckBox.h"
#include "UIComboBox.h"
#include "UIRadioButton.h"
#include "UIImage.h"
#include "UIListBox.h"
#include "UIScrollBar.h"
#include "UISlider.h"
#include "UIProgress.h"
#include "UIEditBox.h"
#include "UIIMEEditBox.h"
#include "UIDisplay.h"
#include "UIComboNumber.h"
#include "UIComboColor.h"
#include "UIComboVec3D.h"
#include "tinyxml.h"
#include "lua_tinker.h"

std::string g_strDialogResFilename;

void UISetDialogRes(const char* szDialogRes)
{
	g_strDialogResFilename = szDialogRes;
}
const std::string& UIGetDialogRes()
{
	return g_strDialogResFilename;
}

lua_State* CUICombo::m_pLuaState = NULL;

//////////////////////////////////////////////////////////////////////////
CUICombo::CUICombo()
{
	m_Type = UI_CONTROL_COMBO;

	m_fTimeLastRefresh = 0;

	m_pParentDialog = NULL;

	m_bKeyboardInput = false;
}

CUICombo::~CUICombo()
{
	for(size_t i=0;i<m_AssignControls.size();++i)
	{
		delete m_AssignControls[i];
	}

	// Close Lua State
	// lua_close(m_pLuaState);
}

bool CUICombo::Create(const char* szID, CUICombo* pParent)
{
	assert(szID);
	SetID(szID);
	SetParent(pParent);
	std::string strLua=UIGetDialogRes();
	strLua.replace(strLua.find_last_of('\\'),1,"/");
	const size_t pos = strLua.find_last_of('/');
	strLua=strLua.substr(0,pos+1);
	strLua.append("script/");
	strLua.append(szID);
	strLua.append(".lua");
	lua_tinker::set(m_pLuaState, szID, this);

	int top = lua_gettop(m_pLuaState);

	int loaderr = luaL_loadfile(m_pLuaState,strLua.c_str());
	if (loaderr)
	{
		const char* errMsg = lua_tostring(m_pLuaState,-1);
		lua_settop(m_pLuaState,top);
#ifdef _DEBUG
		::printf_s("[LuaScriptModule]: %s\n", errMsg);
#endif
		return true;
		//MessageBoxA(NULL,errMsg, "Lua Script Error", 0);
	}

	// call it
	if (lua_pcall(m_pLuaState,0,0,0))
	{
		const char* errMsg = lua_tostring(m_pLuaState,-1);
		lua_settop(m_pLuaState,top);
#ifdef _DEBUG
		::printf_s("[LuaScriptModule]: %s\n", errMsg);
#endif
		MessageBoxA(NULL,errMsg, "Lua Script Error", 0);
	}

	lua_settop(m_pLuaState,top); // just in case :P

	//lua_settop(m_pLuaState,top); // just in case :P

	return true;
}

void CUICombo::OnControlRegister()
{
}

void CUICombo::XMLParse(const TiXmlElement* pElement)
{
	CUIControl::XMLParse(pElement);
	// Parse Controls
	XMLParseControls(pElement);
}

void CUICombo::UpdateRects()
{
	CUIControl::UpdateRects();
}

void CUICombo::XMLParseControls(const TiXmlElement* pElement)
{
	const TiXmlElement *pControlElement = pElement->FirstChildElement("element");
	while (pControlElement)
	{
		CUIControl* pControl = NULL;

		std::string strElement = pControlElement->Attribute("type");
		//
		if (pControlElement->Attribute("id"))
		{
			std::string strID = pControlElement->Attribute("id");
			pControl = getControl(strID.c_str());
			if (pControl)
			{
				RemoveControl(strID.c_str());
				AddControl(pControl);
			}
		}
		if (pControl==NULL)
		{
			if(strElement=="button")
			{
				pControl =  new CUIButton();
			}
			else if(strElement=="static")
			{
				pControl =  new CUIStatic();
			}
			else if(strElement=="image")
			{
				pControl =  new CUIImage();
			}
			else if(strElement=="checkbox")
			{
				pControl =  new CUICheckBox();
			}
			else if(strElement=="radiobutton")
			{
				pControl =  new CUIRadioButton();
			}
			else if(strElement=="slider")
			{
				pControl =  new CUISlider();
			}
			else if(strElement=="progress")
			{
				pControl =  new CUIProgress();
			}
			else if(strElement=="editbox")
			{
				pControl =  new CUIEditBox();
			}
			else if(strElement=="imeeditbox")
			{
				pControl =  new CUIIMEEditBox();
			}
// 			else if(strElement=="grid")
// 			{
// 				pControl =  new CUIGrid();
// 			}
			else if(strElement=="combo")
			{
				pControl =  new CUICombo();
			}
			else if(strElement=="scrollbar")
			{
				pControl =  new CUIScrollBar();
			}
			else if(strElement=="listbox")
			{
				pControl =  new CUIListBox();
			}
			else if(strElement=="combobox")
			{
				pControl =  new CUIComboBox();
			}
			else if(strElement=="numeditbox")
			{
				pControl =  new CUIComboNumber();
			}
			else if(strElement=="color")
			{
				pControl =  new CUIComboColor();
			}
			else if(strElement=="vec3d")
			{
				pControl =  new CUIComboVec3D();
			}
			else
			{
				pControl =  new CUICombo();
			}
			if (pControl)
			{
				m_AssignControls.push_back(pControl);
				AddControl(pControl);
			}
		}
		// 公共属性
		if (pControl)
		{
			if (pControl->isCombo())
			{
				((CUICombo*)pControl)->OnControlRegister();
				((CUICombo*)pControl)->LoadXml(UIGetDialogRes().c_str(), strElement.c_str());
			}
			pControl->XMLParse(pControlElement);
		}
		pControlElement = pControlElement->NextSiblingElement("element");
	}
	// Check all the controls are initialized.
	for(std::vector<CUIControl*>::iterator it=m_Controls.begin(); it != m_Controls.end(); it++)
	{
		const CRect<int>& rcOffset = (*it)->getOffset();
		const CRect<int>& rcScale = (*it)->getScale();
		if(0==rcOffset.left&&0==rcOffset.right&&0==rcOffset.top&&0==rcOffset.bottom&&
			0==rcScale.left&&0==rcScale.right&&0==rcScale.top&&0==rcScale.bottom)
		{
			std::string strInfo = "ID="+(*it)->GetID()+" ParentID="+GetID();
			MessageBoxA(NULL,strInfo.c_str(),"This control is not initialized!",0);
		}
	}
	// Check all the events.
	/*for (std::vector<EVENTMAP_ENTRY>::iterator it=m_EventMapEntry.begin();it!=m_EventMapEntry.end();it++)
	{
		if(GetControl(it->szID)==NULL)
		{
			MessageBoxW(NULL,s2ws("EventID="+it->strID+" ParentID="+GetID()).c_str(),L"Can't find the control of this event!",0);
		}
	}*/
}

// need rewrite, by huihui
void CUICombo::loadString(const char* szFilename)
{
	char szTemp[256]={0};
	for(std::vector<CUIControl*>::iterator it=m_Controls.begin(); it != m_Controls.end(); it++)
	{
		GetPrivateProfileStringA(m_strID.c_str(),("TEXT_"+(*it)->GetID()).c_str(),"",szTemp,256,szFilename);
		if (strlen(szTemp))
		{
			//(*it)->SetText(s2ws(szTemp));
		}
		GetPrivateProfileStringA(m_strID.c_str(),("TIP_"+(*it)->GetID()).c_str(),"",szTemp,256,szFilename);
		if (strlen(szTemp))
		{
			//(*it)->SetTip(s2ws(szTemp));
		}
	}
}

bool CUICombo::create(const TiXmlElement* pRootElement)
{
	//获得第一个dialog节点。
	const TiXmlElement *pDialogElement = pRootElement->FirstChildElement("dialog");
	while (pDialogElement)
	{
		if (pDialogElement->Attribute("id"))
		{
			if (m_strID == pDialogElement->Attribute("id"))
			{
				XMLParse(pDialogElement);
				break;
			}
		}
		// 查找下一个dialog
		pDialogElement = pDialogElement->NextSiblingElement("dialog");
	}
	return true;
}

bool CUICombo::LoadXml(const char* szFilename, const char* szDialog)
{
	TiXmlDocument myDocument;
	myDocument.LoadFile(szFilename, TIXML_ENCODING_UTF8);
	if (myDocument.Error())
	{
		return false;
	}

	//获得根元素，即root。
	TiXmlElement *pRootElement = myDocument.RootElement();
	if (pRootElement==NULL)
	{
		return false;
	}
	SetID(szDialog);
	return create(pRootElement);
}

void CUICombo::RemoveControl(const char* szID)
{
	for(std::vector<CUIControl*>::iterator it=m_Controls.begin(); it != m_Controls.end(); it++)
	{
		CUIControl* pControl = *it;
		if(pControl->GetID() == szID)
		{
			// Clear references to this control
			if(s_pControlPressed == pControl)
				s_pControlPressed = NULL;
			if(s_pControlMouseOver == pControl)
				s_pControlMouseOver = NULL;
			m_Controls.erase(it);
			return;
		}
	}
}

void CUICombo::RemoveAllControls()
{
	if(s_pControlPressed && s_pControlPressed->GetParentDialog() == this)
		s_pControlPressed = NULL;
	if(s_pControlMouseOver && s_pControlMouseOver->GetParentDialog() == this)
		s_pControlMouseOver = NULL;

	m_Controls.clear();
}

void CUICombo::Refresh()
{
	if(s_pControlMouseOver)
		s_pControlMouseOver->OnMouseLeave();

	s_pControlPressed = NULL;
	s_pControlMouseOver = NULL;

	for(size_t i=0;i<m_Controls.size();++i)
	{
		CUIControl* pControl = m_Controls[i];
		pControl->Refresh();
	}
}

void CUICombo::OnFrameMove(double fTime, float fElapsedTime)
{
	for(size_t i=0;i<m_Controls.size();++i)
	{
		if (m_Controls[i])
		{
			m_Controls[i]->OnFrameMove(fTime, fElapsedTime);
		}
	}
}

void CUICombo::OnFrameRender(const Matrix& mTransform, double fTime, float fElapsedTime)
{
	CUIControl::OnFrameRender(mTransform, fTime, fElapsedTime);
	// render controls
	for(size_t i=0;i<m_Controls.size();++i)
	{
		m_Controls[i]->OnFrameRender(m_mWorld,fTime,fElapsedTime);
	}
}

bool CUICombo::progressEvent(const std::string& strMsg)
{
	std::map<std::string,PEVENT>::iterator it = m_mapStrEvent.find(strMsg);
	if (it!=m_mapStrEvent.end())
	{
		(this->*(it->second))(); // 类成员函数指针
		return true;
	}
	if (m_pLuaState)
	{
		std::map<std::string,std::string>::iterator it = m_mapLuaEvent.find(strMsg);
		if (it!=m_mapLuaEvent.end())
		{
			lua_tinker::call<void>(m_pLuaState, it->second.c_str());
			return true;
		}
	}
	return false;
}

bool CUICombo::postMsg(const std::string& strMsg)
{
	return progressEvent(strMsg);
}

bool CUICombo::postMsg(WPARAM wParam, LPARAM lParam)
{
	return true;
}

bool CUICombo::MsgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	// If automation command-line switch is on, enable this dialog's keyboard input
	// upon any key press or mouse click.
//	if(DXUTGetAutomation() &&
//		(WM_LBUTTONDOWN == uMsg || WM_LBUTTONDBLCLK == uMsg || WM_KEYDOWN == uMsg))
//	{
//		GetUIMgr().EnableKeyboardInputForAllDialogs();
//	}

	switch(uMsg)
	{
	case WM_SIZE:
	case WM_MOVE: // window窗体移动
		{
			// Handle sizing and moving messages so that in case the mouse cursor is moved out
			// of an UI control because of the window adjustment, we can properly
			// unhighlight the highlighted control.
			POINT pt = { -1, -1 };
			OnMouseMove(pt);
			break;
		}
	}

	return false;
}

CUIControl* CUICombo::GetControlAtPoint(POINT pt)
{
	for(int i = m_Controls.size()-1; i >= 0; i--)
	{
		CUIControl* pControl = m_Controls[i];
		if(m_Controls[i]->IsVisible() && m_Controls[i]->GetEnabled() && m_Controls[i]->ContainsPoint(pt))
		{
			return pControl;
		}
	}
	return NULL;
}

bool CUICombo::GetControlEnabled(const char* szID)
{
	CUIControl* pControl = getControl(szID);
	if(pControl == NULL)
		return false;
	return pControl->GetEnabled();
}

void CUICombo::SetControlEnabled(const char* szID, bool bEnabled)
{
	CUIControl* pControl = getControl(szID);
	if(pControl == NULL)
		return;
	pControl->SetEnabled(bEnabled);
}

bool CUICombo::isControlVisible(const char* szID)
{
	CUIControl* pControl = getControl(szID);
	if(pControl == NULL)
		return false;
	return pControl->IsVisible();
}

void CUICombo::setControlVisible(const char* szID, bool bEnabled)
{
	CUIControl* pControl = getControl(szID);
	if(pControl == NULL)
		return;
	pControl->SetVisible(bEnabled);
}

void CUICombo::SetVisible(bool bVisible)
{
	CUIControl::SetVisible(bVisible);
}

void CUICombo::OnMouseMove(POINT point)
{
	// Figure out which control the mouse is over now
	CUIControl* pControl = GetControlAtPoint(point);

	if(pControl!=NULL)
	{
		pControl->OnMouseMove(point);
	}

	// If the mouse is still over the same control, nothing needs to be done
	if(pControl == s_pControlMouseOver)
		return;

	// Handle mouse leaving the old control
	if(s_pControlMouseOver)
		s_pControlMouseOver->OnMouseLeave();

	// Handle mouse entering the new control
	s_pControlMouseOver = pControl;
	if(pControl != NULL)
		s_pControlMouseOver->OnMouseEnter();
}

void CUICombo::OnMouseWheel(POINT point,short wheelDelta)
{
	// 控件
	CUIControl* pControl = GetControlAtPoint(point);
	if(pControl!=NULL)
	{
		pControl->OnMouseWheel(point,wheelDelta);
		return;
		//return true;
	}
	// return false;
}

void CUICombo::OnLButtonDblClk(POINT point)
{
	// 控件
	CUIControl* pControl = GetControlAtPoint(point);
	if(pControl!=NULL)
	{
		pControl->OnLButtonDblClk(point);
		return;
	}
}
void CUICombo::OnLButtonDown(POINT point)
{
	// 控件
	CUIControl* pControl = GetControlAtPoint(point);
	if(pControl!=NULL)
	{
		pControl->OnLButtonDown(point);
		return;
	}
}
void CUICombo::OnLButtonUp(POINT point)
{
	//// 消除按住状态
	//if(IsPressed())
	//{
	//	SetPressed(false);
	//	return;
	//}
	// 控件
	CUIControl* pControl = GetControlAtPoint(point);
	if(pControl!=NULL)
	{
		pControl->OnLButtonUp(point);
		return;
	}
}
void CUICombo::OnRButtonDblClk(POINT point)
{
	// 控件
	CUIControl* pControl = GetControlAtPoint(point);
	if(pControl!=NULL)
	{
		pControl->OnRButtonDblClk(point);
		return;
	}
}
void CUICombo::OnRButtonDown(POINT point)
{
	// 控件
	CUIControl* pControl = GetControlAtPoint(point);
	if(pControl!=NULL)
	{
		pControl->OnRButtonDown(point);
		return;
	}
}
void CUICombo::OnRButtonUp(POINT point)
{
	// 控件
	CUIControl* pControl = GetControlAtPoint(point);
	if(pControl!=NULL)
	{
		pControl->OnRButtonUp(point);
		return;
	}
}
void CUICombo::OnMButtonDblClk(POINT point)
{
	// 控件
	CUIControl* pControl = GetControlAtPoint(point);
	if(pControl!=NULL)
	{
		pControl->OnMButtonDblClk(point);
		return;
	}
}
void CUICombo::OnMButtonDown(POINT point)
{
	// 控件
	CUIControl* pControl = GetControlAtPoint(point);
	if(pControl!=NULL)
	{
		pControl->OnMButtonDown(point);
		return;
	}
}
void CUICombo::OnMButtonUp(POINT point)
{
	// 控件
	CUIControl* pControl = GetControlAtPoint(point);
	if(pControl!=NULL)
	{
		pControl->OnMButtonUp(point);
		return;
	}
}

void CUICombo::LuaRegisterControlEvent(const char* szID, const char* szLuafun, unsigned int uEvent)
{
	char szEvent[20]={0};
	_itoa_s(uEvent,szEvent,10);
	std::string strEvent;
	strEvent.append(szID);
	strEvent.append(szEvent);
	LuaRegisterEvent(strEvent.c_str(),szLuafun);
}

void CUICombo::LuaRegisterEvent(const char* szEvent, const char* szLuafun)
{
	if (m_mapLuaEvent.find(szEvent)!=m_mapLuaEvent.end())
	{
		char szTemp[256]={'\0'};
		sprintf_s(szTemp,"The same events!\nDialog:%s; Event:%s;\nCheck the dialog functions::OnControlRegister(), to sure that not have the same events!",m_strID.c_str(),szEvent);
		MessageBoxA(NULL,szTemp,"Register Event Warn!",0);
	}
	m_mapLuaEvent[szEvent]=szLuafun;
}

void CUICombo::RegisterControlEvent(const char* szID, PEVENT pfn, unsigned int uEvent)
{
	char szEvent[20]={0};
	_itoa_s(uEvent,szEvent,10);
	std::string strEvent;
	strEvent.append(szID);
	strEvent.append(szEvent);
	RegisterEvent(strEvent.c_str(),pfn);
}

void CUICombo::RegisterEvent(const char* szEvent, PEVENT pfn)
{
	if (m_mapStrEvent.find(szEvent)!=m_mapStrEvent.end())
	{
		char szTemp[256]={'\0'};
		sprintf_s(szTemp,"The same events!\nDialog:%s; Event:%s;\nCheck the dialog functions::OnControlRegister(), to sure that not have the same events!",m_strID.c_str(),szEvent);
		MessageBoxA(NULL,szTemp,"Register Event Warn!",0);
	}
	m_mapStrEvent[szEvent]=pfn;
}

void CUICombo::RegisterControl(const char* szID, CUIControl& control)
{
	control.SetID(szID);
	AddControl(&control);
}

void CUICombo::LuaRegisterControl(const char* szID, CUIControl* pControl)
{
	assert(pControl);
	pControl->SetID(szID);
	AddControl(pControl);
}

void CUICombo::AddControl(CUIControl* pControl)
{
	assert(pControl);
	pControl->SetParent(this);
	if (getControl(pControl->GetID().c_str())==NULL)
	{
		m_Controls.push_back(pControl);
	}
	else
	{
		MessageBoxA(NULL,pControl->GetID().c_str(),"Already have the same control name!",0);
	}
}

CUIControl* CUICombo::getControl(const char* szID)
{
	if (strlen(szID)==0)
	{
		return NULL;
	}
	for(size_t i=0;i<m_Controls.size();++i)
	{
		if(m_Controls[i]->GetID() == szID)
		{
			return m_Controls[i];
		}
	}
	return NULL;
}

CUIControl* CUICombo::GetControl(const char* szID, unsigned int nControlType)
{
	for(size_t i=0;i<m_Controls.size();++i)
	{
		CUIControl* pControl = m_Controls[i];
		if(pControl->GetID() == szID && pControl->GetType() == nControlType)
		{
			return pControl;
		}
	}
	return NULL;
}

CUIControl* CUICombo::getFirstControl()
{
	if (m_Controls.size()>0)
	{
		return m_Controls.front();
	}
	return NULL;
}

CUIControl* CUICombo::getLastControl()
{
	if (m_Controls.size()>0)
	{
		return m_Controls.back();
	}
	return NULL;
}

CUIControl* CUICombo::GetNextControl(CUIControl* pControl)
{
	for(std::vector<CUIControl*>::iterator it=m_Controls.begin(); it!=m_Controls.end(); it++)
	{
		if (*it==pControl)
		{
			if (it!=m_Controls.end())
			{
				return *++it;
			}
			else
			{
				return *m_Controls.begin();
			}
		}
	}
	return NULL;
}

CUIControl* CUICombo::GetPrevControl(CUIControl* pControl)
{
	for(std::vector<CUIControl*>::iterator it=m_Controls.begin(); it!=m_Controls.end(); it++)
	{
		if (*it==pControl)
		{
			if (it!=m_Controls.begin())
			{
				return *--it;
			}
			else
			{
				return *m_Controls.end();
			}
		}
	}
	return NULL;
}

void CUICombo::ClearRadioButtonGroup(unsigned int nButtonGroup)
{
	// Find all radio buttons with the given group number
	for(size_t i=0;i<m_Controls.size();++i)
	{
		CUIControl* pControl = m_Controls[i];

		if(pControl->GetType() == UI_CONTROL_RADIOBUTTON)
		{
			CUIRadioButton* pRadioButton = (CUIRadioButton*) pControl;

			if(pRadioButton->GetButtonGroup() == nButtonGroup)
				pRadioButton->SetChecked(false);
		}
	}
}

void CUICombo::ClientToScreen(RECT& rc)
{
	CUIControl::ClientToScreen(rc);
}

void CUICombo::ScreenToClient(RECT& rc)
{
}

void CUICombo::OnChildSize(const CRect<int>& rc)
{
	// change Controls' size
	for(size_t i=0;i<m_Controls.size();++i)
	{
		CUIControl* pControl = m_Controls[i];
		if (pControl)
		{
			pControl->OnSize(rc);
		}
	}
}

void CUICombo::OnSize(const CRect<int>& rc)
{
	CUIControl::OnSize(rc);
	OnChildSize(m_rcBoundingBox);
}