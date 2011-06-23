// ------------------------------
// Decompiled by Deathway
// Date : 2007-03-09
// ------------------------------
#pragma once

#include "wsJoinServerCli.h"

#define DS_UNINITIALIZED -1
#define DS_0 0
#define DS_1 1

class DBSockMng
{

public:

	DBSockMng();
	virtual ~DBSockMng();

	void Connect(const char* szIP0, int nPort0, const char* szIP1, int nPort1);
	void ActiveChange();
	int Send(int server, char* buf, int len);
	int Send(char* buf, int len);
	int MsgProc(int line, WPARAM wParam, LPARAM lParam);
public:
	std::string strDBIP[2];
	int			nDBPort[2];
	wsJoinServerCli wsCDBS[2];	// 0
	int m_ActiveServer;	/// 320054
} ;


extern DBSockMng cDBSMng;