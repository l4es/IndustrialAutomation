// MainFrm.h : interface of the CMainFrame class
//
/////////////////////////////////////////////////////////////////////////////
#pragma once
#include <atlcrack.h>
#include <Tabframework/atlgdix.h>
#include <Tabframework/DotNetTabCtrl.h>
#include <Tabframework/TabbedFrame.h>
#include <atlsplit.h>
#include <atlctrlx.h>

#include "Document.h"
#include "GraphicView.h"
#include "TableView.h"
#include "ChannelsView.h"
#include "LimitsView.h"
#include "EventsView.h"
#include "ScriptReport.h"

class CMainFrame 
	: public CFrameWindowImpl<CMainFrame>
	, public CUpdateUI<CMainFrame>
	, public CMessageFilter
	, public CIdleHandler
	, CPrintJobInfo
	
{
protected:
	

public:
	DECLARE_FRAME_WND_CLASS(NULL, IDR_MAINFRAME)

	//Дополнительные окна
	CTabbedChildWindow< CDotNetTabCtrl<CTabViewTabItem> > m_TabBar;
	CSplitterWindow		m_VertSplitter;
	CHorSplitterWindow	m_HorzSplitter;
	CCommandBarCtrl		m_CmdBar;
	CPaneContainer		m_PaneContainer;

	CToolBarCtrl m_MainFrameToolBar;
	CToolBarCtrl m_PreviewToolBar;

	//Документ
	CDocument m_Document;

	//Виды
	CGraphicView	m_GraphicView;
	CTableView		m_TableView;
	CChannelsView	m_ChannelsView;
	CLimitsView		m_LimitsView;
	CEventsView		m_EventsView;

	CPrintPreviewWindow m_wndPreview;
	CPrinter			m_Printer;
	CScriptReport		*m_ScriptReport;
	
	

	
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL OnIdle();

	BEGIN_UPDATE_UI_MAP(CMainFrame)
		UPDATE_ELEMENT(ID_VIEW_TOOLBAR, UPDUI_MENUPOPUP)
		UPDATE_ELEMENT(ID_VIEW_STATUS_BAR, UPDUI_MENUPOPUP)
		UPDATE_ELEMENT(ID_VIEW_CHANNELS, UPDUI_MENUPOPUP)
		UPDATE_ELEMENT(ID_PRINT_PREVIEW, UPDUI_MENUPOPUP|UPDUI_TOOLBAR)
	END_UPDATE_UI_MAP()

	BEGIN_MSG_MAP(CMainFrame)
		MESSAGE_HANDLER(WM_DELETE_TASK, OnDeleteTask)
		MESSAGE_HANDLER(WM_CREATE, OnCreate)
		MESSAGE_HANDLER(WM_POSTCREATE, OnPostCreate)
		MSG_WM_DESTROY(OnDestroy)
		COMMAND_ID_HANDLER(ID_PANE_CLOSE, OnCloseChannelsWindow)
		COMMAND_ID_HANDLER(ID_CLOSE_PREVIEW, OnClosePreview)

		COMMAND_ID_HANDLER(ID_APP_EXIT, OnFileExit)
		COMMAND_ID_HANDLER(ID_FILE_NEW, OnFileNew)
		COMMAND_ID_HANDLER(ID_PRINT_PREVIEW, OnPrintPreview)
		COMMAND_ID_HANDLER(ID_PRED, OnPrevPage)
		COMMAND_ID_HANDLER(ID_NEXT, OnNextPage)

		COMMAND_HANDLER(ID_NEXT, WM_LBUTTONDOWN, OnDownNext);

		COMMAND_ID_HANDLER(ID_VIEW_TOOLBAR, OnViewToolBar)
		COMMAND_ID_HANDLER(ID_VIEW_STATUS_BAR, OnViewStatusBar)
		COMMAND_ID_HANDLER(ID_VIEW_CHANNELS, OnViewChannels)

		COMMAND_ID_HANDLER(ID_APP_ABOUT, OnAppAbout)
		COMMAND_ID_HANDLER(ID_NEXT_PANE, OnNextPane)
		COMMAND_ID_HANDLER(ID_PREV_PANE, OnPrevPane)
		COMMAND_ID_HANDLER(ID_GRAPH_PARAM, OnGraphParam)
		COMMAND_ID_HANDLER(ID_FILE_PRINT, OnFilePrint)
		COMMAND_ID_HANDLER(ID_EXPORT_EXCEL, OnExportExcel)
		NOTIFY_CODE_HANDLER_EX(TCN_SELCHANGE, OnTcnSelChange)
		CHAIN_MSG_MAP(CUpdateUI<CMainFrame>)
		CHAIN_MSG_MAP(CFrameWindowImpl<CMainFrame>)
		REFLECT_NOTIFICATIONS()
	END_MSG_MAP()

// Handler prototypes (uncomment arguments if needed):
//	LRESULT MessageHandler(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
//	LRESULT CommandHandler(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
//	LRESULT NotifyHandler(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/)

	LRESULT OnDeleteTask(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnPostCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnFileExit(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnFileNew(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	//LRESULT OnPrintPreview(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnViewToolBar(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnViewStatusBar(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnAppAbout(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);

	LRESULT OnCloseChannelsWindow(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnViewChannels(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnNextPane(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnPrevPane(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnGraphParam(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnFilePrint(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnExportExcel(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnTcnSelChange(LPNMHDR pnmh);

	LRESULT OnPrintPreview(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnClosePreview(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnPrevPage(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnNextPage(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnDownNext(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		return 0;
	}
	bool PrintPage(UINT /*nPage*/, HDC hDC);
	void BeginPrintJob(HDC hDC);
	void PrePrintPage(UINT /*nPage*/, HDC /*hDC*/){}
	void PostPrintPage(UINT /*nPage*/, HDC /*hDC*/){}

	CDevMode dm;
	LRESULT OnDestroy(void);
};
