// EasterDlg.cpp : implementation file
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
//

#include "stdafx.h"
#include "mod_rssim.h"
#include "StarWarsCtrl.h"
#include "EasterDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CEasterDlg dialog


CEasterDlg::CEasterDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CEasterDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CEasterDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CEasterDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CEasterDlg)
	DDX_Control(pDX, IDC_STARTSCTRL, m_starsCtrl);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CEasterDlg, CDialog)
	//{{AFX_MSG_MAP(CEasterDlg)
	ON_WM_LBUTTONDOWN()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CEasterDlg message handlers

#define BANNER_LINES    68    // # lines of text
#define _USER_EGG       "<<USER>>" 
#define _AUTHOR_MAIL	"<<AUTHORMAIL>>"

char starBannerText[BANNER_LINES][50] = 
{
   "In a galaxy far far away",   //0
   "this app. was written by",   //1
   "",                           //2
   "Conrad Braam",               //3
   "<<AUTHORMAIL>>",		     // 4 "zaphodikus@hotmail.com",       //3 !!!!!!
   "",                           //5
   "Dedicated to my son",
   "Rhys",
   "",
   "",
   "Credits:",   //10
   "Pablo van der Meer -",   //6
   " for this control",   //7
   "Paul DiLascia -",   //8
   " CStaticLink &",   //9
   "\"If this code works...\"",   //15
   "Baldvin Hansson -",   //16
   " Colored ComboBox",    //17
   "JP Arendse -",
   "CFileEditCtrl",
   "www.codeguru.com -",   
   " Great sources",   
   "www.codeproject.com -",   
   " Great sources",   
   "Hirofumi Fudoudou -",
   "bug fixes",
   "Álvaro Palma -",
   "bug fixes",
   "Patrick SAFORCADA -",
   "bug fixes",
   "Dmitry Kochin -",
   "dialog resizer",
   "Ernest Laurentin - ",
   "Script engine addition",
   "Melvin Philips -",
   "Beer money",
   "",   //            TODO: Add credits here...
   "Thanks to:",
   "My wife,", // 
   "my mom",
   "my cat",
   "my dog",
   "my boss",              //
   "",
   "Visit Adroit",
   "on their website",
   "www.adroitscada.com",
   "or",
   "www.adroit.co.za",
   "",
   "",    //
   "",
   "<<USER>>", // Greets-
   "",
   "",
   "",
   "",
   "",
   "[ESC] key ends.",   // 
   "",
   "that's all",
   "",
   "."          // 
};

BOOL CEasterDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	m_starsCtrl.SetStarSpeed(30);
	m_starsCtrl.SetScrollSpeed(2);

   for (int i=0; i < BANNER_LINES; i++)
   {
      if (0== strcmp(starBannerText[i], _USER_EGG))
      {
         m_starsCtrl.AddTextLine("Hi!");
         m_starsCtrl.AddTextLine(m_userName);
      }
      else
		  if (0==strcmp(starBannerText[i], _AUTHOR_MAIL))
		  {
			 m_starsCtrl.AddTextLine(lpAuthor_email);
		  }
		  else
			  m_starsCtrl.AddTextLine(starBannerText[i]);
   }
	UpdateData(FALSE);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

// ----------------------------- OnLButtonDown ----------------------------
// left-click opens the website.
//
void CEasterDlg::OnLButtonDown(UINT nFlags, CPoint point) 
{
CString szLink;
              
   szLink.LoadString(IDS_HOMEPAGEURL);

   OnCancel();


   HINSTANCE h = ShellExecute(NULL, "open", szLink, NULL, NULL, SW_SHOWNORMAL);
   if ((UINT)h > 32) 
   {
      //OnCancel(); // success
   } 
   else 
   {
      MessageBeep(0);          // unable to execute file!
      TRACE(_T("*** WARNING: CEasterDlg: unable to execute file/URL %s\n"),
               (LPCTSTR)szLink);
      //OnCancel(); // close anyway
   }
}
