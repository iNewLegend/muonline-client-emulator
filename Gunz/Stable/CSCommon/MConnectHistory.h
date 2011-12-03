#pragma once

//////////////////////////////////////////////////////////////////////////
// MConnectHistory
// ���� �÷����� �������� �����ð�, �������� ���� ����� �����ϰ� �ִ�.

class MConnectHistory {
public:
	void Update( DWORD dwCurrentTime );
	bool IsExist( const string& strName );
	void Add( const string& strName, DWORD dwCurrentTime );

private:
	typedef map< string, DWORD > NAMETOTIMEMAP;

	NAMETOTIMEMAP m_mapNameToTime;
};