#pragma once
#include <atlcrack.h>
/********************************************************************
	Создан:	        2005/02/17
	Создан:	        17.2.2005   12:56
	Путь к файлу: 	c:\Work\Projects\Visual Studio Projects\М-Немо\Source\Designer\ProjectSoundsDlg.h
	Директория:	    c:\Work\Projects\Visual Studio Projects\М-Немо\Source\Designer
	Имя файла:	    ProjectImageDlg
	Расширение:	    h
	Автор(ы):	    Михаил
    Класс(ы):       CProjectSoundsDlg
	
Описание:	Диалог для работы со звуками проекта
*********************************************************************/

class CProjectSoundsDlg
	: public CDialogImpl<CProjectSoundsDlg>
	, public CDialogResize<CProjectSoundsDlg>
	, public CUpdateUI<CProjectSoundsDlg>
{
	typedef CProjectSoundsDlg thisClass;

	CArchiveManager* _Menager;
public:
	enum { IDD = IDD_PROJECT_SOUNDS };

	CProjectSoundsDlg(CArchiveManager *mng)
		: _Menager(mng){};

	BEGIN_UPDATE_UI_MAP(thisClass)
		UPDATE_ELEMENT(ID_VIEW_PRIMITIVES, UPDUI_CHILDWINDOW)
		UPDATE_ELEMENT(IDC_LOAD_FROM_FILE, UPDUI_CHILDWINDOW)
		UPDATE_ELEMENT(IDC_RENAME, UPDUI_CHILDWINDOW)
		UPDATE_ELEMENT(IDC_DELETE, UPDUI_CHILDWINDOW)
		UPDATE_ELEMENT(IDC_PLAY, UPDUI_CHILDWINDOW)		
		UPDATE_ELEMENT(IDC_STOP, UPDUI_CHILDWINDOW)		
	END_UPDATE_UI_MAP()

	BEGIN_MSG_MAP(thisClass)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		COMMAND_ID_HANDLER(IDOK, OnCloseCmd)
		COMMAND_ID_HANDLER(IDCANCEL, OnCloseCmd)
		COMMAND_HANDLER(IDC_LOAD_FROM_FILE, BN_CLICKED, OnBnClickedLoadFromFile)
		COMMAND_HANDLER(IDC_RENAME, BN_CLICKED, OnBnClickedRename)
		COMMAND_HANDLER(IDC_IMAGE_LIST, LBN_SELCHANGE, OnLbnSelchangeImageList)
		COMMAND_HANDLER(IDC_DELETE, BN_CLICKED, OnBnClickedDelete)
		COMMAND_HANDLER_EX(IDC_PLAY, BN_CLICKED, OnPlayBnClicked)
		COMMAND_HANDLER_EX(IDC_STOP, BN_CLICKED, OnStopBnClicked)
		CHAIN_MSG_MAP(CDialogResize<thisClass>)
		CHAIN_MSG_MAP(CUpdateUI<thisClass>)
		DEFAULT_REFLECTION_HANDLER()
	END_MSG_MAP()

	BEGIN_DLGRESIZE_MAP(thisClass)
		DLGRESIZE_CONTROL(IDC_IMAGE_LIST, DLSZ_SIZE_X|DLSZ_SIZE_Y)
		DLGRESIZE_CONTROL(IDC_LOAD_FROM_FILE,	DLSZ_MOVE_X)
		DLGRESIZE_CONTROL(IDC_RENAME,			DLSZ_MOVE_X)
		DLGRESIZE_CONTROL(IDC_DELETE,			DLSZ_MOVE_X)
		DLGRESIZE_CONTROL(IDC_PLAY,				DLSZ_MOVE_X)
		DLGRESIZE_CONTROL(IDOK,					DLSZ_MOVE_Y|DLSZ_MOVE_X)
	END_DLGRESIZE_MAP()


	// Handler prototypes (uncomment arguments if needed):
	//	LRESULT MessageHandler(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	//	LRESULT CommandHandler(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	//	LRESULT NotifyHandler(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/)
	
	//Заполнение списка именами изображений которые уже есть в проекте
	void FillList();

	LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnCloseCmd(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	//Загрузка изображения из файла
	LRESULT OnBnClickedLoadFromFile(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	//Переименование изображения загруженного в проект
	LRESULT OnBnClickedRename(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	//Выделение изображения в ListBox-е, заполнение просмотра изображения
	LRESULT OnLbnSelchangeImageList(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	//Удаление изображения
	LRESULT OnBnClickedDelete(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnPlayBnClicked(WORD wNotifyCode, WORD wID, HWND hWndCtl);
	LRESULT OnStopBnClicked(WORD wNotifyCode, WORD wID, HWND hWndCtl);

protected:
	CSound _Sound;
};
