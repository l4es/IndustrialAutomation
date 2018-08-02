/*
 *                         IndigoSCADA
 *
 *   This software and documentation are Copyright 2002 to 2009 Enscada 
 *   Limited and its licensees. All rights reserved. See file:
 *
 *                     $HOME/LICENSE 
 *
 *   for full copyright notice and license terms. 
 *
 */

#ifndef OPC_CLIENT_HDA
#define OPC_CLIENT_HDA

#ifndef STRICT
#define STRICT
#endif
#define VC_EXTRALEAN

#ifndef _WIN32_DCOM
 #define _WIN32_DCOM
#endif
#define _ATL_FREE_THREADED

#include <stdio.h>
#include <time.h>
#include <windows.h>
#include <conio.h>
#include <atlbase.h>
#include "opcda.h"	/* The OPC custom interface defintions */
#include "opchda.h"
#include "opccomn.h"
#include "itrace.h"

#define SOURCE_LENGHT 100  //Warning: 100 may be small
#define AREA_LENGHT 100		//Warning: 100 may be small
#define EVENT_DESC_LENGHT 250	//Warning: 250 may be small

struct structHdaItem
{
	DWORD haAggregate;
    DWORD dwCount;
	unsigned int ioa_control_center;//unique inside CASDU
	unsigned int io_list_iec_type; //IEC 104 type 
};


enum opc_client_states {
	OPC_CLIENT_NOT_INITIALIZED = 0,
	OPC_CLIENT_INITIALIZED,
	OPC_CLIENT_WAITING_FOR_FIRST_EVENT,
	OPC_CLIENT_ON_LINE
};

class Opc_client_hda_imp
{
	public:

	Opc_client_hda_imp(char* opc_server_address)
	{ 
		IT_IT("Opc_client_hda_imp::Opc_client_hda_imp");

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
		g_pIOPCHDA_Browser = NULL;
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
		g_dwNumAlarmsEvents = 0;
		g_dwClientHandle = 1;
		g_dwUpdateRate = 60000; //in milliseconds
		nThreads = 1;

		opc_server_prog_id[0] = '\0';

		IT_EXIT;
	};
		
	~Opc_client_hda_imp()
	{
		IT_IT("Opc_client_hda_imp::~Opc_client_hda_imp");
		stop_opc_thread();
		Sleep(1000);
		IT_EXIT;
	}

	 DWORD g_dwUpdateRate;
	 DWORD g_dwClientHandle;
	 DWORD g_dwNumAlarmsEvents;
	 bool  g_bWriteComplete;
	 bool  g_bVer2; // version 2.0 flag
	 DWORD g_dwUpdateTransID;
	 DWORD g_dwCancelID;
	 DWORD g_dwReadTransID;
	 OPCHANDLE g_hClientGroup;
	 static IOPCHDA_Server *g_pIOPCHDAServer;
	 IOPCHDA_Browser *g_pIOPCHDA_Browser;

	 int nThreads;

	 static bool fExit;
	 int opc_client_state_variable;
	 time_t timer_starts_at_epoch;
	 static double dead_band_percent;
	 char ServerIPAddress[80];

	// group interfaces
	 IDataObject *g_pIDataObject;
	 IOPCGroupStateMgt *g_pIOPCGroupStateMgt;
	 IOPCAsyncIO *g_pIOPCAsyncIO;
	 IOPCSyncIO *g_pIOPCSyncIO;
	 IOPCItemMgt *g_pIOPCItemMgt;
	 IOPCAsyncIO2 *g_pIOPCAsyncIO2;
	 IOPCCommon *g_pIOPCCommon;
	 
	 IUnknown *g_pIGroupUnknown;
	 IOPCBrowseServerAddressSpace *g_pIOPCBrowse;
	 IOPCServerList *g_iCatInfo;
	 IOPCItemProperties *g_iOpcProperties;

		char opc_server_prog_id[100];
	 
	 struct structHdaItem* Config_db;
	 int local_server;

	 int OpcStart(char* OpcServerProgID, char* OpcclassId, char* OpcUpdateRate, char* OpcPercentDeadband);
	 int OpcStop();
	 int GetStatus(WORD *pwMav, WORD *pwMiv, WORD *pwB, LPWSTR *pswzV);
	 int load_database(void);
	 static void LogMessage(HRESULT hr = S_OK, LPCSTR pszError = NULL, const char* name = NULL);
	 bool Version2();
	 int Update();
	 static void SendEvent2(VARIANT *pValue, const FILETIME* ft, DWORD pwQualities, OPCHANDLE phClientItem, unsigned char cot);
	 static signed __int64 epoch_from_FILETIME(const FILETIME *fileTime);
	 void get_utc_host_time(struct cp56time2a* time);
	 time_t epoch_from_cp56time2a(const struct cp56time2a* time);
	 static void epoch_to_cp56time2a(cp56time2a *time, signed __int64 epoch_in_millisec);
	 int check_connection_to_server(void);
	
	void stop_opc_thread()
	{
		fExit = true;
	};
};

#endif //OPC_CLIENT_HDA