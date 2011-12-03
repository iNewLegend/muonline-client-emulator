#include "stdafx.h"
#include "ZBandiCapturer.h"
#include "FileInfo.h"
#include <sys/stat.h>

///////////////////////////////////////////////////////////////////////

static BCAP_CONFIG g_nConfig;

BCAP_PRESET a_Resolution[3] = {BCAP_PRESET_320x240, BCAP_PRESET_640x480, BCAP_PRESET_DEFAULT};
enum BCAP_FILESIZE		// ����ũ��(�뷮����)
{
	BCAP_FILESIZE_100MB,
	BCAP_FILESIZE_300MB,
	BCAP_FILESIZE_500MB,
	BCAP_FILESIZE_UNLIMITED,
};
BCAP_FILESIZE m_eFileSize;

void SetBandiCaptureConfig(int iResolution)
{// ������ "�ɼ�"���� �����ϱ� ���� �Լ�
	if(iResolution > 2) // ������ �ʴ� ���� ���ý�...�ӽ���
		iResolution = 0;
	BCapConfigPreset(&g_nConfig, a_Resolution[iResolution]);	// ������ ����
}
void SetBandiCaptureFileSize(int iFileSize)
{// ������ "�ɼ�"���� �����ϱ� ���� �Լ�
	if(iFileSize > 3) // ������ �ʴ� ���� ���ý�...�ӽ���
		iFileSize = 0;
	m_eFileSize = (BCAP_FILESIZE)iFileSize;
}

bool ZBandiCapturer::Init(HWND nHwnd, LPDIRECT3DDEVICE9 pDevice)
{
	m_hHandle = nHwnd;
	m_pDevice = pDevice;

	if(FAILED(m_bandiCaptureLibrary.Create(BANDICAP_DLL_FILE_NAME)))
	{
		_ASSERT(0);
		return false;
	}

	if(FAILED(m_bandiCaptureLibrary.Verify("MAIET-GUNZ-20080916", "d25f910a")))
	{
		ASSERT(0);
		return false;
	}

	m_bInited = true;
	m_fToggleFileSize = 0;
	m_lFileSize = 0;

	return true;
}

bool ZBandiCapturer::Start()
{
	if(m_bandiCaptureLibrary.IsCapturing()==FALSE)
	{
		BCAP_CONFIG cfg = GetConfig();

		//// ȭ��/����
		//BCapConfigPreset(&cfg, BCAP_PRESET_MJPEG_HIGH_QUALITY);	// ������ ����
		m_bandiCaptureLibrary.CheckConfig(&cfg);	// �߸��� ���� �ٷ� ���
		m_bandiCaptureLibrary.SetConfig(&cfg);		// ���� ����
		m_bandiCaptureLibrary.SetMinMaxFPS(15, 999);	// �ּ�, �ִ� ������ ����

		// ����ð��� �̿��ؼ� ���ϸ��� ����ϴ�. ��θ��� ������ ������� "�� ����" ������ ������ �ּ���.
		TCHAR	folderName[MAX_PATH];
		//TCHAR	pathName[MAX_PATH];

		GetFolder(folderName);
		m_bandiCaptureLibrary.MakePathnameByDate(folderName, _T("Gunz"), m_pathName, MAX_PATH);

		// ĸó�� �����մϴ�.
		HRESULT hr = m_bandiCaptureLibrary.Start(m_pathName, m_hHandle, BCAP_MODE_D3D9_SCALE, (LONG_PTR)m_pDevice);
		if(FAILED(hr)) 
		{
			_ASSERT(0);
			return false;
		}
	}
	else
	{
		m_bandiCaptureLibrary.Stop();
	}

	return true;
}


void ZBandiCapturer::Stop()
{
	if(m_bandiCaptureLibrary.IsCapturing())
	{
		m_bandiCaptureLibrary.Stop();
		m_fToggleFileSize = 0;
		m_lFileSize = 0;
	}
}

bool ZBandiCapturer::IsCapturing()
{
	return (m_bandiCaptureLibrary.IsCapturing() == TRUE);
}

void ZBandiCapturer::DrawCapture(MDrawContext* pDC)
{
	if(m_bandiCaptureLibrary.IsCapturing())
	{
		m_bandiCaptureLibrary.Work((LONG_PTR)m_pDevice);

		// ĸó������ �˸��� �޽��� ���
		OnDrawCapture(pDC);
	}

}

void ZBandiCapturer::OnDrawCapture(MDrawContext* pDC)
{
	MFont *pFont=MFontManager::Get("FONTa10_O2Wht");

	pDC->SetFont(pFont);
	pDC->SetColor(MCOLOR(0xFFFF0000));

	char szTemp[64];
	// ��ȭ�ð�ǥ��
	sprintf(szTemp, "Video REC %.3f(sec)", (float)m_bandiCaptureLibrary.GetCaptureTime()/1000);
	pDC->Text(RGetScreenWidth() - 180, 15, szTemp);

	switch(m_eFileSize)
	{
	case BCAP_FILESIZE_100MB:		{	if(m_lFileSize > 100000000)	Stop();		sprintf(szTemp, "[100MB]");		}break;
	case BCAP_FILESIZE_300MB:		{	if(m_lFileSize > 300000000)	Stop();		sprintf(szTemp, "[300MB]");		}break;
	case BCAP_FILESIZE_500MB:		{	if(m_lFileSize > 500000000)	Stop();		sprintf(szTemp, "[500MB]");		}break;
	case BCAP_FILESIZE_UNLIMITED:	{	sprintf(szTemp, "[Unlimited]");		}break;
	}
	if(m_bandiCaptureLibrary.GetCaptureTime() - m_fToggleFileSize > 1000)	// ��ȭ ����ũ��
	{ // 1�ʿ� �ѹ����� ����
		m_lFileSize = get_file_length(m_pathName);
		m_fToggleFileSize = m_bandiCaptureLibrary.GetCaptureTime();
	}
	// ��ȭ ����ũ�� ǥ��
	sprintf(szTemp, "%s %.3fMB", szTemp, (float)m_lFileSize/1000000);
	pDC->Text(RGetScreenWidth() - 180, 30, szTemp);
}

void ZBandiCapturer::ToggleStart()
{
	if (IsCapturing() == false)
	{
		Start();
	}
	else
	{
		Stop();
	}
}

BCAP_CONFIG ZBandiCapturer::GetConfig()
{
	//BCAP_CONFIG cfg;

	g_nConfig.VideoCodec = FOURCC_MJPG;
	g_nConfig.VideoFPS = 24;
	return g_nConfig;
}

void ZBandiCapturer::GetFolder( TCHAR* pPath )
{
	char foldername[_MAX_PATH] = "c:\\";

	if(GetMyDocumentsPath(pPath)) 
	{
		strcpy(foldername, pPath);
		strcat(foldername,GUNZ_FOLDER);
		CreatePath( foldername );
		strcat(foldername, "/Video");
		CreatePath( foldername );
		strcat(foldername, "/");
	}
	strcpy(pPath, foldername);
}
void ZBandiCapturer::SetResolution(int iResolution)
{
	m_iResolution = iResolution;
}
int ZBandiCapturer::GetResolution()
{
	return m_iResolution;
}

int ZBandiCapturer::get_file_length( char *fn )
{
	struct stat sbuf;
	int   len;

	if( stat( fn, &sbuf ) )
	{
		printf("File %s stat error\n", fn );
		len = 0;
	}
	else
	{
		printf("File size = %d\n", sbuf.st_size );

		len = sbuf.st_size;
	}
	return len;
}