////////////////////////////////////////////////////////////////////////////////////////////////////
/// 
/// BandiCaptureLibrary 1.1
/// 
/// Copyright(C) 2008 BandiSoft.com All rights reserved.
///
/// Visit http://www.bandisoft.com for more information.
///
////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef _BANDICAP_H
#define _BANDICAP_H

#include <windows.h>
#include <unknwn.h>
#include <mmsystem.h>
#include <stdio.h>

#ifndef ASSERT
#	include "assert.h"
#	define ASSERT(x) assert(x)
#endif

#ifndef _INC_TCHAR
// "bandicap.h"�� include �ϱ� ���� <tchar.h> �� ���� include �ؾ��մϴ�.
#	error	You have to include <tchar.h> before "bandicap.h"
#endif

#define _BCAP_MAKE_VER(a,b,c,d)	((a<<24)+(b<<16)+(c<<8)+d)
#define BCAP_VERSION			_BCAP_MAKE_VER(1,1,2,10)		// 1.1.2.10

#define FOURCC_MJPG mmioFOURCC('M','J','P','G')		// MJPEG
#define FOURCC_MP4V mmioFOURCC('M','P','4','V')		// MPEG4

#define WAVETAG_PCM	0x0001							// PCM


// ĸó ���
enum 
{
	BCAP_MODE_GDI,				// GDI�� DC�� �̿��� ȭ�� ĸó
	BCAP_MODE_D3D9,				// D3D9�� �̿��� ȭ�� ĸó
	BCAP_MODE_D3D9_SCALE,		// D3D9�� �̿��� ȭ�� ĸó(D3D9 ������¡ ó�� ����)
	BCAP_MODE_D3D8,				// D3D8�� �̿��� ȭ�� ĸó
	BCAP_MODE_D3D10,			// D3D10�� �̿��� ȭ�� ĸó
	BCAP_MODE_GL				// OpenGL�� �̿��� ȭ�� ĸó
};

// BCAP ���� ����ü
struct BCAP_CONFIG
{
	enum { F_AVI };
	enum { V_CBR, V_VBR };
	enum { A_CBR, A_VBR };

	BCAP_CONFIG() { Default(); };
	void Default()
	{
		FileType				= F_AVI;

		VideoSizeW				= 0;
		VideoSizeH				= 0;
		VideoFPS				= 30.000;

		VideoCodec				= FOURCC_MP4V;
		VideoRateControl		= V_VBR;
		VideoKBitrate			= 1024;
		VideoQuality			= 80;
		VideoKeyframeInterval	= 150;

		AudioChannels			= 2;
		AudioSampleRate			= 24000;

		AudioCodec				= WAVETAG_PCM;
		AudioRateControl		= A_CBR;
		AudioKBitrate			= 128;
		AudioQuality			= 30;

		LogoPath[0]				= '\0';
		LogoPosX				= 50;
		LogoPosY				= 100;
		LogoSizeW				= 100;
		LogoSizeH				= 100;	
		LogoAlpha				= 0;
	};

	// �⺻ ���� 
	INT		FileType;				// ����� ������ Ÿ��(F_AVI)

	// ���� ���� 
	INT		VideoSizeW;				// ���� ����ũ�� 
	INT		VideoSizeH;				// ���� ����ũ�� 
	FLOAT	VideoFPS;				// �ʴ� ������ �� 

	DWORD	VideoCodec;				// ���� �ڵ�(FOURCC) 
	INT		VideoRateControl;		// CBR, VBR 
	INT		VideoKBitrate;			// 10k ~ 50000k		(VBR�϶��� �̰��� ����) 
	INT		VideoQuality;			// 30 ~ 100			(CBR�϶��� �̰��� ����) 
	INT		VideoKeyframeInterval;	// 0 ~ 1000 frames
	
	// ����� ���� 
	INT		AudioChannels;			// 1: mono, 2:stereo 
	INT		AudioSampleRate;		// ���÷���Ʈ 

	INT		AudioCodec;				// ����� �ڵ�(Wave Tag) 
	INT		AudioRateControl;		// CBR, VBR 
	INT		AudioKBitrate;			// 32k ~ 320k 
	INT		AudioQuality;			// 30 ~ 100 

	// �ΰ� ����
	WCHAR	LogoPath[MAX_PATH];		// �ΰ� ���� ��� (32bit png)
	INT		LogoAlpha;				// �ΰ� ���� [0 ~ 100]%
	INT		LogoPosX;				// �ΰ� ���� ��ġ [0 ~ 100]%
	INT		LogoPosY;				// �ΰ� ���� ��ġ [0 ~ 100]%
	INT		LogoSizeW;				// �ΰ� ���� ũ�� [0 ~ 999]%
	INT		LogoSizeH;				// �ΰ� ���� ��ġ [0 ~ 999]%
};


// �ݵ�ĸ �������̽�
struct IBandiCapture : public IUnknown
{
	STDMETHOD (Verify)(LPCSTR szID, LPCSTR szKey) PURE;				// �����ϱ�

	STDMETHOD_(INT, GetVersion)() PURE;								// ���� ���� 

	STDMETHOD (Start)(LPCWSTR pwszFilePath, HWND hParentWnd,
					INT nCaptureMode, LONG_PTR lpParam) PURE;		// ĸó ���� 
	STDMETHOD (Stop)() PURE;										// ĸó ���� 
	STDMETHOD (Work)(LONG_PTR lpParam) PURE;						// ĸó �۾� 
	STDMETHOD_(BOOL, IsCapturing)() PURE;							// ĸó �۾� 

	STDMETHOD (CheckConfig)(BCAP_CONFIG *pCfg) PURE;				// ĸó ���� üũ
	STDMETHOD (SetConfig)(BCAP_CONFIG *pCfg) PURE;					// ĸó ���� ���� 
	STDMETHOD (GetConfig)(BCAP_CONFIG *pCfg) PURE;					// ĸó ���� ��� 

	STDMETHOD_(INT, GetCaptureTime)() PURE;							// ������� ĸó�� �ð� (msec) 

	STDMETHOD (SetPriority)(INT nPriority) PURE;					// ĸó ����� �켱���� ���� 
	STDMETHOD_(INT, GetPriority)() PURE;							// ���� ����� �켱���� ��� 

	STDMETHOD (SetMinMaxFPS)(INT nMin, INT nMax) PURE;				// ĸó �������� FPS ���� ����
	STDMETHOD (GetMinMaxFPS)(INT *pnMin, INT *pnMax) PURE;			// ĸó �������� FPS ���� ���
};


// �ݵ�ĸ �������̽� ���� �Լ�
typedef HRESULT (WINAPI *LPCREATEBANDICAP)(UINT SDKVersion, void** pp);


////////////////////////////////////////////////////////////////////////////////////////////////////
// �ݵ�ĸ ���� �ڵ�
#define _FACBC  0x777
#define MAKE_BCHRESULT( code )				MAKE_HRESULT( 1, _FACBC, code )
#define MAKE_BCSTATUS( code )				MAKE_HRESULT( 0, _FACBC, code )

#define BC_OK								S_OK
#define BC_FALSE							S_FALSE
#define BC_OK_ONLY_VIDEO					MAKE_BCSTATUS(0x0010)
#define BC_OK_ONLY_AUDIO					MAKE_BCSTATUS(0x0011)
#define BC_OK_IS_NOT_CAPTURING				MAKE_BCSTATUS(0x0020)

#define BCERR_FAIL							E_FAIL

#define BCERR_INVALIDARG					MAKE_BCHRESULT(0x1010)
#define BCERR_VIDEO_CAPTURE					MAKE_BCHRESULT(0x1011)
#define BCERR_AUDIO_CAPTURE					MAKE_BCHRESULT(0x1012)
#define BCERR_FILE_CREATE					MAKE_BCHRESULT(0x1013)
#define BCERR_FILE_WRITE					MAKE_BCHRESULT(0x1014)
#define BCERR_NOT_ENOUGH_DISKSPACE			MAKE_BCHRESULT(0x1015)

#define BCERR_LOAD_LIBRARY_FAIL				MAKE_BCHRESULT(0x1020)
#define BCERR_ALREADY_CREATED				MAKE_BCHRESULT(0x1021)
#define BCERR_GET_PROC_ADDRESS_FAIL			MAKE_BCHRESULT(0x1022)
#define BCERR_LIBRARY_NOT_LOADED			MAKE_BCHRESULT(0x1023)
#define BCERR_UNSUPPORTED_OS				MAKE_BCHRESULT(0x1024)

#define BCERR_INVALID_VERSION				MAKE_BCHRESULT(0x1030)
#define BCERR_NOT_ENOUGH_MEMORY				MAKE_BCHRESULT(0x1031)


// �Ʒ� ��ƿ��Ƽ Ŭ������ ������� �ʰ��� �� ��� _BANDICAP_NO_UTIL_CLASS �� define �ؼ� ����ϸ� �ȴ�.
#ifndef _BANDICAP_NO_UTIL_CLASS

////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Utility class for dll load
//
#ifdef _DEBUG
#	define BANDICAP_DLL_FILE_NAME			_T("bdcap32_d.dll")
#else
#	define BANDICAP_DLL_FILE_NAME			_T("bdcap32.dll")
#endif
#define BANDICAP_RELEASE_DLL_FILE_NAME		_T("bdcap32.dll")


class CBandiCaptureLibrary : public IBandiCapture
{
public :
	CBandiCaptureLibrary()
	{
		m_hDll = NULL;
		m_pCreateBandiCap = NULL;
		m_pCap = NULL;
	}

	~CBandiCaptureLibrary()
	{
		Destory();
	}

	HRESULT Create(LPCTSTR szDllPathName)
	{
		if(m_hDll) {ASSERT(0); return BCERR_ALREADY_CREATED;}

		// check os version
		OSVERSIONINFO osv;
		osv.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
		if (GetVersionEx(&osv)==FALSE || 
			osv.dwPlatformId <=VER_PLATFORM_WIN32_WINDOWS)		// does not support 98
		{
			return BCERR_UNSUPPORTED_OS;
		}

		m_hDll = LoadLibrary(szDllPathName);
		if(m_hDll==NULL)
		{
			m_hDll = LoadLibrary(BANDICAP_RELEASE_DLL_FILE_NAME);
			if(m_hDll==NULL)
			{
				ASSERT(0);
				return BCERR_LOAD_LIBRARY_FAIL;
			}
		}

		m_pCreateBandiCap = (LPCREATEBANDICAP)GetProcAddress(m_hDll, "CreateBandiCapture");

		if(m_pCreateBandiCap==NULL)
		{
			ASSERT(0);
			FreeLibrary(m_hDll);
			m_hDll = NULL;
			return BCERR_GET_PROC_ADDRESS_FAIL;
		}

		HRESULT hr = CreateNewBandiCaptureInstance(&m_pCap);
		if(FAILED(hr))
		{
			ASSERT(0);
			FreeLibrary(m_hDll);
			m_hDll = NULL;
			return hr;
		}

		return S_OK;
	}

	BOOL	IsCreated()
	{
		return m_pCap ? TRUE : FALSE;
	}

	void	Destory()
	{
		if(m_pCap)
		{
			m_pCap->Stop();
			m_pCap->Release();
		}
		m_pCap = NULL;
		m_pCreateBandiCap = NULL;
		if(m_hDll) FreeLibrary(m_hDll);
		m_hDll = NULL;
	}

	LPCTSTR MakePathnameByDate(LPCTSTR szTargetDirectory, LPCTSTR szName, LPTSTR szBuf, int nBufLen)
	{
		SYSTEMTIME	sysTime;
		GetLocalTime(&sysTime);
#if _MSC_VER < 1400	// VS2005 �̸�
		_stprintf(szBuf, _T("%s\\%s %d-%02d-%02d %02d-%02d-%02d.avi"), szTargetDirectory, szName, sysTime.wYear, sysTime.wMonth, sysTime.wDay, sysTime.wHour, sysTime.wMinute, sysTime.wSecond);
#else				// 2005, 2008
		_stprintf_s(szBuf, nBufLen, _T("%s\\%s %d-%02d-%02d %02d-%02d-%02d.avi"), szTargetDirectory, szName, sysTime.wYear, sysTime.wMonth, sysTime.wDay, sysTime.wHour, sysTime.wMinute, sysTime.wSecond);
#endif
		return szBuf;
	}

public :		// IUnknown
	HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void** p) { return S_OK; }
	ULONG   STDMETHODCALLTYPE AddRef( void){return 0;}
	ULONG   STDMETHODCALLTYPE Release( void){return 0;}

public :		// IBandiCapture

	// �����ϱ�
	STDMETHODIMP	Verify(LPCSTR szID, LPCSTR szKey)
	{
		if(m_pCap==NULL){ASSERT(0); return E_FAIL;}
		return m_pCap->Verify(szID, szKey);
	}

	// ���� ����
	STDMETHODIMP_(INT)	GetVersion()
	{
		return m_pCap->GetVersion();
	}

	// ĸó ���� 
	STDMETHODIMP	Start(LPCSTR pszFilePath, HWND hParentWnd, INT nCaptureMode, LONG_PTR lpParam)
	{
		WCHAR wszPath[MAX_PATH+1];

		MultiByteToWideChar(CP_ACP, NULL, pszFilePath, -1, wszPath, MAX_PATH);
		return m_pCap->Start(wszPath, hParentWnd, nCaptureMode, lpParam);
	}

	STDMETHODIMP	Start(LPCWSTR pwszFilePath, HWND hParentWnd, INT nCaptureMode, LONG_PTR lpParam)
	{
		if(m_pCap==NULL){ASSERT(0); return E_FAIL;}
		return m_pCap->Start(pwszFilePath, hParentWnd, nCaptureMode, lpParam);
	}

	// ĸó ���� 
	STDMETHODIMP Stop()
	{
		if(m_pCap==NULL){ASSERT(0); return E_FAIL;}
		return m_pCap->Stop();
	}

	// ĸó �۾� 
	STDMETHODIMP Work(LONG_PTR lpParam)
	{
		if(m_pCap==NULL){ASSERT(0); return E_FAIL;}
		return m_pCap->Work(lpParam);
	}

	// ĸó ����
	STDMETHODIMP_(BOOL) IsCapturing()
	{
		if(m_pCap==NULL) return FALSE;
		return m_pCap->IsCapturing();
	}

	// ĸó ���� üũ
	STDMETHODIMP CheckConfig(BCAP_CONFIG *pCfg)
	{
		return m_pCap->CheckConfig(pCfg);
	}

	// ĸó ���� ���� 
	STDMETHODIMP SetConfig(BCAP_CONFIG *pCfg)
	{
		return m_pCap->SetConfig(pCfg);
	}

	// ĸó ���� ��� 
	STDMETHODIMP GetConfig(BCAP_CONFIG *pCfg)
	{
		return m_pCap->GetConfig(pCfg);
	}

	// ������� ĸó�� �ð� (msec) 
	STDMETHODIMP_(INT) GetCaptureTime()
	{
		return m_pCap->GetCaptureTime();
	}

	// ĸó ����� �켱���� ���� 
	STDMETHODIMP SetPriority(INT nPriority)
	{
		return m_pCap->SetPriority(nPriority);
	}

	// ���� ����� �켱���� ��� 
	STDMETHODIMP_(INT) GetPriority()
	{
		return m_pCap->GetPriority();
	}

	// ĸó �������� FPS ���� ����
	STDMETHODIMP SetMinMaxFPS(INT nMin, INT nMax)
	{
		return m_pCap->SetMinMaxFPS(nMin, nMax);
	}

	// ĸó �������� FPS ���� ���
	STDMETHODIMP GetMinMaxFPS(INT *pnMin, INT *pnMax)
	{
		return m_pCap->GetMinMaxFPS(pnMin, pnMax);
	}

private :
	HRESULT CreateNewBandiCaptureInstance(IBandiCapture** ppBaCapture)
	{
		if(m_pCreateBandiCap==NULL){ASSERT(0); return BCERR_LIBRARY_NOT_LOADED;}
		*ppBaCapture = NULL;
		HRESULT hr = m_pCreateBandiCap(BCAP_VERSION, (void**)ppBaCapture);
		if(FAILED(hr)) ASSERT(0);
		return hr;
	}

private :
	HMODULE				m_hDll;
	LPCREATEBANDICAP	m_pCreateBandiCap;

public :
	IBandiCapture*		m_pCap;
};


// PRESETS
enum BCAP_PRESET
{
	BCAP_PRESET_DEFAULT,
	BCAP_PRESET_HALFSIZE,
	BCAP_PRESET_320x240,
	BCAP_PRESET_400x300,
	BCAP_PRESET_512x384,
	BCAP_PRESET_576x432,
	BCAP_PRESET_640x480,
	BCAP_PRESET_800x600,

	BCAP_PRESET_MJPEG,
	BCAP_PRESET_MJPEG_HIGH_QUALITY,

	BCAP_PRESET_YOUTUBE,
	BCAP_PRESET_YOUTUBE_HIGH_QUALITY,
	BCAP_PRESET_NAVER_BLOG,
	BCAP_PRESET_DAUM_TVPOT,
	BCAP_PRESET_MNCAST,
};

// SETTING CONFIG BY PRESET
inline BOOL BCapConfigPreset(BCAP_CONFIG* pConfig, BCAP_PRESET preset)
{
	pConfig->Default();

	switch(preset)
	{
	case BCAP_PRESET_DEFAULT :
		break;

	case BCAP_PRESET_HALFSIZE :
		pConfig->VideoSizeW = -1;			// half size
		pConfig->VideoSizeH = -1;
		break;

	case BCAP_PRESET_320x240 :
		pConfig->VideoSizeW = 320;			// 320x240
		pConfig->VideoSizeH = 0;
		pConfig->VideoFPS = 24;
		break;
	case BCAP_PRESET_400x300 :
		pConfig->VideoSizeW = 400;			// 400x300
		pConfig->VideoSizeH = 0;
		pConfig->VideoFPS = 24;
		break;
	case BCAP_PRESET_512x384 :
		pConfig->VideoSizeW = 512;			// 512x384
		pConfig->VideoSizeH = 0;
		pConfig->VideoFPS = 24;
		break;
	case BCAP_PRESET_576x432 :
		pConfig->VideoSizeW = 576;			// 576x432
		pConfig->VideoSizeH = 0;
		pConfig->VideoFPS = 24;
		break;
	case BCAP_PRESET_640x480 :
		pConfig->VideoSizeW = 640;			// 640x480
		pConfig->VideoSizeH = 0;
		break;
	case BCAP_PRESET_800x600 :
		pConfig->VideoSizeW = 800;			// 800x600
		pConfig->VideoSizeH = 0;
		break;

	case BCAP_PRESET_MJPEG :				// MJPEG
		pConfig->VideoCodec	= FOURCC_MJPG;
		pConfig->VideoQuality = 80;
		break;
	case BCAP_PRESET_MJPEG_HIGH_QUALITY :	// MJPEG with High qulaity audio & video
		pConfig->VideoCodec	= FOURCC_MJPG;
		pConfig->VideoQuality = 90;
		pConfig->AudioSampleRate = 44100;
		break;

	case BCAP_PRESET_YOUTUBE :
		pConfig->VideoQuality = 70;
		pConfig->VideoSizeW = 320;			// 320x240
		pConfig->VideoSizeH = 0;
		pConfig->VideoFPS = 24;
		pConfig->AudioChannels = 1;			// mono
		pConfig->AudioSampleRate = 22050;	// 22k
		break;

	case BCAP_PRESET_YOUTUBE_HIGH_QUALITY :
		pConfig->VideoSizeW = 480;			// 480x360
		pConfig->VideoSizeH = 0;
		pConfig->VideoFPS = 24;
		pConfig->AudioChannels = 1;			// mono
		pConfig->AudioSampleRate = 22050;	// 22k
		break;


	case BCAP_PRESET_NAVER_BLOG :
	case BCAP_PRESET_DAUM_TVPOT :
		pConfig->VideoSizeW = 512;			// 512x384
		pConfig->VideoSizeH = 0;
		pConfig->VideoFPS = 24;
		pConfig->AudioSampleRate = 22050;	// 22k
		break;

	case BCAP_PRESET_MNCAST :
		pConfig->VideoSizeW = 500;			// 500x374
		pConfig->VideoSizeH = 0;
		break;

	default :								// error
		ASSERT(0);
		return FALSE;
	}
	return TRUE;
}


#endif // _BANDICAP_NO_UTIL_CLASS

#endif // _BANDICAP_H
