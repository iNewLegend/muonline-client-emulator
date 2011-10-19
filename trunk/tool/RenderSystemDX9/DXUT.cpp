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
	ZeroMemory(this, sizeof(DXUTState)); 
	m_OverrideAdapterOrdinal = -1; 
	m_AutoChangeAdapter = true;
	m_Active = true;
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

HRESULT DXUTCreate3DEnvironment(IDirect3DDevice9* pd3dDeviceFromApp);

void    D3DCleanup3DEnvironment(bool bReleaseSettings = true);

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

HRESULT D3DInit()
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
        return DXUT_ERR(L"Direct3DCreate9", DXUTERR_NODIRECT3D);
    }

    GetDXUTState().SetDXUTInited(true);

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
        //hr = D3DCreateWindow();
       // if(FAILED(hr))
       //     return hr;
    }

    D3DDeviceSettings* pDeviceSettings = new D3DDeviceSettings;
    if(pDeviceSettings == NULL)
        return E_OUTOFMEMORY;
    ZeroMemory(pDeviceSettings, sizeof(D3DDeviceSettings));

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
//     DXUTFindAdapterFormat(pDeviceSettings->AdapterOrdinal, pDeviceSettings->DeviceType, 
//                            pDeviceSettings->pp.BackBufferFormat, pDeviceSettings->pp.Windowed, 
//                            &pDeviceSettings->AdapterFormat);
    pDeviceSettings->BehaviorFlags  = d3dCreationParams.BehaviorFlags;

    // Change to the Direct3D device passed in
    hr = D3DChangeDevice(pDeviceSettings, pd3dDevice, false, false);

    delete pDeviceSettings;

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
    D3DDeviceSettings deviceSettings = DXUTGetDeviceSettings();
    deviceSettings.pp.Windowed = !deviceSettings.pp.Windowed;


    // Go back to previous state

    UINT nWidth  = (deviceSettings.pp.Windowed) ? GetDXUTState().GetWindowBackBufferWidthAtModeChange() : GetDXUTState().GetFullScreenBackBufferWidthAtModeChange();
    UINT nHeight = (deviceSettings.pp.Windowed) ? GetDXUTState().GetWindowBackBufferHeightAtModeChange() : GetDXUTState().GetFullScreenBackBufferHeightAtModeChange();
    if(nWidth > 0 && nHeight > 0)
    {
        //matchOptions.eResolution = DXUTMT_CLOSEST_TO_INPUT;
        deviceSettings.pp.BackBufferWidth = nWidth;
        deviceSettings.pp.BackBufferHeight = nHeight;
    }
    else
    {
        // No previous data, so just switch to defaults
       // matchOptions.eResolution = DXUTMT_IGNORE_INPUT;
    }


    {
        // Create a Direct3D device using the new device settings.  
        // If there is an existing device, then it will either reset or recreate the scene.
        hr = D3DChangeDevice(&deviceSettings, NULL, false, false);

        // If hr == E_ABORT, this means the app rejected the device settings in the ModifySettingsCallback so nothing changed
        if(FAILED(hr) && (hr != E_ABORT))
        {
            // Failed creating device, try to switch back.
            deviceSettings.pp.Windowed = !deviceSettings.pp.Windowed;
            UINT nWidth  = (deviceSettings.pp.Windowed) ? GetDXUTState().GetWindowBackBufferWidthAtModeChange() : GetDXUTState().GetFullScreenBackBufferWidthAtModeChange();
            UINT nHeight = (deviceSettings.pp.Windowed) ? GetDXUTState().GetWindowBackBufferHeightAtModeChange() : GetDXUTState().GetFullScreenBackBufferHeightAtModeChange();
            if(nWidth > 0 && nHeight > 0)
            {
               // matchOptions.eResolution = DXUTMT_CLOSEST_TO_INPUT;
                deviceSettings.pp.BackBufferWidth = nWidth;
                deviceSettings.pp.BackBufferHeight = nHeight;
            }
            else
            {
                //matchOptions.eResolution = DXUTMT_IGNORE_INPUT;
            }


            HRESULT hr2 = D3DChangeDevice(&deviceSettings, NULL, false, false);
            if(FAILED(hr2))
            {
                // If this failed, then shutdown
                DXUTShutdown();
            }
        }
    }

    return hr;
}


// Change to a Direct3D device created from the device settings or passed in.
// The framework will only reset if the device is similar to the previous device 
// otherwise it will cleanup the previous device (if there is one) and recreate the 
// scene using the app's device callbacks.

HRESULT D3DChangeDevice(D3DDeviceSettings* pNewDeviceSettings, IDirect3DDevice9* pd3dDeviceFromApp, bool bForceRecreate, bool bClipWindowToSingleAdapter)
{
    HRESULT hr;
    D3DDeviceSettings* pOldDeviceSettings = GetDXUTState().GetCurrentDeviceSettings();

    if(DXUTGetD3DObject() == NULL)
        return S_FALSE;

    // Make a copy of the pNewDeviceSettings on the heap
    D3DDeviceSettings* pNewDeviceSettingsOnHeap = new D3DDeviceSettings;
    if(pNewDeviceSettingsOnHeap == NULL)
        return E_OUTOFMEMORY;
    memcpy(pNewDeviceSettingsOnHeap, pNewDeviceSettings, sizeof(D3DDeviceSettings));
    pNewDeviceSettings = pNewDeviceSettingsOnHeap;

    GetDXUTState().SetCurrentDeviceSettings(pNewDeviceSettings);

    // Only apply the cmd line overrides if this is the first device created
    // and DXUTSetDevice() isn't used
    if(NULL == pd3dDeviceFromApp && NULL == pOldDeviceSettings)
    {
        // Updates the device settings struct based on the cmd line args.  
        // Warning: if the device doesn't support these new settings then CreateDevice() will fail.
		if(GetDXUTState().GetOverrideAdapterOrdinal() != -1)
			pNewDeviceSettings->AdapterOrdinal = GetDXUTState().GetOverrideAdapterOrdinal();

		pNewDeviceSettings->pp.Windowed = true;

		pNewDeviceSettings->DeviceType = D3DDEVTYPE_HAL;

		if(GetDXUTState().GetOverrideForcePureHWVP())
		{
			pNewDeviceSettings->BehaviorFlags &= ~D3DCREATE_SOFTWARE_VERTEXPROCESSING;
			pNewDeviceSettings->BehaviorFlags |= D3DCREATE_HARDWARE_VERTEXPROCESSING;
			pNewDeviceSettings->BehaviorFlags |= D3DCREATE_PUREDEVICE;
		}
		else
		{
			pNewDeviceSettings->BehaviorFlags &= ~D3DCREATE_SOFTWARE_VERTEXPROCESSING;
			pNewDeviceSettings->BehaviorFlags &= ~D3DCREATE_PUREDEVICE;
			pNewDeviceSettings->BehaviorFlags |= D3DCREATE_HARDWARE_VERTEXPROCESSING;
		}
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
            SetWindowLong(DXUTGetHWND(), GWL_STYLE, GetDXUTState().GetWindowedStyleAtModeChange());
        }
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
            GetWindowPlacement(DXUTGetHWND(), pwp);
            bool bIsTopmost = ((GetWindowLong(DXUTGetHWND(),GWL_EXSTYLE) & WS_EX_TOPMOST) != 0); 
            GetDXUTState().SetTopmostWhileWindowed(bIsTopmost);
            DWORD dwStyle = GetWindowLong(DXUTGetHWND(), GWL_STYLE);
            dwStyle &= ~WS_MAXIMIZE & ~WS_MINIMIZE; // remove minimize/maximize style
            GetDXUTState().SetWindowedStyleAtModeChange(dwStyle);
            if(pOldDeviceSettings)
            {
                GetDXUTState().SetWindowBackBufferWidthAtModeChange(pOldDeviceSettings->pp.BackBufferWidth);
                GetDXUTState().SetWindowBackBufferHeightAtModeChange(pOldDeviceSettings->pp.BackBufferHeight);
            }
        }

        // Hide the window to avoid animation of blank windows
        ShowWindow(DXUTGetHWND(), SW_HIDE);

        // Set FS window style
        SetWindowLong(DXUTGetHWND(), GWL_STYLE, WS_POPUP|WS_SYSMENU);

        WINDOWPLACEMENT wpFullscreen;
        ZeroMemory(&wpFullscreen, sizeof(WINDOWPLACEMENT));
        wpFullscreen.length = sizeof(WINDOWPLACEMENT);
        GetWindowPlacement(DXUTGetHWND(), &wpFullscreen);
        if((wpFullscreen.flags & WPF_RESTORETOMAXIMIZED) != 0)
        {
            // Restore the window to normal if the window was maximized then minimized.  This causes the 
            // WPF_RESTORETOMAXIMIZED flag to be set which will cause SW_RESTORE to restore the 
            // window from minimized to maxmized which isn't what we want
            wpFullscreen.flags &= ~WPF_RESTORETOMAXIMIZED;
            wpFullscreen.showCmd = SW_RESTORE;
            SetWindowPlacement(DXUTGetHWND(), &wpFullscreen);
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
		GetClientRect(DXUTGetHWND(), &rcClient);
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
				if(FAILED(D3DChangeDevice(pNewDeviceSettings, pd3dDeviceFromApp, true, bClipWindowToSingleAdapter)))
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
            D3DCleanup3DEnvironment(false);

        // Create the D3D device and call the app's device callbacks
        hr = DXUTCreate3DEnvironment(pd3dDeviceFromApp);
        if(FAILED(hr))
        {
            S_DEL(pOldDeviceSettings);
            D3DCleanup3DEnvironment();
            DXUTDisplayErrorMessage(hr);
            return hr;
        }
    }

    IDirect3D9* pD3D = DXUTGetD3DObject();
    HMONITOR hAdapterMonitor = pD3D->GetAdapterMonitor(pNewDeviceSettings->AdapterOrdinal);
    GetDXUTState().SetAdapterMonitor(hAdapterMonitor);

    if(pOldDeviceSettings && !pOldDeviceSettings->pp.Windowed && pNewDeviceSettings->pp.Windowed)
    {
        // Going from fullscreen -> windowed

        // Restore the show state, and positions/size of the window to what it was
        // It is important to adjust the window size 
        // after resetting the device rather than beforehand to ensure 
        // that the monitor resolution is correct and does not limit the size of the new window.
        WINDOWPLACEMENT* pwp = GetDXUTState().GetWindowedPlacement();
        SetWindowPlacement(DXUTGetHWND(), pwp);

        // Also restore the z-order of window to previous state
        HWND hWndInsertAfter = GetDXUTState().GetTopmostWhileWindowed() ? HWND_TOPMOST : HWND_NOTOPMOST;
        SetWindowPos(DXUTGetHWND(), hWndInsertAfter, 0, 0, 0, 0, SWP_NOMOVE|SWP_NOREDRAW|SWP_NOSIZE);
    }

    // Check to see if the window needs to be resized.  
    // Handle cases where the window is minimized and maxmimized as well.
    bool bNeedToResize = false;
    if(pNewDeviceSettings->pp.Windowed && // only resize if in windowed mode
        !bKeepCurrentWindowSize)          // only resize if pp.BackbufferWidth/Height were not 0
    {
        UINT nClientWidth;
        UINT nClientHeight;    
        if(IsIconic(DXUTGetHWND()))
        {
			//最小化的时候 不用管它
            // Window is restored or maximized so just get its client rect
            RECT rcClient;
            GetClientRect(DXUTGetHWND(), &rcClient);
            nClientWidth  = (UINT)(rcClient.right - rcClient.left);
            nClientHeight = (UINT)(rcClient.bottom - rcClient.top);
        }
        else
        {
            // Window is restored or maximized so just get its client rect
            RECT rcClient;
            GetClientRect(DXUTGetHWND(), &rcClient);
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

        if(bClipWindowToSingleAdapter && !IsIconic(DXUTGetHWND()))
        {
            // Get the rect of the monitor attached to the adapter
            MONITORINFO miAdapter;
            miAdapter.cbSize = sizeof(MONITORINFO);
            HMONITOR hAdapterMonitor = DXUTGetD3DObject()->GetAdapterMonitor(pNewDeviceSettings->AdapterOrdinal);
            DXUTGetMonitorInfo(hAdapterMonitor, &miAdapter);
            HMONITOR hWindowMonitor = DXUTMonitorFromWindow(DXUTGetHWND(), MONITOR_DEFAULTTOPRIMARY);

            // Get the rect of the window
            RECT rcWindow;
            GetWindowRect(DXUTGetHWND(), &rcWindow);

            // Check if the window rect is fully inside the adapter's vitural screen rect
            if((rcWindow.left   < miAdapter.rcWork.left  ||
                 rcWindow.right  > miAdapter.rcWork.right ||
                 rcWindow.top    < miAdapter.rcWork.top   ||
                 rcWindow.bottom > miAdapter.rcWork.bottom))
            {
                if(hWindowMonitor == hAdapterMonitor && IsZoomed(DXUTGetHWND()))
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
        if(IsIconic(DXUTGetHWND())||IsZoomed(DXUTGetHWND())) 
            ShowWindow(DXUTGetHWND(), SW_RESTORE);

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
            GetWindowRect(DXUTGetHWND(), &rcWindow);

            // Make a window rect with a client rect that is the same size as the backbuffer
            RECT rcResizedWindow;
            rcResizedWindow.left = 0;
            rcResizedWindow.right = nClientWidth;
            rcResizedWindow.top = 0;
            rcResizedWindow.bottom = nClientHeight;
            AdjustWindowRect(&rcResizedWindow, GetWindowLong(DXUTGetHWND(), GWL_STYLE), false);

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
            SetWindowPos(DXUTGetHWND(), 0, rcResizedWindow.left, rcResizedWindow.top, nWindowWidth, nWindowHeight, SWP_NOZORDER);
        }        
        else
        {      
            // Make a window rect with a client rect that is the same size as the backbuffer
            RECT rcWindow = {0};
            rcWindow.right = (long)(pNewDeviceSettings->pp.BackBufferWidth);
            rcWindow.bottom = (long)(pNewDeviceSettings->pp.BackBufferHeight);
            AdjustWindowRect(&rcWindow, GetWindowLong(DXUTGetHWND(), GWL_STYLE), false);

            // Resize the window.  It is important to adjust the window size 
            // after resetting the device rather than beforehand to ensure 
            // that the monitor resolution is correct and does not limit the size of the new window.
            int cx = (int)(rcWindow.right - rcWindow.left);
            int cy = (int)(rcWindow.bottom - rcWindow.top);
            SetWindowPos(DXUTGetHWND(), 0, 0, 0, cx, cy, SWP_NOZORDER|SWP_NOMOVE);
        }

        // Its possible that the new window size is not what we asked for.  
        // No window can be sized larger than the desktop, so see see if the Windows OS resized the 
        // window to something smaller to fit on the desktop.  Also if WM_GETMINMAXINFO
        // will put a limit on the smallest/largest window size.
        RECT rcClient;
        GetClientRect(DXUTGetHWND(), &rcClient);
        UINT nClientWidth  = (UINT)(rcClient.right - rcClient.left);
        UINT nClientHeight = (UINT)(rcClient.bottom - rcClient.top);
        if(nClientWidth  != pNewDeviceSettings->pp.BackBufferWidth ||
            nClientHeight != pNewDeviceSettings->pp.BackBufferHeight)
        {
            // If its different, then resize the backbuffer again.  This time create a backbuffer that matches the 
            // client rect of the current window w/o resizing the window.
            D3DDeviceSettings deviceSettings = DXUTGetDeviceSettings();
            deviceSettings.pp.BackBufferWidth  = 0; 
            deviceSettings.pp.BackBufferHeight = 0;
            hr = D3DChangeDevice(&deviceSettings, NULL, false, bClipWindowToSingleAdapter);
            if(FAILED(hr))
            {
                S_DEL(pOldDeviceSettings);
                D3DCleanup3DEnvironment();
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
// Creates the 3D environment

HRESULT DXUTCreate3DEnvironment(IDirect3DDevice9* pd3dDeviceFromApp)
{
    HRESULT hr = S_OK;

    IDirect3DDevice9* pd3dDevice = NULL;
    D3DDeviceSettings* pNewDeviceSettings = GetDXUTState().GetCurrentDeviceSettings();

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

    // Setup cursor based on current settings (window/fullscreen mode, show cursor state, clip cursor state)
    DXUTSetupCursor();

    // Update GetDXUTState()'s copy of D3D caps 
    D3DCAPS9* pd3dCaps = GetDXUTState().GetCaps();
    DXUTGetD3DDevice()->GetDeviceCaps(pd3dCaps);

    // Update the device stats text

    // Call the app's device created callback if non-NULL

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
    D3DDeviceSettings* pDeviceSettings = GetDXUTState().GetCurrentDeviceSettings();

    hr = pd3dDevice->Reset(&pDeviceSettings->pp);
    if(FAILED(hr))  
    {
		std::wstring msg = DXGetErrorDescription(hr);
        if(hr == D3DERR_DEVICELOST)
            return D3DERR_DEVICELOST; // Reset could legitimately fail if the device is lost
        else
            return DXUT_ERR(L"Reset", DXUTERR_RESETTINGDEVICE);
    }

    // Setup cursor based on current settings (window/fullscreen mode, show cursor state, clip cursor state)
    DXUTSetupCursor();
	GetD3D9RenderSystem().OnResetDevice(); //DXUTGetD3DDevice()
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



// Handles window messages 


// Closes down the window.  When the window closes, it will cleanup everything

void DXUTShutdown()
{
    if(DXUTGetHWND())
        SendMessage(DXUTGetHWND(), WM_CLOSE, 0, 0);

    D3DCleanup3DEnvironment(true);

    S_REL(GetDXUTState().m_D3D);
}

void D3DCleanup3DEnvironment(bool bReleaseSettings)
{
	//      - Calls the device lost callback 
	//      - Calls the device destroyed callback 
	//      - Releases the D3D device
    IDirect3DDevice9* pd3dDevice = GetDXUTState().GetD3DDevice();
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
            S_DEL(GetDXUTState().m_CurrentDeviceSettings);  
        }

        D3DCAPS9* pd3dCaps = GetDXUTState().GetCaps();
        ZeroMemory(pd3dCaps, sizeof(D3DCAPS9));

        GetDXUTState().SetDeviceCreated(false);
    }
}

// External state access functions
IDirect3D9* DXUTGetD3DObject()                      { return GetDXUTState().GetD3D(); }        
IDirect3DDevice9* DXUTGetD3DDevice()                { return GetDXUTState().GetD3DDevice(); }  
HWND DXUTGetHWND()                                  { return GetDXUTState().GetHWND(); }
HWND DXUTGetHWNDFocus()                             { return GetDXUTState().GetHWNDFocus(); }

// Return if windowed in the current device.  If no device exists yet, then returns false
bool DXUTIsWindowed()                               
{ 
    D3DDeviceSettings* pDeviceSettings = GetDXUTState().GetCurrentDeviceSettings(); 
    if(pDeviceSettings) 
        return (pDeviceSettings->pp.Windowed != 0); 
    else 
        return false; 
}

// Return the device settings of the current device.  If no device exists yet, then
// return blank device settings 

D3DDeviceSettings DXUTGetDeviceSettings()   
{ 
    D3DDeviceSettings* pDS = GetDXUTState().GetCurrentDeviceSettings();
    if(pDS)
    {
        return *pDS;
    }
    else
    {
        D3DDeviceSettings ds;
        ZeroMemory(&ds, sizeof(D3DDeviceSettings));
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
            HCURSOR hCursor = (HCURSOR)(ULONG_PTR)GetClassLongPtr(DXUTGetHWND(), GCLP_HCURSOR);
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
        GetWindowRect(DXUTGetHWND(), &rcWindow);
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



