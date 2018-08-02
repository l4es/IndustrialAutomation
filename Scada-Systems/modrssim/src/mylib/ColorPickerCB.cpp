// ColorPickerCB.cpp
//
// (c) 1998 James R. Twine
//
// Based On Code That Was Found On www.codeguru.com, And Was 
// © 1997 Baldvin Hansson
//
//	Bug Fixes And Additions:
//
//	Bug Fix From Marcel Galem:
//			Added Masking Of Inverted COLORREF Value.
//

#include "stdafx.h"
#include "ColorPickerCB.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


//
//	Load Standard Colors...
//	(If You Change This, Be Sure To Load CCB_MAX_COLORS Colors...)
//
SColorAndName	CColorPickerCB::ms_pColors[ CCB_MAX_COLORS ] =
{
	SColorAndName( RGB( 0x00, 0x00, 0x00 ),	"Black" ),
	SColorAndName( RGB( 0x80, 0x00, 0x00 ),	"Maroon" ),
	SColorAndName( RGB( 0x00, 0x80, 0x00 ),	"Green" ),
	SColorAndName( RGB( 0x80, 0x80, 0x00 ),	"Olive" ),
	SColorAndName( RGB( 0x00, 0x00, 0x80 ),	"Navy" ),
	SColorAndName( RGB( 0x80, 0x00, 0x80 ),	"Purple" ),
	SColorAndName( RGB( 0x00, 0x80, 0x80 ),	"Teal" ),
	SColorAndName( RGB( 0x80, 0x80, 0x80 ),	"Grey" ),
	SColorAndName( RGB( 0xC0, 0xC0, 0xC0 ),	"Silver" ),
	SColorAndName( RGB( 0xFF, 0x00, 0x00 ),	"Red" ),
	SColorAndName( RGB( 0x00, 0xFF, 0x00 ),	"Lime" ),
	SColorAndName( RGB( 0xFF, 0xFF, 0x00 ),	"Yellow" ),
	SColorAndName( RGB( 0x00, 0x00, 0xFF ),	"Blue" ),
	SColorAndName( RGB( 0xFF, 0x00, 0xFF ),	"Fushcia" ),
	SColorAndName( RGB( 0x00, 0xFF, 0xFF ),	"Aqua" ),
	SColorAndName( RGB( 0xFF, 0xFF, 0xFF ),	"White" ),
};


CColorPickerCB::CColorPickerCB()
{
	m_bInitialized = false;
}


CColorPickerCB::~CColorPickerCB()
{
}


BEGIN_MESSAGE_MAP(CColorPickerCB, CComboBox)
	//{{AFX_MSG_MAP(CColorPickerCB)
	ON_WM_CREATE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CColorPickerCB message handlers

int CColorPickerCB::OnCreate( LPCREATESTRUCT pCStruct ) 
{
	if( CComboBox::OnCreate( pCStruct ) == -1 )				// If Create Failed
		return( -1 );										// Return Failure
	
	Initialize();											// Initialize Contents
	SetCurSel( 0 );											// Select First Item By Default

	return( 0 );											// Done!
}


void CColorPickerCB::PreSubclassWindow() 
{
	Initialize();											// Initialize Contents
	
	CComboBox::PreSubclassWindow();							// Subclass Control

	SetCurSel( 0 );											// Select First Item By Default

	return;													// Done!
}


void CColorPickerCB::Initialize( void )
{
	int		iAddedItem = -1;

	if( m_bInitialized )									// If Already Initialized
		return;												// Return

	for( int iColor = 0; iColor < CCB_MAX_COLORS; iColor++ )// For All Colors
	{
		iAddedItem = AddString(	ms_pColors[
					iColor ].m_cColor );					// Set Color Name/Text
		if( iAddedItem == CB_ERRSPACE )						// If Not Added
			break;											// Stop
		else												// If Added Successfully
			SetItemData( iAddedItem, ms_pColors[
					iColor ].m_crColor );					// Set Color RGB Value
	}
	m_bInitialized = true;									// Set Initialized Flag
}


void		CColorPickerCB::DrawItem( LPDRAWITEMSTRUCT pDIStruct )
{
	static		CString	sColor;								// No Need To Reallocate Each Time

	CDC			dcContext;
	CRect		rItemRect( pDIStruct -> rcItem );
	CRect		rBlockRect( rItemRect );
	CRect		rTextRect( rBlockRect );
	CBrush		brFrameBrush;
	int			iFourthWidth = 0;
	int			iItem = pDIStruct -> itemID;
	int			iAction = pDIStruct -> itemAction;
	int			iState = pDIStruct -> itemState;
	COLORREF	crColor = 0;
	COLORREF	crNormal = GetSysColor( COLOR_WINDOW );
	COLORREF	crSelected = GetSysColor( COLOR_HIGHLIGHT );
	COLORREF	crText = GetSysColor( COLOR_WINDOWTEXT );

	if( !dcContext.Attach( pDIStruct -> hDC ) )				// Attach CDC Object
		return;												// Stop If Attach Failed

	iFourthWidth = ( rBlockRect.Width() / 4 );				// Get 1/4 Of Item Area
	brFrameBrush.CreateStockObject( BLACK_BRUSH );			// Create Black Brush

	if( iState & ODS_SELECTED )								// If Selected
	{														// Set Selected Attributes
		dcContext.SetTextColor(
				( 0x00FFFFFF & ~( crText ) ) );				// Set Inverted Text Color (With Mask)
		dcContext.SetBkColor( crSelected );					// Set BG To Highlight Color
		dcContext.FillSolidRect( &rBlockRect, crSelected );	// Erase Item
	}
	else													// If Not Selected
	{														// Set Standard Attributes
		dcContext.SetTextColor( crText );					// Set Text Color
		dcContext.SetBkColor( crNormal );					// Set BG Color
		dcContext.FillSolidRect( &rBlockRect, crNormal );	// Erase Item
	}
	if( iState & ODS_FOCUS )								// If Item Has The Focus
		dcContext.DrawFocusRect( &rItemRect );				// Draw Focus Rect

	//
	//	Calculate Text Area
	//
	rTextRect.left += ( iFourthWidth + 2 );					// Set Start Of Text
	rTextRect.top += 2;										// Offset A Bit

	//
	//	Calculate Color Block Area
	//
	rBlockRect.DeflateRect( CSize( 2, 2 ) );				// Reduce Color Block Size
	rBlockRect.right = iFourthWidth;						// Set Width Of Color Block

	//
	//	Draw Color Text And Block
	//
	if( iItem != -1 )										// If Not An Empty Item
	{
		GetLBText( iItem, sColor );							// Get Color Text
		if( iState & ODS_DISABLED )							// If Disabled
		{
			crColor = GetSysColor( COLOR_INACTIVECAPTIONTEXT );
			dcContext.SetTextColor( crColor );				// Set Text Color
		}
		else												// If Normal
			crColor = GetItemData( iItem );					// Get Color Value

		dcContext.SetBkMode( TRANSPARENT );					// Transparent Background
		dcContext.TextOut( rTextRect.left, rTextRect.top,
				sColor );									// Draw Color Name

		dcContext.FillSolidRect( &rBlockRect, crColor );	// Draw Color
				
		dcContext.FrameRect( &rBlockRect, &brFrameBrush );	// Draw Frame
	}
	dcContext.Detach();										// Detach DC From Object
}


COLORREF	CColorPickerCB::GetSelectedColorValue( void )
{
	int		iSelectedItem = GetCurSel();					// Get Selected Item

	if( iSelectedItem == CB_ERR )							// If Nothing Selected
		return( RGB( 0, 0, 0 ) );							// Return Black

	return( GetItemData( iSelectedItem ) );					// Return Selected Color
}


CString		CColorPickerCB::GetSelectedColorName( void )
{
	int		iSelectedItem = GetCurSel();					// Get Selected Item

	if( iSelectedItem == CB_ERR )							// If Nothing Selected
		return( m_sColorName = _T(""));//afxEmptyString );			// Return Nothing (Not "Black!")

	GetLBText( iSelectedItem, m_sColorName );				// Store Name Of Color

	return( m_sColorName );									// Return Selected Color Name
}


void		CColorPickerCB::SetSelectedColorValue( COLORREF crClr )
{
	int		iItems = GetCount();
	
	for( int iItem = 0; iItem < iItems; iItem++ )
	{
		if( crClr == GetItemData( iItem ) )					// If Match Found
			SetCurSel( iItem );								// Select It
	}
	return;													// Done!
}


void		CColorPickerCB::SetSelectedColorName( PCSTR cpColor )
{
	int		iItems = GetCount();
	CString	sColorName;

	for( int iItem = 0; iItem < iItems; iItem++ )
	{
		GetLBText( iItem, sColorName );						// Get Color Name

		if( !sColorName.CompareNoCase( cpColor ) )			// If Match Found
			SetCurSel( iItem );								// Select It
	}
	return;													// Done!
}


bool		CColorPickerCB::RemoveColor( PCSTR cpColor )
{
	int		iItems = GetCount();
	CString	sColor;
	bool	bRemoved = false;

	for( int iItem = 0; iItem < iItems; iItem++ )
	{
		GetLBText( iItem, sColor );							// Get Color Name
		if( !sColor.CompareNoCase( cpColor ) )				// If Match Found
		{
			DeleteString( iItem );							// Remove It
			bRemoved = true;								// Set Flag
			break;											// Stop Checking
		}
	}
	return( bRemoved );										// Done!
}


bool		CColorPickerCB::RemoveColor( COLORREF crClr )
{
	int		iItems = GetCount();
	bool	bRemoved = false;

	for( int iItem = 0; iItem < iItems; iItem++ )
	{
		if( crClr == GetItemData( iItem ) )					// If Desired Color Found
		{
			DeleteString( iItem );							// Remove It
			bRemoved = true;								// Set Flag
			break;											// Stop Checking
		}
	}
	return( bRemoved );										// Done!
}


int			CColorPickerCB::AddColor( PCSTR cpName, COLORREF crColor )
{
	int		iItem = -1;

	iItem = InsertString( -1, cpName );						// Insert String
	if( iItem != LB_ERR )									// If Insert Good
		SetItemData( iItem, crColor );						// Set Color Value 

	return( iItem );										// Done! Return Location
}


	
