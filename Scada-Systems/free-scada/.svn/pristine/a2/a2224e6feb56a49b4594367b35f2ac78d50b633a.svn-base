#pragma once
#include <atlmisc.h>
#include <atlcrack.h>

class CComboTreeButton
	:public CWindowImpl<CComboTreeButton, CButton>
{
	typedef CComboTreeButton thisClass;
	typedef CWindowImpl<CComboTreeButton, CButton> baseClass;

public:
	CComboTreeButton();

	DECLARE_WND_SUPERCLASS(L"ComboButton", baseClass::GetWndClassName())

	BEGIN_MSG_MAP(thisClass)
		MSG_WM_DRAWITEM(OnDrawItem)
	END_MSG_MAP()
	LRESULT OnDrawItem(UINT ControlID, LPDRAWITEMSTRUCT lpDrawItem);
	LRESULT OnPaint(HDC dc);

protected:
	bool _HotDropDown;
};
