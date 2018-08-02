/////////////////////////////////////////////////////////////////////////////
//
// FILE: MicroTick.cpp : implementation file
//
// See _README.CPP
//
// Implements the CMicroTick class
//
// Fixes: See H file
//
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "MOD_RSsim.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// 

CMicroTick::CMicroTick()
{
   m_unchecked = RGB(255,70,90);   // red unchecked colour
   m_checked = RGB(0,255,0);     // green checked colour
   // background
   m_backgroundInActive = RGB(127,127,127);
   m_backgroundActive = RGB(241,241,0);
   m_borderWidth = 1;            // default width
   m_backgroundState = 0;        //inactive

   m_textColor = RGB(0,0,0);  // black
   // m_microTickState will be the tick-box state as set in the dialog template
   // or whatever the parent window has set up.
   m_pFont = NULL;
}

CMicroTick::~CMicroTick()
{
   if (NULL != m_pFont)
      delete m_pFont;
}


BEGIN_MESSAGE_MAP(CMicroTick, CButton)
	//{{AFX_MSG_MAP(CMicroTick)
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	ON_CONTROL_REFLECT(BN_CLICKED, OnClicked)
	ON_WM_SETFOCUS()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMicroTick message handlers

// ----------------------------- OnPaint --------------------------------
// By over-riding onpaint, I have gotten control of the graphical 
// appearance of this control
void CMicroTick::OnPaint() 
{
RECT clientRect;
CBrush backBrush;
CPen borderPen;
COLORREF color, borderColor, textColor;
CHAR buffer[80];
CPen * pOldPen;
CFont * pOldFont;
DWORD i=9, borderWidth = m_borderWidth;
BOOL drawFocus = FALSE;

	// TODO: Add your message handler code here
	CPaintDC dc(this); // device context for painting
   GetClientRect(&clientRect);
   
   // determine the text background color to use
   switch (m_microTickState)
   {
      case 0: // unselected
         color = m_unchecked;
         textColor = m_uncheckedTextColor;
         break;
      case 1:
      default:
         color = m_checked;
         textColor = m_textColor;
         break;
   }
   // determine the border color
   switch (m_backgroundState)
   {
      case 0: // unselected/inactive
         borderColor = m_backgroundInActive;
         break;
      case 1:
      default:
         borderColor = m_backgroundActive;
         break;
   }

   backBrush.CreateSolidBrush(color);
	dc.FillRect(&clientRect, &backBrush);
   
   //////////////////////////////////////////////////////////////////
   // Draw the border
   if (GetFocus()==this)
   {
      borderWidth++;
      drawFocus = TRUE;
   }
   borderPen.CreatePen(PS_SOLID, 1, borderColor);
   pOldPen = (CPen*)dc.SelectObject(&borderPen);
   pOldFont = (CFont*)dc.SelectObject(m_pFont);
   // I used a loop here, since geometric pens are difficult to use
   for (i=0;i < borderWidth; i++)
   {
      // draw a thick border if wanted.
      dc.MoveTo(i,i);
      dc.LineTo(clientRect.right-i-1, i);                  // TOP
      dc.LineTo(clientRect.right-i-1, clientRect.bottom-i-1);// RIGHT
      dc.LineTo(clientRect.left+i, clientRect.bottom-i-1); // BOTTOM
      dc.LineTo(clientRect.left+i, i);                   // LEFT side
      if (drawFocus && (i== borderWidth-2))
         dc.SelectStockObject(BLACK_PEN);
   }   
   
   ///////////////////////////////////////////////////////////////////
   // Display the text
   buffer[0] = '\0';
   GetWindowText(buffer, sizeof(buffer));
   dc.SetBkColor(color);
   // dc.TextOut(); does not work so well, so we did it this way
   clientRect.bottom -=m_borderWidth;   // clip
   clientRect.top    +=m_borderWidth;   // make sure we move text away from the borders
   clientRect.right  -=m_borderWidth;   // clip
   clientRect.left   +=m_borderWidth+1;   // move away
   
   dc.SetBkMode(TRANSPARENT);
   dc.SetTextColor(textColor);
   dc.DrawText(buffer, strlen(buffer), &clientRect, DT_VCENTER |DT_CENTER);

   // select old pen back into DC, not required in a dialog actually
   //dc.SelectObject(pOldPen);
   
   // Do not call CButton::OnPaint() for painting messages
}


// ------------------------ OnEraseBkgnd -----------------------------
BOOL CMicroTick::OnEraseBkgnd(CDC* pDC) 
{
   return (1);
} // OnEraseBkgnd

// --------------------------- OnClicked --------------------------------
//
void CMicroTick::OnClicked() 
{
	// TODO: Add your control notification handler code here
   if (m_microTickState )
      m_microTickState = 0;
   else
      m_microTickState = 1;
   InvalidateRect(NULL);	
} // OnClicked

// ------------------------- PreSubclassWindow ---------------------------
//
void CMicroTick::PreSubclassWindow() 
{
	// TODO: Add your specialized code here and/or call the base class
   m_microTickState = CButton::GetCheck();

   SetButtonStyle(GetButtonStyle() | BS_OWNERDRAW | BS_AUTOCHECKBOX);
   // find a font size
CFont * pFont = GetFont();
LOGFONT logFont;

   pFont->GetLogFont(&logFont);
   logFont.lfWidth=0;
   logFont.lfHeight=-9;
   strcpy(logFont.lfFaceName, "Terminal");
   m_pFont = new CFont;
   m_pFont->CreateFontIndirect(&logFont);
	CButton::PreSubclassWindow();
} // PreSubclassWindow

// --------------------------- SetCheck -------------------------------
void CMicroTick::SetCheck(int nCheck)
{
	m_microTickState = nCheck;
   
   InvalidateRect(NULL);	// force a re-paint
} // SetCheck

// --------------------------- GetCheck -------------------------------
int CMicroTick::GetCheck()
{
   return (m_microTickState? 1 : 0);
} // GetCheck


// ----------------------------- DrawItem ------------------------------
void CMicroTick::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct) 
{
	// TODO: Add your code to draw the specified item
	
   // do nothing, we have to do this.
}

// ----------------------------- OnSetFocus ----------------------------
void CMicroTick::OnSetFocus(CWnd* pOldWnd) 
{
	CButton::OnSetFocus(pOldWnd);
	
	// TODO: Add your message handler code here
   InvalidateRect(NULL);	
}
