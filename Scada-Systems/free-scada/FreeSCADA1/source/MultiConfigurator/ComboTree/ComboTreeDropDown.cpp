#include "StdAfx.h"
#include ".\combotreedropdown.h"
#include "ComboTreeCtrl.h"

LRESULT CComboTreeDropDown::OnLButtonDblClk(UINT /*Flags*/, WTL::CPoint /*Pt*/)
{
	if(m_pParent)
		m_pParent->OnSelection();
	return 0;
}
