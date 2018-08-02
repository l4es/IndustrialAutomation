#pragma once
/********************************************************************
	Создан:	        2005/02/17
	Создан:	        17.2.2005   17:32
	Путь к файлу: 	c:\Work\Projects\Visual Studio Projects\М-Немо\Source\Designer\SimpleVariableDialog.h
	Директория:	    c:\Work\Projects\Visual Studio Projects\М-Немо\Source\Designer
	Имя файла:	    SimpleVariableDialog
	Расширение:	    h
	Автор(ы):	    Алексей
    Класс(ы):       CSimpleVariableDialog
	
Описание:	Диалог для настройки простой переменной
*********************************************************************/

#include "resource.h"

class CSimpleVariableDialog
	: public CDialogImpl<CSimpleVariableDialog>
	, public CDialogResize<CSimpleVariableDialog>
	, public CWinDataExchange<CSimpleVariableDialog>
{
typedef CSimpleVariableDialog thisClass;
public:
	enum { IDD = IDD_SIMLPE_VARIABLE };

	CString m_Name; //Имя переменой
	CString	m_Type;	//Тип переменной
	CString m_Value; //Значение переменной


	BEGIN_DDX_MAP(thisClass)
		DDX_TEXT(IDC_VALUE_SIMPLE_VARIABLE, m_Value)
		DDX_TEXT(IDC_NAME_SIMPLE_VARIABLE, m_Name)
	END_DDX_MAP()

	BEGIN_MSG_MAP(thisClass)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		MESSAGE_HANDLER(WM_CLOSE, OnClose)
		COMMAND_ID_HANDLER(IDOK, OnCloseCmd)
		COMMAND_ID_HANDLER(IDCANCEL, OnCloseCmd)
		COMMAND_CODE_HANDLER(CBN_SELCHANGE, OnCbnSelchange)
		COMMAND_RANGE_CODE_HANDLER(IDC_VALUE_SIMPLE_VARIABLE, IDC_NAME_SIMPLE_VARIABLE, EN_CHANGE, OnEnChange)
		REFLECT_NOTIFICATIONS()
		CHAIN_MSG_MAP(CDialogResize<thisClass>)
	END_MSG_MAP()

	BEGIN_DLGRESIZE_MAP(thisClass)
		DLGRESIZE_CONTROL(IDC_NAME_SIMPLE_VARIABLE, DLSZ_SIZE_X)
		DLGRESIZE_CONTROL(IDC_VALUE_SIMPLE_VARIABLE, DLSZ_SIZE_X)
	END_DLGRESIZE_MAP()

	CSimpleVariableDialog(): m_Name(""), m_Value("0"), m_Type("Float")
	{

	}

	LRESULT OnCloseCmd(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnClose(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnCbnSelchange(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnEnChange(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/);

	//Проверка, все ли параметры установлены
	bool IsSetAll();
protected:
	
};
