#pragma once

#include <string>
#define NUMCHANNELS		    128
#define MAXSONGS		    512
#define FSOUND_BUFFERSIZE   200         /* millisecond value for FMOD buffersize. */
#define MRU_MAX             16
#define STREAM_PLAYING      255


enum
{
	SONGTYPE_NONE = 0,
	SONGTYPE_MOD,
	SONGTYPE_STREAM,
	SONGTYPE_NETSTREAM,
	SONGTYPE_CD,
};

enum MUSIC_STATE
{
	MUSIC_STATE_NONE,
	MUSIC_STATE_LOAD,
	MUSIC_STATE_PLAY,
	MUSIC_STATE_PAUSE,
	MUSIC_STATE_STOP,
};
typedef struct FSOUND_SAMPLE    FSOUND_SAMPLE;
typedef struct FSOUND_STREAM    FSOUND_STREAM;
typedef struct FMUSIC_MODULE    FMUSIC_MODULE;
#define F_CALLBACKAPI __stdcall
typedef struct
{
	FMUSIC_MODULE  *mod;
	FSOUND_STREAM  *stream;
	int             channel;
	int				nNowVolume;
	int				nVolume;
	char           *url;
	int				state;
	int             last_status;
	int             last_netstatus;
	char           *server_status;
	char           *title;
	char           *artist;
	char           *protocol;
	char           *format;
	char           *streamname;
	int             metadata;
	char           *listname;
	int             cdtrack;
	unsigned int    cdtrack_length;
} MUSIC;

class CAudio
{
public:
	CAudio(void);
	~CAudio(void);

	// FrameMove
	void FrameMove(float fElapsedTime);

	// ����
	bool LoadMusic(const char* szFilename);	// ��������
	void SetMusicVolume(int nVolume = 255);	// ���õ�ǰ����������
	void PlayMusic(bool bLoop = true, int nVolume = 255);	// ��������
	void PlayMusicEx(const char* szFilename, bool bLoop = true, int nVolume = 255);

	void ReplayMusic();	// �ز�����
	void PausedMusic();	// ��ͣ����
	void StopMusic();	// ֹͣ����

	void PlayBGM(const char* szFilename, bool bLoop = true, int nVolume = 255);	// ���ű�������

	// ��Ч
	void LoadSound(const char* szFilename);	// ��������
	void playSound(const char* szFilename);	// ��������
	void Play3DSound(const char* szFilename, float x, float y, float fRadius=-1, int nVolume = 255);
	void PlaySoundEx(const char* szFilename, bool bLoop = true, int nVolume = 255);

	void ReplaySound(int nChannel = -1);	// �ز�����
	void PausedSound(int nChannel = -1);	// ��ͣ����
	void StopSound(int nChannel = -1);		// ֹͣ����

	// ��������
	void SetBGMVolume(int nVolume){ m_nBGMVolume = nVolume; }
	void SetSeVolume(int nVolume){ m_nSeVolume = nVolume; }

	// ���Ч��
	signed short* GetOscBuffer();
	int GetBufferLength();
private:
	static void * F_CALLBACKAPI OscCallback(void *originalbuffer, void *newbuffer, int length, void *userdata);
	void InitDSP();
	void CloseDSP();

	int	DSP_Ready;
private:

	// ���־��
	MUSIC	m_Music;
	// �������
	FSOUND_SAMPLE* m_SeHandle[255];

	//
	int		m_nVol0;
	int		m_nVol1;
	//

	// ������������
	int		m_nBGMVolume;
	// ��Ч����
	int		m_nSeVolume;
};
CAudio& GetAudio();