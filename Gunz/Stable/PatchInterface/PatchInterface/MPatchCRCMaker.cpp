// MPatchCRCMaker.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "PatchInterface.h"
#include "MPatchCRCMaker.h"
#include "KeeperManager.h"
#include ".\mpatchcrcmaker.h"
#include "mcrc32.h"


// MPatchCRCMaker ��ȭ �����Դϴ�.

IMPLEMENT_DYNAMIC(MPatchCRCMaker, CDialog)
MPatchCRCMaker::MPatchCRCMaker(CWnd* pParent /*=NULL*/)
	: CDialog(MPatchCRCMaker::IDD, pParent), m_dwServerCRC32( 0 ), m_dwAgentCRC32( 0 )
{
}

MPatchCRCMaker::~MPatchCRCMaker()
{
}

void MPatchCRCMaker::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_SERVER_FILE_NAME, m_edtServerName);
	DDX_Control(pDX, IDC_AGENT_FILE_NAME, m_edtAgentName);
}


DWORD MPatchCRCMaker::MakeFileCRC32( const char* pszFileName )
{
	if( 0 == pszFileName )
		return 0;

	// ���̳ʸ��� ��� �����͸� ����.
	CFile file;
	CFileException e;
	if( !file.Open(pszFileName, CFile::modeRead | CFile::typeBinary, &e) )
	{
		char szErr[ 1024 ] = {0,};
		char szMsg[ 2048 ] = {0, };

		e.GetErrorMessage( szErr, 1024 );

		sprintf( szMsg, "���� ���� ����. %s", szErr );
		AfxMessageBox( szMsg, 0, 0 );
		return 0;
	}

	const unsigned int nFileLen = static_cast< unsigned int >( file.GetLength() );
	char* szFileData = new char[ nFileLen + 1 ];
	file.Read( szFileData, nFileLen );
	file.Close();

	// ���� �����ͷ� CRC32����.
	return MCRC32::BuildCRC32( reinterpret_cast<BYTE*>(szFileData), nFileLen );
}


const string MPatchCRCMaker::GetFileName()
{
	OPENFILENAME OFN;

	char lpstrFile[MAX_PATH]="";
	char szFileTitle[ MAX_PATH ] = "";
	char szTempName[ MAX_PATH ] = "";

	memset(&OFN, 0, sizeof(OPENFILENAME));
	OFN.lStructSize = sizeof(OPENFILENAME);
	OFN.hwndOwner= 0;
	OFN.lpstrFilter="��������(*.zip)\0*.zip";
	OFN.lpstrFile=lpstrFile;
	OFN.lpstrFileTitle = szFileTitle;
	OFN.lpTemplateName = szTempName;
	OFN.nMaxFile= MAX_PATH;
	OFN.nMaxFileTitle = MAX_PATH;
	OFN.lpstrInitialDir=m_strRootDir.c_str();

	if( !GetOpenFileName(&OFN) )
	{
		AfxMessageBox( "���� ���� ����.", 0, 0 );
		return string( "" );
	}

    return string( lpstrFile );
}


void MPatchCRCMaker::Clear()
{
	m_dwServerCRC32 = 0;
	m_dwAgentCRC32 = 0;
	m_edtServerName.SetWindowText( "" );
	m_edtAgentName.SetWindowText( "" );
}


BEGIN_MESSAGE_MAP(MPatchCRCMaker, CDialog)
	ON_BN_CLICKED(IDC_FIND_SERVER_FILE, OnBnClickedFindServerFile)
	ON_BN_CLICKED(IDC_FIND_AGENT_FILE, OnBnClickedFindAgentFile)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, OnBnClickedCancel)
	// ON_BN_CLICKED(IDC_FIND_GUNZ_CLIENT_FILE, OnBnClickedFindGunzClientFile)
	// ON_BN_CLICKED(ID_UPDATE_SERVER_CRC_SETTING, OnBnClickedUpdateServerCrcSetting)
END_MESSAGE_MAP()


// MPatchCRCMaker �޽��� ó�����Դϴ�.

void MPatchCRCMaker::OnBnClickedFindServerFile()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	m_edtServerName.SetWindowText( GetFileName().c_str() );
}

void MPatchCRCMaker::OnBnClickedFindAgentFile()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	m_edtAgentName.SetWindowText( GetFileName().c_str() );
}

void MPatchCRCMaker::OnBnClickedOk()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.

	// �� ��ġ ������ ���������� ���õǾ� �ֳ� Ȯ����.
	const int nServerNameLen	 = m_edtServerName.GetWindowTextLength();
	const int nAgentNameLen		 = m_edtAgentName.GetWindowTextLength();
	
	if( (0 != nServerNameLen) && (9 < nServerNameLen) )
	{
		CString strServerName;
		m_edtServerName.GetWindowText( strServerName );
		m_dwServerCRC32 = MakeFileCRC32( strServerName.GetBuffer() );
	}
	else
	{
		if( IDYES != AfxMessageBox("Server ��ġ ������ ���õ��� �ʾҽ��ϴ�.\n��������ϸ� Server�� ��ġ���� �ʽ��ϴ�.\n��� ���� �Ͻðڽ��ϱ�?", MB_YESNO, 0) )
			return;					
	}

	if( (0 != nAgentNameLen) && (8 < nAgentNameLen) )
	{
		CString strAgentName;
		m_edtAgentName.GetWindowText( strAgentName );
		m_dwAgentCRC32  = MakeFileCRC32( strAgentName.GetBuffer() );
	}
	else
	{
		if( IDYES != AfxMessageBox("Agent��ġ ������ ���õ��� �ʾҽ��ϴ�.\n��������ϸ� Agent�� ��ġ���� �ʽ��ϴ�.\n��� ���� �Ͻðڽ��ϱ�?", MB_YESNO, 0) )
			return ;
	}

	char szMsg[ 1024 ] = {0,};
	sprintf( szMsg, "Maked crc info\nserver.zip crc:%u\nagent.zip  crc:%u\nOutput dir = %s", 
		m_dwServerCRC32, m_dwAgentCRC32, m_strListFilePath.c_str() );
	AfxMessageBox( szMsg );

	MakeListFile();

	// OnOK();
	ShowWindow( SW_HIDE );
}

void MPatchCRCMaker::OnBnClickedCancel()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	// OnCancel();
	ShowWindow( SW_HIDE );
}


void MPatchCRCMaker::MakeListFile()
{
	ASSERT( !m_strListFilePath.empty() );

	FILE* fp = fopen( m_strListFilePath.c_str(), "w" );
	if( 0 == fp )
	{
		AfxMessageBox( "LIST.TXT���� ���� ����." );
		return;
	}

	char szMsg[ 1024 ] = {0,};
	// sprintf( szMsg, "matchserver %u\nmatchagent  %u", m_dwServerCRC32, m_dwAgentCRC32 );
	// fprintf( fp, szMsg );

	if( 0 != m_dwServerCRC32 )
		fprintf( fp, "matchserver %u\n", m_dwServerCRC32 );

	if( 0 != m_dwAgentCRC32 )
		fprintf( fp, "matchagent  %u\n", m_dwAgentCRC32 );

	fclose( fp );
}