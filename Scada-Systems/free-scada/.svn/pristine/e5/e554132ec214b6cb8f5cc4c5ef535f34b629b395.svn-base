#pragma once
#include <atlcrack.h>
#include "resource.h"

#include "Document.h"

class CLimitsView
	: public CDialogImpl<CLimitsView>
	, public CDialogResize<CLimitsView>
	, public CWinDataExchange<CLimitsView>
	, public DVF::CBaseView
{
public:
	enum { IDD = IDD_LIMITS_VIEW };
	
	BEGIN_DDX_MAP(CLimitsView)
	END_DDX_MAP()

	BEGIN_MSG_MAP(CLimitsView)
		COMMAND_HANDLER_EX(IDC_UPDATE, BN_CLICKED, OnUpdateBnClicked)
		NOTIFY_HANDLER(IDC_FROM_DATE, DTN_DATETIMECHANGE, OnDateTimeChange)
		NOTIFY_HANDLER(IDC_FROM_TIME, DTN_DATETIMECHANGE, OnDateTimeChange)
		NOTIFY_HANDLER(IDC_TO_DATE, DTN_DATETIMECHANGE, OnDateTimeChange)
		NOTIFY_HANDLER(IDC_TO_TIME, DTN_DATETIMECHANGE, OnDateTimeChange)
		CHAIN_MSG_MAP(CDialogResize<CLimitsView>)
		DEFAULT_REFLECTION_HANDLER()
	END_MSG_MAP()

	BEGIN_DLGRESIZE_MAP(CLimitsView)
		DLGRESIZE_CONTROL(IDC_STATIC_GROUP, DLSZ_SIZE_X)
		DLGRESIZE_CONTROL(IDC_STATIC_HOR_LINE1, DLSZ_SIZE_X)
		DLGRESIZE_CONTROL(IDC_STATIC_HOR_LINE2, DLSZ_SIZE_X)
		DLGRESIZE_CONTROL(IDC_STATIC_TO_TIME, DLSZ_MOVE_X)
		DLGRESIZE_CONTROL(IDC_STATIC_FROM_TIME, DLSZ_MOVE_X)
		DLGRESIZE_CONTROL(IDC_FROM_DATE, DLSZ_SIZE_X)
		DLGRESIZE_CONTROL(IDC_TO_DATE, DLSZ_SIZE_X)
		DLGRESIZE_CONTROL(IDC_FROM_TIME, DLSZ_MOVE_X)
		DLGRESIZE_CONTROL(IDC_TO_TIME, DLSZ_MOVE_X)
		DLGRESIZE_CONTROL(IDC_UPDATE, DLSZ_SIZE_X)
	END_DLGRESIZE_MAP()

	int m_GraphType;

	BOOL PreTranslateMessage(MSG* pMsg);
	void OnInitialUpdate();
	void OnUpdate();

	void EnableStaticGroup(BOOL Enable=TRUE);
	void EnableDynamicGroup(BOOL Enable=TRUE);
	LRESULT OnBnClickedGraphType(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnDateTimeChange(int /*idCtrl*/, LPNMHDR pNMHDR, BOOL& /*bHandled*/);
	LRESULT OnUpdateBnClicked(WORD wNotifyCode, WORD wID, HWND hWndCtl);
};
