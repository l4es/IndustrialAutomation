#pragma once

/*
	functions AtlPingRemoteHost and AtlCoCreateInstanceEx
	by: Shawn A. Van Ness
	rev: 2001.07.02
		Usage is straightforward:
		HRESULT ConnectToServer( const OLECHAR* szAddress, IFoo **ppFoo)
		{
			HRESULT hr = 0; //ok
			*ppFoo = 0;
			hr = ATLX::AtlPingRemoteHost(szAddress, 3000);
			if (FAILED(hr)) return hr;
			hr = ATLX::AtlCoCreateInstanceEx(szAddress, __uuidof(Foo), ppFoo);
			if (FAILED(hr)) return hr;
			return 0; //ok
		}
	Motivation:  DCOM takes an unreasonably long amount of time to fail an 
	activation request -- this is because it tries each available network 
	protocol (TCP, UDP, IPX, NP, etc.) in turn, until they all fail.  Timouts 
	of 3 minutes are not uncommon.  The DCOM designers say they chose robustness 
	over performance, but that is hogwash.  No user I know will wait 3 minutes 
	for a LAN connection -- 3 seconds is more like it!
*/

template <typename Q>
HRESULT AtlCoCreateInstanceEx(const TCHAR* host, const CLSID& clsid, Q** ppq, bool bUnAuthenticated=false)
{
	(*ppq) = 0;
	if (host && host[0])
	{
#if (_WIN32_WINNT >= 0x0400 ) || defined(_WIN32_DCOM) // DCOM
		COAUTHINFO cai = { RPC_C_AUTHN_WINNT, 0, 0, RPC_C_AUTHN_LEVEL_NONE, RPC_C_IMP_LEVEL_IMPERSONATE, 0, 0 };
		CComBSTR sHost = host;
		COSERVERINFO csi = { 0, sHost.m_str, 0, 0 };
		if (bUnAuthenticated)
			csi.pAuthInfo = &cai;
		
		MULTI_QI mqi = { &(__uuidof(Q)), 0, 0 };
		HRESULT hr = ::CoCreateInstanceEx(clsid,0,CLSCTX_SERVER,&csi,1,&mqi);
		if (FAILED(hr)) return hr;
		
		(*ppq) = (Q*)(mqi.pItf);
		return 0; //ok
#else
		ATLASSERT(false && "DCOM not supported by current build settings");
		return E_UNEXPECTED;
#endif
	}
	return ::CoCreateInstance(clsid,0,CLSCTX_SERVER,__uuidof(Q),(void**)(ppq));
}

#if (_WIN32_WINNT >= 0x0400 ) || defined(_WIN32_DCOM) // DCOM
static DWORD __stdcall AtlPingRemoteHostProc(void* pv)
{
	TCHAR* host = reinterpret_cast<TCHAR*>(pv);
	HRESULT hr = 0;
	// Initialize our COM apartment
//	hr = ::CoInitializeEx(0,COINIT_MULTITHREADED);
	hr = ::CoInitialize(NULL);
	if (FAILED(hr)) return hr;
	// Declare a phony CLSID (we could probably just get away w/ all zeroes, but...)
	CLSID clsidNoSuchThing = // f3839019-166b-4ec4-856c-cc68371ac7b2
							{ 0xf3839019, 0x166b, 0x4ec4, {0x85, 0x6c, 0xcc, 0x68, 0x37, 0x1a, 0xc7, 0xb2} };
	// Ping the remote host
	IUnknown* pDummy = 0;
	hr = AtlCoCreateInstanceEx(host,clsidNoSuchThing,&pDummy,true);
	// Tear down our apartment, and terminate this bkgrnd thread
	::CoUninitialize();
	return hr;
}

static HRESULT AtlPingRemoteHost(const TCHAR* host, DWORD nTimeout)
{
	HRESULT hr = 0;
	// Just a little sanity-check
	if (!host || !host[0]) return 0; //ok
	// Spawn a bkgrnd thread
    HANDLE hThread = ::CreateThread(0,0,AtlPingRemoteHostProc,(void*)host,0,0);
	// Wait a bit for it to complete, and interpret the results
	if (::WaitForSingleObject(hThread,nTimeout) == WAIT_TIMEOUT)
	{
		TerminateThread(hThread,0);
		hr = 0x800706ba; // rpc server unavailable
	}
	else
		::GetExitCodeThread(hThread,(DWORD*)&hr);
	
	// We HOPE we get "class not registered"
	if (hr == 0x80040154)
		return 0; //ok
	return hr;
}
#endif // DCOM