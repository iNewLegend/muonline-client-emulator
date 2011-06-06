#pragma once

// Structs

class CD3DEnumeration;

struct DXUTDeviceSettings
{
    UINT AdapterOrdinal;
    D3DDEVTYPE DeviceType;
    D3DFORMAT AdapterFormat;
    DWORD BehaviorFlags;
    D3DPRESENT_PARAMETERS pp;
};

// Error codes

#define DXUTERR_NODIRECT3D              MAKE_HRESULT(SEVERITY_ERROR, FACILITY_ITF, 0x0901)
#define DXUTERR_NOCOMPATIBLEDEVICES     MAKE_HRESULT(SEVERITY_ERROR, FACILITY_ITF, 0x0902)
#define DXUTERR_MEDIANOTFOUND           MAKE_HRESULT(SEVERITY_ERROR, FACILITY_ITF, 0x0903)
#define DXUTERR_NONZEROREFCOUNT         MAKE_HRESULT(SEVERITY_ERROR, FACILITY_ITF, 0x0904)
#define DXUTERR_CREATINGDEVICE          MAKE_HRESULT(SEVERITY_ERROR, FACILITY_ITF, 0x0905)
#define DXUTERR_RESETTINGDEVICE         MAKE_HRESULT(SEVERITY_ERROR, FACILITY_ITF, 0x0906)
#define DXUTERR_CREATINGDEVICEOBJECTS   MAKE_HRESULT(SEVERITY_ERROR, FACILITY_ITF, 0x0907)
#define DXUTERR_RESETTINGDEVICEOBJECTS  MAKE_HRESULT(SEVERITY_ERROR, FACILITY_ITF, 0x0908)
#define DXUTERR_INCORRECTVERSION        MAKE_HRESULT(SEVERITY_ERROR, FACILITY_ITF, 0x0909)

void    DXUTSetupCursor();

// Callback registration 
typedef bool    (CALLBACK *LPDXUTCALLBACKISDEVICEACCEPTABLE)(D3DCAPS9* pCaps, D3DFORMAT AdapterFormat, D3DFORMAT BackBufferFormat, bool bWindowed, void* pUserContext);

bool    CALLBACK IsDeviceAcceptable(D3DCAPS9* pCaps, D3DFORMAT AdapterFormat, D3DFORMAT BackBufferFormat, bool bWindowed, void* pUserContext);
bool    CALLBACK ModifyDeviceSettings(DXUTDeviceSettings* pDeviceSettings, const D3DCAPS9* pCaps, void* pUserContext);

// Initialization
HRESULT DXUTInit(bool bParseCommandLine = true, bool bShowMsgBoxOnError = true, bool bHandleAltEnter = true);

// Choose either DXUTCreateWindow or DXUTSetWindow.  If using DXUTSetWindow, consider using DXUTStaticWndProc
HRESULT DXUTCreateWindow(WNDPROC pWndProc = NULL, const std::wstring& strWindowTitle = L"Direct3D Window", 
                          HINSTANCE hInstance = NULL, HICON hIcon = NULL, HMENU hMenu = NULL,
                          int x = CW_USEDEFAULT, int y = CW_USEDEFAULT);
HRESULT DXUTSetWindow(WNDPROC pWndProc, HWND hWndFocus, HWND hWndDeviceFullScreen, HWND hWndDeviceWindowed, bool bHandleMessages = true);

// Choose either DXUTCreateDevice or DXUTSetDevice or DXUTCreateDeviceFromSettings
HRESULT DXUTCreateDevice(UINT AdapterOrdinal = D3DADAPTER_DEFAULT, bool bWindowed = true, 
                          int nSuggestedWidth = 0, int nSuggestedHeight = 0);
HRESULT DXUTCreateDeviceFromSettings(DXUTDeviceSettings* pDeviceSettings, bool bPreserveInput = false, bool bClipWindowToSingleAdapter = true);
HRESULT DXUTSetDevice(IDirect3DDevice9* pd3dDevice);


void DXUTCloseWindow();

// Finding valid device settings

enum DXUT_MATCH_TYPE
{
    DXUTMT_IGNORE_INPUT = 0,  // Use the closest valid value to a default 
    DXUTMT_PRESERVE_INPUT,    // Use input without change, but may cause no valid device to be found
    DXUTMT_CLOSEST_TO_INPUT   // Use the closest valid value to the input 
};

struct DXUTMatchOptions
{
    DXUT_MATCH_TYPE eAdapterOrdinal;
    DXUT_MATCH_TYPE eDeviceType;
    DXUT_MATCH_TYPE eWindowed;
    DXUT_MATCH_TYPE eAdapterFormat;
    DXUT_MATCH_TYPE eVertexProcessing;
    DXUT_MATCH_TYPE eResolution;
    DXUT_MATCH_TYPE eBackBufferFormat;
    DXUT_MATCH_TYPE eBackBufferCount;
    DXUT_MATCH_TYPE eMultiSample;
    DXUT_MATCH_TYPE eSwapEffect;
    DXUT_MATCH_TYPE eDepthFormat;
    DXUT_MATCH_TYPE eStencilFormat;
    DXUT_MATCH_TYPE ePresentFlags;
    DXUT_MATCH_TYPE eRefreshRate;
    DXUT_MATCH_TYPE ePresentInterval;
};

HRESULT DXUTFindValidDeviceSettings(DXUTDeviceSettings* pOut, DXUTDeviceSettings* pIn = NULL, DXUTMatchOptions* pMatchOptions = NULL);

// Common Tasks 
HRESULT DXUTToggleFullScreen();
HRESULT DXUTToggleREF();
void    DXUTResetFrameworkState();
void    DXUTShutdown(int nExitCode = 0);

template< typename TYPE >
class CGrowableArray;

// Helper macros to build member functions that access member variables with thread safety

#define SET_ACCESSOR(x, y)       inline void Set##y(x t)  { m_state.m_##y = t; };
#define GET_ACCESSOR(x, y)       inline x Get##y() { return m_state.m_##y; };
#define GET_SET_ACCESSOR(x, y)   SET_ACCESSOR(x, y) GET_ACCESSOR(x, y)

#define SETP_ACCESSOR(x, y)      inline void Set##y(x* t)  { m_state.m_##y = *t; };
#define GETP_ACCESSOR(x, y)      inline x* Get##y() { return &m_state.m_##y; };
#define GETP_SETP_ACCESSOR(x, y) SETP_ACCESSOR(x, y) GETP_ACCESSOR(x, y)


class DXUTState
{
protected:
	struct STATE
	{
		IDirect3D9*          m_D3D;                     // the main D3D object

		IDirect3DDevice9*    m_D3DDevice;               // the D3D rendering device
		CD3DEnumeration*     m_D3DEnumeration;          // CD3DEnumeration object

		DXUTDeviceSettings*  m_CurrentDeviceSettings;   // current device settings
		D3DSURFACE_DESC      m_BackBufferSurfaceDesc;   // back buffer surface description
		D3DCAPS9             m_Caps;                    // D3D caps for current device


		LONG_PTR m_WndProc;//
		HWND  m_HWNDFocus;                  // the main app focus window
		HWND  m_HWNDDeviceFullScreen;       // the main app device window in fullscreen mode
		HWND  m_HWNDDeviceWindowed;         // the main app device window in windowed mode
		HMONITOR m_AdapterMonitor;          // the monitor of the adapter 

		UINT m_FullScreenBackBufferWidthAtModeChange;  // back buffer size of fullscreen mode right before switching to windowed mode.  Used to restore to same resolution when toggling back to fullscreen
		UINT m_FullScreenBackBufferHeightAtModeChange; // back buffer size of fullscreen mode right before switching to windowed mode.  Used to restore to same resolution when toggling back to fullscreen
		UINT m_WindowBackBufferWidthAtModeChange;  // back buffer size of windowed mode right before switching to fullscreen mode.  Used to restore to same resolution when toggling back to windowed mode
		UINT m_WindowBackBufferHeightAtModeChange; // back buffer size of windowed mode right before switching to fullscreen mode.  Used to restore to same resolution when toggling back to windowed mode
		DWORD m_WindowedStyleAtModeChange;  // window style
		WINDOWPLACEMENT m_WindowedPlacement; // record of windowed HWND position/show state/etc
		bool  m_TopmostWhileWindowed;       // if true, the windowed HWND is topmost 
		bool  m_MinimizedWhileFullscreen;   // if true, the HWND is minimized due to a focus switch away when fullscreen mode
		bool  m_IgnoreSizeChange;           // if true, DXUT won't reset the device upon HWND size change

		HINSTANCE m_HInstance;              // handle to the app instance

		bool  m_HandleAltEnter;             // if true, then DXUT will handle Alt-Enter
		bool  m_ShowMsgBoxOnError;          // if true, then msgboxes are displayed upon errors
		bool  m_NoStats;                    // if true, then DXUTGetFrameStats() and DXUTGetDeviceStats() will return blank strings
		bool  m_AutoChangeAdapter;          // if true, then the adapter will automatically change if the window is different monitor
		bool  m_WindowCreatedWithDefaultPositions; // if true, then CW_USEDEFAULT was used and the window should be moved to the right adapter
		int   m_ExitCode;                   // the exit code to be returned to the command line

		bool  m_DXUTInited;                 // if true, then DXUTInit() has succeeded
		bool  m_WindowCreated;              // if true, then DXUTCreateWindow() or DXUTSetWindow() has succeeded
		bool  m_DeviceCreated;              // if true, then DXUTCreateDevice*() or DXUTSetDevice() has succeeded

		bool  m_DXUTInitCalled;             // if true, then DXUTInit() was called
		bool  m_WindowCreateCalled;         // if true, then DXUTCreateWindow() or DXUTSetWindow() was called
		bool  m_DeviceCreateCalled;         // if true, then DXUTCreateDevice*() or DXUTSetDevice() was called

		bool  m_DeviceObjectsCreated;       // if true, then DeviceCreated callback has been called (if non-NULL)
		bool  m_DeviceObjectsReset;         // if true, then DeviceReset callback has been called (if non-NULL)
		bool  m_InsideMainloop;             // if true, then the framework is inside the main loop
		bool  m_Active;                     // if true, then the app is the active top level window
		bool  m_DeviceLost;                 // if true, then the device is lost and needs to be reset
		bool  m_NotifyOnMouseMove;          // if true, include WM_MOUSEMOVE in mousecallback
		bool  m_Automation;                 // if true, automation is enabled

		int   m_OverrideAdapterOrdinal;     // if != -1, then override to use this adapter ordinal
		bool  m_OverrideWindowed;           // if true, then force to start windowed
		bool  m_OverrideFullScreen;         // if true, then force to start full screen
		int   m_OverrideStartX;             // if != -1, then override to this X position of the window
		int   m_OverrideStartY;             // if != -1, then override to this Y position of the window
		int   m_OverrideWidth;              // if != 0, then override to this width
		int   m_OverrideHeight;             // if != 0, then override to this height
		bool  m_OverrideForceHAL;           // if true, then force to HAL device (failing if one doesn't exist)
		bool  m_OverrideForceREF;           // if true, then force to REF device (failing if one doesn't exist)
		bool  m_OverrideForcePureHWVP;      // if true, then force to use pure HWVP (failing if device doesn't support it)
		bool  m_OverrideForceHWVP;          // if true, then force to use HWVP (failing if device doesn't support it)
		bool  m_OverrideForceSWVP;          // if true, then force to use SWVP 
		int   m_OverrideForceVsync;         // if == 0, then it will force the app to use D3DPRESENT_INTERVAL_IMMEDIATE, if == 1 force use of D3DPRESENT_INTERVAL_DEFAULT

		WCHAR                        m_StaticFrameStats[256];           // static part of frames stats 
		WCHAR                        m_DeviceStats[256];                // device stats (description, device type, etc)
	};

	STATE m_state;

public:
	DXUTState();
	~DXUTState();

	void Create();
	void Destroy();

	// Macros to define access functions for thread safe access into m_state 
	GET_SET_ACCESSOR(IDirect3D9*, D3D);

	GET_SET_ACCESSOR(IDirect3DDevice9*, D3DDevice);
	GET_SET_ACCESSOR(CD3DEnumeration*, D3DEnumeration);   
	GET_SET_ACCESSOR(DXUTDeviceSettings*, CurrentDeviceSettings);   
	GETP_SETP_ACCESSOR(D3DSURFACE_DESC, BackBufferSurfaceDesc);
	GETP_SETP_ACCESSOR(D3DCAPS9, Caps);

	GET_SET_ACCESSOR(LONG_PTR, WndProc);
	GET_SET_ACCESSOR(HWND, HWNDFocus);
	GET_SET_ACCESSOR(HWND, HWNDDeviceFullScreen);
	GET_SET_ACCESSOR(HWND, HWNDDeviceWindowed);
	GET_SET_ACCESSOR(HMONITOR, AdapterMonitor);

	GET_SET_ACCESSOR(UINT, FullScreenBackBufferWidthAtModeChange);
	GET_SET_ACCESSOR(UINT, FullScreenBackBufferHeightAtModeChange);
	GET_SET_ACCESSOR(UINT, WindowBackBufferWidthAtModeChange);
	GET_SET_ACCESSOR(UINT, WindowBackBufferHeightAtModeChange);
	GETP_SETP_ACCESSOR(WINDOWPLACEMENT, WindowedPlacement);
	GET_SET_ACCESSOR(DWORD, WindowedStyleAtModeChange);
	GET_SET_ACCESSOR(bool, TopmostWhileWindowed);
	GET_SET_ACCESSOR(bool, MinimizedWhileFullscreen);
	GET_SET_ACCESSOR(bool, IgnoreSizeChange);   

	GET_SET_ACCESSOR(HINSTANCE, HInstance);

	GET_SET_ACCESSOR(bool, HandleAltEnter);
	GET_SET_ACCESSOR(bool, ShowMsgBoxOnError);
	GET_SET_ACCESSOR(bool, NoStats);
	GET_SET_ACCESSOR(bool, AutoChangeAdapter);
	GET_SET_ACCESSOR(bool, WindowCreatedWithDefaultPositions);
	GET_SET_ACCESSOR(int, ExitCode);

	GET_SET_ACCESSOR(bool, DXUTInited);
	GET_SET_ACCESSOR(bool, WindowCreated);
	GET_SET_ACCESSOR(bool, DeviceCreated);
	GET_SET_ACCESSOR(bool, DXUTInitCalled);
	GET_SET_ACCESSOR(bool, WindowCreateCalled);
	GET_SET_ACCESSOR(bool, DeviceCreateCalled);
	GET_SET_ACCESSOR(bool, InsideMainloop);
	GET_SET_ACCESSOR(bool, DeviceLost);
	GET_SET_ACCESSOR(bool, NotifyOnMouseMove);
	GET_SET_ACCESSOR(bool, Automation);

	GET_SET_ACCESSOR(int, OverrideAdapterOrdinal);
	GET_SET_ACCESSOR(bool, OverrideWindowed);
	GET_SET_ACCESSOR(bool, OverrideFullScreen);
	GET_SET_ACCESSOR(int, OverrideStartX);
	GET_SET_ACCESSOR(int, OverrideStartY);
	GET_SET_ACCESSOR(int, OverrideWidth);
	GET_SET_ACCESSOR(int, OverrideHeight);
	GET_SET_ACCESSOR(bool, OverrideForceHAL);
	GET_SET_ACCESSOR(bool, OverrideForceREF);
	GET_SET_ACCESSOR(bool, OverrideForcePureHWVP);
	GET_SET_ACCESSOR(bool, OverrideForceHWVP);
	GET_SET_ACCESSOR(bool, OverrideForceSWVP);
	GET_SET_ACCESSOR(int, OverrideForceVsync);

	GET_ACCESSOR(WCHAR*, StaticFrameStats);
	GET_ACCESSOR(WCHAR*, DeviceStats);    
};

DXUTState& GetDXUTState();
typedef DECLSPEC_IMPORT UINT (WINAPI* LPTIMEBEGINPERIOD)(UINT uPeriod);
void    DXUTParseCommandLine();
void    DXUTDisplayErrorMessage(HRESULT hr);

// State Retrieval  

IDirect3D9*             DXUTGetD3DObject(); // Does not addref unlike typical Get* APIs
IDirect3DDevice9*       DXUTGetD3DDevice(); // Does not addref unlike typical Get* APIs
DXUTDeviceSettings      DXUTGetDeviceSettings(); 
const D3DSURFACE_DESC*  DXUTGetBackBufferSurfaceDesc();
HWND                    DXUTGetHWND();
HWND                    DXUTGetHWNDFocus();
HWND                    DXUTGetHWNDDeviceFullScreen();
HWND                    DXUTGetHWNDDeviceWindowed();
bool                    DXUTIsWindowed();
LPCWSTR                 DXUTGetDeviceStats();
int                     DXUTGetExitCode();
bool                    DXUTGetShowMsgBoxOnError();


HRESULT DXUTChangeDevice(DXUTDeviceSettings* pNewDeviceSettings, IDirect3DDevice9* pd3dDeviceFromApp, bool bForceRecreate, bool bClipWindowToSingleAdapter);
void    DXUTCheckForWindowSizeChange();
void    DXUTCheckForWindowChangingMonitors();
HRESULT DXUTReset3DEnvironment();