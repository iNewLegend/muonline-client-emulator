#ifndef _MHTTPSPOOLER_H
#define _MHTTPSPOOLER_H

//#pragma once

#include<list>
using namespace std;

#include "MThread.h"
#include "MSync.h"
#include "MAsyncHttp.h"


class MHttpSpoolerNode {
protected:
	unsigned int	m_nID;
	unsigned int	m_nChecksum;
	string			m_strURL;
public:
	MHttpSpoolerNode(unsigned int nID, unsigned int nChecksum, const string& strURL) {
		m_nID = nID;
		m_nChecksum = nChecksum;
		m_strURL = strURL;
	}
	virtual ~MHttpSpoolerNode()	{}
	unsigned int GetID()		{ return m_nID; }
	string GetURL()				{ return m_strURL; }
	unsigned int GetChecksum()	{ return m_nChecksum; }
};

class MHttpSpoolerQueue {
protected:
	list<MHttpSpoolerNode*>	m_SpoolQueue;
	MCriticalSection		m_csLock;
public:
	bool CheckExist(unsigned int nID)
	{
		bool bFound = false;
		m_csLock.Lock();
		for (list<MHttpSpoolerNode*>::iterator i=m_SpoolQueue.begin(); i!=m_SpoolQueue.end(); i++) {
			MHttpSpoolerNode* pNode = (*i);
			if (pNode->GetID() == nID)
				bFound = true;
		}
		m_csLock.Unlock();
		return bFound;
	}
	void Post(MHttpSpoolerNode* pSpoolNode) 
	{
		m_csLock.Lock();
		m_SpoolQueue.push_back(pSpoolNode);
		m_csLock.Unlock();
	}
	MHttpSpoolerNode* Pop()
	{
		MHttpSpoolerNode* pSpoolNode = NULL;
		m_csLock.Lock();
		if (!m_SpoolQueue.empty()) {
			pSpoolNode = (*m_SpoolQueue.begin());
			m_SpoolQueue.pop_front();
		}            
		m_csLock.Unlock();
		return pSpoolNode;
	}
};

class MHttpSpooler : public MThread {
protected:
	bool				m_bShutdown;

	MAsyncHttp			m_AsyncHttp;

	MHttpSpoolerQueue	m_RequestQueue;
	MHttpSpoolerQueue	m_ResultQueue;

protected:
	bool CheckShutdown()	{ return m_bShutdown; }
	void Shutdown()			{ m_bShutdown = true; }

	virtual void OnCreate();
	virtual void OnDestroy();
	virtual void Run();

public:
	MHttpSpooler()			{ m_bShutdown = false; }
	virtual ~MHttpSpooler()	{}
	
	const char* GetBasePath()				{ return m_AsyncHttp.GetBasePath(); }
	void SetBasePath(const char* pszPath)	{ m_AsyncHttp.SetBasePath(pszPath); }

	void Post(unsigned int nID, unsigned int nChecksum, const string strURL)
	{
		if (m_RequestQueue.CheckExist(nID) || m_ResultQueue.CheckExist(nID))
			return;

		MHttpSpoolerNode* pNode = new MHttpSpoolerNode(nID, nChecksum, strURL);
		m_RequestQueue.Post(pNode);
	}
	bool Pop(unsigned int* poutID, unsigned int* poutChecksum, string* poutstrURL)
	{
		MHttpSpoolerNode* pNode = m_ResultQueue.Pop();
		if (pNode == NULL) return false;

		*poutID = pNode->GetID();
		*poutChecksum = pNode->GetChecksum();
		*poutstrURL = pNode->GetURL();

		delete pNode;
		return true;
	}
};


#endif