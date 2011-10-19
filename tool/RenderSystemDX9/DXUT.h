#pragma once

struct D3DDeviceSettings
{
    UINT AdapterOrdinal;
    D3DDEVTYPE DeviceType;
    D3DFORMAT AdapterFormat;
    DWORD BehaviorFlags;
    D3DPRESENT_PARAMETERS pp;
};

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

// Initialization
HRESULT D3DInit();


HRESULT DXUTSetDevice(IDirect3DDevice9* pd3dDevice);


void DXUTCloseWindow();

// Common Tasks 
HRESULT DXUTToggleFullScreen();
void    DXUTShutdown();

// Helper macros to build member functions that access member variables with thread safety

#define SET_ACCESSOR(x, y)       inline void Set##y(x t)  { m_##y = t; };
#define GET_ACCESSOR(x, y)       inline x Get##y() { return m_##y; };
#define GET_SET_ACCESSOR(x, y)   SET_ACCESSOR(x, y) GET_ACCESSOR(x, y)

#define SETP_ACCESSOR(x, y)      inline void Set##y(x* t)  { m_##y = *t; };
#define GETP_ACCESSOR(x, y)      inline x* Get##y() { return &m_##y; };
#define GETP_SETP_ACCESSOR(x, y) SETP_ACCESSOR(x, y) GETP_ACCESSOR(x, y)

class DXUTState
{
public:
		IDirect3D9*          m_D3D;                     // the main D3D object

		IDirect3DDevice9*    m_D3DDevice;               // the D3D rendering device

		D3DDeviceSettings*  m_CurrentDeviceSettings;   // current device settings
		D3DCAPS9             m_Caps;                    // D3D caps for current device


		LONG_PTR m_WndProc;//
		HWND  m_HWNDFocus;                  // the main app focus window
		HWND  m_HWND;
		HMONITOR m_AdapterMonitor;          // the monitor of the adapter 

		UINT m_FullScreenBackBufferWidthAtModeChange;  // back buffer size of fullscreen mode right before switching to windowed mode.  Used to restore to same resolution when toggling back to fullscreen
		UINT m_FullScreenBackBufferHeightAtModeChange; // back buffer size of fullscreen mode right before switching to windowed mode.  Used to restore to same resolution when toggling back to fullscreen
		UINT m_WindowBackBufferWidthAtModeChange;  // back buffer size of windowed mode right before switching to fullscreen mode.  Used to restore to same resolution when toggling back to windowed mode
		UINT m_WindowBackBufferHeightAtModeChange; // back buffer size of windowed mode right before switching to fullscreen mode.  Used to restore to same resolution when toggling back to windowed mode
		DWORD m_WindowedStyleAtModeChange;  // window style
		WINDOWPLACEMENT m_WindowedPlacement; // record of windowed HWND position/show state/etc
		bool  m_TopmostWhileWindowed;       // if true, the windowed HWND is topmost 
		bool  m_MinimizedWhileFullscreen;   // if true, the HWND is minimized due to a focus switch away when fullscreen mode

		HINSTANCE m_HInstance;              // handle to the app instance

		bool  m_AutoChangeAdapter;          // if true, then the adapter will automatically change if the window is different monitor

		bool  m_DXUTInited;                 // if true, then DXUTInit() has succeeded
		bool  m_WindowCreated;              // if true, then DXUTCreateWindow() or DXUTSetWindow() has succeeded
		bool  m_DeviceCreated;              // if true, then DXUTCreateDevice*() or DXUTSetDevice() has succeeded

		bool  m_DXUTInitCalled;             // if true, then DXUTInit() was called
		bool  m_WindowCreateCalled;         // if true, then DXUTCreateWindow() or DXUTSetWindow() was called
		bool  m_DeviceCreateCalled;         // if true, then DXUTCreateDevice*() or DXUTSetDevice() was called

		bool  m_DeviceObjectsCreated;       // if true, then DeviceCreated callback has been called (if non-NULL)
		bool  m_DeviceObjectsReset;         // if true, then DeviceReset callback has been called (if non-NULL)
		bool  m_Active;                     // if true, then the app is the active top level window
		bool  m_DeviceLost;                 // if true, then the device is lost and needs to be reset

		int   m_OverrideAdapterOrdinal;     // if != -1, then override to use this adapter ordinal
		bool  m_OverrideForcePureHWVP;      // if true, then force to use pure HWVP (failing if device doesn't support it)
public:
	DXUTState();
	~DXUTState();

	void Create();
	void Destroy();

	// Macros to define access functions for thread safe access into m_state 
	GET_SET_ACCESSOR(IDirect3D9*, D3D);

	GET_SET_ACCESSOR(IDirect3DDevice9*, D3DDevice);
	GET_SET_ACCESSOR(D3DDeviceSettings*, CurrentDeviceSettings);   
	GETP_SETP_ACCESSOR(D3DCAPS9, Caps);

	GET_SET_ACCESSOR(LONG_PTR, WndProc);
	GET_SET_ACCESSOR(HWND, HWNDFocus);
	GET_SET_ACCESSOR(HWND, HWND);
	GET_SET_ACCESSOR(HMONITOR, AdapterMonitor);

	GET_SET_ACCESSOR(UINT, FullScreenBackBufferWidthAtModeChange);
	GET_SET_ACCESSOR(UINT, FullScreenBackBufferHeightAtModeChange);
	GET_SET_ACCESSOR(UINT, WindowBackBufferWidthAtModeChange);
	GET_SET_ACCESSOR(UINT, WindowBackBufferHeightAtModeChange);
	GETP_SETP_ACCESSOR(WINDOWPLACEMENT, WindowedPlacement);
	GET_SET_ACCESSOR(DWORD, WindowedStyleAtModeChange);
	GET_SET_ACCESSOR(bool, TopmostWhileWindowed);

	GET_SET_ACCESSOR(HINSTANCE, HInstance);

	GET_SET_ACCESSOR(bool, AutoChangeAdapter);

	GET_SET_ACCESSOR(bool, DXUTInited);
	GET_SET_ACCESSOR(bool, WindowCreated);
	GET_SET_ACCESSOR(bool, DeviceCreated);

	GET_SET_ACCESSOR(bool, DXUTInitCalled);
	GET_SET_ACCESSOR(bool, WindowCreateCalled);
	GET_SET_ACCESSOR(bool, DeviceCreateCalled);
	GET_SET_ACCESSOR(bool, DeviceLost);

	GET_SET_ACCESSOR(int, OverrideAdapterOrdinal);
	GET_SET_ACCESSOR(bool, OverrideForcePureHWVP);
};

DXUTState& GetDXUTState();
typedef DECLSPEC_IMPORT UINT (WINAPI* LPTIMEBEGINPERIOD)(UINT uPeriod);
void    DXUTDisplayErrorMessage(HRESULT hr);

// State Retrieval  
IDirect3D9*             DXUTGetD3DObject(); // Does not addref unlike typical Get* APIs
IDirect3DDevice9*       DXUTGetD3DDevice(); // Does not addref unlike typical Get* APIs
D3DDeviceSettings      DXUTGetDeviceSettings(); 
HWND                    DXUTGetHWND();
HWND                    DXUTGetHWNDFocus();
bool                    DXUTIsWindowed();

HRESULT D3DChangeDevice(D3DDeviceSettings* pNewDeviceSettings, IDirect3DDevice9* pd3dDeviceFromApp, bool bForceRecreate, bool bClipWindowToSingleAdapter);
HRESULT DXUTReset3DEnvironment();