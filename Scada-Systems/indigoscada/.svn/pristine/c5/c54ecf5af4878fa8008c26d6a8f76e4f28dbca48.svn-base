/*
 *                         IndigoSCADA
 *
 *   This software and documentation are Copyright 2002 to 2013 Enscada 
 *   Limited and its licensees. All rights reserved. See file:
 *
 *                     $HOME/LICENSE 
 *
 *   for full copyright notice and license terms. 
 *
 */
#include <time.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <winsock2.h>
#include "station.hpp"
#include "master.hpp"
#include "datalink.hpp"
#include "custom.hpp"
#include "clear_crc_eight.h"
#include "iec104types.h"
#include "iec_item.h"
#include "process.h"
#include "dnp3_master_app.h"

extern int gl_timeout_connection_with_parent;

////////////////////////////Middleware///////////////////////////////////////////////////////
////////////////////////////////Middleware/////////////////////////////////////



//   
//  Class constructor.   
//   
DNP3MasterApp::DNP3MasterApp(
		char* dnp3server_address, 
		char*dnp3server_port, 
		char* line_number,
		int server_id,
		int polling_time,
		int nIOA_AO,
		int nIOA_BO,
		int nIOA_CI,
		int nIOA_BI,
		int nIOA_AI):
Connected(false), tx_var(NULL), master_p(NULL), 
fExit(false),pollingTime(polling_time), serverID(server_id), 
lineNumber(atoi(line_number))
{   
	strcpy(dnp3ServerAddress, dnp3server_address);
	strcpy(dnp3ServerPort, dnp3server_port);

	db.nIOA_AI = nIOA_AI;
	db.nIOA_AO = nIOA_AO;
	db.nIOA_BI = nIOA_BI;
	db.nIOA_BO = nIOA_BO;
	db.nIOA_CI = nIOA_CI;
	
	received_command = -1;

	/////////////////////Middleware/////////////////////////////////////////////////////////////////
	char fifo_monitor_name[150];
	strcpy(fifo_monitor_name,"fifo_monitor_direction");
	strcat(fifo_monitor_name, line_number);
	strcat(fifo_monitor_name, "dnp3");
	
	char fifo_control_name[150];
	strcpy(fifo_control_name,"fifo_control_direction");
	strcat(fifo_control_name, line_number);
	strcat(fifo_control_name, "dnp3");
	///////////////////////////////////Middleware//////////////////////////////////////////////////

	if(OpenLink(dnp3ServerAddress, atoi(dnp3ServerPort)))
	{
		Connected = false;
	}
	else
	{
		Connected = true;

		debugLevel = 1;
		integrityPollInterval = 10;
		masterConfig.addr = lineNumber;
		masterConfig.consecutiveTimeoutsForCommsFail = 3;
		masterConfig.integrityPollInterval_p = &integrityPollInterval;
		masterConfig.debugLevel_p = &debugLevel;

		stationConfig.addr = serverID;
		stationConfig.debugLevel_p = &debugLevel;

		datalinkConfig.addr                  = masterConfig.addr;
		datalinkConfig.isMaster              = 1;
		datalinkConfig.keepAliveInterval_ms  = 10000;

		tx_var = new CustomInter(&debugLevel, 'M', 'S', getSocket());

		datalinkConfig.tx_p                  = tx_var;
		datalinkConfig.debugLevel_p          = &debugLevel;

		master_p = new Master(masterConfig, datalinkConfig, &stationConfig, 1, &db, &timer);
	}
}   
//   
//  Class destructor.   
//   
DNP3MasterApp::~DNP3MasterApp()   
{   
    // free resources   
	if(tx_var)
	{
		delete tx_var;
	}

	if(master_p)
	{
		delete master_p;
	}

    CloseLink();
	
    return;   
}   

bool DNP3MasterApp::GetSockConnectStatus(void)
{
	return Connected;
}

//   
//  Open TCP/IP connection.   
//   
int DNP3MasterApp::OpenLink(char *serverIP, int port)   
{   
    WORD wVersionRequested;   
    int wsaerr;   
   
    // connected ?   
    if(GetSockConnectStatus())
    {   
		// report warning   
		sprintf(LastError, "Socket is connected!\n");   
		fprintf(stderr, "%s\n", LastError);
		fflush(stderr);

		return EIO;   
    }   
    else   
    {   
		// Using MAKEWORD macro, Winsock version request 2.2   
		wVersionRequested = MAKEWORD(2, 2);   
		wsaerr = WSAStartup(wVersionRequested, &wsaData);   
		if(wsaerr != 0)   
		{   
			/* Tell the user that we could not find a usable */   
			/* WinSock DLL.*/   
			sprintf(LastError, "The Winsock dll not found!\n");
			fprintf(stderr, "%s\n", LastError);
			fflush(stderr);

			return ENOENT;   
		}   
	//#ifdef _DEBUG   
	//	sprintf(LastError, "The Winsock dll found! - The status: %s.\n", wsaData.szSystemStatus);
	//	fprintf(stderr, "%s\n", LastError);
	//	fflush(stderr);
    //#endif   
		sprintf(this->RemoteHost, serverIP);

		this->RemotePort = port;
		
		//fprintf(stderr, "remote port %d\n", this->RemotePort);
		//fflush(stderr);

		// init socket   
		// Create the socket as an IPv4 (AF_INET)
		Socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);   
		if(Socket == INVALID_SOCKET)   
		{   
			//Output the error recieved when creating the socket.   
			sprintf(LastError, "Socket error: %ld\n", WSAGetLastError());
			fprintf(stderr, "%s\n", LastError);
			fflush(stderr);
        
			WSACleanup();   
			Socket = 0;   
        
			return EIO;   
		}   

		//Assign the remote info to our socket address structure.   
		RemoteInfo.sin_family = AF_INET;   
		RemoteInfo.sin_addr.s_addr = inet_addr(this->RemoteHost);   
		RemoteInfo.sin_port = htons((SHORT)this->RemotePort);   
       
		//Time to connect to the remote computer   
		if(connect(Socket, (SOCKADDR*)&RemoteInfo, sizeof(RemoteInfo)) == SOCKET_ERROR)   
		{   
			sprintf(LastError, "Socket error: Unable to establish a connection to %s:%i!\n", RemoteHost, RemotePort);
			fprintf(stderr, "%s\n", LastError);
			fflush(stderr);
        
			closesocket(Socket);   
			WSACleanup();   
			Socket = 0;   
			return EIO;   
		}   
    }
	
    this->Connected=true;   
    return EOK;   
}   
   
//   
//  Close TCP/IP connection   
//   
int DNP3MasterApp::CloseLink(bool free)   
{   
    Connected = false;   
    shutdown(Socket, SD_BOTH);   
    closesocket(Socket);   
    WSACleanup();   
    return EOK;   
}

void DNP3MasterApp::check_for_commands(struct iec_item *queued_item)
{
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

			//Command execution
			received_command = queued_item->iec_type;
		}
		else if(queued_item->iec_type == C_EX_IT_1)
		{
			//Receiving EXIT process command from monitor.exe
			//exit the thread, and stop the process
			fExit = true;
		}
		else if(queued_item->iec_type == C_IC_NA_1)
		{
			//Do General Interrogation
			received_command = queued_item->iec_type;
		}
	}

	return;
}

void DNP3MasterApp::alloc_command_resources(void)
{

}

void DNP3MasterApp::free_command_resources(void)
{

}

int DNP3MasterApp::run(void)
{
	while(true)
	{
		if(GetSockConnectStatus())
		{  
			char data_p[80];
			int n_read;

			for(;;)   
			{   
				if(received_command == C_IC_NA_1
					|| received_command == -1)
				{
					received_command = 0;
					/////////////General interrogation//////////////////////////////////////////
					master_p->poll(Master::INTEGRITY);

					n_read = tx_var->read(getSocket(), data_p, 1, 80, 15);

					if(n_read > 0)
					{
						// put the char data into a Bytes container
						Bytes bytes((unsigned char*)data_p, (unsigned char*)data_p + n_read);

						master_p->rxData(&bytes, 0);
					}
					else
					{
						//error
					}
					/////////////////////////////////////////////////////////////////////////////
				}
			
				//master_p->poll(Master::EVENT);

				master_p->startNewTransaction();

				n_read = tx_var->read(getSocket(), data_p, 1, 80, 15);

				if(n_read > 0)
				{
					// put the char data into a Bytes container
					Bytes bytes((unsigned char*)data_p, (unsigned char*)data_p + n_read);

					master_p->rxData(&bytes, 0);
				}
				else
				{
					fprintf(stderr,"dnp3 on line %d exiting...., due to lack of connection with server\n", lineNumber);
					fflush(stderr);
										
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
					item_to_send.msg_id = 0;
					item_to_send.checksum = clearCrc((unsigned char *)&item_to_send, sizeof(struct iec_item));

					//Send in monitor direction
					//prepare published data

					CloseLink();
			
					if(tx_var)
					{
						delete tx_var;
						tx_var = NULL;
					}

					if(master_p)
					{
						delete master_p;
						master_p = NULL;
					}

					break; //exit inner loop
				}

				if(received_command == C_SC_TA_1
				|| received_command == C_DC_TA_1
				|| received_command == C_SE_TA_1
				|| received_command == C_SE_TB_1
				|| received_command == C_SE_TC_1
				|| received_command == C_BO_TA_1
				|| received_command == C_SC_NA_1
				|| received_command == C_DC_NA_1
				|| received_command == C_SE_NA_1 
				|| received_command == C_SE_NB_1
				|| received_command == C_SE_NC_1
				|| received_command == C_BO_NA_1)
				{
					received_command = 0;
					///////////////////////////////////////////////////////////////////
					ControlOutputRelayBlock CORB = ControlOutputRelayBlock();

					CORB.index = 1;
					master_p->control(CORB); //Select
					///////////////////////////////////////////////////////////////////
				}

				#define USE_KEEP_ALIVE_WATCH_DOG

				#ifdef USE_KEEP_ALIVE_WATCH_DOG
				gl_timeout_connection_with_parent++;

				if(gl_timeout_connection_with_parent > 1000*60/pollingTime)
				{
					return 1; //exit loops for timeout of connection with parent
				}
				#endif

				Sleep((unsigned long)pollingTime);
			}   
		}   
		else   
		{   
			if(OpenLink(dnp3ServerAddress, atoi(dnp3ServerPort)))
			{
				Connected = false;
                Sleep(3000);
			}
			else
			{
				Connected = true;

				debugLevel = 1;
				integrityPollInterval = 10;
				masterConfig.addr = lineNumber;
				masterConfig.consecutiveTimeoutsForCommsFail = 3;
				masterConfig.integrityPollInterval_p = &integrityPollInterval;
				masterConfig.debugLevel_p = &debugLevel;

				stationConfig.addr = serverID;
				stationConfig.debugLevel_p = &debugLevel;

				datalinkConfig.addr                  = masterConfig.addr;
				datalinkConfig.isMaster              = 1;
				datalinkConfig.keepAliveInterval_ms  = 10000;

				tx_var = new CustomInter(&debugLevel, 'M', 'S', getSocket());

				datalinkConfig.tx_p                  = tx_var;
				datalinkConfig.debugLevel_p          = &debugLevel;

				master_p = new Master(masterConfig, datalinkConfig, &stationConfig, 1, &db, &timer);
			}
		}
	}

	return 0;
}


#include <time.h>
#include <sys/timeb.h>

void DNP3MasterApp::get_utc_host_time(struct cp56time2a* time)
{
	struct timeb tb;
	struct tm	*ptm;

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

    return;
}