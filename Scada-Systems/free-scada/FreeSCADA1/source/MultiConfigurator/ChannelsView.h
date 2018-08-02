#pragma once
#include "resource.h"
#include "ComboTree/ComboTreeCtrl.h"


namespace ModuleArchive
{
	class CModuleArchive;

 	class CChannelsView
		: public CDialogImpl<CChannelsView>
		, public CDialogResize<CChannelsView>
		, public CWinDataExchange<CChannelsView>
	{
	public:
		enum { IDD = IDD_CHANNELS };

		CModuleArchive* m_Module;

		CString m_Computer;
		CComboTreeCtrl m_Channels;

		BEGIN_DDX_MAP(CChannelsView) 
			DDX_TEXT(IDC_ARCH_COMPUTER, m_Computer)
		END_DDX_MAP()

		BEGIN_MSG_MAP(CChannelsView)
			MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
			COMMAND_ID_HANDLER( IDC_BROWSE_COMP, OnBrowseComputer )
			COMMAND_ID_HANDLER( IDC_ADD, OnAddChannel )
			COMMAND_ID_HANDLER( IDC_DEL, OnDelChannel )
			COMMAND_ID_HANDLER( IDC_IMPORT, OnImportChannels )
			COMMAND_HANDLER( IDC_ARCH_SERVER, CBN_SELCHANGE, OnSelChangeServer )
			COMMAND_HANDLER(IDC_ARCH_SERVER, CBN_DROPDOWN, OnDropDownServer )
			
			CHAIN_MSG_MAP(CDialogResize<CChannelsView>)
		END_MSG_MAP()


		BEGIN_DLGRESIZE_MAP(CChannelsView)
			DLGRESIZE_CONTROL(IDC_STATIC1, DLSZ_SIZE_X)
			DLGRESIZE_CONTROL(IDC_ARCH_SERVER, DLSZ_SIZE_X)
			DLGRESIZE_CONTROL(IDC_CHANNEL, DLSZ_SIZE_X)
			DLGRESIZE_CONTROL(IDC_ADD, DLSZ_MOVE_X)
			DLGRESIZE_CONTROL(IDC_DEL, DLSZ_MOVE_X)
			DLGRESIZE_CONTROL(IDC_IMPORT, DLSZ_MOVE_X)
			DLGRESIZE_CONTROL(IDC_STATIC2, DLSZ_SIZE_X|DLSZ_SIZE_Y)
			DLGRESIZE_CONTROL(IDC_ARCHIVE, DLSZ_SIZE_X|DLSZ_SIZE_Y)
		END_DLGRESIZE_MAP()

		BOOL PreTranslateMessage(MSG* pMsg);
		LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
		LRESULT OnBrowseComputer( WORD wNotifyCode,  WORD wID,  HWND hWndCtl,  BOOL& bHandled );
		LRESULT OnAddChannel( WORD wNotifyCode,  WORD wID,  HWND hWndCtl,  BOOL& bHandled );
		LRESULT OnDelChannel( WORD wNotifyCode,  WORD wID,  HWND hWndCtl,  BOOL& bHandled );
		LRESULT OnImportChannels( WORD wNotifyCode,  WORD wID,  HWND hWndCtl,  BOOL& bHandled );
		LRESULT OnSelChangeServer( WORD wNotifyCode,  WORD wID,  HWND hWndCtl,  BOOL& bHandled );
		LRESULT OnDropDownServer( WORD wNotifyCode,  WORD wID,  HWND hWndCtl,  BOOL& bHandled );
	
		
		
		CString BrowseComputer();
		void FillServers();
		BOOL ValidateChannel(CString Computer, CLSID* ServerCLSID, CString Channel);

		void FillTree(HTREEITEM root, CComPtr<IOPCBrowseServerAddressSpace> sas);
		void FillTreeFlat(HTREEITEM root, CComPtr<IOPCBrowseServerAddressSpace> sas);
		HRESULT FillChannels();

		CString GetNameFromGUID(BSTR clsid);
		void UpdateArchiveChannels();
	};

}
