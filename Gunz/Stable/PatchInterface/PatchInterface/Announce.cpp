// Announce.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "PatchInterface.h"
#include "Announce.h"


// CAnnounce ��ȭ �����Դϴ�.

IMPLEMENT_DYNCREATE(CAnnounce, CDHtmlDialog)

CAnnounce::CAnnounce(CWnd* pParent /*=NULL*/)
	: CDHtmlDialog(CAnnounce::IDD, CAnnounce::IDH, pParent)
{
}

CAnnounce::~CAnnounce()
{
}

void CAnnounce::DoDataExchange(CDataExchange* pDX)
{
	CDHtmlDialog::DoDataExchange(pDX);
}

BOOL CAnnounce::OnInitDialog()
{
	CDHtmlDialog::OnInitDialog();
	return TRUE;  // ��Ŀ���� ��Ʈ�ѿ� �������� ������ TRUE�� ��ȯ�մϴ�.
}

BEGIN_MESSAGE_MAP(CAnnounce, CDHtmlDialog)
END_MESSAGE_MAP()

BEGIN_DHTML_EVENT_MAP(CAnnounce)
	DHTML_EVENT_ONCLICK(_T("ButtonOK"), OnButtonOK)
	DHTML_EVENT_ONCLICK(_T("ButtonCancel"), OnButtonCancel)
END_DHTML_EVENT_MAP()



// CAnnounce �޽��� ó�����Դϴ�.

HRESULT CAnnounce::OnButtonOK(IHTMLElement* /*pElement*/)
{
	OnOK();
	return S_OK;  // ��Ŀ���� ��Ʈ�ѿ� �������� ������ TRUE�� ��ȯ�մϴ�.
}

HRESULT CAnnounce::OnButtonCancel(IHTMLElement* /*pElement*/)
{
	OnCancel();
	return S_OK;  // ��Ŀ���� ��Ʈ�ѿ� �������� ������ TRUE�� ��ȯ�մϴ�.
}
