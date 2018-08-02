/*
 *                         IndigoSCADA
 *
 *   This software and documentation are Copyright 2002 to 2012 Enscada 
 *   Limited and its licensees. All rights reserved. See file:
 *
 *                     $HOME/LICENSE 
 *
 *   for full copyright notice and license terms. 
 *
 */
////////////////////////////////////////////////////////////////////////////////
// OPC Alarms & Events 1.10
////////////////////////////////////////////////////////////////////////////////
#include "opc_client_ae_app.h"
CComModule _Module;
#include <atlcom.h>
#include <atlctl.h>
#include "iec104types.h"
#include "iec_item.h"
#include "clear_crc_eight.h"
#include "opcaeclasses.h"

/////////////////////////////////////Middleware///////////////////////////////////////////
////////////////////////////////Middleware/////////////////////////////////////

DWORD Opc_client_ae_imp::g_dwNumAlarmsEvents = 0;
struct structEvent* Opc_client_ae_imp::Config_db = NULL;
////////////////////////////////Middleware/////////////////
////////////////////////////////Middleware/////////////////

int Opc_client_ae_imp::Update()
{
	IT_IT("Opc_client_ae_imp::Update");

	int rc = 0;
	int check_server = 0;

	while(true)
	{
		//check connection every g_dwUpdateRate*10 (about 10 or 30 seconds)
		if((check_server%10) == 0)
		{
			rc = check_connection_to_server();
			fprintf(stderr,"check for server connection...\n");
			fflush(stderr);
		}

		check_server++;

		if(rc)
		{ 
			fprintf(stderr,"Opc_client_ae_imp exiting...., due to lack of connection with server\n");
			fflush(stderr);
			IT_COMMENT("Opc_client_ae_imp exiting...., due to lack of connection with server");
			break; 
		}

		if(fExit)
		{
			IT_COMMENT("Opc_client_ae_imp exiting....");
			break; //terminate the thread
		}

		::Sleep(g_dwUpdateRate);
	}

	IT_EXIT;
	return 0;
}

#include "iec104types.h"
#include <process.h>

void WatchDogForBlockedProcesses(void* pParam)
{
	int k;
	time_t actual_epoch = 0;
	cp56time2a iec_actual_epoch;
	time_t delta;

	for(k = 0;; k++)
	{
		Sleep(100);

		if(((Opc_client_ae_imp*)pParam)->state_of_watch_dog_blocked_processes == TRYING_TO_CONNECT_TO_SERVER)
		{
			((Opc_client_ae_imp*)pParam)->get_utc_host_time(&iec_actual_epoch);

			actual_epoch = ((Opc_client_ae_imp*)pParam)->epoch_from_cp56time2a(&iec_actual_epoch);

			//fprintf(stderr,"actual_epoch = %lu\n", actual_epoch);
			//fflush(stderr);
			
			delta = actual_epoch - ((Opc_client_ae_imp*)pParam)->timer_starts_at_epoch;

			//fprintf(stderr,"delta = %lu\n", delta);
			//fflush(stderr);
		
			long timeout_watchdog = 75; //seconds

			if(delta > timeout_watchdog)
			{
				fprintf(stderr,"Timeout of timer waiting for connection with server");
				fflush(stderr);

				//We have lost connection with server so
				//exit opc_client_ae.exe process, hopefully we get a new connection with opc server
				ExitProcess(0);
			}
		}
		else
		{
			break;
		}
	}

	fprintf(stderr,"Exiting WatchDogForBlockedProcesses thread\n");
	fflush(stderr);
}

int Opc_client_ae_imp::OpcStart(char* OpcServerProgID, char* OpcclassId, char* OpcUpdateRate)
{
	IT_IT("Opc_client_ae_imp::OpcStart");

	int rc = 0;
	char show_msg[500];
	sprintf(show_msg, " IndigoSCADA OPC A&E Client Start\n");
	LogMessage(S_OK, show_msg);

	if(strlen(OpcUpdateRate) > 0)
	{
		g_dwUpdateRate = atoi(OpcUpdateRate); 
	}

	if((strlen(ServerIPAddress) == 0))
	{
		strcpy(ServerIPAddress, "127.0.0.1");
	}

	start_timer_for_blocked_processes();

	//////////////////////////////////////////////////////////////////////////
	//start a worker thread to check for blocked programs (and stop if necessary)
	if(_beginthread(WatchDogForBlockedProcesses, 0, this) == -1)
	{
		long nError = GetLastError();
		char pTemp[121];
		sprintf(pTemp, "_beginthread failed, error code = %d", nError);
		LogMessage(S_FALSE, pTemp);
	}
	//////////////////////////////////////////////////////////////////////////
		
	//DCOM connection (and local connection)

	fprintf(stderr,"Trying to connect to remote (or local) A&E server on machine with IP: %s\n", ServerIPAddress);
	fflush(stderr);
	
	HRESULT	hr = ::CoInitializeEx(NULL,COINIT_MULTITHREADED); // setup COM lib

	if(FAILED(hr))
	{
		//fprintf(stderr,"CoInitializeEx failed\n");
		//fflush(stderr);
		LogMessage(hr,"CoInitializeEx failed");
		IT_EXIT;
		return 1;
	}

	CoInitializeSecurity(NULL, -1, NULL, NULL, RPC_C_AUTHN_LEVEL_NONE, RPC_C_IMP_LEVEL_IMPERSONATE, NULL, EOAC_NONE, NULL);
	
	COAUTHINFO athn;
	ZeroMemory(&athn, sizeof(COAUTHINFO));
	// Set up the NULL security information
	athn.dwAuthnLevel = RPC_C_AUTHN_LEVEL_CONNECT;
	//athn.dwAuthnLevel = RPC_C_AUTHN_LEVEL_NONE;
	athn.dwAuthnSvc = RPC_C_AUTHN_WINNT;
	athn.dwAuthzSvc = RPC_C_AUTHZ_NONE;
	athn.dwCapabilities = EOAC_NONE;
	athn.dwImpersonationLevel = RPC_C_IMP_LEVEL_IMPERSONATE;
	athn.pAuthIdentityData = NULL;
	athn.pwszServerPrincName = NULL;

	USES_CONVERSION;

	COSERVERINFO remoteServerInfo;
	ZeroMemory(&remoteServerInfo, sizeof(COSERVERINFO));
	remoteServerInfo.pAuthInfo = &athn;
	remoteServerInfo.pwszName = T2OLE(ServerIPAddress);
	//fprintf(stderr,"Remote host: %s\n", OLE2T(remoteServerInfo.pwszName));
	//fflush(stderr);

	MULTI_QI reqInterface;
	reqInterface.pIID = &IID_IOPCServerList; //requested interface
	reqInterface.pItf = NULL;
	reqInterface.hr = S_OK;
							//requested class
	hr = CoCreateInstanceEx(CLSID_OpcServerList,NULL, CLSCTX_REMOTE_SERVER, &remoteServerInfo, 1, &reqInterface);
	
	if(FAILED(hr))
	{
		fprintf(stderr,"OPC error:Failed to get remote interface, %x\n", hr);
		fflush(stderr);
		LogMessage(hr,"Failed to get remote interface");
		IT_EXIT;
		return 1;
	}
	
	g_iCatInfo = (IOPCServerList *)reqInterface.pItf;

	CATID Implist[1];

	Implist[0] = __uuidof(OPCEventServerCATID);

	IEnumCLSID *iEnum = NULL;

	if(g_iCatInfo == NULL)
	{
		IT_EXIT;
		return 1;
	}

	hr = g_iCatInfo->EnumClassesOfCategories(1, Implist,0, NULL,&iEnum);

	if(FAILED(hr))
	{
		fprintf(stderr,"OPC error:Failed to get enum for categeories, %x\n", hr);
		fflush(stderr);
		LogMessage(hr,"Failed to get enum for categeories");
		IT_EXIT;
		return 1;
	}

	GUID glist;

	ULONG actual;

	fprintf(stderr,"Available A&E server(s) on remote machine:\n");
	fflush(stderr);

	int kk;
	for(kk = 0; (hr = iEnum->Next(1, &glist, &actual)) == S_OK; kk++)
	{
		WCHAR *progID;
		WCHAR *userType;
		HRESULT res = g_iCatInfo->GetClassDetails(glist, &progID, &userType);
		//ProgIDFromCLSID(glist, &progID);

		if(FAILED(res))
		{
			fprintf(stderr,"OPC error:Failed to get ProgId from ClassId, %x\n",res);
			fflush(stderr);
			LogMessage(res,"Failed to get ProgId from ClassId");
			IT_EXIT;
			return 1;
		}
		else 
		{
			USES_CONVERSION;
			char * str = OLE2T(progID);
			char * str1 = OLE2T(userType);
			fprintf(stderr,"AE - %s\n", str);
			fflush(stderr);
			::CoTaskMemFree(progID);
			::CoTaskMemFree(userType);
		}
	}

	if(kk == 0)
	{
		fprintf(stderr,"OPC error:No OPC A&E server on machine with IP: %s\n", ServerIPAddress);
		fflush(stderr);
		IT_EXIT;
		return 1;
	}
				
	if((strlen(OpcServerProgID) == 0))
	{
		//fprintf(stderr,"OPC error: Please supply ProgID\n");
		//fflush(stderr);
		LogMessage(S_FALSE,"Please supply ProgID");
		IT_EXIT;
		return 1;
	}

	strcpy(opc_server_prog_id, OpcServerProgID);
	
	//Get CLSID From RemoteRegistry
	char keyName[100];
	
	strcpy(keyName,"SOFTWARE\\Classes\\");
	strcat(keyName, OpcServerProgID);
	strcat(keyName, "\\Clsid");

	HKEY remoteRegHandle;
	HKEY keyHandle;
	CLSID classId;

	hr = RegConnectRegistry(ServerIPAddress, HKEY_LOCAL_MACHINE, &remoteRegHandle);

	if(hr != S_OK)
	{
		LPVOID lpMsgBuf;

		FormatMessage( 
		FORMAT_MESSAGE_ALLOCATE_BUFFER | 
		FORMAT_MESSAGE_FROM_SYSTEM | 
		FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,
		GetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
		(LPTSTR) &lpMsgBuf,
		0,
		NULL 
		);

		sprintf(show_msg, "RegConnectRegistry failed, with message: %s", lpMsgBuf);

		LocalFree(lpMsgBuf);

		//If this process is started by a service which runs as Local Account, then you need to set
		//the remote classId string (CLSID)

		USES_CONVERSION;

		LPOLESTR sz = A2W(OpcclassId);

		hr = CLSIDFromString(sz,&classId);

		if(FAILED(hr))
		{
			//fprintf(stderr,"CLSIDFromString failed\n");
			//fflush(stderr);
			LogMessage(hr,"CLSIDFromString failed");
			return 1;
		}
	}
	else
	{
	   hr = RegOpenKeyEx(remoteRegHandle, keyName, 0, KEY_READ, &keyHandle);

	   if(SUCCEEDED(hr))
	   {
		   DWORD entryType;

		   unsigned bufferSize = 100;

		   hr = RegQueryValueEx(keyHandle, NULL, 0, &entryType, (LPBYTE)&OpcclassId, (LPDWORD)&bufferSize);

		   if(FAILED(hr))
		   {
				//fprintf(stderr,"RegQueryValueEx failed\n");
				//fflush(stderr);
				LogMessage(hr,"RegQueryValueEx failed");
				IT_EXIT;
				return 1;
		   }
		   else
		   {
				USES_CONVERSION;

				LPOLESTR sz = A2W(OpcclassId);

				hr = CLSIDFromString(sz,&classId);

				if(FAILED(hr))
				{
					//fprintf(stderr,"CLSIDFromString failed\n");
					//fflush(stderr);
					LogMessage(hr,"CLSIDFromString failed");
					IT_EXIT;
					return 1;
				}
		   }
	   }
	   else
	   {
			LogMessage(hr,"RegOpenKeyEx failed");
	   }

	   RegCloseKey(keyHandle);
	   RegCloseKey(remoteRegHandle);
	}	
    
	////////////////////end Get CLSID From Remote Registry

	ZeroMemory(&athn, sizeof(COAUTHINFO));
	// Set up the NULL security information
	athn.dwAuthnLevel = RPC_C_AUTHN_LEVEL_CONNECT;
	//athn.dwAuthnLevel = RPC_C_AUTHN_LEVEL_NONE;
	athn.dwAuthnSvc = RPC_C_AUTHN_WINNT;
	athn.dwAuthzSvc = RPC_C_AUTHZ_NONE;
	athn.dwCapabilities = EOAC_NONE;
	athn.dwImpersonationLevel = RPC_C_IMP_LEVEL_IMPERSONATE;
	athn.pAuthIdentityData = NULL;
	athn.pwszServerPrincName = NULL;
	
	ZeroMemory(&remoteServerInfo, sizeof(COSERVERINFO));
	remoteServerInfo.pAuthInfo = &athn;
	
	remoteServerInfo.pwszName = T2OLE(ServerIPAddress);

	//fprintf(stderr,"%s\n", OLE2T(remoteServerInfo.pwszName));
	//fflush(stderr);
	
	reqInterface.pIID = &IID_IUnknown; //requested interface
	reqInterface.pItf = NULL;
	reqInterface.hr = S_OK;

	fprintf(stderr,"Before CoCreateInstanceEx\n");
	fflush(stderr);
		                      //requsted class
	hr = CoCreateInstanceEx(classId,NULL, CLSCTX_REMOTE_SERVER, &remoteServerInfo, 1, &reqInterface);	

	fprintf(stderr,"After CoCreateInstanceEx\n");
	fflush(stderr);

	state_of_watch_dog_blocked_processes = CONNECTION_ENSTABLISHED;
	
	if (FAILED(hr))
	{
		fprintf(stderr,"OPC error:Failed to get remote interface, %x\n", hr);
		fflush(stderr);
		LogMessage(hr,"Failed to get remote interface");
		IT_EXIT;
		return 1;
	}
	
	IUnknown * pIUnknown = NULL;

	pIUnknown = reqInterface.pItf;

	/////end make Remote Object

	hr = pIUnknown->QueryInterface(IID_IOPCEventServer, (void**)&g_pIOPCServer);

	if (FAILED(hr))
	{
		//fprintf(stderr,"OPC error:Failed to obtain IID_IOPCEventServer interface from server, %x\n", hr);
		//fflush(stderr);
		LogMessage(hr,"Failed to obtain IID_IOPCEventServer interface from server");
		IT_EXIT;
		return 1;
	}

	fprintf(stderr,"Connected to A&E server on machine with IP: %s\n", ServerIPAddress);
	fflush(stderr);

	//load each record of .sql in the struct structEvent, before we get callbacks
	//we need to initialize Config_db;

	rc = load_database();

	if(rc)
	{
		fprintf(stderr,"Failed to load .sql database\n");
		fflush(stderr);
		IT_EXIT;
		return 1;
	}

	WORD wMajor, wMinor, wBuild;
	LPWSTR pwsz = NULL;

	if(!GetStatus(&wMajor, &wMinor, &wBuild, &pwsz))
	{
		//char ver[300];
		fprintf(stderr,"Version: %d.%d.%d\n", wMajor, wMinor, wBuild);
		fflush(stderr);
		fprintf(stderr,"%ls\n\n",pwsz);
		fflush(stderr);
		//sprintf(ver,"Server version: %d.%d.%d, %s", wMajor, wMinor, wBuild, W2T(pwsz));
		//fprintf(stderr,"%s\n\n",ver);

		IT_COMMENT4("Version: %d.%d.%d, %s", wMajor, wMinor, wBuild,W2T(pwsz));
		//LogMessage(S_OK, "",ver);
		::CoTaskMemFree(pwsz);
	}

	hr = g_pIOPCServer->QueryInterface(IID_IOPCCommon, (void**)&g_pIOPCCommon);

	if(FAILED(hr))
	{
		LogMessage(hr,"QueryInterface(IID_IOPCCommon)");
	}
	else
	{
		g_pIOPCCommon->SetClientName(L"IndigoSCADA OPC AE Client");
	}

	BOOL bActive;
	DWORD dwBufferTime;
	DWORD dwMaxSize;
	DWORD hClientSubscription;
	DWORD dwRevisedBufferTime;
	DWORD dwRevisedMaxSize;

	CComCOPCEventSink   *m_pSink = NULL;
	CComCOPCShutdownRequest  *m_pShutdown = NULL;

	dwMaxSize = 1000; //The server can send upto 1000 events for each OnEvent call

	bActive = 0; // NOT active
	
	if(!dwMaxSize)
	{
		dwMaxSize=1;
	}

	srand((unsigned)time(NULL));

	hClientSubscription = rand()*50;

	dwBufferTime = 100; //depend on the OPC AE server

	hr = g_pIOPCServer->CreateEventSubscription(bActive,
						dwBufferTime,
						dwMaxSize,
						hClientSubscription,
						IID_IOPCEventSubscriptionMgt,
					   (IUnknown **)&m_ISubMgt,
					   &dwRevisedBufferTime,
					   &dwRevisedMaxSize);
    /*
	hr = g_pIOPCServer->CreateEventSubscription(bActive,
						dwBufferTime,
						dwMaxSize,
						hClientSubscription,
						IID_IOPCEventSubscriptionMgt2,
					   (IUnknown **)&m_ISubMgt2,
					   &dwRevisedBufferTime,
					   &dwRevisedMaxSize);
	*/


	if(hr != S_OK)
	{
		LPVOID lpMsgBuf;

		FormatMessage( 
		FORMAT_MESSAGE_ALLOCATE_BUFFER | 
		FORMAT_MESSAGE_FROM_SYSTEM | 
		FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,
		GetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
		(LPTSTR) &lpMsgBuf,
		0,
		NULL 
		);

		sprintf(show_msg, "CreateEventSubscription failed, with message: %s", lpMsgBuf);

		LocalFree(lpMsgBuf);
		fprintf(stderr,"%s\n", show_msg);
		fflush(stderr);

		fprintf(stderr,"Failed to Create Subscription\n");
		fflush(stderr);
		IT_EXIT;
		return 1;
	}

	fprintf(stderr,"A&E server dwRevisedBufferTime = %d, dwRevisedMaxSize = %d\n", dwRevisedBufferTime, dwRevisedMaxSize);
	fflush(stderr);

	if(m_ISubMgt == NULL)
	{
		fprintf(stderr,"CreateEventSubscription returned m_ISubMgt NULL\n");
		fflush(stderr);
		IT_EXIT;
		return 1;
	}

	fprintf(stderr,"CreateEventSubscription Done\n");
	fflush(stderr);
	
	// create advise
	CComObject<COPCEventSink>::CreateInstance(&m_pSink);
	m_dwCookie = 0xCDCDCDCD;

	IUnknown* pUnk;

	hr = m_pSink->_InternalQueryInterface( __uuidof(IUnknown), (void**)&pUnk );

	if(hr != S_OK)
	{
		fprintf(stderr,"Failed m_pSink->_InternalQueryInterface\n");
		fflush(stderr);
		IT_EXIT;
		return 1;
	}

	hr = AtlAdvise(m_ISubMgt, pUnk, __uuidof(IOPCEventSink), &m_dwCookie );

	if(hr != S_OK)
	{
		fprintf(stderr,"Failed AtlAdvise m_dwCookie\n");
		fflush(stderr);
		IT_EXIT;
		return 1;
	}

	//shutdown advise 
	///////////////////////////////////////////////Shutdown/////////////////////////////

//does not work...
/*
	CComObject<COPCShutdownRequest>::CreateInstance(&m_pShutdown);
	m_dwShutdownCookie = 0xCDCDCDCD;

	//IUnknown* pUnk;

	hr = m_pShutdown->_InternalQueryInterface( __uuidof(IUnknown), (void**)&pUnk );

	if(hr != S_OK)
	{
		fprintf(stderr,"Failed m_pShutdown->_InternalQueryInterface\n");
		fflush(stderr);
		IT_EXIT;
		return 1;
	}

	//hr = AtlAdvise(m_ISubMgt, m_pShutdown->GetUnknown(),__uuidof(IOPCShutdown), &m_dwShutdownCookie);

	hr = AtlAdvise(m_ISubMgt, pUnk, __uuidof(IOPCShutdown), &m_dwShutdownCookie);
	
	if(hr != S_OK)
	{
		fprintf(stderr,"Failed shutdown advise\n");
		fflush(stderr);
		IT_EXIT;
		return 1;
	}
*/
	////////////////////////GetState and SetState/////////////////////////////////////////////////////////////

	hr = m_ISubMgt->GetState(&bActive,&dwBufferTime,&dwMaxSize,&hClientSubscription);

	if(hr != S_OK)
	{
		fprintf(stderr,"Failed m_ISubMgt->GetState\n");
		fflush(stderr);
		IT_EXIT;
		return 1;
	}

	fprintf(stderr,"Server state: bActive = %d, dwBufferTime = %d, dwMaxSize = %d, hClientSubscription = %d\n", bActive, dwBufferTime, dwMaxSize, hClientSubscription);
	fflush(stderr);

	bActive = 1;

	hr = m_ISubMgt->SetState(&bActive, &dwBufferTime, &dwMaxSize, hClientSubscription, &dwRevisedBufferTime, &dwRevisedMaxSize);

	if(hr != S_OK)
	{
		fprintf(stderr,"Failed m_ISubMgt->SetState\n");
		fflush(stderr);
		IT_EXIT;
		return 1;
	}

	///////////////////////Refresh//////////////////////////////////////////////////////////////
#ifdef REFRESH_AT_STARTUP
	//Conditioned out because some servers send a refresh at activation of subscription,
	//so we do not Refresh at startup
	hr = m_ISubMgt->Refresh(m_dwCookie);

	if(hr != S_OK)
	{
		fprintf(stderr,"Failed Refresh\n");
		fflush(stderr);
		IT_EXIT;
		return 1;
	}
#endif

	///////////////////////SetKeepAlive//////////////////////////////////////////////////////////////

	if(m_ISubMgt2 != NULL)
	{
		DWORD dwRevisedKeepAliveTime = 0;
		// set the keep-alive to 3X the dwRevisedBufferTime
		hr = m_ISubMgt2->SetKeepAlive(3 * dwRevisedBufferTime, &dwRevisedKeepAliveTime);

		if(hr == E_FAIL || hr == E_INVALIDARG)
		{
			fprintf(stderr,"Failed SetKeepAlive\n");
			fflush(stderr);
			IT_EXIT;
			return 1;
		}

		fprintf(stderr,"dwRevisedKeepAliveTime = %d\n", dwRevisedKeepAliveTime);
		fflush(stderr);
	}
	//////////////////////////////////////////////////////////////////////////////////////////////////

	IT_EXIT;
    return(0);
}

int Opc_client_ae_imp::OpcStop()
{
	IT_IT("Opc_client_ae_imp::OpcStop");

	HRESULT hr;

	if(m_ISubMgt != NULL)
	{
		if(m_dwCookie != 0xCDCDCDCD)
		{
			hr = AtlUnadvise(m_ISubMgt, __uuidof(IOPCEventSink), m_dwCookie);
			m_dwCookie = 0xCDCDCDCD;
		}

		if(m_dwShutdownCookie != 0xCDCDCDCD)
		{
			hr = AtlUnadvise(m_ISubMgt, __uuidof(IOPCShutdown), m_dwShutdownCookie);
			m_dwShutdownCookie = 0xCDCDCDCD;
		}
	}

	// terminate server and it will clean up itself
	if(g_pIOPCServer) while(g_pIOPCServer->Release()) ;

	::CoUninitialize();

	char show_msg[200];
	sprintf(show_msg, "IndigoSCADA OPC A&E Client End\n");
	LogMessage(NULL, show_msg);

	//fprintf(stderr,"Server and all group interfaces terminated.\n");
	//fflush(stderr);

	LogMessage(S_OK,"","Server and all group interfaces terminated");
	IT_EXIT;
	return 1;
}

int Opc_client_ae_imp::check_connection_to_server(void)
{
	IT_IT("Opc_client_ae_imp::check_connection_to_server");

	if(m_ISubMgt != NULL)
	{
		HRESULT hr;
		BOOL bActive;
		DWORD dwBufferTime;
		DWORD dwMaxSize;
		DWORD hClientSubscription;

		hr = m_ISubMgt->GetState(&bActive,&dwBufferTime,&dwMaxSize,&hClientSubscription);

		//fprintf(stderr,"hr = 0x%x\n", hr);
		//fflush(stderr);
		IT_COMMENT1("hr = 0x%x\n", hr);

		if(hr != S_OK)
		{
			IT_EXIT;
			return 1;
		}
	}

	return 0;
}

int Opc_client_ae_imp::GetStatus(WORD *pwMav, WORD *pwMiv, WORD *pwB, LPWSTR *pszV)
{
	IT_IT("Opc_client_ae_imp::GetStatus");

	*pwMav = 0;
	*pwMiv = 0;
	*pwB = 0;
	*pszV = NULL;
	OPCEVENTSERVERSTATUS *pStatus = NULL;

	if(g_pIOPCServer == NULL)
	{
	  IT_EXIT;
	  return E_POINTER;
	}

	HRESULT hr = g_pIOPCServer->GetStatus(&pStatus);

	if(FAILED(hr) || (pStatus == NULL) )
	{
		if(FAILED(hr))	LogMessage(hr,"GetStatus()");
		if(pStatus != NULL) ::CoTaskMemFree(pStatus);

		IT_EXIT;
		return E_FAIL;
	}

	*pwMav = pStatus->wMajorVersion;
	*pwMiv = pStatus->wMinorVersion;
	*pwB = pStatus->wBuildNumber;
	*pszV = pStatus->szVendorInfo;
	::CoTaskMemFree(pStatus);

	IT_EXIT;
	return 0;
}

void Opc_client_ae_imp::LogMessage(HRESULT hr, LPCSTR pszError, const char* name)
{
	//TODO: send message to monitor.exe
}

void Opc_client_ae_imp::epoch_to_cp56time2a(cp56time2a *time, signed __int64 epoch_in_millisec)
{
	struct tm	*ptm;
	int ms = (int)(epoch_in_millisec%1000);
	time_t seconds;

	IT_IT("epoch_to_cp56time2a");
	
	memset(time, 0x00,sizeof(cp56time2a));
	seconds = (long)(epoch_in_millisec/1000);
	ptm = localtime(&seconds);
		
	time->hour = ptm->tm_hour;					//<0..23>
	time->min = ptm->tm_min;					//<0..59>
	time->msec = ptm->tm_sec*1000 + ms; //<0..59999>
	time->mday = ptm->tm_mday; //<1..31>
	time->wday = (ptm->tm_wday == 0) ? ptm->tm_wday + 7 : ptm->tm_wday; //<1..7>
	time->month = ptm->tm_mon + 1; //<1..12>
	time->year = (ptm->tm_year + 1900)%100; //<0..99>
	time->iv = 0; //<0..1> Invalid: <0> is valid, <1> is invalid
	time->su = (u_char)ptm->tm_isdst; //<0..1> SUmmer time: <0> is standard time, <1> is summer time

	IT_EXIT;
    return;
}

enum event_state
{
	UNKNOWN = 0,
	ACTIVE,
	INACTIVE
};

static u_int n_msg_sent = 0;

void Opc_client_ae_imp::SendEvent2(ONEVENTSTRUCT* pEvent)
{
	IT_IT("Opc_client_ae_imp::SendEvent2");

	FILETIME* ftTime;
	FILETIME* ftActiveTime;
		
	cp56time2a time;
	cp56time2a active_time;
	
	signed __int64 epoch_in_millisec;
		
	const char* parc;

	USES_CONVERSION;

	IT_COMMENT1("pwQualities = %d", pEvent->wQuality);
	
	ftTime = &(pEvent->ftTime);
	epoch_in_millisec = epoch_from_FILETIME(ftTime);
	epoch_to_cp56time2a(&time, epoch_in_millisec);

	ftActiveTime = &(pEvent->ftActiveTime);
	epoch_in_millisec = epoch_from_FILETIME(ftActiveTime);
	epoch_to_cp56time2a(&active_time, epoch_in_millisec);

	fprintf(stderr,"%d, %d, %ls, h:%i m:%i s:%i ms:%i %02i-%02i-%02i iv %i su %i,\
 %ls, %d, %d, %d, %ls, %ls, %d, %d, %d, h:%i m:%i s:%i ms:%i %02i-%02i-%02i iv %i su %i, %d, %d, %ls.\n", 

	pEvent->wChangeMask,
	pEvent->wNewState,
	pEvent->szSource,
	//FILETIME pEvent->ftTime,
	time.hour,
	time.min,
	time.msec/1000,
	time.msec%1000,
	time.mday,
	time.month,
	time.year,
	time.iv,
	time.su,
	pEvent->szMessage,
	pEvent->dwEventType,
	pEvent->dwEventCategory,
	pEvent->dwSeverity,
	pEvent->szConditionName,
	pEvent->szSubconditionName,
	pEvent->wQuality,
	pEvent->wReserved,
	pEvent->bAckRequired,
	//FILETIME pEvent->ftActiveTime,
	active_time.hour,
	active_time.min,
	active_time.msec/1000,
	active_time.msec%1000,
	active_time.mday,
	active_time.month,
	active_time.year,
	active_time.iv,
	active_time.su,
	pEvent->dwCookie,
	pEvent->dwNumEventAttrs,
	//[size_is] VARIANT *pEventAttributes,
	pEvent->szActorID);

	fflush(stderr);

	if(pEvent->dwNumEventAttrs == 0)
	{
		char sz[150];
		sprintf(sz,"pEvent->dwNumEventAttrs is 0 for %ls\n", pEvent->szConditionName);
		LogMessage(E_FAIL, sz);

		char show_msg[500];

		sprintf(show_msg, "%d, %d, %ls, h:%i m:%i s:%i ms:%i %02i-%02i-%02i iv %i su %i,\
%ls, %d, %d, %d, %ls, %ls, %d, %d, %d, h:%i m:%i s:%i ms:%i %02i-%02i-%02i iv %i su %i, %d, %d, %ls", 
		pEvent->wChangeMask,
		pEvent->wNewState,
		pEvent->szSource,
		//FILETIME pEvent->ftTime,
		time.hour,
		time.min,
		time.msec/1000,
		time.msec%1000,
		time.mday,
		time.month,
		time.year,
		time.iv,
		time.su,
		pEvent->szMessage,
		pEvent->dwEventType,
		pEvent->dwEventCategory,
		pEvent->dwSeverity,
		pEvent->szConditionName,
		pEvent->szSubconditionName,
		pEvent->wQuality,
		pEvent->wReserved,
		pEvent->bAckRequired,
		//FILETIME pEvent->ftActiveTime,
		active_time.hour,
		active_time.min,
		active_time.msec/1000,
		active_time.msec%1000,
		active_time.mday,
		active_time.month,
		active_time.year,
		active_time.iv,
		active_time.su,
		pEvent->dwCookie,
		pEvent->dwNumEventAttrs,
		//[size_is] VARIANT *pEventAttributes,
		pEvent->szActorID);

		LogMessage(S_OK, show_msg);
	}

	struct iec_item item_to_send;
	
	memset(&item_to_send,0x00, sizeof(struct iec_item));
	
	item_to_send.cause = 0x03; //Spontaneous <3> decimal

	epoch_in_millisec = epoch_from_FILETIME(ftTime);
		
	parc = ((const char*)W2T(pEvent->szSubconditionName));

	unsigned int alarm_list_number = atoi(parc);

	int value_to_send_as_parameter = UNKNOWN;

	/*
	TODO: set 

	  value_to_send_as_parameter = INACTIVE;

	  or
	
	  value_to_send_as_parameter = ACTIVE;

	  based on the valued of pEvent->wChangeMask and pEvent->wNewState

	  See OPC AE event state machine
	*/
		
	VARIANT *pValue = NULL;
	VARIANT Value;

	pValue = &Value;

	V_VT(pValue) = VT_BOOL; //if we use this opc type => IEC 104 message is M_SP_TB_1

	//Each event or alarm is sent as M_SP_TB_1
	//We send the ACTIVE or INACTIVE state of the alarm
	//So the states ACTIVE maps to single point value 1, and INACTIVE maps 0

	if(value_to_send_as_parameter == ACTIVE)
	{
		V_BOOL(pValue) = -1;
	}
	else if(value_to_send_as_parameter == INACTIVE)
	{
		V_BOOL(pValue) = 0;
	}
	else
	{
		char sz[150];
		sprintf(sz,"Event identified by number = %d has UNKNOWN state\n", alarm_list_number);
		LogMessage(E_FAIL, sz);
		return;
	}
	
	//V_VT(pValue) = VT_I2; //if we use this opc type => IEC 104 message is M_ME_TE_1
	//V_I2(pValue) = value_to_send_as_parameter;

	int found = 0;

	for(DWORD dw = 0; dw < g_dwNumAlarmsEvents; dw++)
	{ 
		if(Config_db[dw].Number == alarm_list_number)
		{
			found = 1;
			item_to_send.iec_obj.ioa = Config_db[dw].ioa_control_center;
			break;
		}
	}
		
	if(found == 0)
	{
		char sz[150];
		sprintf(sz,"Event with Number = %d is NOT found in database built from .sql file\n", alarm_list_number);
		LogMessage(E_FAIL, sz);
		return;
	}
	
	if(item_to_send.iec_obj.ioa == 0)
	{
		char sz[300];
		sprintf(sz,"Something went wrong in configuration, please check .sql file\n");
		LogMessage(E_FAIL, sz);
		return;
	}
		
	fprintf(stderr,"Sent to monitor with IOA = %d\n", item_to_send.iec_obj.ioa);
	fflush(stderr);
	//fprintf(stderr,"Sent to monitor with value = %d\n", value_to_send_as_parameter);
	//fflush(stderr);
	fprintf(stderr,"\n");
	fflush(stderr);

	DWORD pwQualities = pEvent->wQuality;

	if(!pValue)
	{
		VARIANT Value;
		pValue = &Value;
		V_VT(pValue) = VT_EMPTY;
	}
	
	switch(V_VT(pValue))
	{
		case VT_EMPTY:
		{
			//IT_COMMENT1("Value = %d", V_EMPTY(pValue));
		}
		break;
		case VT_I1:
		{
			item_to_send.iec_type = M_ME_TE_1;
			epoch_to_cp56time2a(&time, epoch_in_millisec);
			item_to_send.iec_obj.o.type35.mv = V_I1(pValue);
			item_to_send.iec_obj.o.type35.time = time;
			
			
			if(pwQualities != OPC_QUALITY_GOOD)
				item_to_send.iec_obj.o.type35.iv = 1;

			IT_COMMENT1("Value = %d", V_I1(pValue));
		}
		break;
		case VT_UI1:
		{
			#ifdef EXTENDEND_IEC_TYPES
			item_to_send.iec_type = M_ME_TR_1;
			epoch_to_cp56time2a(&time, epoch_in_millisec);
			item_to_send.iec_obj.o.type153.mv = V_UI1(pValue);
			item_to_send.iec_obj.o.type153.time = time;

			if(pwQualities != OPC_QUALITY_GOOD)
				item_to_send.iec_obj.o.type153.iv = 1;
			#else
			item_to_send.iec_type = M_ME_TE_1;
			epoch_to_cp56time2a(&time, epoch_in_millisec);
			item_to_send.iec_obj.o.type35.mv = V_UI1(pValue);
			item_to_send.iec_obj.o.type35.time = time;
			
			
			if(pwQualities != OPC_QUALITY_GOOD)
				item_to_send.iec_obj.o.type35.iv = 1;
			#endif
											
			IT_COMMENT1("Value = %d", V_UI1(pValue));
		}
		break;
		case VT_I2:
		{
			item_to_send.iec_type = M_ME_TE_1;
			epoch_to_cp56time2a(&time, epoch_in_millisec);
			item_to_send.iec_obj.o.type35.mv = V_I2(pValue);
			item_to_send.iec_obj.o.type35.time = time;
			

			if(pwQualities != OPC_QUALITY_GOOD)
				item_to_send.iec_obj.o.type35.iv = 1;

			IT_COMMENT1("Value = %d", V_I2(pValue));
		}
		break;
		case VT_UI2:
		{
			#ifdef EXTENDEND_IEC_TYPES
			item_to_send.iec_type = M_ME_TR_1;
			epoch_to_cp56time2a(&time, epoch_in_millisec);
			item_to_send.iec_obj.o.type153.mv = V_UI2(pValue);
			item_to_send.iec_obj.o.type153.time = time;

			if(pwQualities != OPC_QUALITY_GOOD)
				item_to_send.iec_obj.o.type153.iv = 1;
			#else
			item_to_send.iec_type = M_ME_TE_1;
			epoch_to_cp56time2a(&time, epoch_in_millisec);
			item_to_send.iec_obj.o.type35.mv = V_UI2(pValue);
			item_to_send.iec_obj.o.type35.time = time;
			

			if(pwQualities != OPC_QUALITY_GOOD)
				item_to_send.iec_obj.o.type35.iv = 1;
			#endif

			IT_COMMENT1("Value = %d", V_UI2(pValue));
		}
		break;
		case VT_I4:
		{
			#ifdef EXTENDEND_IEC_TYPES
			item_to_send.iec_type = M_ME_TP_1;
			epoch_to_cp56time2a(&time, epoch_in_millisec);
			item_to_send.iec_obj.o.type152.mv = V_I4(pValue);
			item_to_send.iec_obj.o.type152.time = time;

			if(pwQualities != OPC_QUALITY_GOOD)
				item_to_send.iec_obj.o.type152.iv = 1;
			#else
			item_to_send.iec_type = M_ME_TE_1;
			epoch_to_cp56time2a(&time, epoch_in_millisec);
			item_to_send.iec_obj.o.type35.mv = (short)V_I4(pValue);
			item_to_send.iec_obj.o.type35.time = time;
			

			if(pwQualities != OPC_QUALITY_GOOD)
				item_to_send.iec_obj.o.type35.iv = 1;
			#endif

			IT_COMMENT1("Value = %d", V_I4(pValue));
		}
		break;
		case VT_UI4:
		{
			#ifdef EXTENDEND_IEC_TYPES
			item_to_send.iec_type = M_ME_TM_1;
			epoch_to_cp56time2a(&time, epoch_in_millisec);
			item_to_send.iec_obj.o.type151.mv = V_UI4(pValue);
			item_to_send.iec_obj.o.type151.time = time;

			if(pwQualities != OPC_QUALITY_GOOD)
				item_to_send.iec_obj.o.type151.iv = 1;
			#else
			item_to_send.iec_type = M_ME_TE_1;
			epoch_to_cp56time2a(&time, epoch_in_millisec);
			item_to_send.iec_obj.o.type35.mv = (short)V_UI4(pValue);
			item_to_send.iec_obj.o.type35.time = time;
			

			if(pwQualities != OPC_QUALITY_GOOD)
				item_to_send.iec_obj.o.type35.iv = 1;
			#endif

			IT_COMMENT1("Value = %d", V_UI4(pValue));
		}
		break;
		/*
		case VT_I8:
		{
			fprintf(stderr,"Not supported with CV++ 6.0");
			fflush(stderr);
			IT_COMMENT("Not supported with CV++ 6.0");
			
			//IT_COMMENT1("Value = %ld", pValue->llVal);
		}
		break;
		case VT_UI8:
		{
			fprintf(stderr,"Not supported with CV++ 6.0");
			fflush(stderr);
			IT_COMMENT("Not supported with CV++ 6.0");
			//IT_COMMENT1("Value = %d", pValue->ullVal);
		}
		break;
		*/
		case VT_R4:
		{
			item_to_send.iec_type = M_ME_TF_1;
			epoch_to_cp56time2a(&time, epoch_in_millisec);
			item_to_send.iec_obj.o.type36.mv = V_R4(pValue);
			item_to_send.iec_obj.o.type36.time = time;

			if(pwQualities != OPC_QUALITY_GOOD)
				item_to_send.iec_obj.o.type36.iv = 1;

			IT_COMMENT1("Value = %f", V_R4(pValue));
		}
		break;
		case VT_R8:
		{
			#ifdef EXTENDEND_IEC_TYPES
			item_to_send.iec_type = M_ME_TN_1;
			epoch_to_cp56time2a(&time, epoch_in_millisec);
			item_to_send.iec_obj.o.type150.mv = V_R8(pValue);
			item_to_send.iec_obj.o.type150.time = time;

			if(pwQualities != OPC_QUALITY_GOOD)
				item_to_send.iec_obj.o.type150.iv = 1;
			#else
			item_to_send.iec_type = M_ME_TF_1;
			epoch_to_cp56time2a(&time, epoch_in_millisec);
			item_to_send.iec_obj.o.type36.mv = (float)V_R8(pValue);
			item_to_send.iec_obj.o.type36.time = time;

			if(pwQualities != OPC_QUALITY_GOOD)
				item_to_send.iec_obj.o.type36.iv = 1;
			#endif

			IT_COMMENT1("Value = %lf", V_R8(pValue));
		}
		break;
		//case VT_CY:
		//{
			//Currency
		//}
		//break;
		case VT_BOOL:
		{
			item_to_send.iec_type = M_SP_TB_1;
			epoch_to_cp56time2a(&time, epoch_in_millisec);
			item_to_send.iec_obj.o.type30.sp = (V_BOOL(pValue) < 0 ? 1 : 0);
			item_to_send.iec_obj.o.type30.time = time;

			if(pwQualities != OPC_QUALITY_GOOD)
				item_to_send.iec_obj.o.type30.iv = 1;
			
			IT_COMMENT1("Value = %d", V_BOOL(pValue));
		}
		break;
		case VT_DATE:
		{
			/*
			double date = V_DATE(pValue);
			FILETIME ftime;
			
			DateToFileTime(&date, &ftime);

			epoch_in_millisec = epoch_from_FILETIME(&ftime);
			epoch_to_cp56time2a(&time, epoch_in_millisec);
			
			//Just for test we use a single point
			item_to_send.iec_type = M_SP_TB_1;
			item_to_send.iec_obj.o.type30.sp = 0;
			item_to_send.iec_obj.o.type30.time = time;

			if(pwQualities != OPC_QUALITY_GOOD)
				item_to_send.iec_obj.o.type30.iv = 1;
			*/
		}
		break;
		case VT_BSTR:
		{

			//fprintf(stderr,"%ls\t", V_BSTR(pValue));
			//fflush(stderr);
			//IT_COMMENT1("Value STRING = %ls", V_BSTR(pValue));

			//Definizione di BSTR:
			//typedef OLECHAR *BSTR;

			//Conversioni:

			//Da const char* a OLE

			//TCHAR* pColumnName
			//OLECHAR*    pOleColumnName = T2OLE(pColumnName);
								
			//Da OLE a const char*
			TCHAR* str = OLE2T(pValue->bstrVal);
			//fprintf(stderr,"%s\n", str);
			//fflush(stderr);
			
			#ifdef EXTENDEND_IEC_TYPES
			item_to_send.iec_type = M_ME_TN_1;
			epoch_to_cp56time2a(&time, epoch_in_millisec);
			item_to_send.iec_obj.o.type150.mv = atof(str);
			item_to_send.iec_obj.o.type150.time = time;

			if(pwQualities != OPC_QUALITY_GOOD)
				item_to_send.iec_obj.o.type150.iv = 1;

			//fprintf(stderr,"%lf\n", item_to_send.iec_obj.o.type150.mv);
			//fflush(stderr);
			#else //EXTENDEND_IEC_TYPES
			item_to_send.iec_type = M_ME_TF_1;
			epoch_to_cp56time2a(&time, epoch_in_millisec);
						
			//item_to_send.iec_obj.o.type36.mv = (float)atof(str);
			item_to_send.iec_obj.o.type36.time = time;

			if(pwQualities != OPC_QUALITY_GOOD)
				item_to_send.iec_obj.o.type36.iv = 1;
			#endif //EXTENDEND_IEC_TYPES
						
			IT_COMMENT1("Value STRING = %s", str);
		}
		break;
		default:
		{
			IT_COMMENT1("V_VT(pValue) non gestito = %d", V_VT(pValue));
		
			item_to_send.iec_type = 0;
		}
		break;
	}

	IT_COMMENT6("at time: %d_%d_%d_%d_%d_%d", time.hour, time.min, time.msec, time.mday, time.month, time.year);

	item_to_send.msg_id = n_msg_sent;

	//Send messages to monitor.exe

	fprintf(stderr,"Sending message %u th\n", n_msg_sent);
	fflush(stderr);
	IT_COMMENT1("Sending message %u th\n", n_msg_sent);
/*
	unsigned char buf[sizeof(struct iec_item)];
	int len = sizeof(struct iec_item);
	memcpy(buf, &item_to_send, len);

	//FILE* fp;
	//fp = fopen("..\\logs\\producer.c", "a");

	//fprintf(fp,"Sending %u th message \n", n_msg_sent);
	//fflush(fp);

//	for(j = 0;j < len; j++)
//	{
//		unsigned char c = *(buf + j);
		//fprintf(stderr,"tx ---> 0x%02x\n", c);
		//fflush(stderr);
		//fprintf(fp,"tx %d ---> 0x%02x\n", j, c);
		//fflush(fp);
		//IT_COMMENT1("tx ---> 0x%02x\n", c);
//	}

	//fclose(fp);
*/

	//prepare published data

	n_msg_sent++;

	IT_EXIT;
}

//The FILETIME structure is a 64-bit value representing the number 
//of 100-nanosecond intervals since January 1, 1601.
//
//epoch_in_millisec is a 64-bit value representing the number of milliseconds 
//elapsed since January 1, 1970

signed __int64 Opc_client_ae_imp::epoch_from_FILETIME(const FILETIME *fileTime)
{
	IT_IT("epoch_from_FILETIME");
	
	FILETIME localTime;
	struct tm	t;

	time_t sec;
	signed __int64 epoch_in_millisec;

	if(fileTime == NULL)
	{
		return 0;
	}
	
	// first convert file time (UTC time) to local time
	if (!FileTimeToLocalFileTime(fileTime, &localTime))
	{
		return 0;
	}

	// then convert that time to system time
	SYSTEMTIME sysTime;
	if (!FileTimeToSystemTime(&localTime, &sysTime))
	{
		return 0;
	}
	
	memset(&t, 0x00, sizeof(struct tm));
	
	t.tm_hour = sysTime.wHour;
	t.tm_min = sysTime.wMinute;
	t.tm_sec = sysTime.wSecond;
	t.tm_mday = sysTime.wDay;
	t.tm_mon = sysTime.wMonth - 1;
	t.tm_year = sysTime.wYear - 1900;  //tm_year contains years after 1900
	t.tm_isdst = -1; //to force mktime to check for dst
	
	sec = mktime(&t);

	if(sec < 0)
	{
		return 0;
	}

	epoch_in_millisec =  (signed __int64)sec;

	epoch_in_millisec =  epoch_in_millisec*1000 + sysTime.wMilliseconds;

	IT_EXIT;
	return epoch_in_millisec;
}

#include <time.h>
#include <sys/timeb.h>

void Opc_client_ae_imp::get_utc_host_time(struct cp56time2a* time)
{
	struct timeb tb;
	struct tm	*ptm;
		
	IT_IT("get_utc_host_time");

    ftime (&tb);
	ptm = gmtime(&tb.time);
		
	time->hour = ptm->tm_hour;					//<0..23>
	time->min = ptm->tm_min;					//<0..59>
	time->msec = ptm->tm_sec*1000 + tb.millitm; //<0..59999>
	time->mday = ptm->tm_mday; //<1..31>
	time->wday = (ptm->tm_wday == 0) ? ptm->tm_wday + 7 : ptm->tm_wday; //<1..7>
	time->month = ptm->tm_mon + 1; //<1..12>
	time->year = ptm->tm_year - 100; //<0..99>
	time->iv = 0; //<0..1> Invalid: <0> is valid, <1> is invalid
	time->su = (u_char)tb.dstflag; //<0..1> SUmmer time: <0> is standard time, <1> is summer time

	IT_EXIT;
    return;
}

time_t Opc_client_ae_imp::epoch_from_cp56time2a(const struct cp56time2a* time)
{
	struct tm	t;
	time_t epoch = 0;
	int ms;
	
	memset(&t, 0x00, sizeof(struct tm));
	
	t.tm_hour = time->hour;
	t.tm_min = time->min;
	t.tm_sec = time->msec/1000;
	ms = time->msec%1000; //not used
	t.tm_mday = time->mday;
	t.tm_mon = time->month - 1;	  //converted from <1..12> to	<0..11>				
	t.tm_year = time->year + 100; //converted from <0..99> to <years from 1900>
	t.tm_isdst = time->su;
	
	epoch = mktime(&t);

	if((epoch == -1) || (time->iv == 1))
	{
		epoch = 0;
	}

	return epoch;
}

void Opc_client_ae_imp::start_timer_for_blocked_processes(void)
{
	//get current UTC time
	//equivale allo START (o RESTART) del timer
	cp56time2a start_opc_watch_dog_timer;
	get_utc_host_time(&start_opc_watch_dog_timer);
	timer_starts_at_epoch = epoch_from_cp56time2a(&start_opc_watch_dog_timer);
	fprintf(stderr,"---------Waiting CONNECTION_ENSTABLISHED event---------\n");
	fflush(stderr);
	
	state_of_watch_dog_blocked_processes = TRYING_TO_CONNECT_TO_SERVER;
}

#define ITEM_WRITTEN_AT_A_TIME 1
#define MAX_COMMAND_SEND_TIME 60

void Opc_client_ae_imp::check_for_commands(struct iec_item *queued_item)
{
	DWORD dw = 0;
	DWORD nWriteItems = ITEM_WRITTEN_AT_A_TIME;
	HRESULT hr = S_OK;
	HRESULT *pErrorsWrite = NULL;
	HRESULT *pErrorsRead = NULL;
        
	if(!fExit)
	{ 
		fprintf(stderr,"Receiving %d th message \n", queued_item->msg_id);
		fflush(stderr);
					
		/////////////////////write command///////////////////////////////////////////////////////////
		if(queued_item->iec_type == C_SC_TA_1
			|| queued_item->iec_type == C_DC_TA_1
			|| queued_item->iec_type == C_SE_TA_1
			|| queued_item->iec_type == C_SE_TB_1
			|| queued_item->iec_type == C_SE_TC_1
			|| queued_item->iec_type == C_BO_TA_1
			|| queued_item->iec_type == C_SC_NA_1
			|| queued_item->iec_type == C_DC_NA_1
			|| queued_item->iec_type == C_SE_NA_1 
			|| queued_item->iec_type == C_SE_NB_1
			|| queued_item->iec_type == C_SE_NC_1
			|| queued_item->iec_type == C_BO_NA_1)
		{
			Sleep(100); //Delay between one command and the next one

			/////////Here we make the QUERY:////////////////////////////////////////// /////////////////////////////
			//select from Item table hClient where ioa is equal to ioa of packet arriving (command) from monitor.exe
			///////////////////////////////////////////////////////////////////////////////////////
			int found = 0;
			DWORD hClient = -1;
/*
			for(dw = 0; dw < g_dwNumItems; dw++) 
			{ 
				if(queued_item->iec_obj.ioa == Item[dw].ioa_control_center)
				{
					found = 1;
					hClient = Item[dw].hClient;
					break;
				}
			}

			if(found == 0)
			{
				fprintf(stderr,"Error: Command with IOA %d not found in I/O list\n", queued_item->iec_obj.ioa);
				fflush(stderr);
				fprintf(stderr,"Command NOT executed\n");
				fflush(stderr);
				return;
			}
			/////////////////////////////////////////////////////////////////////

			//check iec type of command
			if(Item[hClient - 1].io_list_iec_type != queued_item->iec_type)
			{
				//error
				fprintf(stderr,"Error: Command with IOA %d has iec_type %d, different from IO list type %d\n", queued_item->iec_obj.ioa, queued_item->iec_type, Item[hClient - 1].io_list_iec_type);
				fflush(stderr);
				fprintf(stderr,"Command NOT executed\n");
				fflush(stderr);
				return;
			}
*/
			//Receive a write command
								
			fprintf(stderr,"Receiving command for hClient %d, ioa %d\n", hClient, queued_item->iec_obj.ioa);
			fflush(stderr);
			
			//TODO: implement OPC AE write operation
			
		}
		else if(queued_item->iec_type == C_EX_IT_1)
		{
			//Receiving EXIT process command from monitor.exe
			//exit the thread, and stop the process
			fExit = true;
		}
		else if(queued_item->iec_type == C_IC_NA_1)
		{
			//Do Refresh
		}
	}

	return;
}

void Opc_client_ae_imp::alloc_command_resources(void)
{

}

void Opc_client_ae_imp::free_command_resources(void)
{

}