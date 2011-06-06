#include "IME.h"

// #undef min // use __min instead
// #undef max // use __max instead

#define IMM32_DLLNAME L"\\imm32.dll"
#define VER_DLLNAME L"\\version.dll"

#pragma comment (lib, "imm32.lib")
//#include <windows.h>
#include <imm.h>


#define GETPROCADDRESS(Module, APIName, Temp) \
	Temp = GetProcAddress(Module, #APIName); \
	if(Temp) \
	*(FARPROC*)&_##APIName = Temp

#define PLACEHOLDERPROC(APIName) \
	_##APIName = Dummy_##APIName

// CIME class

// IME constants
#define CHT_IMEFILENAME1    "TINTLGNT.IME" // New Phonetic
#define CHT_IMEFILENAME2    "CINTLGNT.IME" // New Chang Jie
#define CHT_IMEFILENAME3    "MSTCIPHA.IME" // Phonetic 5.1
#define CHS_IMEFILENAME1    "PINTLGNT.IME" // MSPY1.5/2/3
#define CHS_IMEFILENAME2    "MSSCIPYA.IME" // MSPY3 for OfficeXP

#define LANG_CHT            MAKELANGID(LANG_CHINESE, SUBLANG_CHINESE_TRADITIONAL)
#define LANG_CHS            MAKELANGID(LANG_CHINESE, SUBLANG_CHINESE_SIMPLIFIED)
#define _CHT_HKL            ((HKL)(INT_PTR)0xE0080404) // New Phonetic
#define _CHT_HKL2           ((HKL)(INT_PTR)0xE0090404) // New Chang Jie
#define _CHS_HKL            ((HKL)(INT_PTR)0xE00E0804) // MSPY
#define MAKEIMEVERSION(major, minor)      ((DWORD)(((BYTE)(major) << 24) | ((BYTE)(minor) << 16)))

#define IMEID_CHT_VER42 (LANG_CHT | MAKEIMEVERSION(4, 2))   // New(Phonetic/ChanJie)IME98  : 4.2.x.x // Win98
#define IMEID_CHT_VER43 (LANG_CHT | MAKEIMEVERSION(4, 3))   // New(Phonetic/ChanJie)IME98a : 4.3.x.x // Win2k
#define IMEID_CHT_VER44 (LANG_CHT | MAKEIMEVERSION(4, 4))   // New ChanJie IME98b          : 4.4.x.x // WinXP
#define IMEID_CHT_VER50 (LANG_CHT | MAKEIMEVERSION(5, 0))   // New(Phonetic/ChanJie)IME5.0 : 5.0.x.x // WinME
#define IMEID_CHT_VER51 (LANG_CHT | MAKEIMEVERSION(5, 1))   // New(Phonetic/ChanJie)IME5.1 : 5.1.x.x // IME2002(w/OfficeXP)
#define IMEID_CHT_VER52 (LANG_CHT | MAKEIMEVERSION(5, 2))   // New(Phonetic/ChanJie)IME5.2 : 5.2.x.x // IME2002a(w/Whistler)
#define IMEID_CHT_VER60 (LANG_CHT | MAKEIMEVERSION(6, 0))   // New(Phonetic/ChanJie)IME6.0 : 6.0.x.x // IME XP(w/WinXP SP1)
#define IMEID_CHS_VER41 (LANG_CHS | MAKEIMEVERSION(4, 1))   // MSPY1.5  // SCIME97 or MSPY1.5 (w/Win98, Office97)
#define IMEID_CHS_VER42 (LANG_CHS | MAKEIMEVERSION(4, 2))   // MSPY2    // Win2k/WinME
#define IMEID_CHS_VER53 (LANG_CHS | MAKEIMEVERSION(5, 3))   // MSPY3    // WinXP

// Function pointers
INPUTCONTEXT* (WINAPI * CIME::_ImmLockIMC)(HIMC) = CIME::Dummy_ImmLockIMC;
BOOL (WINAPI * CIME::_ImmUnlockIMC)(HIMC) = CIME::Dummy_ImmUnlockIMC;
LPVOID (WINAPI * CIME::_ImmLockIMCC)(HIMCC) = CIME::Dummy_ImmLockIMCC;
BOOL (WINAPI * CIME::_ImmUnlockIMCC)(HIMCC) = CIME::Dummy_ImmUnlockIMCC;
BOOL (WINAPI * CIME::_ImmDisableTextFrameService)(DWORD) = CIME::Dummy_ImmDisableTextFrameService;
LONG (WINAPI * CIME::_ImmGetCompositionStringW)(HIMC, DWORD, LPVOID, DWORD) = CIME::Dummy_ImmGetCompositionStringW;
DWORD (WINAPI * CIME::_ImmGetCandidateListW)(HIMC, DWORD, LPCANDIDATELIST, DWORD) = CIME::Dummy_ImmGetCandidateListW;
HIMC (WINAPI * CIME::_ImmGetContext)(HWND) = CIME::Dummy_ImmGetContext;
BOOL (WINAPI * CIME::_ImmReleaseContext)(HWND, HIMC) = CIME::Dummy_ImmReleaseContext;
HIMC (WINAPI * CIME::_ImmAssociateContext)(HWND, HIMC) = CIME::Dummy_ImmAssociateContext;
BOOL (WINAPI * CIME::_ImmGetOpenStatus)(HIMC) = CIME::Dummy_ImmGetOpenStatus;
BOOL (WINAPI * CIME::_ImmSetOpenStatus)(HIMC, BOOL) = CIME::Dummy_ImmSetOpenStatus;
BOOL (WINAPI * CIME::_ImmGetConversionStatus)(HIMC, LPDWORD, LPDWORD) = CIME::Dummy_ImmGetConversionStatus;
HWND (WINAPI * CIME::_ImmGetDefaultIMEWnd)(HWND) = CIME::Dummy_ImmGetDefaultIMEWnd;
UINT (WINAPI * CIME::_ImmGetIMEFileNameA)(HKL, LPSTR, UINT) = CIME::Dummy_ImmGetIMEFileNameA;
UINT (WINAPI * CIME::_ImmGetVirtualKey)(HWND) = CIME::Dummy_ImmGetVirtualKey;
BOOL (WINAPI * CIME::_ImmNotifyIME)(HIMC, DWORD, DWORD, DWORD) = CIME::Dummy_ImmNotifyIME;
BOOL (WINAPI * CIME::_ImmSetConversionStatus)(HIMC, DWORD, DWORD) = CIME::Dummy_ImmSetConversionStatus;
BOOL (WINAPI * CIME::_ImmSimulateHotKey)(HWND, DWORD) = CIME::Dummy_ImmSimulateHotKey;
BOOL (WINAPI * CIME::_ImmIsIME)(HKL) = CIME::Dummy_ImmIsIME;
UINT (WINAPI * CIME::_GetReadingString)(HIMC, UINT, LPWSTR, PINT, BOOL*, PUINT) = CIME::Dummy_GetReadingString; // Traditional Chinese IME
BOOL (WINAPI * CIME::_ShowReadingWindow)(HIMC, BOOL) = CIME::Dummy_ShowReadingWindow; // Traditional Chinese IME
BOOL (APIENTRY * CIME::_VerQueryValueA)(const LPVOID, LPSTR, LPVOID *, PUINT) = CIME::Dummy_VerQueryValueA;
BOOL (APIENTRY * CIME::_GetFileVersionInfoA)(LPSTR, DWORD, DWORD, LPVOID)= CIME::Dummy_GetFileVersionInfoA;
DWORD (APIENTRY * CIME::_GetFileVersionInfoSizeA)(LPSTR, LPDWORD) = CIME::Dummy_GetFileVersionInfoSizeA;

HINSTANCE CIME::s_hDllImm32;      // IMM32 DLL handle
HINSTANCE CIME::s_hDllVer;        // Version DLL handle
HKL       CIME::s_hklCurrent;     // Current keyboard layout of the process
bool      CIME::s_bVerticalCand;  // Indicates that the candidates are listed vertically
WCHAR     CIME::s_aszIndicator[5][3] = // String to draw to indicate current input locale
            {
                L"En",
                L"\x7B80",
                L"\x7E41",
                L"\xAC00",
                L"\x3042",
            };
LPWSTR    CIME::s_wszCurrIndicator = CIME::s_aszIndicator[0];  // Points to an indicator string that corresponds to current input locale
bool      CIME::s_bInsertOnType;     // Insert the character as soon as a key is pressed (Korean behavior)
HINSTANCE CIME::s_hDllIme;           // Instance handle of the current IME module
HIMC      CIME::s_hImcDef;           // Default input context
CIME::IMESTATE  CIME::s_ImeState = IMEUI_STATE_OFF;
bool      CIME::s_bEnableImeSystem;  // Whether the IME system is active
POINT     CIME::s_ptCompString;      // Composition string position. Updated every frame.
int       CIME::s_nCompCaret;
int       CIME::s_nFirstTargetConv;  // Index of the first target converted char in comp string.  If none, -1.
CUniBuffer CIME::s_CompString = CUniBuffer(0);
BYTE      CIME::s_abCompStringAttr[MAX_COMPSTRING_SIZE];
DWORD     CIME::s_adwCompStringClause[MAX_COMPSTRING_SIZE];
//WCHAR     CIME::s_wszReadingString[32];
std::wstring	CIME::s_wstrReadingString;
CIME::CCandList CIME::s_CandList;       // Data relevant to the candidate list
bool      CIME::s_bShowReadingWindow; // Indicates whether reading window is visible
bool      CIME::s_bHorizontalReading; // Indicates whether the reading window is vertical or horizontal
bool      CIME::s_bChineseIME;
std::vector<CIME::CInputLocale> CIME::s_Locale; // Array of loaded keyboard layout on system
#if defined(DEBUG) || defined(_DEBUG)
bool      CIME::m_bIMEStaticMsgProcCalled = false;
#endif


void CIME::Initialize()
{
    if(s_hDllImm32) // Only need to do once
        return;

    FARPROC Temp;

   // s_CompString.SetBufferSize(MAX_COMPSTRING_SIZE);

    WCHAR wszPath[MAX_PATH+1];
    if(!::GetSystemDirectoryW(wszPath, MAX_PATH+1))
        return;

	std::wstring wstrPath = wszPath;
	wstrPath += IMM32_DLLNAME;
    s_hDllImm32 = LoadLibraryW(wstrPath.c_str());
    if(s_hDllImm32)
    {
        GETPROCADDRESS(s_hDllImm32, ImmLockIMC, Temp);
        GETPROCADDRESS(s_hDllImm32, ImmUnlockIMC, Temp);
        GETPROCADDRESS(s_hDllImm32, ImmLockIMCC, Temp);
        GETPROCADDRESS(s_hDllImm32, ImmUnlockIMCC, Temp);
        GETPROCADDRESS(s_hDllImm32, ImmDisableTextFrameService, Temp);
        GETPROCADDRESS(s_hDllImm32, ImmGetCompositionStringW, Temp);
        GETPROCADDRESS(s_hDllImm32, ImmGetCandidateListW, Temp);
        GETPROCADDRESS(s_hDllImm32, ImmGetContext, Temp);
        GETPROCADDRESS(s_hDllImm32, ImmReleaseContext, Temp);
        GETPROCADDRESS(s_hDllImm32, ImmAssociateContext, Temp);
        GETPROCADDRESS(s_hDllImm32, ImmGetOpenStatus, Temp);
        GETPROCADDRESS(s_hDllImm32, ImmSetOpenStatus, Temp);
        GETPROCADDRESS(s_hDllImm32, ImmGetConversionStatus, Temp);
        GETPROCADDRESS(s_hDllImm32, ImmGetDefaultIMEWnd, Temp);
        GETPROCADDRESS(s_hDllImm32, ImmGetIMEFileNameA, Temp);
        GETPROCADDRESS(s_hDllImm32, ImmGetVirtualKey, Temp);
        GETPROCADDRESS(s_hDllImm32, ImmNotifyIME, Temp);
        GETPROCADDRESS(s_hDllImm32, ImmSetConversionStatus, Temp);
        GETPROCADDRESS(s_hDllImm32, ImmSimulateHotKey, Temp);
        GETPROCADDRESS(s_hDllImm32, ImmIsIME, Temp);
    }

    if(!::GetSystemDirectoryW(wszPath, MAX_PATH+1))
        return;
	wstrPath = wszPath;
	wstrPath+=VER_DLLNAME;
    s_hDllVer = LoadLibraryW(wstrPath.c_str());
    if(s_hDllVer)
    {
        GETPROCADDRESS(s_hDllVer, VerQueryValueA, Temp);
        GETPROCADDRESS(s_hDllVer, GetFileVersionInfoA, Temp);
        GETPROCADDRESS(s_hDllVer, GetFileVersionInfoSizeA, Temp);
    }
}



void CIME::Uninitialize()
{
    if(s_hDllImm32)
    {
        PLACEHOLDERPROC(ImmLockIMC);
        PLACEHOLDERPROC(ImmUnlockIMC);
        PLACEHOLDERPROC(ImmLockIMCC);
        PLACEHOLDERPROC(ImmUnlockIMCC);
        PLACEHOLDERPROC(ImmDisableTextFrameService);
        PLACEHOLDERPROC(ImmGetCompositionStringW);
        PLACEHOLDERPROC(ImmGetCandidateListW);
        PLACEHOLDERPROC(ImmGetContext);
        PLACEHOLDERPROC(ImmReleaseContext);
        PLACEHOLDERPROC(ImmAssociateContext);
        PLACEHOLDERPROC(ImmGetOpenStatus);
        PLACEHOLDERPROC(ImmSetOpenStatus);
        PLACEHOLDERPROC(ImmGetConversionStatus);
        PLACEHOLDERPROC(ImmGetDefaultIMEWnd);
        PLACEHOLDERPROC(ImmGetIMEFileNameA);
        PLACEHOLDERPROC(ImmGetVirtualKey);
        PLACEHOLDERPROC(ImmNotifyIME);
        PLACEHOLDERPROC(ImmSetConversionStatus);
        PLACEHOLDERPROC(ImmSimulateHotKey);
        PLACEHOLDERPROC(ImmIsIME);

        FreeLibrary(s_hDllImm32);
        s_hDllImm32 = NULL;
    }
    if(s_hDllIme)
    {
        PLACEHOLDERPROC(GetReadingString);
        PLACEHOLDERPROC(ShowReadingWindow);

        FreeLibrary(s_hDllIme);
        s_hDllIme = NULL;
    }
    if(s_hDllVer)
    {
        PLACEHOLDERPROC(VerQueryValueA);
        PLACEHOLDERPROC(GetFileVersionInfoA);
        PLACEHOLDERPROC(GetFileVersionInfoSizeA);

        FreeLibrary(s_hDllVer);
        s_hDllVer = NULL;
    }
}


// Called by CUIDialogResMgr::OnCreateDevice.  This gives the class a
// chance to initialize its default input context associated with the app window.
HRESULT CIME::StaticOnCreateDevice()
{
	// Save the default input context
	s_hImcDef = ImmGetContext(UIGetHWND());
	//s_hImcDef = _ImmGetContext(UIGetHWND());
	_ImmReleaseContext(UIGetHWND(), s_hImcDef);

	return S_OK;
}


bool CIME::StaticMsgProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	HIMC hImc;

	if(!s_bEnableImeSystem)
		return false;

#if defined(DEBUG) || defined(_DEBUG)
	m_bIMEStaticMsgProcCalled = true;
#endif

	switch(uMsg)
	{
	case WM_ACTIVATEAPP:
		if(wParam)
		{
			// Populate s_Locale with the list of keyboard layouts.
			UINT cKL = GetKeyboardLayoutList(0, NULL);
			s_Locale.clear();
			HKL *phKL = new HKL[cKL];
			if(phKL)
			{
				GetKeyboardLayoutList(cKL, phKL);
				for(UINT i = 0; i < cKL; ++i)
				{
					CInputLocale Locale;

					// Filter out East Asian languages that are not IME.
					if((PRIMARYLANGID(LOWORD(phKL[i])) == LANG_CHINESE ||
						PRIMARYLANGID(LOWORD(phKL[i])) == LANG_JAPANESE ||
						PRIMARYLANGID(LOWORD(phKL[i])) == LANG_KOREAN) &&
						!_ImmIsIME(phKL[i]))
						continue;

					// If this language is already in the list, don't add it again.
					bool bBreak = false;
					for(size_t e = 0; e < s_Locale.size(); ++e)
						if(LOWORD(s_Locale[e].m_hKL) ==
							LOWORD(phKL[i]))
						{
							bBreak = true;
							break;
						}
						if(bBreak)
							break;

						Locale.m_hKL = phKL[i];
						WCHAR wszDesc[128] = L"";
						switch(PRIMARYLANGID(LOWORD(phKL[i])))
						{
							// Simplified Chinese
						case LANG_CHINESE:
							switch(SUBLANGID(LOWORD(phKL[i])))
							{
							case SUBLANG_CHINESE_SIMPLIFIED:
								Locale.m_wstrLangAbb = s_aszIndicator[INDICATOR_CHS];
								break;
							case SUBLANG_CHINESE_TRADITIONAL:
								Locale.m_wstrLangAbb = s_aszIndicator[INDICATOR_CHT];
								break;
							default:    // unsupported sub-language
								GetLocaleInfoW(MAKELCID(LOWORD(phKL[i]), SORT_DEFAULT), LOCALE_SABBREVLANGNAME, wszDesc, 128);
								Locale.m_wstrLangAbb = wszDesc[0];
								Locale.m_wstrLangAbb += towlower(wszDesc[1]);
								break;
							}
							break;
							// Korean
						case LANG_KOREAN:
							Locale.m_wstrLangAbb = s_aszIndicator[INDICATOR_KOREAN];
							break;
							// Japanese
						case LANG_JAPANESE:
							Locale.m_wstrLangAbb = s_aszIndicator[INDICATOR_JAPANESE];
							break;         
						default:
							// A non-IME language.  Obtain the language abbreviation
							// and store it for rendering the indicator later.
							GetLocaleInfoW(MAKELCID(LOWORD(phKL[i]), SORT_DEFAULT), LOCALE_SABBREVLANGNAME, wszDesc, 128);
							Locale.m_wstrLangAbb = wszDesc[0];
							Locale.m_wstrLangAbb += towlower(wszDesc[1]);
							break;
						}

						GetLocaleInfoW(MAKELCID(LOWORD(phKL[i]), SORT_DEFAULT), LOCALE_SLANGUAGE, wszDesc, 128);
						Locale.m_wstrLang = wszDesc;

						s_Locale.push_back(Locale);
				}
				delete[] phKL;
			}
		}
		break;

	case WM_INPUTLANGCHANGE:
		UITRACE(L"WM_INPUTLANGCHANGE\n");
		{
			UINT uLang = GetPrimaryLanguage();
			CheckToggleState();
			if (uLang != GetPrimaryLanguage())
			{
				// Korean IME always inserts on keystroke.  Other IMEs do not.
				s_bInsertOnType = (GetPrimaryLanguage() == LANG_KOREAN);
			}

			// IME changed.  Setup the new IME.
			SetupImeApi();
			if(_ShowReadingWindow)
			{
				if (NULL != (hImc = ImmGetContext(UIGetHWND())))
				{
					_ShowReadingWindow(hImc, false);
					_ImmReleaseContext(UIGetHWND(), hImc);
				}
			}
		}
		return true;

	case WM_IME_SETCONTEXT:
		UITRACE(L"WM_IME_SETCONTEXT\n");
		//
		// We don't want anything to display, so we have to clear this
		//
		lParam = 0;
		return false;

		// Handle WM_IME_STARTCOMPOSITION here since
		// we do not want the default IME handler to see
		// this when our fullscreen app is running.
	case WM_IME_STARTCOMPOSITION:
		UITRACE(L"WM_IME_STARTCOMPOSITION\n");
		ResetCompositionString();
		// Since the composition string has its own caret, we don't render
		// the edit control's own caret to avoid double carets on screen.
///ghghgh		s_bHideCaret = true;
		return true;

	case WM_IME_COMPOSITION:
		UITRACE(L"WM_IME_COMPOSITION\n");
		return false;
	}

	return false;
}


// Enable/disable the entire IME system.  When disabled, the default IME handling
// kicks in.
void CIME::EnableImeSystem(bool bEnable)
{
	s_bEnableImeSystem = bEnable;
}


void CIME::SendKey(BYTE nVirtKey)
{
	keybd_event(nVirtKey, 0, 0,               0);
	keybd_event(nVirtKey, 0, KEYEVENTF_KEYUP, 0);
}


//  GetImeId(UINT uIndex)
//      returns 
//  returned value:
//  0: In the following cases
//      - Non Chinese IME input locale
//      - Older Chinese IME
//      - Other error cases
//
//  Othewise:
//      When uIndex is 0 (default)
//          bit 31-24:  Major version
//          bit 23-16:  Minor version
//          bit 15-0:   Language ID
//      When uIndex is 1
//          pVerFixedInfo->dwFileVersionLS
//
//  Use IMEID_VER and IMEID_LANG macro to extract version and language information.
//  

// We define the locale-invariant ID ourselves since it doesn't exist prior to WinXP
// For more information, see the CompareString() reference.
#define LCID_INVARIANT MAKELCID(MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US), SORT_DEFAULT)

DWORD CIME::GetImeId(UINT uIndex)
{
	static HKL hklPrev = 0;
	static DWORD dwID[2] = { 0, 0 };  // Cache the result

	DWORD   dwVerSize;
	DWORD   dwVerHandle;
	LPVOID  lpVerBuffer;
	LPVOID  lpVerData;
	UINT    cbVerData;
	char    szTmp[1024];

	if(uIndex >= sizeof(dwID) / sizeof(dwID[0]))
		return 0;

	if(hklPrev == s_hklCurrent)
		return dwID[uIndex];

	hklPrev = s_hklCurrent;  // Save for the next invocation

	// Check if we are using an older Chinese IME
	if(!((s_hklCurrent == _CHT_HKL) || (s_hklCurrent == _CHT_HKL2) || (s_hklCurrent == _CHS_HKL)))
	{
		dwID[0] = dwID[1] = 0;
		return dwID[uIndex];
	}

	// Obtain the IME file name
	if (!_ImmGetIMEFileNameA(s_hklCurrent, szTmp, (sizeof(szTmp) / sizeof(szTmp[0])) - 1))
	{
		dwID[0] = dwID[1] = 0;
		return dwID[uIndex];
	}

	// Check for IME that doesn't implement reading string API
	if (!_GetReadingString)
	{
		if((CompareStringA(LCID_INVARIANT, NORM_IGNORECASE, szTmp, -1, CHT_IMEFILENAME1, -1) != CSTR_EQUAL) &&
			(CompareStringA(LCID_INVARIANT, NORM_IGNORECASE, szTmp, -1, CHT_IMEFILENAME2, -1) != CSTR_EQUAL) &&
			(CompareStringA(LCID_INVARIANT, NORM_IGNORECASE, szTmp, -1, CHT_IMEFILENAME3, -1) != CSTR_EQUAL) &&
			(CompareStringA(LCID_INVARIANT, NORM_IGNORECASE, szTmp, -1, CHS_IMEFILENAME1, -1) != CSTR_EQUAL) &&
			(CompareStringA(LCID_INVARIANT, NORM_IGNORECASE, szTmp, -1, CHS_IMEFILENAME2, -1) != CSTR_EQUAL))
		{
			dwID[0] = dwID[1] = 0;
			return dwID[uIndex];
		}
	}

	dwVerSize = _GetFileVersionInfoSizeA(szTmp, &dwVerHandle);
	if(dwVerSize)
	{
		lpVerBuffer = HeapAlloc(GetProcessHeap(), 0, dwVerSize);
		if(lpVerBuffer)
		{
			if(_GetFileVersionInfoA(szTmp, dwVerHandle, dwVerSize, lpVerBuffer))
			{
				if(_VerQueryValueA(lpVerBuffer, "\\", &lpVerData, &cbVerData))
				{
					DWORD dwVer = ((VS_FIXEDFILEINFO*)lpVerData)->dwFileVersionMS;
					dwVer = (dwVer & 0x00ff0000) << 8 | (dwVer & 0x000000ff) << 16;
					if(_GetReadingString
						||
						(GetLanguage() == LANG_CHT &&
						(dwVer == MAKEIMEVERSION(4, 2) || 
						dwVer == MAKEIMEVERSION(4, 3) || 
						dwVer == MAKEIMEVERSION(4, 4) || 
						dwVer == MAKEIMEVERSION(5, 0) ||
						dwVer == MAKEIMEVERSION(5, 1) ||
						dwVer == MAKEIMEVERSION(5, 2) ||
						dwVer == MAKEIMEVERSION(6, 0)))
						||
						(GetLanguage() == LANG_CHS &&
						(dwVer == MAKEIMEVERSION(4, 1) ||
						dwVer == MAKEIMEVERSION(4, 2) ||
						dwVer == MAKEIMEVERSION(5, 3)))
						)
					{
						dwID[0] = dwVer | GetLanguage();
						dwID[1] = ((VS_FIXEDFILEINFO*)lpVerData)->dwFileVersionLS;
					}
				}
			}
			HeapFree(GetProcessHeap(), 0, lpVerBuffer);
		}
	}

	return dwID[uIndex];
}



void CIME::CheckInputLocale()
{
	static HKL hklPrev = 0;
	s_hklCurrent = GetKeyboardLayout(0);
	if (hklPrev == s_hklCurrent)
		return;

	hklPrev = s_hklCurrent;
	switch (GetPrimaryLanguage())
	{
		// Simplified Chinese
	case LANG_CHINESE:
		s_bVerticalCand = true;
		switch (GetSubLanguage())
		{
		case SUBLANG_CHINESE_SIMPLIFIED:
			s_wszCurrIndicator = s_aszIndicator[INDICATOR_CHS];
			s_bVerticalCand = GetImeId() == 0;
			break;
		case SUBLANG_CHINESE_TRADITIONAL:
			s_wszCurrIndicator = s_aszIndicator[INDICATOR_CHT];
			break;
		default:    // unsupported sub-language
			s_wszCurrIndicator = s_aszIndicator[INDICATOR_NON_IME];
			break;
		}
		break;
		// Korean
	case LANG_KOREAN:
		s_wszCurrIndicator = s_aszIndicator[INDICATOR_KOREAN];
		s_bVerticalCand = false;
		break;
		// Japanese
	case LANG_JAPANESE:
		s_wszCurrIndicator = s_aszIndicator[INDICATOR_JAPANESE];
		s_bVerticalCand = true;
		break;
	default:
		// A non-IME language.  Obtain the language abbreviation
		// and store it for rendering the indicator later.
		s_wszCurrIndicator = s_aszIndicator[INDICATOR_NON_IME];
	}

	// If non-IME, use the language abbreviation.
	if(s_wszCurrIndicator == s_aszIndicator[INDICATOR_NON_IME])
	{
		WCHAR wszLang[5];
		GetLocaleInfoW(MAKELCID(LOWORD(s_hklCurrent), SORT_DEFAULT), LOCALE_SABBREVLANGNAME, wszLang, 5);
		s_wszCurrIndicator[0] = wszLang[0];
		s_wszCurrIndicator[1] = towlower(wszLang[1]);
	}
}



void CIME::CheckToggleState()
{
	CheckInputLocale();
	bool bIme = _ImmIsIME(s_hklCurrent) != 0;
	s_bChineseIME = (GetPrimaryLanguage() == LANG_CHINESE) && bIme;

	HIMC hImc;
	if(NULL != (hImc = ImmGetContext(UIGetHWND())))
	{
		if(s_bChineseIME)
		{
			DWORD dwConvMode, dwSentMode;
			_ImmGetConversionStatus(hImc, &dwConvMode, &dwSentMode);
			s_ImeState = (dwConvMode & IME_CMODE_NATIVE) ? IMEUI_STATE_ON : IMEUI_STATE_ENGLISH;
		}
		else
		{
			s_ImeState = (bIme && _ImmGetOpenStatus(hImc) != 0) ? IMEUI_STATE_ON : IMEUI_STATE_OFF;
		}
		_ImmReleaseContext(UIGetHWND(), hImc);
	}
	else
		s_ImeState = IMEUI_STATE_OFF;
}



// Sets up IME-specific APIs for the IME edit controls.  This is called every time
// the input locale changes.
void CIME::SetupImeApi()
{
	char szImeFile[MAX_PATH + 1];

	_GetReadingString = NULL;
	_ShowReadingWindow = NULL;
	if(_ImmGetIMEFileNameA(s_hklCurrent, szImeFile, sizeof(szImeFile)/sizeof(szImeFile[0]) - 1) == 0)
		return;

	if(s_hDllIme) FreeLibrary(s_hDllIme);
	s_hDllIme = LoadLibraryA(szImeFile);
	if (!s_hDllIme)
		return;
	_GetReadingString = (UINT (WINAPI*)(HIMC, UINT, LPWSTR, PINT, BOOL*, PUINT))
		(GetProcAddress(s_hDllIme, "GetReadingString"));
	_ShowReadingWindow =(BOOL (WINAPI*)(HIMC, BOOL))
		(GetProcAddress(s_hDllIme, "ShowReadingWindow"));
}



// Resets the composition string.
void CIME::ResetCompositionString()
{
	s_nCompCaret = 0;
	s_CompString.SetText(L"");
	ZeroMemory(s_abCompStringAttr, sizeof(s_abCompStringAttr));
}


// Determine whether the reading window should be vertical or horizontal.
void CIME::GetReadingWindowOrientation(DWORD dwId)
{
	s_bHorizontalReading = (s_hklCurrent == _CHS_HKL) || (s_hklCurrent == _CHT_HKL2) || (dwId == 0);
	if(!s_bHorizontalReading && (dwId & 0x0000FFFF) == LANG_CHT)
	{
		std::wstring wstrRegPath;
		HKEY hKey;
		DWORD dwVer = dwId & 0xFFFF0000;
		wstrRegPath=L"software\\microsoft\\windows\\currentversion\\";
		wstrRegPath+=(dwVer >= MAKEIMEVERSION(5, 1)) ? L"MSTCIPH" : L"TINTLGNT";

		LONG lRc = RegOpenKeyExW(HKEY_CURRENT_USER, wstrRegPath.c_str(), 0, KEY_READ, &hKey);
		if (lRc == ERROR_SUCCESS)
		{
			DWORD dwSize = sizeof(DWORD), dwMapping, dwType;
			lRc = RegQueryValueExW(hKey, L"Keyboard Mapping", NULL, &dwType, (PBYTE)&dwMapping, &dwSize);
			if (lRc == ERROR_SUCCESS)
			{
				if ((dwVer <= MAKEIMEVERSION(5, 0) && 
					((BYTE)dwMapping == 0x22 || (BYTE)dwMapping == 0x23))
					||
					((dwVer == MAKEIMEVERSION(5, 1) || dwVer == MAKEIMEVERSION(5, 2)) &&
					(BYTE)dwMapping >= 0x22 && (BYTE)dwMapping <= 0x24)
					)
				{
					s_bHorizontalReading = true;
				}
			}
			RegCloseKey(hKey);
		}
	}
}


// Obtain the reading string upon WM_IME_NOTIFY/INM_PRIVATE notification.
void CIME::GetPrivateReadingString()
{
	DWORD dwId = GetImeId();
	if(!dwId)
	{
		s_bShowReadingWindow = false;
		return;
	}

	HIMC hImc;
	hImc = ImmGetContext(UIGetHWND());
	if(!hImc)
	{
		s_bShowReadingWindow = false;
		return;
	}

	DWORD dwReadingStrLen = 0;
	DWORD dwErr = 0;
	WCHAR *pwszReadingStringBuffer = NULL;  // Buffer for when the IME supports GetReadingString()
	WCHAR *wstr = 0;
	bool bUnicodeIme = false;  // Whether the IME context component is Unicode.
	INPUTCONTEXT *lpIC = NULL;

	if(_GetReadingString)
	{
		UINT uMaxUiLen;
		BOOL bVertical;
		// Obtain the reading string size
		dwReadingStrLen = _GetReadingString(hImc, 0, NULL, (PINT)&dwErr, &bVertical, &uMaxUiLen);
		if(dwReadingStrLen)
		{
			wstr = pwszReadingStringBuffer = (LPWSTR)HeapAlloc(GetProcessHeap(), 0, sizeof(WCHAR) * dwReadingStrLen);
			if(!pwszReadingStringBuffer)
			{
				// Out of memory. Exit.
				_ImmReleaseContext(UIGetHWND(), hImc);
				return;
			}

			// Obtain the reading string
			dwReadingStrLen = _GetReadingString(hImc, dwReadingStrLen, wstr, (PINT)&dwErr, &bVertical, &uMaxUiLen);
		}

		s_bHorizontalReading = !bVertical;
		bUnicodeIme = true;
	}
	else
	{
		// IMEs that doesn't implement Reading String API

		lpIC = _ImmLockIMC(hImc);

		LPBYTE p = 0;
		switch(dwId)
		{
		case IMEID_CHT_VER42: // New(Phonetic/ChanJie)IME98  : 4.2.x.x // Win98
		case IMEID_CHT_VER43: // New(Phonetic/ChanJie)IME98a : 4.3.x.x // WinMe, Win2k
		case IMEID_CHT_VER44: // New ChanJie IME98b          : 4.4.x.x // WinXP
			p = *(LPBYTE *)((LPBYTE)_ImmLockIMCC(lpIC->hPrivate) + 24);
			if(!p) break;
			dwReadingStrLen = *(DWORD *)(p + 7 * 4 + 32 * 4);
			dwErr = *(DWORD *)(p + 8 * 4 + 32 * 4);
			wstr = (WCHAR *)(p + 56);
			bUnicodeIme = true;
			break;

		case IMEID_CHT_VER50: // 5.0.x.x // WinME
			p = *(LPBYTE *)((LPBYTE)_ImmLockIMCC(lpIC->hPrivate) + 3 * 4);
			if(!p) break;
			p = *(LPBYTE *)((LPBYTE)p + 1*4 + 5*4 + 4*2);
			if(!p) break;
			dwReadingStrLen = *(DWORD *)(p + 1*4 + (16*2+2*4) + 5*4 + 16);
			dwErr = *(DWORD *)(p + 1*4 + (16*2+2*4) + 5*4 + 16 + 1*4);
			wstr = (WCHAR *)(p + 1*4 + (16*2+2*4) + 5*4);
			bUnicodeIme = false;
			break;

		case IMEID_CHT_VER51: // 5.1.x.x // IME2002(w/OfficeXP)
		case IMEID_CHT_VER52: // 5.2.x.x // (w/whistler)
		case IMEID_CHS_VER53: // 5.3.x.x // SCIME2k or MSPY3 (w/OfficeXP and Whistler)
			p = *(LPBYTE *)((LPBYTE)_ImmLockIMCC(lpIC->hPrivate) + 4);
			if(!p) break;
			p = *(LPBYTE *)((LPBYTE)p + 1*4 + 5*4);
			if(!p) break;
			dwReadingStrLen = *(DWORD *)(p + 1*4 + (16*2+2*4) + 5*4 + 16 * 2);
			dwErr = *(DWORD *)(p + 1*4 + (16*2+2*4) + 5*4 + 16 * 2 + 1*4);
			wstr  = (WCHAR *) (p + 1*4 + (16*2+2*4) + 5*4);
			bUnicodeIme = true;
			break;

			// the code tested only with Win 98 SE (MSPY 1.5/ ver 4.1.0.21)
		case IMEID_CHS_VER41:
			{
				int nOffset;
				nOffset = (GetImeId(1) >= 0x00000002) ? 8 : 7;

				p = *(LPBYTE *)((LPBYTE)_ImmLockIMCC(lpIC->hPrivate) + nOffset * 4);
				if(!p) break;
				dwReadingStrLen = *(DWORD *)(p + 7*4 + 16*2*4);
				dwErr = *(DWORD *)(p + 8*4 + 16*2*4);
				dwErr = __min(dwErr, dwReadingStrLen);
				wstr = (WCHAR *)(p + 6*4 + 16*2*1);
				bUnicodeIme = true;
				break;
			}

		case IMEID_CHS_VER42: // 4.2.x.x // SCIME98 or MSPY2 (w/Office2k, Win2k, WinME, etc)
			{
				OSVERSIONINFOW osi;
				osi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOW);
				GetVersionExW(&osi);

				int nTcharSize = (osi.dwPlatformId == VER_PLATFORM_WIN32_NT) ? sizeof(WCHAR) : sizeof(char);
				p = *(LPBYTE *)((LPBYTE)_ImmLockIMCC(lpIC->hPrivate) + 1*4 + 1*4 + 6*4);
				if(!p) break;
				dwReadingStrLen = *(DWORD *)(p + 1*4 + (16*2+2*4) + 5*4 + 16 * nTcharSize);
				dwErr = *(DWORD *)(p + 1*4 + (16*2+2*4) + 5*4 + 16 * nTcharSize + 1*4);
				wstr  = (WCHAR *) (p + 1*4 + (16*2+2*4) + 5*4);
				bUnicodeIme = (osi.dwPlatformId == VER_PLATFORM_WIN32_NT) ? true : false;
			}
		}   // switch
	}

	// Copy the reading string to the candidate list first
	s_CandList.awszCandidate[0][0] = 0;
	s_CandList.awszCandidate[1][0] = 0;
	s_CandList.awszCandidate[2][0] = 0;
	s_CandList.awszCandidate[3][0] = 0;
	s_CandList.dwCount = dwReadingStrLen;
	s_CandList.dwSelection = (DWORD)-1; // do not select any char
	if(bUnicodeIme)
	{
		UINT i;
		for(i = 0; i < dwReadingStrLen; ++i) // dwlen > 0, if known IME
		{
			if(dwErr <= i && s_CandList.dwSelection == (DWORD)-1)
			{
				// select error char
				s_CandList.dwSelection = i;
			}

			s_CandList.awszCandidate[i][0] = wstr[i];
			s_CandList.awszCandidate[i][1] = 0;
		}
		s_CandList.awszCandidate[i][0] = 0;
	}
	else
	{
		char *p = (char *)wstr;
		DWORD i, j;
		for(i = 0, j = 0; i < dwReadingStrLen; ++i, ++j) // dwlen > 0, if known IME
		{
			if(dwErr <= i && s_CandList.dwSelection == (DWORD)-1)
			{
				s_CandList.dwSelection = j;
			}
			// Obtain the current code page
			WCHAR wszCodePage[8];
			UINT uCodePage = CP_ACP;  // Default code page
			if(GetLocaleInfoW(MAKELCID(GetLanguage(), SORT_DEFAULT),
				LOCALE_IDEFAULTANSICODEPAGE,
				wszCodePage,
				sizeof(wszCodePage)/sizeof(wszCodePage[0])))
			{
				uCodePage = wcstoul(wszCodePage, NULL, 0);
			}
			MultiByteToWideChar(uCodePage, 0, p + i, IsDBCSLeadByteEx(uCodePage, p[i]) ? 2 : 1,
				s_CandList.awszCandidate[j], 1);
			if(IsDBCSLeadByteEx(uCodePage, p[i]))
				++i;
		}
		s_CandList.awszCandidate[j][0] = 0;
		s_CandList.dwCount = j;
	}
	if(!_GetReadingString)
	{
		_ImmUnlockIMCC(lpIC->hPrivate);
		_ImmUnlockIMC(hImc);
		GetReadingWindowOrientation(dwId);
	}
	_ImmReleaseContext(UIGetHWND(), hImc);

	if(pwszReadingStringBuffer)
		HeapFree(GetProcessHeap(), 0, pwszReadingStringBuffer);

	// Copy the string to the reading string buffer
	if(s_CandList.dwCount > 0)
		s_bShowReadingWindow = true;
	else
		s_bShowReadingWindow = false;
	if(s_bHorizontalReading)
	{
		s_CandList.nReadingError = -1;
		s_wstrReadingString.clear();
		for(UINT i = 0; i < s_CandList.dwCount; ++i)
		{
			if(s_CandList.dwSelection == i)
				s_CandList.nReadingError = s_wstrReadingString.length();
			s_wstrReadingString+=s_CandList.awszCandidate[i];
		}
	}

	s_CandList.dwPageSize = MAX_CANDLIST;
}