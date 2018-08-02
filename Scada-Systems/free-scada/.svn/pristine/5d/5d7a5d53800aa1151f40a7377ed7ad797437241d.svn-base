#include "StdAfx.h"
#include ".\graphicparamdlg.h"

LRESULT CGraphicParamDlg::OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	DoDataExchange(DDX_LOAD);

	CenterWindow(GetParent());
	CComboBox cb = (CComboBox)GetDlgItem(IDC_COMBO_METHOD);
	cb.AddString(L"Precise");
	cb.AddString(L"Approximate");
	cb.AddString(L"Linear");
	cb.SetCurSel(m_Method);

	CUpDownCtrl spin = GetDlgItem(IDC_SPIN1);
	spin.SetRange(2,1000);
	spin.SetPos(m_Points);

	spin = GetDlgItem(IDC_SPIN2);
	spin.SetRange(2, 10);
	spin.SetPos(m_Degree);
	return TRUE;
}

LRESULT CGraphicParamDlg::OnCloseCmd(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	DoDataExchange(DDX_SAVE);
	EndDialog(wID);
	return 0;
}
LRESULT CGraphicParamDlg::OnCbnSelchangeComboMethod(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	CComboBox cb = (CComboBox)GetDlgItem(IDC_COMBO_METHOD);
	m_Method = cb.GetCurSel();
	return 0;
}
