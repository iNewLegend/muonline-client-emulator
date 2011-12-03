#pragma once

#include "afxdb.h"

#include <string>

using std::string;

typedef void(LOGCALLBACK)( const string& strLog );


class MDatabase
{
public:
	MDatabase(void);
	~MDatabase(void);

	CDatabase* GetDatabase()	{ return &m_DB; }

	bool CheckOpen();
	CString BuildDSNString(const CString strDSN, const CString strUserName, const CString strPassword);
	bool Connect(CString strDSNConnect);
	void Disconnect();
	BOOL IsOpen() const;
	void ExecuteSQL(LPCTSTR lpszSQL);
	// thread unsafe
	void SetLogCallback( LOGCALLBACK* fnLogCallback ) { m_fnLogCallback = fnLogCallback; }

private :
	void WriteLog( const string& strLog );

private :
	CDatabase		m_DB;
	CString			m_strDSNConnect;
	DWORD			m_dwOptions;
	LOGCALLBACK*	m_fnLogCallback;
};
