#pragma once
/********************************************************************
	Создан:	        2005/02/17
	Создан:	        17.2.2005   12:31
	Путь к файлу: 	c:\Work\Projects\Visual Studio Projects\М-Немо\Source\Designer\OPCVariableDialog.h
	Директория:	    c:\Work\Projects\Visual Studio Projects\М-Немо\Source\Designer
	Имя файла:	    OPCVariableDialog
	Расширение:	    h
	Автор(ы):	    Михаил, Алексей
    Класс(ы):       COPCVariableDialog
	
Описание:	Класс диалога для настроек OPC переменных
*********************************************************************/

#include <atlcrack.h>

class COPCVariableDialog
	: public CDialogImpl<COPCVariableDialog>
	, public CDialogResize<COPCVariableDialog>
	, public CWinDataExchange<COPCVariableDialog>

	
{
public:
	typedef COPCVariableDialog thisClass;

	enum { IDD = IDD_OPC_VARIABLE };

	CString m_Name;			//Имя переменной
	CString	m_Type;			//Тип переменной
	CString m_ComputerName;	//Имя компьютера
	CString m_ServerName;	//Имя сервера
	CString m_ChannelName;	//Имя канала
	GUID	m_ServerGUID;	//UUID сервера
	
	COPCVariableDialog(void) 
		: m_ComputerName("localhost")
		, m_ServerName("")
	{}


	BEGIN_DDX_MAP(thisClass)
		DDX_TEXT(IDC_NAME_OPC_VARIABLE, m_Name)
		DDX_TEXT(IDC_NAME_COMPUTER, m_ComputerName)
		DDX_TEXT(IDC_NAME_CHANNEL, m_ChannelName)
	END_DDX_MAP()

	BEGIN_MSG_MAP(thisClass)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		MESSAGE_HANDLER(WM_CLOSE, OnClose)
		MSG_WM_GETMINMAXINFO(OnGetMinMaxInfo)
		COMMAND_ID_HANDLER(IDOK, OnCloseCmd)
		COMMAND_ID_HANDLER(IDCANCEL, OnCloseCmd)
		COMMAND_ID_HANDLER(IDC_BROWSE_COMPUTER, OnBrowseComputer)
		COMMAND_ID_HANDLER(IDC_BROWSE_CHANNEL, OnBrowseChannel)
		COMMAND_HANDLER(IDC_NAME_SERVER, CBN_SELCHANGE, OnServerSelchange)
		COMMAND_HANDLER(IDC_TYPE_VARIABLE, CBN_SELCHANGE, OnTypeSelchange)
		COMMAND_HANDLER(IDC_NAME_SERVER, CBN_DROPDOWN, OnCbnDropDown)
		COMMAND_HANDLER(IDC_NAME_COMPUTER, EN_CHANGE, OnEnChange)
		COMMAND_HANDLER(IDC_NAME_OPC_VARIABLE, EN_CHANGE, OnNameEnChange)

		REFLECT_NOTIFICATIONS()
		CHAIN_MSG_MAP(CDialogResize<thisClass>)
	END_MSG_MAP()

	BEGIN_DLGRESIZE_MAP(thisClass)
		DLGRESIZE_CONTROL(IDC_NAME_OPC_VARIABLE, DLSZ_SIZE_X)
		DLGRESIZE_CONTROL(IDC_TYPE_VARIABLE, DLSZ_SIZE_X)
		DLGRESIZE_CONTROL(IDC_GROUP, DLSZ_SIZE_X)
		DLGRESIZE_CONTROL(IDC_NAME_COMPUTER, DLSZ_SIZE_X)
		DLGRESIZE_CONTROL(IDC_NAME_SERVER, DLSZ_SIZE_X)
		DLGRESIZE_CONTROL(IDC_NAME_CHANNEL, DLSZ_SIZE_X)
		DLGRESIZE_CONTROL(IDC_BROWSE_COMPUTER, DLSZ_MOVE_X)
		DLGRESIZE_CONTROL(IDC_BROWSE_CHANNEL, DLSZ_MOVE_X)
		DLGRESIZE_CONTROL(IDOK, DLSZ_MOVE_X|DLSZ_MOVE_Y)
		DLGRESIZE_CONTROL(IDCANCEL, DLSZ_MOVE_X|DLSZ_MOVE_Y)
	END_DLGRESIZE_MAP()

	LRESULT OnCloseCmd(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnClose(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	//Выбор сервера
	LRESULT OnServerSelchange(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	//Выбор типа переменной
	LRESULT OnTypeSelchange(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	//Заполнение списка серверов
	LRESULT OnCbnDropDown(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	//Получение имени компьютера, занесение имени в Edit
	LRESULT OnBrowseComputer(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	//Вызов диалога выбора канала
	LRESULT OnBrowseChannel(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	//Получение имени компьютера при вписывании его "вручную"
	LRESULT OnEnChange(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	//Изменение названия переменной
	LRESULT OnNameEnChange(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	//Вызов диалога выбора компьютера
	CString BrowseComputer();
	//Заполнение ComboBox-а списком серверов
	void FillListServer();
	//Проверка заполнены ли все поля для ОРС переменной
	bool IsSetAll();
	//Обновление контролов отображающих название сервера и тэга
	void UpdateCtrls();

	LRESULT OnBnClickedBrowseChannel(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnGetMinMaxInfo(LPMINMAXINFO lpMinMaxInfo);
};
