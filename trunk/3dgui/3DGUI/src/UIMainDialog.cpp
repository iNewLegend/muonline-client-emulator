#include "UIMainDialog.h"
#include "IME.h"
#include "tinyxml.h"

CUIMainDialog::CUIMainDialog()
{
}

CUIMainDialog::~CUIMainDialog()
{
	CIME::Uninitialize();
}

bool CUIMainDialog::OnInitDialog()
{
	CIME::StaticOnCreateDevice();
	CUIControl::s_TipStyle.setStyle("tip");
	return CUIDialog::OnInitDialog();
}
void CUIMainDialog::OnFrameMove(double fTime, float fElapsedTime)
{
	UIGraph::getInstance().frameUpdate();
	CUIDialog::OnFrameMove(fTime,fElapsedTime);
}

void CUIMainDialog::OnFrameRender(const Matrix& mTransform, double fTime, float fElapsedTime)
{
	CUIDialog::OnFrameRender(mTransform,fTime,fElapsedTime);
	// 显示Tips
	if (s_pControlMouseOver)
	{
		m_mWorld = UIGraph::getInstance().setUIMatrix(mTransform,m_rcRelativeBox.getRECT(),m_vTranslation,m_vRotate);
		s_pControlMouseOver->drawTip(mTransform,m_rcBoundingBox.getRECT(), fTime, fElapsedTime);
	}
	else
	{
		CUIControl::s_TipStyle.Blend(CONTROL_STATE_HIDDEN,fElapsedTime);
	}
}

bool CUIMainDialog::MsgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	// Mouse messages
	switch(uMsg)
	{
	case WM_LBUTTONDOWN:
	case WM_MBUTTONDOWN:
	case WM_RBUTTONDOWN:
		SetCapture(hWnd);
		break;
	case WM_LBUTTONUP:
	case WM_MBUTTONUP:
	case WM_RBUTTONUP:
		ReleaseCapture();
		break;
	}
	switch(uMsg)
	{
	case WM_MOUSEMOVE:
	case WM_LBUTTONDOWN:
	case WM_LBUTTONUP:
	case WM_MBUTTONDOWN:
	case WM_MBUTTONUP:
	case WM_RBUTTONDOWN:
	case WM_RBUTTONUP:
	case WM_LBUTTONDBLCLK:
	case WM_MBUTTONDBLCLK:
	case WM_RBUTTONDBLCLK:
	case WM_MOUSEWHEEL:
		{
			POINT mousePoint = { short(LOWORD(lParam)), short(HIWORD(lParam)) };
			switch(uMsg)
			{
			case WM_LBUTTONDBLCLK:
				OnLButtonDblClk(mousePoint);
				break;
			case WM_LBUTTONDOWN:
				OnLButtonDown(mousePoint);
				break;
			case WM_LBUTTONUP:
				OnLButtonUp(mousePoint);
				break;
			case WM_RBUTTONDBLCLK:
				OnRButtonDblClk(mousePoint);
				break;
			case WM_RBUTTONDOWN:
				OnRButtonDown(mousePoint);
				break;
			case WM_RBUTTONUP:
				OnRButtonUp(mousePoint);
				break;
			case WM_MBUTTONDBLCLK:
				OnMButtonDblClk(mousePoint);
				break;
			case WM_MBUTTONDOWN:
				OnMButtonDown(mousePoint);
				break;
			case WM_MBUTTONUP:
				OnMButtonUp(mousePoint);
				break;
			case WM_MOUSEMOVE:
				OnMouseMove(mousePoint);
				break;
			case WM_MOUSEWHEEL:
				//GetCursorPos(&mousePoint);
				::ScreenToClient(hWnd,&mousePoint);
				OnMouseWheel(mousePoint,(short) HIWORD(wParam));
				break;
			}
		}
		break;
	}

	UISetHWND(hWnd);
	// Let the CUIIMEEditBox's static message proc handle the msg.
	// This is because some IME messages must be handled to ensure
	// proper functionalities and the static msg proc ensures that
	// this happens even if no control has the input focus.
	if(CIME::StaticMsgProc(uMsg, wParam, lParam))
	{
		//return true;
	}

	switch(uMsg)
	{
	case WM_SIZE:
		{
			RECT rc={0,0,LOWORD(lParam), HIWORD(lParam)};
			OnSize(rc);
		}
		break;
	case WM_ACTIVATEAPP:
		break;
		// Keyboard messages
	case WM_KEYDOWN:
	case WM_SYSKEYDOWN:
	case WM_KEYUP:
	case WM_SYSKEYUP:
		{
			// If a control is in focus, it belongs to this dialog, and it's enabled, then give
			// it the first chance at handling the message.
// 			if(s_pControlFocus && 
// 				s_pControlFocus->GetEnabled())
// 			{
// 				if(s_pControlFocus->HandleKeyboard(uMsg, wParam, lParam))
// 					return true;
// 			}
		}
		break;
	case WM_CAPTURECHANGED:
		{
			// The application has lost mouse capture.
			// The dialog object may not have received
			// a WM_MOUSEUP when capture changed. Reset
			// m_bDrag so that the dialog does not mistakenly
			// think the mouse button is still held down.
// 			if((HWND)lParam != hWnd)
// 			{
// 				SetPressed(false);
// 			}
// 			break;
		}
	}

	// 含焦点的控件 要优先处理消息 跨dialog处理
// 	if(s_pControlFocus && 
// 		s_pControlFocus->GetEnabled())
// 	{
// 		// If the control MsgProc handles it, then we don't.
// 		if(s_pControlFocus->MsgProc(uMsg, wParam, lParam))
// 			return true;
// 	}

	if(CUIDialog::MsgProc(hWnd, uMsg, wParam, lParam))
	{
		return true;
	}

	//ClearState();
	return false;
}

void CUIMainDialog::OnMouseMove(POINT point)
{
	if(s_pControlPressed)
	{
		s_pControlPressed->OnMouseMove(point);
		return;
	}
	CUIDialog::OnMouseMove(point);
}

void CUIMainDialog::OnLButtonUp(POINT point)
{
	// 按住的控件
	if(s_pControlPressed)
	{
		s_pControlPressed->OnLButtonUp(point);
		//s_pControlPressed->SetPressed(false);
		//s_pControlPressed = NULL;
		return;
	}
	CUIDialog::OnLButtonUp(point);
}

void CUIMainDialog::OnMouseWheel(POINT point,short wheelDelta)
{
	CUIDialog::OnMouseWheel(point,wheelDelta);
	if(false)
	{
// 		// 焦点控件 for what? I can't remanber.
// 		if(s_pControlFocus)
// 		{
// 			s_pControlFocus->OnMouseWheel(point,wheelDelta);
// 			//return true;
// 		}
	}
}

bool CUIMainDialog::create(const char* szFilename, const char* szID)
{
	UISetDialogRes(szFilename);
	CUIDialog::Create(szID,NULL);

	TiXmlDocument myDocument;
	myDocument.LoadFile(szFilename, TIXML_ENCODING_UTF8);
	if (myDocument.Error())
	{
		return false;
	}

	//获得根元素，即root。
	const TiXmlElement* pRootElement = myDocument.RootElement();
	if (pRootElement==NULL)
	{
		return false;
	}
	CUIDialog::create(pRootElement);
}