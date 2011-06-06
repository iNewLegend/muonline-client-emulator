#include "Audio.h"
#if defined(_DEBUG)
#pragma comment(lib, "fmodvc.lib")
#else
#pragma comment(lib, "fmodvc.lib")
#endif
#include "fmod.h"
#include "fmod_errors.h"
#include "windows.h"
/*
Oscilliscope stuff
*/

FSOUND_DSPUNIT      *OscUnit;
static signed short *OscBuffer;
static int           OscBlock;

CAudio& GetAudio()
{
	static CAudio g_Audio;
	return g_Audio;
}

CAudio::CAudio(void)
{
	FSOUND_Init(44100, 32, 0);
	m_Music.mod = NULL;
	m_Music.stream = NULL;
	m_Music.nNowVolume = 0;
	m_Music.nVolume = 255;
	m_nBGMVolume = 255;
	m_nSeVolume = 255;

	DSP_Ready = false;
	OscUnit = NULL;
	OscBuffer = NULL;
	OscBlock = 0;

	InitDSP();
}

CAudio::~CAudio(void)
{
	CloseDSP();

	if (m_Music.mod)
	{
		FMUSIC_FreeSong(m_Music.mod);
		m_Music.mod = NULL;
	}
	if (m_Music.stream)
	{
		FSOUND_Stream_Close(m_Music.stream);
		m_Music.stream = NULL;
	}
	FSOUND_Close();
}

void CAudio::FrameMove(float fElapsedTime)
{
	m_Music.nNowVolume += int((m_Music.nVolume*1000 - m_Music.nNowVolume)*fElapsedTime);
	// 
	if (m_Music.mod)
	{
		FMUSIC_SetMasterVolume(m_Music.mod, m_Music.nNowVolume/1000);
	}
	else if (m_Music.stream)
	{
		FSOUND_SetVolume(m_Music.channel, m_Music.nNowVolume/1000);
	}
}

bool CAudio::LoadMusic(const char* szFilename)
{
	// 先清空
	if (m_Music.mod)
	{
		FMUSIC_FreeSong(m_Music.mod);
		m_Music.mod = NULL;
	}
	if (m_Music.stream)
	{
		FSOUND_Stream_Close(m_Music.stream);
		m_Music.stream = NULL;
	}
	// 用MOD载入
	m_Music.mod=FMUSIC_LoadSong(szFilename);

	// 不能载入就用流载入
	if (!m_Music.mod)
	{
		m_Music.stream = FSOUND_Stream_Open(szFilename, FSOUND_NORMAL | FSOUND_2D | FSOUND_MPEGACCURATE | FSOUND_NONBLOCKING, 0, 0);
	}
	if (!m_Music.mod && !m_Music.stream)
	{
		//MessageBoxA(0, FMOD_ErrorString(FSOUND_GetError()), "Loading a song", MB_ICONHAND|MB_OK|MB_SYSTEMMODAL);
		return false;
	}
	return true;
}

void CAudio::SetMusicVolume(int nVolume)
{
	m_Music.nVolume = nVolume*m_nBGMVolume/255;
}

void CAudio::PlayMusic(bool bLoop, int nVolume)
{
	SetMusicVolume(nVolume);
	if (m_Music.mod)
	{
		FMUSIC_SetLooping(m_Music.mod, bLoop);
		FMUSIC_PlaySong(m_Music.mod);
	}
	else if (m_Music.stream)
	{
		if (FSOUND_Stream_GetOpenState(m_Music.stream) != 0)
		{
			while (FSOUND_Stream_GetOpenState(m_Music.stream) == -2)
			{
				Sleep(100);
			}
		}
		FSOUND_Stream_SetMode(m_Music.stream, bLoop ? FSOUND_LOOP_NORMAL : FSOUND_LOOP_OFF);
		m_Music.channel = FSOUND_Stream_Play(FSOUND_FREE, m_Music.stream);
	}
}

void CAudio::PlayBGM(const char* szFilename, bool bLoop, int nVolume)
{
	if (LoadMusic(szFilename))
	{
		PlayMusic(bLoop, nVolume);
	}
}

void CAudio::PlayMusicEx(const char* szFilename, bool bLoop, int nVolume)
{
}

void CAudio::ReplayMusic()
{
	if (m_Music.mod)
	{
		FMUSIC_SetPaused (m_Music.mod, false);
	}
	else if (m_Music.stream)
	{
		//FSOUND_Stream_Stop(m_mod);
	}
}

void CAudio::PausedMusic()
{
	if (m_Music.mod)
	{
		FMUSIC_SetPaused(m_Music.mod, true);
	}
	else if (m_Music.stream)
	{
		// FSOUND_Stream_Paused(m_mod, true);
	}
}

void CAudio::StopMusic()
{
	if (m_Music.mod)
	{
		FMUSIC_StopSong(m_Music.mod);
	}
	else if (m_Music.stream)
	{
		FSOUND_Stream_Stop(m_Music.stream);
	}
}

signed short* CAudio::GetOscBuffer()
{
	if (OscBuffer)
	{
		//int count, count2, offset;
		int offset;
		//float xoff, step;
		signed short *src;
		/*
		The next pcmblock (Oscblock + 1) is the one that is audible.
		*/
		offset = (OscBlock + 1) * FSOUND_DSP_GetBufferLength();
		if (offset >= FSOUND_DSP_GetBufferLengthTotal())
		{
			offset -= FSOUND_DSP_GetBufferLengthTotal();
		}

		src = &OscBuffer[offset];
		return src;
	}
	return 0;
}

int CAudio::GetBufferLength()
{
	return FSOUND_DSP_GetBufferLength();
}

void CAudio::InitDSP()
{
	int bytesperoutputsample;
	int mixertype = FSOUND_GetMixer();

	DSP_Ready = false;

	if (mixertype == FSOUND_MIXER_MMXP5 || mixertype == FSOUND_MIXER_MMXP6 || mixertype == FSOUND_MIXER_QUALITY_MMXP5 || mixertype == FSOUND_MIXER_QUALITY_MMXP6)
	{
		bytesperoutputsample = 4;   // 16bit stereo
	}
	else
	{
		bytesperoutputsample = 8;   // 32bit stereo
	}

	/*
	Initalize and create lowpass buffer and DSP unit 
	*/
	//LowPass_Init();
	//LowPassBuffer = calloc(FSOUND_DSP_GetBufferLength()+256, bytesperoutputsample);
	//LowPass_Update(LowPassResonance, LowPassCutoffFrequency, outputfreq);
	//LowPassUnit = FSOUND_DSP_Create(&LowPassCallback,	FSOUND_DSP_DEFAULTPRIORITY_USER+1,	LowPassBuffer);

	/*
	Create buffer and dsp unit for echo effect
	*/
	//EchoLen		= MAXECHOLEN;				/* 500ms */
	//EchoBuffer  = calloc(EchoLen, 4);	    /* The echo buff is always 16bit stereo int regardless of the mixer format, so * 4 */
	//EchoOffset	= 0;
	//EchoUnit	= FSOUND_DSP_Create(&EchoCallback, FSOUND_DSP_DEFAULTPRIORITY_USER+2, EchoBuffer);

	/*
	Create buffer and dsp unit for oscilliscope.
	*/
	OscUnit	    = FSOUND_DSP_Create(&OscCallback, FSOUND_DSP_DEFAULTPRIORITY_USER+3,	0);
	OscBuffer   = (short*)calloc(FSOUND_DSP_GetBufferLengthTotal() + 16, 2); /* *2 for mono 16bit buffer */

	 FSOUND_DSP_SetActive(OscUnit, true);
	/*
	Initialize reverb stuff
	*/
	//Reverb_Init();

	DSP_Ready = true;
}

void CAudio::CloseDSP()
{
	DSP_Ready = false;

	//if (LowPassUnit)
	//{
	//	FSOUND_DSP_Free(LowPassUnit);
	//}
	//LowPassUnit = NULL;

	//if (EchoUnit)
	//{
	//	FSOUND_DSP_Free(EchoUnit);
	//}
	//EchoUnit = NULL;

	if (OscUnit)
	{
		FSOUND_DSP_Free(OscUnit);
	}
	OscUnit = NULL;

	/*
	Free buffers
	*/
	//if (LowPassBuffer)
	//{
	//	free(LowPassBuffer);
	//}
	//LowPassBuffer = NULL;

	//if (EchoBuffer)
	//{
	//	free(EchoBuffer);
	//}
	//EchoBuffer = NULL;

	if (OscBuffer)
	{
		free(OscBuffer);
	}
	OscBuffer = NULL;

	//Reverb_Close();
	//LowPass_Close();
}

void * F_CALLBACKAPI CAudio::OscCallback(void *originalbuffer, void *newbuffer, int length, void *userdata)
{
	int             mixertype = FSOUND_GetMixer();
	int             count;
	int             totalblocks; 
	signed short    *dest;

	totalblocks = FSOUND_DSP_GetBufferLengthTotal() / FSOUND_DSP_GetBufferLength(); 

	/*
	Convert and downmix into a mono short int buffer.
	*/

	dest = &OscBuffer[OscBlock * FSOUND_DSP_GetBufferLength()];

	if (mixertype == FSOUND_MIXER_QUALITY_FPU)
	{
		float *src = (float *)newbuffer;

		for (count=0; count < length; count++)
		{
			dest[count] = (signed short)((src[count << 1] + src[(count << 1) + 1]) * 0.5f);
		}
	}
	else if (mixertype == FSOUND_MIXER_MMXP5 || mixertype == FSOUND_MIXER_MMXP6 || mixertype == FSOUND_MIXER_QUALITY_MMXP5 || mixertype == FSOUND_MIXER_QUALITY_MMXP6)
	{
		signed short *src = (signed short *)newbuffer;

		for (count=0; count < length; count++)
		{
			dest[count] = (signed short)(((int)src[count << 1] + (int)src[(count << 1) + 1]) >> 1);
		}
	}
	else
	{
		signed int *src = (signed int *)newbuffer;

		for (count=0; count < length; count++)
		{
			dest[count] = (signed short)((src[count << 1] + src[(count << 1) + 1]) >> 1);
		}
	}

	OscBlock++;
	if (OscBlock >= totalblocks)
	{
		OscBlock = 0;
	}

	return newbuffer;
}


void CAudio::LoadSound(const char* szFilename)
{

}
void CAudio::playSound(const char* szFilename)
{
	FSOUND_STREAM  *stream = FSOUND_Stream_Open(szFilename, FSOUND_NORMAL | FSOUND_2D | FSOUND_MPEGACCURATE, 0, 0);
	//if (FSOUND_Stream_GetOpenState(stream) != 0)
	{
		//while (FSOUND_Stream_GetOpenState(stream) == -2)
		{
			//Sleep(100);
		}
	}
	FSOUND_Stream_SetMode(stream, FSOUND_LOOP_OFF);
	m_Music.channel = FSOUND_Stream_Play(FSOUND_FREE, stream);
}
void CAudio::Play3DSound(const char* szFilename, float x, float y, float fRadius, int nVolume)
{

}
void CAudio::PlaySoundEx(const char* szFilename, bool bLoop, int nVolume)
{

}

void CAudio::ReplaySound(int nChannel)
{

}
void CAudio::PausedSound(int nChannel)
{

}
void CAudio::StopSound(int nChannel)
{

}