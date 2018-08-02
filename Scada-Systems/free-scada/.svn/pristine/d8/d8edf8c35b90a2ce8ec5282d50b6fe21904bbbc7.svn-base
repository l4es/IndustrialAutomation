#pragma once

/********************************************************************
	Создан:	        2005/02/17
	Создан:	        17.2.2005   16:09
	Путь к файлу: 	c:\Work\Projects\Visual Studio Projects\М-Немо\Source\Designer\SelectChannelDialog.h
	Директория:	    c:\Work\Projects\Visual Studio Projects\М-Немо\Source\Designer
	Имя файла:	    SelectChannelDialog
	Расширение:	    h
	Автор(ы):	    Михаил
    Класс(ы):       CSelectChannelDialog, sServerInfo
	
Описание:	
*********************************************************************/

struct sServerInfo
{
	CComBSTR	m_Computer;
	CLSID		m_ServerCLSID;
};

class CSelectChannelDialog
	: public CDialogImpl<CSelectChannelDialog>
	, public CDialogResize<CSelectChannelDialog>
	, public CWinDataExchange<CSelectChannelDialog>
{
public:
	typedef CSelectChannelDialog thisClass;

	enum { IDD = IDD_SELECT_CHANNEL };

	CTreeViewCtrl m_Tree;
	CString m_Computer; //Имя компьютера
	CString m_Channel;	//Имя тэга
	CLSID	m_OPCServerCLSID; //CLSID сервера

	BEGIN_DDX_MAP(thisClass)

	END_DDX_MAP()

	BEGIN_MSG_MAP(thisClass)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		MESSAGE_HANDLER(WM_CLOSE, OnClose)
		COMMAND_ID_HANDLER(IDOK, OnCloseCmd)
		COMMAND_ID_HANDLER(IDCANCEL, OnCloseCmd)

		NOTIFY_HANDLER(IDC_TREE_CHANNEL, TVN_SELCHANGED, OnTvnSelChanged)
		
		REFLECT_NOTIFICATIONS()
		CHAIN_MSG_MAP(CDialogResize<thisClass>)
	END_MSG_MAP()

	BEGIN_DLGRESIZE_MAP(thisClass)
		//	DLGRESIZE_CONTROL(IDC_VARIABLE_LIST, DLSZ_SIZE_X|DLSZ_SIZE_Y)
		//	DLGRESIZE_CONTROL(IDC_ADD_VAR, DLSZ_MOVE_Y)
		//	DLGRESIZE_CONTROL(IDC_DEL_VAR, DLSZ_MOVE_Y)
		//	DLGRESIZE_CONTROL(IDC_CHANGE_VAR, DLSZ_MOVE_Y)
	END_DLGRESIZE_MAP()

	CSelectChannelDialog(void) : m_Channel(""){}
	LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnCloseCmd(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnClose(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnTvnSelChanged(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/);
	/// Fill tags tree for HIERARCHIAL OPC
	void FillTree(HTREEITEM root, CComPtr<IOPCBrowseServerAddressSpace> sas);
	/// Fill tags tree for FLAT OPC
	void FillTreeFlat(HTREEITEM root, CComPtr<IOPCBrowseServerAddressSpace> sas);
	//Попытка создать объект на удаленном компьютере 
	HRESULT PingRemoteServer(CString Computer, CLSID ServerOPOC, DWORD nTimeout);
};
