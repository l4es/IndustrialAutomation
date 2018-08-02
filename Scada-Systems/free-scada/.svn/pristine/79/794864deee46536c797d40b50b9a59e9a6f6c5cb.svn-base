// MainFrm.cpp : implmentation of the CMainFrame class
//
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "resource.h"

#include "aboutdlg.h"
#include "GraphicParamDlg.h"
#include "ExportToExcelDlg.h"
#include "ExportThreadDlg.h"
#include ".\mainfrm.h"



BOOL CMainFrame::PreTranslateMessage(MSG* pMsg)
{
	if(CFrameWindowImpl<CMainFrame>::PreTranslateMessage(pMsg))
		return TRUE;

	HWND focus = ::GetFocus();

	if(m_GraphicView.IsChild(focus))
		return m_GraphicView.PreTranslateMessage(pMsg);

	if(m_LimitsView.IsChild(focus))
	{
		return m_LimitsView.PreTranslateMessage(pMsg);
	}

	return FALSE;
}

BOOL CMainFrame::OnIdle()
{
	UIUpdateToolBar();
	UIUpdateMenuBar();
	return FALSE;
}

LRESULT CMainFrame::OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	SetWindowText(L"FreeSCADA: Report Generator");

	// create command bar window
	HWND hWndCmdBar = m_CmdBar.Create(m_hWnd, rcDefault, NULL, ATL_SIMPLE_CMDBAR_PANE_STYLE);
	// attach menu
	m_CmdBar.AttachMenu(GetMenu());
	// load command bar images
	m_CmdBar.LoadImages(IDR_MAINFRAME);
	// remove old menu
	SetMenu(NULL);

	m_MainFrameToolBar= CreateSimpleToolBarCtrl(m_hWnd, IDR_MAINFRAME, FALSE, ATL_SIMPLE_TOOLBAR_PANE_STYLE);
	m_PreviewToolBar = CreateSimpleToolBarCtrl(m_hWnd, IDR_PREVIEW1, FALSE, ATL_SIMPLE_TOOLBAR_PANE_STYLE);

	m_wndPreview.Create(m_hWnd, rcDefault, _T("Print preview"),  WS_CHILD );

	CreateSimpleReBar(ATL_SIMPLE_REBAR_NOBORDER_STYLE);
	AddSimpleReBarBand(hWndCmdBar);
	AddSimpleReBarBand(m_MainFrameToolBar.m_hWnd, NULL, TRUE);
	AddSimpleReBarBand(m_PreviewToolBar.m_hWnd, NULL, TRUE);


	CreateSimpleStatusBar();

	//Создаем разделители и окно-контейнер
	m_hWndClient = m_VertSplitter.Create(m_hWnd, rcDefault, NULL, WS_CHILD|WS_VISIBLE|WS_CLIPSIBLINGS|WS_CLIPCHILDREN, WS_EX_CLIENTEDGE);
	m_PaneContainer.Create(m_VertSplitter,L"Channels");
	m_HorzSplitter.Create(m_PaneContainer, rcDefault, NULL, WS_CHILD|WS_VISIBLE|WS_CLIPSIBLINGS|WS_CLIPCHILDREN, WS_EX_CLIENTEDGE);
	m_PaneContainer.SetClient(m_HorzSplitter);

	//Создаем панель закладок
	m_TabBar.SetForwardNotifications();
	m_TabBar.SetTabStyles(CTCS_TOOLTIPS);
	m_TabBar.Create(m_VertSplitter, rcDefault);

	m_Printer.OpenDefaultPrinter();
	m_ScriptReport = NULL;
	

	//Создаем окна видов
	//График
	m_GraphicView.Create(m_TabBar);
	m_TabBar.AddTab(m_GraphicView, L"Trends");
	m_Document.AddView(&m_GraphicView);
	//Таблица
	m_TableView.Create(m_TabBar, rcDefault, NULL, WS_CHILD|WS_VISIBLE|WS_CLIPSIBLINGS|WS_CLIPCHILDREN, WS_EX_CLIENTEDGE);
	m_TabBar.AddTab(m_TableView, L"Table");
	m_Document.AddView(&m_TableView);
	//События
	m_EventsView.Create(m_TabBar);
	m_TabBar.AddTab(m_EventsView, L"Events");
	m_Document.AddView(&m_EventsView);
	//Каналы
	m_ChannelsView.Create(m_HorzSplitter, rcDefault, NULL, WS_CHILD|WS_VISIBLE|WS_CLIPSIBLINGS|WS_CLIPCHILDREN, WS_EX_CLIENTEDGE);
	m_Document.AddView(&m_ChannelsView);
	//Пределы графика
	m_LimitsView.Create(m_HorzSplitter);
	m_Document.AddView(&m_LimitsView);

	//Подключаем виды к разделителям
	m_VertSplitter.SetSplitterPanes(m_PaneContainer, m_TabBar);
	m_HorzSplitter.SetSplitterPanes(m_ChannelsView, m_LimitsView);

	UIAddToolBar(m_MainFrameToolBar.m_hWnd);
	UISetCheck(ID_VIEW_TOOLBAR,		TRUE);
	UISetCheck(ID_VIEW_STATUS_BAR,	TRUE);
	UISetCheck(ID_VIEW_CHANNELS,	TRUE);

	// register object for message filtering and idle updates
	CMessageLoop* pLoop = _Module.GetMessageLoop();
	ATLASSERT(pLoop != NULL);
	pLoop->AddMessageFilter(this);
	pLoop->AddIdleHandler(this);

	PostMessage(WM_POSTCREATE);
	return 0;
}

LRESULT CMainFrame::OnPostCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	CRect rc;
	GetClientRect(rc);
	m_VertSplitter.SetSplitterPos(rc.Width()/3);
	m_HorzSplitter.SetSplitterPos(static_cast<int>(rc.Height()/2.5f));

	CReBarCtrl ReBar = m_hWndToolBar;
	int index = ReBar.IdToIndex(ATL_IDW_BAND_FIRST+2);
	ReBar.ShowBand(index, FALSE);

	return 0;
}

LRESULT CMainFrame::OnFileExit(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	PostMessage(WM_CLOSE);
	return 0;
}

LRESULT CMainFrame::OnFileNew(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	// TODO: add code to initialize document

	return 0;
}

LRESULT CMainFrame::OnViewToolBar(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	static BOOL bVisible = TRUE;	// initially visible
	bVisible = !bVisible;
	CReBarCtrl rebar = m_hWndToolBar;
	int nBandIndex = rebar.IdToIndex(ATL_IDW_BAND_FIRST + 1);	// toolbar is 2nd added band
	rebar.ShowBand(nBandIndex, bVisible);
	UISetCheck(ID_VIEW_TOOLBAR, bVisible);
	UpdateLayout();
	return 0;
}

LRESULT CMainFrame::OnViewStatusBar(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	BOOL bVisible = !::IsWindowVisible(m_hWndStatusBar);
	::ShowWindow(m_hWndStatusBar, bVisible ? SW_SHOWNOACTIVATE : SW_HIDE);
	UISetCheck(ID_VIEW_STATUS_BAR, bVisible);
	UpdateLayout();
	return 0;
}

LRESULT CMainFrame::OnAppAbout(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	CAboutDlg dlg;
	dlg.DoModal();
	return 0;
}

LRESULT CMainFrame::OnCloseChannelsWindow(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	m_VertSplitter.SetSinglePaneMode(1);
	UISetCheck(ID_VIEW_CHANNELS, FALSE);
	return 0;
}

LRESULT CMainFrame::OnViewChannels(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	if(m_VertSplitter.GetSinglePaneMode() == SPLIT_PANE_NONE)
	{
		m_VertSplitter.SetSinglePaneMode(1);
		UISetCheck(ID_VIEW_CHANNELS, FALSE);
	}
	else
	{
		m_VertSplitter.SetSinglePaneMode();
		UISetCheck(ID_VIEW_CHANNELS, TRUE);
	}
	return 0;
}
LRESULT CMainFrame::OnNextPane(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	int item = m_TabBar.GetTabCtrl().GetCurSel();
	if(item>=0)
	{
		item++;
		if(item >= 2)
			item = 0;
		m_TabBar.GetTabCtrl().SetCurSel(item);
	}
	return 0;
}

LRESULT CMainFrame::OnPrevPane(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	int item = m_TabBar.GetTabCtrl().GetCurSel();
	if(item>=0)
	{
		item--;
		if(item < 0)
			item = 1;
		m_TabBar.GetTabCtrl().SetCurSel(item);
	}
	return 0;
}

LRESULT CMainFrame::OnGraphParam(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	CGraphicParamDlg dlg;
	dlg.m_Degree= m_Document.m_ApproxDegree;
	dlg.m_Points = m_Document.m_GraphicPoints;
	dlg.m_Method = m_Document.m_ApproxMethod;
	if(dlg.DoModal() == IDOK)
	{
		m_Document.m_GraphicPoints	= dlg.m_Points;
		m_Document.m_ApproxDegree = dlg.m_Degree;
		m_Document.m_ApproxMethod = dlg.m_Method;
	}
	return 0;
}

LRESULT CMainFrame::OnFilePrint(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	const int CurSel = m_TabBar.GetTabCtrl().GetCurSel();

	CPrintDialog	dlg(FALSE, PD_PAGENUMS );
	dlg.m_pd.nFromPage = 1;
	dlg.m_pd.nMinPage =1;

	switch(CurSel)
	{
	case 0:
		m_GraphicView._graph.Print();
		return 0;
	case 1:
		{
			if(m_ScriptReport)
				delete m_ScriptReport;
			m_ScriptReport = new CScriptReport(IDR_CHANNEL, m_Document);
		}
		break;
	case 2:
		{
			if(m_ScriptReport)
				delete m_ScriptReport;
			m_ScriptReport = new CScriptReport(IDR_EVENTS, m_Document, &m_EventsView);
		}
		break;
	default: 
		return 0;
	}

	if(m_ScriptReport)
	{
		CDC dc = m_Printer.CreatePrinterDC();
		dlg.m_pd.nToPage = static_cast<WORD>(m_ScriptReport->GetPageCount(dc));
	}
	dlg.m_pd.nMaxPage = dlg.m_pd.nToPage;

	if(dlg.DoModal() != IDOK)
				return 0;

	CDevMode devMode = dlg.m_pd.hDevMode;
	CPrinter Printer;
	Printer.OpenPrinter(dlg.m_pd.hDevNames, devMode.m_pDevMode);
	CDC PrinterDC = Printer.CreatePrinterDC(devMode.m_pDevMode);

	CPrintJob		pj;
	const ULONG StartPage = max(1, dlg.m_pd.nFromPage-1);
	const ULONG EndPage = min(dlg.m_pd.nToPage, m_ScriptReport->GetPageCount(PrinterDC));

	CString JobName;
	if(m_ScriptReport)
		JobName.Format(L"Printing report «%s»", m_ScriptReport->GetReportName());
	else
		JobName = L"Printing report";
	pj.StartPrintJob(false, Printer, devMode, this, JobName, StartPage, EndPage);

	return 0;
}

LRESULT CMainFrame::OnExportExcel(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	CExportToExcelDlg dlg(&m_Document.m_DB);
	dlg.DoModal();

	return 0;
}

LRESULT CMainFrame::OnDeleteTask(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	CExportTaskMenager* p = CExportTaskMenager::Instance();
	p->DeleteTask((CExportThreadDlg*)wParam);
	p->Release();
	return 0;
}
LRESULT CMainFrame::OnTcnSelChange(LPNMHDR /*pnmh*/)
{
	const int CurSel = m_TabBar.GetTabCtrl().GetCurSel();
	
	switch(CurSel) 
	{
	case 0:	m_Document.m_ActiveView = CDocument::AV_GRAPH; break;
	case 1:	m_Document.m_ActiveView = CDocument::AV_TABLE; break;
	case 2:	m_Document.m_ActiveView = CDocument::AV_EVENTS; break;
	}

	UIEnable(ID_PRINT_PREVIEW, m_Document.m_ActiveView != CDocument::AV_GRAPH);
	return 0;
}

LRESULT CMainFrame::OnPrintPreview(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	CDC PrinterDC = m_Printer.CreatePrinterDC(dm.m_pDevMode);
	CTagDB::TVChannel Channels = m_Document.m_DB.GetHumanReadableChannelsList();

	const int CurSel = m_TabBar.GetTabCtrl().GetCurSel();

	if(CurSel == 1)
	{
		if(m_ScriptReport)
			delete m_ScriptReport;
		m_ScriptReport = new CScriptReport(IDR_CHANNEL, m_Document);
	}
	else
	{
		if(CurSel == 2)
		{
			if(m_ScriptReport)
				delete m_ScriptReport;
			m_ScriptReport = new CScriptReport(IDR_EVENTS, m_Document, &m_EventsView);
		}
		else
			return 0;
	}


	CRect rect;
	GetClientRect(&rect);

	dm.CopyFromPrinter(m_Printer);


	m_wndPreview.SetPrintPreviewInfo(m_Printer,dm,this,1, m_ScriptReport->GetPageCount(PrinterDC));

	m_wndPreview.SetPage(1);

	m_VertSplitter.ShowWindow(SW_HIDE);
	m_hWndClient =  m_wndPreview;
	m_wndPreview.ShowWindow(SW_SHOW);

	CReBarCtrl ReBar = m_hWndToolBar;
	int index = ReBar.IdToIndex(ATL_IDW_BAND_FIRST+1);
	int index1 = ReBar.IdToIndex(ATL_IDW_BAND_FIRST+2);

	ReBar.ShowBand(index, FALSE);
	ReBar.ShowBand(index1, TRUE);

	this->UpdateLayout();

	return 0;
}


LRESULT CMainFrame::OnClosePreview(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	CReBarCtrl ReBar = m_hWndToolBar;
	int index = ReBar.IdToIndex(ATL_IDW_BAND_FIRST+1);
	int index1 = ReBar.IdToIndex(ATL_IDW_BAND_FIRST+2);
	
	ReBar.ShowBand(index, TRUE);
	ReBar.ShowBand(index1, FALSE);
	m_hWndClient = m_VertSplitter;
	m_VertSplitter.ShowWindow(SW_SHOW);
	m_wndPreview.ShowWindow(SW_HIDE);
	UpdateLayout();
	return 0;
}

void CMainFrame::BeginPrintJob(HDC /*hDC*/)
{
	m_wndPreview.m_nMaxPage =10;
}

bool CMainFrame::PrintPage(UINT nPage, HDC hDC)
{
	if(m_ScriptReport)
		m_ScriptReport->PrintPage(hDC, nPage-1);
	return true;
}

LRESULT CMainFrame::OnPrevPage(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	m_wndPreview.m_pDefDevMode = dm.m_pDevMode;
	m_wndPreview.PrevPage();
	return 0;
}

LRESULT CMainFrame::OnNextPage(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	m_wndPreview.m_pDefDevMode = dm.m_pDevMode;
	m_wndPreview.NextPage();
	return 0;
}
LRESULT CMainFrame::OnDestroy(void)
{
	if(m_ScriptReport)
		delete m_ScriptReport;
	SetMsgHandled(FALSE);
	return 0;
}