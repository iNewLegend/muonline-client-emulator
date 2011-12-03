// BASingleDlg.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "BAReport.h"
#include "BASingleDlg.h"
#include "BASysInfo.h"
#include "FileTransfer.h"
#include "BAReportDlg.h"


// BASingleDlg ��ȭ �����Դϴ�.

IMPLEMENT_DYNAMIC(BASingleDlg, CDialog)
BASingleDlg::BASingleDlg(CWnd* pParent /*=NULL*/)
	: CDialog(BASingleDlg::IDD, pParent)
{
}

BASingleDlg::~BASingleDlg()
{
}

void BASingleDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_NAME, m_Name);
	DDX_Control(pDX, IDC_EDIT_SYMPTOM, m_Symptom);
}


BEGIN_MESSAGE_MAP(BASingleDlg, CDialog)
END_MESSAGE_MAP()

BOOL BASingleDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  ���⿡ �߰� �ʱ�ȭ �۾��� �߰��մϴ�.

	m_Name.SetWindowText("NULL");
	m_Name.SetSel(0,-1);
	m_Name.SetFocus();
	return FALSE;  // return TRUE unless you set the focus to a control
	// ����: OCX �Ӽ� �������� FALSE�� ��ȯ�ؾ� �մϴ�.
}

void BASingleDlg::OnOK()
{
	// TODO: ���⿡ Ư��ȭ�� �ڵ带 �߰� ��/�Ǵ� �⺻ Ŭ������ ȣ���մϴ�.

	CWnd *pStatic = GetDlgItem(IDC_STATIC_SINGLE);
	pStatic->SetWindowText("�ʿ��� ������ �����ϰ� �ֽ��ϴ�. ��ø� ��ٷ��ּ���.");

	time_t currtime;
	time(&currtime);
	struct tm* pTM = localtime(&currtime);

	CString strName;
	m_Name.GetWindowText(strName);

	char szRemoteFileName[256];
	wsprintf(szRemoteFileName, "incoming/info/_report_%s_%.2d%.2d_%.2d%.2d.txt",
		(LPCSTR)strName, pTM->tm_mon+1, pTM->tm_mday, pTM->tm_hour, pTM->tm_min );

#define SYSINFO_FILENAME	"sysinfo.txt"

	FILE *pFile = fopen(SYSINFO_FILENAME,"w+");
	if(pFile) {
		CString strSymptom;
		m_Symptom.GetWindowText(strSymptom);
		fprintf(pFile,(LPCSTR)strSymptom);
		fprintf(pFile,"\n");
		fclose(pFile);

		BASysInfo(SYSINFO_FILENAME);

		g_pFileTransfer->PutFileQueue("www.battlearena.com", 21, "ftp", "ftp", SYSINFO_FILENAME, szRemoteFileName);

		CBAReportDlg* pDlg = (CBAReportDlg*)AfxGetMainWnd();
		pDlg->Upload();
		DestroyWindow();
	}else {
		AfxMessageBox("�����������.");
	}

//	CDialog::OnOK();
}

void BASingleDlg::OnCancel()
{
	// TODO: ���⿡ Ư��ȭ�� �ڵ带 �߰� ��/�Ǵ� �⺻ Ŭ������ ȣ���մϴ�.
	CBAReportDlg* pDlg = (CBAReportDlg*)AfxGetMainWnd();
	pDlg->DestroyWindow();

//	CDialog::OnCancel();
}
