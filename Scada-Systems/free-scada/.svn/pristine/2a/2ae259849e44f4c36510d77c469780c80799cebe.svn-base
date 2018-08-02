// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#pragma once

// Change these values to use different versions
#define WINVER		0x0400
#define _WIN32_WINNT	0x0400
#define _WIN32_IE	0x0400
#define _RICHEDIT_VER	0x0100

#define _ATL_APARTMENT_THREADED
#define  _WTL_NO_CSTRING

#include <atlstr.h>
#include <atlbase.h>
#include <atlapp.h>

extern CAppModule _Module;

// This is here only to tell VC7 Class Wizard this is an ATL project
#ifdef ___VC7_CLWIZ_ONLY___
CComModule
CExeModule
#endif

#include <atlcom.h>
#include <atlwin.h>

#include <atlframe.h>
#include <atlctrls.h>
#include <atldlgs.h>
#include <atlddx.h>
#include <atlctrlx.h>
#include <atlcrack.h>

//Other
#include "../Library/Library.h"

#include <HtmlHelp.h>

#pragma comment(lib,"Htmlhelp.Lib")
#pragma comment(lib,"comdlg32.lib")

//Constants
const wchar_t cFilterProject[]	= L"Project files (*.mn)\0*.mn\0";
const wchar_t cProduct[]		= L"FreeSCADA\\Визуализатор";

//интервал обновления графики
#define GUI_UPDATE_ITERVAL		50 /*мс*/
//интервал обновления действий
#define ACTIONS_UPDATE_ITERVAL	100 /*мс*/
//Множитель тайаута ответа от OPC
#define TIMEOUT_MUL				5