#include "UIComboBox.h"

CUIComboBox::CUIComboBox()
{
	m_Type = UI_CONTROL_COMBOBOX;
}

CUIComboBox::~CUIComboBox()
{
}

void CUIComboBox::OnControlRegister()
{
	CUICombo::OnControlRegister();
	RegisterControl("IDC_LISTBOX",m_ListBox);
	RegisterControlEvent("IDC_LISTBOX",(PEVENT)&CUIComboBox::OnSelectionChanged);
	RegisterControlEvent("IDC_LISTBOX",(PEVENT)&CUIComboBox::OnSelectionChanged,EVENT_LISTBOX_SELECTION);

	m_ListBox.SetVisible(false);
}

void CUIComboBox::OnSelectionChanged()
{
	if(m_ListBox.IsVisible())
	{
		m_ListBox.SetVisible(false);
	}
	SendEvent(EVENT_COMBOBOX_SELECTION_CHANGED);
}

bool CUIComboBox::ContainsPoint(POINT pt) 
{ 
	return m_rcBoundingBox.ptInRect(pt) || (m_ListBox.IsVisible() && m_ListBox.ContainsPoint(pt)); 
}

void CUIComboBox::OnFocusOut()
{
	CUICombo::OnFocusOut();
	m_ListBox.SetVisible(false);
}

bool CUIComboBox::HandleKeyboard(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	const DWORD REPEAT_MASK = (0x40000000);

	if(!m_bEnabled || !m_bVisible)
		return false;

	// Let the scroll bar have a chance to handle it first
	if(CUIComboBox::HandleKeyboard(uMsg, wParam, lParam))
		return true;

	switch(uMsg)
	{
	case WM_KEYDOWN:
		{
			switch(wParam)
			{
			case VK_RETURN:
				if(m_ListBox.IsVisible())
				{
					// hh 2010-2-27 It's no idea.
					//if(m_iSelected != m_iFocused)
					//{
					//	m_iSelected = m_iFocused;
					//	SendEvent(EVENT_COMBOBOX_SELECTION_CHANGED);
					//}
					m_ListBox.SetVisible(false);
					SetFocus(false);

					return true;
				}
				break;

			case VK_F4:
				// Filter out auto-repeats
				if(lParam & REPEAT_MASK)
					return true;
				m_ListBox.SetVisible(!m_ListBox.IsVisible());
				if(!m_ListBox.IsVisible())
				{
					SendEvent(EVENT_COMBOBOX_SELECTION_CHANGED);
					SetFocus(false);
				}
				return true;
			case VK_LEFT:
			case VK_UP:
				if(!m_ListBox.IsVisible())
				{
					m_ListBox.SelectItem(m_ListBox.GetSelectedIndex()-1);
				}
				return true;
			case VK_RIGHT:
			case VK_DOWN:
				if(!m_ListBox.IsVisible())
				{
					m_ListBox.SelectItem(m_ListBox.GetSelectedIndex()+1);
				}
				return true;
			}
			break;
		}
	}
	return false;
}

void CUIComboBox::OnMouseMove(POINT point)
{
}

void CUIComboBox::OnLButtonDown(POINT point)
{
	CUICombo::OnLButtonDown(point);
	if(m_rcBoundingBox.ptInRect(point))
	{
		// Pressed while inside the control
		SetFocus();
		SetPressed(true);

		// Toggle dropdown
		if(IsFocus())
		{
			m_ListBox.SetVisible(!m_ListBox.IsVisible());

			if(!m_ListBox.IsVisible())
			{
				SetFocus(false);
			}
		}
		return;
	}
}

void CUIComboBox::OnLButtonUp(POINT point)
{
	CUICombo::OnLButtonUp(point);
	if(IsPressed() && ContainsPoint(point))
	{
		// Button click
		SetPressed(false);
	}
}

void CUIComboBox::OnMouseWheel(POINT point,short wheelDelta)
{
	int zDelta = wheelDelta / WHEEL_DELTA;
	if(m_ListBox.IsVisible())
	{
		m_ListBox.OnMouseWheel(point,wheelDelta);
	}
	else
	{
		if(zDelta > 0)
		{
			m_ListBox.SelectItem(m_ListBox.GetSelectedIndex()-1);
		}
		else
		{
			m_ListBox.SelectItem(m_ListBox.GetSelectedIndex()+1);
		}
	}
}

void CUIComboBox::OnHotkey()
{
	if(m_ListBox.IsVisible())
		return;

	if(m_ListBox.GetItemCount()==0)
		return;

	if(GetParentDialog()->IsKeyboardInputEnabled())
	{
		SetFocus();
	}

	if(m_ListBox.GetSelectedIndex()>=m_ListBox.GetItemCount()-1)
	{
		m_ListBox.SelectItem(0);
	}
	else
	{
		m_ListBox.SelectItem(m_ListBox.GetSelectedIndex()+1);
	}
}

void CUIComboBox::OnFrameRender(const Matrix& mTransform, double fTime, float fElapsedTime)
{
	CUICombo::OnFrameRender(mTransform,fTime,fElapsedTime);

	CONTROL_STATE iState = GetState();
	if(m_ListBox.IsVisible())
	{
		iState = CONTROL_STATE_PRESSED;
	}
	// Main text box
	UIListBoxItem* pItem = m_ListBox.GetSelectedItem();
	if(pItem)
	{
		m_Style.draw(mTransform,m_rcRelativeBox,pItem->wstrText.c_str(),iState, fElapsedTime);
	}
}

bool CUIComboBox::AddItem(const wchar_t* wcsText, void* pData)
{
	if (m_ListBox.AddItem(wcsText,pData))
	{
		if(m_ListBox.GetItemCount()==1)
		{
			m_ListBox.SelectItem(0);
		}
		return true;
	}
	return false;
}

const wchar_t* CUIComboBox::GetText()const
{
	const UIListBoxItem* pItem = m_ListBox.GetSelectedItem();
	if(pItem)
	{
		return pItem->wstrText.c_str();
	}
	return NULL;
}

CUIListBox& CUIComboBox::getListBox()
{
	return m_ListBox;
}
