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

#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <winsock2.h>
#include "clear_crc_eight.h"
#include "iec104types.h"
#include "iec_item.h"
#include "process.h"

///////////////////Start RFC1006 specific/////////////////////////////////////////////////
//#define PLAY_WITH_KEEPALIVE
#include "nodave.h"

#ifdef BCCWIN
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdio.h>
#endif

#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include "log2.h"
#include <winsock2.h>

#ifdef LINUX
#include <unistd.h>
#include <sys/time.h>
#include <fcntl.h>
#define UNIX_STYLE
#endif

#ifdef BCCWIN
#include <time.h>
void wait(void);
#define WIN_STYLE    
#endif

#ifdef PLAY_WITH_KEEPALIVE
#include <sys/socket.h>
#endif
#include <errno.h>

////////////////////////////end RFC1006 spedific ////////////////////////////////////////

int gl_timeout_connection_with_parent = 0;

#define SUPPLIER "@ enscada.com"
#define APPLICATION "rfc1006client.exe"

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

struct args{
	char line_number[80];
};

void PipeWorker(void* pParam);

#define RUNTIME_USAGE "Run time usage: %s -a server IP address -p server TCP port -s position of 343 or 443 PLC slot -c PLC address -l line number\n"

void usage(char** argv)
{
	fprintf(stderr, RUNTIME_USAGE, argv[0]);
	fflush(stderr);
}

int main( int argc, char **argv )
{
	char version[100];
	char rfc1006ServerAddress[80];
	char rfc1006ServerPort[80];
	char ethSlot[80];
	char plc_address[80];
	char line_number[80];
	char fifo_monitor_direction_name[70];
	char fifo_control_direction_name[70];
	char OldConsoleTitle[500];
	char NewConsoleTitle[500];
	int c;
		
	rfc1006ServerAddress[0] = '\0';
	rfc1006ServerPort[0] = '\0';
	ethSlot[0] = '\0';
	plc_address[0] = '\0';
	line_number[0] = '\0';
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

	while( ( c = getopt ( argc, argv, "a:p:s:c:l:?" )) != EOF ) {
		switch ( c ) {
			case 'a' :
			strcpy(rfc1006ServerAddress, optarg);
			break;
			case 'p' :
			strcpy(rfc1006ServerPort, optarg);
			break;
			case 's' :
			strcpy(ethSlot, optarg);
			break;
			case 'c' :
			strcpy(plc_address, optarg);
			break;
			case 'l' :
			strcpy(line_number, optarg);
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

	if(strlen(rfc1006ServerAddress) == 0)
	{
		fprintf(stderr,"RFC1006 server IP address is not known\n");
		fflush(stderr);
		return EXIT_FAILURE;
	}

	if(strlen(rfc1006ServerPort) == 0)
	{
		fprintf(stderr,"RFC1006 TCP port is not known\n");
		fflush(stderr);
		return EXIT_FAILURE;
	}

	if(strlen(ethSlot) == 0)
	{
		fprintf(stderr,"CP 343 or 443 slot is not known\n");
		fflush(stderr);
		return EXIT_FAILURE;
	}

	if(strlen(plc_address) == 0)
	{
		fprintf(stderr,"PLC address is not known\n");
		fflush(stderr);
		return EXIT_FAILURE;
	}
	
	if(strlen(line_number) == 0)
	{
		fprintf(stderr,"line_number is not known\n");
		fflush(stderr);
		return EXIT_FAILURE;
	}
	
	strcpy(fifo_monitor_direction_name, "fifo_monitor_direction");
	strcpy(fifo_control_direction_name, "fifo_control_direction");
	
	if(strlen(line_number) > 0)
	{
		strcat(fifo_monitor_direction_name, line_number);
		strcat(fifo_control_direction_name, line_number);
	}

    strcat(fifo_control_direction_name, "rfc1006");
    strcat(fifo_monitor_direction_name, "rfc1006");

	strcpy(NewConsoleTitle, "rfc1006client IP ");
	strcat(NewConsoleTitle, rfc1006ServerAddress);
	strcat(NewConsoleTitle, " PORT ");
	strcat(NewConsoleTitle, rfc1006ServerPort);
	strcat(NewConsoleTitle, " SLOT ");
	strcat(NewConsoleTitle, ethSlot);
	strcat(NewConsoleTitle, " PLC_ADDRESS ");
	strcat(NewConsoleTitle, plc_address);
	strcat(NewConsoleTitle, " LINE ");
	strcat(NewConsoleTitle, line_number);

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

	#ifdef TEST_ALONE
	if(_beginthread(PipeWorker, 0, (void*)&arg) == -1)
	{
		long nError = GetLastError();

		fprintf(stderr,"PipeWorker _beginthread failed, error code = %d", nError);
		fflush(stderr);
		return EXIT_FAILURE;	
	}
	#endif

	//TODO:  Sobstitute the following code with the class rfc1006_imp

	///////////////////////////////////Start RFC1006 client protocol//////////////////////////////////

	int res, useProtocol, useSlot, rfc1006port, plc_addr;

#ifdef PLAY_WITH_KEEPALIVE    	
    int opt;
#endif    

    daveInterface * di;
    daveConnection * dc;
    _daveOSserialType fds;
    daveSetDebug(daveDebugAll);
    useProtocol = daveProtoISOTCP;
    useSlot = 2;
	plc_addr = 2;
	rfc1006port = atoi(rfc1006ServerPort);
	useSlot = atoi(ethSlot);
	plc_addr = atoi(plc_address);

	///////////open the socket//////////////////////////////////////////
	SOCKET fd;
    struct sockaddr_in addr;
    int addrlen;
    WSADATA wsadata;

    res = WSAStartup(MAKEWORD(2,0), &wsadata);

    addr.sin_family = AF_INET;
    addr.sin_port =htons(rfc1006port);
    addr.sin_addr.s_addr=inet_addr(rfc1006ServerAddress);

    fd = socket(AF_INET, SOCK_STREAM, AF_UNSPEC);

    addrlen = sizeof(addr);

    if(connect(fd, (struct sockaddr *) & addr, addrlen)) 
	{
		closesocket(fd);
		fd = 0;
    } 
	else 
	{
	    fprintf(stderr, "Connected to host: %s \n", rfc1006ServerAddress);
		fflush(stderr);
		errno = 0;
    }

	fds.rfd = (HANDLE)fd;
	/////////////////open socket done///////////////////////////////////

#ifdef PLAY_WITH_KEEPALIVE
    errno=0;    
    opt=1;
    res=setsockopt(fds.rfd, SOL_SOCKET, SO_KEEPALIVE, &opt, 4);
    LOG3("setsockopt %s %d\n", strerror(errno),res);
#endif
    fds.wfd=fds.rfd;
    
    if(fds.rfd > 0) 
	{ 
		di = daveNewInterface(fds,"IF1",0, useProtocol, daveSpeed187k);
		daveSetTimeout(di,5000000);
		dc = daveNewConnection(di, plc_addr, 0, useSlot);  // insert your rack and slot here
	
		if(daveConnectPLC(dc) == 0) 
		{
			printf("Connected.\n");

			fprintf(stderr, "Trying to read 100 bytes (50 dwords) from data block 20\n");
			fflush(stderr);

			//Read a data block

			res = daveReadBytes(dc, daveDB, 20, 0, 100, NULL);

			if(res == 0)
			{
				int a;
				a = daveGetU16(dc);
				printf("DB20:DW0: %d\n", a);
			} 
			else 
				printf("failed! (%d)\n",res);  

			closesocket((SOCKET)(fds.rfd));

			printf("Finished.\n");

			return 0;
		} 
		else 
		{
			printf("Couldn't connect to PLC.\n Please make sure you use the -2 option with a CP243 but not with CPs 343 or 443.\n");	
			closesocket((SOCKET)(fds.rfd));
			return -2;
		}
    } 
	else 
	{
		printf("Couldn't open TCP port. \nPlease make sure a CP is connected and the IP address is ok. \n");	
    	return -1;
    }    
	///////////////////////////////////End RFC1006 client protocol//////////////////////////////////
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

	strcpy(pipe_name, "\\\\.\\pipe\\rfc1006client_namedpipe");
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
