#pragma once

typedef void * LPGGAUTH;

struct _GG_AUTH_DATA
{
	DWORD dwIndex;	// 0
	DWORD dwValue1;	// 4
	DWORD dwValue2;	// 8
	DWORD dwValue3;	// C
};

struct _GG_AUTH_PROTOCOL
{
	HINSTANCE hDllHinstance;	// 0
	DWORD dwPtrcVersion;	// 4
	DWORD dwQueryCount;	// 8
	DWORD unkC;
	DWORD  (*PtrcGetAuthQuery)(int, LPGGAUTH, _GG_AUTH_DATA*, _GG_AUTH_DATA*);	// 10
	DWORD  (*PtrcCheckAuthAnswer)(int, LPGGAUTH, _GG_AUTH_DATA*, _GG_AUTH_DATA*);	// 14
	_GG_AUTH_PROTOCOL * lpPrevProtocol;	// 18

};

class CCSAuth2
{
public:

	CCSAuth2();
	~CCSAuth2();

	void Init();
	unsigned long  GetAuthQuery();
	unsigned long  CheckAuthAnswer();

public:

	_GG_AUTH_PROTOCOL* m_pProtocol;	// 0
	unsigned long m_bPrtcRef;	// 4
	DWORD m_dwUserFlag;	// 8
	_GG_AUTH_DATA m_AuthQuery;	// C
	_GG_AUTH_DATA m_AuthAnswer;	// 1C
};