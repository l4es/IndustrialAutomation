#pragma once

#ifndef HELPERS_LIB_API
#	ifdef HELPERS_LIB_EXPORTS
#		define HELPERS_LIB_API __declspec(dllexport)
#	else
#		define HELPERS_LIB_API __declspec(dllimport)
#	endif
#endif //HELPERS_LIB_API


#define STL_USING_ALL
#include "Stl.h"
#include <algorithm>

namespace HelpersLib
{

#ifdef UNICODE
	typedef std::wstring tstring;
	typedef std::wifstream tifstream;
	typedef std::wofstream tofstream;
	typedef std::wostringstream tostringstream;
	typedef std::wstringstream tstringstream;
#else
	typedef std::string tstring;
	typedef std::ifstream tifstream;
	typedef std::ofstream tofstream;
	typedef std::ostringstream tostringstream;
	typedef std::stringstream tstringstream;
#endif

}
