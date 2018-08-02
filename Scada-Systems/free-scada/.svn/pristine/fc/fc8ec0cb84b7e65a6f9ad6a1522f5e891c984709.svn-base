#pragma once
/********************************************************************
	Создан:	        2005/02/17
	Создан:	        17.2.2005   12:56
	Путь к файлу: 	c:\Work\Projects\Visual Studio Projects\М-Немо\Source\Designer\ProjectImageDlg.h
	Директория:	    c:\Work\Projects\Visual Studio Projects\М-Немо\Source\Designer
	Имя файла:	    ProjectImageDlg
	Расширение:	    h
	Автор(ы):	    Михаил
    Класс(ы):       CProjectImageDlg
	
Описание:	Диалог для работы с изображениями проекта
*********************************************************************/

class CProjectImageDlg
	: public CDialogImpl<CProjectImageDlg>
	, public CDialogResize<CProjectImageDlg>
	, public CWinDataExchange<CProjectImageDlg>
{
	typedef CProjectImageDlg thisClass;

	CArchiveManager* _Menager;
public:
	enum { IDD = IDD_PROJECT_IMAGES };

	
	UINT m_Width; //Ширина выбранного изображения
	UINT m_Height;	//Высота выбранного изображения
	CBitmap m_Preview; //Bitmap просмотра изображения

	CProjectImageDlg(CArchiveManager *mng)
		: _Menager(mng)
		, m_Width(0)
		, m_Height(0){};

	BEGIN_DDX_MAP(thisClass)
		DDX_UINT(IDC_WIDTH, m_Width)
		DDX_UINT(IDC_HEIGHT, m_Height)
	END_DDX_MAP()

	BEGIN_MSG_MAP(thisClass)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		COMMAND_ID_HANDLER(IDOK, OnCloseCmd)
		COMMAND_ID_HANDLER(IDCANCEL, OnCloseCmd)
		COMMAND_HANDLER(IDC_LOAD_FROM_FILE, BN_CLICKED, OnBnClickedLoadFromFile)
		COMMAND_HANDLER(IDC_RENAME, BN_CLICKED, OnBnClickedRename)
		COMMAND_HANDLER(IDC_IMAGE_LIST, LBN_SELCHANGE, OnLbnSelchangeImageList)
		COMMAND_HANDLER(IDC_DELETE, BN_CLICKED, OnBnClickedDelete)
		MESSAGE_HANDLER(WM_PAINT, OnPaint)
		CHAIN_MSG_MAP(CDialogResize<thisClass>)
		DEFAULT_REFLECTION_HANDLER()
	END_MSG_MAP()

	BEGIN_DLGRESIZE_MAP(thisClass)

		DLGRESIZE_CONTROL(IDC_IMAGE_LIST, DLSZ_SIZE_X|DLSZ_SIZE_Y)
		DLGRESIZE_CONTROL(IDC_PREVIEW, DLSZ_MOVE_X)

		DLGRESIZE_CONTROL(IDC_LOAD_FROM_FILE,	DLSZ_MOVE_Y|DLSZ_MOVE_X|DLSZ_SIZE_X)
		DLGRESIZE_CONTROL(IDC_RENAME,			DLSZ_MOVE_Y|DLSZ_MOVE_X|DLSZ_SIZE_X)
		DLGRESIZE_CONTROL(IDC_DELETE,			DLSZ_MOVE_Y|DLSZ_MOVE_X|DLSZ_SIZE_X)

		DLGRESIZE_CONTROL(IDC_GROUP,			DLSZ_MOVE_X|DLSZ_SIZE_X)
		DLGRESIZE_CONTROL(IDC_STATIC_W,			DLSZ_MOVE_X)
		DLGRESIZE_CONTROL(IDC_STATIC_H,			DLSZ_MOVE_X)
		DLGRESIZE_CONTROL(IDC_WIDTH,			DLSZ_MOVE_X)
		DLGRESIZE_CONTROL(IDC_HEIGHT,			DLSZ_MOVE_X)

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
	//Рисование Bitmap-а
	LRESULT OnPaint(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
};
