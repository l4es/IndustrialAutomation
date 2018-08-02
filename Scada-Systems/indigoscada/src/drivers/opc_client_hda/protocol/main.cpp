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

#define STRICT
#define VC_EXTRALEAN
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//#include <assert.h>
#include <errno.h>
#include "stdint.h"
#include "iec104types.h"
#include <signal.h>
#include "getopt.h"
#include "itrace.h"
#include "opc_client_hda_app.h"

#define SUPPLIER "@ enscada.com"
#define APPLICATION "opc_client_hda.exe"

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

char *optarg;

#define RUNTIME_USAGE "Run time usage: %s -a server_IP_address\
 -p OPCServerProgID -q CLSID -l line_number -t polling_time_in_ms\n"

void usage(char** argv)
{
	fprintf(stderr, RUNTIME_USAGE, argv[0]);
	fflush(stderr);
}

int main(int argc, char **argv)
{
	char version[100];
	int c;
	char OPCServerAddress[80];
	char OpcServerProgID[80];
	char OpcUpdateRate[80];
	char OpcPercentDeadband[80];
	char OpcclassId[80];

	DWORD len;
	char OldConsoleTitle[500];
	char NewConsoleTitle[500];
	char line_number[80];
		
	IT_IT("main OPC CLIENT DA");

	//version control///////////////////////////////////////////////////////////////
	sprintf(version, ""APPLICATION" - Built on %s %s %s",__DATE__,__TIME__,SUPPLIER);
	fprintf(stderr, "%s\n", version);
	fflush(stderr);
	IT_COMMENT(version);
	////////////////////////////////////////////////////////////////////////////////

	OPCServerAddress[0] = '\0';
	OpcServerProgID[0] = '\0';
	OpcUpdateRate[0] = '\0';
	OpcPercentDeadband[0] = '\0';
	OpcclassId[0] = '\0';
	OldConsoleTitle[0] = '\0';
	NewConsoleTitle[0] = '\0';
	line_number[0] = '\0';

	while( ( c = getopt ( argc, argv, "a:p:q:l:t:?" )) != EOF ) {
		switch ( c ) {
			case 'a' :
				strcpy(OPCServerAddress, optarg);
				strcat(NewConsoleTitle, optarg);
				strcat(NewConsoleTitle, "   ");
			break;
			case 'p' :
				strcpy(OpcServerProgID, optarg);
				strcat(NewConsoleTitle, optarg);
				strcat(NewConsoleTitle, "   ");
			break;
			case 'q' :
				strcpy(OpcclassId, optarg);
				strcat(NewConsoleTitle, optarg);
				strcat(NewConsoleTitle, "   ");
			break;
			case 'l' :
				strcpy(line_number, optarg);
				strcat(NewConsoleTitle, optarg);
				strcat(NewConsoleTitle, "   ");
			break;
			case 't' :
				strcpy(OpcUpdateRate, optarg);
				strcat(NewConsoleTitle, optarg);
				strcat(NewConsoleTitle, "   ");
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

	if(strlen(OPCServerAddress) == 0)
	{
		fprintf(stderr,"OPC server IP address not known\n");
		fflush(stderr);
		IT_EXIT;
		return EXIT_FAILURE;
	}

	if((len = GetConsoleTitle(OldConsoleTitle, sizeof(OldConsoleTitle))) > 0)
	{
		SetConsoleTitle(NewConsoleTitle);
	}

	if(!IsSingleInstance(line_number))
	{
		fprintf(stderr,"Another instance is already running\n");
		fflush(stderr);
		IT_EXIT;
		return EXIT_FAILURE;
	}

	//Alloc OPC class and start
	Opc_client_hda_imp* p = new Opc_client_hda_imp(OPCServerAddress);

	 // connect to an OPC server
	int nRet = p->OpcStart(OpcServerProgID, OpcclassId, OpcUpdateRate, OpcPercentDeadband);
	
	if(nRet)
	{
		p->OpcStop();
		IT_EXIT;
		return EXIT_FAILURE;
	}

	nRet = p->load_database();

	if(nRet)
	{
		p->OpcStop();
		IT_EXIT;
		return EXIT_FAILURE;
	}

	p->opc_client_state_variable = OPC_CLIENT_INITIALIZED;

	p->Update();

	p->OpcStop();

	if(p)
	{
		delete p;
	}

	IT_EXIT;
	return EXIT_SUCCESS;
}

