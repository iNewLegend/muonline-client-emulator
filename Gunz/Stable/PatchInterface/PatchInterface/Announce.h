#pragma once


// CAnnounce ��ȭ �����Դϴ�.

class CAnnounce : public CDHtmlDialog
{
	DECLARE_DYNCREATE(CAnnounce)

public:
	CAnnounce(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~CAnnounce();
// ������
	HRESULT OnButtonOK(IHTMLElement *pElement);
	HRESULT OnButtonCancel(IHTMLElement *pElement);

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_PATCHINTERFACE_DIALOG, IDH = IDR_HTML_ANNOUNCE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.
	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()
	DECLARE_DHTML_EVENT_MAP()
};
