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

#define WIN32_LEAN_AND_MEAN
#define STRICT
#define VC_EXTRALEAN
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "stdint.h"
#include "iec104types.h"
#include "iec_item.h"
#include <signal.h>
#include "getopt.h"
#include "itrace.h"
#include "client.h"
#include "clear_crc_eight.h"

#define SUPPLIER "@ enscada.com"
#define APPLICATION "iec61850client.exe"

int IsSingleInstance(char *name)
{
   HANDLE hMutex;
   char mutex_name[50];

   strcpy(mutex_name, ""APPLICATION"");
   strcat(mutex_name, name);

   hMutex = CreateMutex (NULL, TRUE, mutex_name);

   if (GetLastError() == ERROR_ALREADY_EXISTS)
   {
      if(hMutex)
	  {
			CloseHandle(hMutex);
	  }
	  return 0;
   }
   return 1;
}

///////////////////////////////////////////////
#include "process.h"
struct args{
	char line_number[80];
};

int gl_timeout_connection_with_parent;

void PipeWorker(void* pParam);
///////////////////////////////////////////////

char *optarg;

//Default server_TCP_port is 102

#define RUNTIME_USAGE "Run time usage: %s -a server_IP_address -p server_TCP_port -l\
 line_number -t polling_time_in_ms -d mms_domain -e dump_variables\n"

void usage(char** argv)
{
	fprintf(stderr, RUNTIME_USAGE, argv[0]);
	fflush(stderr);
}

int main(int argc, char **argv)
{
	char version[100];
	int c;
	char iec61850ServerAddress[80];
	char iec61850ServerPort[80];
	char line_number[80];
	char polling_time[80];
	DWORD len;
	char OldConsoleTitle[500];
	char NewConsoleTitle[500];
	char dumpFlag[80];
	SYSTEMTIME oT;
	char mms_domain[80];
		
	IT_IT("main IEC61850 client");

	//version control///////////////////////////////////////////////////////////////
	sprintf(version, ""APPLICATION" - Built on %s %s %s",__DATE__,__TIME__,SUPPLIER);
	fprintf(stderr, "%s\n", version);
	fflush(stderr);
	IT_COMMENT(version);

	GetLocalTime(&oT);
	fprintf(stderr,"%02d/%02d/%04d, %02d:%02d:%02d Starting ... %s\n",oT.wMonth,oT.wDay,oT.wYear,oT.wHour,oT.wMinute,oT.wSecond,APPLICATION); 
	fflush(stderr);
	////////////////////////////////////////////////////////////////////////////////

	iec61850ServerAddress[0] = '\0';
	iec61850ServerPort[0] = '\0';
	polling_time[0] = '\0';
	OldConsoleTitle[0] = '\0';
	NewConsoleTitle[0] = '\0';
	dumpFlag[0] = '\0';
	line_number[0] = '\0';
	polling_time[0] = '\0';
	mms_domain[0] = '\0';

	strcpy(NewConsoleTitle, "iec61850client ");
		
	while( ( c = getopt ( argc, argv, "a:p:l:t:d:e:?" )) != EOF ) {
		switch ( c ) {
			case 'a' :
				strcpy(iec61850ServerAddress, optarg);
				strcat(NewConsoleTitle, " IP ");
				strcat(NewConsoleTitle, optarg);
			break;
			case 'p' :
				strcpy(iec61850ServerPort, optarg);
				strcat(NewConsoleTitle, " port ");
				strcat(NewConsoleTitle, optarg);

			break;
			case 'l' :
				strcat(NewConsoleTitle, " line ");
				strcpy(line_number, optarg);
				strcat(NewConsoleTitle, optarg);
				
			break;
			case 't' :
				strcpy(polling_time, optarg);
				strcat(NewConsoleTitle, " polling time ");
				strcat(NewConsoleTitle, optarg);
			break;
			case 'd' :
				strcpy(mms_domain, optarg);
				strcat(NewConsoleTitle, " MMS domain ");
				strcat(NewConsoleTitle, optarg);
			break;
			case 'e' :
				strcpy(dumpFlag, optarg);
			break;
			case '?' :
				fprintf(stderr, RUNTIME_USAGE, argv[0]);
				fflush(stderr);
				IT_EXIT;
				return EXIT_FAILURE;
		}
	}

	if(argc < 2) 
	{
		usage(argv);
		IT_EXIT;
		return EXIT_FAILURE;
    }

	if(strlen(iec61850ServerAddress) == 0)
	{
		fprintf(stderr,"IEC61850 server IP address not known\n");
		fflush(stderr);
		IT_EXIT;
		return EXIT_FAILURE;
	}

	if(strlen(iec61850ServerPort) == 0)
	{
		fprintf(stderr,"IEC61850 TCP port is not known\n");
		fflush(stderr);
		return EXIT_FAILURE;
	}

	if((len = GetConsoleTitle(OldConsoleTitle, sizeof(OldConsoleTitle))) > 0)
	{
		SetConsoleTitle(NewConsoleTitle);
	}

	if(strlen(polling_time) == 0)
	{
		fprintf(stderr,"polling_time is not known\n");
		fflush(stderr);
		return EXIT_FAILURE;
	}

	/*
	if(strlen(mms_domain) == 0)
	{
		fprintf(stderr,"mms_domain is not known\n");
		fflush(stderr);
		return EXIT_FAILURE;
	}
	*/

	if(strlen(line_number) > 0)
	{
		if(!IsSingleInstance(line_number))
		{
			fprintf(stderr,"Another instance is already running\n");
			fflush(stderr);
			IT_EXIT;
			return EXIT_FAILURE;
		}
	}

	/////////////////keep alive////////////////////////////////////////////////////
    struct args arg;
    strcpy(arg.line_number, line_number);

	if(_beginthread(PipeWorker, 0, (void*)&arg) == -1)
	{
		long nError = GetLastError();

		fprintf(stderr,"PipeWorker _beginthread failed, error code = %d", nError);
		fflush(stderr);
		return EXIT_FAILURE;	
	}
	/////////////////end keep alive////////////////////////////////////////////////
	
	//Alloc IEC61850 class and start
	IEC61850_client_imp* po = new IEC61850_client_imp(iec61850ServerAddress, iec61850ServerPort, polling_time, line_number, mms_domain);

	// connect to an IEC61850 server
	int nRet = po->Start();
	
	if(nRet)
	{
		po->Stop();
		IT_EXIT;
		return EXIT_FAILURE;
	}

	if(strlen(dumpFlag) > 0)
	{
		po->CreateSqlConfigurationFile();
		IT_EXIT;
		return EXIT_SUCCESS;
	}

	nRet = po->AddItems();

	if(nRet)
	{
		po->Stop();
		IT_EXIT;
		return EXIT_FAILURE;
	}

	po->client_state_variable = CLIENT_INITIALIZED;
	
	po->alloc_command_resources();

	po->sendEvents();

	po->free_command_resources();

	po->Stop();

	if(po)
	{
		delete po;
	}

	IT_EXIT;
	return EXIT_SUCCESS;
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

	strcpy(pipe_name, "\\\\.\\pipe\\iec61850client_namedpipe");
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