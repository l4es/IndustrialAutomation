#include "stdafx.h"
#include "TestModule.h"

#include <string>
#include <boost/format.hpp>

BOOL TestHR(HRESULT hr,LPCSTR file, ULONG line)
{
	if(hr != S_OK)
	{
		LPVOID lpMsgBuf;

		FormatMessage(
						FORMAT_MESSAGE_ALLOCATE_BUFFER | 
						FORMAT_MESSAGE_FROM_SYSTEM | 
						FORMAT_MESSAGE_IGNORE_INSERTS,
						NULL,
						hr,
						MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
						(LPTSTR) &lpMsgBuf,
						0,
						NULL );
		std::string msg;
#ifdef _DEBUG
		msg = (boost::format("File:%s\nLine:%u\n%s\nHRESULT: %X\n\nПродолжить выполнение программы?") % file % line % lpMsgBuf % hr).str();
		LocalFree( lpMsgBuf );
		if(MessageBoxA( NULL, msg.c_str(), "Ошибка", MB_YESNO | MB_ICONERROR | MB_DEFBUTTON2 ) != IDYES)
			TerminateProcess(GetCurrentProcess(),hr);
#else
		msg = (boost::format("================================\nFile:%s\nLine:%u\n%s\n=========================================\n") % file % line % lpMsgBuf).str();
		LocalFree( lpMsgBuf );
#endif		
		return FALSE;
	}
	return TRUE;
}