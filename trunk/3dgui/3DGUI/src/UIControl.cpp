#include "UIControl.h"
#include "UICombo.h"
#include "tinyxml.h"

CUIControl* CUIControl::s_pControlFocus = NULL;
CUIControl* CUIControl::s_pControlPressed = NULL;
CUIControl* CUIControl::s_pControlMouseOver = NULL;

CUIStyle CUIControl::s_TipStyle;

HWND g_hWndUI;

HWND UIGetHWND()
{
	return g_hWndUI;
}

void UISetHWND(HWND hWnd)
{
	g_hWndUI = hWnd;
}

CUIControl::CUIControl()
{
	m_Type = UI_CONTROL_BUTTON;
	m_pUserData = NULL;

	m_bEnabled = true;
	m_bVisible = true;
	m_bMouseOver = false;
	m_bIsDefault = false;

	m_pParentDialog = NULL;

	m_rcOffset.set(0,0,0,0);
	m_rcScale.set(0,0,0,0);

	m_rcBoundingBox.set(0,0,0,0);

	m_vTranslation.x=0.0f;
	m_vTranslation.y=0.0f;
	m_vTranslation.z=0.0f;

	m_vRotate.x=0.0f;
	m_vRotate.y=0.0f;
	m_vRotate.z=0.0f;

	for (size_t i=0;i< CONTROL_STATE_MAX;++i)
	{
		m_setBlendRate[i]=0.8f;
	}
	for (size_t i=0;i< CONTROL_STATE_MAX;++i)
	{
		m_setTranslation[i].x=0.0f;
		m_setTranslation[i].y=0.0f;
		m_setTranslation[i].z=0.0f;
	}
	for (size_t i=0;i< CONTROL_STATE_MAX;++i)
	{
		m_setRotate[i].x=0.0f;
		m_setRotate[i].y=0.0f;
		m_setRotate[i].z=0.0f;
	}
}

CUIControl::~CUIControl()
{
}

void CUIControl::SetParent(CUICombo* pControl)
{
	m_pParentDialog=NULL;
	if (pControl)
	{
		if(pControl->isCombo())
		{
			m_pParentDialog = pControl;
		}
		else
		{
			std::string strInfo = strInfo+"void CUIControl::SetParent(CUICombo* pControl)\nThe parent of this control is unknown.\nControl:"+GetID()+"; Parent Control:"+pControl->GetID()+";";
			MessageBoxA(NULL,strInfo.c_str(),"Error",0);
		}
	}
}

void CUIControl::XMLParse(const TiXmlElement* pControlElement)
{
	// control name
	if (pControlElement->Attribute("id"))
	{
		SetID(pControlElement->Attribute("id"));
	}
	{
		if (pControlElement->Attribute("scale"))
		{
			m_rcScale.setByString(pControlElement->Attribute("scale"));
		}

		if (pControlElement->Attribute("rect"))
		{
			m_rcOffset.setByString(pControlElement->Attribute("rect"));
			m_rcOffset.right+=m_rcOffset.left;
			m_rcOffset.bottom+=m_rcOffset.top;
		}
		else if (pControlElement->Attribute("offset"))
		{
			m_rcOffset.setByString(pControlElement->Attribute("offset"));
		}
	}
	// Tip
	if(pControlElement->Attribute("tip"))
	{
		const char* pText = pControlElement->Attribute("tip");
		int nLength = strlen(pText)+1;
		WCHAR* pTextBuffer = new WCHAR[nLength];
		MultiByteToWideChar(CP_UTF8,0,pText,-1,pTextBuffer,nLength);
		SetTip(pTextBuffer);
		delete pTextBuffer;
	}
	// hotkey
	if (pControlElement->Attribute("hotkey"))
	{
		std::string strHotkey = pControlElement->Attribute("hotkey");
		SetHotkey(strHotkey);
	}
	// control Is Default
	if (pControlElement->Attribute("IsDefault"))
	{
		bool bIsDefault = false;
		pControlElement->Attribute("IsDefault", &bIsDefault);
		SetDefault(bIsDefault);
	}
	// style
	if (pControlElement->Attribute("style"))
	{
		SetStyle(pControlElement->Attribute("style"));
	}
	else
	{
		m_Style.XMLParse(*pControlElement);
		if (m_Style.m_StyleSprites.size()==0)
		{
			if (pControlElement->Attribute("type"))
			{
				SetStyle(pControlElement->Attribute("type"));
			}
			else
			{
				SetStyle("dialog");
			}
			//	std::wstring wstrInfo = FormatW(L"Can not find the style of this control.\nControl:%s;",s2ws(GetID()).c_str());
			//	MessageBoxW(NULL,wstrInfo.c_str(),L"Register Control Warn!",0);
		}
	}
	// text
	if (pControlElement->GetText()||pControlElement->Attribute("text"))
	{
		const char* pText = pControlElement->GetText();
		if(pText==NULL)
		{
			pText = pControlElement->Attribute("text");
		}
		int nLength = strlen(pText)+1;
		WCHAR* pTextBuffer = new WCHAR[nLength];
		MultiByteToWideChar(CP_UTF8,0,pText,-1,pTextBuffer,nLength);
		SetText(pTextBuffer);
		delete pTextBuffer;
	}
	{
		const TiXmlElement *pElement = pControlElement->FirstChildElement("blend");
		if (pElement)
		{
			const char* pszText = pElement->GetText();
			if(pszText)
			{
				float fBlend = (float)atof(pszText);
				for (size_t i=0;i< CONTROL_STATE_MAX;++i)
				{
					m_setBlendRate[i] = fBlend;
				}
			}
			for (size_t i=0;i< CONTROL_STATE_MAX;++i)
			{
				pszText =  pElement->Attribute(szControlState[i]);
				if (pszText)
				{
					m_setBlendRate[i] = (float)atof(pszText);
				}
			}
		}
	}
	{
		const TiXmlElement *pElement = pControlElement->FirstChildElement("translation");
		if (pElement)
		{
			const char* pszText = pElement->GetText();
			if(pszText)
			{
				MY3DGUI_VEC3D v;
				sscanf_s(pszText, "%f,%f,%f", &v.x, &v.y, &v.z);
				for (size_t i=0;i< CONTROL_STATE_MAX;++i)
				{
					m_setTranslation[i] = v;
				}
			}
			for (size_t i=0;i< CONTROL_STATE_MAX;++i)
			{
				pszText =  pElement->Attribute(szControlState[i]);
				if (pszText)
				{
					sscanf_s(pszText, "%f,%f,%f", &m_setTranslation[i].x, &m_setTranslation[i].y, &m_setTranslation[i].z);
				}
			}
		}
	}
	{
		const TiXmlElement *pElement = pControlElement->FirstChildElement("rotate");
		if (pElement)
		{
			const char* pszText = pElement->GetText();
			if(pszText)
			{
				MY3DGUI_VEC3D v;
				sscanf_s(pszText, "%f,%f,%f", &v.x, &v.y, &v.z);
				v.x*=3.14159f/180.0f;
				v.y*=3.14159f/180.0f;
				v.z*=3.14159f/180.0f;
				for (size_t i=0;i< CONTROL_STATE_MAX;++i)
				{
					m_setRotate[i] = v;
				}
			}
			for (size_t i=0;i< CONTROL_STATE_MAX;++i)
			{
				pszText =  pElement->Attribute(szControlState[i]);
				if (pszText)
				{
					sscanf_s(pszText, "%f,%f,%f", &m_setRotate[i].x, &m_setRotate[i].y, &m_setRotate[i].z);
					m_setRotate[i].x*=3.14159f/180.0f;
					m_setRotate[i].y*=3.14159f/180.0f;
					m_setRotate[i].z*=3.14159f/180.0f;
				}
			}
		}
	}
}

void CUIControl::SetInt(int val)
{
	wchar_t wszVal[256]={'\0'};
	_itow_s(val,wszVal,10);
	SetText(wszVal);
}

void CUIControl::SetFloat(float val,const int slen,const int alen)
{
	wchar_t wszVal[256]={'\0'};
	swprintf(wszVal,L"%*.*lf",slen,alen,val);
	SetText(wszVal);
}

int	CUIControl::GetInt()
{
	const wchar_t* pwcsText = GetText();
	if (pwcsText)
	{
		return _wtoi(pwcsText);
	}
	return 0;
}

float CUIControl::GetFloat()
{
	const wchar_t* pwcsText = GetText();
	if (pwcsText)
	{
		return _wtof(pwcsText);
	}
	return 0.0f;
}

void CUIControl::OnMove(int x, int y)
{
	m_rcRelativeBox.offset(x-m_rcBoundingBox.left,y-m_rcBoundingBox.top);
	m_rcBoundingBox.offset(x-m_rcBoundingBox.left,y-m_rcBoundingBox.top);
	UpdateRects();
}

void CUIControl::OnSize(const CRect<int>& rc)
{
	m_rcBoundingBox.left	= rc.left+rc.getWidth()*m_rcScale.left/100;
	m_rcBoundingBox.right	= rc.left+rc.getWidth()*m_rcScale.right/100;
	m_rcBoundingBox.top		= rc.top+rc.getHeight()*m_rcScale.top/100;
	m_rcBoundingBox.bottom	= rc.top+rc.getHeight()*m_rcScale.bottom/100;

	m_rcBoundingBox+= m_rcOffset;

	m_rcRelativeBox = m_rcBoundingBox;
	m_rcRelativeBox.offset(-rc.left,-rc.top);
	UpdateRects();
}

const CRect<int>& CUIControl::getOffset()
{
	return m_rcOffset;
}

const CRect<int>& CUIControl::getScale()
{
	return m_rcScale;
}

void CUIControl::setOffset(const CRect<int>& rc)
{
	m_rcOffset = rc;
}

void CUIControl::setScale(const CRect<int>& rc)
{
	m_rcScale = rc;
}

void CUIControl::SetHotkey(std::string& strHotkey)
{
	if (strHotkey.size() == 1)
	{
		if (0 <= strHotkey[0] && 9 >= strHotkey[0])
		{
			m_nHotkey = strHotkey[0];
		}
		else if ('A' <= strHotkey[0] && 'Z' >= strHotkey[0])
		{
			m_nHotkey = strHotkey[0];
		}
		else if ('a' <= strHotkey[0] && 'z' >= strHotkey[0])
		{
			m_nHotkey = strHotkey[0]+'A'-'a';
		}
	}
	else if (strHotkey == "esc")
	{
		m_nHotkey = VK_ESCAPE;
	}
}

void CUIControl::SetStyle(const std::string& strStyleName)
{
	m_Style.setStyle(strStyleName);
}

void CUIControl::Refresh()
{
	m_bMouseOver = false;
	//IsFocus()
	//m_bHasFocus = false;
}

void CUIControl::OnFrameRender(const Matrix& mTransform, double fTime, float fElapsedTime)
{
	CUIControl::updateUIMatrix(mTransform, fTime, fElapsedTime);
	RECT rc;
	rc.left =0;
	rc.right = m_rcRelativeBox.getWidth();
	rc.top =0;
	rc.bottom = m_rcRelativeBox.getHeight();
	m_Style.draw(rc, GetText(), GetState(), fElapsedTime);
}

void CUIControl::updateUIMatrix(const Matrix& mTransform, double fTime, float fElapsedTime)
{
	CONTROL_STATE state = GetState();
	if (m_fRate<1.0f)
	{
		m_fRate			+= m_setBlendRate[state]*fElapsedTime;//1.0f - powf(styleData.setBlendRate[iState], 30 * fElapsedTime);
		m_fRate			= min(1.0f,m_fRate);
		m_vRotate		= interpolate(m_fRate, m_vRotate, m_setRotate[state]);
		m_vTranslation	= interpolate(m_fRate, m_vTranslation, m_setTranslation[state]);
	}
	m_mWorld = UIGraph::getInstance().setUIMatrix(mTransform,m_rcRelativeBox.getRECT(),m_vTranslation,m_vRotate);
}

bool CUIControl::ContainsPoint(POINT pt)
{
	return m_rcBoundingBox.ptInRect(pt);
}

void CUIControl::sendEvent(const char* szEvent)
{
	if (GetParentDialog())
	{
		std::string strEvent=GetID()+szEvent;
		GetParentDialog()->progressEvent(strEvent);
	}
}

void CUIControl::drawTip(const Matrix& mTransform,const CRect<int>& rc, double fTime, float fElapsedTime)
{
	if (m_wstrTip.length()<=0)
	{
		return;
	}
	CRect<float> rect(0.0f,0.0f,1000.0f,1000.0f);
	UIGraph::getInstance().CalcTextRect(m_wstrTip.c_str(), rect);// 计算文本框大小

	RECT rcOffset = CUIControl::s_TipStyle.getFontStyle().rcOffset;
	rect.left -= rcOffset.left;
	rect.top -= rcOffset.top;
	rect.right -= rcOffset.right;
	rect.bottom -= rcOffset.bottom;

	int nTipWidth = rect.getWidth();
	int nTipHeight = rect.getHeight();

	if (rc.right<m_rcBoundingBox.left+nTipWidth)
	{
		rect.left	= rc.right-nTipWidth;
		rect.right	= rc.right;
	}
	else if (rc.left>m_rcBoundingBox.left)
	{
		rect.right	= rc.left+nTipWidth;
		rect.left	= rc.left;
	}
	else
	{
		rect.right	= m_rcBoundingBox.left+nTipWidth;
		rect.left	= m_rcBoundingBox.left;
	}

	if (rc.bottom<m_rcBoundingBox.bottom+nTipHeight)
	{
		if (m_rcBoundingBox.top<nTipHeight)
		{
			rect.top	= rc.bottom-nTipHeight;
			rect.bottom	= rc.bottom;
		}
		else
		{
			rect.top	= m_rcBoundingBox.top-nTipHeight;
			rect.bottom	= m_rcBoundingBox.top;
		}
	}
	else
	{
		rect.top	= m_rcBoundingBox.bottom;
		rect.bottom	= m_rcBoundingBox.bottom+nTipHeight;
	}

	CUIControl::s_TipStyle.draw(rect.getRECT(),m_wstrTip.c_str(),CONTROL_STATE_NORMAL, fElapsedTime);
}

void CUIControl::SetFocus(bool bFocus)
{
	if (bFocus)
	{
		if(s_pControlFocus==this)
			return;
		if(!CanHaveFocus())
			return;
		CUIControl* pOldControlFocus=s_pControlFocus;
		s_pControlFocus=this;
		if(pOldControlFocus&&!pOldControlFocus->IsFocus())
		{
			pOldControlFocus->OnFocusOut();
		}
		OnFocusIn();
	}
	else if(GetParentDialog()&&!GetParentDialog()->IsKeyboardInputEnabled())
	{
		ClearFocus();
	}
}

void CUIControl::SetPressed(bool bPressed)
{
	if (bPressed==false)
	{
		m_Style.playSound();
	}
	s_pControlPressed=bPressed?this:NULL;
}

void CUIControl::ClearFocus()
{
	clearFocus();
	//ReleaseCapture();
}

void CUIControl::clearFocus()
{
	if(s_pControlFocus)
	{
		CUIControl* pOldControlFocus=s_pControlFocus;
		s_pControlFocus=NULL;
		pOldControlFocus->OnFocusOut();
	}
}

void CUIControl::ClientToScreen(RECT& rc)
{
	if (m_pParentDialog)
	{
		m_pParentDialog->ClientToScreen(rc);
	}
	float fScale = 1;//m_Style.m_crSpriteColor[DIALOF_STYLE_SPRITE_INDEX_BACKGROUND].a / 255.0f;
	//int nCenterX = m_x+m_width/2;
	//int nCenterY = m_y+m_height/2;
// 	rc.left		= int((rc.left-m_width/2)*fScale+m_x+m_width/2);
// 	rc.right	= int((rc.right-m_width/2)*fScale+m_x+m_width/2);
// 	rc.top		= int((rc.top-m_height/2)*fScale+m_y+m_height/2);
// 	rc.bottom	= int((rc.bottom-m_height/2)*fScale+m_y+m_height/2);
}

void CUIControl::screenToClient(POINT& pt)
{
	pt.x -= m_rcBoundingBox.left;
	pt.y -= m_rcBoundingBox.top;
}

CRect<int> CUIControl::screenToClient(const CRect<int>& rc)
{
	CUICombo* pParent = GetParentDialog();
	if(pParent)
	{
		CRect<int> ret;
		ret = rc;
		ret.offset(-pParent->GetBoundingBox().left,-pParent->GetBoundingBox().top);
		return ret;
	}
	return rc;
}

void CUIControl::UpdateRects()
{
}

CONTROL_STATE CUIControl::GetState()
{
	CONTROL_STATE iState = CONTROL_STATE_NORMAL;

	CUICombo* pParent=GetParentDialog(); 
	if(pParent&&pParent->GetState()==CONTROL_STATE_HIDDEN)
	{
		iState = CONTROL_STATE_HIDDEN;
	}
	else if(m_bVisible == false)
	{
		iState = CONTROL_STATE_HIDDEN;
	}
	else if(m_bEnabled == false)
	{
		iState = CONTROL_STATE_DISABLED;
	}
	else if(IsPressed())
	{
		iState = CONTROL_STATE_PRESSED;
	}
	else if(m_bMouseOver)
	{
		iState = CONTROL_STATE_MOUSEOVER;
	}
	else if(IsFocus())
	{
		iState = CONTROL_STATE_FOCUS;
	}
	return iState;
}