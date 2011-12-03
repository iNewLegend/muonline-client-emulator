#include "stdafx.h"
#include "MMatchAccountPenaltyInfo.h"

MMatchAccountPenaltyInfo::MMatchAccountPenaltyInfo(void)
{
	for(int i = 0; i < MPC_MAX; i++) {
		ClearPenaltyInfo((MPenaltyCode)i);
	}	
}

MMatchAccountPenaltyInfo::~MMatchAccountPenaltyInfo(void)
{
}

void MMatchAccountPenaltyInfo::ClearPenaltyInfo(MPenaltyCode nCode)
{
	memset(&m_PenaltyTable[(int)nCode], 0, sizeof(MPenaltyInfo));
}

void MMatchAccountPenaltyInfo::SetPenaltyInfo(MPenaltyCode nCode, int nPenaltyHour)
{
	if( nCode <= MPC_NONE || nCode >= MPC_MAX ) {
		_ASSERT(0);
		return;
	}

	SetPenaltyInfo(nCode, GetPenaltyEndDate(nPenaltyHour));
}

void MMatchAccountPenaltyInfo::SetPenaltyInfo(MPenaltyCode nCode, SYSTEMTIME sysPenaltyEndDate)
{
	if( nCode <= MPC_NONE || nCode >= MPC_MAX ) {
		_ASSERT(0);
		return;
	}

	m_PenaltyTable[nCode].nPenaltyCode		= nCode;
	m_PenaltyTable[nCode].sysPenaltyEndDate	= sysPenaltyEndDate;
}

SYSTEMTIME MMatchAccountPenaltyInfo::GetPenaltyEndDate(int nPenaltyHour)
{ 
#define IN_HOUR		(__int64)10000000*60*60
#define IN_MINUTE	(__int64)10000000*60
#define IN_SECOND	(__int64)10000000

	SYSTEMTIME sysTime;
	FILETIME ft;
	LARGE_INTEGER lgInt;

	GetLocalTime(&sysTime);						// ���� �ð� ���ϱ�
	SystemTimeToFileTime(&sysTime, &ft);			// SYSTEMTIME -> FILETIME ��ȯ
	memcpy(&lgInt, &ft, sizeof(FILETIME));		// FILETIME -> LARGE_INTEGER ��ȯ
	lgInt.QuadPart += IN_HOUR * nPenaltyHour;	// �ð� ���ϱ�
	memcpy(&ft, &lgInt, sizeof(FILETIME));		// LARGE_INTEGER -> FILETIME ��ȯ
	FileTimeToSystemTime(&ft, &sysTime);			// FILETIME -> SYSTEMMTIME ��ȯ

	return sysTime;
}

bool MMatchAccountPenaltyInfo::IsBlock(MPenaltyCode nCode)
{
	if( nCode <= MPC_NONE || nCode >= MPC_MAX ) {
		_ASSERT(0);
		return false;
	}

	if( m_PenaltyTable[nCode].nPenaltyCode == MPC_NONE ) {
		return false;
	}

	SYSTEMTIME stLocal;
	GetLocalTime( &stLocal );


	if( m_PenaltyTable[nCode].sysPenaltyEndDate.wYear < stLocal.wYear )			return false;
	else if( m_PenaltyTable[nCode].sysPenaltyEndDate.wYear > stLocal.wYear )	return true;

	if( m_PenaltyTable[nCode].sysPenaltyEndDate.wMonth < stLocal.wMonth )		return false;
	else if( m_PenaltyTable[nCode].sysPenaltyEndDate.wMonth > stLocal.wMonth )	return true;

	if( m_PenaltyTable[nCode].sysPenaltyEndDate.wDay < stLocal.wDay )			return false;
	else if( m_PenaltyTable[nCode].sysPenaltyEndDate.wDay > stLocal.wDay )		return true;

	if( m_PenaltyTable[nCode].sysPenaltyEndDate.wHour < stLocal.wHour )			return false;
	else if( m_PenaltyTable[nCode].sysPenaltyEndDate.wHour > stLocal.wHour )	return true;

	if( m_PenaltyTable[nCode].sysPenaltyEndDate.wMinute < stLocal.wMinute )		return false;

	return true;
}
