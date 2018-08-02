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

#include "..\fastdb\inc\fastdb.h"
#include "realtimesqlserver.h"
#include "..\fastdb\inc\server.h"
#include "..\fastdb\inc\sync.h"
#include "inifile.h"
#include "IndentedTrace.h"

USE_FASTDB_NAMESPACE

RealTimeSqlServer::RealTimeSqlServer()
{
    IT_IT("RealTimeSqlServer::RealTimeSqlServer");
}


RealTimeSqlServer::~RealTimeSqlServer() 
{
	IT_IT("RealTimeSqlServer::~RealTimeSqlServer");

	char ini_file[256]; 

	ini_file[0] = '\0';

	#ifdef WIN32
	if(GetModuleFileName(NULL, ini_file, _MAX_PATH))
	{
		*(strrchr(ini_file, '\\')) = '\0';        // Strip \\filename.exe off path
		*(strrchr(ini_file, '\\')) = '\0';        // Strip \\bin off path
    }
	#endif
	
	strcat(ini_file, "\\project\\realtimedb.ini");
	
	Inifile iniFile(ini_file);

	char serv[INIFILE_MAX_LINELEN+2];
	char buff[INIFILE_MAX_LINELEN+2];
	char seps[]   = " ,\t\n";
	char* token;

	if(iniFile.find("online","databases"))
	{
		strcpy(buff, iniFile.find("online","databases"));
				
		token = strtok( buff, seps );
								
		for(int i = 0; token != NULL ;i++)
		{
			if( iniFile.find("address",token) && 
			    iniFile.find("port",token) )
			{
				strcpy(serv, iniFile.find("address",token));
				strcat(serv,":");
				strcat(serv,iniFile.find("port",token));
				
				dbServer* server = dbServer::find(serv);
   				if (server != NULL) 
				{
					server->stop();
					printf("Server stopped for URL %s\n", serverURL);
				} 
				else 
				{
					printf("No server was started for URL %s\n", serv);
				}
			}
			
			token = strtok( NULL, seps );
		}
	}

	dbServer::cleanup();
}


void RealTimeSqlServer::run(int argc, char* argv[])
{
	IT_IT("RealTimeSqlServer::run");
	
	char ini_file[256];
	
	ini_file[0] = '\0';

	#ifdef WIN32
	if(GetModuleFileName(NULL, ini_file, _MAX_PATH))
	{
		*(strrchr(ini_file, '\\')) = '\0';        // Strip \\filename.exe off path
		*(strrchr(ini_file, '\\')) = '\0';        // Strip \\bin off path
    }
	#endif

	strcat(ini_file, "\\project\\realtimedb.ini");
		
	Inifile iniFile(ini_file);

	char serv[INIFILE_MAX_LINELEN+2];
	char buff[INIFILE_MAX_LINELEN+2];
	char seps[]   = " ,\t\n";
	char* token;

	if(iniFile.find("online","databases"))
	{
		strcpy(buff, iniFile.find("online","databases"));
				
		token = strtok( buff, seps );
								
		for(int i = 0; token != NULL ;i++)
		{
			if( iniFile.find("address",token) && 
			    iniFile.find("port",token) )
			{
				strcpy(serv, iniFile.find("address",token));
				strcat(serv,":");
				strcat(serv,iniFile.find("port",token));
				
				dbServer* server = dbServer::find(serv);
				if (server == NULL) 
				{
					serverURL = new char_t[strlen(serv)+1];
					strcpy(serverURL, serv);
					
					dbDatabase* db = new dbDatabase(dbDatabase::dbAllAccess, 256);

					server = new dbServer(db, serverURL);
					printf(_T("\tReal time sql server started for URL %s\n"), serverURL);
				}

				if (server != NULL) 
				{
					server->start();
				}
			}
			
			token = strtok( NULL, seps );
		}
	}
}



