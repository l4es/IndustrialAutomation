#pragma once
namespace ModuleArchive
{

class CEnterUserDlg
	: public CDialogImpl<CEnterUserDlg>
	, public CWinDataExchange<CEnterUserDlg>
{
public:
	typedef CEnterUserDlg thisClass;

	enum { IDD = IDD_ENTER_USER };

	CString m_User;
	CString m_Password;

	BEGIN_DDX_MAP(thisClass)
		DDX_TEXT(IDC_ENTER_USER, m_User)
		DDX_TEXT(IDC_ENTER_PASS, m_Password)
	END_DDX_MAP()

	BEGIN_MSG_MAP(thisClass)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		MESSAGE_HANDLER(WM_CLOSE, OnClose)
		COMMAND_ID_HANDLER(IDOK, OnCloseCmd)
		COMMAND_ID_HANDLER(IDCANCEL, OnCloseCmd)
	END_MSG_MAP()

	CEnterUserDlg(void) : m_User(""), m_Password(""){}
	LRESULT OnCloseCmd(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnClose(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
};

}
