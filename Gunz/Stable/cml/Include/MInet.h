#ifndef _MINET_H
#define _MINET_H

#include "MThread.h"
#include "MSync.h"

#pragma comment(lib, "Wininet.lib")

#include <list>
#include <string>
using namespace std;

/// HTTP���������� �̿��Ῡ Url�� ������ �ܾ�´�.
bool MHTTP_Get(const char *szUrl, char *out, int nOutLen);

typedef bool(MHttpRecvCallback)(void* pCallbackContext, char* pRecvData);

#define HTTP_BUFSIZE 65536

/// HTTP ������
class MHttpThread : public MThread
{
private:
protected:
	MSignalEvent			m_QueryEvent;
	MSignalEvent			m_KillEvent;
	CRITICAL_SECTION		m_csQueryLock;
	bool					m_bActive;
	list<string>			m_QueryList;
	list<string>			m_TempQueryList;		// Temporary for Sync
	char					m_szRecvBuf[HTTP_BUFSIZE];
	void FlushQuery();
public:
	MHttpThread();
	virtual ~MHttpThread();
	virtual void Run();
	virtual void Create();
	virtual void Destroy();
	void ClearQuery();
	void Query(char* szQuery);
	void LockQuery() { EnterCriticalSection(&m_csQueryLock); }
	void UnlockQuery() { LeaveCriticalSection(&m_csQueryLock); }
	void*					m_pCallbackContext;
	MHttpRecvCallback*		m_fnRecvCallback;
};

/// HTTP Ŭ����
/// - ����Ϸ��� �� Ŭ������ ��ӹ޾� �ʿ��� ������ �ܾ���� �Լ��� �߰��ϸ� �ȴ�.
class MHttp
{
private:
protected:
	MHttpThread		m_HttpThread;
	void ReplaceBlank(char* szOut, char* szSrc);
public:
	MHttp();
	virtual ~MHttp();
	
	bool Create();
	void Destroy();
	void Query(char* szQuery);

	void SetRecvCallback(void* pCallbackContext, MHttpRecvCallback pCallback)
	{
		m_HttpThread.m_pCallbackContext = pCallbackContext;
		m_HttpThread.m_fnRecvCallback = pCallback;
	}
	void Clear();
};



#endif