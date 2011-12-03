#include "MClient.h"
#include "MGameClient.h"
#include "ConsoleCmds.h"
#include "MCommand.h"
#include "MCommandManager.h"

static MConsoleCmdManager	g_ConsoleCmdManager;
extern MConsoleFrame*		g_pConsole;
extern MGameClient			g_Client;


void OutputToConsole(char* str)
{
	g_pConsole->AddLines(str);
}

// �ֿܼ��� �Է�â�� �Է��������� �̺�Ʈ�� �޴� �Լ�
void ConsoleInputEvent(const char* szInputStr)
{
	char szBuf[1024];

	if (!g_ConsoleCmdManager.DoCommand(szInputStr, szBuf))
	{
		OutputToConsole(szBuf);
	}
}

// ���� ���� �̺�Ʈ
void SocketErrorEvent(void* pCallbackContext, SOCKET sock, SOCKET_ERRER_EVENT ErrorEvent, int &ErrorCode)
{
	char szText[1024];
	sprintf(szText, "���Ͽ����Դϴ�. ErrorCode: %d", ErrorCode);
	OutputToConsole(szText);
}

// ���� ���� �̺�Ʈ
bool SocketConnectEvent(void* pCallbackContext, SOCKET sock)
{
	char szText[1024];
	sprintf(szText, "���ӵǾ����ϴ�.");
	OutputToConsole(szText);

	return true;
}

bool SocketDisconnectEvent(void* pCallbackContext, SOCKET sock)
{
	char szText[1024];
	sprintf(szText, "������ �����Ǿ����ϴ�.");
	OutputToConsole(szText);

	return true;
}

// �ֿܼ� ���� ��ɾ� �ʱ�ȭ
void InitConsoleCmds()
{
	g_pConsole->SetInputCallback(ConsoleInputEvent);

	g_ConsoleCmdManager.AddCmd
	(
		"?",									// ��ɾ� �̸�
		HelpCmd,								// ��ɾ��Է½� ������ �Լ� ������
		"[optional commands]",					// usage
		"shows commands, or detailed help on specified commands" // help
	);

	g_ConsoleCmdManager.AddCmd
	(
		"help", 
		HelpCmd,
		"[optional commands]",
		"shows commands, or detailed help on specified commands"
	);

	g_ConsoleCmdManager.AddCmd
	(
		"clear", 
		ClearCmd,
		"[Clear]",
		"�ܼ� ȭ���� �����."
	);

	g_ConsoleCmdManager.AddCmd
	(
		"@", 
		ClientCmd,
		"[Client]",
		"Client�� ���� Command�� ������."
	);

	g_ConsoleCmdManager.AddCmd
		(
		"connect", 
		ConnectCmd,
		"[connect ip port]",
		"�ش� ip�� ������ �����Ѵ�. ip, port�� �Է����� ������ 127.0.0.1 6000�� �����Ѵ�."
		);
	g_ConsoleCmdManager.AddCmd
		(
		"con", 
		ConnectCmd,
		"[con]",
		"�ش� ip�� ������ �����Ѵ�. ip, port�� �Է����� ������ 127.0.0.1 6000�� �����Ѵ�."
		);

	g_ConsoleCmdManager.AddCmd
		(
		"disconnect", 
		DisconnectCmd,
		"[disconnect]",
		"�������� ������ ���´�."
		);
	g_ConsoleCmdManager.AddCmd
		(
		"dis", 
		DisconnectCmd,
		"[dis]",
		"�������� ������ ���´�."
		);

	g_ConsoleCmdManager.AddCmd
		(
		"send",
		SendCmd,
		"[send data]",
		"������ Custom Data ����"
		);

}

void HelpCmd(const int argc, char **const argv)
{
	char szBuf[1024];
	char szBuf2[1024];

	bool bShowAll = false;
	int nColumns = 40;

	if (argc == 2) 
	{
		bShowAll = (stricmp(argv[1], "showall") == 0);
	}

	if ((argc == 1) || (bShowAll))
	{
		OutputToConsole("'help <command>' gives a brief description of <command>\n");

		int nCol = 0;
		memset(szBuf, 0, sizeof(szBuf));

		for (MConsoleCmdItor itor = g_ConsoleCmdManager.GetCmdMap()->begin(); 
			          itor != g_ConsoleCmdManager.GetCmdMap()->end(); ++itor)
		{
			string sName = (*itor).first;

			sprintf(szBuf2, "%16s", sName.c_str());
			strcat(szBuf, szBuf2);
			
			nCol++;
			if (nCol > 2)
			{
				nCol = 0;
				OutputToConsole(szBuf);
				memset(szBuf, 0, sizeof(szBuf));
			}
		}
		if (nCol > 0) OutputToConsole(szBuf);

		return;
	}

	for (int i = 1; i < argc; i++)
	{
		MConsoleCmd* pCmd = g_ConsoleCmdManager.GetCmdByName(argv[i]);
		if (pCmd != NULL)
		{
			sprintf(szBuf, "help: %s\nusage: %s\n", pCmd->GetHelp(), pCmd->GetUsage());
			OutputToConsole(szBuf);
		}
	}
}

void ClearCmd(const int argc, char **const argv)
{
	g_pConsole->ClearLines();
}

void ClientCmd(const int argc, char **const argv)
{
	char szBuf[65535];

	strcpy(szBuf, "");
	for (int i = 1; i < argc; i++)
	{
		strcat(szBuf, argv[i]);
		if (i != argc-1) strcat(szBuf, " ");
	}

	char szErr[1024];
	if (!g_Client.Post(szErr, 1024, szBuf)) OutputToConsole("Post Error");
}


// ����
void ConnectCmd(const int argc, char **const argv)
{
	if (g_Client.GetClientSocket()->IsActive())
	{
		OutputToConsole("�̹� ���ӵǾ� �ֽ��ϴ�.");
		return;
	}

	if (argc < 3)
	{
		// Test �������� argv�� ������ 127.0.0.1, 6000 port �� �����Ѵ�.
		g_Client.Connect("127.0.0.1", 6000);
	}
	else if (argc == 3)
	{
		g_Client.Connect(argv[1], atoi(argv[2]));
	}
	else
	{
		OutputToConsole("���ڰ� Ʋ�Ƚ��ϴ�.");
	}
	
}

// ��������
void DisconnectCmd(const int argc, char **const argv)
{
	if (!g_Client.GetClientSocket()->IsActive())
	{
		OutputToConsole("���ӵǾ� ���� �ʽ��ϴ�.");
		return;
	}
	g_Client.GetClientSocket()->Disconnect();
}


void SendCmd(const int argc, char **const argv)
{
	if (argc < 2) 
	{
		OutputToConsole("���ڰ� �����մϴ�.");
		return;
	}

	if (!g_Client.GetClientSocket()->IsActive())
	{
		OutputToConsole("���ӵǾ� ���� �ʽ��ϴ�.");
		return;
	}

	char szBuf[256];
	strcpy(szBuf, argv[1]);

	g_Client.GetClientSocket()->Send(szBuf, DWORD(strlen(szBuf) + 1));
}
