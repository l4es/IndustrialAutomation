#pragma once

class CSelectServerDialog
	: public CDialogImpl<CSelectServerDialog>
	, public CDialogResize<CSelectServerDialog>
	, public CWinDataExchange<CSelectServerDialog>
{
public:
	typedef CSelectServerDialog thisClass;
	enum { IDD = IDD_SELECT_SERVER };

	CListBox m_ListBox;
	TStrings m_ListServer;
	CString m_Computer;

	BEGIN_DDX_MAP(thisClass)
	
	END_DDX_MAP()
		
	BEGIN_MSG_MAP(thisClass)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		MESSAGE_HANDLER(WM_CLOSE, OnClose)
		COMMAND_ID_HANDLER(IDOK, OnCloseCmd)
		COMMAND_ID_HANDLER(IDCANCEL, OnCloseCmd)
		REFLECT_NOTIFICATIONS()
		CHAIN_MSG_MAP(CDialogResize<thisClass>)
	END_MSG_MAP()

	BEGIN_DLGRESIZE_MAP(thisClass)
	//	DLGRESIZE_CONTROL(IDC_VARIABLE_LIST, DLSZ_SIZE_X|DLSZ_SIZE_Y)
	//	DLGRESIZE_CONTROL(IDC_ADD_VAR, DLSZ_MOVE_Y)
	//	DLGRESIZE_CONTROL(IDC_DEL_VAR, DLSZ_MOVE_Y)
	//	DLGRESIZE_CONTROL(IDC_CHANGE_VAR, DLSZ_MOVE_Y)
	END_DLGRESIZE_MAP()

	CSelectServerDialog(void);
	LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnCloseCmd(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnClose(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);

	void FillListServer();
};
