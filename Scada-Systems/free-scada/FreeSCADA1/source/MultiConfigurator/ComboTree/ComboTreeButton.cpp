#include "StdAfx.h"
#include ".\combotreebutton.h"

CComboTreeButton::CComboTreeButton()
{
	_HotDropDown = false;
}
LRESULT CComboTreeButton::OnDrawItem(UINT /*ControlID*/, LPDRAWITEMSTRUCT lpDrawItem)
{
	CTheme theme;
	CRect rc(lpDrawItem->rcItem);

	if(theme.IsThemingSupported())
	{
		if(theme.OpenThemeData(m_hWnd, L"ComboBox"))
		{
			int nDropButtonStyle = CBXS_NORMAL;
			if(lpDrawItem->itemState & ODS_DISABLED)
				nDropButtonStyle = CBXS_DISABLED ;
			if(lpDrawItem->itemState & ODS_SELECTED)
				nDropButtonStyle = CBXS_PRESSED;

			theme.DrawThemeBackground(lpDrawItem->hDC, EP_CARET, 0, &rc);
			
			rc.DeflateRect(0,1,1,1);
			theme.DrawThemeBackground(lpDrawItem->hDC, CP_DROPDOWNBUTTON, nDropButtonStyle, &rc);

			theme.CloseThemeData();
			return 0;
		}
	}
	
    //Стандартный контрол
	UINT style = DFCS_SCROLLDOWN;
	if (lpDrawItem->itemState & ODS_SELECTED)
		style |= DFCS_PUSHED;
	if (lpDrawItem->itemState & ODS_DISABLED)
		style |= DFCS_INACTIVE;
	DrawFrameControl(lpDrawItem->hDC, &rc, DFC_SCROLL, style);

	return 0;
}
