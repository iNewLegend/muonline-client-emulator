// MScheduleMaker.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "PatchInterface.h"
#include "MScheduleMaker.h"
#include ".\mschedulemaker.h"
#include "KeeperManager.h"
#include "MMatchGlobal.h"
#include "MMatchSchedule.h"


// MScheduleMaker ��ȭ �����Դϴ�.

IMPLEMENT_DYNAMIC(MScheduleMaker, CDialog)
MScheduleMaker::MScheduleMaker(CWnd* pParent /*=NULL*/)
	: CDialog(MScheduleMaker::IDD, pParent), m_nScheduleListCount( 0 )
{
}

MScheduleMaker::~MScheduleMaker()
{
}


void MScheduleMaker::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_SCHEDULE_TYPE, m_cbScheduleType);
	DDX_Control(pDX, IDC_SCHEDULE_DAY, m_cbScheduleDay);
	DDX_Control(pDX, IDC_SCHEDULE_YEAR, m_cbScheduleYear);
	DDX_Control(pDX, IDC_SCHEDULE_MONTH, m_cbScheduleMonth);
	DDX_Control(pDX, IDC_SCHEDULE_HOUR, m_cbScheduleHour);
	DDX_Control(pDX, IDC_SCHEDULE_MIN, m_cbScheduleMin);
	DDX_Control(pDX, IDC_SCHEDULE_COMMAND, m_cbScheduleCommand);
	DDX_Control(pDX, IDC_ANNOUNCE, m_edtAnnounce);
	DDX_Control(pDX, IDC_SCHEDULE_COUNT, m_edtCount);
}


void MScheduleMaker::InitScheduleTypeComboBox()
{
	for( int i = 0; i < SCHEDULE_TYPE_MAX_COUNT; ++i )
		m_cbScheduleType.InsertString( i, SCHEDULE_TYPE[i] );
}


void MScheduleMaker::InitScheduleRelativeYearComboBox()
{
	m_cbScheduleYear.InsertString( 0, SCHEDULE_YEAR[0] );

	for( int i = 1; i < 7; ++i )
		m_cbScheduleYear.InsertString( i, SCHEDULE_YEAR[i] );
}


void MScheduleMaker::InitScheduleYearComboBox()
{
	m_cbScheduleYear.InsertString( 0, SCHEDULE_YEAR[0] );

	for( int i = 7; i < SCHEDULE_YEAR_COUNT; ++i )
		m_cbScheduleYear.InsertString( i - 6, SCHEDULE_YEAR[i] );
}


void MScheduleMaker::InitScheduleMonthComboBox()
{
	for( int i = 0; i < SCHEDULE_MONTH_COUNT; ++i )
		m_cbScheduleMonth.InsertString( i, SCHEDULE_MONTH[i] );
}


void MScheduleMaker::InitScheduleDayComboBox()
{
	const int nMaxDay = static_cast< int >( GetMaxDay() ) + 2;
	for( int i = 0; i < nMaxDay; ++i )
		m_cbScheduleDay.InsertString( i, SCHEDULE_DAY[i] );
}


void MScheduleMaker::InitScheduleHourComboBox()
{
	for( int i = 0; i < SCHEDULE_HOUR_COUNT; ++i )
		m_cbScheduleHour.InsertString( i, SCHEDULE_HOUR[i] );
}


void MScheduleMaker::InitScheduleMinComboBox()
{
	for( int i = 0; i < SCHEDULE_MIN_COUNT; ++i )
		m_cbScheduleMin.InsertString( i, SCHEDULE_MIN[i] );
}


void MScheduleMaker::InitScheduleCmdComboBox()
{
	for( int i = 0; i < SCHEDULE_CMD_COUNT; ++i )
		m_cbScheduleCommand.InsertString( i, SCHEDULE_CMD[i] );
}


bool MScheduleMaker::Init()
{
	InitScheduleCmdComboBox();

	SetDefaultDataAllMemberWnd();
	return true;
}


const CScheduleInfo* MScheduleMaker::GetScheduleInfo()
{
	if( CheckInputData() )
	{
		CString strCount;
		CString strAnnounce;

		m_edtCount.GetWindowText( strCount );
		m_edtAnnounce.GetWindowText( strAnnounce );

		const int nType	 = GetScheduleType();
		const int nYear	 = GetYear(); 
		const int nMonth = GetMonth();
		const int nDay	 = GetDay();
		const int nHour	 = GetHour();
		const int nMin	 = GetMin();
		const int nCount = atoi( strCount.GetBuffer() );
		const int nCmd	 = GetCommandType();

		return new CScheduleInfo( nType, nYear, nMonth,
			nDay, nHour, nMin, nCount, 
			nCmd, strAnnounce.GetBuffer() );
	}

	return 0;
}


bool MScheduleMaker::CheckInputData()
{
	// ���� ���� ���� �˻�.
	if( (0 == m_cbScheduleType.GetCurSel()) || (0 == m_cbScheduleCommand.GetCurSel())  )
	{
		AfxMessageBox( "Ŀ�ǵ� �����̳� ������ ���������� Ȯ���� �ּ���." );
		return false;
	}

	// ��¥ �˻�.
	if( (0 == m_cbScheduleYear.GetCurSel()) || (0 == m_cbScheduleMonth.GetCurSel()) ||
		(0 == m_cbScheduleDay.GetCurSel()) || (0 == m_cbScheduleHour.GetCurSel()) ||
		(0 == m_cbScheduleMin.GetCurSel()) )
	{
		AfxMessageBox( "�������� ���� �ð��� �ִ��� Ȯ���� �ּ���." );
		return false;
	}

	// Ŀ�ǵ忡 ���õ� �κ� �˻�.
	if( KMSC_ANNOUNCE == GetCommandType() )
	{
		if( !IsAnnounceValid() )
			return false;
	
		if( KMST_REPEAT == GetScheduleType() )
		{
			if( !CheckDateForCountAndRepeatSchedule() )
				return false;
		}
		else if( KMST_COUNT == GetScheduleType() )
		{
			if( !CheckDateForCountAndRepeatSchedule() )
				return false;

			if( !IsCountRangeValid() )
				return false;
		}
		else if( KMST_ONCE == GetScheduleType() )
		{
			if( !CheckDateForOnceSchedule() )
				return false;
		}
	}
	else if( KMSC_STOP_SERVER )
	{
		if( !CheckDateForOnceSchedule() )
			return false;
	}
	else if( KMSC_RESTART_SERVER )
	{
		if( !CheckDateForOnceSchedule() )
			return false;
	}
	else
	{
		AfxMessageBox( "Ŀ�ǵ� ������ Ȯ���� �ּ���." );
		return false;
	}

	return true;
}


bool MScheduleMaker::CheckDateForCountAndRepeatSchedule()
{
	// �ݺ��۾��� ��� �ϳ��� ���ڰ� 0���� Ŀ�� ��.
	if( (1 == m_cbScheduleYear.GetCurSel()) && (1 == m_cbScheduleMonth.GetCurSel()) &&
		(1 == m_cbScheduleDay.GetCurSel()) && (1 == m_cbScheduleHour.GetCurSel()) &&
		(1 == m_cbScheduleMin.GetCurSel()) )
	{
		AfxMessageBox( "�ϳ��� 0���� ū �ð��� �־�� �մϴ�." );
		return false;
	}

	return true;
}


const SYSTEMTIME MScheduleMaker::GetCurTime()
{
	SYSTEMTIME stTime;
	GetSystemTime( &stTime );

	stTime.wHour += 9;
	if( 24 <= stTime.wHour )
	{
		stTime.wHour -= 24;
		++stTime.wDay;
	}
	if( GetMaxDay() <= stTime.wDay )
	{
		stTime.wDay -= GetMaxDay();
		++stTime.wMonth;
	}
	if( 12 < stTime.wMonth )
	{
		stTime.wMonth -= 12;
		++stTime.wYear;
	}

	return stTime;
}


bool MScheduleMaker::CheckDateForOnceSchedule()
{
	// ������ �ð��߿� 0�� ����Ҽ� ����.
	if( (2 > m_cbScheduleMonth.GetCurSel()) &&	(2 > m_cbScheduleDay.GetCurSel()) && 
		(2 > m_cbScheduleHour.GetCurSel()) && (2 > m_cbScheduleMin.GetCurSel()) )
	{
		AfxMessageBox( "0�� �ش� ���O�ٿ��� ����Ҽ� �����ϴ�." );
		return false;
	}

	if( CheckTimeIsPastForOnceSchedule() )
		return false;

	if( !CheckTimeIsSameForOnceSchedule() )
		return false;

	return true;
}


bool MScheduleMaker::CheckTimeIsPastForOnceSchedule()
{
	const SYSTEMTIME stTime = GetCurTime();

	// ���� �ð����� ������ �˻�.
	bool bPastTime = false;
	bool bContinue = false;

	if( GetYear() < (stTime.wYear % 100) )
		bPastTime = true;
	else
		bContinue = (GetYear() == (stTime.wYear % 100));

	if( bContinue && (GetMonth() < stTime.wMonth) )
	{
		bPastTime = true;
		bContinue = false;
	}
	else
		bContinue = (GetMonth() == stTime.wMonth);

	if( bContinue && (GetDay() < stTime.wDay) )
	{
		bPastTime = true;
		bContinue = false;
	}
	else
		bContinue = (GetDay() == stTime.wDay);

	if( bContinue && (GetHour() < stTime.wHour) )
	{
		bPastTime = true;
		bContinue = false;
	}
	else
		bContinue = (GetHour() == stTime.wHour);

	if( bContinue && (GetMin() < stTime.wMinute) )
		bPastTime = true;

	if( bPastTime )
	{
		AfxMessageBox( "�̹� ���� �ð��� �����Ҽ� �����ϴ�." );
		return true;
	}

	return false;
}


bool MScheduleMaker::CheckTimeIsSameForOnceSchedule()
{
	const SYSTEMTIME stTime = GetCurTime();

	// ���� ���� �ð��� ������ ������ ����� �����Ҽ� �ִٰ� �˷���.
	if( (GetYear() == (stTime.wYear % 100)) && (GetMonth() == stTime.wMonth) && (GetDay() == stTime.wDay) &&
		(GetHour() == stTime.wHour) && (GetMin() == stTime.wMinute) )
	{
		if( IDYES == AfxMessageBox("���� �ð��� ���� �������� ����� �Ǿ ������� ���� �� �ֽ��ϴ�.\n�����Ͻðڽ��ϱ�?", MB_YESNO, 0) )
			return true;
	}

	return true;
}


bool MScheduleMaker::IsCountRangeValid()
{
	// �Էµ� ���� �������� ������ �ִ��� �˻�.
	if( (0 >= m_edtCount.GetWindowTextLength()) || (127 < m_edtCount.GetWindowTextLength()) )
	{
		AfxMessageBox( "�Է��� ���� Ȯ���� �ּ���. �ʹ� Ŭ�� �ֽ��ϴ�." );
		return false;
	}

	char szVal[ 128 ] = {0,};
	m_edtCount.GetWindowText( szVal, 127 );

	if( 0 >= atoi( szVal ) )
	{
		AfxMessageBox( "0���� �������� ����Ҽ� �����ϴ�." );
		return false;
	}

	return true;
}


bool MScheduleMaker::IsAnnounceValid()
{
	// �������� ���� �˻�.
	if( (0 >= m_edtAnnounce.GetWindowTextLength()) || (127 < m_edtAnnounce.GetWindowTextLength()) )
	{
		AfxMessageBox( "�������� ������ Ȯ���� �ּ���. ������ �ʹ� ����� �ֽ��ϴ�." );
		return false;
	}

	// �������� �Է°� �˻�.
	char szVal[ 128 ] = {0,};
	m_edtAnnounce.GetWindowText( szVal, 127 );

	if( 0 == strcmp(szVal, "���������Է�") )
	{
		AfxMessageBox( "���������� �Է��� �ּ���." );
		return false;
	}

	return true;
}


void MScheduleMaker::DisableAllMemberWnd()
{
	m_cbScheduleType.EnableWindow( false );
	m_cbScheduleYear.EnableWindow( false );
	m_cbScheduleMonth.EnableWindow( false );
	m_cbScheduleDay.EnableWindow( false );
	m_cbScheduleHour.EnableWindow( false );
	m_cbScheduleMin.EnableWindow( false );
	m_cbScheduleCommand.EnableWindow( false );
	m_edtCount.EnableWindow( false );
	m_edtAnnounce.EnableWindow( false );

	m_edtCount.SetWindowText( "" );
	m_edtAnnounce.SetWindowText( "" );
}


void MScheduleMaker::DesableDateWnd()
{
	m_cbScheduleYear.EnableWindow( false );
	m_cbScheduleMonth.EnableWindow( false );
	m_cbScheduleDay.EnableWindow( false );
	m_cbScheduleHour.EnableWindow( false );
	m_cbScheduleMin.EnableWindow( false );
	m_edtCount.EnableWindow( false );

	m_edtCount.SetWindowText( "" );
}


void MScheduleMaker::SetDefaultDataAllMemberWnd()
{
	m_cbScheduleType.SetCurSel( 0 );
	m_cbScheduleYear.SetCurSel( 0 );
	m_cbScheduleMonth.SetCurSel( 0 );
	m_cbScheduleDay.SetCurSel( 0 );
	m_cbScheduleHour.SetCurSel( 0 );
	m_cbScheduleMin.SetCurSel( 0 );
	m_cbScheduleCommand.SetCurSel( 1 );
	m_edtCount.SetWindowText( "COUNT" );
	m_edtAnnounce.SetWindowText( "���������Է�" );
}


void MScheduleMaker::MakeAnnounceScheduleType()
{
	m_cbScheduleType.ResetContent();

	// repeat, count, once.
	for( int i = 0; i < SCHEDULE_TYPE_MAX_COUNT; ++i )
		m_cbScheduleType.InsertString( i, SCHEDULE_TYPE[i] );

	m_nScheduleListCount = SCHEDULE_TYPE_MAX_COUNT;

	m_cbScheduleType.EnableWindow( true );
	m_edtAnnounce.EnableWindow( true );
	m_cbScheduleType.SetCurSel( 0 );
}


void MScheduleMaker::MakeStopServerScheduleType()
{
	m_cbScheduleType.ResetContent();

	// once.
	m_cbScheduleType.InsertString( 0, SCHEDULE_TYPE[0] );
	m_cbScheduleType.InsertString( 1, SCHEDULE_TYPE[3] );

	m_nScheduleListCount = 2;

	m_cbScheduleType.EnableWindow( true );
	m_edtAnnounce.EnableWindow( true );
	m_cbScheduleType.SetCurSel( 0 );
}


void MScheduleMaker::MakeRestartServerScheduleType()
{
	m_cbScheduleType.ResetContent();

	// once.
	m_cbScheduleType.InsertString( 0, SCHEDULE_TYPE[0] );
	m_cbScheduleType.InsertString( 1, SCHEDULE_TYPE[3] );

	m_nScheduleListCount = 2;

	m_cbScheduleType.EnableWindow( true );
	m_cbScheduleType.SetCurSel( 0 );
}


void MScheduleMaker::ResetScheduleEnv()
{
	m_cbScheduleYear.ResetContent();
	m_cbScheduleMonth.ResetContent();
	m_cbScheduleDay.ResetContent();
	m_cbScheduleHour.ResetContent();
	m_cbScheduleMin.ResetContent();

	m_cbScheduleYear.EnableWindow( true );
	m_cbScheduleMonth.EnableWindow( true );
	m_cbScheduleDay.EnableWindow( true );
	m_cbScheduleHour.EnableWindow( true );
	m_cbScheduleMin.EnableWindow( true );
	
	m_edtCount.EnableWindow( false );
}


void MScheduleMaker::DefaultScheduleEnv()
{
	
}


void MScheduleMaker::SetDefaultScheduleEnv()
{
	m_cbScheduleYear.SetCurSel( 0 );
	m_cbScheduleMonth.SetCurSel( 0 );
	m_cbScheduleDay.SetCurSel( 0 );
	m_cbScheduleHour.SetCurSel( 0 );
	m_cbScheduleMin.SetCurSel( 0 );
}


void MScheduleMaker::MakeRepeatScheduleEnv()
{
	ResetScheduleEnv();

	InitScheduleRelativeYearComboBox();
	InitScheduleMonthComboBox();
	InitScheduleDayComboBox();
	InitScheduleHourComboBox();
	InitScheduleMinComboBox();

	SetDefaultScheduleEnv();
}


void MScheduleMaker::MakeCountScheduleEnv()
{
	ResetScheduleEnv();

	InitScheduleRelativeYearComboBox();
	InitScheduleMonthComboBox();
	InitScheduleDayComboBox();
	InitScheduleHourComboBox();
	InitScheduleMinComboBox();

	m_edtCount.EnableWindow( true );

	SetDefaultScheduleEnv();
}


void MScheduleMaker::MakeOnceScheduleEnv()
{
	ResetScheduleEnv();
	
	InitScheduleYearComboBox();
	InitScheduleMonthComboBox();
	InitScheduleDayComboBox();
	InitScheduleHourComboBox();
	InitScheduleMinComboBox();

	SetDefaultScheduleEnv();
}


void MScheduleMaker::Reset()
{
	void DisableAllMemberWnd();

	m_cbScheduleCommand.EnableWindow( true );
	m_cbScheduleCommand.SetCurSel( 0 );
}


const KMS_COMMAND_TYPE MScheduleMaker::GetCommandType() const
{
	return static_cast<KMS_COMMAND_TYPE>(m_cbScheduleCommand.GetCurSel());
}


const KMS_SCHEDULE_TYPE MScheduleMaker::GetScheduleType() const
{
	if( 0 == m_cbScheduleType.GetCurSel() )
		return KMST_NO;

	if( KMSC_ANNOUNCE == GetCommandType() )
	{
		return static_cast<KMS_SCHEDULE_TYPE>(m_cbScheduleType.GetCurSel());
	}
	else if( KMSC_STOP_SERVER == GetCommandType() )
	{
		return KMST_ONCE;
	}
	else if( KMSC_RESTART_SERVER == GetCommandType() )
	{
		return KMST_ONCE;
	}
	else
	{
		ASSERT( 0 && "�߸��� Ŀ�ǵ� Ÿ��" );
	}

	return KMST_NO;
}


const int MScheduleMaker::GetMonth() const
{
	int a = m_cbScheduleMonth.GetCurSel();

	if( 0 < m_cbScheduleMonth.GetCurSel() )
		return atoi( SCHEDULE_MONTH[m_cbScheduleMonth.GetCurSel()] );

	return -1;
}


const int MScheduleMaker::GetDay() const
{
	int a = m_cbScheduleDay.GetCurSel();

	if( 0 < m_cbScheduleDay.GetCurSel() )
		return atoi( SCHEDULE_DAY[m_cbScheduleDay.GetCurSel()] );

	return -1;
}


const int MScheduleMaker::GetHour() const
{
	int a = m_cbScheduleHour.GetCurSel();

	if( 0 < m_cbScheduleHour.GetCurSel() )
		return atoi( SCHEDULE_HOUR[m_cbScheduleHour.GetCurSel()] );

	return -1;
}


const int MScheduleMaker::GetMin() const
{
	int a = m_cbScheduleMin.GetCurSel();

	if( 0 < m_cbScheduleMin.GetCurSel() )
		return atoi( SCHEDULE_MIN[m_cbScheduleMin.GetCurSel()] );

	return -1;
}


const int MScheduleMaker::GetYear() const
{
	if( 0 == m_cbScheduleYear.GetCurSel() )
		return KMST_NO;

	if( KMST_ONCE == GetScheduleType() )
	{
		return atoi( SCHEDULE_YEAR[m_cbScheduleYear.GetCurSel() + 6] ) % 100;
	}
	else
	{
		int a = m_cbScheduleYear.GetCurSel();
		return atoi( SCHEDULE_YEAR[m_cbScheduleYear.GetCurSel()] ) % 100;
	}

	return -1;
}


BEGIN_MESSAGE_MAP(MScheduleMaker, CDialog)
	ON_CBN_SELCHANGE(IDC_SCHEDULE_TYPE, OnCbnSelchangeScheduleType)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_CBN_SELCHANGE(IDC_SCHEDULE_MONTH, OnCbnSelchangeScheduleMonth)
	ON_BN_CLICKED(IDCANCEL, OnBnClickedCancel)
ON_EN_SETFOCUS(IDC_SCHEDULE_COUNT, OnEnSetfocusScheduleCount)
ON_EN_SETFOCUS(IDC_ANNOUNCE, OnEnSetfocusAnnounce)
ON_CBN_SELCHANGE(IDC_SCHEDULE_COMMAND, OnCbnSelchangeScheduleCommand)
ON_EN_CHANGE(IDC_ANNOUNCE, OnEnChangeAnnounce)
END_MESSAGE_MAP()


// MScheduleMaker �޽��� ó�����Դϴ�.

void MScheduleMaker::OnCbnSelchangeScheduleType()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.

	DesableDateWnd();

	// switch( m_cbScheduleType.GetCurSel() )
	switch( GetScheduleType() )
	{
	case 0 :
		{
		}
		break;

	case KMST_REPEAT :
		{
			MakeRepeatScheduleEnv();
		}
		break;

	case KMST_COUNT :
		{
			MakeCountScheduleEnv();
		}
		break;

	case KMST_ONCE :
		{
			MakeOnceScheduleEnv();
		}
		break;

	default :
		{
			ASSERT( 0 );
		}
		break;
	}
}


void MScheduleMaker::OnBnClickedOk()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.

	ShowWindow( SW_SHOW );

	const CScheduleInfo* pScheduleInfo = GetScheduleInfo();
	if( 0 != pScheduleInfo )
	{
		GetKeeperMgr.RequestKeeperManagerSchedule( pScheduleInfo );
		delete pScheduleInfo;
	}
	else
		AfxMessageBox( "���� ���� ����." );

	// OnOK();
}


void MScheduleMaker::OnCbnSelchangeScheduleMonth()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
}


void MScheduleMaker::OnBnClickedCancel()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	ShowWindow( SW_HIDE );
	// OnCancel();
}


BOOL MScheduleMaker::Create(UINT nIDTemplate, CWnd* pParentWnd)
{
	// TODO: ���⿡ Ư��ȭ�� �ڵ带 �߰� ��/�Ǵ� �⺻ Ŭ������ ȣ���մϴ�.

	if( CDialog::Create(nIDTemplate, pParentWnd) )
	{
		DisableAllMemberWnd();

		return TRUE;
	}

	return FALSE;
}


void MScheduleMaker::OnEnSetfocusScheduleCount()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.

	m_edtCount.SetWindowText( "" );
}	
void MScheduleMaker::OnEnSetfocusAnnounce()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	m_edtAnnounce.SetWindowText( "" );
}

void MScheduleMaker::OnCbnSelchangeScheduleCommand()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.

	enum COMM_TYPE
	{
		CMT_ANNOUNCE = 1,
		CMT_STOP_SERVER,
		CMT_RESTART_SERVER,
	};

	DisableAllMemberWnd();

	m_cbScheduleCommand.EnableWindow( true );

	switch( m_cbScheduleCommand.GetCurSel() )
	{
	case CMT_ANNOUNCE :
		{
			MakeAnnounceScheduleType();
		}
		break;

	case CMT_STOP_SERVER :
		{
			MakeStopServerScheduleType();
		}
		break;

	case CMT_RESTART_SERVER :
		{
			MakeRestartServerScheduleType();
		}
		break;

	default :
		{
			ASSERT( 0 && "�߸��� Ŀ�ǵ�." );
		}
		break;
	}
}

void MScheduleMaker::OnEnChangeAnnounce()
{
	// TODO:  RICHEDIT ��Ʈ���� ��� �� ��Ʈ����
	// CDialog::����ũ�� OR �����Ͽ� ������ ENM_CHANGE �÷��׸� �����Ͽ�
	// CRichEditCtrl().SetEventMask()�� ȣ���ϵ��� OnInitDialog() �Լ��� ���������� ������
	// �� �˸��� ������ �ʽ��ϴ�.

	// TODO:  ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
}
