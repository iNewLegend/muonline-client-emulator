// ------------------------------
// Decompiled by Deathway
// Date : 2007-03-09
// ------------------------------

#pragma once

#define MY_WINSOCK_VERSION 0x202		// 2.2

#include <winsock2.h>

class MyWinsockBase
{
private:
	BOOL Startup();
public:

	BOOL CreateSocket();
	BOOL Close(SOCKET socket);
	BOOL Close();
	BOOL Close2(SOCKET socket);
	SOCKET GetSocket();
	int GetSendBuffSize();
	int GetRecvBuffSize();
	BOOL GetConnect();
	void SetConnect(BOOL connected);

	MyWinsockBase();
	~MyWinsockBase();

protected:
	int m_iMaxSockets;	// 0
	SOCKET m_socket;	// 8
	int m_sendbufsize;	// C
	int m_recvbufsize;	// 10
	BOOL m_Connect;	// 14
};