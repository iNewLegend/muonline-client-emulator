#include "stdafx.h"
//#include "../MatchServer/vld/vld.h"

#ifdef _HSHIELD
#include "HShield/HShield.h"
#endif

#ifdef _XTRAP
#include "./XTrap/Xtrap_C_Interface.h"						// update sgk 0702 start
#include "./XTrap/XTrap4Launcher.h"
#pragma comment (lib, "./XTrap/XTrap4Launcher_mt.lib")
#pragma comment (lib, "./XTrap/XTrap4Client_mt.lib")	// update sgk 0702 end
#endif

#include "ZPrerequisites.h"
#include "ZConfiguration.h"
#include "ZGameClient.h"
#include <windows.h>
#include <wingdi.h>
#include <mmsystem.h>
#include <shlwapi.h>
#include <shellapi.h>

#include "dxerr9.h"

#include "main.h"
#include "resource.h"
#include "VersionNo.h"

#include "Mint4R2.h"
#include "ZApplication.h"
#include "MDebug.h"
#include "ZMessages.h"
#include "MMatchNotify.h"
#include "RealSpace2.h"
#include "Mint.h"
#include "ZGameInterface.h"
#include "RFrameWork.h"
#include "ZButton.h"
#include "ZDirectInput.h"
#include "ZActionDef.h"
#include "MRegistry.h"
#include "ZInitialLoading.h"
#include "MDebug.h"
#include "MCrashDump.h"
#include "ZEffectFlashBang.h"
#include "ZMsgBox.h"
#include "ZSecurity.h"
#include "ZStencilLight.h"
#include "ZReplay.h"
#include "ZUtil.h"
#include "ZOptionInterface.h"
#include "HMAC_SHA1.h"

#ifdef USING_VERTEX_SHADER
#include "RShaderMgr.h"
#endif

//#include "mempool.h"
#include "RLenzFlare.h"
#include "ZLocale.h"
#include "MSysInfo.h"

#include "MTraceMemory.h"
#include "ZInput.h"
#include "Mint4Gunz.h"
#include "SecurityTest.h"
#include "CheckReturnCallStack.h"

#ifdef _DEBUG
//jintriple3 메모리 릭 vld
//#include "vld.h"
#endif

#ifdef _DEBUG
RMODEPARAMS	g_ModeParams={640,480,false,D3DFMT_R5G6B5};
//RMODEPARAMS	g_ModeParams={1024,768,false,RPIXELFORMAT_565};
#else
RMODEPARAMS	g_ModeParams={800,600,true,D3DFMT_R5G6B5};
#endif

#ifndef _DEBUG
#define SUPPORT_EXCEPTIONHANDLING
#endif


#ifdef LOCALE_NHNUSA
#include "ZNHN_USA.h"
#include "ZNHN_USA_Report.h"
#include "ZNHN_USA_Poll.h"
#endif

#ifdef _GAMEGUARD
#include "ZGameguard.h"
#endif

RRESULT RenderScene(void *pParam);

#define RD_STRING_LENGTH 512
char cstrReleaseDate[512];// = "ReleaseDate : 12/22/2003";

ZApplication	g_App;
MDrawContextR2* g_pDC = NULL;
MFontR2*		g_pDefFont = NULL;
ZDirectInput	g_DInput;
ZInput*			g_pInput = NULL;
Mint4Gunz		g_Mint;

HRESULT GetDirectXVersionViaDxDiag( DWORD* pdwDirectXVersionMajor, DWORD* pdwDirectXVersionMinor, TCHAR* pcDirectXVersionLetter );

void zexit(int returnCode)
{
	// 게임가드는 제대로 delete되어야 오류발생시 자체 로그를 올바르게 남길 수 있다.
	// 그냥 exit()해도 ZGameGuard를 싱글턴으로 만들었기 때문에 소멸자에서 게임가드가 delete되지만 어째서인지 그때 크래시가 일어난다.
	// exit()하기 전에 게임가드를 수동으로 해제하면 그런 문제가 일어나지 않는다.
	// 해킹 검출 등의 이유로 클라이언트 종료시 exit하지말고 zexit를 쓰자.
#ifdef _GAMEGUARD
	GetZGameguard().Release();
#endif
	exit(returnCode);
}

void CrcFailExitApp() { 
#ifdef _PUBLISH
	PostMessage(g_hWnd, WM_CLOSE, 0, 0); 
#else
	int* crash = NULL;
	*crash = 0;
#endif
}

void _ZChangeGameState(int nIndex)
{
	GunzState state = GunzState(nIndex);

	if (ZApplication::GetGameInterface())
	{
		ZApplication::GetGameInterface()->SetState(state);
	}
}

//list<HANDLE>	g_FontMemHandles;

RRESULT OnCreate(void *pParam)
{
	g_App.PreCheckArguments();

	string strFileLenzFlare("System/LenzFlare.xml");
// #ifndef _DEBUG
// 	strFileLenzFlare += ".mef";
// #endif
	RCreateLenzFlare(strFileLenzFlare.c_str());
	RGetLenzFlare()->Initialize();

	mlog("main : RGetLenzFlare()->Initialize() \n");

	RBspObject::CreateShadeMap("sfx/water_splash.bmp");
	//D3DCAPS9 caps;
	//RGetDevice()->GetDeviceCaps( &caps );
	//if( caps.VertexShaderVersion < D3DVS_VERSION(1, 1) )
	//{
	//	RGetShaderMgr()->mbUsingShader				= false;
	//	RGetShaderMgr()->shader_enabled				= false;
	//	mlog("main : VideoCard Dosen't support Vertex Shader...\n");
	//}
	//else
	//{
	//	mlog("main : VideoCard support Vertex Shader...\n");
	//}

	sprintf( cstrReleaseDate, "Version : %d", ZGetSVNRevision());
	mlog(cstrReleaseDate); mlog("\n");
	g_DInput.Create(g_hWnd, FALSE, FALSE);
	g_pInput = new ZInput(&g_DInput);
	/*
	for(int i=0; i<ZApplication::GetFileSystem()->GetFileCount(); i++){
		const char* szFileName = ZApplication::GetFileSystem()->GetFileName(i);
		size_t nStrLen = strlen(szFileName);
		if(nStrLen>3){
			if(stricmp(szFileName+nStrLen-3, "ttf")==0){
				int nFileLenth = ZApplication::GetFileSystem()->GetFileLength(i);
				char* pFileData = new char[nFileLenth];
				ZApplication::GetFileSystem()->ReadFile(szFileName, pFileData, nFileLenth);
				int nInstalled = 0;
				HANDLE hFontMem = AddFontMemResourceEx(pFileData, 1, 0, &nInstalled);
				g_FontMemHandles.insert(g_FontMemHandles.end(), hFontMem);
				delete[] pFileData;
			}
		}
	}
	*/
	RSetGammaRamp(Z_VIDEO_GAMMA_VALUE);
	RSetRenderFlags(RRENDER_CLEAR_BACKBUFFER);

	ZGetInitialLoading()->Initialize(  1, 0, 0, RGetScreenWidth(), RGetScreenHeight(), 0, 0, 1024, 768 );

	mlog("InitialLoading success.\n");

	struct _finddata_t c_file;
	intptr_t hFile;
	char szFileName[256];
#define FONT_DIR	"Font/"
#define FONT_EXT	"ttf"
	if( (hFile = _findfirst(FONT_DIR"*."FONT_EXT, &c_file )) != -1L ){
		do{
			strcpy(szFileName, FONT_DIR);
			strcat(szFileName, c_file.name);
			AddFontResource(szFileName);
		}while( _findnext( hFile, &c_file ) == 0 );
		_findclose(hFile);
	}

	g_pDefFont = new MFontR2;

	if( !g_pDefFont->Create("Default", Z_LOCALE_DEFAULT_FONT, DEFAULT_FONT_HEIGHT, 1.0f) )
//	if( !g_pDefFont->Create("Default", RGetDevice(), "FONTb11b", 9, 1.0f, true, false) )
//	if( !g_pDefFont->Create("Default", RGetDevice(), "FONTb11b", 14, 1.0f, true, false) )
	{
		mlog("Fail to Create defualt font : MFontR2 / main.cpp.. onCreate\n" );
		g_pDefFont->Destroy();
		SAFE_DELETE( g_pDefFont );
		g_pDefFont	= NULL;
	}
	//pDefFont->Create("Default", RGetDevice(), "FONTb11b", 10, 1.0f, true, false);
	//pDefFont->Create("Default", RGetDevice(), "FONTb11b", 16, 1.0f, true, false, -1, 4);
	//pDefFont->Create("Default", RGetDevice(), "-2002", 10, 1.0f, false, false, -1, 1);
	//pDefFont->Create("Default", RGetDevice(), "HY수평선L", 12, 1.0f, false, false, -1, 2);

	//MLoadDesignerMode();
	// 기본 800x600 디자인으로 생성하고, 나중에 Resize를 화면 크기로 해준다.

	g_pDC = new MDrawContextR2(RGetDevice());

#ifndef _FASTDEBUG
	if( ZGetInitialLoading()->IsUseEnable() )
	{
		if( ZGetLocale()->IsTeenMode() )
		{
			ZGetInitialLoading()->AddBitmap( 0, "Interface/Default/LOADING/loading_teen.jpg" );
		}
		else
		{
			ZGetInitialLoading()->AddBitmap( 0, "Interface/Default/LOADING/loading_adult.jpg" );
		}
		ZGetInitialLoading()->AddBitmapBar( "Interface/Default/LOADING/loading.bmp" );
		ZGetInitialLoading()->SetText( g_pDefFont, 10, 30, cstrReleaseDate );

		ZGetInitialLoading()->AddBitmapGrade( "Interface/Default/LOADING/loading_grade_fifteen.jpg" );

		ZGetInitialLoading()->SetPercentage( 0.0f );
		ZGetInitialLoading()->Draw( MODE_FADEIN, 0 , true );
	}
#endif

//	ZGetInitialLoading()->SetPercentage( 10.0f );
//	ZGetInitialLoading()->Draw( MODE_DEFAULT, 0 , true );

	g_Mint.Initialize(800, 600, g_pDC, g_pDefFont);
	Mint::GetInstance()->SetHWND(RealSpace2::g_hWnd);

	mlog("interface Initialize success\n");

//	ZGetConfiguration()->LoadHotKey(FILENAME_CONFIG);

	ZLoadingProgress appLoading("application");
	if(!g_App.OnCreate(&appLoading))
	{
		ZGetInitialLoading()->Release();
		return R_ERROR_LOADING;
	}

//	ZGetInitialLoading()->SetPercentage( 50.0f );
//	ZGetInitialLoading()->Draw( MODE_DEFAULT, 0, true );
	
	// mlog("main : g_App.OnCreate() \n");

	ZGetSoundEngine()->SetEffectVolume(Z_AUDIO_EFFECT_VOLUME);
	ZGetSoundEngine()->SetMusicVolume(Z_AUDIO_BGM_VOLUME);
	ZGetSoundEngine()->SetEffectMute(Z_AUDIO_EFFECT_MUTE);
	ZGetSoundEngine()->SetMusicMute(Z_AUDIO_BGM_MUTE);

	g_Mint.SetWorkspaceSize(g_ModeParams.nWidth, g_ModeParams.nHeight);
	g_Mint.GetMainFrame()->SetSize(g_ModeParams.nWidth, g_ModeParams.nHeight);
	ZGetOptionInterface()->Resize(g_ModeParams.nWidth, g_ModeParams.nHeight);

//	ZGetInitialLoading()->SetPercentage( 80.f );
//	ZGetInitialLoading()->Draw( MODE_DEFAULT, 0, true );
    
	// Default Key
	for(int i=0; i<ZACTION_COUNT; i++){
//		g_Mint.RegisterActionKey(i, ZGetConfiguration()->GetKeyboard()->ActionKeys[i].nScanCode);
		ZACTIONKEYDESCRIPTION& keyDesc = ZGetConfiguration()->GetKeyboard()->ActionKeys[i];
		g_pInput->RegisterActionKey(i, keyDesc.nVirtualKey);
		if(keyDesc.nVirtualKeyAlt!=-1)
			g_pInput->RegisterActionKey(i, keyDesc.nVirtualKeyAlt);
	}

	g_App.SetInitialState();

//	ParseParameter(g_szCmdLine);

	ZGetFlashBangEffect()->SetDrawCopyScreen(true);

	static const char *szDone = "Done.";
	ZGetInitialLoading()->SetLoadingStr(szDone);
	if( ZGetInitialLoading()->IsUseEnable() )
	{
#ifndef _FASTDEBUG
		ZGetInitialLoading()->SetPercentage( 100.f );
		ZGetInitialLoading()->Draw( MODE_FADEOUT, 0 ,true  );
#endif
		ZGetInitialLoading()->Release();
	}

	mlog("main : OnCreate() done\n");

	SetFocus(g_hWnd);

	return R_OK;
}


bool CheckDll(char* fileName, BYTE* SHA1_Value)
{
	BYTE digest[20];
	BYTE Key[GUNZ_HMAC_KEY_LENGTH];

	memset(Key, 0, 20);
	memcpy(Key, GUNZ_HMAC_KEY, strlen(GUNZ_HMAC_KEY));

	CHMAC_SHA1 HMAC_SHA1 ;
	HMAC_SHA1.HMAC_SHA1_file(fileName, Key, GUNZ_HMAC_KEY_LENGTH, digest) ;

	if(memcmp(digest, SHA1_Value, 20) ==0)
	{
		return true;
	}

	return false;
}



RRESULT OnDestroy(void *pParam)
{
	mlog("Destroy gunz\n");

	g_App.OnDestroy();

	SAFE_DELETE(g_pDefFont);

	g_Mint.Finalize();

	mlog("interface finalize.\n");

	SAFE_DELETE(g_pInput);
	g_DInput.Destroy();

	mlog("game input destroy.\n");

	RGetShaderMgr()->Release();

//	g_App.OnDestroy();

	// mlog("main : g_App.OnDestroy()\n");

	ZGetConfiguration()->Destroy();

	mlog("game gonfiguration destroy.\n");

	delete g_pDC;

	struct _finddata_t c_file;
	intptr_t hFile;
	char szFileName[256];
#define FONT_DIR	"Font/"
#define FONT_EXT	"ttf"
	if( (hFile = _findfirst(FONT_DIR"*."FONT_EXT, &c_file )) != -1L ){
		do{
			strcpy(szFileName, FONT_DIR);
			strcat(szFileName, c_file.name);
			RemoveFontResource(szFileName);
		}while( _findnext( hFile, &c_file ) == 0 );
		_findclose(hFile);
	}

	MFontManager::Destroy();
	MBitmapManager::Destroy();
	MBitmapManager::DestroyAniBitmap();

	mlog("Bitmap manager destroy Animation bitmap.\n");

	/*
	for(list<HANDLE>::iterator i=g_FontMemHandles.begin(); i!=g_FontMemHandles.end(); i++){
		RemoveFontMemResourceEx(*i);
	}
	*/

	//ReleaseMemPool(RealSoundEffectPlay);
	//UninitMemPool(RealSoundEffectPlay);

	//ReleaseMemPool(RealSoundEffect);
	//UninitMemPool(RealSoundEffect);

	//ReleaseMemPool(RealSoundEffectFx);
	//UninitMemPool(RealSoundEffectFx);

	//mlog("main : UninitMemPool(RealSoundEffectFx)\n");

	// 메모리풀 헤제
	ZBasicInfoItem::Release(); // 할당되어 있는 메모리 해제
//	ZHPInfoItem::Release();

	ZGetStencilLight()->Destroy();
	LightSource::Release();

//	ZStencilLight::GetInstance()->Destroy();

	RBspObject::DestroyShadeMap();
	RDestroyLenzFlare();
	RAnimationFileMgr::GetInstance()->Destroy();
	
	ZStringResManager::ResetInstance();

	mlog("destroy gunz finish.\n");

	return R_OK;
}

RRESULT OnUpdate(void* pParam)
{
	//_ASSERTE( _CrtCheckMemory( ) );

	__BP(100, "main::OnUpdate");

	g_pInput->Update();

	g_App.OnUpdate();

	const DWORD dwCurrUpdateTime = timeGetTime();

#ifndef _DEBUG

#ifdef _GAMEGUARD
	if( !GetZGameguard().CheckGameGuardRunning(dwCurrUpdateTime) ||
		!GetZGameguard().CheckD3dDllHooking(dwCurrUpdateTime) )
	{
		if( 0 != ZGetGameClient() ) 
			ZGetGameClient()->Disconnect();

		PostQuitMessage(0);
	}
#endif

#endif
	__EP(100);

	return R_OK;
}

RRESULT OnRender(void *pParam)
{
	__BP(101, "main::OnRender");
	if( !RIsActive() && RIsFullScreen() )
	{
		__EP(101);
		return R_NOTREADY;
	}

	g_App.OnDraw();


#ifdef _SMOOTHLOOP
	Sleep(10);
#endif

#ifndef _PUBLISH

	if(g_pDefFont) {
		static char __buffer[256];

		float fMs = 1000.f/g_fFPS;
		float fScore = 100-(fMs-(1000.f/60.f))*2;

		sprintf(__buffer, "FPS : %3.3f %.3f점 (%.3f ms)",g_fFPS,fScore,fMs);
		g_pDefFont->m_Font.DrawText( MGetWorkspaceWidth()-200,0,__buffer );
//		OutputDebugString(__buffer);
	}

#endif

	__EP(101);

	return R_OK;
}

RRESULT OnInvalidate(void *pParam)
{
	MBitmapR2::m_dwStateBlock=NULL;

	g_App.OnInvalidate();
	
	return R_OK;
}

RRESULT OnRestore(void *pParam)
{
	for(int i=0; i<MBitmapManager::GetCount(); i++){
		MBitmapR2* pBitmap = (MBitmapR2*)MBitmapManager::Get(i);
		pBitmap->OnLostDevice();
	}

	g_App.OnRestore();

	return R_OK;
}

RRESULT OnActivate(void *pParam)
{
	if (ZGetGameInterface() && ZGetGameClient() && Z_ETC_BOOST)
		ZGetGameClient()->PriorityBoost(true);
	return R_OK;
}

RRESULT OnDeActivate(void *pParam)
{
	if (ZGetGameInterface() && ZGetGameClient())
		ZGetGameClient()->PriorityBoost(false);
	return R_OK;
}

RRESULT OnError(void *pParam)
{
	mlog("RealSpace::OnError(%d) \n", RGetLastError());

	switch (RGetLastError())
	{
	case RERROR_INVALID_DEVICE:
		{
			D3DADAPTER_IDENTIFIER9 *ai=RGetAdapterID();
			char szLog[512];
			ZTransMsg( szLog, MSG_DONOTSUPPORT_GPCARD, 1, ai->Description);

			int ret=MessageBox(NULL, szLog, ZMsg( MSG_WARNING), MB_YESNO);
			if(ret!=IDYES)
				return R_UNKNOWN;
		}
		break;
	case RERROR_CANNOT_CREATE_D3D:
		{
			ShowCursor(TRUE);

			char szLog[512];
			sprintf(szLog, ZMsg( MSG_DIRECTX_NOT_INSTALL));

			int ret=MessageBox(NULL, szLog, ZMsg( MSG_WARNING), MB_YESNO);
			if(ret==IDYES)
			{
				ShellExecute(g_hWnd, "open", ZMsg( MSG_DIRECTX_DOWNLOAD_URL), NULL, NULL, SW_SHOWNORMAL); 
			}
		}
		break;

	};

	return R_OK;
}

void SetModeParams()
{
#ifdef _PUBLISH
	g_ModeParams.bFullScreen = true;
#else
	#ifdef _DEBUG
		g_ModeParams.bFullScreen = false;
	#else
		g_ModeParams.bFullScreen = ZGetConfiguration()->GetVideo()->bFullScreen;
	#endif
#endif

	g_ModeParams.nWidth = ZGetConfiguration()->GetVideo()->nWidth;
	g_ModeParams.nHeight = ZGetConfiguration()->GetVideo()->nHeight;
	ZGetConfiguration()->GetVideo()->nColorBits == 32 ? 
		g_ModeParams.PixelFormat = D3DFMT_X8R8G8B8 : g_ModeParams.PixelFormat = D3DFMT_R5G6B5 ;

}

void ResetAppResource()
{
	// (거의)모든 리소스를 제거하고 다시 로딩한다
	_ASSERTE( _CrtCheckMemory( ) );

	// save user id
#ifdef LOCALE_NHNUSA
	ZNHN_USAAuthInfo* pUSAAuthInfo = (ZNHN_USAAuthInfo*)ZGetLocale()->GetAuthInfo();
	string strUserID = pUSAAuthInfo->GetUserID();
#endif

	// Reset GameInterface except its gameclient object. the account connection must be alive.
	ZGetGameInterface()->m_sbRemainClientConnectionForResetApp = true;	// let GameInterface don't clear its gameclient object. (why this interface object has network object???)
	ZGetGameInterface()->GetGameClient()->Destroy();	// but must clear queued messages

	// whole client resource reload
	OnDestroy(0);

	ZGetConfiguration()->Destroy();
	ZGetConfiguration()->Load();

	SetModeParams();

	if( !ZApplication::GetInstance()->InitLocale() )
		MLog("In changing language... InitLocale error !!!\n");

	ZGetConfiguration()->Load_StringResDependent();
	OnCreate(0);
	RParticleSystem::Restore();
	OnRestore(0);
	ZGetGameInterface()->m_sbRemainClientConnectionForResetApp = false;

	ZPostRequestCharacterItemListForce(ZGetGameClient()->GetPlayerUID());

	ZGetGameInterface()->UpdateDuelTournamantMyCharInfoUI();
	ZGetGameInterface()->UpdateDuelTournamantMyCharInfoPreviousUI();

	// restore user id
#ifdef LOCALE_NHNUSA
	pUSAAuthInfo = (ZNHN_USAAuthInfo*)ZGetLocale()->GetAuthInfo();
	pUSAAuthInfo->SetUserID(strUserID);
#endif
}

// 느려도 관계없다~~ -.-

int FindStringPos(char* str,char* word)
{
	if(!str || str[0]==0)	return -1;
	if(!word || word[0]==0)	return -1;

	int str_len = (int)strlen(str);
	int word_len = (int)strlen(word);

	char c;
	bool bCheck = false;

	for(int i=0;i<str_len;i++) {
		c = str[i];
		if(c == word[0]) {

			bCheck = true;

			for(int j=1;j<word_len;j++) {
				if(str[i+j]!=word[j]) {
					bCheck = false;
					break;
				}
			}

			if(bCheck) {
				return i;
			}
		}
	}
	return -1;
}

bool FindCrashFunc(char* pName)
{
//	Function Name
//	File Name 
	if(!pName) return false;

	FILE *fp;
	fp = fopen( "mlog.txt", "r" );
	if(fp==NULL)  return false;

	fseek(fp,0,SEEK_END);
	int size = ftell(fp);
	fseek(fp,0,SEEK_SET);

	char* pBuffer = new char [size];

	fread(pBuffer,1,size,fp);

	fclose(fp);

	// 우리 쏘스에서 찾는다.
	int posSource = FindStringPos(pBuffer,"ublish");
	if(posSource==-1) return false;

	int posA = FindStringPos(pBuffer+posSource,"Function Name");
//	int posB = FindStringPos(pBuffer,"File Name");	
	// filename 이 없는 경우도 있어서 이렇게 바꿨다
	int posB = posA + FindStringPos(pBuffer+posSource+posA,"\n");

	if(posA==-1) return false;
	if(posB==-1) return false;

	posA += 16;

//	int memsize = posB-posA-6;
	int memsize = posB-posA;
	memcpy(pName,&pBuffer[posA+posSource],memsize);

	pName[memsize] = 0;

	delete [] pBuffer;

	for(int i=0;i<memsize;i++) {
		if(pName[i]==':') {
			pName[i] = '-';
		}
	}

	return true;
}

void HandleExceptionLog()
{
	#define ERROR_REPORT_FOLDER	"ReportError"

	extern char* logfilename;	// Instance on MDebug.cpp

	// ERROR_REPORT_FOLDER 존재하는지 검사하고, 없으면 생성
	WIN32_FIND_DATA FindFileData;
	HANDLE hFind;

	hFind = FindFirstFile(ERROR_REPORT_FOLDER, &FindFileData);
	if (hFind == INVALID_HANDLE_VALUE) {
		if (!CreateDirectory("ReportError", NULL)) {
			MessageBox(g_hWnd, "ReportError 폴더를 생성할 수 없습니다.", APPLICATION_NAME , MB_ICONERROR|MB_OK);
			return;
		}
	} else 	{
		FindClose(hFind);
	}


/* 2007년 2월 13일 BAReport 더이상 사용 못하게 막음


	// mlog.txt 를 ERROR_REPORT_FOLDER 로 복사

	//acesaga_0928_911_moanus_rslog.txt
	//USAGE_EX) BAReport app=acesaga;addr=moon.maiet.net;port=21;id=ftp;passwd=ftp@;gid=10;user=moanus;localfile=rslog.txt;remotefile=remote_rslog.txt;

//	if(ZGetCharacterManager()) {
//		ZGetCharacterManager()->OutputDebugString_CharacterState();
//	}


	ZGameClient* pClient = (ZGameClient*)ZGameClient::GetInstance();

	char* pszCharName = NULL;
	MUID uidChar;
	MMatchObjCache* pObj;
	char szPlayer[128];

	if( pClient ) {

		uidChar = pClient->GetPlayerUID();
		pObj = pClient->FindObjCache(uidChar);
		if (pObj)
			pszCharName = pObj->GetName();

		wsprintf(szPlayer, "%s(%d%d)", pszCharName?pszCharName:"Unknown", uidChar.High, uidChar.Low);
	}
	else { 
		wsprintf(szPlayer, "Unknown(-1.-1)");
	}


//	if (pClient) {

		time_t currtime;
		time(&currtime);
		struct tm* pTM = localtime(&currtime);

		char cFuncName[1024];

		if(FindCrashFunc(cFuncName)==false) {
			strcpy(cFuncName,"Unknown Error");
		}

		char szFileName[_MAX_DIR], szDumpFileName[_MAX_DIR];
		wsprintf(szFileName, "%s_%s_%.2d%.2d_%.2d%.2d_%s_%s", cFuncName,
				APPLICATION_NAME, pTM->tm_mon+1, pTM->tm_mday, pTM->tm_hour, pTM->tm_min, szPlayer, "mlog.txt");
		wsprintf(szDumpFileName, "%s.dmp", szFileName);

		char szFullFileName[_MAX_DIR], szDumpFullFileName[_MAX_DIR];
		wsprintf(szFullFileName, "%s/%s", ERROR_REPORT_FOLDER, szFileName);
		wsprintf(szDumpFullFileName, "%s/%s", ERROR_REPORT_FOLDER, szDumpFileName);

		if (CopyFile("mlog.txt", szFullFileName, TRUE))
		{
			CopyFile("Gunz.dmp", szDumpFullFileName, TRUE);

			// BAReport 실행
			char szCmd[4048];
			char szRemoteFileName[_MAX_DIR], szRemoteDumpFileName[_MAX_DIR];
			wsprintf(szRemoteFileName, "%s/%s/%s", ZGetConfiguration()->GetBAReportDir(), "gunzlog", szFileName);
			wsprintf(szRemoteDumpFileName, "%s/%s/%s", ZGetConfiguration()->GetBAReportDir(), "gunzlog", szDumpFileName);

			wsprintf(szCmd, "BAReport app=%s;addr=%s;port=21;id=ftp;passwd=ftp@;user=%s;localfile=%s,%s;remotefile=%s,%s", 
				APPLICATION_NAME, ZGetConfiguration()->GetBAReportAddr(), szPlayer, szFullFileName, szDumpFullFileName, szRemoteFileName, szRemoteDumpFileName);

			WinExec(szCmd, SW_SHOW);

			FILE *file = fopen("bareportpara.txt","w+");
			fprintf(file,szCmd);
			fclose(file);
		}
//	}
*/
}

long FAR PASCAL WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
#ifdef LOCALE_JAPAN
		case WM_COPYDATA:
			{
				ZBaseAuthInfo* pAuth = ZGetLocale()->GetAuthInfo();
				if( ((ZGameOnJPAuthInfo*)pAuth)->NewLogin(wParam, lParam) )
				{
					MessageBox(g_hWnd, "Same id accessing from a different PC", NULL, MB_OK);
					zexit(-1);
				}
			}
			break;
#endif

		case WM_SYSCHAR:
			if(ZIsLaunchDevelop() && wParam==VK_RETURN)
			{
#ifndef _PUBLISH
				RFrame_ToggleFullScreen();
#endif
				return 0;
			}
			break;

		case WM_CREATE:
			if (strlen(Z_LOCALE_HOMEPAGE_TITLE) > 0)
			{
				ShowIExplorer(false, Z_LOCALE_HOMEPAGE_TITLE);
			}
			break;
		case WM_DESTROY:
			if (strlen(Z_LOCALE_HOMEPAGE_TITLE) > 0)
			{
				ShowIExplorer(true, Z_LOCALE_HOMEPAGE_TITLE);
			}
			break;
		case WM_SETCURSOR:
			if(ZApplication::GetGameInterface())
				ZApplication::GetGameInterface()->OnResetCursor();
			return TRUE; // prevent Windows from setting cursor to window class cursor

		case WM_ENTERIDLE:
			// 모달 대화상자가 코드를 블럭하고 있을 때 부모에게 보내는 idle 통지메시지
			// (일본 IME에 모달 대화상자가 있어서 넣었음)
			// 모달 대화상자로 업데이트 루프를 블럭해서 무적 어뷰즈로 악용되기 때문에 여기서 업데이트를 실행한다
			RFrame_UpdateRender();
			break;

			/*
		case  WM_LBUTTONDOWN:
			SetCapture(hWnd);
			return TRUE;
		case WM_LBUTTONUP:
			ReleaseCapture();
			return TRUE;
			*/
		case WM_KEYDOWN:
			{
				bool b = false;
			}
	}

	/*if(Mint::GetInstance()->ProcessEvent(hWnd, message, wParam, lParam)==true)
	{
		if (ZGetGameInterface() && ZGetGameInterface()->IsReservedResetApp())	// for language changing
		{
			ZGetGameInterface()->ReserveResetApp(false);
			ResetAppResource();
		}

		return 0;
	}*/

	// thread safe하기위해 넣음
	if (message == WM_CHANGE_GAMESTATE)
	{
		_ZChangeGameState(wParam);
	}


	return DefWindowProc(hWnd, message, wParam, lParam);
}

/*
class mtrl {
public:

};

class node {
public:
	int		m_nIndex[5];
};


class _map{
public:
	mtrl* GetMtrl(node* node,int index) { return GetMtrl(node->m_nIndex[index]); }
	mtrl* GetMtrl(int id) { return m_pIndex[id]; }

	mtrl*	m_pIndex[5];
};

class game {
public:
	_map m_map;	
};

game _game;
game* g_game;
*/


void ClearTrashFiles()
{
}

bool CheckFileList()
{
	MZFileSystem *pfs=ZApplication::GetFileSystem();
	MZFile mzf;

	string strFileNameFillist(FILENAME_FILELIST);
// #ifndef _DEBUG
// 	strFileNameFillist += ".mef";
// #endif

	if(!mzf.Open(strFileNameFillist.c_str() ,pfs))
		return false;

	char *buffer;
	buffer=new char[mzf.GetLength()+1];
	mzf.Read(buffer,mzf.GetLength());
	buffer[mzf.GetLength()]=0;

	MXmlDocument aXml;
	aXml.Create();
	if(!aXml.LoadFromMemory(buffer))
	{
		delete buffer;
		return false;
	}

	delete buffer;

	int iCount, i;
	MXmlElement		aParent, aChild;
	aParent = aXml.GetDocumentElement();
	iCount = aParent.GetChildNodeCount();

	char szTagName[256];
	for (i = 0; i < iCount; i++)
	{
		aChild = aParent.GetChildNode(i);
		aChild.GetTagName(szTagName);
		if(stricmp(szTagName,"FILE")==0)
		{
			char szContents[256],szCrc32[256];
			aChild.GetAttribute(szContents,"NAME");
			aChild.GetAttribute(szCrc32,"CRC32");

			if(stricmp(szContents,"config.xml")!=0)
			{
				unsigned int crc32_list = pfs->GetCRC32(szContents);
				unsigned int crc32_current;
				sscanf(szCrc32,"%x",&crc32_current);

#ifndef _DEBUG
				if(crc32_current!=crc32_list)
				{
					// mlog("crc error , file %s ( current = %x, original = %x ).\n",szContents,crc32_current,crc32_list);

					// 모든 파일을 검사는 한다
					return false; 
				}
#endif
			}
		}
	}

	return true;
}


enum RBASE_FONT{
	RBASE_FONT_GULIM = 0,
	RBASE_FONT_BATANG = 1,

	RBASE_FONT_END
};

static int g_base_font[RBASE_FONT_END];
static char g_UserDefineFont[256];

bool _GetFileFontName(char* pUserDefineFont)
{
	if(pUserDefineFont==NULL) return false;

	FILE* fp = fopen("_Font", "rt");
	if (fp) {
		fgets(pUserDefineFont,256, fp);
		fclose(fp);
		return true;
	}
	return false;
}


bool CheckFont()
{
	char FontPath[MAX_PATH];
	char FontNames[MAX_PATH+100];

	::GetWindowsDirectory(FontPath, MAX_PATH);

	strcpy(FontNames,FontPath);
	strcat(FontNames, "\\Fonts\\gulim.ttc");

	if (_access(FontNames,0) != -1)	{ g_base_font[RBASE_FONT_GULIM] = 1; }
	else							{ g_base_font[RBASE_FONT_GULIM] = 0; }

	strcpy(FontNames,FontPath);
	strcat(FontNames, "\\Fonts\\batang.ttc");

	if (_access(FontNames,0) != -1)	{ g_base_font[RBASE_FONT_BATANG] = 1; }
	else							{ g_base_font[RBASE_FONT_BATANG] = 0; }

	//	strcpy(FontNames,FontPath);
	//	strcat(FontNames, "\\Fonts\\System.ttc");
	//	if (_access(FontNames,0) != -1)	{ g_font[RBASE_FONT_BATANG] = 1; }
	//	else							{ g_font[RBASE_FONT_BATANG] = 0; }

	if(g_base_font[RBASE_FONT_GULIM]==0 && g_base_font[RBASE_FONT_BATANG]==0) {//둘다없으면..

		if( _access("_Font",0) != -1) { // 이미 기록되어 있다면..
			_GetFileFontName( g_UserDefineFont );
		}
		else {

			int hr = IDOK;

			//hr = ::MessageBox(NULL,"귀하의 컴퓨터에는 건즈가 사용하는 (굴림,돋움) 폰트가 없는 것 같습니다.\n 다른 폰트를 선택 하시겠습니까?","알림",MB_OKCANCEL);
			//hr = ::MessageBox(NULL,"귀하의 컴퓨터에는 건즈가 사용하는 (굴림,돋움) 폰트가 없는 것 같습니다.\n 계속 진행 하시겠습니까?","알림",MB_OKCANCEL);

			if(hr==IDOK) {
				/*			
				CFontDialog dlg;
				if(dlg.DoModal()==IDOK) {
				CString facename = dlg.GetFaceName();
				lstrcpy((LPSTR)g_UserDefineFont,(LPSTR)facename.operator const char*());

				hr = ::MessageBox(NULL,"선택하신 폰트를 저장 하시겠습니까?","알림",MB_OKCANCEL);

				if(hr==IDOK)
				_SetFileFontName(g_UserDefineFont);
				}
				*/
				return true;
			}
			else {
				return false;
			}
		}
	}
	return true;
}

#include "shlobj.h"

void CheckFileAssociation()
{
#define GUNZ_REPLAY_CLASS_NAME	"GunzReplay"

	// 체크해봐서 등록이 안되어있으면 등록한다. 사용자에게 물어볼수도 있겠다.
	char szValue[256];
	if(!MRegistry::Read(HKEY_CLASSES_ROOT,"."GUNZ_REC_FILE_EXT,NULL,szValue))
	{
		MRegistry::Write(HKEY_CLASSES_ROOT,"."GUNZ_REC_FILE_EXT,NULL,GUNZ_REPLAY_CLASS_NAME);

		char szModuleFileName[_MAX_PATH] = {0,};
		GetModuleFileName(NULL, szModuleFileName, _MAX_DIR);

		char szCommand[_MAX_PATH];
		sprintf(szCommand,"\"%s\" \"%%1\"",szModuleFileName);

		MRegistry::Write(HKEY_CLASSES_ROOT,GUNZ_REPLAY_CLASS_NAME"\\shell\\open\\command",NULL,szCommand);

		SHChangeNotify(SHCNE_ASSOCCHANGED, SHCNF_FLUSH, NULL, NULL);
	}
}

// 해당 텍스트 파일에 해당 글귀가 있으면 XTRAP 테스트 코드가 수행된다. (뒷구멍) //
bool CheckXTrapIsGetTestCode()														// add sgk 0621
{
	char szBuf[256] = "";
	FILE* fp = fopen("XTrapTest.txt", "rt");
	if (fp)
	{
		fgets(szBuf, 256, fp);
		mlog("XTrapTest.txt : \n");
		mlog(szBuf);
		mlog("\n");
		fclose(fp);

		if (stricmp(szBuf, "RUN_XTRAP_TEST_CODE") == 0)
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	else
	{
		mlog("fail to open XTrapTest.txt\n");
		return false;
	}
}

void OnGetXTrapRealCodeArgv(char* szTemp, bool* bPatchSkip)							// add sgk 0621
{
	/* LOCALE_KOREA */
	wsprintf(szTemp, "660970B478F9CB6395356D98440FE8629F86DDCD4DBB54B513680296C770DFCDB63DCCFE8DFE34B3A6B2BAA8CFB5C8C9761A2D3F1E491F9EFF6757489912B1AB0F7D04245246E409A061005FC9527B0823FF73E0B9F4C91EB67C40AC1A16EABB8FE52BDE25C76F6955E9E52A0812CE");

#ifdef LOCALE_JAPAN
	memset(szTemp, 0, 256);
	wsprintf(szTemp, "660970B45869CA6395356D98440FE8624C8FEA6EF181FD7D095D6CBA9911AFB0B5661B972C3C82BB0FF2D47A32DFB56D407CB146190E29B1EA46F49C1E86160F0F7D04245246E409A061005FC9527B086EF578A8BCFCC91FB67C51F65E05AAB85F7E306086BDFF03DF1BA46A66C605FFBC6263206088B68D6930514A");
#endif

#ifdef LOCALE_US
	memset(szTemp, 0, 256);
	wsprintf(szTemp, "660970B497F9CB6395356D98440FE8629AE854BDDBD13EDCE69AC1898F7A23CEF138AD2BF2758B368950133F1B021D0D218BFB058146B32450591F8B22CBE6A2");
	*bPatchSkip = true;
#endif

#ifdef LOCALE_INDIA
	memset(szTemp, 0, 256);
//	wsprintf(szTemp, "660970B47C69CB6795356D98440FE8625582AC40166A109C00E4D6A6056D18A02BBAC0A19DA6BEE6B4D43AD07CFB61697FD7FF586D98ECFF1DA11222DD82028D0F7D04245246E417A4610E569557620395165EECCBF7CD9008C4C0120CA7A0AD9D568C0DC8C7BD38C629B7EAAE5435B46105721F036F7C5BF0");
	wsprintf(szTemp, "660970B47C69CB6795356D98490FE862FEBC286C65D77538F80891D97D18B65B43E538B6EADB14290A04CF119B162DE7AA91984B54023E368FB4C25D4A91F68A0F7D04245246E417A4610E569557620395165EECCBF7CD9008C4C0120CA7A0AD9D568C0DC8C7BD38C629B7EAAE5435B46105721F036F7C5BF0");
#endif

#ifdef LOCALE_BRAZIL
	memset(szTemp, 0, 256);
	wsprintf(szTemp, "660970B448FBCB6395356D98440FE8621A6EADB8532B3C5F1949386F921C6C0970FEF0A168B5352668BE414ADF1375136173F493A8A2C075AC0F919AC7241A650F7D04245246E401B574195DD31E6305975703051B9F4F5CA2A8046A5FF3331AB0C8F040AFA98BB5CE3134520AC79D1328E836DF645FC479");
#endif
}

void OnGetXTrapTestCodeArgv(char* szTemp, bool* bPatchSkip)							// add sgk 0621
{
	/* LOCALE_KOREA */
	wsprintf(szTemp, "660970B478F9CB6395356D98440FE8629F86DDCD4DBB54B513680296C770DFCDB63DCCFE8DFE34B3A6B2BAA8CFB5C8C9761A2D3F1E491F9EFF6757489912B1AB0F7D04245246E409A061005FC9527B0823FF73E0B9F4C91EB67C40AC1A16EABB8FE52BDE25C76F6955E9E52A0812A88323D4");

#ifdef LOCALE_JAPAN
	memset(szTemp, 0, 256);
	wsprintf(szTemp, "660970B45869CA6395356D98440FE8624C8FEA6EF181FD7D095D6CBA9911AFB0B5661B972C3C82BB0FF2D47A32DFB56D407CB146190E29B1EA46F49C1E86160F0F7D04245246E409A061005FC9527B086EF578A8BCFCC91FB67C51F65E05AAB85F7E306086BDFF03DF1BA46A66C605FFBC6263206088B68D6930512C295649");
#endif

#ifdef LOCALE_US
	memset(szTemp, 0, 256);
	wsprintf(szTemp, "660970B497F9CB6395356D98440FE8629AE854BDDBD13EDCE69AC1898F7A23CEF138AD2BF2758B368950133F1B021D0D218BFB058146B32450591F8B22CBE6A2");
	*bPatchSkip = true;
#endif

#ifdef LOCALE_INDIA
	memset(szTemp, 0, 256);
//	wsprintf(szTemp, "660970B47C69CB6795356D98440FE8625582AC40166A109C00E4D6A6056D18A02BBAC0A19DA6BEE6B4D43AD07CFB61697FD7FF586D98ECFF1DA11222DD82028D0F7D04245246E417A4610E569557620395165EECCBF7CD9008C4C0120CA7A0AD9D568C0DC8C7BD38C629B7EAAE5435B46105721F036F7C5B962980B7");
	wsprintf(szTemp, "660970B47C69CB6795356D98490FE862FEBC286C65D77538F80891D97D18B65B43E538B6EADB14290A04CF119B162DE7AA91984B54023E368FB4C25D4A91F68A0F7D04245246E417A4610E569557620395165EECCBF7CD9008C4C0120CA7A0AD9D568C0DC8C7BD38C629B7EAAE5435B46105721F036F7C5B962980B7");
#endif

#ifdef LOCALE_BRAZIL
	memset(szTemp, 0, 256);
	wsprintf(szTemp, "660970B448FBCB6395356D98440FE8621A6EADB8532B3C5F1949386F921C6C0970FEF0A168B5352668BE414ADF1375136173F493A8A2C075AC0F919AC7241A650F7D04245246E401B574195DD31E6305975703051B9F4F5CA2A8046A5FF3331AB0C8F040AFA98BB5CE3134520AC79D1328E836DF645FC41F2B9A7E");
#endif
}

void UpgradeMrsFile()
{
	char temp_path[ 1024];
	sprintf( temp_path,"*");

	FFileList file_list;
	GetFindFileListWin(temp_path,".mrs",file_list);
	file_list.UpgradeMrs();
}

HANDLE Mutex = 0;

#ifdef _HSHIELD
int __stdcall AhnHS_Callback(long lCode, long lParamSize, void* pParam);
#endif

DWORD g_dwMainThreadID;


//------------------------------------------- nhn usa -------------------------------------------------------------
bool InitReport()
{
#ifdef LOCALE_NHNUSA
	mlog( "Init report start\n" );
	if( !GetNHNUSAReport().InitReport(((ZNHN_USAAuthInfo*)(ZGetLocale()->GetAuthInfo()))->GetUserID().c_str(),
		((ZNHN_USAAuthInfo*)(ZGetLocale()->GetAuthInfo()))->GetGameStr()) )
	{
		mlog( "Init nhn report fail.\n" );
		return false;
	}
	GetNHNUSAReport().ReportStartGame();
	mlog( "Init report success.\n" );
#endif

	return true;
}

bool InitPoll()
{
#ifdef LOCALE_NHNUSA
	mlog( "Init poll start\n" );

	((ZNHN_USAAuthInfo*)(ZGetLocale()->GetAuthInfo()))->ZUpdateGameString();

	if( !GetNHNUSAPoll().ZHanPollInitGameString( ((ZNHN_USAAuthInfo*)(ZGetLocale()->GetAuthInfo()))->GetGameStr()) )
		return false;
#endif

	return true;
}


bool CheckGameGuardHackToolUser()
{
#ifdef _GAMEGUARD

	string strUserID;
	ZBaseAuthInfo* pAuth = ZGetLocale()->GetAuthInfo();

	if ( pAuth == NULL)
		return true;


#ifdef LOCALE_NHNUSA

#ifdef _DEBUG_NHN_USA_AUTH
	return true;
#endif

	strUserID = ((ZNHN_USAAuthInfo*)pAuth)->GetUserID();

#elif LOCALE_JAPAN

//	strUserID = ((ZGameOnJPAuthInfo*)pAuth)->GetUserID();
	return true;

#endif


	if( !GetZGameguard().CheckHackToolUser( strUserID.c_str()) )
		return false;


#endif	// _GAMEGUARD

	return true;
}


//------------------------------------------- nhn usa end----------------------------------------------------------

int PASCAL WinMain(HINSTANCE this_inst, HINSTANCE prev_inst, LPSTR cmdline, int cmdshow)
{
	_CrtSetDbgFlag ( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );

	//_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_CHECK_CRT_DF | _CRTDBG_LEAK_CHECK_DF | _CRTDBG_DELAY_FREE_MEM_DF);

	InitLog(MLOGSTYLE_DEBUGSTRING|MLOGSTYLE_FILE);

	g_fpOnCrcFail = CrcFailExitApp;

#ifdef LOCALE_JAPAN
	ZGameOnJPAuthInfo::m_hLauncher = ::FindWindow( NULL, TITLE_PUBLAGENT );
#endif

	g_dwMainThreadID = GetCurrentThreadId();
	
#ifdef _MTRACEMEMORY
	MInitTraceMemory();
#endif

	//_CrtSetBreakAlloc(994464);

	// Current Directory를 맞춘다.
	char szModuleFileName[_MAX_DIR] = {0,};
	GetModuleFileName(NULL, szModuleFileName, _MAX_DIR);
	PathRemoveFileSpec(szModuleFileName);
	SetCurrentDirectory(szModuleFileName);

#ifndef _GAMEGUARD
	#ifdef _PUBLISH
		// 중복 실행 금지
		Mutex = CreateMutex(NULL, TRUE, "Gunz");
		if (GetLastError() == ERROR_ALREADY_EXISTS)
		{
			zexit(-1);
			return 0;
		}
	#endif
#endif

#ifdef _HSHIELD

	TCHAR szFullFileName[_MAX_DIR];
	GetCurrentDirectory( sizeof( szFullFileName), szFullFileName);
	strcat( szFullFileName, "\\HShield\\EhSvc.dll");

	int nRet = 0;

#ifdef _DEBUG
#define AHNHS_CHKOPT_GUNZ		AHNHS_CHKOPT_SPEEDHACK
#else
#define AHNHS_CHKOPT_GUNZ		AHNHS_CHKOPT_ALL
#endif

#ifdef  LOCALE_BRAZIL					/* Brazil */
	nRet = _AhnHS_Initialize(szFullFileName, AhnHS_Callback, 4001, "DA0EF49C0F6D029F", AHNHS_CHKOPT_GUNZ
		| AHNHS_ALLOW_SVCHOST_OPENPROCESS | AHNHS_ALLOW_LSASS_OPENPROCESS | AHNHS_ALLOW_CSRSS_OPENPROCESS | AHNHS_DONOT_TERMINATE_PROCESS
		, AHNHS_SPEEDHACK_SENSING_RATIO_NORMAL);

#elif  LOCALE_INDIA						/* India */
	nRet = _AhnHS_Initialize(szFullFileName, AhnHS_Callback, 4003, "CC4686AE10F63E4A", AHNHS_CHKOPT_GUNZ
		| AHNHS_ALLOW_SVCHOST_OPENPROCESS | AHNHS_ALLOW_LSASS_OPENPROCESS | AHNHS_ALLOW_CSRSS_OPENPROCESS | AHNHS_DONOT_TERMINATE_PROCESS
		, AHNHS_SPEEDHACK_SENSING_RATIO_NORMAL);
#endif

#ifndef _DEBUG
	// 아래 코드는 릴리즈시 _AhnHS_Initialize 옵션이 AHNHS_CHKOPT_ALL로 되어있지 않을 경우 
	// 컴파일 타임에 에러를 일으켜서 알 수 있게 한다.
	void* OptionCheckTool[(AHNHS_CHKOPT_GUNZ == AHNHS_CHKOPT_ALL)?1:0];
#endif

	

	//아래 에러는 개발과정에서만 발생할 수 있으면 
	//이후 결코 발생해서는 안되는 에러이므로 assert처리를 했습니다.
	assert(nRet != HS_ERR_INVALID_PARAM);
	assert(nRet != HS_ERR_INVALID_LICENSE);
	assert(nRet != HS_ERR_ALREADY_INITIALIZED);

	TCHAR szTitle[256] = "Hack Shield Error";

	if (nRet != HS_ERR_OK) 
	{
		//Error 처리 
		switch(nRet)
		{
		case HS_ERR_ANOTHER_SERVICE_RUNNING:
			{
//				MessageBox(NULL, _T("다른 게임이 실행중입니다.\n프로그램을 종료합니다."), szTitle, MB_OK);
				mlog( "다른 게임이 실행중입니다. 프로그램을 종료합니다.\n");
				break;
			}
		case HS_ERR_INVALID_FILES:
			{
//				MessageBox(NULL, _T("잘못된 파일 설치되었습니다.\n프로그램을 재설치하시기 바랍니다."), szTitle, MB_OK);
				mlog( "잘못된 파일 설치되었습니다. 프로그램을 재설치하시기 바랍니다.\n");
				break;
			}
		case HS_ERR_DEBUGGER_DETECT:
			{
//				MessageBox(NULL, _T("컴퓨터에서 디버거 실행이 감지되었습니다.\n디버거의 실행을 중지시킨 뒤에 다시 실행시켜주시기바랍니다."), szTitle, MB_OK);
				mlog( "컴퓨터에서 디버거 실행이 감지되었습니다. 디버거의 실행을 중지시킨 뒤에 다시 실행시켜주시기바랍니다.\n");
				break;
			}
		case HS_ERR_NEED_ADMIN_RIGHTS:
			{
//				MessageBox(NULL, _T("Admin 권한으로 실행되어야 합니다.\n프로그램을 종료합니다."), szTitle, MB_OK);
				mlog( "Admin 권한으로 실행되어야 합니다. 프로그램을 종료합니다.\n");
				break;
			}
		case HS_ERR_COMPATIBILITY_MODE_RUNNING:
			{
//				MessageBox(NULL, _T("호환성 모드로 프로그램이 실행중입니다.\n프로그램을 종료합니다."), szTitle, MB_OK);
				mlog( "호환성 모드로 프로그램이 실행중입니다. 프로그램을 종료합니다.\n");
				break;				
			}
		default:
			{
				TCHAR szMsg[255];
				wsprintf(szMsg, _T("해킹방지 기능에 문제가 발생하였습니다.(Error Code = %x)\n프로그램을 종료합니다."), nRet);
//				MessageBox(NULL, szMsg, szTitle, MB_OK);
				mlog( szMsg);
				break;
			}
		}

		return FALSE;
	}

	//시작 함수 호출 
	nRet = _AhnHS_StartService();

	assert(nRet != HS_ERR_NOT_INITIALIZED);
	assert(nRet != HS_ERR_ALREADY_SERVICE_RUNNING);

	if (nRet != HS_ERR_OK)
	{
//		TCHAR szMsg[255];
		mlog(_T("해킹방지 기능에 문제가 발생하였습니다.(Error Code = %x)\n프로그램을 종료합니다."), nRet);
//		MessageBox(NULL, szMsg, szTitle, MB_OK);

		return FALSE;
	}

	nRet = _AhnHS_SaveFuncAddress(6, _AhnHS_Initialize, _AhnHS_StartService,			// 핵실드 함수
										ZCheckHackProcess, ZGetMZFileChecksum,				// 핵판별 자체함수
										ZSetupDataChecker_Global, ZSetupDataChecker_Game);
	if(nRet != ERROR_SUCCESS)
	{
//		AfxMessageBox(_T("_AhnHS_SaveFuncAddress Failed!"));
		return FALSE;
	}
#endif // _HSHIELD


#ifdef _XTRAP														// update sgk 0702 start
	mlog("XTRAP Start\n");

	char szTemp[256] = {0,};
	bool bPatchSkip = false;

	bool bIsXtrapTest = CheckXTrapIsGetTestCode();
	if (bIsXtrapTest)
	{
		// XTRAP 테스트용 파일경로로 실행된다. //
		OnGetXTrapTestCodeArgv(szTemp, &bPatchSkip);
	}
	else
	{
		// XTRAP 리얼 서버용 파일경로로 실행된다. //
		OnGetXTrapRealCodeArgv(szTemp, &bPatchSkip);
	}

	if (!bPatchSkip)
	{
		XTrap_L_Patch(szTemp, szModuleFileName, 60);
	}

	XTrap_C_Start(szTemp, NULL);
	XTrap_C_KeepAlive();

	mlog("XTRAP End\n");
#endif																// update sgk 0702 end

	ClearTrashFiles();

	srand( (unsigned)time( NULL ));

#if defined(LOCALE_BRAZIL) || defined(LOCALE_INDIA) || defined(LOCALE_US) || defined(LOCALE_KOREA)
	//#ifndef _DEBUG
	#ifdef _PUBLISH
		// GunzLock을 띄워놓고 Gunz.exe를 실행하면 종료직전 대기한다. (XProtector 프로세스이미지스캔 작업용)
		HANDLE hMutexGunzLock = CreateMutex(NULL, TRUE, "GunzLock");
		if (GetLastError() == ERROR_ALREADY_EXISTS)
		{
			WaitForSingleObject(hMutexGunzLock, INFINITE);
			CloseHandle(hMutexGunzLock);
		}
	#endif
#endif	// LOCALE_NHNUSA

	// 로그 시작
	mlog("GUNZ " STRFILEVER " launched. build ("__DATE__" "__TIME__") \n");
	char szDateRun[128]="";
	char szTimeRun[128]="";
	_strdate( szDateRun );
	_strtime( szTimeRun );
	mlog("Log time (%s %s)\n", szDateRun, szTimeRun);

#ifndef _PUBLISH
	mlog("cmdline = %s\n",cmdline);

#endif

#ifndef _LAUNCHER
	UpgradeMrsFile();// mrs1 이라면 mrs2로 업그래이드 한다..
#endif

	MSysInfoLog();

//	if (CheckVideoAdapterSupported() == false)
//		return 0;

	CheckFileAssociation();

	// Initialize MZFileSystem - MUpdate 
	MRegistry::szApplicationName=APPLICATION_NAME;

	g_App.InitFileSystem();
//	mlog("CheckSum: %u \n", ZApplication::GetFileSystem()->GetTotalCRC());

	//if(!InitializeMessage(ZApplication::GetFileSystem())) {
	//	MLog("Check Messages.xml\n");
	//	return 0;
	//}

//	넷마블 버전은 구분해야함... 넷마블 버전은 MZIPREADFLAG_MRS1 도 읽어야함...

#ifdef _PUBLISH
//	#ifndef NETMARBLE_VERSION
		MZFile::SetReadMode( MZIPREADFLAG_MRS2 );
//	#endif
#endif


#ifdef LOCALE_NHNUSA
	// NHNUSA는 커맨드라인으로 언어선택을 알려준다, 다른 지역빌드라도 디파인 필요없음
	ZGetLanguageSetting_forNHNUSA()->SetLanguageIndexFromCmdLineStr(cmdline);
#endif

	// config와 string 로딩
	ZGetConfiguration()->Load();

	ZStringResManager::MakeInstance();
	if( !ZApplication::GetInstance()->InitLocale() )
	{
		MLog("Locale Init error !!!\n");
		return false;
	}

	ZGetConfiguration()->Load_StringResDependent();

	// 여기서 메크로 컨버팅... 먼가 구리구리~~ -by SungE.
	if( !ZGetConfiguration()->LateStringConvert() )
	{
		MLog( "main.cpp - Late string convert fale.\n" );
		return false;
	}

	DWORD ver_major = 0;
	DWORD ver_minor = 0;
	TCHAR ver_letter = ' ';

	// 의미없음 ... 외부에서 dll 이 없다고 먼저뜸...

/*_
	bool DXCheck = false;

	if( SUCCEEDED( GetDirectXVersionViaDxDiag( &ver_major, &ver_minor, &ver_letter ) ) ) {
		if(ver_major >= 8)
			DXCheck = true;
	} // 성공 못한 경우 알수없으므로 실패~

	if(DXCheck==false) {
		::MessageBox(NULL,"DirectX 8.0 이상을 설치하고 다시 실행해 주시기 바랍니다.","알림",MB_OK);
	}
*/

#ifdef SUPPORT_EXCEPTIONHANDLING
	char szDumpFileName[256];
	sprintf(szDumpFileName, "Gunz.dmp");
	__try{
#endif

	if (ZApplication::GetInstance()->ParseArguments(cmdline) == false)
	{
		// Korean or Japan Version
		if ((ZGetLocale()->GetCountry() == MC_KOREA) || (ZGetLocale()->GetCountry() == MC_JAPAN))
		{
			mlog("Routed to Website \n");

			ShellExecute(NULL, "open", ZGetConfiguration()->GetLocale()->szHomepageUrl, NULL, NULL, SW_SHOWNORMAL);

			char szMsgWarning[128]="";
			char szMsgCertFail[128]="";
			ZTransMsg(szMsgWarning,MSG_WARNING);
			ZTransMsg(szMsgCertFail,MSG_REROUTE_TO_WEBSITE);
//			MessageBox(g_hWnd, szMsgCertFail, szMsgWarning, MB_OK);

			mlog(szMsgWarning);
			mlog(" : ");
			mlog(szMsgCertFail);

			return 0;
		}
		else
		{
			return 0;
		}
	}

//#ifdef _PUBLISH
	// if(!CheckFileList()) {
		// 종료하는것은 일단 보류
		// int ret=MessageBox(NULL, "파일이 손상되었습니다.", "중요!", MB_OK);
		// return 0;
	//}
//#endif


#ifdef LOCALE_NHNUSA
	BYTE SHA_HanAuthForClient[20] = {0x29,0xc0,0x7e,0x6b,0x8d,0x1d,0x30,0xd2,0xed,0xac,0xaf,0xea,0x78,0x16,0x51,0xf0,0x50,0x52,0x26,0x91};
	BYTE SHA_hanpollforclient[20] = {0x09,0x04,0x51,0x9d,0x95,0xbb,0x66,0x2a,0xfb,0x93,0x87,0x2d,0x21,0xa2,0x93,0x1d,0x6a,0xcb,0xa5,0x4f};
	BYTE SHA_HanReportForClient[20] = {0x4c,0x62,0xaf,0x4d,0x5b,0x54,0xb8,0x96,0x46,0x66,0x8f,0x1e,0x12,0xe7,0xf2,0xd7,0xe4,0x58,0x65,0xc9}; 
	if(!CheckDll("hanauthforclient.dll", SHA_HanAuthForClient) || 
	   !CheckDll("hanpollforclient.dll", SHA_hanpollforclient) ||
	   !CheckDll("hanreportforclient.dll", SHA_HanReportForClient) )
	{
		MessageBox(g_hWnd,"Dll Hacking detected",  NULL, MB_OK);
		return false;
	}

	if( !InitReport() ) 
		return 0;


	if ( !InitPoll())
		return 0;
#endif



#ifndef _DEBUG
	BYTE SHA_fmod[20] = {0x88,0x8f,0x1f,0x7b,0x7e,0x3c,0x43,0x38,0x4f,0x4b,0x80,0xb5,0x77,0xfe,0x09,0x1a,0xc0,0x45,0x38,0x3c};
	BYTE SHA_dbghelp[20] = {0x8d,0x12,0xc4,0x3a,0x84,0x12,0xc2,0x1,0x58,0xb4,0x46,0x70,0x9,0x5,0xcb,0xd4,0xfa,0xb1,0xe2,0x4b}; 
	if(!CheckDll("fmod.dll", SHA_fmod) ||
		!CheckDll("dbghelp.dll", SHA_dbghelp) )
	{
		MessageBox(g_hWnd,"Dll Hacking detected",  NULL, MB_OK);
		return false;
	}

#endif


#ifndef LOCALE_NHNUSA
	if (ZCheckHackProcess() == true)
	{
//		MessageBox(NULL,
//			ZMsg(MSG_HACKING_DETECTED), ZMsg( MSG_WARNING), MB_OK);
		mlog(ZMsg(MSG_HACKING_DETECTED));
		mlog("\n");
		mlog("I'm hacker.\n" );
		return 0;
	}
#endif

	if(!InitializeNotify(ZApplication::GetFileSystem())) {
		MLog("Check notify.xml\n");
		return 0;
	}
	else 
	{
		mlog( "InitializeNotify ok.\n" );
	}

	// font 있는가 검사..

	if(CheckFont()==false) {
		MLog("폰트가 없는 유저가 폰트 선택을 취소\n");
		return 0;
	}

	RSetFunction(RF_CREATE	,	OnCreate);
	RSetFunction(RF_RENDER	,	OnRender);
	RSetFunction(RF_UPDATE	,	OnUpdate);
	RSetFunction(RF_DESTROY ,	OnDestroy);
	RSetFunction(RF_INVALIDATE,	OnInvalidate);
	RSetFunction(RF_RESTORE,	OnRestore);
	RSetFunction(RF_ACTIVATE,	OnActivate);
	RSetFunction(RF_DEACTIVATE,	OnDeActivate);
	RSetFunction(RF_ERROR,		OnError);

	SetModeParams();

//	while(ShowCursor(FALSE)>0);

	const int nRMainReturn = RMain(APPLICATION_NAME,this_inst,prev_inst,cmdline,cmdshow,&g_ModeParams,WndProc,IDI_ICON1);
	if( 0 != nRMainReturn )
		return nRMainReturn;


#ifdef _GAMEGUARD
	mlog("start gameguard\n");

	ZGMAEGUARD_MODE mode = ZGGM_END;
	char szArg[ 64] = "";
	ZBaseAuthInfo* pAuth = ZGetLocale()->GetAuthInfo();
	

#ifdef LOCALE_NHNUSA
	if ( ((ZNHN_USAAuthInfo*)pAuth)->IsReal())
	{
		mode = ZGGM_REAL;
		strcpy( szArg, "GunzUS");
	}
	else if ( ((ZNHN_USAAuthInfo*)pAuth)->IsAlpha())
	{
		mode = ZGGM_ALPHA;
		strcpy( szArg, "GunzUSTest");
	}
	else
	{
		mlog( "error in gameguard mode...\n" );
		zexit( -1);
		return 0;
	}

#elif LOCALE_JAPAN
	if ( ((ZGameOnJPAuthInfo*)pAuth)->IsReal())
	{
		mode = ZGGM_REAL;
		strcpy( szArg, "GUNZWEI");
	}
	else if ( ((ZGameOnJPAuthInfo*)pAuth)->IsAlpha())
	{
		mode = ZGGM_ALPHA;
		strcpy( szArg, "GUNZWEITest");
	}
	else
	{
		mlog( "error in gameguard mode...\n" );
		zexit( -1);
		return 0;
	}

	if( !((ZGameOnJPAuthInfo*)pAuth)->SendMsgToLauncher(GET_MSG_HWND_TERMINATE_PUBGAME) )
	{
		mlog("Can't find GameOn Agent");
	}
#endif	// LOCALE_JAPAN


	if( !GetZGameguard().Init( mode, szArg) )
	{
		mlog( "error init gameguard...\n" );
		zexit( -1 );
		return 0;
	}


	GetZGameguard().SetMainWnd( g_hWnd );

	if( !CheckGameGuardHackToolUser() )
		return 0;

#endif	// _GAMEGUARD


#ifdef _GAMEGUARD
	#ifdef _PUBLISH
		// 중복 실행 금지
		Mutex = CreateMutex(NULL, TRUE, "Gunz");
		if (GetLastError() == ERROR_ALREADY_EXISTS)
		{
			zexit(-1);
			return 0;
		}
	#endif
#endif



#ifdef LOCALE_NHNUSA
	GetNHNUSAReport().ReportInitGameGuard();
#endif

	if( 0 != RInitD3D(&g_ModeParams) )
	{
		MessageBox(g_hWnd, "fail to initialize DirectX", NULL, MB_OK);
		mlog( "error init RInitD3D\n" );
		return 0;
	}

	const int nRRunReturn = RRun();

	//종료시 게임가드가 Xfire의 메모리쓰기 에러를 유발하는데 이때 오류창이 풀스크린 뒤에 뜨는 것 방지하기 위해
	//종료전에 건즈를 최소화/비활성화 시켜놓는다. xfire의 즉각적인 문제 해결을 기대하기 어려우므로 이렇게 처리
	ShowWindow(g_hWnd, SW_MINIMIZE);

#ifdef _GAMEGUARD
	GetZGameguard().Release();
#endif

#ifdef _MTRACEMEMORY
	MShutdownTraceMemory();
#endif

#ifdef _HSHIELD
	_AhnHS_StopService();
	_AhnHS_Uninitialize();		
#endif


#ifdef LOCALE_NHNUSA
	mlog( "Poll Process\n" );
	int nRetPoll = GetNHNUSAPoll().ZHanPollProcess( NULL);
#endif

	ZStringResManager::FreeInstance();

	return nRRunReturn;

//	ShowCursor(TRUE);

#ifdef SUPPORT_EXCEPTIONHANDLING
	}

	//__except(MFilterException(GetExceptionInformation())){
	__except(CrashExceptionDump(GetExceptionInformation(), szDumpFileName, true))
	{
#ifdef LOCALE_NHNUSA
		GetNHNUSAReport().ReportCrashedGame();
#endif

		HandleExceptionLog();
//		MessageBox(g_hWnd, "예상치 못한 오류가 발생했습니다.", APPLICATION_NAME , MB_ICONERROR|MB_OK);
	}
#endif

#ifdef _PUBLISH
	if (Mutex != 0) CloseHandle(Mutex);
#endif

//	CoUninitialize();

	return 0;
}

#ifdef _HSHIELD
int __stdcall AhnHS_Callback(long lCode, long lParamSize, void* pParam)
{
//	TCHAR szTitle[256];

	switch(lCode)
	{
		//Engine Callback
	case AHNHS_ENGINE_DETECT_GAME_HACK:
		{
			TCHAR szMsg[255];
			wsprintf(szMsg, _T("다음 위치에서 해킹툴이 발견되어 프로그램을 종료시켰습니다.\n%s"), (char*)pParam);
//			MessageBox(NULL, szMsg, szTitle, MB_OK);
			mlog(szMsg);
			PostThreadMessage(g_dwMainThreadID, WM_QUIT, 0, 0);
			break;
		}

		//일부 API가 이미 후킹되어 있는 상태
		//그러나 실제로는 이를 차단하고 있기 때문에 다른 후킹시도 프로그램은 동작하지 않습니다.
		//이 Callback은 단지 경고 내지는 정보제공 차원에서 제공되므로 게임을 종료할 필요가 없습니다.
	case AHNHS_ACTAPC_DETECT_ALREADYHOOKED:
		{
			PACTAPCPARAM_DETECT_HOOKFUNCTION pHookFunction = (PACTAPCPARAM_DETECT_HOOKFUNCTION)pParam;
			TCHAR szMsg[255];
			wsprintf(szMsg, _T("[HACKSHIELD] Already Hooked\n- szFunctionName : %s\n- szModuleName : %s\n"), 
				pHookFunction->szFunctionName, pHookFunction->szModuleName);
			OutputDebugString(szMsg);
			break;
		}

		//Speed 관련
	case AHNHS_ACTAPC_DETECT_SPEEDHACK:
	case AHNHS_ACTAPC_DETECT_SPEEDHACK_APP:
		{
//			MessageBox(NULL, _T("현재 이 PC에서 SpeedHack으로 의심되는 동작이 감지되었습니다."), szTitle, MB_OK);
			mlog("현재 이 PC에서 SpeedHack으로 의심되는 동작이 감지되었습니다.");
			PostThreadMessage(g_dwMainThreadID, WM_QUIT, 0, 0);
			break;
		}

		//디버깅 방지 
	case AHNHS_ACTAPC_DETECT_KDTRACE:	
	case AHNHS_ACTAPC_DETECT_KDTRACE_CHANGED:
		{
			TCHAR szMsg[255];
			wsprintf(szMsg, _T("프로그램에 대하여 디버깅 시도가 발생하였습니다. (Code = %x)\n프로그램을 종료합니다."), lCode);
//			MessageBox(NULL, szMsg, szTitle, MB_OK);
			mlog(szMsg);
			PostThreadMessage(g_dwMainThreadID, WM_QUIT, 0, 0);
			break;
		}

		//그외 해킹 방지 기능 이상 
	case AHNHS_ACTAPC_DETECT_AUTOMOUSE:
	case AHNHS_ACTAPC_DETECT_DRIVERFAILED:
	case AHNHS_ACTAPC_DETECT_HOOKFUNCTION:
	case AHNHS_ACTAPC_DETECT_MESSAGEHOOK:
	case AHNHS_ACTAPC_DETECT_MODULE_CHANGE:
		{
			TCHAR szMsg[255];
			wsprintf(szMsg, _T("해킹 방어 기능에 이상이 발생하였습니다. (Code = %x)\n프로그램을 종료합니다."), lCode);
//			MessageBox(NULL, szMsg, szTitle, MB_OK);
			mlog(szMsg);
			PostThreadMessage(g_dwMainThreadID, WM_QUIT, 0, 0);
			break;
		}
	}

	return 1;
}
#endif