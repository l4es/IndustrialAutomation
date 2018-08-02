// ArchiverView.h : interface of the CArchiverView class
//
/////////////////////////////////////////////////////////////////////////////

#pragma once
#include "resource.h"

typedef CWinTraitsOR<LVS_REPORT | LVS_SINGLESEL | LVS_NOSORTHEADER | LVS_SHOWSELALWAYS|LVS_NOCOLUMNHEADER> CChannelsTraits;

class CMainFrame;

class CChannelsView 
	: public CWindowImpl<CChannelsView, CListViewCtrl,CChannelsTraits>
	, public DVF::CBaseView
{
public:
	DECLARE_WND_SUPERCLASS(NULL, CListViewCtrl::GetWndClassName())

	BEGIN_MSG_MAP(CChannelsView)
		MESSAGE_HANDLER(WM_CREATE, OnCreate)
		REFLECTED_NOTIFY_CODE_HANDLER(LVN_ITEMCHANGED, OnLVSelChanged)

		DEFAULT_REFLECTION_HANDLER();
		MESSAGE_HANDLER(WM_SIZE, OnSize)
	END_MSG_MAP()

	CMainFrame* m_MainFrame;

// Handler prototypes (uncomment arguments if needed):
	//	LRESULT MessageHandler(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	//	LRESULT CommandHandler(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	//	LRESULT NotifyHandler(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/)

	LRESULT OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& bHandled);
	void OnUpdate();
	void OnInitialUpdate();

	LRESULT OnSize(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnLVSelChanged(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/);
};
