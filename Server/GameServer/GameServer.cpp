//------------------------------------------
// GameServer.cpp
// Webzen Mu Game Server
// Date: 02-02-2007
// Decompilation by: Deathway
// Description : Main entry of the Program, 
//				 Start all servers
//------------------------------------------
//	GS-N	1.00.18	JPN	0x0052D520	-	Completed
#include "stdafx.h"
#include "MonsterHerd.h"
#include "resource.h"
#include "GameServer.h"
#include "Gamemain.h"
#include "Giocp.h"
#include "logproc.h"
#include "GameServerAuth.h"
#include "RingAttackEvent.h"
#include "CWhatsUpDummyServer.h"
#include "DBSockMng.h"
#include "DSProtocol.h"
#include "EDSProtocol.h"
#include "SProtocol.h"
#include "ChaosCastle.h"
#include "Protocol.h"
#include "AcceptIP.h"
#include "user.h"
#include "closeplayer.h"
#include "MapServerManager.h"
#include "protocol.h"
#include "Shop.h"
#include "Weapon.h"
#include "CastleSiegeSync.h"
#include "BloodCastle.h"
#include "EledoradoEvent.h"
#include "EventManagement.h"
#include "TServerInfoDisplayer.h"
#include "TMonsterAI.h"
#include "Kanturu.h"

char ExDbIp[256];

int gServerReady;
int gCloseMsg;
int gCloseMsgTime;
int gCurPaintPlayer;
int gCurPaintType;
int gCurPaintMapNumber;




CRingMonsterHerd g_MonsterHerdTest;
CRingAttackEvent g_RingAttackEvent;
CWhatsUpDummyServer gWhatsUpDummyServer;

HINSTANCE hInst; // Current Instance
HWND ghWnd;

char szTitle[100];
char szWindowClass[64];




BOOL GameServerStart(void);
int InitInstance(HINSTANCE, int);	// Init GameServer
ATOM MyRegisterClass(HINSTANCE);	// Register Window Class
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);// Brain of the GameServer
LRESULT CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);

//#pragma pack(2)
//#pragma pack(show)  // C4810

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{

/*-----------------------------------------
    GameServer.exe
	Company: Webzen
	Country: Korea
	Decompiled by: Deathway
------------------------------------------*/

	MSG msg;
	HACCEL hAccelTable;

/*
	HANDLE hMutex=CreateMutex(NULL, FALSE, "WZ_MU_GS_MUTEX");
	if (GetLastError()==ERROR_ALREADY_EXISTS) 
	{
		CloseHandle(hMutex);
		MessageBox(NULL, "Webzen Mu Game Server is already Running.","Error", MB_OK|MB_ICONHAND|MB_APPLMODAL);
		return 0;
	}
*/
	// Check if the original language == KOREA
	if (gLanguage==0) 
	{
		WIN32_FIND_DATA  wfd;	// Nice trick from WebZen local Scope
		if (FindFirstFile(COMMONSERVER_PATH,&wfd)==INVALID_HANDLE_VALUE)
		{
			//MsgBox(COMMONSERVER_PATH);
			MessageBox(NULL, "Cannot find COMMONSERVER.", "Error", MB_OK|MB_ICONHAND|MB_APPLMODAL);
			return 0; 
		}
	}	
	//Load Title and WindowClass Name
	LoadString(hInstance,IDS_APP_TITLE, szTitle, 100);
	LoadString(hInstance,IDS_GAMESERVER, szWindowClass, 100);

	MyRegisterClass(hInstance);

	InitCommonControls();
	
	// Check if error ocurs
	if (!InitInstance(hInstance, nCmdShow))
	{
		return 0;
	}

	ShowWindow(ghWnd, SW_SHOWMAXIMIZED);// Showm GameServer Window

	hAccelTable=LoadAccelerators(hInstance,(LPCSTR)IDA_GAMESERVER);// Load Accelerators
	GiocpInit();
	GameMainInit(ghWnd);

	memset(ExDbIp, 0, sizeof(ExDbIp));
	ExDbPort=55906;

	if ( ExDbIp[0] == 0 )
		//memcpy(ExDbIp, DataServerIp, sizeof(ExDbIp));

	if ( ExDbPort == 0 )
		ExDbPort = 55906;

	GameServerPort=GetPrivateProfileInt("GameServerInfo","GameServerPort",55970,".\\GameServer.cfg");
	gWhatsUpDummyServer.Start(ghWnd, GameServerPort+1);
	AllServerStart();

	// Main Message Loop
	while(GetMessage(&msg, NULL, 0, 0) != 0)
	{
		if (!TranslateAccelerator(msg.hwnd,hAccelTable,&msg))
		{
		    TranslateMessage(&msg);
		    DispatchMessage(&msg);
		}
	}

	return msg.wParam;
}


		
BOOL AllServerStart(void){
	
	if (JoinServerConnected==0)
	{
		char szJoinServerIp[256];
		GetPrivateProfileString("GameServerInfo","JoinServerIP","",szJoinServerIp,256,".\\GameServer.cfg");
		int nPort = GetPrivateProfileInt("GameServerInfo","JoinServerPort",55970,".\\GameServer.cfg");
		if (GMJoinServerConnect(szJoinServerIp, nPort, WM_GM_JOIN_CLIENT_MSG_PROC) == 0)
		{	
			// LoginServer connect fail
			MsgBox(lMsg.Get(MSGGET(1, 144)));
			return 0;
		}
	}


	/*if ((DevilSquareEventConnect==0) && (IsDevilSquareEventConnected==0))
	{
		if (GMRankingServerConnect(gDevilSquareEventServerIp, WM_GM_RANKING_CLIENT_MSG_PROC) == 0)
		{
			MsgBox("Ranking Server에 접속 안됨");
			return 0;
		}
		IsDevilSquareEventConnected=1;
	}*/

	/*
	if ((EventChipServerConnect!=0) && (IsEventChipServerConnected==0))
	{
		if (GMEventChipServerConnect(gEventChipServerIp, WM_GM_EVENTCHIP_CLIENT_MSG_PROC) == 0)
		{
			MsgBox("Event Chip Server에 접속 안됨");
			return 0;
		}
		IsEventChipServerConnected=1;
	}*/

	{
		char szDBServerIp0[256];
		char szDBServerIp1[256];
		GetPrivateProfileString("GameServerInfo","DataServerIP","",szDBServerIp0,256,".\\GameServer.cfg");
		GetPrivateProfileString("GameServerInfo","DataServerIP",szDBServerIp0,szDBServerIp1,256,".\\GameServer.cfg");
		int nPort0=GetPrivateProfileInt("GameServerInfo","DataServerPort",55960,".\\GameServer.cfg");
		int nPort1=GetPrivateProfileInt("GameServerInfo","DataServerPort2",55962,".\\GameServer.cfg");

		cDBSMng.Connect(szDBServerIp0,nPort0,szDBServerIp1,nPort1);
	}
	return TRUE;
}



BOOL GameServerStart(void) 
{
	if (GameMainServerCreate(WM_GM_SERVER_MSG_PROC, WM_GM_CLIENT_MSG_PROC) == FALSE )
	{
		MsgBox(lMsg.Get(MSGGET( 1, 142) ));
		return FALSE;
	}


	CreateGIocp(GameServerPort);

	SetTimer(ghWnd, WM_LOG_PAINT, 2000, NULL);
	SetTimer(ghWnd, WM_FIRST_MSG_PROCESS, 1000, NULL);

	SetTimer(ghWnd, WM_SET_DATE, 60000, NULL);
	SetTimer(ghWnd, WM_LOG_DATE_CHANGE, 60000, NULL);
	SetTimer(ghWnd, WM_CONNECT_DATASERVER, 10000, NULL);


	SetTimer(ghWnd, WM_SECOND_MSG_PROCESS, 1000, NULL);
	SetTimer(ghWnd, WM_GS_CLOSE, 1000, NULL);
	SetTimer(ghWnd, WM_MONSTER_AND_MSG_PROC, 500, NULL);


	SetTimer(ghWnd, WM_MOVE_MONSTER_PROC, 300, NULL);
	SetTimer(ghWnd, WM_EVENT_RUN_PROC, 100, NULL);

	SetTimer(ghWnd, WM_AI_MONSTER_MOVE_PROC, 100, NULL);
	SetTimer(ghWnd, WM_AI_MONSTER_PROC, 1011, NULL);

	SetTimer(ghWnd, WM_GUILD_LIST_REQUEST, 1000, NULL);

	return TRUE;
}




//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
//  COMMENTS:
//
//    This function and its usage is only necessary if you want this code
//    to be compatible with Win32 systems prior to the 'RegisterClassEx'
//    function that was added to Windows 95. It is important to call this function
//    so that the application will get 'well formed' small icons associated
//    with it.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX); 

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= (WNDPROC)WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, (LPCTSTR)IDI_NORMAL_V1);
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= (LPCSTR)IDR_MENU;
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, (LPCTSTR)IDI_ICON_MINIMIZED);

	return RegisterClassEx(&wcex);
}

//
//   FUNCTION: InitInstance(HANDLE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{

	HWND hWnd;
	hInst=hInstance;


    hWnd = CreateWindowEx(
        0,
        szWindowClass,
        szTitle,
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, 0, CW_USEDEFAULT,0,
        NULL, NULL, hInstance, NULL);
	
	if (hWnd==0) 
	{
		return 0;
	}

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	ghWnd=hWnd;
	return TRUE;
}

/*
Function       : static, [0x0012de50][0x0001:0x0012ce50], len = 00000edb, long __stdcall WndProc(struct HWND__ *,unsigned int,unsigned int,long)
FuncDebugStart :   static, [0x0012de71][0x0001:0x0012ce71]
FuncDebugEnd   :   static, [0x0012ebf4][0x0001:0x0012dbf4]
Data           :   static, [0x0012ed0f][0x0001:0x0012dd0f], Static Local, 
Data           :   static, [0x0012ecf3][0x0001:0x0012dcf3], Static Local, 
Data           :   static, [0x0012ec7f][0x0001:0x0012dc7f], Static Local, 
Data           :   static, [0x0012ec4f][0x0001:0x0012dc4f], Static Local, 
Data           :   static, [0x0012ec37][0x0001:0x0012dc37], Static Local, 
Data           :   static, [0x0012ec07][0x0001:0x0012dc07], Static Local, 
Label          :   static, [0x0012e0f1][0x0001:0x0012d0f1], $L83241
Label          :   static, [0x0012e121][0x0001:0x0012d121], $L83244
Label          :   static, [0x0012e12b][0x0001:0x0012d12b], $L83245
Label          :   static, [0x0012e149][0x0001:0x0012d149], $L83248
Label          :   static, [0x0012e168][0x0001:0x0012d168], $L83250
Label          :   static, [0x0012e187][0x0001:0x0012d187], $L83252
Label          :   static, [0x0012e191][0x0001:0x0012d191], $L83253
Label          :   static, [0x0012e1a0][0x0001:0x0012d1a0], $L83254
Label          :   static, [0x0012e1fa][0x0001:0x0012d1fa], $L83258
Label          :   static, [0x0012e213][0x0001:0x0012d213], $L83259
Label          :   static, [0x0012e22c][0x0001:0x0012d22c], $L83260
Label          :   static, [0x0012e25d][0x0001:0x0012d25d], $L83263
Label          :   static, [0x0012e281][0x0001:0x0012d281], $L83266
Label          :   static, [0x0012e2a5][0x0001:0x0012d2a5], $L83269
Label          :   static, [0x0012e31b][0x0001:0x0012d31b], $L83273
Label          :   static, [0x0012e356][0x0001:0x0012d356], $L83275
Label          :   static, [0x0012e365][0x0001:0x0012d365], $L83276
Label          :   static, [0x0012e374][0x0001:0x0012d374], $L83277
Label          :   static, [0x0012e383][0x0001:0x0012d383], $L83278
Label          :   static, [0x0012e3a1][0x0001:0x0012d3a1], $L83280
Label          :   static, [0x0012e3b0][0x0001:0x0012d3b0], $L83281
Label          :   static, [0x0012e3bf][0x0001:0x0012d3bf], $L83282
Label          :   static, [0x0012e3ce][0x0001:0x0012d3ce], $L83283
Label          :   static, [0x0012e3dd][0x0001:0x0012d3dd], $L83284
Label          :   static, [0x0012e3ec][0x0001:0x0012d3ec], $L83285
Label          :   static, [0x0012e3fb][0x0001:0x0012d3fb], $L83286
Label          :   static, [0x0012e40a][0x0001:0x0012d40a], $L83287
Label          :   static, [0x0012e419][0x0001:0x0012d419], $L83288
Label          :   static, [0x0012e428][0x0001:0x0012d428], $L83289
Label          :   static, [0x0012e437][0x0001:0x0012d437], $L83290
Label          :   static, [0x0012e446][0x0001:0x0012d446], $L83291
Label          :   static, [0x0012e455][0x0001:0x0012d455], $L83292
Label          :   static, [0x0012e461][0x0001:0x0012d461], $L83293
Label          :   static, [0x0012e46d][0x0001:0x0012d46d], $L83294
Label          :   static, [0x0012e479][0x0001:0x0012d479], $L83295
Label          :   static, [0x0012e485][0x0001:0x0012d485], $L83296
Label          :   static, [0x0012e491][0x0001:0x0012d491], $L83297
Label          :   static, [0x0012e49d][0x0001:0x0012d49d], $L83298
Label          :   static, [0x0012e4a9][0x0001:0x0012d4a9], $L83299
Label          :   static, [0x0012e4b5][0x0001:0x0012d4b5], $L83300
Label          :   static, [0x0012e517][0x0001:0x0012d517], $L83306
Label          :   static, [0x0012e52c][0x0001:0x0012d52c], $L83307
Label          :   static, [0x0012e541][0x0001:0x0012d541], $L83308
Label          :   static, [0x0012e556][0x0001:0x0012d556], $L83309
Label          :   static, [0x0012e56f][0x0001:0x0012d56f], $L83310
Label          :   static, [0x0012e588][0x0001:0x0012d588], $L83311
Label          :   static, [0x0012e59d][0x0001:0x0012d59d], $L83312
Label          :   static, [0x0012e675][0x0001:0x0012d675], $L83319
Label          :   static, [0x0012e71c][0x0001:0x0012d71c], $L83326
Label          :   static, [0x0012e726][0x0001:0x0012d726], $L83327
Label          :   static, [0x0012e73a][0x0001:0x0012d73a], $L83328
Label          :   static, [0x0012e77b][0x0001:0x0012d77b], $L83329
Label          :   static, [0x0012ea79][0x0001:0x0012da79], $L83370
Label          :   static, [0x0012eb08][0x0001:0x0012db08], $L83384
Label          :   static, [0x0012eb2f][0x0001:0x0012db2f], $L83386
Label          :   static, [0x0012eb56][0x0001:0x0012db56], $L83389
Label          :   static, [0x0012eb78][0x0001:0x0012db78], $L83391
Label          :   static, [0x0012eb9f][0x0001:0x0012db9f], $L83393
Data           :   ebp Relative, [0x00000008], Param, Type: struct HWND__ *, hWnd
Data           :   ebp Relative, [0x0000000c], Param, Type: unsigned int, message
Data           :   ebp Relative, [0x00000010], Param, Type: unsigned int, wParam
Data           :   ebp Relative, [0x00000014], Param, Type: long, lParam
Data           :   ebp Relative, [0xffffffec], Local, Type: int, wmEvent
Data           :   ebp Relative, [0xfffffff0], Local, Type: int, wmId
Block          :   static, [0x0012e0f1][0x0001:0x0012d0f1], len = 0000002b, (none)
Data           :     ebp Relative, [0xffffffe8], Local, Type: class Ccloseplayer, cp
Block          :   static, [0x0012e4f6][0x0001:0x0012d4f6], len = 0000001c, (none)
Data           :     ebp Relative, [0xffffffe0], Local, Type: int, lp
Data           :     ebp Relative, [0xffffffe4], Local, Type: int, wp
Block          :   static, [0x0012e691][0x0001:0x0012d691], len = 00000081, (none)
Data           :     ebp Relative, [0xffffffcc], Local, Type: struct HDC__ *, hdc
Data           :     ebp Relative, [0xffffffd0], Local, Type: struct tagRECT, rect
Block          :   static, [0x0012e78f][0x0001:0x0012d78f], len = 0000014a, (none)
Data           :     ebp Relative, [0xffffffc8], Local, Type: int, n
Block          :   static, [0x0012e8de][0x0001:0x0012d8de], len = 00000082, (none)
Data           :     ebp Relative, [0xffffffc4], Local, Type: int, n
Block          :   static, [0x0012e9db][0x0001:0x0012d9db], len = 00000049, (none)
Data           :     ebp Relative, [0xfffffec4], Local, Type: char[0x100], szTemp
Block          :   static, [0x0012eb9f][0x0001:0x0012db9f], len = 00000025, (none)
Data           :     ebp Relative, [0xfffffeb4], Local, Type: struct PMSG_CHARMAPJOIN, Msg
*/

//
//  FUNCTION: WndProc(HWND, unsigned, WORD, LONG)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND	- process the application menu
//  WM_PAINT	- Paint the main window
//  WM_DESTROY	- post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	INT wmId, wmEvent;
	
	switch (message) 
	{
		case WM_CREATE:
			//gWhatsUpDummyServer.Start(hWnd, 55902);
			return 0;
		case WM_COMMAND:
			wmId    = wParam & 0xFFFF & 0xFFFF; 
			wmEvent = (wParam >> 16) & 0xFFFF; 
					
			// Parse the menu selections:
			switch (wmId)
			{
				case ID_CONTROL_CHAT:
					DialogBox(hInst, (LPCTSTR)IDD_ABOUT_BOX, hWnd, (DLGPROC)About);
					break;
				case IDM_ABOUT:
				   DialogBox(hInst, (LPCTSTR)IDD_ABOUT_BOX, hWnd, (DLGPROC)About);
				   break;
				case IDM_EXIT:
				   SendMessage(hWnd, WM_CLOSE, 0, 0);
				   break;
				case IDM_CLOSE_PLAYER:
					{
						Ccloseplayer closeplayer;
						closeplayer.Load("closeplayer.txt");
					}
					break;
				case IDM_ALL_USER_CLOSE:
					gObjAllDisconnect();
					break;
				case IDM_ALL_USER_LOGOUT:
					gObjAllLogOut();
					break;
				case IDM_MONSTER_POSITION_RELOAD:
					GameMonsterAllCloseAndReLoad();
					break;
				case IDM_SHOP_RELOAD:
					ShopDataLoad();
					break;
				case IDM_MAP_SERVER_INFO_RELOAD:
					g_MapServerManager.LoadData(gDirPath.GetNewPath("MapServerInfo.dat") );
					break;
				#if(GS_CASTLE==1)					
				case IDM_CASTLE_SIEGE_RELOAD: //GS-CS Decompiled 100%
					if( g_CastleSiege.Ready(g_MapServerManager.GetMapSvrGroup()) == TRUE )
					{
						if( g_CastleSiege.LoadData(gDirPath.GetNewPath("MuCastleData.dat")) )
						{
							g_CastleSiege.LoadPreFixData(gDirPath.GetNewPath("commonserver.cfg"));
							g_CastleSiege.SetDataLoadState(CASTLESIEGE_DATALOAD_2);
						}
					}
					break;
				#endif
				case IDM_IP_LOAD:
					acceptIP.Load(gDirPath.GetNewPath("Iplist.dat"));
					break;
				case IDM_OPTION_RELOAD:
					ReadCommonServerInfo();
					break;
				case IDM_LOG_PAINT:
					gCurPaintType=0;
					break;
				case  IDM_LORENCIA:  //4000
					gCurPaintType=1;
					gCurPaintMapNumber = wmId-4000;
					break;
				case  IDM_DUNGEON:  //4001			   
					gCurPaintType=1;
					gCurPaintMapNumber = wmId-4000;
					break;
				case  IDM_DEVIAS:  //4002
					gCurPaintType=1;
					gCurPaintMapNumber = wmId-4000;
					break;
				case  IDM_NORIA:  //4003
					gCurPaintType=1;
					gCurPaintMapNumber = wmId-4000;
					break;
				case  IDM_LOSTTOWER:  //400//4
					gCurPaintType=1;
					gCurPaintMapNumber = wmId-4000;
					break;
				case  IDM_ATHLANSE:  //4007
					gCurPaintType=1;
					gCurPaintMapNumber = wmId-4000;
					break;
				case  IDM_TARKAN:  //4008
					gCurPaintType=1;
					gCurPaintMapNumber = wmId-4000;
					break;
				case  IDM_ICARUS:  //400a
					gCurPaintType=1;
					gCurPaintMapNumber = wmId-4000;
					break;
				case IDM_DRAGON_EVENT:
					if (  DragonEvent->GetState() != 0 )
						DragonEvent->End();
					else
						DragonEvent->Start();
					break;
				case IDM_STARTOFXMAS_EVENT:
					if ( gXMasEvent != 0 )
						gXMasEvent = 0;
					else
						gXMasEvent=1;
					break;
				case IDM_FIRECRACKER:
					if ( gFireCrackerEvent != 0 )
						gFireCrackerEvent = 0;
					else
						gFireCrackerEvent=1;
					break;
				case IDM_5_MINUTE_CLOSE:
					if ( gCloseMsg == 0 )
					{
						gCloseMsgTime = 300;
						gCloseMsg=1;
						AllSendServerMsg( lMsg.Get(MSGGET(4, 104)) );
					}
					break;
				case IDM_3_MINUTE_CLOSE:
					if ( gCloseMsg == 0 )
					{
						gCloseMsgTime = 180;
						gCloseMsg=2;
						AllSendServerMsg(lMsg.Get(MSGGET(4, 105)) );
					}
					break;
				case IDM_1_MINUTE_CLOSE:
					if ( gCloseMsg == 0 )
					{
						gCloseMsgTime = 60;
						gCloseMsg=3;
						AllSendServerMsg(lMsg.Get(MSGGET(4, 106)) );
					}
					break;
					case IDM_GS_CLOSE_BUG:
					if ( gCloseMsg == 0 )
					{
						gCloseMsgTime = 1;
						gCloseMsg=3;
						AllSendServerMsg("Server Closing for fixing bugs.");
						AllSendServerMsg("We back in few minutes.");
					}
					break;
				case IDM_RELOAD_ALL_EVENT:
					ReadEventInfo(MU_EVENT_ALL );
					break;
				case IDM_RELOAD_DEVILSQUARE:
					ReadEventInfo(MU_EVENT_DEVILSQUARE );
					break;
				case IDM_RELOAD_BLOODCASTLE:
					ReadEventInfo(MU_EVENT_BLOODCASTLE );
					break;
				case IDM_RELOAD_UNDERWORLD_ATTACK:
					ReadEventInfo(MU_EVENT_ATTACKEVENTTROOP );
					break;
				case IDM_RELOAD_GOLDENTROOP_ATTACK:
					ReadEventInfo(MU_EVENT_GOLDENTROOP );
					break;
				case IDM_RELOAD_WHITEMAGE_ATTACK:
					ReadEventInfo(MU_EVENT_WHITEMAGETROOP );
					break;
				case IDM_RELOAD_LOVE_PANGPANG:
					ReadEventInfo(MU_EVENT_LOVEPANGPANG );
					break;
				case IDM_RELOAD_FIRECRACKER:
					ReadEventInfo(MU_EVENT_FIRECRACKER );
					break;
				case IDM_RELOAD_XMAS_STAR:
					ReadEventInfo(MU_EVENT_XMASSTAR );
					break;
				case IDM_RELOAD_HEART_OF_LOVE:
					ReadEventInfo(MU_EVENT_HEARTOFLOVE );
					break;
				case IDM_RELOAD_NPC_SAY_HAPPY_NEW_YEAR:
					ReadEventInfo(MU_EVENT_SAY_HAPPYNEWYEAR );
					break;
				case IDM_RELOAD_NPC_SAY_MERRY_XMAS:
					ReadEventInfo(MU_EVENT_SAY_MERRYXMAS );
					break;
				case IDM_RELOAD_CHAOSCASTLE:
					ReadEventInfo(MU_EVENT_CHAOSCASTLE );
					break;
				case IDM_RELOAD_CHRISTMAS_RIBBONBOX:
					ReadEventInfo(MU_EVENT_CHRISTMAS_RIBBONBOX );
					break;
				case IDM_RELOAD_VALENTINE_DAY_CHOCOLATE_BOX:
					ReadEventInfo(MU_EVENT_VALENTINESDAY_CHOCOLATEBOX );
					break;
				case IDM_RELOAD_WHITE_DAY_CANDY_BOX:
					ReadEventInfo(MU_EVENT_WHITEDAY_CANDYBOX  );
					break;
				case IDM_RELOAD_ALL_ETC_OPTION:
					ReadGameEtcInfo(MU_ETC_ALL );
					break;
				case IDM_RELOAD_CREATE_CHARACTER:
					ReadGameEtcInfo(MU_ETC_CREATECHARACTER );
					break;
				case IDM_RELOAD_GUILD:
					ReadGameEtcInfo(MU_ETC_GUILD );
					break;
				case IDM_RELOAD_TRADE:
					ReadGameEtcInfo(MU_ETC_TRADE );
					break;
				case IDM_RELOAD_CHAOSBOX:
					ReadGameEtcInfo(MU_ETC_USECHAOSBOX );
					break;
				case IDM_RELOAD_PERSONAL_SHOP:
					ReadGameEtcInfo(MU_ETC_PERSONALSHOP );
					break;
				case IDM_RELOAD_PK_ITEM_DROP:
					ReadGameEtcInfo(MU_ETC_PKITEMDROP );
					break;
				case IDM_RELOAD_ITEM_DROP_RATE:
					ReadGameEtcInfo(MU_ETC_ITEMDROPRATE );
					break;
				case IDM_RELOAD_SPEEDHACK:
					ReadGameEtcInfo(MU_ETC_SPEEDHACK );
					break;
				case IDM_RELOAD_GAMEGUARD_CHECKSUM_CHECK:
					ReadGameEtcInfo(MU_ETC_GAMEGUARD );
					break;
				default:
					return DefWindowProc(hWnd, message, wParam, lParam);
					break;
			}
			break;
		case WM_GM_SERVER_MSG_PROC:
			GMServerMsgProc(wParam, lParam);
			break;
		case WM_GM_CLIENT_MSG_PROC:
			{
				int wp = wParam;
				int lp = lParam;
				GMClientMsgProc(wParam, lParam);
			}
			break;
		case WM_GM_JOIN_CLIENT_MSG_PROC:
			GMJoinClientMsgProc(wParam, lParam);
			break;
		case WM_GM_RANKING_CLIENT_MSG_PROC:
			GMRankingClientMsgProc(wParam, lParam);
			break;
		case WM_GM_EVENTCHIP_CLIENT_MSG_PROC:
			GMEventChipClientMsgProc(wParam, lParam);
			break;
		case WM_GM_DATA1_CLIENT_MSG_PROC:
			cDBSMng.MsgProc(0, wParam, lParam);
			break;
		case WM_GM_DATA2_CLIENT_MSG_PROC:
			cDBSMng.MsgProc(1, wParam, lParam);
			break;
		case WM_GM_EXDATA_CLIENT_MSG_PROC:
			ExDataClientMsgProc(wParam, lParam);
			break;
		case WM_START_SERVER:
			if ( gServerReady == 2 )
				GameServerStart();
			break;
		case WM_TIMER:
			switch ( wParam )
			{
				case WM_GUILD_LIST_REQUEST:
					DGGuildListRequest();
					KillTimer(ghWnd, WM_GUILD_LIST_REQUEST);
					break;
				case WM_LOG_PAINT:
					{
						RECT rect;
						HDC hdc = GetDC(hWnd);
						GetClientRect(hWnd, &rect);
						FillRect(hdc, &rect, (HBRUSH)GetStockObject(0));
						ReleaseDC(hWnd, hdc);

						if ( gCurPaintType == 0)
						{
							if ( LogTextPaint != NULL )
							{
								LogTextPaint(hWnd);
							}
						}
						
						gObjViewportPaint(hWnd, gCurPaintPlayer);
						g_ServerInfoDisplayer.Run(hWnd);
						GJNotifyMaxUserCount();
					}
					break;
				case WM_MONSTER_AND_MSG_PROC:
					MonsterAndMsgProc();
					break;
				case WM_MOVE_MONSTER_PROC:
					MoveMonsterProc();
					g_Kanturu.UserMonsterCountCheck();
					break;
				case WM_EVENT_RUN_PROC:
					g_BloodCastle.Run();
					g_RingAttackEvent.Run();
					g_ChaosCastle.Run();
				#if (GS_CASTLE==1)
					g_CastleSiege.Run();
					g_CastleDeepEvent.Run();
				#endif
					g_CsNPC_Weapon.WeaponAttackProc();
									#if (GS_CASTLE==1)
					g_Crywolf.Run();
				#endif
				#if (GS_CASTLE==0)
					g_Kanturu.Run(); 
				#endif

					break;
				case WM_AI_MONSTER_MOVE_PROC:
					TMonsterAI::MonsterMoveProc();
					break;
				case WM_AI_MONSTER_PROC:
					TMonsterAI::MonsterAIProc();
					break;
				case WM_FIRST_MSG_PROCESS:
					{
						for ( int n=0;n<OBJMAX;n++)
						{
							gObjStateSetCreate(n);
						}
						for (int  n=0;n<OBJMAX;n++)
						{
							gObjViewportListDestroy(n);
						}
						for (int  n=0;n<OBJMAX;n++)
						{
							gObjViewportListCreate(n);
						}
						for (int n=0;n<OBJMAX;n++)
						{
							gObjViewportListProtocol(n);
						}
#pragma message("Optimize this loops on WndProc")
						if ( gDoPShopOpen != FALSE )
						{
							for (int n= OBJ_STARTUSERINDEX;n<OBJMAX;n++)
							{
								PShop_ViewportListRegenarate(n);
							}
							for (int n=OBJ_STARTUSERINDEX;n<OBJMAX;n++)
							{
								if (gObjIsConnected(n) != PLAYER_EMPTY )
								{
									gObj[n].m_bPShopItemChange = false;
								}
							}
						}
						for (int n=OBJ_STARTUSERINDEX;n<OBJMAX;n++)
						{
							gObjUnionUpdateProc(n);
						}
						gObjSetState();
						GameServerInfoSend();
					}
					break;
				case WM_SECOND_MSG_PROCESS:
					{
						for (int n = 0;n<MAX_NUMBER_MAP;n++)
						{
							MapC[n].WeatherVariationProcess();
						}
						wsGServer.m_SendSec = wsGServer.m_SendSecTmp;
						wsGServer.m_RecvSec = wsGServer.m_RecvSecTmp;
						wsGServer.m_SendSecTmp = 0;
						wsGServer.m_RecvSecTmp = 0;
						DragonEvent->Run();
						AttackEvent->Run();
						gEledoradoEvent.Run();
						g_EventManager.Run();
					}
					break;
				case WM_GS_CLOSE:
					if ( gCloseMsg != 0 )
					{
						gCloseMsgTime--;

						if ( gCloseMsgTime <= 1 )
						{
							if ( gCloseMsgTime == 1 )
							{
								AllSendServerMsg(lMsg.Get(MSGGET(1, 193)));
								LogAddC(2, lMsg.Get(MSGGET(1, 193)));
							}
						}
						else
						{
							if ( (gCloseMsgTime % 10) == 0 )
							{
								char szTemp[256];

								wsprintf( szTemp, lMsg.Get(MSGGET(1, 194)), gCloseMsgTime);
								AllSendServerMsg(szTemp);
								LogAddC(2, szTemp);
							}
						}

						if ( gCloseMsgTime < 0 )
						{
							KillTimer(ghWnd, WM_GS_CLOSE);
							gObjAllLogOut();
							gCloseMsgTime = 0;
							gCloseMsg = 0;
						}
					}
					gObjSecondProc();
					#if (GS_CASTLE == 1 )
					if( cDBSMng.m_ActiveServer > DS_UNINITIALIZED && g_CastleSiege.GetDataLoadState() == 2 )
					{
						g_CastleSiege.DataRequest();
					}
					
					g_CastleSiege.SendCastleStateSync();

					#endif
					g_CastleSiegeSync.AdjustTributeMoney();
					break;
				case WM_SET_DATE:
					gSetDate();
					WhisperCash.TimeCheckCashDelete();
					#if(GS_CASTLE == 1)
					g_Crywolf.CrywolfSecondAct();
					#endif
					break;
				case WM_CONNECT_DATASERVER:
					{
						char szDBServerIp0[256];
						char szDBServerIp1[256];
						GetPrivateProfileString("GameServerInfo","DataServerIP","",szDBServerIp0,256,".\\GameServer.cfg");
						GetPrivateProfileString("GameServerInfo","DataServerIP",szDBServerIp0,szDBServerIp1,256,".\\GameServer.cfg");
						int nPort0=GetPrivateProfileInt("GameServerInfo","DataServerPort",55960,".\\GameServer.cfg");
						int nPort1=GetPrivateProfileInt("GameServerInfo","DataServerPort2",55962,".\\GameServer.cfg");
						cDBSMng.Connect(szDBServerIp0,nPort0,szDBServerIp1,nPort1);
					}
					gObjCheckAllUserDuelStop();
					break;
				case WM_LOG_DATE_CHANGE:
#if (GS_CASTLE == 1 )
					if( LogDateChange() == TRUE )
					{
						g_iCastleItemMixLimit = 1;
					}
#else
					LogDateChange();
#endif
					break;
					break;
			}
			break;
		case WM_CLOSE:
			if ( gLanguage != 3 )
			{
				if (MessageBox(ghWnd, "GameServer close?", "Close", MB_YESNO|MB_APPLMODAL) == IDYES)
				{
					DestroyWindow(hWnd);
				}
			}
			else
			{
				DestroyWindow(hWnd);
			}
			break;
		case WM_KEYDOWN:
			switch ( wParam )
			{
				case 'p':
					gCurPaintPlayer--;
					if ( gCurPaintPlayer<0)
						gCurPaintPlayer=0;
					break;
				case 'q':
					gCurPaintPlayer++;
					if ( gCurPaintPlayer > OBJMAX -1 )
						gCurPaintPlayer = OBJMAX -1;
					break;
				case 't':
					gServerMaxUser -= 10;
					if ( gServerMaxUser < 10 )
						gServerMaxUser = 10;
					break;

				case 'u':
					gServerMaxUser += 10;
					if ( gServerMaxUser > 1500 )
						gServerMaxUser = 1500;
					break;
				case 'v':
					{
						PMSG_CHARMAPJOIN szMsg("테스트");// Test
						CGPCharacterMapJoinRequest(&szMsg, 0);
					}
					break;
			}
			break;
		case WM_DESTROY:
			GameMainFree();
			GiocpDelete();
			PostQuitMessage(0);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
   }
   return 0;
}


// Message handler for about box.
LRESULT CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
		case WM_INITDIALOG:
				return TRUE;

		case WM_COMMAND:
			if ( ( wParam & 0xFFFF & 0xFFFF ) == IDOK || (wParam & 0xFFFF & 0xFFFF ) == IDCANCEL) 
			{
				EndDialog(hDlg, (wParam& 0xFFFF & 0xFFFF ));
				return TRUE;
			}
			break;
	}
    return FALSE;
}


