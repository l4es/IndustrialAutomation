#include "stdafx.h"
#include "MainFrm.h"
#include ".\mainfrm.h"

BOOL CMainFrame::PreTranslateMessage(MSG* pMsg)
{
	BOOL Result = CFrameWindowImpl<CMainFrame>::PreTranslateMessage(pMsg);

	if(m_ModuleManager.GetCurrentNode() !=NULL)
	{
		CBaseModule* Module = m_ModuleManager.GetCurrentNode()->Module;
		return Module->PreTranslateMessage(pMsg);
	}
	return Result;
}


BOOL CMainFrame::OnIdle()
{
	UIUpdateToolBar();
	return FALSE;
}

LRESULT CMainFrame::OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	// create command bar window
	HWND hWndCmdBar = m_CmdBar.Create(m_hWnd, rcDefault, NULL, ATL_SIMPLE_CMDBAR_PANE_STYLE);
	// attach menu
	m_CmdBar.AttachMenu(GetMenu());
	// load command bar images
	m_CmdBar.LoadImages(IDR_MAINFRAME);
	// remove old menu
	SetMenu(NULL);

	HWND hWndToolBar = CreateSimpleToolBarCtrl(m_hWnd, IDR_MAINFRAME, FALSE, ATL_SIMPLE_TOOLBAR_PANE_STYLE);
	m_hModuleToolBar = NULL;
	CreateSimpleReBar(ATL_SIMPLE_REBAR_NOBORDER_STYLE);

	AddSimpleReBarBand(hWndCmdBar);

	CSize BtnMainFrm;
	((CToolBarCtrl)hWndToolBar).GetButtonSize(BtnMainFrm);
	int count = ((CToolBarCtrl)hWndToolBar).GetButtonCount();
	AddSimpleReBarBand(hWndToolBar, NULL, TRUE, BtnMainFrm.cx*count, TRUE);

	CreateSimpleStatusBar();
	//////////////////////////////////////////////////////////////////////////

	m_hWndClient = m_wndSplitter.Create(m_hWnd, rcDefault, NULL, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN, WS_EX_CLIENTEDGE);
	m_Tree.Create(m_wndSplitter, rcDefault, NULL, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN, WS_EX_CLIENTEDGE, ID_TREE);
	DWORD Style = m_Tree.GetWindowLong(GWL_STYLE);
	Style &= ~TVS_EDITLABELS;
	m_Tree.SetWindowLong(GWL_STYLE, Style);

	m_wndSplitter.SetSplitterPanes(m_Tree, NULL);
	m_CurrentWindow = NULL;
	//////////////////////////////////////////////////////////////////////////

	UIAddToolBar(hWndToolBar);

	UISetCheck(ID_VIEW_TOOLBAR, FALSE);
	UISetCheck(ID_VIEW_STATUS_BAR, FALSE);
	UIEnable(ID_FILE_OPEN, FALSE);
	UIEnable(ID_FILE_SAVE, FALSE);

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
	CRect rect;
	GetClientRect(&rect);
	m_wndSplitter.SetSplitterPos(rect.Width()/3);
	m_ModuleManager.m_MainFrame = this; 
	m_ModuleManager.LoadModules(m_wndSplitter, m_hWnd, rcDefault);	//Загрузка модулей менеджер

	for(vector<CBaseModule*>::iterator it = m_ModuleManager.m_Modules.begin();
		it != m_ModuleManager.m_Modules.end(); it++)
		m_Tree.AddModuleNode((*it)->m_Node);

	m_Tree.InitialUpdate();

	return 0;
}


LRESULT CMainFrame::OnUpdate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	m_Tree.Update(m_ModuleManager.GetCurrentNode()); //Обновляем дерево и делаем выбранным текущий узел
	return 0;
}



LRESULT CMainFrame::OnFileExit(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	PostMessage(WM_CLOSE);
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


LRESULT CMainFrame::OnSelChanged(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/)
{
	UIEnable(ID_FILE_OPEN, FALSE);
	UIEnable(ID_FILE_SAVE, FALSE);

	HTREEITEM Item = m_Tree.GetSelectedItem();
	if(Item == NULL)
	{
		m_ModuleManager.OnSetCurrent(NULL);
		return 0;
	}
	//Определяем из дерева текущий узел передаем указатель на него в менеджер 
	sNode* Curr = (sNode*)m_Tree.GetItemData(Item);
	if(Curr)
	{
		m_ModuleManager.OnSetCurrent(Curr);
		UIEnable(ID_FILE_OPEN, Curr->Module->ModuleCanLoad());
		UIEnable(ID_FILE_SAVE, Curr->Module->ModuleCanSave());
		UIUpdateToolBar();
	}

	LoadView();
	return 0;
}

//Загрузка вида выбранного узла в правое окно
void CMainFrame::LoadView()
{
	//если модуль не выбран выгружаем вид из правого окна
	if(m_ModuleManager.GetCurrentNode() == NULL)
	{
		if(::IsWindow(m_CurrentWindow))
		{
			m_CurrentWindow.ShowWindow(SW_HIDE);
			m_CurrentWindow = NULL;
		}
		m_wndSplitter.SetSplitterPanes(m_Tree, NULL);
		return;
	}

	//удаляем toolbarр модуля
	CReBarCtrl Rebar = m_hWndToolBar;
	if(Rebar.GetBandCount() > 2)
	{
		int nBandIndex = Rebar.IdToIndex(ATL_IDW_BAND_FIRST + 2);	// toolbar is 2nd added band
		CRect OldBandRt;
		((CWindow)m_hModuleToolBar).GetWindowRect(&OldBandRt);
		Rebar.DeleteBand(nBandIndex);
	}

	//Получаем вид текущего модуля
	CWindow View = m_ModuleManager.GetCurrentHWND();

	//Если вида нет, устанавливаем текущий вид правого окна в NULL
	if(View == NULL)
	{
		if(::IsWindow(m_CurrentWindow))
		{
			m_CurrentWindow.ShowWindow(SW_HIDE);
			m_CurrentWindow = NULL;
		}
		m_wndSplitter.SetSplitterPanes(m_Tree, NULL);
	}
	else
	{
		CRect rect;
		m_wndSplitter.GetSplitterPaneRect(1, &rect);
		ClientToScreen(&rect);
		//устанавливаем размеры для вид выбранного узла
		//			View.SetWindowPos(HWND_TOP, &rect, SWP_NOZORDER);

		//скрываем текущий вид
		if(m_CurrentWindow != NULL && m_CurrentWindow!=View)
		{
			m_CurrentWindow.ShowWindow(SW_HIDE);
		}
		//делаем вид выбранного модуля текущим
		m_CurrentWindow = View;
		View.ShowWindow(SW_SHOW);
		//показываем текущий вид
		m_wndSplitter.SetSplitterPanes(m_Tree, View);
	}
	//если у текущего модуля есть toolbar делаем его текущим
	if(m_ModuleManager.GetCurrentNode()->Module->m_hToolBar != NULL)
	{
		m_hModuleToolBar = m_ModuleManager.GetCurrentNode()->Module->m_hToolBar;
	}
	//если у текущего модуля нет toolbar-а делаем текущим пустой toolbar
	else
	{
		m_hModuleToolBar = NULL;
	}

	if(m_hModuleToolBar)
	{
		CRect NewBandRt;
		((CWindow)m_hModuleToolBar).GetWindowRect(&NewBandRt);
		((CWindow)m_hModuleToolBar).SetWindowPos(NULL, &NewBandRt, SWP_NOOWNERZORDER);

		AddSimpleReBarBand(m_hModuleToolBar);
		//Передаем текущему toolbar текущий вид в качесве обработчика сообщений
//		((CToolBarCtrl)m_hModuleToolBar).SetNotifyWnd(m_CurrentWindow);
	}

	UpdateLayout();
}



LRESULT CMainFrame::OnItemExpanding(int /*idCtrl*/, LPNMHDR pnmh, BOOL& /*bHandled*/)
{
	NMTREEVIEW* pnmtv = (NMTREEVIEW*)pnmh;

	//Если сворачиваем модуль - все как обычно
	if(pnmtv->action == TVE_EXPAND) 
	{
		HTREEITEM Item = pnmtv->itemNew.hItem;
		if(Item != NULL)
		{
			//Определяем sNode раскрываемого узла
			sNode* Node = (sNode*)m_Tree.GetItemData(Item);
			//Если узел не позволяет его раскрывать - не раскрываем его (return TRUE)
			if(!m_ModuleManager.OnExpandNode(Node))
				return TRUE;
		}
	}

	sNode *CurrentNode = (sNode*)m_Tree.GetItemData(pnmtv->itemNew.hItem);
	if(CurrentNode)
	{
		if(CurrentNode->Type == T_BRANCH)
		{
			if(pnmtv->action == TVE_EXPAND)
			{
				//Меняем иконку на открытую папку
				TVITEM item;
				item.hItem = pnmtv->itemNew.hItem;
				item.mask = TVIF_IMAGE|TVIF_SELECTEDIMAGE;
				m_Tree.GetItem(&item);
				item.iImage = CurrentNode->IconOpen;
				item.iSelectedImage = CurrentNode->IconOpen;
				m_Tree.SetItem(&item);
			}
			else
			{
				//Меняем иконку на закрытую папку
				TVITEM item;
				item.hItem = pnmtv->itemNew.hItem;
				item.mask = TVIF_IMAGE|TVIF_SELECTEDIMAGE;
				m_Tree.GetItem(&item);
				item.iImage = CurrentNode->IconClose;
				item.iSelectedImage = CurrentNode->IconClose;
				m_Tree.SetItem(&item);
			}
		}
	}
	return FALSE;
}

LRESULT CMainFrame::OnFileSave( WORD /*wNotifyCode*/,  WORD /*wID*/,  HWND /*hWndCtl*/,  BOOL& /*bHandled*/ )
{
	//Определяем текущий модуль
	sNode* Node = m_ModuleManager.GetCurrentNode();

	if(Node)
	{
		CBaseModule* Module = Node->Module;
		//Передаем обработку сообщения модулю
		if(Module)
			Module->OnFileSave();
	}

	return 0;
}
LRESULT CMainFrame::OnFileOpen( WORD /*wNotifyCode*/,  WORD /*wID*/,  HWND /*hWndCtl*/,  BOOL& /*bHandled*/ )
{
	//Определяем текущий модуль
	sNode* Node = m_ModuleManager.GetCurrentNode();

	if(Node)
	{
		CBaseModule* Module = Node->Module;
		//Передаем обработку сообщения модулю
		if(Module)
			Module->OnFileOpen();
	}
	return 0;
}

LRESULT CMainFrame::OnCommand(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled = FALSE;
	//Определяем текущий модуль
	sNode* Node = m_ModuleManager.GetCurrentNode();

	if(Node)
	{
		CBaseModule* Module = Node->Module;
		//Передаем обработку сообщения модулю
		if(Module)
			bHandled = Module->OnCommand(HIWORD(wParam), LOWORD(wParam), (HWND)lParam);
	}
	return 0;
}
LRESULT CMainFrame::OnDestroy(void)
{
	SetMsgHandled(FALSE);
	m_ModuleManager.OnSetCurrent(m_ModuleManager.GetCurrentNode());
	return 0;
}

LRESULT CMainFrame::OnHelp(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	TCHAR buffer[MAX_PATH+1];
	GetModuleFileName(NULL, buffer, MAX_PATH);

	wstring Folder = HelpersLib::ExtractFilePath(buffer);
	Folder += L"\\MultyConfigurator.chm";
	HtmlHelp(NULL, Folder.c_str(), HH_DISPLAY_TOPIC, 0);
	return 0;
}

LRESULT CMainFrame::OnHelp(UINT, int, HWND)
{
	SendMessage(WM_HELP);
	return 0;
}
