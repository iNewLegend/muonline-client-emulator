#include "dxstdafx.h"
#include "D3D9RenderSystem.h"

#undef min // use __min instead inside this source file
#undef max // use __max instead inside this source file


DXUTState::DXUTState()
{
	Create();
}

DXUTState::~DXUTState()
{
	Destroy();
}

void DXUTState::Create()
{
	// Make sure these are created before DXUTState so they 
	// destroyed last because DXUTState cleanup needs them

	ZeroMemory(&m_state, sizeof(STATE)); 
	m_state.m_OverrideAdapterOrdinal = -1; 
	m_state.m_AutoChangeAdapter = true;
	m_state.m_Active = true;
}

void DXUTState::Destroy()
{
	DXUTShutdown();
}


// Global state class

DXUTState& GetDXUTState()
{
    // Using an accessor function gives control of the construction order
    static DXUTState state;
    return state;
}


// Internal functions forward declarations

typedef IDirect3D9* (WINAPI* LPDIRECT3DCREATE9)(UINT SDKVersion);

CD3DEnumeration* DXUTPrepareEnumerationObject(bool bEnumerate = false);
void    DXUTBuildOptimalDeviceSettings(DXUTDeviceSettings* pOptimalDeviceSettings, DXUTDeviceSettings* pDeviceSettingsIn, DXUTMatchOptions* pMatchOptions);
bool    DXUTDoesDeviceComboMatchPreserveOptions(CD3DEnumDeviceSettingsCombo* pDeviceSettingsCombo, DXUTDeviceSettings* pDeviceSettingsIn, DXUTMatchOptions* pMatchOptions);
float   DXUTRankDeviceCombo(CD3DEnumDeviceSettingsCombo* pDeviceSettingsCombo, DXUTDeviceSettings* pDeviceSettingsIn, D3DDISPLAYMODE* pAdapterDesktopDisplayMode);
void    DXUTBuildValidDeviceSettings(DXUTDeviceSettings* pDeviceSettings, CD3DEnumDeviceSettingsCombo* pBestDeviceSettingsCombo, DXUTDeviceSettings* pDeviceSettingsIn, DXUTMatchOptions* pMatchOptions);
HRESULT DXUTFindValidResolution(CD3DEnumDeviceSettingsCombo* pBestDeviceSettingsCombo, D3DDISPLAYMODE displayModeIn, D3DDISPLAYMODE* pBestDisplayMode);
HRESULT DXUTFindAdapterFormat(UINT AdapterOrdinal, D3DDEVTYPE DeviceType, D3DFORMAT BackBufferFormat, BOOL Windowed, D3DFORMAT* pAdapterFormat);

void    DXUTUpdateDeviceSettingsWithOverrides(DXUTDeviceSettings* pNewDeviceSettings);
HRESULT DXUTCreate3DEnvironment(IDirect3DDevice9* pd3dDeviceFromApp);

void    DXUTCleanup3DEnvironment(bool bReleaseSettings = true);

void    DXUTUpdateDeviceStats(D3DDEVTYPE DeviceType, DWORD BehaviorFlags, D3DADAPTER_IDENTIFIER9* pAdapterIdentifier);
void    DXUTUpdateStaticFrameStats();
bool    DXUTIsNextArg(WCHAR*& strCmdLine, WCHAR* strArg);
bool    DXUTGetCmdParam(WCHAR*& strCmdLine, WCHAR* strFlag);



UINT    DXUTColorChannelBits(D3DFORMAT fmt);
UINT    DXUTStencilBits(D3DFORMAT fmt);
UINT    DXUTDepthBits(D3DFORMAT fmt);
HRESULT DXUTGetAdapterOrdinalFromMonitor(HMONITOR hMonitor, UINT* pAdapterOrdinal);
void    DXUTUpdateBackBufferDesc();
void    DXUTSetupCursor();
HRESULT DXUTSetDeviceCursor(IDirect3DDevice9* pd3dDevice, HCURSOR hCursor, bool bAddWatermark);



// Rejects any devices that aren't acceptable by returning false

bool CALLBACK IsDeviceAcceptable(D3DCAPS9* pCaps, D3DFORMAT AdapterFormat, 
								 D3DFORMAT BackBufferFormat, bool bWindowed, void* pUserContext)
{
	// Typically want to skip backbuffer formats that don't support alpha blending
	IDirect3D9* pD3D = DXUTGetD3DObject(); 
	if(FAILED(pD3D->CheckDeviceFormat(pCaps->AdapterOrdinal, pCaps->DeviceType,
		AdapterFormat, D3DUSAGE_QUERY_POSTPIXELSHADER_BLENDING, 
		D3DRTYPE_TEXTURE, BackBufferFormat)))
		return false;

	return true;
}

bool CALLBACK ModifyDeviceSettings(DXUTDeviceSettings* pDeviceSettings, const D3DCAPS9* pCaps, void* pUserContext)
{
	return true;
}


// Optionally parses the command line and sets if default hotkeys are handled
//
//       Possible command line parameters are:
//          -adapter:#              forces app to use this adapter # (fails if the adapter doesn't exist)
//          -windowed               forces app to start windowed
//          -fullscreen             forces app to start full screen
//          -forcehal               forces app to use HAL (fails if HAL doesn't exist)
//          -forceref               forces app to use REF (fails if REF doesn't exist)
//          -forcepurehwvp          forces app to use pure HWVP (fails if device doesn't support it)
//          -forcehwvp              forces app to use HWVP (fails if device doesn't support it)
//          -forceswvp              forces app to use SWVP 
//          -forcevsync:#           if # is 0, forces app to use D3DPRESENT_INTERVAL_IMMEDIATE otherwise force use of D3DPRESENT_INTERVAL_DEFAULT 
//          -width:#                forces app to use # for width. for full screen, it will pick the closest possible supported mode
//          -height:#               forces app to use # for height. for full screen, it will pick the closest possible supported mode
//          -startx:#               forces app to use # for the x coord of the window position for windowed mode
//          -starty:#               forces app to use # for the y coord of the window position for windowed mode
//          -quitafterframe:x       forces app to quit after # frames
//          -noerrormsgboxes        prevents the display of message boxes generated by the framework so the application can be run without user interaction
//          -nostats                prevents the display of the stats
//          -relaunchmce            re-launches the MCE UI after the app exits
//
//      Hotkeys handled by default are:
//          Alt-Enter           toggle between full screen & windowed (hotkey always enabled)
//          ESC                 exit app 
//          F3                  toggle HAL/REF
//          F8                  toggle wire-frame mode
//          Pause               pause time

HRESULT DXUTInit(bool bParseCommandLine)
{
    GetDXUTState().SetDXUTInitCalled(true);

    // Not always needed, but lets the app create GDI dialogs
    InitCommonControls();

    // Increase the accuracy of Sleep() without needing to link to winmm.lib
    WCHAR wszPath[MAX_PATH+1];
    if(GetSystemDirectory(wszPath, MAX_PATH+1))
    {
        StringCchCat(wszPath, MAX_PATH, L"\\winmm.dll");
        HINSTANCE hInstWinMM = LoadLibrary(wszPath);
        if(hInstWinMM) 
        {
            LPTIMEBEGINPERIOD pTimeBeginPeriod = (LPTIMEBEGINPERIOD)GetProcAddress(hInstWinMM, "timeBeginPeriod");
            if(NULL != pTimeBeginPeriod)
                pTimeBeginPeriod(1);

            FreeLibrary(hInstWinMM);
        }
    }

    if(bParseCommandLine)
        DXUTParseCommandLine();

    // Verify D3DX version
    //if(!D3DXCheckVersion(D3D_SDK_VERSION, D3DX_SDK_VERSION))
    //{
    //    DXUTDisplayErrorMessage(DXUTERR_INCORRECTVERSION);
    //    return DXUT_ERR(L"D3DXCheckVersion", DXUTERR_INCORRECTVERSION);
    //}

    // Create a Direct3D object if one has not already been created
    IDirect3D9* pD3D = DXUTGetD3DObject();
    if(pD3D == NULL)
    {
        // This may fail if DirectX 9 isn't installed
        // This may fail if the DirectX headers are out of sync with the installed DirectX DLLs
        pD3D = DXUT_Dynamic_Direct3DCreate9(D3D_SDK_VERSION);
        GetDXUTState().SetD3D(pD3D);
    }

    if(pD3D == NULL)
    {
        // If still NULL, then something went wrong
        DXUTDisplayErrorMessage(DXUTERR_NODIRECT3D);
        return DXUT_ERR(L"Direct3DCreate9", DXUTERR_NODIRECT3D);
    }

    // Reset the timer
    GetGlobalTimer().Reset();

    GetDXUTState().SetDXUTInited(true);

    return S_OK;
}



// Parses the command line for parameters.  See DXUTInit() for list 

void DXUTParseCommandLine()
{
    WCHAR* strCmdLine;
    WCHAR strFlag[MAX_PATH];

    int nNumArgs;
    WCHAR** pstrArgList = CommandLineToArgvW(GetCommandLine(), &nNumArgs);
    for(int iArg=1; iArg<nNumArgs; iArg++)
    {
        strCmdLine = pstrArgList[iArg];

        // Handle flag args
        if(*strCmdLine == L'/' || *strCmdLine == L'-')
        {
            strCmdLine++;

            if(DXUTIsNextArg(strCmdLine, L"adapter"))
            {
                if(DXUTGetCmdParam(strCmdLine, strFlag))
                {
                    int nAdapter = _wtoi(strFlag);
                    GetDXUTState().SetOverrideAdapterOrdinal(nAdapter);
                    continue;
                }
            }

            if(DXUTIsNextArg(strCmdLine, L"windowed"))
            {
                GetDXUTState().SetOverrideWindowed(true);
                continue;
            }

            if(DXUTIsNextArg(strCmdLine, L"fullscreen"))
            {
                GetDXUTState().SetOverrideFullScreen(true);
                continue;
            }

            if(DXUTIsNextArg(strCmdLine, L"width"))
            {
                if(DXUTGetCmdParam(strCmdLine, strFlag))
                {
                    int nWidth = _wtoi(strFlag);
                    GetDXUTState().SetOverrideWidth(nWidth);
                    continue;
                }
            }

            if(DXUTIsNextArg(strCmdLine, L"height"))
            {
                if(DXUTGetCmdParam(strCmdLine, strFlag))
                {
                    int nHeight = _wtoi(strFlag);
                    GetDXUTState().SetOverrideHeight(nHeight);
                continue;
                }
            }

            if(DXUTIsNextArg(strCmdLine, L"nostats"))
            {
                GetDXUTState().SetNoStats(true);
                continue;
            }
        }

        // Unrecognized flag
        StringCchCopy(strFlag, 256, strCmdLine); 
        WCHAR* strSpace = strFlag;
        while (*strSpace && (*strSpace > L' '))
            strSpace++;
        *strSpace = 0;

        DXUTOutputDebugString(L"Unrecognized flag: %s", strFlag);
        strCmdLine += wcslen(strFlag);
    }
}



// Helper function for DXUTParseCommandLine

bool DXUTIsNextArg(WCHAR*& strCmdLine, WCHAR* strArg)
{
    int nArgLen = (int) wcslen(strArg);
    int nCmdLen = (int) wcslen(strCmdLine);

    if(nCmdLen >= nArgLen && 
        _wcsnicmp(strCmdLine, strArg, nArgLen) == 0 && 
        (strCmdLine[nArgLen] == 0 || strCmdLine[nArgLen] == L':'))
    {
        strCmdLine += nArgLen;
        return true;
    }

    return false;
}



// Helper function for DXUTParseCommandLine.  Updates strCmdLine and strFlag 
//      Example: if strCmdLine=="-width:1024 -forceref"
// then after: strCmdLine==" -forceref" and strFlag=="1024"

bool DXUTGetCmdParam(WCHAR*& strCmdLine, WCHAR* strFlag)
{
    if(*strCmdLine == L':')
    {       
        strCmdLine++; // Skip ':'

        // Place NULL terminator in strFlag after current token
        StringCchCopy(strFlag, 256, strCmdLine);
        WCHAR* strSpace = strFlag;
        while (*strSpace && (*strSpace > L' '))
            strSpace++;
        *strSpace = 0;

        // Update strCmdLine
        strCmdLine += wcslen(strFlag);
        return true;
    }
    else
    {
        strFlag[0] = 0;
        return false;
    }
}



// Creates a window with the specified window title, icon, menu, and 
// starting position.  If DXUTInit() has not already been called, it will
// call it with the default parameters.  Instead of calling this, you can 
// call DXUTSetWindow() to use an existing window.  

HRESULT DXUTCreateWindow(WNDPROC pWndProc, const std::wstring& strWindowTitle, HINSTANCE hInstance, 
                          HICON hIcon, HMENU hMenu, int x, int y)
{
    HRESULT hr;

    GetDXUTState().SetWindowCreateCalled(true);

    if(!GetDXUTState().GetDXUTInited()) 
    {
        // If DXUTInit() was already called and failed, then fail.
        // DXUTInit() must first succeed for this function to succeed
        if(GetDXUTState().GetDXUTInitCalled())
            return E_FAIL; 

        // If DXUTInit() hasn't been called, then automatically call it
        // with default params
        hr = DXUTInit();
        if(FAILED(hr))
            return hr;
    }

    if(DXUTGetHWNDFocus() == NULL)
    {
        if(hInstance == NULL) 
            hInstance = (HINSTANCE)GetModuleHandle(NULL);
        GetDXUTState().SetHInstance(hInstance);

        WCHAR szExePath[MAX_PATH];
        GetModuleFileName(NULL, szExePath, MAX_PATH);
        if(hIcon == NULL) // If the icon is NULL, then use the first one found in the exe
            hIcon = ExtractIcon(hInstance, szExePath, 0); 

        // Register the windows class
        WNDCLASS wndClass;
        wndClass.style = CS_DBLCLKS;
        wndClass.lpfnWndProc = pWndProc;
        wndClass.cbClsExtra = 0;
        wndClass.cbWndExtra = 0;
        wndClass.hInstance = hInstance;
        wndClass.hIcon = hIcon;
        wndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
        wndClass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
        wndClass.lpszMenuName = NULL;
        wndClass.lpszClassName = L"Direct3DWindowClass";

        if(!RegisterClass(&wndClass))
        {
            DWORD dwError = GetLastError();
            if(dwError != ERROR_CLASS_ALREADY_EXISTS)
                return DXUT_ERR_MSGBOX(L"RegisterClass", HRESULT_FROM_WIN32(dwError));
        }

        RECT rc;

        // Find the window's initial size, but it might be changed later
        int nDefaultWidth = 640;
        int nDefaultHeight = 480;
        if(GetDXUTState().GetOverrideWidth() != 0)
            nDefaultWidth = GetDXUTState().GetOverrideWidth();
        if(GetDXUTState().GetOverrideHeight() != 0)
            nDefaultHeight = GetDXUTState().GetOverrideHeight();
        SetRect(&rc, 0, 0, nDefaultWidth, nDefaultHeight);        
        AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, (hMenu != NULL) ? true : false);

        // Create the render window
        HWND hWnd = CreateWindowExW(0L, L"Direct3DWindowClass", strWindowTitle.c_str(), WS_OVERLAPPEDWINDOW,
                               x, y, (rc.right-rc.left), (rc.bottom-rc.top), 0,
                               hMenu, hInstance, 0);
        if(hWnd == NULL)
        {
            DWORD dwError = GetLastError();
            return DXUT_ERR_MSGBOX(L"CreateWindow", HRESULT_FROM_WIN32(dwError));
        }

        GetDXUTState().SetWindowCreated(true);
        GetDXUTState().SetHWNDFocus(hWnd);
        GetDXUTState().SetHWNDDeviceFullScreen(hWnd);
        GetDXUTState().SetHWNDDeviceWindowed(hWnd);
    }

    return S_OK;
}



// Sets a previously created window for the framework to use.  If DXUTInit() 
// has not already been called, it will call it with the default parameters.  
// Instead of calling this, you can call DXUTCreateWindow() to create a new window.  

HRESULT DXUTSetWindow(WNDPROC pWndProc, HWND hWndFocus, HWND hWndDeviceFullScreen, HWND hWndDeviceWindowed, bool bHandleMessages)
{
    HRESULT hr;

    GetDXUTState().SetWindowCreateCalled(true);

    // To avoid confusion, we do not allow any HWND to be NULL here.  The
    // caller must pass in valid HWND for all three parameters.  The same
    // HWND may be used for more than one parameter.
    if(hWndFocus == NULL || hWndDeviceFullScreen == NULL || hWndDeviceWindowed == NULL)
        return DXUT_ERR_MSGBOX(L"DXUTSetWindow", E_INVALIDARG);

    // If subclassing the window, set the pointer to the local window procedure
    if(bHandleMessages)
    {
		// 注入消息
        // Switch window procedures
#ifdef _WIN64
        LONG_PTR nResult = SetWindowLongPtr(hWndFocus, GWLP_WNDPROC, (LONG_PTR)pWndProc);
#else
        LONG_PTR nResult = SetWindowLongPtr(hWndFocus, GWLP_WNDPROC, (LONG)(LONG_PTR)pWndProc);
#endif 
		GetDXUTState().SetWndProc(nResult);//ffffff

        DWORD dwError = GetLastError();
        if(nResult == 0)
            return DXUT_ERR_MSGBOX(L"SetWindowLongPtr", HRESULT_FROM_WIN32(dwError));
    }

    if(!GetDXUTState().GetDXUTInited()) 
    {
        // If DXUTInit() was already called and failed, then fail.
        // DXUTInit() must first succeed for this function to succeed
        if(GetDXUTState().GetDXUTInitCalled())
            return E_FAIL; 

        // If DXUTInit() hasn't been called, then automatically call it
        // with default params
        hr = DXUTInit();
        if(FAILED(hr))
            return hr;
    }

    HINSTANCE hInstance = (HINSTANCE) (LONG_PTR) GetWindowLongPtr(hWndFocus, GWLP_HINSTANCE); 
    GetDXUTState().SetHInstance(hInstance);
    GetDXUTState().SetWindowCreated(true);
    GetDXUTState().SetHWNDFocus(hWndFocus);
    GetDXUTState().SetHWNDDeviceFullScreen(hWndDeviceFullScreen);
    GetDXUTState().SetHWNDDeviceWindowed(hWndDeviceWindowed);

    return S_OK;
}

void DXUTCloseWindow()
{
#ifdef _WIN64
	LONG_PTR nResult = SetWindowLongPtr(GetDXUTState().GetHWNDFocus(), GWLP_WNDPROC, (LONG_PTR)GetDXUTState().GetWndProc());
#else
	LONG_PTR nResult = SetWindowLongPtr(GetDXUTState().GetHWNDFocus(), GWLP_WNDPROC, (LONG)(LONG_PTR)GetDXUTState().GetWndProc());
#endif
	DWORD dwError = GetLastError();
	if(nResult == 0)
		DXUT_ERR_MSGBOX(L"SetWindowLongPtr", HRESULT_FROM_WIN32(dwError));
}


// Creates a Direct3D device. If DXUTCreateWindow() or DXUTSetWindow() has not already 
// been called, it will call DXUTCreateWindow() with the default parameters.  
// Instead of calling this, you can call DXUTSetDevice() or DXUTCreateDeviceFromSettings() 

HRESULT DXUTCreateDevice(UINT AdapterOrdinal, bool bWindowed, 
                          int nSuggestedWidth, int nSuggestedHeight)
{
    HRESULT hr;


    GetDXUTState().SetDeviceCreateCalled(true);

    // If DXUTCreateWindow() or DXUTSetWindow() has not already been called, 
    // then call DXUTCreateWindow() with the default parameters.         
    if(!GetDXUTState().GetWindowCreated()) 
    {
        // If DXUTCreateWindow() or DXUTSetWindow() was already called and failed, then fail.
        // DXUTCreateWindow() or DXUTSetWindow() must first succeed for this function to succeed
        if(GetDXUTState().GetWindowCreateCalled())
            return E_FAIL; 

        // If DXUTCreateWindow() or DXUTSetWindow() hasn't been called, then 
        // automatically call DXUTCreateWindow() with default params
        hr = DXUTCreateWindow();
        if(FAILED(hr))
            return hr;
    }

    // Force an enumeration with the new IsDeviceAcceptable callback
    DXUTPrepareEnumerationObject(true);

    DXUTMatchOptions matchOptions;
    matchOptions.eAdapterOrdinal     = DXUTMT_PRESERVE_INPUT;
    matchOptions.eDeviceType         = DXUTMT_IGNORE_INPUT;
    matchOptions.eWindowed           = DXUTMT_PRESERVE_INPUT;
    matchOptions.eAdapterFormat      = DXUTMT_IGNORE_INPUT;
    matchOptions.eVertexProcessing   = DXUTMT_IGNORE_INPUT;
    if(bWindowed || (nSuggestedWidth != 0 && nSuggestedHeight != 0))
        matchOptions.eResolution     = DXUTMT_CLOSEST_TO_INPUT;
    else
        matchOptions.eResolution     = DXUTMT_IGNORE_INPUT;
    matchOptions.eBackBufferFormat   = DXUTMT_IGNORE_INPUT;
    matchOptions.eBackBufferCount    = DXUTMT_IGNORE_INPUT;
    matchOptions.eMultiSample        = DXUTMT_IGNORE_INPUT;
    matchOptions.eSwapEffect         = DXUTMT_IGNORE_INPUT;
    matchOptions.eDepthFormat        = DXUTMT_IGNORE_INPUT;
    matchOptions.eStencilFormat      = DXUTMT_IGNORE_INPUT;
    matchOptions.ePresentFlags       = DXUTMT_IGNORE_INPUT;
    matchOptions.eRefreshRate        = DXUTMT_IGNORE_INPUT;
    matchOptions.ePresentInterval    = DXUTMT_IGNORE_INPUT;

    DXUTDeviceSettings deviceSettings;
    ZeroMemory(&deviceSettings, sizeof(DXUTDeviceSettings));
    deviceSettings.AdapterOrdinal      = AdapterOrdinal;
    deviceSettings.pp.Windowed         = bWindowed;
    deviceSettings.pp.BackBufferWidth  = nSuggestedWidth;
    deviceSettings.pp.BackBufferHeight = nSuggestedHeight;

    // Override with settings from the command line
    if(GetDXUTState().GetOverrideWidth() != 0)
        deviceSettings.pp.BackBufferWidth = GetDXUTState().GetOverrideWidth();
    if(GetDXUTState().GetOverrideHeight() != 0)
        deviceSettings.pp.BackBufferHeight = GetDXUTState().GetOverrideHeight();

    if(GetDXUTState().GetOverrideAdapterOrdinal() != -1)
        deviceSettings.AdapterOrdinal = GetDXUTState().GetOverrideAdapterOrdinal();

    if(GetDXUTState().GetOverrideFullScreen())
    {
        deviceSettings.pp.Windowed = FALSE;
        if(GetDXUTState().GetOverrideWidth() == 0 && GetDXUTState().GetOverrideHeight() == 0)
            matchOptions.eResolution = DXUTMT_IGNORE_INPUT;
    }
    if(GetDXUTState().GetOverrideWindowed())
        deviceSettings.pp.Windowed = TRUE;

    deviceSettings.DeviceType = D3DDEVTYPE_HAL;
    matchOptions.eDeviceType = DXUTMT_PRESERVE_INPUT;


    if(GetDXUTState().GetOverrideForcePureHWVP())//纯设备不支持查询某些设备状态,不会滤除任何冗余状态变化,减省cpu
    {
        deviceSettings.BehaviorFlags = D3DCREATE_HARDWARE_VERTEXPROCESSING | D3DCREATE_PUREDEVICE;
    }
    else
	{
		deviceSettings.BehaviorFlags = D3DCREATE_HARDWARE_VERTEXPROCESSING;
	}
	matchOptions.eVertexProcessing = DXUTMT_PRESERVE_INPUT;

	deviceSettings.pp.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE/*D3DPRESENT_INTERVAL_DEFAULT*/;
	matchOptions.ePresentInterval = DXUTMT_PRESERVE_INPUT;

    hr = DXUTFindValidDeviceSettings(&deviceSettings, &deviceSettings, &matchOptions);
    if(FAILED(hr)) // the call will fail if no valid devices were found
    {
        DXUTDisplayErrorMessage(hr);
        return DXUT_ERR(L"DXUTFindValidDeviceSettings", hr);
    }

    // Change to a Direct3D device created from the new device settings.  
    // If there is an existing device, then either reset or recreated the scene
    hr = DXUTChangeDevice(&deviceSettings, NULL, false, true);
    if(FAILED(hr))
        return hr;

    return S_OK;
}



// Passes a previously created Direct3D device for use by the framework.  
// If DXUTCreateWindow() has not already been called, it will call it with the 
// default parameters.  Instead of calling this, you can call DXUTCreateDevice() or 
// DXUTCreateDeviceFromSettings() 

HRESULT DXUTSetDevice(IDirect3DDevice9* pd3dDevice)
{
    HRESULT hr;

    if(pd3dDevice == NULL)
        return DXUT_ERR_MSGBOX(L"DXUTSetDevice", E_INVALIDARG);

    GetDXUTState().SetDeviceCreateCalled(true);

    // If DXUTCreateWindow() or DXUTSetWindow() has not already been called, 
    // then call DXUTCreateWindow() with the default parameters.         
    if(!GetDXUTState().GetWindowCreated()) 
    {
        // If DXUTCreateWindow() or DXUTSetWindow() was already called and failed, then fail.
        // DXUTCreateWindow() or DXUTSetWindow() must first succeed for this function to succeed
        if(GetDXUTState().GetWindowCreateCalled())
            return E_FAIL; 

        // If DXUTCreateWindow() or DXUTSetWindow() hasn't been called, then 
        // automatically call DXUTCreateWindow() with default params
        hr = DXUTCreateWindow();
        if(FAILED(hr))
            return hr;
    }

    DXUTDeviceSettings* pDeviceSettings = new DXUTDeviceSettings;
    if(pDeviceSettings == NULL)
        return E_OUTOFMEMORY;
    ZeroMemory(pDeviceSettings, sizeof(DXUTDeviceSettings));

    // Get the present params from the swap chain
    IDirect3DSurface9* pBackBuffer = NULL;
    hr = pd3dDevice->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &pBackBuffer);
    if(SUCCEEDED(hr))
    {
        IDirect3DSwapChain9* pSwapChain = NULL;
        hr = pBackBuffer->GetContainer(IID_IDirect3DSwapChain9, (void**) &pSwapChain);
        if(SUCCEEDED(hr))
        {
            pSwapChain->GetPresentParameters(&pDeviceSettings->pp);
            S_REL(pSwapChain);
        }

        S_REL(pBackBuffer);
    }

    D3DDEVICE_CREATION_PARAMETERS d3dCreationParams;
    pd3dDevice->GetCreationParameters(&d3dCreationParams);

    // Fill out the rest of the device settings struct
    pDeviceSettings->AdapterOrdinal = d3dCreationParams.AdapterOrdinal;
    pDeviceSettings->DeviceType     = d3dCreationParams.DeviceType;
    DXUTFindAdapterFormat(pDeviceSettings->AdapterOrdinal, pDeviceSettings->DeviceType, 
                           pDeviceSettings->pp.BackBufferFormat, pDeviceSettings->pp.Windowed, 
                           &pDeviceSettings->AdapterFormat);
    pDeviceSettings->BehaviorFlags  = d3dCreationParams.BehaviorFlags;

    // Change to the Direct3D device passed in
    hr = DXUTChangeDevice(pDeviceSettings, pd3dDevice, false, false);

    delete pDeviceSettings;

    if(FAILED(hr)) 
        return hr;

    return S_OK;
}



// Tells the framework to change to a device created from the passed in device settings
// If DXUTCreateWindow() has not already been called, it will call it with the 
// default parameters.  Instead of calling this, you can call DXUTCreateDevice() 
// or DXUTSetDevice() 

HRESULT DXUTCreateDeviceFromSettings(DXUTDeviceSettings* pDeviceSettings, bool bPreserveInput, bool bClipWindowToSingleAdapter)
{
    HRESULT hr;

    GetDXUTState().SetDeviceCreateCalled(true);

    // If DXUTCreateWindow() or DXUTSetWindow() has not already been called, 
    // then call DXUTCreateWindow() with the default parameters.         
    if(!GetDXUTState().GetWindowCreated()) 
    {
        // If DXUTCreateWindow() or DXUTSetWindow() was already called and failed, then fail.
        // DXUTCreateWindow() or DXUTSetWindow() must first succeed for this function to succeed
        if(GetDXUTState().GetWindowCreateCalled())
            return E_FAIL; 

        // If DXUTCreateWindow() or DXUTSetWindow() hasn't been called, then 
        // automatically call DXUTCreateWindow() with default params
        hr = DXUTCreateWindow();
        if(FAILED(hr))
            return hr;
    }

    if(!bPreserveInput)
    {
        // If not preserving the input, then find the closest valid to it
        DXUTMatchOptions matchOptions;
        matchOptions.eAdapterOrdinal     = DXUTMT_CLOSEST_TO_INPUT;
        matchOptions.eDeviceType         = DXUTMT_CLOSEST_TO_INPUT;
        matchOptions.eWindowed           = DXUTMT_CLOSEST_TO_INPUT;
        matchOptions.eAdapterFormat      = DXUTMT_CLOSEST_TO_INPUT;
        matchOptions.eVertexProcessing   = DXUTMT_CLOSEST_TO_INPUT;
        matchOptions.eResolution         = DXUTMT_CLOSEST_TO_INPUT;
        matchOptions.eBackBufferFormat   = DXUTMT_CLOSEST_TO_INPUT;
        matchOptions.eBackBufferCount    = DXUTMT_CLOSEST_TO_INPUT;
        matchOptions.eMultiSample        = DXUTMT_CLOSEST_TO_INPUT;
        matchOptions.eSwapEffect         = DXUTMT_CLOSEST_TO_INPUT;
        matchOptions.eDepthFormat        = DXUTMT_CLOSEST_TO_INPUT;
        matchOptions.eStencilFormat      = DXUTMT_CLOSEST_TO_INPUT;
        matchOptions.ePresentFlags       = DXUTMT_CLOSEST_TO_INPUT;
        matchOptions.eRefreshRate        = DXUTMT_CLOSEST_TO_INPUT;
        matchOptions.ePresentInterval    = DXUTMT_CLOSEST_TO_INPUT;

        hr = DXUTFindValidDeviceSettings(pDeviceSettings, pDeviceSettings, &matchOptions);
        if(FAILED(hr)) // the call will fail if no valid devices were found
        {
            DXUTDisplayErrorMessage(hr);
            return DXUT_ERR(L"DXUTFindValidDeviceSettings", hr);
        }
    }

    // Change to a Direct3D device created from the new device settings.  
    // If there is an existing device, then either reset or recreate the scene
    hr = DXUTChangeDevice(pDeviceSettings, NULL, false, bClipWindowToSingleAdapter);
    if(FAILED(hr))
        return hr;

    return S_OK;
}



// Toggle between full screen and windowed

HRESULT DXUTToggleFullScreen()
{
    HRESULT hr;

    // Get the current device settings and flip the windowed state then
    // find the closest valid device settings with this change
    DXUTDeviceSettings deviceSettings = DXUTGetDeviceSettings();
    deviceSettings.pp.Windowed = !deviceSettings.pp.Windowed;

    DXUTMatchOptions matchOptions;
    matchOptions.eAdapterOrdinal     = DXUTMT_PRESERVE_INPUT;
    matchOptions.eDeviceType         = DXUTMT_CLOSEST_TO_INPUT;
    matchOptions.eWindowed           = DXUTMT_PRESERVE_INPUT;
    matchOptions.eAdapterFormat      = DXUTMT_IGNORE_INPUT;
    matchOptions.eVertexProcessing   = DXUTMT_CLOSEST_TO_INPUT;
    matchOptions.eBackBufferFormat   = DXUTMT_IGNORE_INPUT;
    matchOptions.eBackBufferCount    = DXUTMT_CLOSEST_TO_INPUT;
    matchOptions.eMultiSample        = DXUTMT_CLOSEST_TO_INPUT;
    matchOptions.eSwapEffect         = DXUTMT_CLOSEST_TO_INPUT;
    matchOptions.eDepthFormat        = DXUTMT_CLOSEST_TO_INPUT;
    matchOptions.eStencilFormat      = DXUTMT_CLOSEST_TO_INPUT;
    matchOptions.ePresentFlags       = DXUTMT_CLOSEST_TO_INPUT;
    matchOptions.eRefreshRate        = DXUTMT_IGNORE_INPUT;
    matchOptions.ePresentInterval    = DXUTMT_CLOSEST_TO_INPUT;

    // Go back to previous state

    UINT nWidth  = (deviceSettings.pp.Windowed) ? GetDXUTState().GetWindowBackBufferWidthAtModeChange() : GetDXUTState().GetFullScreenBackBufferWidthAtModeChange();
    UINT nHeight = (deviceSettings.pp.Windowed) ? GetDXUTState().GetWindowBackBufferHeightAtModeChange() : GetDXUTState().GetFullScreenBackBufferHeightAtModeChange();
    if(nWidth > 0 && nHeight > 0)
    {
        matchOptions.eResolution = DXUTMT_CLOSEST_TO_INPUT;
        deviceSettings.pp.BackBufferWidth = nWidth;
        deviceSettings.pp.BackBufferHeight = nHeight;
    }
    else
    {
        // No previous data, so just switch to defaults
        matchOptions.eResolution = DXUTMT_IGNORE_INPUT;
    }

    hr = DXUTFindValidDeviceSettings(&deviceSettings, &deviceSettings, &matchOptions);
    if(SUCCEEDED(hr)) 
    {
        // Create a Direct3D device using the new device settings.  
        // If there is an existing device, then it will either reset or recreate the scene.
        hr = DXUTChangeDevice(&deviceSettings, NULL, false, false);

        // If hr == E_ABORT, this means the app rejected the device settings in the ModifySettingsCallback so nothing changed
        if(FAILED(hr) && (hr != E_ABORT))
        {
            // Failed creating device, try to switch back.
            deviceSettings.pp.Windowed = !deviceSettings.pp.Windowed;
            UINT nWidth  = (deviceSettings.pp.Windowed) ? GetDXUTState().GetWindowBackBufferWidthAtModeChange() : GetDXUTState().GetFullScreenBackBufferWidthAtModeChange();
            UINT nHeight = (deviceSettings.pp.Windowed) ? GetDXUTState().GetWindowBackBufferHeightAtModeChange() : GetDXUTState().GetFullScreenBackBufferHeightAtModeChange();
            if(nWidth > 0 && nHeight > 0)
            {
                matchOptions.eResolution = DXUTMT_CLOSEST_TO_INPUT;
                deviceSettings.pp.BackBufferWidth = nWidth;
                deviceSettings.pp.BackBufferHeight = nHeight;
            }
            else
            {
                matchOptions.eResolution = DXUTMT_IGNORE_INPUT;
            }

            DXUTFindValidDeviceSettings(&deviceSettings, &deviceSettings, &matchOptions);

            HRESULT hr2 = DXUTChangeDevice(&deviceSettings, NULL, false, false);
            if(FAILED(hr2))
            {
                // If this failed, then shutdown
                DXUTShutdown();
            }
        }
    }

    return hr;
}

// Internal helper function to prepare the enumeration object by creating it if it 
// didn't already exist and enumerating if desired.

CD3DEnumeration* DXUTPrepareEnumerationObject(bool bEnumerate)
{
    // Create a new CD3DEnumeration object and enumerate all devices unless its already been done
    CD3DEnumeration* pd3dEnum = GetDXUTState().GetD3DEnumeration();
    if(pd3dEnum == NULL)
    {
        pd3dEnum = DXUTGetEnumeration(); 
        GetDXUTState().SetD3DEnumeration(pd3dEnum);

        bEnumerate = true;
    }

    if(bEnumerate)
    {
        // Enumerate for each adapter all of the supported display modes, 
        // device types, adapter formats, back buffer formats, window/full screen support, 
        // depth stencil formats, multisampling types/qualities, and presentations intervals.
        //
        // For each combination of device type (HAL/REF), adapter format, back buffer format, and
        // IsWindowed it will call the app's ConfirmDevice callback.  This allows the app
        // to reject or allow that combination based on its caps/etc.  It also allows the 
        // app to change the BehaviorFlags.  The BehaviorFlags defaults non-pure HWVP 
        // if supported otherwise it will default to SWVP, however the app can change this 
        // through the ConfirmDevice callback.
        IDirect3D9* pD3D = DXUTGetD3DObject();
        pd3dEnum->Enumerate(pD3D, IsDeviceAcceptable, NULL);
    }

    return pd3dEnum;
}



// This function tries to find valid device settings based upon the input device settings 
// struct and the match options.  For each device setting a match option in the 
// DXUTMatchOptions struct specifies how the function makes decisions.  For example, if 
// the caller wants a HAL device with a back buffer format of D3DFMT_A2B10G10R10 but the 
// HAL device on the system does not support D3DFMT_A2B10G10R10 however a REF device is 
// installed that does, then the function has a choice to either use REF or to change to 
// a back buffer format to compatible with the HAL device.  The match options lets the 
// caller control how these choices are made.
//
// Each match option must be one of the following types: 
//      DXUTMT_IGNORE_INPUT: Uses the closest valid value to a default 
//      DXUTMT_PRESERVE_INPUT: Uses the input without change, but may cause no valid device to be found
//      DXUTMT_CLOSEST_TO_INPUT: Uses the closest valid value to the input 
//
// If pMatchOptions is NULL then, all of the match options are assumed to be DXUTMT_IGNORE_INPUT.  
// The function returns failure if no valid device settings can be found otherwise 
// the function returns success and the valid device settings are written to pOut.

HRESULT DXUTFindValidDeviceSettings(DXUTDeviceSettings* pOut, DXUTDeviceSettings* pIn, 
                                     DXUTMatchOptions* pMatchOptions)
{
    if(pOut == NULL)
        return DXUT_ERR_MSGBOX(L"DXUTFindValidDeviceSettings", E_INVALIDARG);

    CD3DEnumeration* pd3dEnum = DXUTPrepareEnumerationObject(false);
    IDirect3D9*      pD3D     = DXUTGetD3DObject();

    // Default to DXUTMT_IGNORE_INPUT for everything unless pMatchOptions isn't NULL
    DXUTMatchOptions defaultMatchOptions;
    if(NULL == pMatchOptions)
    {
        ZeroMemory(&defaultMatchOptions, sizeof(DXUTMatchOptions));
        pMatchOptions = &defaultMatchOptions;
    }

    // Build an optimal device settings structure based upon the match 
    // options.  If the match option is set to ignore, then a optimal default value is used.
    // The default value may not exist on the system, but later this will be taken 
    // into account.
    DXUTDeviceSettings optimalDeviceSettings;
    DXUTBuildOptimalDeviceSettings(&optimalDeviceSettings, pIn, pMatchOptions);

    // Find the best combination of:
    //      Adapter Ordinal
    //      Device Type
    //      Adapter Format
    //      Back Buffer Format
    //      Windowed
    // given what's available on the system and the match options combined with the device settings input.
    // This combination of settings is encapsulated by the CD3DEnumDeviceSettingsCombo class.
    float fBestRanking = -1.0f;
    CD3DEnumDeviceSettingsCombo* pBestDeviceSettingsCombo = NULL;
    D3DDISPLAYMODE adapterDesktopDisplayMode;

    CGrowableArray<CD3DEnumAdapterInfo*>* pAdapterList = pd3dEnum->GetAdapterInfoList();
    for(int iAdapter=0; iAdapter<pAdapterList->GetSize(); iAdapter++)
    {
        CD3DEnumAdapterInfo* pAdapterInfo = pAdapterList->GetAt(iAdapter);

        // Get the desktop display mode of adapter 
        pD3D->GetAdapterDisplayMode(pAdapterInfo->AdapterOrdinal, &adapterDesktopDisplayMode);

        // Enum all the device types supported by this adapter to find the best device settings
        for(int iDeviceInfo=0; iDeviceInfo<pAdapterInfo->deviceInfoList.GetSize(); iDeviceInfo++)
        {
            CD3DEnumDeviceInfo* pDeviceInfo = pAdapterInfo->deviceInfoList.GetAt(iDeviceInfo);

            // Enum all the device settings combinations.  A device settings combination is 
            // a unique set of an adapter format, back buffer format, and IsWindowed.
            for(int iDeviceCombo=0; iDeviceCombo<pDeviceInfo->deviceSettingsComboList.GetSize(); iDeviceCombo++)
            {
                CD3DEnumDeviceSettingsCombo* pDeviceSettingsCombo = pDeviceInfo->deviceSettingsComboList.GetAt(iDeviceCombo);

                // If windowed mode the adapter format has to be the same as the desktop 
                // display mode format so skip any that don't match
                if (pDeviceSettingsCombo->Windowed && (pDeviceSettingsCombo->AdapterFormat != adapterDesktopDisplayMode.Format))
                    continue;

                // Skip any combo that doesn't meet the preserve match options
                if(false == DXUTDoesDeviceComboMatchPreserveOptions(pDeviceSettingsCombo, pIn, pMatchOptions))
                    continue;           

                // Get a ranking number that describes how closely this device combo matches the optimal combo
                float fCurRanking = DXUTRankDeviceCombo(pDeviceSettingsCombo, &optimalDeviceSettings, &adapterDesktopDisplayMode);

                // If this combo better matches the input device settings then save it
                if(fCurRanking > fBestRanking)
                {
                    pBestDeviceSettingsCombo = pDeviceSettingsCombo;
                    fBestRanking = fCurRanking;
                }                
            }
        }
    }

    // If no best device combination was found then fail
    if(pBestDeviceSettingsCombo == NULL) 
        return DXUTERR_NOCOMPATIBLEDEVICES;

    // Using the best device settings combo found, build valid device settings taking heed of 
    // the match options and the input device settings
    DXUTDeviceSettings validDeviceSettings;
    DXUTBuildValidDeviceSettings(&validDeviceSettings, pBestDeviceSettingsCombo, pIn, pMatchOptions);
    *pOut = validDeviceSettings;

    return S_OK;
}



// Internal helper function to build a device settings structure based upon the match 
// options.  If the match option is set to ignore, then a optimal default value is used.
// The default value may not exist on the system, but later this will be taken 
// into account.

void DXUTBuildOptimalDeviceSettings(DXUTDeviceSettings* pOptimalDeviceSettings, 
                                     DXUTDeviceSettings* pDeviceSettingsIn, 
                                     DXUTMatchOptions* pMatchOptions)
{
    IDirect3D9* pD3D = DXUTGetD3DObject();
    D3DDISPLAYMODE adapterDesktopDisplayMode;

    ZeroMemory(pOptimalDeviceSettings, sizeof(DXUTDeviceSettings)); 

    //---------------------
    // Adapter ordinal
    //---------------------    
    if(pMatchOptions->eAdapterOrdinal == DXUTMT_IGNORE_INPUT)
        pOptimalDeviceSettings->AdapterOrdinal = D3DADAPTER_DEFAULT; 
    else
        pOptimalDeviceSettings->AdapterOrdinal = pDeviceSettingsIn->AdapterOrdinal;      

    //---------------------
    // Device type
    //---------------------
    if(pMatchOptions->eDeviceType == DXUTMT_IGNORE_INPUT)
        pOptimalDeviceSettings->DeviceType = D3DDEVTYPE_HAL; 
    else
        pOptimalDeviceSettings->DeviceType = pDeviceSettingsIn->DeviceType;

    //---------------------
    // Windowed
    //---------------------
    if(pMatchOptions->eWindowed == DXUTMT_IGNORE_INPUT)
        pOptimalDeviceSettings->pp.Windowed = TRUE; 
    else
        pOptimalDeviceSettings->pp.Windowed = pDeviceSettingsIn->pp.Windowed;

    //---------------------
    // Adapter format
    //---------------------
    if(pMatchOptions->eAdapterFormat == DXUTMT_IGNORE_INPUT)
    {
        // If windowed, default to the desktop display mode
        // If fullscreen, default to the desktop display mode for quick mode change or 
        // default to D3DFMT_X8R8G8B8 if the desktop display mode is < 32bit
        pD3D->GetAdapterDisplayMode(pOptimalDeviceSettings->AdapterOrdinal, &adapterDesktopDisplayMode);
        if(pOptimalDeviceSettings->pp.Windowed || DXUTColorChannelBits(adapterDesktopDisplayMode.Format) >= 8)
            pOptimalDeviceSettings->AdapterFormat = adapterDesktopDisplayMode.Format;
        else
            pOptimalDeviceSettings->AdapterFormat = D3DFMT_X8R8G8B8;
    }
    else
    {
        pOptimalDeviceSettings->AdapterFormat = pDeviceSettingsIn->AdapterFormat;
    }

    //---------------------
    // Vertex processing
    //---------------------
    if(pMatchOptions->eVertexProcessing == DXUTMT_IGNORE_INPUT)
        pOptimalDeviceSettings->BehaviorFlags = D3DCREATE_HARDWARE_VERTEXPROCESSING; 
    else
        pOptimalDeviceSettings->BehaviorFlags = pDeviceSettingsIn->BehaviorFlags;

    //---------------------
    // Resolution
    //---------------------
    if(pMatchOptions->eResolution == DXUTMT_IGNORE_INPUT)
    {
        // If windowed, default to 640x480
        // If fullscreen, default to the desktop res for quick mode change
        if(pOptimalDeviceSettings->pp.Windowed)
        {
            pOptimalDeviceSettings->pp.BackBufferWidth = 640;
            pOptimalDeviceSettings->pp.BackBufferHeight = 480;
        }
        else
        {
            pD3D->GetAdapterDisplayMode(pOptimalDeviceSettings->AdapterOrdinal, &adapterDesktopDisplayMode);
            pOptimalDeviceSettings->pp.BackBufferWidth = adapterDesktopDisplayMode.Width;
            pOptimalDeviceSettings->pp.BackBufferHeight = adapterDesktopDisplayMode.Height;
        }
    }
    else
    {
        pOptimalDeviceSettings->pp.BackBufferWidth = pDeviceSettingsIn->pp.BackBufferWidth;
        pOptimalDeviceSettings->pp.BackBufferHeight = pDeviceSettingsIn->pp.BackBufferHeight;
    }

    //---------------------
    // Back buffer format
    //---------------------
    if(pMatchOptions->eBackBufferFormat == DXUTMT_IGNORE_INPUT)
        pOptimalDeviceSettings->pp.BackBufferFormat = pOptimalDeviceSettings->AdapterFormat; // Default to match the adapter format
    else
        pOptimalDeviceSettings->pp.BackBufferFormat = pDeviceSettingsIn->pp.BackBufferFormat;

    //---------------------
    // Back buffer count
    //---------------------
    if(pMatchOptions->eBackBufferCount == DXUTMT_IGNORE_INPUT)
        pOptimalDeviceSettings->pp.BackBufferCount = 2; // Default to triple buffering for perf gain
    else
        pOptimalDeviceSettings->pp.BackBufferCount = pDeviceSettingsIn->pp.BackBufferCount;

    //---------------------
    // Multisample
    //---------------------
    if(pMatchOptions->eMultiSample == DXUTMT_IGNORE_INPUT)
    {
        // Default to no multisampling 
        pOptimalDeviceSettings->pp.MultiSampleType = D3DMULTISAMPLE_NONE;
        pOptimalDeviceSettings->pp.MultiSampleQuality = 0; 
    }
    else
    {
        pOptimalDeviceSettings->pp.MultiSampleType = pDeviceSettingsIn->pp.MultiSampleType;
        pOptimalDeviceSettings->pp.MultiSampleQuality = pDeviceSettingsIn->pp.MultiSampleQuality;
    }

    //---------------------
    // Swap effect
    //---------------------
    if(pMatchOptions->eSwapEffect == DXUTMT_IGNORE_INPUT)
        pOptimalDeviceSettings->pp.SwapEffect = D3DSWAPEFFECT_DISCARD; 
    else
        pOptimalDeviceSettings->pp.SwapEffect = pDeviceSettingsIn->pp.SwapEffect;

    //---------------------
    // Depth stencil 
    //---------------------
    if(pMatchOptions->eDepthFormat == DXUTMT_IGNORE_INPUT &&
        pMatchOptions->eStencilFormat == DXUTMT_IGNORE_INPUT)
    {
        UINT nBackBufferBits = DXUTColorChannelBits(pOptimalDeviceSettings->pp.BackBufferFormat);
        if(nBackBufferBits >= 8)
            pOptimalDeviceSettings->pp.AutoDepthStencilFormat = D3DFMT_D24S8;//D3DFMT_D32; 
        else
            pOptimalDeviceSettings->pp.AutoDepthStencilFormat = D3DFMT_D16; 
    }
    else
    {
        pOptimalDeviceSettings->pp.AutoDepthStencilFormat = pDeviceSettingsIn->pp.AutoDepthStencilFormat;
    }

    //---------------------
    // Present flags
    //---------------------
    if(pMatchOptions->ePresentFlags == DXUTMT_IGNORE_INPUT)
        pOptimalDeviceSettings->pp.Flags = D3DPRESENTFLAG_DISCARD_DEPTHSTENCIL;
    else
        pOptimalDeviceSettings->pp.Flags = pDeviceSettingsIn->pp.Flags;

    //---------------------
    // Refresh rate
    //---------------------
    if(pMatchOptions->eRefreshRate == DXUTMT_IGNORE_INPUT)
        pOptimalDeviceSettings->pp.FullScreen_RefreshRateInHz = 0;
    else
        pOptimalDeviceSettings->pp.FullScreen_RefreshRateInHz = pDeviceSettingsIn->pp.FullScreen_RefreshRateInHz;

    //---------------------
    // Present interval
    //---------------------
    if(pMatchOptions->ePresentInterval == DXUTMT_IGNORE_INPUT)
    {
        // For windowed and fullscreen, default to D3DPRESENT_INTERVAL_DEFAULT
        // which will wait for the vertical retrace period to prevent tearing.
        // For benchmarking, use D3DPRESENT_INTERVAL_DEFAULT  which will
        // will wait not for the vertical retrace period but may introduce tearing.
        pOptimalDeviceSettings->pp.PresentationInterval = D3DPRESENT_INTERVAL_DEFAULT;
    }
    else
    {
        pOptimalDeviceSettings->pp.PresentationInterval = pDeviceSettingsIn->pp.PresentationInterval;
    }
}



// Returns false for any CD3DEnumDeviceSettingsCombo that doesn't meet the preserve 
// match options against the input pDeviceSettingsIn.

bool DXUTDoesDeviceComboMatchPreserveOptions(CD3DEnumDeviceSettingsCombo* pDeviceSettingsCombo, 
                                                 DXUTDeviceSettings* pDeviceSettingsIn, 
                                                 DXUTMatchOptions* pMatchOptions)
{
    //---------------------
    // Adapter ordinal
    //---------------------
    if(pMatchOptions->eAdapterOrdinal == DXUTMT_PRESERVE_INPUT && 
        (pDeviceSettingsCombo->AdapterOrdinal != pDeviceSettingsIn->AdapterOrdinal))
        return false;

    //---------------------
    // Device type
    //---------------------
    if(pMatchOptions->eDeviceType == DXUTMT_PRESERVE_INPUT && 
        (pDeviceSettingsCombo->DeviceType != pDeviceSettingsIn->DeviceType))
        return false;

    //---------------------
    // Windowed
    //---------------------
    if(pMatchOptions->eWindowed == DXUTMT_PRESERVE_INPUT && 
        (pDeviceSettingsCombo->Windowed != pDeviceSettingsIn->pp.Windowed))
        return false;

    //---------------------
    // Adapter format
    //---------------------
    if(pMatchOptions->eAdapterFormat == DXUTMT_PRESERVE_INPUT && 
        (pDeviceSettingsCombo->AdapterFormat != pDeviceSettingsIn->AdapterFormat))
        return false;

    //---------------------
    // Vertex processing
    //---------------------
    // If keep VP and input has HWVP, then skip if this combo doesn't have HWTL 
    if(pMatchOptions->eVertexProcessing == DXUTMT_PRESERVE_INPUT && 
        ((pDeviceSettingsIn->BehaviorFlags & D3DCREATE_HARDWARE_VERTEXPROCESSING) != 0) && 
        ((pDeviceSettingsCombo->pDeviceInfo->Caps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT) == 0))
        return false;

    //---------------------
    // Resolution
    //---------------------
    // If keep resolution then check that width and height supported by this combo
    if(pMatchOptions->eResolution == DXUTMT_PRESERVE_INPUT)
    {
        bool bFound = false;
        for(int i=0; i< pDeviceSettingsCombo->pAdapterInfo->displayModeList.GetSize(); i++)
        {
            D3DDISPLAYMODE displayMode = pDeviceSettingsCombo->pAdapterInfo->displayModeList.GetAt(i);
            if(displayMode.Format != pDeviceSettingsCombo->AdapterFormat)
                continue; // Skip this display mode if it doesn't match the combo's adapter format

            if(displayMode.Width == pDeviceSettingsIn->pp.BackBufferWidth &&
                displayMode.Height == pDeviceSettingsIn->pp.BackBufferHeight)
            {
                bFound = true;
                break;
            }
        }

        // If the width and height are not supported by this combo, return false
        if(!bFound)
            return false;
    }

    //---------------------
    // Back buffer format
    //---------------------
    if(pMatchOptions->eBackBufferFormat == DXUTMT_PRESERVE_INPUT && 
        pDeviceSettingsCombo->BackBufferFormat != pDeviceSettingsIn->pp.BackBufferFormat)
        return false;

    //---------------------
    // Back buffer count
    //---------------------
    // No caps for the back buffer count

    //---------------------
    // Multisample
    //---------------------
    if(pMatchOptions->eMultiSample == DXUTMT_PRESERVE_INPUT)
    {
        bool bFound = false;
        for(int i=0; i<pDeviceSettingsCombo->multiSampleTypeList.GetSize(); i++)
        {
            D3DMULTISAMPLE_TYPE msType = pDeviceSettingsCombo->multiSampleTypeList.GetAt(i);
            DWORD msQuality  = pDeviceSettingsCombo->multiSampleQualityList.GetAt(i);

            if(msType == pDeviceSettingsIn->pp.MultiSampleType &&
                msQuality >= pDeviceSettingsIn->pp.MultiSampleQuality)
            {
                bFound = true;
                break;
            }
        }

        // If multisample type/quality not supported by this combo, then return false
        if(!bFound)
            return false;
    }

    //---------------------
    // Swap effect
    //---------------------
    // No caps for swap effects

    //---------------------
    // Depth stencil 
    //---------------------
    // If keep depth stencil format then check that the depth stencil format is supported by this combo
    if(pMatchOptions->eDepthFormat == DXUTMT_PRESERVE_INPUT &&
        pMatchOptions->eStencilFormat == DXUTMT_PRESERVE_INPUT)
    {
        if(pDeviceSettingsIn->pp.AutoDepthStencilFormat != D3DFMT_UNKNOWN &&
            !pDeviceSettingsCombo->depthStencilFormatList.Contains(pDeviceSettingsIn->pp.AutoDepthStencilFormat))
            return false;
    }

    // If keep depth format then check that the depth format is supported by this combo
    if(pMatchOptions->eDepthFormat == DXUTMT_PRESERVE_INPUT &&
        pDeviceSettingsIn->pp.AutoDepthStencilFormat != D3DFMT_UNKNOWN)
    {
        bool bFound = false;
        UINT dwDepthBits = DXUTDepthBits(pDeviceSettingsIn->pp.AutoDepthStencilFormat);
        for(int i=0; i<pDeviceSettingsCombo->depthStencilFormatList.GetSize(); i++)
        {
            D3DFORMAT depthStencilFmt = pDeviceSettingsCombo->depthStencilFormatList.GetAt(i);
            UINT dwCurDepthBits = DXUTDepthBits(depthStencilFmt);
            if(dwCurDepthBits - dwDepthBits == 0)
                bFound = true;
        }

        if(!bFound)
            return false;
    }

    // If keep depth format then check that the depth format is supported by this combo
    if(pMatchOptions->eStencilFormat == DXUTMT_PRESERVE_INPUT &&
        pDeviceSettingsIn->pp.AutoDepthStencilFormat != D3DFMT_UNKNOWN)
    {
        bool bFound = false;
        UINT dwStencilBits = DXUTStencilBits(pDeviceSettingsIn->pp.AutoDepthStencilFormat);
        for(int i=0; i<pDeviceSettingsCombo->depthStencilFormatList.GetSize(); i++)
        {
            D3DFORMAT depthStencilFmt = pDeviceSettingsCombo->depthStencilFormatList.GetAt(i);
            UINT dwCurStencilBits = DXUTStencilBits(depthStencilFmt);
            if(dwCurStencilBits - dwStencilBits == 0)
                bFound = true;
        }

        if(!bFound)
            return false;
    }

    //---------------------
    // Present flags
    //---------------------
    // No caps for the present flags

    //---------------------
    // Refresh rate
    //---------------------
    // If keep refresh rate then check that the resolution is supported by this combo
    if(pMatchOptions->eRefreshRate == DXUTMT_PRESERVE_INPUT)
    {
        bool bFound = false;
        for(int i=0; i<pDeviceSettingsCombo->pAdapterInfo->displayModeList.GetSize(); i++)
        {
            D3DDISPLAYMODE displayMode = pDeviceSettingsCombo->pAdapterInfo->displayModeList.GetAt(i);
            if(displayMode.Format != pDeviceSettingsCombo->AdapterFormat)
                continue;
            if(displayMode.RefreshRate == pDeviceSettingsIn->pp.FullScreen_RefreshRateInHz)
            {
                bFound = true;
                break;
            }
        }

        // If refresh rate not supported by this combo, then return false
        if(!bFound)
            return false;
    }

    //---------------------
    // Present interval
    //---------------------
    // If keep present interval then check that the present interval is supported by this combo
    if(pMatchOptions->ePresentInterval == DXUTMT_PRESERVE_INPUT &&
        !pDeviceSettingsCombo->presentIntervalList.Contains(pDeviceSettingsIn->pp.PresentationInterval))
        return false;

    return true;
}



// Returns a ranking number that describes how closely this device 
// combo matches the optimal combo based on the match options and the optimal device settings

float DXUTRankDeviceCombo(CD3DEnumDeviceSettingsCombo* pDeviceSettingsCombo, 
                           DXUTDeviceSettings* pOptimalDeviceSettings,
                           D3DDISPLAYMODE* pAdapterDesktopDisplayMode)
{
    float fCurRanking = 0.0f; 

    // Arbitrary weights.  Gives preference to the ordinal, device type, and windowed
    const float fAdapterOrdinalWeight   = 1000.0f;
    const float fDeviceTypeWeight       = 100.0f;
    const float fWindowWeight           = 10.0f;
    const float fAdapterFormatWeight    = 1.0f;
    const float fVertexProcessingWeight = 1.0f;
    const float fResolutionWeight       = 1.0f;
    const float fBackBufferFormatWeight = 1.0f;
    const float fMultiSampleWeight      = 1.0f;
    const float fDepthStencilWeight     = 1.0f;
    const float fRefreshRateWeight      = 1.0f;
    const float fPresentIntervalWeight  = 1.0f;

    //---------------------
    // Adapter ordinal
    //---------------------
    if(pDeviceSettingsCombo->AdapterOrdinal == pOptimalDeviceSettings->AdapterOrdinal)
        fCurRanking += fAdapterOrdinalWeight;

    //---------------------
    // Device type
    //---------------------
    if(pDeviceSettingsCombo->DeviceType == pOptimalDeviceSettings->DeviceType)
        fCurRanking += fDeviceTypeWeight;
    // Slightly prefer HAL 
    if(pDeviceSettingsCombo->DeviceType == D3DDEVTYPE_HAL)
        fCurRanking += 0.1f; 

    //---------------------
    // Windowed
    //---------------------
    if(pDeviceSettingsCombo->Windowed == pOptimalDeviceSettings->pp.Windowed)
        fCurRanking += fWindowWeight;

    //---------------------
    // Adapter format
    //---------------------
    if(pDeviceSettingsCombo->AdapterFormat == pOptimalDeviceSettings->AdapterFormat)
    {
        fCurRanking += fAdapterFormatWeight;
    }
    else
    {
        int nBitDepthDelta = abs((long) DXUTColorChannelBits(pDeviceSettingsCombo->AdapterFormat) -
                                  (long) DXUTColorChannelBits(pOptimalDeviceSettings->AdapterFormat));
        float fScale = __max(0.9f - (float)nBitDepthDelta*0.2f, 0.0f);
        fCurRanking += fScale * fAdapterFormatWeight;
    }

    if(!pDeviceSettingsCombo->Windowed)
    {
        // Slightly prefer when it matches the desktop format or is D3DFMT_X8R8G8B8
        bool bAdapterOptimalMatch;
        if(DXUTColorChannelBits(pAdapterDesktopDisplayMode->Format) >= 8)
            bAdapterOptimalMatch = (pDeviceSettingsCombo->AdapterFormat == pAdapterDesktopDisplayMode->Format);
        else
            bAdapterOptimalMatch = (pDeviceSettingsCombo->AdapterFormat == D3DFMT_X8R8G8B8);

        if(bAdapterOptimalMatch)
            fCurRanking += 0.1f;
    }

    //---------------------
    // Vertex processing
    //---------------------
    if((pOptimalDeviceSettings->BehaviorFlags & D3DCREATE_HARDWARE_VERTEXPROCESSING) != 0 || 
        (pOptimalDeviceSettings->BehaviorFlags & D3DCREATE_MIXED_VERTEXPROCESSING) != 0)
    {
        if((pDeviceSettingsCombo->pDeviceInfo->Caps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT) != 0)
            fCurRanking += fVertexProcessingWeight;
    }
    // Slightly prefer HW T&L
    if((pDeviceSettingsCombo->pDeviceInfo->Caps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT) != 0)
        fCurRanking += 0.1f;

    //---------------------
    // Resolution
    //---------------------
    bool bResolutionFound = false;
    for(int idm = 0; idm < pDeviceSettingsCombo->pAdapterInfo->displayModeList.GetSize(); idm++)
    {
        D3DDISPLAYMODE displayMode = pDeviceSettingsCombo->pAdapterInfo->displayModeList.GetAt(idm);
        if(displayMode.Format != pDeviceSettingsCombo->AdapterFormat)
            continue;
        if(displayMode.Width == pOptimalDeviceSettings->pp.BackBufferWidth &&
            displayMode.Height == pOptimalDeviceSettings->pp.BackBufferHeight)
            bResolutionFound = true;
    }
    if(bResolutionFound)
        fCurRanking += fResolutionWeight;

    //---------------------
    // Back buffer format
    //---------------------
    if(pDeviceSettingsCombo->BackBufferFormat == pOptimalDeviceSettings->pp.BackBufferFormat)
    {
        fCurRanking += fBackBufferFormatWeight;
    }
    else
    {
        int nBitDepthDelta = abs((long) DXUTColorChannelBits(pDeviceSettingsCombo->BackBufferFormat) -
                                  (long) DXUTColorChannelBits(pOptimalDeviceSettings->pp.BackBufferFormat));
        float fScale = __max(0.9f - (float)nBitDepthDelta*0.2f, 0.0f);
        fCurRanking += fScale * fBackBufferFormatWeight;
    }

    // Check if this back buffer format is the same as 
    // the adapter format since this is preferred.
    bool bAdapterMatchesBB = (pDeviceSettingsCombo->BackBufferFormat == pDeviceSettingsCombo->AdapterFormat);
    if(bAdapterMatchesBB)
        fCurRanking += 0.1f;

    //---------------------
    // Back buffer count
    //---------------------
    // No caps for the back buffer count

    //---------------------
    // Multisample
    //---------------------
    bool bMultiSampleFound = false;
    for(int i=0; i<pDeviceSettingsCombo->multiSampleTypeList.GetSize(); i++)
    {
        D3DMULTISAMPLE_TYPE msType = pDeviceSettingsCombo->multiSampleTypeList.GetAt(i);
        DWORD msQuality  = pDeviceSettingsCombo->multiSampleQualityList.GetAt(i);

        if(msType == pOptimalDeviceSettings->pp.MultiSampleType &&
            msQuality >= pOptimalDeviceSettings->pp.MultiSampleQuality)
        {
            bMultiSampleFound = true;
            break;
        }
    }
    if(bMultiSampleFound)
        fCurRanking += fMultiSampleWeight;

    //---------------------
    // Swap effect
    //---------------------
    // No caps for swap effects

    //---------------------
    // Depth stencil 
    //---------------------
    if(pDeviceSettingsCombo->depthStencilFormatList.Contains(pOptimalDeviceSettings->pp.AutoDepthStencilFormat))
        fCurRanking += fDepthStencilWeight;

    //---------------------
    // Present flags
    //---------------------
    // No caps for the present flags

    //---------------------
    // Refresh rate
    //---------------------
    bool bRefreshFound = false;
    for(int idm = 0; idm < pDeviceSettingsCombo->pAdapterInfo->displayModeList.GetSize(); idm++)
    {
        D3DDISPLAYMODE displayMode = pDeviceSettingsCombo->pAdapterInfo->displayModeList.GetAt(idm);
        if(displayMode.Format != pDeviceSettingsCombo->AdapterFormat)
            continue;
        if(displayMode.RefreshRate == pOptimalDeviceSettings->pp.FullScreen_RefreshRateInHz)
            bRefreshFound = true;
    }
    if(bRefreshFound)
        fCurRanking += fRefreshRateWeight;

    //---------------------
    // Present interval
    //---------------------
    // If keep present interval then check that the present interval is supported by this combo
    if(pDeviceSettingsCombo->presentIntervalList.Contains(pOptimalDeviceSettings->pp.PresentationInterval))
        fCurRanking += fPresentIntervalWeight;

    return fCurRanking;
}



// Builds valid device settings using the match options, the input device settings, and the 
// best device settings combo found.

void DXUTBuildValidDeviceSettings(DXUTDeviceSettings* pValidDeviceSettings, 
                                   CD3DEnumDeviceSettingsCombo* pBestDeviceSettingsCombo, 
                                   DXUTDeviceSettings* pDeviceSettingsIn, 
                                   DXUTMatchOptions* pMatchOptions)
{
    IDirect3D9* pD3D = DXUTGetD3DObject();
    D3DDISPLAYMODE adapterDesktopDisplayMode;
    pD3D->GetAdapterDisplayMode(pBestDeviceSettingsCombo->AdapterOrdinal, &adapterDesktopDisplayMode);

    // For each setting pick the best, taking into account the match options and 
    // what's supported by the device

    //---------------------
    // Adapter Ordinal
    //---------------------
    // Just using pBestDeviceSettingsCombo->AdapterOrdinal

    //---------------------
    // Device Type
    //---------------------
    // Just using pBestDeviceSettingsCombo->DeviceType

    //---------------------
    // Windowed 
    //---------------------
    // Just using pBestDeviceSettingsCombo->Windowed

    //---------------------
    // Adapter Format
    //---------------------
    // Just using pBestDeviceSettingsCombo->AdapterFormat

    //---------------------
    // Vertex processing
    //---------------------
    DWORD dwBestBehaviorFlags = 0;
    if(pMatchOptions->eVertexProcessing == DXUTMT_PRESERVE_INPUT)   
    {
        dwBestBehaviorFlags = pDeviceSettingsIn->BehaviorFlags;
    }
    else if(pMatchOptions->eVertexProcessing == DXUTMT_IGNORE_INPUT)    
    {
        // The framework defaults to HWVP if available otherwise use SWVP
        if ((pBestDeviceSettingsCombo->pDeviceInfo->Caps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT) != 0)
            dwBestBehaviorFlags |= D3DCREATE_HARDWARE_VERTEXPROCESSING;
        else
            dwBestBehaviorFlags |= D3DCREATE_SOFTWARE_VERTEXPROCESSING;
    }
    else // if(pMatchOptions->eVertexProcessing == DXUTMT_CLOSEST_TO_INPUT)    
    {
        // Default to input, and fallback to SWVP if HWVP not available 
        dwBestBehaviorFlags = pDeviceSettingsIn->BehaviorFlags;
        if ((pBestDeviceSettingsCombo->pDeviceInfo->Caps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT) == 0 && 
            ((dwBestBehaviorFlags & D3DCREATE_HARDWARE_VERTEXPROCESSING) != 0 || 
              (dwBestBehaviorFlags & D3DCREATE_MIXED_VERTEXPROCESSING) != 0))
        {
            dwBestBehaviorFlags &= ~D3DCREATE_HARDWARE_VERTEXPROCESSING;
            dwBestBehaviorFlags &= ~D3DCREATE_MIXED_VERTEXPROCESSING;
            dwBestBehaviorFlags |= D3DCREATE_SOFTWARE_VERTEXPROCESSING;
        }

        // One of these must be selected
        if((dwBestBehaviorFlags & D3DCREATE_HARDWARE_VERTEXPROCESSING) == 0 &&
            (dwBestBehaviorFlags & D3DCREATE_MIXED_VERTEXPROCESSING) == 0 &&
            (dwBestBehaviorFlags & D3DCREATE_SOFTWARE_VERTEXPROCESSING) == 0)
        {
            if ((pBestDeviceSettingsCombo->pDeviceInfo->Caps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT) != 0)
                dwBestBehaviorFlags |= D3DCREATE_HARDWARE_VERTEXPROCESSING;
            else
                dwBestBehaviorFlags |= D3DCREATE_SOFTWARE_VERTEXPROCESSING;
        }
    }

    //---------------------
    // Resolution
    //---------------------
    D3DDISPLAYMODE bestDisplayMode;  
    if(pMatchOptions->eResolution == DXUTMT_PRESERVE_INPUT)   
    {
        bestDisplayMode.Width = pDeviceSettingsIn->pp.BackBufferWidth;
        bestDisplayMode.Height = pDeviceSettingsIn->pp.BackBufferHeight;
    }
    else 
    {
        D3DDISPLAYMODE displayModeIn;  
        if(pMatchOptions->eResolution == DXUTMT_CLOSEST_TO_INPUT &&
            pDeviceSettingsIn)   
        {
            displayModeIn.Width = pDeviceSettingsIn->pp.BackBufferWidth;
            displayModeIn.Height = pDeviceSettingsIn->pp.BackBufferHeight;
        }
        else // if(pMatchOptions->eResolution == DXUTMT_IGNORE_INPUT)   
        {
            if(pBestDeviceSettingsCombo->Windowed)
            {
                // The framework defaults to 640x480 for windowed
                displayModeIn.Width = 640;
                displayModeIn.Height = 480;
            }
            else
            {
                // The framework defaults to desktop resolution for fullscreen to try to avoid slow mode change
                displayModeIn.Width = adapterDesktopDisplayMode.Width;
                displayModeIn.Height = adapterDesktopDisplayMode.Height;
            }
        }

        // Call a helper function to find the closest valid display mode to the optimal 
        DXUTFindValidResolution(pBestDeviceSettingsCombo, displayModeIn, &bestDisplayMode);
    }

    //---------------------
    // Back Buffer Format
    //---------------------
    // Just using pBestDeviceSettingsCombo->BackBufferFormat

    //---------------------
    // Back buffer count
    //---------------------
    UINT bestBackBufferCount;
    if(pMatchOptions->eBackBufferCount == DXUTMT_PRESERVE_INPUT)   
    {
        bestBackBufferCount = pDeviceSettingsIn->pp.BackBufferCount;
    }
    else if(pMatchOptions->eBackBufferCount == DXUTMT_IGNORE_INPUT)   
    {
        // The framework defaults to triple buffering 
        bestBackBufferCount = 2;
    }
    else // if(pMatchOptions->eBackBufferCount == DXUTMT_CLOSEST_TO_INPUT)   
    {
        bestBackBufferCount = pDeviceSettingsIn->pp.BackBufferCount;
        if(bestBackBufferCount > 3)
            bestBackBufferCount = 3;
        if(bestBackBufferCount < 1)
            bestBackBufferCount = 1;
    }

    //---------------------
    // Multisample
    //---------------------
    D3DMULTISAMPLE_TYPE bestMultiSampleType;
    DWORD bestMultiSampleQuality;
    if(pDeviceSettingsIn && pDeviceSettingsIn->pp.SwapEffect != D3DSWAPEFFECT_DISCARD)
    {
        // Swap effect is not set to discard so multisampling has to off
        bestMultiSampleType = D3DMULTISAMPLE_NONE;
        bestMultiSampleQuality = 0;
    }
    else
    {
        if(pMatchOptions->eMultiSample == DXUTMT_PRESERVE_INPUT)   
        {
            bestMultiSampleType    = pDeviceSettingsIn->pp.MultiSampleType;
            bestMultiSampleQuality = pDeviceSettingsIn->pp.MultiSampleQuality;
        }
        else if(pMatchOptions->eMultiSample == DXUTMT_IGNORE_INPUT)   
        {
            // Default to no multisampling (always supported)
            bestMultiSampleType = D3DMULTISAMPLE_NONE;
            bestMultiSampleQuality = 0;
        }
        else if(pMatchOptions->eMultiSample == DXUTMT_CLOSEST_TO_INPUT)   
        {
            // Default to no multisampling (always supported)
            bestMultiSampleType = D3DMULTISAMPLE_NONE;
            bestMultiSampleQuality = 0;

            for(int i=0; i < pBestDeviceSettingsCombo->multiSampleTypeList.GetSize(); i++)
            {
                D3DMULTISAMPLE_TYPE type = pBestDeviceSettingsCombo->multiSampleTypeList.GetAt(i);
                DWORD qualityLevels = pBestDeviceSettingsCombo->multiSampleQualityList.GetAt(i);

                // Check whether supported type is closer to the input than our current best
                if(abs(type - pDeviceSettingsIn->pp.MultiSampleType) < abs(bestMultiSampleType - pDeviceSettingsIn->pp.MultiSampleType))
                {
                    bestMultiSampleType = type; 
                    bestMultiSampleQuality = __min(qualityLevels-1, pDeviceSettingsIn->pp.MultiSampleQuality);
                }
            }
        }
        else
        {
            // Error case
            bestMultiSampleType = D3DMULTISAMPLE_NONE;
            bestMultiSampleQuality = 0;
        }
    }

    //---------------------
    // Swap effect
    //---------------------
    D3DSWAPEFFECT bestSwapEffect;
    if(pMatchOptions->eSwapEffect == DXUTMT_PRESERVE_INPUT)   
    {
        bestSwapEffect = pDeviceSettingsIn->pp.SwapEffect;
    }
    else if(pMatchOptions->eSwapEffect == DXUTMT_IGNORE_INPUT)   
    {
        bestSwapEffect = D3DSWAPEFFECT_DISCARD;
    }
    else // if(pMatchOptions->eSwapEffect == DXUTMT_CLOSEST_TO_INPUT)   
    {
        bestSwapEffect = pDeviceSettingsIn->pp.SwapEffect;

        // Swap effect has to be one of these 3
        if(bestSwapEffect != D3DSWAPEFFECT_DISCARD &&
            bestSwapEffect != D3DSWAPEFFECT_FLIP &&
            bestSwapEffect != D3DSWAPEFFECT_COPY)
        {
            bestSwapEffect = D3DSWAPEFFECT_DISCARD;
        }
    }

    //---------------------
    // Depth stencil 
    //---------------------
    D3DFORMAT bestDepthStencilFormat;
    BOOL bestEnableAutoDepthStencil;

    CGrowableArray< int > depthStencilRanking;
    depthStencilRanking.SetSize(pBestDeviceSettingsCombo->depthStencilFormatList.GetSize());

    UINT dwBackBufferBitDepth = DXUTColorChannelBits(pBestDeviceSettingsCombo->BackBufferFormat);       
    UINT dwInputDepthBitDepth = 0;
    if(pDeviceSettingsIn)
        dwInputDepthBitDepth = DXUTDepthBits(pDeviceSettingsIn->pp.AutoDepthStencilFormat);

    for(int i=0; i<pBestDeviceSettingsCombo->depthStencilFormatList.GetSize(); i++)
    {
        D3DFORMAT curDepthStencilFmt = pBestDeviceSettingsCombo->depthStencilFormatList.GetAt(i);
        DWORD dwCurDepthBitDepth = DXUTDepthBits(curDepthStencilFmt);
        int nRanking;

        if(pMatchOptions->eDepthFormat == DXUTMT_PRESERVE_INPUT)
        {                       
            // Need to match bit depth of input
            if(dwCurDepthBitDepth == dwInputDepthBitDepth)
                nRanking = 0;
            else
                nRanking = 10000;
        }
        else if(pMatchOptions->eDepthFormat == DXUTMT_IGNORE_INPUT)
        {
            // Prefer match of backbuffer bit depth
            nRanking = abs((int)dwCurDepthBitDepth - (int)dwBackBufferBitDepth*4);
        }
        else // if(pMatchOptions->eDepthFormat == DXUTMT_CLOSEST_TO_INPUT)
        {
            // Prefer match of input depth format bit depth
            nRanking = abs((int)dwCurDepthBitDepth - (int)dwInputDepthBitDepth);
        }

        depthStencilRanking.Add(nRanking);
    }

    UINT dwInputStencilBitDepth = 0;
    if(pDeviceSettingsIn)
        dwInputStencilBitDepth = DXUTStencilBits(pDeviceSettingsIn->pp.AutoDepthStencilFormat);

    for(int i=0; i<pBestDeviceSettingsCombo->depthStencilFormatList.GetSize(); i++)
    {
        D3DFORMAT curDepthStencilFmt = pBestDeviceSettingsCombo->depthStencilFormatList.GetAt(i);
        int nRanking = depthStencilRanking.GetAt(i);
        DWORD dwCurStencilBitDepth = DXUTStencilBits(curDepthStencilFmt);

        if(pMatchOptions->eStencilFormat == DXUTMT_PRESERVE_INPUT)
        {                       
            // Need to match bit depth of input
            if(dwCurStencilBitDepth == dwInputStencilBitDepth)
                nRanking += 0;
            else
                nRanking += 10000;
        }
        else if(pMatchOptions->eStencilFormat == DXUTMT_IGNORE_INPUT)
        {
            // Prefer 0 stencil bit depth
            nRanking += dwCurStencilBitDepth;
        }
        else // if(pMatchOptions->eStencilFormat == DXUTMT_CLOSEST_TO_INPUT)
        {
            // Prefer match of input stencil format bit depth
            nRanking += abs((int)dwCurStencilBitDepth - (int)dwInputStencilBitDepth);
        }

        depthStencilRanking.SetAt(i, nRanking);
    }

    int nBestRanking = 100000;
    int nBestIndex = -1;
    for(int i=0; i<pBestDeviceSettingsCombo->depthStencilFormatList.GetSize(); i++)
    {
        int nRanking = depthStencilRanking.GetAt(i);
        if(nRanking < nBestRanking)
        {
            nBestRanking = nRanking;
            nBestIndex = i;
        }
    }

    if(nBestIndex >= 0)
    {
        bestDepthStencilFormat = pBestDeviceSettingsCombo->depthStencilFormatList.GetAt(nBestIndex);
        bestEnableAutoDepthStencil = true;
    }
    else
    {
        bestDepthStencilFormat = D3DFMT_UNKNOWN;
        bestEnableAutoDepthStencil = false;
    }


    //---------------------
    // Present flags
    //---------------------
    DWORD dwBestFlags;
    if(pMatchOptions->ePresentFlags == DXUTMT_PRESERVE_INPUT)   
    {
        dwBestFlags = pDeviceSettingsIn->pp.Flags;
    }
    else if(pMatchOptions->ePresentFlags == DXUTMT_IGNORE_INPUT)   
    {
        dwBestFlags = 0;
        if(bestEnableAutoDepthStencil)
            dwBestFlags = D3DPRESENTFLAG_DISCARD_DEPTHSTENCIL;            
    }
    else // if(pMatchOptions->ePresentFlags == DXUTMT_CLOSEST_TO_INPUT)   
    {
        dwBestFlags = pDeviceSettingsIn->pp.Flags;
        if(bestEnableAutoDepthStencil)
            dwBestFlags |= D3DPRESENTFLAG_DISCARD_DEPTHSTENCIL;
    }

    //---------------------
    // Refresh rate
    //---------------------
    if(pBestDeviceSettingsCombo->Windowed)
    {
        // Must be 0 for windowed
        bestDisplayMode.RefreshRate = 0;
    }
    else
    {
        if(pMatchOptions->eRefreshRate == DXUTMT_PRESERVE_INPUT)   
        {
            bestDisplayMode.RefreshRate = pDeviceSettingsIn->pp.FullScreen_RefreshRateInHz;
        }
        else 
        {
            UINT refreshRateMatch;
            if(pMatchOptions->eRefreshRate == DXUTMT_CLOSEST_TO_INPUT)   
            {
                refreshRateMatch = pDeviceSettingsIn->pp.FullScreen_RefreshRateInHz;
            }
            else // if(pMatchOptions->eRefreshRate == DXUTMT_IGNORE_INPUT)   
            {
                refreshRateMatch = adapterDesktopDisplayMode.RefreshRate;
            }

            bestDisplayMode.RefreshRate = 0;

            if(refreshRateMatch != 0)
            {
                int nBestRefreshRanking = 100000;
                CGrowableArray<D3DDISPLAYMODE>* pDisplayModeList = &pBestDeviceSettingsCombo->pAdapterInfo->displayModeList;
                for(int iDisplayMode=0; iDisplayMode<pDisplayModeList->GetSize(); iDisplayMode++)
                {
                    D3DDISPLAYMODE displayMode = pDisplayModeList->GetAt(iDisplayMode);                
                    if(displayMode.Format != pBestDeviceSettingsCombo->AdapterFormat || 
                        displayMode.Height != bestDisplayMode.Height ||
                        displayMode.Width != bestDisplayMode.Width)
                        continue; // Skip display modes that don't match 

                    // Find the delta between the current refresh rate and the optimal refresh rate 
                    int nCurRanking = abs((int)displayMode.RefreshRate - (int)refreshRateMatch);

                    if(nCurRanking < nBestRefreshRanking)
                    {
                        bestDisplayMode.RefreshRate = displayMode.RefreshRate;
                        nBestRefreshRanking = nCurRanking;

                        // Stop if perfect match found
                        if(nBestRefreshRanking == 0)
                            break;
                    }
                }
            }
        }
    }

    //---------------------
    // Present interval
    //---------------------
    UINT bestPresentInterval;
    if(pMatchOptions->ePresentInterval == DXUTMT_PRESERVE_INPUT)   
    {
        bestPresentInterval = pDeviceSettingsIn->pp.PresentationInterval;
    }
    else if(pMatchOptions->ePresentInterval == DXUTMT_IGNORE_INPUT)   
    {
        // For windowed and fullscreen, default to D3DPRESENT_INTERVAL_DEFAULT
        // which will wait for the vertical retrace period to prevent tearing.
        // For benchmarking, use D3DPRESENT_INTERVAL_DEFAULT  which will
        // will wait not for the vertical retrace period but may introduce tearing.
        bestPresentInterval = D3DPRESENT_INTERVAL_DEFAULT;
    }
    else // if(pMatchOptions->ePresentInterval == DXUTMT_CLOSEST_TO_INPUT)   
    {
        if(pBestDeviceSettingsCombo->presentIntervalList.Contains(pDeviceSettingsIn->pp.PresentationInterval))
        {
            bestPresentInterval = pDeviceSettingsIn->pp.PresentationInterval;
        }
        else
        {
            bestPresentInterval = D3DPRESENT_INTERVAL_DEFAULT;
        }
    }

    // Fill the device settings struct
    ZeroMemory(pValidDeviceSettings, sizeof(DXUTDeviceSettings));
    pValidDeviceSettings->AdapterOrdinal                 = pBestDeviceSettingsCombo->AdapterOrdinal;
    pValidDeviceSettings->DeviceType                     = pBestDeviceSettingsCombo->DeviceType;
    pValidDeviceSettings->AdapterFormat                  = pBestDeviceSettingsCombo->AdapterFormat;
    pValidDeviceSettings->BehaviorFlags                  = dwBestBehaviorFlags;
    pValidDeviceSettings->pp.BackBufferWidth             = bestDisplayMode.Width;
    pValidDeviceSettings->pp.BackBufferHeight            = bestDisplayMode.Height;
    pValidDeviceSettings->pp.BackBufferFormat            = pBestDeviceSettingsCombo->BackBufferFormat;
    pValidDeviceSettings->pp.BackBufferCount             = bestBackBufferCount;
    pValidDeviceSettings->pp.MultiSampleType             = bestMultiSampleType;  
    pValidDeviceSettings->pp.MultiSampleQuality          = bestMultiSampleQuality;
    pValidDeviceSettings->pp.SwapEffect                  = bestSwapEffect;
    pValidDeviceSettings->pp.hDeviceWindow               = pBestDeviceSettingsCombo->Windowed ? DXUTGetHWNDDeviceWindowed() : DXUTGetHWNDDeviceFullScreen();
    pValidDeviceSettings->pp.Windowed                    = pBestDeviceSettingsCombo->Windowed;
    pValidDeviceSettings->pp.EnableAutoDepthStencil      = bestEnableAutoDepthStencil;  
    pValidDeviceSettings->pp.AutoDepthStencilFormat      = bestDepthStencilFormat;
    pValidDeviceSettings->pp.Flags                       = dwBestFlags;                   
    pValidDeviceSettings->pp.FullScreen_RefreshRateInHz  = bestDisplayMode.RefreshRate;
    pValidDeviceSettings->pp.PresentationInterval        = bestPresentInterval;
}



// Internal helper function to find the closest allowed display mode to the optimal 

HRESULT DXUTFindValidResolution(CD3DEnumDeviceSettingsCombo* pBestDeviceSettingsCombo, 
                                D3DDISPLAYMODE displayModeIn, D3DDISPLAYMODE* pBestDisplayMode)
{
    D3DDISPLAYMODE bestDisplayMode;
    ZeroMemory(&bestDisplayMode, sizeof(D3DDISPLAYMODE));

    if(pBestDeviceSettingsCombo->Windowed)
    {
        // In windowed mode, all resolutions are valid but restritions still apply 
        // on the size of the window.  See DXUTChangeDevice() for details
        *pBestDisplayMode = displayModeIn;
    }
    else
    {
        int nBestRanking = 100000;
        int nCurRanking;
        CGrowableArray<D3DDISPLAYMODE>* pDisplayModeList = &pBestDeviceSettingsCombo->pAdapterInfo->displayModeList;
        for(int iDisplayMode=0; iDisplayMode<pDisplayModeList->GetSize(); iDisplayMode++)
        {
            D3DDISPLAYMODE displayMode = pDisplayModeList->GetAt(iDisplayMode);

            // Skip display modes that don't match the combo's adapter format
            if(displayMode.Format != pBestDeviceSettingsCombo->AdapterFormat)
                continue;

            // Find the delta between the current width/height and the optimal width/height
            nCurRanking = abs((int)displayMode.Width - (int)displayModeIn.Width) + 
                          abs((int)displayMode.Height- (int)displayModeIn.Height);

            if(nCurRanking < nBestRanking)
            {
                bestDisplayMode = displayMode;
                nBestRanking = nCurRanking;

                // Stop if perfect match found
                if(nBestRanking == 0)
                    break;
            }
        }

        if(bestDisplayMode.Width == 0)
        {
            *pBestDisplayMode = displayModeIn;
            return E_FAIL; // No valid display modes found
        }

        *pBestDisplayMode = bestDisplayMode;
    }

    return S_OK;
}



// Internal helper function to return the adapter format from the first device settings 
// combo that matches the passed adapter ordinal, device type, backbuffer format, and windowed.  

HRESULT DXUTFindAdapterFormat(UINT AdapterOrdinal, D3DDEVTYPE DeviceType, D3DFORMAT BackBufferFormat, 
                               BOOL Windowed, D3DFORMAT* pAdapterFormat)
{
    CD3DEnumeration* pd3dEnum = DXUTPrepareEnumerationObject();
    CD3DEnumDeviceInfo* pDeviceInfo = pd3dEnum->GetDeviceInfo(AdapterOrdinal, DeviceType);
    if(pDeviceInfo)
    {
        for(int iDeviceCombo=0; iDeviceCombo<pDeviceInfo->deviceSettingsComboList.GetSize(); iDeviceCombo++)
        {
            CD3DEnumDeviceSettingsCombo* pDeviceSettingsCombo = pDeviceInfo->deviceSettingsComboList.GetAt(iDeviceCombo);
            if(pDeviceSettingsCombo->BackBufferFormat == BackBufferFormat &&
                pDeviceSettingsCombo->Windowed == Windowed)
            {
                // Return the adapter format from the first match
                *pAdapterFormat = pDeviceSettingsCombo->AdapterFormat;
                return S_OK;
            }
        }
    }

    *pAdapterFormat = BackBufferFormat;
    return E_FAIL;
}



// Change to a Direct3D device created from the device settings or passed in.
// The framework will only reset if the device is similar to the previous device 
// otherwise it will cleanup the previous device (if there is one) and recreate the 
// scene using the app's device callbacks.

HRESULT DXUTChangeDevice(DXUTDeviceSettings* pNewDeviceSettings, IDirect3DDevice9* pd3dDeviceFromApp, bool bForceRecreate, bool bClipWindowToSingleAdapter)
{
    HRESULT hr;
    DXUTDeviceSettings* pOldDeviceSettings = GetDXUTState().GetCurrentDeviceSettings();

    if(DXUTGetD3DObject() == NULL)
        return S_FALSE;

    // Make a copy of the pNewDeviceSettings on the heap
    DXUTDeviceSettings* pNewDeviceSettingsOnHeap = new DXUTDeviceSettings;
    if(pNewDeviceSettingsOnHeap == NULL)
        return E_OUTOFMEMORY;
    memcpy(pNewDeviceSettingsOnHeap, pNewDeviceSettings, sizeof(DXUTDeviceSettings));
    pNewDeviceSettings = pNewDeviceSettingsOnHeap;

    GetDXUTState().SetCurrentDeviceSettings(pNewDeviceSettings);

    // Only apply the cmd line overrides if this is the first device created
    // and DXUTSetDevice() isn't used
    if(NULL == pd3dDeviceFromApp && NULL == pOldDeviceSettings)
    {
        // Updates the device settings struct based on the cmd line args.  
        // Warning: if the device doesn't support these new settings then CreateDevice() will fail.
        DXUTUpdateDeviceSettingsWithOverrides(pNewDeviceSettings);
    }

    // Take note if the backbuffer width & height are 0 now as they will change after pd3dDevice->Reset()
    bool bKeepCurrentWindowSize = false;
    if(pNewDeviceSettings->pp.BackBufferWidth == 0 && pNewDeviceSettings->pp.BackBufferHeight == 0)
        bKeepCurrentWindowSize = true;

    //////////////////////////
    // Before reset
    /////////////////////////
    if(pNewDeviceSettings->pp.Windowed)
    {
        // Going to windowed mode

        if(pOldDeviceSettings && !pOldDeviceSettings->pp.Windowed)
        {
            // Going from fullscreen -> windowed
            GetDXUTState().SetFullScreenBackBufferWidthAtModeChange(pOldDeviceSettings->pp.BackBufferWidth);
            GetDXUTState().SetFullScreenBackBufferHeightAtModeChange(pOldDeviceSettings->pp.BackBufferHeight);

            // Restore windowed mode style
            SetWindowLong(DXUTGetHWNDDeviceWindowed(), GWL_STYLE, GetDXUTState().GetWindowedStyleAtModeChange());
        }

        // If different device windows are used for windowed mode and fullscreen mode,
        // hide the fullscreen window so that it doesn't obscure the screen.
        if(DXUTGetHWNDDeviceFullScreen() != DXUTGetHWNDDeviceWindowed())
            ShowWindow(DXUTGetHWNDDeviceFullScreen(), SW_HIDE);
    }
    else 
    {
        // Going to fullscreen mode

        if(pOldDeviceSettings == NULL || (pOldDeviceSettings && pOldDeviceSettings->pp.Windowed))
        {
            // Transistioning to full screen mode from a standard window so 
            // save current window position/size/style now in case the user toggles to windowed mode later 
            WINDOWPLACEMENT* pwp = GetDXUTState().GetWindowedPlacement();
            ZeroMemory(pwp, sizeof(WINDOWPLACEMENT));
            pwp->length = sizeof(WINDOWPLACEMENT);
            GetWindowPlacement(DXUTGetHWNDDeviceWindowed(), pwp);
            bool bIsTopmost = ((GetWindowLong(DXUTGetHWNDDeviceWindowed(),GWL_EXSTYLE) & WS_EX_TOPMOST) != 0); 
            GetDXUTState().SetTopmostWhileWindowed(bIsTopmost);
            DWORD dwStyle = GetWindowLong(DXUTGetHWNDDeviceWindowed(), GWL_STYLE);
            dwStyle &= ~WS_MAXIMIZE & ~WS_MINIMIZE; // remove minimize/maximize style
            GetDXUTState().SetWindowedStyleAtModeChange(dwStyle);
            if(pOldDeviceSettings)
            {
                GetDXUTState().SetWindowBackBufferWidthAtModeChange(pOldDeviceSettings->pp.BackBufferWidth);
                GetDXUTState().SetWindowBackBufferHeightAtModeChange(pOldDeviceSettings->pp.BackBufferHeight);
            }
        }

        // Hide the window to avoid animation of blank windows
        ShowWindow(DXUTGetHWNDDeviceFullScreen(), SW_HIDE);

        // Set FS window style
        SetWindowLong(DXUTGetHWNDDeviceFullScreen(), GWL_STYLE, WS_POPUP|WS_SYSMENU);

        WINDOWPLACEMENT wpFullscreen;
        ZeroMemory(&wpFullscreen, sizeof(WINDOWPLACEMENT));
        wpFullscreen.length = sizeof(WINDOWPLACEMENT);
        GetWindowPlacement(DXUTGetHWNDDeviceFullScreen(), &wpFullscreen);
        if((wpFullscreen.flags & WPF_RESTORETOMAXIMIZED) != 0)
        {
            // Restore the window to normal if the window was maximized then minimized.  This causes the 
            // WPF_RESTORETOMAXIMIZED flag to be set which will cause SW_RESTORE to restore the 
            // window from minimized to maxmized which isn't what we want
            wpFullscreen.flags &= ~WPF_RESTORETOMAXIMIZED;
            wpFullscreen.showCmd = SW_RESTORE;
            SetWindowPlacement(DXUTGetHWNDDeviceFullScreen(), &wpFullscreen);
        }
    }

    // If AdapterOrdinal and DeviceType are the same, we can just do a Reset().
    // If they've changed, we need to do a complete device tear down/rebuild.
    // Also only allow a reset if pd3dDevice is the same as the current device 
	// 如果AdapterOrdinal 和 DeviceType 是一样的，我们只需要做一次重置。
	// 如果他们改变了，我们得把设备完全的销毁然后重建。
	// 我们也只应许当pd3dDevice跟当前设备一样时进行一次重置。
    if(!bForceRecreate && 
        (pd3dDeviceFromApp == NULL || pd3dDeviceFromApp == DXUTGetD3DDevice()) && 
        DXUTGetD3DDevice() &&
        pOldDeviceSettings &&
        pOldDeviceSettings->AdapterOrdinal == pNewDeviceSettings->AdapterOrdinal &&
        pOldDeviceSettings->DeviceType == pNewDeviceSettings->DeviceType &&
        pOldDeviceSettings->BehaviorFlags == pNewDeviceSettings->BehaviorFlags)
    {
		RECT rcClient;
		GetClientRect(DXUTGetHWNDDeviceWindowed(), &rcClient);
		pNewDeviceSettings->pp.BackBufferWidth=(UINT)(rcClient.right - rcClient.left);
		pNewDeviceSettings->pp.BackBufferHeight=(UINT)(rcClient.bottom - rcClient.top);
		 GetDXUTState().SetDeviceLost(true);
        // Reset the Direct3D device and call the app's device callbacks
       hr = DXUTReset3DEnvironment();
        if(FAILED(hr))
        {
			
			
            if(D3DERR_DEVICELOST == hr)
            {
                // The device is lost, just mark it as so and continue on with 
                // capturing the state and resizing the window/etc.
                GetDXUTState().SetDeviceLost(true);
            } 
            else if(DXUTERR_RESETTINGDEVICEOBJECTS == hr || 
                     DXUTERR_MEDIANOTFOUND == hr)
            {
                // Something bad happened in the app callbacks
                S_DEL(pOldDeviceSettings);
                DXUTDisplayErrorMessage(hr);
                DXUTShutdown();
                return hr;
            }
            else // DXUTERR_RESETTINGDEVICE
            {
                // Reset failed and the device wasn't lost and it wasn't the apps fault, 
                // so recreate the device to try to recover
                GetDXUTState().SetCurrentDeviceSettings(pOldDeviceSettings);
                if(FAILED(DXUTChangeDevice(pNewDeviceSettings, pd3dDeviceFromApp, true, bClipWindowToSingleAdapter)))
                {
                    // If that fails, then shutdown
                    S_DEL(pOldDeviceSettings);
                    DXUTShutdown();
                    return DXUTERR_CREATINGDEVICE;
                }
                else
                {
					// 这里不知道有什么错
                    S_DEL(pOldDeviceSettings);
                    return S_OK;
                }
            }
        }
    }
    else
    {
        // Cleanup if not first device created
        if(pOldDeviceSettings)
            DXUTCleanup3DEnvironment(false);

        // Create the D3D device and call the app's device callbacks
        hr = DXUTCreate3DEnvironment(pd3dDeviceFromApp);
        if(FAILED(hr))
        {
            S_DEL(pOldDeviceSettings);
            DXUTCleanup3DEnvironment();
            DXUTDisplayErrorMessage(hr);
            return hr;
        }
    }

    IDirect3D9* pD3D = DXUTGetD3DObject();
    HMONITOR hAdapterMonitor = pD3D->GetAdapterMonitor(pNewDeviceSettings->AdapterOrdinal);
    GetDXUTState().SetAdapterMonitor(hAdapterMonitor);

    // Update the device stats text
    DXUTUpdateStaticFrameStats();

    if(pOldDeviceSettings && !pOldDeviceSettings->pp.Windowed && pNewDeviceSettings->pp.Windowed)
    {
        // Going from fullscreen -> windowed

        // Restore the show state, and positions/size of the window to what it was
        // It is important to adjust the window size 
        // after resetting the device rather than beforehand to ensure 
        // that the monitor resolution is correct and does not limit the size of the new window.
        WINDOWPLACEMENT* pwp = GetDXUTState().GetWindowedPlacement();
        SetWindowPlacement(DXUTGetHWNDDeviceWindowed(), pwp);

        // Also restore the z-order of window to previous state
        HWND hWndInsertAfter = GetDXUTState().GetTopmostWhileWindowed() ? HWND_TOPMOST : HWND_NOTOPMOST;
        SetWindowPos(DXUTGetHWNDDeviceWindowed(), hWndInsertAfter, 0, 0, 0, 0, SWP_NOMOVE|SWP_NOREDRAW|SWP_NOSIZE);
    }

    // Check to see if the window needs to be resized.  
    // Handle cases where the window is minimized and maxmimized as well.
    bool bNeedToResize = false;
    if(pNewDeviceSettings->pp.Windowed && // only resize if in windowed mode
        !bKeepCurrentWindowSize)          // only resize if pp.BackbufferWidth/Height were not 0
    {
        UINT nClientWidth;
        UINT nClientHeight;    
        if(IsIconic(DXUTGetHWNDDeviceWindowed()))
        {
			//最小化的时候 不用管它
            // Window is restored or maximized so just get its client rect
            RECT rcClient;
            GetClientRect(DXUTGetHWNDDeviceWindowed(), &rcClient);
            nClientWidth  = (UINT)(rcClient.right - rcClient.left);
            nClientHeight = (UINT)(rcClient.bottom - rcClient.top);
        }
        else
        {
            // Window is restored or maximized so just get its client rect
            RECT rcClient;
            GetClientRect(DXUTGetHWNDDeviceWindowed(), &rcClient);
            nClientWidth  = (UINT)(rcClient.right - rcClient.left);
            nClientHeight = (UINT)(rcClient.bottom - rcClient.top);
        }

        // Now that we know the client rect, compare it against the back buffer size
        // to see if the client rect is already the right size
        if(nClientWidth  != pNewDeviceSettings->pp.BackBufferWidth ||
            nClientHeight != pNewDeviceSettings->pp.BackBufferHeight)
        {
            bNeedToResize = true;
        }       

        if(bClipWindowToSingleAdapter && !IsIconic(DXUTGetHWNDDeviceWindowed()))
        {
            // Get the rect of the monitor attached to the adapter
            MONITORINFO miAdapter;
            miAdapter.cbSize = sizeof(MONITORINFO);
            HMONITOR hAdapterMonitor = DXUTGetD3DObject()->GetAdapterMonitor(pNewDeviceSettings->AdapterOrdinal);
            DXUTGetMonitorInfo(hAdapterMonitor, &miAdapter);
            HMONITOR hWindowMonitor = DXUTMonitorFromWindow(DXUTGetHWND(), MONITOR_DEFAULTTOPRIMARY);

            // Get the rect of the window
            RECT rcWindow;
            GetWindowRect(DXUTGetHWNDDeviceWindowed(), &rcWindow);

            // Check if the window rect is fully inside the adapter's vitural screen rect
            if((rcWindow.left   < miAdapter.rcWork.left  ||
                 rcWindow.right  > miAdapter.rcWork.right ||
                 rcWindow.top    < miAdapter.rcWork.top   ||
                 rcWindow.bottom > miAdapter.rcWork.bottom))
            {
                if(hWindowMonitor == hAdapterMonitor && IsZoomed(DXUTGetHWNDDeviceWindowed()))
                {
                    // If the window is maximized and on the same monitor as the adapter, then 
                    // no need to clip to single adapter as the window is already clipped 
                    // even though the rcWindow rect is outside of the miAdapter.rcWork
                }
                else
                {
                    bNeedToResize = true;
                }
            }
        }
    }

    // Only resize window if needed 
    if(bNeedToResize) 
    {
        // Need to resize, so if window is maximized or minimized then restore the window
        if(IsIconic(DXUTGetHWNDDeviceWindowed())) 
            ShowWindow(DXUTGetHWNDDeviceWindowed(), SW_RESTORE);
        if(IsZoomed(DXUTGetHWNDDeviceWindowed())) // doing the IsIconic() check first also handles the WPF_RESTORETOMAXIMIZED case
            ShowWindow(DXUTGetHWNDDeviceWindowed(), SW_RESTORE);

        if(bClipWindowToSingleAdapter)
        {
            // Get the rect of the monitor attached to the adapter
            MONITORINFO miAdapter;
            miAdapter.cbSize = sizeof(MONITORINFO);
            DXUTGetMonitorInfo(DXUTGetD3DObject()->GetAdapterMonitor(pNewDeviceSettings->AdapterOrdinal), &miAdapter);

            // Get the rect of the monitor attached to the window
            MONITORINFO miWindow;
            miWindow.cbSize = sizeof(MONITORINFO);
            DXUTGetMonitorInfo(DXUTMonitorFromWindow(DXUTGetHWND(), MONITOR_DEFAULTTOPRIMARY), &miWindow);

            // Do something reasonable if the BackBuffer size is greater than the monitor size
            int nAdapterMonitorWidth = miAdapter.rcWork.right - miAdapter.rcWork.left;
            int nAdapterMonitorHeight = miAdapter.rcWork.bottom - miAdapter.rcWork.top;

            int nClientWidth = pNewDeviceSettings->pp.BackBufferWidth;
            int nClientHeight = pNewDeviceSettings->pp.BackBufferHeight;

            // Get the rect of the window
            RECT rcWindow;
            GetWindowRect(DXUTGetHWNDDeviceWindowed(), &rcWindow);

            // Make a window rect with a client rect that is the same size as the backbuffer
            RECT rcResizedWindow;
            rcResizedWindow.left = 0;
            rcResizedWindow.right = nClientWidth;
            rcResizedWindow.top = 0;
            rcResizedWindow.bottom = nClientHeight;
            AdjustWindowRect(&rcResizedWindow, GetWindowLong(DXUTGetHWNDDeviceWindowed(), GWL_STYLE), false);

            int nWindowWidth = rcResizedWindow.right - rcResizedWindow.left;
            int nWindowHeight = rcResizedWindow.bottom - rcResizedWindow.top;

            if(nWindowWidth > nAdapterMonitorWidth)
                nWindowWidth = (nAdapterMonitorWidth - 0);
            if(nWindowHeight > nAdapterMonitorHeight)
                nWindowHeight = (nAdapterMonitorHeight - 0);

            if(rcResizedWindow.left < miAdapter.rcWork.left ||
                rcResizedWindow.top < miAdapter.rcWork.top ||
                rcResizedWindow.right > miAdapter.rcWork.right ||
                rcResizedWindow.bottom > miAdapter.rcWork.bottom)
            {
                int nWindowOffsetX = (nAdapterMonitorWidth - nWindowWidth) / 2;
                int nWindowOffsetY = (nAdapterMonitorHeight - nWindowHeight) / 2;

                rcResizedWindow.left = miAdapter.rcWork.left + nWindowOffsetX;
                rcResizedWindow.top = miAdapter.rcWork.top + nWindowOffsetY;
                rcResizedWindow.right = miAdapter.rcWork.left + nWindowOffsetX + nWindowWidth;
                rcResizedWindow.bottom = miAdapter.rcWork.top + nWindowOffsetY + nWindowHeight;
            }

            // Resize the window.  It is important to adjust the window size 
            // after resetting the device rather than beforehand to ensure 
            // that the monitor resolution is correct and does not limit the size of the new window.
            SetWindowPos(DXUTGetHWNDDeviceWindowed(), 0, rcResizedWindow.left, rcResizedWindow.top, nWindowWidth, nWindowHeight, SWP_NOZORDER);
        }        
        else
        {      
            // Make a window rect with a client rect that is the same size as the backbuffer
            RECT rcWindow = {0};
            rcWindow.right = (long)(pNewDeviceSettings->pp.BackBufferWidth);
            rcWindow.bottom = (long)(pNewDeviceSettings->pp.BackBufferHeight);
            AdjustWindowRect(&rcWindow, GetWindowLong(DXUTGetHWNDDeviceWindowed(), GWL_STYLE), false);

            // Resize the window.  It is important to adjust the window size 
            // after resetting the device rather than beforehand to ensure 
            // that the monitor resolution is correct and does not limit the size of the new window.
            int cx = (int)(rcWindow.right - rcWindow.left);
            int cy = (int)(rcWindow.bottom - rcWindow.top);
            SetWindowPos(DXUTGetHWNDDeviceWindowed(), 0, 0, 0, cx, cy, SWP_NOZORDER|SWP_NOMOVE);
        }

        // Its possible that the new window size is not what we asked for.  
        // No window can be sized larger than the desktop, so see see if the Windows OS resized the 
        // window to something smaller to fit on the desktop.  Also if WM_GETMINMAXINFO
        // will put a limit on the smallest/largest window size.
        RECT rcClient;
        GetClientRect(DXUTGetHWNDDeviceWindowed(), &rcClient);
        UINT nClientWidth  = (UINT)(rcClient.right - rcClient.left);
        UINT nClientHeight = (UINT)(rcClient.bottom - rcClient.top);
        if(nClientWidth  != pNewDeviceSettings->pp.BackBufferWidth ||
            nClientHeight != pNewDeviceSettings->pp.BackBufferHeight)
        {
            // If its different, then resize the backbuffer again.  This time create a backbuffer that matches the 
            // client rect of the current window w/o resizing the window.
            DXUTDeviceSettings deviceSettings = DXUTGetDeviceSettings();
            deviceSettings.pp.BackBufferWidth  = 0; 
            deviceSettings.pp.BackBufferHeight = 0;
            hr = DXUTChangeDevice(&deviceSettings, NULL, false, bClipWindowToSingleAdapter);
            if(FAILED(hr))
            {
                S_DEL(pOldDeviceSettings);
                DXUTCleanup3DEnvironment();
                return hr;
            }
        }
    }

    // Make the window visible
    if(!IsWindowVisible(DXUTGetHWND()))
        ShowWindow(DXUTGetHWND(), SW_SHOW);

    // Ensure that the display doesn't power down when fullscreen but does when windowed
    if(!DXUTIsWindowed())
        SetThreadExecutionState(ES_DISPLAY_REQUIRED | ES_CONTINUOUS); 
    else
        SetThreadExecutionState(ES_CONTINUOUS);   

    S_DEL(pOldDeviceSettings);
    GetDXUTState().SetDeviceCreated(true);

    return S_OK;
}


// Enables/disables Windows keys, and disables or restores the StickyKeys/ToggleKeys/FilterKeys 
// shortcut to help prevent accidental task switching



// Updates the device settings struct based on the cmd line args.  

void DXUTUpdateDeviceSettingsWithOverrides(DXUTDeviceSettings* pDeviceSettings)
{
    if(GetDXUTState().GetOverrideAdapterOrdinal() != -1)
        pDeviceSettings->AdapterOrdinal = GetDXUTState().GetOverrideAdapterOrdinal();

    if(GetDXUTState().GetOverrideFullScreen())
        pDeviceSettings->pp.Windowed = false;
    if(GetDXUTState().GetOverrideWindowed())
        pDeviceSettings->pp.Windowed = true;

    pDeviceSettings->DeviceType = D3DDEVTYPE_HAL;

    if(GetDXUTState().GetOverrideWidth() != 0)
        pDeviceSettings->pp.BackBufferWidth = GetDXUTState().GetOverrideWidth();
    if(GetDXUTState().GetOverrideHeight() != 0)
        pDeviceSettings->pp.BackBufferHeight = GetDXUTState().GetOverrideHeight();

    if(GetDXUTState().GetOverrideForcePureHWVP())
    {
        pDeviceSettings->BehaviorFlags &= ~D3DCREATE_SOFTWARE_VERTEXPROCESSING;
        pDeviceSettings->BehaviorFlags |= D3DCREATE_HARDWARE_VERTEXPROCESSING;
        pDeviceSettings->BehaviorFlags |= D3DCREATE_PUREDEVICE;
    }
    else
    {
        pDeviceSettings->BehaviorFlags &= ~D3DCREATE_SOFTWARE_VERTEXPROCESSING;
        pDeviceSettings->BehaviorFlags &= ~D3DCREATE_PUREDEVICE;
        pDeviceSettings->BehaviorFlags |= D3DCREATE_HARDWARE_VERTEXPROCESSING;
    }
}



// Creates the 3D environment

HRESULT DXUTCreate3DEnvironment(IDirect3DDevice9* pd3dDeviceFromApp)
{
    HRESULT hr = S_OK;

    IDirect3DDevice9* pd3dDevice = NULL;
    DXUTDeviceSettings* pNewDeviceSettings = GetDXUTState().GetCurrentDeviceSettings();

    // Only create a Direct3D device if one hasn't been supplied by the app
    if(pd3dDeviceFromApp == NULL)
    {
        // Try to create the device with the chosen settings
        IDirect3D9* pD3D = DXUTGetD3DObject();
        hr = pD3D->CreateDevice(pNewDeviceSettings->AdapterOrdinal, pNewDeviceSettings->DeviceType, 
                                 DXUTGetHWNDFocus(), pNewDeviceSettings->BehaviorFlags,
                                 &pNewDeviceSettings->pp, &pd3dDevice);
        if(hr == D3DERR_DEVICELOST) 
        {
            GetDXUTState().SetDeviceLost(true);
            return S_OK;
        }
        else if(FAILED(hr))
        {
            DXUT_ERR(L"CreateDevice", hr);
            return DXUTERR_CREATINGDEVICE;
        }
    }
    else
    {
        pd3dDeviceFromApp->AddRef();
        pd3dDevice = pd3dDeviceFromApp;
    }

    GetDXUTState().SetD3DDevice(pd3dDevice);

    // If switching to REF, set the exit code to 11.  If switching to HAL and exit code was 11, then set it back to 0.
    if(pNewDeviceSettings->DeviceType == D3DDEVTYPE_REF && GetDXUTState().GetExitCode() == 0)
        GetDXUTState().SetExitCode(11);
    else if(pNewDeviceSettings->DeviceType == D3DDEVTYPE_HAL && GetDXUTState().GetExitCode() == 11)
        GetDXUTState().SetExitCode(0);

    // Update back buffer desc before calling app's device callbacks
    DXUTUpdateBackBufferDesc();

    // Setup cursor based on current settings (window/fullscreen mode, show cursor state, clip cursor state)
    DXUTSetupCursor();

    // Update GetDXUTState()'s copy of D3D caps 
    D3DCAPS9* pd3dCaps = GetDXUTState().GetCaps();
    DXUTGetD3DDevice()->GetDeviceCaps(pd3dCaps);

    // Update the device stats text
    CD3DEnumeration* pd3dEnum = DXUTPrepareEnumerationObject();
    CD3DEnumAdapterInfo* pAdapterInfo = pd3dEnum->GetAdapterInfo(pNewDeviceSettings->AdapterOrdinal);
    DXUTUpdateDeviceStats(pNewDeviceSettings->DeviceType, 
                        pNewDeviceSettings->BehaviorFlags, 
                        &pAdapterInfo->AdapterIdentifier);

    // Call the app's device created callback if non-NULL

    const D3DSURFACE_DESC* pbackBufferSurfaceDesc = DXUTGetBackBufferSurfaceDesc();
	hr = S_OK;
	{
		//hr = GetRoot().OnCreateDevice(DXUTGetD3DDevice());
	}
    if(DXUTGetD3DDevice() == NULL) // Handle DXUTShutdown from inside callback
        return E_FAIL;
    if(FAILED(hr))  
    {
        DXUT_ERR(L"DeviceCreated callback", hr);        
        return (hr == DXUTERR_MEDIANOTFOUND) ? DXUTERR_MEDIANOTFOUND : DXUTERR_CREATINGDEVICEOBJECTS;
    }

    // Call the app's device reset callback if non-NULL
	GetD3D9RenderSystem().OnResetDevice();

    if(DXUTGetD3DDevice() == NULL) // Handle DXUTShutdown from inside callback
        return E_FAIL;
    if(FAILED(hr))
    {
        DXUT_ERR(L"DeviceReset callback", hr);
        return (hr == DXUTERR_MEDIANOTFOUND) ? DXUTERR_MEDIANOTFOUND : DXUTERR_RESETTINGDEVICEOBJECTS;
    }

    return S_OK;
}



// Resets the 3D environment by:
//      - Calls the device lost callback 
//      - Resets the device
//      - Stores the back buffer description
//      - Sets up the full screen Direct3D cursor if requested
//      - Calls the device reset callback 

//#include "dxerr9.h"
HRESULT DXUTReset3DEnvironment()
{
    HRESULT hr;

    IDirect3DDevice9* pd3dDevice = DXUTGetD3DDevice();
    assert(pd3dDevice != NULL);

    // Call the app's device lost callback
	GetD3D9RenderSystem().OnLostDevice();

    // Reset the device
    DXUTDeviceSettings* pDeviceSettings = GetDXUTState().GetCurrentDeviceSettings();

    hr = pd3dDevice->Reset(&pDeviceSettings->pp);
    if(FAILED(hr))  
    {
		std::wstring msg = DXGetErrorDescription(hr);
        if(hr == D3DERR_DEVICELOST)
            return D3DERR_DEVICELOST; // Reset could legitimately fail if the device is lost
        else
            return DXUT_ERR(L"Reset", DXUTERR_RESETTINGDEVICE);
    }

    // Update back buffer desc before calling app's device callbacks
    DXUTUpdateBackBufferDesc();

    // Setup cursor based on current settings (window/fullscreen mode, show cursor state, clip cursor state)
    DXUTSetupCursor();

    // Call the app's OnDeviceReset callback
    const D3DSURFACE_DESC* pbackBufferSurfaceDesc = DXUTGetBackBufferSurfaceDesc();
	{
		GetD3D9RenderSystem().OnResetDevice(); //DXUTGetD3DDevice()
	}
    if(FAILED(hr))
    {
        // If callback failed, cleanup
        DXUT_ERR(L"DeviceResetCallback", hr);
        if(hr != DXUTERR_MEDIANOTFOUND)
            hr = DXUTERR_RESETTINGDEVICEOBJECTS;

		{
			GetD3D9RenderSystem().OnLostDevice();
		}

        return hr;
    }

    return S_OK;
}

// Checks if the window client rect has changed and if it has, then reset the device

void DXUTCheckForWindowSizeChange()
{
    // Skip the check for various reasons
    if( !GetDXUTState().GetDeviceCreated() || 
        !GetDXUTState().GetCurrentDeviceSettings()->pp.Windowed)
        return;

    RECT rcCurrentClient;
    GetClientRect(DXUTGetHWND(), &rcCurrentClient);

    if((UINT)rcCurrentClient.right != GetDXUTState().GetCurrentDeviceSettings()->pp.BackBufferWidth ||
        (UINT)rcCurrentClient.bottom != GetDXUTState().GetCurrentDeviceSettings()->pp.BackBufferHeight)
    {
        // A new window size will require a new backbuffer size
        // size, so the device must be reset and the D3D structures updated accordingly.

        // Tell DXUTChangeDevice and D3D to size according to the HWND's client rect
        DXUTDeviceSettings deviceSettings = DXUTGetDeviceSettings();
        deviceSettings.pp.BackBufferWidth  = 0; 
        deviceSettings.pp.BackBufferHeight = 0;
        DXUTChangeDevice(&deviceSettings, NULL, false, false);
    }
}

// Updates the string which describes the device 

void DXUTUpdateDeviceStats(D3DDEVTYPE DeviceType, DWORD BehaviorFlags, D3DADAPTER_IDENTIFIER9* pAdapterIdentifier)
{
    if(GetDXUTState().GetNoStats())
        return;

    // Store device description
    WCHAR* pstrDeviceStats = GetDXUTState().GetDeviceStats();

    if(BehaviorFlags & D3DCREATE_HARDWARE_VERTEXPROCESSING &&
        BehaviorFlags & D3DCREATE_PUREDEVICE)
    {
        if(DeviceType == D3DDEVTYPE_HAL)
            StringCchCat(pstrDeviceStats, 256, L" (pure hw vp)");
        else
            StringCchCat(pstrDeviceStats, 256, L" (simulated pure hw vp)");
    }
    else if(BehaviorFlags & D3DCREATE_HARDWARE_VERTEXPROCESSING)
    {
        if(DeviceType == D3DDEVTYPE_HAL)
            StringCchCat(pstrDeviceStats, 256, L" (hw vp)");
        else
            StringCchCat(pstrDeviceStats, 256, L" (simulated hw vp)");
    }
    else if(BehaviorFlags & D3DCREATE_MIXED_VERTEXPROCESSING)
    {
        if(DeviceType == D3DDEVTYPE_HAL)
            StringCchCat(pstrDeviceStats, 256, L" (mixed vp)");
        else
            StringCchCat(pstrDeviceStats, 256, L" (simulated mixed vp)");
    }
    else if(BehaviorFlags & D3DCREATE_SOFTWARE_VERTEXPROCESSING)
    {
        StringCchCat(pstrDeviceStats, 256, L" (sw vp)");
    }

    if(DeviceType == D3DDEVTYPE_HAL)
    {
        // Be sure not to overflow m_strDeviceStats when appending the adapter 
        // description, since it can be long.  
        StringCchCat(pstrDeviceStats, 256, L": ");

        // Try to get a unique description from the CD3DEnumDeviceSettingsCombo
        DXUTDeviceSettings* pDeviceSettings = GetDXUTState().GetCurrentDeviceSettings();
        CD3DEnumeration* pd3dEnum = DXUTPrepareEnumerationObject();
        CD3DEnumDeviceSettingsCombo* pDeviceSettingsCombo = pd3dEnum->GetDeviceSettingsCombo(pDeviceSettings->AdapterOrdinal, pDeviceSettings->DeviceType, pDeviceSettings->AdapterFormat, pDeviceSettings->pp.BackBufferFormat, pDeviceSettings->pp.Windowed);
        if(pDeviceSettingsCombo)
        {
            StringCchCat(pstrDeviceStats, 256, pDeviceSettingsCombo->pAdapterInfo->szUniqueDescription);
        }
        else
        {
            const int cchDesc = sizeof(pAdapterIdentifier->Description);
            WCHAR szDescription[cchDesc];
            MultiByteToWideChar(CP_ACP, 0, pAdapterIdentifier->Description, -1, szDescription, cchDesc);
            szDescription[cchDesc-1] = 0;
            StringCchCat(pstrDeviceStats, 256, szDescription);
        }
    }
}

// Updates the static part of the frame stats so it doesn't have be generated every frame

void DXUTUpdateStaticFrameStats()
{
    if(GetDXUTState().GetNoStats())
        return;

    DXUTDeviceSettings* pDeviceSettings = GetDXUTState().GetCurrentDeviceSettings();
    if(NULL == pDeviceSettings)
        return;
    CD3DEnumeration* pd3dEnum = DXUTPrepareEnumerationObject();
    if(NULL == pd3dEnum)
        return;

    CD3DEnumDeviceSettingsCombo* pDeviceSettingsCombo = pd3dEnum->GetDeviceSettingsCombo(pDeviceSettings->AdapterOrdinal, pDeviceSettings->DeviceType, pDeviceSettings->AdapterFormat, pDeviceSettings->pp.BackBufferFormat, pDeviceSettings->pp.Windowed);
    if(NULL == pDeviceSettingsCombo)
        return;

    WCHAR strFmt[100];
    D3DPRESENT_PARAMETERS* pPP = &pDeviceSettings->pp;

    if(pDeviceSettingsCombo->AdapterFormat == pDeviceSettingsCombo->BackBufferFormat)
    {
        StringCchCopy(strFmt, 100, DXUTD3DFormatToString(pDeviceSettingsCombo->AdapterFormat, false));
    }
    else
    {
        StringCchPrintf(strFmt, 100, L"backbuf %s, adapter %s", 
            DXUTD3DFormatToString(pDeviceSettingsCombo->BackBufferFormat, false), 
            DXUTD3DFormatToString(pDeviceSettingsCombo->AdapterFormat, false));
    }

    WCHAR strDepthFmt[100];
    if(pPP->EnableAutoDepthStencil)
    {
        StringCchPrintf(strDepthFmt, 100, L" (%s)", DXUTD3DFormatToString(pPP->AutoDepthStencilFormat, false));
    }
    else
    {
        // No depth buffer
        strDepthFmt[0] = 0;
    }

    WCHAR strMultiSample[100];
    switch(pPP->MultiSampleType)
    {
        case D3DMULTISAMPLE_NONMASKABLE: StringCchCopy(strMultiSample, 100, L" (Nonmaskable Multisample)"); break;
        case D3DMULTISAMPLE_NONE:        StringCchCopy(strMultiSample, 100, L""); break;
        default:                         StringCchPrintf(strMultiSample, 100, L" (%dx Multisample)", pPP->MultiSampleType); break;
    }

    WCHAR* pstrStaticFrameStats = GetDXUTState().GetStaticFrameStats();
    StringCchPrintf(pstrStaticFrameStats, 256, L"%%s\nVsync %s\n(%dx%d)\n%s%s%s", //%%sVsync %s (%dx%d), %s%s%s
                (pPP->PresentationInterval == D3DPRESENT_INTERVAL_IMMEDIATE) ? L"off" : L"on", 
                pPP->BackBufferWidth, pPP->BackBufferHeight,
                strFmt, strDepthFmt, strMultiSample);
}

// Handles window messages 


// Resets the state associated with DXUT 

void DXUTResetFrameworkState()
{
    GetDXUTState().Destroy();
    GetDXUTState().Create();
}



// Closes down the window.  When the window closes, it will cleanup everything

void DXUTShutdown(int nExitCode)
{
    HWND hWnd = DXUTGetHWND();
    if(hWnd != NULL)
        SendMessage(hWnd, WM_CLOSE, 0, 0);

    GetDXUTState().SetExitCode(nExitCode);

    DXUTCleanup3DEnvironment(true);

    GetDXUTState().SetD3DEnumeration(NULL);

    IDirect3D9* pD3D = DXUTGetD3DObject();
    S_REL(pD3D);
    GetDXUTState().SetD3D(NULL);

}



// Cleans up the 3D environment by:
//      - Calls the device lost callback 
//      - Calls the device destroyed callback 
//      - Releases the D3D device

void DXUTCleanup3DEnvironment(bool bReleaseSettings)
{
    IDirect3DDevice9* pd3dDevice = DXUTGetD3DDevice();
    if(pd3dDevice != NULL)
    {
		GetD3D9RenderSystem().OnLostDevice();
		GetD3D9RenderSystem().OnDestroyDevice();

        // Release the D3D device and in debug configs, displays a message box if there 
        // are unrelease objects.
        if(pd3dDevice)
        {
            if(pd3dDevice->Release() > 0)  
            {
                DXUTDisplayErrorMessage(DXUTERR_NONZEROREFCOUNT);
                DXUT_ERR(L"DXUTCleanup3DEnvironment", DXUTERR_NONZEROREFCOUNT);
            }
        }
        GetDXUTState().SetD3DDevice(NULL);

        if(bReleaseSettings)
        {
            DXUTDeviceSettings* pOldDeviceSettings = GetDXUTState().GetCurrentDeviceSettings();
            S_DEL(pOldDeviceSettings);  
            GetDXUTState().SetCurrentDeviceSettings(NULL);
        }

        D3DSURFACE_DESC* pbackBufferSurfaceDesc = GetDXUTState().GetBackBufferSurfaceDesc();
        ZeroMemory(pbackBufferSurfaceDesc, sizeof(D3DSURFACE_DESC));

        D3DCAPS9* pd3dCaps = GetDXUTState().GetCaps();
        ZeroMemory(pd3dCaps, sizeof(D3DCAPS9));

        GetDXUTState().SetDeviceCreated(false);
    }
}



// Stores back buffer surface desc in GetDXUTState().GetBackBufferSurfaceDesc()
void DXUTUpdateBackBufferDesc()
{
    HRESULT hr;
    IDirect3DSurface9* pBackBuffer;
    hr = GetDXUTState().GetD3DDevice()->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &pBackBuffer);
    D3DSURFACE_DESC* pBBufferSurfaceDesc = GetDXUTState().GetBackBufferSurfaceDesc();
    ZeroMemory(pBBufferSurfaceDesc, sizeof(D3DSURFACE_DESC));
    if(SUCCEEDED(hr))
    {
        pBackBuffer->GetDesc(pBBufferSurfaceDesc);
        S_REL(pBackBuffer);
    }
}

// External state access functions

IDirect3D9* DXUTGetD3DObject()                      { return GetDXUTState().GetD3D(); }        
IDirect3DDevice9* DXUTGetD3DDevice()                { return GetDXUTState().GetD3DDevice(); }  
const D3DSURFACE_DESC* DXUTGetBackBufferSurfaceDesc() { return GetDXUTState().GetBackBufferSurfaceDesc(); }
HWND DXUTGetHWND()                                  { return DXUTIsWindowed() ? GetDXUTState().GetHWNDDeviceWindowed() : GetDXUTState().GetHWNDDeviceFullScreen(); }
HWND DXUTGetHWNDFocus()                             { return GetDXUTState().GetHWNDFocus(); }
HWND DXUTGetHWNDDeviceFullScreen()                  { return GetDXUTState().GetHWNDDeviceFullScreen(); }
HWND DXUTGetHWNDDeviceWindowed()                    { return GetDXUTState().GetHWNDDeviceWindowed(); }
LPCWSTR DXUTGetDeviceStats()                        { return GetDXUTState().GetDeviceStats(); }
int DXUTGetExitCode()                               { return GetDXUTState().GetExitCode(); }

// Return if windowed in the current device.  If no device exists yet, then returns false

bool DXUTIsWindowed()                               
{ 
    DXUTDeviceSettings* pDeviceSettings = GetDXUTState().GetCurrentDeviceSettings(); 
    if(pDeviceSettings) 
        return (pDeviceSettings->pp.Windowed != 0); 
    else 
        return false; 
}

// Return the device settings of the current device.  If no device exists yet, then
// return blank device settings 

DXUTDeviceSettings DXUTGetDeviceSettings()   
{ 
    DXUTDeviceSettings* pDS = GetDXUTState().GetCurrentDeviceSettings();
    if(pDS)
    {
        return *pDS;
    }
    else
    {
        DXUTDeviceSettings ds;
        ZeroMemory(&ds, sizeof(DXUTDeviceSettings));
        return ds;
    }
} 

#ifndef SM_REMOTESESSION  // needs WINVER >= 0x0500
#define SM_REMOTESESSION  0x1000
#endif



// Display an custom error msg box 

void DXUTDisplayErrorMessage(HRESULT hr)
{
    WCHAR strBuffer[512];

    int nExitCode;
    bool bFound = true; 
    switch(hr)
    {
        case DXUTERR_NODIRECT3D:             nExitCode = 2; StringCchCopy(strBuffer, 512, L"Could not initialize Direct3D. You may want to check that the latest version of DirectX is correctly installed on your system.  Also make sure that this program was compiled with header files that match the installed DirectX DLLs."); break;
        case DXUTERR_INCORRECTVERSION:       nExitCode = 10; StringCchCopy(strBuffer, 512, L"Incorrect version of Direct3D and/or D3DX."); break;
        case DXUTERR_MEDIANOTFOUND:          nExitCode = 4; StringCchCopy(strBuffer, 512, L"Could not find required media. Ensure that the DirectX SDK is correctly installed."); break;
        case DXUTERR_NONZEROREFCOUNT:        nExitCode = 5; StringCchCopy(strBuffer, 512, L"The D3D device has a non-zero reference count, meaning some objects were not released."); break;
        case DXUTERR_CREATINGDEVICE:         nExitCode = 6; StringCchCopy(strBuffer, 512, L"Failed creating the Direct3D device."); break;
        case DXUTERR_RESETTINGDEVICE:        nExitCode = 7; StringCchCopy(strBuffer, 512, L"Failed resetting the Direct3D device."); break;
        case DXUTERR_CREATINGDEVICEOBJECTS:  nExitCode = 8; StringCchCopy(strBuffer, 512, L"Failed creating Direct3D device objects."); break;
        case DXUTERR_RESETTINGDEVICEOBJECTS: nExitCode = 9; StringCchCopy(strBuffer, 512, L"Failed resetting Direct3D device objects."); break;
        case DXUTERR_NOCOMPATIBLEDEVICES:    
            nExitCode = 3; 
            if(GetSystemMetrics(SM_REMOTESESSION) != 0)
                StringCchCopy(strBuffer, 512, L"Direct3D does not work over a remote session."); 
            else
                StringCchCopy(strBuffer, 512, L"Could not find any compatible Direct3D devices."); 
            break;
        default: bFound = false; nExitCode = 1;break;
    }   

    GetDXUTState().SetExitCode(nExitCode);

    if(bFound)
    {
		MessageBox(DXUTGetHWND(), strBuffer, L"DirectX Application", MB_ICONERROR|MB_OK);
    }
}



// Display error msg box to help debug 

HRESULT WINAPI DXUTTrace(const CHAR* strFile, DWORD dwLine, HRESULT hr,
                          const WCHAR* strMsg, bool bPopMsgBox)
{
    return DXTrace(strFile, dwLine, hr, strMsg, bPopMsgBox);
}



// Checks to see if the HWND changed monitors, and if it did it creates a device 
// from the monitor's adapter and recreates the scene.

void DXUTCheckForWindowChangingMonitors()
{
    // Skip this check for various reasons
    if(!GetDXUTState().GetAutoChangeAdapter() || 
        !GetDXUTState().GetDeviceCreated() ||
        !GetDXUTState().GetCurrentDeviceSettings()->pp.Windowed)
    {
        return;
    }

    HRESULT hr;
    HMONITOR hWindowMonitor = DXUTMonitorFromWindow(DXUTGetHWND(), MONITOR_DEFAULTTOPRIMARY);
    HMONITOR hAdapterMonitor = GetDXUTState().GetAdapterMonitor();
    if(hWindowMonitor != hAdapterMonitor)
    {
        UINT newOrdinal;
        if(SUCCEEDED(DXUTGetAdapterOrdinalFromMonitor(hWindowMonitor, &newOrdinal)))
        {
            // Find the closest valid device settings with the new ordinal
            DXUTDeviceSettings deviceSettings = DXUTGetDeviceSettings();
            deviceSettings.AdapterOrdinal = newOrdinal;

            DXUTMatchOptions matchOptions;
            matchOptions.eAdapterOrdinal     = DXUTMT_PRESERVE_INPUT;
            matchOptions.eDeviceType         = DXUTMT_CLOSEST_TO_INPUT;
            matchOptions.eWindowed           = DXUTMT_CLOSEST_TO_INPUT;
            matchOptions.eAdapterFormat      = DXUTMT_CLOSEST_TO_INPUT;
            matchOptions.eVertexProcessing   = DXUTMT_CLOSEST_TO_INPUT;
            matchOptions.eResolution         = DXUTMT_CLOSEST_TO_INPUT;
            matchOptions.eBackBufferFormat   = DXUTMT_CLOSEST_TO_INPUT;
            matchOptions.eBackBufferCount    = DXUTMT_CLOSEST_TO_INPUT;
            matchOptions.eMultiSample        = DXUTMT_CLOSEST_TO_INPUT;
            matchOptions.eSwapEffect         = DXUTMT_CLOSEST_TO_INPUT;
            matchOptions.eDepthFormat        = DXUTMT_CLOSEST_TO_INPUT;
            matchOptions.eStencilFormat      = DXUTMT_CLOSEST_TO_INPUT;
            matchOptions.ePresentFlags       = DXUTMT_CLOSEST_TO_INPUT;
            matchOptions.eRefreshRate        = DXUTMT_CLOSEST_TO_INPUT;
            matchOptions.ePresentInterval    = DXUTMT_CLOSEST_TO_INPUT;

            hr = DXUTFindValidDeviceSettings(&deviceSettings, &deviceSettings, &matchOptions);
            if(SUCCEEDED(hr)) 
            {
                // Create a Direct3D device using the new device settings.  
                // If there is an existing device, then it will either reset or recreate the scene.
                hr = DXUTChangeDevice(&deviceSettings, NULL, false, false);

                // If hr == E_ABORT, this means the app rejected the device settings in the ModifySettingsCallback
                if(hr == E_ABORT)
                {
                    // so nothing changed and keep from attempting to switch adapters next time
                    GetDXUTState().SetAutoChangeAdapter(false);
                }
                else if(FAILED(hr))
                {
                    DXUTShutdown();
                    return;
                }
            }
        }
    }    
}



// Look for an adapter ordinal that is tied to a HMONITOR

HRESULT DXUTGetAdapterOrdinalFromMonitor(HMONITOR hMonitor, UINT* pAdapterOrdinal)
{
    *pAdapterOrdinal = 0;

    CD3DEnumeration* pd3dEnum = DXUTPrepareEnumerationObject();
    IDirect3D9*      pD3D     = DXUTGetD3DObject();

    CGrowableArray<CD3DEnumAdapterInfo*>* pAdapterList = pd3dEnum->GetAdapterInfoList();
    for(int iAdapter=0; iAdapter<pAdapterList->GetSize(); iAdapter++)
    {
        CD3DEnumAdapterInfo* pAdapterInfo = pAdapterList->GetAt(iAdapter);
        HMONITOR hAdapterMonitor = pD3D->GetAdapterMonitor(pAdapterInfo->AdapterOrdinal);
        if(hAdapterMonitor == hMonitor)
        {
            *pAdapterOrdinal = pAdapterInfo->AdapterOrdinal;
            return S_OK;
        }
    }

    return E_FAIL;
}

// Setup cursor based on current settings (window/fullscreen mode, show cursor state, clip cursor state)

void DXUTSetupCursor()
{
    // Show the cursor again if returning to fullscreen 
    IDirect3DDevice9* pd3dDevice = DXUTGetD3DDevice();
    if(!DXUTIsWindowed() && pd3dDevice)
    {
        if(1/*GetDXUTState().GetShowCursorWhenFullScreen()*/)
        {
            SetCursor(NULL); // Turn off Windows cursor in full screen mode
            HCURSOR hCursor = (HCURSOR)(ULONG_PTR)GetClassLongPtr(DXUTGetHWNDDeviceFullScreen(), GCLP_HCURSOR);
            DXUTSetDeviceCursor(pd3dDevice, hCursor, false);
            DXUTGetD3DDevice()->ShowCursor(true);
        }
        else
        {
            SetCursor(NULL); // Turn off Windows cursor in full screen mode
            DXUTGetD3DDevice()->ShowCursor(false);
        }
    }

    // Clip cursor if requested
    if(!DXUTIsWindowed() && 1/*GetDXUTState().GetClipCursorWhenFullScreen()*/)
    {
        // Confine cursor to full screen window
        RECT rcWindow;
        GetWindowRect(DXUTGetHWNDDeviceFullScreen(), &rcWindow);
        ClipCursor(&rcWindow);
    }
    else
    {
        ClipCursor(NULL);
    }
}


// Gives the D3D device a cursor with image and hotspot from hCursor.

HRESULT DXUTSetDeviceCursor(IDirect3DDevice9* pd3dDevice, HCURSOR hCursor, bool bAddWatermark)
{
    HRESULT hr = E_FAIL;
    ICONINFO iconinfo;
    bool bBWCursor;
    LPDIRECT3DSURFACE9 pCursorSurface = NULL;
    HDC hdcColor = NULL;
    HDC hdcMask = NULL;
    HDC hdcScreen = NULL;
    BITMAP bm;
    DWORD dwWidth;
    DWORD dwHeightSrc;
    DWORD dwHeightDest;
    COLORREF crColor;
    COLORREF crMask;
    UINT x;
    UINT y;
    BITMAPINFO bmi;
    COLORREF* pcrArrayColor = NULL;
    COLORREF* pcrArrayMask = NULL;
    DWORD* pBitmap;
    HGDIOBJ hgdiobjOld;

    ZeroMemory(&iconinfo, sizeof(iconinfo));
    if(!GetIconInfo(hCursor, &iconinfo))
        goto End;

    if (0 == GetObject((HGDIOBJ)iconinfo.hbmMask, sizeof(BITMAP), (LPVOID)&bm))
        goto End;
    dwWidth = bm.bmWidth;
    dwHeightSrc = bm.bmHeight;

    if(iconinfo.hbmColor == NULL)
    {
        bBWCursor = TRUE;
        dwHeightDest = dwHeightSrc / 2;
    }
    else 
    {
        bBWCursor = FALSE;
        dwHeightDest = dwHeightSrc;
    }

    // Create a surface for the fullscreen cursor
    if(FAILED(hr = pd3dDevice->CreateOffscreenPlainSurface(dwWidth, dwHeightDest, 
        D3DFMT_A8R8G8B8, D3DPOOL_SCRATCH, &pCursorSurface, NULL)))
    {
        goto End;
    }

    pcrArrayMask = new DWORD[dwWidth * dwHeightSrc];

    ZeroMemory(&bmi, sizeof(bmi));
    bmi.bmiHeader.biSize = sizeof(bmi.bmiHeader);
    bmi.bmiHeader.biWidth = dwWidth;
    bmi.bmiHeader.biHeight = dwHeightSrc;
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 32;
    bmi.bmiHeader.biCompression = BI_RGB;

    hdcScreen = GetDC(NULL);
    hdcMask = CreateCompatibleDC(hdcScreen);
    if(hdcMask == NULL)
    {
        hr = E_FAIL;
        goto End;
    }
    hgdiobjOld = SelectObject(hdcMask, iconinfo.hbmMask);
    GetDIBits(hdcMask, iconinfo.hbmMask, 0, dwHeightSrc, 
        pcrArrayMask, &bmi, DIB_RGB_COLORS);
    SelectObject(hdcMask, hgdiobjOld);

    if (!bBWCursor)
    {
        pcrArrayColor = new DWORD[dwWidth * dwHeightDest];
        hdcColor = CreateCompatibleDC(hdcScreen);
        if(hdcColor == NULL)
        {
            hr = E_FAIL;
            goto End;
        }
        SelectObject(hdcColor, iconinfo.hbmColor);
        GetDIBits(hdcColor, iconinfo.hbmColor, 0, dwHeightDest, 
            pcrArrayColor, &bmi, DIB_RGB_COLORS);
    }

    // Transfer cursor image into the surface
    D3DLOCKED_RECT lr;
    pCursorSurface->LockRect(&lr, NULL, 0);
    pBitmap = (DWORD*)lr.pBits;
    for(y = 0; y < dwHeightDest; y++)
    {
        for(x = 0; x < dwWidth; x++)
        {
            if (bBWCursor)
            {
                crColor = pcrArrayMask[dwWidth*(dwHeightDest-1-y) + x];
                crMask = pcrArrayMask[dwWidth*(dwHeightSrc-1-y) + x];
            }
            else
            {
                crColor = pcrArrayColor[dwWidth*(dwHeightDest-1-y) + x];
                crMask = pcrArrayMask[dwWidth*(dwHeightDest-1-y) + x];
            }
            if (crMask == 0)
                pBitmap[dwWidth*y + x] = 0xff000000 | crColor;
            else
                pBitmap[dwWidth*y + x] = 0x00000000;

            // It may be helpful to make the D3D cursor look slightly 
            // different from the Windows cursor so you can distinguish 
            // between the two when developing/testing code.  When
            // bAddWatermark is TRUE, the following code adds some
            // small grey "D3D" characters to the upper-left corner of
            // the D3D cursor image.
            if(bAddWatermark && x < 12 && y < 5)
            {
                // 11.. 11.. 11.. .... CCC0
                // 1.1. ..1. 1.1. .... A2A0
                // 1.1. .1.. 1.1. .... A4A0
                // 1.1. ..1. 1.1. .... A2A0
                // 11.. 11.. 11.. .... CCC0

                const WORD wMask[5] = { 0xccc0, 0xa2a0, 0xa4a0, 0xa2a0, 0xccc0 };
                if(wMask[y] & (1 << (15 - x)))
                {
                    pBitmap[dwWidth*y + x] |= 0xff808080;
                }
            }
        }
    }
    pCursorSurface->UnlockRect();

    // Set the device cursor
    if(FAILED(hr = pd3dDevice->SetCursorProperties(iconinfo.xHotspot, 
        iconinfo.yHotspot, pCursorSurface)))
    {
        goto End;
    }

    hr = S_OK;

End:
    if(iconinfo.hbmMask != NULL)
        DeleteObject(iconinfo.hbmMask);
    if(iconinfo.hbmColor != NULL)
        DeleteObject(iconinfo.hbmColor);
    if(hdcScreen != NULL)
        ReleaseDC(NULL, hdcScreen);
    if(hdcColor != NULL)
        DeleteDC(hdcColor);
    if(hdcMask != NULL)
        DeleteDC(hdcMask);
    S_DELS(pcrArrayColor);
    S_DELS(pcrArrayMask);
    S_REL(pCursorSurface);
    return hr;
}



