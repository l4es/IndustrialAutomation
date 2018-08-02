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

#define  _WTL_NO_CSTRING
#define _ATL_USE_DDX_FLOAT

//ATL/WTL
#include <atlstr.h>
#include <atlbase.h>
#include <atlapp.h>
extern CAppModule _Module;
#include <atlwin.h>
#include <atlframe.h>
#include <atlctrls.h>
#include <atldlgs.h>
#include <atlctrlw.h>
#include <atlmisc.h>
#include <atlcoll.h>
#include <atlctrlx.h>
#include <atlddx.h>

//WTL
#pragma warning(push,0) //Disable warnings for 3rd parties
#include <Docking/DockingFrame.h>
#include <Docking/ExtDockingWindow.h>
#include <Tabframework/atlgdix.h>
#include <Tabframework/DotNetTabCtrl.h>
#include <Tabframework/TabbedFrame.h>
#include <Tabframework/TabbedDockingWindow.h>
#include <viksoe/atlctrlxp.h>
#include <viksoe/atlctrlxp2.h>
#pragma warning(pop) //Restore original warnings

//Boost
#include "boost/format.hpp"
using boost::format;
using boost::io::str;

//Other
#include "Library.h"
#include "resource.h"
#include <atlcrack.h>

#include "Primitives/Param.h"
#include <HtmlHelp.h>

#pragma comment(lib,"Htmlhelp.Lib")
#pragma comment(lib,"comdlg32.lib")

//Custom messages
enum enWindowMessages
{
	WM_POSTCREATE					= WM_APP+1,
	WM_UPDATEVIEW					= WM_APP+2,
	WM_UPDATE_PRIMITIVES_TOOLBAR	= WM_APP+3
};

//CustomColor
const COLORREF cBackgroundColor			= RGB(180,180,180);	//Цвет фона рабочей области
const COLORREF cDefaultPolylineColor	= RGB(0,125,0);		//Цвет по умолчанию для полилинии
const COLORREF cDefaultRectangleColor	= RGB(0,255,0);		//Цвет по умолчанию для прямоугольника
const COLORREF cDefaultEllipseColor		= RGB(0,0,125);		//Цвет по умолчанию для элипса
const COLORREF cDefaultImageColor		= RGB(128,128,128);	//Цвет по умолчанию для изображения
const COLORREF cDefaultTextColor		= RGB(0,0,0);	//Цвет по умолчанию для текста

//File filters
const wchar_t cFilterProject[]		= L"Project files (*.mn)\0*.mn\0";
const wchar_t cFilterSounds[]		= L"Audio files (*.wav)\0*.wav\0";
const wchar_t cFilterAllGraphics[]	= L"Graphic files\0*.gif;*.bmp;*.ico;*.jpg;*.jpeg;*.jpe;*.jif;*.jfif;*.pcd;*.pcx;*.png;*.ras;*.tga;*.tif;*.tiff;*.xif\0Все файлы (*.*)\0*.*\0Windows Bitmap (*.bmp)\0*.bmp\0Windows Icons (*.ico)\0*.ico\0JPEG (*.jpeg)\0*.jpg;*.jpeg;*.jpe;*.jif;*.jfif\0Kodak Photo CD (*.pcd)\0*.pcd\0PCX (*.pcx)\0*.pcx\0Portable Network Graphics (*.png)\0*.png\0Sun Raster File (*.ras)\0*.ras\0Targa (*.tga)\0*.tga\0Tag Image File Format (*.tif)\0*.tif;*.tiff;*.xif\0";
const wchar_t cProduct[]			= L"FreeSCADA\\Designer";