#pragma once

#include "UIEditBox.h"
//-----------------------------------------------------------------------------
// IME-enabled EditBox control
//-----------------------------------------------------------------------------
#define MAX_CANDLIST 10
#define MAX_COMPSTRING_SIZE 256

class CIME
{
//protected:
	public:
	// Empty implementation of the IMM32 API
	static INPUTCONTEXT* WINAPI Dummy_ImmLockIMC(HIMC) { return NULL; }
	static BOOL WINAPI Dummy_ImmUnlockIMC(HIMC) { return FALSE; }
	static LPVOID WINAPI Dummy_ImmLockIMCC(HIMCC) { return NULL; }
	static BOOL WINAPI Dummy_ImmUnlockIMCC(HIMCC) { return FALSE; }
	static BOOL WINAPI Dummy_ImmDisableTextFrameService(DWORD) { return TRUE; }
	static LONG WINAPI Dummy_ImmGetCompositionStringW(HIMC, DWORD, LPVOID, DWORD) { return IMM_ERROR_GENERAL; }
	static DWORD WINAPI Dummy_ImmGetCandidateListW(HIMC, DWORD, LPCANDIDATELIST, DWORD) { return 0; }
	static HIMC WINAPI Dummy_ImmGetContext(HWND) { return NULL; }
	static BOOL WINAPI Dummy_ImmReleaseContext(HWND, HIMC) { return FALSE; }
	static HIMC WINAPI Dummy_ImmAssociateContext(HWND, HIMC) { return NULL; }
	static BOOL WINAPI Dummy_ImmGetOpenStatus(HIMC) { return 0; }
	static BOOL WINAPI Dummy_ImmSetOpenStatus(HIMC, BOOL) { return 0; }
	static BOOL WINAPI Dummy_ImmGetConversionStatus(HIMC, LPDWORD, LPDWORD) { return 0; }
	static HWND WINAPI Dummy_ImmGetDefaultIMEWnd(HWND) { return NULL; }
	static UINT WINAPI Dummy_ImmGetIMEFileNameA(HKL, LPSTR, UINT) { return 0; }
	static UINT WINAPI Dummy_ImmGetVirtualKey(HWND) { return 0; }
	static BOOL WINAPI Dummy_ImmNotifyIME(HIMC, DWORD, DWORD, DWORD) { return FALSE; }
	static BOOL WINAPI Dummy_ImmSetConversionStatus(HIMC, DWORD, DWORD) { return FALSE; }
	static BOOL WINAPI Dummy_ImmSimulateHotKey(HWND, DWORD) { return FALSE; }
	static BOOL WINAPI Dummy_ImmIsIME(HKL) { return FALSE; }

	// Traditional Chinese IME
	static UINT WINAPI Dummy_GetReadingString(HIMC, UINT, LPWSTR, PINT, BOOL*, PUINT) { return 0; }
	static BOOL WINAPI Dummy_ShowReadingWindow(HIMC, BOOL) { return FALSE; }

	// Verion library imports
	static BOOL APIENTRY Dummy_VerQueryValueA(const LPVOID, LPSTR, LPVOID *, PUINT) { return 0; }
	static BOOL APIENTRY Dummy_GetFileVersionInfoA(LPSTR, DWORD, DWORD, LPVOID) { return 0; }
	static DWORD APIENTRY Dummy_GetFileVersionInfoSizeA(LPSTR, LPDWORD) { return 0; }

	// Function pointers: IMM32
	static INPUTCONTEXT* (WINAPI * _ImmLockIMC)(HIMC);
	static BOOL (WINAPI * _ImmUnlockIMC)(HIMC);
	static LPVOID (WINAPI * _ImmLockIMCC)(HIMCC);
	static BOOL (WINAPI * _ImmUnlockIMCC)(HIMCC);
	static BOOL (WINAPI * _ImmDisableTextFrameService)(DWORD);
	static LONG (WINAPI * _ImmGetCompositionStringW)(HIMC, DWORD, LPVOID, DWORD);
	static DWORD (WINAPI * _ImmGetCandidateListW)(HIMC, DWORD, LPCANDIDATELIST, DWORD);
	static HIMC (WINAPI * _ImmGetContext)(HWND);
	static BOOL (WINAPI * _ImmReleaseContext)(HWND, HIMC);
	static HIMC (WINAPI * _ImmAssociateContext)(HWND, HIMC);
	static BOOL (WINAPI * _ImmGetOpenStatus)(HIMC);
	static BOOL (WINAPI * _ImmSetOpenStatus)(HIMC, BOOL);
	static BOOL (WINAPI * _ImmGetConversionStatus)(HIMC, LPDWORD, LPDWORD);
	static HWND (WINAPI * _ImmGetDefaultIMEWnd)(HWND);
	static UINT (WINAPI * _ImmGetIMEFileNameA)(HKL, LPSTR, UINT);
	static UINT (WINAPI * _ImmGetVirtualKey)(HWND);
	static BOOL (WINAPI * _ImmNotifyIME)(HIMC, DWORD, DWORD, DWORD);
	static BOOL (WINAPI * _ImmSetConversionStatus)(HIMC, DWORD, DWORD);
	static BOOL (WINAPI * _ImmSimulateHotKey)(HWND, DWORD);
	static BOOL (WINAPI * _ImmIsIME)(HKL);

	// Function pointers: Traditional Chinese IME
	static UINT (WINAPI * _GetReadingString)(HIMC, UINT, LPWSTR, PINT, BOOL*, PUINT);
	static BOOL (WINAPI * _ShowReadingWindow)(HIMC, BOOL);

	// Function pointers: Verion library imports
	static BOOL (APIENTRY * _VerQueryValueA)(const LPVOID, LPSTR, LPVOID *, PUINT);
	static BOOL (APIENTRY * _GetFileVersionInfoA)(LPSTR, DWORD, DWORD, LPVOID);
	static DWORD (APIENTRY * _GetFileVersionInfoSizeA)(LPSTR, LPDWORD);
//protected:
public:
	static WORD GetLanguage() { return LOWORD(s_hklCurrent); }
	static WORD GetPrimaryLanguage() { return PRIMARYLANGID(LOWORD(s_hklCurrent)); }
	static WORD GetSubLanguage() { return SUBLANGID(LOWORD(s_hklCurrent)); }
	static void SendKey(BYTE nVirtKey);
	static DWORD GetImeId(UINT uIndex = 0);
	static void CheckInputLocale();
	static void CheckToggleState();
	static void SetupImeApi();
	static void ResetCompositionString();

	static void GetReadingWindowOrientation(DWORD dwId);
	static void GetPrivateReadingString();

public:
	static void Initialize();
	static void Uninitialize();

	static  HRESULT StaticOnCreateDevice();
	static  bool StaticMsgProc(UINT uMsg, WPARAM wParam, LPARAM lParam);

	static  void EnableImeSystem(bool bEnable);

//protected:
public:
	enum { INDICATOR_NON_IME, INDICATOR_CHS, INDICATOR_CHT, INDICATOR_KOREAN, INDICATOR_JAPANESE };
	enum IMESTATE { IMEUI_STATE_OFF, IMEUI_STATE_ON, IMEUI_STATE_ENGLISH };

	struct CCandList
	{
		WCHAR awszCandidate[MAX_CANDLIST][256];
		CUniBuffer HoriCand; // Candidate list string (for horizontal candidate window)
		int   nFirstSelected; // First character position of the selected string in HoriCand
		int   nHoriSelectedLen; // Length of the selected string in HoriCand
		DWORD dwCount;       // Number of valid entries in the candidate list
		DWORD dwSelection;   // Currently selected candidate entry relative to page top
		DWORD dwPageSize;
		int   nReadingError; // Index of the error character
		bool  bShowWindow;   // Whether the candidate list window is visible
		CRect<int>  rcCandidate;   // Candidate rectangle computed and filled each time before rendered
	};

	struct CInputLocale
	{
		HKL   m_hKL;            // Keyboard layout
		std::wstring  m_wstrLangAbb;  // Language abbreviation
		std::wstring m_wstrLang;    // Localized language name
	};

	// Application-wide data
	static HINSTANCE s_hDllImm32;         // IMM32 DLL handle
	static HINSTANCE s_hDllVer;           // Version DLL handle
	static HIMC      s_hImcDef;           // Default input context

	static HKL     s_hklCurrent;          // Current keyboard layout of the process
	static bool    s_bVerticalCand;       // Indicates that the candidates are listed vertically
	static LPWSTR  s_wszCurrIndicator;    // Points to an indicator string that corresponds to current input locale
	static WCHAR   s_aszIndicator[5][3];  // String to draw to indicate current input locale
	static bool    s_bInsertOnType;       // Insert the character as soon as a key is pressed (Korean behavior)
	static HINSTANCE s_hDllIme;           // Instance handle of the current IME module
	static IMESTATE  s_ImeState;          // IME global state
	static bool    s_bEnableImeSystem;    // Whether the IME system is active
	static POINT   s_ptCompString;        // Composition string position. Updated every frame.
	static int     s_nCompCaret;          // Caret position of the composition string
	static int     s_nFirstTargetConv;    // Index of the first target converted char in comp string.  If none, -1.
	static CUniBuffer s_CompString;       // Buffer to hold the composition string (we fix its length)
	static BYTE    s_abCompStringAttr[MAX_COMPSTRING_SIZE];
	static DWORD   s_adwCompStringClause[MAX_COMPSTRING_SIZE];
	//static WCHAR   s_wszReadingString[32];// Used only with horizontal reading window (why?)
	static std::wstring   s_wstrReadingString;// Used only with horizontal reading window (why?)
	static CCandList s_CandList;          // Data relevant to the candidate list
	static bool    s_bShowReadingWindow;  // Indicates whether reading window is visible
	static bool    s_bHorizontalReading;  // Indicates whether the reading window is vertical or horizontal
	static bool    s_bChineseIME;
	static std::vector<CInputLocale> s_Locale; // Array of loaded keyboard layout on system

#if defined(DEBUG) || defined(_DEBUG)
	static bool    m_bIMEStaticMsgProcCalled;
#endif
};