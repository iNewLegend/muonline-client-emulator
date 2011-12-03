#pragma once

#include <vector>
#include <list>

using std::vector;
using std::list;

class MCommand;
class MServer;
class MMatchScheduleImpl;

/*
 * ��� �������� bIsNeedDelete�� true�� �Ǹ� ���ŵ�.
 *
 * once : �ѹ� �����ϸ� ������ bIsNeedDelete�� true�� ������.
 *
 * count : ����� m_nCount������ 1�� ���ҽ�Ű�� 0�� �Ǵ� �������� bIsNeedDelete�� true�� ������.
 *			cMonth, cDay, cHour, cMin�� ���� �ð��� ���ƾ� ������.
 *			cNextMonth, cNextDay, cNextHour, cNextMin�� ������ ����� �ð��� ����ġ.
 *
 * repeat : ����ؼ� ������. bIsNeedDelete�� true�� ������ ����.
 *			cMonth, cDay, cHour, cMin�� ���� �ð��� ���ƾ� ������.
 */

class MMatchDayOfMonth
{
public :
	MMatchDayOfMonth();
	~MMatchDayOfMonth();

	static MMatchDayOfMonth& GetInst()
	{
		static MMatchDayOfMonth inst;
		return inst;
	}

	inline char GetMaxDay( const int iMonth ) 
	{
		if( 1 > iMonth ) 
			return -1;

		// �迭������ ������ ó��.
		return m_cDayOfMonth[ iMonth - 1 ];
	}
private :
	void Init();

private :
	enum MONTH_NUM
	{
		MonthNum = 12,
	};

	char m_cDayOfMonth[ MONTH_NUM::MonthNum ];
};

class MMatchScheduleData
{
public:
	// �������� ������ 1:repeat, 2:Count, 3:Once�� �Ǿ�����.
	enum MMatchScheduleState
	{
		REPEAT = 0,
		COUNT,
		ONCE,
	};

	MMatchScheduleData(void);
	~MMatchScheduleData(void);

	void CorrectTime();

	inline int					GetType()		{ return m_nType; }
	inline MCommand*			GetCommand()	{ return m_pCmd; }
	inline bool					IsNeedDelete()	{ return m_bIsNeedDelete; }
	inline MMatchScheduleImpl*	GetImpl()		{ return m_pImpl; }
	inline int					GetErrorTime()	{ return m_nErrorTime; }

	inline int GetCount() { return m_nCount; }

	inline void IncreaseCount() { ++m_nCount; }
	inline void DecreaseCount() { --m_nCount; }

	inline unsigned char GetYear()	{ return m_cYear; }
	inline unsigned char GetMonth()	{ return m_cMonth; }
	inline unsigned char GetDay()	{ return m_cDay; }
	inline unsigned char GetHour()	{ return m_cHour; }
	inline unsigned char GetMin()	{ return m_cMin; }

	inline unsigned char GetNextYear()	{ return m_cNextYear; }
	inline unsigned char GetNextMonth()	{ return m_cNextMonth; }
	inline unsigned char GetNextDay()	{ return m_cNextDay; }
	inline unsigned char GetNextHour()	{ return m_cNextHour; }
	inline unsigned char GetNextMin()	{ return m_cNextMin; }

	inline void SetType( const int nType )				{ m_nType = nType; }
	inline void SetCommand( MCommand* pCmd )			{ m_pCmd = pCmd; }
	inline void SetImpl( MMatchScheduleImpl* pImpl )	{ m_pImpl = pImpl; }
	inline void SetDeleteState( const bool bState )		{ m_bIsNeedDelete = bState; }
	inline void SetErrorTime( const int nErrorTime )	{ m_nErrorTime = nErrorTime; }

	inline void SetCount( const int nStartCount ) { m_nCount = nStartCount; }

	inline void SetYear( const unsigned char cYear )	{ m_cYear = cYear; }
	inline void	SetMonth( const unsigned char cMonth )	{ m_cMonth = cMonth; }
	inline void	SetDay( const unsigned char cDay )		{ m_cDay = cDay; }
	inline void SetHour( const unsigned char cHour )	{ m_cHour = cHour; }
	inline void	SetMin( const unsigned char cMin )		{ m_cMin = cMin; }

	inline void SetNextYear( const unsigned char cYear )	{ m_cNextYear = cYear; }
	inline void	SetNextMonth( const unsigned char cMonth )	{ m_cNextMonth = cMonth; }
	inline void	SetNextDay( const unsigned char cDay )		{ m_cNextDay = cDay; }
	inline void SetNextHour( const unsigned char cHour )	{ m_cNextHour = cHour; }
	inline void	SetNextMin( const unsigned char cMin )		{ m_cNextMin = cMin; }

	// return if same, then 0, less -1, greater 1.
	int	CompareCurrentTime();
	bool SetTimes( const unsigned char cYear, 
				   const unsigned char cMonth, 
				   const unsigned char cDay, 
				   const unsigned char cHour, 
				   const unsigned char cMin );
	bool SetNextTimes( const unsigned char cNextYear, 
					   const unsigned char cNextMonth, 
					   const unsigned char cNextDay, 
					   const unsigned char cNextHour, 
					   const unsigned char cNextMin );

	void ResetTime();

	void Release();

private :
	// int				nID;
	int					m_nType;			// ������ Ÿ��( REPAT, COUNT, ONCE ).
	MCommand*			m_pCmd;				// �������� Ȱ��ȭ �Ǿ����� ������ ���.
	bool				m_bIsNeedDelete;	// true�̸� �������� ���� �Ǵ� ��Ȱ��ȭ ��.
	MMatchScheduleImpl*	m_pImpl;			// �������� ����Ǳ����� �ؾ� �� ������.

	int	m_nCount;		// Ư�� Ƚ���� ������ ��� ���.
	int m_nErrorTime;	// ������ ����ɼ� �ִ� �Ѱ�ġ( �� ).

	unsigned char	m_cYear;	// ���� �Ǿ�� �ϴ� ��.
	unsigned char	m_cMonth;	// ���� �Ǿ�� �ϴ� ��.
	unsigned char	m_cDay;		// ���� �Ǿ�� �ϴ� ��.
	unsigned char	m_cHour;	// ���� �Ǿ�� �ϴ� �ð�.
	unsigned char	m_cMin;		// ���� �Ǿ�� �ϴ� ��.
	
	unsigned char	m_cNextYear;	// ������ ���صǾ�� �ϴ� ���� ����ġ.
	unsigned char	m_cNextMonth;	// ������ ����Ǿ�� �ϴ� ���� ����ġ.
	unsigned char	m_cNextDay;		// ������ ����Ǿ�� �ϴ� ���� ����ġ.
	unsigned char	m_cNextHour;	// ������ ����Ǿ�� �ϴ� �ð��� ����ġ.
	unsigned char	m_cNextMin;		// ������ ����Ǿ�� �ϴ� ���� ����ġ.
};

class MMatchScheduleReleaser
{
public :
	void operator() ( MMatchScheduleData*& rfSchedulerData )
	{
		if( 0 != rfSchedulerData ){
			rfSchedulerData->Release();
			delete rfSchedulerData;
			rfSchedulerData = 0;
		}
	}
};

/// �� ������ Ÿ�Կ� �´� ������ ó���� ���� ������.
// ���� ���� �������̽� Ŭ����.
class MMatchScheduleImpl
{
public :
	virtual ~MMatchScheduleImpl() {}

	// �ð� ���� ����.
	virtual void CorrectTime( MMatchScheduleData* pScheduleData ) = 0;
	// �ð��� �ٽ� ������.
	virtual void Reset( MMatchScheduleData* pScheduleData ) = 0;
};
// ���ŵ��� �ʰ� ��� �ݺ��Ǵ� ������.
class MMatchRepeatScheduleImpl : public MMatchScheduleImpl
{
public :
	// �ð� ���� ����.
	void CorrectTime( MMatchScheduleData* pScheduleData );
	// �ð��� �ٽ� ������.
	void Reset( MMatchScheduleData* pScheduleData );
};
// ���� Ƚ���� �����ϰ� ���ŵǴ� ������.
class MMatchCountScheduleImpl : public MMatchScheduleImpl
{
public :
	// �ð� ���� ����.
	void CorrectTime( MMatchScheduleData* pScheduleData );
	// �ð��� �ٽ� ������.
	void Reset( MMatchScheduleData* pScheduleData );
};
// �ѹ��� �����ϰ� ���ŵǴ� ������.
class MMatchOnceScheduleImpl : public MMatchScheduleImpl
{
public :
	// �ð� ���� ����.
	void CorrectTime( MMatchScheduleData* pScheduleData );
	// �ð��� �ٽ� ������.
	void Reset( MMatchScheduleData* pScheduleData );
};


/// �������� �����ϰ� �����ϴ� ������ ������.
typedef vector< MMatchScheduleData* > ScheduleVec;
typedef ScheduleVec::iterator	ScheduleVecIter;

typedef list< MMatchScheduleData* >	ScheduleLst;
typedef ScheduleLst::iterator	ScheduleLstIter;


class MMatchScheduleMgr
{
public :
	MMatchScheduleMgr( MServer* pServer );
	~MMatchScheduleMgr();

	// ������ ������ ���. ���ǿ� ���ؼ� �������� ����.
	bool AddDynamicSchedule( MMatchScheduleData* pNewSchedule );
	// �ý����� �����ϸ鼭 ����ɶ����� �����Ǿ�� �ϴ� ������.
	bool AddStaticSchedule( MMatchScheduleData* pNewSchedule );

	// Schedule data maker.
	MMatchScheduleData* MakeRepeatScheduleData( const unsigned char	cNextYear,	// ������ ����� ���� ��밪.	ex) 1�� ��.
												const unsigned char cNextMonth, // ������ ����� ���� ��밪.	ex) 1�� ��.
												const unsigned char cNextDay,	// ������ ����� ���� ��밪.	ex) 1�� ��.
												const unsigned char cNextHour,	// ������ ����� �ð��� ��밪.	ex) 1�ð� ��.
												const unsigned char cNextMin,	// ������ ����� ���� ��밪.	ex) 1�� ��.
												MCommand* pCmd					// ������ ���.
												);
	MMatchScheduleData* MakeCountScheduleData( const unsigned char cNextYear,	// ������ ����� ���� ��밪.	ex) 1�� ��.
											   const unsigned char cNextMonth,	// ������ ����� ���� ��밪.	ex) 1�� ��.
											   const unsigned char cNextDay,	// ������ ����� ���� ��밪.	ex) 1�� ��.
											   const unsigned char cNextHour,	// ������ ����� �ð��� ��밪.	ex) 1�ð� ��.
											   const unsigned char cNextMin,	// ������ ����� ���� ��밪.	ex) 1�� ��.
											   const unsigned int nCount,		// ����� Ƚ��.
											   MCommand* pCmd					// ������ ���.
											   );
	MMatchScheduleData* MakeOnceScheduleData( const unsigned char cYear,	// ����� ��.
											  const unsigned char cMonth,	// ����� ��.
											  const unsigned char cDay,		// ����� ��.
                                              const unsigned char cHour,	// ����� �ð�.
                                              const unsigned char cMin,		// ����� ��.
                                              MCommand* pCmd				// ������ ���.	
											  );

	// �ʱ�ȭ.
	bool Init();

	inline void SetUpdateTerm( time_t tmTime ) { m_tmUpdateTerm = tmTime; }

	// ��ϵ��ִ� ��� �������� Ž���Ͽ� ������Ʈ��.
	void Update();

	// ����ñ��� �����ִ� �������� ������.
	void Release();

private :
	// ������ ������Ʈ ������������ ����ð� ���.
	const time_t CalculateElapseUpdateTime();
	// ���������� ���������� �˻���.
	bool CheckData( MMatchScheduleData* pScheduleData ) const;
	// ������Ʈ �Ǿ�� �ϴ� �ð����� �˻���.
	bool CompareTime( MMatchScheduleData* pScheduleData );
	// ������Ʈ�� �������� �˻�.
	bool IsUpdate();

	void UpdateStaticSchedule();
	void UpdateDynamicSchedule();

	void SetLastUpdateTime();

	void ReleaseStaticSchedule();
	void ReleaseDynamicSchedule();

	enum MONTH_NUM { MonthNum = 12, };

	// �����ڸ� ������ �ִ� ���� Ŭ����.
	class MMatchScheduleImplPrototype
	{
	private :
		friend MMatchScheduleMgr;

		typedef vector< MMatchScheduleImpl* >	ScheduleImplVec;

		MMatchScheduleImplPrototype() {}
		~MMatchScheduleImplPrototype() {}

		bool Init();
		// �ش� Ÿ��ID�� �´� �������� �����͸� �Ѱ���. ���� delete�ϸ� �ʵ�.
		MMatchScheduleImpl* GetImpl( const unsigned int nTypeID );
		void Release();

		// �����ڸ� �����ϰ� ���� ����.
		ScheduleImplVec	m_ScheduleImplVec;
	};

	class MMatchScheduleImplRelease
	{
	public :
		void operator() ( MMatchScheduleImpl*& rfImpl )
		{
			if( 0 != rfImpl ){
				delete rfImpl;
				rfImpl = 0;
			}
		}
	};

private :

	MServer*	m_pServer;

	int	m_nIndex;

	time_t m_tmUpdateTerm;		// ������Ʈ�Ǵ� ����.
	time_t m_tmLastUpdateTime;	// ������ ������Ʈ�� �ð�.

	MMatchScheduleImplPrototype	m_ScheduleImplPrototype;

	vector< MMatchScheduleData* >	m_vecStaticSchedule;	// �ý��� ������ ��ϵǴ� ������. �ý��ۼ���� ����.
	list< MMatchScheduleData* >		m_lstDynamicSchedule;	// ��������.
};

struct tm* MMatchGetLocalTime();
char GetMaxDay( const int iMonth );
char GetMaxDay();
bool AddDynamicSchedule( MMatchScheduleMgr* pScheduleMgr, const int nType, MCommand* pCmd, const int nYear, const int nMonth, const int nDay, const int nHour, const int nMin, const int nCount );