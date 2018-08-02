// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#pragma once

// Change these values to use different versions
#define WINVER		0x0400
#define _WIN32_WINNT	0x0501
#define _WIN32_IE	0x0500
#define _RICHEDIT_VER	0x0100

#define _WTL_NO_CSTRING
#include <atlstr.h>

#include <atlbase.h>
#include <atlapp.h>

extern CAppModule _Module;


#include <atlwin.h>
#include <atlsplit.h>
#include <atlframe.h>
#define _ATL_USE_DDX_FLOAT
#include <atlddx.h>
#include <atlfile.h>
#include <atlctrls.h>
#include <atlcrack.h>
#include <atldlgs.h>
#include <atlctrlw.h>
#include <atlctrlx.h>
#include <atltheme.h>
#include <atlmisc.h>

#include "aboutdlg.h"
using namespace WTL;

#include <map>
#include <vector>
#include <string>
using namespace std;

#define _NO_TESTS
#include "../HelpersLib/HelpersLib.h"

#include "../3rdParty/opc/opcda.h"
#include "../3rdParty/opc/OpcEnum.h"

//DBAccessor
#import "libid:{2B4525FF-0734-4D9C-9FF4-0E3B6D462702}" rename_namespace("DBAccessor")
using namespace DBAccessor;

#include <HtmlHelp.h>
#pragma comment(lib,"Htmlhelp.Lib")
#pragma comment(lib,"comdlg32.lib")

#define WM_POSTCREATE (WM_APP+1)
#define WM_UPDATE (WM_APP+2)

//Node Icons
#define NI_FLDR_CLOSE		0
#define NI_FLDR_OPEN		1
#define NI_OPC_TAG			2
#define NI_DETAIL_OPTION	3
#define NI_OPC				4
#define NI_OPC_FLDR_CLOSE	5
#define NI_OPC_FLDR_OPEN	6
#define NI_UART				7
#define NI_SERVICES			8
#define NI_ROUTER			9
#define NI_ARCHIVER			10


extern CString BrowseComputer(HWND hParent);