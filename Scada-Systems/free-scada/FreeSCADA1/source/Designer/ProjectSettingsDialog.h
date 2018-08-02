#pragma once
/********************************************************************
	Создан:	        2005/02/17
	Создан:	        17.2.2005   13:40
	Путь к файлу: 	c:\Work\Projects\Visual Studio Projects\М-Немо\Source\Designer\ProjectSettingsDialog.h
	Директория:	    c:\Work\Projects\Visual Studio Projects\М-Немо\Source\Designer
	Имя файла:	    ProjectSettingsDialog
	Расширение:	    h
	Автор(ы):	    Михаил
    Класс(ы):       CProjectSettingsDialog
	
Описание:	Класс диалога настроек проекта
*********************************************************************/

class CProjectSettingsDialog
	: public CDialogImpl<CProjectSettingsDialog>
	, public CWinDataExchange<CProjectSettingsDialog>
{
public:
	typedef CProjectSettingsDialog thisClass;

	enum { IDD = IDD_PROJECT_SETTINGS };

	TStrings m_SchemaNames;		//Список названий схем
	CString m_InitialSchema;	//Начальная схема
	float m_IntervalOPC;		//Интервал опроса ОРС сервера
	int m_AutoLoad;

	CProjectSettingsDialog()
		: m_IntervalOPC(5)
		, m_InitialSchema("")
		, m_AutoLoad(0){}

	BEGIN_DDX_MAP(thisClass)
		DDX_FLOAT(IDC_INTERVAL_OPC, m_IntervalOPC)
		DDX_RADIO(IDC_RADIO1, m_AutoLoad)
	END_DDX_MAP()

	BEGIN_MSG_MAP(thisClass)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		MESSAGE_HANDLER(WM_CLOSE, OnClose)
		COMMAND_ID_HANDLER(IDOK, OnCloseCmd)
		COMMAND_ID_HANDLER(IDCANCEL, OnCloseCmd)
		COMMAND_HANDLER(IDC_COMBO_NAMES, CBN_SELCHANGE, OnCbnSelchange)
		COMMAND_RANGE_CODE_HANDLER(IDC_RADIO1, IDC_RADIO2, BN_CLICKED, OnBnClickedAutoLoad)

		REFLECT_NOTIFICATIONS()
	END_MSG_MAP()

	LRESULT OnClose(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnCloseCmd(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnCbnSelchange(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/);

	LRESULT OnBnClickedAutoLoad(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
};
