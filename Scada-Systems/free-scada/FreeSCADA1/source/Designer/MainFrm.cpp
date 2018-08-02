#include "..\visualizator\mainfrm.h"
// MainFrm.cpp : implmentation of the CMainFrame class
//
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "resource.h"

#include "aboutdlg.h"
#include "DesignerView.h"
#include "MainFrm.h"
#include "ProjectImageDlg.h"
#include "ProjectSoundsDlg.h"
#include "InputBox.h"
#include "SchemaDialog.h"
#include "VariableDialog.h"
#include "ProjectSettingsDialog.h"
#include "DeleteSchemaDialog.h"
#include "ObjectsDialog.h"
#include "ObjectMenager.h"
#include ".\mainfrm.h"

//#define ID_TOOLBAR 123

CMainFrame *g_MainFrame = NULL;

CMainFrame::CMainFrame()
{
	//Окно свойств 
	m_PropertyWindow = CObjectPropertyWindow::Instance();
	g_MainFrame = this;
}

CMainFrame::~CMainFrame()
{
	m_PropertyWindow->FreeInst();
	g_MainFrame = NULL;
};

BOOL CMainFrame::PreTranslateMessage(MSG* pMsg)
{
	if(baseClass::PreTranslateMessage(pMsg))
		return TRUE;

	return m_View.PreTranslateMessage(pMsg);
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
	m_CmdBar.LoadImages(IDR_ALIGN);
	// remove old menu
	SetMenu(NULL);

	HWND hWndToolBar = CreateSimpleToolBarCtrl(m_hWnd, IDR_MAINFRAME, FALSE, ATL_SIMPLE_TOOLBAR_PANE_STYLE);
	m_PrimitivesToolBar = CreateSimpleToolBarCtrl(m_hWnd,IDR_PRIMITIVES, FALSE, ATL_SIMPLE_TOOLBAR_PANE_STYLE);
	m_AlignToolBar = CreateSimpleToolBarCtrl(m_hWnd,IDR_ALIGN, FALSE, ATL_SIMPLE_TOOLBAR_PANE_STYLE);
	m_SchemaBar.Create(m_hWnd);

	CreateSimpleReBar(ATL_SIMPLE_REBAR_NOBORDER_STYLE);
	AddSimpleReBarBand(hWndCmdBar);

	CSize BtnSizePrim, BtnSizeAlign;
	CToolBarCtrl(hWndToolBar).GetButtonSize(BtnSizePrim);
	int count = CToolBarCtrl(hWndToolBar).GetButtonCount();
	AddSimpleReBarBand(hWndToolBar, NULL, TRUE, BtnSizePrim.cx*count+BtnSizePrim.cx/2);

	m_AlignToolBar.GetButtonSize(BtnSizeAlign);
	count = m_AlignToolBar.GetButtonCount();
	AddSimpleReBarBand(m_AlignToolBar, NULL, FALSE, BtnSizeAlign.cx*count,TRUE);
	AddSimpleReBarBand(m_SchemaBar, NULL);

	m_PrimitivesToolBar.GetButtonSize(BtnSizePrim);
	count = m_PrimitivesToolBar.GetButtonCount();
	AddSimpleReBarBand(m_PrimitivesToolBar, NULL, TRUE, BtnSizePrim.cx*count);
	
	CreateSimpleStatusBar();

	m_CmdBar.AddToolbar(hWndToolBar);
	m_CmdBar.AddToolbar(m_PrimitivesToolBar);
	m_CmdBar.AddToolbar(m_SchemaBar);
	m_CmdBar.Prepare();

	CRect rect(0, 0, 220, 300);
	m_PropertyWindow->Create(m_hWnd, rect, NULL, WS_OVERLAPPEDWINDOW | WS_POPUP | WS_CLIPCHILDREN | WS_CLIPSIBLINGS /*| WS_VISIBLE*/);
	m_Doc.AddView(m_PropertyWindow);
	m_hWndClient = m_View.Create(m_hWnd, rcDefault, NULL, WS_HSCROLL|WS_VSCROLL|WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN, WS_EX_CLIENTEDGE);
	UIAddToolBar(hWndToolBar);
	UIAddToolBar(m_PrimitivesToolBar);
	UIAddToolBar(m_AlignToolBar);
	UISetCheck(ID_VIEW_TOOLBAR,		TRUE);
	UISetCheck(ID_VIEW_PRIMITIVES,	TRUE);
	UISetCheck(ID_VIEW_STATUS_BAR,	TRUE);
	UISetCheck(ID_EMPTY,			TRUE);
	UISetCheck(ID_USE_GRID,			m_Doc.m_ShowGrid?TRUE:FALSE);

	// register object for message filtering and idle updates
	CMessageLoop* pLoop = _Module.GetMessageLoop();
	ATLASSERT(pLoop != NULL);
	pLoop->AddMessageFilter(this);
	pLoop->AddIdleHandler(this);

	InitializeDockingFrame(CStyle::sUseSysSettings);

	PostMessage(WM_POSTCREATE);

	m_Doc.AddView(&m_View);
	
	return 0;
}

LRESULT CMainFrame::OnPostCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
 	CRect rect;
	GetClientRect(&rect);
	rect.right = rect.left + 200;
	

	DockWindow( *m_PropertyWindow,  
				dockwins::CDockingSide::sRight,
				0, 
				float(0.0), 
				220, 
				300);
	BOOL tmp=FALSE;
	OnFileNew(NULL,NULL, NULL, tmp);

	return 0;
}

LRESULT CMainFrame::OnUpdateView(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	m_View.RedrawWindow();
	return 0;
}

LRESULT CMainFrame::OnFileExit(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	PostMessage(WM_CLOSE);
	return 0;
}

//Создание нового архива
LRESULT CMainFrame::OnFileNew(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	m_Doc.NewProject();
	FillSchemaList();
	m_Doc.UpdateAllViews();
	return 0;
}

LRESULT CMainFrame::OnViewToolBar(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	static BOOL bToolbarVisible = TRUE;	// initially visible
	bToolbarVisible = !bToolbarVisible;
	CReBarCtrl rebar = m_hWndToolBar;
	int nBandIndex = rebar.IdToIndex(ATL_IDW_BAND_FIRST + 1);	// toolbar is 2nd added band
	rebar.ShowBand(nBandIndex, bToolbarVisible);
	UISetCheck(ID_VIEW_TOOLBAR, bToolbarVisible);
	UpdateLayout();
	return 0;
}

LRESULT CMainFrame::OnViewPrimitivesBar(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	static BOOL bPrimitivesBarVisible = TRUE;	// initially visible
	bPrimitivesBarVisible = !bPrimitivesBarVisible;
	CReBarCtrl rebar = m_hWndToolBar;
	int nBandIndex = rebar.IdToIndex(ATL_IDW_BAND_FIRST + 2);	// toolbar is 3 added band
	rebar.ShowBand(nBandIndex, bPrimitivesBarVisible);
	UISetCheck(ID_VIEW_PRIMITIVES, bPrimitivesBarVisible);
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

LRESULT CMainFrame::OnViewProperty(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	m_PropertyWindow->Toggle();
	return 0;
}

LRESULT CMainFrame::OnAppAbout(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	CAboutDlg dlg;
	dlg.DoModal();
	return 0;
}

//Установка режима при котором не создаются примитивы
LRESULT CMainFrame::OnCreateEmpty(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	const bool Check = UIGetState(ID_EMPTY)&UPDUI_CHECKED?true:false;
	if(Check == false)
	{
		m_Doc.m_CurrentCreatingPrimitive = P_EMPTY;
		SendMessage(WM_UPDATE_PRIMITIVES_TOOLBAR);
	}
	return 0;
}

//Установка режима создания полилиний
LRESULT CMainFrame::OnCreatePolyline(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	const bool Check = UIGetState(ID_POLYLINE)&UPDUI_CHECKED?true:false;
	if(!Check)
		m_Doc.m_CurrentCreatingPrimitive = P_POLYLINE;
	else
		m_Doc.m_CurrentCreatingPrimitive = P_EMPTY;

	SendMessage(WM_UPDATE_PRIMITIVES_TOOLBAR);

	return 0;
}

//Установка режима создания прямоугольников
LRESULT CMainFrame::OnCreateRectangle(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	const bool Check = UIGetState(ID_RECTANGLE)&UPDUI_CHECKED?true:false;
	if(!Check)
		m_Doc.m_CurrentCreatingPrimitive = P_RECTANGLE;
	else
		m_Doc.m_CurrentCreatingPrimitive = P_EMPTY;

	SendMessage(WM_UPDATE_PRIMITIVES_TOOLBAR);
	return 0;
}

//Установка режима создания эллипсов
LRESULT CMainFrame::OnCreateEllipse(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	const bool Check = UIGetState(ID_ELIPSE)&UPDUI_CHECKED?true:false;
	if(!Check)
		m_Doc.m_CurrentCreatingPrimitive = P_ELLIPSE;
	else
		m_Doc.m_CurrentCreatingPrimitive = P_EMPTY;

	SendMessage(WM_UPDATE_PRIMITIVES_TOOLBAR);
	return 0;
}

//Установка режима создания картинок
LRESULT CMainFrame::OnCreateImage(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	const bool Check = UIGetState(ID_IMAGE)&UPDUI_CHECKED?true:false;
	if(!Check)
		m_Doc.m_CurrentCreatingPrimitive = P_IMAGE;
	else
		m_Doc.m_CurrentCreatingPrimitive = P_EMPTY;

	SendMessage(WM_UPDATE_PRIMITIVES_TOOLBAR);
	return 0;
}

//Установка режима создания текста
LRESULT CMainFrame::OnCreateText(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	const bool Check = UIGetState(ID_TEXT)&UPDUI_CHECKED?true:false;
	if(!Check)
		m_Doc.m_CurrentCreatingPrimitive = P_TEXT;
	else
		m_Doc.m_CurrentCreatingPrimitive = P_EMPTY;

	SendMessage(WM_UPDATE_PRIMITIVES_TOOLBAR);
	return 0;
}

//Установка режима создания кнопки
LRESULT CMainFrame::OnCreateButton(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	const bool Check = UIGetState(ID_BUTTON)&UPDUI_CHECKED?true:false;
	if(!Check)
		m_Doc.m_CurrentCreatingPrimitive = P_BUTTON;
	else
		m_Doc.m_CurrentCreatingPrimitive = P_EMPTY;

	SendMessage(WM_UPDATE_PRIMITIVES_TOOLBAR);
	return 0;
}

//Установка режима создания окошка ввода 
LRESULT CMainFrame::OnCreateInput(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	const bool Check = UIGetState(ID_INPUT)&UPDUI_CHECKED?true:false;
	if(!Check)
		m_Doc.m_CurrentCreatingPrimitive = P_INPUT;
	else
		m_Doc.m_CurrentCreatingPrimitive = P_EMPTY;

	SendMessage(WM_UPDATE_PRIMITIVES_TOOLBAR);
	return 0;
}

LRESULT CMainFrame::OnCreateSound(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	const bool Check = UIGetState(ID_SOUND)&UPDUI_CHECKED?true:false;
	if(!Check)
		m_Doc.m_CurrentCreatingPrimitive = P_SOUND;
	else
		m_Doc.m_CurrentCreatingPrimitive = P_EMPTY;

	SendMessage(WM_UPDATE_PRIMITIVES_TOOLBAR);
	return 0;
}

LRESULT CMainFrame::OnCreateGraph(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	const bool Check = UIGetState(ID_GRAPH)&UPDUI_CHECKED?true:false;
	if(!Check)
		m_Doc.m_CurrentCreatingPrimitive = P_GRAPH;
	else
		m_Doc.m_CurrentCreatingPrimitive = P_EMPTY;

	SendMessage(WM_UPDATE_PRIMITIVES_TOOLBAR);
	return 0;
}



//Открытие архива
LRESULT CMainFrame::OnFileOpen(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	CFileDialog dlg(TRUE, L".mp", NULL, OFN_HIDEREADONLY, cFilterProject);
	if(dlg.DoModal() == IDOK)
	{
		if(m_Doc.LoadProject(dlg.m_szFileName) != true)
		{
			MessageBox(L"Cannot open project file.",L"Warning!",MB_OK|MB_ICONWARNING);
			return 0;
		}
		FillSchemaList();
	}
	return 0;
}

//Сохранение архива
LRESULT CMainFrame::OnFileSave(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	wstring Name = m_Doc.m_Archive.GetArchiveFile();
	//Если имеется имя архива сохраняем с этим именем
	if(!Name.empty() && !m_Doc.IsFileTemp())
	{
		if(m_Doc.SaveProject(Name) != true)
		{
			MessageBox(L"Cannot save project file.",L"Warning!",MB_OK|MB_ICONWARNING);
			return 0;
		}
	}
	//Выводим диалог "Сохранить Как"
	else
	{
		BOOL Handled;
		OnFileSaveAs(0,0,0,Handled);
	}
	return 0;
}

LRESULT CMainFrame::OnFileSaveAs(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	CFileDialog dlg(FALSE, L".mp", NULL, OFN_HIDEREADONLY, cFilterProject);
	if(dlg.DoModal() == IDOK)
	{
		if(m_Doc.SaveProject(dlg.m_szFileName) != true)
		{
			MessageBox(L"Cannot save project file.",L"Warning!",MB_OK|MB_ICONWARNING);
			return 0;
		}
	}
	return 0;
}

//Обновление панели инструментов примитивов
LRESULT CMainFrame::OnUpdatePrimitivesToolbar(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	switch(m_Doc.m_CurrentCreatingPrimitive)
	{
	case P_EMPTY:
		UISetCheck(ID_EMPTY,		true);
		UISetCheck(ID_POLYLINE,		false);
		UISetCheck(ID_RECTANGLE,	false);
		UISetCheck(ID_ELIPSE,		false);
		UISetCheck(ID_IMAGE,		false);
		UISetCheck(ID_TEXT,			false);
		UISetCheck(ID_BUTTON,		false);
		UISetCheck(ID_INPUT,		false);
		UISetCheck(ID_SOUND,		false);
		UISetCheck(ID_GRAPH,		false);
		break;
	case P_POLYLINE:
		UISetCheck(ID_EMPTY,		false);
		UISetCheck(ID_POLYLINE,		true);
		UISetCheck(ID_RECTANGLE,	false);
		UISetCheck(ID_ELIPSE,		false);
		UISetCheck(ID_IMAGE,		false);
		UISetCheck(ID_TEXT,			false);
		UISetCheck(ID_BUTTON,		false);
		UISetCheck(ID_INPUT,		false);
		UISetCheck(ID_SOUND,		false);
		UISetCheck(ID_GRAPH,		false);
		break;
	case P_RECTANGLE:
		UISetCheck(ID_EMPTY,		false);
		UISetCheck(ID_POLYLINE,		false);
		UISetCheck(ID_RECTANGLE,	true);
		UISetCheck(ID_ELIPSE,		false);
		UISetCheck(ID_IMAGE,		false);
		UISetCheck(ID_TEXT,			false);
		UISetCheck(ID_BUTTON,		false);
		UISetCheck(ID_INPUT,		false);
		UISetCheck(ID_SOUND,		false);
		UISetCheck(ID_GRAPH,		false);
		break;
	case P_ELLIPSE:
		UISetCheck(ID_EMPTY,		false);
		UISetCheck(ID_POLYLINE,		false);
		UISetCheck(ID_RECTANGLE,	false);
		UISetCheck(ID_ELIPSE,		true);
		UISetCheck(ID_IMAGE,		false);
		UISetCheck(ID_TEXT,			false);
		UISetCheck(ID_BUTTON,		false);
		UISetCheck(ID_INPUT,		false);
		UISetCheck(ID_SOUND,		false);
		UISetCheck(ID_GRAPH,		false);
		break;
	case P_IMAGE:
		UISetCheck(ID_EMPTY,		false);
		UISetCheck(ID_POLYLINE,		false);
		UISetCheck(ID_RECTANGLE,	false);
		UISetCheck(ID_ELIPSE,		false);
		UISetCheck(ID_IMAGE,		true);
		UISetCheck(ID_TEXT,			false);
		UISetCheck(ID_BUTTON,		false);
		UISetCheck(ID_INPUT,		false);
		UISetCheck(ID_SOUND,		false);
		UISetCheck(ID_GRAPH,		false);
		break;
	case P_TEXT:
		UISetCheck(ID_EMPTY,		false);
		UISetCheck(ID_POLYLINE,		false);
		UISetCheck(ID_RECTANGLE,	false);
		UISetCheck(ID_ELIPSE,		false);
		UISetCheck(ID_IMAGE,		false);
		UISetCheck(ID_TEXT,			true);
		UISetCheck(ID_BUTTON,		false);
		UISetCheck(ID_INPUT,		false);
		UISetCheck(ID_SOUND,		false);
		UISetCheck(ID_GRAPH,		false);
		break;
	case P_BUTTON:
		UISetCheck(ID_EMPTY,		false);
		UISetCheck(ID_POLYLINE,		false);
		UISetCheck(ID_RECTANGLE,	false);
		UISetCheck(ID_ELIPSE,		false);
		UISetCheck(ID_IMAGE,		false);
		UISetCheck(ID_TEXT,			false);
		UISetCheck(ID_BUTTON,		true);
		UISetCheck(ID_INPUT,		false);
		UISetCheck(ID_SOUND,		false);
		UISetCheck(ID_GRAPH,		false);
		break;
	case P_INPUT:
		UISetCheck(ID_EMPTY,		false);
		UISetCheck(ID_POLYLINE,		false);
		UISetCheck(ID_RECTANGLE,	false);
		UISetCheck(ID_ELIPSE,		false);
		UISetCheck(ID_IMAGE,		false);
		UISetCheck(ID_TEXT,			false);
		UISetCheck(ID_BUTTON,		false);
		UISetCheck(ID_INPUT,		true);
		UISetCheck(ID_SOUND,		false);
		UISetCheck(ID_GRAPH,		false);
		break;
	case P_SOUND:
		UISetCheck(ID_EMPTY,		false);
		UISetCheck(ID_POLYLINE,		false);
		UISetCheck(ID_RECTANGLE,	false);
		UISetCheck(ID_ELIPSE,		false);
		UISetCheck(ID_IMAGE,		false);
		UISetCheck(ID_TEXT,			false);
		UISetCheck(ID_BUTTON,		false);
		UISetCheck(ID_INPUT,		false);
		UISetCheck(ID_SOUND,		true);
		UISetCheck(ID_GRAPH,		false);
		break;
	case P_GRAPH:
		UISetCheck(ID_EMPTY,		false);
		UISetCheck(ID_POLYLINE,		false);
		UISetCheck(ID_RECTANGLE,	false);
		UISetCheck(ID_ELIPSE,		false);
		UISetCheck(ID_IMAGE,		false);
		UISetCheck(ID_TEXT,			false);
		UISetCheck(ID_BUTTON,		false);
		UISetCheck(ID_INPUT,		false);
		UISetCheck(ID_SOUND,		false);
		UISetCheck(ID_GRAPH,		true);
		break;
	
	}
	UIUpdateToolBar();
	return 0;
}

//Вывод диалога загрузки изображений
LRESULT CMainFrame::OnImagesDlg(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	CProjectImageDlg dlg(&m_Doc.m_Archive);
	dlg.DoModal();
	return 0;
}

LRESULT CMainFrame::OnSoundsDlg(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	CProjectSoundsDlg dlg(&m_Doc.m_Archive);
	dlg.DoModal();
	return 0;
}

//Создание новой схемы
LRESULT CMainFrame::OnNewSchema(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	CInputBox InputBox(L"Input the new circuit name:", L"Create new circuit");
	if(InputBox.DoModal() == IDOK)
	{
		wstring Name = InputBox.GetValAsText();
		if( !Name.empty() )
		{
			//Если есть текущая схема, созраняем ее
			if(!m_Doc.GetCurrentSchemaName().empty())
				m_Doc.SaveCurrentSchema();
			//Создаем новую схему
			m_Doc.NewSchema(Name);
	
			CComboBox box = (CComboBox)m_SchemaBar.GetDlgItem(IDC_SCHEMAS);
			if(box != NULL)
			{
				box.AddString(Name.c_str());
				box.SelectString(0, Name.c_str());
			}
			UpdateViewSettings();
		}
	}
	return 0;
}

//Удалнение схемы
LRESULT CMainFrame::OnDeleteSchema(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	CDeleteSchemaDialog dlg;
	TStrings Names;
	m_Doc.GetSchemaNames(Names);
	wstring CurrentSchema = m_Doc.GetCurrentSchemaName();
	dlg.m_SchemaNames = Names;
	dlg.m_Schema = CurrentSchema.c_str();
	if(dlg.DoModal() == IDOK)
	{
		CComboBox box = (CComboBox)m_SchemaBar.GetDlgItem(IDC_SCHEMAS);
		int idx = box.FindString(-1, (LPCTSTR)dlg.m_Schema);
		box.DeleteString(idx);
		m_Doc.DeleteSchema((wstring)dlg.m_Schema);
		if(CurrentSchema == (wstring)dlg.m_Schema)
		{
			box.SelectString(0, m_Doc.GetCurrentSchemaName().c_str());
			m_Doc.LoadCurrentSchema();
			UpdateViewSettings();
		}
	}
	return 0;
}

//Редактирование настроек текущей схемы
LRESULT CMainFrame::OnSettingCurrentSchema(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	CSchemaDialog dlg;
	dlg.m_Name = m_Doc.GetCurrentSchemaName().c_str();
	sSchemaSettings stgs = m_Doc.GetSchemaSettings();

	dlg.m_FixedSize = stgs.FixedSize;
	dlg.m_HasImageBackgrd = stgs.HasBackgrd;
	dlg.m_StretchBackGrd = stgs.StretchBackGrd;
	dlg.m_Height = stgs.cy;
	dlg.m_Width = stgs.cx;
	dlg.m_ImageName = stgs.BackGrdName.c_str();
	dlg.m_BGDColor = stgs.BackGrdColor;
	TStrings Names;
	m_Doc.GetImageNames(Names);
	dlg.LoadImageNames(Names);

	if(dlg.DoModal() == IDOK)
	{
		sSchemaSettings stgs;
		stgs.FixedSize = dlg.m_FixedSize;
		stgs.HasBackgrd = dlg.m_HasImageBackgrd;
		stgs.StretchBackGrd = dlg.m_StretchBackGrd;
		stgs.cx = dlg.m_Width;
		stgs.cy = dlg.m_Height;
		stgs.BackGrdName = (LPCTSTR)dlg.m_ImageName;
		stgs.BackGrdColor = dlg.m_BGDColor;
		
		if(m_Doc.GetCurrentSchemaName() != (wstring)dlg.m_Name)
		{
			RenameCurrentSchema((wstring)dlg.m_Name);
		}
		m_Doc.SetSchemaSettings(stgs);
		UpdateViewSettings();
	}

	return 0;
}

//Переименование текущей схемы
void CMainFrame::RenameCurrentSchema(wstring NewName)
{
	wstring OldName = m_Doc.GetCurrentSchemaName();
	m_Doc.RenameCurrentSchema(NewName);
	CComboBox box = (CComboBox)m_SchemaBar.GetDlgItem(IDC_SCHEMAS);
	if(box != NULL)
	{
		int idx = box.FindString(-1, OldName.c_str());
		if(idx != CB_ERR )
		{
			box.DeleteString(idx);
			box.InsertString(idx, NewName.c_str());
			box.SelectString(-1, NewName.c_str());
		}
	}
}

//Обновление графического окна с настройками текущей схемы
void CMainFrame::UpdateViewSettings()
{
	sSchemaSettings Settings = m_Doc.GetSchemaSettings();

	m_View.m_IsLimit = Settings.FixedSize;
	m_View.m_WorkArea.x = Settings.cx;
	m_View.m_WorkArea.y = Settings.cy;
	m_View.m_BackGrdColor = Settings.BackGrdColor;


	if((Settings.HasBackgrd) && (!Settings.BackGrdName.empty()))
	{
		::CImage img;
		if(m_Doc.m_Archive.GetImage(Settings.BackGrdName, img))
		{
			m_View.m_IsBGImage = true;
			m_View.m_BGImage = img;
			
			if(Settings.StretchBackGrd)
				m_View.m_StrchBGImage = true;
			m_View.CreateBackground();
		}
	}
	else
	{
		m_View.m_IsBGImage = false;
	}
	m_View.UpdateBackGround();

}


//Выбор текущей схемы
LRESULT CMainFrame::OnCbSelChange(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	m_Doc.SaveCurrentSchema();
	CComboBox box = (CComboBox)m_SchemaBar.GetDlgItem(IDC_SCHEMAS);
	if(box != NULL)
	{
		CWaitCursor wait;
		
		wchar_t buffer[0xff];
		box.GetLBText(box.GetCurSel(), buffer);
		wstring Name(buffer);
		m_Doc.SetCurrentSchemaName(Name);
		m_Doc.LoadCurrentSchema();
		UpdateViewSettings();
		wait.Restore();
	}
	return 0;
}

//Открытие диалога настойки переменных проекта
LRESULT CMainFrame::OnVariables(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	CVariableDialog dlg;
	dlg.DoModal();

	return 0;
}

//Редактирование настроек проекта
LRESULT CMainFrame::OnProjectSettings(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	m_Doc.SaveCurrentSchema();
	CProjectSettingsDialog dlg;
	dlg.m_InitialSchema = m_Doc.GetProjectSettings().InitialSchema.c_str();
	dlg.m_IntervalOPC = (float)m_Doc.GetProjectSettings().IntervalOPC/1000;
	TStrings Names;
	m_Doc.GetSchemaNames(Names);
	dlg.m_SchemaNames = Names;
	if(dlg.DoModal() == IDOK)
	{
		sProjectSettings Settings;
		Settings.InitialSchema = dlg.m_InitialSchema;
		Settings.IntervalOPC = dlg.m_IntervalOPC*1000;
		m_Doc.SetProjectSettings(Settings);
		m_Doc.SaveProjectSettings();
	}
	return true;
}


//Заполнение ComboBox-а выбора схемы названиями схем
void CMainFrame::FillSchemaList()
{
	TStrings Names;
	if(	m_Doc.GetSchemaNames(Names) && !Names.empty())
	{
		CComboBox box = (CComboBox)m_SchemaBar.GetDlgItem(IDC_SCHEMAS);
		if(box != NULL)
		{
			box.ResetContent();
			for(TStrings::iterator it = Names.begin(); it != Names.end(); it++)
				box.AddString(it->c_str());

			box.SelectString(0,  m_Doc.GetCurrentSchemaName().c_str());
		}
		UpdateViewSettings();
	}
}

//Выбор примитивов с помощью диалога
LRESULT CMainFrame::OnSelect(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	CObjectsDialog dlg;
	if(dlg.DoModal() == IDOK)
	{
		if(!dlg.m_ObjectNames.empty())
		{
			CObjectMenagerPtr mng;
			for(TStrings::iterator it = dlg.m_ObjectNames.begin(); 
				it != dlg.m_ObjectNames.end(); it++)
			{
				CObject* obj = mng->FindObject(*it);
				if(mng->GetSelected() == NULL)
					mng->SelectObject(obj);
				else
					mng->AddToSelected(obj);
			}
		}
	}
	return 0;
}

//Перемещение примитива на передний план
LRESULT CMainFrame::OnFront(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	CObjectMenagerPtr mng;
	CObject* obj = mng->GetSelected();
	
	if(obj != NULL)
	{
		DWORD Pos = mng->GenerateOrderPos();
		obj->SetOrderPos(Pos);
		mng->SortObjects();
		mng->SetObjOrderPos();

		if(obj == mng->GetSelection())
		{
			mng->GetSelection()->SetOrderPos(CSelection::POS_FRONT);
		}
		m_View.RedrawWindow();
	}

	return 0;
}

//Перемещение примитива на задний план
LRESULT CMainFrame::OnBack(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	CObjectMenagerPtr mng;

	CObject* obj = mng->GetSelected();

	if(obj != NULL)
	{
		obj->SetOrderPos(0);
		mng->SortObjects();
		mng->SetObjOrderPos();

		if(obj == mng->GetSelection())
		{
			mng->GetSelection()->SetOrderPos(CSelection::POS_BACK);
		}
		m_View.RedrawWindow();
	}

	return 0;
}


LRESULT CMainFrame::OnHelp(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	HelpersLib::CCfgReg reg;
	reg.Open(cProduct, true, false);
	wstring Path = reg.ReadString(L"Help Path");
	if(Path.empty())
	{
		TCHAR buffer[MAX_PATH+1];
		GetModuleFileName(NULL, buffer, MAX_PATH);

		wstring Folder = HelpersLib::ExtractFilePath(buffer) + L"\\Help\\";
		Path = Folder + L"Designer.chm";
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
LRESULT CMainFrame::OnUseGrid(WORD wNotifyCode, WORD wID, HWND hWndCtl)
{
	m_Doc.m_ShowGrid = !m_Doc.m_ShowGrid;
	UISetCheck(ID_USE_GRID, m_Doc.m_ShowGrid?TRUE:FALSE);
	m_View.RedrawWindow();
	return 0;
}
LRESULT CMainFrame::OnCopy(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	CObjectMenagerPtr mng;
	CObject *obj=mng->GetSelected();

/*	CSelection *sel=mng->GetSelection();
	TStrings objs=sel->GetObjects();
	TStrings::iterator it;
	if(!objs.empty())_Clipboard.clear();
	for(it=objs.begin();it!=objs.end();it++)
	{
		CObject *obj=mng->FindObject(*it);
		if(obj)
			_Clipboard.push_back(obj);

	}*/
	return 0;
}
LRESULT CMainFrame::OnCut(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
		return 0;
}
LRESULT CMainFrame::OnPaste(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{

		return 0;
}
