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

/*
*Header For: historic database main startup
*Purpose:
*/
#include "historicdb.h"
#include "qinputdialog.h"
#include "common.h"
#include "inifile.h"
#include "IndentedTrace.h"
//
//
// We define here the historical database interfaces 
//
static HistoricDbDict databases;

QSDatabase *GetHistoricResultDb ()
{
	HistoricDbDict::iterator j =  databases.find("resultsdb");

	if(j != databases.end())
	{
		return (*j).second;
	}

	return NULL;
};

HistoricDbDict& GetHistoricDbDict()
{
	return databases;
};

//
// return true on success else false
//
bool OpenHistoricConnections()	// open the connections with the historical server
{
	IT_IT("OpenHistoricConnections");

	bool res = true;  
	QString DbUserName;
	QString DbPassword;
			
	QString ini_file = GetScadaHomeDirectory() + "\\project\\historicdb.ini";
	Inifile iniFile((const char*)ini_file);
	
	if( iniFile.find("user","sqlserver") &&
		iniFile.find("password","sqlserver") )
	{
		DbUserName = iniFile.find("user","sqlserver");
		DbPassword = iniFile.find("password","sqlserver");
	}
	else
	{
		res = false;
	}

	char buff[INIFILE_MAX_LINELEN+2];
	char* token;
	char seps[]   = " ,\t\n";

	bool is_low_freq = true;

	//open connection with databases
	if(iniFile.find("online","databases"))
	{
		strcpy(buff, iniFile.find("online","databases"));
				
		token = strtok( buff, seps );
								
		for(int i = 0; token != NULL ;i++)
		{
			if( iniFile.find("address",token) && 
				iniFile.find("port",token) &&
				iniFile.find("is_low_freq",token) )
			{
				QString host = QString(iniFile.find("address",token)) + ":" + QString(iniFile.find("port",token));

				QString dbname = token;

				HistoricDbDict::value_type pr(dbname,new HISTORIC_DB());
				databases.insert(pr); // put in the dictionary

				if(!strcmp("1",iniFile.find("is_low_freq",token)))
				{
					is_low_freq =  true;
				}
				else
				{
					is_low_freq =  false;				
				}

				HistoricDbDict::iterator j =  databases.find(token);

				if(!(j == databases.end()))
				{
					res &= (*j).second->Connect (host, dbname, DbUserName, DbPassword, is_low_freq);
				}
				else
				{
					res = false;
				}
			}
			else
			{
				res = false;
			}
			
			token = strtok( NULL, seps );
		}
	}
	else
	{
		res = false;
	}

	if(res == false)
	{
		for(HistoricDbDict::iterator it = databases.begin(); it != databases.end(); it++)
		{
			(*it).second->Disconnect();
			delete ((*it).second);

			databases.erase(it);
		}
	}

	return res;
};

//
// return true on success else false
//
bool ConnectToHistoricDatabases()
{
	IT_IT("ConnectToHistoricDatabases");

	bool res = true;  
	QString DbUserName;
	QString DbPassword;
			
	QString ini_file = GetScadaHomeDirectory() + "\\project\\historicdb.ini";
	Inifile iniFile((const char*)ini_file);
	
	if( iniFile.find("user","sqlserver") &&
		iniFile.find("password","sqlserver") )
	{
		DbUserName = iniFile.find("user","sqlserver");
		DbPassword = iniFile.find("password","sqlserver");
	}
	else
	{
		res = false;
	}

	char buff[INIFILE_MAX_LINELEN+2];
	char* token;
	char seps[]   = " ,\t\n";

	bool is_low_freq = true;

	//open connection with databases
	if(iniFile.find("online","databases"))
	{
		strcpy(buff, iniFile.find("online","databases"));
				
		token = strtok( buff, seps );
								
		for(int i = 0; token != NULL ;i++)
		{
			if( iniFile.find("address",token) && 
				iniFile.find("port",token) &&
				iniFile.find("is_low_freq",token) )
			{
				QString host = QString(iniFile.find("address",token)) + ":" + QString(iniFile.find("port",token));

				QString dbname = token;

				//HistoricDbDict::value_type pr(dbname,new HISTORIC_DB());
				//databases.insert(pr); // put in the dictionary

				if(!strcmp("1",iniFile.find("is_low_freq",token)))
				{
					is_low_freq =  true;
				}
				else
				{
					is_low_freq =  false;				
				}

				HistoricDbDict::iterator j =  databases.find(token);

				if(!(j == databases.end()))
				{
					res &= (*j).second->Connect (host, dbname, DbUserName, DbPassword, is_low_freq);
				}
				else
				{
					res = false;
				}
			}
			else
			{
				res = false;
			}
			
			token = strtok( NULL, seps );
		}
	}
	else
	{
		res = false;
	}

	return res;
};

//
//
//
void CloseHistoricConnections()
{
	IT_IT("CloseHistoricConnections");

	for(HistoricDbDict::iterator it = databases.begin(); !(it == databases.end()); it++)
	{
		(*it).second->Disconnect();

		delete ((*it).second);

		databases.erase(it);
	}
};

void DisconnectFromHistoricDatabases()
{
	IT_IT("DisconnectFromHistoricDatabases");

	for(HistoricDbDict::iterator it = databases.begin(); !(it == databases.end()); it++)
	{
		(*it).second->Disconnect();
	}
};

