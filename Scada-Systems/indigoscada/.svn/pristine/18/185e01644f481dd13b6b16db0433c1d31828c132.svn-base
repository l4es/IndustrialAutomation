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

#include <stdio.h>
#include <string.h>
#include "RIPCServer.h"
#include "getopt.h"

#define SUPPLIER "@ enscada.com"
#define APPLICATION "mserver.exe"

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

#define RUNTIME_USAGE "Run time usage: %s -d debug_level -p TCP_port\n"

void usage(char** argv)
{
	fprintf(stderr, RUNTIME_USAGE, argv[0]);
	fflush(stderr);
}

int main(int argc, char* argv[])
{ 
	char version[100];
	char ServerTCPPort[20];
	char debugLevel[20];
	int port = 0;
	int c;
    int debug = RIPCServer::DEFAULT_DEBUG_LEVEL;

	ServerTCPPort[0] = '\0';
	debugLevel[0] = '\0';
	
   	//version control///////////////////////////////////////////////////////////////
	sprintf(version, ""APPLICATION" - Built on %s %s %s",__DATE__,__TIME__,SUPPLIER);
	fprintf(stderr, "%s\n", version);
	fflush(stderr);
	SYSTEMTIME oT;
	::GetLocalTime(&oT);
	fprintf(stderr,"%02d/%02d/%04d, %02d:%02d:%02d Starting ... %s\n",oT.wMonth,oT.wDay,oT.wYear,oT.wHour,oT.wMinute,oT.wSecond,APPLICATION); 
	fflush(stderr);
	////////////////////////////////////////////////////////////////////////////////

	if(!IsSingleInstance("middleware"))
	{
		fprintf(stderr,"Another instance is already running\n");
		fflush(stderr);
		return 1;
	}
	
	while( ( c = getopt ( argc, argv, "d:p:?" )) != EOF ) {
		switch ( c ) {
			case 'd' :
			strcpy(debugLevel, optarg);
			break;
			case 'p' :
			strcpy(ServerTCPPort, optarg);
			break;
			case '?' :
			fprintf(stderr, RUNTIME_USAGE, argv[0]);
			fflush(stderr);
			return 1;
		}
	}

	if(argc < 2) 
	{
		usage(argv);
		return 1;
    }

	if(strlen(ServerTCPPort) == 0)
	{
		fprintf(stderr,"ServerTCPPort is not known\n");
		fflush(stderr);
		return 1;
	}

	port = atoi(ServerTCPPort);

    if (port == 0) 
	{ 
		usage(argv);
		return 1;
    }

	if(strlen(debugLevel) > 0)
	{
		debug = atoi(debugLevel);
		return 1;
	}

    RIPCServer* server = new RIPCServer(port, debug);
 	
	server->start();
	server->dialog();

    return 0;
}

