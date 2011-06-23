// ------------------------------
// Decompiled by Deathway
// Date : 2007-03-09
// ------------------------------

// GS-N 0.99.60T Status : Completed
//	GS-N	1.00.18	JPN	0x00423E80	-	Completed

#include "stdafx.h"
#include "..\common\MyWinsockBase.h"
#include "wsJoinServerCli.h"
#include "GameServer.h"
#include "GameMain.h"
#include "DBSockMng.h"
#include "DSProtocol.h"
#include "logproc.h"



DBSockMng cDBSMng;	// DataServer Manager

DBSockMng::DBSockMng() 
{
	this->m_ActiveServer=DS_UNINITIALIZED;
}

DBSockMng::~DBSockMng()
{

}


void DBSockMng::Connect(const char* szIP0, int nPort0, const char* szIP1, int nPort1)
{
	strDBIP[0]=szIP0;
	strDBIP[1]=szIP1;
	nDBPort[0]=nPort0;
	nDBPort[1]=nPort1;

	for(size_t i=0;i<2;++i)
	{
		if (this->wsCDBS[i].GetSocket()  == INVALID_SOCKET )
		{
			int State;
			this->wsCDBS[i].CreateSocket(ghWnd);
			this->wsCDBS[i].SetProtocolCore(DataServerProtocolCore);
			State=this->wsCDBS[i].Connect(strDBIP[i].c_str(), nDBPort[i], WM_GM_DATA1_CLIENT_MSG_PROC+i);
			if (State == 0)
			{
				LogAdd(lMsg.Get(MSGGET(1, 147)), strDBIP[i].c_str(), nDBPort[i]);
				this->wsCDBS[i].Close();
			}
			else
			{
				LogAdd(lMsg.Get(MSGGET(1, 146)), strDBIP[i].c_str(), nDBPort[i]);	
				if (this->m_ActiveServer==-1)
				{
					this->m_ActiveServer=i;//DS_0;
				}
				DataServerLogin(i);//DS_0
			}
		}
	}

	if ( wsExDbCli.GetSocket()  == INVALID_SOCKET)
	{
		LogAddC(2, "ExDB Server Check..");
		wsExDbCli.CreateSocket(ghWnd);
		ExDataServerConnect(ExDbIp, WM_GM_EXDATA_CLIENT_MSG_PROC);
	}
	else
	{
		if ( wsExDbCli.GetConnect() == 0 )
		{
			LogAddC(2, "ExDB Server Check..");
			ExDataServerConnect(ExDbIp, WM_GM_EXDATA_CLIENT_MSG_PROC);
		}
	}
}

void DBSockMng::ActiveChange()
{
	{
		int Status;
		if (this->m_ActiveServer==DS_0)
		{
			this->m_ActiveServer=DS_1;
			Status=1;
		}
		else
		{
			this->m_ActiveServer=DS_0;
			Status=0;
		}
		LogAdd("Active DB Change");
	}
}

int DBSockMng::Send(char* buf, int len)
{
	if (this->m_ActiveServer >= 2)
	{
		return 0;
	}
	if (this->m_ActiveServer < 0)
	{
		return 0;
	}
	if ( this->wsCDBS[this->m_ActiveServer].DataSend((char*)buf, len) == 0)
	{
		this->ActiveChange();
		this->wsCDBS[this->m_ActiveServer].DataSend((char*)buf, len);
	}
	return 1;

}


int DBSockMng::Send(int server, char* buf, int len)
{
	if (server >= 2)
	{
		return 0;
	}
	if (server < 0)
	{
		return 0;
	}
	if (this->wsCDBS[server].DataSend(buf, len) == 0)
	{
		this->ActiveChange();
	}
	return 1;
}


int DBSockMng::MsgProc(int line, WPARAM wParam, LPARAM lParam)
{
	switch (lParam & 0xFFFF & 0xFFFF)
	{
	case 1: // Recv Data
		this->wsCDBS[line].DataRecv();
		break;
	case 2: // Send Data
		this->wsCDBS[line].FDWRITE_MsgDataSend();
		break;
	case 32: // Close
		this->wsCDBS[line].Close();
	}
	return 1;
}


			





