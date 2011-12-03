#pragma once


#include <vector>
using namespace std;


class MShutdownNotify {
protected:
	unsigned short	m_nDelay;
	char			m_szMessage[128];	
public:
	MShutdownNotify(unsigned short nDelay, const char* pszMsg) {
		// ������ �����Ҷ� ���ڿ� ���̸� �˻���. 
		// ���� �ִ� ���̸� �Ѿ�� �����Ҽ� �ִ� ���̸�ŭ�� ����. - by �߱���.
		memset( m_szMessage, 0, 128 );
		m_nDelay = nDelay;
		const size_t len = strlen( pszMsg );
		ASSERT( (len < 128) && "���ڿ��� ���̰� 128���� �۾ƾ� ��." );
		strncpy( m_szMessage, pszMsg,  len < 128 ? len : 127 );
	}
	virtual ~MShutdownNotify() {}

	unsigned short GetDelay()	{ return m_nDelay; }
	char* GetString()			{ return m_szMessage; }
};

class MMatchShutdown {
protected:
	vector<MShutdownNotify*>	m_ShutdownNotifyArray;

	bool						m_bShutdown;
	unsigned short				m_nProgressIndex;
	unsigned long				m_nTimeLastProgress;

	unsigned short GetProgressIndex()		{ return m_nProgressIndex; }
	unsigned long GetTimeLastProgress()		{ return m_nTimeLastProgress; }
	void MMatchShutdown::SetProgress(int nIndex, unsigned long nClock);

public:
	MMatchShutdown() { m_bShutdown = false; }
	virtual ~MMatchShutdown();

	bool LoadXML_ShutdownNotify(const char* pszFileName);

	void Start(unsigned long nClock);
	void Notify(int nIndex);
	void Terminate();

	bool IsShutdown()	{ return m_bShutdown; }

	void OnRun(unsigned long nClock);
};
