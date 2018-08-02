// CG: This file was added by the Splash Screen component.
// Splash.cpp : implementation file
//
// Mod_RSSim (c) Embedded Intelligence Ltd. 1993,2009
// AUTHOR: Conrad Braam.  http://www.plcsimulator.org
//
//    This program is free software: you can redistribute it and/or modify
//    it under the terms of the GNU Affero General Public License as
//    published by the Free Software Foundation, either version 3 of the
//    License, or (at your option) any later version.
//
//    This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU Affero General Public License for more details.
//
//    You should have received a copy of the GNU Affero General Public License
//    along with this program.  If not, see <http://www.gnu.org/licenses/>.
//

#include "stdafx.h"  
#include "resource.h"
#include "Splash.h"  

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
//   Splash Screen class

BOOL CSplashWnd::c_bShowSplashWnd;
CSplashWnd* CSplashWnd::c_pSplashWnd;

int   CSplashWnd::m_splashShowCount;
int   CSplashWnd::m_splashShowDelay=1;

// Dynamic-text area control constants
#define LEFTMARGIN         36
#define TOPMARGIN          236      // 305


// ------------------------------- CSplashWnd --------------------------------
// constructor
CSplashWnd::CSplashWnd()
{
   m_pFont = NULL;
   m_pLargeFont = NULL;
   m_textDrawPosY = TOPMARGIN;
}

CSplashWnd::~CSplashWnd()
{
	// Clear the static window pointer.
	ASSERT(c_pSplashWnd == this);
	c_pSplashWnd = NULL;
   if (NULL != m_pFont)
      delete m_pFont;
   if (NULL !=m_pLargeFont)
      delete m_pLargeFont;
}

BEGIN_MESSAGE_MAP(CSplashWnd, CWnd)
	//{{AFX_MSG_MAP(CSplashWnd)
	ON_WM_CREATE()
	ON_WM_PAINT()
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

// --------------------------- EnableSplashScreen ------------------------------
void CSplashWnd::EnableSplashScreen(BOOL bEnable /*= TRUE*/)
{
	c_bShowSplashWnd = bEnable;
}

// -------------------------- ShowSplashScreen ----------------------------------
void CSplashWnd::ShowSplashScreen(CWnd* pParentWnd /*= NULL*/)
{
	if (!c_bShowSplashWnd || c_pSplashWnd != NULL)
		return;

	// Allocate a new splash screen, and create the window.
	c_pSplashWnd = new CSplashWnd;
	if (!c_pSplashWnd->Create(pParentWnd))
		delete c_pSplashWnd;
	else
		c_pSplashWnd->UpdateWindow();
}

// ------------------------------ PreTranslateAppMessage ------------------------
BOOL CSplashWnd::PreTranslateAppMessage(MSG* pMsg)
{
	if (c_pSplashWnd == NULL)
		return FALSE;

	// If we get a keyboard or mouse message, hide the splash screen.
	if (pMsg->message == WM_KEYDOWN ||
	    pMsg->message == WM_SYSKEYDOWN ||
	    pMsg->message == WM_LBUTTONDOWN ||
	    pMsg->message == WM_RBUTTONDOWN ||
	    pMsg->message == WM_MBUTTONDOWN ||
	    pMsg->message == WM_NCLBUTTONDOWN ||
	    pMsg->message == WM_NCRBUTTONDOWN ||
	    pMsg->message == WM_NCMBUTTONDOWN)
	{
      // at least 1 seconds delay
      if (abs(m_splashShowDelay - m_splashShowCount) >=1)
      {
		   c_pSplashWnd->HideSplashScreen();
		   return TRUE;	// message handled here
      }
	}

	return FALSE;	// message not handled
}

// ---------------------------------- Create -------------------------------
BOOL CSplashWnd::Create(CWnd* pParentWnd /*= NULL*/)
{
   
   if (!m_bitmap.LoadBitmap(IDB_SPLASH))
		return FALSE;

	BITMAP bm;
	m_bitmap.GetBitmap(&bm);

	return CreateEx(0,
		AfxRegisterWndClass(0, AfxGetApp()->LoadStandardCursor(IDC_ARROW)),
		NULL, WS_POPUP | WS_VISIBLE, 0, 0, bm.bmWidth, bm.bmHeight, pParentWnd->GetSafeHwnd(), NULL);
}

// -------------------------------- HideSplashScreen -----------------------
void CSplashWnd::HideSplashScreen()
{
	// Destroy the window, and update the mainframe.
	DestroyWindow();
	AfxGetMainWnd()->UpdateWindow();
}

// --------------------------------- PostNcDestroy ----------------------
void CSplashWnd::PostNcDestroy()
{
	// Free the C++ class.
	delete this;
}

// --------------------------------- OnCreate ---------------------------
int CSplashWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	// Center the window.
	CenterWindow();

	// Set a timer to destroy the splash screen.
	SetTimer(1, 1000, NULL);
	m_splashShowCount = m_splashShowDelay;

	return 0;
}

// ------------------------------- AddDrawText --------------------------
// output a line of text on top of the splash, and incremnt a vertical pixel position
void CSplashWnd::AddDrawText(CPaintDC& dc, const char *text, COLORREF color/*= INVALID_SPLASH_TXT_COLOR*/)
{
LOGFONT logFont;
   if (color != INVALID_SPLASH_TXT_COLOR)
      dc.SetTextColor(color);
   dc.TextOut(LEFTMARGIN, m_textDrawPosY, text);
   
   // increment vertical positioning
   dc.GetCurrentFont()->GetLogFont(&logFont);
   m_textDrawPosY += (int)(logFont.lfHeight * 0.95f); //DRAWTEXT_HEIGHT;
}


// --------------------------------- OnPaint ------------------------------
void CSplashWnd::OnPaint()
{
CFont *pOldFont;
CString tempText;

   CPaintDC dc(this);

	CDC dcImage;
	if (!dcImage.CreateCompatibleDC(&dc))
		return;

	BITMAP bm;
	m_bitmap.GetBitmap(&bm);

	// Paint the image.
	CBitmap* pOldBitmap = dcImage.SelectObject(&m_bitmap);
	dc.BitBlt(0, 0, bm.bmWidth, bm.bmHeight, &dcImage, 0, 0, SRCCOPY);

   ///////////////////////////////////////////////////////////
   // Draw some text over the splash bitmap
   if (NULL == m_pFont)
   {
   LOGFONT logFont;

      memset(&logFont,0,sizeof(logFont));
      logFont.lfHeight = 16;
      logFont.lfWeight = 400;
      strcpy_s(logFont.lfFaceName, sizeof(logFont.lfFaceName), "Arial");
      m_pFont = new CFont;
      m_pFont->CreateFontIndirect(&logFont);
      
      logFont.lfHeight = 26;
      logFont.lfWeight = 400; //normal
      m_pLargeFont = new CFont;
      m_pLargeFont->CreateFontIndirect(&logFont);
   }

   ASSERT(NULL != m_pFont);
   pOldFont = (CFont*)dc.SelectObject(m_pLargeFont);

   dc.SetBkMode(TRANSPARENT);
   m_textDrawPosY = TOPMARGIN;

   tempText.Format("Version %s", lpsMyAppVersion);
   AddDrawText(dc, tempText, RGB(5,5,255));   // blue

   // smaller white text
   dc.SelectObject(m_pFont);

   AddDrawText(dc, "Mod_RSSim is a test utility used to test communications protocol", RGB(255,255,255));  // white
   AddDrawText(dc, "drivers. (c) 1993,2009 Embedded Intelligence Limited"); 
   tempText.Format("Email: %s or WEB: www.plcsimulator.org", lpAuthor_email); //zaphodikus@hotmail.com";
   AddDrawText(dc, tempText);
   AddDrawText(dc, " ");
   AddDrawText(dc, "This program is free software: you can redistribute it and/or modify");
   AddDrawText(dc, "it under the terms of the GNU General Public License as published by");
   AddDrawText(dc, "the Free Software Foundation, either version 3 of the License, or");
   AddDrawText(dc, "(at your option) any later version.");
   AddDrawText(dc, "This program is distributed in the hope that it will be useful,");
   AddDrawText(dc, "but WITHOUT ANY WARRANTY; without even the implied warranty of");
   AddDrawText(dc, "MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the");
   AddDrawText(dc, "GNU General Public License for more details.");
   AddDrawText(dc, " ");

   // registration code
   if (pGlobalDialog->m_registration.IsRegistered())
      tempText.Format("Registered user: %s   Key: %s", pGlobalDialog->m_registeredUser, pGlobalDialog->m_registeredKey);
   else
      tempText.Format("Unregistered user");
   AddDrawText(dc, tempText, RGB(175,5,5));   // dark red


   // release DC resources
   dc.SelectObject(pOldFont);
	dcImage.SelectObject(pOldBitmap);
}

// ------------------------------- OnTimer -----------------------------
// count down timer messages untill we want to hide the splash screen
void CSplashWnd::OnTimer(UINT nIDEvent)
{
   if (0 == m_splashShowCount)
   {
   	// Destroy the splash screen window.
	   HideSplashScreen();
   }
   else
      m_splashShowCount--;
}
