#ifndef _ZAPPLICATION_H
#define _ZAPPLICATION_H

#include "MZFileSystem.h"
//#include "ZGameInterface.h"
#include "ZSoundEngine.h"
#include "ZDirectInput.h"
#include "MDataChecker.h"
#include "ZLanguageConf.h"
#include "ZTimer.h"
#include "ZEmblemInterface.h"
#include "ZStageInterface.h"
#include "ZSkill.h"
#include "ZWorldManager.h"

#include "../RealSpace2/Include/RAniEventInfo.h"

//class MMessenger;
class ZGame;
class ZGameClient;
class ZGameInterface;
class ZLoadingProgress;
class ZProfiler;
class ZOptionInterface;

enum GunzState{
	GUNZ_NA = 0,
	GUNZ_GAME = 1,
	GUNZ_LOGIN = 2,
	GUNZ_DIRECTLOGIN = 3,
	GUNZ_LOBBY = 4,
	GUNZ_STAGE = 5,
	GUNZ_GREETER = 6,
	GUNZ_CHARSELECTION = 7,
	GUNZ_CHARCREATION = 8,
	GUNZ_PREVIOUS = 10,
	GUNZ_SHUTDOWN = 11,
	GUNZ_BIRDTEST
};

class ZApplication 
{
public:
	enum ZLAUNCH_MODE {
		ZLAUNCH_MODE_DEBUG,
		ZLAUNCH_MODE_NETMARBLE,
		ZLAUNCH_MODE_GAMEON,
		ZLAUNCH_MODE_STANDALONE,
		ZLAUNCH_MODE_STANDALONE_DEVELOP,		// ������ ���		// �� ���������
		ZLAUNCH_MODE_STANDALONE_REPLAY,
		ZLAUNCH_MODE_STANDALONE_GAME,
		ZLAUNCH_MODE_STANDALONE_DUMMY,
		ZLAUNCH_MODE_STANDALONE_AI,
		ZLAUNCH_MODE_STANDALONE_QUEST,
		ZLAUNCH_MODE_NHNUSA,
	};

private:
	ZGameInterface*			m_pGameInterface;
	GunzState				m_nInitialState;			///< �ʱ� ����
	ZStageInterface*		m_pStageInterface;
	ZOptionInterface*		m_pOptionInterface;
	ZLAUNCH_MODE			m_nLaunchMode;
    char					m_szFileName[_MAX_PATH];	// ���÷��� Ȥ�� standalone ���̸�
	char					m_szCmdLine[ 4096 ];			// �ʱ� Ŀ�ǵ���� ����
	UINT					m_nTimerRes;
	MDataChecker			m_GlobalDataChecker;
	bool					m_bLaunchDevelop;			///< �����ڹ��� �ɼ�
	bool					m_bLaunchTest;				///< �׽�Ʈ���� �ɼ�

	void ParseStandAloneArguments(const char* pszArgs);
protected:
	static ZApplication*	m_pInstance;	///< Singleton Instance
	static MZFileSystem		m_FileSystem;
	static ZSoundEngine		m_SoundEngine;
	static RMeshMgr			m_NPCMeshMgr;
	static RMeshMgr			m_MeshMgr;
	static RMeshMgr			m_WeaponMeshMgr;
	static ZTimer			m_Timer;
	static ZEmblemInterface	m_EmblemInterface;
	static ZSkillManager	m_SkillManager;				///< ��ų �Ŵ���

	//�ִϸ��̼� �̺�Ʈ �޴���...
	static RAniEventMgr		m_AniEventMgr;

	ZWorldManager			m_WorldManager;
	MZFileCheckList			m_fileCheckList;
#ifdef _ZPROFILER
	ZProfiler				*m_pProfiler;
#endif

public:
	ZApplication();
	~ZApplication();

	const char* GetCmdLine() { return m_szCmdLine; }
	bool ParseArguments(const char* pszArgs);
	void SetInterface();
	void SetNextInterface();
	void CheckSound();
	void SetInitialState();
	bool GetSystemValue(const char* szField, char* szData);		///< ������Ʈ���� ���� �о��
	void SetSystemValue(const char* szField, const char* szData);		///< ������Ʈ���� ����

	void InitFileSystem();

	bool OnCreate(ZLoadingProgress *pLoadingProgress);
	void OnDestroy();
	bool OnDraw();
	void OnUpdate();
	void OnInvalidate();
	void OnRestore();

	static void ResetTimer();	// ù update �� �θ��� ���� �ҷ��ش�.
	static void Exit();
	/// Singleton Instance
	__forceinline static ZApplication*		GetInstance(void);
	/// Singleton Current Interface
	__forceinline static ZGameInterface*	GetGameInterface(void);
	__forceinline static ZStageInterface*	GetStageInterface(void);
	__forceinline static ZOptionInterface*	GetOptionInterface(void);
	/// Singleton Zip File System
	__forceinline static MZFileSystem*		GetFileSystem(void);
	/// Singleton ZGameClient
	__forceinline static ZTimer*			GetTimer(void);
	static ZSoundEngine*		GetSoundEngine(void);
	static RMeshMgr*			GetNpcMeshMgr()			{ return &m_NPCMeshMgr;}
	static RMeshMgr*			GetMeshMgr()			{ return &m_MeshMgr; }
	static RMeshMgr*			GetWeaponMeshMgr()		{ return &m_WeaponMeshMgr; }
	static ZEmblemInterface*	GetEmblemInterface()	{ return &m_EmblemInterface; }
	static ZSkillManager*		GetSkillManager()		{ return &m_SkillManager; }

	//�ִϸ��̼� �̺�Ʈ �޴��� ������ �κ�
	static RAniEventMgr*		GetAniEventMgr()		{ return &m_AniEventMgr;}

	__forceinline ZWorldManager* GetWorldManager()		{ return &m_WorldManager; }

	__forceinline ZLAUNCH_MODE GetLaunchMode()			{ return m_nLaunchMode; }
	__forceinline void SetLaunchMode(ZLAUNCH_MODE nMode)	{ m_nLaunchMode = nMode; }
	__forceinline bool IsLaunchDevelop()					{ return m_bLaunchDevelop; }
	__forceinline bool IsLaunchTest()					{ return m_bLaunchTest; }

	__forceinline unsigned int GetFileListCRC()			{ return m_fileCheckList.GetCRC32(); }

	bool InitLocale();
	void PreCheckArguments();
};

__forceinline ZApplication* ZApplication::GetInstance(void)
{
	return m_pInstance;
}

__forceinline ZGameInterface* ZApplication::GetGameInterface(void)
{
	ZApplication* pApp = GetInstance();
	if(pApp==NULL) return NULL;
	return pApp->m_pGameInterface;		// ���Ʋ⣬Ŀǰ�ӿ�ZGameInterface����ȫ�����Ǳ����
}
__forceinline ZStageInterface* ZApplication::GetStageInterface(void)
{
	ZApplication* pApp = GetInstance();
	if(pApp==NULL) return NULL;
	return pApp->m_pStageInterface;
}
__forceinline ZOptionInterface* ZApplication::GetOptionInterface(void)
{
	ZApplication* pApp = GetInstance();
	if(pApp==NULL) return NULL;
	return pApp->m_pOptionInterface;
}
__forceinline MZFileSystem* ZApplication::GetFileSystem(void)
{
	return &m_FileSystem;
}
__forceinline ZTimer* ZApplication::GetTimer(void)
{
	return &m_Timer;
}

#endif