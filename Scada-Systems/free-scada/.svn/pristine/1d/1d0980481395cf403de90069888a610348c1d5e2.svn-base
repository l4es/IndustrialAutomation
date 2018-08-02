#include "StdAfx.h"
#include ".\combotreeedit.h"

LRESULT CComboTreeEdit::OnKeyDown(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	TCHAR vk = (TCHAR)wParam;

	if(vk == VK_F4 || vk == VK_TAB)
	{
		GetParent().SendMessage(WM_KEYDOWN, wParam, lParam);
		bHandled = TRUE;
		return 0;
	}
	bHandled = FALSE;
	return 0;
}

LRESULT CComboTreeEdit::OnKillFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	DefWindowProc();

	bHandled = TRUE;
	HWND hwndNewFocus = (HWND)wParam;

	if(hwndNewFocus != GetParent())
		GetParent().SendMessage(uMsg, wParam, lParam);

	return 0;
}
