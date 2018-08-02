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
#endif // _WIN32
#include <stdio.h>
#include <stdio.h>
#include <sqlite3.h>
#include "stdint.h"
#include "iec104types.h"
#include "iec_item.h"
#include "itrace.h"	
#include "mqtt_client_app.h"
#include "GeneralHashFunctions.h"

static gl_row_counter = 0;
static gl_column_counter = 0;
static struct structItem* gl_Config_db = 0;

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
				//column 1 in table mqtt_client_table
				//mqtt_topic
				if(argv[i] != NULL)
					strcpy(gl_Config_db[gl_row_counter].spname, argv[i]);
			}
			break;
			case 1:
			{
				//column 2 in table mqtt_client_table
				//ioa_control_center Unstructured
				if(argv[i] != NULL)
					gl_Config_db[gl_row_counter].ioa_control_center = atoi(argv[i]);
			}
			break;
			case 2:
			{
				if(argv[i] != NULL)
				{
					//column 3 in table mqtt_client_table
					//iec_type
					if(strcmp(argv[i], "M_ME_TF_1") == 0)
					{
						gl_Config_db[gl_row_counter].io_list_iec_type = M_ME_TF_1;
					}
					else if(strcmp(argv[i], "M_SP_TB_1") == 0)
					{
						gl_Config_db[gl_row_counter].io_list_iec_type = M_SP_TB_1;
					}
					else if(strcmp(argv[i], "M_DP_TB_1") == 0)
					{
						gl_Config_db[gl_row_counter].io_list_iec_type = M_DP_TB_1;
					}
					else if(strcmp(argv[i], "C_DC_NA_1") == 0)
					{
						gl_Config_db[gl_row_counter].io_list_iec_type = C_DC_NA_1;
					}
					else if(strcmp(argv[i], "C_SC_NA_1") == 0)
					{
						gl_Config_db[gl_row_counter].io_list_iec_type = C_SC_NA_1;
					}
					else if(strcmp(argv[i], "M_IT_TB_1") == 0)
					{
						gl_Config_db[gl_row_counter].io_list_iec_type = M_IT_TB_1;
					}
					else if(strcmp(argv[i], "M_ME_TN_1") == 0)
					{
						gl_Config_db[gl_row_counter].io_list_iec_type = M_ME_TN_1;
					}
					else
					{
						fprintf(stderr,"IEC type %s from I/O list NOT supported\n", argv[i]);
						fflush(stderr);
						//ExitProcess(0);
					}
				}
			}	
			break;
			case 3:
			{
				//column 4 in table mqtt_client_table
				//readable
				if(argv[i] != NULL)
					gl_Config_db[gl_row_counter].readable = atoi(argv[i]);
			}
			break;
			case 4:
			{
				//column 5 in table mqtt_client_table
				//writeable
				if(argv[i] != NULL)
					gl_Config_db[gl_row_counter].writeable = atoi(argv[i]);
			}
			break;
			case 5:
			{
				//column 6 in table mqtt_client_table
				//HiHiLimit
				if(argv[i] != NULL)
					gl_Config_db[gl_row_counter].max_measure = (float)atof(argv[i]);
			}
			break;
			case 6:
			{
				//column 7 in table mqtt_client_table
				//LoLoLimit
				if(argv[i] != NULL)
					gl_Config_db[gl_row_counter].min_measure = (float)atof(argv[i]);				
			}
			break;
			case 7:
			{
				//column 8 in table mqtt_client_table
				//opc_type in OPC format 
				if(argv[i] != NULL)
					strcpy(gl_Config_db[gl_row_counter].opc_type, argv[i]);
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

#define MAX_CONFIGURABLE_OPC_ITEMIDS 30000

int MQTT_client_imp::AddItems()
{
	IT_IT("MQTT_client_imp::AddItems");

	sqlite3 *db;
	char *zErrMsg = 0;
	int rc, j;
	int n_rows = 0;
	int m_columns = 0;

	char db_name[100];

	#ifdef WIN32
	if(GetModuleFileName(NULL, db_name, _MAX_PATH))
	{
		*(strrchr(db_name, '\\')) = '\0';        // Strip \\filename.exe off path
		*(strrchr(db_name, '\\')) = '\0';        // Strip \\bin off path
	}
	#endif

	strcat(db_name, "\\project\\");
	strcat(db_name, BrokerHostName);
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

	g_dwNumItems = MAX_CONFIGURABLE_OPC_ITEMIDS;
	
	Config_db = (struct structItem*)calloc(1, g_dwNumItems*sizeof(struct structItem));

	gl_Config_db = Config_db;

	gl_row_counter = 0;

	rc = sqlite3_exec(db, "select * from mqtt_client_table;", db_callback, 0, &zErrMsg);

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
	
	g_dwNumItems = n_rows;

	Item = (struct structItem*)calloc(1, g_dwNumItems*sizeof(struct structItem));

	//|----------------MQTT Topic -------------|---ioa_control_center---|---io_list_iec_type---|--min--|--max--|
	//	Simulated Card.Simulated Node.Random.Ra			1					31			
	//	Simulated Card.Simulated Node.Random.Rb			2					30					Engineering values (min and max)
	//	Simulated Card.Simulated Node.Random.Rc			3					46					of measure or set point
	
	
	//////dump for debug////////////////////////////////////////////////////////////////////////////////
	//fp = fopen("C:\\scada\\logs\\Config_db.txt", "w"); // only for test

	//for(j = 0; j < n_rows; j++)
	//{
	//	fprintf(fp, "%s\n" , Config_db[j].spname); // only for test
	//}

	//fclose(fp);
	///////////////////////////////////////////////////////////////////////////////////////////////////
	
	///Loading items from DA server
	
	//fp = fopen("C:\\scada\\logs\\opc_itemid_loaded.txt", "w"); // only for test

	// loop until all items are added
	
	int nTestItem = 0; // how many items there are
			
	for(j = 0; j < n_rows; j++) //loop over each record
	{
		strcpy(Item[nTestItem].spname, Config_db[j].spname);

		//fprintf(stderr,"Item[nTestItem].spname = %s\n", Item[nTestItem].spname);
		//fflush(stderr);
					
		Item[nTestItem].ioa_control_center = Config_db[j].ioa_control_center;
		Item[nTestItem].io_list_iec_type = Config_db[j].io_list_iec_type;

		Item[nTestItem].hash_key = 0;

		if(strlen(Item[nTestItem].spname) > 0)
		{
			Item[nTestItem].hash_key = APHash(Item[nTestItem].spname, strlen(Item[nTestItem].spname));
		}

		//ended to read a record

		nTestItem++;

		fprintf(stderr,"nTestItem = %d\r", nTestItem);
		fflush(stderr);
	}
		
	//fclose(fp); //only for test
		
	fprintf(stderr,"MQTT topics: %d\n", g_dwNumItems);
	fflush(stderr);

	if(g_dwNumItems <= 0)
	{
		IT_EXIT;
		return 1;
	}

	IT_EXIT;
	return(0);
}

void MQTT_client_imp::CreateSqlConfigurationFile(char* sql_file_name)
{
	//Subcribe to MQTT broker for available Topics

	char program_path[_MAX_PATH];
		
	program_path[0] = '\0';

	if(GetModuleFileName(NULL, program_path, _MAX_PATH))
	{
		*(strrchr(program_path, '\\')) = '\0';        // Strip \\filename.exe off path
		*(strrchr(program_path, '\\')) = '\0';        // Strip \\bin off path
    }

	char sql_file_path[MAX_PATH];

	strcpy(sql_file_path, program_path);

	strcat(sql_file_path, "\\project\\"); 

	strcat(sql_file_path, sql_file_name);
	
	dump = fopen(sql_file_path, "w");

	fprintf(dump, "create table mqtt_client_table(mqtt_topic varchar(150), ioa_control_center varchar(150), iec_type varchar(150), readable varchar(150), writeable varchar(150), HiHiLimit varchar(150), LoLoLimit varchar(150), mqtt_type varchar(150));\n");
	fflush(dump);
							
	if(dump == NULL)
	{
		fprintf(stderr,"Error opening file: %s\n", sql_file_path);
		fflush(stderr);
		IT_EXIT;
		return;
	}
	
	IT_EXIT;
}
