#pragma once
#include "Global.h"
#include <string>
#include <atlstr.h>

namespace HelpersLib
{
	HELPERS_LIB_API std::string conv_wstr_to_str(const std::wstring& str);
	HELPERS_LIB_API std::wstring conv_str_to_wstr(const std::string& str);
	
	HELPERS_LIB_API tstring conv_to_tstring(const std::wstring& str);
	HELPERS_LIB_API tstring conv_to_tstring(const std::string& str);
	HELPERS_LIB_API tstring conv_to_tstring(const ATL::CString& str);

	HELPERS_LIB_API std::string conv_to_utf8(const tstring& str);

	HELPERS_LIB_API tstring conv_to_lower(const tstring& str);
}
