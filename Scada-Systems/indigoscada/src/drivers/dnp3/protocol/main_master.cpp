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

#include <time.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <winsock2.h>
#include "clear_crc_eight.h"
#include "iec104types.h"
#include "iec_item.h"
#include "process.h"
#include "dnp3_master_app.h"

#define SUPPLIER "@ enscada.com"
#define APPLICATION "dnp3master.exe"

int IsSingleInstance(const char* name)
{
   HANDLE hMutex;
   char mutex_name[200];

   strcpy(mutex_name, ""APPLICATION"");
   strcat(mutex_name, name);

   hMutex = CreateMutex(NULL, TRUE, mutex_name);

   if(GetLastError() == ERROR_ALREADY_EXISTS)
   {
      if(hMutex)
	  {
		 CloseHandle(hMutex);
	  }

	  return 0;
   }

   return 1;
}

#include "getopt.h"

///////////////////////////////////////////////
struct args{
	char line_number[80];
};

int gl_timeout_connection_with_parent;

void PipeWorker(void* pParam);
///////////////////////////////////////////////

#define RUNTIME_USAGE "Run time usage: %s -a server IP address -p server TCP port -l\
 line number -t polling time\n"

void usage(char** argv)
{
	fprintf(stderr, RUNTIME_USAGE, argv[0]);
	fflush(stderr);
}

int main( int argc, char **argv )
{
	char version[100];
	char dnp3ServerAddress[80];
	char dnp3ServerPort[80];
	char line_number[80];
	char polling_time[80];
	char IOA_AO[80];
	char IOA_BO[80];
	char IOA_CI[80];
	char IOA_BI[80];
	char IOA_AI[80];
	char server_id[5];

	char fifo_monitor_direction_name[70];
	char fifo_control_direction_name[70];
	char OldConsoleTitle[500];
	char NewConsoleTitle[500];
	int c;
	int pollingTime = 1000;
	int nIOA_AO = 200;
	int nIOA_BO = 150;
	int nIOA_CI = 100;
	int nIOA_BI = 50;
	int nIOA_AI = 1;
	int serverID;
		
	dnp3ServerAddress[0] = '\0';
	dnp3ServerPort[0] = '\0';
	line_number[0] = '\0';
	polling_time[0] = '\0';
	IOA_AO[0] = '\0';
	IOA_BO[0] = '\0';
	IOA_CI[0] = '\0';
	IOA_BI[0] = '\0';
	IOA_AI[0] = '\0';
	server_id[0] = '\0';

	fifo_monitor_direction_name[0] = '\0';
	fifo_control_direction_name[0] = '\0';

	//version control///////////////////////////////////////////////////////////////
	sprintf(version, ""APPLICATION" - Built on %s %s %s",__DATE__,__TIME__,SUPPLIER);
	fprintf(stderr, "%s\n", version);
	fflush(stderr);
	SYSTEMTIME oT;
	::GetLocalTime(&oT);
	fprintf(stderr,"%02d/%02d/%04d, %02d:%02d:%02d Starting ... %s\n",oT.wMonth,oT.wDay,oT.wYear,oT.wHour,oT.wMinute,oT.wSecond,APPLICATION); 
	fflush(stderr);
	////////////////////////////////////////////////////////////////////////////////

	while( ( c = getopt ( argc, argv, "a:c:d:e:f:g:p:l:t:s:?" )) != EOF ) {
		switch ( c ) {
			case 'a' :
			strcpy(dnp3ServerAddress, optarg);
			break;
			case 'p' :
			strcpy(dnp3ServerPort, optarg);
			break;
			case 'l' :
			strcpy(line_number, optarg);
			break;
			case 't' :
			strcpy(polling_time, optarg);
			break;
			case 'c' :
			strcpy(IOA_AO, optarg);
			break;
			case 'd' :
			strcpy(IOA_BO, optarg);
			break;
			case 'e' :
			strcpy(IOA_CI, optarg);
			break;
			case 'f' :
			strcpy(IOA_BI, optarg);
			break;
			case 'g' :
			strcpy(IOA_AI, optarg);
			break;
			case 's' :
			strcpy(server_id, optarg);
			break;
			case '?' :
			fprintf(stderr, RUNTIME_USAGE, argv[0]);
			fflush(stderr);
			exit( 0 );
		}
	}

	if(argc < 2) 
	{
		usage(argv);
		exit(-1);
    }

	if(strlen(dnp3ServerAddress) == 0)
	{
		fprintf(stderr,"DNP3 server IP address is not known\n");
		fflush(stderr);
		return EXIT_FAILURE;
	}

	if(strlen(dnp3ServerPort) == 0)
	{
		fprintf(stderr,"DNP3 TCP port is not known\n");
		fflush(stderr);
		return EXIT_FAILURE;
	}

	if(strlen(line_number) == 0)
	{
		fprintf(stderr,"line_number is not known\n");
		fflush(stderr);
		return EXIT_FAILURE;
	}

	if(strlen(server_id) == 0)
	{
		fprintf(stderr,"server_id is not known\n");
		fflush(stderr);
		return EXIT_FAILURE;
	}

	serverID = atoi(server_id);

	if(strlen(polling_time) == 0)
	{
		fprintf(stderr,"polling_time is not known\n");
		fflush(stderr);
		return EXIT_FAILURE;
	}

	pollingTime = atoi(polling_time);

	nIOA_AO = atoi(IOA_AO);
	nIOA_BO = atoi(IOA_BO);
	nIOA_CI = atoi(IOA_CI);
	nIOA_BI = atoi(IOA_BI);
	nIOA_AI = atoi(IOA_AI);

	strcpy(fifo_monitor_direction_name, "fifo_monitor_direction");
	strcpy(fifo_control_direction_name, "fifo_control_direction");
	
	if(strlen(line_number) > 0)
	{
		strcat(fifo_monitor_direction_name, line_number);
		strcat(fifo_control_direction_name, line_number);
	}

    strcat(fifo_control_direction_name, "dnp3");
    strcat(fifo_monitor_direction_name, "dnp3");

	strcpy(NewConsoleTitle, "dnp3master IP ");
	strcat(NewConsoleTitle, dnp3ServerAddress);
	strcat(NewConsoleTitle, " PORT ");
	strcat(NewConsoleTitle, dnp3ServerPort);
	strcat(NewConsoleTitle, " LINE ");
	strcat(NewConsoleTitle, line_number);
	strcat(NewConsoleTitle, " polling time ");
	strcat(NewConsoleTitle, polling_time);

	strcat(NewConsoleTitle, " IOA_AO ");
	strcat(NewConsoleTitle, IOA_AO);

	strcat(NewConsoleTitle, " IOA_BO ");
	strcat(NewConsoleTitle, IOA_BO);

	strcat(NewConsoleTitle, " IOA_CI ");
	strcat(NewConsoleTitle, IOA_CI);

	strcat(NewConsoleTitle, " IOA_BI ");
	strcat(NewConsoleTitle, IOA_BI);

	strcat(NewConsoleTitle, " IOA_AI ");
	strcat(NewConsoleTitle, IOA_AI);

	strcat(NewConsoleTitle, " SERVER_ID ");
	strcat(NewConsoleTitle, server_id);

	if(!IsSingleInstance(NewConsoleTitle))
	{
		fprintf(stderr,"Another instance is already running\n");
		fflush(stderr);
		return EXIT_FAILURE;
	}

	int rc;
	if((rc = GetConsoleTitle(OldConsoleTitle, sizeof(OldConsoleTitle))) > 0)
	{
		SetConsoleTitle(NewConsoleTitle);
	}

    struct args arg;
    strcpy(arg.line_number, line_number);

	if(_beginthread(PipeWorker, 0, (void*)&arg) == -1)
	{
		long nError = GetLastError();

		fprintf(stderr,"PipeWorker _beginthread failed, error code = %d", nError);
		fflush(stderr);
		return EXIT_FAILURE;	
	}

	///////////////////////////////////Protocol//////////////////////////////////

	DNP3MasterApp* master_app;

	master_app = new DNP3MasterApp(dnp3ServerAddress, 
		dnp3ServerPort, 
		line_number,
		serverID,
		pollingTime,
		nIOA_AO,
		nIOA_BO,
		nIOA_CI,
		nIOA_BI,
		nIOA_AI);
	
	master_app->run();
	
	if(master_app)
	{
		delete master_app;
	}

	return 0;
}

///////////////////////////////////Keep alive pipe management/////////////////////////////////////////////////////

#define BUF_SIZE 200
#define N_PIPES 3

void PipeWorker(void* pParam)
{
	HANDLE pipeHnds[N_PIPES];
	char in_buffer[N_PIPES][BUF_SIZE];
	OVERLAPPED ovrp[N_PIPES];
	HANDLE evnt[N_PIPES];
	DWORD rc, len, pipe_id;
    unsigned char buf[sizeof(struct iec_item)];
    struct iec_item* p_item;
	char pipe_name[150];
    int i;

    struct args* arg = (struct args*)pParam;

	strcpy(pipe_name, "\\\\.\\pipe\\dnp3master_namedpipe");
    strcat(pipe_name, arg->line_number);

	for(i = 0; i < N_PIPES; i++)
	{
		if ((pipeHnds[i] = CreateNamedPipe(
			pipe_name,
			PIPE_ACCESS_DUPLEX | FILE_FLAG_OVERLAPPED,
			PIPE_TYPE_BYTE | PIPE_READMODE_BYTE, N_PIPES,
			0, 
			0, 
			1000, 
			NULL)) == INVALID_HANDLE_VALUE)
		{
			fprintf(stderr,"CreateNamedPipe for pipe %d failed with error %d\n", i, GetLastError());
			fflush(stderr);
			//ExitProcess(0);
		}

		if ((evnt[i] = CreateEvent(NULL, TRUE, FALSE, NULL)) == NULL)
		{
			fprintf(stderr,"CreateEvent for pipe %d failed with error %d\n",	i, GetLastError());
			fflush(stderr);
			//ExitProcess(0);
		}

		ZeroMemory(&ovrp[i], sizeof(OVERLAPPED));

		ovrp[i].hEvent = evnt[i];

		if (ConnectNamedPipe(pipeHnds[i], &ovrp[i]) == 0)
		{
			if (GetLastError() != ERROR_IO_PENDING)
			{
				fprintf(stderr,"ConnectNamedPipe for pipe %d failed with error %d\n", i, GetLastError());
				fflush(stderr);
				
				CloseHandle(pipeHnds[i]);
				//ExitProcess(0);
			}
		}
	}

	while(1)
	{
		if((rc = WaitForMultipleObjects(N_PIPES, evnt, FALSE, INFINITE)) == WAIT_FAILED)
		{
			fprintf(stderr,"WaitForMultipleObjects failed with error %d\n", GetLastError());
			fflush(stderr);
			//ExitProcess(0);
		}

		pipe_id = rc - WAIT_OBJECT_0;

		ResetEvent(evnt[pipe_id]);

		if(GetOverlappedResult(pipeHnds[pipe_id], &ovrp[pipe_id], &len, TRUE) == 0)
		{
			fprintf(stderr,"GetOverlapped result failed %d start over\n", GetLastError());
			fflush(stderr);
		
			if(DisconnectNamedPipe(pipeHnds[pipe_id]) == 0)
			{
				fprintf(stderr,"DisconnectNamedPipe failed with error %d\n", GetLastError());
				fflush(stderr);
				//ExitProcess(0);
			}

			if(ConnectNamedPipe(pipeHnds[pipe_id],	&ovrp[pipe_id]) == 0)
			{
				if(GetLastError() != ERROR_IO_PENDING)
				{
					fprintf(stderr,"ConnectNamedPipe for pipe %d failed with error %d\n", i, GetLastError());
					fflush(stderr);
					CloseHandle(pipeHnds[pipe_id]);
				}
			}
		}
		else
		{
			ZeroMemory(&ovrp[pipe_id], sizeof(OVERLAPPED));

			ovrp[pipe_id].hEvent = evnt[pipe_id];

			if((rc = ReadFile(pipeHnds[pipe_id], in_buffer[pipe_id], sizeof(struct iec_item), NULL, &ovrp[pipe_id])) == 0)
			{
				if(GetLastError() != ERROR_IO_PENDING)
				{
					fprintf(stderr,"ReadFile failed with error %d\n", GetLastError());
					fflush(stderr);
				}
			}
			
			memcpy(buf, in_buffer[pipe_id], sizeof(struct iec_item));
			
			if(len)
			{
				p_item = (struct iec_item*)buf;

				//fprintf(stderr, "Receiving from pipe %d th message\n", p_item->msg_id);
				//fflush(stderr);
											
				//for (j = 0; j < len; j++) 
				//{ 
					//unsigned char c = *((unsigned char*)buf + j);
					//fprintf(stderr, "rx pipe <--- 0x%02x-", c);
					//fflush(stderr);
					//
				//}
				
				rc = clearCrc((unsigned char *)buf, sizeof(struct iec_item));
				if(rc != 0)
				{
					//ExitProcess(0);
				}

				if(p_item->iec_obj.ioa == 4004)
				{ 
					gl_timeout_connection_with_parent = 0;
					//fprintf(stderr, "Receive keep alive # %d from front end\n", p_item->msg_id);
                    fprintf(stderr, "wdg %d\r", p_item->msg_id);
				    fflush(stderr);
				}
			}
		}
	}
}
