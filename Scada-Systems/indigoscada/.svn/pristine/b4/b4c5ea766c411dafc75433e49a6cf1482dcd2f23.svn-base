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

#include <crtdbg.h>
#include <stdio.h>
#include <sqlite3.h>
#include "stdint.h"
#include "iec104types.h"
#include "iec_item.h"
#include "itrace.h"	
#include "rfc1006_item.h"
#include "rfc1006_imp.h"
#include "stdlib.h"
#include "string.h"
#endif // _WIN32

static gl_row_counter = 0;
static gl_column_counter = 0;
static struct rfc1006Item* gl_Config_db = 0;

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
				//column 1 in table rfc_1006_table
				//item_id
				strcpy(gl_Config_db[gl_row_counter].name, argv[i]);
			}
			break;
			case 1:
			{
				//column 2 in table rfc_1006_table
				//type, could be s7_u8, s7_s8, s7_u16, s7_s16, s7_u32, s7_s32, s7_float, s7_string, s7_bit
				strcpy(gl_Config_db[gl_row_counter].type, argv[i]);
			}
			break;
			case 2:
			{
				//column 3 in table rfc_1006_table
				//datablock
				gl_Config_db[gl_row_counter].datablock = atoi(argv[i]);
			}
			break;
			case 3:
			{
				//column 4 in table rfc_1006_table
				//offset_addr
				gl_Config_db[gl_row_counter].offset_addr = atoi(argv[i]);
			}
			break;
			case 4:
			{
				//column 5 in table rfc_1006_table
				//bit
				gl_Config_db[gl_row_counter].bit = atoi(argv[i]);
			}
			break;
			case 5:
			{
				//column 6 in table rfc_1006_table
				//item_id
				gl_Config_db[gl_row_counter].writeable = atoi(argv[i]);
			}
			break;
			case 6:
			{
				//column 7 in table rfc_1006_table
				//ioa_control_center Unstructured
				gl_Config_db[gl_row_counter].ioa_control_center = atoi(argv[i]);
			}
			break;
			case 7:
			{
				//column 8 in table rfc_1006_table
				//iec_type
				if(strcmp(argv[i], "M_ME_TF_1") == 0)
				{
					gl_Config_db[gl_row_counter].iec_type = M_ME_TF_1;
				}
				else if(strcmp(argv[i], "M_SP_TB_1") == 0)
				{
					gl_Config_db[gl_row_counter].iec_type = M_SP_TB_1;
				}
				else if(strcmp(argv[i], "M_DP_TB_1") == 0)
				{
					gl_Config_db[gl_row_counter].iec_type = M_DP_TB_1;
				}
				else if(strcmp(argv[i], "C_DC_NA_1") == 0)
				{
					gl_Config_db[gl_row_counter].iec_type = C_DC_NA_1;
				}
				else if(strcmp(argv[i], "C_SC_NA_1") == 0)
				{
					gl_Config_db[gl_row_counter].iec_type = C_SC_NA_1;
				}
				else if(strcmp(argv[i], "M_IT_TB_1") == 0)
				{
					gl_Config_db[gl_row_counter].iec_type = M_IT_TB_1;
				}
				else if(strcmp(argv[i], "M_ME_TN_1") == 0)
				{
					gl_Config_db[gl_row_counter].iec_type = M_ME_TN_1;
				}
				else
				{
					fprintf(stderr,"IEC type %s from I/O list NOT supported\n", argv[i]);
					fflush(stderr);
					//ExitProcess(0);
				}
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

#define MAX_CONFIGURABLE_RFC10006_ITEMIDS 30000

int rfc1006_imp::AddItems(void)
{
	IT_IT("rfc1006_imp::AddItems");

	sqlite3 *db;
	char *zErrMsg = 0;
	int rc;
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
	strcat(db_name, plc_server_prog_id);
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

	g_dwNumItems = MAX_CONFIGURABLE_RFC10006_ITEMIDS;
	
	Config_db = (struct rfc1006Item*)calloc(1, g_dwNumItems*sizeof(struct rfc1006Item));

	gl_Config_db = Config_db;

	gl_row_counter = 0;

	rc = sqlite3_exec(db, "select * from rfc_1006_table;", db_callback, 0, &zErrMsg);

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


	return(0);
}

void rfc1006_imp::CreateSqlConfigurationFile(char* sql_file_name, char* opc_path)
{
	//Make browsing of S7 PLC server for available ItemID's

/*	
	HRESULT hr = 0;
	FILE *dump = NULL;
	char iec_type[100];
	char opc_type[100];
	char program_path[_MAX_PATH];
	double max = 0.0;
	double min = 0.0;
	
	iec_type[0] = '\0';
	opc_type[0] = '\0';
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

	fprintf(dump, "create table rfc_1006_table(opc_server_item_id varchar(150), ioa_control_center varchar(150), iec_type varchar(150), readable varchar(150), writeable varchar(150), HiHiLimit varchar(150), LoLoLimit varchar(150), opc_type varchar(150));\n");
	fflush(dump);
							
	if(dump == NULL)
	{
		fprintf(stderr,"Error opening file: %s\n", sql_file_path);
		fflush(stderr);
		IT_EXIT;
		return;
	}
	
	Item = (struct rfc1006Item*)calloc(1, g_dwNumItems*sizeof(struct rfc1006Item));

	for(int i = 0; i < g_dwNumItems; i++)
	{
	
		fprintf(dump, "insert into rfc_1006_table values('%s', '%d', '%s', '%d', '%d', '%lf', '%lf', '%s');\n", 
		Item[nTestItem].spname, nTestItem + 1, iec_type, readable,	writeable,	max, min, opc_type);
		fflush(dump);
	}
	
	////////////////////////////end dumping one record/////////////////////////////////////////////

	nTestItem++;

	if(nTestItem >= MAX_CONFIGURABLE_RFC10006_ITEMIDS)
	{ 
		printf("Warning! Increase ""MAX_CONFIGURABLE_RFC10006_ITEMIDS"" items\n");
		break;
	}

	if(dump)
	{
		fclose(dump);
		dump = NULL;
	}

	fprintf(stderr,"PLC browsing is complete!\n");

	fflush(stderr);
*/
	
	IT_EXIT;
}
