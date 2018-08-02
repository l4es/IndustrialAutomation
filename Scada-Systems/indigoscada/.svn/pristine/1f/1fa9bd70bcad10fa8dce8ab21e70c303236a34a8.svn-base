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
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <shellapi.h>
#include <crtdbg.h>
#include <stdio.h>
#include <stdio.h>
#include <sqlite3.h>
#include "stdint.h"
#include "iec104types.h"
#include "iec_item.h"
#include "itrace.h"	
#include "client.h"
#endif // _WIN32

static gl_row_counter = 0;
static gl_column_counter = 0;
static struct structItem* gl_Config_db = NULL;

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
				//column 1 in table client_table
				//server_item_id
				if(argv[i] != NULL)
					strcpy(gl_Config_db[gl_row_counter].spname, argv[i]);
			}
			break;
			case 1:
			{
				//column 2 in table client_table
				//ioa_control_center Unstructured
				if(argv[i] != NULL)
					gl_Config_db[gl_row_counter].ioa_control_center = atoi(argv[i]);
			}
			break;
			case 2:
			{
				if(argv[i] != NULL)
				{
					//column 3 in table client_table
					//iec_104_type
					if(strcmp(argv[i], "M_ME_TF_1") == 0)
					{
						gl_Config_db[gl_row_counter].iec_104_type = M_ME_TF_1;
					}
					else if(strcmp(argv[i], "M_SP_TB_1") == 0)
					{
						gl_Config_db[gl_row_counter].iec_104_type = M_SP_TB_1;
					}
					else if(strcmp(argv[i], "M_DP_TB_1") == 0)
					{
						gl_Config_db[gl_row_counter].iec_104_type = M_DP_TB_1;
					}
					else if(strcmp(argv[i], "C_DC_NA_1") == 0)
					{
						gl_Config_db[gl_row_counter].iec_104_type = C_DC_NA_1;
					}
					else if(strcmp(argv[i], "C_SC_NA_1") == 0)
					{
						gl_Config_db[gl_row_counter].iec_104_type = C_SC_NA_1;
					}
					else if(strcmp(argv[i], "M_IT_TB_1") == 0)
					{
						gl_Config_db[gl_row_counter].iec_104_type = M_IT_TB_1;
					}
					else if(strcmp(argv[i], "M_ME_TN_1") == 0)
					{
						gl_Config_db[gl_row_counter].iec_104_type = M_ME_TN_1;
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
				//column 4 in table client_table
				//HiHiLimit
				if(argv[i] != NULL)
					gl_Config_db[gl_row_counter].max_measure = (float)atof(argv[i]);
			}
			break;
			case 4:
			{
				//column 5 in table client_table
				//LoLoLimit
				if(argv[i] != NULL)
					gl_Config_db[gl_row_counter].min_measure = (float)atof(argv[i]);				
			}
			break;
			case 5:
			{
				//column 6 in table client_table
				//type in IEC61850 type 
				if(argv[i] != NULL)
				{
					if(strcmp(argv[i], "MMS_ARRAY") == 0)
					{
						gl_Config_db[gl_row_counter].iec61850_type = MMS_ARRAY;
					}
					else if(strcmp(argv[i], "MMS_STRUCTURE") == 0)
					{
						gl_Config_db[gl_row_counter].iec61850_type = MMS_STRUCTURE;
					}
					else if(strcmp(argv[i], "MMS_BOOLEAN") == 0)
					{
						gl_Config_db[gl_row_counter].iec61850_type = MMS_BOOLEAN;
					}
					else if(strcmp(argv[i], "MMS_BIT_STRING") == 0)
					{
						gl_Config_db[gl_row_counter].iec61850_type = MMS_BIT_STRING;
					}
					else if(strcmp(argv[i], "MMS_INTEGER") == 0)
					{
						gl_Config_db[gl_row_counter].iec61850_type = MMS_INTEGER;
					}
					else if(strcmp(argv[i], "MMS_UNSIGNED") == 0)
					{
						gl_Config_db[gl_row_counter].iec61850_type = MMS_UNSIGNED;
					}
					else if(strcmp(argv[i], "MMS_FLOAT") == 0)
					{
						gl_Config_db[gl_row_counter].iec61850_type = MMS_FLOAT;
					}
					else if(strcmp(argv[i], "MMS_OCTET_STRING") == 0)
					{
						gl_Config_db[gl_row_counter].iec61850_type = MMS_OCTET_STRING;
					}
					else if(strcmp(argv[i], "MMS_VISIBLE_STRING") == 0)
					{
						gl_Config_db[gl_row_counter].iec61850_type = MMS_VISIBLE_STRING;
					}
					else if(strcmp(argv[i], "MMS_GENERALIZED_TIME") == 0)
					{
						gl_Config_db[gl_row_counter].iec61850_type = MMS_GENERALIZED_TIME;
					}
					else if(strcmp(argv[i], "MMS_BINARY_TIME") == 0)
					{
						gl_Config_db[gl_row_counter].iec61850_type = MMS_BINARY_TIME;
					}
					else if(strcmp(argv[i], "MMS_BCD") == 0)
					{
						gl_Config_db[gl_row_counter].iec61850_type = MMS_BCD;
					}
					else if(strcmp(argv[i], "MMS_OBJ_ID") == 0)
					{
						gl_Config_db[gl_row_counter].iec61850_type = MMS_OBJ_ID;
					}
					else if(strcmp(argv[i], "MMS_STRING") == 0)
					{
						gl_Config_db[gl_row_counter].iec61850_type = MMS_STRING;
					}
					else if(strcmp(argv[i], "MMS_UTC_TIME") == 0)
					{
						gl_Config_db[gl_row_counter].iec61850_type = MMS_UTC_TIME;
					}
					else
					{
						fprintf(stderr,"IEC61850 type %s from I/O list NOT supported\n", argv[i]);
						fflush(stderr);
					}
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

#define MAX_CONFIGURABLE_ITEMIDS 30000

int IEC61850_client_imp::AddItems()
{
	IT_IT("IEC61850_client_imp::AddItems");

	sqlite3 *db;
	char *zErrMsg = 0;
	int rc, j;
	int n_rows = 0;
	int m_columns = 0;

	char db_name[MAX_PATH];

	#ifdef WIN32
	if(GetModuleFileName(NULL, db_name, _MAX_PATH))
	{
		*(strrchr(db_name, '\\')) = '\0';        // Strip \\filename.exe off path
		*(strrchr(db_name, '\\')) = '\0';        // Strip \\bin off path
	}
	#endif

	strcat(db_name, "\\project\\");
	strcat(db_name, mmsDomain);
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

	g_dwNumItems = MAX_CONFIGURABLE_ITEMIDS;
	
	gl_Config_db = (struct structItem*)calloc(1, g_dwNumItems*sizeof(struct structItem));

	gl_row_counter = 0;

	rc = sqlite3_exec(db, "select * from client_table;", db_callback, 0, &zErrMsg);

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

	//|----------------ItemID-----------------|---ioa_control_center---|---iec_104_type---|--min--|--max--|--hClient--
	//	LLN0$ST$Beh$									1					31			
	//	LPHD1$ST$Proxy$									2					30					Engineering values (min and max)
	//	DGEN1$ST$OpTmh$									3					46					of measure or set point
	
	
	//////dump for debug////////////////////////////////////////////////////////////////////////////////
	//fp = fopen("C:\\scada\\logs\\Config_db.txt", "w"); // only for test

	//for(j = 0; j < n_rows; j++)
	//{
	//	fprintf(fp, "%s\n" , gl_Config_db[j].spname); // only for test
	//}

	//fclose(fp);
	///////////////////////////////////////////////////////////////////////////////////////////////////
	
	///Loading items from  server
	
	//fp = fopen("C:\\scada\\logs\\itemid_loaded.txt", "w"); // only for test

	// loop until all items are added
	
	int nTestItem; // how many items there are
	int error_add_items = 0;
	int found_duplicated_item_id = 0;
	
	nTestItem = 0;
	
	for(j = 0; j < n_rows; j++) //loop over each record
	{
		error_add_items = 0;

		found_duplicated_item_id = 0;
			
		strcpy(Item[nTestItem].spname, gl_Config_db[j].spname);

		//fprintf(stderr,"Item[nTestItem].spname = %s\n", Item[nTestItem].spname);
		//fflush(stderr);

		//Look if the name is already loaded in the database
		
		for(int k_1 = 0; k_1 < nTestItem + 1; k_1++)
		{
			if(strcmp(Item[k_1].spname, Item[nTestItem].spname) == 0)
			{
				found_duplicated_item_id++; 
			}
		}

		if(found_duplicated_item_id == 2)
		{
			g_dwNumItems--;
			continue;
			//the current itemID name is already loaded in the  server
			//so nTestItem is NOT incremented
		}
				
		Item[nTestItem].hClient = nTestItem + 1;

		//fprintf(fp, "%s %d" ,Item[nTestItem].spname, g_dwClientHandle - 1); // only for test
				
		if(error_add_items == 0)
		{
			//fprintf(fp, " Added\n"); // only for test
		}
			
		Item[nTestItem].ioa_control_center = gl_Config_db[j].ioa_control_center;
		Item[nTestItem].iec_104_type = gl_Config_db[j].iec_104_type;

		//ended to read a record

		nTestItem++;

		fprintf(stderr,"nTestItem = %d\r", nTestItem);
		fflush(stderr);
	}
		
	//fclose(fp); //only for test

	fprintf(stderr, "g_dwNumItems = %d\n", g_dwNumItems);
	fflush(stderr);
	
	if(g_dwNumItems <= 0)
	{
		IT_EXIT;
		return 1;
	}

	if(gl_Config_db)
	{
		free(gl_Config_db);
	}

	IT_EXIT;
	return(0);
}

void IEC61850_client_imp::CreateSqlConfigurationFile(void)
{
	//Make browsing of server for available ItemID's
		
	FILE *dump = NULL;
	char iec_104_type[100];
	char iec61850_type[100];
	char program_path[_MAX_PATH];
	double max = 0.0;
	double min = 0.0;
	char spname[200];
	int insert_record = 0;
	
	iec_104_type[0] = '\0';
	iec61850_type[0] = '\0';
	program_path[0] = '\0';

	if(GetModuleFileName(NULL, program_path, _MAX_PATH))
	{
		*(strrchr(program_path, '\\')) = '\0';        // Strip \\filename.exe off path
		*(strrchr(program_path, '\\')) = '\0';        // Strip \\bin off path
    }

	char sql_file_path[MAX_PATH];

	strcpy(sql_file_path, program_path);

	strcat(sql_file_path, "\\project\\"); 

	strcat(sql_file_path, mmsDomain);

	strcat(sql_file_path, ".sql");
	
	dump = fopen(sql_file_path, "w");

	fprintf(dump, "create table client_table(server_item_id varchar(150), ioa_control_center varchar(150), iec_104_type varchar(150), HiHiLimit varchar(150), LoLoLimit varchar(150), iec61850type varchar(150));\n");
	fflush(dump);
							
	if(dump == NULL)
	{
		fprintf(stderr,"Error opening file: %s\n", sql_file_path);
		fflush(stderr);
		IT_EXIT;
		return;
	}

	LinkedList element = nameList;
	int elementCount = 0;
	char* str;

	iec_104_type[0] = '\0';
										
	for(int nTestItem = 0; (element = LinkedList_getNext(element)) != NULL; )
	{
		insert_record = 0;

		str = (char*) (element->data);

		strcpy(spname, str);

		typeSpec = MmsConnection_getVariableAccessAttributes(con, mmsDomain, str);

		//NOTE: keep aligned this swith with the one in pollServer

		switch(typeSpec->type)
		{
			//case MMS_ARRAY:
			//{
				//Not supported mapping at moment
			//	strcpy(iec61850_type, "MMS_ARRAY");
			//}
			//break;
			//case MMS_STRUCTURE:
			//{
			//  //Not to be mapped
			//	strcpy(iec61850_type, "MMS_STRUCTURE");
			//}
			//break;
			case MMS_BIT_STRING:
			{
				strcpy(iec61850_type, "MMS_BIT_STRING");
				insert_record = 1;
			}
			break;
			//case MMS_OCTET_STRING:
			//{
				//Not to be mapped
			//	strcpy(iec61850_type, "MMS_OCTET_STRING");
			//}
			//break;
			case MMS_GENERALIZED_TIME:
			{
				strcpy(iec61850_type, "MMS_GENERALIZED_TIME");
				insert_record = 1;
			}
			break;
			case MMS_BINARY_TIME:
			{
				strcpy(iec61850_type, "MMS_BINARY_TIME");
				insert_record = 1;
			}
			break;
			case MMS_BCD:
			{
				strcpy(iec61850_type, "MMS_BCD");
				insert_record = 1;
				nTestItem++;
			}
			break;
			//case MMS_OBJ_ID:
			//{
			//	Not to be mapped at moment
			//	strcpy(iec61850_type, "MMS_OBJ_ID");
			//}
			//break;
			//case MMS_STRING:
			//{
			//  //Not to be mapped
			//	strcpy(iec61850_type, "MMS_STRING");
			//}
			//break;
			case MMS_UTC_TIME:
			{
				strcpy(iec61850_type, "MMS_UTC_TIME");
				insert_record = 1;
			}
			break;
			case MMS_UNSIGNED:
			{
				strcpy(iec_104_type, "M_IT_TB_1");
				strcpy(iec61850_type, "MMS_UNSIGNED");
				insert_record = 1;
				nTestItem++;
			}
			break;
			case MMS_INTEGER:
			{
				strcpy(iec_104_type, "M_IT_TB_1");
				strcpy(iec61850_type, "MMS_INTEGER");
				insert_record = 1;
				nTestItem++;
			}
			break;
			case MMS_FLOAT:
			{
				nTestItem++;
				insert_record = 1;
				if(typeSpec->typeSpec.floatingpoint.formatWidth == 64) 
				{
					strcpy(iec_104_type, "M_ME_TN_1");
					strcpy(iec61850_type, "MMS_FLOAT");
				}
				else
				{
					strcpy(iec_104_type, "M_ME_TF_1");
					strcpy(iec61850_type, "MMS_FLOAT");
				}
			}
			break;
			case MMS_BOOLEAN:
			{
				strcpy(iec_104_type, "M_SP_TB_1");
				strcpy(iec61850_type, "MMS_BOOLEAN");
				insert_record = 1;
				nTestItem++;
			}
			break;
			//case MMS_VISIBLE_STRING:
			//{
			//  //Not to be mapped
			//	strcpy(iec61850_type, "MMS_VISIBLE_STRING");
			//}
			//break;
			default:
			{
				//IEC61850 type NOT supported
			}
			break;
		}

		if(insert_record && strlen(iec_104_type) > 0)
		{
			fprintf(dump, "insert into client_table values('%s', '%d', '%s', '%lf', '%lf', '%s');\n", 
			spname, nTestItem, iec_104_type, max, min, iec61850_type);
			fflush(dump);
		}
		
		////////////////////////////end dumping one record/////////////////////////////////////////////
	}

	Stop();

	if(dump)
	{
		fclose(dump);
		dump = NULL;
	}

	fprintf(stderr,"Server browsing is complete!\n");
	fflush(stderr);
		
	IT_EXIT;
}
