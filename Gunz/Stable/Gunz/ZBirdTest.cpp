#include "stdafx.h"

#include "ZGameInterface.h"
#include "ZApplication.h"
#include "ZPost.h"
#include "ZConsole.h"
#include "MCommandLogFrame.h"
#include "ZConfiguration.h"
#include "FileInfo.h"
#include "ZInterfaceItem.h"
#include "ZInterfaceListener.h"
#include "MDebug.h"
#include "MMatchTransDataType.h"
#include "MBlobArray.h"
#include "MTCPSocket.h"
#include "ZGameClient.h"
#include "time.h"
#include "ZSecurity.h"

// ���⼭���� �׽�Ʈ�� ���� �ڵ� - Bird ////////////////////////////////////////////////
#ifdef _BIRDTEST

#include "ZBirdDummyClient.h"
#include "ZBirdDummyAI.h"


#define MAX_CLIENT		2000
#define MAX_DUMMYCASE	6

int g_nDummyCount = MAX_CLIENT;
int g_nDummyCaseCount[MAX_DUMMYCASE] = {0, };

ZBirdDummyClient			g_DummyClient[MAX_CLIENT];
ZBirdDummyAIJoinFlood		g_DummyAI[MAX_CLIENT];


////////////////////////////////////////////////////////////////////////////////////////////
typedef void (*ZBT_ONCommand)(MCommand* pCmd);


char g_szBT_ID[256] = "";
ZGameInterface* g_pGIBirdTest = NULL;


// ���� ���� Ŭ���̾�Ʈ �÷���
void OnBTDummyTest(MCommand* pCmd);
void InitBTDummyClient();
void UpdateBTDummyClient();
void OnBTDummyConnFloodOnCommand(ZBirdDummyClient* pClient, MCommand* pCmd);
void OnBTDummyJoinRoomFloodOnCommand(ZBirdDummyClient* pClient, MCommand* pCmd);
void OnBTDummyGameFloodOnCommand(ZBirdDummyClient* pClient, MCommand* pCmd);
void OnBTDummyChannelChatFloodOnCommand(ZBirdDummyClient* pClient, MCommand* pCmd);
void OnBTDummyEchoFloodOnCommand(ZBirdDummyClient* pClient, MCommand* pCmd);
void OnBTDummyChannelChangeOnCommand(ZBirdDummyClient* pClient, MCommand* pCmd);

// ���� �׽�Ʈ�ϰ� �ִ� �׸� ����
ZBT_ONCommand g_pBTCommandCallBack = NULL;

void ZGameInterface::OnBirdTestCreate()
{
	g_pGIBirdTest = this;

	char szID[256], szPassword[256];
	ZIDLResource* pResource = ZApplication::GetGameInterface()->GetIDLResource();
	MWidget* pWidget = pResource->FindWidget("LoginID");
	strcpy(szID, pWidget->GetText());

	pWidget = pResource->FindWidget("LoginPassword");
	strcpy(szPassword, pWidget->GetText());

	
	g_nDummyCount = GetPrivateProfileInt("OPTION", "DummyCount", 100, "./BirdTest.ini");

	for (int i = 0; i < MAX_DUMMYCASE; i++)
	{
		char szKey[256];
		sprintf(szKey, "DummyCase%d", i+1);
		g_nDummyCaseCount[i] = GetPrivateProfileInt("OPTION", szKey, 0, "./BirdTest.ini");
	}
	
	g_pBTCommandCallBack = OnBTDummyTest;
	InitBTDummyClient();
}

void ZGameInterface::OnBirdTestDestroy()
{

}

void ZGameInterface::OnBirdTestUpdate()
{
	UpdateBTDummyClient();
}

void ZGameInterface::OnBirdTestDraw()
{

}

void ZGameInterface::OnBirdTestCommand(MCommand* pCmd)
{
	if (g_pBTCommandCallBack != NULL) g_pBTCommandCallBack(pCmd);
}

//////////////////////////////////////////////////////////////////////////////////////
void InitBTDummyClient()
{
	ZIDLResource* pResource = ZApplication::GetGameInterface()->GetIDLResource();
	MWidget* pAddr = pResource->FindWidget("ServerAddress");
	MWidget* pPort = pResource->FindWidget("ServerPort");
	if (!pAddr || !pPort) return;

	ZBT_DummyONCommand pCallBackFunc;

	int nCaseCounter = 0;
	for (int nCase = 0; nCase < MAX_DUMMYCASE; nCase++)
	{
		switch (nCase)
		{
		case 0: pCallBackFunc = OnBTDummyConnFloodOnCommand; break;
		case 1: pCallBackFunc = OnBTDummyJoinRoomFloodOnCommand; break;
		case 2: pCallBackFunc = OnBTDummyGameFloodOnCommand; break;
		case 3: pCallBackFunc = OnBTDummyChannelChatFloodOnCommand; break;
		case 4: pCallBackFunc = OnBTDummyEchoFloodOnCommand; break;
		case 5: pCallBackFunc = OnBTDummyChannelChangeOnCommand; break;
		}

		for (int i = 0; i < g_nDummyCaseCount[nCase]; i++)
		{
			g_DummyClient[nCaseCounter].Create(nCaseCounter, pCallBackFunc);
			ZGetGameClient()->GetCommandManager()->AssignDescs(g_DummyClient[nCaseCounter].GetCommandManager());

			if (nCase == 1)
			{
				g_DummyAI[nCaseCounter].Create(&g_DummyClient[nCaseCounter]);
			}

			// ������ �����Ѵ�
			char szCmd[256];
			sprintf(szCmd, "%s:%u", pAddr->GetText(), atoi(pPort->GetText()));
			ZBIRDPOSTCMD1(&g_DummyClient[nCaseCounter], MC_NET_CONNECT, MCmdParamStr(szCmd));

			nCaseCounter++;
		}
	}
}

void UpdateBTDummyClient()
{
	for (int i = 0; i < g_nDummyCount; i++)
	{
		g_DummyAI[i].Run();
		g_DummyClient[i].Run();
	}
}

// ���� ���� Ŭ���̾�Ʈ �÷��� - ���ξ�����
void OnBTDummyTest(MCommand* pCmd)
{
	switch(pCmd->GetID())
	{
	case MC_MATCH_RESPONSE_LOGIN:
		{

		}
		break;
	case MC_NET_ONCONNECT:
		{
			
		}
		break;
	case MC_NET_ONDISCONNECT:
		{

		}
		break;
	case MC_NET_ONERROR:
		{

		}
		break;

	}
}

bool OnCommonLogin(ZBirdDummyClient* pClient, MCommand* pCmd)
{
	switch (pCmd->GetID())
	{
	case MC_NET_ONCONNECT:
		{
			sprintf(g_szBT_ID, "BirdTest%d", pClient->GetDummyID());
			char szPassword[256] = "1111";

			ZBIRDPOSTCMD4(pClient, MC_MATCH_LOGIN, MCmdParamStr(g_szBT_ID), MCmdParamStr(szPassword), MCommandParameterInt(MCOMMAND_VERSION),
						MCommandParameterUInt(0));
		}
		return true;
	case MC_MATCH_RESPONSE_LOGIN:
		{
			ZBIRDPOSTCMD1(pClient, MC_MATCH_REQUEST_ACCOUNT_CHARLIST, MCommandParameterUID(pClient->GetPlayerUID()));
		}
		return true;
	case MC_NET_ONDISCONNECT:
		{
			ZIDLResource* pResource = ZApplication::GetGameInterface()->GetIDLResource();
			MWidget* pAddr = pResource->FindWidget("ServerAddress");
			MWidget* pPort = pResource->FindWidget("ServerPort");
			if (!pAddr || !pPort) return true;

			char szCmd[256];
			sprintf(szCmd, "%s:%u", pAddr->GetText(), atoi(pPort->GetText()));
			ZBIRDPOSTCMD1(pClient, MC_NET_CONNECT, MCmdParamStr(szCmd));
		}
		return true;
	case MC_NET_ONERROR:
		{

		}
		return true;
	case MC_MATCH_RESPONSE_ACCOUNT_CHARLIST:
		{
			MCommandParameter* pParam = pCmd->GetParameter(0);
			if(pParam->GetType()!=MPT_BLOB) break;
			void* pCharListBlob = pParam->GetPointer();
			int nCount = MGetBlobArrayCount(pCharListBlob);

			bool bExistChar = false;
			for (int i = 0; i < nCount; i++)
			{
				MTD_CharInfo* pCharInfo = (MTD_CharInfo*)MGetBlobArrayElement(pCharListBlob, i);
				if (pCharInfo->nCharNum == 0)
				{
					ZBIRDPOSTCMD2(pClient, MC_MATCH_REQUEST_SELECT_CHAR,
						MCommandParameterUID(pClient->GetPlayerUID()), MCommandParameterUInt(0));


					bExistChar = true;
					break;
				}
			}

			if (!bExistChar)
			{
				// ĳ���� ������ ���� ����
				char szCharName[256];
				sprintf(szCharName, "���岿��%d", pClient->GetDummyID());

				ZBIRDPOSTCMD7(pClient, MC_MATCH_REQUEST_CREATE_CHAR, 
					MCommandParameterUID(pClient->GetPlayerUID()), 
					MCommandParameterUInt(0),
					MCommandParameterString(szCharName), 
					MCommandParameterUInt(0), 
					MCommandParameterUInt(0),
					MCommandParameterUInt(0), 
					MCommandParameterUInt(0));
			}
		}
		return true;
	case MC_MATCH_RESPONSE_CREATE_CHAR:
		{
			ZBIRDPOSTCMD1(pClient, MC_MATCH_REQUEST_ACCOUNT_CHARLIST, MCommandParameterUID(pClient->GetPlayerUID()));
		}
		return true;
	case MC_MATCH_RESPONSE_SELECT_CHAR:
		{
			int nResult;
			pCmd->GetParameter(&nResult, 0, MPT_INT);
			if (nResult == MOK)
			{
				MCommandParameter* pParam = pCmd->GetParameter(1);
				if(pParam->GetType()!=MPT_BLOB) break;

				void* pCharBlob = pParam->GetPointer();
				MTD_CharInfo* pCharInfo = (MTD_CharInfo*)MGetBlobArrayElement(pCharBlob, 0);

				// ������ ����
				pClient->SetPlayerName(pCharInfo->szName);


				ZBIRDPOSTCMD0(pClient, MC_MATCH_REQUEST_RECOMMANDED_CHANNEL);
			}
			else
			{
				ZBIRDPOSTCMD1(pClient, MC_MATCH_REQUEST_ACCOUNT_CHARLIST, MCommandParameterUID(pClient->GetPlayerUID()));
			}
		}
		return true;
	case MC_MATCH_RESPONSE_RECOMMANDED_CHANNEL:
		{
			MUID uidChannel;
			pCmd->GetParameter(&uidChannel, 0, MPT_UID);

//			strcpy(szChannelName, pClient->GetChannelName());

			ZBIRDPOSTCMD2(pClient, MC_MATCH_CHANNEL_REQUEST_JOIN,
				MCommandParameterUID(pClient->GetPlayerUID()), 
				MCommandParameterUID(pClient->GetChannelUID()));

		}
		return true;
	}

	return false;
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////

// �α���, �α׾ƿ� �ݺ�
void OnBTDummyConnFloodOnCommand(ZBirdDummyClient* pClient, MCommand* pCmd)
{
	switch (pCmd->GetID())
	{
	case MC_NET_ONCONNECT:
		{
			sprintf(g_szBT_ID, "BirdTest%d", pClient->GetDummyID());
			char szPassword[256] = "1111";

			ZBIRDPOSTCMD4(pClient, MC_MATCH_LOGIN, MCmdParamStr(g_szBT_ID), MCmdParamStr(szPassword), MCommandParameterInt(MCOMMAND_VERSION),
						MCommandParameterUInt(0));


			static unsigned long int stConnCount = 0;
			stConnCount++;
			char szTemp[512];
			if ((stConnCount % 100) == 0)
			{
				sprintf(szTemp, "ConnFlood OnConnected(%u)", stConnCount++);
				AddToLogFrame(pClient->GetDummyID(), szTemp);
			}

			return;
		}
		break;
	case MC_NET_ONDISCONNECT:
		{
			ZIDLResource* pResource = ZApplication::GetGameInterface()->GetIDLResource();
			MWidget* pAddr = pResource->FindWidget("ServerAddress");
			MWidget* pPort = pResource->FindWidget("ServerPort");
			if (!pAddr || !pPort) return;

			char szCmd[256];
			sprintf(szCmd, "%s:%u", pAddr->GetText(), atoi(pPort->GetText()));
			ZBIRDPOSTCMD1(pClient, MC_NET_CONNECT, MCmdParamStr(szCmd));
			return;
		}
		break;
	case MC_MATCH_RESPONSE_RECOMMANDED_CHANNEL:
		{
			MUID uidChannel;
			char szChannelName[64];
			pCmd->GetParameter(&uidChannel, 0, MPT_UID);
			pCmd->GetParameter(szChannelName, 1, MPT_STR, sizeof(szChannelName) );

			ZBIRDPOSTCMD0(pClient, MC_NET_DISCONNECT);
			return;
		}
		break;
	}
	if (OnCommonLogin(pClient, pCmd)) return;
}

// �� ���� ���Դ� �ϱ�
void OnBTDummyJoinRoomFloodOnCommand(ZBirdDummyClient* pClient, MCommand* pCmd)
{
	if (OnCommonLogin(pClient, pCmd)) return;

	g_DummyAI[pClient->GetDummyID()].OnCommand(pCmd);
}

// ���� ������ �÷��� ����
void OnBTDummyGameFloodOnCommand(ZBirdDummyClient* pClient, MCommand* pCmd)
{

}

char g_szDummyChattingMsg[10][256] =
{ 
"�� �� �ϼ�..ļļļ",
"�� ���� �����̴�...����븶�մ� ����..~(-_-)~",
"�̹� ���� ��ڴԵ� ����̰ھ��. ���� �α⸦ �ݿ��Ѵٰ� ������...",
"ǥ������� �߸��ƴٰ� ���ϰ� �����̴ϴ�. ����� �ش�ð� ���� �ش�°��� �߸��ƴٰ� �����մϴٸ�..",
"������ �Ȱſ���? ó���̶�...",
"������ Ŭ���� ã�� �ֽ��ϴ�.",
"��� ���� ���� ���׿�... �װ� ���� �ּ���.¥����...",
"�̰��ӹ��Ό��?",
"���� ����� �ҷ��� 256�� ��� -_-��",
"���̸��̰ɷ�����ġ�����ֻ���"
};

// ä�ο��� �׾�� ä������
void OnBTDummyChannelChatFloodOnCommand(ZBirdDummyClient* pClient, MCommand* pCmd)
{
	if (OnCommonLogin(pClient, pCmd)) return;

	switch (pCmd->GetID())
	{
	case MC_MATCH_CHANNEL_RESPONSE_JOIN:
		{
			int nChatIndex = rand() % 10;
			ZBIRDPOSTCMD3(pClient, MC_MATCH_CHANNEL_REQUEST_CHAT,
                MCommandParameterUID(pClient->GetPlayerUID()), 
				MCommandParameterUID(pClient->GetChannelUID()), 
				MCmdParamStr(g_szDummyChattingMsg[nChatIndex])	);
		}
		break;
	case MC_MATCH_CHANNEL_CHAT:
		{
			MUID uidChannel;
			char szPlayerName[256];
			pCmd->GetParameter(&uidChannel,		0, MPT_UID);
			pCmd->GetParameter(szPlayerName,	1, MPT_STR, sizeof(szPlayerName) );

			// �� ä�� �޼����̸� �� ������
			
			if (!stricmp(szPlayerName, pClient->GetPlayerName()))
			{
				int nChatIndex = rand() % 10;
				ZBIRDPOSTCMD3(pClient, MC_MATCH_CHANNEL_REQUEST_CHAT,
					MCommandParameterUID(pClient->GetPlayerUID()), 
					MCommandParameterUID(pClient->GetChannelUID()), 
					MCmdParamStr(g_szDummyChattingMsg[nChatIndex]));
			}

			static unsigned long int st_nChatCount = 0;
			st_nChatCount++;
			char szTemp[512];

			if ((st_nChatCount % 100) == 0) {
				sprintf(szTemp, "ChatFlood(%u)", st_nChatCount);
				AddToLogFrame(pClient->GetDummyID(), szTemp);
			}
		}
		break;
	}
}

// �׾�� Echo �ϱ�
void OnBTDummyEchoFloodOnCommand(ZBirdDummyClient* pClient, MCommand* pCmd)
{
	switch (pCmd->GetID())
	{
	case MC_NET_ONCONNECT:
		{
			ZBIRDPOSTCMD1(pClient, MC_NET_ECHO, MCmdParamStr("����ȭ ���� �Ǿ����ϴ�.����ȭ ���� �Ǿ����ϴ�.����ȭ ���� �Ǿ����ϴ�."));
		}
		break;
	case MC_NET_ONDISCONNECT:
		{

		}
		break;
	case MC_NET_ONERROR:
		{

		}
		break;
	case MC_NET_ECHO:
		{
			static unsigned long int st_nEchoCount = 0;
			st_nEchoCount++;
			char szTemp[512];
			if ((st_nEchoCount % 100) == 0)
			{
				sprintf(szTemp, "EchoFlood(%u)", st_nEchoCount);
				AddToLogFrame(pClient->GetDummyID(), szTemp);
			}

			ZBIRDPOSTCMD1(pClient, MC_NET_ECHO, MCmdParamStr("����ȭ ���� �Ǿ����ϴ�.����ȭ ���� �Ǿ����ϴ�.����ȭ ���� �Ǿ����ϴ�."));
		}
		break;
	}
}

// ä�κ��� �׽�Ʈ
void OnBTDummyChannelChangeOnCommand(ZBirdDummyClient* pClient, MCommand* pCmd)
{
	if (OnCommonLogin(pClient, pCmd)) return;


	switch (pCmd->GetID())
	{
	case MC_MATCH_CHANNEL_RESPONSE_JOIN:
		{
			static unsigned long int st_nCount = 0;
			st_nCount++;
			char szTemp[128];
			if ((st_nCount % 100) == 0)
			{
				sprintf(szTemp, "Change Channel Flood(%u)", st_nCount);
				AddToLogFrame(pClient->GetDummyID(), szTemp);
			}


			ZBIRDPOSTCMD2(pClient, MC_MATCH_CHANNEL_LIST_START, MCommandParameterUID(pClient->GetPlayerUID()), MCommandParameterInt(0));
		}
		break;
	case MC_MATCH_CHANNEL_LIST:
		{
			MCommandParameter* pParam = pCmd->GetParameter(0);
			if(pParam->GetType()!=MPT_BLOB) break;
			void* pBlob = pParam->GetPointer();
			int nCount = MGetBlobArrayCount(pBlob);
		
			MUID uidChannels[100];

			for(int i=0; i<nCount; i++)
			{
				MCHANNELLISTNODE* pNode = (MCHANNELLISTNODE*)MGetBlobArrayElement(pBlob, i);

				if (i<100) uidChannels[i] = pNode->uidChannel;
			}

			int nIndex = rand() % 2;

			ZBIRDPOSTCMD1(pClient, MC_MATCH_CHANNEL_LIST_STOP, MCommandParameterUID(pClient->GetPlayerUID()));

			ZBIRDPOSTCMD2(pClient, MC_MATCH_CHANNEL_REQUEST_JOIN, 
				MCommandParameterUID(pClient->GetPlayerUID()), MCommandParameterUID(uidChannels[nIndex]));

		}
		break;
	}

}

// ������� �׽�Ʈ�� ���� �ڵ� - Bird //////////////////////////////////////////////////

#endif		// #ifdef _BIRDTEST
