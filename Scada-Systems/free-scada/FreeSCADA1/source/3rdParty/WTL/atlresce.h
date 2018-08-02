// Windows Template Library - WTL version 8.0
// Copyright (C) Microsoft Corporation. All rights reserved.
//
// This file is a part of the Windows Template Library.
// The use and distribution terms for this software are covered by the
// Common Public License 1.0 (http://opensource.org/licenses/cpl.php)
// which can be found in the file CPL.TXT at the root of this distribution.
// By using this software in any fashion, you are agreeing to be bound by
// the terms of this license. You must not remove this notice, or
// any other, from this software.

#ifndef __ATLRESCE_H__
#define __ATLRESCE_H__

#pragma once

#ifndef _WIN32_WCE
	#error atlresCE.h is only for Windows CE
#endif


#ifdef RC_INVOKED
#ifndef _INC_WINDOWS

  #define VS_VERSION_INFO     1

  #ifdef APSTUDIO_INVOKED
    #define APSTUDIO_HIDDEN_SYMBOLS // Ignore following symbols
  #endif // APSTUDIO_INVOKED

  #ifndef WINVER
    #define WINVER 0x0400   // default to Windows Version 4.0
  #endif // !WINVER

  #if !defined(WCEOLE_ENABLE_DIALOGEX)
    #define DIALOGEX DIALOG DISCARDABLE
  #endif

  #include <commctrl.h>
  #define  SHMENUBAR RCDATA

  #if defined(SHELLSDK_MODULES_AYGSHELL)
    #include <aygshell.h> 
  #else
    #define I_IMAGENONE            (-2)
    #define NOMENU                 0xFFFF
    #define IDS_SHNEW              1
    #define IDM_SHAREDNEW          10
    #define IDM_SHAREDNEWDEFAULT   11
  #endif

  #include <windows.h>

#endif // !_INC_WINDOWS
#endif // RC_INVOKED

#include <atlres.h>

#ifdef APSTUDIO_INVOKED
	#undef APSTUDIO_HIDDEN_SYMBOLS
#endif // APSTUDIO_INVOKED

#define IDC_INFOSTATIC 0xFFFE   // == IDC_STATIC -1

///////////////////////////////////////////////////////////////////////////////
// Smartphone Resource IDs

#ifdef WIN32_PLATFORM_WFSP

#define ID_MENU_OK                      0xE790
#define ID_MENU_CANCEL                  0xE791

#define ATL_IDW_SPIN_ID                 9999

#define ATL_IDS_DONE                    0xEF20
#define ATL_IDS_CANCEL                  0xEF21

#define ATL_IDM_MENU_DONE               0xE701
#define ATL_IDM_MENU_CANCEL             0xE702
#define ATL_IDM_MENU_DONECANCEL         0xE703

#endif // WIN32_PLATFORM_WFSP

#endif // __ATLRESCE_H__
