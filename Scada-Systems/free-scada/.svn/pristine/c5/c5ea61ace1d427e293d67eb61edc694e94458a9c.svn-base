#include "StdAfx.h"
#include "StdPaths.h"

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <shlobj.h>
#include <atlpath.h>

namespace HelpersLib
{
	bool CreatePath(const tstring& p)
	{
		int res = SHCreateDirectoryExW(NULL, p.c_str(),NULL);
		return (res == ERROR_SUCCESS) || (res == ERROR_FILE_EXISTS) || (res == ERROR_ALREADY_EXISTS);
	}

	tstring GetSharedConfigPath(const tstring& AppName)
	{
		HRESULT hr;
		TCHAR fldr_path[MAX_PATH];
		hr = SHGetFolderPath(NULL, CSIDL_COMMON_APPDATA|CSIDL_FLAG_CREATE, NULL, SHGFP_TYPE_CURRENT,fldr_path);
		if(FAILED(hr))
			return tstring();

		ATL::CPath p(fldr_path);
		if(!AppName.empty())
		{
			p.Append(AppName.c_str());
			if(CreatePath((LPCTSTR)p))
				return (LPCTSTR)p;
			else
				return tstring();
		}
		return (LPCTSTR)p;
	}

	tstring GetUserConfigPath(const tstring& AppName)
	{
		HRESULT hr;
		TCHAR fldr_path[MAX_PATH];
		hr = SHGetFolderPath(NULL, CSIDL_APPDATA|CSIDL_FLAG_CREATE, NULL, SHGFP_TYPE_CURRENT,fldr_path);
		if(FAILED(hr))
			return tstring();

		if(FAILED(hr))
			return tstring();

		ATL::CPath p(fldr_path);
		if(!AppName.empty())
		{
			p.Append(AppName.c_str());
			if(CreatePath((LPCTSTR)p))
				return (LPCTSTR)p;
			else
				return tstring();
		}
		return (LPCTSTR)p;
	}

	tstring GetDocumentsPath()
	{
		HRESULT hr;
		TCHAR fldr_path[MAX_PATH];
		hr = SHGetFolderPathW(NULL, CSIDL_PERSONAL|CSIDL_FLAG_CREATE, NULL, SHGFP_TYPE_CURRENT,fldr_path);
		if(FAILED(hr))
			return tstring();

		return fldr_path;
	}

	tstring GetTempPath()
	{
		TCHAR fldr_path[MAX_PATH+1];
#ifdef UNICODE
		::GetTempPathW(MAX_PATH, fldr_path);
#else
		::GetTempPathA(MAX_PATH, fldr_path);
#endif
		GetLongPathName(fldr_path, fldr_path, MAX_PATH);

		if(CreatePath(fldr_path))
			return fldr_path;
		else
			return tstring();
	}

	tstring GetExecutablePath()
	{
		HMODULE hThisModule = GetModuleHandle(NULL);
		TCHAR filename[MAX_PATH+1];
		int nTotalLen = GetModuleFileName(hThisModule, filename, MAX_PATH);
		filename[nTotalLen] = 0;
		ATL::CPath p(filename);
		p.RemoveFileSpec();
		return (LPCTSTR)p;
	}
}
