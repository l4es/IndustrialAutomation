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

#ifdef _WIN32

#include <crtdbg.h>
#include <stdio.h>
#include <sqlite3.h>
#include "stdint.h"
#include "iec104types.h"
#include "iec_item.h"
#include "itrace.h"	
#include "modbus_db.h"
#include "modbus_imp.h"
#include "stdlib.h"
#include "string.h"
#endif // _WIN32

static gl_row_counter = 0;
static gl_column_counter = 0;
static struct modbusDbRecord* gl_Config_db = 0;

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
				//column 1 in table modbus_table
				//slave ID
				if(argv[i] != NULL)
					gl_Config_db[gl_row_counter].slave_id = atoi(argv[i]);
			}
			break;
			case 1:
			{
				//column 2 in table modbus_table
				//modbus_function_read
				if(argv[i] != NULL)
					gl_Config_db[gl_row_counter].modbus_function_read = atoi(argv[i]);
			}
			break;
			case 2:
			{
				//column 3 in table modbus_table
				//modbus_function_write
				if(argv[i] != NULL)
					gl_Config_db[gl_row_counter].modbus_function_write = atoi(argv[i]);
			}
			break;
			case 3:
			{
				//column 4 in table modbus_table
				//modbus_address
				if(argv[i] != NULL)
					gl_Config_db[gl_row_counter].modbus_address = atoi(argv[i]);
			}
			break;
			case 4:
			{
				//column 5 in table modbus_table
				//modbus_type expressed like an OPC type
				if(argv[i] != NULL)
				{
					if(strcmp(argv[i], "VT_BOOL") == 0)
					{
						gl_Config_db[gl_row_counter].modbus_type = VT_BOOL;
					}
					else if(strcmp(argv[i], "VT_I2") == 0)
					{
						gl_Config_db[gl_row_counter].modbus_type = VT_I2;
					}
					else if(strcmp(argv[i], "VT_UI2") == 0)
					{
						gl_Config_db[gl_row_counter].modbus_type = VT_UI2;
					}
					else if(strcmp(argv[i], "VT_I4") == 0)
					{
						gl_Config_db[gl_row_counter].modbus_type = VT_I4;
					}
					else if(strcmp(argv[i], "VT_UI4") == 0)
					{
						gl_Config_db[gl_row_counter].modbus_type = VT_UI4;
					}
					else if(strcmp(argv[i], "VT_R4") == 0)
					{
						gl_Config_db[gl_row_counter].modbus_type = VT_R4;
					}
					else if(strcmp(argv[i], "VT_R4SWAP") == 0)
					{
						gl_Config_db[gl_row_counter].modbus_type = VT_R4SWAP;
					}
					else if(strcmp(argv[i], "VT_R8") == 0)
					{
						gl_Config_db[gl_row_counter].modbus_type = VT_R8;
					}
					else if(strcmp(argv[i], "VT_I1") == 0)
					{
						gl_Config_db[gl_row_counter].modbus_type = VT_I1;
					}
					else if(strcmp(argv[i], "VT_UI1") == 0)
					{
						gl_Config_db[gl_row_counter].modbus_type = VT_UI1;
					}
					else if(strcmp(argv[i], "VT_I8") == 0)
					{
						gl_Config_db[gl_row_counter].modbus_type = VT_I8;
					}
					else if(strcmp(argv[i], "VT_UI8") == 0)
					{
						gl_Config_db[gl_row_counter].modbus_type = VT_UI8;
					}
					else
					{
						gl_Config_db[gl_row_counter].modbus_type = VT_ERROR;
					}
				}
				else
				{
					gl_Config_db[gl_row_counter].modbus_type = VT_ERROR;
				}
			}
			break;
			case 5:
			{
				//column 6 in table modbus_table
				//ioa_control_center Unstructured
				if(argv[i] != NULL)
					gl_Config_db[gl_row_counter].ioa_control_center = atoi(argv[i]);
			}
			break;
			case 6:
			{
				//column 7 in table modbus_table
				//deadband
				if(argv[i] != NULL)
					gl_Config_db[gl_row_counter].deadband = (float)atof(argv[i]);
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

#define MAX_CONFIGURABLE_MODBUS_ITEMIDS 30000

int modbus_imp::AddItems(void)
{
	IT_IT("modbus_imp::AddItems");

	sqlite3 *db;
	char *zErrMsg = 0;
	int rc;
	char line_number[10];

	itoa(lineNumber, line_number, 10);
	
	#ifdef WIN32
	if(GetModuleFileName(NULL, database_name, _MAX_PATH))
	{
		*(strrchr(database_name, '\\')) = '\0';        // Strip \\filename.exe off path
		*(strrchr(database_name, '\\')) = '\0';        // Strip \\bin off path
	}
	#endif

	strcat(database_name, "\\project\\");
	strcat(database_name, "modbus_database");
	strcat(database_name, line_number);
	strcat(database_name, ".db");

	rc = sqlite3_open(database_name, &db);

	if(rc)
	{
	  fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
	  fflush(stderr);
	  sqlite3_close(db);
	  IT_EXIT;
	  return 1;
	}

	g_dwNumItems = MAX_CONFIGURABLE_MODBUS_ITEMIDS;
	
	Config_db = (struct modbusDbRecord*)calloc(1, g_dwNumItems*sizeof(struct modbusDbRecord));

	gl_Config_db = Config_db;

	gl_row_counter = 0;

	rc = sqlite3_exec(db, "select * from modbus_table;", db_callback, 0, &zErrMsg);

	if(rc != SQLITE_OK)
	{
	  fprintf(stderr, "SQL error: %s\n", zErrMsg);
	  fflush(stderr);
	  sqlite3_free(zErrMsg);
	}

	sqlite3_close(db);

	db_n_rows = gl_row_counter;
	db_m_columns = gl_column_counter;

	if(db_n_rows == 0)
	{
		fprintf(stderr, "Error: db_n_rows = %d\n", db_n_rows);
		fflush(stderr);
		IT_EXIT;
		return 1;
	}
	
	g_dwNumItems = db_n_rows;


	return(0);
}
