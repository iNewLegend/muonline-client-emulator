// ------------------------------
// Designed by RPGSky
// Date : 2010-07-30
// ------------------------------
// 
#pragma once
#include "UICombo.h"

#include "UIComboBox.h"
#include "UIRadioButton.h"
#include "UIImage.h"
#include "UIProgress.h"
#include "UIIMEEditBox.h"
#include "UIDisplay.h"
#include "UIComboNumber.h"
#include "UIComboColor.h"
#include "UIComboVec3D.h"

class CUIDialog : public CUICombo
{
public:
	CUIDialog();
	~CUIDialog();
public:
	virtual bool Create(const char* szID, CUICombo* pParent=NULL);
	virtual bool create(const TiXmlElement* pRootElement);

	virtual void loadString(const char* szFilename);
	virtual void XMLParse(const TiXmlElement* pControlElement);
	virtual void SetStyle(const std::string& strStyleName);
	virtual void OnChildSize(const CRect<int>& rc);
	virtual void OnSize(const CRect<int>& rc);
	virtual void UpdateRects();
	virtual bool ContainsPoint(POINT pt);
	// my message
	virtual bool postMsg(const std::string& strMsg);
	// Windows消息处理
	virtual bool MsgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

	CUIDialog* getChildDialog(const char* szID);
	CUIDialog* GetChildDialogAtPoint(POINT pt);

	virtual void SetVisible(bool bVisible);
	virtual void SetFocus(bool bFocus=true);
	virtual bool CanHaveFocus() { return (m_bVisible && m_bEnabled); }

	virtual void ClientToScreen(RECT& rc);
	virtual void ScreenToClient(RECT& rc);

	// Attributes

	bool IsExclusive() { return m_bExclusive; }
	void SetExclusive(bool bExclusive) { m_bExclusive = bExclusive; }

	void EnableCaption(bool bEnable) { m_bCaption = bEnable; }
	int GetCaptionHeight() const { return m_nCaptionHeight; }
	void SetCaptionHeight(int nHeight) { m_nCaptionHeight = nHeight; }
	void SetCaptionText(const std::wstring& wstrText) { m_wstrCaption = wstrText; }

	void SetCanMove(bool bCanMove) { m_bCanMove = bCanMove; }
	bool CanMove() { return m_bCanMove; }
	void SetCaptionEnable(bool bCaption) { m_bCaption = bCaption; }
	bool GetCaptionEnable() { return m_bCaption; }

	// Device state notification
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


	bool RegisterDialog(CUIDialog *pDialog);
	bool UnregisterDialog(const CUIDialog *pDialog);
	bool toTop();
protected:
	virtual void OnMove(int x, int y);
	virtual	bool OnInitDialog();

	// Control events
	bool OnCycleFocus(bool bForward);

	bool m_bCaption;
	bool m_bCanMove;
	bool m_bExclusive;
	std::wstring m_wstrCaption;

	CRect<int>	m_rcCaption;

	int m_nCaptionHeight;
	int m_nMouseOriginX;
	int m_nMouseOriginY;

	std::vector<CUIDialog*>		m_Dialogs;            // Dialogs registered
};