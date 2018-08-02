// ColorPickerCB.cpp
//
// (c) 1998 James R. Twine
//
// Based On Code That Was Found On www.codeguru.com, And Was 
// © 1997 Baldvin Hansson
//
//	Create A ComboBox (Drop List) And Set The
//	"Owner Draw Fixed" And "Has Strings" Styles/Checkboxes, And
//	Map This Class To It.  See?  Simple!
//


#if !defined(AFX_COLORPICKERCB_H__C74333B7_A13A_11D1_ADB6_C04D0BC10000__INCLUDED_)
#define AFX_COLORPICKERCB_H__C74333B7_A13A_11D1_ADB6_C04D0BC10000__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

//
//	Constants...
//
#define		CCB_MAX_COLORS			16							// Colors In List
#define		CCB_MAX_COLOR_NAME		16							// Max Chars For Color Name - 1


//
//	Internal Structure For Color/Name Storage...
//
struct	SColorAndName
{
	/**/	SColorAndName()
	{
		ZeroMemory( this, sizeof( SColorAndName ) );		// Init Structure
	};
	/**/	SColorAndName( COLORREF crColor, PCSTR cpColor )
	{
		ZeroMemory( this, sizeof( SColorAndName ) );		// Init Structure
		m_crColor = crColor;								// Set Color RGB Value
		strncpy( m_cColor, cpColor, CCB_MAX_COLOR_NAME );	// Set Color Name
	};
	COLORREF	m_crColor;									// Actual Color RGB Value
	char		m_cColor[ CCB_MAX_COLOR_NAME ];				// Actual Name For Color
};



class CColorPickerCB : public CComboBox
{
// Construction
public:
	CColorPickerCB();

// Attributes
private:
	bool			m_bInitialized;							// Control Initialized?
	CString			m_sColorName;							// Name Of Selected Color
	static
	SColorAndName	ms_pColors[ CCB_MAX_COLORS ];			// Array Of Colors And Names

private:

	void			Initialize( void );						// Initialize Control/Colors

public:
	COLORREF		GetSelectedColorValue( void );			// Get Selected Color Value
	CString			GetSelectedColorName( void );			// Get Selected Color Name

	void			SetSelectedColorValue( COLORREF crClr );// Set Selected Color Value
	void			SetSelectedColorName( PCSTR cpColor );	// Set Selected Color Name

	bool			RemoveColor( PCSTR cpColor );			// Remove Color From List
	bool			RemoveColor( COLORREF crClr );			// Remove Color From List
	
	int				AddColor( PCSTR cpNam, COLORREF crClr );// Insert A New Color

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CColorPickerCB)
	protected:
	virtual void PreSubclassWindow();
	//}}AFX_VIRTUAL
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);

// Implementation
public:
	virtual	~CColorPickerCB();

	// Generated message map functions
protected:
	//{{AFX_MSG(CColorPickerCB)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_COLORPICKERCB_H__C74333B7_A13A_11D1_ADB6_C04D0BC10000__INCLUDED_)





/*
I found it useful to add some custom DDX routines to aid get/set of dialog control values from/to member variables.

So here they are. Just add the declarations to ColorPickerCB.h, and the definitions to ColorPickerCB.cpp:

// DECLARATIONS

// Gets/sets color value from/to COLORREF member variable
void DDX_ColorPicker(CDataExchange* pDX_, int nIDC_, COLORREF& prgbColor_);

// Gets/sets color name from/to CString member variable
void DDX_ColorPicker(CDataExchange* pDX_, int nIDC_, CString& cName_);

// DEFINITIONS

void DDX_ColorPicker(CDataExchange* pDX_, int nIDC_, COLORREF& rgbColor_)
{
	HWND hWndCtrl = pDX_->PrepareCtrl(nIDC_);
	ASSERT(hWndCtrl);
	
	CColorPickerCB* pPicker = (CColorPickerCB*) CWnd::FromHandle(hWndCtrl);
	ASSERT(pPicker);
	
	if (!pDX_->m_bSaveAndValidate)
	{
		pPicker->SetSelectedColorValue(rgbColor_);
	}
	else
	{
		rgbColor_ = pPicker->GetSelectedColorValue();
	}
}


void DDX_ColorPicker(CDataExchange* pDX_, int nIDC_, CString& cName_)
{
	HWND hWndCtrl = pDX_->PrepareCtrl(nIDC_);
	ASSERT(hWndCtrl);
	
	CColorPickerCB* pPicker = (CColorPickerCB*) CWnd::FromHandle(hWndCtrl);
	ASSERT(pPicker);
	
	if (!pDX_->m_bSaveAndValidate)
	{
		pPicker->SetSelectedColorName(cName_);
	}
	else
	{
		cName_ = pPicker->GetSelectedColorName();
	}
}

Declare the following variables in the dialog header file:

CColorPickerCB m_cColorCtl;	// Required to subclass combo
COLORREF 	m_rgbColor; 	// Optional - if want COLORREF value
CString			m_szColor;		// Optional - if want color name


And then modify DoDataExchange to look something like this:

void CMyDialog::DoDataExchange(CDataExchange* pDX_)
{
	CDialog::DoDataExchange(pDX_);
	//{{AFX_DATA_MAP(CMyDialog)
	DDX_Control(pDX_, IDC_COLOR,	m_cColorCtl);	// Required to subclass combo
	//}}AFX_DATA_MAP
	
	DDX_ColorPicker(pDX_, IDC_COLOR, m_rgbColor);	// Optional - Maps COLORREF to member variable m_rgbColor
	DDX_ColorPicker(pDX_, IDC_COLOR, m_szColor); 	// Optional - Maps color name to member variable m_szColor
}

And thats it! It should be a mite easier to set-up your color dialog!
*/
