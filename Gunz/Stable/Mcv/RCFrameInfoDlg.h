#pragma once
#include "afxwin.h"


// CRCFrameInfoDlg ��ȭ �����Դϴ�.

class CRCFrameInfoDlg : public CDHtmlDialog
{
	DECLARE_DYNCREATE(CRCFrameInfoDlg)

public:
	CRCFrameInfoDlg(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~CRCFrameInfoDlg();
// ������
	HRESULT OnButtonOK(IHTMLElement *pElement);
	HRESULT OnButtonCancel(IHTMLElement *pElement);

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_FRAME_INFO, IDH = IDR_HTML_RCFRAMEINFODLG };

	void ClearListBox();
	void Begin();
	void End();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.
	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()
	DECLARE_DHTML_EVENT_MAP()
public:
	CListBox m_ListBox;
	afx_msg void OnLbnSelchangeNodeListbox();
};
