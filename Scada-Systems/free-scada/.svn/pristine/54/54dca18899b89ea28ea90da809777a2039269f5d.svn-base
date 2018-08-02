#pragma once

namespace ModuleArchive
{
	class CModuleArchive;
	class CUsersSettingsView
		: public CDialogImpl<CUsersSettingsView>
		, public CDialogResize<CUsersSettingsView>
		, public CWinDataExchange<CUsersSettingsView>
	{
	public:
		enum { IDD = IDD_USERS_SETTINGS};

		CModuleArchive* m_Module;

		CUsersSettingsView() 
			: m_Module(NULL)
		{}
		
		CString m_FirstName;
		CString m_MiddleName;
		CString m_LastName;

		CString m_CurrentLogin;

		ULONG m_Permission;
		int m_CanEditUsers;
		int m_CanViewUsers;
		int m_CanEditData;
		int m_CanViewData;
		int m_CanEditEvents;
		int m_CanViewEvents;
		int m_CanEditChannels;

		BEGIN_DDX_MAP(CUsersSettingsView)
			DDX_TEXT(IDC_FIRST_NAME, m_FirstName)
			DDX_TEXT(IDC_MIDDLE_NAME, m_MiddleName)
			DDX_TEXT(IDC_LAST_NAME, m_LastName)

			DDX_CHECK(IDC_CANEDITUSER, m_CanEditUsers)
			DDX_CHECK(IDC_CANVIEWUSER, m_CanViewUsers)
			DDX_CHECK(IDC_CANEDITDATA, m_CanEditData)
			DDX_CHECK(IDC_CANVIEWDATA, m_CanViewData)
			DDX_CHECK(IDC_CANEDITEVENTS, m_CanEditEvents)
			DDX_CHECK(IDC_CANVIEWEVENTS, m_CanViewEvents)
			DDX_CHECK(IDC_CANEDITCHANNELS, m_CanEditChannels)
		END_DDX_MAP()

		BEGIN_MSG_MAP(CUsersSettingsView)
			MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
			COMMAND_ID_HANDLER(IDC_ADD_USER, OnAddUser)
			COMMAND_ID_HANDLER(IDC_DEL_USER, OnDelUser)
			COMMAND_ID_HANDLER(IDC_CHANGE_USER, OnChangeUser)
			COMMAND_ID_HANDLER(IDC_SET_PASSWORD, OnSetPassword)

			COMMAND_ID_HANDLER(IDC_USER_PERMISSION, OnSetUserPermission)
			COMMAND_ID_HANDLER(IDC_ADMIN_PERMISSION, OnSetAdminPermission)

			NOTIFY_HANDLER(IDC_USERS, LVN_ITEMCHANGED, OnItemChanged)
			CHAIN_MSG_MAP(CDialogResize<CUsersSettingsView>)
		END_MSG_MAP()


		BEGIN_DLGRESIZE_MAP(CUsersSettingsView)
			DLGRESIZE_CONTROL(IDC_USERS, DLSZ_SIZE_X | DLSZ_SIZE_Y)
			DLGRESIZE_CONTROL(IDC_STATIC1, DLSZ_MOVE_X) 
			DLGRESIZE_CONTROL(IDC_STATIC2, DLSZ_MOVE_X)
			DLGRESIZE_CONTROL(IDC_STATIC3, DLSZ_MOVE_X)
			DLGRESIZE_CONTROL(IDC_STATIC4, DLSZ_MOVE_X)
			DLGRESIZE_CONTROL(IDC_STATIC5, DLSZ_MOVE_X)
			DLGRESIZE_CONTROL(IDC_STATIC7, DLSZ_MOVE_X)

			DLGRESIZE_CONTROL(IDC_FIRST_NAME, DLSZ_MOVE_X)
			DLGRESIZE_CONTROL(IDC_MIDDLE_NAME, DLSZ_MOVE_X)
			DLGRESIZE_CONTROL(IDC_LAST_NAME, DLSZ_MOVE_X)

			DLGRESIZE_CONTROL(IDC_SET_PASSWORD, DLSZ_MOVE_X)
			
			DLGRESIZE_CONTROL(IDC_CANEDITUSER, DLSZ_MOVE_X)
			DLGRESIZE_CONTROL(IDC_CANVIEWUSER, DLSZ_MOVE_X)
			DLGRESIZE_CONTROL(IDC_CANEDITDATA, DLSZ_MOVE_X)
			DLGRESIZE_CONTROL(IDC_CANVIEWDATA, DLSZ_MOVE_X)
			DLGRESIZE_CONTROL(IDC_CANEDITEVENTS, DLSZ_MOVE_X)
			DLGRESIZE_CONTROL(IDC_CANVIEWEVENTS, DLSZ_MOVE_X)
			DLGRESIZE_CONTROL(IDC_CANEDITCHANNELS, DLSZ_MOVE_X)

			DLGRESIZE_CONTROL(IDC_ADD_USER, DLSZ_MOVE_X)
			DLGRESIZE_CONTROL(IDC_DEL_USER, DLSZ_MOVE_X)
			DLGRESIZE_CONTROL(IDC_CHANGE_USER, DLSZ_MOVE_X)
			DLGRESIZE_CONTROL(IDC_USER_PERMISSION, DLSZ_MOVE_X)
			DLGRESIZE_CONTROL(IDC_ADMIN_PERMISSION, DLSZ_MOVE_X)
		END_DLGRESIZE_MAP()

		BOOL PreTranslateMessage(MSG* pMsg);
		LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
		LRESULT OnAddUser(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
		LRESULT OnDelUser(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
		LRESULT OnChangeUser(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
		LRESULT OnSetPassword(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
		LRESULT OnItemChanged(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/);
		LRESULT OnSetUserPermission(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
		LRESULT OnSetAdminPermission(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
		void FillUsers();
		void ReadPermission();
		void Reset();
	};

}
