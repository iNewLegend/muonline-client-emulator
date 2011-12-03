// BAAgreementDlg.cpp : implementation file
//

#include "stdafx.h"
#include "BAReport.h"
#include "BAAgreementDlg.h"
#include "BAReportDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// BAAgreementDlg dialog


BAAgreementDlg::BAAgreementDlg(CWnd* pParent /*=NULL*/)
	: CDialog(BAAgreementDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(BAAgreementDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void BAAgreementDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(BAAgreementDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(BAAgreementDlg, CDialog)
	//{{AFX_MSG_MAP(BAAgreementDlg)
	ON_BN_CLICKED(ID_AGREE, OnAgree)
	ON_BN_CLICKED(ID_DISAGREE, OnDisagree)
	//}}AFX_MSG_MAP
	ON_WM_SHOWWINDOW()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// BAAgreementDlg message handlers

BOOL BAAgreementDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	CEdit* pEdit = (CEdit*)GetDlgItem(IDC_EDIT_AGREEMENT);
	if (!g_bAgree)
		pEdit->SetWindowText("���α׷��� ġ������ ������ �߻��Ͽ����Ƿ� ���� ������ ���߻翡 �����ϵ��� �����մϴ�.\r\n\r\n�� ���α׷��� ���α׷� ������ ������ �м��� ���Ͽ� ���α׷� �������� �� ������ �ý��ۻ��(�ü��/�޸�/����ī�� ����)�� ���翡 �����Ұ��Դϴ�.\r\n\r\n���۵� �ڷ�� ���α׷� ���� ����� ���ؼ��� ���˴ϴ�.\r\n������ �����ðڽ��ϱ�?");


	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void BAAgreementDlg::OnCancel()
{
}

void BAAgreementDlg::OnAgree() 
{
	CBAReportDlg* pDlg = (CBAReportDlg*)AfxGetMainWnd();
	pDlg->Upload();
	DestroyWindow();
}

void BAAgreementDlg::OnDisagree() 
{
	CBAReportDlg* pDlg = (CBAReportDlg*)AfxGetMainWnd();
	pDlg->DestroyWindow();
}

BOOL BAAgreementDlg::OnCommand(WPARAM wParam, LPARAM lParam) 
{
	if (LOWORD(wParam) == ID_AGREE)
		TRACE("SHIT!");
	return CDialog::OnCommand(wParam, lParam);
}

void BAAgreementDlg::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDialog::OnShowWindow(bShow, nStatus);

}
