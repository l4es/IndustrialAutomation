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
}