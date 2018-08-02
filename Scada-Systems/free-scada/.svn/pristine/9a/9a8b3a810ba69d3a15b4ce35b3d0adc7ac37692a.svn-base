#pragma once
#include "resource.h"

class CInputBox
	: public CDialogImpl<CInputBox>
	, public CDialogResize<CInputBox>
{
	typedef CInputBox thisClass;
public:
	enum { IDD = IDD_INPUT_BOX };

	BEGIN_MSG_MAP(thisClass)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		COMMAND_ID_HANDLER(IDOK, OnCloseCmd)
		COMMAND_ID_HANDLER(IDCANCEL, OnCloseCmd)
		CHAIN_MSG_MAP(CDialogResize<thisClass>)
		DEFAULT_REFLECTION_HANDLER()
	END_MSG_MAP()

	BEGIN_DLGRESIZE_MAP(thisClass)
		DLGRESIZE_CONTROL(IDC_VARIABLE_NAME, DLSZ_SIZE_X)
		DLGRESIZE_CONTROL(IDC_EDIT1, DLSZ_SIZE_X|DLSZ_SIZE_Y)
	END_DLGRESIZE_MAP()

	//VariableName	- название переменной
	//Caption		- заголовок окна
	//DefaultVal	- значение по умолчанию
	CInputBox(wstring VariableName, wstring Caption = _T(""), wstring DefaultVal=_T(""))
		: _Name(VariableName)
		, _DefaultValue(DefaultVal)
	{
		if(Caption.empty())
		{
			CString str;
			CWindow(::GetActiveWindow()).GetWindowText(str);
			_Caption = str;
		}
		else
			_Caption = Caption;
	};

	LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
		DlgResize_Init();
		CenterWindow(GetParent());
		
		SetWindowText(_Caption.c_str());
		SetDlgItemText(IDC_VARIABLE_NAME, _Name.c_str());
		SetDlgItemText(IDC_EDIT1, _DefaultValue.c_str());
		return TRUE;
	}

	LRESULT OnCloseCmd(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		CString tmp;
		GetDlgItemText(IDC_EDIT1, tmp);
		_Value = tmp;

		EndDialog(wID);
		return 0;
	}

	//Получить значение как текст
	inline wstring GetValAsText(){return _Value;}

protected:
	wstring _Name;
	wstring _Caption;
	wstring _DefaultValue;
	wstring _Value;
};