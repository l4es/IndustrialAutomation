/*****************************************************************************
 *
 *  Filename : DRAGSIZERBMP.CPP
 *
 *****************************************************************************
 *
 *  Copyright : (c) Embedded Intelligence Ltd. 1993,2009
 *
 *****************************************************************************
 *
 *  Description : This is the implementation for CDragSizerBmp class .
 *
 * USE: to use this class, subclass a static control, and then provide a 
 * resourse id via the SetBitmapID() function. By default the drag is NW-SE
 * (bottom right) to change this specify....
 *****************************************************************************/


#include "stdafx.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// Colors
#define rgbWhite RGB(255,255,255)
// Raster op codes
#define DSa     0x008800C6L
#define DSx     0x00660046L

/////////////////////////////////////////////////////////////////////////////
// CDragSizerBmp

CDragSizerBmp::CDragSizerBmp()
{
   m_iWidth = 0;
   m_iHeight = 0;
   m_hbmMask = NULL;
   m_buttonDown = FALSE;
}

CDragSizerBmp::~CDragSizerBmp()
{

}

// --------------------------------------- PreSubclassWindow -------------------
void CDragSizerBmp::PreSubclassWindow()
{
LONG style;

   style = GetWindowLong(this->GetSafeHwnd(), GWL_STYLE);
   ASSERT( style & SS_NOTIFY);  // If your debugger stops here, 
   // turn on the "Notify" window property for the static control.
}


// -------------------------------- SetBitmapID ---------------------------------
// set resource Id (int this module) to the bitmap to paint.
// the bitmap uses a transparent background.
BOOL CDragSizerBmp::SetBitmapID(int resourceId)
{
   return(m_bitMap.LoadBitmap(resourceId));
}

// ---------------------------------- SetCornerType ----------------------------
// Specify the mouse cursor and the bitmap resource ID to load.
// If "shiftControl" is true, the control is moved to the relevant corner if not 
// already there. The gap is calculated using GetSystemMetrics(SM_CXSIZEFRAME),
// and the bitmap size, not the control size, which may be changed
//
BOOL CDragSizerBmp::SetCornerType(_CursorCornerType corner, 
                                  int resourceId,
                                  BOOL shiftControl)
{
BOOL ret;
int frameWidth;

   m_cornerType = corner;
   ret = SetBitmapID(resourceId);
   if (shiftControl)
   {
   CRect dRect, parentRect;
   CWnd *pParent = GetParent();
      // move the control towards the desired corner

      // To get rid of an annoying 1-pixel gap, between the edge of this control and the dialog frame.
      // for some reason, the dialog editor may not be positioning the control right up to the edge
      GetWindowRect(&dRect);
      ScreenToClient(&dRect);

      pParent->GetWindowRect(&parentRect);
      pParent->ScreenToClient(&parentRect);

      frameWidth = GetSystemMetrics(SM_CXSIZEFRAME);  // use the same value for x and y
      switch (m_cornerType)
      {
      case CORNER_BOTTOMRIGHT :
         dRect.right = parentRect.right - frameWidth;
         dRect.bottom = parentRect.bottom - frameWidth;
         dRect.left = dRect.right - GetWidth();
         dRect.top = dRect.bottom - GetHeight();
         break;
      case CORNER_BOTTOMLEFT :
         dRect.left = 0;
         dRect.bottom = parentRect.bottom - frameWidth;
         dRect.right = dRect.left + GetWidth();
         dRect.top = dRect.bottom - GetHeight();
         break;
      case CORNER_TOPRIGHT :
         dRect.right = parentRect.right - frameWidth;
         dRect.top = 0;
         dRect.left = dRect.right - GetWidth();
         dRect.bottom = dRect.top + GetHeight();
         break;
      case CORNER_TOPLEFT :
         dRect.left = 0;
         dRect.top = 0;
         dRect.right = dRect.left + GetWidth();
         dRect.bottom = dRect.bottom + GetHeight();
         break;
      }
      MoveWindow(&dRect, TRUE);

   }
   return(ret);
}


IMPLEMENT_DYNAMIC(CDragSizerBmp, CWnd)

BEGIN_MESSAGE_MAP(CDragSizerBmp, CWnd)
	//{{AFX_MSG_MAP(CDragSizerBmp)
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	ON_WM_SETCURSOR()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CDragSizerBmp message handlers

void CDragSizerBmp::GetMetrics()
{
    // Get the width and height
    BITMAP bm;
    m_bitMap.GetObject(sizeof(bm), &bm);
    m_iWidth = bm.bmWidth;
    m_iHeight = bm.bmHeight;
}


int CDragSizerBmp::GetWidth()
{
    if ((m_iWidth == 0) || (m_iHeight == 0)){
        GetMetrics();
    }
    return m_iWidth;
}

int CDragSizerBmp::GetHeight()
{
    if ((m_iWidth == 0) || (m_iHeight == 0)){
        GetMetrics();
    }
    return m_iHeight;
}

// ----------------------------------- DrawTrans ---------------------------------
void CDragSizerBmp::DrawTrans(HDC hDC, int x, int y)
{
    ASSERT(hDC);
    if (!m_hbmMask) 
       CreateMask(hDC);

    ASSERT(m_hbmMask);
    int dx = GetWidth();
    int dy = GetHeight();

    // Create a memory DC to do the drawing to
    HDC hdcOffScr = ::CreateCompatibleDC(hDC);
    // Create a bitmap for the off-screen DC that is really
    // color compatible with the destination DC.
    HBITMAP hbmOffScr = ::CreateBitmap(dx, dy, 
                             (BYTE)GetDeviceCaps(hDC, PLANES),
                             (BYTE)GetDeviceCaps(hDC, BITSPIXEL),
                             NULL);
    // Select the buffer bitmap into the off-screen DC
    HBITMAP hbmOldOffScr = (HBITMAP)::SelectObject(hdcOffScr, hbmOffScr);

    // Copy the image of the destination rectangle to the
    // off-screen buffer DC so we can play with it
    ::BitBlt(hdcOffScr, 0, 0, dx, dy, hDC, x, y, SRCCOPY);

    // Create a memory DC for the source image
    HDC hdcImage = ::CreateCompatibleDC(hDC); 
    HBITMAP hbmOldImage = (HBITMAP)::SelectObject(hdcImage, m_bitMap.m_hObject);

    // Create a memory DC for the mask
    HDC hdcMask = ::CreateCompatibleDC(hDC);
    HBITMAP hbmOldMask = (HBITMAP)::SelectObject(hdcMask, m_hbmMask);

    // XOR the image with the destination
    ::SetBkColor(hdcOffScr,rgbWhite);
    ::BitBlt(hdcOffScr, 0, 0, dx, dy ,hdcImage, 0, 0, DSx);
    // AND the destination with the mask
    ::BitBlt(hdcOffScr, 0, 0, dx, dy, hdcMask, 0,0, DSa);
    // XOR the destination with the image again
    ::BitBlt(hdcOffScr, 0, 0, dx, dy, hdcImage, 0, 0, DSx);

    // Copy the resultant image back to the screen DC
    ::BitBlt(hDC, x, y, dx, dy, hdcOffScr, 0, 0, SRCCOPY);

    // Tidy up
    ::SelectObject(hdcOffScr, hbmOldOffScr);
    ::SelectObject(hdcImage, hbmOldImage);
    ::SelectObject(hdcMask, hbmOldMask);
    ::DeleteObject(hbmOffScr);
    ::DeleteDC(hdcOffScr);
    ::DeleteDC(hdcImage);
    ::DeleteDC(hdcMask);
}

// ---------------------------------- DrawTrans ------------------------------
// Draw transparent bitmap, using a CDC
void CDragSizerBmp::DrawTrans(CDC* pDC, int x, int y)
{
    ASSERT(pDC);
    HDC hDC = pDC->GetSafeHdc();
    DrawTrans(hDC, x, y);
}


// ------------------------------------- CreateMask -----------------------------
void CDragSizerBmp::CreateMask(HDC hDC)
{
    // Nuke any existing mask
    if (m_hbmMask) {
        ::DeleteObject(m_hbmMask);
    }
    // Create memory DCs to work with
    HDC hdcMask = ::CreateCompatibleDC(hDC);
    HDC hdcImage = ::CreateCompatibleDC(hDC);
    // Create a monochrome bitmap for the mask
    m_hbmMask = ::CreateBitmap(GetWidth(),
                               GetHeight(),
                               1,
                               1,
                               NULL);
    // Select the mono bitmap into its DC
    HBITMAP hbmOldMask = (HBITMAP)::SelectObject(hdcMask, m_hbmMask);
    // Select the image bitmap into its DC
    HBITMAP hbmOldImage = (HBITMAP)::SelectObject(hdcImage, m_bitMap.m_hObject);
    // Set the transparency color to be the top-left pixel
    ::SetBkColor(hdcImage, ::GetPixel(hdcImage, 0, 0));
    // Make the mask
    ::BitBlt(hdcMask,
             0, 0,
             GetWidth(), GetHeight(),
             hdcImage,
             0, 0,
             SRCCOPY);
    // Tidy up
    ::SelectObject(hdcMask, hbmOldMask);
    ::SelectObject(hdcImage, hbmOldImage);
    ::DeleteDC(hdcMask);
    ::DeleteDC(hdcImage);
}


void CDragSizerBmp::OnPaint() 
{

   if (m_bitMap.GetSafeHandle())
   {
	   CPaintDC dc(this); // device context for painting
      // draw a transparent bitmap
      DrawTrans(&dc, 0,0);
   }
	// Do not call CWnd::OnPaint() for painting messages
}

BOOL CDragSizerBmp::OnEraseBkgnd(CDC* pDC) 
{
	// DO nothing
	
	return 1;//CWnd::OnEraseBkgnd(pDC);
}

// ------------------------------ OnSetCursor -----------------------------
// called by the framework, the control must have the NOTIFY style
BOOL CDragSizerBmp::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message) 
{
	// TODO: Add your message handler code here and/or call default
	
	//return CWnd::OnSetCursor(pWnd, nHitTest, message);
   HCURSOR hCursor = ::LoadCursor(NULL, IDC_SIZENWSE);
   ASSERT(hCursor);
   ::SetCursor(hCursor);
   return TRUE;

}

// ------------------------------- OnLButtonDown ---------------------------
// start capture of the mouse
void CDragSizerBmp::OnLButtonDown(UINT nFlags, CPoint point) 
{
   m_buttonDown = TRUE;
   m_downPosition = point; // remember this point, it is used as the drag 
   // starting-reference point
   ::SetCapture(m_hWnd);
   //OutputDebugString("Mouse down\n");
	CWnd::OnLButtonDown(nFlags, point);
}

// -------------------------------- OnLButtonUp -----------------------------
// releases the mouse, and stops dragging
void CDragSizerBmp::OnLButtonUp(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
   m_buttonDown = FALSE;
	ReleaseCapture();
   //OutputDebugString("Mouse up\n");
	CWnd::OnLButtonUp(nFlags, point);
}

// -------------------------------- OnMouseMove ----------------------------
// uses mouse drag to size (sends WM_SIZE) the window, if we have capture!
void CDragSizerBmp::OnMouseMove(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
   if (m_buttonDown)
   {
      if (MK_LBUTTON && nFlags)
      {
      CRect parentRect;
      CWnd *pParent = GetParent();
      LONG x,y;

         //OutputDebugString("Mouse move\n");  
         pParent->GetWindowRect(&parentRect);
         // calc the distance moved
         x = point.x - m_downPosition.x;
         y = point.y - m_downPosition.y;
         parentRect.right += x;
         parentRect.bottom += y;
         // re-position the window
         pParent->MoveWindow(&parentRect);
      }
   }
	
	CWnd::OnMouseMove(nFlags, point);
}
