// PatchInterfaceDlg.cpp : ���� ����
//

#include "stdafx.h"
#include "PatchInterface.h"
#include "PatchInterfaceDlg.h"
#include "ReportCtrl.h"
#include ".\patchinterfacedlg.h"
#include "KeeperManager.h"
#include "mcrc32.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// ���� ���α׷� ������ ���Ǵ� CAboutDlg ��ȭ �����Դϴ�.

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// ��ȭ ���� ������
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ����

// ����
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{

}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// CPatchInterfaceDlg ��ȭ ����



CPatchInterfaceDlg::CPatchInterfaceDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CPatchInterfaceDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_nTimerIDCheckProcess = 0;
}

void CPatchInterfaceDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_ADVERTISEMENT, m_AnnounceEdit);
	DDX_Control(pDX, IDC_SEND, m_AnnounceSendBtn);
	// DDX_Control(pDX, IDC_CRC32, m_CRC32);
}

BEGIN_MESSAGE_MAP(CPatchInterfaceDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_WM_TIMER()
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDCANCEL, OnBnClickedCancel)
	ON_BN_CLICKED(IDC_SEND, OnBnClickedSend)
	ON_COMMAND(ID_CONNECT_KEEPER, OnConnectKeeper)
	ON_COMMAND(ID_DISCONNECT_KEEPER, OnDisconnectKeeper)
	ON_COMMAND(ID_CONNECT_SERVER, OnConnectServer)
	ON_COMMAND(ID_DISCONNECT_SERVER, OnDisconnectServer)
	ON_COMMAND(ID_AGENT_STOP, OnAgentStop)
	ON_COMMAND(ID_AGENT_START, OnAgentStart)
	ON_COMMAND(ID_SERVER_STOP, OnServerStop)
	ON_COMMAND(ID_SERVER_START, OnServerStart)
	ON_COMMAND(ID_SERVER_STOP_WITH_ANNCOUNCE, OnServerStopWithAnncounce)
	ON_COMMAND(ID_REFRESH_LIST, OnRefreshList)
	ON_COMMAND(ID_REBOOT, OnReboot)
	ON_COMMAND(ID_PATCH, OnPatch)
	ON_COMMAND(ID_MAKE_CRC, OnMakeCrc)
	ON_COMMAND(ID_SCHEDULE, OnSchedule)
	ON_EN_SETFOCUS(IDC_ADVERTISEMENT, OnEnSetfocusAdvertisement)
	ON_COMMAND(ID_SERVER_INFO, OnServerInfo)
	ON_COMMAND(ID_LoadServerList, OnLoadserverlist)
	ON_COMMAND(ID_SERVER_RELOAD_CONFIG, OnReloadServerConfig)
END_MESSAGE_MAP()


// CPatchInterfaceDlg �޽��� ó����

BOOL CPatchInterfaceDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// �ý��� �޴��� "����..." �޴� �׸��� �߰��մϴ�.

	// IDM_ABOUTBOX�� �ý��� ��� ������ �־�� �մϴ�.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// �� ��ȭ ������ �������� �����մϴ�. ���� ���α׷��� �� â�� ��ȭ ���ڰ� �ƴ� ��쿡��
	// �����ӿ�ũ�� �� �۾��� �ڵ����� �����մϴ�.
	SetIcon(m_hIcon, TRUE);			// ū �������� �����մϴ�.
	SetIcon(m_hIcon, FALSE);		// ���� �������� �����մϴ�.

	// TODO: ���⿡ �߰� �ʱ�ȭ �۾��� �߰��մϴ�.

	if( !LoadConfig() )
	{
		AfxMessageBox( "CPatchInterfaceDlg::OnInitDialog - config.ini���� �ε� ����." );
		return false;
	}

	m_nTimerIDCheckProcess = SetTimer( TIMERID, TIMER, NULL );

	GetReport.Create( this, 0 );

	if( !GetKeeperMgr.LoadKeeperList(KEEPER_LIST_FILE) )
	{
		AfxMessageBox( "KeeperList.txt�⺻ ���Ϸ� ���� ����Ʈ ���� ����.", 0, 0 );
		// return FALSE;
	}

	if( !GetKeeperMgr.InitKeeperObjs() )
	{
		AfxMessageBox( "Init Keeper Manager's fail", 0, 0 );
		return FALSE;
	}

	if( !CreateReportCtrl() )
		return FALSE;

	if( m_PatchCRCMaker.Create(IDD_PATCH_CRC32MAKER_DIALOG, this) )
	{
		m_PatchCRCMaker.SetRootDir( m_strPatchRootDir );
		m_PatchCRCMaker.SetListFilePath( m_strListFilePath );
	}	
	else
	{
		AfxMessageBox( "CRC32���� ������ ��������.", 0, 0 );
		return FALSE;
	}

	if( m_ScheduleMaker.Create(IDD_SCHEDULE_MAKER_DLG, this) )
	{
		if( !m_ScheduleMaker.Init() )
			return false;
	}
	else
	{
		AfxMessageBox( "Schedule���� ������ ��������.", 0, 0 );
		return FALSE;
	}

	if( !m_Patch.Create(IDD_PATCH_DLG, this) )
	{
		AfxMessageBox( "Patch������ ���� ����." );
		return FALSE;
	}

	if( m_ServerInfo.Create(IDD_SERVER_INFO_DLG, this) )
	{
		m_ServerInfo.Init();
	}
	else
	{
		AfxMessageBox( "�������� ������ ���� ����." );
		return FALSE;
	}

	if( !m_ReloadServerConfigDlg.Create(IDD_ReloadFileListDlg, this) )
	{
		AfxMessageBox( "Reload���� ���� ����." );
		return FALSE;
	}
	
	return TRUE;  // ��Ʈ�ѿ� ���� ��Ŀ���� �������� ���� ��� TRUE�� ��ȯ�մϴ�.
}


bool CPatchInterfaceDlg::OnRun()
{
	GetKeeperMgr.Update();

	return true;
}


bool CPatchInterfaceDlg::LoadConfig()
{
	char szVal[ 512 ];
	GetPrivateProfileString( "NETWORK", "PORT", "", szVal, 511, CONFIG_FILE );
	if( 0 == strlen(szVal) ) return false;
	GetKeeperMgr.SetPort( atoi(szVal) );

	GetPrivateProfileString( "PATCH_ROOT_DIR", "DIR", "", szVal, 511, CONFIG_FILE );
	if( 0 == strlen(szVal) ) return false;
	m_strPatchRootDir = szVal;

	GetPrivateProfileString( "OUTPUT_DIR", "LIST_FILE_PATH", "", szVal, 511, CONFIG_FILE );
	if( 0 == strlen(szVal) ) return false;
	m_strListFilePath = szVal;

	return true;
}


// Report List�� �����Ͽ� ��Ͻ�Ŵ.
bool CPatchInterfaceDlg::CreateReportCtrl()
{
	// GetReport.Create( this, 0 );
	GetReport.ShowWindow( SW_SHOW );
	GetReport.InsertColumns();
	GetReport.SetGridLines( true );
	GetReport.SetFullrowSelectStyle( true );
	GetReport.SetCheckboxStyle( 1 );
	GetReport.InsertItems();

	return true;
}

void CPatchInterfaceDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// ��ȭ ���ڿ� �ּ�ȭ ���߸� �߰��� ��� �������� �׸����� 
// �Ʒ� �ڵ尡 �ʿ��մϴ�. ����/�� ���� ����ϴ� MFC ���� ���α׷��� ��쿡��
// �����ӿ�ũ���� �� �۾��� �ڵ����� �����մϴ�.

void CPatchInterfaceDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // �׸��⸦ ���� ����̽� ���ؽ�Ʈ

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Ŭ���̾�Ʈ �簢������ �������� ����� ����ϴ�.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// �������� �׸��ϴ�.
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// ����ڰ� �ּ�ȭ�� â�� ���� ���ȿ� Ŀ���� ǥ�õǵ��� �ý��ۿ���
//  �� �Լ��� ȣ���մϴ�. 
HCURSOR CPatchInterfaceDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CPatchInterfaceDlg::OnBnClickedCancel()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.

	if( IDYES == AfxMessageBox("�����Ͻðڽ��ϱ�?", MB_YESNO, 0) )
	{
		GetKeeperMgr.Disconnect();
		GetKeeperMgr.Release();

		OnCancel();
	}
}

void CPatchInterfaceDlg::OnTimer(UINT nIDEvent)
{
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰� ��/�Ǵ� �⺻���� ȣ���մϴ�.

	GetKeeperMgr.Update();

	CDialog::OnTimer(nIDEvent);
}

void CPatchInterfaceDlg::OnDestroy()
{
	CDialog::OnDestroy();

	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰��մϴ�.

	if (m_nTimerIDCheckProcess)
		KillTimer(m_nTimerIDCheckProcess);

	GetKeeperMgr.Disconnect();
	GetKeeperMgr.Release();
}


void CPatchInterfaceDlg::OnBnClickedSend()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.

	char szBuf[ 256 ] = {0, };

	m_AnnounceEdit.GetWindowText( szBuf, 255 );

	GetKeeperMgr.RequestAnnounce( szBuf, static_cast<int>(strlen(szBuf)) );
}


void CPatchInterfaceDlg::OnConnectKeeper()
{
	// TODO: ���⿡ ��� ó���� �ڵ带 �߰��մϴ�.

	GetKeeperMgr.Connect(); 
}

void CPatchInterfaceDlg::OnDisconnectKeeper()
{
	// TODO: ���⿡ ��� ó���� �ڵ带 �߰��մϴ�.

	GetKeeperMgr.Disconnect();
}

void CPatchInterfaceDlg::OnConnectServer()
{
	// TODO: ���⿡ ��� ó���� �ڵ带 �߰��մϴ�.

	GetKeeperMgr.RequestKeeperConnectMatchServer();
}

void CPatchInterfaceDlg::OnDisconnectServer()
{
	// TODO: ���⿡ ��� ó���� �ڵ带 �߰��մϴ�.

	GetKeeperMgr.RequestDisconnectServer();
}

void CPatchInterfaceDlg::OnAgentStop()
{
	// TODO: ���⿡ ��� ó���� �ڵ带 �߰��մϴ�.

	GetKeeperMgr.RequestStopAgentServer();
}

void CPatchInterfaceDlg::OnAgentStart()
{
	// TODO: ���⿡ ��� ó���� �ڵ带 �߰��մϴ�.

	GetKeeperMgr.RequestStartAgentServer();
}

void CPatchInterfaceDlg::OnServerStop()
{
	// TODO: ���⿡ ��� ó���� �ڵ带 �߰��մϴ�.

	GetKeeperMgr.RequestStopServer();
}

void CPatchInterfaceDlg::OnServerStart()
{
	// TODO: ���⿡ ��� ó���� �ڵ带 �߰��մϴ�.

	GetKeeperMgr.RequestStartServer();
}

void CPatchInterfaceDlg::OnServerStopWithAnncounce()
{
	// TODO: ���⿡ ��� ó���� �ڵ带 �߰��մϴ�.

	GetKeeperMgr.RequestStopServerWithAnnounce();
}

void CPatchInterfaceDlg::OnRefreshList()
{
	// TODO: ���⿡ ��� ó���� �ڵ带 �߰��մϴ�.

	GetKeeperMgr.RequestRefreshServerList();
}

void CPatchInterfaceDlg::OnReboot()
{
	// TODO: ���⿡ ��� ó���� �ڵ带 �߰��մϴ�.

	if( IDYES == AfxMessageBox("������ ����� �Ͻðڽ��ϱ�?", MB_YESNO, 0) )
	{
		GetKeeperMgr.RequestRebootWindows();
	}
}

void CPatchInterfaceDlg::OnPatch()
{
	// TODO: ���⿡ ��� ó���� �ڵ带 �߰��մϴ�.

	m_Patch.ShowWindow( SW_SHOW );
}

void CPatchInterfaceDlg::OnMakeCrc()
{
	// TODO: ���⿡ ��� ó���� �ڵ带 �߰��մϴ�.

	m_PatchCRCMaker.Clear();
	m_PatchCRCMaker.ShowWindow( SW_SHOW );
}

void CPatchInterfaceDlg::OnSchedule()
{
	// TODO: ���⿡ ��� ó���� �ڵ带 �߰��մϴ�.

	m_ScheduleMaker.Reset();
	m_ScheduleMaker.ShowWindow( SW_SHOW );
}


void CPatchInterfaceDlg::OnOK()
{
	// TODO: ���⿡ Ư��ȭ�� �ڵ带 �߰� ��/�Ǵ� �⺻ Ŭ������ ȣ���մϴ�.

	// CDialog::OnOK();
}

void CPatchInterfaceDlg::OnEnSetfocusAdvertisement()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
}

void CPatchInterfaceDlg::OnServerInfo()
{
	// TODO: ���⿡ ��� ó���� �ڵ带 �߰��մϴ�.

	m_ServerInfo.Reset();
	m_ServerInfo.ShowWindow( SW_SHOW );
}

void CPatchInterfaceDlg::OnLoadserverlist()
{
	// TODO: ���⿡ ��� ó���� �ڵ带 �߰��մϴ�.

	// ���⿡�� ����Ʈ�� �о���� ������ ����Ʈ�� �������ϰ� �ٽ� ������ ��.

	if( 0 != GetReport.GetItemCount() )
	{
		if( IDYES != AfxMessageBox("Ű�ۿ��� ��� ������ ����˴ϴ�. \n��� �Ͻðڽ��ϱ�?", MB_YESNO, 0) )
			return;
	}

	OPENFILENAME OFN;

	char lpstrFile[MAX_PATH]="";
	char szFileTitle[ MAX_PATH ] = "";
	char szTempName[ MAX_PATH ] = "";

	memset(&OFN, 0, sizeof(OPENFILENAME));
	OFN.lStructSize = sizeof(OPENFILENAME);
	OFN.hwndOwner= 0;
	OFN.lpstrFilter="����Ʈ����(*.txt)\0*.txt";
	OFN.lpstrFile=lpstrFile;
	OFN.lpstrFileTitle = szFileTitle;
	OFN.lpTemplateName = szTempName;
	OFN.nMaxFile= MAX_PATH;
	OFN.nMaxFileTitle = MAX_PATH;
	OFN.lpstrInitialDir=".\\";

	GetOpenFileName( &OFN );
	
	if( 0 == strlen(lpstrFile) ) return;

	GetKeeperMgr.DeleteKeeperObjs();

	if( !GetKeeperMgr.LoadKeeperList(lpstrFile) ) 
	{
		AfxMessageBox( "���� �б� ����.", 0, 0 );
		return;
	}

	if( !GetKeeperMgr.InitKeeperObjs() )
	{
		AfxMessageBox( "Keeper Manager�ʱ�ȭ ����.", 0, 0 );
		return;
	}
	GetReport.DeleteAllItems();
	CreateReportCtrl();
}

void CPatchInterfaceDlg::OnReloadServerConfig()
{
	// make reload file list dialog box.
	m_ReloadServerConfigDlg.ShowWindow( SW_SHOW );
	// GetKeeperMgr.RquestReloadServerConfig();
}


