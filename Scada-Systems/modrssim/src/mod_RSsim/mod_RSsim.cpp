/////////////////////////////////////////////////////////////////////////////
//
// FILE: MOD_sim.cpp : implementation file
//
// Mod_RSSim (c) Embedded Intelligence Ltd. 1993,2009
// AUTHOR: Conrad Braam.  http://www.plcsimulator.org
// email: zaphodikus@hotmail.com
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
// Defines the class behaviors for the application.
//
//
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"



#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

const CHAR lpAuthor_email[40] = "zaphodikus@hotmail.com";


void HandleTooltipsActivation(MSG *pMsg, CWnd *This, CWnd *disabledCtrls[], int numOfCtrls, CToolTipCtrl *pTooltip)
{
CRect  rect;
POINT  pt;

HWND   hWnd     = pMsg->hwnd;
LPARAM lParam   = pMsg->lParam;

//---------------------------------------------------------------------------
//      Disabled control do not show tool tips, in modal dialog
//
//
//      The hwnd of the WM_MOUSEMOVE above a disabled control
//      is the hWnd of the Dialog itself, this confuse the tooltip
//
//      To correct this, if we get WM_MOUSEMOVE and the hwnd is the dialog's hwnd
//
//      We check on all the controls that are Visible, but disabled if the point is in their
//  rectangle.
//
// In this case we alter the msg to the controls hWnd and coordinates before
// Relaying it to the toolTip control
//----------------------------------------

    
   if( (pMsg->message == WM_MOUSEMOVE) && (pMsg->hwnd == This->m_hWnd)) 
   {
      //---------------------------
      // The point is in the dialog 
      // client coordinates
      //---------------------------
      pt.x = LOWORD(pMsg->lParam);  // horizontal position of cursor 
      pt.y = HIWORD(pMsg->lParam);  // vertical position of cursor 

      for (int i = 0; i < numOfCtrls; i++) 
      {
         //---------------------------------
         // rect is the control rectangel in
         // Dialog client coordinates
         //----------------------------------
         disabledCtrls[i]->GetWindowRect(&rect);
         This->ScreenToClient(&rect);                                        

         if(rect.PtInRect(pt) ) 
         {
                 //----------------------------------------------------------------
                 // The mouse is inside the control
                 //
                 // 1. We change the Msg hwnd    to the controls hWnd
                 // 2. We change the Msg lParam  to the controls client coordinates
                 //
                 //----------------------------------------------------------------

                 pMsg->hwnd = disabledCtrls[i]->m_hWnd;

                 This->ClientToScreen(&pt);
                 disabledCtrls[i]->ScreenToClient(&pt);
                 pMsg->lParam = MAKELPARAM(pt.x, pt.y);
                 break;
         }
      }
   }


   //---------------------------------------
   //      Relay the msg to the tool tip control
   //---------------------------------------
   pTooltip->RelayEvent(pMsg);
   pTooltip->Activate(TRUE);

   //--------------------------------------
   //      Restore the original msg
   //--------------------------------------
   pMsg->hwnd = hWnd;
   pMsg->lParam = lParam;
}

// some little functiosn used to implement the alph-blend transparency
lpfnSetLayeredWindowAttributes m_pSetLayeredWindowAttributes = NULL;

// ---------------------------------------- GetTNImport ---------------------------
// This only works on 2000 onwards.
//
// Get a Fn pointer from USER32.DLL to do the transparency
BOOL GetTNImport()
{
OSVERSIONINFO os = { sizeof(os) };
GetVersionEx(&os);
// use m_bWin2k before any call to the m_pSetLayeredWindowAttributes to make sure we are runninng Win2K
BOOL m_bWin2K = ( VER_PLATFORM_WIN32_NT == os.dwPlatformId && os.dwMajorVersion >= 5 ); 

   if (!m_bWin2K)  // bail, requires Win2K or XP to work
      return FALSE;
   // Here we import the function from USER32.DLL
   HMODULE hUser32 = GetModuleHandle(_T("USER32.DLL"));
   m_pSetLayeredWindowAttributes = (lpfnSetLayeredWindowAttributes)GetProcAddress(hUser32, "SetLayeredWindowAttributes");

   // If the import did not succeed, make sure your app can handle it!
   if (NULL == m_pSetLayeredWindowAttributes)
	   return FALSE; //Bail out!!!
   return TRUE;
} // GetTNImport


/////////////////////////////////////////////////////////////////////////////
// CMOD_simApp

BEGIN_MESSAGE_MAP(CMOD_simApp, CWinApp)
	//{{AFX_MSG_MAP(CMOD_simApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG
	ON_COMMAND(ID_HELP, OnHelp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMOD_simApp construction

CMOD_simApp::CMOD_simApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}


/////////////////////////////////////////////////////////////////////////////
// The one and only CMOD_simApp object

CMOD_simApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CMOD_simApp initialization

BOOL CMOD_simApp::InitInstance()
{
//	if (!AfxSocketInit())
//	{
//		AfxMessageBox(IDP_SOCKETS_INIT_FAILED);
//		return FALSE;
//	}

	CoInitialize(NULL);
	AfxEnableControlContainer();
	// Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.

WNDCLASS wndcls;
   
   // Start with NULL defaults.
   memset(&wndcls, 0, sizeof(WNDCLASS));   
   // Get class information for default window class.
   ::GetClassInfo(AfxGetInstanceHandle(),"#32770",&wndcls);
   wndcls.lpszClassName = SIMULATOR_WINDOW_CLASSNAME;

   AfxRegisterClass(&wndcls);

   { // MOD simulator
	CMOD_simDlg dlg;
	m_pMainWnd = &dlg;
	int nResponse = dlg.DoModal();

	   if (nResponse == IDOK)
	   {
		   // TODO: Place code here to handle when the dialog is
		   //  dismissed with OK
	   }
	   else if (nResponse == IDCANCEL)
	   {
		   // TODO: Place code here to handle when the dialog is
		   //  dismissed with Cancel
	   }
   }
	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}

void CMOD_simApp::GetMyHelpTopic(CString &topic) 
{ 
   if (m_htmBookmark.GetLength()) 
      topic=m_htmBookmark.Mid(1);
   else 
      topic="";
}

void CMOD_simApp::SetMyHelpTopic(LPCTSTR topic) 
{ 
   if (strlen(topic)) 
      m_htmBookmark = "#";
   else 
      m_htmBookmark = "";
   m_htmBookmark += topic;
}

////////////////////////////////////////////////////////////////////////
// SetCurrentTopic class implementation

SetCurrentTopic::SetCurrentTopic()
{
   GetTopic(m_topicLast);
}


SetCurrentTopic::SetCurrentTopic(LPCTSTR topic)
{
   GetTopic(m_topicLast);
   SetTopic(topic);
}


SetCurrentTopic::~SetCurrentTopic()
{
   SetTopic(m_topicLast);
}

void SetCurrentTopic::SetTopic(LPCTSTR topic)
{
   ((CMOD_simApp*)AfxGetApp())->SetMyHelpTopic(topic);
}

void SetCurrentTopic::GetTopic(CString& topic)
{
   ((CMOD_simApp*)AfxGetApp())->GetMyHelpTopic(topic);
}

