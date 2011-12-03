#include "stdafx.h"
#include "MCustomClient.h"


MCustomClient::MCustomClient()
{
	InitializeCriticalSection(&m_csRecvLock);

	m_ClientSocket.SetCallbackContext(this);
	m_ClientSocket.SetConnectCallback(SocketConnectEvent);
	m_ClientSocket.SetDisconnectCallback(SocketDisconnectEvent);
	m_ClientSocket.SetRecvCallback(SocketRecvEvent);
	m_ClientSocket.SetSocketErrorCallback(SocketErrorEvent);
}

MCustomClient::~MCustomClient()
{
	DeleteCriticalSection(&m_csRecvLock);
}

bool MCustomClient::OnSockConnect(SOCKET sock)
{
	return true;
}

bool MCustomClient::OnSockDisconnect(SOCKET sock)
{
	return true;
}

bool MCustomClient::OnSockRecv(SOCKET sock, char* pPacket, DWORD dwSize)
{
	return false;
}

void MCustomClient::OnSockError(SOCKET sock, SOCKET_ERROR_EVENT ErrorEvent, int &ErrorCode)
{

}

int MCustomClient::Connect(char* szIP, int nPort)
{
	if (m_ClientSocket.Connect(NULL, szIP, nPort))
		return MOK;
	else
		return MERR_UNKNOWN;
}

void MCustomClient::Send(char* pBuf, DWORD nSize)
{
	m_ClientSocket.Send(pBuf, nSize);
}

bool MCustomClient::SocketRecvEvent(void* pCallbackContext, SOCKET sock, char* pPacket, DWORD dwSize)
{
	MCustomClient* pClient = (MCustomClient*)pCallbackContext;

	return pClient->OnSockRecv(sock, pPacket, dwSize);
}

bool MCustomClient::SocketConnectEvent(void* pCallbackContext, SOCKET sock)
{
	MCustomClient* pClient = (MCustomClient*)pCallbackContext;

	return pClient->OnSockConnect(sock);
}

bool MCustomClient::SocketDisconnectEvent(void* pCallbackContext, SOCKET sock)
{
	MCustomClient* pClient = (MCustomClient*)pCallbackContext;
	return pClient->OnSockDisconnect(sock);
}

void MCustomClient::SocketErrorEvent(void* pCallbackContext, SOCKET sock, SOCKET_ERROR_EVENT ErrorEvent, int &ErrorCode)
{
	MCustomClient* pClient = (MCustomClient*)pCallbackContext;
	pClient->OnSockError(sock, ErrorEvent, ErrorCode);
}

