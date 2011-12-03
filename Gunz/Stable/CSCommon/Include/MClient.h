#ifndef MCLIENT_H
#define MCLIENT_H

#include "MCommandCommunicator.h"
#include "MPacketCrypter.h"

class MClientCommandProcessor;
class MCommand;

#define RINGBUFSIZE 16384

// ���߿� ��Ŷ ���� ��ü�� ����� �������� �켱 �����д�.
class MRingBuffer {
private:
	int			m_iBufSize;
	char*		m_Buf;
	char*		m_cpBegin, m_cpEnd;
protected:
public:
	MRingBuffer();
	MRingBuffer(int iBufSize);
	virtual ~MRingBuffer();
	void Reserve(int iBufSize); 
	bool Enqueue(char* cp, int iDataSize);
	bool Dequeue(char* cpOut, int iDataSize);
	int GetSize()		{ return m_iBufSize; }
};

/// Message Type
enum MZMOMType{
	MZMDM_GENERAL,			///< �Ϲ� �޼���
	MZMOM_USERCOMMAND,		///< ����� �Է� Ŀ�ǵ�
	MZMOM_ERROR,			///< ����
	MZMOM_LOCALREPLY,		///< ���� ���� �޼���
	MZMOM_SERVERREPLY,		///< ���� ���� �޼���
};


/// Ŭ���̾�Ʈ
class MClient : public MCommandCommunicator{
private:
	static MClient*	m_pInstance;				///< ���� �ν��Ͻ�
protected:
	MUID				m_Server;				///< ����� Ŀ�´�������

	MClientSocket		m_ClientSocket;			///< Ŭ���̾�Ʈ ���Ͽ� Ŭ����
	CRITICAL_SECTION	m_csRecvLock;			///< CommandQueue critical section

	MCommandBuilder*	m_pCommandBuilder;
	MPacketCrypter		m_ServerPacketCrypter;	///< MatchServer���� ��ȣȭ Ŭ����

protected:
	void LockRecv() { EnterCriticalSection(&m_csRecvLock); }
	void UnlockRecv() { LeaveCriticalSection(&m_csRecvLock); }

	virtual void OnRegisterCommand(MCommandManager* pCommandManager);
	virtual bool OnCommand(MCommand* pCommand);

	/// ���� ������ ����Ѵ�.
	virtual void OutputLocalInfo(void) = 0;
	/// �⺻ ���
	virtual void OutputMessage(const char* szMessage, MZMOMType nType=MZMDM_GENERAL) = 0;

	MCommandBuilder* GetCommandBuilder()	{ return m_pCommandBuilder; }
	virtual void SendCommand(MCommand* pCommand);
	virtual MCommand* GetCommandSafe();

	virtual int OnConnected(MUID* pTargetUID, MUID* pAllocUID, unsigned int nTimeStamp, MCommObject* pCommObj);
	virtual int OnConnected(SOCKET sock, MUID* pTargetUID, MUID* pAllocUID, unsigned int nTimeStamp);
	int MakeCmdPacket(char* pOutPacket, int iMaxPacketSize, MPacketCrypter* pPacketCrypter, MCommand* pCommand);
	

	// Socket Event
	virtual bool OnSockConnect(SOCKET sock);
	virtual bool OnSockDisconnect(SOCKET sock);
	virtual bool OnSockRecv(SOCKET sock, char* pPacket, DWORD dwSize);
	virtual void OnSockError(SOCKET sock, SOCKET_ERROR_EVENT ErrorEvent, int &ErrorCode);

public:
	MClient();
	virtual ~MClient();

	/// ���� �ν��Ͻ� ���
	static MClient* GetInstance(void);

	MUID GetServerUID(void){ return m_Server; }
	virtual MUID GetSenderUIDBySocket(SOCKET socket);
	MClientSocket* GetClientSocket()						{ return &m_ClientSocket; }
	void GetTraffic(int* nSendTraffic, int* nRecvTraffic)	{ return m_ClientSocket.GetTraffic(nSendTraffic, nRecvTraffic); }

	virtual bool Post(MCommand* pCommand);
	virtual bool Post(char* szErrMsg, int nErrMsgCount, const char* szCommand);

	virtual int Connect(MCommObject* pCommObj);
	virtual int Connect(SOCKET* pSocket, char* szIP, int nPort);
	virtual void Disconnect( const MUID&  uid );
	virtual void Log(const char* szLog){}

	/// �Ķ���� ���
	void OutputMessage(MZMOMType nType, const char *pFormat,...);

	static bool SocketRecvEvent(void* pCallbackContext, SOCKET sock, char* pPacket, DWORD dwSize);
	static bool SocketConnectEvent(void* pCallbackContext, SOCKET sock);
	static bool SocketDisconnectEvent(void* pCallbackContext, SOCKET sock);
	static void SocketErrorEvent(void* pCallbackContext, SOCKET sock, SOCKET_ERROR_EVENT ErrorEvent, int &ErrorCode);

	bool IsConnected() { return m_ClientSocket.IsActive(); }
};

void SplitIAddress(char* szIP, int* pPort, const char* szAddress);

#endif
