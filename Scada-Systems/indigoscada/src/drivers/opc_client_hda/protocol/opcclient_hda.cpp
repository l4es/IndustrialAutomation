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
// OPC HDA 1.20
////////////////////////////////////////////////////////////////////////////////
#include <stdio.h>
#include <time.h>
#include <windows.h>
#include <conio.h>
#include <atlbase.h>
CComModule _Module;
#include <atlcom.h>
#include <atlctl.h>
#include "opchda.h"	/* The OPC custom interface defintions */
#include "opccomn.h"
#include "OpcEnum.h"
#include "OpcHda_Error.h"
#include "opcerror.h"
#include "general_defines.h"
#include "itrace.h"
#include "opc_client_hda_app.h"
#include "opchdaclasses.h"
#include "iec104types.h"
#include "iec_item.h"

bool Opc_client_hda_imp::fExit = false;
double Opc_client_hda_imp::dead_band_percent = 0.0;
IOPCHDA_Server* Opc_client_hda_imp::g_pIOPCHDAServer = NULL;

int Opc_client_hda_imp::Update()
{
	IT_IT("Opc_client_hda_imp::Update");

    int rc = 0;
	int check_server = 0;

	while(true)
	{
		if(fExit)
		{
			IT_COMMENT("Opc_client_hda_imp exiting....");
			break; //terminate the thread
		}

        //check connection every g_dwUpdateRate*10
		if((check_server%10) == 0)
		{
			rc = check_connection_to_server();
			fprintf(stderr,"check for server connection...\n");
			fflush(stderr);
		}

		check_server++;

		if(rc)
		{ 
			fprintf(stderr,"Opc_client_hda_imp exiting...., due to lack of connection with server\n");
			fflush(stderr);
			break; 
		}

		::Sleep(g_dwUpdateRate);
	}

	return 0;
}

int Opc_client_hda_imp::OpcStart(char* OpcServerProgID, char* OpcclassId, char* OpcUpdateRate, char* OpcPercentDeadband)
{
	IT_IT("Opc_client_hda_imp::OpcStart");

	char show_msg[150];

	if((strlen(ServerIPAddress) == 0))
	{
		strcpy(ServerIPAddress, "127.0.0.1");
	}
		
	//DCOM connection (and local connection)

	printf("Trying to connect to remote (or local) HDA server on machine with IP: %s\n", ServerIPAddress);
	sprintf(show_msg, "Trying to connect to remote (or local) HDA server on machine with IP: %s", ServerIPAddress);

	LogMessage(S_OK, "", show_msg);
	
	HRESULT	hr = ::CoInitializeEx(NULL,COINIT_MULTITHREADED); // setup COM lib

	if(FAILED(hr))
	{
		printf("CoInitializeEx failed\n");
		LogMessage(hr,"CoInitializeEx failed");
		return 1;
	}

	hr = CoInitializeSecurity(NULL, -1, NULL, NULL, RPC_C_AUTHN_LEVEL_NONE, RPC_C_IMP_LEVEL_IMPERSONATE, NULL, EOAC_NONE, NULL);

	if(FAILED(hr))
	{
		fprintf(stderr,"CoInitializeSecurity failed\n");
		fflush(stderr);
		LogMessage(hr,"CoInitializeSecurity()");
		IT_EXIT;
		return 1;
	}
	
	COAUTHINFO athn;
	ZeroMemory(&athn, sizeof(COAUTHINFO));
	// Set up the NULL security information
	athn.dwAuthnLevel = RPC_C_AUTHN_LEVEL_CONNECT;
	//athn.dwAuthnLevel = RPC_C_AUTHN_LEVEL_NONE;
	athn.dwAuthnSvc = RPC_C_AUTHN_WINNT;
	athn.dwAuthzSvc = RPC_C_AUTHZ_NONE;
	athn.dwCapabilities = EOAC_NONE;
	athn.dwImpersonationLevel = RPC_C_IMP_LEVEL_IMPERSONATE;
	//athn.dwImpersonationLevel = RPC_C_IMP_LEVEL_ANONYMOUS;
	athn.pAuthIdentityData = NULL;
	athn.pwszServerPrincName = NULL;

	USES_CONVERSION;

	COSERVERINFO remoteServerInfo;
	ZeroMemory(&remoteServerInfo, sizeof(COSERVERINFO));
	remoteServerInfo.pAuthInfo = &athn;
	remoteServerInfo.pwszName = T2OLE(ServerIPAddress);
	//printf("Remote host: %s\n", OLE2T(remoteServerInfo.pwszName));

	MULTI_QI reqInterface;
	reqInterface.pIID = &IID_IOPCServerList; //requested interface
	reqInterface.pItf = NULL;
	reqInterface.hr = S_OK;
							//requested class
	hr = CoCreateInstanceEx(CLSID_OpcServerList,NULL, CLSCTX_REMOTE_SERVER, &remoteServerInfo, 1, &reqInterface);
	
	if (FAILED(hr))
	{
		printf("OPC error:Failed to get remote interface, %x\n", hr);
		LogMessage(hr,"Failed to get remote interface");
		return 1;
	}
	
	g_iCatInfo = (IOPCServerList *)reqInterface.pItf;

	///////////////////////////////////////

	CATID Implist[1];

	Implist[0] = IID_CATID_OPCHDAServer10;

	IEnumCLSID *iEnum = NULL;

	hr = g_iCatInfo->EnumClassesOfCategories(1, Implist,0, NULL,&iEnum);

	if (FAILED(hr))
	{
		printf("OPC error:Failed to get enum for categeories, %x\n", hr);
		LogMessage(hr,"Failed to get enum for categeories");
		return 1;
	}

	GUID glist;

	ULONG actual;

	printf("Available HDA server(s) on remote machine:\n");

	while((hr = iEnum->Next(1, &glist, &actual)) == S_OK)
	{
		WCHAR *progID;
		WCHAR *userType;
		HRESULT res = g_iCatInfo->GetClassDetails(glist, &progID, &userType);/*ProgIDFromCLSID(glist, &progID)*/;

		if(FAILED(res))
		{
			printf("OPC error:Failed to get ProgId from ClassId, %x\n",res);
			LogMessage(res,"Failed to get ProgId from ClassId");
			return 1;
		}
		else 
		{
			USES_CONVERSION;
			char * str = OLE2T(progID);
			char * str1 = OLE2T(userType);
			printf("HDA - %s\n", str);
			::CoTaskMemFree(progID);
			::CoTaskMemFree(userType);
		}
	}
			
	if((strlen(OpcServerProgID) == 0))
	{
		printf("OPC error: Please supply ProgID\n");
		LogMessage(S_FALSE,"Please supply ProgID");
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
	char classIdString[100];
	CLSID classId;
	
	hr = RegConnectRegistry(ServerIPAddress, HKEY_LOCAL_MACHINE, &remoteRegHandle);

	if(hr != S_OK)
	{
		char show_msg[150];

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
		printf("RegConnectRegistry failed: %s\n", lpMsgBuf);
		Opc_client_hda_imp::LogMessage(hr, "", show_msg);			

		LocalFree(lpMsgBuf);
		
		if(strlen(OpcclassId) > 0)
		{
			//If this thread is run as Local Account, then you need to have the remote classId string (CLSID)
							
			strcpy(classIdString, OpcclassId);

			USES_CONVERSION;

			LPOLESTR sz = A2W(classIdString);

			hr = CLSIDFromString(sz,&classId);

			if(FAILED(hr))
			{
				fprintf(stderr,"CLSIDFromString failed\n");
				fflush(stderr);
				LogMessage(hr,"CLSIDFromString failed");
				return 1;
			}
		}
	}
	else
	{
	   hr = RegOpenKeyEx(remoteRegHandle, keyName, 0, KEY_READ, &keyHandle);

	   if(SUCCEEDED(hr))
	   {
		   DWORD entryType;

		   unsigned bufferSize = 100;

		   hr = RegQueryValueEx(keyHandle, NULL, 0, &entryType, (LPBYTE)&classIdString, (LPDWORD)&bufferSize);

		   if(FAILED(hr))
		   {
				printf("RegQueryValueEx failed\n");
				LogMessage(hr,"RegQueryValueEx failed");
				return 1;
		   }
		   else
		   {
				USES_CONVERSION;

				LPOLESTR sz = A2W(classIdString);

				hr = CLSIDFromString(sz,&classId);

				if(FAILED(hr))
				{
					printf("CLSIDFromString failed\n");
					LogMessage(hr,"CLSIDFromString failed");
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
	//athn.dwImpersonationLevel = RPC_C_IMP_LEVEL_ANONYMOUS;
	athn.pAuthIdentityData = NULL;
	athn.pwszServerPrincName = NULL;
	
	ZeroMemory(&remoteServerInfo, sizeof(COSERVERINFO));
	remoteServerInfo.pAuthInfo = &athn;
	
	remoteServerInfo.pwszName = T2OLE(ServerIPAddress);

	//printf("%s\n", OLE2T(remoteServerInfo.pwszName));
	
	reqInterface.pIID = &IID_IUnknown; //requested interface
	reqInterface.pItf = NULL;
	reqInterface.hr = S_OK;
		                      //requsted class
	hr = CoCreateInstanceEx(classId,NULL, CLSCTX_REMOTE_SERVER, &remoteServerInfo, 1, &reqInterface);	
	
	if (FAILED(hr))
	{
		printf("OPC error:Failed to get remote interface, %x\n", hr);
		LogMessage(hr,"Failed to get remote interface");
		return 1;
	}
	
	IUnknown * pIUnknown = NULL;

	pIUnknown = reqInterface.pItf;

	/////end make Remote Object

	hr = pIUnknown->QueryInterface(IID_IOPCHDA_Server, (void**)&g_pIOPCHDAServer);

	if (FAILED(hr))
	{
		printf("OPC error:Failed to obtain IID_IOPCHDA_Server interface from server, %x\n", hr);
		LogMessage(hr,"Failed to obtain IID_IOPCHDA_Server interface from server");
		return 1;
	}

	printf("Connected to server %s.\n", ServerIPAddress);

	sprintf(show_msg, "Connected to HDA server on machine with IP: %s", ServerIPAddress);
	Opc_client_hda_imp::LogMessage(S_OK, "", show_msg);

	WORD wMajor, wMinor, wBuild;
	LPWSTR pwsz = NULL;

	if(!GetStatus(&wMajor, &wMinor, &wBuild, &pwsz))
	{
		char ver[250];
		//printf("Version: %d.%d.%d\n", wMajor, wMinor, wBuild);
		//printf("%ls\n\n",pwsz);
		sprintf(ver,"Server version: %d.%d.%d, %s", wMajor, wMinor, wBuild, W2T(pwsz));
		printf("%s\n\n",ver);

		IT_COMMENT4("Version: %d.%d.%d, %s", wMajor, wMinor, wBuild,W2T(pwsz));
		Opc_client_hda_imp::LogMessage(S_OK, "",ver);
		::CoTaskMemFree(pwsz);
	}

	hr = g_pIOPCHDAServer->QueryInterface(IID_IOPCCommon, (void**)&g_pIOPCCommon);

	if(FAILED(hr))
	{
		LogMessage(hr,"QueryInterface(IID_IOPCCommon)");
	}
	else
	{
		g_pIOPCCommon->SetClientName(L"IndigoSCADA OPC HDA Client");
	}

	VARIANT vFilter;
	HRESULT *ppErrors = NULL;
	OPCHDA_OPERATORCODES pOperator;
	DWORD pdwAttrID;
	DWORD dwCount = 1000;

	hr = g_pIOPCHDAServer->CreateBrowse( 
            /* [in] */ dwCount,
            /* [size_is][in] */ &pdwAttrID,
            /* [size_is][in] */ &pOperator,
            /* [size_is][in] */ &vFilter,
            /* [out] */ &g_pIOPCHDA_Browser,
            /* [size_is][size_is][out] */ &ppErrors);

	if(FAILED(hr))
	{
		LogMessage(hr,"QueryInterface(IID_IOPCHDA_Browser)");
	}

	BOOL bActive;
	DWORD dwBufferTime;
	
	DWORD hClientSubscription;
	DWORD dwRevisedBufferTime;
	DWORD dwRevisedMaxSize;

	CComCOPCHistoricDASink   *m_pSink = NULL;
	CComCOPCShutdownRequest  *m_pShutdown = NULL;

	ATLTRY(m_pSink = new CComCOPCHistoricDASink);

	if(m_pSink == NULL)
	{
		LogMessage(E_OUTOFMEMORY,"new CComCOPCHistoricDASink");
		return 1;
	}

	//hClientSubscription = 1243272;
	//dwBufferTime = 10000; //this is a parameter
	//dwBufferTime = 0; //this is a parameter

	//hr = g_pIOPCHDAServer->CreateEventSubscription(bActive,
	//					dwBufferTime,
	//					dwMaxSize,
	//					hClientSubscription,
	//					IID_IOPCEventSubscriptionMgt,
	//				   (IUnknown **)&m_ISubMgt,
	//				   &dwRevisedBufferTime,
	//				   &dwRevisedMaxSize);

	//if(hr != S_OK)
	//{
	//	printf("Failed to Create Subscription\n");
	//	return 1;
	//}

//	printf("HDA server dwRevisedBufferTime = %d, dwRevisedMaxSize = %d\n", dwRevisedBufferTime, dwRevisedMaxSize);

//	if(m_ISubMgt == NULL)
//	{
//		printf("CreateEventSubscription returned m_ISubMgt NULL\n");
//		return 1;
//	}

//	printf("CreateEventSubscription Done\n");
	
	// create advise
//	CComObject<CComCOPCHistoricDASink>::CreateInstance(&m_pSink);
//	m_dwCookie = 0xCDCDCDCD;

	IUnknown* pUnk;

	hr = m_pSink->_InternalQueryInterface( __uuidof(IUnknown), (void**)&pUnk );

	if(hr != S_OK)
	{
		printf("Failed m_pSink->_InternalQueryInterface\n");
		return 1;
	}

//	hr = AtlAdvise(m_ISubMgt, pUnk, __uuidof(IOPCHDA_DataCallback), &m_dwCookie );

//	if(hr != S_OK)
//	{
//		printf("Failed AtlAdvise m_dwCookie\n");
//		return 1;
//	}

	//shutdown advise 
	///////////////////////////////////////////////Shutdown/////////////////////////////
/*
//does not work...

	CComObject<COPCShutdownRequest>::CreateInstance(&m_pShutdown);
	m_dwShutdownCookie = 0xCDCDCDCD;

	//IUnknown* pUnk;

	hr = m_pShutdown->_InternalQueryInterface( __uuidof(IUnknown), (void**)&pUnk );

	if(hr != S_OK)
	{
		printf("Failed m_pShutdown->_InternalQueryInterface\n");
		return 1;
	}

	//hr = AtlAdvise(m_ISubMgt, m_pShutdown->GetUnknown(),__uuidof(IOPCShutdown), &m_dwShutdownCookie);

	hr = AtlAdvise(m_ISubMgt, pUnk, __uuidof(IOPCShutdown), &m_dwShutdownCookie);
	
	if(hr != S_OK)
	{
		printf("Failed shutdown advise\n");
		return 1;
	}
*/
	////////////////////////GetState and SetState/////////////////////////////////////////////////////////////

//	hr = m_ISubMgt->GetState(&bActive,&dwBufferTime,&dwMaxSize,&hClientSubscription);

//	if(hr != S_OK)
//	{
//		printf("Failed m_ISubMgt->GetState\n");
//		return 1;
//	}

//	printf("Server state: bActive = %d, dwBufferTime = %d, dwMaxSize = %d, hClientSubscription = %d\n", bActive, dwBufferTime, dwMaxSize, hClientSubscription);
/*
Here only for test, it works.

	hr = m_ISubMgt->SetState(&bActive, &dwBufferTime, &dwMaxSize, hClientSubscription, &dwRevisedBufferTime, &dwRevisedMaxSize);

	if(hr != S_OK)
	{
		printf("Failed m_ISubMgt->SetState\n");
		return 1;
	}
*/
	///////////////////////Refresh//////////////////////////////////////////////////////////////

//	hr = m_ISubMgt->Refresh(m_dwCookie);

//	if(hr != S_OK)
//	{
//		printf("Failed Refresh\n");
//		return 1;
//	}

	///////////////////////SetKeepAlive//////////////////////////////////////////////////////////////
	
	//IOPCEventSubscriptionMgt2Ptr ISubMgt2 = m_ISubMgt;

/*
//does not work...

	IOPCEventSubscriptionMgt2* ISubMgt2 = (struct IOPCEventSubscriptionMgt2*)m_ISubMgt;
	
	if(ISubMgt2 != NULL)
	{
		DWORD dwRevisedKeepAliveTime = 0;
		// set the keep-alive to 3X the dwRevisedBufferTime
		hr = ISubMgt2->SetKeepAlive(3 * dwRevisedBufferTime, &dwRevisedKeepAliveTime);

		printf("dwRevisedKeepAliveTime = %d\n", dwRevisedKeepAliveTime);
	}
*/

/*
	IOPCEventSubscriptionMgt2* ISubMgt2;

	hr = g_pIOPCHDAServer->QueryInterface(IID_IOPCEventSubscriptionMgt2, (void**)&ISubMgt2);

	if(FAILED(hr))
	{
		printf("OPC error:Failed to obtain IID_IOPCEventSubscriptionMgt2 interface %x\n",hr);
		LogMessage(hr,"Failed to obtain IID_IOPCEventSubscriptionMgt2 interface");
		return 1;
	}
*/

    return(0);
}

int Opc_client_hda_imp::OpcStop()
{
	IT_IT("Opc_client_hda_imp::OpcStop");

//	HRESULT hr;

//	if(m_ISubMgt != NULL)
//	{
//		if(m_dwCookie != 0xCDCDCDCD)
//		{
//			hr = AtlUnadvise(m_ISubMgt, __uuidof(IOPCHDA_DataCallback), m_dwCookie);
//			m_dwCookie = 0xCDCDCDCD;
//		}
//
//		if(m_dwShutdownCookie != 0xCDCDCDCD)
//		{
//			hr = AtlUnadvise(m_ISubMgt, __uuidof(IOPCShutdown), m_dwShutdownCookie);
//			m_dwShutdownCookie = 0xCDCDCDCD;
//		}
//	}

	// terminate server and it will clean up itself
	if(g_pIOPCHDAServer) while(g_pIOPCHDAServer->Release()) ;
	::CoUninitialize();

	printf("Server and all group interfaces terminated.\n");

	LogMessage(S_OK,"","Server and all group interfaces terminated");
	return 1;
}

/*
void ::OnOpcDisconnect() 
{
	CWaitCursor	cWait;		//show wait cursor. 
	HRESULT hr;

	m_bConnected = FALSE;
	m_bSubscription = FALSE;
	
	if(m_ISub != NULL)
	{
		OLE_TRY(hr = AtlUnadvise( m_ISub,__uuidof(IOPCHDA_DataCallback), m_dwCookie ));
		OLE_TRY(hr = AtlUnadvise( m_ISub,__uuidof(IOPCShutdown), m_dwShutdownCookie ));
		m_ISub.Attach(NULL);
	}

	m_IEventServer2.Attach(NULL);
	m_IEventServer.Attach(NULL);  //detach server
	m_ICommon.Attach(NULL);
	
	OnViewClearAll();
	
}
*/

int Opc_client_hda_imp::GetStatus(WORD *pwMav, WORD *pwMiv, WORD *pwB, LPWSTR *pszV)
{
	IT_IT("Opc_client_hda_imp::GetStatus");

	*pwMav = 0;
	*pwMiv = 0;
	*pwB = 0;
	*pszV = NULL;
	OPCHDA_SERVERSTATUS *pStatus = NULL;
	FILETIME *pftCurrentTime = NULL;
	FILETIME *pftStartTime = NULL;
	DWORD pdwMaxReturnValues;
	LPWSTR ppszStatusString = NULL;
	

	if(g_pIOPCHDAServer == NULL) return E_POINTER;
	
	HRESULT hr = g_pIOPCHDAServer->GetHistorianStatus(
		pStatus,
		&pftCurrentTime, 
		&pftStartTime, 
		pwMav, pwMiv, pwB, 
		&pdwMaxReturnValues,
		&ppszStatusString, 
		pszV);

	if(FAILED(hr) || (pStatus == NULL) )
	{
		if(FAILED(hr))	LogMessage(hr,"GetStatus()");
		//if(pStatus != NULL) ::CoTaskMemFree(pStatus);
		return E_FAIL;
	}

	//*pwMav = pStatus->wMajorVersion;
	//*pwMiv = pStatus->wMinorVersion;
	//*pwB = pStatus->wBuildNumber;
	//*pszV = pStatus->szVendorInfo;
	//::CoTaskMemFree(pStatus);
	

	return 0;
}

int Opc_client_hda_imp::check_connection_to_server(void)
{
	IT_IT("Opc_client_hda_imp::check_connection_to_server");

	WORD wMajor, wMinor, wBuild;

	LPWSTR pwsz = NULL;

	if(!GetStatus(&wMajor, &wMinor, &wBuild, &pwsz))
	{
		::CoTaskMemFree(pwsz);
	}
	else
	{
		IT_EXIT;
		return 1;
	}

	return 0;
}

void Opc_client_hda_imp::LogMessage(HRESULT hr, LPCSTR pszError, const char* name)
{
	//TODO: send message to monitor.exe
}

#include <time.h>
#include <sys/timeb.h>

void Opc_client_hda_imp::get_utc_host_time(struct cp56time2a* time)
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

