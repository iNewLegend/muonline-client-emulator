#ifndef _MASYNCDBJOB_EVENT
#define _MASYNCDBJOB_EVENT


#include <vector>
#include <string>

using std::vector;
using std::string;


#include "MAsyncDBJob.h"
#include "MUID.h"


struct AsyncEventObj
{
	MUID  uidUser;
	DWORD dwAID;
	DWORD dwCID;
};

typedef vector< AsyncEventObj > AsyncEventObjVec;


class MAsyncDBJob_EventLog : public MAsyncJob
{
public :
	MAsyncDBJob_EventLog() : 
	   MAsyncJob( MASYNCJOB_PROBABILITYEVENTPERTIME, MUID(0, 0) ) {}

	~MAsyncDBJob_EventLog() {}
   
	const AsyncEventObjVec& GetEventObjList()	{ return m_vEventObj; }
	const string& GetEventName()				{ return m_strEventName; }
	const string& GetAnnounce()					{ return m_strAnnounce; }

	bool Input( AsyncEventObjVec& vEventObj, const string& strEventName, const string& strAnnounce );
	void Run( void* pContext );

private :
	AsyncEventObjVec	m_vEventObj;
	string				m_strEventName;
	string				m_strAnnounce;
};

#endif