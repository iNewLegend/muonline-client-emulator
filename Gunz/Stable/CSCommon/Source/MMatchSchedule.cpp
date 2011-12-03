#include "stdafx.h"
#include "MServer.h"
#include "MMatchSchedule.h"

MMatchDayOfMonth::MMatchDayOfMonth()
{
	Init();
}

MMatchDayOfMonth::~MMatchDayOfMonth()
{
}

void MMatchDayOfMonth::Init()
{
	m_cDayOfMonth[ 0 ] = 31;
	
	struct tm* ptm = MMatchGetLocalTime();
	if( 0 != (ptm->tm_year % 4) )
		m_cDayOfMonth[ 1 ] = 28;
	else
		m_cDayOfMonth[ 1 ] = 29;

	m_cDayOfMonth[ 2 ] = 31;
	m_cDayOfMonth[ 3 ] = 30;
	m_cDayOfMonth[ 4 ] = 31;
	m_cDayOfMonth[ 5 ] = 30;
	m_cDayOfMonth[ 6 ] = 31;
	m_cDayOfMonth[ 7 ] = 31;
	m_cDayOfMonth[ 8 ] = 30;
	m_cDayOfMonth[ 9 ] = 31;
	m_cDayOfMonth[ 10 ] = 30;
	m_cDayOfMonth[ 11 ] = 31;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////

MMatchScheduleData::MMatchScheduleData(void) : m_cYear( 0 ), m_cMonth( 0 ), m_cDay( 0 ), m_cHour( 0 ), m_cMin( 0 ), 
	m_pCmd( 0 ), m_nType( -1 ), m_bIsNeedDelete( false ), // ,nID( 0 ),
	m_cNextYear( 0 ), m_cNextMonth( 0 ), m_cNextDay( 0 ), m_cNextHour( 0 ), m_cNextMin( 0 ), m_nCount( 0 )
{
}

MMatchScheduleData::~MMatchScheduleData(void)
{
}

void MMatchScheduleData::CorrectTime()
{
	const struct tm* tmLocalTime = MMatchGetLocalTime();

	// �д����� ������ �ð����� ������ �ʰ� �����ð����� ū�� �˻�.
	if( tmLocalTime->tm_sec > m_nErrorTime ) return;

	// ���� �ð��� ������ ������Ŵ.
	++m_cMin;

	if( 59 < m_cMin ){
		m_cMin -= 60;
		++m_cHour;
	}

	if( 23 < m_cHour ){
		m_cHour -= 24;
		++m_cDay;
	}

	// �̹� ���� ���ϱ��� �ִ����� �˾ƿ;� ��... 
	if( GetMaxDay() < m_cDay ){
		m_cDay -= GetMaxDay();
		m_cYear += 1;
	}
	//
}

// return if same, then 0, less -1, greater 1.
int	MMatchScheduleData::CompareCurrentTime()
{
	const struct tm* pLocalTime = MMatchGetLocalTime();

	// ���� ���� �ð��� �����쿡 ����� �ð��� ����.
		
	if( (pLocalTime->tm_year - 100) > GetYear() )
		return -1;
	else if( (pLocalTime->tm_year - 100) < GetYear() )
		return 1;

	if(	(pLocalTime->tm_mon + 1) > GetMonth() )
		return -1;
	else if( (pLocalTime->tm_mon + 1) < GetMonth() )
		return 1;

	if(	pLocalTime->tm_mday > GetDay() )
		return -1;
	else if( pLocalTime->tm_mday < GetDay() )
		return 1;

	if(	pLocalTime->tm_hour > GetHour() ) 
		return -1;
	else if( pLocalTime->tm_hour < GetHour() ) 
		return 1;

	if( pLocalTime->tm_min > GetMin()	) 
		return -1;
	else if( pLocalTime->tm_min < GetMin() ) 
		return 1;

	return 0;
}

bool MMatchScheduleData::SetTimes( const unsigned char cYear,
								   const unsigned char cMonth, 
								   const unsigned char cDay, 
								   const unsigned char cHour, 
								   const unsigned char cMin )
{
	if( 04 > cYear )	return false;
	if( 12 < cMonth )	return false;
	if( 31 < cDay )		return false;
	if( 23 < cHour )	return false;
	if( 59 < cMin )		return false;

	SetYear( cYear );
	SetMonth( cMonth );
	SetDay( cDay );
	SetHour( cHour );
	SetMin( cMin );

	return true;
}

bool MMatchScheduleData::SetNextTimes( const unsigned char cNextYear,
									   const unsigned char cNextMonth, 
									   const unsigned char cNextDay, 
									   const unsigned char cNextHour, 
									   const unsigned char cNextMin )
{
	if( 12 < cNextMonth )	return false;
	if( 31 < cNextDay )		return false;
	if( 23 < cNextHour )	return false;
	if( 59 < cNextMin )		return false;

	SetNextYear( cNextYear );
	SetNextMonth( cNextMonth );
	SetNextDay( cNextDay );
	SetNextHour( cNextHour );
	SetNextMin( cNextMin );

	return true;
}

void MMatchScheduleData::ResetTime()
{
	m_cMin += m_cNextMin;
	if( 59 < m_cMin ){
        m_cMin -= 60;
		++m_cHour;
	}

	m_cHour += m_cNextHour;
	if( 23 < m_cHour ){
		m_cHour -= 24;
		++m_cDay;
	}

	m_cDay += m_cNextDay;
	if( GetMaxDay() < m_cDay ){
		m_cDay -= GetMaxDay();
		++m_cMonth;
	}

	m_cMonth += m_cNextMonth;
	if( 12 < m_cMonth ){
		m_cMonth -= 12;	
		++m_cYear;
	}
}

void MMatchScheduleData::Release()
{
	if( 0 != m_pCmd ){
		delete m_pCmd;
		m_pCmd = 0;
	}
}

/////////////////////////////////////////////////////
// �ð��� �ٽ� ����.
void MMatchRepeatScheduleImpl::Reset( MMatchScheduleData* pScheduleData )
{
	pScheduleData->ResetTime();
}
// �ð� ���� ����.
void MMatchRepeatScheduleImpl::CorrectTime( MMatchScheduleData* pScheduleData )
{
	pScheduleData->CorrectTime();
	// ������ �ص� ���� �ð����� ������ ������ �ִ� ���̹Ƿ� �����쿡�� ������.
	if( 0 > pScheduleData->CompareCurrentTime() ){
		pScheduleData->SetDeleteState( true );
		return;
	}

	// ���� �ð����� ����.
	const struct tm* tmLocalTime = MMatchGetLocalTime();
	pScheduleData->SetTimes( tmLocalTime->tm_year - 100, 
							 tmLocalTime->tm_mon + 1, 
							 tmLocalTime->tm_mday, 
							 tmLocalTime->tm_hour, 
							 tmLocalTime->tm_min );
}

// �ð��� �ٽ� ����.
void MMatchCountScheduleImpl::Reset( MMatchScheduleData* pScheduleData )
{
	// countŸ���� �������� count���� 0�̵Ǹ� ���� �Ǿ�� �Ѵ�.
	pScheduleData->DecreaseCount();
	if( 0 == pScheduleData->GetCount() ) 
		pScheduleData->SetDeleteState( true );

	pScheduleData->ResetTime();
}
// �ð� ���� ����.
void MMatchCountScheduleImpl::CorrectTime( MMatchScheduleData* pScheduleData )
{
	pScheduleData->CorrectTime();
	// ������ �ص� ���� �ð����� ������ ī��Ʈ�� �ϳ� ���ҽ�Ŵ.
	if( 0 > pScheduleData->CompareCurrentTime() ){
		pScheduleData->DecreaseCount();
		if( 0 == pScheduleData->GetCount() )
			pScheduleData->SetDeleteState( true );
		return;
	}

	// ���� �ð����� ����.
	const struct tm* tmLocalTime = MMatchGetLocalTime();
	pScheduleData->SetTimes( tmLocalTime->tm_year - 100, 
							 tmLocalTime->tm_mon + 1, 
							 tmLocalTime->tm_mday, 
							 tmLocalTime->tm_hour, 
							 tmLocalTime->tm_min );
}

// �ð��� �ٽ� ����.
void MMatchOnceScheduleImpl::Reset( MMatchScheduleData* pScheduleData )
{
	pScheduleData->SetDeleteState( true );
}
// �ð� ���� ����.
void MMatchOnceScheduleImpl::CorrectTime( MMatchScheduleData* pScheduleData )
{
	// �����ð����� �ٽ� ��������. 
	const struct tm* tmLocalTime = MMatchGetLocalTime();
	pScheduleData->SetTimes( tmLocalTime->tm_year - 100, 
							 tmLocalTime->tm_mon + 1, 
							 tmLocalTime->tm_mday, 
							 tmLocalTime->tm_hour, 
							 tmLocalTime->tm_min );
}

/////////////////////////////////////////////////////

MMatchScheduleMgr::MMatchScheduleMgr( MServer* pServer ) : m_pServer( pServer ), m_nIndex( 0 ),
	m_tmUpdateTerm( 0 )
{
}

MMatchScheduleMgr::~MMatchScheduleMgr()
{
}

// ������ ������Ʈ ������������ ����ð� ���.
const time_t MMatchScheduleMgr::CalculateElapseUpdateTime()
{
	time_t tmCurTime;
	time( &tmCurTime );
	return ( tmCurTime - m_tmLastUpdateTime );
}

bool MMatchScheduleMgr::Init()
{
	if( !m_ScheduleImplPrototype.Init() ) return false;

	SetLastUpdateTime();
	
	return true;
}

bool MMatchScheduleMgr::IsUpdate()
{
	if( CalculateElapseUpdateTime() > m_tmUpdateTerm ) return true;

	return false;
}

// first	: �߱���. 2004. 11. 08
// last		: �߱���. 2004. 11. 08
// �������� ���������� �˻��Ͽ� �����ϰ�츸 true�� ��ȯ��.
bool MMatchScheduleMgr::CheckData( MMatchScheduleData* pScheduleData ) const
{
	if( 0 == pScheduleData )				return false;
	if( 0 == pScheduleData->GetCommand() )	return false;
	if( 0 > pScheduleData->GetType() )		return false;

	if( 04 > pScheduleData->GetYear() )				return false;
	if( 12 < pScheduleData->GetMonth() )			return false;
	if( GetMaxDay() < pScheduleData->GetDay() )		return false;
	if( 23 < pScheduleData->GetHour() )				return false;
	if( 59 < pScheduleData->GetMin() )				return false;

	if( 12 < pScheduleData->GetNextMonth() )	return false;
	if( 31 < pScheduleData->GetNextDay() )		return false;
	if( 23 < pScheduleData->GetNextHour() )		return false;
	if( 59 < pScheduleData->GetNextMin() )		return false;

	// pScheduleData->nID = m_nIndex;
	// ++m_nIndex;

	return true;
}

// first	: �߱���. 2004. 11. 08
// last		: �߱���. 2004. 11. 08
// �ð��� ���Ͽ� ������츸 true�� ��ȯ��.
bool MMatchScheduleMgr::CompareTime( MMatchScheduleData* pScheduleData )
{
	struct tm* pCurTime = MMatchGetLocalTime();

	bool bIsEqual = true;

	const int nCompareResult = pScheduleData->CompareCurrentTime();

	// ���� �ð��� ��.
	if( 0 < nCompareResult )
		return false;
	else if( 0 == nCompareResult )
		return true;
	else if( 0 > nCompareResult )
		bIsEqual = false;

	// �ð��� �ٸ���� ���������� ��.
	if( !bIsEqual ){
		pScheduleData->GetImpl()->CorrectTime( pScheduleData );

		bIsEqual = true;

		// ������ �ð��� ������ �ٽ� ����.
		if( 0 != pScheduleData->CompareCurrentTime() )
			bIsEqual = false;
	}
	
	return bIsEqual;
}

void MMatchScheduleMgr::SetLastUpdateTime()
{
	// set start time.
	time( &m_tmLastUpdateTime );
}

// first	: �߱���. 2004. 11. 08
// last		: �߱���. 2004. 11. 08
// ������ ����Ʈ�� ������Ʈ ��.
void MMatchScheduleMgr::Update()
{
	if( !IsUpdate() ) return;

	UpdateStaticSchedule();
	UpdateDynamicSchedule();

	SetLastUpdateTime();
}

// first	: �߱���. 2004. 11. 08
// last		: �߱���. 2004. 11. 08
// ������ ���� ���� ���� ������ ������Ʈ.
void MMatchScheduleMgr::UpdateStaticSchedule()
{
	MCommand* pCmd = 0;
	ScheduleVecIter it, end;

	end = m_vecStaticSchedule.end();
	for( it = m_vecStaticSchedule.begin(); it != end; ++it )
	{
		// ����� �ð��ΰ�?
		if( !CompareTime((*it)) ) continue;

		pCmd = (*it)->GetCommand()->Clone();
		if( 0 != pCmd ){
			m_pServer->GetCommandManager()->Post( pCmd );
		}

		// Ȥ�� ���� ���ΰ�?
		// ���� �������� IsNeedDelete()�� true�� ���ڸ� ������Ʈ ���� �ʾ� ����Ǵ°��� ����.
		if( (*it)->IsNeedDelete() ) continue;

		(*it)->GetImpl()->Reset( (*it) );
	}
}

// first	: �߱���. 2004. 11. 08
// last		: �߱���. 2004. 11. 08
// �������� ������ ������Ʈ.
void MMatchScheduleMgr::UpdateDynamicSchedule()
{
	MCommand* pCmd = 0;
	ScheduleLstIter it, end;

	end = m_lstDynamicSchedule.end();
	for( it = m_lstDynamicSchedule.begin(); it != end; )
	{
		if( !CompareTime((*it)) )
		{
			++it;
			continue;
		}

		pCmd = (*it)->GetCommand()->Clone();
		if( 0 != pCmd )
		{
			m_pServer->GetCommandManager()->Post( pCmd );
		}

		(*it)->GetImpl()->Reset( (*it) );

		// �̹��� ���� �����ΰ�?
		if( (*it)->IsNeedDelete() )
		{
			delete *it;
			m_lstDynamicSchedule.erase( it++ );
			continue;
		}

		++it;
	}
}

// first	: �߱���. 2004. 11. 08
// last		: �߱���. 2004. 11. 08
// ���ο� ���� ������ �����.
bool MMatchScheduleMgr::AddDynamicSchedule( MMatchScheduleData* pNewSchedule )
{
	if( !CheckData(pNewSchedule) ) return false;

	if( 0 == pNewSchedule->GetImpl() )
		pNewSchedule->SetImpl( m_ScheduleImplPrototype.GetImpl(pNewSchedule->GetType()) );

	if( 0 == pNewSchedule->GetImpl() ) return false;

	m_lstDynamicSchedule.push_back( pNewSchedule );

	return true;
}

// first	: �߱���. 2004. 11. 08
// last		: �߱���. 2004. 11. 08
// ���� ������ ���.
bool MMatchScheduleMgr::AddStaticSchedule( MMatchScheduleData* pNewSchedule )
{
	if( !CheckData(pNewSchedule) ) return false;

	if( 0 == pNewSchedule->GetImpl() )
		pNewSchedule->SetImpl( m_ScheduleImplPrototype.GetImpl(pNewSchedule->GetType()) );

	if( 0 == pNewSchedule->GetImpl() ) return false;

	m_vecStaticSchedule.push_back( pNewSchedule );

	return true;
}

MMatchScheduleData* MMatchScheduleMgr::MakeRepeatScheduleData( const unsigned char cNextYear,	// ������ ����� ���� ��밪.	ex) 1�� ��.
															   const unsigned char cNextMonth,	// ������ ����� ���� ��밪.	ex) 1�� ��.
															   const unsigned char cNextDay,	// ������ ����� ���� ��밪.	ex) 1�� ��.
															   const unsigned char cNextHour,	// ������ ����� �ð��� ��밪.	ex) 1�ð� ��.
															   const unsigned char cNextMin,	// ������ ����� ���� ��밪.	ex) 1�� ��.
															   MCommand* pCmd					// ������ ���.
															   )
{
	if( 0 == pCmd ) return 0;

	MMatchScheduleData* pData = new MMatchScheduleData;
	if( 0 == pData ) return 0;

	struct tm* tmTime = MMatchGetLocalTime();
	if( 0 == tmTime ) return 0;

	if( !pData->SetTimes(tmTime->tm_year + cNextYear - 100, 
						 tmTime->tm_mon + cNextMonth + 1, 
						 tmTime->tm_mday + cNextDay, 
						 tmTime->tm_hour + cNextHour, 
						 tmTime->tm_min + cNextMin) )
		return 0;

	if( !pData->SetNextTimes(cNextYear, cNextMonth, cNextDay, cNextHour, cNextMin) )
		return 0;

	pData->SetType( MMatchScheduleData::REPEAT );
	pData->SetCommand( pCmd );
	pData->SetErrorTime( static_cast<int>(m_tmUpdateTerm) );

	pData->SetImpl( m_ScheduleImplPrototype.GetImpl(pData->GetType()) );
	if( 0 == pData->GetImpl() ) return 0;

	return pData;
}

MMatchScheduleData* MMatchScheduleMgr::MakeCountScheduleData( const unsigned char cNextYear,	// ������ ����� ���� ��밪.	ex) 1�� ��.
															  const unsigned char cNextMonth,	// ������ ����� ���� ��밪.	ex) 1�� ��.
															  const unsigned char cNextDay,		// ������ ����� ���� ��밪.	ex) 1�� ��.
															  const unsigned char cNextHour,	// ������ ����� �ð��� ��밪.	ex) 1�ð� ��.
															  const unsigned char cNextMin,		// ������ ����� ���� ��밪.	ex) 1�� ��.
															  const unsigned int nCount,		// ����� Ƚ��.
															  MCommand* pCmd					// ������ ���.
															  )
{
	if( 0 == pCmd ) return 0;

	MMatchScheduleData* pData = new MMatchScheduleData;
	if( 0 == pData ) return 0;

	struct tm* tmTime = MMatchGetLocalTime();
	if( 0 == tmTime ) return 0;

	pData->SetCount( nCount );

	if( !pData->SetTimes(tmTime->tm_year + cNextYear - 100, 
					     tmTime->tm_mon + cNextMonth + 1, 
						 tmTime->tm_mday + cNextDay, 
						 tmTime->tm_hour + cNextHour, 
						 tmTime->tm_min + cNextMin) )
		return 0;

	if( !pData->SetNextTimes(cNextYear, cNextMonth, cNextDay, cNextHour, cNextMin) ) 
		return 0;

	pData->SetType( MMatchScheduleData::COUNT );
	pData->SetCommand( pCmd );
	pData->SetErrorTime( static_cast<int>(m_tmUpdateTerm) );

	pData->SetImpl( m_ScheduleImplPrototype.GetImpl(pData->GetType()) );
	if( 0 == pData->GetImpl() ) return 0;

	return pData;
}

MMatchScheduleData* MMatchScheduleMgr::MakeOnceScheduleData( const unsigned char cYear,		// ����� ��.
															 const unsigned char cMonth,	// ����� ��.
															 const unsigned char cDay,		// ����� ��.
															 const unsigned char cHour,		// ����� �ð�.
															 const unsigned char cMin,		// ����� ��.
															 MCommand* pCmd					// ������ ���.	
															 )
{
	if( 0 == pCmd ) return 0;

	MMatchScheduleData* pData = new MMatchScheduleData;
	if( 0 == pData ) return 0;

	// cYear % 100 => �ο� ���ڸ����� �����.
	if( !pData->SetTimes(cYear % 100, cMonth, cDay, cHour, cMin) ) return 0;

	mlog( "\n%u\n", m_tmUpdateTerm );

	pData->SetType( MMatchScheduleData::ONCE );
	pData->SetCommand( pCmd );
	pData->SetErrorTime( static_cast<int>(m_tmUpdateTerm) );

	pData->SetImpl( m_ScheduleImplPrototype.GetImpl(pData->GetType()) );
	if( 0 == pData->GetImpl() ) return 0;

	return pData;
}

// first	: �߱���. 2004. 11. 08
// last		: �߱���. 2004. 11. 08
// ��� ����Ʈ ����.
void MMatchScheduleMgr::Release()
{
	ReleaseStaticSchedule();
	ReleaseDynamicSchedule();
	m_ScheduleImplPrototype.Release();
}

// first	: �߱���. 2004. 11. 08
// last		: �߱���. 2004. 11. 08
// ���� ������ ����.
void MMatchScheduleMgr::ReleaseStaticSchedule()
{
	for_each( m_vecStaticSchedule.begin(), 
			  m_vecStaticSchedule.end(), 
			  MMatchScheduleReleaser() );
	m_vecStaticSchedule.clear();
}

// first	: �߱���. 2004. 11. 08
// last		: �߱���. 2004. 11. 08
// �������� ������ ����.
void MMatchScheduleMgr::ReleaseDynamicSchedule()
{
	for_each( m_lstDynamicSchedule.begin(),
			  m_lstDynamicSchedule.end(),
			  MMatchScheduleReleaser() );
	m_lstDynamicSchedule.clear();
}

// first	: �߱���. 2004. 11. 09.
// last		: �߱���. 2004. 11. 09.
// Impl�� prototypeŬ������ �����.
bool MMatchScheduleMgr::MMatchScheduleImplPrototype::Init()
{
	MMatchScheduleImpl* pImpl;

	pImpl = new MMatchRepeatScheduleImpl;
	if( 0 == pImpl ) return false;
	m_ScheduleImplVec.push_back( pImpl );

	pImpl = new MMatchCountScheduleImpl;
	if( 0 == pImpl ) return false;
	m_ScheduleImplVec.push_back( pImpl );

	pImpl = new MMatchOnceScheduleImpl;
	if( 0 == pImpl ) return false;
	m_ScheduleImplVec.push_back( pImpl );

	return true;
}

// first	: �߱���. 2004. 11. 09.
// last		: �߱���. 2005. 01. 27.
// �ش� ������ Ÿ�Կ� �´� �����ڸ� ��ȯ����.
MMatchScheduleImpl* MMatchScheduleMgr::MMatchScheduleImplPrototype::GetImpl( const unsigned int nTypeID )
{
	if( nTypeID >= m_ScheduleImplVec.size() ) return 0;

	return m_ScheduleImplVec[ nTypeID ];
}

// first	: �߱���. 2004. 11. 09.
// last		: �߱���. 2004. 11. 09.
// ��ϵǾ��ִ� �����ڸ� ������.
void MMatchScheduleMgr::MMatchScheduleImplPrototype::Release()
{
	for_each( m_ScheduleImplVec.begin(), 
			  m_ScheduleImplVec.end(), 
			  MMatchScheduleImplRelease() );
	m_ScheduleImplVec.clear();
}

//////////////////////////////////////////////////////////////////////
struct tm* MMatchGetLocalTime()
{
	__time64_t long_time;

	_time64( &long_time );

	/* Obtain coordinated universal time: */
	return _localtime64( &long_time );
}

char GetMaxDay( const int iMonth )
{
	return MMatchDayOfMonth::GetInst().GetMaxDay( iMonth );
}

char GetMaxDay()
{
	return GetMaxDay( MMatchGetLocalTime()->tm_mon + 1 );
}


bool AddDynamicSchedule( MMatchScheduleMgr* pScheduleMgr, const int nType, MCommand* pCmd, const int nYear, const int nMonth, const int nDay, const int nHour, const int nMin, const int nCount )
{
	if( 0 == pScheduleMgr ) return false;

	unsigned char nConYear = nYear % 100;

	switch( nType )
	{
	case MMatchScheduleData::REPEAT :
		{
			MMatchScheduleData* pScheduleData = pScheduleMgr->MakeRepeatScheduleData( static_cast<unsigned char>(nYear), 
																					  static_cast<unsigned char>(nMonth),
																					  static_cast<unsigned char>(nDay), 
																					  static_cast<unsigned char>(nHour), 
																					  static_cast<unsigned char>(nMin),
																					  pCmd );
			if( 0 != pScheduleData )
			{
				if( !pScheduleMgr->AddDynamicSchedule(pScheduleData) )
					delete pScheduleData;
				return true;
			}
			else
			{
				return false;
			}
		}
		break;

	case MMatchScheduleData::COUNT :
		{
			MMatchScheduleData* pScheduleData = pScheduleMgr->MakeCountScheduleData( static_cast<unsigned char>(nYear), 
																					 static_cast<unsigned char>(nMonth),
																					 static_cast<unsigned char>(nDay), 
																					 static_cast<unsigned char>(nHour), 
																					 static_cast<unsigned char>(nMin),
																					 static_cast<unsigned int>(nCount),
																					 pCmd );

			if( 0 != pScheduleData )
			{
				if( !pScheduleMgr->AddDynamicSchedule(pScheduleData) )
					delete pScheduleData;
				return true;
			}
			else
			{
				return false;
			}
		}
		break;

	case MMatchScheduleData::ONCE :
		{
			MMatchScheduleData* pScheduleData = pScheduleMgr->MakeOnceScheduleData( static_cast<unsigned char>(nYear), 
																					static_cast<unsigned char>(nMonth),
																					static_cast<unsigned char>(nDay), 
																					static_cast<unsigned char>(nHour), 
																					static_cast<unsigned char>(nMin),
																					pCmd );

			if( 0 != pScheduleData )
			{
				if( !pScheduleMgr->AddDynamicSchedule(pScheduleData) )
					delete pScheduleData;
				return true;
			}
			else
			{
				return false;
			}
		}
		break;

	default :
		{
			ASSERT( 0 );
			return false;
		}
		break;
	}
}