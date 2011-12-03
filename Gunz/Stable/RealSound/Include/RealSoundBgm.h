#ifndef REALSOUNDBGM_H
#define REALSOUNDBGM_H

/**
	Background Music Class
	������ Streaming�Ͽ� �����Ѵ�.
*/

#include "RealSound.h"
#include "RealSoundWaveFile.h"
#include "MMTimer.h"

class RealSoundBgm
{
protected:
    void Cue (void);
    BOOL WriteWaveData (UINT cbSize);
    BOOL WriteSilence (UINT cbSize);
    DWORD GetMaxWriteSize (void);
    BOOL ServiceBuffer (void);
    static BOOL TimerCallback (DWORD dwUser);

    RealSound* m_pRealSound;
    LPDIRECTSOUNDBUFFER m_pdsb;
    RSMWaveFile* m_pwavefile;
    MMTimer * m_ptimer;

	///////////////////////////////////////
	// Semaphore
	///////////////////////////////////////
    bool m_fCued;					// semaphore (stream cued)
    bool m_fPlaying;				// semaphore (stream playing)
    LONG m_lInService;				// reentrancy semaphore

	DSBUFFERDESC m_dsbd;			// sound buffer description
	    
    UINT m_cbBufOffset;				// last write position
    UINT m_nBufLength;				// length of sound buffer in msec
    UINT m_cbBufSize;				// size of sound buffer in bytes
    UINT m_nBufService;				// service interval in msec
    UINT m_nDuration;				// duration of wave file
    UINT m_nTimeStarted;			// time (in system time) playback started
    UINT m_nTimeElapsed;			// elapsed time in msec since playback started

	BOOL m_bLooped;					// Loop control

public:
    RealSoundBgm (void);
    virtual ~RealSoundBgm (void);

	// BGM��ü�� ����
	BOOL Create (LPSTR pszFilename, RealSound* pRealSound);
    void Destroy();

	// ������ BGM��ü�� ���� �Ѵ�.
    void Play(BOOL bLoop);

	// ���ָ� ������Ű�� ���� ��ġ�� ó������ ������.
    void Stop(void);
	//void Pause();

	bool IsPlaying(void);
	void SetVolume(float t);
};


#endif