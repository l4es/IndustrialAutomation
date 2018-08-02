#pragma once
/********************************************************************
	Создан:	        2005/02/08
	Создан:	        8.2.2005   12:25
	Путь к файлу: 	d:\Work\Projects\М-Немо\Source\Visualizator\SettingsDlg.h
	Директория:	    d:\Work\Projects\М-Немо\Source\Visualizator
	Имя файла:	    SettingsDlg
	Расширение:	    h
	Автор(ы):	    Михаил
    Класс(ы):       CSettingsDlg
	
Описание:	Диалог для настройки параметров визуализатора.
*********************************************************************/

#include <atlcrack.h>
#include "resource.h"

class CSettingsDlg
	: public CDialogImpl<CSettingsDlg>
	, public CWinDataExchange<CSettingsDlg>
{
	typedef CSettingsDlg thisClass;
public:
	enum { IDD = IDD_SETTINGS };

	int m_FileAutoRun;		//Флаг "автозагрузка файла"
	int m_AutoRun;			//Флаг "автозапуск программы"
	CString m_FileName;		//Имя файла для автозагрузки

	BEGIN_MSG_MAP(thisClass)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		COMMAND_ID_HANDLER(IDOK, OnCloseCmd)
		COMMAND_ID_HANDLER(IDCANCEL, OnCloseCmd)
		DEFAULT_REFLECTION_HANDLER()
		COMMAND_HANDLER(IDC_FILE_AUTORUN, BN_CLICKED, OnBnClickedFileAutorun)
		COMMAND_HANDLER(IDC_SELECT_FILE, BN_CLICKED, OnBnClicked)
	END_MSG_MAP()

	BEGIN_DDX_MAP(thisClass)
		DDX_CHECK(IDC_FILE_AUTORUN, m_FileAutoRun)
		DDX_CHECK(IDC_AUTORUN, m_AutoRun)
		DDX_TEXT(IDC_FILENAME, m_FileName)
	END_DDX_MAP()

	LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnCloseCmd(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnBnClickedFileAutorun(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnBnClicked(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
};


namespace ConfigFunc
{
	//Функция возвращает true, если включена опция автозагрузки файла
	inline bool FileAutoRunIsOn();

	//Получить полное имя автозагружаемого файла
	inline wstring FileAutoRun();

	//Включить опцию автозагрузки для указанного файла
	inline void SetFileAutoRun(wstring FileName);
}