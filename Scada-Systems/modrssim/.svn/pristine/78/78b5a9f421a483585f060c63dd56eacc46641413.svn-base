/****************************************************************/
/*																                */
/*  StarWarsCtrl.cpp											             */
/*																                */
/*  Implementation of the CStarWarsCtrl.cpp class.				    */
/*																                */
/*  Programmed by Pablo van der Meer							       */
/*  Copyright Pablo Software Solutions 2002						    */
/*  http://www.pablovandermeer.nl								       */
/*																                */
/*  Last updated: 26 may 2002									          */
/*     Waviness added 2 Nov 02 - CDB                            */
/*																                */
/****************************************************************/


#include "stdafx.h"
#include "StarWarsCtrl.h"
#include <math.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define getrandom(min,max) ((rand()%(int)(((max)+1)-(min)))+(min))


CStarWarsCtrl::CStarWarsCtrl()
{
	// protected bitmaps to restore the memory DC's
	m_pOldBitmap = NULL;

	m_Font.CreateFont(24, 0, 0, 0, FW_BOLD,
                       FALSE, FALSE, 0, ANSI_CHARSET,
                       OUT_DEFAULT_PRECIS, 
                       CLIP_DEFAULT_PRECIS,
                       DEFAULT_QUALITY, 
                       DEFAULT_PITCH|FF_SWISS, "Tahoma");
	m_nScrollSpeed = 2;
	m_nStarsSpeed = 30;
}

CStarWarsCtrl::~CStarWarsCtrl()
{
	if (m_pOldBitmap != NULL)
		m_MainDC.SelectObject(m_pOldBitmap);  
}


BEGIN_MESSAGE_MAP(CStarWarsCtrl, CStatic)
	//{{AFX_MSG_MAP(CStarWarsCtrl)
	ON_WM_PAINT()
	ON_WM_SIZE()
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/********************************************************************/
/*																	                 */
/* Function name : PreSubclassWindow								        */
/* Description   : Initialize some stuff							        */
/*																	                 */
/********************************************************************/
void CStarWarsCtrl::PreSubclassWindow() 
{
	CClientDC dc(this);

	GetClientRect(m_rectClient);

	// initialize stars
	for (int i = 0; i < NUM_STARS; i++)
	{
		m_StarArray[i].x = getrandom(0, 1024);
		m_StarArray[i].x -= 512;
		m_StarArray[i].y = getrandom(0, 1024);
		m_StarArray[i].y -= 512;
		m_StarArray[i].z = getrandom(0, 512);
		m_StarArray[i].z -= 256;
	}
/*
	m_TextLines.Add("A long time ago");
	m_TextLines.Add("");
	m_TextLines.Add("in a galaxy far far away");
	m_TextLines.Add("");
	m_TextLines.Add("this application was programmed by");
	m_TextLines.Add("");
	m_TextLines.Add("Pablo van der Meer");
	m_TextLines.Add("");
	m_TextLines.Add("Have fun!");
*/	
	m_nScrollPos = m_rectClient.Height();

	m_nScrollSpeed = 2;

	SetTimer(1, 75, NULL);  // 75ms

	CStatic::PreSubclassWindow();
}


/********************************************************************/
/*																	                 */
/* Function name : OnPaint											           */
/* Description   : Called when the application makes a request to	  */
/*				   repaint a portion of the window.					        */
/*																	                 */
/********************************************************************/
void CStarWarsCtrl::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	CDC memDC;
	CBitmap memBitmap;
	CBitmap* oldBitmap;

	// to avoid flicker, establish a memory DC, draw to it 
	// and then BitBlt it to the client
	memDC.CreateCompatibleDC(&dc);
	memBitmap.CreateCompatibleBitmap(&dc, m_rectClient.Width(), m_rectClient.Height());
	oldBitmap = (CBitmap *)memDC.SelectObject(&memBitmap);

	if (memDC.GetSafeHdc() != NULL)
	{
		// first drop the bitmap on the memory DC
		memDC.BitBlt(0, 0, m_rectClient.Width(), m_rectClient.Height(), &m_MainDC, 0, 0, SRCCOPY);
		// finally send the result to the display DC
		dc.BitBlt(0, 0, m_rectClient.Width(), m_rectClient.Height(), &memDC, 0, 0, SRCCOPY);
	}
	memDC.SelectObject(oldBitmap);
}


/********************************************************************/
/*																	                 */
/* Function name : OnSize											           */
/* Description   : The framework calls this member function after	  */
/*				   the window’s size has changed.					        */
/*																	                 */
/********************************************************************/
void CStarWarsCtrl::OnSize(UINT nType, int cx, int cy) 
{
	CStatic::OnSize(nType, cx, cy);
	
	// OnSize automatically gets called during the setup of the control
	GetClientRect(m_rectClient);

	// destroy and recreate the main bitmap
	CClientDC dc(this);
	if (m_pOldBitmap && m_MainBitmap.GetSafeHandle() && m_MainDC.GetSafeHdc())
	{
		m_MainDC.SelectObject(m_pOldBitmap);
		m_MainBitmap.DeleteObject();
		m_MainBitmap.CreateCompatibleBitmap(&dc, m_rectClient.Width(), m_rectClient.Height());
		m_pOldBitmap = m_MainDC.SelectObject(&m_MainBitmap);
	}
}


/********************************************************************/
/*																	                 */
/* Function name : DoStars											           */
/* Description   : Draw stars										           */
/*																	                 */
/********************************************************************/
void CStarWarsCtrl::DoStars(CDC *pDC)
{
	m_MainDC.SetBkColor(RGB(0,0,0));
	m_MainDC.SetTextColor(RGB(255,255,255));
	m_MainDC.FillSolidRect(m_rectClient, RGB(0,0,0));

	int nFunFactor = 100;
	int x, y, z;
	for(int i = 0; i < NUM_STARS; i++)
	{
      m_StarArray[i].z = m_StarArray[i].z - m_nStarsSpeed;
      // wrap the starfield
      if (m_StarArray[i].z > 255)
			m_StarArray[i].z = -255;
      if (m_StarArray[i].z < -255)
			m_StarArray[i].z = 255;
      // work out star co-oridinates for the current "zoom" or window size  
		z = m_StarArray[i].z + 256;
      x = (m_StarArray[i].x * nFunFactor / z) + (m_rectClient.Width() / 2);
      y = (m_StarArray[i].y * nFunFactor / z) + (m_rectClient.Height() / 2);
		
		CPen myPen;

		// create a white pen which luminosity depends on the z position (for 3D effect!)
		int nColor = 255 - m_StarArray[i].z;
		myPen.CreatePen(PS_COSMETIC, 1, RGB(nColor,nColor,nColor));

		CPen *pOldPen = (CPen *)m_MainDC.SelectObject(&myPen);
		// draw star
		m_MainDC.Ellipse(CRect(x, y, x+3, y+3));
		m_MainDC.SelectObject(pOldPen);
    }
}


/********************************************************************/
/*					                  											  */
/* Function name : InvalidateCtrl         								  */
/* Description   : Draw the Matrix to a bitmap.			      		  */
/*																	                 */
/********************************************************************/
void CStarWarsCtrl::InvalidateCtrl()
{
	// in case we haven't established the memory dc's
	CClientDC dc(this);

	// if we don't have one yet, set up a memory dc for the control 
	if (m_MainDC.GetSafeHdc() == NULL)
	{
		m_MainDC.CreateCompatibleDC(&dc);
		m_MainBitmap.CreateCompatibleBitmap(&dc, m_rectClient.Width(), m_rectClient.Height());
		m_pOldBitmap = m_MainDC.SelectObject(&m_MainBitmap);
	}
  
	DoStars(&dc);	
	DoScrollText(&dc);

	// finally, force redraw
	InvalidateRect(m_rectClient);
} 


/********************************************************************/
/*																	                 */
/* Function name : OnTimer											           */
/* Description   : Update display									        */
/*																	                 */
/********************************************************************/
void CStarWarsCtrl::OnTimer(UINT nIDEvent) 
{
	if (nIDEvent == 1)
		InvalidateCtrl();

	CStatic::OnTimer(nIDEvent);
}

// -------------------------------------- RGBFor -----------------------------
// return an RGB value for the distance in the range 0-255
COLORREF RGBFor(int distance)
{
int RGcomponent;

   // clip all distances to 0 thru 255.
   if (distance >255)
      distance = 255;
   if (distance<0)
      distance = 0;
   // RED and GREEN are directly related to distance
   RGcomponent = distance;
   // B is always 0
   return (RGB(RGcomponent, RGcomponent, 0));
} // RGBFor


/********************************************************************/
/*																	                 */
/* Function name : DoScrollText										        */
/* Description   : Do scrolling text like in the movie 'Star Wars'  */
/*																	                 */
/********************************************************************/
void CStarWarsCtrl::DoScrollText(CDC *pDC)
{
int nPosX =0;
int nPosY =0;

CDC memDC;
CBitmap memBitmap;
CFont *oldFont;

	memDC.CreateCompatibleDC(pDC);
	memBitmap.CreateCompatibleBitmap(pDC, m_rectClient.Width(), m_rectClient.Height());
	memDC.SelectObject(&memBitmap);
	memDC.SetBkColor(RGB(0,0,0));
	memDC.SetTextColor(RGB(0,255,0));
   memDC.SetBkMode(TRANSPARENT);

	oldFont = memDC.SelectObject(&m_Font);

	// black
	memDC.BitBlt(0, 0, m_rectClient.Width(), m_rectClient.Height(), NULL, 0, 0, BLACKNESS);
	
	// draw Credits on the hidden Picture
	for(int i=0; i < m_TextLines.GetSize(); i++)
	{
   int waveEffect;   // CDB makes the text wave a little bit, and not move smooth
   CString currentTextLine(m_TextLines.GetAt(i));
		// set position for this line
		CSize size = memDC.GetTextExtent(currentTextLine);

		nPosY = m_nScrollPos + (i * size.cy);
      if (nPosY > m_rectClient.Height())
         break;

      // CDB
      // this next bit will confuse most folk looking, but the original Star Wars Scroller
      // used to have this wavy motion too.
      waveEffect = getrandom(0,1);
      if (0 == getrandom(0,1))
         nPosY -= waveEffect;

		if (nPosY > 0)
		{
			nPosX = (m_rectClient.Width() / 2) - (size.cx / 2);
         memDC.SetTextColor(RGBFor(nPosY));
			
			// print text
         currentTextLine = m_TextLines.GetAt(i);
			memDC.TextOut(nPosX, nPosY, currentTextLine); 
#ifdef _DEBUG
         {
            CString deb;
            deb.Format("% 2d (%d) %s  \n", i, nPosY, currentTextLine);
            OutputDebugString(deb);
         }
#endif
		}
		else
		{
			// start all over ...
			if (i == (m_TextLines.GetSize()-1))
			{
				m_nScrollPos = m_rectClient.Height();
			}
		}
	}

	int nWidth = m_rectClient.Width();
	int nHeight = m_rectClient.Height();

	// shrink text from bottom to top to create Star Wars effect
	for (int y=0; y <nHeight; y++)
	{
		double nScale = (double)y/(double)nHeight;

		int nCentreX = (int)(nWidth - nWidth*nScale)/2;
		m_MainDC.StretchBlt(nCentreX, y, (int)(nWidth*nScale), 1, 
                          &memDC, 0, y, nWidth, 1, SRCPAINT);
	}
   
	//	m_MainDC.StretchBlt(0, 0, (const int)m_rectClient.Width(), (const int)m_rectClient.Height(), 
   //                       &memDC, 0, 0, nWidth, nHeight, SRCPAINT);


	// restore the font
	memDC.SelectObject(oldFont);

	// move text up one pixel
	m_nScrollPos = m_nScrollPos - m_nScrollSpeed;
}


/********************************************************************/
/*																	                 */
/* Function name : SetScrollSpeed									        */
/* Description   : Set speed of scrolling							        */
/*																	                 */
/********************************************************************/
void CStarWarsCtrl::SetScrollSpeed(int nSpeed)
{
	m_nScrollSpeed = nSpeed;
}


/********************************************************************/
/*																	                 */
/* Function name : SetStarSpeed										        */
/* Description   : Set speed of the stars							        */
/*																	                 */
/********************************************************************/
void CStarWarsCtrl::SetStarSpeed(int nSpeed)
{
	m_nStarsSpeed = nSpeed;
}


/********************************************************************/
/*																	                 */
/* Function name : AddTextLine										        */
/* Description   : Add line to credits								        */
/*																	                 */
/********************************************************************/
void CStarWarsCtrl::AddTextLine(LPCTSTR lpszText)
{
	m_TextLines.Add(lpszText);
}

