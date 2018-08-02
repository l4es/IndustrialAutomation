// Designer.cpp : main source file for Designer.exe
//

#include "stdafx.h"

#include "resource.h"

#include "DesignerView.h"
#include "aboutdlg.h"
#include "MainFrm.h"

#include "../CrashReporter/CrashReporter.h"

CAppModule _Module;

int Run(LPTSTR /*lpstrCmdLine*/ = NULL, int nCmdShow = SW_SHOWDEFAULT)
{
	CMessageLoop theLoop;
	_Module.AddMessageLoop(&theLoop);

	CMainFrame wndMain;

	if(wndMain.CreateEx() == NULL)
	{
		ATLTRACE(_T("Main window creation failed!\n"));
		return 0;
	}

	wndMain.ShowWindow(nCmdShow);

	int nRet = theLoop.Run();

	_Module.RemoveMessageLoop();

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
	CoInitializeSecurity(	NULL, //Points to security descriptor
							-1, //Count of entries in asAuthSvc
							NULL, //Array of names to register
							NULL,//Reserved for future use
							RPC_C_AUTHN_LEVEL_NONE, //The default authentication //level for proxies 
							RPC_C_IMP_LEVEL_IMPERSONATE, //The default impersonation //level for proxies
							NULL, //Reserved; must be set to NULL
							EOAC_NONE, //Additional client or //server-side capabilities
							NULL //Reserved for future use
							);

	// this resolves ATL window thunking problem when Microsoft Layer for Unicode (MSLU) is used
	::DefWindowProc(NULL, 0, 0, 0L);

	AtlInitCommonControls(ICC_COOL_CLASSES | ICC_BAR_CLASSES);	// add flags to support other controls

	hRes = _Module.Init(NULL, hInstance);
	ATLASSERT(SUCCEEDED(hRes));

	int nRet = Run(lpstrCmdLine, nCmdShow);

	_Module.Term();
	::CoUninitialize();

	crash_reporter::Uninstall(crash_state);
	return nRet;
}
