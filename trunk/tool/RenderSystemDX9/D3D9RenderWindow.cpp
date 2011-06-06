#include "D3D9RenderWindow.h"
#include "dxstdafx.h"

CD3D9RenderWindow::CD3D9RenderWindow():
m_bActive(true),
m_bMoving(false),
m_bMinimized(false)
{
}

CD3D9RenderWindow::~CD3D9RenderWindow()
{
}

bool CD3D9RenderWindow::Init(bool bParseCommandLine, bool bShowMsgBoxOnError, bool bHandleAltEnter)
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

	GetDXUTState().SetShowMsgBoxOnError(bShowMsgBoxOnError);
	GetDXUTState().SetHandleAltEnter(bHandleAltEnter);

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
		DXUT_ERR(L"Direct3DCreate9", DXUTERR_NODIRECT3D);
		return false;//DXUT_ERR(L"Direct3DCreate9", DXUTERR_NODIRECT3D);
	}
	// Reset the timer
	GetGlobalTimer().Reset();
	GetDXUTState().SetDXUTInited(true);
	// 设置shader 是否有效
	///GetShaderMgr().SetEnable(true);
	return true;
}

bool CD3D9RenderWindow::Create(WNDPROC pWndProc, const std::wstring& wstrWindowTitle, unsigned long uWidth, unsigned long uHeight, bool bFullScreen)
{
	//////////////////////////////////////////////////////////////////////////
#if defined(DEBUG) | defined(_DEBUG)
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif
	// TODO: Perform any application-level initialization here

	// This sample requires a stencil buffer. See the callback function for details.
	/*CGrowableArray<D3DFORMAT>* pDSFormats = DXUTGetEnumeration()->GetPossibleDepthStencilFormatList();
	pDSFormats->RemoveAll();
	pDSFormats->Add(D3DFMT_D24S8);
	pDSFormats->Add(D3DFMT_D24X4S4);
	pDSFormats->Add(D3DFMT_D15S1);
	pDSFormats->Add(D3DFMT_D24FS8);*/
	Init(true, true, true);
	DXUTSetupCursor();
	DXUTCreateWindow(pWndProc, wstrWindowTitle);
	DXUTCreateDevice(D3DADAPTER_DEFAULT, !bFullScreen, uWidth, uHeight);
	return true;
}

void CD3D9RenderWindow::SetFullscreen(bool bFullScreen, unsigned long width, unsigned long height)
{

}

double CD3D9RenderWindow::GetTime()
{
	return GetGlobalTimer().GetTime();
}

float CD3D9RenderWindow::GetElapsedTime()
{
	return GetGlobalTimer().GetElapsedTime();
}

bool CD3D9RenderWindow::FrameBegin()
{
	HRESULT hr;
	DWORD dwBeginRenderTime = GetTickCount();

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
			DXUTDeviceSettings deviceSettings = DXUTGetDeviceSettings();
			DXUTChangeDevice(&deviceSettings, NULL, false, true);
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
				DXUTDeviceSettings* pDeviceSettings = GetDXUTState().GetCurrentDeviceSettings();
				pD3D->GetAdapterDisplayMode(pDeviceSettings->AdapterOrdinal, &adapterDesktopDisplayMode);
				if(pDeviceSettings->AdapterFormat != adapterDesktopDisplayMode.Format)
				{
					DXUTMatchOptions matchOptions;
					matchOptions.eAdapterOrdinal     = DXUTMT_PRESERVE_INPUT;
					matchOptions.eDeviceType         = DXUTMT_PRESERVE_INPUT;
					matchOptions.eWindowed           = DXUTMT_PRESERVE_INPUT;
					matchOptions.eAdapterFormat      = DXUTMT_PRESERVE_INPUT;
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

					DXUTDeviceSettings deviceSettings = DXUTGetDeviceSettings();
					deviceSettings.AdapterFormat = adapterDesktopDisplayMode.Format;

					hr = DXUTFindValidDeviceSettings(&deviceSettings, &deviceSettings, &matchOptions);
					if(FAILED(hr)) // the call will fail if no valid devices were found
					{
						DXUTDisplayErrorMessage(DXUTERR_NOCOMPATIBLEDEVICES);
						DXUTShutdown();
					}

					// Change to a Direct3D device created from the new device settings.  
					// If there is an existing device, then either reset or recreate the scene
					hr = DXUTChangeDevice(&deviceSettings, NULL, false, false);
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
						DXUTDeviceSettings* pDeviceSettings = GetDXUTState().GetCurrentDeviceSettings();
						if(FAILED(DXUTChangeDevice(pDeviceSettings, NULL, true, false)))
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
	DWORD dwEndRenderTime = GetTickCount();


	// Get the app's time, in seconds. Skip rendering if no time elapsed
	double fTime, fAbsTime; float fElapsedTime, fSleepTime;
	GetGlobalTimer().GetTimeValues(&fTime, &fAbsTime, &fElapsedTime, fSleepTime, 60);

	// 锁帧
	//{
	//	double fSleepTime = 1.0f/60.0f - fElapsedTime;
	//	if (fSleepTime > 0.0f)
	//	{
	//		Sleep(fSleepTime*1000);
	//		fElapsedTime += fSleepTime;
	//		fTime += fSleepTime;
	//		fAbsTime += fSleepTime;
	//	}
	//	else
	//	{
	//		fSleepTime = 0.0f;
	//	}
	//	GetDXUTState().SetSleepTime(fSleepTime);
	//}

	return true;
}

void CD3D9RenderWindow::FrameEnd()
{
	HRESULT hr;

	IDirect3DDevice9* pd3dDevice = DXUTGetD3DDevice();
	if(NULL == pd3dDevice) // Handle DXUTShutdown from inside callback
		return;


#if defined(DEBUG) || defined(_DEBUG)
	// check backbuffer covers the entire window
	/*RECT rcClient;
	GetClientRect(GetHWND(), &rcClient);
	if(!IsIconic(GetHWND()))
	{
		GetClientRect(GetHWND(), &rcClient);
		assert(DXUTGetBackBufferSurfaceDesc()->Width == (UINT)rcClient.right);
		assert(DXUTGetBackBufferSurfaceDesc()->Height == (UINT)rcClient.bottom);
	}*/
#endif

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

LRESULT CD3D9RenderWindow::MsgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch(uMsg)
	{
	case WM_PAINT:
		{
			IDirect3DDevice9* pd3dDevice = DXUTGetD3DDevice();

			if(pd3dDevice && m_bMoving)
			{
				HRESULT hr;
				// double fTime = DXUTGetTime();
				// float fElapsedTime = DXUTGetElapsedTime();

				// GetRoot().OnFrameRender(fTime, fElapsedTime);

				hr = pd3dDevice->Present(NULL, NULL, NULL, NULL);
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
				DXUTCheckForWindowSizeChange();
				DXUTCheckForWindowChangingMonitors();
			}
			else if(SIZE_RESTORED == wParam)
			{      
				m_bMinimized = false;
				if(!m_bMoving)
				{
					DXUTCheckForWindowSizeChange();
					DXUTCheckForWindowChangingMonitors();
				}
			}
		}
		break;
#define DXUT_MIN_WINDOW_SIZE_X 200
#define DXUT_MIN_WINDOW_SIZE_Y 200
	case WM_GETMINMAXINFO:
		((MINMAXINFO*)lParam)->ptMinTrackSize.x = DXUT_MIN_WINDOW_SIZE_X;
		((MINMAXINFO*)lParam)->ptMinTrackSize.y = DXUT_MIN_WINDOW_SIZE_Y;
		break;

	case WM_ENTERSIZEMOVE:
		// Halt frame movement while the app is sizing or moving
		m_bMoving = true;
		break;

	case WM_EXITSIZEMOVE:
		DXUTCheckForWindowSizeChange();
		DXUTCheckForWindowChangingMonitors();
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
			if(GetDXUTState().GetMinimizedWhileFullscreen()) 
			{
				m_bMinimized = false;
				GetDXUTState().SetMinimizedWhileFullscreen(false);
			}

		}
		else if(wParam == FALSE && m_bActive) // Handle only if previously active 
		{           
			m_bActive = false;
			if(!DXUTIsWindowed())
			{
				m_bMinimized = true;
				// Going from full screen to a minimized state 
				ClipCursor(NULL);      // don't limit the cursor anymore
				GetDXUTState().SetMinimizedWhileFullscreen(true); 
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
		GetGlobalTimer().Reset();                   
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
					if(GetDXUTState().GetHandleAltEnter())
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
				case VK_F3:
					{
						DXUTToggleREF();                     
						break;
					}

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
						SendMessage(hWnd, WM_CLOSE, 0, 0);
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
			HMENU hMenu;
			hMenu = GetMenu(hWnd);
			if(hMenu != NULL)
				DestroyMenu(hMenu);
			DestroyWindow(hWnd);
			UnregisterClass(L"Direct3DWindowClass", NULL);
			GetDXUTState().SetHWNDFocus(NULL);
			GetDXUTState().SetHWNDDeviceFullScreen(NULL);
			GetDXUTState().SetHWNDDeviceWindowed(NULL);
			return 0;
		}

	case WM_DESTROY:
		PostQuitMessage(WM_QUIT);
		break;
	}
	return 0;
}