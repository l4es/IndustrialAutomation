/////////////////////////////////////////////////////////////////////////////
//
// FILE: MOD_sim.h : headder file
//
// Mod_RSSim (c) Embedded Intelligence Ltd. 1993,2009
// AUTHOR: Conrad Braam.  http://www.plcsimulator.org
// email:zaphodikus@hotmail.com
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
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_TI_SIM_H__D74EE401_3D1B_4510_B1F5_58781391FEF1__INCLUDED_)
#define AFX_TI_SIM_H__D74EE401_3D1B_4510_B1F5_58781391FEF1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

#ifndef WS_EX_LAYERED
#define WS_EX_LAYERED           0x00080000
#define LWA_COLORKEY            0x00000001
#define LWA_ALPHA               0x00000002
#endif // ndef WS_EX_LAYERED

// Preparation for the function we want to import from USER32.DLL
typedef BOOL (WINAPI *lpfnSetLayeredWindowAttributes)(HWND hWnd, COLORREF crKey, BYTE bAlpha, DWORD dwFlags);

extern lpfnSetLayeredWindowAttributes m_pSetLayeredWindowAttributes;


extern BOOL GetTNImport();

extern CHAR lpsMyAppVersion[10];
#define __MY_APPVERSION__      (lpsMyAppVersion)

#define SIMULATOR_WINDOW_CLASSNAME  "MYSimulator_Window_Classname"

extern const CHAR lpAuthor_email[40];

/////////////////////////////////////////////////////////////////////////////
// CMOD_simApp:
// See MOD_sim.cpp for the implementation of this class
//

class CMOD_simApp : public CWinApp
{
public:
	CMOD_simApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMOD_simApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation
private:
   CString m_htmBookmark;

public:
   void GetMyHelpTopic(CString &topic);
   void SetMyHelpTopic(LPCTSTR topic);

   void OnHelp();
	//{{AFX_MSG(CMOD_simApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

class SetCurrentTopic
{
public:
   SetCurrentTopic();
   SetCurrentTopic(LPCTSTR topic);
   ~SetCurrentTopic();

   void SetHelpTopic(LPCTSTR topic) { SetTopic(topic);};

private:
   CString m_topicLast;
   void SetTopic(LPCTSTR topic);
   void GetTopic(CString& topic);
};




/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TI_SIM_H__D74EE401_3D1B_4510_B1F5_58781391FEF1__INCLUDED_)
