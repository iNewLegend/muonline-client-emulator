#include "stdafx.h"
#include "MMatchServer.h"
#include "MSharedCommandTable.h"
#include "MMatchStatus.h"
#include "MDebug.h"
//#include "MErrorTable.h"
//#include "MMatchRule.h"


MMatchStatus::MMatchStatus()
{
	m_pMatchServer = NULL;
	m_bCreated = false;

	m_nTotalCommandQueueCount = 0;
	memset(m_nCmdCount, 0, sizeof(unsigned long int) * MSTATUS_MAX_CMD_COUNT * 3);
	memset(m_nDBQueryCount, 0, sizeof(unsigned long int) * MSTATUS_MAX_DBQUERY_COUNT * 3);
	// memset(m_nCmdHistory, 0, sizeof(unsigned long int) * MSTATUS_MAX_CMD_HISTORY);
	m_nHistoryCursor = 0;
	m_nRunStatus = -1;

	memset(m_szDump, 0, sizeof(m_szDump));
}

MMatchStatus::~MMatchStatus()
{

}

MMatchStatus* MMatchStatus::GetInstance()
{
	static MMatchStatus m_stMatchStatus;
	return &m_stMatchStatus;
}

bool MMatchStatus::Create(MMatchServer* pMatchServer)
{
	m_pMatchServer = pMatchServer;
	m_bCreated = true;
	m_nStartTime = timeGetTime();

	return true;
}

void MMatchStatus::SaveToLogFile()
{
	if (m_pMatchServer == NULL) return;

	char szBuf[65535];
	char szTemp[1024];

	// ����ð�
	sprintf(szBuf, "============================\n����ð� = %d��\n", (timeGetTime() - m_nStartTime) / 1000);
	mlog(szBuf);

	// �����ڼ�
	sprintf(szBuf, "�����ڼ� = %d\n", (int)MMatchServer::GetInstance()->GetObjects()->size());
	mlog(szBuf);

	// �氳��
	sprintf(szBuf, "�氳�� = %d\n", (int)MMatchServer::GetInstance()->GetStageMap()->size());
	mlog(szBuf);

	// �� ó�� ť ����, ����ƽ�� ť ����
	sprintf(szBuf, "��ó���� Ŀ�ǵ� = %u , ����ƽ Ŀ�ǵ� = %u\n", 
		m_nTotalCommandQueueCount, m_nTickCommandQueueCount);
	mlog(szBuf);

	mlog("ť ó�� ����\n");
	// �� ť�� ó�� ����
	szBuf[0] = 0;
	
	for (int i = 0; i < MSTATUS_MAX_CMD_COUNT; i++)
	{
		if (m_nCmdCount[i][0] != 0)
		{
			int nAvg = m_nCmdCount[i][1] / m_nCmdCount[i][0];

			sprintf(szTemp, "%5d : %4u, ���ó���ð�: %u(ms), ������ó���ð�: %u(ms)\n", i, m_nCmdCount[i][0], nAvg,
				m_nCmdCount[i][2]);
			strcat(szBuf, szTemp);
		}
	}
	mlog(szBuf);
	// �� ��� ������ ó�� ����
	mlog("������� ó�� ����\n");
	szBuf[0] = 0;

	for (int i = 0; i < MSTATUS_MAX_DBQUERY_COUNT; i++)
	{
		if (m_nDBQueryCount[i][0] != 0)
		{
			int nAvg = m_nDBQueryCount[i][1] / m_nDBQueryCount[i][0];

			sprintf(szTemp, "%5d : %4u, ���ó���ð�: %u(ms), ������ó���ð�: %u(ms)\n", i, m_nDBQueryCount[i][0], nAvg,
				m_nDBQueryCount[i][2]);
			strcat(szBuf, szTemp);
		}
	}
	mlog(szBuf);
	

}

void MMatchStatus::AddCmdHistory(unsigned long int nCmdID, const MUID& uidSender)
{
	m_CmdHistory[m_nHistoryCursor].nCmdID		= nCmdID;
	m_CmdHistory[m_nHistoryCursor].uidSender	= uidSender;

	m_nHistoryCursor++;
	if (m_nHistoryCursor >= MSTATUS_MAX_CMD_HISTORY) m_nHistoryCursor = 0;

	//m_nCmdHistory[m_nHistoryCursor] = nCmdID;
	//m_nHistoryCursor++;
	//if (m_nHistoryCursor >= MSTATUS_MAX_CMD_HISTORY) m_nHistoryCursor = 0;
}


void MMatchStatus::SetLog(const char* szDump)
{
	if ((int)strlen(szDump) < MATCHSTATUS_DUMP_LEN)
	{
		strcpy(m_szDump, szDump);
	}
	else
	{
		memset(m_szDump, 0, sizeof(m_szDump));
	}
}

