// ReportGenerator.cpp : main source file for ReportGenerator.exe
//

#include "stdafx.h"

#include "resource.h"

#include "aboutdlg.h"
#include "MainFrm.h"
#include "LoginDlg.h"

#include "../CrashReporter/CrashReporter.h"

CAppModule _Module;

int Run(LPTSTR /*lpstrCmdLine*/ = NULL, int nCmdShow = SW_SHOWDEFAULT)
{
	CMessageLoop theLoop;
	_Module.AddMessageLoop(&theLoop);

	int nRet = 0;

	HelpersLib::CCfgReg reg;
	reg.Open(cProduct);
	CLoginDlg dlg;
	dlg.m_Server	= reg.ReadString(L"LastServer",L"localhost").c_str();
	dlg.m_User		= reg.ReadString(L"LastUser",L"Administrator").c_str();

	BOOL Continue=FALSE;
	int tries = 3;
	while(tries-->0 && Continue == FALSE)
	{
		dlg.m_Password.Empty();
		if(dlg.DoModal() == IDOK)
		{
			Continue = dlg.IsValid();
			if(!Continue)
				MessageBox(NULL, L"Incorrect user name or password.", L"Login", MB_OK|MB_ICONWARNING);
			else
			{
				reg.WriteString(L"LastServer", (LPCTSTR)dlg.m_Server);
				reg.WriteString(L"LastUser", (LPCTSTR)dlg.m_User);
				g_Connection = dlg.GetConnection();
			}
		}
		else
			break;
	}
    
	if(Continue)
	{
		CMainFrame wndMain;
		if(wndMain.CreateEx() == NULL)
		{
			ATLTRACE(_T("Main window creation failed!\n"));
			return 0;
		}
		wndMain.ShowWindow(nCmdShow);
		nRet = theLoop.Run();
		_Module.RemoveMessageLoop();
	}
	g_Connection = NULL;
	return nRet;
}

int WINAPI _tWinMain(HINSTANCE hInstance, HINSTANCE /*hPrevInstance*/, LPTSTR lpstrCmdLine, int nCmdShow)
{
	LPVOID crash_state = crash_reporter::Install(NULL);

	HRESULT hRes = ::CoInitialize(NULL);
// If you are running on NT 4.0 or higher you can use the following call instead to 
// make the EXE free threaded. This means that calls come in on a random RPC thread.
//	HRESULT hRes = ::CoInitializeEx(NULL, COINIT_MULTITHREADED);
	ATLASSERT(SUCCEEDED(hRes));

	// this resolves ATL window thunking problem when Microsoft Layer for Unicode (MSLU) is used
	::DefWindowProc(NULL, 0, 0, 0L);

	AtlInitCommonControls(ICC_COOL_CLASSES | ICC_BAR_CLASSES | ICC_DATE_CLASSES);	// add flags to support other controls

	hRes = _Module.Init(NULL, hInstance, &LIBID_ATLLib);
	ATLASSERT(SUCCEEDED(hRes));

	int nRet = Run(lpstrCmdLine, nCmdShow);

	_Module.Term();
	::CoUninitialize();

	crash_reporter::Uninstall(crash_state);
	return nRet;
}
