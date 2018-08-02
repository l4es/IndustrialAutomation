// MainFrm.cpp : implmentation of the CMainFrame class
//
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "resource.h"

#include "aboutdlg.h"
#include "VisualizatorView.h"
#include "SettingsDlg.h"
#include ".\mainfrm.h"

CMainFrame* g_MainFrame = NULL;		//Глобальный указатель на CMainFrame

CMainFrame::CMainFrame()
{
	m_Doc = NULL;
	g_MainFrame = this;
}
CMainFrame::~CMainFrame()
{
	g_MainFrame = NULL;
}

BOOL CMainFrame::PreTranslateMessage(MSG* pMsg)
{
	if(CFrameWindowImpl<CMainFrame>::PreTranslateMessage(pMsg))
		return TRUE;

	return m_View.PreTranslateMessage(pMsg);
}

BOOL CMainFrame::OnIdle()
{
	return FALSE;
}

LRESULT CMainFrame::OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	CreateSimpleStatusBar();

	m_Doc = new CDoc();

	m_hWndClient = m_View.Create(m_hWnd, rcDefault, NULL, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN, WS_EX_CLIENTEDGE);
	m_Doc->AddView(&m_View);

	UISetCheck(ID_VIEW_STATUS_BAR, 1);

	// register object for message filtering and idle updates
	CMessageLoop* pLoop = _Module.GetMessageLoop();
	ATLASSERT(pLoop != NULL);
	pLoop->AddMessageFilter(this);
	pLoop->AddIdleHandler(this);

	LoadSchemaMenu();

	PostMessage(WM_POSTCREATE);

	return 0;
}

LRESULT CMainFrame::OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
{
	bHandled = FALSE;
	if(m_Doc)
	{
		delete m_Doc;
		m_Doc = NULL;
	}
	return 0;
}

LRESULT CMainFrame::OnFileExit(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	PostMessage(WM_CLOSE);
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

LRESULT CMainFrame::OnFileOpen(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	

	CFileDialog dlg(TRUE, L".mp", NULL, OFN_HIDEREADONLY, cFilterProject);
	if(dlg.DoModal() == IDOK)
	{
		if(m_Doc->LoadProject(dlg.m_szFileName) != true)
		{
			m_Doc->EmptyView();
			LoadSchemaMenu();
			m_Doc->UpdateAllViews();
			MessageBox(L"Cannot load project file.",L"Warning!",MB_OK|MB_ICONWARNING);
			return 0;
		}
		LoadSchemaMenu();
	}
	return 0;
}

void CMainFrame::LoadSchemaMenu()
{
	CMenuHandle MainMenu = GetMenu();
	if(MainMenu.GetMenuItemCount()>2)
		MainMenu.DeleteMenu(1,MF_BYPOSITION);
	
	CMenu menu;
	menu.CreateMenu();

	TStrings Schemas;
	m_Doc->m_Archive.GetDirectoryFileList(Schemas, CArchiveManager::SF_SCHEMAS);

	for(ULONG i=0;i<(ULONG)Schemas.size();i++)
	{
		menu.AppendMenu(MF_STRING,ID_MENU_SCHEMAS+i, Schemas[i].c_str());
	}
	MainMenu.InsertMenu(1, MF_BYPOSITION|MF_POPUP|MF_STRING, (UINT_PTR)(HMENU)menu, L"Circuits");
	menu.Detach();
	DrawMenuBar();
}

LRESULT CMainFrame::OnSchemaSelect(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	TStrings Schemas;
	m_Doc->m_Archive.GetDirectoryFileList(Schemas, CArchiveManager::SF_SCHEMAS);

	ATLASSERT((wID - ID_MENU_SCHEMAS) < (WORD)Schemas.size());
	m_Doc->SelectSchema(Schemas[(wID - ID_MENU_SCHEMAS)]);
	return 0;
}

LRESULT CMainFrame::OnGetMinMaxInfo(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& /*bHandled*/)
{
	MINMAXINFO* mmi = (MINMAXINFO*)lParam;
	
	mmi->ptMinTrackSize.x = 300;
	mmi->ptMinTrackSize.y = 200;
	return 0;
}

LRESULT CMainFrame::OnSettings(WORD wNotifyCode, WORD wID, HWND hWndCtl)
{
	CSettingsDlg dlg;
	dlg.DoModal();
	return 0;
}

LRESULT CMainFrame::OnHelp(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	hl::CCfgReg reg;
	
	reg.Open(cProduct, true, false);
	wstring Path = reg.ReadString(L"Help Path");
	if(Path.empty())
	{
		TCHAR buffer[MAX_PATH+1];
		GetModuleFileName(NULL, buffer, MAX_PATH);

		wstring Folder = HelpersLib::ExtractFilePath(buffer) + L"\\Help\\";
		Path = Folder + L"Visualizator.chm";
	}
	HtmlHelp(NULL, Path.c_str(), HH_DISPLAY_TOPIC, 0);

	return 0;
}

LRESULT CMainFrame::OnIdHelp(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	BOOL Handled;
	OnHelp(0, 0, 0, Handled);
	return 0;
}

LRESULT CMainFrame::OnPostCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	if(ConfigFunc::FileAutoRunIsOn())
		m_Doc->LoadProject(ConfigFunc::FileAutoRun());

	LoadSchemaMenu();

	return 0;
}
