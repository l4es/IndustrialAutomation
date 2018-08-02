#pragma once
// Change these values to use different versions
#define WINVER		0x0400
#define _WIN32_WINNT	0x0400
#define _WIN32_IE	0x0400
#define _RICHEDIT_VER	0x0100

#include <atlbase.h>
#include <atlapp.h>

extern CAppModule _Module;

#include <atlwin.h>
#include <atlstr.h>

#include "../HelpersLib/HelpersLib.h"

#include <map>
#include <string>
#include <vector>

#pragma comment(lib,"comdlg32.lib")

namespace crash_reporter
{
	typedef std::map<std::wstring,std::wstring> string_map_t;
}
