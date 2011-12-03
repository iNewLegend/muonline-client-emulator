#ifndef _ZSOUNDENGINE_H
#define _ZSOUNDENGINE_H

class ZLoadingProgress;

#ifdef _BIRDSOUND

#include "RealSound2.h"
#include "MZFileSystem.h"

#include <map>
#include <string>
using namespace std;

struct MMatchItemDesc;
struct FSOUND_SAMPLE;

#pragma comment(lib, "dxguid.lib")

class ZSoundEngine : public RealSound2
{
private:
protected:
	virtual void OnCreate();
	virtual void OnUpdate();
	bool LoadResource( char* pFileName);
	void GetSoundName(const char* szSrcSoundName, char* out);
public:
	ZSoundEngine() { }
	virtual ~ZSoundEngine() { }

	// ���� ���� �Լ�
	bool OpenMusic(int nBgmIndex);
	int PlaySoundCharacter(const char* szSoundName, rvector& pos, bool bHero, int nPriority=128);
	//int PlaySound(const char* szSoundName, rvector& pos, int nPriority=128, DWORD dwDelay=0);
	void PlaySound(char* Name,rvector& pos,bool bHero=false, bool bLoop = false, DWORD dwDelay = 0 );

	virtual int PlaySound(const char* szSoundName, int nPriority=128);
	virtual int PlaySound(const char* szSoundName, float* pos, float* vel, int nPriority=128);


	void SetEffectVolume(float fVolume);
	void SetEffectMute(bool bMute);



	// �����Լ�
	int PlaySE( FSOUND_SAMPLE* pFS, rvector& pos, int Priority , bool bPlayer = false, bool bLoop = false ) { }
	//void PlaySound(char* Name,rvector& pos,bool bHero=false, bool bLoop = false, DWORD dwDelay = 0 ) { }
	void PlaySoundElseDefault(char* Name,char* NameDefault,rvector& pos,bool bHero=false, bool bLoop = false, DWORD dwDelay = 0 ) { }
	//void PlaySound(char* Name, bool bLoop=false, DWORD dwDelay = 0 ) { }

	void PlaySEFire(MMatchItemDesc *pDesc, float x, float y, float z, bool bHero=false);
	void PlaySEDryFire(MMatchItemDesc *pDesc, float x, float y, float z, bool bHero=false);
	void PlaySEReload(MMatchItemDesc *pDesc, float x, float y, float z, bool bHero=false);

	// �Ѿ� ƨ��� �Ҹ�
	void PlaySERicochet(float x, float y, float z);
	void PlaySEHitObject( float x, float y, float z, RBSPPICKINFO& info_ );

	// �Ѿ��� ���� �´� �Ҹ�
	void PlaySEHitBody(float x, float y, float z);
	bool Get3DSoundUpdate() const { return false; }
};


////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////
#else

#include "MZFileSystem.h"
#include "ZActorSound.h"
#include "MQuestNPC.h"

#include <map>
#include <string>
using namespace std;

#pragma comment(lib, "dxguid.lib")

struct MMatchItemDesc;
struct FSOUND_SAMPLE;

struct SoundSource
{
	FSOUND_SAMPLE* pFS;
	float fMaxDistance; // to Culling
	unsigned long int	nLastPlayedTime;
	SoundSource()
	{
		pFS = 0;
		fMaxDistance = 1000000000.0f; // 2d
		nLastPlayedTime = 0;
	}
};

struct DelaySound
{
	SoundSource* pSS;
	DWORD dwDelay;
	rvector pos;
	int priority;
	bool bPlayer;
	DelaySound()
	{
		pSS = 0;
		dwDelay = 0;
		priority = 0;
		bPlayer = false;
	}
};

struct AmbSound
{
	int type;
	SoundSource* pSS;
	rvector pos[2];		//�簢��(min, max)
	float radius;
	rvector center;
	int iChannel;
	float dx, dy, dz;
	char szSoundName[64];	// ���� �̸�.. reset�Ҷ� �ʿ���
	AmbSound()
	{
		type = 0;
		pSS = NULL;
		iChannel = -1;
		dx = dy = dz = 0;
		szSoundName[0] = 0;
	}
};

typedef map<string, SoundSource*> SESMAP;
typedef list<DelaySound> DSLIST;
typedef list<AmbSound> ASLIST;

class ZSoundEngine
{
private:
	char					m_SoundFileName[256];	// for Reload
	ZActorSoundManager		m_ASManager;			// NPC�� ���� ���� Ŭ����
protected:
	char*			m_pMusicBuffer;
	char			m_szOpenedMusicName[256];

	SESMAP			m_SoundEffectSource;
	SESMAP			m_SoundEffectSource2D;
	DSLIST			m_DelaySoundList; // ���� ���� ����Ʈ
	ASLIST			m_AmbientSoundList; // ȯ�� ���� ����Ʈ

	float			m_fEffectVolume;
	float			m_fMusicVolume;

	bool			m_bEffectMute;
	bool			m_bMusicMute;
	bool			m_bSoundEnable;
	bool			m_b3DSound;
	bool			m_b3DSoundUpdate;
	rvector			m_ListenerPos;
	bool			m_bInverse;
	bool			m_b8Bits;
	bool			m_bHWMixing;

	DWORD	m_Time;
	DWORD	m_DelayTime;

	bool			m_bEffectVolControl;	
	float			m_fEffectVolFactor;
	float			m_fEffectVolEnd;
	
	bool			m_bBGMVolControl;
	float			m_fBGMVolFactor;
	float			m_fBGMVolEnd;

	MZFileSystem*	m_pfs;
	bool			m_bBattleMusic;
	const char*		GetBGMFileName(int nBgmIndex);
	bool OpenMusic(const char* szFileName, MZFileSystem* pfs);
	bool CheckCulling(char* szName, SoundSource* pSS, const rvector& vSoundPos, bool bHero, int* pnoutPriority=NULL);
	static void MusicEndCallback(void* pCallbackContext);
public:
	ZSoundEngine();
	virtual ~ZSoundEngine();
	bool Create(HWND hwnd, bool bHWMixing = false, ZLoadingProgress *pLoading = NULL);
	bool Reset(HWND hwnd, bool bHWMixing );
	void Destroy();
	bool LoadResource( char* pFileName_ ,ZLoadingProgress *pLoading = NULL );
	bool Reload();

	// BGM
	bool OpenMusic(int nBgmIndex, MZFileSystem* pfs);
	void CloseMusic();
	void PlayMusic(bool bLoop = true);
	void StopMusic();
	void SetMusicVolume(float fVolume);
	float GetMusicVolume( void);
	void SetMusicMute(bool b);

	/// Sound Effect Source
	void SetEffectVolume(float fVolume);
	void SetEffectVolume( int iChannel, float fVolume );
	void StopLoopSound();
	void StopSound( int iChannel );
	void SetEffectMute(bool b);
	bool SetSamplingBits( bool b8bit );
	void SetInverseSound( bool bInverse )	{ m_bInverse = bInverse; }

	// Game-Specified Sound Effect
	// �� �Լ� �迭�� ����� ��� max distance culling, Priority ������ �ȵ�
	//int PlaySE( FSOUND_SAMPLE* pFS, rvector& pos, bool bPlayer = false, bool bLoop = false );
	//int PlaySE( FSOUND_SAMPLE* pFS, bool bLoop = false );
	int PlaySE( FSOUND_SAMPLE* pFS, const rvector& pos, int Priority , bool bPlayer = false, bool bLoop = false );

	/**
		WEAPON SOUND
	**/
	// melee ���� �Ҹ�
	void PlaySoundBladeConcrete(MMatchItemDesc *pDesc, rvector pos);				// �� �ܴ� �Ҹ� 
	void PlaySoundBladeDamage(MMatchItemDesc *pDesc, rvector& pos);					// �ǰ� �Ҹ�
	void PlaySoundHangOnWall(MMatchItemDesc *pDesc, rvector& pos);					// ���� Į �Ŵ¼Ҹ�
	void PlaySoundChargeComplete(MMatchItemDesc *pDesc, const rvector& pos);				// �������(����) �Ϸ�Ǵ� �Ҹ�
	void PlaySoundSmash(MMatchItemDesc *pDesc, rvector& pos, bool bObserverTarget);	// ������(������) ���ݼҸ�
	void PlaySoundSheath(MMatchItemDesc *pDesc, const rvector& pos, bool bObserverTarget);// ���� ������ �Ҹ�

	void PlaySEFire(MMatchItemDesc *pDesc, float x, float y, float z, bool bPlayer=false);
	void PlaySEDryFire(MMatchItemDesc *pDesc, float x, float y, float z, bool bPlayer=false);
	void PlaySEReload(MMatchItemDesc *pDesc, float x, float y, float z, bool bPlayer=false);

	// �Ѿ� ƨ��� �Ҹ�
	void PlaySERicochet(float x, float y, float z);
	void PlaySEHitObject( float x, float y, float z, RBSPPICKINFO& info_ );

	// �Ѿ��� ���� �´� �Ҹ�
	void PlaySEHitBody(float x, float y, float z);

	int PlaySound(char* Name,const rvector& pos,bool bHero=false, bool bLoop = false, DWORD dwDelay = 0 );
	void PlaySoundElseDefault(char* Name,char* NameDefault,rvector& pos,bool bHero=false, bool bLoop = false, DWORD dwDelay = 0 );
	int PlaySound(char* Name, bool bLoop=false, DWORD dwDelay = 0 ); // only 2D sound
	
	bool isPlayAble(char* name);
	bool isPlayAbleMtrl(char* name);//Ư��������~

	void Run(void);
	void UpdateAmbSound(rvector& Pos, rvector& Ori);
	float GetArea( rvector& Pos, AmbSound& a );

	int GetEnumDeviceCount();
	const char* GetDeviceDescription( int index );

	FSOUND_SAMPLE* GetFS( const char* szName, bool bHero = false );
	SoundSource* GetSoundSource( const char* szName, bool bHero );

	/*
	 *	for Sound Debugging
	 */
	void SetFramePerSecond( int n ) { m_DelayTime = 1000 / n; }
	void Set3DSoundUpdate(bool b);
	bool Get3DSoundUpdate() const { return m_b3DSoundUpdate; }

	// AmbientSound
	void SetAmbientSoundBox( char* Name, rvector& pos1, rvector& pos2, bool b2d = true );
	void SetAmbientSoundSphere( char* Name, rvector& pos, float radius, bool b2d = true );
	void ClearAmbientSound();

	// Special Functions
	void PlayVoiceSound(char* szName);
    
	// �����ð��� ���� ���� ������������ �� �������� ������ �ִ� �Լ�
	void SetVolumeControlwithDuration( float fStartPercent, float fEndPercent, DWORD dwDuration, bool bEffect, bool bBGM );

	// NPC Sound ���� �Լ�
	bool LoadNPCResource(MQUEST_NPC nNPC, ZLoadingProgress* pLoading = NULL);
	void ReleaseNPCResources();
	void PlayNPCSound(MQUEST_NPC nNPC, MQUEST_NPC_SOUND nSound, rvector& pos, bool bMyKill=true);
};

#endif	// _BIRDSOUND

#define BGMID_INTRO				0
#define BGMID_LOBBY				1
#define BGMID_BATTLE			2
#define BGMID_FIN				12

#define MAX_BGM	13




#define VOICE_COOL					"nar/NAR01"		// 0.7  �߰� �Ϸ�
#define VOICE_NICE					"nar/NAR02"		// 1.0  �߰� �Ϸ�
#define VOICE_GREAT					"nar/NAR03"		// 0.8  �߰� �Ϸ�
#define VOICE_WONDERFUL				"nar/NAR04"		// 1.2  �߰� �Ϸ�
#define VOICE_KILLEDALL				"nar/NAR05"		// 2.0  �߰� �Ϸ�
#define VOICE_HEADSHOT				"nar/NAR06"		// 0.7  �߰� �Ϸ�
#define VOICE_FANTASTIC				"nar/NAR07"		// 1.5  �߰� �Ϸ�
#define VOICE_EXCELLENT				"nar/NAR08"		// 1.0  �߰� �Ϸ�
#define VOICE_UNBELIEVABLE			"nar/NAR09"		// 1.3  �߰� �Ϸ�
#define VOICE_GET_READY				"nar/NAR10"		// 1.3  �߰� �Ϸ�
#define VOICE_LETS_ROCK				"nar/NAR11"		// 1.1  �߰� �Ϸ�
#define VOICE_FINAL_ROUND			"nar/NAR27"		// 1.2  �߰� �Ϸ�
#define VOICE_YOU_WON				"nar/NAR12"		// 1.0  �߰� �Ϸ�
#define VOICE_YOU_LOSE				"nar/NAR13"		// 1.3  �߰� �Ϸ�
#define VOICE_RED_TEAM_WON			"nar/NAR14"		// 1.4  �߰� �Ϸ�
#define VOICE_BLUE_TEAM_WON			"nar/NAR15"		// 1.4  �߰� �Ϸ�
#define VOICE_DRAW_GAME				"nar/NAR16"		// 1.2  �߰� �Ϸ�
#define VOICE_REDTEAM_BOSS_DOWN		"nar/NAR19"		// 2.0  �߰� �Ϸ�
#define VOICE_BLUETEAM_BOSS_DOWN	"nar/NAR20"		// 2.1  �߰� �Ϸ�
#define VOICE_PLAYER_NOT_READY		"nar/NAR26"		// 1.5  �߰� �Ϸ�
#define VOICE_BERSERKER_DOWN		"nar/NAR28"		// 1.3  �߰� �Ϸ�
#define VOICE_GOT_BERSERKER			"nar/NAR29"		// 1.6  �߰� �Ϸ�
#define VOICE_QUEST_START_FAIL		"nar/NAR22"		// 2.8  �߰� �Ϸ�
#define VOICE_FIGHT					""
#define VOICE_NEW_INTRUDER			"nar/NAR17"		// 1.3
#define VOICE_NEW_CHALLENGER		"nar/NAR18"		// 1.6
#define VOICE_FIRST_KILL			""
#define VOICE_DOUBLE_KILL			""
#define VOICE_MULTI_KILL			""
#define VOICE_GOGOGO				""
#define VOICE_GO_LEFT				""
#define VOICE_GO_RIGHT				""
#define VOICE_GO_BACK				""
#define VOICE_FOLLOW_ME				""
#define VOICE_BACK_ME_UP			""
#define VOICE_COVER_ME				""
#define VOICE_ENEMY_IN_SIGHT		""
#define VOICE_THANK_YOU				""
#define VOICE_SORRY					""
#define VOICE_HAHAHA				""
#define VOICE_OOPS					""
#define VOICE_UH_OH					""
#define VOICE_YES					""
#define VOICE_NO					""
#define VOICE_HEAL_ME_PLEASE		""
#define VOICE_MEDIC					""
#define VOICE_HOLD					""
#define VOICE_DEFEND				""
#define VOICE_NEGATIVE				""
#define VOICE_AFFIRMATIVE			""

#define VOICE_MAX					45






















#endif