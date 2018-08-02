#include "StdAfx.h"
#include ".\projectsettingsdialog.h"


LRESULT CProjectSettingsDialog::OnClose(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	EndDialog(IDCANCEL);
	return TRUE;
}

LRESULT CProjectSettingsDialog::OnCloseCmd(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	if(wID == IDOK)
	{
		DoDataExchange(DDX_SAVE);
		if(!m_AutoLoad)
			m_InitialSchema.Empty();
	}
	EndDialog(wID);
	return 0;
}

LRESULT CProjectSettingsDialog::OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	CenterWindow(GetParent());

	CComboBox Combo = (CComboBox)GetDlgItem(IDC_COMBO_NAMES);
	if(!m_SchemaNames.empty())
	{
		for(TStrings::iterator it= m_SchemaNames.begin();
			it != m_SchemaNames.end(); it++)
		{
			Combo.AddString(it->c_str());
		}
		if(Combo.SelectString(-1, (LPCTSTR)m_InitialSchema) >=0 )
			m_AutoLoad = 1;
	}

	DoDataExchange(DDX_LOAD);

	BOOL tmp=FALSE;
	OnBnClickedAutoLoad(NULL,NULL,NULL,tmp);
	return TRUE;
}


LRESULT CProjectSettingsDialog::OnCbnSelchange(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	CComboBox Combo = (CComboBox)GetDlgItem(IDC_COMBO_NAMES);
	Combo.GetLBText(Combo.GetCurSel(), m_InitialSchema);

	return TRUE;
}

LRESULT CProjectSettingsDialog::OnBnClickedAutoLoad(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	DoDataExchange(DDX_SAVE);

	CWindow(GetDlgItem(IDC_COMBO_NAMES)).EnableWindow(m_AutoLoad?TRUE:FALSE);
	return 0;
}
