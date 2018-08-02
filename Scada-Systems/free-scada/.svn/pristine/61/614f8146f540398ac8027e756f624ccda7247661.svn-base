
#include "stdafx.h"
#include "Utility.h"
#include <atldlgs.h>

namespace crash_reporter
{
	FILETIME CUtility::getLastWriteFileTime(std::wstring sFile)
	{
		FILETIME          ftLocal = {0};
		HANDLE            hFind;
		WIN32_FIND_DATA   ff32;
		hFind = FindFirstFile(sFile.c_str(), &ff32);
		if (INVALID_HANDLE_VALUE != hFind)
		{
			FileTimeToLocalFileTime(&(ff32.ftLastWriteTime), &ftLocal);
			FindClose(hFind);        
		}
		return ftLocal;
	}


	std::wstring CUtility::getAppName()
	{
		TCHAR szFileName[_MAX_PATH];
		GetModuleFileName(NULL, szFileName, _MAX_FNAME);

		CString sAppName; // Extract from last '\' to '.'
		sAppName = szFileName;
		sAppName = sAppName.Mid(sAppName.ReverseFind(_T('\\')) + 1)
			.SpanExcluding(_T("."));

		return (LPCWSTR)sAppName;
	}


	std::wstring CUtility::getTempFileName()
	{
		TCHAR szTempDir[MAX_PATH - 14]   = _T("");
		TCHAR szTempFile[MAX_PATH]       = _T("");

		if (GetTempPathW(MAX_PATH - 14, szTempDir))
			GetTempFileNameW(szTempDir, getAppName().c_str(), 0, szTempFile);

		return szTempFile;
	}


	std::wstring CUtility::getSaveFileName()
	{
		CFileDialog fd(FALSE, _T("zip"), getAppName().c_str(), 
			OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, L"Zip files (*.zip)\0*.zip\0");

		if (IDOK == fd.DoModal())
		{
			DeleteFile(fd.m_szFileName);  // Just in-case it already exist
			return fd.m_szFileName;
		}

		return _T("");
	}
}
