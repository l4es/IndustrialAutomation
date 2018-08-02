// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#pragma once

// Change these values to use different versions
#define WINVER			0x0400
#define _WIN32_WINNT	0x0400
#define _WIN32_IE		0x0500
#define _RICHEDIT_VER	0x0100

#define _WTL_NO_CSTRING
#include <atlstr.h>
#include <atlbase.h>
#include <atlapp.h>
#include <ATLComTime.h>

extern CAppModule _Module;
#define _ATL_USE_DDX_FLOAT

#include <atlcom.h>
#include <atlhost.h>
#include <atlwin.h>
#include <atlctl.h>

#include <atlframe.h>
#include <atlctrls.h>
#include <atldlgs.h>
#include <atlctrlw.h>
#include <atlcoll.h>
#include <atlddx.h>
#include <atlctrlx.h>
#include <atlmisc.h>

#include "resource.h"
#include "../HelpersLib/HelpersLib.h"

#include <vector>
#include <string>
using namespace std;

//Script Parser
#include "../ScriptParser/ScriptParser.h"

#pragma comment(lib,"comdlg32.lib")
#pragma comment(lib,"winspool.lib")

//DBAccessor
#import "libid:{2B4525FF-0734-4D9C-9FF4-0E3B6D462702}" no_namespace

#define WM_POSTCREATE	(WM_APP+1)
//wParam - адресс задачи
#define WM_DELETE_TASK	(WM_APP+2)

const TCHAR cProduct[] = _T("FreeSCADA\\Генератор отчетов");
typedef vector<CString> TVString;

extern IDBConnectionPtr g_Connection;

extern CString BrowseComputer(HWND hParent);