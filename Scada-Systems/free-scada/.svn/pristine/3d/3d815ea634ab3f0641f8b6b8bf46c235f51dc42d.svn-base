#include "StdAfx.h"
#include ".\simplevariabledialog.h"
#include "VariableDialog.h"

LRESULT CSimpleVariableDialog::OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	DlgResize_Init(true, true, NULL);
	DoDataExchange(DDX_LOAD);
	CenterWindow(GetParent());

	CComboBox Type = (CComboBox)GetDlgItem(IDC_TYPE_VARIABLE);

	Type.AddString(L"Boolean");
	Type.AddString(L"Integer");
	Type.AddString(L"Float");

	Type.SelectString(-1, m_Type);

	((CButton)GetDlgItem(IDOK)).EnableWindow(IsSetAll());

	return TRUE;
}


LRESULT CSimpleVariableDialog::OnClose(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	EndDialog(IDCANCEL);
	return TRUE;
}

LRESULT CSimpleVariableDialog::OnCloseCmd(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	DoDataExchange(DDX_SAVE);
	EndDialog(wID);
	return 0;
}

LRESULT CSimpleVariableDialog::OnCbnSelchange(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	CComboBox Combo = (CComboBox)GetDlgItem(IDC_TYPE_VARIABLE);
	int i = Combo.GetCurSel();
	if(i == -1) return 0;
	wchar_t buffer[0xff];
	Combo.GetLBText(i, buffer);
	wstring Type(buffer);
	m_Type = Type.c_str();

	((CButton)GetDlgItem(IDOK)).EnableWindow(IsSetAll());

	return 0;
}

LRESULT CSimpleVariableDialog::OnEnChange(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	((CButton)GetDlgItem(IDOK)).EnableWindow(IsSetAll());
	return 0;
}

//ѕроверка, все ли параметры установлены
bool CSimpleVariableDialog::IsSetAll()
{
//	DoDataExchange(TRUE);
	wchar_t bName[0xff], bValue[0xff];
	GetDlgItemText(IDC_NAME_SIMPLE_VARIABLE, bName, 0xff);
	GetDlgItemText(IDC_VALUE_SIMPLE_VARIABLE, bValue, 0xff);
	int Selected = ((CComboBox)GetDlgItem(IDC_TYPE_VARIABLE)).GetCurSel();
	wstring sName(bName), sValue(bValue);

	return((Selected != -1)  && (!sValue.empty()) && (!sName.empty()));
}

