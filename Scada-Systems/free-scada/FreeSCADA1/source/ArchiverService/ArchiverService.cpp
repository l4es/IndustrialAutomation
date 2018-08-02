// ArchiverService.cpp : Implementation of WinMain

#include "stdafx.h"
#include "resource.h"
#include "ArchiverService.h"
#include "EventLogger.h"

#include "Engine.h"

using namespace Helpers;

class CArchiverServiceModule : public CAtlServiceModuleT< CArchiverServiceModule, IDS_SERVICENAME >
{
	typedef CAtlServiceModuleT< CArchiverServiceModule, IDS_SERVICENAME > baseClass;

public :
	DECLARE_LIBID(LIBID_ArchiverServiceLib)
	DECLARE_REGISTRY_APPID_RESOURCEID(IDR_ARCHIVERSERVICE, "{48EFF3C3-BB7E-4FE5-9E11-964C4370353B}")
	
	CArchiverServiceModule()
	{
		m_status.dwControlsAccepted = SERVICE_ACCEPT_STOP|SERVICE_ACCEPT_PAUSE_CONTINUE;
	}

	HRESULT InitializeSecurity() throw()
	{
		CoInitializeSecurity(
			NULL, //Points to security descriptor
			-1, //Count of entries in asAuthSvc
			NULL, //Array of names to register
			NULL,//Reserved for future use
			RPC_C_AUTHN_LEVEL_NONE, //The default authentication //level for proxies 
			RPC_C_IMP_LEVEL_IMPERSONATE, //The default impersonation //level for proxies
			NULL, //Reserved; must be set to NULL
			EOAC_NONE, //Additional client or //server-side capabilities
			NULL //Reserved for future use
			);

		return S_OK;
	}

	bool ParseCommandLine(LPCTSTR lpCmdLine, HRESULT* pnRetCode) throw()
	{
		*pnRetCode = S_OK;

		TCHAR szTokens[] = _T("-/");

		LPCTSTR lpszToken = FindOneOf(lpCmdLine, szTokens);
		while (lpszToken != NULL)
		{
			if (StrCmpI(lpszToken, _T("UnregServer"))==0)
			{
				*pnRetCode = UnregisterServer(TRUE);
				if (SUCCEEDED(*pnRetCode))
					*pnRetCode = UnregisterAppId();
				return false;
			}

			// Register as Local Server
			if (StrCmpI(lpszToken, _T("RegServer"))==0)
			{
				*pnRetCode = RegisterAppId();
				if (SUCCEEDED(*pnRetCode))
					*pnRetCode = RegisterServer(TRUE);
				return false;
			}

			if (StrCmpI(lpszToken, _T("Service"))==0)
			{
				*pnRetCode = RegisterAppId(true);
				if (SUCCEEDED(*pnRetCode))
					*pnRetCode = RegisterServer(TRUE);
				return false;
			}

			lpszToken = FindOneOf(lpszToken, szTokens);
		}

		return true;
	}

	HRESULT PreMessageLoop(int nShowCmd)
	{
		HRESULT hr = baseClass::PreMessageLoop(nShowCmd);
		if(SUCCEEDED(hr))
		{
			CSingletonPtr<CEngine> engine;
			engine->Start();
			return S_OK;
		}
		else
			return hr;
	}
	void OnPause()
	{
		CSingletonPtr<CEngine> engine;
		engine->Pause();
		baseClass::OnPause();
	}
	void OnContinue()
	{
		CSingletonPtr<CEngine> engine;
		engine->Continue();
		baseClass::OnContinue();
	}
	void OnStop()
	{
		CSingletonPtr<CEngine> engine;
		engine->Stop();
		baseClass::OnStop();
	}
};

CArchiverServiceModule _AtlModule;


//
extern "C" int WINAPI _tWinMain(HINSTANCE /*hInstance*/, HINSTANCE /*hPrevInstance*/, 
                                LPTSTR /*lpCmdLine*/, int nShowCmd)
{
	Helpers::CEventLoggerPtr EventLogger;

    return _AtlModule.WinMain(nShowCmd);
}

