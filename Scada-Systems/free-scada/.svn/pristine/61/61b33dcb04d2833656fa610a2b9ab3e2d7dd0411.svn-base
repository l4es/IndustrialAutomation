#pragma once


namespace ModuleArchive
{
	class CModuleArchive;
	class CChannelSettingsView
		: public CDialogImpl<CChannelSettingsView>
		, public CDialogResize<CChannelSettingsView>
		, public CWinDataExchange<CChannelSettingsView>
	{

	struct sAtribute 
	{
		CString Name;
		CComVariant Value;
	};

	typedef map<UINT, sAtribute> TAtribMap;


	public:
		enum { IDD = IDD_CHANNEL_SETTINGS};

		CModuleArchive* m_Module;

		int m_ItemIndex;
		int m_State;

		ULONG m_CurrentID;

		TAtribMap m_Atributes;
		BOOL m_LockUpdate;

		CString m_Name;

		BEGIN_DDX_MAP(CChannelSettingsView)
			DDX_FLOAT(IDC_UPDATE_INTERVAL, m_Atributes[IDC_UPDATE_INTERVAL].Value.fltVal)
			DDX_CHECK(IDC_EVENTS_CHANGE, m_Atributes[IDC_EVENTS_CHANGE].Value.intVal)
			DDX_CHECK(IDC_EVENTS_CROSS_VAL1, m_Atributes[IDC_EVENTS_CROSS_VAL1].Value.intVal)
			DDX_CHECK(IDC_EVENTS_CROSS_VAL2, m_Atributes[IDC_EVENTS_CROSS_VAL2].Value.intVal)
			DDX_FLOAT(IDC_EVENTS_CROSS_VAL1_SET, m_Atributes[IDC_EVENTS_CROSS_VAL1_SET].Value.fltVal)
			DDX_FLOAT(IDC_EVENTS_CROSS_VAL2_SET, m_Atributes[IDC_EVENTS_CROSS_VAL2_SET].Value.fltVal)
			DDX_CHECK(IDC_ARCH_ON_LESS, m_Atributes[IDC_ARCH_ON_LESS].Value.intVal)
			DDX_CHECK(IDC_ARCH_ON_GREATER, m_Atributes[IDC_ARCH_ON_GREATER].Value.intVal)
			DDX_CHECK(IDC_ARCH_ON_EQUAL, m_Atributes[IDC_ARCH_ON_EQUAL].Value.intVal)
			DDX_FLOAT(IDC_ARCH_ON_LESS_SET, m_Atributes[IDC_ARCH_ON_LESS_SET].Value.fltVal)
			DDX_FLOAT(IDC_ARCH_ON_GREATER_SET, m_Atributes[IDC_ARCH_ON_GREATER_SET].Value.fltVal)
			DDX_FLOAT(IDC_ARCH_ON_EQUAL_SET, m_Atributes[IDC_ARCH_ON_EQUAL_SET].Value.fltVal)
			DDX_TEXT(IDC_NAME, m_Name)
		END_DDX_MAP()

		BEGIN_MSG_MAP(CChannelSettingsView)
			MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
			COMMAND_RANGE_CODE_HANDLER(IDC_EVENTS_CHANGE, IDC_ARCH_ON_GREATER, BN_CLICKED, OnClickCheck)
			COMMAND_HANDLER(IDC_ARCH_ON_EQUAL, BN_CLICKED, OnClickCheck)
			NOTIFY_HANDLER(IDC_CHANNELS, LVN_ITEMCHANGED, OnItemChanged)
			MSG_WM_SHOWWINDOW(OnShowWindow)
			MSG_WM_DESTROY(OnDestroy)

			CHAIN_MSG_MAP(CDialogResize<CChannelSettingsView>)
		END_MSG_MAP()


		BEGIN_DLGRESIZE_MAP(CChannelSettingsView)
			DLGRESIZE_CONTROL(IDC_EVENTS_CROSS_VAL2, DLSZ_MOVE_Y)
			DLGRESIZE_CONTROL(IDC_EVENTS_CROSS_VAL2_SET, DLSZ_MOVE_Y)
			DLGRESIZE_CONTROL(IDC_EVENTS_CROSS_VAL1, DLSZ_MOVE_Y)
			DLGRESIZE_CONTROL(IDC_EVENTS_CROSS_VAL1_SET, DLSZ_MOVE_Y)
			DLGRESIZE_CONTROL(IDC_EVENTS_CHANGE, DLSZ_MOVE_Y)
			DLGRESIZE_CONTROL(IDC_STATIC1, DLSZ_MOVE_Y)
			DLGRESIZE_CONTROL(IDC_STATIC3, DLSZ_MOVE_Y)
			DLGRESIZE_CONTROL(IDC_STATIC5, DLSZ_MOVE_Y)
			DLGRESIZE_CONTROL(IDC_UPDATE_INTERVAL, DLSZ_MOVE_Y)
			DLGRESIZE_CONTROL(IDC_NAME, DLSZ_MOVE_Y)
			DLGRESIZE_CONTROL(IDC_STATIC2, DLSZ_MOVE_Y|DLSZ_MOVE_X)
			DLGRESIZE_CONTROL(IDC_ARCH_ON_LESS, DLSZ_MOVE_Y|DLSZ_MOVE_X)
			DLGRESIZE_CONTROL(IDC_ARCH_ON_LESS_SET, DLSZ_MOVE_Y|DLSZ_MOVE_X)
			DLGRESIZE_CONTROL(IDC_ARCH_ON_GREATER, DLSZ_MOVE_Y|DLSZ_MOVE_X)
			DLGRESIZE_CONTROL(IDC_ARCH_ON_GREATER_SET, DLSZ_MOVE_Y|DLSZ_MOVE_X)
			DLGRESIZE_CONTROL(IDC_ARCH_ON_EQUAL, DLSZ_MOVE_Y|DLSZ_MOVE_X)
			DLGRESIZE_CONTROL(IDC_ARCH_ON_EQUAL_SET, DLSZ_MOVE_Y|DLSZ_MOVE_X)
			DLGRESIZE_CONTROL(IDC_CHANNELS, DLSZ_SIZE_Y|DLSZ_SIZE_X)
			DLGRESIZE_CONTROL(IDC_STATIC4, DLSZ_SIZE_X|DLSZ_MOVE_Y)
		END_DLGRESIZE_MAP()

		// Handler prototypes (uncomment arguments if needed):
		//	LRESULT MessageHandler(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
		//	LRESULT CommandHandler(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
		//	LRESULT NotifyHandler(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/)


		BOOL PreTranslateMessage(MSG* pMsg);
		LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
		LRESULT OnClickCheck(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
		LRESULT OnItemChanged(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/);
		LRESULT OnShowWindow(BOOL fShow, UINT status);
		LRESULT OnDestroy(void);

		void UpdateChannels();
		CString GetNameFromGUID(BSTR Computer, BSTR clsid);

		void UpdateEnable();

		bool ChangeCurrentAtributes();
		void InitializeAtributes();

		void SaveChannel(ULONG ID);
		void LoadChannel(ULONG ID);
		
	};
}
