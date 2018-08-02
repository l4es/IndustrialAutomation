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
*Header For: database main startup
*Purpose:
*/

#include "realtimedb.h"
#include "qinputdialog.h"
#include "common.h"
#include "dispatch.h"
#include "inifile.h"
#include "IndentedTrace.h"

//
//
// We define here the database interfaces 
//

#ifdef USE_STD_MAP

static RealTimeDbDict databases;

#else

#include "ds.h"

static SET databases = NULL;

static const char* nullstr(const char *s)
{
    if(!s)
	{
		return "(null)";
	}

    return s;
}

/* Use this to compare set elements, in this case strings */
static int setcmp(void *s1,void *s2)
{
    return(!strcmp((const char*)s1,(const char*)s2));
}

//# define PR_SET(s,set)	\
//    for (s = (char*)setFirst(set); s; s = setNext(set))	\
//	printf("s = %s\n",nullstr(s));

#endif

//
// Access functions
// 
QSDatabase *GetConfigureDb ()
{
	#ifdef USE_STD_MAP
	RealTimeDbDict::iterator j =  databases.find("configdb");

	if(!(j == databases.end()))
	{
		return (*j).second;
	}
	return NULL;

	#else

	return NULL;
	#endif
	
};

QSDatabase *GetCurrentDb ()
{
	#ifdef USE_STD_MAP
	RealTimeDbDict::iterator j =  databases.find("currentdb");

	if(!(j == databases.end()))
	{
		return (*j).second;
	}
	return NULL;

	#else

	return NULL;
	#endif
};

QSDatabase *GetResultDb ()
{
	#ifdef USE_STD_MAP
	RealTimeDbDict::iterator j =  databases.find("resultsdb");

	if(!(j == databases.end()))
	{
		return (*j).second;
	}
	return NULL;

	#else

	return NULL;
	#endif

};

#ifdef USE_STD_MAP
RealTimeDbDict& GetRealTimeDbDict()
{
	return databases;
};

#else
SET GetRealTimeDbDict()
{
	return databases;
};
#endif
//
//
//
// return true on success else false
//
bool OpenRealTimeConnections()	// open the connections with the servers
{
	IT_IT("OpenRealTimeConnections");

	bool res = true;  
	QString DbUserName;
	QString DbPassword;
	
	QString ini_file = GetScadaHomeDirectory() + "\\project\\realtimedb.ini";
	Inifile iniFile((const char*)ini_file);

	if( iniFile.find("user","rtsqlserver") &&
		iniFile.find("password","rtsqlserver") )
	{
		DbUserName = iniFile.find("user","rtsqlserver");
		DbPassword = iniFile.find("password","rtsqlserver");
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

				#ifdef USE_STD_MAP
				RealTimeDbDict::value_type pr(dbname,new REALTIME_DB());
				databases.insert(pr); // put in the dictionary
				#else

				databases  = setNew((SETCMPFUN)strcmp, 1, 1);
				setAdd(databases, strdup(dbname));

				#endif

				if(!strcmp("1",iniFile.find("is_low_freq",token)))
				{
					is_low_freq =  true;
				}
				else
				{
					is_low_freq =  false;				
				}

				#ifdef USE_STD_MAP
				RealTimeDbDict::iterator j =  databases.find(token);

				if(!(j == databases.end()))
				{
					res &= (*j).second->Connect (host, dbname, DbUserName, DbPassword, is_low_freq);
				}
				else
				{
					res = false;
				}

				#else

				#endif
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
		#ifdef USE_STD_MAP
		for(RealTimeDbDict::iterator it = databases.begin(); it != databases.end(); it++)
		{
			(*it).second->Disconnect();
			delete ((*it).second);

			databases.erase(it);
		}
		#else

		#endif
	}

	return res;
};


//
// return true on success else false
//
bool ConnectToRealTimeDatabases()
{
	IT_IT("ConnectToRealTimeDatabases");

	bool res = true;  
	QString DbUserName;
	QString DbPassword;
	
	QString ini_file = GetScadaHomeDirectory() + "\\project\\realtimedb.ini";
	Inifile iniFile((const char*)ini_file);

	if( iniFile.find("user","rtsqlserver") &&
		iniFile.find("password","rtsqlserver") )
	{
		DbUserName = iniFile.find("user","rtsqlserver");
		DbPassword = iniFile.find("password","rtsqlserver");
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

				//RealTimeDbDict::value_type pr(dbname,new REALTIME_DB());
				//databases.insert(pr); // put in the dictionary

				if(!strcmp("1",iniFile.find("is_low_freq",token)))
				{
					is_low_freq =  true;
				}
				else
				{
					is_low_freq =  false;				
				}

				#ifdef USE_STD_MAP
				RealTimeDbDict::iterator j =  databases.find(token);

				if(!(j == databases.end()))
				{
					res &= (*j).second->Connect (host, dbname, DbUserName, DbPassword, is_low_freq);
				}
				else
				{
					res = false;
				}
				#else

				#endif
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
void CloseRealTimeConnections ()
{
	IT_IT("CloseRealTimeConnections");

	#ifdef USE_STD_MAP
	for(RealTimeDbDict::iterator it = databases.begin(); !(it == databases.end()); it++)
	{
		(*it).second->Disconnect();

		delete ((*it).second);

		databases.erase(it);
	}
	#else
	#endif

};


void DisconnectFromRealTimeDatabases()
{
	IT_IT("DisconnectFromRealTimeDatabases");

	#ifdef USE_STD_MAP
	for(RealTimeDbDict::iterator it = databases.begin(); !(it == databases.end()); it++)
	{
		(*it).second->Disconnect();
	}
	#else

	#endif

};

//////////////////////////////Realtime spare database on Spare host

static RealTimeDbDict spare_databases;

//
// Access functions
// 
QSDatabase *GetSpareConfigureDb ()
{
	#ifdef USE_STD_MAP
	RealTimeDbDict::iterator j =  spare_databases.find("configdb");

	if(!(j == spare_databases.end()))
	{
		return (*j).second;
	}

	return NULL;
	#else

	return NULL;
	#endif

};

QSDatabase *GetSpareCurrentDb ()
{
	#ifdef USE_STD_MAP
	RealTimeDbDict::iterator j =  spare_databases.find("currentdb");

	if(!(j == spare_databases.end()))
	{
		return (*j).second;
	}
	return NULL;
	#else

	return NULL;
	#endif

};

QSDatabase *GetSpareResultDb ()
{
	#ifdef USE_STD_MAP
	RealTimeDbDict::iterator j =  spare_databases.find("resultsdb");

	if(!(j == spare_databases.end()))
	{
		return (*j).second;
	}
	return NULL;
	#else

	return NULL;
	#endif

};

RealTimeDbDict& GetSpareRealTimeDbDict()
{
	return spare_databases;
};
//
//

//
// return true on success else false
//
bool OpenSpareRealTimeConnections()	// open the connections with the servers
{
	IT_IT("OpenSpareRealTimeConnections");

	bool res = true;

	QString spare_val;

	QString scada_ini_file = GetScadaHomeDirectory() + "\\project\\scada.ini";

	Inifile scada_iniFile((const char*)scada_ini_file);

	if(scada_iniFile.find("spare","redundancy"))
	{
		spare_val = scada_iniFile.find("spare","redundancy");
	}

	if(atoi((const char*)spare_val) == 1)
	{
	
		QString DbUserName;
		QString DbPassword;
		
		QString ini_file = GetScadaHomeDirectory() + "\\project\\spare_realtimedb.ini";
		Inifile iniFile((const char*)ini_file);

		if( iniFile.find("user","rtsqlserver") &&
			iniFile.find("password","rtsqlserver") )
		{
			DbUserName = iniFile.find("user","rtsqlserver");
			DbPassword = iniFile.find("password","rtsqlserver");
		}
		else
		{
			res = false;
		}

		char buff[INIFILE_MAX_LINELEN+2];
		char* token;
		char seps[]   = " ,\t\n";

		bool is_low_freq = true;

		//open connection with spare_databases
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

					#ifdef USE_STD_MAP
					RealTimeDbDict::value_type pr(dbname,new REALTIME_DB());
					spare_databases.insert(pr); // put in the dictionary

					if(!strcmp("1",iniFile.find("is_low_freq",token)))
					{
						is_low_freq =  true;
					}
					else
					{
						is_low_freq =  false;				
					}

					RealTimeDbDict::iterator j =  spare_databases.find(token);

					if(!(j == spare_databases.end()))
					{
						res &= (*j).second->Connect (host, dbname, DbUserName, DbPassword, is_low_freq);
					}
					else
					{
						res = false;
					}
					
					#else

					#endif

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
			#ifdef USE_STD_MAP
			for(RealTimeDbDict::iterator it = spare_databases.begin(); it != spare_databases.end(); it++)
			{
				(*it).second->Disconnect();
				delete ((*it).second);

				spare_databases.erase(it);
			}
			#else

			#endif
		}
	}
	else
	{
		res = false;
	}

	return res;
};


//
// return true on success else false
//
bool ConnectToSpareRealTimeDatabases()
{
	IT_IT("ConnectToSpareRealTimeDatabases");

	bool res = true;  
	QString DbUserName;
	QString DbPassword;
	
	QString ini_file = GetScadaHomeDirectory() + "\\project\\spare_realtimedb.ini";
	Inifile iniFile((const char*)ini_file);

	if( iniFile.find("user","rtsqlserver") &&
		iniFile.find("password","rtsqlserver") )
	{
		DbUserName = iniFile.find("user","rtsqlserver");
		DbPassword = iniFile.find("password","rtsqlserver");
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

				//RealTimeDbDict::value_type pr(dbname,new REALTIME_DB());
				//spare_databases.insert(pr); // put in the dictionary

				if(!strcmp("1",iniFile.find("is_low_freq",token)))
				{
					is_low_freq =  true;
				}
				else
				{
					is_low_freq =  false;				
				}

				#ifdef USE_STD_MAP
				RealTimeDbDict::iterator j =  spare_databases.find(token);

				if(!(j == spare_databases.end()))
				{
					res &= (*j).second->Connect (host, dbname, DbUserName, DbPassword, is_low_freq);
				}
				else
				{
					res = false;
				}
				#else

				#endif
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
void CloseSpareRealTimeConnections ()
{
	IT_IT("CloseSpareRealTimeConnections");

	#ifdef USE_STD_MAP
	for(RealTimeDbDict::iterator it = spare_databases.begin(); !(it == spare_databases.end()); it++)
	{
		(*it).second->Disconnect();

		delete ((*it).second);

		spare_databases.erase(it);
	}
	#else

	#endif
};


void DisconnectFromSpareRealTimeDatabases()
{
	IT_IT("DisconnectFromSpareRealTimeDatabases");

	#ifdef USE_STD_MAP
	for(RealTimeDbDict::iterator it = spare_databases.begin(); !(it == spare_databases.end()); it++)
	{
		(*it).second->Disconnect();
	}
	#else

	#endif
};

