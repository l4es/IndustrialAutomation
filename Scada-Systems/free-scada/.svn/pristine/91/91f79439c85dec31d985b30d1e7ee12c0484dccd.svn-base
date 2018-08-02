#include "StdAfx.h"
#include <atlmisc.h>
#include ".\combotreectrl.h"


void CComboTreeCtrl::SubclassDlgItemID(UINT nID, HWND hParent)
{
	CComboBox combo = CWindow(hParent).GetDlgItem(nID);
	if(combo == NULL)
		return;

	CRect DroppedRect;
	combo.GetDroppedControlRect(&DroppedRect);

	CRect rect;
	combo.GetWindowRect (&rect);
	CWindow(hParent).ScreenToClient (&rect);

	if (!Create (rect, DroppedRect.Width(), DroppedRect.Height(), hParent, nID))
		return;

	//Set Z Order to follow after original combo
	SetWindowPos (combo, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE | SWP_SHOWWINDOW);

	combo.DestroyWindow();
}

BOOL CComboTreeCtrl::Create( RECT& rect, int DroppedWidth, int DroppedHeight, HWND hParent, UINT nID )
{
	ATLASSERT(hParent != NULL);

	m_ComboWidth = DroppedWidth;
	m_ComboHeight = DroppedHeight;
	m_IsDropped	= FALSE;

	CTheme theme;
	if(theme.IsThemingSupported())
	{
		if(theme.OpenThemeData(hParent, L"ComboBox"))
		{
			if (baseClass::Create(hParent, rect, NULL, WS_CHILD | WS_VISIBLE | WS_TABSTOP, NULL, nID) == NULL)
			{
				theme.CloseThemeData();
				return FALSE;
			}
			theme.CloseThemeData();
			UpdateWindow();
			return TRUE;
		}
	}
	
	if (baseClass::Create(hParent, rect, NULL, WS_CHILD | WS_VISIBLE | WS_TABSTOP, WS_EX_CLIENTEDGE, nID) == NULL)
		return FALSE;

	UpdateWindow();
	return TRUE;
}

LRESULT CComboTreeCtrl::OnCreate(LPCREATESTRUCT /*lpCreateStruct*/)
{
	LRESULT res = DefWindowProc();
	if(res) return res;

	if(!CreateButton())
		return -1;
	if(!CreateEdit())
		return -1;
	if(!CreateTree())
		return -1;
	return 0;
}

BOOL CComboTreeCtrl::CreateButton()
{
	CRect btnRect;
	GetClientRect (&btnRect);

	int width = GetSystemMetrics (SM_CXVSCROLL);
	btnRect.left = 	btnRect.right - width - 1;

	m_DropDownButton.Create(m_hWnd, btnRect, NULL, WS_TABSTOP | BS_PUSHBUTTON | BS_NOTIFY | WS_VISIBLE | WS_CHILD | BS_OWNERDRAW,
							NULL, IDC_COMBOTREE_BUTTON);

	m_DropDownButton.MoveWindow (&btnRect);

	return TRUE;
}

BOOL CComboTreeCtrl::CreateEdit()
{
	CRect clientRect;
	GetClientRect (&clientRect);

	CRect btnRect;
	m_DropDownButton.GetWindowRect (&btnRect);
	ScreenToClient (&btnRect);

	CRect editRect;
	
	editRect.left = clientRect.left;
	editRect.top = clientRect.top+2;
	editRect.bottom = clientRect.bottom;
	editRect.right = btnRect.left;
	CTheme theme;
	if(theme.IsThemingSupported())
	{
		if(theme.OpenThemeData(GetParent(), L"ComboBox"))
		{
			editRect.DeflateRect(1,1);
			theme.CloseThemeData();
		}
	}

	if(m_Edit.Create(m_hWnd, editRect, NULL, 
		WS_CHILD |  WS_TABSTOP | WS_VISIBLE | ES_AUTOHSCROLL, NULL, IDC_COMBOTREE_EDIT) == NULL)
	{
		return FALSE;
	}

	m_Edit.SetFont(CWindow(GetParent()).GetFont());
	m_Edit.SetMargins(2,2);
	return TRUE;
}

LRESULT CComboTreeCtrl::OnDrawItem(UINT ControlID, LPDRAWITEMSTRUCT lpDrawItem)
{
	if(ControlID == IDC_COMBOTREE_BUTTON)
		return m_DropDownButton.SendMessage(WM_DRAWITEM, ControlID, (LPARAM)lpDrawItem);
	return 0;
}

LRESULT CComboTreeCtrl::OnPaint(HDC /*dc_*/)
{	
	CTheme theme;
	if(theme.IsThemingSupported())
	{
		if(theme.OpenThemeData(m_hWnd, L"ComboBox"))
		{
			CPaintDC dc(m_hWnd);
			CRect rc;
			GetClientRect(&rc);
			theme.DrawThemeBackground(dc, EP_CARET, 0, &rc);
			theme.CloseThemeData();
			return 0;
		}
	}

	SetMsgHandled(FALSE);
	return 0;
}

BOOL CComboTreeCtrl::CreateTree()
{
	CRect treeRect (0,0, m_ComboWidth, m_ComboHeight);

	const DWORD Style = WS_CHILD | WS_BORDER | /*TVS_SINGLEEXPAND |*/ TVS_HASLINES | TVS_LINESATROOT | TVS_HASBUTTONS | TVS_NOTOOLTIPS;
	const DWORD ExStyle = WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_TOOLWINDOW;

	if(m_Tree.Create(GetDesktopWindow(), treeRect, NULL, Style, ExStyle, IDC_COMBOTREE_DROPDOWN) 
		== NULL)
	{
		return FALSE;
	}

	m_Tree.m_pParent = this;
	return TRUE;
}
LRESULT CComboTreeCtrl::OnDestroy(void)
{
	if(m_Edit.IsWindow())
		m_Edit.DestroyWindow();
	if(m_DropDownButton.IsWindow())
		m_DropDownButton.DestroyWindow();
	if(m_Tree.IsWindow())
		m_Tree.DestroyWindow();
	return 0;
}

LRESULT CComboTreeCtrl::OnKey(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
{
	if(GetDroppedState())
	{
		BOOL bContinue;
		bContinue = OnKeyDropdownActive(uMsg, (UINT)wParam);
		if (bContinue)
			bContinue = m_Tree.SendMessage( uMsg, wParam, lParam)?TRUE:FALSE;
	}
	else
		OnKeyDropdownInactive(uMsg, (UINT)wParam);
	return 0;
}

BOOL CComboTreeCtrl::OnKeyDropdownActive(UINT message, UINT nChar) 
{
	if(!GetDroppedState())
	{
		return FALSE;
	}

	if (message == WM_SYSKEYDOWN)
	{
		if ((nChar == VK_UP))
		{
			OnSelection();
			return FALSE;
		}
	}
	else if ((message == WM_KEYDOWN) && (nChar == VK_F4))
	{
		OnSelection();
		return FALSE;
	}
	else if (nChar == VK_RETURN)
	{
		OnSelection();
		return FALSE;
	}
	else if (nChar == VK_ESCAPE)
	{
		OnCancel();
		return FALSE;
	}
	else if ((message == WM_KEYDOWN) && (nChar == VK_TAB))
	{
		OnSelection();

		BOOL bShift = (GetKeyState(VK_SHIFT) < 0);

		CWindow pComboParent = GetParent();
		if (pComboParent != NULL)
		{
			CWindow pNext = pComboParent.GetNextDlgTabItem(m_hWnd, bShift);
			if (pNext != NULL)
				pNext.SetFocus();
		}
		return FALSE;
	}

	return TRUE;
}

BOOL CComboTreeCtrl::OnKeyDropdownInactive(UINT message, UINT nChar) 
{
	if(GetDroppedState())
	{
		return FALSE;
	}

	if ((message == WM_SYSKEYDOWN) )
	{
		if ((nChar == VK_DOWN))
		{
			ShowTree ();
			return FALSE;
		}
	}
	else if ((message == WM_KEYDOWN) && (nChar == VK_F4))
	{
		ShowTree ();
		return FALSE;
	}
	else if ((message == WM_KEYDOWN) && (nChar == VK_TAB))
	{

		BOOL bShift = (GetKeyState(VK_SHIFT) < 0);

		CWindow pComboParent = GetParent ();
		if (pComboParent != NULL)
		{
			CWindow pNext = pComboParent.GetNextDlgTabItem (m_hWnd, bShift);
			if (pNext != NULL)
				pNext.SetFocus ();
		}
		return FALSE;
	}

	return TRUE;
}

void CComboTreeCtrl::OnSelection()
{
	if(m_Tree != NULL)
	{
		HTREEITEM hItem = m_Tree.GetSelectedItem ();
		if (hItem)
		{
			HTREEITEM tmp = hItem;
			CString Text;
			while(tmp != NULL)
			{
				CString ItemText;
				m_Tree.GetItemText(tmp, ItemText);
				Text = ItemText + "." + Text;

				tmp = m_Tree.GetParentItem(tmp);
			}
			
			Text.Trim(L".");
			m_Edit.SetWindowText(Text);
			m_Edit.SetSel(Text.GetLength(),Text.GetLength());
			SendParentComboMessage(CBN_SELENDOK);
			Invalidate();
		}
	}
	HideTree();
}

void CComboTreeCtrl::OnCancel()
{
	HideTree();
}

void CComboTreeCtrl::ShowTree()
{
	//Must set focus to edit here or dropdown will still have it and act on
	//arrow key messages

	m_Edit.SetFocus();	
	DisplayTree ();
	SendParentComboMessage (CBN_DROPDOWN);

	//Must call this to intercept parent window messsages
//	m_ComboTreeHook.HookParent();


	m_Edit.Invalidate ();
	m_DropDownButton.Invalidate();
	Invalidate();
}

void CComboTreeCtrl::HideTree()
{
	SetDroppedState (FALSE);
	m_Tree.ShowWindow( SW_HIDE );

	//Tree is no longer displayed, stop message interception
//	m_ComboTreeHook.UnhookParent();

	m_Edit.Invalidate ();
	m_DropDownButton.Invalidate();
	Invalidate();
}

void CComboTreeCtrl::DisplayTree()
{
	CRect rect(0,0,200,200);

	CalculateDroppedRect (&rect);


	m_Tree.SetWindowPos (HWND_TOPMOST, rect.left, rect.top,
		rect.Width (), rect.Height (), SWP_SHOWWINDOW );

//	m_BeginPath = m_Tree.GetCurrentTreePath ();
	CString Path;
	m_Edit.GetWindowText(Path);
	CString tmp;
	HTREEITEM item = TVI_ROOT;
	if(m_Tree.GetNextItem(item, TVGN_CHILD))
		m_Tree.SelectItem(m_Tree.GetNextItem(item, TVGN_CHILD));
	for(int pos = 0;pos<Path.GetLength();pos++)
	{
		if(Path[pos] != '.')
			tmp += Path[pos];
		
		if(Path[pos] == '.' || pos == Path.GetLength()-1)
		{
			HTREEITEM tmpItem;
			for(tmpItem = m_Tree.GetNextItem(item, TVGN_CHILD);tmpItem != NULL;
				tmpItem = m_Tree.GetNextItem(tmpItem, TVGN_NEXT))
			{
				CString s;
				m_Tree.GetItemText(tmpItem, s);
				if(s == tmp)
				{
					tmp.Empty();
					item = tmpItem;
					break;
				}
			}
			if(item == tmpItem)
			{
				m_Tree.SelectItem(item);
			}
			else
			{
				break;
			}
		}
	}

	SetDroppedState (TRUE);


	m_Tree.PostMessage(WM_SETFOCUS);
	m_Tree.SetActiveWindow();
}

LRESULT CComboTreeCtrl::SendParentComboMessage (UINT Action)
{
	CWindow pParent = GetParent();
	if (pParent != NULL)
		return pParent.SendMessage (WM_COMMAND, MAKEWPARAM( GetDlgCtrlID(), Action),(LPARAM) m_hWnd);

	return 0L;
}

void CComboTreeCtrl::CalculateDroppedRect(LPRECT lpDroppedRect)
{
	_ASSERTE (lpDroppedRect);

	if (!lpDroppedRect)
		return;

	CRect rectCombo;
	GetWindowRect(&rectCombo);
	m_ComboWidth = rectCombo.Width();

	//adjust to either the top or bottom
	int DropTop = rectCombo.bottom;
	int ScreenHeight = GetSystemMetrics (SM_CYSCREEN);
	if ((DropTop + m_ComboHeight) > ScreenHeight)
	{
		DropTop = rectCombo.top - m_ComboHeight;
	}

	//adjust to either the right or left
	int DropLeft = rectCombo.left;
	int ScreenWidth = GetSystemMetrics (SM_CXSCREEN);
	if ((DropLeft + m_ComboWidth) > ScreenWidth)
	{
		DropLeft = rectCombo.right - m_ComboWidth;
	}

	lpDroppedRect->left  = DropLeft;
	lpDroppedRect->top   = DropTop;
	lpDroppedRect->bottom = DropTop + m_ComboHeight;
	lpDroppedRect->right  = DropLeft + m_ComboWidth;

}

LRESULT CComboTreeCtrl::OnCombotreeButtonBnClicked(UINT /*wNotifyCode*/, int /*wID*/, HWND /*hWndCtl*/)
{
	if (GetDroppedState())
		OnSelection ();
	else
		ShowTree();
	return 0;
}

LRESULT CComboTreeCtrl::OnSetFocus(HWND /*hwndOldFocus*/)
{
	DefWindowProc();

	if (::GetFocus() != m_DropDownButton)
	{
		m_Edit.SetFocus();
		m_Edit.Invalidate ();
	}

	return 0;
}

LRESULT CComboTreeCtrl::OnKillFocus(HWND /*hwndNewFocus*/)
{
	DefWindowProc();
	m_Edit.Invalidate();
	
	if(GetDroppedState())
	{
		OnCancel();
		//перед нажатием кнопки(TreeButton) контрол теряеет фокус
		//что приводит к скрытию дерева и повторному отображению при нажатии кнопки
		//т.е. при нажатии кнопки дереве всегда скрыто.
		// С помощью таймера изменение состояния дерева немного откладывается
		m_IsDropped = TRUE;
		SetTimer(1, 400);
	}
	return 0;
}

void CComboTreeCtrl::SetValue(CString Val)
{
	m_Edit.SetWindowText(Val);
}

CString CComboTreeCtrl::GetValue(void)
{
	CString str;
	m_Edit.GetWindowText(str);
	return str;
}

void CComboTreeCtrl::RefreshTreePos()
{
	if(GetDroppedState())
	{
		CRect rect(0,0,200,200);
		CalculateDroppedRect (&rect);

		m_Tree.SetWindowPos (HWND_TOPMOST, rect.left, rect.top,
			rect.Width (), rect.Height (), SWP_SHOWWINDOW );
	}
}

LRESULT CComboTreeCtrl::OnSize(UINT /*state*/, CSize /*Size*/)
{
	if(GetDroppedState())
		OnCancel();

	//Передвигаем кнопку
	CRect btnRect;
	GetClientRect (&btnRect);

	int width = GetSystemMetrics (SM_CXVSCROLL);
	btnRect.left = 	btnRect.right - width - 1;

	m_DropDownButton.MoveWindow (&btnRect);

	//Передвигаем edit
	CRect clientRect;
	GetClientRect (&clientRect);

	m_DropDownButton.GetWindowRect (&btnRect);
	ScreenToClient (&btnRect);

	CRect editRect;

	editRect.left = clientRect.left;
	editRect.top = clientRect.top+2;
	editRect.bottom = clientRect.bottom;
	editRect.right = btnRect.left;
	CTheme theme;
	if(theme.IsThemingSupported())
	{
		if(theme.OpenThemeData(GetParent(), L"ComboBox"))
		{
			editRect.DeflateRect(1,1);
			theme.CloseThemeData();
		}
	}

	m_Edit.MoveWindow(editRect);

	SetMsgHandled(FALSE);
	return 0;
}

LRESULT CComboTreeCtrl::OnMove(CPoint /*Pt*/)
{
	RefreshTreePos();
	return 0;
}

LRESULT CComboTreeCtrl::OnTimer(UINT id, TIMERPROC /*lpTimerProc*/)
{
	if(id == 1)
	{
		OnCancel();
		KillTimer(1);
	}
	return 0;
}
