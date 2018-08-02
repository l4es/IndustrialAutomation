/*
 *                         IndigoSCADA
 *
 *   This software and documentation are Copyright 2002 to 2011 Enscada
 *   Limited and its licensees. All rights reserved. See file:
 *
 *                     $HOME/LICENSE 
 *
 *   for full copyright notice and license terms. 
 *
 */
#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <shellapi.h>
#include <crtdbg.h>
#include "opc_client_ae_app.h"
#include "itrace.h"	
#endif // _WIN32

#include <stdio.h>
#include <sqlite3.h>

static gl_row_counter = 0;
static gl_column_counter = 0;
static struct structEvent* gl_config_db = 0;

static int db_callback(void *NotUsed, int argc, char **argv, char **azColName)
{
	int i;

	gl_column_counter = argc;

	for(i = 0; i < argc; i++)
	{
		fprintf(stderr, "%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
		fflush(stderr);

		switch(i)
		{
			case 0:
			{
				//column 1 in table opc_client_ae_table
				//AE Event description
				strcpy(gl_config_db[gl_row_counter].Event_decription, argv[i]);
			}
			break;
			case 1:
			{
				//column 2 in table opc_client_ae_table
				////AE Source
				strcpy(gl_config_db[gl_row_counter].Source, argv[i]);	
			}
			break;
			case 2:
			{
				//column 3 in table opc_client_ae_table
				//IOA unstructured
				gl_config_db[gl_row_counter].ioa_control_center = atoi(argv[i]);
			}
			break;
			case 3:
			{
				//column 4 in table opc_client_ae_table
				//AE area
				strcpy(gl_config_db[gl_row_counter].Area, argv[i]);
			}	
			break;
			case 4:
			{
				//column 5 in table opc_client_ae_table
				//AE Number
				gl_config_db[gl_row_counter].Number = atoi(argv[i]);
			}	
			break;
			default:
			break;
		}
	}

	//ended to read a record
	gl_row_counter++;

	fprintf(stderr, "\n");
	fflush(stderr);
	return 0;
}

#define MAX_CONFIGURABLE_OPC_ALARMS_AND_EVENTS 30000

int Opc_client_ae_imp::load_database(void)
{
	IT_IT("Opc_client_ae_imp::load_database");
	
	IEnumString* pEnumString = NULL;
		
	USES_CONVERSION;
	
	LPOLESTR pszName = NULL;
	ULONG count = 0;
	ULONG nCount = 0;

	sqlite3 *db;
	char *zErrMsg = 0;
	int rc, n_rows, m_columns;

	char db_name[100];

	#ifdef WIN32
	if(GetModuleFileName(NULL, db_name, _MAX_PATH))
	{
		*(strrchr(db_name, '\\')) = '\0';        // Strip \\filename.exe off path
		*(strrchr(db_name, '\\')) = '\0';        // Strip \\bin off path
	}
	#endif

	strcat(db_name, "\\project\\");
	strcat(db_name, opc_server_prog_id);
	strcat(db_name, ".db");

	rc = sqlite3_open(db_name, &db);

	if(rc)
	{
	  fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
	  fflush(stderr);
	  sqlite3_close(db);
	  IT_EXIT;
	  return 1;
	}

	g_dwNumAlarmsEvents = MAX_CONFIGURABLE_OPC_ALARMS_AND_EVENTS;

	Config_db = (struct structEvent*)calloc(1, g_dwNumAlarmsEvents*sizeof(struct structEvent));

	gl_config_db = Config_db;

	gl_row_counter = 0;

	rc = sqlite3_exec(db, "select * from opc_client_ae_table;", db_callback, 0, &zErrMsg);

	if(rc != SQLITE_OK)
	{
	  fprintf(stderr, "SQL error: %s\n", zErrMsg);
	  fflush(stderr);
	  sqlite3_free(zErrMsg);
	}

	sqlite3_close(db);

	n_rows = gl_row_counter;
	m_columns = gl_column_counter;

	if(n_rows == 0)
	{
		fprintf(stderr, "Error: n_rows = %d\n", n_rows);
		fflush(stderr);
		IT_EXIT;
		return 1;
	}

	fprintf(stderr,"rows = %d, columns = %d\n", n_rows, m_columns);
	fflush(stderr);

	g_dwNumAlarmsEvents = n_rows;

	fprintf(stderr, "g_dwNumAlarmsEvents = %d\n", g_dwNumAlarmsEvents);
	fflush(stderr);

	IT_EXIT;
	return 0;
}

