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
#ifndef OPC_CLIENT_AE
#define OPC_CLIENT_AE

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
#include "opc_ae.h"	/* The OPC custom interface defintions */
#include "opcaedef.h"
#include "opccomn.h"
#include "OpcEnum.h"
#include "opcerror.h"
#include "itrace.h"

#define SOURCE_LENGHT 100  //Warning: 100 may be small
#define AREA_LENGHT 100		//Warning: 100 may be small
#define EVENT_DESC_LENGHT 250	//Warning: 250 may be small

struct structEvent
{
	DWORD Number;
	CHAR Source[SOURCE_LENGHT];
	CHAR Area[AREA_LENGHT]; 
	CHAR Event_decription[EVENT_DESC_LENGHT]; 
	unsigned int ioa_control_center;  //unstructured
};

enum watch_dog_blocked_processes_states {
	INITIAL_STATE = 0,
	TRYING_TO_CONNECT_TO_SERVER,
	CONNECTION_ENSTABLISHED
};

enum opc_client_states {
	OPC_CLIENT_NOT_INITIALIZED = 0,
	OPC_CLIENT_INITIALIZED,
	OPC_CLIENT_WAITING_FOR_FIRST_EVENT,
	OPC_CLIENT_ON_LINE
};

////////////////////////////Middleware/////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////

class Opc_client_ae_imp
{
	public:

	Opc_client_ae_imp(char* opc_server_address, char* line_number)
	{ 
		IT_IT("Opc_client_ae_imp::Opc_client_ae_imp");

		g_dwUpdateRate = 1000; //milliseconds
		g_hClientGroup = 0;
		g_dwUpdateTransID = 1;
		g_dwCancelID = 1;
		g_pIOPCCommon = NULL;
		g_pIGroupUnknown = NULL;
		g_pIOPCSubscriptionMgt = NULL;
		g_iCatInfo = NULL;
		g_iOpcAreaBrowser = NULL;
		g_bWriteComplete = true;
		g_pIOPCCommon = NULL;
		m_ISubMgt = NULL;
		m_ISubMgt2 = NULL;
		m_dwCookie = 0xCDCDCDCD;
		m_dwShutdownCookie = 0xCDCDCDCD;
		fExit = false;
		g_dwUpdateRate = 1000; //in milliseconds
		g_iCatInfo = NULL;
		state_of_watch_dog_blocked_processes = INITIAL_STATE;
		timer_starts_at_epoch = 0;
		g_pIOPCServer = NULL;

		strcpy(ServerIPAddress, opc_server_address);

		opc_server_prog_id[0] = '\0';

		/////////////////////Middleware/////////////////////////////////////////////////////////////////
		char fifo_monitor_name[150];
		strcpy(fifo_monitor_name,"fifo_monitor_direction");
		strcat(fifo_monitor_name, line_number);
		strcat(fifo_monitor_name, "ae");
		
		char fifo_control_name[150];
		strcpy(fifo_control_name,"fifo_control_direction");
		strcat(fifo_control_name, line_number);
		strcat(fifo_control_name, "ae");

		///////////////////////////////////Middleware//////////////////////////////////////////////////

		IT_EXIT;
	};

	~Opc_client_ae_imp()
	{
		IT_IT("Opc_client_ae_imp::~Opc_client_ae_imp");
		stop_opc_thread();
		Sleep(1000);
		IT_EXIT;
	}

	 DWORD g_dwUpdateRate;
	 static DWORD g_dwNumAlarmsEvents;
	 bool  g_bWriteComplete;
	 DWORD g_dwUpdateTransID;
	 DWORD g_dwCancelID;
	 OPCHANDLE g_hClientGroup;
	 IOPCEventServer *g_pIOPCServer;
	 bool fExit;
	 int state_of_watch_dog_blocked_processes;
	 char ServerIPAddress[80];
	 IOPCCommon *g_pIOPCCommon;
	 IUnknown *g_pIGroupUnknown;
	 IOPCEventSubscriptionMgt *g_pIOPCSubscriptionMgt;
	 IOPCServerList *g_iCatInfo;
	 IOPCEventAreaBrowser *g_iOpcAreaBrowser;
	 static struct structEvent* Config_db;
	 DWORD m_dwCookie, m_dwShutdownCookie;
	 IOPCEventSubscriptionMgt* m_ISubMgt;
	 IOPCEventSubscriptionMgt2* m_ISubMgt2;
	 char opc_server_prog_id[100];

 	 /////////////////////Middleware/////////////////////////////////////////////////////////////////
	 ///////////////////////////////////Middleware//////////////////////////////////////////////////

	 int OpcStart(char* OpcServerProgID, char* OpcclassId, char* OpcUpdateRate);
	 int OpcStop();
	 int GetStatus(WORD *pwMav, WORD *pwMiv, WORD *pwB, LPWSTR *pswzV);
	 int Update();
	 static void LogMessage(HRESULT hr = S_OK, LPCSTR pszError = NULL, const char* name = NULL);
	 static void SendEvent2(ONEVENTSTRUCT* pEvent);
	 static signed __int64 epoch_from_FILETIME(const FILETIME *fileTime);
	 int check_connection_to_server(void);
	 int load_database(void);
	 void start_timer_for_blocked_processes(void);
	 void get_utc_host_time(struct cp56time2a* time);
	 static void epoch_to_cp56time2a(cp56time2a *time, signed __int64 epoch_in_millisec);
	 time_t epoch_from_cp56time2a(const struct cp56time2a* time);
	 time_t timer_starts_at_epoch;
	 ////////////////////Middleware////////////////////////////////////////////////////
	 void check_for_commands(struct iec_item *item);
	 void alloc_command_resources(void);
	 void free_command_resources(void);
	 ////////////////////Middleware////////////////////////////////////////////////////

	 void stop_opc_thread()
	 {
		fExit = true;
	 };
};

#endif //OPC_CLIENT_AE