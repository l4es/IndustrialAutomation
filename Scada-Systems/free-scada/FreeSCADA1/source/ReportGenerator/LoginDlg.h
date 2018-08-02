#pragma once
#include <atlcrack.h>

class CLoginDlg
	: public CDialogImpl<CLoginDlg>
	, public CWinDataExchange<CLoginDlg>
{
public:
	enum { IDD = IDD_LOGIN };

	BEGIN_MSG_MAP(CLoginDlg)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		COMMAND_ID_HANDLER(IDOK, OnCloseCmd)
		COMMAND_ID_HANDLER(IDCANCEL, OnCloseCmd)
		COMMAND_HANDLER_EX(IDC_BUTTON2, BN_CLICKED, OnButtonBrowse)
	END_MSG_MAP()

	CString m_Server;
	CString m_User;
	CString m_Password;

	BEGIN_DDX_MAP(CLoginDlg)
		DDX_TEXT(IDC_SERVER, m_Server)
		DDX_TEXT(IDC_USER, m_User)
		DDX_TEXT(IDC_PASSWORD, m_Password)
	END_DDX_MAP()

	LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnCloseCmd(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/);

	BOOL IsValid();
	IDBConnectionPtr GetConnection();
	LRESULT OnButtonBrowse(WORD wNotifyCode, WORD wID, HWND hWndCtl);
};
