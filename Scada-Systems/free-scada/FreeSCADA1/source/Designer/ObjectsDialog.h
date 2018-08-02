#pragma once
/********************************************************************
	Создан:	        2005/02/17
	Создан:	        17.2.2005   12:13
	Путь к файлу: 	c:\Work\Projects\Visual Studio Projects\М-Немо\Source\Designer\ObjectsDialog.h
	Директория:	    c:\Work\Projects\Visual Studio Projects\М-Немо\Source\Designer
	Имя файла:	    ObjectsDialog
	Расширение:	    h
	Автор(ы):	    Алексей
    Класс(ы):       CObjectsDialog
	
Описание:	Класс, реализующий диалог выбора примитивов по имени
*********************************************************************/

class CObjectsDialog
	: public CDialogImpl<CObjectsDialog>
	, public CDialogResize<CObjectsDialog>
	, public CWinDataExchange<CObjectsDialog>
{
public:
	typedef CObjectsDialog thisClass;

	enum { IDD = IDD_OBJECTS };

	CObjectsDialog(void){};
	BEGIN_DDX_MAP(thisClass)

	END_DDX_MAP()

	BEGIN_MSG_MAP(thisClass)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		MESSAGE_HANDLER(WM_CLOSE, OnClose)
		COMMAND_ID_HANDLER(IDOK, OnCloseCmd)
		COMMAND_ID_HANDLER(IDCANCEL, OnCloseCmd)
		REFLECT_NOTIFICATIONS()
		CHAIN_MSG_MAP(CDialogResize<thisClass>)
	END_MSG_MAP()

	BEGIN_DLGRESIZE_MAP(thisClass)
		DLGRESIZE_CONTROL(IDC_LIST_OBJECTS, DLSZ_SIZE_X | DLSZ_SIZE_Y)
		DLGRESIZE_CONTROL(IDOK, DLSZ_MOVE_Y)
		DLGRESIZE_CONTROL(IDCANCEL, DLSZ_MOVE_X|DLSZ_MOVE_Y)
	END_DLGRESIZE_MAP()

	LRESULT OnCloseCmd(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnClose(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);

	CListBox m_List;
	TStrings m_ObjectNames; //Список с именами примитивов
};
