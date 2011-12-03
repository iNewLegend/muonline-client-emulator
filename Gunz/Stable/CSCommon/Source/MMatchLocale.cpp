#include "stdafx.h"
#include "MMatchLocale.h"
#include "MMatchServer.h"
#include "MMatchConfig.h"

MMatchLocale* MMatchLocale::GetInstance()
{
	static MMatchLocale m_stLocale;
	return &m_stLocale;
}

MMatchLocale::MMatchLocale() : MBaseLocale(), m_pDBAgentClient(NULL), m_bCheckAntiHackCrack(false)
{

}

MMatchLocale::~MMatchLocale()
{
	if (m_pDBAgentClient) 
	{
		delete m_pDBAgentClient; m_pDBAgentClient = NULL;
	}
}

bool MMatchLocale::OnInit()
{
	m_bCheckAntiHackCrack = false;

	switch (m_nCountry)
	{
		case MC_KOREA:
			break;

		case MC_US:
			break;

		case MC_JAPAN:
			{
//				int nGameCode = MGetServerConfig()->GetNJDBAgentGameCode();
//				int nServerCode = MGetServerConfig()->GetServerID();
//				m_pDBAgentClient = new MNJ_DBAgentClient(nGameCode, nServerCode);
//				ConnectToDBAgent();		// �Ϻ� �ݸ����Ǹ� DBAgent�� �����Ѵ�.
			}
			break;

		case MC_BRAZIL:
			break;

		case MC_INDIA:
			break;
	}

#ifdef _XTRAP
	// XTrap Crack�� üũ�Ѵ�.
	if( MGetServerConfig()->IsUseXTrap() )
		m_bCheckAntiHackCrack = true;		
#endif

	return true;
}

bool MMatchLocale::ConnectToDBAgent()
{
//	if (m_nCountry != MC_JAPAN) return false;

	if ((m_pDBAgentClient) && (!m_pDBAgentClient->IsConnected()) && (!m_pDBAgentClient->GetSock()->IsActive()))
	{
		char szIP[128];
		strcpy(szIP, MGetServerConfig()->GetNJDBAgentIP());
		m_pDBAgentClient->Connect(szIP, MGetServerConfig()->GetNJDBAgentPort());

		MMatchServer::GetInstance()->LOG(MMatchServer::LOG_PROG, "Connect to DBAgent\n");
		return true;
	}

	return false;
}


bool MMatchLocale::PostLoginInfoToDBAgent(const MUID& uidComm, const char* szCN, const char* szPW, bool bFreeLoginIP, unsigned long nChecksumPack, int nTotalUserCount)
{
//	if (m_nCountry != MC_JAPAN)
	{
		mlog( "country code is not JPN.\n" );
		return false;
	}

	if (m_pDBAgentClient)
	{
		if (!m_pDBAgentClient->GetSock()->IsActive())
		{
			if (!m_pDBAgentClient->IsConnected())
			{
				mlog( "Retry connect to DBAgent.\n" );
				ConnectToDBAgent();
				return false;
			}
		}

		if (m_pDBAgentClient->IsConnected())
		{
			m_pDBAgentClient->Send(uidComm, szCN, szPW, bFreeLoginIP, nChecksumPack, nTotalUserCount);
			return true;
		}

		mlog( "DBAgent not connect.\n" );
	}
	
	mlog( "DBAgentClient NULL.\n" );
	
	return false;
}

//bool MMatchLocale::SkipCheckAntiHackCrack()
//{
//#ifdef _DEBUG
//	return true;
//#endif

//	return !m_bCheckAntiHackCrack;
//}