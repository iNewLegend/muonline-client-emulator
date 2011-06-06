
#include "dxstdafx.h"
#undef min // use __min instead
#undef max // use __max instead


// Returns the string for the given D3DFORMAT.

LPCWSTR DXUTD3DFormatToString(D3DFORMAT format, bool bWithPrefix)
{
    WCHAR* pstr = NULL;
    switch(format)
    {
    case D3DFMT_UNKNOWN:         pstr = L"D3DFMT_UNKNOWN"; break;
    case D3DFMT_R8G8B8:          pstr = L"D3DFMT_R8G8B8"; break;
    case D3DFMT_A8R8G8B8:        pstr = L"D3DFMT_A8R8G8B8"; break;
    case D3DFMT_X8R8G8B8:        pstr = L"D3DFMT_X8R8G8B8"; break;
    case D3DFMT_R5G6B5:          pstr = L"D3DFMT_R5G6B5"; break;
    case D3DFMT_X1R5G5B5:        pstr = L"D3DFMT_X1R5G5B5"; break;
    case D3DFMT_A1R5G5B5:        pstr = L"D3DFMT_A1R5G5B5"; break;
    case D3DFMT_A4R4G4B4:        pstr = L"D3DFMT_A4R4G4B4"; break;
    case D3DFMT_R3G3B2:          pstr = L"D3DFMT_R3G3B2"; break;
    case D3DFMT_A8:              pstr = L"D3DFMT_A8"; break;
    case D3DFMT_A8R3G3B2:        pstr = L"D3DFMT_A8R3G3B2"; break;
    case D3DFMT_X4R4G4B4:        pstr = L"D3DFMT_X4R4G4B4"; break;
    case D3DFMT_A2B10G10R10:     pstr = L"D3DFMT_A2B10G10R10"; break;
    case D3DFMT_A8B8G8R8:        pstr = L"D3DFMT_A8B8G8R8"; break;
    case D3DFMT_X8B8G8R8:        pstr = L"D3DFMT_X8B8G8R8"; break;
    case D3DFMT_G16R16:          pstr = L"D3DFMT_G16R16"; break;
    case D3DFMT_A2R10G10B10:     pstr = L"D3DFMT_A2R10G10B10"; break;
    case D3DFMT_A16B16G16R16:    pstr = L"D3DFMT_A16B16G16R16"; break;
    case D3DFMT_A8P8:            pstr = L"D3DFMT_A8P8"; break;
    case D3DFMT_P8:              pstr = L"D3DFMT_P8"; break;
    case D3DFMT_L8:              pstr = L"D3DFMT_L8"; break;
    case D3DFMT_A8L8:            pstr = L"D3DFMT_A8L8"; break;
    case D3DFMT_A4L4:            pstr = L"D3DFMT_A4L4"; break;
    case D3DFMT_V8U8:            pstr = L"D3DFMT_V8U8"; break;
    case D3DFMT_L6V5U5:          pstr = L"D3DFMT_L6V5U5"; break;
    case D3DFMT_X8L8V8U8:        pstr = L"D3DFMT_X8L8V8U8"; break;
    case D3DFMT_Q8W8V8U8:        pstr = L"D3DFMT_Q8W8V8U8"; break;
    case D3DFMT_V16U16:          pstr = L"D3DFMT_V16U16"; break;
    case D3DFMT_A2W10V10U10:     pstr = L"D3DFMT_A2W10V10U10"; break;
    case D3DFMT_UYVY:            pstr = L"D3DFMT_UYVY"; break;
    case D3DFMT_YUY2:            pstr = L"D3DFMT_YUY2"; break;
    case D3DFMT_DXT1:            pstr = L"D3DFMT_DXT1"; break;
    case D3DFMT_DXT2:            pstr = L"D3DFMT_DXT2"; break;
    case D3DFMT_DXT3:            pstr = L"D3DFMT_DXT3"; break;
    case D3DFMT_DXT4:            pstr = L"D3DFMT_DXT4"; break;
    case D3DFMT_DXT5:            pstr = L"D3DFMT_DXT5"; break;
    case D3DFMT_D16_LOCKABLE:    pstr = L"D3DFMT_D16_LOCKABLE"; break;
    case D3DFMT_D32:             pstr = L"D3DFMT_D32"; break;
    case D3DFMT_D15S1:           pstr = L"D3DFMT_D15S1"; break;
    case D3DFMT_D24S8:           pstr = L"D3DFMT_D24S8"; break;
    case D3DFMT_D24X8:           pstr = L"D3DFMT_D24X8"; break;
    case D3DFMT_D24X4S4:         pstr = L"D3DFMT_D24X4S4"; break;
    case D3DFMT_D16:             pstr = L"D3DFMT_D16"; break;
    case D3DFMT_L16:             pstr = L"D3DFMT_L16"; break;
    case D3DFMT_VERTEXDATA:      pstr = L"D3DFMT_VERTEXDATA"; break;
    case D3DFMT_INDEX16:         pstr = L"D3DFMT_INDEX16"; break;
    case D3DFMT_INDEX32:         pstr = L"D3DFMT_INDEX32"; break;
    case D3DFMT_Q16W16V16U16:    pstr = L"D3DFMT_Q16W16V16U16"; break;
    case D3DFMT_MULTI2_ARGB8:    pstr = L"D3DFMT_MULTI2_ARGB8"; break;
    case D3DFMT_R16F:            pstr = L"D3DFMT_R16F"; break;
    case D3DFMT_G16R16F:         pstr = L"D3DFMT_G16R16F"; break;
    case D3DFMT_A16B16G16R16F:   pstr = L"D3DFMT_A16B16G16R16F"; break;
    case D3DFMT_R32F:            pstr = L"D3DFMT_R32F"; break;
    case D3DFMT_G32R32F:         pstr = L"D3DFMT_G32R32F"; break;
    case D3DFMT_A32B32G32R32F:   pstr = L"D3DFMT_A32B32G32R32F"; break;
    case D3DFMT_CxV8U8:          pstr = L"D3DFMT_CxV8U8"; break;
    default:                     pstr = L"Unknown format"; break;
    }
    if(bWithPrefix || wcsstr(pstr, L"D3DFMT_")== NULL)
        return pstr;
    else
        return pstr + lstrlen(L"D3DFMT_");
}




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
typedef INT         (WINAPI * LPD3DPERF_BEGINEVENT)(D3DCOLOR, LPCWSTR);
typedef INT         (WINAPI * LPD3DPERF_ENDEVENT)(void);
typedef VOID        (WINAPI * LPD3DPERF_SETMARKER)(D3DCOLOR, LPCWSTR);
typedef VOID        (WINAPI * LPD3DPERF_SETREGION)(D3DCOLOR, LPCWSTR);
typedef BOOL        (WINAPI * LPD3DPERF_QUERYREPEATFRAME)(void);
typedef VOID        (WINAPI * LPD3DPERF_SETOPTIONS)(DWORD dwOptions);
typedef DWORD       (WINAPI * LPD3DPERF_GETSTATUS)(void);

// Module and function pointers
static HMODULE s_hModD3D9 = NULL;
static LPDIRECT3DCREATE9 s_DynamicDirect3DCreate9 = NULL;
static LPD3DPERF_BEGINEVENT s_DynamicD3DPERF_BeginEvent = NULL;
static LPD3DPERF_ENDEVENT s_DynamicD3DPERF_EndEvent = NULL;
static LPD3DPERF_SETMARKER s_DynamicD3DPERF_SetMarker = NULL;
static LPD3DPERF_SETREGION s_DynamicD3DPERF_SetRegion = NULL;
static LPD3DPERF_QUERYREPEATFRAME s_DynamicD3DPERF_QueryRepeatFrame = NULL;
static LPD3DPERF_SETOPTIONS s_DynamicD3DPERF_SetOptions = NULL;
static LPD3DPERF_GETSTATUS s_DynamicD3DPERF_GetStatus = NULL;

// Ensure function pointers are initialized
static bool DXUT_EnsureD3DAPIs(void)
{
    // If module is non-NULL, this function has already been called.  Note
    // that this doesn't guarantee that all D3D9 procaddresses were found.
    if(s_hModD3D9 != NULL)
        return true;

    // This may fail if DirectX 9 isn't installed
    WCHAR wszPath[MAX_PATH+1];
    if(!::GetSystemDirectory(wszPath, MAX_PATH+1))
        return false;
    StringCchCat(wszPath, MAX_PATH, L"\\d3d9.dll");
    s_hModD3D9 = LoadLibrary(wszPath);
    if(s_hModD3D9 == NULL) 
        return false;
    s_DynamicDirect3DCreate9 = (LPDIRECT3DCREATE9)GetProcAddress(s_hModD3D9, "Direct3DCreate9");
    s_DynamicD3DPERF_BeginEvent = (LPD3DPERF_BEGINEVENT)GetProcAddress(s_hModD3D9, "D3DPERF_BeginEvent");
    s_DynamicD3DPERF_EndEvent = (LPD3DPERF_ENDEVENT)GetProcAddress(s_hModD3D9, "D3DPERF_EndEvent");
    s_DynamicD3DPERF_SetMarker = (LPD3DPERF_SETMARKER)GetProcAddress(s_hModD3D9, "D3DPERF_SetMarker");
    s_DynamicD3DPERF_SetRegion = (LPD3DPERF_SETREGION)GetProcAddress(s_hModD3D9, "D3DPERF_SetRegion");
    s_DynamicD3DPERF_QueryRepeatFrame = (LPD3DPERF_QUERYREPEATFRAME)GetProcAddress(s_hModD3D9, "D3DPERF_QueryRepeatFrame");
    s_DynamicD3DPERF_SetOptions = (LPD3DPERF_SETOPTIONS)GetProcAddress(s_hModD3D9, "D3DPERF_SetOptions");
    s_DynamicD3DPERF_GetStatus = (LPD3DPERF_GETSTATUS)GetProcAddress(s_hModD3D9, "D3DPERF_GetStatus");
    return true;
}

IDirect3D9 * WINAPI DXUT_Dynamic_Direct3DCreate9(UINT SDKVersion) 
{
    if(DXUT_EnsureD3DAPIs() && s_DynamicDirect3DCreate9 != NULL)
        return s_DynamicDirect3DCreate9(SDKVersion);
    else
        return NULL;
}


WCHAR* DXUTTraceD3DDECLTYPEtoString(BYTE t)
{
    switch(t)
    {
        case D3DDECLTYPE_FLOAT1: return L"D3DDECLTYPE_FLOAT1";
        case D3DDECLTYPE_FLOAT2: return L"D3DDECLTYPE_FLOAT2";
        case D3DDECLTYPE_FLOAT3: return L"D3DDECLTYPE_FLOAT3";
        case D3DDECLTYPE_FLOAT4: return L"D3DDECLTYPE_FLOAT4";
        case D3DDECLTYPE_D3DCOLOR: return L"D3DDECLTYPE_D3DCOLOR";
        case D3DDECLTYPE_UBYTE4: return L"D3DDECLTYPE_UBYTE4";
        case D3DDECLTYPE_SHORT2: return L"D3DDECLTYPE_SHORT2";
        case D3DDECLTYPE_SHORT4: return L"D3DDECLTYPE_SHORT4";
        case D3DDECLTYPE_UBYTE4N: return L"D3DDECLTYPE_UBYTE4N";
        case D3DDECLTYPE_SHORT2N: return L"D3DDECLTYPE_SHORT2N";
        case D3DDECLTYPE_SHORT4N: return L"D3DDECLTYPE_SHORT4N";
        case D3DDECLTYPE_USHORT2N: return L"D3DDECLTYPE_USHORT2N";
        case D3DDECLTYPE_USHORT4N: return L"D3DDECLTYPE_USHORT4N";
        case D3DDECLTYPE_UDEC3: return L"D3DDECLTYPE_UDEC3";
        case D3DDECLTYPE_DEC3N: return L"D3DDECLTYPE_DEC3N";
        case D3DDECLTYPE_FLOAT16_2: return L"D3DDECLTYPE_FLOAT16_2";
        case D3DDECLTYPE_FLOAT16_4: return L"D3DDECLTYPE_FLOAT16_4";
        case D3DDECLTYPE_UNUSED: return L"D3DDECLTYPE_UNUSED";
        default: return L"D3DDECLTYPE Unknown";
    }
}

WCHAR* DXUTTraceD3DDECLMETHODtoString(BYTE m)
{
    switch(m)
    {
        case D3DDECLMETHOD_DEFAULT: return L"D3DDECLMETHOD_DEFAULT";
        case D3DDECLMETHOD_PARTIALU: return L"D3DDECLMETHOD_PARTIALU";
        case D3DDECLMETHOD_PARTIALV: return L"D3DDECLMETHOD_PARTIALV";
        case D3DDECLMETHOD_CROSSUV: return L"D3DDECLMETHOD_CROSSUV";
        case D3DDECLMETHOD_UV: return L"D3DDECLMETHOD_UV";
        case D3DDECLMETHOD_LOOKUP: return L"D3DDECLMETHOD_LOOKUP";
        case D3DDECLMETHOD_LOOKUPPRESAMPLED: return L"D3DDECLMETHOD_LOOKUPPRESAMPLED";
        default: return L"D3DDECLMETHOD Unknown";
    }
}

WCHAR* DXUTTraceD3DDECLUSAGEtoString(BYTE u)
{
    switch(u)
    {
        case D3DDECLUSAGE_POSITION: return L"D3DDECLUSAGE_POSITION";
        case D3DDECLUSAGE_BLENDWEIGHT: return L"D3DDECLUSAGE_BLENDWEIGHT";
        case D3DDECLUSAGE_BLENDINDICES: return L"D3DDECLUSAGE_BLENDINDICES";
        case D3DDECLUSAGE_NORMAL: return L"D3DDECLUSAGE_NORMAL";
        case D3DDECLUSAGE_PSIZE: return L"D3DDECLUSAGE_PSIZE";
        case D3DDECLUSAGE_TEXCOORD: return L"D3DDECLUSAGE_TEXCOORD";
        case D3DDECLUSAGE_TANGENT: return L"D3DDECLUSAGE_TANGENT";
        case D3DDECLUSAGE_BINORMAL: return L"D3DDECLUSAGE_BINORMAL";
        case D3DDECLUSAGE_TESSFACTOR: return L"D3DDECLUSAGE_TESSFACTOR";
        case D3DDECLUSAGE_POSITIONT: return L"D3DDECLUSAGE_POSITIONT";
        case D3DDECLUSAGE_COLOR: return L"D3DDECLUSAGE_COLOR";
        case D3DDECLUSAGE_FOG: return L"D3DDECLUSAGE_FOG";
        case D3DDECLUSAGE_DEPTH: return L"D3DDECLUSAGE_DEPTH";
        case D3DDECLUSAGE_SAMPLE: return L"D3DDECLUSAGE_SAMPLE";
        default: return L"D3DDECLUSAGE Unknown";
    }
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

typedef DWORD (WINAPI* LPXINPUTGETSTATE)(DWORD dwUserIndex, XINPUT_STATE* pState);
typedef DWORD (WINAPI* LPXINPUTSETSTATE)(DWORD dwUserIndex, XINPUT_VIBRATION* pVibration);
typedef DWORD (WINAPI* LPXINPUTGETCAPABILITIES)(DWORD dwUserIndex, DWORD dwFlags, XINPUT_CAPABILITIES* pCapabilities);
typedef void  (WINAPI* LPXINPUTENABLE)(BOOL bEnable);