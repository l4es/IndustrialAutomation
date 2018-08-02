/*
 *                         IndigoSCADA
 *
 *   This software and documentation are Copyright 2002 to 2014 Enscada 
 *   Limited and its licensees. All rights reserved. See file:
 *
 *                     $HOME/LICENSE 
 *
 *   for full copyright notice and license terms. 
 *
 */
//TODO: AddItems: add support of arrays
#include "opc_client_da_app.h"
CComModule _Module;
#include <atlcom.h>
#include <atlctl.h>
#include "iec104types.h"
#include "iec_item.h"
#include "clear_crc_eight.h"

#define MAX_KEYLEN 256
#define MAX_COMMAND_SEND_TIME 60

static DWORD g_dwSleepInLoop = 1000;

#include <sys/stat.h>

#include "opcda_2_0_classes.h"

#ifdef USE_RIPC_MIDDLEWARE
/////////////////////////////////////Middleware////////////////////////////////
static void iec_call_exit_handler(int line, char* file, char* reason);
///////commands
void control_dir_consumer(void* pParam)
{
	struct subs_args* arg = (struct subs_args*)pParam;
	struct iec_item item;
	RIPCObject objDesc(&item, sizeof(struct iec_item));

	Opc_client_da_imp* parent = (Opc_client_da_imp*)arg->parent;

	while(1)
	{
		if(parent->exit_threads)
		{
			break;
		}

		parent->queue_control_dir->get(objDesc);

		parent->check_for_commands(&item);
	}
}
////////////////////////////////Middleware/////////////////
#endif

/////////////////////////////////////Middleware///////////////////////////////////////////
Boolean  quite = ORTE_FALSE;
int	regfail=0;

//event system
void onRegFail(void *param) 
{
  printf("registration to a manager failed\n");
  regfail = 1;
}

void rebuild_iec_item_message(struct iec_item *item2, iec_item_type *item1)
{
	unsigned char checksum;

	///////////////Rebuild struct iec_item//////////////////////////////////
	item2->iec_type = item1->iec_type;
	memcpy(&(item2->iec_obj), &(item1->iec_obj), sizeof(struct iec_object));
	item2->cause = item1->cause;
	item2->msg_id = item1->msg_id;
	item2->ioa_control_center = item1->ioa_control_center;
	item2->casdu = item1->casdu;
	item2->is_neg = item1->is_neg;
	item2->checksum = item1->checksum;
	///////and check the 1 byte checksum////////////////////////////////////
	checksum = clearCrc((unsigned char *)item2, sizeof(struct iec_item));

//	fprintf(stderr,"new checksum = %u\n", checksum);

	//if checksum is 0 then there are no errors
	if(checksum != 0)
	{
		//log error message
		ExitProcess(0);
	}

	/*
	fprintf(stderr,"iec_type = %u\n", item2->iec_type);
	fprintf(stderr,"iec_obj = %x\n", item2->iec_obj);
	fprintf(stderr,"cause = %u\n", item2->cause);
	fprintf(stderr,"msg_id =%u\n", item2->msg_id);
	fprintf(stderr,"ioa_control_center = %u\n", item2->ioa_control_center);
	fprintf(stderr,"casdu =%u\n", item2->casdu);
	fprintf(stderr,"is_neg = %u\n", item2->is_neg);
	fprintf(stderr,"checksum = %u\n", item2->checksum);
	*/
}

void recvCallBack(const ORTERecvInfo *info,void *vinstance, void *recvCallBackParam) 
{
	Opc_client_da_imp * cl = (Opc_client_da_imp*)recvCallBackParam;
	iec_item_type *item1 = (iec_item_type*)vinstance;

	switch (info->status) 
	{
		case NEW_DATA:
		{
		  if(!quite)
		  {
			  struct iec_item item2;
			  rebuild_iec_item_message(&item2, item1);
			  cl->received_command_callback = 1;
			  cl->check_for_commands(&item2);
			  cl->received_command_callback = 0;
		  }
		}
		break;
		case DEADLINE:
		{
			//printf("deadline occurred\n");
		}
		break;
	}
}
////////////////////////////////Middleware/////////////////////////////////////

extern int gl_timeout_connection_with_parent;

bool Opc_client_da_imp::fExit = false;
struct structItem* Opc_client_da_imp::Item = NULL;
double Opc_client_da_imp::dead_band_percent = 0.0;
DWORD Opc_client_da_imp::g_dwWriteTransID = 2;
IOPCServer* Opc_client_da_imp::g_pIOPCServer = NULL;
#ifdef USE_RIPC_MIDDLEWARE
RIPCQueue*  Opc_client_da_imp::queue_monitor_dir = NULL;
#endif

////////////////////////////////Middleware/////////////////
iec_item_type Opc_client_da_imp::instanceSend;
ORTEPublication* Opc_client_da_imp::publisher = NULL;
////////////////////////////////Middleware/////////////////

static u_int n_msg_sent = 0;

Opc_client_da_imp::Opc_client_da_imp(char* opc_server_address, char* line_number)
{ 
	IT_IT("Opc_client_da_imp::Opc_client_da_imp");

	strcpy(ServerIPAddress, opc_server_address);

	g_bWriteComplete = true;
	g_dwReadTransID = 1;
	Config_db = NULL;
	opc_client_state_variable = OPC_CLIENT_NOT_INITIALIZED;
	timer_starts_at_epoch = 0;
	local_server = 0;
	g_iOpcProperties = NULL;
	g_iCatInfo = NULL;
	g_pIOPCBrowse = NULL;
	g_pIGroupUnknown = NULL;
	g_pIOPCCommon = NULL;
	g_pIOPCAsyncIO2 = NULL;
	g_pIOPCItemMgt = NULL;
	g_pIOPCSyncIO  = NULL;
	g_pIOPCAsyncIO = NULL ;
	g_pIOPCGroupStateMgt = NULL;
	g_pIDataObject = NULL;
	g_dwCancelID = 1;
	g_dwUpdateTransID = 1;
	g_hClientGroup = 0;
	g_bVer2 = false;
	g_dwNumItems = 0;
	g_dwClientHandle = 1;
	g_dwUpdateRate = 60000; //in milliseconds
	nThreads = 1;
	hServerRead = NULL;
	id_of_ItemToWrite = 0;

	opc_server_prog_id[0] = '\0';
	
	#ifdef USE_RIPC_MIDDLEWARE
	/////////////////////Middleware/////////////////////////////////////////////////////////////////

    factory1 = NULL;
	factory2 = NULL;
	session1 = NULL;
	session2 = NULL;
	//queue_monitor_dir = NULL;
	queue_control_dir = NULL;

	port = 6000;
	hostname = "localhost";

	char fifo_monitor_name[150];
	char fifo_control_name[150];

	strcpy(fifo_monitor_name,"fifo_monitor_direction");
	strcat(fifo_monitor_name, line_number);
	strcat(fifo_monitor_name, "da");
		
	strcpy(fifo_control_name,"fifo_control_direction");
	strcat(fifo_control_name, line_number);
	strcat(fifo_control_name, "da");

	factory1 = RIPCClientFactory::getInstance();
	factory2 = RIPCClientFactory::getInstance();

	session1 = factory1->create(hostname, port);
	session2 = factory2->create(hostname, port);
	queue_monitor_dir = session1->createQueue(fifo_monitor_name);
	queue_control_dir = session2->createQueue(fifo_control_name);

	arg.parent = this;
	///////////fifo//////////////////
	unsigned long threadid;
	
	strcat(fifo_control_name, "_fifo_client");

	#define MAX_FIFO_SIZE 65535
	fifo_control_direction = fifo_open(fifo_control_name, MAX_FIFO_SIZE, iec_call_exit_handler);
		
	CreateThread(NULL, 0, LPTHREAD_START_ROUTINE(control_dir_consumer), (void*)&arg, 0, &threadid);
	///////////////////////////////////Middleware//////////////////////////////////////////////////
	#endif

	/////////////////////Middleware/////////////////////////////////////////////////////////////////
	received_command_callback = 0;

	int32_t                 strength = 1;
	NtpTime                 persistence, deadline, minimumSeparation, delay;
	IPAddress				smIPAddress = IPADDRESS_INVALID;
	ORTEDomainProp          dp;
	ORTEDomainAppEvents     events;
	
	subscriber = NULL;

	ORTEInit();
	ORTEDomainPropDefaultGet(&dp);
	NTPTIME_BUILD(minimumSeparation,0); 
	NTPTIME_BUILD(delay,1); //1s

	//initiate event system
	ORTEDomainInitEvents(&events);

	events.onRegFail = onRegFail;

	//Create application     
	domain = ORTEDomainAppCreate(ORTE_DEFAULT_DOMAIN,&dp,&events,ORTE_FALSE);

	iec_item_type_type_register(domain);

	//Create publisher
	NTPTIME_BUILD(persistence,5);

	char fifo_monitor_name[150];
	strcpy(fifo_monitor_name,"fifo_monitor_direction");
	strcat(fifo_monitor_name, line_number);
	strcat(fifo_monitor_name, "da");

	publisher = ORTEPublicationCreate(
	domain,
	fifo_monitor_name,
	"iec_item_type",
	&instanceSend,
	&persistence,
	strength,
	NULL,
	NULL,
	NULL);

	//if(publisher == NULL){} //check this error
	
	char fifo_control_name[150];
	strcpy(fifo_control_name,"fifo_control_direction");
	strcat(fifo_control_name, line_number);
	strcat(fifo_control_name, "da");

	//Create subscriber
	NTPTIME_BUILD(deadline,3);

	subscriber = ORTESubscriptionCreate(
	domain,
	IMMEDIATE,
	BEST_EFFORTS,
	fifo_control_name,
	"iec_item_type",
	&instanceRecv,
	&deadline,
	&minimumSeparation,
	recvCallBack,
	this,
	smIPAddress);
	///////////////////////////////////Middleware//////////////////////////////////////////////////

	IT_EXIT;
}
		
Opc_client_da_imp::~Opc_client_da_imp()
{
	IT_IT("Opc_client_da_imp::~Opc_client_da_imp");
	stop_opc_thread();
		
	#ifdef USE_RIPC_MIDDLEWARE
	////////Middleware/////////////
	exit_threads = 1;
	fifo_close(fifo_control_direction);
	queue_monitor_dir->close();
	queue_control_dir->close();
	session1->close();
	delete session1;
	session2->close();
	delete session2;
	////////Middleware/////////////
	#endif
	///////////////////////////////////Middleware//////////////////////////////////////////////////
	ORTEDomainAppDestroy(domain);
    domain = NULL;
	///////////////////////////////////Middleware//////////////////////////////////////////////////

	IT_EXIT;
}

int Opc_client_da_imp::Async2Update()
{
	IT_IT("Opc_client_da_imp::Async2Update");

	if(g_pIOPCAsyncIO2 == NULL)
	{
		LogMessage(E_FAIL,"g_pIOPCAsyncIO2 == NULL");
		fprintf(stderr,"Exit al line %d\n", __LINE__);
		fflush(stderr);
		IT_EXIT;
		return 1;
	}

	IConnectionPointContainer *pCPC = NULL;
	IConnectionPoint *pCP = NULL;
	DWORD callbackHandle = 0;
	HRESULT hr = S_OK;

	// create the sink
	CComCOPCCallback *pSink = NULL;
	
	ATLTRY(pSink = new CComCOPCCallback);

	if(pSink == NULL)
	{
		LogMessage(E_OUTOFMEMORY,"new CComCOPCCallback");
		fprintf(stderr,"Exit al line %d\n", __LINE__);
		fflush(stderr);
		IT_EXIT;
		return 1;
	}

	// obtain connection points
	hr = g_pIGroupUnknown->QueryInterface(IID_IConnectionPointContainer, (void**)&pCPC);

	if(FAILED(hr))
	{
		LogMessage(hr, "QueryInterface(IID_IConnectionPointContainer)");
		fprintf(stderr,"Exit al line %d\n", __LINE__);
		fflush(stderr);
		IT_EXIT;
		return 1;
	}

	hr = pCPC->FindConnectionPoint(IID_IOPCDataCallback, &pCP);

	if(FAILED(hr))
	{
		LogMessage(hr, "FindConnectionPoint(IID_IOPCDataCallback)");
		fprintf(stderr,"Exit al line %d\n", __LINE__);
		fflush(stderr);
		IT_EXIT;
		return 1;
	}

	hr = pCP->Advise(pSink, &callbackHandle);

	if(FAILED(hr))
	{
		LogMessage(hr, "Advise()");
		fprintf(stderr,"Exit al line %d\n", __LINE__);
		fflush(stderr);
		IT_EXIT;
		return 1;
	}
	
	//Here only for GENERAL INTERROGATION development
	//g_pIOPCAsyncIO2->SetEnable(FALSE); // turn off update callbacks 
	
	int rc = 0;
	int check_server = 0;

	while(true)
	{
		//check connection every g_dwUpdateRate*10 (about 10 or 30 secondi)
		if((check_server%10) == 0)
		{
			rc = check_connection_to_server();
			fprintf(stderr,"check for server connection...\n");
			fflush(stderr);
		}

		check_server++;

		if(rc)
		{ 
			fprintf(stderr,"Opc_client_da_imp exiting...., due to lack of connection with server\n");
			fflush(stderr);
			IT_COMMENT("Opc_client_da_imp exiting...., due to lack of connection with server");
			
			//Send LOST message to parent (monitor.exe)
			struct iec_item item_to_send;
			struct cp56time2a actual_time;
			get_utc_host_time(&actual_time);

			memset(&item_to_send,0x00, sizeof(struct iec_item));

			item_to_send.iec_obj.ioa = 0;

			item_to_send.cause = 0x03;
			item_to_send.iec_type = C_LO_ST_1;
			item_to_send.iec_obj.o.type30.sp = 0;
			item_to_send.iec_obj.o.type30.time = actual_time;
			item_to_send.iec_obj.o.type30.iv = 0;
			item_to_send.msg_id = n_msg_sent;
			item_to_send.checksum = clearCrc((unsigned char *)&item_to_send, sizeof(struct iec_item));

			//Send in monitor direction
			#ifdef USE_RIPC_MIDDLEWARE
			////////Middleware/////////////
			//publishing data
			queue_monitor_dir->put(&item_to_send, sizeof(struct iec_item));
			////////Middleware/////////////
			#endif
			
			//prepare published data
			memset(&instanceSend,0x00, sizeof(iec_item_type));

			instanceSend.iec_type = item_to_send.iec_type;
			memcpy(&(instanceSend.iec_obj), &(item_to_send.iec_obj), sizeof(struct iec_object));
			instanceSend.cause = item_to_send.cause;
			instanceSend.msg_id = item_to_send.msg_id;
			instanceSend.ioa_control_center = item_to_send.ioa_control_center;
			instanceSend.casdu = item_to_send.casdu;
			instanceSend.is_neg = item_to_send.is_neg;
			instanceSend.checksum = item_to_send.checksum;

			ORTEPublicationSend(publisher);

			break; 
		}

		if(fExit)
		{
			//g_pIOPCAsyncIO2->SetEnable(FALSE); // turn off update callbacks
			IT_COMMENT("Terminate opc loop!");
			break;
		}

		gl_timeout_connection_with_parent++;

		if(gl_timeout_connection_with_parent > 60)
		{
			break; //exit loop for timeout of connection with parent
		}
				
		::Sleep(g_dwSleepInLoop);
	}
	
	// release interfaces
	hr = pCP->Unadvise(callbackHandle);

	if(hr == E_UNEXPECTED)
	{
	  IT_COMMENT("Unexpected Unadvise");
	}
	
	if(FAILED(hr))
	{
		LogMessage(hr, "Unadvise()");
	}
	
	pCP->Release();
	pCPC->Release();
	
	IT_EXIT;
	return 0;
}

int Opc_client_da_imp::OpcStart(char* OpcServerProgID, char* OpcclassId, char* OpcUpdateRate, char* OpcPercentDeadband)
{
	IT_IT("Opc_client_da_imp::OpcStart");
	
	char show_msg[200];
	sprintf(show_msg, " IndigoSCADA OPC DA Client Start\n");
	LogMessage(NULL, show_msg);

	float fTemp = 0.0f;

	if(strlen(OpcUpdateRate) > 0)
	{
		g_dwUpdateRate = atoi(OpcUpdateRate);
	}

	if(strlen(OpcPercentDeadband) > 0)
	{
		dead_band_percent = atof(OpcPercentDeadband);
	}
	
	if((strlen(ServerIPAddress) == 0) || (strcmp("127.0.0.1", ServerIPAddress) == 0))
	{
		local_server = 1;
	}

	strcpy(opc_server_prog_id, OpcServerProgID);
	
	if(local_server)
	{
		//COM connection
		// browse registry for OPC servers
		HKEY hk = HKEY_CLASSES_ROOT;
		TCHAR szKey[MAX_KEYLEN];

		for(int nIndex = 0; ::RegEnumKey(hk, nIndex, szKey, MAX_KEYLEN) == ERROR_SUCCESS; nIndex++)
		{
			HKEY hProgID;
			TCHAR szDummy[MAX_KEYLEN];

			if(::RegOpenKey(hk, szKey, &hProgID) == ERROR_SUCCESS)
			{
				LONG lSize = MAX_KEYLEN;

				if(::RegQueryValue(hProgID, "OPC", szDummy, &lSize) == ERROR_SUCCESS)
				{
					fprintf(stderr,"%s\n",szKey);
					fflush(stderr);
					IT_COMMENT1("%s",szKey);
				}

				::RegCloseKey(hProgID);
			}
		}

		WCHAR wszServerName[100];
		
		HRESULT hr;

		USES_CONVERSION;
		
		wcscpy(wszServerName, T2W(opc_server_prog_id));

		CLSID clsid;
			
		hr = ::CLSIDFromProgID(wszServerName, &clsid );

		if(FAILED(hr))
		{
			LogMessage(hr,"CLSIDFromProgID()");
			IT_EXIT;
			return(1);
		}

		fprintf(stderr,"Server ID found.\n");
		fflush(stderr);
		IT_COMMENT("Server ID found.\n");
		
		hr = ::CoInitializeEx(NULL,COINIT_MULTITHREADED); // setup COM lib

		if(FAILED(hr))
		{
			LogMessage(hr,"CoInitializeEx()");
			IT_EXIT;
			return(1);
		}

		// Create a running object from that class ID
		// (CLSCTX_ALL will allow in-proc, local and remote)

		hr = ::CoCreateInstance(clsid, NULL, CLSCTX_ALL, IID_IOPCServer, (void**)&g_pIOPCServer);

		if(FAILED(hr) || (g_pIOPCServer == NULL))
		{
			if(FAILED(hr)){ LogMessage(hr,"CoCreateInstance()");}
			fprintf(stderr,"You may not have registered the OPC Proxy dll!\n");
			fflush(stderr);
			IT_COMMENT("You may not have registered the OPC Proxy dll!");

			IT_EXIT;
			return(1);
		}
			
		fprintf(stderr,"Connected to server\n");
		fflush(stderr);
		IT_COMMENT("Connected to server");

		WORD wMajor, wMinor, wBuild;

		LPWSTR pwsz = NULL;

		if(!GetStatus(&wMajor, &wMinor, &wBuild, &pwsz))
		{
			fprintf(stderr,"Version: %d.%d.%d\n", wMajor, wMinor, wBuild);
			fflush(stderr);
			fprintf(stderr,"%ls\n\n",pwsz);
			fflush(stderr);
						
			::CoTaskMemFree(pwsz);
		}

		g_bVer2 = Version2();

		if(g_bVer2)
		{
			fprintf(stderr,"Server supports OPC 2.0 interfaces\n\n");
			fflush(stderr);
			IT_COMMENT("Server supports OPC 2.0 interfaces");
		}

		hr = g_pIOPCServer->QueryInterface(IID_IOPCBrowseServerAddressSpace, (void**)&g_pIOPCBrowse);

		if(FAILED(hr))
		{
			LogMessage(hr,"QueryInterface(IID_IOPCBrowseServerAddressSpace)");

			fprintf(stderr,"OPC error:Failed to obtain IID_IOPCBrowseServerAddressSpace interface, %x\n",hr);
			fflush(stderr);
			IT_EXIT;
			return 1;
		}

		hr = g_pIOPCServer->QueryInterface(IID_IOPCItemProperties, (void**)&g_iOpcProperties);

		if (FAILED(hr))
		{
			fprintf(stderr,"OPC error:Failed to obtain IID_IOPCItemProperties interface, %x\n",hr);
			fflush(stderr);

			LogMessage(hr,"QueryInterface(IID_IOPCItemProperties)");
			IT_EXIT;
			return 1;
		}
		
		long lTimeBias = 0;
		DWORD dwRevisedUpdateRate = 0;
		
		hr = g_pIOPCServer->AddGroup(L"",					// [in] Server name, if NULL OPC Server will generate a unique name
									 TRUE		,			// [in] State of group to add
									 g_dwUpdateRate,		// [in] Requested update rate for group (ms)
									 1234,					// [in] Client handle to OPC Group
									 &lTimeBias,			// [in] Time 
									 &fTemp,				// [in] Percent Deadband
									 0,						// [in] Localization ID
									 &g_hClientGroup,		// [out] Server Handle to group
									 &dwRevisedUpdateRate,	// [out] Revised update rate
									 IID_IUnknown,			// [in] Type of interface desired
									 &g_pIGroupUnknown);	// [out] where to store the interface pointer
			
		if(FAILED(hr))
		{
			LogMessage(hr,"AddGroup()");
			g_pIOPCServer->Release();
			IT_EXIT;
			return(1);
		}

		fprintf(stderr,"Group added, update rate = %ld ms\n", dwRevisedUpdateRate);
		fflush(stderr);
		IT_COMMENT1("Group added, update rate = %ld ms.", dwRevisedUpdateRate);

		hr = g_pIGroupUnknown->QueryInterface(IID_IDataObject, (void**)&g_pIDataObject);

		if(FAILED(hr))
		{
			LogMessage(hr,"QueryInterface(IID_IDataObject)");
		}

		hr = g_pIGroupUnknown->QueryInterface(IID_IOPCGroupStateMgt, (void**)&g_pIOPCGroupStateMgt);

		if(FAILED(hr))
		{
			LogMessage(hr,"QueryInterface(IID_IOPCGroupStateMgt)");
		}

		hr = g_pIGroupUnknown->QueryInterface(IID_IOPCAsyncIO, (void**)&g_pIOPCAsyncIO);

		if(FAILED(hr))
		{
			LogMessage(hr,"QueryInterface(IID_IOPCAsyncIO)");
		}

		hr = g_pIGroupUnknown->QueryInterface(IID_IOPCItemMgt, (void**)&g_pIOPCItemMgt);

		if(FAILED(hr))
		{
			LogMessage(hr,"QueryInterface(IID_IOPCItemMgt)");
		}

		hr = g_pIGroupUnknown->QueryInterface(IID_IOPCSyncIO, (void**)&g_pIOPCSyncIO);

		if(FAILED(hr))
		{
			LogMessage(hr,"QueryInterface(IID_IOPCSyncIO)");
		}

		if(g_bVer2)
		{
			hr = g_pIGroupUnknown->QueryInterface(IID_IOPCAsyncIO2, (void**)&g_pIOPCAsyncIO2);
			if(FAILED(hr))
			{
				LogMessage(hr,"QueryInterface(IID_IOPCAsyncIO2)");
			}

			hr = g_pIOPCServer->QueryInterface(IID_IOPCCommon, (void**)&g_pIOPCCommon);
			if(FAILED(hr))
			{
				LogMessage(hr,"QueryInterface(IID_IOPCCommon)");
			}
			else
			{
				g_pIOPCCommon->SetClientName(L"IndigoSCADA OPC DA Client");
			}
		}
		
		if(FAILED(hr))
		{
			g_pIOPCServer->Release();
			//fprintf(stderr,"OPC error: secondary QI failed\n");
			//fflush(stderr);
			IT_COMMENT("OPC error: secondary QI failed");
			LogMessage(hr, "secondary QI failed");
			IT_EXIT;
			return(1);
		}

		if(dwRevisedUpdateRate != g_dwUpdateRate)
		{
			g_dwUpdateRate = dwRevisedUpdateRate;
		}

		fprintf(stderr,"Active Group interface added\n");
		fflush(stderr);
		IT_COMMENT("Active Group interface added");
	}
	else
	{
		//DCOM connection

		fprintf(stderr,"Try to connect to remote server: %s\n", ServerIPAddress);
		fflush(stderr);
		
		HRESULT	hr = ::CoInitializeEx(NULL,COINIT_MULTITHREADED); // setup COM lib

		if(FAILED(hr))
		{
			//fprintf(stderr,"CoInitializeEx failed\n");
			//fflush(stderr);
			LogMessage(hr,"CoInitializeEx()");
			IT_EXIT;
			return 1;
		}

		hr = CoInitializeSecurity(NULL, -1, NULL, NULL, RPC_C_AUTHN_LEVEL_NONE, RPC_C_IMP_LEVEL_IMPERSONATE, NULL, EOAC_NONE, NULL);
		//hr = CoInitializeSecurity(NULL, -1, NULL, NULL, RPC_C_AUTHN_LEVEL_NONE, RPC_C_IMP_LEVEL_ANONYMOUS, NULL, EOAC_NONE, NULL);
				
		if(FAILED(hr))
		{
			//fprintf(stderr,"CoInitializeSecurity failed\n");
			//fflush(stderr);
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
			fprintf(stderr,"Failed to get remote interface, %x\n", hr);
			fflush(stderr);
			LogMessage(hr, "CoCreateInstanceEx()");
			IT_EXIT;
			return 1;
		}
		
		g_iCatInfo = (IOPCServerList *)reqInterface.pItf;

		if(g_iCatInfo == NULL)
		{
			//fprintf(stderr,"g_iCatInfo == NULL\n");
			//fflush(stderr);
			LogMessage(hr, "g_iCatInfo == NULL");
			IT_EXIT;
			return 1;
		}

		CATID Implist[1];

		Implist[0] = IID_CATID_OPCDAServer20;

		IEnumCLSID *iEnum = NULL;

		hr = g_iCatInfo->EnumClassesOfCategories(1, Implist,0, NULL,&iEnum);

		if(FAILED(hr))
		{
			fprintf(stderr,"Failed to get enum for categeories, %x\n", hr);
			fflush(stderr);
			LogMessage(hr, "EnumClassesOfCategories()");
			IT_EXIT;
			return 1;
		}

		GUID glist;

		ULONG actual;

		fprintf(stderr,"Available DA servers on remote server:\n");
		fflush(stderr);

		while((hr = iEnum->Next(1, &glist, &actual)) == S_OK)
		{
			WCHAR *progID;
			WCHAR *userType;
			HRESULT res = g_iCatInfo->GetClassDetails(glist, &progID, &userType);
			//ProgIDFromCLSID(glist, &progID);

			if(FAILED(res))
			{
				fprintf(stderr,"Failed to get ProgId from ClassId, %x\n",res);
				fflush(stderr);
				LogMessage(hr, "GetClassDetails()");
				IT_EXIT;
				return 1;
			}
			else 
			{
				USES_CONVERSION;
				char * str = OLE2T(progID);
				char * str1 = OLE2T(userType);
				fprintf(stderr,"%s\n", str);
				fflush(stderr);
				::CoTaskMemFree(progID);
				::CoTaskMemFree(userType);
			}
		}
				
		if((strlen(OpcServerProgID) == 0))
		{
			//fprintf(stderr,"ProgID not initialized\n");
			//fflush(stderr);
			HRESULT hr;
			hr = GetLastError();
			LogMessage(hr, "ProgID not initialized");

			IT_EXIT;
			return 1;
		}
		
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

			if(strlen(OpcclassId) > 0)
			{
				//If this process is started by a service which runs as Local Account, then you need to set
				//the remote classId string (CLSID)
				
				strcpy(classIdString, OpcclassId);

				USES_CONVERSION;

				LPOLESTR sz = A2W(classIdString);

				hr = CLSIDFromString(sz,&classId);

				if(FAILED(hr))
				{
					//fprintf(stderr,"CLSIDFromString failed\n");
					//fflush(stderr);
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
					//fprintf(stderr,"RegQueryValueEx failed\n");
					//fflush(stderr);
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
						//fprintf(stderr,"CLSIDFromString failed\n");
						//fflush(stderr);
						LogMessage(hr,"CLSIDFromString failed");
						return 1;
					}
			   }
		   }
		   else
		   {
				LogMessage(hr,"RegOpenKeyEx failed");
				return 1;
		   }

		   RegCloseKey(keyHandle);
		   RegCloseKey(remoteRegHandle);
		}	

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

		//fprintf(stderr,"%s\n", OLE2T(remoteServerInfo.pwszName));
		//fflush(stderr);
		
		reqInterface.pIID = &IID_IUnknown; //requested interface
		reqInterface.pItf = NULL;
		reqInterface.hr = S_OK;
		                          //requsted class
		hr = CoCreateInstanceEx(classId,NULL, CLSCTX_REMOTE_SERVER, &remoteServerInfo, 1, &reqInterface);	
		
		if(FAILED(hr))
		{
			fprintf(stderr,"Failed to get remote interface, %x\n", hr);
			fflush(stderr);
			LogMessage(hr, "CoCreateInstanceEx()");
			IT_EXIT;
			return 1;
		}
		
		IUnknown * pIUnknown = NULL;

		pIUnknown = reqInterface.pItf;

		hr = pIUnknown->QueryInterface(IID_IOPCServer, (void**)&g_pIOPCServer);

		if (FAILED(hr))
		{
			fprintf(stderr,"OPC error:Failed obtain IID_IOPCServer interface from server, %x\n", hr);
			fflush(stderr);
			LogMessage(hr, "QueryInterface(IID_IOPCServer)");
			IT_EXIT;
			return 1;
		}

		fprintf(stderr,"Connected to server %s\n", ServerIPAddress);
		fflush(stderr);

		WORD wMajor, wMinor, wBuild;
		LPWSTR pwsz = NULL;

		if(!GetStatus(&wMajor, &wMinor, &wBuild, &pwsz))
		{
			IT_COMMENT4("Version: %d.%d.%d, %ls", wMajor, wMinor, wBuild, pwsz);
						
			fprintf(stderr,"Version: %d.%d.%d\n", wMajor, wMinor, wBuild);
			fflush(stderr);
			fprintf(stderr,"%ls\n\n",pwsz);
			fflush(stderr);
			
			::CoTaskMemFree(pwsz);
		}

		g_bVer2 = Version2();

		if(g_bVer2)
		{
			fprintf(stderr,"Server supports OPC 2.0 interfaces\n\n");
			fflush(stderr);
		}
		
		hr = g_pIOPCServer->QueryInterface(IID_IOPCBrowseServerAddressSpace, (void**)&g_pIOPCBrowse);

		if (FAILED(hr))
		{
			fprintf(stderr,"OPC error:Failed to obtain IID_IOPCBrowseServerAddressSpace interface %x\n",hr);
			fflush(stderr);
			LogMessage(hr, "QueryInterface(IID_IOPCBrowseServerAddressSpace)");
			IT_EXIT;
			return 1;
		}
		
		hr = g_pIOPCServer->QueryInterface(IID_IOPCItemProperties, (void**)&g_iOpcProperties);

		if (FAILED(hr))
		{
			fprintf(stderr,"OPC error:Failed to obtain IID_IOPCItemProperties interface, %x\n",hr);
			fflush(stderr);
			LogMessage(hr, "QueryInterface(IID_IOPCItemProperties)");
			IT_EXIT;
			return 1;
		}

		long lTimeBias = 0;

		DWORD dwRevisedUpdateRate = 0;

		hr = g_pIOPCServer->AddGroup(L"",					// [in] Server name, if NULL OPC Server will generate a unique name
									 TRUE		,			// [in] State of group to add
									 g_dwUpdateRate,		// [in] Requested update rate for group (ms)
									 1234,					// [in] Client handle to OPC Group
									 &lTimeBias,			// [in] Time 
									 &fTemp,				// [in] Percent Deadband
									 0,						// [in] Localization ID
									 &g_hClientGroup,		// [out] Server Handle to group
									 &dwRevisedUpdateRate,	// [out] Revised update rate
									 IID_IUnknown,			// [in] Type of interface desired
									 &g_pIGroupUnknown);	// [out] where to store the interface pointer

		if(FAILED(hr))
		{
			LogMessage(hr,"AddGroup()");
			g_pIOPCServer->Release();
			IT_EXIT;
			return 1;
		}

		if(hr == OPC_S_UNSUPPORTEDRATE)
		{
			//fprintf(stderr,"The server does not support the requested data rate but will use the closest available rate.\n");
			//fflush(stderr);
			IT_COMMENT("The server does not support the requested data rate but will use the closest available rate.");
			LogMessage(hr, "The server does not support the requested data rate but will use the closest available rate");
		}

		fprintf(stderr,"Group added, update rate = %ld ms\n", dwRevisedUpdateRate);
		fflush(stderr);
		IT_COMMENT2("Group added, update rate = %ld ms, percent deadband = %.01f %%\n", dwRevisedUpdateRate, fTemp);
		
		// Get pointer to OPC Server interfaces required

		hr = g_pIGroupUnknown->QueryInterface(IID_IDataObject, (void**)&g_pIDataObject);

		if(FAILED(hr))
		{
			LogMessage(hr,"QueryInterface(IID_IDataObject)");
		}

		hr = g_pIGroupUnknown->QueryInterface(IID_IOPCGroupStateMgt, (void**)&g_pIOPCGroupStateMgt);

		if(FAILED(hr))
		{
			LogMessage(hr,"QueryInterface(IID_IOPCGroupStateMgt)");
			IT_EXIT;
			return 1;
		}

		hr = g_pIGroupUnknown->QueryInterface(IID_IOPCAsyncIO, (void**)&g_pIOPCAsyncIO);

		if(FAILED(hr))
		{
			LogMessage(hr,"QueryInterface(IID_IOPCAsyncIO)");
		}

		hr = g_pIGroupUnknown->QueryInterface(IID_IOPCItemMgt, (void**)&g_pIOPCItemMgt);

		if(FAILED(hr))
		{
			LogMessage(hr,"QueryInterface(IID_IOPCItemMgt)");
			IT_EXIT;
			return 1;
		}

		hr = g_pIGroupUnknown->QueryInterface(IID_IOPCSyncIO, (void**)&g_pIOPCSyncIO);

		if(FAILED(hr))
		{
			LogMessage(hr,"QueryInterface(IID_IOPCSyncIO)");
			IT_EXIT;
			return 1;
		}

		if(g_bVer2)
		{
			hr = g_pIGroupUnknown->QueryInterface(IID_IOPCAsyncIO2, (void**)&g_pIOPCAsyncIO2);

			if(FAILED(hr))
			{
				LogMessage(hr,"QueryInterface(IID_IOPCAsyncIO2)");
				IT_EXIT;
				return 1;
			}

			hr = g_pIOPCServer->QueryInterface(IID_IOPCCommon, (void**)&g_pIOPCCommon);

			if(FAILED(hr))
			{
				LogMessage(hr,"QueryInterface(IID_IOPCCommon)");
				IT_EXIT;
				return 1;
			}
			else
			{
				g_pIOPCCommon->SetClientName(L"IndigoSCADA OPC DA Client");
			}
		}

		if(FAILED(hr))
		{
			g_pIOPCServer->Release();
			//fprintf(stderr,"OPC error: secondary QI failed\n");
			//fflush(stderr);
			LogMessage(hr, "secondary QI failed");
			IT_EXIT;
			return 1;
		}

		if(dwRevisedUpdateRate != g_dwUpdateRate)
		{
			g_dwUpdateRate = dwRevisedUpdateRate;
		}

		fprintf(stderr,"Active Group interface added.\n");
		fflush(stderr);
	}

	IT_EXIT;
    return(0);
}

int Opc_client_da_imp::OpcStop()
{
	IT_IT("Opc_client_da_imp::OpcStop");

	fprintf(stderr,"Entering OpcStop()\n");
	fflush(stderr);
		
	if(Item)
	{
		free(Item);
		Item = NULL;
	}
	
	// terminate server and it will clean up itself
	if(g_pIOPCServer)
	{
		for(int i = g_pIOPCServer->Release();;)
		{
			fprintf(stderr,"refCount = %lu\n", i);
			fflush(stderr);
			IT_COMMENT1("refCount = %lu\n", i);

			if((i = g_pIOPCServer->Release()) == 0)
			{
				fprintf(stderr,"refCount = %lu\n", i);
				fflush(stderr);
				IT_COMMENT1("refCount = %lu\n", i);
				break;
			}
		}
	}

	fprintf(stderr,"Server and all group interfaces terminated.\n");
	fflush(stderr);

	::CoUninitialize();

	char show_msg[200];
	sprintf(show_msg, " IndigoSCADA OPC DA Client End\n");
	LogMessage(NULL, show_msg);

	IT_EXIT;
	return 1;
}

int Opc_client_da_imp::check_connection_to_server(void)
{
	IT_IT("Opc_client_da_imp::check_connection_to_server");

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

int Opc_client_da_imp::GetStatus(WORD *pwMav, WORD *pwMiv, WORD *pwB, LPWSTR *pszV)
{
	IT_IT("Opc_client_da_imp::GetStatus");

	*pwMav = 0;
	*pwMiv = 0;
	*pwB = 0;
	*pszV = NULL;
	OPCSERVERSTATUS *pStatus = NULL;

	if(g_pIOPCServer == NULL)
	{
		IT_EXIT;
		return E_POINTER;
	}

	HRESULT hr = g_pIOPCServer->GetStatus(&pStatus);

	if(FAILED(hr) || (pStatus == NULL) || (pStatus->dwServerState != OPC_STATUS_RUNNING))
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

bool Opc_client_da_imp::Version2()
{
	IT_IT("Opc_client_da_imp::Version2");

	if(g_pIOPCServer == NULL)
	{
		IT_EXIT;
		return false;
	}

	IConnectionPointContainer *pCPC = NULL;

	if(FAILED(g_pIOPCServer->QueryInterface(IID_IConnectionPointContainer, (void**)&pCPC)))
	{
		IT_EXIT;
		return false;
	}

	pCPC->Release();
	IT_EXIT;
	return true;
}

struct log_message{

	int ioa;
	char message[150];
};

void Opc_client_da_imp::LogMessage(HRESULT hr, LPCSTR pszError, const char* name)
{
	//TODO: send message to monitor.exe as a single point

	/*
	struct iec_item item_to_send;
	struct cp56time2a actual_time;
	get_utc_host_time(&actual_time);

	memset(&item_to_send,0x00, sizeof(struct iec_item));

	//item_to_send.iec_obj.ioa =  Find ioa given the message in a vector of log_message

	item_to_send.cause = 0x03;
	item_to_send.iec_type = M_SP_TB_1;
	item_to_send.iec_obj.o.type30.sp = 0;
	item_to_send.iec_obj.o.type30.time = actual_time;
	item_to_send.iec_obj.o.type30.iv = 0;
	*/
}

#include <time.h>
#include <sys/timeb.h>

void Opc_client_da_imp::get_utc_host_time(struct cp56time2a* time)
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
	time->year = ptm->tm_year - 100; //<0.99>
	time->iv = 0; //<0..1> Invalid: <0> is valid, <1> is invalid
	time->su = (u_char)tb.dstflag; //<0..1> SUmmer time: <0> is standard time, <1> is summer time

	IT_EXIT;
    return;
}

time_t Opc_client_da_imp::epoch_from_cp56time2a(const struct cp56time2a* time)
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
	t.tm_mon = time->month - 1;	  //from <1..12> to	<0..11>				
	t.tm_year = time->year + 100; //from <0..99> to <years from 1900>
	t.tm_isdst = time->su;
	
	epoch = mktime(&t);

	if((epoch == -1) || (time->iv == 1))
	{
		epoch = 0;
	}

	return epoch;
}

void Opc_client_da_imp::epoch_to_cp56time2a(cp56time2a *time, signed __int64 epoch_in_millisec)
{
	struct tm	*ptm;
	int ms = (int)(epoch_in_millisec%1000);
	time_t seconds;

	IT_IT("epoch_to_cp56time2a");
	
	memset(time, 0x00,sizeof(cp56time2a));
	seconds = (long)(epoch_in_millisec/1000);
	ptm = localtime(&seconds);
		
    if(ptm)
	{
		time->hour = ptm->tm_hour;					//<0.23>
		time->min = ptm->tm_min;					//<0..59>
		time->msec = ptm->tm_sec*1000 + ms; //<0.. 59999>
		time->mday = ptm->tm_mday; //<1..31>
		time->wday = (ptm->tm_wday == 0) ? ptm->tm_wday + 7 : ptm->tm_wday; //<1..7>
		time->month = ptm->tm_mon + 1; //<1..12>
		time->year = ptm->tm_year - 100; //<0.99>
		time->iv = 0; //<0..1> Invalid: <0> is valid, <1> is invalid
		time->su = (u_char)ptm->tm_isdst; //<0..1> SUmmer time: <0> is standard time, <1> is summer time
	}

	IT_EXIT;
    return;
}

//#define ABS(x) ((x) >= 0 ? (x) : -(x))

void Opc_client_da_imp::SendEvent2(VARIANT *pValue, const FILETIME* ft, DWORD pwQualities, OPCHANDLE phClientItem, unsigned char cot)
{
	IT_IT("Opc_client_da_imp::SendEvent2");

	cp56time2a time;
	signed __int64 epoch_in_millisec;
	struct iec_item item_to_send;
	double delta = 0.0;
	
	USES_CONVERSION;

	IT_COMMENT1("pwQualities = %d", pwQualities);
	IT_COMMENT1("phClientItem = %d", phClientItem);

	if(Item == NULL)
	{
		//print error message
		return;
	}
    	
	memset(&item_to_send,0x00, sizeof(struct iec_item));
		
	item_to_send.iec_obj.ioa = Item[phClientItem - 1].ioa_control_center;

	item_to_send.cause = cot;

	const char* parc = (const char*)W2T(Item[phClientItem - 1].wszName); // togliere - 1 poiche il primo elemento del vettore Item parte da 0
	
	if(parc == NULL)
	{
		//print error message
		return;
	}
		
	//strcpy(item_to_send.opc_server_item_id, parc);

	epoch_in_millisec = epoch_from_FILETIME(ft);
	
	if(!pValue)
	{
		VARIANT Value;
		pValue = &Value;
		V_VT(pValue) = VT_EMPTY;
	}
	
	switch(V_VT(pValue))
	{
		//case VT_EMPTY:
		//{
			//IT_COMMENT1("Value = %d", V_EMPTY(pValue));
		//}
		//break;
		case VT_I1:
		{
			switch(Item[phClientItem - 1].io_list_iec_type)
			{
				case M_IT_TB_1:
				{
					item_to_send.iec_type = M_IT_TB_1;
					epoch_to_cp56time2a(&time, epoch_in_millisec);
					item_to_send.iec_obj.o.type37.counter = V_I1(pValue);
					item_to_send.iec_obj.o.type37.time = time;
						
					if(pwQualities != OPC_QUALITY_GOOD)
						item_to_send.iec_obj.o.type37.iv = 1;

					IT_COMMENT1("Value = %d", V_I1(pValue));
				}
				break;
				case M_ME_TE_1:
				{
					/*
					item_to_send.iec_type = M_ME_TE_1;
					epoch_to_cp56time2a(&time, epoch_in_millisec);
					item_to_send.iec_obj.o.type35.mv = V_I1(pValue);
					item_to_send.iec_obj.o.type35.time = time;

					if(pwQualities != OPC_QUALITY_GOOD)
						item_to_send.iec_obj.o.type35.iv = 1;

					IT_COMMENT1("Value = %d", V_I1(pValue));
					*/
					int error = 0;

					item_to_send.iec_type = M_ME_TE_1;
					epoch_to_cp56time2a(&time, epoch_in_millisec);
					item_to_send.iec_obj.o.type35.mv = rescale_value(V_I1(pValue),
					Item[phClientItem - 1].min_measure, 
					Item[phClientItem - 1].max_measure, &error);
					
					if(pwQualities != OPC_QUALITY_GOOD)
						item_to_send.iec_obj.o.type35.iv = 1;

					IT_COMMENT1("Value = %d", V_I1(pValue));
				}
				break;
				case C_DC_NA_1:
				{
					fprintf(stderr,"IEC type %d is NOT sent in monitoring direction\n", Item[phClientItem - 1].io_list_iec_type);
					fflush(stderr);
				}
				break;
				default:
				{
				  fprintf(stderr,"IEC type %d is NOT supported for VT_I1\n", Item[phClientItem - 1].io_list_iec_type);
				  fflush(stderr);
				}
				break;
			}
		}
		break;
		case VT_UI1:
		{
			switch(Item[phClientItem - 1].io_list_iec_type)
			{
				case M_IT_TB_1:
				{
					item_to_send.iec_type = M_IT_TB_1;
					epoch_to_cp56time2a(&time, epoch_in_millisec);
					item_to_send.iec_obj.o.type37.counter = V_UI1(pValue);
					item_to_send.iec_obj.o.type37.time = time;
						
					if(pwQualities != OPC_QUALITY_GOOD)
						item_to_send.iec_obj.o.type37.iv = 1;

					IT_COMMENT1("Value = %d", V_UI1(pValue));
				}
				break;
				case M_ME_TE_1:
				{
					/*
					item_to_send.iec_type = M_ME_TE_1;
					epoch_to_cp56time2a(&time, epoch_in_millisec);
					item_to_send.iec_obj.o.type35.mv = V_UI1(pValue);
					item_to_send.iec_obj.o.type35.time = time;

					if(pwQualities != OPC_QUALITY_GOOD)
						item_to_send.iec_obj.o.type35.iv = 1;

					IT_COMMENT1("Value = %d", V_UI1(pValue));
					*/
					int error = 0;

					item_to_send.iec_type = M_ME_TE_1;
					epoch_to_cp56time2a(&time, epoch_in_millisec);
					item_to_send.iec_obj.o.type35.mv = rescale_value(V_UI1(pValue),
					Item[phClientItem - 1].min_measure, 
					Item[phClientItem - 1].max_measure, &error);
					
					if(pwQualities != OPC_QUALITY_GOOD)
						item_to_send.iec_obj.o.type35.iv = 1;

					IT_COMMENT1("Value = %d", V_UI1(pValue));
				}
				break;
				case C_DC_NA_1:
				{
					fprintf(stderr,"IEC type %d is NOT sent in monitoring direction\n", Item[phClientItem - 1].io_list_iec_type);
					fflush(stderr);
				}
				break;
				default:
				{
				  fprintf(stderr,"IEC type %d is NOT supported for VT_UI1\n", Item[phClientItem - 1].io_list_iec_type);
				  fflush(stderr);
				}
				break;
			}
		}
		break;
		case VT_I2:
		{
				switch(Item[phClientItem - 1].io_list_iec_type)
				{
					case M_IT_TB_1:
					{
						item_to_send.iec_type = M_IT_TB_1;
						epoch_to_cp56time2a(&time, epoch_in_millisec);
						item_to_send.iec_obj.o.type37.counter = V_I2(pValue);
						item_to_send.iec_obj.o.type37.time = time;
							
						if(pwQualities != OPC_QUALITY_GOOD)
							item_to_send.iec_obj.o.type37.iv = 1;

						IT_COMMENT1("Value = %d", V_I2(pValue));
					}
					break;
					case M_ME_TE_1:
					{
						/*
						item_to_send.iec_type = M_ME_TE_1;
						epoch_to_cp56time2a(&time, epoch_in_millisec);
						item_to_send.iec_obj.o.type35.mv = V_I2(pValue);
						item_to_send.iec_obj.o.type35.time = time;

						if(pwQualities != OPC_QUALITY_GOOD)
							item_to_send.iec_obj.o.type35.iv = 1;

						IT_COMMENT1("Value = %d", V_I2(pValue));
						*/
						int error = 0;

						item_to_send.iec_type = M_ME_TE_1;
						epoch_to_cp56time2a(&time, epoch_in_millisec);
						item_to_send.iec_obj.o.type35.mv = rescale_value(V_I2(pValue),
						Item[phClientItem - 1].min_measure, 
						Item[phClientItem - 1].max_measure, &error);
						
						if(pwQualities != OPC_QUALITY_GOOD)
							item_to_send.iec_obj.o.type35.iv = 1;

						IT_COMMENT1("Value = %d", V_I2(pValue));
					}
					break;
					case C_DC_NA_1:
					{
						fprintf(stderr,"IEC type %d is NOT sent in monitoring direction\n", Item[phClientItem - 1].io_list_iec_type);
						fflush(stderr);
					}
					break;
					default:
					{
					  fprintf(stderr,"IEC type %d is NOT supported for VT_I2\n", Item[phClientItem - 1].io_list_iec_type);
					  fflush(stderr);
					}
					break;
				}
		}
		break;
		case VT_UI2:
		{
				switch(Item[phClientItem - 1].io_list_iec_type)
				{
					case M_IT_TB_1:
					{
						item_to_send.iec_type = M_IT_TB_1;
						epoch_to_cp56time2a(&time, epoch_in_millisec);
						item_to_send.iec_obj.o.type37.counter = V_UI2(pValue);
						item_to_send.iec_obj.o.type37.time = time;
							
						if(pwQualities != OPC_QUALITY_GOOD)
							item_to_send.iec_obj.o.type37.iv = 1;

						IT_COMMENT1("Value = %d", V_UI2(pValue));
					}
					break;
					case M_ME_TE_1:
					{
						/*
						item_to_send.iec_type = M_ME_TE_1;
						epoch_to_cp56time2a(&time, epoch_in_millisec);
						item_to_send.iec_obj.o.type35.mv = V_UI2(pValue);
						item_to_send.iec_obj.o.type35.time = time;
						

						if(pwQualities != OPC_QUALITY_GOOD)
							item_to_send.iec_obj.o.type35.iv = 1;
						

						IT_COMMENT1("Value = %d", V_UI2(pValue));
						*/
						int error = 0;

						item_to_send.iec_type = M_ME_TE_1;
						epoch_to_cp56time2a(&time, epoch_in_millisec);
						item_to_send.iec_obj.o.type35.mv = rescale_value(V_UI2(pValue),
						Item[phClientItem - 1].min_measure, 
						Item[phClientItem - 1].max_measure, &error);
						
						if(pwQualities != OPC_QUALITY_GOOD)
							item_to_send.iec_obj.o.type35.iv = 1;

						IT_COMMENT1("Value = %d", V_UI2(pValue));
					}
					break;
					default:
					{
					  fprintf(stderr,"IEC type %d is NOT supported for VT_UI2\n", Item[phClientItem - 1].io_list_iec_type);
					  fflush(stderr);
					}
					break;
				}
		}
		break;
		case VT_I4:
		{
				switch(Item[phClientItem - 1].io_list_iec_type)
				{
					case M_IT_TB_1:
					{
						item_to_send.iec_type = M_IT_TB_1;
						epoch_to_cp56time2a(&time, epoch_in_millisec);
						item_to_send.iec_obj.o.type37.counter = V_I4(pValue);
						item_to_send.iec_obj.o.type37.time = time;
							
						if(pwQualities != OPC_QUALITY_GOOD)
							item_to_send.iec_obj.o.type37.iv = 1;

						IT_COMMENT1("Value = %d", V_I4(pValue));
					}
					break;
					case M_ME_TE_1:
					{
						int error = 0;

						item_to_send.iec_type = M_ME_TE_1;
						epoch_to_cp56time2a(&time, epoch_in_millisec);
						item_to_send.iec_obj.o.type35.mv = rescale_value(V_I4(pValue),
						Item[phClientItem - 1].min_measure, 
						Item[phClientItem - 1].max_measure, &error);
							
						if(pwQualities != OPC_QUALITY_GOOD)
							item_to_send.iec_obj.o.type35.iv = 1;

						IT_COMMENT1("Value = %d", V_I4(pValue));

						/*
						item_to_send.iec_type = M_ME_TE_1;
						epoch_to_cp56time2a(&time, epoch_in_millisec);
						item_to_send.iec_obj.o.type35.mv = (short)V_I4(pValue);
						item_to_send.iec_obj.o.type35.time = time;

						if(pwQualities != OPC_QUALITY_GOOD)
							item_to_send.iec_obj.o.type35.iv = 1;
						
						IT_COMMENT1("Value = %d", V_I4(pValue));
						*/
					}
					break;
					default:
					{
					  fprintf(stderr,"IEC type %d is NOT supported for VT_I4\n", Item[phClientItem - 1].io_list_iec_type);
					  fflush(stderr);
					}
					break;
				}
		}
		break;
		case VT_UI4:
		{
				switch(Item[phClientItem - 1].io_list_iec_type)
				{
					case M_IT_TB_1:
					{
						item_to_send.iec_type = M_IT_TB_1;
						epoch_to_cp56time2a(&time, epoch_in_millisec);
						item_to_send.iec_obj.o.type37.counter = V_UI4(pValue); //over 2^31 - 1 there is overflow!
						item_to_send.iec_obj.o.type37.time = time;
							
						if(pwQualities != OPC_QUALITY_GOOD)
							item_to_send.iec_obj.o.type37.iv = 1;

						IT_COMMENT1("Value = %d", V_UI4(pValue));
					}
					break;
					case M_ME_TE_1:
					{
						int error = 0;

						item_to_send.iec_type = M_ME_TE_1;
						epoch_to_cp56time2a(&time, epoch_in_millisec);
						item_to_send.iec_obj.o.type35.mv = rescale_value(V_UI4(pValue),
						Item[phClientItem - 1].min_measure, 
						Item[phClientItem - 1].max_measure, &error);
							
						if(pwQualities != OPC_QUALITY_GOOD)
							item_to_send.iec_obj.o.type35.iv = 1;

						IT_COMMENT1("Value = %d", V_UI4(pValue));

						/*
						item_to_send.iec_type = M_ME_TE_1;
						epoch_to_cp56time2a(&time, epoch_in_millisec);
						item_to_send.iec_obj.o.type35.mv = (short)V_UI4(pValue);
						item_to_send.iec_obj.o.type35.time = time;

						if(pwQualities != OPC_QUALITY_GOOD)
							item_to_send.iec_obj.o.type35.iv = 1;
						

						IT_COMMENT1("Value = %d", V_UI4(pValue));
						*/
					}
					break;
					default:
					{
					  fprintf(stderr,"IEC type %d is NOT supported for VT_UI4\n", Item[phClientItem - 1].io_list_iec_type);
					  fflush(stderr);
					}
					break;
				}
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

			/*
			int error = 0;
			item_to_send.iec_type = M_ME_TE_1;
			epoch_to_cp56time2a(&time, epoch_in_millisec);
			item_to_send.iec_obj.o.type35.mv = rescale_value(V_R4(pValue),
			Item[phClientItem - 1].min_measure, 
			Item[phClientItem - 1].max_measure, &error);
				
			if(pwQualities != OPC_QUALITY_GOOD)
				item_to_send.iec_obj.o.type35.iv = 1;

			IT_COMMENT1("Value = %f", V_R4(pValue));
			*/
		}
		break;
		case VT_R8:
		{
			/*
			item_to_send.iec_type = M_ME_TF_1;
			epoch_to_cp56time2a(&time, epoch_in_millisec);
			item_to_send.iec_obj.o.type36.mv = (float)V_R8(pValue);
			item_to_send.iec_obj.o.type36.time = time;

			if(pwQualities != OPC_QUALITY_GOOD)
				item_to_send.iec_obj.o.type36.iv = 1;
			
			IT_COMMENT1("Value = %lf", V_R8(pValue));
			*/

			item_to_send.iec_type = M_ME_TN_1;
			epoch_to_cp56time2a(&time, epoch_in_millisec);
			item_to_send.iec_obj.o.type150.mv = V_R8(pValue);
			item_to_send.iec_obj.o.type150.time = time;

			if(pwQualities != OPC_QUALITY_GOOD)
				item_to_send.iec_obj.o.type150.iv = 1;
			
			IT_COMMENT1("Value = %lf", V_R8(pValue));

	/*
			int error = 0;
			item_to_send.iec_type = M_ME_TE_1;
			epoch_to_cp56time2a(&time, epoch_in_millisec);
			item_to_send.iec_obj.o.type35.mv = rescale_value(V_R8(pValue),
			Item[phClientItem - 1].min_measure, 
			Item[phClientItem - 1].max_measure, &error );
				
			if(pwQualities != OPC_QUALITY_GOOD)
				item_to_send.iec_obj.o.type35.iv = 1;

			IT_COMMENT1("Value = %lf", V_R8(pValue));
	*/
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
			
			//Metti per il momento il time in un punto singolo per prova
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
				//fprintf(stderr,"ItemID = %s: ", (const char*)W2T(Item[phClientItem - 1].wszName));
				//fflush(stderr);

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
				
				item_to_send.iec_type = M_ME_TF_1;
				epoch_to_cp56time2a(&time, epoch_in_millisec);
							
				item_to_send.iec_obj.o.type36.mv = (float)atof(str);
				item_to_send.iec_obj.o.type36.time = time;

				if(pwQualities != OPC_QUALITY_GOOD)
					item_to_send.iec_obj.o.type36.iv = 1;
										
				IT_COMMENT1("Value STRING = %s", str);
		}
		break;
		/*
		case VT_VARIANT:
		{					
			IT_COMMENT("VT_VARIANT non gestito");
		}
		break;
		case VT_ARRAY | VT_I1:
		{	
			IT_COMMENT("VT_ARRAY | VT_I1 non gestito");
		}
		break;
		case VT_ARRAY | VT_UI1:
		{	
			IT_COMMENT("VT_ARRAY | VT_UI1 non gestito");
		}
		break;
		case VT_ARRAY | VT_I2:
		{
			IT_COMMENT("VT_ARRAY | VT_I2 non gestito");
		}
		break;
		case VT_ARRAY | VT_UI2:
		{
			IT_COMMENT("VT_ARRAY | VT_UI2 non gestito");
		}
		break;
		case VT_ARRAY | VT_I4:
		{	
			IT_COMMENT("VT_ARRAY | VT_I4 non gestito");
		}
		break;
		case VT_ARRAY | VT_UI4:
		{
			IT_COMMENT("VT_ARRAY | VT_UI4 non gestito");
		}
		break;
		case VT_ARRAY | VT_I8:
		{
			IT_COMMENT("VT_ARRAY | VT_I8 non gestito");
		}
		break;
		case VT_ARRAY | VT_UI8:
		{
			IT_COMMENT("VT_ARRAY | VT_UI8 non gestito");
		}
		break;
		case VT_ARRAY | VT_R4:
		{
			IT_COMMENT("VT_ARRAY | VT_R4 non gestito");
		}
		break;
		case VT_ARRAY | VT_R8:
		{
			IT_COMMENT("VT_ARRAY | VT_R8 non gestito");
		}
		break;
		case VT_ARRAY | VT_CY:
		{
			IT_COMMENT("VT_ARRAY | VT_CY non gestito");
		}
		break;
		case VT_ARRAY | VT_BOOL:
		{
			IT_COMMENT("VT_ARRAY | VT_BOOL non gestito");
		}
		break;
		case VT_ARRAY | VT_DATE:
		{
			IT_COMMENT("VT_ARRAY | VT_DATE non gestito");
		}
		break;
		case VT_ARRAY | VT_BSTR:
		{
			IT_COMMENT(" VT_ARRAY | VT_BSTR non gestito");
		}
		break;
		case VT_ARRAY | VT_VARIANT:
		{
			IT_COMMENT("VT_ARRAY | VT_VARIANT non gestito");
		}
		break;
		*/
		default:
		{
			IT_COMMENT1("V_VT(pValue) non gestito = %d", V_VT(pValue));
		
			item_to_send.iec_type = 0;
		}
		break;
	}

	//IT_COMMENT6("at time: %d_%d_%d_%d_%d_%d", time.hour, time.min, time.msec, time.mday, time.month, time.year);

	item_to_send.msg_id = n_msg_sent;
	item_to_send.checksum = clearCrc((unsigned char *)&item_to_send, sizeof(struct iec_item));

	//unsigned char buf[sizeof(struct iec_item)];
	//int len = sizeof(struct iec_item);
	//memcpy(buf, &item_to_send, len);
	//	for(j = 0;j < len; j++)
	//	{
	//	  unsigned char c = *(buf + j);
		//fprintf(stderr,"tx ---> 0x%02x\n", c);
		//fflush(stderr);
		//IT_COMMENT1("tx ---> 0x%02x\n", c);
	//	}

	//Send in monitor direction
	fprintf(stderr,"Sending message %u th\n", n_msg_sent);
	fflush(stderr);
	IT_COMMENT1("Sending message %u th\n", n_msg_sent);
	
	#ifdef USE_RIPC_MIDDLEWARE
	////////Middleware/////////////
	//publishing data
	queue_monitor_dir->put(&item_to_send, sizeof(struct iec_item));
	////////Middleware/////////////
	#endif

	Sleep(10); //Without delay there is missing of messages in the loading

	//prepare published data
	memset(&instanceSend,0x00, sizeof(iec_item_type));

	instanceSend.iec_type = item_to_send.iec_type;
	memcpy(&(instanceSend.iec_obj), &(item_to_send.iec_obj), sizeof(struct iec_object));
	instanceSend.cause = item_to_send.cause;
	instanceSend.msg_id = item_to_send.msg_id;
	instanceSend.ioa_control_center = item_to_send.ioa_control_center;
	instanceSend.casdu = item_to_send.casdu;
	instanceSend.is_neg = item_to_send.is_neg;
	instanceSend.checksum = item_to_send.checksum;

	ORTEPublicationSend(publisher);

	n_msg_sent++;

	IT_EXIT;
}

//The FILETIME structure is a 64-bit value representing the number 
//of 100-nanosecond intervals since January 1, 1601.
//
//epoch_in_millisec is a 64-bit value representing the number of milliseconds 
//elapsed since January 1, 1970

signed __int64 Opc_client_da_imp::epoch_from_FILETIME(const FILETIME *fileTime)
{
	IT_IT("epoch_from_FILETIME");
	
	FILETIME localTime;
	struct tm	t;

	time_t sec;
	signed __int64 epoch_in_millisec;

	if(fileTime == NULL)
	{
		IT_EXIT;
		return 0;
	}
	
	// first convert file time (UTC time) to local time
	if (!FileTimeToLocalFileTime(fileTime, &localTime))
	{
		IT_EXIT;
		return 0;
	}

	// then convert that time to system time
	SYSTEMTIME sysTime;
	if (!FileTimeToSystemTime(&localTime, &sysTime))
	{
		IT_EXIT;
		return 0;
	}
	
	memset(&t, 0x00, sizeof(struct tm));
	
	t.tm_hour = sysTime.wHour;
	t.tm_min = sysTime.wMinute;
	t.tm_sec = sysTime.wSecond;
	t.tm_mday = sysTime.wDay;
	t.tm_mon = sysTime.wMonth - 1;
	t.tm_year = sysTime.wYear - 1900; //tm_year contains years after 1900
	t.tm_isdst = -1; //to force mktime to check for dst
	
	sec = mktime(&t);

	if(sec < 0)
	{
		IT_EXIT;
		return 0;
	}

	epoch_in_millisec =  (signed __int64)sec;

	epoch_in_millisec =  epoch_in_millisec*1000 + sysTime.wMilliseconds;

	IT_EXIT;
	return epoch_in_millisec;
}

#define _EPSILON_ ((double)(2.220446E-16))

#define DO_NOT_RESCALE

short Opc_client_da_imp::rescale_value(double V, double Vmin, double Vmax, int* error)
{
	#ifdef DO_SCALE
	double Amin;
	double Amax;
	double r;
	//double V; //Observed value in ingegneristic unit
	double A = 0.0; //Calculate scaled value between Amin = -32768 and Amax = 32767
	double denomin;

	IT_IT("rescale_value");

	*error = 0;

	Amin = -32768.0;
	Amax = 32767.0;

	if(((V - Vmin) > 0.0) && ((V - Vmax) < 0.0))
	{
		denomin = Vmax - Vmin;

		if(denomin > 0.0)
		{
			r = (Amax - Amin)/denomin;
			A = r*(V - Vmin) + Amin;
		}
		else
		{
			*error = 1;
		}
	}
	else if(((V - Vmin) < 0.0))
	{
		A = Amin;
	}
	else if(!fcmp(V, Vmin, _EPSILON_))
	{
		A = Amin;
	}
	else if(((V - Vmax) > 0.0))
	{
		A = Amax;
	}
	else if(!fcmp(V, Vmax, _EPSILON_))
	{
		A = Amax;
	}
	
	IT_COMMENT4("V = %lf, Vmin = %lf, Vmax = %lf, A = %lf", V, Vmin, Vmax, A);

	IT_EXIT;

	return (short)A;

	#endif

	#ifdef DO_NOT_RESCALE

	return (short)V;

	#endif //DO_NOT_RESCALE
}

double Opc_client_da_imp::rescale_value_inv(double A, double Vmin, double Vmax, int* error)
{
	#ifdef DO_SCALE
	double Amin;
	double Amax;
	double r;
	double V; //Calculated value in ingegneristic unit
	//double A = 0.0; //Given a scaled value between Amin = -32768 and Amax = 32767
	double denomin;

	IT_IT("rescale_value_inv");

	*error = 0;

	Amin = -32768.0;
	Amax = 32767.0;

	denomin = Vmax - Vmin;

	if(denomin > 0.0)
	{
		r = (Amax - Amin)/denomin;
		V = Vmin + (A - Amin)/r;
	}
	else
	{
		*error = 1;
	}
		
	IT_COMMENT4("V = %lf, Vmin = %lf, Vmax = %lf, A = %lf", V, Vmin, Vmax, A);

	IT_EXIT;

	return V;

	#endif

	#ifdef DO_NOT_RESCALE

	return A;

	#endif //DO_NOT_RESCALE
}


void Opc_client_da_imp::check_for_commands(struct iec_item *queued_item)
{
	DWORD dw = 0;
	DWORD nWriteItems = ITEM_WRITTEN_AT_A_TIME;
	HRESULT hr = S_OK;
	HRESULT *pErrorsWrite = NULL;
	HRESULT *pErrorsRead = NULL;


	struct cp56time2a actual_time;
	get_utc_host_time(&actual_time);

	time_t command_arrive_time_in_seconds = epoch_from_cp56time2a(&actual_time);

	while(!g_pIOPCAsyncIO2)
	{
		//LogMessage(E_FAIL,"g_pIOPCAsyncIO2 == NULL");
		//fprintf(stderr,"Exit al line %d\n", __LINE__);
		//fflush(stderr);

		Sleep(100);
		
		if(g_pIOPCAsyncIO2)
		{
			break;
		}
		else
		{
			get_utc_host_time(&actual_time);

			time_t attual_time_in_seconds = epoch_from_cp56time2a(&actual_time);

			if(attual_time_in_seconds - command_arrive_time_in_seconds > 10)
			{
				ExitProcess(0);
			}
		}
	}
        
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
			// select from Item table hClient where ioa is equal to ioa of packet arriving (command) from monitor.exe
			///////////////////////////////////////////////////////////////////////////////////////
			int found = 0;
			DWORD hClient = -1;

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
			#ifdef CHECK_TYPE
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
			#endif

			//Receive a write command
								
			fprintf(stderr,"Receiving command for hClient %d, ioa %d\n", hClient, queued_item->iec_obj.ioa);
			fflush(stderr);
			
			//Check the life time of the command/////////////////////////////////////////////////////////////////
			//If life time > MAX_COMMAND_SEND_TIME seconds => DO NOT execute the command

			time_t command_generation_time_in_seconds = 0;

			switch(queued_item->iec_type)
			{
				case C_SC_TA_1:
				case C_SC_NA_1:
				{
					//time contains the UTC time
					command_generation_time_in_seconds = epoch_from_cp56time2a(&(queued_item->iec_obj.o.type58.time));

					fprintf(stderr,"Command generation at UTC time: h:%i m:%i s:%i ms:%i %02i-%02i-%02i, iv %i, su %i\n",
					queued_item->iec_obj.o.type58.time.hour,
					queued_item->iec_obj.o.type58.time.min,
					queued_item->iec_obj.o.type58.time.msec/1000,
					queued_item->iec_obj.o.type58.time.msec%1000,
					queued_item->iec_obj.o.type58.time.mday,
					queued_item->iec_obj.o.type58.time.month,
					queued_item->iec_obj.o.type58.time.year,
					queued_item->iec_obj.o.type58.time.iv,
					queued_item->iec_obj.o.type58.time.su);
					fflush(stderr);
				}
				break;
				case C_DC_TA_1:
				case C_DC_NA_1:
				{
					//time contains the UTC time
					command_generation_time_in_seconds = epoch_from_cp56time2a(&(queued_item->iec_obj.o.type59.time));

					fprintf(stderr,"Command generation at UTC time: h:%i m:%i s:%i ms:%i %02i-%02i-%02i, iv %i, su %i\n",
					queued_item->iec_obj.o.type59.time.hour,
					queued_item->iec_obj.o.type59.time.min,
					queued_item->iec_obj.o.type59.time.msec/1000,
					queued_item->iec_obj.o.type59.time.msec%1000,
					queued_item->iec_obj.o.type59.time.mday,
					queued_item->iec_obj.o.type59.time.month,
					queued_item->iec_obj.o.type59.time.year,
					queued_item->iec_obj.o.type59.time.iv,
					queued_item->iec_obj.o.type59.time.su);
					fflush(stderr);
				}
				break;
				case C_SE_TA_1:
				case C_SE_NA_1:
				{
					//time contains the UTC time
					command_generation_time_in_seconds = epoch_from_cp56time2a(&(queued_item->iec_obj.o.type61.time));

					fprintf(stderr,"Command generation at UTC time: h:%i m:%i s:%i ms:%i %02i-%02i-%02i, iv %i, su %i\n",
					queued_item->iec_obj.o.type61.time.hour,
					queued_item->iec_obj.o.type61.time.min,
					queued_item->iec_obj.o.type61.time.msec/1000,
					queued_item->iec_obj.o.type61.time.msec%1000,
					queued_item->iec_obj.o.type61.time.mday,
					queued_item->iec_obj.o.type61.time.month,
					queued_item->iec_obj.o.type61.time.year,
					queued_item->iec_obj.o.type61.time.iv,
					queued_item->iec_obj.o.type61.time.su);
					fflush(stderr);
				}
				break;
				case C_SE_TB_1:
				case C_SE_NB_1:
				{
					//time contains the UTC time
					command_generation_time_in_seconds = epoch_from_cp56time2a(&(queued_item->iec_obj.o.type62.time));

					fprintf(stderr,"Command generation at UTC time: h:%i m:%i s:%i ms:%i %02i-%02i-%02i, iv %i, su %i\n",
					queued_item->iec_obj.o.type62.time.hour,
					queued_item->iec_obj.o.type62.time.min,
					queued_item->iec_obj.o.type62.time.msec/1000,
					queued_item->iec_obj.o.type62.time.msec%1000,
					queued_item->iec_obj.o.type62.time.mday,
					queued_item->iec_obj.o.type62.time.month,
					queued_item->iec_obj.o.type62.time.year,
					queued_item->iec_obj.o.type62.time.iv,
					queued_item->iec_obj.o.type62.time.su);
					fflush(stderr);
				}
				break;
				case C_SE_TC_1:
				case C_SE_NC_1:
				{
					//time contains the UTC time
					command_generation_time_in_seconds = epoch_from_cp56time2a(&(queued_item->iec_obj.o.type63.time));

					fprintf(stderr,"Command generation at UTC time: h:%i m:%i s:%i ms:%i %02i-%02i-%02i, iv %i, su %i\n",
					queued_item->iec_obj.o.type63.time.hour,
					queued_item->iec_obj.o.type63.time.min,
					queued_item->iec_obj.o.type63.time.msec/1000,
					queued_item->iec_obj.o.type63.time.msec%1000,
					queued_item->iec_obj.o.type63.time.mday,
					queued_item->iec_obj.o.type63.time.month,
					queued_item->iec_obj.o.type63.time.year,
					queued_item->iec_obj.o.type63.time.iv,
					queued_item->iec_obj.o.type63.time.su);
					fflush(stderr);
				}
				break;
				case C_BO_TA_1:
				case C_BO_NA_1:
				{
					//time contains the UTC time
					command_generation_time_in_seconds = epoch_from_cp56time2a(&(queued_item->iec_obj.o.type64.time));

					fprintf(stderr,"Command generation at UTC time: h:%i m:%i s:%i ms:%i %02i-%02i-%02i, iv %i, su %i\n",
					queued_item->iec_obj.o.type64.time.hour,
					queued_item->iec_obj.o.type64.time.min,
					queued_item->iec_obj.o.type64.time.msec/1000,
					queued_item->iec_obj.o.type64.time.msec%1000,
					queued_item->iec_obj.o.type64.time.mday,
					queued_item->iec_obj.o.type64.time.month,
					queued_item->iec_obj.o.type64.time.year,
					queued_item->iec_obj.o.type64.time.iv,
					queued_item->iec_obj.o.type64.time.su);
					fflush(stderr);
				}
				break;
				default:
				{
					//error
					//fprintf(stderr,"Error %d, %s\n",__LINE__, __FILE__);
					//fflush(stderr);

					char show_msg[200];
					sprintf(show_msg, "Error %d, %s\n",__LINE__, __FILE__);
					Opc_client_da_imp::LogMessage(0, show_msg);
				
					return;
				}
				break;
			}

			struct cp56time2a actual_time;
			get_utc_host_time(&actual_time);

			time_t command_execution_time_in_seconds = epoch_from_cp56time2a(&actual_time);

			fprintf(stderr,"Command execution UTC time: h:%i m:%i s:%i ms:%i %02i-%02i-%02i, iv %i, su %i\n",
			actual_time.hour,
			actual_time.min,
			actual_time.msec/1000,
			actual_time.msec%1000,
			actual_time.mday,
			actual_time.month,
			actual_time.year,
			actual_time.iv,
			actual_time.su);
			fflush(stderr);

			time_t delta = command_execution_time_in_seconds  - command_generation_time_in_seconds;

			fprintf(stderr,"Aged delta time= %d\n", delta);
			fflush(stderr);

			if(delta < MAX_COMMAND_SEND_TIME && delta >= 0)
			{
				hServer[id_of_ItemToWrite] = Item[hClient - 1].hServer; //<--the server handle identifies the item to write

				switch(V_VT(&Item[hClient - 1]))
				{
					case VT_BSTR:
					{
						#define COMMAND_STR_LEN 20
						char command_string[COMMAND_STR_LEN];

						double val_to_write = 0.0;
						
						switch(queued_item->iec_type)
						{
							case C_SC_TA_1:
							{
								val_to_write = queued_item->iec_obj.o.type58.scs;
								sprintf(command_string, "%f", val_to_write);
							}
							break;
							case C_DC_TA_1:
							{
								val_to_write = queued_item->iec_obj.o.type59.dcs;
								sprintf(command_string, "%f", val_to_write);
							}
							break;
							case C_SE_TA_1:
							{
								double Vmin = Item[hClient - 1].min_measure;
								double Vmax = Item[hClient - 1].max_measure;
								double A = (double)queued_item->iec_obj.o.type61.sv;
								int error = 0;

								val_to_write = rescale_value_inv(A, Vmin, Vmax, &error);
								if(error){ return;}

								sprintf(command_string, "%lf", val_to_write);
							}
							break;
							case C_SE_TB_1:
							{
								double Vmin = Item[hClient - 1].min_measure;
								double Vmax = Item[hClient - 1].max_measure;
								double A = (double)queued_item->iec_obj.o.type62.sv;
								int error = 0;

								val_to_write = rescale_value_inv(A, Vmin, Vmax, &error);
								if(error){ return;}
								
								sprintf(command_string, "%lf", val_to_write);
							}
							break;
							case C_SE_TC_1:
							{
								val_to_write = queued_item->iec_obj.o.type63.sv;
								sprintf(command_string, "%lf", val_to_write);
							}
							break;
							case C_BO_TA_1:
							{
								memset(command_string, 0x00, COMMAND_STR_LEN);
								memcpy(command_string, &(queued_item->iec_obj.o.type64.stcd), sizeof(struct iec_stcd));
							}
							break;
							case C_SC_NA_1:
							{
								val_to_write = queued_item->iec_obj.o.type45.scs;
								sprintf(command_string, "%lf", val_to_write);
							}
							break;
							case C_DC_NA_1:
							{
								val_to_write = queued_item->iec_obj.o.type46.dcs;
								sprintf(command_string, "%lf", val_to_write);
							}
							break;
							case C_SE_NA_1:
							{
								double Vmin = Item[hClient - 1].min_measure;
								double Vmax = Item[hClient - 1].max_measure;
								double A = (double)queued_item->iec_obj.o.type48.sv;
								int error = 0;

								val_to_write = rescale_value_inv(A, Vmin, Vmax, &error);
								if(error){ return;}
							
								sprintf(command_string, "%lf", val_to_write);
							}
							break;
							case C_SE_NB_1:
							{
								double Vmin = Item[hClient - 1].min_measure;
								double Vmax = Item[hClient - 1].max_measure;
								double A = (double)queued_item->iec_obj.o.type49.sv;
								int error = 0;

								val_to_write = rescale_value_inv(A, Vmin, Vmax, &error);
								if(error){ return;}
								
								sprintf(command_string, "%lf", val_to_write);
							}
							break;
							case C_SE_NC_1:
							{
								val_to_write = queued_item->iec_obj.o.type50.sv;
								sprintf(command_string, "%lf", val_to_write);
							}
							break;
							case C_BO_NA_1:
							{
								memset(command_string, 0x00, COMMAND_STR_LEN);
								memcpy(command_string, &(queued_item->iec_obj.o.type51.stcd), sizeof(struct iec_stcd));
							}
							break;
							default:
							{
								//error
								//fprintf(stderr,"Error %d, %s\n",__LINE__, __FILE__);
								//fflush(stderr);

								char show_msg[200];
								sprintf(show_msg, "Error %d, %s\n",__LINE__, __FILE__);
								Opc_client_da_imp::LogMessage(0, show_msg);


								
								return;
							}
							break;
						}
						
						USES_CONVERSION;

						V_VT(&vCommandValue) = VT_BSTR;

						V_BSTR(&vCommandValue) = SysAllocString(T2COLE(command_string));
						
						if(FAILED(::VariantCopy(&Val[id_of_ItemToWrite], &vCommandValue)))
						{
							//fprintf(stderr,"Error %d, %s\n",__LINE__, __FILE__);
							//fflush(stderr);

							char show_msg[200];
							sprintf(show_msg, "Error %d, %s\n",__LINE__, __FILE__);
							Opc_client_da_imp::LogMessage(0, show_msg);
							
							return;
						}

						//fprintf(stderr,"Command for sample point %s, value: %s\n", Item[hClient - 1].spname, OLE2T(V_BSTR(&vCommandValue)));
						//fflush(stderr);

						char show_msg[450];
						sprintf(show_msg, "Command for sample point %s, value: %s\n", Item[hClient - 1].spname, OLE2T(V_BSTR(&vCommandValue)));
						LogMessage(NULL, show_msg);
						
						IT_COMMENT2("Command for sample point %s, value: %s\n", Item[hClient - 1].spname, OLE2T(V_BSTR(&vCommandValue)));

						SysFreeString(V_BSTR(&vCommandValue));
					}
					break;
					default:
					{
						V_VT(&vCommandValue) = VT_R4;

						unsigned int v = 0;
						double cmd_val = 0.0;

						switch(queued_item->iec_type)
						{
							case C_SC_TA_1:
							{
								v = queued_item->iec_obj.o.type58.scs;
								cmd_val = (double)v;
							}
							break;
							case C_DC_TA_1:
							{
								v = queued_item->iec_obj.o.type59.dcs;
								cmd_val = (double)v;
							}
							break;
							case C_SE_TA_1:
							{
								double Vmin = Item[hClient - 1].min_measure;
								double Vmax = Item[hClient - 1].max_measure;
								double A = (double)queued_item->iec_obj.o.type61.sv;
								int error = 0;

								cmd_val = rescale_value_inv(A, Vmin, Vmax, &error);
								if(error){ return;}
							}
							break;
							case C_SE_TB_1:
							{
								double Vmin = Item[hClient - 1].min_measure;
								double Vmax = Item[hClient - 1].max_measure;
								double A = (double)queued_item->iec_obj.o.type62.sv;
								int error = 0;

								cmd_val = rescale_value_inv(A, Vmin, Vmax, &error);
								if(error){ return;}
							}
							break;
							case C_SE_TC_1:
							{
								cmd_val = queued_item->iec_obj.o.type63.sv;
							}
							break;
							case C_BO_TA_1:
							{
								memcpy(&v, &(queued_item->iec_obj.o.type64.stcd), sizeof(struct iec_stcd));
								cmd_val = (double)v;
							}
							break;
							case C_SC_NA_1:
							{
								v = queued_item->iec_obj.o.type45.scs;
								cmd_val = (double)v;
							}
							break;
							case C_DC_NA_1:
							{
								v = queued_item->iec_obj.o.type46.dcs;
								cmd_val = (double)v;
							}
							break;
							case C_SE_NA_1:
							{
								double Vmin = Item[hClient - 1].min_measure;
								double Vmax = Item[hClient - 1].max_measure;
								double A = (double)queued_item->iec_obj.o.type48.sv;
								int error = 0;

								cmd_val = rescale_value_inv(A, Vmin, Vmax, &error);
								if(error){ return;}
							}
							break;
							case C_SE_NB_1:
							{
								double Vmin = Item[hClient - 1].min_measure;
								double Vmax = Item[hClient - 1].max_measure;
								double A = (double)queued_item->iec_obj.o.type49.sv;
								int error = 0;

								cmd_val = rescale_value_inv(A, Vmin, Vmax, &error);
								if(error){ return;}
							}
							break;
							case C_SE_NC_1:
							{
								cmd_val = queued_item->iec_obj.o.type50.sv;
							}
							break;
							case C_BO_NA_1:
							{
								memcpy(&v, &(queued_item->iec_obj.o.type51.stcd), sizeof(struct iec_stcd));
								cmd_val = (double)v;
							}
							break;
							default:
							{
								//error
								//fprintf(stderr,"Error %d, %s\n",__LINE__, __FILE__);
								//fflush(stderr);

								char show_msg[200];
								sprintf(show_msg, "Error %d, %s\n",__LINE__, __FILE__);
								Opc_client_da_imp::LogMessage(0, show_msg);
								
								return;
							}
							break;
						}
						
						V_R4(&vCommandValue) = (float)cmd_val;

						if (FAILED(::VariantCopy(&Val[id_of_ItemToWrite], &vCommandValue)))
						{
							//fprintf(stderr,"Error %d, %s\n",__LINE__, __FILE__);
							//fflush(stderr);

							char show_msg[200];
							sprintf(show_msg, "Error %d, %s\n",__LINE__, __FILE__);
							Opc_client_da_imp::LogMessage(0, show_msg);
						
							return;
						}

						//fprintf(stderr,"Command for sample point %s, value: %lf\n", Item[hClient - 1].spname, cmd_val);
						//fflush(stderr);

						char show_msg[450];
						sprintf(show_msg, " Command for sample point %s, value: %lf\n", Item[hClient - 1].spname, cmd_val);
						LogMessage(NULL, show_msg);

						//IT_COMMENT2("Command for sample point %s, value: %lf", Item[hClient - 1].spname, cmd_val);
					}
					break;
				}

				if(FAILED(::VariantChangeType(&Val[id_of_ItemToWrite], &Val[id_of_ItemToWrite], 0, V_VT(&Item[hClient - 1]))))
				{
					//fprintf(stderr,"Error %d, %s\n",__LINE__, __FILE__);
					//fflush(stderr);

					char show_msg[200];
					sprintf(show_msg, "Error %d, %s\n",__LINE__, __FILE__);
					Opc_client_da_imp::LogMessage(0, show_msg);

					return;
				}
								
				DWORD dwAccessRights = Item[hClient - 1].dwAccessRights;

				dwAccessRights = dwAccessRights & OPC_WRITEABLE;

				if(dwAccessRights == OPC_WRITEABLE)
				{
					//Opc_client_da_imp::g_bWriteComplete = false;

					hr = g_pIOPCAsyncIO2->Write(nWriteItems, hServer, Val, ++g_dwWriteTransID, &g_dwCancelID, &pErrorsWrite);

					if(FAILED(hr))
					{
						LogMessage(hr,"AsyncIO2->Write()");

						return;
					}
					else if(hr == S_FALSE)
					{
						for(dw = 0; dw < nWriteItems; dw++)
						{
							if(FAILED(pErrorsWrite[dw]))
							{
								LogMessage(pErrorsWrite[dw],"AsyncIO2->Write() item returned");

								return;
							}
						}

						::CoTaskMemFree(pErrorsWrite);
					}
					else // S_OK
					{
						::CoTaskMemFree(pErrorsWrite);
					}

					if(V_VT(&Val[id_of_ItemToWrite]) == VT_BSTR)
					{
						SysFreeString(V_BSTR(&Val[id_of_ItemToWrite]));
					}
				}
				else
				{
					IT_COMMENT1("No access write for sample point %s", Item[hClient - 1].spname);
					//fprintf(stderr,"No access write for sample point %s\n", Item[hClient - 1].spname);
					//fflush(stderr);
					
					char show_msg[200];
					sprintf(show_msg, "No access write for sample point %s\n", Item[hClient - 1].spname);
					Opc_client_da_imp::LogMessage(0, show_msg);
					
					return;
				}
			}
			else
			{
				IT_COMMENT3("Rejeced command for sample point %s, aged for %ld s; max aging time %d s\n", Item[hClient - 1].spname, delta, MAX_COMMAND_SEND_TIME);
				//fprintf(stderr,"Rejeced command for sample point %s, aged for %ld s; max aging time %d s\n", Item[hClient - 1].spname, delta, MAX_COMMAND_SEND_TIME);
				//fflush(stderr);

				char show_msg[200];
				sprintf(show_msg, "Rejeced command for sample point %s, aged for %ld s; max aging time %d s\n", Item[hClient - 1].spname, delta, MAX_COMMAND_SEND_TIME);
				Opc_client_da_imp::LogMessage(0, show_msg);
			
				return;
			}
		}
		else if(queued_item->iec_type == C_EX_IT_1)
		{
			//Receiving EXIT process command from monitor.exe
			//exit the thread, and stop the process
			fExit = true;
		}
		else if(queued_item->iec_type == C_IC_NA_1)
		{
			//Receiving general interrogation command from monitor.exe
			IT_COMMENT("Receiving general interrogation command from monitor.exe");
			fprintf(stderr,"Receiving general interrogation command from monitor.exe\n");
			fflush(stderr);

			//Check if resources are allocated

			if(hServerRead == NULL)
				alloc_command_resources();

			for(dw = 0; dw < g_dwNumItems; dw++)
			{
				hServerRead[dw] = Item[dw].hServer;
			}
			
			// read all items in group

			hr = g_pIOPCAsyncIO2->Read(g_dwNumItems, hServerRead, ++g_dwReadTransID, &g_dwCancelID, &pErrorsRead);

			if(FAILED(hr))
			{
				LogMessage(hr,"AsyncIO2->Read()");
				//When this happen the read is no more working,
				//this means that the General Interrogation is no more working
				//The asyncronous events could still arriving form the server
				//So we exit the process
				fExit = 1;
			}
			else if(hr == S_FALSE)
			{
				//If we arrive here there is something wrong in AddItems()
				for(dw = 0; dw < g_dwNumItems; dw++)
				{
					if(FAILED(pErrorsRead[dw]))
					{
						LogMessage(pErrorsRead[dw],"AsyncIO2->Read() item returned");
					}
				}

				::CoTaskMemFree(pErrorsRead);

				//So we exit the process
				fExit = 1;
			}
			else // S_OK
			{
				::CoTaskMemFree(pErrorsRead);
			}
			/////////end General interrogation command
		}
	}

	return;
}

void Opc_client_da_imp::alloc_command_resources(void)
{
	hServerRead = (OPCHANDLE*)calloc(1, g_dwNumItems*sizeof(OPCHANDLE));
		
	DWORD dw = 0;
	DWORD nWriteItems = ITEM_WRITTEN_AT_A_TIME;

	::VariantInit(&vCommandValue);

	for(dw = 0; dw < nWriteItems; dw++)
	{
		id_of_ItemToWrite = dw;
		::VariantInit(&Val[dw]);
	}
}

void Opc_client_da_imp::free_command_resources(void)
{
	DWORD dw = 0;
	DWORD nWriteItems = ITEM_WRITTEN_AT_A_TIME;

	for(dw = 0; dw < nWriteItems; dw++)
	{
		::VariantClear(&Val[dw]);
	}
	
	::VariantClear(&vCommandValue);
}

#include <signal.h>

static char* get_date_time()
{
	static char sz[128];
	time_t t = time(NULL);
	struct tm *ptm = localtime(&t);
	
	strftime(sz, sizeof(sz)-2, "%m/%d/%y %H:%M:%S", ptm);

	strcat(sz, "|");
	return sz;
}

static void iec_call_exit_handler(int line, char* file, char* reason)
{
	FILE* fp;
	char program_path[_MAX_PATH];
	char log_file[_MAX_FNAME+_MAX_PATH];
	IT_IT("iec_call_exit_handler");

	program_path[0] = '\0';
#ifdef WIN32
	if(GetModuleFileName(NULL, program_path, _MAX_PATH))
	{
		*(strrchr(program_path, '\\')) = '\0';        // Strip \\filename.exe off path
		*(strrchr(program_path, '\\')) = '\0';        // Strip \\bin off path
    }
#elif __unix__
	if(getcwd(program_path, _MAX_PATH))
	{
		*(strrchr(program_path, '/')) = '\0';        // Strip \\filename.exe off path
		*(strrchr(program_path, '/')) = '\0';        // Strip \\bin off path
    }
#endif

	strcpy(log_file, program_path);

#ifdef WIN32
	strcat(log_file, "\\logs\\modbus.log");
#elif __unix__
	strcat(log_file, "/logs/modbus.log");	
#endif

	fp = fopen(log_file, "a");

	if(fp)
	{
		if(line && file && reason)
		{
			fprintf(fp, "PID:%d time:%s exit process at line: %d, file %s, reason:%s\n", GetCurrentProcessId, get_date_time(), line, file, reason);
		}
		else if(line && file)
		{
			fprintf(fp, "PID:%d time:%s exit process at line: %d, file %s\n", GetCurrentProcessId, get_date_time(), line, file);
		}
		else if(reason)
		{
			fprintf(fp, "PID:%d time:%s exit process for reason %s\n", GetCurrentProcessId, get_date_time(), reason);
		}

		fflush(fp);
		fclose(fp);
	}

	//raise(SIGABRT);   //raise abort signal which in turn starts automatically a separete thread and call exit SignalHandler
	ExitProcess(0);

	IT_EXIT;
}
