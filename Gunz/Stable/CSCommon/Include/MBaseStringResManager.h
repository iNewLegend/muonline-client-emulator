#ifndef _MBASE_STRINGRES_MANAGER_H
#define _MBASE_STRINGRES_MANAGER_H


#include "MStringRes.h"

const char FILENAME_ERROR_TABLE[]		= "cserror.xml";
const char FILENAME_STRING_TABLE[]		= "strings.xml";
const char FILENAME_MESSAGES[]			= "messages.xml";



class MBaseStringResManager
{
protected:
	static MBaseStringResManager*	m_pInstance;
	string							m_strPath;
	MZFileSystem*					m_pFS;
	MStringRes<int>					m_ErrorTable;
	MStringRes<string>				m_StringTable;

	virtual bool OnInit() { return true; }
public:
	MBaseStringResManager();
	virtual ~MBaseStringResManager();
	bool Init(const char* szPath, const int nLangID, MZFileSystem* pfs=NULL );
	static MBaseStringResManager* GetInstance();
	static void FreeInstance();

	const char* GetErrorStr(int nID);
	const char* GetString(string& key);
	const char* GetStringFromXml(const char* str);
};


inline MBaseStringResManager* MGetStringResManager()
{
	return MBaseStringResManager::GetInstance();
}









#endif