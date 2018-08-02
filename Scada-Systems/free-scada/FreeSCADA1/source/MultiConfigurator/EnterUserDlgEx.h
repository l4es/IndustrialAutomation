#pragma once

namespace ModuleArchive
{

	class CEnterUserDlgEx
		: public CDialogImpl<CEnterUserDlgEx>
		, public CWinDataExchange<CEnterUserDlgEx>
	{
	public:
		typedef CEnterUserDlgEx thisClass;

		enum { IDD = IDD_ENTER_USER_EX };

		bool m_EnableUser;
		CString m_User;
		CString m_Pass;
		CString m_RepeatPass;

		BEGIN_DDX_MAP(thisClass)
			DDX_TEXT(IDC_USER, m_User)
			DDX_TEXT(IDC_PASS, m_Pass)
			DDX_TEXT(IDC_REPEAT_PASS, m_RepeatPass)
		END_DDX_MAP()

		BEGIN_MSG_MAP(thisClass)
			MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
			MESSAGE_HANDLER(WM_CLOSE, OnClose)
			COMMAND_ID_HANDLER(IDOK, OnCloseCmd)
			COMMAND_ID_HANDLER(IDCANCEL, OnCloseCmd)
		END_MSG_MAP()

		CEnterUserDlgEx(void) 
			: m_User("Administrator")
			, m_Pass("")
			, m_RepeatPass("")
			, m_EnableUser(true){}
		LRESULT OnCloseCmd(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
		LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
		LRESULT OnClose(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	};
}
