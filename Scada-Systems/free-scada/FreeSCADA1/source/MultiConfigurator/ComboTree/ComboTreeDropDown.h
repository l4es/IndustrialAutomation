#pragma once

class CComboTreeCtrl;

class CComboTreeDropDown
	:public CWindowImpl<CComboTreeDropDown, CTreeViewCtrl>
{
	typedef CComboTreeDropDown thisClass;
	typedef CWindowImpl<CComboTreeDropDown, CTreeViewCtrl> baseClass;

public:
	DECLARE_WND_SUPERCLASS(L"ComboTreeDropDown", baseClass::GetWndClassName())

	BEGIN_MSG_MAP(thisClass)
		MSG_WM_LBUTTONDBLCLK(OnLButtonDblClk)
	END_MSG_MAP()
	LRESULT OnLButtonDblClk(UINT Flags, CPoint Pt);

	CComboTreeCtrl* m_pParent;
};
