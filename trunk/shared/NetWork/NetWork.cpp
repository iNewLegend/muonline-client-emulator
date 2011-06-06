#include "NetWork.h"
//#include "logproc.h"

CNetWork::CNetWork()
{
	m_WinMsgNum = 0;
}

CNetWork::~CNetWork()
{
	return;
}

void CNetWork::SetHWND(HWND hWnd)
{
	this->m_hWnd =hWnd;
}

void CNetWork::SetWinMsgNum(DWORD WinMsgNum)
{
	m_WinMsgNum = WinMsgNum;
}

bool CNetWork::Connect(LPCSTR ip_addr, WORD port)
{
	hostent* host;
	sockaddr_in addr;
	int nResult;

	host = 0;

	if ( this->m_hWnd  == 0 )
	{
		//LogAdd(" Connect Error %s %d", __FILE__, __LINE__);
		return FALSE;
	}
	addr.sin_family = AF_INET;
	addr.sin_port =  htons(port);
	addr.sin_addr.S_un.S_addr = inet_addr(ip_addr);

	if ( addr.sin_addr.S_un.S_addr == -1 )
	{
		host = gethostbyname(ip_addr);
		if ( host == 0 )
		{
			//LogAdd("server name not found [%s]", ip_addr);
			return FALSE;
		}

		memcpy(&addr.sin_addr.S_un.S_addr, *host->h_addr_list  , host->h_length  );	
	}

	CreateSocket();
	nResult=connect(this->m_socket, (sockaddr*)&addr, 16);

	if (nResult== -1)
	{
		if ( WSAGetLastError() != WSAEWOULDBLOCK)
		{
			this->Close(this->m_socket );
			return FALSE;
		}
	}

	nResult=WSAAsyncSelect(this->m_socket, this->m_hWnd , m_WinMsgNum, FD_READ|FD_WRITE|FD_CLOSE);

	if ( nResult == -1 )
	{
		closesocket(this->m_socket );
		//LogAdd("Client WSAAsyncSelect error %d", WSAGetLastError());
		return FALSE;
	}
	this->m_SendBufLen=0;
	this->m_RecvBufLen=0;
	this->SetConnect(TRUE);

	return TRUE;
}

bool CNetWork::DataSend(const unsigned char* buf, int len)
{
	int nResult;
	int nLeft;
	int nDx;	// Degeneration X :)
	
	if ( len <= 0 )
	{
		return 0;
	}

	if ( this->m_socket  == INVALID_SOCKET )	// 
	{
		return 0;
	}

	nDx = 0;

	if ( this->m_SendBufLen   > 0 )
	{
		if ( (this->m_SendBufLen +len) < MAX_WS_BUF_LEN )
		{
			memcpy( &this->m_SendBuf[this->m_SendBufLen]  , buf, len);	// Review this later

			this->m_SendBufLen +=len;
			return 1;
		}
		this->Close();
		return FALSE;
	}

	nLeft= len;

	while ( true )
	{
		nResult=send(this->m_socket, (char*)&buf[nDx], (len - nDx), 0);

		if ( nResult == -1)
		{
			if ( WSAGetLastError() != WSAEWOULDBLOCK )
			{
				//LogAdd("error-L3 : %d (%s %d)", WSAGetLastError(), __FILE__, __LINE__);
				len=0;
				this->Close(this->m_socket );
				return FALSE;
			}

			if ( (len + nLeft) > MAX_WS_BUF_LEN )
			{
				//LogAddC(2,"error-L3 : buffer error closed %d, %d", (len+nLeft), MAX_WS_BUF_LEN);
				len = 0;
				this->Close(this->m_socket);
				return FALSE;
			}

			if ( nDx >= 0 )
			{
				memcpy( &this->m_SendBuf[this->m_SendBufLen],  &buf[nDx], nLeft);
				
				this->m_SendBufLen +=nLeft;

				//LogAdd("error-L3 : Copy1 : nLeft:%d len:%d nDx:%d", nLeft, this->m_SendBufLen , nDx);
				return TRUE;
			}
		}
		else if ( nResult == 0 )
		{
			//LogAdd("error-L3 : send()  result is zero", WSAGetLastError());
			break;
		}

		if ( nResult >0 )
		{
			nDx += nResult;
			nLeft -= nResult;
			len -= nResult;
		}
		if ( nLeft <= 0)
		{
			break;
		}
	}
	return TRUE;
}

bool CNetWork::FDWRITE_MsgDataSend()
{
	int nResult;
	int nDx;	// Degeneration X :)
	LPBYTE sendbuf;

	nDx=0;
	sendbuf=&this->m_SendBuf[0];

	if (this->m_SendBufLen < 1)
	{
		return TRUE;
	}

	while (this->m_SendBufLen>0)
	{
		nResult=send(this->m_socket, (char*)&sendbuf[nDx], this->m_SendBufLen-nDx, 0);
		if (nResult == -1)
		{
			if ( WSAGetLastError() != WSAEWOULDBLOCK)
			{
				//LogAdd("FD_WRITE send() ¿¡·¯ %d %d", WSAGetLastError(), this->m_SendBufLen);
				this->Close();
				return FALSE;
			}
			else
			{
				//LogAdd("FD_WRITE send() WSAEWOULDBLOCK : %d", WSAGetLastError());
				break;
			}
		}
		if (nResult <= 0)
		{
			//LogAdd("send() result is zero %d", WSAGetLastError());
			break;
		}
		//LogAdd("error-L3 : nDx %d m_SendBufLen %d", nDx, this->m_SendBufLen );

		if (nResult > 0)
		{
			nDx += nResult;
			this->m_SendBufLen -= nResult;
		}

		//LogAdd("error-L3 : nDx %d m_SendBufLen %d", nDx, this->m_SendBufLen );
	}
	return TRUE;
}

int CNetWork::DataRecv()
{
	int nResult;
	unsigned char* recvbuf;
	int lOfs;
	int size;

	recvbuf= (UCHAR*)&this->m_RecvBuf[0]   ;
	nResult=recv(this->m_socket , (CHAR*)&recvbuf[this->m_RecvBufLen], (MAX_WS_BUF_LEN-this->m_RecvBufLen), 0);

	if ( nResult == 0 )
	{
		//LogAdd("error-L3 : closed %d - buff:%d(%d) ",
		//	WSAGetLastError(), this->m_RecvBufLen, MAX_WS_BUF_LEN - this->m_RecvBufLen);

		return 1;
	}
	else if (nResult == -1 )
	{
		if ( WSAGetLastError() == WSAEWOULDBLOCK )
		{
			return 1;
		}
		//LogAdd("error-L3 : recv error %d", WSAGetLastError() );
		return 1;
	}

	this->m_RecvBufLen += nResult;

	if (this->m_RecvBufLen<3)
	{
		return 3;
	}

	lOfs=0;
	size=0;

	while ( true )
	{
		size = ProtocolCore(&recvbuf[lOfs],this->m_RecvBufLen);

		if ( size <= 0 )
		{
			//LogAdd("error-L2 : header error (%s %d)lOfs:%d, size:%d", __FILE__, __LINE__, lOfs, this->m_RecvBufLen);
			this->m_RecvBufLen = 0;
			//LogAdd("error-L2 : size %d", size);
			return 0;
		}

		if ( size <= this->m_RecvBufLen )
		{
			lOfs += size;
			this->m_RecvBufLen -= size;

			if ( this->m_RecvBufLen <= 0)
			{
				break;
			}
		}
		else if ( lOfs > 0 )
		{
			if ( this->m_RecvBufLen < 1 ) 
			{
				break;
			}

			memcpy(recvbuf, &recvbuf[lOfs], this->m_RecvBufLen);
			break;
		}
		else
		{
			break;
		}
	}
	return 0;
}