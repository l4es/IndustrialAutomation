// ColorStatic.cpp : implementation file
//

#include "stdafx.h"

#include <afxdlgs.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CColorStatic

CColorStatic::CColorStatic()
{
   m_color = RGB(255,0,0); //initialize to red by default, so that folks 
   // know that we are here.
}

CColorStatic::~CColorStatic()
{
}


BEGIN_MESSAGE_MAP(CColorStatic, CStatic)
	//{{AFX_MSG_MAP(CColorStatic)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
   ON_WM_CTLCOLOR_REFLECT()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CColorStatic message handlers

//////////////////// Handle reflected WM_CTLCOLOR to set custom control color.
// For a text control, use visited/unvisited colors and underline font.
// For non-text controls, do nothing. 
//
HBRUSH CColorStatic::CtlColor(CDC* pDC, UINT nCtlColor)
{
   ASSERT(nCtlColor == CTLCOLOR_STATIC);
   DWORD dwStyle = GetStyle();
/*
   if (!(dwStyle & SS_NOTIFY)) {
         // Turn on notify flag to get mouse messages and STN_CLICKED.
         // Otherwise, I'll never get any mouse clicks!
         ::SetWindowLong(m_hWnd, GWL_STYLE, dwStyle | SS_NOTIFY);
     }
  */   
   HBRUSH hbr = NULL;
   if ((dwStyle & 0xFF) <= SS_RIGHT) 
   {
      // this is a text control: set up font and colors
      if (!(HFONT)m_font) 
      {
         // first time init: create font
         LOGFONT lf;
         GetFont()->GetObject(sizeof(lf), &lf);
         m_font.CreateFontIndirect(&lf);
      }
 
      // use ...
      pDC->SelectObject(&m_font);
      pDC->SetTextColor(m_color);
      pDC->SetBkMode(TRANSPARENT);
 
      // return hollow brush to preserve parent background color
      hbr = (HBRUSH)::GetStockObject(HOLLOW_BRUSH);
   }
   return hbr;
}


/////////////////////////////////////////////////////////////////////////////
// CSampler

CSampler::CSampler()
{
   m_color = 0;   // default to black
}

CSampler::~CSampler()
{
}


BEGIN_MESSAGE_MAP(CSampler, CEdit)
	//{{AFX_MSG_MAP(CSampler)
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSampler message handlers

// ------------------------------ OnPaint ------------------------------
void CSampler::OnPaint() 
{
RECT clientRect;
CBrush backBrush;

	CPaintDC dc(this); // device context for painting
	
	// TODO: Add your message handler code here
   GetClientRect(&clientRect);
   backBrush.CreateSolidBrush(m_color);
	dc.FillRect(&clientRect, &backBrush);

	// Do not call CEdit::OnPaint() for painting messages
} // OnPaint

// ----------------------------------- SetNewColor ---------------------------
//
void CSampler::SetNewColor(COLORREF color)
{
   ASSERT(m_hWnd);   // window must already exist
   m_color = color;
   InvalidateRect(NULL);
} // SetNewColor

// -------------------------------- BrowseColor ------------------------------
COLORREF CSampler::BrowseColor()
{
static COLORREF custColors[16];

   ASSERT(m_hWnd);   // window must already exist

   // init custom colors to all white
   for (int i=0;i<16;i++)
      custColors[i] = RGB(255,255,255);
   custColors[0] = m_color;  // set our old or existing color into the custom colors

	CColorDialog cc( m_color,/*CC_PREVENTFULLOPEN | */CC_RGBINIT | CC_ANYCOLOR);
   cc.m_cc.lpCustColors = custColors;   // point to my custom colours

   // invoke dialog
   cc.DoModal();
   
   // retrieve colour from dialog into the control
   SetNewColor(cc.m_cc.rgbResult);
   return(cc.m_cc.rgbResult);
}

