#pragma once

#include <map>
#include <string>
using std::map;
using std::string;

#include "MClient.h"
#include "..\\..\\ServerKeeper\\ServerKeeperConst.h"
#include "MMatchGlobal.h"

// #define PORT				7500
#define KEEPERSERVER_UID	MUID(0, 3)	///< KEEPERSERVER_UID�ǰ��� UID(�Һ�), ServerKeeper������Ʈ�� �ߺ����ǰ� ����. 
#define ADD_MAX_LEN			15

class CScheduleInfo;

typedef map< COLUMN_ID, CONFIG_STATE > ConfigStateMap;


class CKeeper : public MClient
{
public:
	CKeeper( const int nID, const int nPort );
	~CKeeper(void);

	/// ���� ������ ����Ѵ�.
	void OutputLocalInfo(void)
	{
	}

	/// �⺻ ���
	void OutputMessage(const char* szMessage, MZMOMType nType=MZMDM_GENERAL)
	{
	}

	string					GetName() const				{ return m_strName; }
	string					GetAddr() const				{ return m_strAddr; }
	int						GetID()						{ return m_nID; }
	const CONNECTION_STATE	GetConnectionState()		{ return m_ConnectionState; }
	const JOB_STATE			GetLastJobState()			{ return m_LastJobState; }
	const RUN_STATE			GetServerRunState()			{ return m_ServerRunState; }
	const RUN_STATE			GetAgentRunState()			{ return m_AgentRunState; }
	const SERVER_ERR_STATE	GetServerErrState()			{ return m_ServerErrState; }
	const CONFIG_STATE		GetConfigState( const COLUMN_ID nColumnID);
	const string			GetServerResVer() const		{ return m_strServerResVer; }
	const string			GetServerLastMod() const	{ return m_strServerLastMod; }
	const ULONGLONG			GetServerFileSize() const	{ return m_nServerFileSize; }
	const string			GetAgentLastMod() const		{ return m_strAgentLastMod; }
	const ULONGLONG			GetAgentFileSize() const	{ return m_nAgentFileSize; }
	const string			GetKeeperLastMod() const 	{ return m_strKeeperLastMod; }
	const unsigned char		GetAgentCount() const		{ return m_nAgentCount; }

	bool SetName( const char* pszName );
	bool SetAddr( const char* pszAddr );
		
	const bool IsConnectedKeeper()	const { return m_bIsConnected; }
	
	void ReleaseAll();

	void InvertServerConfigState();
	void InvertAgentConfigState();
	void InvertDownloadConfigState();
	void InvertPrepareConfigState();
	void InvertPatchConfigState();

	bool OneTimeInit();
    
	bool Connect();
	void Disconnect(MUID uid);
	bool PostConnect();
	void PostSetOneConfig( const COLUMN_ID nColumnID, const CONFIG_STATE nConfigState );

	bool OnCommand( MCommand* pCommand );
	void OnRecvPing( MCommand* pCommand );
	
	void OnResponseConnectKeeperManager( MUID& uidMyUID );
	
	// 2005.06.03
	void CheckKeeperHeartbeat();

	// Keeper������Ʈ. ���⼭ �ϰ� �۾��� �̷����.
	void Update();

	void RequestKeeperConnectMatchServer();
	void RequestConnectionState();
	void RequestRefreshServer();
	void RequestLastJobState();
	void RequestAnnounce( const char* pszAnnounce, const int nLen );
	void ReqeustResetPatch();
	void RequestDisconnectServer();
	void RequestRebootWindows();
	void RequestServerStopWithAnnounce();
	void RequestSchedule( const CScheduleInfo* pScheduleInfo );
	void ReqeustServerAgentState();
	void RequestWriteClientCRC( const DWORD dwClientCRC );
	void RequestReloadServerConfig( const string& strReloadFileList );
	void RequestAddHashMap( const string& strNewHashMap );
	
	//////////////////////////////////////////////////////////////////////////////////
	/// ���� ��ġ ���� �κ�.
	void RequestStopServer();
	void RequestStartServer();
	void RequestPrepareServerPatch();
	void RequestServerPatch();
	void RequestDownloadServerPatchFile();
	//////////////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////////////
	/// ������Ʈ ��ġ ���� �κ�.
	void RequestStopAgentServer();
	void RequestStartAgentServer();
	void RequestPrepareAgentPatch();
	void RequestAgentPatch();
	void RequestDownloadAgentPatchFile();
	//////////////////////////////////////////////////////////////////////////////////
	
	////
	virtual int OnConnected(SOCKET sock, MUID* pTargetUID, MUID* pAllocUID, unsigned int nTimeStamp);
	virtual bool OnSockDisconnect(SOCKET sock);
	virtual void SendCommand( MCommand* pCommand );
	///

private :
	void OnResponseKeeperConnectMatchServer( const MUID& uidKeeper );
	void OnResponseConnectionState( const CONNECTION_STATE nServerState );
	void OnResponseLastJobState( const JOB_STATE nJob, const JOB_STATE nResult );
	void OnResponseConfigState( const CONFIG_STATE nServerConfig, const CONFIG_STATE nAgentConfig, const CONFIG_STATE nDownloadConfig,
								const CONFIG_STATE nPrepareConfig, const CONFIG_STATE nPatchConfig );
	void OnResponseSetOneConfig( const COLUMN_ID nColumnID, const CONFIG_STATE nConfigState );
	void OnResponseServerStopWithAnnounce();
	void OnResponseServerAgentState( const RUN_STATE ServerRunState, const RUN_STATE AgentRunState );

	void OnResponseServerStatus( const SERVER_ERR_STATE ServerErrState, 
								 const char* pszServerResVer, 
								 const char* pszServerLastMod, 
								 const char* pszAgentLastMod,
								 const char* pszKeeperLastMod,
								 const ULONGLONG nServerFileSize,
								 const ULONGLONG nAgentFileSize,
								 const ULONGLONG nKeeperFileSize,
								 const unsigned char nAgentCount );

	void OnResponseWriteClientCRC( const bool bResult );
	void OnResponseKeeperAddHashMap( const MUID& ServerUID, const bool bRes );

private :
	CKeeper(void);

	void SetConnectionState( const CONNECTION_STATE nState );
	void SetLastJobState( const JOB_STATE nState );
	void SetConfigState( const COLUMN_ID nColumnID, const CONFIG_STATE nConfigState );
	void SetLastRecvCmdTime( const DWORD dwTime = timeGetTime() );
	void SetServerRunState( const RUN_STATE ServerRunState	);
	void SetAgentRunState( const RUN_STATE AgentRunState );
	void SetServerErrState( const SERVER_ERR_STATE ServerState );

	void InitConfigStateColumns();
	void AddColumnConfigState( const COLUMN_ID nColumnID, const CONFIG_STATE nConfigState );
	void ResetColumnConfigState();
	

private :
	// int		m_State;
	string	m_strAddr;
	string	m_strName;

	int				m_nID;
	int				m_nPort;
	bool			m_bIsConnected;

	CONNECTION_STATE	m_ConnectionState;
	JOB_STATE			m_LastJobState;
	ConfigStateMap		m_ConfigStateMap;
	DWORD				m_dwLastRecvCmdTime;
	RUN_STATE			m_ServerRunState;
	RUN_STATE			m_AgentRunState;

	SERVER_ERR_STATE	m_ServerErrState;
	string				m_strServerResVer;		// ���� ������ ����.
	string				m_strServerLastMod;		// ���� �������� ������ ������.
	string				m_strAgentLastMod;		// ������Ʈ �������� ������ ������.
	string				m_strKeeperLastMod;
	ULONGLONG			m_nServerFileSize;
	ULONGLONG			m_nAgentFileSize;
	ULONGLONG			m_nKeeperFileSize;
	unsigned char		m_nAgentCount;
};