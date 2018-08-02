#include "StdAfx.h"
#include ".\eventlogger.h"
#include <atlstr.h>
#include <shlobj.h>

namespace Helpers
{
	CEventLogger::CEventLogger(void)
	{
		TCHAR szPath[MAX_PATH];

		if(SUCCEEDED(SHGetFolderPath(NULL,
									CSIDL_COMMON_APPDATA|CSIDL_FLAG_CREATE,
									NULL, 
									SHGFP_TYPE_CURRENT, 
									szPath))) 
		{
			PathAppend(szPath, L"FreeSCADA");
			PathAppend(szPath, L"Logs");
			const int res = SHCreateDirectoryEx(NULL, szPath, NULL);
			if(	res == ERROR_SUCCESS ||
				res == ERROR_FILE_EXISTS ||
				res == ERROR_ALREADY_EXISTS)
			{
				PathAppend(szPath, L"Archiver.log");
				_out.open(szPath);
			}
		}

	}

	CEventLogger::~CEventLogger(void)
	{
		_out.close();
	}

	void CEventLogger::LogMessage(CString msg)
	{
		if(_out.is_open())
		{
			_out<<GetTime()<<" "<<msg<<endl;
		}
	}

	void CEventLogger::DecodeIError(IErrorPtr err)
	{
		if(err == NULL)
			return;

		CComBSTR msg;
		err->get_ErrorDescription(&msg);
		OutputDebugString(_T("[Archiver] DecodeIError: "));
		OutputDebugString(CString(msg));
		OutputDebugString(L"\n");
		
		if(_out.is_open())
		{
			CString str;
			str = _T("DecodeIError: ") + CString(msg);
			_out<<GetTime()<<" "<<str<<endl;
		}
	}

	void CEventLogger::GenMsg(HRESULT msg)
	{
		HANDLE h; 

		h = RegisterEventSource(NULL, _T("FreeSCADA Archiver"));
		if (h == NULL)
			return;

		WORD wType;
		switch (msg & 0xC0000000)
		{
		case ERROR_SEVERITY_SUCCESS:
			wType = EVENTLOG_SUCCESS;
			break;
		case ERROR_SEVERITY_INFORMATIONAL:
			wType = EVENTLOG_INFORMATION_TYPE;
			break;
		case ERROR_SEVERITY_WARNING:
			wType = EVENTLOG_WARNING_TYPE;
			break;
		case ERROR_SEVERITY_ERROR:
			wType = EVENTLOG_ERROR_TYPE;
			break;
		default:
			_ASSERTE(0);
			__assume(0);
		}

		if (!ReportEvent(h, wType, 0, msg, NULL, 0, 0, NULL, NULL))
			return;

		DeregisterEventSource(h);

		TCHAR szMessage[1024];
		FormatMessage(FORMAT_MESSAGE_FROM_HMODULE|FORMAT_MESSAGE_ARGUMENT_ARRAY,
			NULL, msg, 0, szMessage, sizeof(TCHAR)*1024, (va_list *)NULL);
		OutputDebugString(_T("[Archiver] LogEvent: "));
		OutputDebugString(szMessage);

		if(_out.is_open())
		{
			CString str;
			str = _T("LogEvent: ") + CString(szMessage);
			_out<<GetTime()<<" "<<str<<endl;
		}
	}

	CString CEventLogger::GetTime()
	{
		CString str;
		SYSTEMTIME st;
		GetLocalTime(&st);
		str.Format(L"[%04u.%02u.%02u %02u:%02u:%02u:%03u]",	st.wYear,
				st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond, st.wMilliseconds);
		return str;
	}

	void CEventLogger::DecodeHRESULT(HRESULT err)
	{
		LPVOID lpMsgBuf;
		if (!FormatMessage(
			FORMAT_MESSAGE_ALLOCATE_BUFFER | 
			FORMAT_MESSAGE_FROM_SYSTEM | 
			FORMAT_MESSAGE_IGNORE_INSERTS,
			NULL,
			err,
			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
			(LPTSTR) &lpMsgBuf,
			0,
			NULL))
		{
			return;
		}
		if(_out.is_open())
		{
			_out<<GetTime()<<" "<<_T("DecodeHRESULT: ")<<(LPCTSTR)lpMsgBuf<<endl;
		}
		LocalFree( lpMsgBuf );
	}
}