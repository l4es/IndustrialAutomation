#pragma once

#include "resource.h"
#include "TagDB.h"
#include <string>
using namespace std;

class CExportToExcelDlg
	: public CDialogImpl<CExportToExcelDlg>
	, public CWinDataExchange<CExportToExcelDlg>
	, public CDialogResize<CExportToExcelDlg>
{
public:
	enum { IDD = IDD_EXPORT_TO_EXCEL };

	BEGIN_DDX_MAP(CExportToExcelDlg)
	END_DDX_MAP()

	BEGIN_MSG_MAP(CExportToExcelDlg)
		NOTIFY_HANDLER(IDC_CHANNEL_LIST, LVN_ITEMCHANGING, OnLVSelChanging)
		NOTIFY_HANDLER(IDC_CHANNEL_LIST, LVN_ITEMCHANGED, OnLVSelChanged)

		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		COMMAND_ID_HANDLER(IDOK, OnCloseCmd)
		COMMAND_ID_HANDLER(IDCANCEL, OnCloseCmd)

		CHAIN_MSG_MAP(CDialogResize<CExportToExcelDlg>)
		DEFAULT_REFLECTION_HANDLER()
	END_MSG_MAP()

	BEGIN_DLGRESIZE_MAP(CExportToExcelDlg)
		DLGRESIZE_CONTROL(IDC_CHANNEL_LIST, DLSZ_SIZE_X|DLSZ_SIZE_Y)
		DLGRESIZE_CONTROL(IDC_STATIC_GROUP, DLSZ_MOVE_X)
		DLGRESIZE_CONTROL(IDC_STATIC_HOR_LINE1, DLSZ_MOVE_X)
		DLGRESIZE_CONTROL(IDC_STATIC_HOR_LINE2, DLSZ_MOVE_X)
		DLGRESIZE_CONTROL(IDC_FILENAME, DLSZ_MOVE_X)
		DLGRESIZE_CONTROL(IDC_FILESELECT, DLSZ_MOVE_X)
		DLGRESIZE_CONTROL(IDC_FROM_DATE, DLSZ_MOVE_X)
		DLGRESIZE_CONTROL(IDC_TO_DATE, DLSZ_MOVE_X)
		DLGRESIZE_CONTROL(IDC_FROM_TIME, DLSZ_MOVE_X)
		DLGRESIZE_CONTROL(IDC_TO_TIME, DLSZ_MOVE_X)

		DLGRESIZE_CONTROL(IDC_STATIC_FROM, DLSZ_MOVE_X)
		DLGRESIZE_CONTROL(IDC_STATIC_DATE_FROM, DLSZ_MOVE_X)
		DLGRESIZE_CONTROL(IDC_STATIC_TO, DLSZ_MOVE_X)
		DLGRESIZE_CONTROL(IDC_STATIC_DATE_TO, DLSZ_MOVE_X)
		DLGRESIZE_CONTROL(IDC_STATIC_FILE, DLSZ_MOVE_X)
		DLGRESIZE_CONTROL(IDC_STATIC_FROM_TIME, DLSZ_MOVE_X)
		DLGRESIZE_CONTROL(IDC_STATIC_TO_TIME, DLSZ_MOVE_X)

		DLGRESIZE_CONTROL(IDOK, DLSZ_MOVE_X|DLSZ_MOVE_Y)
		DLGRESIZE_CONTROL(IDCANCEL, DLSZ_MOVE_X|DLSZ_MOVE_Y)
	END_DLGRESIZE_MAP()

	CExportToExcelDlg(CTagDB* _DB):_db(_DB){};

	CDateTimePickerCtrl		m_FromDate;
	CDateTimePickerCtrl		m_FromTime;
	CDateTimePickerCtrl		m_ToDate;
	CDateTimePickerCtrl		m_ToTime;
	CListViewCtrl			m_List;

// Handler prototypes (uncomment arguments if needed):
	//	LRESULT MessageHandler(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	//	LRESULT CommandHandler(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	//	LRESULT NotifyHandler(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/)

	LRESULT OnLVSelChanged(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/);
	LRESULT OnLVSelChanging(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/);
	LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnCloseCmd(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/);

protected:
	CTagDB* _db;
	void FillChannelList(void);
	int GetCurSel();
	void DoExport(UINT TagID, COleDateTime From, COleDateTime To, wstring FileName);
};