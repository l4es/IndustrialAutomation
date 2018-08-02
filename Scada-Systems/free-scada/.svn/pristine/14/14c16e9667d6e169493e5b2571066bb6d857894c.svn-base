#pragma once
#include <atlcrack.h>

class CComboTreeEdit
	:public CWindowImpl<CComboTreeEdit, CEdit>
{
	typedef CComboTreeEdit thisClass;
	typedef CWindowImpl<CComboTreeEdit, CEdit> baseClass;

public:
	DECLARE_WND_SUPERCLASS(L"ComboEdit", baseClass::GetWndClassName())

	BEGIN_MSG_MAP(thisClass)
		MESSAGE_HANDLER(WM_KEYDOWN, OnKeyDown)
		MESSAGE_HANDLER(WM_KILLFOCUS, OnKillFocus)
		
	END_MSG_MAP()
	LRESULT OnKeyDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnKillFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
};
