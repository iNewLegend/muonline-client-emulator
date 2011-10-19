#include "D3D9RenderWindow.h"
#include "dxstdafx.h"


// Look for an adapter ordinal that is tied to a HMONITOR
HRESULT DXUTGetAdapterOrdinalFromMonitor(HMONITOR hMonitor, UINT* pAdapterOrdinal)
{
	*pAdapterOrdinal = 0;
	return S_OK;
	return E_FAIL;
}

CD3D9RenderWindow::CD3D9RenderWindow():
m_bActive(true),
m_bMoving(false),
m_bMinimized(false)
{
}

CD3D9RenderWindow::~CD3D9RenderWindow()
{
}

bool CD3D9RenderWindow::Init()
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
		DXUT_ERR(L"Direct3DCreate9", DXUTERR_NODIRECT3D);
		return false;//DXUT_ERR(L"Direct3DCreate9", DXUTERR_NODIRECT3D);
	}
	GetDXUTState().SetDXUTInited(true);
	// 设置shader 是否有效
	///GetShaderMgr().SetEnable(true);
	return true;
}

// Creates a window with the specified window title, icon, menu, and 
// starting position.  If DXUTInit() has not already been called, it will
// call it with the default parameters.  Instead of calling this, you can 
// call DXUTSetWindow() to use an existing window.  

HRESULT CD3D9RenderWindow::D3DCreateWindow(WNDPROC pWndProc, const std::wstring& strWindowTitle, HINSTANCE hInstance, 
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
		hr = D3DInit();
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
		GetDXUTState().SetHWND(hWnd);
	}

	return S_OK;
}

// Sets a previously created window for the framework to use.  If DXUTInit() 
// has not already been called, it will call it with the default parameters.  
// Instead of calling this, you can call DXUTCreateWindow() to create a new window.  

HRESULT CD3D9RenderWindow::DXUTSetWindow(WNDPROC pWndProc, HWND hWndFocus, HWND hWndDeviceWindowed, bool bHandleMessages)
{
	HRESULT hr;

	GetDXUTState().SetWindowCreateCalled(true);

	// To avoid confusion, we do not allow any HWND to be NULL here.  The
	// caller must pass in valid HWND for all three parameters.  The same
	// HWND may be used for more than one parameter.
	if(hWndFocus == NULL || hWndDeviceWindowed == NULL)
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
		hr = D3DInit();
		if(FAILED(hr))
			return hr;
	}

	HINSTANCE hInstance = (HINSTANCE) (LONG_PTR) GetWindowLongPtr(hWndFocus, GWLP_HINSTANCE); 
	GetDXUTState().SetHInstance(hInstance);
	GetDXUTState().SetWindowCreated(true);
	GetDXUTState().SetHWNDFocus(hWndFocus);
	GetDXUTState().SetHWND(hWndDeviceWindowed);

	return S_OK;
}

// Creates a Direct3D device. If DXUTCreateWindow() or DXUTSetWindow() has not already 
// been called, it will call DXUTCreateWindow() with the default parameters.  
// Instead of calling this, you can call DXUTSetDevice() or DXUTCreateDeviceFromSettings() 

HRESULT CD3D9RenderWindow::D3DCreateDevice(UINT AdapterOrdinal, bool bWindowed, 
	int nSuggestedWidth, int nSuggestedHeight)
{
	HRESULT hr;


	GetDXUTState().SetDeviceCreateCalled(true);

	// If DXUTCreateWindow() or DXUTSetWindow() has not already been called, 
	// then call DXUTCreateWindow() with the default parameters.         
	if(!GetDXUTState().GetWindowCreated()) 
	{
		if(GetDXUTState().GetWindowCreateCalled())
			return E_FAIL; 

		hr = D3DCreateWindow();
		if(FAILED(hr))
			return hr;
	}

	// Force an enumeration with the new IsDeviceAcceptable callback

	D3DDeviceSettings deviceSettings;
	ZeroMemory(&deviceSettings, sizeof(D3DDeviceSettings));
	deviceSettings.AdapterOrdinal			= AdapterOrdinal;
	deviceSettings.pp.Windowed				= bWindowed;
	deviceSettings.pp.BackBufferWidth		= nSuggestedWidth;
	deviceSettings.pp.BackBufferHeight		= nSuggestedHeight;
	deviceSettings.pp.BackBufferFormat		= D3DFMT_X8R8G8B8;
	deviceSettings.pp.BackBufferCount		= 1;
	deviceSettings.pp.MultiSampleType		= D3DMULTISAMPLE_NONE;
	deviceSettings.pp.MultiSampleQuality	= 0;
	deviceSettings.pp.SwapEffect			= D3DSWAPEFFECT_DISCARD;
	deviceSettings.pp.hDeviceWindow			= DXUTGetHWND();
	deviceSettings.pp.EnableAutoDepthStencil= true;
	deviceSettings.pp.AutoDepthStencilFormat= D3DFMT_D24S8;
	deviceSettings.pp.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;
	deviceSettings.pp.PresentationInterval	= D3DPRESENT_INTERVAL_IMMEDIATE/*D3DPRESENT_INTERVAL_DEFAULT*/;

	if(GetDXUTState().GetOverrideAdapterOrdinal() != -1)
		deviceSettings.AdapterOrdinal = GetDXUTState().GetOverrideAdapterOrdinal();
	deviceSettings.AdapterFormat			= D3DFMT_X8R8G8B8;
	deviceSettings.DeviceType				= D3DDEVTYPE_HAL;

	if(GetDXUTState().GetOverrideForcePureHWVP())//纯设备不支持查询某些设备状态,不会滤除任何冗余状态变化,减省cpu
	{
		deviceSettings.BehaviorFlags = D3DCREATE_HARDWARE_VERTEXPROCESSING | D3DCREATE_PUREDEVICE;
	}
	else
	{
		deviceSettings.BehaviorFlags = D3DCREATE_HARDWARE_VERTEXPROCESSING;
	}

	// Change to a Direct3D device created from the new device settings.  
	// If there is an existing device, then either reset or recreated the scene
	hr = D3DChangeDevice(&deviceSettings, NULL, false, true);
	if(FAILED(hr))
		return hr;

	return S_OK;
}

bool CD3D9RenderWindow::Create(WNDPROC pWndProc, const std::wstring& wstrWindowTitle, unsigned long uWidth, unsigned long uHeight, bool bFullScreen)
{
	Init();
	DXUTSetupCursor();
	D3DCreateWindow(pWndProc, wstrWindowTitle);
	D3DCreateDevice(D3DADAPTER_DEFAULT, !bFullScreen, uWidth, uHeight);
	return true;
}

void CD3D9RenderWindow::SetFullscreen(bool bFullScreen, unsigned long width, unsigned long height)
{

}

double CD3D9RenderWindow::GetTime()
{
	return 0;
}

float CD3D9RenderWindow::GetElapsedTime()
{
	return 0;
}

bool CD3D9RenderWindow::FrameBegin()
{
	HRESULT hr;
	if (m_bMoving||m_bMinimized)
	{
		Sleep(50); 
		return false;
	}
	if(!m_bActive)// Window is minimized so yield CPU time to other processes
	{
		Sleep(50); 
	}

	IDirect3DDevice9* pd3dDevice = DXUTGetD3DDevice();
	if(NULL == pd3dDevice)
	{
		if(GetDXUTState().GetDeviceLost())
		{
			D3DDeviceSettings deviceSettings = DXUTGetDeviceSettings();
			D3DChangeDevice(&deviceSettings, NULL, false, true);
		}
		return false;
	}

	if(GetDXUTState().GetDeviceLost())
	{
		// Test the cooperative level to see if it's okay to render
		if(pd3dDevice->TestCooperativeLevel()==D3DERR_DEVICELOST)
		{
			return false;
		}
		else
		{
			// If we are windowed, read the desktop format and 
			// ensure that the Direct3D device is using the same format 
			// since the user could have changed the desktop bitdepth 
			if(DXUTIsWindowed())
			{
				D3DDISPLAYMODE adapterDesktopDisplayMode;
				IDirect3D9* pD3D = DXUTGetD3DObject();
				D3DDeviceSettings* pDeviceSettings = GetDXUTState().GetCurrentDeviceSettings();
				pD3D->GetAdapterDisplayMode(pDeviceSettings->AdapterOrdinal, &adapterDesktopDisplayMode);
				if(pDeviceSettings->AdapterFormat != adapterDesktopDisplayMode.Format)
				{

					D3DDeviceSettings deviceSettings = DXUTGetDeviceSettings();
					deviceSettings.AdapterFormat = adapterDesktopDisplayMode.Format;

					// Change to a Direct3D device created from the new device settings.  
					// If there is an existing device, then either reset or recreate the scene
					hr = D3DChangeDevice(&deviceSettings, NULL, false, false);
					if(FAILED(hr))  
					{
						// If this fails, try to go fullscreen and if this fails also shutdown.
						if(FAILED(DXUTToggleFullScreen()))
							DXUTShutdown();
					}

					return false;
				}
			}
			//if(D3DERR_DEVICENOTRESET==hr)
			{
				// Try to reset the device
				if(FAILED(hr = DXUTReset3DEnvironment()))
				{
					if(D3DERR_DEVICELOST == hr)
					{
						// The device was lost again, so continue waiting until it can be reset.
						return false;
					}
					else if(DXUTERR_RESETTINGDEVICEOBJECTS == hr || 
						DXUTERR_MEDIANOTFOUND == hr)
					{
						DXUTDisplayErrorMessage(hr);
						DXUTShutdown();
						return false;
					}
					else
					{
						// Reset failed, but the device wasn't lost so something bad happened, 
						// so recreate the device to try to recover
						D3DDeviceSettings* pDeviceSettings = GetDXUTState().GetCurrentDeviceSettings();
						if(FAILED(D3DChangeDevice(pDeviceSettings, NULL, true, false)))
						{
							DXUTShutdown();
							return false;
						}
					}
				}
			}
		}

		GetDXUTState().SetDeviceLost(false);
	}
	// Get the app's time, in seconds. Skip rendering if no time elapsed
	return true;
}

void CD3D9RenderWindow::FrameEnd()
{
	HRESULT hr;

	IDirect3DDevice9* pd3dDevice = DXUTGetD3DDevice();
	if(NULL == pd3dDevice) // Handle DXUTShutdown from inside callback
		return;

	// Show the frame on the primary surface.
	hr = pd3dDevice->Present(NULL, NULL, NULL, NULL);
	if(FAILED(hr))
	{
		if(D3DERR_DEVICELOST == hr)
		{
			GetDXUTState().SetDeviceLost(true);
		}
		else if(D3DERR_DRIVERINTERNALERROR == hr)
		{
			// D3D9新增的D3DERR_DRIVERINTERNALERROR，遇到这个你就完蛋了，基本不可能恢复了，终止程序吧。
			GetDXUTState().SetDeviceLost(true);
		}
	}
}

HWND CD3D9RenderWindow::GetHWND()
{
	return DXUTGetHWND();
}

void CD3D9RenderWindow::CheckForWindowSizeChange()
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
		D3DDeviceSettings deviceSettings = DXUTGetDeviceSettings();
		deviceSettings.pp.BackBufferWidth  = 0; 
		deviceSettings.pp.BackBufferHeight = 0;
		D3DChangeDevice(&deviceSettings, NULL, false, false);
	}
}

// Checks to see if the HWND changed monitors, and if it did it creates a device 
// from the monitor's adapter and recreates the scene.
void CD3D9RenderWindow::CheckForWindowChangingMonitors()
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
			D3DDeviceSettings deviceSettings = DXUTGetDeviceSettings();
			deviceSettings.AdapterOrdinal = newOrdinal;


			//hr = DXUTFindValidDeviceSettings(&deviceSettings, &deviceSettings);
			//if(SUCCEEDED(hr)) 
			{
				// Create a Direct3D device using the new device settings.  
				// If there is an existing device, then it will either reset or recreate the scene.
				hr = D3DChangeDevice(&deviceSettings, NULL, false, false);

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

LRESULT CD3D9RenderWindow::MsgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch(uMsg)
	{
	case WM_PAINT:
		{
			IDirect3DDevice9* pd3dDevice = DXUTGetD3DDevice();

			if(pd3dDevice && m_bMoving)
			{
				HRESULT hr = pd3dDevice->Present(NULL, NULL, NULL, NULL);
				if(D3DERR_DEVICELOST == hr)
				{
					GetDXUTState().SetDeviceLost(true);
				}
				else if(D3DERR_DRIVERINTERNALERROR == hr)
				{
					// D3D9新增的D3DERR_DRIVERINTERNALERROR，遇到这个你就完蛋了，基本不可能恢复了，终止程序吧。
					GetDXUTState().SetDeviceLost(true);
				}
			}
			break;
		}

	case WM_SIZE:
		if(SIZE_MINIMIZED == wParam)
		{
			m_bMinimized = true;
		}
		else
		{
			RECT rcCurrentClient;
			GetClientRect(GetHWND(), &rcCurrentClient);
			if(rcCurrentClient.top == 0 && rcCurrentClient.bottom == 0)
			{
				// Rapidly clicking the task bar to minimize and restore a window
				// can cause a WM_SIZE message with SIZE_RESTORED when 
				// the window has actually become minimized due to rapid change
				// so just ignore this message
			}
			else if(SIZE_MAXIMIZED == wParam)
			{
				m_bMinimized = false;
				CheckForWindowSizeChange();
				CheckForWindowChangingMonitors();
			}
			else if(SIZE_RESTORED == wParam)
			{      
				m_bMinimized = false;
				if(!m_bMoving)
				{
					CheckForWindowSizeChange();
					CheckForWindowChangingMonitors();
				}
			}
		}
		break;

	case WM_GETMINMAXINFO:
		// 设定窗口的最大最小尺寸，拖动尺寸... 
		((MINMAXINFO*)lParam)->ptMinTrackSize.x = 200;
		((MINMAXINFO*)lParam)->ptMinTrackSize.y = 200;
		break;

	case WM_ENTERSIZEMOVE:
		// Halt frame movement while the app is sizing or moving
		m_bMoving = true;
		break;

	case WM_EXITSIZEMOVE:
		CheckForWindowSizeChange();
		CheckForWindowChangingMonitors();
		m_bMoving = false;
		break;

	case WM_MOUSEMOVE:
		if(m_bActive && !DXUTIsWindowed())
		{
			IDirect3DDevice9* pd3dDevice = DXUTGetD3DDevice();
			if(pd3dDevice)
			{
				POINT ptCursor;
				GetCursorPos(&ptCursor);
				pd3dDevice->SetCursorPosition(ptCursor.x, ptCursor.y, 0);
			}
		}
		break;

	case WM_SETCURSOR:
		if(m_bActive && !DXUTIsWindowed())
		{
			IDirect3DDevice9* pd3dDevice = DXUTGetD3DDevice();
			if(pd3dDevice && 1/*GetDXUTState().GetShowCursorWhenFullScreen()*/)
				pd3dDevice->ShowCursor(true);
			return true; // prevent Windows from setting cursor to window class cursor
		}
		break;

	case WM_ACTIVATEAPP:
		if(wParam == TRUE && !m_bActive) // Handle only if previously not active 
		{
			m_bActive = true;

			// The GetMinimizedWhileFullscreen() varible is used instead of !DXUTIsWindowed()
			// to handle the rare case toggling to windowed mode while the fullscreen application 
			// is minimized and thus making the pause count wrong
			m_bMinimized = false;
		}
		else if(wParam == FALSE && m_bActive) // Handle only if previously active 
		{           
			m_bActive = false;
			if(!DXUTIsWindowed())
			{
				m_bMinimized = true;
				// Going from full screen to a minimized state 
				ClipCursor(NULL);      // don't limit the cursor anymore
			}
		}
		break;

	case WM_NCHITTEST:
		// Prevent the user from selecting the menu in full screen mode
		if(!DXUTIsWindowed())
			return HTCLIENT;
		break;

	case WM_POWERBROADCAST:
		switch(wParam)
		{
#ifndef PBT_APMQUERYSUSPEND
#define PBT_APMQUERYSUSPEND 0x0000
#endif
	case PBT_APMQUERYSUSPEND:
		// At this point, the app should save any data for open
		// network connections, files, etc., and prepare to go into
		// a suspended mode.  The app can use the MsgProc callback
		// to handle this if desired.
		return true;

#ifndef PBT_APMRESUMESUSPEND
#define PBT_APMRESUMESUSPEND 0x0007
#endif
	case PBT_APMRESUMESUSPEND:
		// At this point, the app should recover any data, network
		// connections, files, etc., and resume running from when
		// the app was suspended. The app can use the MsgProc callback
		// to handle this if desired.

		// QPC may lose consistency when suspending, so reset the timer
		// upon resume.
		return true;
		}
		break;

	case WM_SYSCOMMAND:
		// Prevent moving/sizing in full screen mode
		switch(wParam)
		{
		case SC_MOVE:
		case SC_SIZE:
		case SC_MAXIMIZE:
		case SC_KEYMENU:
			if(!DXUTIsWindowed())
				return 0;
			break;
		}
		break;

	case WM_SYSKEYDOWN:
		{
			switch(wParam)
			{
			case VK_RETURN:
				{
					{
						// Toggle full screen upon alt-enter 
						DWORD dwMask = (1 << 29);
						if((lParam & dwMask) != 0) // Alt is down also
						{
							// Toggle the full screen/window mode
							DXUTToggleFullScreen();                   
							return 0;
						}
					}
				}
			}
			break;
		}

	case WM_KEYDOWN:
		{
			{
				switch(wParam)
				{
				case VK_F8:
					{
						static bool bWireFrame = false;
						bWireFrame = !bWireFrame; 
						IDirect3DDevice9* pd3dDevice = DXUTGetD3DDevice();
						if(pd3dDevice)
							pd3dDevice->SetRenderState(D3DRS_FILLMODE, (bWireFrame) ? D3DFILL_WIREFRAME : D3DFILL_SOLID); 
						break;
					}

				case VK_ESCAPE:
					{
						// Received key to exit app
						// SendMessage(hWnd, WM_CLOSE, 0, 0);
					}

				case VK_PAUSE: 
					{
						break; 
					}
				}
			}
			break;
		}

	case WM_CLOSE:
		{
			DestroyWindow(hWnd);
			UnregisterClass(L"Direct3DWindowClass", NULL);
			GetDXUTState().SetHWNDFocus(NULL);
			GetDXUTState().SetHWND(NULL);
			return 0;
		}

	case WM_DESTROY:
		PostQuitMessage(WM_QUIT);
		break;
	}
	return 0;
}