#pragma once
/********************************************************************
	Создан:	        2005/02/17
	Создан:	        17.2.2005   12:31
	Путь к файлу: 	c:\Work\Projects\Visual Studio Projects\М-Немо\Source\Designer\OPCVariableDialog.h
	Директория:	    c:\Work\Projects\Visual Studio Projects\М-Немо\Source\Designer
	Имя файла:	    OPCVariableDialog
	Расширение:	    h
	Автор(ы):	    Михаил, Алексей
    Класс(ы):       CImportOPCChannelsDlg
	
Описание:	Класс диалога для настроек OPC переменных
*********************************************************************/

#include <atlcrack.h>

class CImportOPCChannelsDlg
	: public CDialogImpl<CImportOPCChannelsDlg>
	, public CWinDataExchange<CImportOPCChannelsDlg>
{
public:
	typedef CImportOPCChannelsDlg thisClass;

	enum { IDD = IDD_IMPORT_OPC_CHANNELS };

	CString m_Prefix;		//Префикс переменной
	CString m_ComputerName;	//Имя компьютера
	CString m_ServerName;	//Имя сервера
	GUID	m_ServerGUID;	//UUID сервера
	
	CImportOPCChannelsDlg(void) 
		: m_ComputerName("localhost")
		, m_ServerName("")
	{}


	BEGIN_DDX_MAP(thisClass)
		DDX_TEXT(IDC_VARIABLE_PREFIX, m_Prefix)
		DDX_TEXT(IDC_NAME_COMPUTER, m_ComputerName)
	END_DDX_MAP()

	BEGIN_MSG_MAP(thisClass)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		MESSAGE_HANDLER(WM_CLOSE, OnClose)
		COMMAND_ID_HANDLER(IDOK, OnCloseCmd)
		COMMAND_ID_HANDLER(IDCANCEL, OnCloseCmd)
		COMMAND_ID_HANDLER(IDC_BROWSE_COMPUTER, OnBrowseComputer)
		COMMAND_HANDLER(IDC_NAME_SERVER, CBN_SELCHANGE, OnServerSelchange)
		COMMAND_HANDLER(IDC_NAME_SERVER, CBN_DROPDOWN, OnCbnDropDown)
		COMMAND_HANDLER(IDC_NAME_COMPUTER, EN_CHANGE, OnEnChange)
		COMMAND_HANDLER(IDC_VARIABLE_PREFIX, EN_CHANGE, OnPrefixEnChange)

		REFLECT_NOTIFICATIONS()
	END_MSG_MAP()

	LRESULT OnCloseCmd(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnClose(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	//Выбор сервера
	LRESULT OnServerSelchange(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	//Заполнение списка серверов
	LRESULT OnCbnDropDown(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	//Получение имени компьютера, занесение имени в Edit
	LRESULT OnBrowseComputer(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	//Вызов диалога выбора канала
	LRESULT OnBrowseChannel(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	//Получение имени компьютера при вписывании его "вручную"
	LRESULT OnEnChange(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	//Изменение названия переменной
	LRESULT OnPrefixEnChange(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	//Вызов диалога выбора компьютера
	CString BrowseComputer();
	//Заполнение ComboBox-а списком серверов
	void FillListServer();
	//Проверка заполнены ли все поля
	bool IsSetAll();
	//Обновление контролов отображающих название сервера и тэга
	void UpdateCtrls();
};
