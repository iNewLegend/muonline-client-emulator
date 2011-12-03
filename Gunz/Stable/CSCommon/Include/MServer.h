#ifndef MSERVER_H
#define MSERVER_H

#include "MCommandCommunicator.h"
#include "RealCPNet.h"

#include <list>
using namespace std;

class MCommand;

/// ����
class MServer : public MCommandCommunicator {
protected:
	MRealCPNet					m_RealCPNet;

	list<MCommObject*>			m_AcceptWaitQueue;
	CRITICAL_SECTION			m_csAcceptWaitQueue;

	void LockAcceptWaitQueue()		{ EnterCriticalSection(&m_csAcceptWaitQueue); }
	void UnlockAcceptWaitQueue()		{ LeaveCriticalSection(&m_csAcceptWaitQueue); }

	MUIDRefCache				m_CommRefCache;			///< ���� ������ ������ �ٸ� Ŀ�´������� ĳ��
	CRITICAL_SECTION			m_csCommList;

	void LockCommList()			{ EnterCriticalSection(&m_csCommList); }
	void UnlockCommList()		{ LeaveCriticalSection(&m_csCommList); }

	MCommandList				m_SafeCmdQueue;
	CRITICAL_SECTION			m_csSafeCmdQueue;
	void LockSafeCmdQueue()		{ EnterCriticalSection(&m_csSafeCmdQueue); }
	void UnlockSafeCmdQueue()	{ LeaveCriticalSection(&m_csSafeCmdQueue); }

	/// ���ο� UID ����
	// virtual MUID UseUID(void) = 0;
	virtual MUID UseUID(void) { return MUID(0, 0); }

	void AddCommObject(const MUID& uid, MCommObject* pCommObj);
	void RemoveCommObject(const MUID& uid);
	void InitCryptCommObject(MCommObject* pCommObj, unsigned int nTimeStamp);

	void PostSafeQueue(MCommand* pNew);
	void PostSafeQueue(MCommandBuilder* pCommandBuilder);

	/// Low-Level Command Transfer Function. ���߿� ��Ƶξ��ٰ� �� ���۵��� �����ϰ� ���� �� �ִ�.
	void SendCommand(MCommand* pCommand);
	void ParsePacket(MCommObject* pCommObj, MPacketHeader* pPacket);

	/// Ŀ�´������� ���� �� �غ�
	virtual void  OnPrepareRun(void);
	/// Ŀ�´������� ����
	virtual void OnRun(void);
	/// ����� Ŀ�ǵ� ó��
	virtual bool OnCommand(MCommand* pCommand);

	virtual void OnNetClear(const MUID& CommUID);
	virtual void OnNetPong(const MUID& CommUID, unsigned int nTimeStamp);
	virtual void OnHShieldPong(const MUID& CommUID, unsigned int nTimeStamp) {};

	bool SendMsgReplyConnect(MUID* pHostUID, MUID* pAllocUID, unsigned int nTimeStamp, MCommObject* pCommObj);
	bool SendMsgCommand(DWORD nClientKey, char* pBuf, int nSize, unsigned short nMsgHeaderID, MPacketCrypterKey* pCrypterKey);

	static void RCPCallback(void* pCallbackContext, RCP_IO_OPERATION nIO, DWORD nKey, MPacketHeader* pPacket, DWORD dwPacketLen);	// Thread not safe

	bool m_bFloodCheck;

public:	// For Debugging
	char m_szName[128];
	void SetName(char* pszName) { strcpy(m_szName, pszName); }
	void DebugLog(char* pszLog) {
		#ifdef _DEBUG
		char szLog[128];
		wsprintf(szLog, "[%s] %s \n", m_szName, pszLog);
		OutputDebugString(szLog);
		#endif
	}

public:
	MServer(void);
	~MServer(void);

	/// �ʱ�ȭ
	bool Create(int nPort, const bool bReuse = false );
	/// ����
	void Destroy(void);
	int GetCommObjCount();


	/// �ٸ� Ŀ��Ƽ�����ͷ� ���� ����
	/// @param	pAllocUID	�ڱ� Communicator�� �������� UID
	/// @return				���� �ڵ� (MErrorTable.h ����)
	virtual int Connect(MCommObject* pCommObj);	// ������н� �ݵ�� Disconnect() ȣ���ؾ���
	int ReplyConnect(MUID* pTargetUID, MUID* pAllocUID, unsigned int nTimeStamp, MCommObject* pCommObj);
	virtual int OnAccept(MCommObject* pCommObj);
	/// �α��εǾ�����
	virtual void OnLocalLogin(MUID CommUID, MUID PlayerUID);
	/// ���� ����
	virtual void Disconnect( const MUID& uid );	
	virtual int OnDisconnect(const MUID& uid);	// Thread not safe

	virtual void Log(unsigned int nLogLevel, const char* szLog){}

	void SetFloodCheck(bool bVal)	{ m_bFloodCheck = bVal; }			///< (ũ�� ������ ���� �ʰ�����) �����ڿ����� ȣ�����ֵ��� �սô�.
	bool IsFloodCheck()				{ return m_bFloodCheck; }
};

#endif
