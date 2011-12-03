/*
	RealSoundBgm.cpp
	-------------

	Backgound Music Waveform Object Handling
	
	WaGe�� 4���� Ʈ���� �����ϸ�, ���� �Ѱ��� �� ���̰� �� ���̺�
	ȭ���� ���� �Ҵ��Ѵ�. 
	�̴� ���̰� �� ���� ���, �Ǵ� ���� ȭ���� ���� ����ϰ� �Ǹ�
	�� ������ 22kHz, 16bit Stereo Sampling�� �������� �Ѵ�.
	BGM Object�� Pannig���� ȿ���� �ټ� ������ ���� ������ ���� ��
	���� �� �� �ִ�.

	Programming by Chojoongpil
	All copyright 1996 (c), MAIET entertainment software
*/
#include "stdafx.h"
#include <crtdbg.h>
//#include "..\CoreLib\CMLog.h"
#include "RealSoundBgm.h"
#include <math.h>

#ifdef _DEBUG
	#define _D		::OutputDebugString
#else 
	#define _D		
#endif

/////////////////////////////////////////
// RealSoundBgm Class Implement

// The following constants are the defaults for our streaming buffer operation.
const UINT DefBufferLength          = 1000;	// default buffer length in msec
const UINT DefBufferServiceInterval = 250;	// default bSuffer service interval in msec

//////////////////////////////////////
// Constructor & Destructor

RealSoundBgm::RealSoundBgm()
{
    // Initialize data members
    m_pRealSound = NULL;					// WaGe (Waveform Generator) Object's Pointer
    m_pwavefile = NULL;				// Wavefile Object's Pointer
    m_pdsb = NULL;					// BGM �����͸� ������ DirectSound Buffer
    m_ptimer = NULL;				// Multimedia Timer

    m_bLooped = m_fPlaying = m_fCued = FALSE;	// Semaphores for Streaming
    m_lInService = FALSE;			// Semaphore

    m_cbBufOffset = 0;				// Write Position
    m_nBufLength = DefBufferLength;	// Sound Buffer�� ���� (in msec) ?????
    m_cbBufSize = 0;				// Waveform Object Wave Buffer Size
    m_nBufService = DefBufferServiceInterval;	// Service Interval
    m_nDuration = 0;				// duration of wave file ( in KB(s) )
	/*
	    m_nDataSize = m_mmckiData.cksize;
	    m_nDuration = (m_nDataSize * 1000) / m_nAvgDataRate;
	*/
    m_nTimeStarted = 0;				// Play�� ���۵� �ý��� �ð�.
    m_nTimeElapsed = 0;				// Play�� ������ ��� �ð�.	
}

RealSoundBgm::~RealSoundBgm()
{
    Destroy ();
}

//////////////////////////////////////
// Member Functions Implementation

// Multimedia Timer��ü�� ���� Callback�Լ�.
BOOL RealSoundBgm::TimerCallback (DWORD dwUser)
{
    RealSoundBgm * pas = (RealSoundBgm *) dwUser;	// C++������ Callback�Լ��� ���� �̿� ���� ����� ����Ͽ���.
	if( pas ) return pas->ServiceBuffer(); else return FALSE;
}

/*
	pszFilename���� �־��� Waveform file�� ���ʷ� BGM Object�� �����Ѵ�.

	pszFilename : Wave file name
	pRealSound       : WaGe object pointer
*/
BOOL RealSoundBgm::Create (LPSTR pszFilename, RealSound* pRealSound)
{
	Destroy();

#ifdef _DEBUG
    _ASSERT (pszFilename);
    _ASSERT (pRealSound);
#endif	   
    m_pRealSound = pRealSound;

	if( !(m_pwavefile = new RSMWaveFile) ){
		_D("RealSoundBgm::Create error : Cannot Create Wave Object\n");
		return FALSE;
	}
	
	if( !m_pwavefile->Open( pszFilename ) ){
		delete (m_pwavefile);
		m_pwavefile = NULL;
		return FALSE;
	}	
	
	m_cbBufSize = (m_pwavefile->GetAvgDataRate () * m_nBufLength) / 1000;	

	m_cbBufSize = (m_cbBufSize > m_pwavefile->GetDataSize ()) ? m_pwavefile->GetDataSize () : m_cbBufSize;

	// Get duration of sound
	m_nDuration = m_pwavefile->GetDuration();
	                
	// Create sound buffer
	HRESULT hr;
	memset (&m_dsbd, 0, sizeof (DSBUFFERDESC));
	m_dsbd.dwSize = sizeof (DSBUFFERDESC);
	m_dsbd.dwFlags = DSBCAPS_CTRLVOLUME;
	m_dsbd.dwBufferBytes = m_cbBufSize;
	m_dsbd.lpwfxFormat = m_pwavefile->m_pwfmt;
	hr = (m_pRealSound->GetDS())->CreateSoundBuffer (&m_dsbd, &m_pdsb, NULL);
	
	if( hr != DS_OK ){
		_D("RealSoundBgm::Create error : Cannot create sound buffer.\n");
		return FALSE;
	}

	Cue();	// �غ��ȣ�� �ش�.
        
    return TRUE;
}

/*
	Destroy

	Destroy A BGM Object.
*/
void RealSoundBgm::Destroy (void)
{
    Stop ();	// if playing, stop it!
	m_bLooped = m_fPlaying = FALSE;
	        
    if (m_pdsb){		
        m_pdsb->Release ();		// release DirectSoundBuffer for BGM.
        m_pdsb = NULL;
    }
	    
    if (m_pwavefile){
        delete (m_pwavefile);	// Destroy Wavefile Object...
        m_pwavefile = NULL;
    }
}

/*
	Cue

	Play�� ���ؼ� ��ȣ�� �ش�. (ready��Ų��.)
*/
void RealSoundBgm::Cue (void)
{   
    if (!m_fCued)
    {	        
        m_cbBufOffset = 0;				// Write Position�� zero�� ���½�Ų��.		        
        m_pwavefile->Cue ();			// File Pointer���� ����.        
        m_pdsb->SetCurrentPosition (0); // Buffer���¸� �ʱ�ȭ. 
        WriteWaveData (m_cbBufSize);	// Fill buffer with wave data        
        m_fCued = TRUE;					// Semaphore True�� �ʱ�ȭ.
    }
}

/*
	WriteWaveData

	size��ŭ Sound Buffer�� Wavedata�� ����Ѵ�.
*/
BOOL RealSoundBgm::WriteWaveData (UINT size)
{
    HRESULT hr;
    LPBYTE lpbuf1 = NULL;
    LPBYTE lpbuf2 = NULL;
    DWORD dwsize1 = 0;
    DWORD dwsize2 = 0;
    DWORD dwbyteswritten1 = 0;
    DWORD dwbyteswritten2 = 0;
    
    // Lock the sound buffer
    hr = m_pdsb->Lock(m_cbBufOffset, size, (void **)&lpbuf1, &dwsize1, (void **)&lpbuf2, &dwsize2, 0);
	if( hr != DS_OK ) return FALSE;
   
	// Write data to sound buffer. Because the sound buffer is circular, we may have to
	// do two write operations if locked portion of buffer wraps around to start of buffer.

	_ASSERT (lpbuf1);	
	if ((dwbyteswritten1 = m_pwavefile->Read (lpbuf1, dwsize1)) != dwsize1) return FALSE;
		// Second write required?
	
	if ( lpbuf2 )	// ���� 2�� ���۱��� ��� �Ѵٸ� ���� ������ ���Ѵ�.
		if ((dwbyteswritten2 = m_pwavefile->Read (lpbuf2, dwsize2)) != dwsize2) 
			return FALSE;	

	// Update Write Cursor
    m_cbBufOffset = (m_cbBufOffset + dwbyteswritten1 + dwbyteswritten2) % m_cbBufSize;

    m_pdsb->Unlock (lpbuf1, dwbyteswritten1, lpbuf2, dwbyteswritten2);
	
    return TRUE;
}

/*
	WriteSilence

	size��ŭ�� Silence�����͸� ��ִ´�.
*/
BOOL RealSoundBgm::WriteSilence (UINT size)
{	
    HRESULT hr;
    LPBYTE lpbuf1 = NULL;
    LPBYTE lpbuf2 = NULL;
    DWORD dwsize1 = 0;
    DWORD dwsize2 = 0;
    DWORD dwbyteswritten1 = 0;
    DWORD dwbyteswritten2 = 0;
        
    hr = m_pdsb->Lock(m_cbBufOffset, size, (void **)&lpbuf1, &dwsize1, (void **)&lpbuf2, &dwsize2, 0);
	if( hr != DS_OK ){
		_D("RealSoundBgm::WriteSilence error : Unable to lock.\n");
		return FALSE;
	}

	BYTE bSilence = m_pwavefile->GetSilenceData ();
        
    memset(lpbuf1, bSilence, dwsize1);	// 1�� ���ۿ� ���� ������ ���Ѵ�. (Silence Data)
    
	dwbyteswritten1 = dwsize1;
        
    if (lpbuf2){	// 2nd Buffer�� �ʿ��ϴٸ� 2�� ���۵� ���� ������ ���Ѵ�. (Silence Data)
        memset (lpbuf1, bSilence, dwsize1);
        dwbyteswritten2 = dwsize2;
    }
	
    // Write Cursor Position�� Update�Ѵ�.
    m_cbBufOffset = (m_cbBufOffset + dwbyteswritten1 + dwbyteswritten2) % m_cbBufSize;
    m_pdsb->Unlock (lpbuf1, dwbyteswritten1, lpbuf2, dwbyteswritten2);

    return TRUE;
}


/*
	GetMaxWriteSize
	
	���� ������ ���� Sound Buffer�� �ִ� ũ�⸦ ���Ѵ�.
*/
DWORD RealSoundBgm::GetMaxWriteSize (void)
{
    DWORD dwWriteCursor, dwPlayCursor, dwMaxSize;
	
	if (m_pdsb->GetCurrentPosition (&dwPlayCursor, &dwWriteCursor) != DS_OK){
		_D("Fatal ! : Unable To Get Maximum Writable Size\n");
		dwMaxSize = 0;
	}

    if (m_cbBufOffset <= dwPlayCursor)	// ���� Ŀ���� ��ġ�� �÷��� Ŀ���� ��ġ���� ������
    {
        // Our write position trails play cursor
        dwMaxSize = dwPlayCursor - m_cbBufOffset;
    }

    else // (m_cbBufOffset > dwPlayCursor)
    {	// Play Cursor�� Wrapping��Ų��. (Sound Buffer�� ȯ���̴�.)
        dwMaxSize = m_cbBufSize - m_cbBufOffset + dwPlayCursor;
    }
	       
    return (dwMaxSize);
}

/*
	Play()

	Create�� ��ü�� �����Ų��.
	Play�� �� ��ü�� ó������ ����ϰ� �ȴ�.
*/
void RealSoundBgm::Play( BOOL bLoop )
{
    if (m_pdsb)						// Buffer�� �����ؾ� �Ѵ�.
    {
        if (m_fPlaying) Stop ();	// Play���϶��� ���� ����.
        if (!m_fCued) Cue ();		// ���� �ʱ�ȭ �Ǿ� ���� �ʴٸ� Buffer�� ���� �ʱ�ȭ�� �Ѵ�.
        HRESULT hr = m_pdsb->Play (0, 0, DSBPLAY_LOOPING);	// Streaming ��������� ���۸� �����Ͽ�
															// �����ϴ� ����� ����Ѵ�.
															// Sound Buffer�� ȯ������ �����ϰ� �츮��
															// ��Ƽ�̵�� Ÿ�̸Ӹ� �̿��Ͽ� �̸� �����Ͽ�
															// ���� ���۸� ����Ͽ� ū �����͸� ������ ��
															// �ְ� �Ѵ�.
        if( hr != DS_OK ) _D("RealSoundBgm::Play : Fail to play\n");
		
		m_nTimeStarted = timeGetTime ();
		m_ptimer = new MMTimer ();	// ���� ������ ���Ͽ� Multimedia Timer�� ������Ų��.
		if (m_ptimer){
			if( !m_ptimer->Create (m_nBufService, m_nBufService, DWORD (this), TimerCallback) ){
				m_pdsb->Stop ();		
		        delete (m_ptimer);
				m_ptimer = NULL;
				m_fPlaying = FALSE;

				Cue();
				//LOG("RealSoundBgm::Play Error : Unable to create multimedia timer. GetLastError() = %d.\n", GetLastError());
				//_D ("RealSoundBgm::Play Error : Unable to create multimedia timer.\n");
				return;
			}
		} else {
			m_pdsb->Stop ();		
	        delete (m_ptimer);
			m_ptimer = NULL;
			m_fPlaying = FALSE;

			Cue();
			_D ("RealSoundBgm::Play Error : Unable to create multimedia timer.\n");
			return;
		}
		
		m_fPlaying = TRUE;	// Playing Flag�� ��Ʈ��Ų��.
		m_fCued = FALSE;	// Cue flag�� ��������. (�̹� �÷��̴� ���۵Ǿ���.)
		m_bLooped = bLoop;
    }
}

/*
	Stop

	BGM�� ���ָ� ������Ų��. �ʱ���·� ��� ��ȯ��Ų��.
*/
void RealSoundBgm::Stop()
{
    if (m_fPlaying)
    {		
        m_pdsb->Stop ();		
        delete (m_ptimer);
        m_ptimer = NULL;
        m_fPlaying = FALSE;
    }
}

/*
	ServiceBuffer

	Ÿ�̸ӿ� ���� ����Ǵ� ���� ���� ��ƾ, RealSoundBgm�� �ٽ� ��Ʈ�̴�.
	Boolean���� ������� ��ȯ�Ѵ�. ������ ��� -> TRUE, ������ ������ ��� -> FALSE
*/
BOOL RealSoundBgm::ServiceBuffer (void)
{
    BOOL fRtn = TRUE;

	//LOG("RealSoundBgm::ServiceBuffer...\n");

    // Check for reentrance
    if (InterlockedExchange (&m_lInService, TRUE) == FALSE)
    {		
        // Not reentered, proceed normally
		// Maintain elapsed time count
        m_nTimeElapsed = timeGetTime () - m_nTimeStarted;

        // Stop if all of sound has played
        if (m_nTimeElapsed < m_nDuration)
        {
            // All of sound not played yet, send more data to buffer
            DWORD dwFreeSpace = GetMaxWriteSize ();

            // Determine free space in sound buffer
            if (dwFreeSpace)
            {
                // See how much wave data remains to be sent to buffer
                DWORD dwDataRemaining = m_pwavefile->GetNumBytesRemaining ();
                if (dwDataRemaining == 0)
                {
                    // All wave data has been sent to buffer
                    // Fill free space with silence
                    if (WriteSilence (dwFreeSpace) == FALSE)
                    {
                        // Error writing silence data
                        fRtn = FALSE;
                        _ASSERT (0);
                        _D("RealSoundBgm::ServiceBuffer Error : WriteSilence failed\n");
                    }
                }
                else if (dwDataRemaining >= dwFreeSpace)
                {
                    // Enough wave data remains to fill free space in buffer
                    // Fill free space in buffer with wave data
                    if (WriteWaveData (dwFreeSpace) == FALSE)
                    {
                        // Error writing wave data
                        fRtn = FALSE;
                        _ASSERT (0);
                        _D("RealSoundBgm::ServiceBuffer Error : WriteWaveData failed\n");
                    }
                }
                else
                {
                    // Some wave data remains, but not enough to fill free space
                    // Send wave data to buffer, fill remainder of free space with silence
                    if (WriteWaveData (dwDataRemaining) == TRUE)
                    {
                        if (WriteSilence (dwFreeSpace - dwDataRemaining) == FALSE)
                        {
                            // Error writing silence data
                            fRtn = FALSE;
                            _ASSERT (0);
                            _D("RealSoundBgm::ServiceBuffer Error : WriteSilence failed\n");
                        }
                    }
                    else
                    {
                        // Error writing wave data
                        fRtn = FALSE;
                        _ASSERT (0);
                        _D("RealSoundBgm::ServiceBuffer Error : WriteWaveData failed\n");
                    }
                }
            }
            else
            {
                // No free space in buffer for some reason
                fRtn = FALSE;
            }
        }
        else
        {
			//LOG("RealSoundBgm::ServiceBuffer, Wrapping Around.\n");
            // All of sound has played, stop playback            
			if( m_bLooped ){
				Play(TRUE);
			} else Stop();
        }

        // Reset reentrancy semaphore
        InterlockedExchange (&m_lInService, FALSE);
    }
    else
    {
		//LOG("RealSoundBgm::ServiceBuffer, Cannot InterlockedExchange.\n");
        // Service routine reentered. Do nothing, just return
        fRtn = FALSE;
    }
    return (fRtn);
}

bool RealSoundBgm::IsPlaying(void)
{
	return m_fPlaying;
}

void RealSoundBgm::SetVolume(float t)
{
	float fVolumeConstant = t;
	//fVolumeConstant = (float)cos(3.141592*(1.0f-t));
#define MIN	(DSBVOLUME_MIN/2)
	if(m_pdsb!=NULL) m_pdsb->SetVolume(long(MIN + (DSBVOLUME_MAX-MIN)*fVolumeConstant));
}
