#include "StdAfx.h"
#include ".\deleteschemadialog.h"

LRESULT CDeleteSchemaDialog::OnClose(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	EndDialog(IDCANCEL);
	return TRUE;
}

LRESULT CDeleteSchemaDialog::OnCloseCmd(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	EndDialog(wID);
	return 0;
}

LRESULT CDeleteSchemaDialog::OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	CenterWindow(GetParent());

	CComboBox Combo = (CComboBox)GetDlgItem(IDC_COMBO_NAMES);
	if(!m_SchemaNames.empty())
	{
		//Заполнение комбо-бокса названиями схем
		for(TStrings::iterator it= m_SchemaNames.begin();
			it != m_SchemaNames.end(); it++)
		{
			Combo.AddString(it->c_str());
		}
		Combo.SelectString(-1, (LPCTSTR)m_Schema);

	}
	return TRUE;
}


LRESULT CDeleteSchemaDialog::OnCbnSelchange(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	CComboBox Combo = (CComboBox)GetDlgItem(IDC_COMBO_NAMES);
	Combo.GetLBText(Combo.GetCurSel(), m_Schema);

	return TRUE;
}

