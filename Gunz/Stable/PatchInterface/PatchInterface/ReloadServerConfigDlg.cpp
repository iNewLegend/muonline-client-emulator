// ReloadServerConfigDlg.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "PatchInterface.h"
#include "ReloadServerConfigDlg.h"
#include "KeeperManager.h"
#include ".\reloadserverconfigdlg.h"


// ReloadServerConfigDlg ��ȭ �����Դϴ�.

IMPLEMENT_DYNAMIC(ReloadServerConfigDlg, CDialog)
ReloadServerConfigDlg::ReloadServerConfigDlg(CWnd* pParent /*=NULL*/)
	: CDialog(ReloadServerConfigDlg::IDD, pParent)
{
}

ReloadServerConfigDlg::~ReloadServerConfigDlg()
{
}

void ReloadServerConfigDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	// DDX_Control(pDX, IDC_RELOAD_LIST, m_ReloadList);
	DDX_Control(pDX, IDC_NEWHASHVALUE, m_NewHashValue);
}


bool ReloadServerConfigDlg::LoadReloadList()
{
	FILE* fp = fopen( RELOAD_LIST, "r" );
	if( 0 == fp )
		return false;

	m_ReloadFileList.clear();

	char szLine[ 256 ]	= {0,};
	char szFile[ 64 ]	= {0,};

	while( 0 != fgets(szLine, 255, fp) )
	{
		sscanf( szLine, "%s", szFile );

		m_ReloadFileList.push_back( string(szFile) );

		memset( szLine, 0, 256 );
		memset( szFile, 0, 64 );
	}

	fclose( fp );

	return true;
}


bool ReloadServerConfigDlg::InitReport()
{
	m_ReloadList.InsertColumn( 0, "Reload file name.", LVCFMT_LEFT, 300 );

	int i = 0;
	ReloadListVec::iterator it, end;
	end = m_ReloadFileList.end();
	for( it = m_ReloadFileList.begin(); it != end; ++it, ++i )
	{
		m_ReloadList.InsertItem(i, (*it).c_str() );
	}

	return true;
}



BEGIN_MESSAGE_MAP(ReloadServerConfigDlg, CDialog)
	ON_WM_CREATE()
	ON_BN_CLICKED(IDCANCEL, OnBnClickedCancel)
	ON_BN_CLICKED(ID_RELOAD, OnBnClickedReload)
	ON_BN_CLICKED(IDC_ADDHASHMAP, OnBnClickedAddhashmap)
END_MESSAGE_MAP()


// ReloadServerConfigDlg �޽��� ó�����Դϴ�.

int ReloadServerConfigDlg::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDialog::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  ���⿡ Ư��ȭ�� �ۼ� �ڵ带 �߰��մϴ�.

	if( !LoadReloadList() )
		return -1;

	RECT rt;
	SetRect( &rt, 10, 10, 310, 300 );
	m_ReloadList.Create( WS_BORDER | LVS_REPORT | LVS_SHOWSELALWAYS, rt, this, 0 ); //IDC_RELOAD_LIST );

	m_ReloadList.SetExtendedStyle( /*LVS_EX_BORDERSELECT |*/ LVS_EX_CHECKBOXES | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES );

	if( !InitReport() )
		return -1;

	m_ReloadList.ShowWindow( SW_SHOW );

	return 0;
}

void ReloadServerConfigDlg::OnBnClickedCancel()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	// OnCancel();
	ShowWindow( SW_HIDE );
}

void ReloadServerConfigDlg::OnBnClickedReload()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.

	// Check list�� ��û��.

	char szText[ 64 ];
	ReloadListVec SelectedReloadFileList;
	const int nItemCount = m_ReloadList.GetItemCount();
	for( int i = 0; i < nItemCount; ++i )
	{
		if( m_ReloadList.GetCheck(i) )
		{
			m_ReloadList.GetItemText(i, 0, szText, 64 );
			SelectedReloadFileList.push_back( szText );
		}
	}

	string strFileList;
	ReloadListVec::iterator it, end;
	end = SelectedReloadFileList.end();
	for( it = SelectedReloadFileList.begin(); it != end; ++it )
	{
		if( !strFileList.empty() )
			 strFileList += string(",");
		strFileList += (*it);
	}

	GetKeeperMgr.RequestReloadServerConfig( strFileList );
}

void ReloadServerConfigDlg::OnBnClickedAddhashmap()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.

	CString strNewHashValue;
	m_NewHashValue.GetWindowText( strNewHashValue );

	if( '@' == strNewHashValue[0] )
	{
		if( IDYES  != AfxMessageBox("'@'��ũ������ ������ ����Ǿ��ִ� ��� hashcode�� �������� �Է��� ���� ���� �߰��˴ϴ�. �����Ͻðڽ��ϱ�?",
			MB_YESNO, 0) )
		{
			return;
		}
	}

	GetKeeperMgr.RequestAddHashMap( strNewHashValue.GetBuffer() );
}
