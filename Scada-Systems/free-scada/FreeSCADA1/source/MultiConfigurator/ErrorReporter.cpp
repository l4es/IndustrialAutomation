#include "StdAfx.h"
#include ".\errorreporter.h"

namespace Helpers
{
	CAutoSingletonCreator<CErrorReporter> __ErrorReporter;

	CErrorReporter::CErrorReporter(void)
	{
	}

	CErrorReporter::~CErrorReporter(void)
	{
	}

	void CErrorReporter::ShowError(IErrorPtr pErr)
	{
		CString msg = (LPCTSTR)pErr->ErrorDescription;
		MessageBox(NULL, msg, _T("Error"), MB_OK|MB_ICONSTOP);
	}

	void CErrorReporter::DecodeHRESULT(HRESULT hr)
	{
		LPVOID lpMsgBuf;
		if (!FormatMessage(
			FORMAT_MESSAGE_ALLOCATE_BUFFER | 
			FORMAT_MESSAGE_FROM_SYSTEM | 
			FORMAT_MESSAGE_IGNORE_INSERTS,
			NULL,
			hr,
			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
			(LPTSTR) &lpMsgBuf,
			0,
			NULL))
		{
			return;
		}

		MessageBox( NULL, (LPCTSTR)lpMsgBuf, L"Error", MB_OK | MB_ICONSTOP );
		LocalFree( lpMsgBuf );
	}
}