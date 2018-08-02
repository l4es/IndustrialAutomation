#pragma once

class CSchemaDialog
	: public CDialogImpl<CSchemaDialog>
	, public CDialogResize<CSchemaDialog>
	, public CWinDataExchange<CSchemaDialog>
	, public CUpdateUI<CSchemaDialog>
{
	typedef CSchemaDialog thisClass;


public:
	enum { IDD = IDD_SCHEMA_SETTINGS };

	CString m_Name;
	bool	m_FixedSize;
	bool	m_HasImageBackgrd;
	UINT	m_Width;
	UINT	m_Height;
	CString m_ImageName;
	bool	m_StretchBackGrd;
	TStrings m_ListImage;
	sRGB	m_BGDColor;

	CBrush	_Brush;
	CStatic _ColorStatic;

	CSchemaDialog(void);
	~CSchemaDialog(void);

	BEGIN_DDX_MAP(thisClass)
		DDX_TEXT(IDC_SCHEMA_NAME, m_Name)
		DDX_CHECK(IDC_FIXED_SIZE, m_FixedSize)
		DDX_CHECK(IDC_IMAGE_BACKGRD, m_HasImageBackgrd)
		DDX_UINT(IDC_WIDTH, m_Width)
		DDX_UINT(IDC_HEIGHT, m_Height)
	END_DDX_MAP()

	BEGIN_MSG_MAP(thisClass)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		COMMAND_ID_HANDLER(IDOK, OnCloseCmd)
		COMMAND_ID_HANDLER(IDCANCEL, OnCloseCmd)
		CHAIN_MSG_MAP(CUpdateUI<thisClass>)
		COMMAND_ID_HANDLER(IDC_FIXED_SIZE, OnFixedSize)
		COMMAND_ID_HANDLER(IDC_IMAGE_BACKGRD, OnImageBackgrd)
		COMMAND_ID_HANDLER(IDC_STRETCH_IMAGE, OnStretchBackGrd)
		COMMAND_CODE_HANDLER(CBN_SELCHANGE, OnCbnSelchange)
		COMMAND_ID_HANDLER(IDC_BROWSE_COLOR, OnBrowseColor)
		MESSAGE_HANDLER(WM_CTLCOLORSTATIC, OnCtlColorStatic)
		CHAIN_MSG_MAP(CDialogResize<thisClass>)
		
		DEFAULT_REFLECTION_HANDLER()
	END_MSG_MAP()

	BEGIN_DLGRESIZE_MAP(thisClass)

	END_DLGRESIZE_MAP()

	BEGIN_UPDATE_UI_MAP(thisClass)
		UPDATE_ELEMENT(IDC_WIDTH, UPDUI_CHILDWINDOW)
		UPDATE_ELEMENT(IDC_HEIGHT, UPDUI_CHILDWINDOW)
		UPDATE_ELEMENT(IDC_IMAGE_NAMES, UPDUI_CHILDWINDOW)
		UPDATE_ELEMENT(IDC_STRETCH_IMAGE, UPDUI_CHILDWINDOW)
	END_UPDATE_UI_MAP()

	LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnCloseCmd(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnFixedSize(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnImageBackgrd(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnStretchBackGrd(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);

	LRESULT OnCbnSelchange(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	
	LRESULT OnBrowseColor(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnCtlColorStatic(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);

	void LoadImageNames(TStrings Names);
};
