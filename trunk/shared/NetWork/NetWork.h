#pragma once
#include "MyWinsockBase.h"

#define MAX_WS_BUF_LEN	819200

class CNetWork : public MyWinsockBase
{
public:
	CNetWork();
	virtual ~CNetWork();
	void SetHWND(HWND hWnd);
	void SetWinMsgNum(DWORD WinMsgNum);
	bool FDWRITE_MsgDataSend();
	int DataRecv();
	virtual int ProtocolCore(const unsigned char* buf, int len)=0;
	bool DataSend(const unsigned char* buf, int len);
	bool Connect(LPCSTR ip_addr, WORD port);
protected:
	HWND m_hWnd;
	DWORD m_WinMsgNum;
	BYTE m_SendBuf[MAX_WS_BUF_LEN];	// 1C
	int m_SendBufLen;	// C801C
	BYTE m_RecvBuf[MAX_WS_BUF_LEN];	// C8020
	int  m_RecvBufLen;	// 190020
};