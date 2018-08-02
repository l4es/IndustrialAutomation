#pragma once

namespace ModuleArchive
{
	class CModuleArchive;
	class CDBSettingsView
		: public CDialogImpl<CDBSettingsView>
		, public CDialogResize<CDBSettingsView>
		, public CWinDataExchange<CDBSettingsView>
	{
	public:
		enum { IDD = IDD_DB_SETTINGS};


		CModuleArchive* m_Module;

		int m_Access;
		int m_MySQL;
		CString m_NameAccess;
		CString m_NameMySQL;
		CString m_PathAccess;
		CString m_ServerMySQL;


		CDBSettingsView() 
			: m_Module(NULL)
		{}

		BEGIN_DDX_MAP(CDBSettingsView)
			DDX_CHECK(IDC_RADIO_ACCESS, m_Access)
			DDX_CHECK(IDC_RADIO_MYSQL, m_MySQL)

			DDX_TEXT(IDC_NAME_ACCESS, m_NameAccess)
			DDX_TEXT(IDC_NAME_MYSQL, m_NameMySQL)
			DDX_TEXT(IDC_PATH_ACCESS, m_PathAccess)
			DDX_TEXT(IDC_SERVER_MYSQL, m_ServerMySQL)
		END_DDX_MAP()

		BEGIN_MSG_MAP(CDBSettingsView)
			MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
			COMMAND_RANGE_CODE_HANDLER(IDC_RADIO_ACCESS, IDC_RADIO_MYSQL, BN_CLICKED, OnSelRadio)
			COMMAND_ID_HANDLER(IDC_CREATE_ACCESS, OnCreateAccess)
			COMMAND_ID_HANDLER(IDC_CREATE_MYSQL, OnCreateMySQL)
			COMMAND_ID_HANDLER(IDC_SET_USER, OnSetUser)
			CHAIN_MSG_MAP(CDialogResize<CDBSettingsView>)
		END_MSG_MAP()


		BEGIN_DLGRESIZE_MAP(CDBSettingsView)
		END_DLGRESIZE_MAP()

		// Handler prototypes (uncomment arguments if needed):
		//	LRESULT MessageHandler(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
		//	LRESULT CommandHandler(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
		//	LRESULT NotifyHandler(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/)

		BOOL PreTranslateMessage(MSG* pMsg);
		LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
		LRESULT OnSelRadio(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
		LRESULT OnCreateAccess(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
		LRESULT OnCreateMySQL(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
		LRESULT OnSetUser(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
		
		void UpdateEnable();
	};

}
