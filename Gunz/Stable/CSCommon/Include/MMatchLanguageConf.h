#pragma once

#include "MLanguageConf.h"

class MMessenger;

const char SERV_LANG_DIR[] = "lang";

class MMatchMessageNameGroup : public MMessageNameGroup
{
public :
	MMatchMessageNameGroup() {}
	virtual ~MMatchMessageNameGroup() {}

	// name group������ �߰����� ��� �ʿ��ϸ� �̰��� �߰��ϸ� ��.
};


class MMatchLanguageConf : public MLanguageConf
{
public :
	MMatchLanguageConf();
	virtual ~MMatchLanguageConf();

private :
	virtual void MakeCompleteDir();
	virtual bool CheckSubTaskFile( MZFileSystem* pfs, MMessageNameGroup* pMsgNameGroup );
};


class MMatchMessengerManager : public MMessengerManager
{
public :
	MMatchMessengerManager();
	virtual ~MMatchMessengerManager();

	// inline MMessenger* GetErrMessenger() { return m_pErrMessenger; }

private :
	virtual bool AllocateSubTask( const string& strKey, const string& strFile, MZFileSystem* pfs );
	virtual void ReleaseSubTask();

private :
	// MMessenger* m_pErrMessenger;
};