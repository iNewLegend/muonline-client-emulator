#include "MMatchServer.h"
#include "MMatchSchedule.h"

MMatchScheduleData::MMatchScheduleData(void) : m_cMonth( 0 ), m_cDay( 0 ), m_cHour( 0 ), m_cMin( 0 ), m_pCmd( 0 ), m_nType( -1 ), m_bIsNeedDelete( false ), // ,nID( 0 )
	m_cNextMonth( 0 ), m_cNextDay( 0 ), m_cNextHour( 0 ), m_cNextMin( 0 ), m_nCount( 0 )
{
}

MMatchScheduleData::~MMatchScheduleData(void)
{
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

	m_cMonth += m_cNextMonth;
	if( 12 < m_cMonth )
		m_cMonth -= 12;	
}

void MMatchScheduleData::Release()
{
	delete m_pCmd;
}

/////////////////////////////////////////////////////

void MMatchRepeatScheduleImpl::Reset( MMatchScheduleData* pScheduleData )
{
	pScheduleData->ResetTime();
}

void MMatchCountScheduleImpl::Reset( MMatchScheduleData* pScheduleData )
{
	// countŸ���� �������� count���� 0�̵Ǹ� ���� �Ǿ�� �Ѵ�.
	pScheduleData->DecreaseCount();
	if( 0 == pScheduleData->GetCount() ) 
		pScheduleData->SetDeleteState( true );

	pScheduleData->ResetTime();
}

void MMatchOnceScheduleImpl::Reset( MMatchScheduleData* pScheduleData )
{
	pScheduleData->SetDeleteState( true );
}

/////////////////////////////////////////////////////

MMatchScheduleMgr::MMatchScheduleMgr( MMatchServer* pMatchServer ) : m_pMatchServer( pMatchServer ), m_nIndex( 0 )
{
}

MMatchScheduleMgr::~MMatchScheduleMgr()
{
}

bool MMatchScheduleMgr::Init()
{
	if( !m_ScheduleImplPrototype.Init() ) return false;

	return true;
}

// first	: �߱���. 2004. 11. 08
// last		: �߱���. 2004. 11. 08
// �������� ���������� �˻��Ͽ� �����ϰ�츸 true�� ��ȯ��.
bool MMatchScheduleMgr::CheckData( MMatchScheduleData* pScheduleData ) const
{
	if( 0 == pScheduleData )				return false;
	if( 0 == pScheduleData->GetCommand() )	return false;
	if( 0 > pScheduleData->GetType() )		return false;

	// pScheduleData->nID = m_nIndex;
	// ++m_nIndex;

	return true;
}

// first	: �߱���. 2004. 11. 08
// last		: �߱���. 2004. 11. 08
// �ð��� ���Ͽ� ������츸 true�� ��ȯ��.
bool MMatchScheduleMgr::CompareTime( MMatchScheduleData* pScheduleData ) const
{
	tm* pCurTime = MMatchGetLocalTime();

	if( (pCurTime->tm_mon + 1) != pScheduleData->GetMonth() )	return false;
	if( pCurTime->tm_mday != pScheduleData->GetDay() )			return false;
	if( pCurTime->tm_hour != pScheduleData->GetHour() )			return false;
	if( pCurTime->tm_min != pScheduleData->GetMin() )			return false;

	return true;
}

// first	: �߱���. 2004. 11. 08
// last		: �߱���. 2004. 11. 08
// ������ ����Ʈ�� ������Ʈ ��.
void MMatchScheduleMgr::Update()
{
	UpdateStaticSchedule();
	UpdateAnnounceSchedule();
}

// first	: �߱���. 2004. 11. 08
// last		: �߱���. 2004. 11. 08
// ������ ���� ���� ���� ������ ������Ʈ.
void MMatchScheduleMgr::UpdateStaticSchedule()
{
	MCommand* pCmd = 0;
	ScheduleVecIter it, end;

	end = m_vecStaticSchedule.end();
	for( it = m_vecStaticSchedule.begin(); it != end; ++it ){
		// ����� �ð��ΰ�?
		if( !CompareTime((*it)) ) continue;

		pCmd = (*it)->GetCommand()->Clone();
		if( 0 != pCmd ){
			m_pMatchServer->GetCommandManager()->Post( pCmd );
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
void MMatchScheduleMgr::UpdateAnnounceSchedule()
{
	MCommand* pCmd = 0;
	ScheduleLstIter it, end;

	end = m_lstAnnounceSchedule.end();
	for( it = m_lstAnnounceSchedule.begin(); it != end; ++it ){
		if( !CompareTime((*it)) ) continue;

		pCmd = (*it)->GetCommand()->Clone();
		if( 0 != pCmd ){
			m_pMatchServer->GetCommandManager()->Post( pCmd );
		}

		// �̹��� ���� �����ΰ�?
		if( (*it)->IsNeedDelete() ){
			m_lstAnnounceSchedule.erase( it-- );
			continue;
		}

		(*it)->GetImpl()->Reset( (*it) );
	}
}

// first	: �߱���. 2004. 11. 08
// last		: �߱���. 2004. 11. 08
// ���ο� ���� ������ �����.
void MMatchScheduleMgr::AddAnnounceSchedule( MMatchScheduleData* pNewSchedule )
{
	if( !CheckData(pNewSchedule) ) return;

	pNewSchedule->SetImpl( m_ScheduleImplPrototype.GetImpl(pNewSchedule->GetType()) );

	if( 0 == pNewSchedule->GetImpl() ) return;

	m_lstAnnounceSchedule.push_back( pNewSchedule );
}

// first	: �߱���. 2004. 11. 08
// last		: �߱���. 2004. 11. 08
// ���� ������ ���.
void MMatchScheduleMgr::AddStaticSchedule( MMatchScheduleData* pNewSchedule )
{
	if( !CheckData(pNewSchedule) ) return;

	pNewSchedule->SetImpl( m_ScheduleImplPrototype.GetImpl(pNewSchedule->GetType()) );

	if( 0 == pNewSchedule->GetImpl() ) return;

	m_vecStaticSchedule.push_back( pNewSchedule );
}

// first	: �߱���. 2004. 11. 08
// last		: �߱���. 2004. 11. 08
// ��� ����Ʈ ����.
void MMatchScheduleMgr::Release()
{
	ReleaseStaticSchedule();
	ReleaseAnnounceSchedule();
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
void MMatchScheduleMgr::ReleaseAnnounceSchedule()
{
	for_each( m_lstAnnounceSchedule.begin(),
			  m_lstAnnounceSchedule.end(),
			  MMatchScheduleReleaser() );
	m_lstAnnounceSchedule.clear();
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
// last		: �߱���. 2004. 11. 09.
// �ش� ������ Ÿ�Կ� �´� �����ڸ� ��ȯ����.
MMatchScheduleImpl* MMatchScheduleMgr::MMatchScheduleImplPrototype::GetImpl( const int nTypeID )
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
}

//////////////////////////////////////////////////////////////////////
struct tm* MMatchGetLocalTime()
{
	__time64_t long_time;

	_time64( &long_time );


	/* Obtain coordinated universal time: */
	return _localtime64( &long_time );
}