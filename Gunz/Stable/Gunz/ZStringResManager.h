#ifndef _ZSTRINGRES_MANAGER_H
#define _ZSTRINGRES_MANAGER_H


#include "MBaseStringResManager.h"

class ZStringResManager : public MBaseStringResManager
{
protected:
	MStringRes<int>			m_Messages;
	virtual bool OnInit();
public:
	ZStringResManager();
	virtual ~ZStringResManager();
	static void MakeInstance();
	static void ResetInstance();

	const char* GetMessageStr(int nID);
	MStringRes<int>*	GetMessages()	{ return &m_Messages; }

};

inline ZStringResManager* ZGetStringResManager()
{
	return (ZStringResManager*)MBaseStringResManager::GetInstance();
}

inline const char* ZMsg(const int nID) { return ZGetStringResManager()->GetMessageStr(nID); }
inline const char* ZErrStr(const int nID) { return ZGetStringResManager()->GetErrorStr(nID); }
inline const char* ZStr(string& key) { return ZGetStringResManager()->GetString(key); }
inline const char* ZStr(const char* key) { return ZStr(string(key)); }

inline bool ZTransMsg(char* poutStr, int nMsgID, const int argnum=0, const char* arg1=NULL, ... )
{
	va_list args;
	va_start(args, arg1);
	bool ret = ZGetStringResManager()->GetMessages()->Translate(poutStr, nMsgID, argnum, arg1, args);
	va_end(args);
	return ret;
}


#endif