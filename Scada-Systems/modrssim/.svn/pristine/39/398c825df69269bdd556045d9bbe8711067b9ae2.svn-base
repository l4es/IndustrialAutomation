// CommsLamp.cpp : implementation file
//

#include "stdafx.h"
//#include "CommsLamp.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCommsLamp

CCommsLamp::CCommsLamp()
{
   m_state = FALSE;
   m_ONcolor = RGB(255,0,0);
   m_OFFcolor = RGB(190,0,0);
   m_timerDuration = 0; // no timer
}

CCommsLamp::~CCommsLamp()
{

}

// ------------------------------- UpdateMyCtrl ----------------------------
void CCommsLamp::UpdateMyCtrl()
{
   // does control RECT invalidation, maybe more in future
   InvalidateRect(NULL);
}

// ---------------------------- SetupLamp --------------------------
// a value of 0 kills the timer
BOOL CCommsLamp::SetupLamp(DWORD onTimerDuration)
{
   if (m_timerDuration)
   {
      // kill a timer already in existence
      KillTimer(m_timerID);
      m_timerDuration = 0;
   }

   if (onTimerDuration)
   {
      // set up a timer
      m_timerID = SetTimer(1, onTimerDuration, NULL);
      if (m_timerID)
      {
         m_timerDuration = onTimerDuration;
         return TRUE;
      }
   }
   else
      return TRUE;
   return FALSE;
}

BEGIN_MESSAGE_MAP(CCommsLamp, CButton)
	//{{AFX_MSG_MAP(CCommsLamp)
	ON_WM_PAINT()
	ON_WM_DESTROY()
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCommsLamp message handlers

	// Do not call CStatic::OnPaint() for painting messages
void CCommsLamp::OnPaint() 
{
CBrush   darkBrush;  // background bit
CPen     borderPen;  // silver/gray border
CBrush   lightBrush; // lighter spot
CPen     lightPen;   // light reflective area
CPen boxPen;

COLORREF mainColor;


   CPaintDC dc(this); // device context for painting
	
	// TODO: Add your message handler code here
	CRect rcClient;
	GetClientRect(&rcClient);

   // square off our drawing rectangle, base-
   // class is hopefully going to paint our background properly
   if (rcClient.Width() > rcClient.Height())
      rcClient.right = rcClient.bottom;
   else
      rcClient.bottom = rcClient.right;

   if (m_state)
      mainColor = m_ONcolor;
   else
      mainColor = m_OFFcolor;
   // Set up all create all pens and brushes
   lightPen.CreatePen(PS_SOLID,1,mainColor);
   boxPen.CreatePen(PS_SOLID,1, RGB(0,255,0));
   darkBrush.CreateSolidBrush(RGB(GetRValue(mainColor)/2, GetGValue(mainColor)/2,GetBValue(mainColor)/2));
   lightBrush.CreateSolidBrush(mainColor);

   borderPen.CreatePen(PS_SOLID,1, RGB(255,255,255));
   //borderPen.CreatePen(PS_SOLID,2, RGB(0,255,0));

   // a debugging box
   //dc.SelectObject(&boxPen);
   //dc.Rectangle(rcClient);
   
   // start, with the darker background bit
	dc.SelectObject(&borderPen);
	dc.SelectObject(&darkBrush);
	dc.Ellipse(rcClient);
   
   // draw the inner shiny bit
   rcClient.DeflateRect(2,2);
	dc.SelectObject(&lightPen);
	dc.SelectObject(&lightBrush);
	dc.Ellipse(rcClient);
     
   //rcClient.DeflateRect(-3,-3);
   
   if (m_state)
   {
   // The arc function is just to add a 3D effect for the control
      dc.SelectStockObject(WHITE_PEN);
		CPoint ptStart,ptEnd;
		ptStart = CPoint(rcClient.Width()/2,rcClient.bottom);
		ptEnd	= CPoint(rcClient.right,rcClient.top);
   
 		dc.MoveTo(ptStart);
		dc.Arc(rcClient,ptStart,ptEnd);
   }
   // select old object back into the DC
   //dc.SelectObject(pOldPen);

}

void CCommsLamp::OnDestroy() 
{
	CButton::OnDestroy();
	
	// TODO: Add your message handler code here
	if (m_hWnd && m_timerDuration)
   {
      KillTimer(m_timerID);
   }
}

// ---------------------------- OnTimer ------------------------------
void CCommsLamp::OnTimer(UINT nIDEvent) 
{
	// TODO: Add your message handler code here and/or call default
   if (m_timerDuration && (m_timerID==nIDEvent))
   {
      // turn LED off
      if (m_state)
         SetState(FALSE);
   }
	
	CButton::OnTimer(nIDEvent);
}

// ---------------------------- SetState --------------------------------
void CCommsLamp::SetState(BOOL state)
{
   if (state != m_state)
   {
      m_state = state; 
      UpdateMyCtrl();
   }
}

// ------------------------ PreSubclassWindow --------------------------
void CCommsLamp::PreSubclassWindow() 
{
	// TODO: Add your specialized code here and/or call the base class
   SetButtonStyle(GetButtonStyle() | BS_OWNERDRAW | BS_AUTOCHECKBOX);
	
	CButton::PreSubclassWindow();
}

// -------------------------- DrawItem ---------------------------------
void CCommsLamp::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct) 
{
	// TODO: Add your code to draw the specified item
	
   // do nothing, we have to do this.
}
