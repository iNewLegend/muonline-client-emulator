#include "UIRollup.h"

CUIRollup::CUIRollup()
{
	m_Type = UI_CONTROL_LISTBOX;
	m_dwStyle = 0;
	m_nSBWidth = 16;
	m_nSelected = -1;
	m_nSelStart = 0;
	m_bDrag = false;
	m_nBorder = 6;
	m_nMargin = 5;
	m_nTextHeight = 0;
}


CUIRollup::~CUIRollup()
{
	RemoveAllPages();
}

void CUIRollup::SetParent(CUICombo *pControl)
{
	CUIControl::SetParent(pControl);
	m_ScrollBar.SetParent(pControl);
}

void CUIRollup::XMLParse(const TiXmlElement* pControlElement)
{
	CUIControl::XMLParse(pControlElement);
	//
}


void CUIRollup::UpdateRects()
{
	CUIControl::UpdateRects();

	m_rcSelection = m_rcBoundingBox;
	m_rcSelection.right -= m_nSBWidth;
	m_rcSelection.InflateRect(-m_nBorder, -m_nBorder);
	m_rcText = m_rcSelection;
	m_rcText.InflateRect(-m_nMargin, 0);

	// Update the scrollbar's rects
	//m_ScrollBar.SetLocation(m_rcBoundingBox.right - m_nSBWidth, m_rcBoundingBox.top);
	//m_ScrollBar.SetSize(m_nSBWidth, m_height);
	{
		m_ScrollBar.SetPageSize(m_rcText.getHeight() / UIGraph::getInstance().GetFontSize());

		// The selected Page may have been scrolled off the page.
		// Ensure that it is in page again.
		m_ScrollBar.ShowItem(m_nSelected);
	}
}



HRESULT CUIRollup::AddPage(const WCHAR *wszText, CUIDialog* pDialog, void *pData)
{
	UIRollupPage *pNewPage = new UIRollupPage;
	if(!pNewPage)
		return E_OUTOFMEMORY;

	pNewPage->wstrName = wszText;
	pNewPage->pData = pData;
	SetRect(&pNewPage->rcActive, 0, 0, 0, 0);
	pNewPage->bSelected = false;
	pNewPage->pDialog = pDialog;

	m_Pages.push_back(pNewPage);
	m_ScrollBar.SetTrackRange(0, m_Pages.size());

	return S_OK;
}

HRESULT CUIRollup::InsertPage(int nIndex, const WCHAR *wszText, CUIDialog* pDialog, void *pData)
{
	UIRollupPage *pNewPage = new UIRollupPage;
	if(!pNewPage)
		return E_OUTOFMEMORY;

	pNewPage->wstrName = wszText;
	pNewPage->pData = pData;
	SetRect(&pNewPage->rcActive, 0, 0, 0, 0);
	pNewPage->bSelected = false;
	pNewPage->pDialog = pDialog;
	//pNewPage->pButton = new CUIButton;

	m_Pages.insert(m_Pages.begin()+nIndex, pNewPage);
	m_ScrollBar.SetTrackRange(0, m_Pages.size());
	return S_OK;
}

void CUIRollup::RemovePage(int nIndex)
{
	if(nIndex < 0 || nIndex >= (int)m_Pages.size())
		return;

	UIRollupPage *pPage = m_Pages[nIndex];

	delete pPage;
	m_Pages.erase(m_Pages.begin()+nIndex);
	m_ScrollBar.SetTrackRange(0, m_Pages.size());
	if(m_nSelected >= (int)m_Pages.size())
		m_nSelected = m_Pages.size() - 1;

	sendEvent(EVENT_LISTBOX_SELECTION);
}

void CUIRollup::RemovePageByText(WCHAR *wszText)
{
}

void CUIRollup::RemovePageByData(void *pData)
{
}

void CUIRollup::RemoveAllPages()
{
	for(size_t i=0;i<m_Pages.size();++i)
	{
		UIRollupPage *pPage = m_Pages[i];
		delete pPage;
	}
	m_nSelected = -1;
	m_Pages.clear();
	m_ScrollBar.SetTrackRange(0, 1);
}

UIRollupPage *CUIRollup::GetPage(int nIndex)
{
	if(nIndex < 0 || nIndex >= (int)m_Pages.size())
		return NULL;

	return m_Pages[nIndex];
}


// For single-selection listbox, returns the index of the selected Page.
// For multi-selection, returns the first selected Page after the nPreviousSelected position.
// To search for the first selected Page, the app passes -1 for nPreviousSelected.  For
// subsequent searches, the app passes the returned index back to GetSelectedIndex as.
// nPreviousSelected.
// Returns -1 on error or if no Page is selected.
int CUIRollup::GetSelectedIndex(int nPreviousSelected)
{
	if(nPreviousSelected < -1)
		return -1;

	if(m_dwStyle & MULTISELECTION)
	{
		// Multiple selection enabled. Search for the next Page with the selected flag.
		for(int i = nPreviousSelected + 1; i < (int)m_Pages.size(); ++i)
		{
			UIRollupPage *pPage = m_Pages[i];

			if(pPage->bSelected)
				return i;
		}

		return -1;
	}
	else
	{
		// Single selection
		return m_nSelected;
	}
}

void CUIRollup::SelectPage(int nNewIndex)
{
	// If no Page exists, do nothing.
	if(m_Pages.size() == 0)
		return;

	int nOldSelected = m_nSelected;

	// Adjust m_nSelected
	m_nSelected = nNewIndex;

	// Perform capping
	if(m_nSelected < 0)
		m_nSelected = 0;
	if(m_nSelected >= (int)m_Pages.size())
		m_nSelected = m_Pages.size() - 1;

	if(nOldSelected != m_nSelected)
	{
		if(m_dwStyle & MULTISELECTION)
		{
			m_Pages[m_nSelected]->bSelected = true;
		}

		// Update selection start
		m_nSelStart = m_nSelected;

		// Adjust scroll bar
		m_ScrollBar.ShowItem(m_nSelected);
	}

	sendEvent(EVENT_LISTBOX_SELECTION);
}



bool CUIRollup::HandleKeyboard(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if(!m_bEnabled || !m_bVisible)
		return false;

	// Let the scroll bar have a chance to handle it first
	if(m_ScrollBar.HandleKeyboard(uMsg, wParam, lParam))
		return true;

	switch(uMsg)
	{
	case WM_KEYDOWN:
		switch(wParam)
		{
		case VK_UP:
		case VK_DOWN:
		case VK_NEXT:
		case VK_PRIOR:
		case VK_HOME:
		case VK_END:
			{
				// If no Page exists, do nothing.
				if(m_Pages.size() == 0)
					return true;

				int nOldSelected = m_nSelected;

				// Adjust m_nSelected
				switch(wParam)
				{
				case VK_UP: --m_nSelected; break;
				case VK_DOWN: ++m_nSelected; break;
				case VK_NEXT: m_nSelected += m_ScrollBar.GetPageSize() - 1; break;
				case VK_PRIOR: m_nSelected -= m_ScrollBar.GetPageSize() - 1; break;
				case VK_HOME: m_nSelected = 0; break;
				case VK_END: m_nSelected = m_Pages.size() - 1; break;
				}

				// Perform capping
				if(m_nSelected < 0)
					m_nSelected = 0;
				if(m_nSelected >= (int)m_Pages.size())
					m_nSelected = m_Pages.size() - 1;

				if(nOldSelected != m_nSelected)
				{
					if(m_dwStyle & MULTISELECTION)
					{
						// Multiple selection

						// Clear all selection
						for(int i = 0; i < (int)m_Pages.size(); ++i)
						{
							UIRollupPage *pPage = m_Pages[i];
							pPage->bSelected = false;
						}

						if(GetKeyState(VK_SHIFT) < 0)
						{
							// Select all Pages from m_nSelStart to
							// m_nSelected
							int nEnd = __max(m_nSelStart, m_nSelected);

							for(int n = __min(m_nSelStart, m_nSelected); n <= nEnd; ++n)
								m_Pages[n]->bSelected = true;
						}
						else
						{
							m_Pages[m_nSelected]->bSelected = true;

							// Update selection start
							m_nSelStart = m_nSelected;
						}
					} else
						m_nSelStart = m_nSelected;

					// Adjust scroll bar

					m_ScrollBar.ShowItem(m_nSelected);

					// Send notification

					sendEvent(EVENT_LISTBOX_SELECTION);
				}
				return true;
			}

			// Space is the hotkey for double-clicking an Page.
			//
		case VK_SPACE:
			sendEvent(EVENT_LISTBOX_ITEM_DBLCLK);
			return true;
		}
		break;
	}

	return false;
}



bool CUIRollup::HandleMouse(UINT uMsg, POINT pt, WPARAM wParam, LPARAM lParam)
{
	if(!m_bEnabled || !m_bVisible)
		return false;

	// First acquire focus
	if(WM_LBUTTONDOWN == uMsg)
	{
		SetFocus();
	}

	// Let the scroll bar handle it first.
	//if(m_ScrollBar.HandleMouse(uMsg, pt, wParam, lParam))
	//	return true;

	switch(uMsg)
	{
	case WM_LBUTTONDOWN:
	case WM_LBUTTONDBLCLK:
		// Check for clicks in the text area
		if(m_Pages.size() > 0 && m_rcSelection.ptInRect(pt))
		{
			// Compute the index of the clicked Page

			int nClicked;
			if(m_nTextHeight)
				nClicked = m_ScrollBar.GetTrackPos() + (pt.y - m_rcText.top) / m_nTextHeight;
			else
				nClicked = -1;

			// Only proceed if the click falls on top of an Page.

			if(nClicked >= m_ScrollBar.GetTrackPos() &&
				nClicked < (int)m_Pages.size() &&
				nClicked < m_ScrollBar.GetTrackPos() + m_ScrollBar.GetPageSize())
			{
				//SetCapture(UIGetHWND());
				m_bDrag = true;

				// If this is a double click, fire off an event and exit
				// since the first click would have taken care of the selection
				// updating.
				if(uMsg == WM_LBUTTONDBLCLK)
				{
					sendEvent(EVENT_LISTBOX_ITEM_DBLCLK);
					return true;
				}

				m_nSelected = nClicked;
				if(!(wParam & MK_SHIFT))
					m_nSelStart = m_nSelected;

				// If this is a multi-selection listbox, update per-Page
				// selection data.

				if(m_dwStyle & MULTISELECTION)
				{
					// Determine behavior based on the state of Shift and Ctrl

					UIRollupPage *pSelPage = m_Pages[m_nSelected];
					if((wParam & (MK_SHIFT|MK_CONTROL)) == MK_CONTROL)
					{
						// Control click. Reverse the selection of this Page.

						pSelPage->bSelected = !pSelPage->bSelected;
					} else
						if((wParam & (MK_SHIFT|MK_CONTROL)) == MK_SHIFT)
						{
							// Shift click. Set the selection for all Pages
							// from last selected Page to the current Page.
							// Clear everything else.

							int nBegin = __min(m_nSelStart, m_nSelected);
							int nEnd = __max(m_nSelStart, m_nSelected);

							for(int i = 0; i < nBegin; ++i)
							{
								UIRollupPage *pPage = m_Pages[i];
								pPage->bSelected = false;
							}

							for(int i = nEnd + 1; i < (int)m_Pages.size(); ++i)
							{
								UIRollupPage *pPage = m_Pages[i];
								pPage->bSelected = false;
							}

							for(int i = nBegin; i <= nEnd; ++i)
							{
								UIRollupPage *pPage = m_Pages[i];
								pPage->bSelected = true;
							}
						} else
							if((wParam & (MK_SHIFT|MK_CONTROL)) == (MK_SHIFT|MK_CONTROL))
							{
								// Control-Shift-click.

								// The behavior is:
								//   Set all Pages from m_nSelStart to m_nSelected to
								//     the same state as m_nSelStart, not including m_nSelected.
								//   Set m_nSelected to selected.

								int nBegin = __min(m_nSelStart, m_nSelected);
								int nEnd = __max(m_nSelStart, m_nSelected);

								// The two ends do not need to be set here.

								bool bLastSelected = m_Pages[m_nSelStart]->bSelected;
								for(int i = nBegin + 1; i < nEnd; ++i)
								{
									UIRollupPage *pPage = m_Pages[i];
									pPage->bSelected = bLastSelected;
								}

								pSelPage->bSelected = true;

								// Restore m_nSelected to the previous value
								// This matches the Windows behavior

								m_nSelected = m_nSelStart;
							} else
							{
								// Simple click.  Clear all Pages and select the clicked
								// Page.


								for(int i = 0; i < (int)m_Pages.size(); ++i)
								{
									UIRollupPage *pPage = m_Pages[i];
									pPage->bSelected = false;
								}

								pSelPage->bSelected = true;
							}
				}  // End of multi-selection case

				sendEvent(EVENT_LISTBOX_SELECTION);
			}

			return true;
		}
		break;

	case WM_LBUTTONUP:
		{
			//ReleaseCapture();
			m_bDrag = false;

			if(m_nSelected != -1)
			{
				// Set all Pages between m_nSelStart and m_nSelected to
				// the same state as m_nSelStart
				int nEnd = __max(m_nSelStart, m_nSelected);

				for(int n = __min(m_nSelStart, m_nSelected) + 1; n < nEnd; ++n)
					m_Pages[n]->bSelected = m_Pages[m_nSelStart]->bSelected;
				m_Pages[m_nSelected]->bSelected = m_Pages[m_nSelStart]->bSelected;

				// If m_nSelStart and m_nSelected are not the same,
				// the user has dragged the mouse to make a selection.
				// Notify the application of this.
				if(m_nSelStart != m_nSelected)
					sendEvent(EVENT_LISTBOX_SELECTION);

				sendEvent(EVENT_LISTBOX_SELECTION_END);
			}
			return false;
		}

	case WM_MOUSEMOVE:
		if(m_bDrag)
		{
			// Compute the index of the Page below cursor

			int nPage;
			if(m_nTextHeight)
				nPage = m_ScrollBar.GetTrackPos() + (pt.y - m_rcText.top) / m_nTextHeight;
			else
				nPage = -1;

			// Only proceed if the cursor is on top of an Page.

			if(nPage >= (int)m_ScrollBar.GetTrackPos() &&
				nPage < (int)m_Pages.size() &&
				nPage < m_ScrollBar.GetTrackPos() + m_ScrollBar.GetPageSize())
			{
				m_nSelected = nPage;
				sendEvent(EVENT_LISTBOX_SELECTION);
			} else
				if(nPage < (int)m_ScrollBar.GetTrackPos())
				{
					// User drags the mouse above window top
					m_ScrollBar.Scroll(-1);
					m_nSelected = m_ScrollBar.GetTrackPos();
					sendEvent(EVENT_LISTBOX_SELECTION);
				} else
					if(nPage >= m_ScrollBar.GetTrackPos() + m_ScrollBar.GetPageSize())
					{
						// User drags the mouse below window bottom
						m_ScrollBar.Scroll(1);
						m_nSelected = __min((int)m_Pages.size(), m_ScrollBar.GetTrackPos() + m_ScrollBar.GetPageSize()) - 1;
						sendEvent(EVENT_LISTBOX_SELECTION);
					}
		}
		break;

	case WM_MOUSEWHEEL:
		{
			UINT uLines;
			SystemParametersInfo(SPI_GETWHEELSCROLLLINES, 0, &uLines, 0);
			int nScrollAmount = int((short)HIWORD(wParam)) / WHEEL_DELTA * uLines;
			m_ScrollBar.Scroll(-nScrollAmount);
			return true;
		}
	}

	return false;
}

void CUIRollup::OnFrameRender(const Matrix& mTransform, double fTime, float fElapsedTime)
{
	if(m_bVisible == false)
		return;


	//m_Style.Blend(CONTROL_STATE_NORMAL, fElapsedTime);
//	UIGraph::getInstance().DrawSprite(m_Style, 0, m_rcBoundingBox);

	// Render the text
	if(m_Pages.size() > 0)
	{
		// Find out the height of a single line of text
		CRect<int> rc = m_rcText;
		CRect<int> rcSel = m_rcSelection;
		rc.bottom = rc.top+UIGraph::getInstance().GetFontSize();

		// Update the line height formation
		m_nTextHeight = rc.bottom - rc.top;

		static bool bSBInit;
		if(!bSBInit)
		{
			// Update the page size of the scroll bar
			if(m_nTextHeight)
				m_ScrollBar.SetPageSize(m_rcText.getHeight() / m_nTextHeight);
			else
				m_ScrollBar.SetPageSize(m_rcText.getHeight());
			bSBInit = true;
		}

		rc.right = m_rcText.right;
		for(int i = m_ScrollBar.GetTrackPos(); i < (int)m_Pages.size(); ++i)
		{
			if(rc.bottom > m_rcText.bottom)
				break;

			UIRollupPage *pPage = m_Pages[i];

			// Determine if we need to render this Page with the
			// selected element.
			bool bSelectedStyle = false;

			if(!(m_dwStyle & MULTISELECTION) && i == m_nSelected)
				bSelectedStyle = true;
			else
				if(m_dwStyle & MULTISELECTION)
				{
					if(m_bDrag &&
						((i >= m_nSelected && i < m_nSelStart) ||
						(i <= m_nSelected && i > m_nSelStart)))
						bSelectedStyle = m_Pages[m_nSelStart]->bSelected;
					else
						if(pPage->bSelected)
							bSelectedStyle = true;
				}

				if(bSelectedStyle)
				{
					rcSel.top = rc.top; rcSel.bottom = rc.bottom;
	//				UIGraph::getInstance().DrawSprite(m_Style, 1, rcSel);
//					UIGraph::getInstance().DrawText(pPage->wstrName, m_Style, 1, rc.getRECT());
				}
				else
//					UIGraph::getInstance().DrawText(pPage->wstrName, m_Style, 0, rc.getRECT());

				rc.offset(0, m_nTextHeight);
		}
	}

	// Render the scroll bar

	m_ScrollBar.OnFrameRender(mTransform,fTime,fElapsedTime);
}