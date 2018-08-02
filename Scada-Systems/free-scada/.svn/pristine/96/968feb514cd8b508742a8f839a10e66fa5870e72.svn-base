// MainFrm.h : interface of the CMainFrame class
//
/////////////////////////////////////////////////////////////////////////////
#pragma once
/********************************************************************
	Создан:	        2005/02/17
	Создан:	        17.2.2005   11:00
	Путь к файлу: 	c:\Work\Projects\Visual Studio Projects\М-Немо\Source\Designer\MainFrm.h
	Директория:	    c:\Work\Projects\Visual Studio Projects\М-Немо\Source\Designer
	Имя файла:	    MainFrm
	Расширение:	    h
	Автор(ы):	    Михаил, Алексей
    Класс(ы):       CSchemaBar, CMainFrm
	
Описание: Класс CMainFrm реализует главное окно приложения
*********************************************************************/

#include <atlcrack.h>
#include "PropertyWindow/ObjectPropertyWindow.h"
#include "DesignerView.h"
#include "Doc.h"

//Класс реализует DialogBar с СomboBox-ом для выбора названия схемы
class CSchemaBar : public CDialogImpl<CSchemaBar>
{
public:
	enum { IDD = IDD_SCHEMAS };

	BEGIN_MSG_MAP(CSchemaBar)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		FORWARD_NOTIFICATIONS()
	END_MSG_MAP()

	LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
		return TRUE;
	}
};


//Главное окно программы
class CMainFrame 
	: public dockwins::CDockingFrameImpl<CMainFrame>
	, public CUpdateUI<CMainFrame>
	, public CMessageFilter
	, public CIdleHandler
{
	

	typedef dockwins::CDockingFrameImpl<CMainFrame> baseClass;
public:
	DECLARE_FRAME_WND_CLASS(NULL, IDR_MAINFRAME)

	CDesignerView			m_View; //Окно для рисования графческих объектов
	CObjectPropertyWindow*	m_PropertyWindow; //Окно свойств, отображает и 
								//позволяет редактировать свойства объектов(примитивов)
	CDoc					m_Doc;//Объект документа

	CCommandBarXPCtrl		m_CmdBar;//
	CToolBarCtrl			m_PrimitivesToolBar;//Панель инструментов для выбора 
								//примитива для создания
	CToolBarCtrl			m_AlignToolBar; //Панель инструментов для изменения порядка
								//примитивов на переднем или назаднем плане) и вызова
								//диалоа выбора примитивов

	CSchemaBar				m_SchemaBar; //Панель инструментов с ComboBox-ом выбора схемы

	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL OnIdle();

	BEGIN_UPDATE_UI_MAP(CMainFrame)
		UPDATE_ELEMENT(ID_VIEW_PRIMITIVES, UPDUI_MENUPOPUP)
		UPDATE_ELEMENT(ID_VIEW_TOOLBAR, UPDUI_MENUPOPUP)
		UPDATE_ELEMENT(ID_VIEW_STATUS_BAR, UPDUI_MENUPOPUP)
		UPDATE_ELEMENT(ID_VIEW_PROPERTY, UPDUI_MENUPOPUP)
		UPDATE_ELEMENT(ID_USE_GRID, UPDUI_MENUPOPUP|UPDUI_TOOLBAR)
		UPDATE_ELEMENT(ID_IMAGE, UPDUI_TOOLBAR)
		UPDATE_ELEMENT(ID_POLYLINE, UPDUI_TOOLBAR)
		UPDATE_ELEMENT(ID_RECTANGLE, UPDUI_TOOLBAR)
		UPDATE_ELEMENT(ID_ELIPSE, UPDUI_TOOLBAR)
		UPDATE_ELEMENT(ID_EMPTY, UPDUI_TOOLBAR)
		UPDATE_ELEMENT(ID_TEXT, UPDUI_TOOLBAR)
		UPDATE_ELEMENT(ID_BUTTON, UPDUI_TOOLBAR)
		UPDATE_ELEMENT(ID_INPUT, UPDUI_TOOLBAR)
		UPDATE_ELEMENT(ID_SOUND, UPDUI_TOOLBAR)
		UPDATE_ELEMENT(ID_GRAPH, UPDUI_TOOLBAR)
	END_UPDATE_UI_MAP()

	BEGIN_MSG_MAP(CMainFrame)
		MESSAGE_HANDLER(WM_CREATE, OnCreate)
		MESSAGE_HANDLER(WM_POSTCREATE, OnPostCreate)
		MESSAGE_HANDLER(WM_HELP, OnHelp)
		MESSAGE_HANDLER(WM_UPDATEVIEW, OnUpdateView)
		MESSAGE_HANDLER(WM_UPDATE_PRIMITIVES_TOOLBAR, OnUpdatePrimitivesToolbar)
		COMMAND_ID_HANDLER(ID_APP_EXIT, OnFileExit)
		COMMAND_ID_HANDLER(ID_HELP, OnIdHelp)
		COMMAND_ID_HANDLER(ID_FILE_NEW, OnFileNew)
		COMMAND_ID_HANDLER(ID_FILE_SAVE, OnFileSave)
		COMMAND_ID_HANDLER(ID_FILE_SAVEAS, OnFileSaveAs)
		COMMAND_ID_HANDLER(ID_VIEW_TOOLBAR, OnViewToolBar)
		COMMAND_ID_HANDLER(ID_VIEW_PRIMITIVES, OnViewPrimitivesBar)
		COMMAND_ID_HANDLER(ID_VIEW_STATUS_BAR, OnViewStatusBar)
		COMMAND_ID_HANDLER(ID_VIEW_PROPERTY, OnViewProperty)
		COMMAND_ID_HANDLER(ID_APP_ABOUT, OnAppAbout)
		COMMAND_ID_HANDLER(ID_EMPTY, OnCreateEmpty)
		COMMAND_ID_HANDLER(ID_POLYLINE, OnCreatePolyline)
		COMMAND_ID_HANDLER(ID_RECTANGLE, OnCreateRectangle)
		COMMAND_ID_HANDLER(ID_ELIPSE, OnCreateEllipse)
		COMMAND_ID_HANDLER(ID_IMAGE, OnCreateImage)
		COMMAND_ID_HANDLER(ID_TEXT, OnCreateText)
		COMMAND_ID_HANDLER(ID_BUTTON, OnCreateButton)
		COMMAND_ID_HANDLER(ID_INPUT, OnCreateInput)
		COMMAND_ID_HANDLER(ID_SOUND, OnCreateSound)
		COMMAND_ID_HANDLER(ID_GRAPH, OnCreateGraph)
		COMMAND_ID_HANDLER(ID_FILE_OPEN, OnFileOpen)
		COMMAND_ID_HANDLER(ID_IMAGES, OnImagesDlg)
		COMMAND_ID_HANDLER(ID_SOUNDS, OnSoundsDlg)
		COMMAND_ID_HANDLER(ID_NEW_SCHEMA, OnNewSchema)
		COMMAND_ID_HANDLER(ID_DELETE_SCHEMA, OnDeleteSchema)
		COMMAND_ID_HANDLER(ID_SETTING_CURRENT, OnSettingCurrentSchema)
		COMMAND_ID_HANDLER(ID_VARIABLES, OnVariables)
		COMMAND_ID_HANDLER(ID_PROJECT_SETTINGS, OnProjectSettings)
		COMMAND_ID_HANDLER(ID_BACK, OnBack)
		COMMAND_ID_HANDLER(ID_FRONT, OnFront)
		COMMAND_ID_HANDLER(ID_SELECT, OnSelect)
		COMMAND_ID_HANDLER(ID_COPY, OnCopy)
		COMMAND_ID_HANDLER(ID_CUT, OnCut)
		COMMAND_ID_HANDLER(ID_PASTE, OnPaste)
		COMMAND_HANDLER(IDC_SCHEMAS, CBN_SELCHANGE, OnCbSelChange)
		COMMAND_ID_HANDLER_EX(ID_USE_GRID, OnUseGrid)
		
		CHAIN_MSG_MAP(CUpdateUI<CMainFrame>)
		
		CHAIN_MSG_MAP(baseClass)
		
		
	END_MSG_MAP()

// Handler prototypes (uncomment arguments if needed):
//	LRESULT MessageHandler(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
//	LRESULT CommandHandler(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
//	LRESULT NotifyHandler(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/)


//Вывод диалога загрузки изображений
	LRESULT OnImagesDlg(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnSoundsDlg(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnHelp(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnPostCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnUpdateView(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnFileExit(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnFileNew(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnIdHelp(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnFileSave(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnFileSaveAs(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnViewToolBar(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnViewPrimitivesBar(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnViewStatusBar(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnViewProperty(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnAppAbout(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
//Установка режима при котором не создаются примитивы
	LRESULT OnCreateEmpty(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);

//Установка режима создания примитива
	LRESULT OnCreatePolyline(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnCreateRectangle(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnCreateEllipse(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnCreateImage(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnCreateText(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnCreateButton(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnCreateInput(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnCreateSound(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnCreateGraph(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);

//Загрузка нового проекта
	LRESULT OnFileOpen(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
//Обновление панели инструментов
	LRESULT OnUpdatePrimitivesToolbar(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
//Создание новой схемы
	LRESULT OnNewSchema(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
//Удаление схемы
	LRESULT OnDeleteSchema(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
//Открытие диалога настройки текущей схемы
	LRESULT OnSettingCurrentSchema(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
//Выбор текущей схемы
	LRESULT OnCbSelChange(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
//Открытие диалога настойки переменных проекта
	LRESULT OnVariables(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
//Редактирование настроек проекта
	LRESULT OnProjectSettings(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
//Выбор примитивов с помощью диалога
	LRESULT OnSelect(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
//Перемещение примитива на задний план
	LRESULT OnBack(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
//Перемещение примитива на передний план
	LRESULT OnFront(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);

	LRESULT OnCopy(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnCut(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnPaste(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);

//Обновление  графического окна с настройками текущей схемы
	void UpdateViewSettings();
//Переименование текущей схемы
	void RenameCurrentSchema(wstring NewName);
	
	HWND CreateRebar();
//Заполнение ComboBox-а выбора схемы названиями схем
	void FillSchemaList();
	CMainFrame();
	~CMainFrame();
	LRESULT OnUseGrid(WORD wNotifyCode, WORD wID, HWND hWndCtl);
	list<CObject*> _Clipboard;
};

extern CMainFrame *g_MainFrame;