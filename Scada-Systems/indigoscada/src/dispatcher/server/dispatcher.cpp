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

#include "stdtp.h"
#include "sync.h"
#include "dispatcher.h"
#include "server_dispatcher.h"
#include "inifile.h"
#include "IndentedTrace.h"

USE_GIGABASE_NAMESPACE

DispatcherServer::DispatcherServer()
{
	IT_IT("DispatcherServer::DispatcherServer");
}

DispatcherServer::~DispatcherServer() 
{
	IT_IT("DispatcherServer::~DispatcherServer");
	
	dispServer* server = dispServer::find(serverURL);
   	if (server != NULL) 
	{
		server->stop();
		printf("Server stopped for URL %s\n", serverURL);
	} 
	else 
	{
		printf("No server was started for URL %s\n", serverURL);
	}

    dispServer::cleanup();
}

void DispatcherServer::run(int argc, char* argv[])
{
	IT_IT("DispatcherServer::run");

	char serv[INIFILE_MAX_LINELEN+2];

	char ini_file[256];

	#ifdef WIN32
	ini_file[0] = '\0';
	if(GetModuleFileName(NULL, ini_file, _MAX_PATH))
	{
		*(strrchr(ini_file, '\\')) = '\0';        // Strip \\filename.exe off path
		*(strrchr(ini_file, '\\')) = '\0';        // Strip \\bin off path
	}
	#endif
	
	strcat(ini_file, "\\project\\dispatcher.ini");
	Inifile iniFile(ini_file);


	if( iniFile.find("address","dispatcherserver") &&
		iniFile.find("port","dispatcherserver") )
	{
		strcpy(serv, iniFile.find("address","dispatcherserver"));
		strcat(serv,":");
		strcat(serv,iniFile.find("port","dispatcherserver"));

		dispServer* server = dispServer::find(serv);
		if (server == NULL) 
		{
			serverURL = new char_t[STRLEN(serv)+1];
			STRCPY(serverURL, serv);
			
			server = new dispServer(serverURL, 8);
			PRINTF(_T("\tDispatcher server started for URL %s\n"), serverURL);
		}

		if (server != NULL) 
		{
			server->start();
		}
	}

	//dbMutex mutex;
    //dbCriticalSection cs(mutex);
    //TerminationEvent.open();
    //TerminationEvent.wait(mutex);
    //TerminationEvent.close();
}



