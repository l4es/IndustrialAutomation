#pragma once

#ifndef ZLIB_DLL
#	define ZLIB_DLL
#endif // !ZLIB_DLL

#ifndef _WINDOWS
#	define _WINDOWS
#endif // !_WINDOWS

#ifndef _zip_H
#	include "zip.h"
#	pragma comment(lib, "zlib.lib")
#endif // _zip_H

#include <string>

class CZLib
{
public:
	CZLib();
	virtual ~CZLib();

	BOOL Open(std::wstring f_file, int f_nAppend = 0);
	BOOL AddFile(std::wstring f_file);
	void Close();
protected:
	zipFile m_zf;
};

