
#include "dxstdafx.h"
#undef min // use __min instead
#undef max // use __max instead


// Outputs to the debug stream a formatted Unicode string with a variable-argument list.

VOID DXUTOutputDebugStringW(LPCWSTR strMsg, ...)
{
#if defined(DEBUG) || defined(_DEBUG)
    WCHAR strBuffer[512];

    va_list args;
    va_start(args, strMsg);
    StringCchVPrintfW(strBuffer, 512, strMsg, args);
    strBuffer[511] = L'\0';
    va_end(args);

    OutputDebugString(strBuffer);
#else
    UNREFERENCED_PARAMETER(strMsg);
#endif
}

// Outputs to the debug stream a formatted MBCS string with a variable-argument list.

VOID DXUTOutputDebugStringA(LPCSTR strMsg, ...)
{
#if defined(DEBUG) || defined(_DEBUG)
    CHAR strBuffer[512];

    va_list args;
    va_start(args, strMsg);
    StringCchVPrintfA(strBuffer, 512, strMsg, args);
    strBuffer[511] = '\0';
    va_end(args);

    OutputDebugStringA(strBuffer);
#else
    UNREFERENCED_PARAMETER(strMsg);
#endif
}

// Direct3D9 dynamic linking support -- calls top-level D3D9 APIs with graceful
// failure if APIs are not present.

// Function prototypes
typedef IDirect3D9* (WINAPI * LPDIRECT3DCREATE9) (UINT);
IDirect3D9 * WINAPI DXUT_Dynamic_Direct3DCreate9(UINT SDKVersion) 
{
	static HMODULE s_hModD3D9 = NULL;
	static LPDIRECT3DCREATE9 s_DynamicDirect3DCreate9 = NULL;
	if(s_hModD3D9 == NULL)
	{
		// This may fail if DirectX 9 isn't installed
		WCHAR wszPath[MAX_PATH+1];
		if(::GetSystemDirectory(wszPath, MAX_PATH+1))
		{
			StringCchCat(wszPath, MAX_PATH, L"\\d3d9.dll");
			s_hModD3D9 = LoadLibrary(wszPath);
		}
	}

	if(s_hModD3D9&&s_DynamicDirect3DCreate9==NULL) 
	{
		s_DynamicDirect3DCreate9 = (LPDIRECT3DCREATE9)GetProcAddress(s_hModD3D9, "Direct3DCreate9");
	}

    if(s_DynamicDirect3DCreate9)
        return s_DynamicDirect3DCreate9(SDKVersion);
    else
        return NULL;
}

// Multimon API handling for OSes with or without multimon API support

#define DXUT_PRIMARY_MONITOR ((HMONITOR)0x12340042)
typedef HMONITOR (WINAPI* LPMONITORFROMWINDOW)(HWND, DWORD);
typedef BOOL     (WINAPI* LPGETMONITORINFO)(HMONITOR, LPMONITORINFO);

BOOL DXUTGetMonitorInfo(HMONITOR hMonitor, LPMONITORINFO lpMonitorInfo)
{
    static bool s_bInited = false;
    static LPGETMONITORINFO s_pFnGetMonitorInfo = NULL;
    if(!s_bInited)        
    {
        s_bInited = true;
        HMODULE hUser32 = GetModuleHandle(L"USER32");
        if (hUser32) 
        {
            OSVERSIONINFOA osvi = {0}; osvi.dwOSVersionInfoSize = sizeof(osvi); GetVersionExA((OSVERSIONINFOA*)&osvi);
            bool bNT = (VER_PLATFORM_WIN32_NT == osvi.dwPlatformId);    
            s_pFnGetMonitorInfo = (LPGETMONITORINFO) (bNT ? GetProcAddress(hUser32,"GetMonitorInfoW") : GetProcAddress(hUser32,"GetMonitorInfoA"));
        }
    }

    if(s_pFnGetMonitorInfo) 
        return s_pFnGetMonitorInfo(hMonitor, lpMonitorInfo);

    RECT rcWork;
    if ((hMonitor == DXUT_PRIMARY_MONITOR) && lpMonitorInfo && (lpMonitorInfo->cbSize >= sizeof(MONITORINFO)) && SystemParametersInfoA(SPI_GETWORKAREA, 0, &rcWork, 0))
    {
        lpMonitorInfo->rcMonitor.left = 0;
        lpMonitorInfo->rcMonitor.top  = 0;
        lpMonitorInfo->rcMonitor.right  = GetSystemMetrics(SM_CXSCREEN);
        lpMonitorInfo->rcMonitor.bottom = GetSystemMetrics(SM_CYSCREEN);
        lpMonitorInfo->rcWork = rcWork;
        lpMonitorInfo->dwFlags = MONITORINFOF_PRIMARY;
        return TRUE;
    }
    return FALSE;
}


HMONITOR DXUTMonitorFromWindow(HWND hWnd, DWORD dwFlags)
{
    static bool s_bInited = false;
    static LPMONITORFROMWINDOW s_pFnGetMonitorFronWindow = NULL;
    if(!s_bInited)        
    {
        s_bInited = true;
        HMODULE hUser32 = GetModuleHandle(L"USER32");
        if (hUser32) s_pFnGetMonitorFronWindow = (LPMONITORFROMWINDOW) GetProcAddress(hUser32,"MonitorFromWindow");
    }

    if(s_pFnGetMonitorFronWindow) 
        return s_pFnGetMonitorFronWindow(hWnd, dwFlags);
    if (dwFlags & (MONITOR_DEFAULTTOPRIMARY | MONITOR_DEFAULTTONEAREST))
        return DXUT_PRIMARY_MONITOR;
    return NULL;
}