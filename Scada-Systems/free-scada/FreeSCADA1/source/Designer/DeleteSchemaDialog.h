#pragma once
/********************************************************************
	Создан:	        2005/02/09
	Создан:	        9.2.2005   17:02
	Путь к файлу: 	c:\Work\Projects\Visual Studio Projects\М-Немо\Source\Designer\DeleteSchemaDialog.h
	Директория:	    c:\Work\Projects\Visual Studio Projects\М-Немо\Source\Designer
	Имя файла:	    DeleteSchemaDialog
	Расширение:	    h
	Автор(ы):	    Алексей
    Класс(ы):       CDeleteSchemaDialog
	
Описание:	Класс диалога для удаления схемы из проекта
*********************************************************************/

class CDeleteSchemaDialog
	: public CDialogImpl<CDeleteSchemaDialog>
{
public:
	typedef CDeleteSchemaDialog thisClass;

	TStrings m_SchemaNames; //Список названий схем
	CString m_Schema;		//Схема для удаления
	enum { IDD = IDD_DELETE_SCHEMA };

	BEGIN_MSG_MAP(thisClass)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		MESSAGE_HANDLER(WM_CLOSE, OnClose)
		COMMAND_ID_HANDLER(IDOK, OnCloseCmd)
		COMMAND_ID_HANDLER(IDCANCEL, OnCloseCmd)
		COMMAND_HANDLER(IDC_COMBO_NAMES, CBN_SELCHANGE, OnCbnSelchange)

		REFLECT_NOTIFICATIONS()
	END_MSG_MAP()

	LRESULT OnClose(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnCloseCmd(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnCbnSelchange(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
};
