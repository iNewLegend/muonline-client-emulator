// BAReport.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "BAReport.h"
#include "BAReportDlg.h"
#include "FileTransfer.h"
#include "BAMainFrame.h"
#include "BAReport.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// ��������
bool g_bAgree = false;		// true�� ���ǹ�ư�� �ȴ����� ���۵ȴ�.
CFileTransfer* g_pFileTransfer = NULL;
static void AgreeTransferCallback(int nReason, DWORD dwSentSize, DWORD dwLocalSize);

/////////////////////////////////////////////////////////////////////////////
// CBAReportApp

BEGIN_MESSAGE_MAP(CBAReportApp, CWinApp)
	//{{AFX_MSG_MAP(CBAReportApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CBAReportApp construction

CBAReportApp::CBAReportApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

CBAReportApp::~CBAReportApp()
{
	if (g_pFileTransfer) 
	{
		delete g_pFileTransfer; 
		g_pFileTransfer = 0;
	}
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CBAReportApp object

CBAReportApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CBAReportApp initialization

BOOL CBAReportApp::InitInstance()
{
	if (!AfxSocketInit())
	{
		AfxMessageBox(IDP_SOCKETS_INIT_FAILED);
		return FALSE;
	}

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.

#ifdef _AFXDLL
	Enable3dControls();			// Call this when using MFC in a shared DLL
#else
	Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif

	g_pFileTransfer = new CFileTransfer;

	CBAReportDlg dlg;
	m_pMainWnd = &dlg;


	if (m_lpCmdLine[0] == _T('\0')) {
		// �ܵ� ����ÿ��� �ĺ��Ҽ� �ִ� �̸� �Է¹ް� ����� �ٷ� �����Ѵ�
		dlg.SetSingle(true);
	} else {
		SetCommand(m_lpCmdLine);
	}


	// �Ʒ��� �ڵ�� ���ǵǾ������� ������ ���� ��׶���� ������ ����̴�.
	//if (g_bAgree)
	//{
	//	CBAMainFrame* pWnd = new CBAMainFrame;
	//	CString strWndClass = AfxRegisterWndClass(0, LoadCursor(IDC_ARROW), 0); 
	//	pWnd->CreateEx(0, strWndClass, NULL, 0, CRect(), NULL, NULL); 
	//	m_pMainWnd = pWnd;


	//	g_pFileTransfer->SetTransferCallback(AgreeTransferCallback);
	//	g_pFileTransfer->BeginTransfer();

	//	return TRUE;
	//}
	// ------------- �������


	int nResponse = dlg.DoModal();

	if (nResponse == IDOK)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with OK
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with Cancel
	}



	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}



void AgreeTransferCallback(int nReason, DWORD dwSentSize, DWORD dwLocalSize)
{

	switch(nReason) {
	case REASON_PROGRESS:
		{
			Sleep(100);
		}
		break;
	case REASON_END:
		{
			CBAMainFrame* pDlg = (CBAMainFrame*)AfxGetMainWnd();
			pDlg->PostMessage(WM_QUIT);
		}
		break;
	case REASON_ERROR:
		break;
	}
}
