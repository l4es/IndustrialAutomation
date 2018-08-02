#include "StdAfx.h"
#include "StrConv.h"

#include <stdlib.h>
#include <boost/bind.hpp>
#include <locale>
#include <algorithm>

#ifndef LINUX
#	define WIN32_LEAN_AND_MEAN
#	include <Windows.h>
#endif

namespace HelpersLib
{
	std::string conv_wstr_to_str(const std::wstring& str)
	{
#ifndef LINUX
		const int buff_size = WideCharToMultiByte(CP_ACP, NULL, str.c_str(), (int)str.size(), NULL, NULL, NULL, NULL);
		char *buff = new char[buff_size+1];
		WideCharToMultiByte(CP_ACP, NULL, str.c_str(), (int)str.size(), buff, buff_size, NULL, NULL);
		buff[buff_size] = '\0';
		std::string result = buff;
		delete[] buff;
		return result;

#else
		const int buff_size = wcstombs(NULL, str.c_str(), 0);
		char *buff = new char[buff_size+1];
		wcstombs(buff, str.c_str(), buff_size);
		std::string result = buff;
		delete buff;
		return result;
#endif
	}

	std::wstring conv_str_to_wstr(const std::string& str)
	{
#ifndef LINUX
		const int buff_size = MultiByteToWideChar(CP_ACP, NULL, str.c_str(), (int)str.size(), NULL, NULL);
		wchar_t *buff = new wchar_t[buff_size+1];
		MultiByteToWideChar(CP_ACP, NULL, str.c_str(), (int)str.size(), buff, buff_size);
		buff[buff_size] = L'\0';
		std::wstring result = buff;
		delete[] buff;
		return result;
#else
		int buff_size = mbstowcs( NULL, str.c_str(), 0);
		wchar_t *buff = new wchar_t[buff_size+1];
		mbstowcs(buff, str.c_str(), buff_size); 
		std::wstring result = buff;
		delete buff;
		return result;
#endif
	}

	tstring conv_to_tstring(const std::wstring& str)
	{
#ifdef UNICODE
		return str;
#else
		return conv_wstr_to_str(str);
#endif
	}

	tstring conv_to_tstring(const std::string& str)
	{
#ifdef UNICODE
		return conv_str_to_wstr(str);
#else
		return str;
#endif
	}

	tstring conv_to_tstring(const ATL::CString& str)
	{
		return (LPCTSTR)str;
	}

	std::string conv_to_utf8(const tstring& str)
	{
		const int buff_size = WideCharToMultiByte(CP_UTF8, NULL, str.c_str(), (int)str.size(), NULL, NULL, NULL, NULL);
		char *buff = new char[buff_size+1];
		WideCharToMultiByte(CP_UTF8, NULL, str.c_str(), (int)str.size(), buff, buff_size, NULL, NULL);
		buff[buff_size] = '\0';
		std::string result = buff;
		delete[] buff;
		return result;
	}

	tstring conv_to_lower(const tstring& str)
	{
		tstring res;
		std::locale loc;
		std::transform(	str.begin(), str.end(), std::back_inserter(res),
			boost::bind(std::tolower<tstring::value_type>, _1, loc));

		return res;
	}
}
