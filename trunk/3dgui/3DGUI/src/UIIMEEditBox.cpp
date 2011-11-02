#include "UIIMEEditBox.h"
#include "UIDialog.h"
#include "IME.h"

#undef min // use __min instead
#undef max // use __max instead


// CUIIMEEditBox class


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

CUIIMEEditBox::CUIIMEEditBox()
{
    CIME::Initialize(); // ensure static vars are properly init'ed first
    CIME::_ImmDisableTextFrameService((DWORD)-1);  // Disable TSF for the current process
    m_Type = UI_CONTROL_IMEEDITBOX;
    CIME::s_bEnableImeSystem = true;
    m_nIndicatorWidth = 0;
    m_ReadingColor = Color32(188, 255, 255, 255);
    m_ReadingWinColor = Color32(128, 0, 0, 0);
    m_ReadingSelColor = Color32(255, 255, 0, 0);
    m_ReadingSelBkColor = Color32(128, 80, 80, 80);
    m_CandidateColor = Color32(255, 200, 200, 200);
    m_CandidateWinColor = Color32(128, 0, 0, 0);
    m_CandidateSelColor = Color32(255, 255, 255, 255);
    m_CandidateSelBkColor = Color32(128, 158, 158, 158);
    m_CompColor = Color32(255, 200, 200, 255);
    m_CompWinColor = Color32(198, 0, 0, 0);
    m_CompCaretColor = Color32(255, 255, 255, 255);
    m_CompTargetColor = Color32(255, 255, 255, 255);
    m_CompTargetBkColor = Color32(255, 150, 150, 150);
    m_CompTargetNonColor = Color32(255, 255, 255, 0);
    m_CompTargetNonBkColor = Color32(255, 150, 150, 150);
    m_IndicatorImeColor = Color32(255, 255, 255, 255);
    m_IndicatorEngColor = Color32(255, 0, 0, 0);
    m_IndicatorBkColor = Color32(255, 128, 128, 128);
}



CUIIMEEditBox::~CUIIMEEditBox()
{
}


void CUIIMEEditBox::UpdateRects()
{
    // Temporary adjust m_width so that CUIEditBox can compute
    // the correct rects for its rendering since we need to make space
    // for the indicator button
 //   int nWidth = m_width;
 //   m_width -= m_nIndicatorWidth; // Make room for the indicator button
	CUIEditBox::UpdateRects();
 //   m_width = nWidth;  // Restore

    // Compute the indicator button rectangle
 //   SetRect(&m_rcIndicator, m_rcBoundingBox.right, m_rcBoundingBox.top, m_x + m_width, m_rcBoundingBox.bottom);
//    InflateRect(&m_rcIndicator, -m_nBorder, -m_nBorder);
 //   m_rcBoundingBox.right = m_rcBoundingBox.left + m_width;
}


// This function is used only briefly in CHT IME handling,
// so accelerator isn't processed.
void CUIIMEEditBox::PumpMessage()
{
    MSG msg;

    while(PeekMessageW(&msg, NULL, 0, 0, PM_NOREMOVE))
    {
        if(!GetMessageW(&msg, NULL, 0, 0))
        {
            PostQuitMessage((int)msg.wParam);
            return;
        }
        TranslateMessage(&msg);
        DispatchMessageA(&msg);
    }
}


void CUIIMEEditBox::OnFocusIn()
{
    CUIEditBox::OnFocusIn();

    if(CIME::s_bEnableImeSystem)
    {
        CIME::_ImmAssociateContext(UIGetHWND(), CIME::s_hImcDef);
        CIME::CheckToggleState();
    } else
        CIME::_ImmAssociateContext(UIGetHWND(), NULL);

    //
    // Set up the IME global state according to the current instance state
    //
    HIMC hImc;
    if(NULL != (hImc = ImmGetContext(UIGetHWND()))) 
    {
        if(!CIME::s_bEnableImeSystem)
            CIME::s_ImeState = CIME::IMEUI_STATE_OFF;

        CIME::_ImmReleaseContext(UIGetHWND(), hImc);
        CIME::CheckToggleState();
    }
}



void CUIIMEEditBox::OnFocusOut()
{
    CUIEditBox::OnFocusOut();

    FinalizeString(false);  // Don't send the comp string as to match RichEdit behavior

    CIME::_ImmAssociateContext(UIGetHWND(), NULL);
}

void CUIIMEEditBox::OnLButtonDown(POINT point)
{
	//???????UIFontNode* pFont = UIGraph::getInstance().GetFont(GetStyle(9)->iFont);

	// Check if this click is on top of the composition string
	int nCompStrWidth;
	CIME::s_CompString.CPtoX(CIME::s_CompString.GetTextSize(), FALSE, nCompStrWidth);

	if(CIME::s_ptCompString.x <= point.x &&
		CIME::s_ptCompString.y <= point.y &&
		CIME::s_ptCompString.x + nCompStrWidth > point.x &&
		CIME::s_ptCompString.y + 0/*pFont->nHeight*/ > point.y)
	{
		int nCharBodyHit, nCharHit;
		bool bTrail;

		// Determine the character clicked on.
		CIME::s_CompString.XtoCP(point.x - CIME::s_ptCompString.x, nCharBodyHit, bTrail);
		if(bTrail && nCharBodyHit < CIME::s_CompString.GetTextSize())
			nCharHit = nCharBodyHit + 1;
		else
			nCharHit = nCharBodyHit;

		// Now generate keypress events to move the comp string cursor
		// to the click point.  First, if the candidate window is displayed,
		// send Esc to close it.
		HIMC hImc = ImmGetContext(UIGetHWND());
		if(!hImc)
			return;

		CIME::_ImmNotifyIME(hImc, NI_CLOSECANDIDATE, 0, 0);
		CIME::_ImmReleaseContext(UIGetHWND(), hImc);

		switch(CIME::GetPrimaryLanguage())
		{
		case LANG_JAPANESE:
			// For Japanese, there are two cases.  If CIME::s_nFirstTargetConv is
			// -1, the comp string hasn't been converted yet, and we use
			// CIME::s_nCompCaret.  For any other value of CIME::s_nFirstTargetConv,
			// the string has been converted, so we use clause information.

			if(CIME::s_nFirstTargetConv != -1)
			{
				int nClauseClicked = 0;
				while((int)CIME::s_adwCompStringClause[nClauseClicked + 1] <= nCharBodyHit)
					++nClauseClicked;

				int nClauseSelected = 0;
				while((int)CIME::s_adwCompStringClause[nClauseSelected + 1] <= CIME::s_nFirstTargetConv)
					++nClauseSelected;

				BYTE nVirtKey = nClauseClicked > nClauseSelected ? VK_RIGHT : VK_LEFT;
				int nSendCount = abs(nClauseClicked - nClauseSelected);
				while(nSendCount-- > 0)
					CIME::SendKey(nVirtKey);

				return;
			}

			// Not converted case. Fall thru to Chinese case.

		case LANG_CHINESE:
			{
				// For Chinese, use CIME::s_nCompCaret.
				BYTE nVirtKey = nCharHit > CIME::s_nCompCaret ? VK_RIGHT : VK_LEFT;
				int nSendCount = abs(nCharHit - CIME::s_nCompCaret);
				while(nSendCount-- > 0)
					CIME::SendKey(nVirtKey);
				break;
			}
		}
		return;
	}

	// Check if the click is on top of the candidate window
	if(CIME::s_CandList.bShowWindow && CIME::s_CandList.rcCandidate.ptInRect(point))
	{
		if(CIME::s_bVerticalCand)
		{
			// Vertical candidate window

			// Compute the row the click is on
			int nRow = (point.y - CIME::s_CandList.rcCandidate.top) /*/ pFont->nHeight*/;

			if(nRow < (int)CIME::s_CandList.dwCount)
			{
				// nRow is a valid entry.
				// Now emulate keystrokes to select the candidate at this row.
				switch(CIME::GetPrimaryLanguage())
				{
				case LANG_CHINESE:
				case LANG_KOREAN:
					// For Chinese and Korean, simply send the number keystroke.
					CIME::SendKey((BYTE) ('0' + nRow + 1));
					break;

				case LANG_JAPANESE:
					// For Japanese, move the selection to the target row,
					// then send Right, then send Left.

					BYTE nVirtKey;
					if(nRow > (int)CIME::s_CandList.dwSelection)
						nVirtKey = VK_DOWN;
					else
						nVirtKey = VK_UP;
					int nNumToHit = abs(int(nRow - CIME::s_CandList.dwSelection));
					for(int nStrike = 0; nStrike < nNumToHit; ++nStrike)
						CIME::SendKey(nVirtKey);

					// Do this to close the candidate window without ending composition.
					CIME::SendKey(VK_RIGHT);
					CIME::SendKey(VK_LEFT);

					break;
				}
			}
		}
		else
		{
			// Horizontal candidate window

			// Determine which the character the click has hit.
			int nCharHit;
			bool bTrail;
			CIME::s_CandList.HoriCand.XtoCP(point.x - CIME::s_CandList.rcCandidate.left, nCharHit, bTrail);

			// Determine which candidate string the character belongs to.
			int nCandidate = CIME::s_CandList.dwCount - 1;

			int nEntryStart = 0;
			for(UINT i = 0; i < CIME::s_CandList.dwCount; ++i)
			{
				if(nCharHit >= nEntryStart)
				{
					// Haven't found it.
					nEntryStart += lstrlenW(CIME::s_CandList.awszCandidate[i]) + 1;  // plus space separator
				} else
				{
					// Found it.  This entry starts at the right side of the click point,
					// so the char belongs to the previous entry.
					nCandidate = i - 1;
					break;
				}
			}

			// Now emulate keystrokes to select the candidate entry.
			switch(CIME::GetPrimaryLanguage())
			{
			case LANG_CHINESE:
			case LANG_KOREAN:
				// For Chinese and Korean, simply send the number keystroke.
				CIME::SendKey((BYTE) ('0' + nCandidate + 1));
				break;
			}
		}
		return;
	}

	CUIEditBox::OnLButtonDown(point);
}

#include "assert.h"
bool CUIIMEEditBox::MsgProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    if(!m_bEnabled || !m_bVisible)
        return false;

#if defined(DEBUG) || defined(_DEBUG)
    // DXUT.cpp used to call CUIIMEEditBox::StaticMsgProc() so that, but now
    // this is the application's responsiblity.  To do this, call 
    // CUIDialogResMgr::MsgProc() before calling this function.
	assert(CIME::m_bIMEStaticMsgProcCalled && L"To fix, call CUIDialogResMgr::MsgProc() first");
#endif

    bool trappedData;
    bool *trapped = &trappedData;

    HIMC hImc;
    static LPARAM lAlt = 0x80000000, lCtrl = 0x80000000, lShift = 0x80000000;

    *trapped = false;
    if(!CIME::s_bEnableImeSystem)
        return CUIEditBox::MsgProc(uMsg, wParam, lParam);

    switch(uMsg)
    {
        //
        //  IME Handling
        //
        case WM_IME_COMPOSITION:
//            UITRACE(L"WM_IME_COMPOSITION\n");
            {
                LONG lRet;  // Returned count in CHARACTERS
                WCHAR wszCompStr[MAX_COMPSTRING_SIZE];

                *trapped = true;
                if(NULL == (hImc = ImmGetContext(UIGetHWND())))
                {
                    break;
                }

                // Get the caret position in composition string
                if (lParam & GCS_CURSORPOS)
                {
                    CIME::s_nCompCaret = CIME::_ImmGetCompositionStringW(hImc, GCS_CURSORPOS, NULL, 0);
                    if(CIME::s_nCompCaret < 0)
                        CIME::s_nCompCaret = 0; // On error, set caret to pos 0.
                }

                // ResultStr must be processed before composition string.
                //
                // This is because for some IMEs, such as CHT, pressing Enter
                // to complete the composition sends WM_IME_COMPOSITION with both
                // GCS_RESULTSTR and GCS_COMPSTR.  Retrieving the result string
                // gives the correct string, while retrieving the comp string
                // (GCS_COMPSTR) gives empty string.  GCS_RESULTSTR should be
                // handled first so that the application receives the string.  Then
                // GCS_COMPSTR can be handled to clear the comp string buffer.

                if (lParam & GCS_RESULTSTR)
                {
                    UITRACE(L"  GCS_RESULTSTR\n");
                    lRet = CIME::_ImmGetCompositionStringW(hImc, GCS_RESULTSTR, wszCompStr, sizeof(wszCompStr));
                    if(lRet > 0)
                    {
                        lRet /= sizeof(WCHAR);
                        wszCompStr[lRet] = 0;  // Force terminate
                        TruncateCompString(false, (int)wcslen(wszCompStr));
                        CIME::s_CompString.SetText(wszCompStr);
                        SendCompString();
                        CIME::ResetCompositionString();
                    }
                }

                //
                // Reads in the composition string.
                //
                if (lParam & GCS_COMPSTR)
                {
                    UITRACE(L"  GCS_COMPSTR\n");
                    //////////////////////////////////////////////////////
                    // Retrieve the latest user-selected IME candidates
                    lRet = CIME::_ImmGetCompositionStringW(hImc, GCS_COMPSTR, wszCompStr, sizeof(wszCompStr));
                    if(lRet > 0)
                    {
                        lRet /= sizeof(WCHAR);  // Convert size in byte to size in char
                        wszCompStr[lRet] = 0;  // Force terminate
                        //
                        // Remove the whole of the string
                        //
                        TruncateCompString(false, (int)wcslen(wszCompStr));

                        CIME::s_CompString.SetText(wszCompStr);

                        // Older CHT IME uses composition string for reading string
                        if (CIME::GetLanguage() == LANG_CHT && !CIME::GetImeId())
                        {
                            if(lstrlenW(CIME::s_CompString.GetBuffer()))
                            {
                                CIME::s_CandList.dwCount = 4;             // Maximum possible length for reading string is 4
                                CIME::s_CandList.dwSelection = (DWORD)-1; // don't select any candidate

                                // Copy the reading string to the candidate list
                                for(int i = 3; i >= 0; --i)
                                {
                                    if(i > lstrlenW(CIME::s_CompString.GetBuffer()) - 1)
                                        CIME::s_CandList.awszCandidate[i][0] = 0;  // Doesn't exist
                                    else
                                    {
                                        CIME::s_CandList.awszCandidate[i][0] = CIME::s_CompString[i];
                                        CIME::s_CandList.awszCandidate[i][1] = 0;
                                    }
                                }
                                CIME::s_CandList.dwPageSize = MAX_CANDLIST;
                                // Clear comp string after we are done copying
                                ZeroMemory((LPVOID)CIME::s_CompString.GetBuffer(), 4 * sizeof(WCHAR));
                                CIME::s_bShowReadingWindow = true;
                                CIME::GetReadingWindowOrientation(0);
                                if(CIME::s_bHorizontalReading)
                                {
                                    CIME::s_CandList.nReadingError = -1;  // Clear error

                                    // Create a string that consists of the current
                                    // reading string.  Since horizontal reading window
                                    // is used, we take advantage of this by rendering
                                    // one string instead of several.
                                    //
                                    // Copy the reading string from the candidate list
                                    // to the reading string buffer.
                                   CIME::s_wstrReadingString.clear();
                                    for(UINT i = 0; i < CIME::s_CandList.dwCount; ++i)
                                    {
                                        if(CIME::s_CandList.dwSelection == i)
                                            CIME::s_CandList.nReadingError = CIME::s_wstrReadingString.length();
										CIME::s_wstrReadingString+=CIME::s_CandList.awszCandidate[i];
                                    }
                                }
                            }
                            else
                            {
                                CIME::s_CandList.dwCount = 0;
                                CIME::s_bShowReadingWindow = false;
                            }
                        }

                        if(CIME::s_bInsertOnType)
                        {
                            // Send composition string to the edit control
                            SendCompString();
                            // Restore the caret to the correct location.
                            // It's at the end right now, so compute the number
                            // of times left arrow should be pressed to
                            // send it to the original position.
                            int nCount = lstrlenW(CIME::s_CompString.GetBuffer() + CIME::s_nCompCaret);
                            // Send left keystrokes
                            for(int i = 0; i < nCount; ++i)
                                SendMessage(UIGetHWND(), WM_KEYDOWN, VK_LEFT, 0);
                            SendMessage(UIGetHWND(), WM_KEYUP, VK_LEFT, 0);
                        }
                    }

                    ResetCaretBlink();
                }

                // Retrieve comp string attributes
                if(lParam & GCS_COMPATTR)
                {
                    lRet = CIME::_ImmGetCompositionStringW(hImc, GCS_COMPATTR, CIME::s_abCompStringAttr, sizeof(CIME::s_abCompStringAttr));
                    if(lRet > 0)
                        CIME::s_abCompStringAttr[lRet] = 0;  // ??? Is this needed for attributes?
                }

                // Retrieve clause information
                if(lParam & GCS_COMPCLAUSE)
                {
                    lRet = CIME::_ImmGetCompositionStringW(hImc, GCS_COMPCLAUSE, CIME::s_adwCompStringClause, sizeof(CIME::s_adwCompStringClause));
                    CIME::s_adwCompStringClause[lRet / sizeof(DWORD)] = 0;  // Terminate
                }

                CIME::_ImmReleaseContext(UIGetHWND(), hImc);
            }
            break;

        case WM_IME_ENDCOMPOSITION:
            UITRACE(L"WM_IME_ENDCOMPOSITION\n");
            TruncateCompString();
            CIME::ResetCompositionString();
            // We can show the edit control's caret again.
            s_bHideCaret = false;
            // Hide reading window
            CIME::s_bShowReadingWindow = false;
            break;

        case WM_IME_NOTIFY:
            UITRACE(L"WM_IME_NOTIFY %u\n", wParam);
            switch(wParam)
            {
                case IMN_SETCONVERSIONMODE:
                    UITRACE(L"  IMN_SETCONVERSIONMODE\n");
                case IMN_SETOPENSTATUS:
                    UITRACE(L"  IMN_SETOPENSTATUS\n");
                    CIME::CheckToggleState();
                    break;

                case IMN_OPENCANDIDATE:
                case IMN_CHANGECANDIDATE:
                {
                    UITRACE(wParam == IMN_CHANGECANDIDATE ? L"  IMN_CHANGECANDIDATE\n" : L"  IMN_OPENCANDIDATE\n");

                    CIME::s_CandList.bShowWindow = true;
                    *trapped = true;
                    if(NULL == (hImc = ImmGetContext(UIGetHWND())))
                        break;

                    LPCANDIDATELIST lpCandList = NULL;
                    DWORD dwLenRequired;

                    CIME::s_bShowReadingWindow = false;
                    // Retrieve the candidate list
                    dwLenRequired = CIME::_ImmGetCandidateListW(hImc, 0, NULL, 0);
                    if(dwLenRequired)
                    {
                        lpCandList = (LPCANDIDATELIST)HeapAlloc(GetProcessHeap(), 0, dwLenRequired);
                        dwLenRequired = CIME::_ImmGetCandidateListW(hImc, 0, lpCandList, dwLenRequired);
                    }

                    if(lpCandList)
                    {
                        // Update candidate list data
                        CIME::s_CandList.dwSelection = lpCandList->dwSelection;
                        CIME::s_CandList.dwCount = lpCandList->dwCount;

                        int nPageTopIndex = 0;
                        CIME::s_CandList.dwPageSize = __min(lpCandList->dwPageSize, MAX_CANDLIST);
                        if(CIME::GetPrimaryLanguage() == LANG_JAPANESE)
                        {
                            // Japanese IME organizes its candidate list a little
                            // differently from the other IMEs.
                            nPageTopIndex = (CIME::s_CandList.dwSelection / CIME::s_CandList.dwPageSize) * CIME::s_CandList.dwPageSize;
                        }
                        else
                            nPageTopIndex = lpCandList->dwPageStart;

                        // Make selection index relative to first entry of page
                        CIME::s_CandList.dwSelection = (CIME::GetLanguage() == LANG_CHS && !CIME::GetImeId()) ? (DWORD)-1
                                                 : CIME::s_CandList.dwSelection - nPageTopIndex;

                        ZeroMemory(CIME::s_CandList.awszCandidate, sizeof(CIME::s_CandList.awszCandidate));
                        for(UINT i = nPageTopIndex, j = 0;
                            (DWORD)i < lpCandList->dwCount && j < CIME::s_CandList.dwPageSize;
                            i++, j++)
                        {
                            // Initialize the candidate list strings
                            LPWSTR pwsz = CIME::s_CandList.awszCandidate[j];
                            // For every candidate string entry,
                            // write [index] + Space + [String] if vertical,
                            // write [index] + [String] + Space if horizontal.
                            *pwsz++ = (WCHAR)(L'0' + ((j + 1) % 10));  // Index displayed is 1 based
                            if(CIME::s_bVerticalCand)
                                *pwsz++ = L' ';
                            WCHAR *pwszNewCand = (LPWSTR)((LPBYTE)lpCandList + lpCandList->dwOffset[i]);
                            while (*pwszNewCand)
                                *pwsz++ = *pwszNewCand++;
                            if(!CIME::s_bVerticalCand)
                                *pwsz++ = L' ';
                            *pwsz = 0;  // Terminate
                        }

                        // Make dwCount in CIME::s_CandList be number of valid entries in the page.
                        CIME::s_CandList.dwCount = lpCandList->dwCount - lpCandList->dwPageStart;
                        if(CIME::s_CandList.dwCount > lpCandList->dwPageSize)
                            CIME::s_CandList.dwCount = lpCandList->dwPageSize;

                        HeapFree(GetProcessHeap(), 0, lpCandList);
                        CIME::_ImmReleaseContext(UIGetHWND(), hImc);

                        // Korean and old Chinese IME can't have selection.
                        // User must use the number hotkey or Enter to select
                        // a candidate.
                        if(CIME::GetPrimaryLanguage() == LANG_KOREAN ||
                            CIME::GetLanguage() == LANG_CHT && !CIME::GetImeId())
                        {
                            CIME::s_CandList.dwSelection = (DWORD)-1;
                        }

                        // Initialize CIME::s_CandList.HoriCand if we have a
                        // horizontal candidate window.
                        if(!CIME::s_bVerticalCand)
                        {
							std::wstring wstrCand;

                            CIME::s_CandList.nFirstSelected = 0;
                            CIME::s_CandList.nHoriSelectedLen = 0;
                            for(UINT i = 0; i < MAX_CANDLIST; ++i)
                            {
                                if(CIME::s_CandList.awszCandidate[i][0] == L'\0')
                                    break;

								std::wstring wstrEntry;
								wstrEntry = CIME::s_CandList.awszCandidate[i];
                                // If this is the selected entry, mark its char position.
                                if(CIME::s_CandList.dwSelection == i)
                                {
									CIME::s_CandList.nFirstSelected = wstrCand.length();
									CIME::s_CandList.nHoriSelectedLen = wstrEntry.length() - 1;  // Minus space
                                }
								wstrCand += wstrEntry;
                               // StringCchCat(wszCand, 256, wszEntry);
                            }
                          //  wszCand[lstrlenW(wszCand) - 1] = L'\0';  // Remove the last space
                            CIME::s_CandList.HoriCand.SetText(wstrCand.c_str());
                        }
                    }
                    break;
                }

                case IMN_CLOSECANDIDATE:
                {
                    UITRACE(L"  IMN_CLOSECANDIDATE\n");
                    CIME::s_CandList.bShowWindow = false;
                    if(!CIME::s_bShowReadingWindow)
                    {
                        CIME::s_CandList.dwCount = 0;
                        ZeroMemory(CIME::s_CandList.awszCandidate, sizeof(CIME::s_CandList.awszCandidate));
                    }
                    *trapped = true;
                    break;
                }

                case IMN_PRIVATE:
                    UITRACE(L"  IMN_PRIVATE\n");
                    {
                        if(!CIME::s_CandList.bShowWindow)
                            CIME::GetPrivateReadingString();

                        // Trap some messages to hide reading window
                        DWORD dwId = CIME::GetImeId();
                        switch(dwId)
                        {
                            case IMEID_CHT_VER42:
                            case IMEID_CHT_VER43:
                            case IMEID_CHT_VER44:
                            case IMEID_CHS_VER41:
                            case IMEID_CHS_VER42:
                                if((lParam == 1) || (lParam == 2))
                                {
                                    *trapped = true;
                                }
                                break;

                            case IMEID_CHT_VER50:
                            case IMEID_CHT_VER51:
                            case IMEID_CHT_VER52:
                            case IMEID_CHT_VER60:
                            case IMEID_CHS_VER53:
                                if((lParam == 16) || (lParam == 17) || (lParam == 26) || (lParam == 27) || (lParam == 28))
                                {
                                    *trapped = true;
                                }
                                break;
                        }
                    }
                    break;

                default:
                    *trapped = true;
                    break;
            }
            break;

        // When Text Service Framework is installed in Win2K, Alt+Shift and Ctrl+Shift combination (to switch input
        // locale / keyboard layout) doesn't send WM_KEYUP message for the key that is released first. We need to check
        // if these keys are actually up whenever we receive key up message for other keys.
        case WM_KEYUP:
        case WM_SYSKEYUP:
            if (!(lAlt & 0x80000000) && wParam != VK_MENU && (GetAsyncKeyState(VK_MENU) & 0x8000) == 0)
            {
                PostMessageW(GetFocus(), WM_KEYUP, (WPARAM)VK_MENU, (lAlt & 0x01ff0000) | 0xC0000001);
            }   
            else if (!(lCtrl & 0x80000000) && wParam != VK_CONTROL && (GetAsyncKeyState(VK_CONTROL) & 0x8000) == 0)
            {
                PostMessageW(GetFocus(), WM_KEYUP, (WPARAM)VK_CONTROL, (lCtrl & 0x01ff0000) | 0xC0000001);
            }
            else if (!(lShift & 0x80000000) && wParam != VK_SHIFT && (GetAsyncKeyState(VK_SHIFT) & 0x8000) == 0)
            {
                PostMessageW(GetFocus(), WM_KEYUP, (WPARAM)VK_SHIFT, (lShift & 0x01ff0000) | 0xC0000001);
            }
            // fall through WM_KEYDOWN / WM_SYSKEYDOWN
        case WM_KEYDOWN:
        case WM_SYSKEYDOWN:
            switch (wParam)
            {
                case VK_MENU:
                    lAlt = lParam;
                    break;
                case VK_SHIFT:
                    lShift = lParam;
                    break;
                case VK_CONTROL:
                    lCtrl = lParam;
                    break;
            }
            //break;
            // Fall through to default case
            // so we invoke the parent.

        default:
            // Let the parent handle the message that we
            // don't handle.
            return CUIEditBox::MsgProc(uMsg, wParam, lParam);

    }  // switch

    return *trapped;
}

void CUIIMEEditBox::RenderCandidateReadingWindow(float fElapsedTime, bool bReading)
{
    CRect<float> rc;
    UINT nNumEntries = bReading ? 4 : MAX_CANDLIST;
    Color32 clrTextColor, clrTextBkColor, clrSelTextColor, clrSelBkColor;
    int nX, nXFirst, nXComp;
    m_Buffer.CPtoX(m_nCaret, FALSE, nX);
    m_Buffer.CPtoX(m_nFirstVisible, FALSE, nXFirst);

    if(bReading)
    {
        clrTextColor = m_ReadingColor;
        clrTextBkColor = m_ReadingWinColor;
        clrSelTextColor = m_ReadingSelColor;
        clrSelBkColor = m_ReadingSelBkColor;
    } else
    {
        clrTextColor = m_CandidateColor;
        clrTextBkColor = m_CandidateWinColor;
        clrSelTextColor = m_CandidateSelColor;
        clrSelBkColor = m_CandidateSelBkColor;
    }

    // For Japanese IME, align the window with the first target converted character.
    // For all other IMEs, align with the caret.  This is because the caret
    // does not move for Japanese IME.
    if (CIME::GetLanguage() == LANG_CHT && !CIME::GetImeId())
        nXComp = 0;
    else
    if(CIME::GetPrimaryLanguage() == LANG_JAPANESE)
        CIME::s_CompString.CPtoX(CIME::s_nFirstTargetConv, FALSE, nXComp);
    else
        CIME::s_CompString.CPtoX(CIME::s_nCompCaret, FALSE, nXComp);

    // Compute the size of the candidate window
    int nWidthRequired = 0;
    int nHeightRequired = 0;
    int nSingleLineHeight = 0;

    if((CIME::s_bVerticalCand && !bReading) ||
        (!CIME::s_bHorizontalReading && bReading))
    {
        // Vertical window
        for(UINT i = 0; i < nNumEntries; ++i)
        {
            if(CIME::s_CandList.awszCandidate[i][0] == L'\0')
                break;
            rc.set(0, 0, 0, 0);
            UIGraph::getInstance().CalcTextRect(CIME::s_CandList.awszCandidate[i], rc);
            nWidthRequired = __max(nWidthRequired, rc.right - rc.left);
            nSingleLineHeight = __max(nSingleLineHeight, rc.bottom - rc.top);
        }
        nHeightRequired = nSingleLineHeight * nNumEntries;
    } else
    {
        // Horizontal window
        rc.set(0, 0, 0, 0);
        if(bReading)
			UIGraph::getInstance().CalcTextRect(CIME::s_wstrReadingString.c_str(), rc);
        else
			UIGraph::getInstance().CalcTextRect(CIME::s_CandList.HoriCand.GetBuffer(), rc);
        nWidthRequired = rc.right - rc.left;
        nSingleLineHeight = nHeightRequired = rc.bottom - rc.top;
    }

    // Now that we have the dimension, calculate the location for the candidate window.
    // We attempt to fit the window in this order:
    // bottom, top, right, left.

    bool bHasPosition = false;

    // Bottom
    rc.set(CIME::s_ptCompString.x + nXComp, CIME::s_ptCompString.y + m_rcBoundingBox.bottom - m_rcBoundingBox.top,
                  CIME::s_ptCompString.x + nXComp + nWidthRequired, CIME::s_ptCompString.y + m_rcBoundingBox.bottom - m_rcBoundingBox.top + nHeightRequired);
    // if the right edge is cut off, move it left.
   /* if(rc.right > m_pParentDialog->GetWidth())
    {
        rc.left -= rc.right - m_pParentDialog->GetWidth();
        rc.right = m_pParentDialog->GetWidth();
    }
    if(rc.bottom <= m_pParentDialog->GetHeight())
        bHasPosition = true;
*/
    // Top
    if(!bHasPosition)
    {
        rc.set(CIME::s_ptCompString.x + nXComp, CIME::s_ptCompString.y - nHeightRequired,
                      CIME::s_ptCompString.x + nXComp + nWidthRequired, CIME::s_ptCompString.y);
        // if the right edge is cut off, move it left.
  /*      if(rc.right > m_pParentDialog->GetWidth())
        {
            rc.left -= rc.right - m_pParentDialog->GetWidth();
            rc.right = m_pParentDialog->GetWidth();
        }
        if(rc.top >= 0)
            bHasPosition = true;*/
    }

    // Right
    if(!bHasPosition)
    {
        int nXCompTrail;
        CIME::s_CompString.CPtoX(CIME::s_nCompCaret, TRUE, nXCompTrail);
        rc.set(CIME::s_ptCompString.x + nXCompTrail, 0,
                      CIME::s_ptCompString.x + nXCompTrail + nWidthRequired, nHeightRequired);
       /* if(rc.right <= m_pParentDialog->GetWidth())
            bHasPosition = true;*/
    }

    // Left
    if(!bHasPosition)
    {
        rc.set(CIME::s_ptCompString.x + nXComp - nWidthRequired, 0,
                      CIME::s_ptCompString.x + nXComp, nHeightRequired);
        if(rc.right >= 0)
            bHasPosition = true;
    }

    if(!bHasPosition)
    {
        // The dialog is too small for the candidate window.
        // Fall back to render at 0, 0.  Some part of the window
        // will be cut off.
        rc.left = 0;
        rc.right = nWidthRequired;
    }

    // If we are rendering the candidate window, save the position
    // so that mouse clicks are checked properly.
    if(!bReading)
        CIME::s_CandList.rcCandidate = rc.getRECT();

    // Render the elements
	// 候选字词列表背景
    ///UIGraph::getInstance().FillRect(rc, clrTextBkColor);
    if((CIME::s_bVerticalCand && !bReading) ||
        (!CIME::s_bHorizontalReading && bReading))
    {
        // Vertical candidate window
        for(UINT i = 0; i < nNumEntries; ++i)
        {
            // Here we are rendering one line at a time
            rc.bottom = rc.top + nSingleLineHeight;
            // Use a different color for the selected string
            if(CIME::s_CandList.dwSelection == i)
            {
   ///             UIGraph::getInstance().FillRect(rc, clrSelBkColor);
  ///              m_Style.m_mapFont[1].color = clrSelTextColor;
            } else
   ///             m_Style.m_mapFont[1].color = clrTextColor;

            UIGraph::getInstance().DrawText(CIME::s_CandList.awszCandidate[i], m_Style, 1, rc);

            rc.top += nSingleLineHeight;
        }
    } else
    {
        // Horizontal candidate window
///		m_Style.m_mapFont[1].color = clrTextColor;
        if(bReading)
            UIGraph::getInstance().DrawText(CIME::s_wstrReadingString.c_str(), m_Style, 1, rc);
        else
            UIGraph::getInstance().DrawText(CIME::s_CandList.HoriCand.GetBuffer(), m_Style, 1, rc);

        // Render the selected entry differently
        if(!bReading)
        {
            int nXLeft, nXRight;
            CIME::s_CandList.HoriCand.CPtoX(CIME::s_CandList.nFirstSelected, FALSE, nXLeft);
            CIME::s_CandList.HoriCand.CPtoX(CIME::s_CandList.nFirstSelected + CIME::s_CandList.nHoriSelectedLen, FALSE, nXRight);

            rc.right = rc.left + nXRight;
            rc.left += nXLeft;
///            UIGraph::getInstance().FillRect(rc, clrSelBkColor);
 ///           m_Style.m_mapFont[1].color = clrSelTextColor;
            UIGraph::getInstance().DrawText(CIME::s_CandList.HoriCand.GetBuffer() + CIME::s_CandList.nFirstSelected,
                                m_Style, 1, rc, CIME::s_CandList.nHoriSelectedLen);
        }
    }
}

void CUIIMEEditBox::RenderComposition(float fElapsedTime)
{
    RECT rcCaret = { 0, 0, 0, 0 };
    int nX, nXFirst;
    m_Buffer.CPtoX(m_nCaret, FALSE, nX);
    m_Buffer.CPtoX(m_nFirstVisible, FALSE, nXFirst);
    //CUIStyle *pStyle = GetStyle(1);

    // Get the required width
	CRect<float> rc;
	rc.set( m_rcBoundingBox.left + nX - nXFirst, m_rcBoundingBox.top,
                m_rcBoundingBox.left + nX - nXFirst, m_rcBoundingBox.bottom);
	UIGraph::getInstance().CalcTextRect(CIME::s_CompString.GetBuffer(), rc);

    // If the composition string is too long to fit within
    // the text area, move it to below the current line.
    // This matches the behavior of the default IME.
    if(rc.right > m_rcBoundingBox.right)
        rc.offset(m_rcBoundingBox.left - rc.left, rc.bottom - rc.top);

    // Save the rectangle position for processing highlighted text.
    RECT rcFirst = rc.getRECT();

    // Update CIME::s_ptCompString for RenderCandidateReadingWindow().
    CIME::s_ptCompString.x = rc.left; CIME::s_ptCompString.y = rc.top;

    Color32 TextColor = m_CompColor;
    // Render the window and string.
    // If the string is too long, we must wrap the line.
 ///   m_Style.m_mapFont[1].color = TextColor;
    const WCHAR *pwszComp = CIME::s_CompString.GetBuffer();
    int nCharLeft = CIME::s_CompString.GetTextSize();
    for(; ;)
    {
        // Find the last character that can be drawn on the same line.
        int nLastInLine;
        bool bTrail;
        CIME::s_CompString.XtoCP(m_rcBoundingBox.right - rc.left, nLastInLine, bTrail);
        int nNumCharToDraw = __min(nCharLeft, nLastInLine);
        UIGraph::getInstance().CalcTextRect(pwszComp, rc/*, nNumCharToDraw*/);

        // Draw the background
        // For Korean IME, blink the composition window background as if it
        // is a cursor.
        if(CIME::GetPrimaryLanguage() == LANG_KOREAN)
        {
            if(m_bCaretOn)
            {
///                UIGraph::getInstance().FillRect(rc, m_CompWinColor);
            }
            else
            {
                // Not drawing composition string background. We
                // use the editbox's text color for composition
                // string text.
//                TextColor = m_Style.GetCyclostyle().m_FontStyle[0].setColor[CONTROL_STATE_NORMAL].getColor();
            }
        } else
        {
            // Non-Korean IME. Always draw composition background.
///            UIGraph::getInstance().FillRect(rc, m_CompWinColor);
        }

        // Draw the text
///        m_Style.m_mapFont[1].color = TextColor;
        UIGraph::getInstance().DrawText(pwszComp, m_Style, 1, rc, nNumCharToDraw);

        // Advance pointer and counter
        nCharLeft -= nNumCharToDraw;
        pwszComp += nNumCharToDraw;
        if(nCharLeft <= 0)
            break;

        // Advance rectangle coordinates to beginning of next line
       rc.offset(m_rcBoundingBox.left - rc.left, rc.bottom - rc.top);
    }

    // Load the rect for the first line again.
    rc = rcFirst;

    // Inspect each character in the comp string.
    // For target-converted and target-non-converted characters,
    // we display a different background color so they appear highlighted.
    int nCharFirst = 0;
    nXFirst = 0;
    CIME::s_nFirstTargetConv = -1;
    BYTE *pAttr;
    const WCHAR *pcComp;
    for(pcComp = CIME::s_CompString.GetBuffer(), pAttr = CIME::s_abCompStringAttr;
          *pcComp != L'\0'; ++pcComp, ++pAttr)
    {
        Color32 bkColor;

        // Render a different background for this character
        int nXLeft, nXRight;
        CIME::s_CompString.CPtoX(int(pcComp - CIME::s_CompString.GetBuffer()), FALSE, nXLeft);
        CIME::s_CompString.CPtoX(int(pcComp - CIME::s_CompString.GetBuffer()), TRUE, nXRight);

        // Check if this character is off the right edge and should
        // be wrapped to the next line.
        if(nXRight - nXFirst > m_rcBoundingBox.right - rc.left)
        {
            // Advance rectangle coordinates to beginning of next line
            rc.offset(m_rcBoundingBox.left - rc.left, rc.bottom - rc.top);

            // Update the line's first character information
            nCharFirst = int(pcComp - CIME::s_CompString.GetBuffer());
            CIME::s_CompString.CPtoX(nCharFirst, FALSE, nXFirst);
        }

        // If the caret is on this character, save the coordinates
        // for drawing the caret later.
        if(CIME::s_nCompCaret == int(pcComp - CIME::s_CompString.GetBuffer()))
        {
            rcCaret = rc.getRECT();
            rcCaret.left += nXLeft - nXFirst - 1;
            rcCaret.right = rcCaret.left + 2;
        }

        // Set up color based on the character attribute
        if(*pAttr == ATTR_TARGET_CONVERTED)
        {
 ///           m_Style.m_mapFont[1].color = m_CompTargetColor;
            bkColor = m_CompTargetBkColor;
        }
        else
        if(*pAttr == ATTR_TARGET_NOTCONVERTED)
        {
///            m_Style.m_mapFont[1].color = m_CompTargetNonColor;
            bkColor = m_CompTargetNonBkColor;
        }
        else
        {
            continue;
        }

        RECT rcTarget = { rc.left + nXLeft - nXFirst, rc.top, rc.left + nXRight - nXFirst, rc.bottom };
///        UIGraph::getInstance().FillRect(rcTarget, bkColor);
        UIGraph::getInstance().DrawText(pcComp, m_Style, 1, rcTarget, 1);

        // Record the first target converted character's index
        if(-1 == CIME::s_nFirstTargetConv)
            CIME::s_nFirstTargetConv = int(pAttr - CIME::s_abCompStringAttr);
    }

    // Render the composition caret
    if(m_bCaretOn)
    {
        // If the caret is at the very end, its position would not have
        // been computed in the above loop.  We compute it here.
        if(CIME::s_nCompCaret == CIME::s_CompString.GetTextSize())
        {
            CIME::s_CompString.CPtoX(CIME::s_nCompCaret, FALSE, nX);
            rcCaret = rc.getRECT();
            rcCaret.left += nX - nXFirst - 1;
            rcCaret.right = rcCaret.left + 2;
        }

///        UIGraph::getInstance().FillRect(rcCaret, m_CompCaretColor);
    }
}

void CUIIMEEditBox::RenderIndicator(float fElapsedTime)
{
	m_Style.Blend(/*m_rcIndicator,*/CONTROL_STATE_NORMAL, fElapsedTime);

    //UIGraph::getInstance().DrawSprite(m_Style, 1, m_rcIndicator);?????????????????????????????????????????我注释掉
    RECT rc = m_rcIndicator;
///    m_Style.m_mapFont[1].color = CIME::s_ImeState == CIME::IMEUI_STATE_ON && CIME::s_bEnableImeSystem ? m_IndicatorImeColor : m_IndicatorEngColor;
    CRect<float> rcCalc;
	rcCalc.set( 0, 0, 0, 0 );
    // If IME system is off, draw English indicator.
    WCHAR *pwszIndicator = CIME::s_bEnableImeSystem ? CIME::s_wszCurrIndicator : CIME::s_aszIndicator[0];

    UIGraph::getInstance().CalcTextRect(pwszIndicator, rcCalc);
    UIGraph::getInstance().DrawText(pwszIndicator,m_Style, 1, rc);
}

void CUIIMEEditBox::XMLParse(const TiXmlElement* pControlElement)
{
	CUIEditBox::XMLParse(pControlElement);
}

void CUIIMEEditBox::OnFrameRender(const Matrix& mTransform, double fTime, float fElapsedTime)
{
    // If we have not computed the indicator symbol width,
    // do it.
    if(!m_nIndicatorWidth)
    {
        for(int i = 0; i < 5; ++i)
        {
            CRect<float> rc;
			rc.set(0,0,0,0);
            UIGraph::getInstance().CalcTextRect(CIME::s_aszIndicator[i], rc);
            m_nIndicatorWidth = __max(m_nIndicatorWidth, rc.right - rc.left);
        }
        // Update the rectangles now that we have the indicator's width
        UpdateRects();
    }

    // Let the parent render first (edit control)
    CUIEditBox::OnFrameRender(mTransform,fTime,fElapsedTime);

    //
    // Now render the IME elements
    //
	if(1/*IsFocus()*/)
	{
		// Render the input locale indicator
		RenderIndicator(fElapsedTime);

		// Display the composition string.
		// This method should also update CIME::s_ptCompString
		// for RenderCandidateReadingWindow.
		// 你输入待定的文字
		RenderComposition(fElapsedTime);

		// Display the reading/candidate window. RenderCandidateReadingWindow()
		// uses CIME::s_ptCompString to position itself.  CIME::s_ptCompString must have
		// been filled in by RenderComposition().
		// 就候选文字背景
		if(CIME::s_bShowReadingWindow)
		{
			// Reading window
			RenderCandidateReadingWindow(fElapsedTime, true);
		}
		else if(CIME::s_CandList.bShowWindow)
		{
			// Candidate list window
			RenderCandidateReadingWindow(fElapsedTime, false);
		}
	}
}



// Truncate composition string by sending keystrokes to the window.
void CUIIMEEditBox::TruncateCompString(bool bUseBackSpace, int iNewStrLen)
{
	if(!CIME::s_bInsertOnType)
		return;

	int cc = (int) wcslen(CIME::s_CompString.GetBuffer());
	assert(iNewStrLen == 0 || iNewStrLen >= cc);

	// Send right arrow keystrokes to move the caret
	//   to the end of the composition string.
	for (int i = 0; i < cc - CIME::s_nCompCaret; ++i)
		SendMessage(UIGetHWND(), WM_KEYDOWN, VK_RIGHT, 0);
	SendMessage(UIGetHWND(), WM_KEYUP, VK_RIGHT, 0);

	if(bUseBackSpace || m_bInsertMode)
		iNewStrLen = 0;

	// The caller sets bUseBackSpace to false if there's possibility of sending
	// new composition string to the app right after this function call.
	// 
	// If the app is in overwriting mode and new comp string is 
	// shorter than current one, delete previous comp string 
	// till it's same long as the new one. Then move caret to the beginning of comp string.
	// New comp string will overwrite old one.
	if(iNewStrLen < cc)
	{
		for(int i = 0; i < cc - iNewStrLen; ++i)
		{
			SendMessage(UIGetHWND(), WM_KEYDOWN, VK_BACK, 0);  // Backspace character
			SendMessageW(UIGetHWND(), WM_CHAR, VK_BACK, 0);
		}
		SendMessage(UIGetHWND(), WM_KEYUP, VK_BACK, 0);
	}
	else
		iNewStrLen = cc;

	// Move the caret to the beginning by sending left keystrokes
	for (int i = 0; i < iNewStrLen; ++i)
		SendMessage(UIGetHWND(), WM_KEYDOWN, VK_LEFT, 0);
	SendMessage(UIGetHWND(), WM_KEYUP, VK_LEFT, 0);
}


// Outputs current composition string then cleans up the composition task.
void CUIIMEEditBox::FinalizeString(bool bSend)
{
	HIMC hImc;
	if(NULL == (hImc = ImmGetContext(UIGetHWND())))
		return;

	static bool bProcessing = false;
	if(bProcessing)    // avoid infinite recursion
	{
		UITRACE(L"CIME::FinalizeString: Reentrant detected!\n");
		CIME::_ImmReleaseContext(UIGetHWND(), hImc);
		return;
	}
	bProcessing = true;

	if(!CIME::s_bInsertOnType && bSend)
	{
		// Send composition string to app.
		LONG lLength = lstrlenW(CIME::s_CompString.GetBuffer());
		// In case of CHT IME, don't send the trailing double byte space, if it exists.
		if(CIME::GetLanguage() == LANG_CHT
			&& CIME::s_CompString[lLength - 1] == 0x3000)
		{
			CIME::s_CompString[lLength - 1] = 0;
		}
		SendCompString();
	}

	CIME::ResetCompositionString();
	// Clear composition string in IME
	CIME::_ImmNotifyIME(hImc, NI_COMPOSITIONSTR, CPS_CANCEL, 0);
	// the following line is necessary as Korean IME doesn't close cand list
	// when comp string is cancelled.
	CIME::_ImmNotifyIME(hImc, NI_CLOSECANDIDATE, 0, 0); 
	CIME::_ImmReleaseContext(UIGetHWND(), hImc);
	bProcessing = false;
}


// Sends the current composition string to the application by sending keystroke
// messages.
void CUIIMEEditBox::SendCompString()
{
	for(int i = 0; i < lstrlenW(CIME::s_CompString.GetBuffer()); ++i)
		MsgProc(WM_CHAR, (WPARAM)CIME::s_CompString[i], 0);
}