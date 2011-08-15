#pragma once

#define KEY_WAS_DOWN_MASK 0x80
#define KEY_IS_DOWN_MASK  0x01

#define MOUSE_LEFT_BUTTON   0x01
#define MOUSE_MIDDLE_BUTTON 0x02
#define MOUSE_RIGHT_BUTTON  0x04
#define MOUSE_WHEEL         0x08

// Returns the string for the given D3DFORMAT.
//       bWithPrefix determines whether the string should include the "D3DFMT_"
LPCWSTR DXUTD3DFormatToString(D3DFORMAT format, bool bWithPrefix);

// Debug printing support
// See dxerr.h for more debug printing support
void DXUTOutputDebugStringW(LPCWSTR strMsg, ...);
void DXUTOutputDebugStringA(LPCSTR strMsg, ...);
HRESULT WINAPI DXUTTrace(const CHAR* strFile, DWORD dwLine, HRESULT hr, const WCHAR* strMsg, bool bPopMsgBox);

#ifdef UNICODE
#define DXUTOutputDebugString DXUTOutputDebugStringW
#else
#define DXUTOutputDebugString DXUTOutputDebugStringA
#endif

// These macros are very similar to dxerr's but it special cases the HRESULT defined
// by DXUT to pop better message boxes. 
#if defined(DEBUG) || defined(_DEBUG)
#define DXUT_ERR(str,hr)           DXUTTrace(__FILE__, (DWORD)__LINE__, hr, str, false)
#define DXUT_ERR_MSGBOX(str,hr)    DXUTTrace(__FILE__, (DWORD)__LINE__, hr, str, true)
#define DXUTTRACE                  DXUTOutputDebugString
#else
#define DXUT_ERR(str,hr)           (hr)
#define DXUT_ERR_MSGBOX(str,hr)    (hr)
#define DXUTTRACE                  (__noop)
#endif



// Direct3D9 dynamic linking support -- calls top-level D3D9 APIs with graceful
// failure if APIs are not present.


IDirect3D9 * WINAPI DXUT_Dynamic_Direct3DCreate9(UINT SDKVersion);


// Multimon handling to support OSes with or without multimon API support.  
// Purposely avoiding the use of multimon.h so DXUT.lib doesn't require 
// COMPILE_MULTIMON_STUBS and cause complication with MFC or other users of multimon.h

#ifndef MONITOR_DEFAULTTOPRIMARY
    #define MONITORINFOF_PRIMARY        0x00000001
    #define MONITOR_DEFAULTTONULL       0x00000000
    #define MONITOR_DEFAULTTOPRIMARY    0x00000001
    #define MONITOR_DEFAULTTONEAREST    0x00000002
    typedef struct tagMONITORINFO
    {
        DWORD   cbSize;
        RECT    rcMonitor;
        RECT    rcWork;
        DWORD   dwFlags;
    } MONITORINFO, *LPMONITORINFO;
    typedef struct tagMONITORINFOEXW : public tagMONITORINFO
    {
        WCHAR       szDevice[CCHDEVICENAME];
    } MONITORINFOEXW, *LPMONITORINFOEXW;
    typedef MONITORINFOEXW MONITORINFOEX;
    typedef LPMONITORINFOEXW LPMONITORINFOEX;
#endif

HMONITOR DXUTMonitorFromWindow(HWND hWnd, DWORD dwFlags);
BOOL     DXUTGetMonitorInfo(HMONITOR hMonitor, LPMONITORINFO lpMonitorInfo);