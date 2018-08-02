#pragma once
#include <atlcrack.h>
/********************************************************************
	Создан:	        2005/02/09
	Создан:	        9.2.2005   12:18
	Путь к файлу: 	d:\Work\Projects\М-Немо\Source\Designer\VariableDialog.h
	Директория:	    d:\Work\Projects\М-Немо\Source\Designer
	Имя файла:	    VariableDialog
	Расширение:	    h
	Автор(ы):	    Алексей, Михаил
    Класс(ы):       CVariableDialog
	
Описание:	Диалог для отображения и манипулирование списком переменных
*********************************************************************/


class CVariableDialog
	: public CDialogImpl<CVariableDialog>
	, public CDialogResize<CVariableDialog>
{
	typedef CVariableDialog thisClass;
public:
	enum { IDD = IDD_VARIABLE_DIALOG };

	CListViewCtrl m_List;
		
	BEGIN_MSG_MAP(thisClass)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		MESSAGE_HANDLER(WM_CLOSE, OnClose)
		MESSAGE_HANDLER(WM_SIZE, OnSize)

		COMMAND_ID_HANDLER(ID_ADD_SIMPLE_VARIABLE, OnAddSimpleVariable)
		COMMAND_ID_HANDLER(ID_ADD_OPC_VARIABLE, OnAddOPCVariable)
		COMMAND_ID_HANDLER_EX(ID_IMPORT_OPC_CHANNELS, OnImportOpcChannels)

		COMMAND_ID_HANDLER(IDOK, OnCloseCmd)
		COMMAND_ID_HANDLER(IDC_ADD_VAR, OnAddVariable)
		COMMAND_ID_HANDLER(IDC_DEL_VAR, OnDelVariable)
		COMMAND_ID_HANDLER(IDC_CHANGE_VAR, OnChangeVariable)
		NOTIFY_HANDLER_EX(IDC_VARIABLE_LIST, LVN_ITEMCHANGED, OnItemchanged)

		REFLECT_NOTIFICATIONS()
		CHAIN_MSG_MAP(CDialogResize<thisClass>)
	END_MSG_MAP()

	BEGIN_DLGRESIZE_MAP(thisClass)
		DLGRESIZE_CONTROL(IDC_VARIABLE_LIST, DLSZ_SIZE_X|DLSZ_SIZE_Y)
		DLGRESIZE_CONTROL(IDC_ADD_VAR, DLSZ_MOVE_Y)
		DLGRESIZE_CONTROL(IDC_DEL_VAR, DLSZ_MOVE_Y)
		DLGRESIZE_CONTROL(IDC_CHANGE_VAR, DLSZ_MOVE_Y)
		DLGRESIZE_CONTROL(IDOK, DLSZ_MOVE_X|DLSZ_MOVE_Y)
	END_DLGRESIZE_MAP()


	LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnCloseCmd(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnClose(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnSize(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);

	LRESULT OnAddVariable(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnDelVariable(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnChangeVariable(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	
	LRESULT OnAddSimpleVariable(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnAddOPCVariable(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);


	CComVariant StringToVariant(wstring Value, VARTYPE Type);
	wstring VariantToString(CComVariant Var);
	wstring VARTYPEToString(VARTYPE Type);
	VARTYPE StringToVARTYPE(wstring Type);
	wstring DecodeOPCDesc(TVariableSettings var);
	LRESULT OnItemchanged(LPNMHDR pnmh);
	LRESULT OnImportOpcChannels(WORD wNotifyCode, WORD wID, HWND hWndCtl);
};