#pragma once
//#include <atlframe.h>
#include <atlddx.h>
#include "resource.h"


class CInputDialog
	: public CDialogImpl<CInputDialog>
	//, public CDialogResize<CInputDialog>
	, public CWinDataExchange<CInputDialog>
{
	typedef CInputDialog thisClass;
public:
	enum { IDD = IDD_INPUT };

	BEGIN_DDX_MAP(thisClass)
		DDX_TEXT(IDC_EDIT_INPUT, m_Value)
	END_DDX_MAP()

	BEGIN_MSG_MAP(thisClass)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		COMMAND_ID_HANDLER(IDOK, OnCloseCmd)
		COMMAND_ID_HANDLER(IDCANCEL, OnCloseCmd)
	//	CHAIN_MSG_MAP(CDialogResize<thisClass>)
		DEFAULT_REFLECTION_HANDLER()
	END_MSG_MAP()

/*	BEGIN_DLGRESIZE_MAP(thisClass)
		DLGRESIZE_CONTROL(IDC_EDIT_INPUT, DLSZ_SIZE_X|DLSZ_SIZE_Y)
	END_DLGRESIZE_MAP()*/

	CInputDialog()
	{
	};

	LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
		DoDataExchange(DDX_LOAD);
	//	DlgResize_Init();
		CenterWindow(GetParent());

		return TRUE;
	}

	LRESULT OnCloseCmd(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		DoDataExchange(DDX_SAVE);
		EndDialog(wID);
		return 0;
	}

	WTL::CString m_Value;
protected:
	
};