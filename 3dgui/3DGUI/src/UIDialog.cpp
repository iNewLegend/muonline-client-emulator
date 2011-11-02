#include "UIDialog.h"
#include "tinyxml.h"
#include "lua_tinker.h"


int utf8ToUnicode16(const char* utf8, wchar_t* unicode16, int length) {
	char c;
	int i = 0;
	--length;
	while (c = *utf8) {
		if (c & 0x80) {
			if (c & 0x20) {
				if (i < length) {
					unicode16[i] = ((utf8[0] & 0xf) << 12) | ((utf8[1] & 0x3f) << 6) | ((utf8[2] & 0x3f));
				}
				++i;
				utf8 += 3;
			} else {
				if (i < length) {
					unicode16[i] = ((utf8[0] & 0x1f) << 6) | ((utf8[1] & 0x3f));
				}
				++i;
				utf8 += 2;
			}
		} else {
			if (i < length) {
				unicode16[i] = c;
			}
			++i;
			++utf8;
		}
	}
	if (i <= length) {
		unicode16[i] = 0;
	}
	return i + 1;
}

static int l_utf8ToUnicode16(lua_State* L) {
	wchar_t a[256];
	wchar_t* unicode16 = a;
	int length = 0;
	const char* utf8;

	utf8 = luaL_checkstring(L, 1);
	length = utf8ToUnicode16(utf8, unicode16, 256);
	if (length <= 256) {
		lua_pushlstring(L, (const char*) unicode16, length * 2);
	} else {
		unicode16 = (wchar_t*)malloc(length);
		utf8ToUnicode16(utf8, unicode16, length);
		lua_pushlstring(L, (const char*) unicode16, length * 2);
		free(unicode16);
	}
	return 1;
}

static int l_wcstombs(lua_State* L) {
	char a[256];
	char* utf8 = a;
	int length = 0;
	const wchar_t* unicode16 = (const wchar_t*)luaL_checkstring(L, 1);
	setlocale(LC_ALL,"chs");
	length = wcstombs(utf8, unicode16, 256);
	if (length<0)
	{
		const char szError[] = "error:lua_wcstombs";
		lua_pushlstring(L, szError, sizeof(szError));
	}
	else if (length <= 256) {
		lua_pushlstring(L, (const char*) utf8, length);
	}
	else {
		utf8 = (char*)malloc(length);
		wcstombs(utf8, unicode16, length);
		lua_pushlstring(L, (const char*) utf8, length);
		free(utf8);
	}
	return 1;
}

CUIDialog::CUIDialog()
{
	m_nMouseOriginX = 0;
	m_nMouseOriginY = 0;
	m_Type = UI_CONTROL_DIALOG;

	m_bCaption = false;
	m_bCanMove = true;
	m_bExclusive = false;
	m_nCaptionHeight = 0;

	// Create Lua State
	if (m_pLuaState==NULL)
	{
		m_pLuaState = lua_open();
		luaopen_base(m_pLuaState);
		luaL_openlibs(m_pLuaState);
		luaopen_string(m_pLuaState);

		// utf8_to_utf16
		lua_register(m_pLuaState, "L", l_utf8ToUnicode16);
		// l_wcstombs
		lua_register(m_pLuaState, "ws2s", l_wcstombs);

		lua_tinker::class_<wchar_t>(m_pLuaState,"wchar_t");
		lua_tinker::class_<void*>(m_pLuaState,"void*");

		lua_tinker::class_<CUIControl>(m_pLuaState,"CUIControl")
			.con(lua_tinker::constructor<void>())
			.def("IsVisible",	&CUIControl::IsVisible)
			.def("SetVisible",	&CUIControl::SetVisible)
			.def("SetText",		&CUIControl::SetText)
			.def("GetText",		&CUIControl::GetText);

		lua_tinker::class_<CUIStatic>(m_pLuaState,"CUIStatic")
			.inh<CUIControl>()
			.con(lua_tinker::constructor<void>());

		lua_tinker::class_<CUIButton>(m_pLuaState,"CUIButton")
			.inh<CUIControl>()
			.con(lua_tinker::constructor<void>());

		lua_tinker::class_<CUIEditBox>(m_pLuaState,"CUIEditBox")
			.inh<CUIControl>()
			.con(lua_tinker::constructor<void>());

		lua_tinker::class_<CUICombo>(m_pLuaState,"CUICombo")
			.inh<CUIControl>()
			.con(lua_tinker::constructor<void>())
			.def("getControl", &CUICombo::GetControl)
			.def("isControlVisible", &CUICombo::isControlVisible)
			.def("setControlVisible", &CUICombo::setControlVisible)
			.def("regEvent", &CUICombo::LuaRegisterEvent)
			.def("regControlEvent", &CUICombo::LuaRegisterControlEvent)
			.def("regControl", &CUICombo::LuaRegisterControl);

		lua_tinker::class_<CUIListBox>(m_pLuaState,"CUIListBox")
			.inh<CUICombo>()
			.con(lua_tinker::constructor<void>())
			.def("getItemCount", &CUIListBox::GetItemCount)
			.def("addItem", &CUIListBox::AddItem)
			.def("insertItem", &CUIListBox::InsertItem)
			.def("removeItem", &CUIListBox::RemoveItem)
			.def("getSelectedIndex", &CUIListBox::GetSelectedIndex)
			.def("findItem", &CUIListBox::FindItem)
			.def("selectItem", &CUIListBox::SelectItem)
			.def("selectByText", &CUIListBox::selectByText)
			;
		lua_tinker::class_<CUIDialog>(m_pLuaState,"CUIDialog")
			.inh<CUICombo>()
			.con(lua_tinker::constructor<void>())
			.def("create", &CUIDialog::Create);
	}
}

CUIDialog::~CUIDialog()
{
	if (GetParentDialog())
	{
		((CUIDialog*)GetParentDialog())->UnregisterDialog(this);
	}

	for(size_t i=0;i<m_Dialogs.size();++i)
	{
		if (m_Dialogs[i]->GetParentDialog() == this)
		{
			m_Dialogs[i]->SetParent(NULL);
		}
	}

	for(size_t i=0;i<m_AssignControls.size();++i)
	{
		delete m_AssignControls[i];
	}
}

bool CUIDialog::Create(const char* szID, CUICombo* pParent)
{
	CUICombo::Create(szID, pParent);
	if (GetParentDialog()!=NULL)
	{
		((CUIDialog*)GetParentDialog())->RegisterDialog(this);
	}
	OnControlRegister();
	return true;
}

bool CUIDialog::create(const TiXmlElement* pRootElement)
{
	if (CUICombo::create(pRootElement)==false)
	{
		return false;
	}
	for(size_t i=0;i<m_Dialogs.size();++i)
	{
		if (m_Dialogs[i]->create(pRootElement)==false)
		{
			return false;
		}
	}
	if (OnInitDialog()==false)
	{
		return false;
	}
	return true;
}

bool CUIDialog::OnInitDialog()
{
	return true;
}

// Need Rewrite
void CUIDialog::loadString(const char* szFilename)
{
	CUICombo::loadString(szFilename);
	//std::string strCaption = IniGetStr(strFilename.c_str(),m_strID.c_str(),"CAPTION");
	//if (!strCaption.empty())
	{
	//	SetCaptionText(s2ws(strCaption));
	}
	for(std::vector<CUIDialog*>::iterator it=m_Dialogs.begin(); it != m_Dialogs.end(); it++)
	{
		(*it)->loadString(szFilename);
	}
}

void CUIDialog::XMLParse(const TiXmlElement* pElement)
{
	if (pElement->Attribute("canmove"))
	{
		pElement->Attribute("canmove", &m_bCanMove);
	}
	if (pElement->Attribute("exclusive"))
	{
		pElement->Attribute("exclusive",&m_bExclusive);
	}
	// caption
	{
		const TiXmlElement *pCaptionElement = pElement->FirstChildElement("caption");
		if (pCaptionElement)
		{
			if(pCaptionElement->GetText())
			{
				WCHAR strText[256]={0};
				MultiByteToWideChar(CP_UTF8,0,pCaptionElement->GetText(),-1,strText,256);
				SetCaptionText(strText);
			}

			if (pCaptionElement->Attribute("height"))
			{
				pCaptionElement->Attribute("height",&m_nCaptionHeight);
			}
			else
			{
				m_nCaptionHeight=18;
			}
			SetCaptionEnable(true);
		}
	}
	CUICombo::XMLParse(pElement);
}

void CUIDialog::UpdateRects()
{
	CUICombo::UpdateRects();
	m_rcCaption = m_rcBoundingBox;
	m_rcCaption.bottom = m_rcCaption.top+m_nCaptionHeight;
}

bool CUIDialog::ContainsPoint(POINT pt)
{
	if (CUICombo::ContainsPoint(pt))
	{
		return true;
	}
	CUIControl* pControl = GetControlAtPoint(pt);
	return pControl!=NULL;
}

void CUIDialog::OnFrameMove(double fTime, float fElapsedTime)
{
	if(m_bVisible)
	{
		for(size_t i=0;i<m_Controls.size();++i)
		{
			if (m_Controls[i])
			{
				m_Controls[i]->OnFrameMove(fTime, fElapsedTime);
			}
		}
		//
		for(size_t i=0;i<m_Dialogs.size();++i)
		{
			m_Dialogs[i]->OnFrameMove(fTime, fElapsedTime);
		}
	}
}

void CUIDialog::OnFrameRender(const Matrix& mTransform, double fTime, float fElapsedTime)
{
	//CUICombo::OnFrameRender(mTransform,fTime,fElapsedTime);
	CUIControl::OnFrameRender(mTransform, fTime, fElapsedTime);

	// If the dialog is minimized, skip rendering
	// its controls.
	Matrix mNewTransform = m_mWorld*Matrix::newTranslation(Vec3D(0,m_nCaptionHeight,0));

	// render controls
	for(size_t i=0;i<m_Controls.size();++i)
	{
		m_Controls[i]->OnFrameRender(mNewTransform,fTime,fElapsedTime);
	}
	//
	for(size_t i=0;i<m_Dialogs.size();++i)
	{
		CUIDialog* pDialog = m_Dialogs[i];
		if (pDialog->IsVisible()||pDialog->isStyleVisible())
		{
			pDialog->OnFrameRender(mNewTransform,fTime,fElapsedTime);
		}
	}
}

bool CUIDialog::postMsg(const std::string& strMsg)
{
	for (int i=m_Dialogs.size()-1; i>=0; --i)
	{
		if (m_Dialogs[i]->IsVisible())
		{
			if(m_Dialogs[i]->postMsg(strMsg))
			{
				return true;
			}
		}
	}
	return CUICombo::postMsg(strMsg);
}

bool CUIDialog::MsgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	for (int i=m_Dialogs.size()-1; i>=0; --i)
	{
		if (m_Dialogs[i]->IsVisible())
		{
			if(m_Dialogs[i]->MsgProc(hWnd, uMsg, wParam, lParam))
			{
				return true;
			}
		}
	}

	return CUICombo::MsgProc(hWnd, uMsg, wParam, lParam);
}

CUIDialog* CUIDialog::getChildDialog(const char* szID)
{
	for (int i=m_Dialogs.size()-1; i>=0; --i)
	{
		CUIDialog* pDialog = m_Dialogs[i];
		if(pDialog->GetID()==szID)
		{
			return pDialog;
		}
	}
	return NULL;
}

CUIDialog* CUIDialog::GetChildDialogAtPoint(POINT pt)
{
	for (int i=m_Dialogs.size()-1; i>=0; --i)
	{
		CUIDialog* pDialog = m_Dialogs[i];
		if(pDialog->IsExclusive()&&pDialog->GetEnabled() && pDialog->IsVisible())
		{
			return pDialog;
		}
	}
	for (int i=m_Dialogs.size()-1; i>=0; --i)
	{
		CUIDialog* pDialog = m_Dialogs[i];
		if(pDialog->ContainsPoint(pt)&&pDialog->GetEnabled() && pDialog->IsVisible())
		{
			return pDialog;
		}
	}
	return NULL;
}

void CUIDialog::SetVisible(bool bVisible)
{
	CUICombo::SetVisible(bVisible);
	if (bVisible)
	{
		toTop();
	}
}

void CUIDialog::OnMouseMove(POINT point)
{
	if (IsPressed())
	{
		OnMove(point.x - m_nMouseOriginX,point.y - m_nMouseOriginY);
		return;
	}
	// 子对话框
	CUIDialog* pChildDialog = GetChildDialogAtPoint(point);
	if(pChildDialog)
	{
		pChildDialog->OnMouseMove(point);
		return;
	}
	// 
	CUICombo::OnMouseMove(point);
}

void CUIDialog::OnMouseWheel(POINT point,short wheelDelta)
{
	// 子对话框
	CUIDialog* pChildDialog = GetChildDialogAtPoint(point);
	if(pChildDialog)
	{
		pChildDialog->OnMouseWheel(point,wheelDelta);
		return;
	}
	CUICombo::OnMouseWheel(point,wheelDelta);
}

void CUIDialog::OnLButtonDblClk(POINT point)
{
	// 子对话框
	CUIDialog* pChildDialog = GetChildDialogAtPoint(point);
	if(pChildDialog)
	{
		pChildDialog->OnLButtonDblClk(point);
		return;
	}
	CUICombo::OnLButtonDblClk(point);
	// 标题
	if(m_bCaption&&m_rcCaption.ptInRect(point))
	{
		//ReleaseCapture();
		//m_bMinimized = !m_bMinimized;
		UpdateRects();
		return;
	}
}
void CUIDialog::OnLButtonDown(POINT point)
{
	// 设置为焦点对话框
	toTop();
	// 子对话框
	CUIDialog* pChildDialog = GetChildDialogAtPoint(point);
	if(pChildDialog)
	{
		pChildDialog->OnLButtonDown(point);
		return;
	}
	// 控件
	CUIControl* pControl = GetControlAtPoint(point);
	if(pControl!=NULL)
	{
		pControl->OnLButtonDown(point);
		return;
	}
	// 标题
	if(m_bCaption&&m_bCanMove&&m_rcCaption.ptInRect(point))
	{
		m_nMouseOriginX = point.x-m_rcBoundingBox.left;
		m_nMouseOriginY = point.y-m_rcBoundingBox.top;
		SetPressed(true);
		return;
	}
}
void CUIDialog::OnLButtonUp(POINT point)
{
	// 消除按住状态
	if(IsPressed())
	{
		SetPressed(false);
		return;
	}
	CUICombo::OnLButtonUp(point);
	// 子对话框
	CUIDialog* pChildDialog = GetChildDialogAtPoint(point);
	if(pChildDialog)
	{
		pChildDialog->OnLButtonUp(point);
		return;
	}
}
void CUIDialog::OnRButtonDblClk(POINT point)
{
	// 子对话框
	CUIDialog* pChildDialog = GetChildDialogAtPoint(point);
	if(pChildDialog)
	{
		pChildDialog->OnRButtonDblClk(point);
		return;
	}
	CUICombo::OnRButtonDblClk(point);
}
void CUIDialog::OnRButtonDown(POINT point)
{
	// 设置为焦点对话框
	toTop();
	// 子对话框
	CUIDialog* pChildDialog = GetChildDialogAtPoint(point);
	if(pChildDialog)
	{
		pChildDialog->OnRButtonDown(point);
		return;
	}
	CUICombo::OnRButtonDown(point);
}
void CUIDialog::OnRButtonUp(POINT point)
{
	// 子对话框
	CUIDialog* pChildDialog = GetChildDialogAtPoint(point);
	if(pChildDialog)
	{
		pChildDialog->OnRButtonUp(point);
		return;
	}
	CUICombo::OnRButtonUp(point);
}
void CUIDialog::OnMButtonDblClk(POINT point)
{
	// 子对话框
	CUIDialog* pChildDialog = GetChildDialogAtPoint(point);
	if(pChildDialog)
	{
		pChildDialog->OnMButtonDblClk(point);
		return;
	}
	CUICombo::OnMButtonDblClk(point);
}
void CUIDialog::OnMButtonDown(POINT point)
{
	// 设置为焦点对话框
	toTop();
	// 子对话框
	CUIDialog* pChildDialog = GetChildDialogAtPoint(point);
	if(pChildDialog)
	{
		pChildDialog->OnMButtonDown(point);
		return;
	}
	CUICombo::OnMButtonDown(point);
}
void CUIDialog::OnMButtonUp(POINT point)
{
	// 子对话框
	CUIDialog* pChildDialog = GetChildDialogAtPoint(point);
	if(pChildDialog)
	{
		pChildDialog->OnMButtonUp(point);
		return;
	}
	CUICombo::OnMButtonUp(point);
}

void CUIDialog::OnMove(int x, int y)
{
	CUICombo::OnMove(x, y);
	CRect<int> rcChild = m_rcBoundingBox;
	rcChild.top+=m_nCaptionHeight;
	OnChildSize(rcChild);
}

void CUIDialog::ClientToScreen(RECT& rc)
{
	CUICombo::ClientToScreen(rc);
	rc.top		+= m_bCaption?m_nCaptionHeight:0;
	rc.bottom	+= m_bCaption?m_nCaptionHeight:0;
}

void CUIDialog::ScreenToClient(RECT& rc)
{
}

void CUIDialog::OnChildSize(const CRect<int>& rc)
{
	for(size_t i=0;i<m_Dialogs.size();++i)
	{
		m_Dialogs[i]->OnSize(rc);
	}
	CUICombo::OnChildSize(rc);
}

void CUIDialog::OnSize(const CRect<int>& rc)
{
	//CUICombo::OnSize(rc); two OnChildSize
	CUIControl::OnSize(rc);
	CRect<int> rcChild = m_rcBoundingBox;
	rcChild.top+=m_nCaptionHeight;
	OnChildSize(rcChild);
}

#include <algorithm>
bool CUIDialog::RegisterDialog(CUIDialog *pDialog)
{
	std::vector<CUIDialog*>::const_iterator it = std::find(m_Dialogs.begin(),m_Dialogs.end(),pDialog);
	if(it != m_Dialogs.end())
	{
		return false;
	}
	if (!m_Dialogs.empty())
	{
		if(m_Dialogs.back()->IsExclusive()&&m_Dialogs.back()->GetEnabled() && m_Dialogs.back()->IsVisible())
		{
			m_Dialogs.insert(m_Dialogs.end()-1,pDialog);
			return true;
		}
	}
	m_Dialogs.push_back(pDialog);
	return true;
}

bool CUIDialog::UnregisterDialog(const CUIDialog *pDialog)
{
	std::vector<CUIDialog*>::const_iterator it = std::find(m_Dialogs.begin(),m_Dialogs.end(),pDialog);
	if(it != m_Dialogs.end())
	{
		m_Dialogs.erase(it);
		return true;
	}
	return false;
}

bool CUIDialog::toTop()
{
	CUIDialog* pParentDialog=(CUIDialog*)GetParentDialog();
	if (pParentDialog==NULL)
	{
		return false;
	}
	if (!pParentDialog->UnregisterDialog(this))
	{
		return false;
	}
	return pParentDialog->RegisterDialog(this);
}