#ifndef MCOMMANDCOMMUNICATOR_H
#define MCOMMANDCOMMUNICATOR_H

#include "MCommandManager.h"
#include "MUID.h"
#include "MTCPSocket.h"
#include "MPacketCrypter.h"

//#define _CMD_PROFILE

#ifdef _CMD_PROFILE
#include "MCommandProfiler.h"
#endif


class MCommandCommunicator;
class MCommandBuilder;


/// Ŀ�´������Ϳ��� ������ ���� ��ü. MCommandCommunicator::Connect()�� �Ķ���ͷ� ����.
class MCommObject {
protected:
	MUID					m_uid;

	MCommandBuilder*		m_pCommandBuilder;
	MPacketCrypter			m_PacketCrypter;

	MCommandCommunicator*	m_pDirectConnection;	// ������ Ŀ�ؼ� Direction Connection
	DWORD					m_dwUserContext;

	char					m_szIP[128];
	int						m_nPort;
	DWORD					m_dwIP;
	bool					m_bAllowed;

	bool					m_bPassiveSocket;
public:
	MCommObject(MCommandCommunicator* pCommunicator);
	virtual ~MCommObject();

	MUID GetUID()			{ return m_uid; }
	void SetUID(MUID uid)	{ m_uid = uid; }

	MCommandBuilder*	GetCommandBuilder()				{ return m_pCommandBuilder; }
	MPacketCrypter*		GetCrypter()					{ return &m_PacketCrypter; }

	MCommandCommunicator* GetDirectConnection()			{ return m_pDirectConnection; }
	void SetDirectConnection(MCommandCommunicator* pDC)	{ m_pDirectConnection = pDC; }
	DWORD GetUserContext()								{ return m_dwUserContext; }
	void SetUserContext(DWORD dwContext)				{ m_dwUserContext = dwContext; }

	char* GetIPString()		{ return m_szIP; }
	DWORD GetIP()			{ return m_dwIP; }
	int GetPort()			{ return m_nPort; }
	void SetAddress(const char* pszIP, int nPort) {
		strcpy(m_szIP, pszIP);
		m_dwIP = inet_addr(m_szIP);
		m_nPort = nPort;
	}
	void SetAllowed(bool bAllowed) { m_bAllowed = bAllowed; }
	bool IsAllowed() { return m_bAllowed; }

	// ���� �����ص� ������, ���� IOCP�� ������� ���Ͽ� ���� �κ�
	// Passive��, ����ڿ� ���� ������ CommObject�� �ƴ�, IOCP�󿡼� �ڵ����� �����Ǵ� CommObject�� �ǹ�.
	// (RCP_IO_ACCEPT���� ������, ���������� �����Ǵ� ����) 
	//																2010. 04. 06 - ȫ����
	void SetPassiveSocket(bool bActive) { m_bPassiveSocket = bActive; }
	bool IsPassiveSocket() { return m_bPassiveSocket; }

};


class MPacketInfo {
public:
	MCommObject*		m_pCommObj;
	MPacketHeader*		m_pPacket;

	MPacketInfo(MCommObject* pCommObj, MPacketHeader* pPacket) { m_pCommObj = pCommObj, m_pPacket = pPacket; }
};
typedef list<MPacketInfo*>			MPacketInfoList;
typedef MPacketInfoList::iterator	MPacketInfoListItor;


/// Ŀ�ǵ� ����� ����� ���������� Ŀ�´�������
class MCommandCommunicator{
protected:
	MCommandManager	m_CommandManager;		///< Ŀ�ǵ� �Ŵ���

	MUID			m_This;					///< �ڱ� Ŀ�´������� UID
	MUID			m_DefaultReceiver;		///< Ŀ�ǵ带 �Ľ��Ҷ� �⺻�� �Ǵ� Ÿ�� Ŀ�´������� UID

protected:
	/// Low-Level Command Transfer Function. ���߿� ��Ƶξ��ٰ� �� ���۵��� �����ϰ� ���� �� �ִ�.
	virtual void SendCommand(MCommand* pCommand)=0;
	/// Low-Level Command Transfer Function. ���߿� ��Ƶξ��ٰ� �� ���۵��� �����ϰ� ���� �� �ִ�.
	virtual void ReceiveCommand(MCommand* pCommand);

	/// �ʱ� Ŀ�ǵ� ����Ҷ� �Ҹ��� �Լ�
	virtual void OnRegisterCommand(MCommandManager* pCommandManager);
	/// Ŀ�´������Ͱ� Ŀ�ǵ带 ó���ϱ� ���� �Ҹ��� �Լ�
	virtual bool OnCommand(MCommand* pCommand);
	/// Ŀ�´������� ���� �� �غ�
	virtual void OnPrepareRun(void);
	/// Ŀ�ǵ带 ó���ϱ� ����
	virtual void OnPrepareCommand(MCommand* pCommand);
	/// Ŀ�´������� ����
	virtual void OnRun(void);

	/// Post()�Ǵ� �⺻ ���ù� Ŀ�´������� ����
	void SetDefaultReceiver(MUID Receiver);
public:
	MCommandCommunicator(void);
	virtual ~MCommandCommunicator(void);

	/// �ʱ�ȭ
	bool Create(void);
	/// ����
	void Destroy(void);

	/// �ٸ� Ŀ��Ƽ�����ͷ� ���� ����
	/// @param	pAllocUID	�ڱ� Communicator�� �������� UID
	/// @return				���� �ڵ� (MErrorTable.h ����)
	virtual int Connect(MCommObject* pCommObj)=0;
	/// Ŀ�ؼ��� �̷���� ���
	virtual int OnConnected(MUID* pTargetUID, MUID* pAllocUID, unsigned int nTimeStamp, MCommObject* pCommObj);
	/// ���� ����
	virtual void Disconnect( const MUID& uid)=0;

	/// Ŀ�ǵ� �Է�
	virtual bool Post(MCommand* pCommand);
	/// ��Ʈ������ Ŀ�ǵ� �Է�
	virtual bool Post(char* szErrMsg, int nErrMsgCount, const char* szCommand);

	virtual MCommand* GetCommandSafe();

	/// ���� ( Ŀ�ǵ� ���۹� ó�� )
	void Run(void);

	/// Ŀ�ǵ� �Ŵ��� ���
	MCommandManager* GetCommandManager(void){
		return &m_CommandManager;
	}
	MCommand* CreateCommand(int nCmdID, const MUID& TargetUID);

	// �ǹ̰� �����ǷΤ� LOG_PROG ���ֹ�����

	enum _LogLevel	{ LOG_DEBUG = 1, LOG_FILE = 3, LOG_PROG = 7, }; //, LOG_PROG = 7,  };
	/// ����Ʈ �α� ��� ( �α� ������ �����̷����� �� �ְ� ���ش�. )
	virtual void Log(unsigned int nLogLevel, const char* szLog){}
	void LOG(unsigned int nLogLevel, const char *pFormat,...);

	/// �ڽ��� UID�� ����.
	MUID GetUID(void){ return m_This; }

#ifdef _CMD_PROFILE
	MCommandProfiler		m_CommandProfiler;
#endif
};

int CalcPacketSize(MCommand* pCmd);

#endif
