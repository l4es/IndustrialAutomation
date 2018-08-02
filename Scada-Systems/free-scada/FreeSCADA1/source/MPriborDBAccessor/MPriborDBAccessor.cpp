// MPriborDBAccessor.cpp : Implementation of WinMain

#include "stdafx.h"
#include "resource.h"
#include "MPriborDBAccessor.h"

class CMPriborDBAccessorModule : public CAtlServiceModuleT< CMPriborDBAccessorModule, IDS_SERVICENAME >
{
public :
	DECLARE_LIBID(LIBID_MPriborDBAccessorLib)
	DECLARE_REGISTRY_APPID_RESOURCEID(IDR_MPRIBORDBACCESSOR, "{0E95BF2F-4E23-4AA7-82E8-B4E0E70C5370}")
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

	// Parses the command line and registers/unregisters the rgs file if necessary
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
};

CMPriborDBAccessorModule _AtlModule;


//
extern "C" int WINAPI _tWinMain(HINSTANCE /*hInstance*/, HINSTANCE /*hPrevInstance*/, 
                                LPTSTR /*lpCmdLine*/, int nShowCmd)
{
    return _AtlModule.WinMain(nShowCmd);
}

