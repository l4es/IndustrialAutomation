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
#include <stdio.h>
#include <stdio.h>
#include <sqlite3.h>
#include "stdint.h"
#include "iec104types.h"
#include "iec_item.h"
#include "itrace.h"	
#include "opc_client_da_app.h"
#endif // _WIN32

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
				//column 1 in table opc_client_da_table
				//opc_server_item_id
				if(argv[i] != NULL)
					strcpy(gl_Config_db[gl_row_counter].spname, argv[i]);
			}
			break;
			case 1:
			{
				//column 2 in table opc_client_da_table
				//ioa_control_center Unstructured
				if(argv[i] != NULL)
					gl_Config_db[gl_row_counter].ioa_control_center = atoi(argv[i]);
			}
			break;
			case 2:
			{
				if(argv[i] != NULL)
				{
					//column 3 in table opc_client_da_table
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
				//column 4 in table opc_client_da_table
				//readable
				if(argv[i] != NULL)
					gl_Config_db[gl_row_counter].readable = atoi(argv[i]);
			}
			break;
			case 4:
			{
				//column 5 in table opc_client_da_table
				//writeable
				if(argv[i] != NULL)
					gl_Config_db[gl_row_counter].writeable = atoi(argv[i]);
			}
			break;
			case 5:
			{
				//column 6 in table opc_client_da_table
				//HiHiLimit
				if(argv[i] != NULL)
					gl_Config_db[gl_row_counter].max_measure = (float)atof(argv[i]);
			}
			break;
			case 6:
			{
				//column 7 in table opc_client_da_table
				//LoLoLimit
				if(argv[i] != NULL)
					gl_Config_db[gl_row_counter].min_measure = (float)atof(argv[i]);				
			}
			break;
			case 7:
			{
				//column 8 in table opc_client_da_table
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

int Opc_client_da_imp::AddItems()
{
	IT_IT("Opc_client_da_imp::AddItems");

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

	g_dwNumItems = MAX_CONFIGURABLE_OPC_ITEMIDS;
	
	Config_db = (struct structItem*)calloc(1, g_dwNumItems*sizeof(struct structItem));

	gl_Config_db = Config_db;

	gl_row_counter = 0;

	rc = sqlite3_exec(db, "select * from opc_client_da_table;", db_callback, 0, &zErrMsg);

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

	//|----------------ItemID-----------------|---ioa_control_center---|---io_list_iec_type---|--min--|--max--|
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
	HRESULT hr = 0;
	int nTestItem = 0; // how many items there are
	int iec_type = 0;
	int error_add_items = 0;
	int found_duplicated_item_id = 0;
	
	IEnumString* pEnumString = NULL;
	
	USES_CONVERSION;
	
	LPOLESTR pszName = NULL;
	ULONG count = 0;
	char buf[256];
	ULONG nCount = 0;

	nTestItem = 0;
	
	for(j = 0; j < n_rows; j++) //loop over each record
	{
		error_add_items = 0;

		found_duplicated_item_id = 0;
			
		strcpy(Item[nTestItem].spname, Config_db[j].spname);

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
			//the current itemID name is already loaded in the DA server
			//so nTestItem is NOT incremented
		}
	
		//TODO: we need to free
		#define customA2W(lpa) (((_lpa = lpa) == NULL) ? NULL : (_convert = (lstrlenA(_lpa)+1), ATLA2WHELPER((LPWSTR) malloc(_convert*2), _lpa, _convert)))

		strcpy(buf, Item[nTestItem].spname);

		wcscpy(Item[nTestItem].wszName, customA2W(buf));

		////////////////////////////////////////////
		#if 0

		LPWSTR Wide_buf = customA2W(buf);

		wcscpy(Item[nTestItem].wszName, Wide_buf);

		if(Wide_buf)
		{
			free(Wide_buf);
		}

		#endif
		/////////////////////////////////////////////
		
		//set VT_EMPTY and the server will select the right type////
		Item[nTestItem].vt = VT_EMPTY;
					
		OPCITEMRESULT *pItemResult = NULL;
		HRESULT *pErrors = NULL;
		OPCITEMDEF ItemDef;
		ItemDef.szAccessPath = L"";
		ItemDef.szItemID = Item[nTestItem].wszName;
		ItemDef.bActive = TRUE;
		ItemDef.hClient = g_dwClientHandle++; //it starts from 1
		ItemDef.dwBlobSize = 0;
		ItemDef.pBlob = NULL;
		ItemDef.vtRequestedDataType = Item[nTestItem].vt;
		Item[nTestItem].hClient = ItemDef.hClient;

		//fprintf(fp, "%s %d" ,Item[nTestItem].spname, g_dwClientHandle - 1); // only for test

		hr = g_pIOPCItemMgt->AddItems(1, &ItemDef, &pItemResult, &pErrors);

		if(FAILED(hr))
		{
			LogMessage(hr,"AddItems()");
			//nTestItem is NOT incremented
			error_add_items = 1;
			//fprintf(fp, " Removed\n");
			g_dwClientHandle--;
			g_dwNumItems--;
			continue;
		}

		hr = S_OK;

		if(pErrors == NULL)
		{
			//nTestItem is NOT incremented
			LogMessage(hr,"AddItems()");
			error_add_items = 1;
			//fprintf(fp, " Removed\n"); // only for test
			g_dwClientHandle--;
			g_dwNumItems--;
			continue;
		}

		if(FAILED(pErrors[0]))
		{
			LogMessage(pErrors[0],"AddItems() item");
			//nTestItem is NOT incremented
			error_add_items = 1;
			//fprintf(fp, " Removed\n"); // only for test
			g_dwClientHandle--;
			g_dwNumItems--;
			continue;

		}

		if(error_add_items == 0)
		{
			//fprintf(fp, " Added\n"); // only for test
		}

		// record unique handle for this item
		Item[nTestItem].hServer = pItemResult->hServer;
		Item[nTestItem].vt = pItemResult->vtCanonicalDataType;
		Item[nTestItem].dwAccessRights = pItemResult->dwAccessRights;

		::CoTaskMemFree(pItemResult);
		::CoTaskMemFree(pErrors);
			
		Item[nTestItem].ioa_control_center = Config_db[j].ioa_control_center;
		Item[nTestItem].io_list_iec_type = Config_db[j].io_list_iec_type;

		//ended to read a record

		nTestItem++;

		fprintf(stderr,"nTestItem = %d\r", nTestItem);
		fflush(stderr);
	}
		
	//fclose(fp); //only for test

	//WARNING: if g_dwNumItems is wrong the function g_pIOPCAsyncIO2->Read of General Interrogation does not work
	fprintf(stderr, "g_dwNumItems = %d\n", g_dwNumItems);
	fflush(stderr);
	
	fprintf(stderr,_T("OPC items: %d\n"), g_dwNumItems);
	fflush(stderr);

	if(g_dwNumItems <= 0)
	{
		IT_EXIT;
		return 1;
	}

	IT_EXIT;
	return(0);
}

void Opc_client_da_imp::CreateSqlConfigurationFile(char* sql_file_name, char* opc_path)
{
	//Make browsing of OPC DA server for available ItemID's
	/*

	TODO: implementing the browsing along OPC directories

	HRESULT hr = 0;
	FILE *dump = NULL;
	//OPCNAMESPACETYPE nameSpaceType;
	char program_path[_MAX_PATH];

	program_path[0] = '\0';

	if(GetModuleFileName(NULL, program_path, _MAX_PATH))
	{
		*(strrchr(program_path, '\\')) = '\0';        // Strip \\filename.exe off path
		*(strrchr(program_path, '\\')) = '\0';        // Strip \\bin off path
    }

	//hr = g_pIOPCBrowse->QueryOrganization(&nameSpaceType);

	if(FAILED(hr))
	{
		LogMessage(hr, _T("QueryOrganization()"));
		IT_EXIT;
		return;
	}
	
	IEnumString* pEnumStringLevel = NULL;
	
	USES_CONVERSION;
							
	if(dump == NULL)
	{
		fprintf(stderr,"Error opening file: %s\n", sql_file_path);
		fflush(stderr);
		IT_EXIT;
		return;
	}

	if(strlen(opc_path) > 0)
	{
		WCHAR structure_level[MAX_PATH];

		wcscpy(structure_level, T2W(opc_path));

		hr = g_pIOPCBrowse->ChangeBrowsePosition(OPC_BROWSE_TO, structure_level);

		if(FAILED(hr))
		{
			LogMessage(hr, _T("ChangeBrowsePosition()"));
			fprintf(stderr,"Error ChangeBrowsePosition to %s\n", opc_path);
			fflush(stderr);
			IT_EXIT;
			return;
		}
	}
	
	hr = g_pIOPCBrowse->BrowseOPCItemIDs(OPC_FLAT, L"", VT_EMPTY, 0, &pEnumStringLevel);
	
	if(FAILED(hr))
	{
		LogMessage(hr, _T("BrowseOPCItemIDs()"));
		IT_EXIT;
		return;
	}

	*/
	
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

	fprintf(dump, "create table opc_client_da_table(opc_server_item_id varchar(150), ioa_control_center varchar(150), iec_type varchar(150), readable varchar(150), writeable varchar(150), HiHiLimit varchar(150), LoLoLimit varchar(150), opc_type varchar(150));\n");
	fflush(dump);
							
	if(dump == NULL)
	{
		fprintf(stderr,"Error opening file: %s\n", sql_file_path);
		fflush(stderr);
		IT_EXIT;
		return;
	}

	IEnumString* pEnumString = NULL;

	USES_CONVERSION;

	///////////////////////////ChangeBrowsePosition/////////////////////////////////////
	if(strlen(opc_path) > 0)
	{
		WCHAR structure_level[MAX_PATH];

		wcscpy(structure_level, T2W(opc_path));

		hr = g_pIOPCBrowse->ChangeBrowsePosition(OPC_BROWSE_TO, structure_level);

		if(FAILED(hr))
		{
			LogMessage(hr, _T("ChangeBrowsePosition()"));
			fprintf(stderr,"Error ChangeBrowsePosition to %s\n", opc_path);
			fflush(stderr);
			IT_EXIT;
			return;
		}
	}
	////////////////////////////////end///////////////////////////////////////////////

	hr = g_pIOPCBrowse->BrowseOPCItemIDs(OPC_FLAT, L""/*NULL*/, VT_EMPTY, 0, &pEnumString);

	if(FAILED(hr))
	{
		LogMessage(hr, _T("BrowseOPCItemIDs()"));
		IT_EXIT;
		return;
	}

	if(hr == S_OK)
	{
		LPOLESTR pszName = NULL;
		LPOLESTR pszItemID = NULL;
		ULONG count = 0;
		char buf[256];
		ULONG nCount = 0;

		while((hr = pEnumString->Next(1, &pszName, &count)) == S_OK)
		{
			nCount++;
			::CoTaskMemFree(pszName);
		}

		g_dwNumItems = nCount;

		printf(_T("OPC items: %d\n"), nCount);

		Item = (struct structItem*)calloc(1, g_dwNumItems*sizeof(struct structItem));

		pEnumString->Release();

		hr = g_pIOPCBrowse->BrowseOPCItemIDs(OPC_FLAT, L""/*NULL*/, VT_EMPTY, 0, &pEnumString);

		if(FAILED(hr))
		{
			LogMessage(hr, _T("BrowseOPCItemIDs()"));
			IT_EXIT;
			return;
		}
								
		for(int nTestItem = 0; (hr = pEnumString->Next(1, &pszName, &count)) == S_OK; )
		{
			//printf(_T("%s\n"), OLE2T(pszName));
			//strcpy(buf, OLE2T(pszName)); Do NOT use OLE2T
			
			///////////////////////////ChangeBrowsePosition/////////////////////////////////////
			if(strlen(opc_path) > 0)
			{
				hr = g_pIOPCBrowse->GetItemID(pszName, &pszItemID);

				if(FAILED(hr))
				{
					LogMessage(hr, _T("GetItemID()"));
					IT_EXIT;
					return;
				}

				sprintf(buf, "%ls", pszItemID);
			}
			else
			{
				sprintf(buf, "%ls", pszName);
			}

			strcpy(Item[nTestItem].spname, buf);
			
			//wcscpy(Item[nTestItem].wszName, T2W(buf));

			#define customA2W(lpa) (((_lpa = lpa) == NULL) ? NULL : (_convert = (lstrlenA(_lpa)+1), ATLA2WHELPER((LPWSTR) malloc(_convert*2), _lpa, _convert)))

			LPWSTR Wide_buf = customA2W(buf);

			wcscpy(Item[nTestItem].wszName, Wide_buf);

			if(Wide_buf)
			{
				free(Wide_buf);
			}
			
			//set VT_EMPTY and the server will select the right type////
			Item[nTestItem].vt = VT_EMPTY;
						
			OPCITEMRESULT *pItemResult = NULL;
			HRESULT *pErrors = NULL;
			OPCITEMDEF ItemDef;
			ItemDef.szAccessPath = L"";
			ItemDef.szItemID = Item[nTestItem].wszName;
			ItemDef.bActive = TRUE;
			ItemDef.hClient = g_dwClientHandle++; //starts at 1
			ItemDef.dwBlobSize = 0;
			ItemDef.pBlob = NULL;
			ItemDef.vtRequestedDataType = Item[nTestItem].vt;
			Item[nTestItem].hClient = ItemDef.hClient;

			hr = g_pIOPCItemMgt->AddItems(1, &ItemDef, &pItemResult, &pErrors);

			if(FAILED(hr))
			{
				LogMessage(hr,"AddItems()");
				IT_EXIT;
				return;
			}

			hr = S_OK;

			if(FAILED(pErrors[0]))
			{
				LogMessage(pErrors[0],"AddItems() item");
				IT_EXIT;
				return;
			}

			// record unique handle for this item
			Item[nTestItem].hServer = pItemResult->hServer;
			Item[nTestItem].vt = pItemResult->vtCanonicalDataType;
			Item[nTestItem].dwAccessRights = pItemResult->dwAccessRights;
							
			//NOTE: please keep aligned this switch with the one in SendEvent2

			switch(Item[nTestItem].vt)
			{
				case VT_EMPTY:
				{
					strcpy(iec_type, "not_supported");
					strcpy(opc_type, "VT_EMPTY");
				}
				break;
				case VT_I1:
				{
					strcpy(iec_type, "M_IT_TB_1");
					strcpy(opc_type, "VT_I1");
				}
				break;
				case VT_UI1:
				{
					strcpy(iec_type, "M_IT_TB_1");
					strcpy(opc_type, "VT_UI1");
				}
				break;
				case VT_I2:
				{
					strcpy(iec_type, "M_IT_TB_1");
					strcpy(opc_type, "VT_I2");
				}
				break;
				case VT_UI2:
				{
					strcpy(iec_type, "M_IT_TB_1");
					strcpy(opc_type, "VT_UI2");
				}
				break;
				case VT_I4:
				{
					strcpy(iec_type, "M_IT_TB_1");
					strcpy(opc_type, "VT_I4");
				}
				break;
				case VT_UI4:
				{
					strcpy(iec_type, "M_IT_TB_1");
					strcpy(opc_type, "VT_UI4");
				}
				break;
				case VT_I8:
				{
					strcpy(iec_type, "not_supported");
					strcpy(opc_type, "VT_I8");
				}
				break;
				case VT_UI8:
				{
					strcpy(iec_type, "not_supported");
					strcpy(opc_type, "VT_UI8");
				}
				break;
				case VT_R4:
				{
					strcpy(iec_type, "M_ME_TF_1");
					strcpy(opc_type, "VT_R4");
				}
				break;
				case VT_R8:
				{
					//strcpy(iec_type, "M_ME_TF_1");
					strcpy(iec_type, "M_ME_TN_1");
					strcpy(opc_type, "VT_R8");
				}
				break;
				case VT_CY:
				{
					strcpy(iec_type, "not_supported");
					strcpy(opc_type, "VT_CY");
				}
				break;
				case VT_BOOL:
				{
					strcpy(iec_type, "M_SP_TB_1");
					strcpy(opc_type, "VT_BOOL");
				}
				break;
				case VT_DATE:
				{
					strcpy(iec_type, "M_SP_TB_1");
					strcpy(opc_type, "VT_DATE");
				}
				break;
				case VT_BSTR:
				{
					strcpy(iec_type, "M_ME_TF_1");
					strcpy(opc_type, "VT_BSTR");
				}
				break;
				case VT_VARIANT:
				{
					strcpy(iec_type, "not_supported");
					strcpy(opc_type, "VT_VARIANT");
				}
				break;
				case VT_ARRAY | VT_I1:
				{
					strcpy(iec_type, "not_supported");
					strcpy(opc_type, "VT_ARRAY | VT_I1");
				}
				break;
				case VT_ARRAY | VT_UI1:
				{
					strcpy(iec_type, "not_supported");
					strcpy(opc_type, "VT_ARRAY | VT_UI1");
				}
				break;
				case VT_ARRAY | VT_I2:
				{
					strcpy(iec_type, "not_supported");
					strcpy(opc_type, "VT_ARRAY | VT_I2");
				}
				break;
				case VT_ARRAY | VT_UI2:
				{	
					strcpy(iec_type, "not_supported");
					strcpy(opc_type, "VT_ARRAY | VT_UI2");
				}
				break;
				case VT_ARRAY | VT_I4:
				{
					strcpy(iec_type, "not_supported");
					strcpy(opc_type, "VT_ARRAY | VT_I4");
				}
				break;
				case VT_ARRAY | VT_UI4:
				{
					strcpy(iec_type, "not_supported");
					strcpy(opc_type, "VT_ARRAY | VT_UI4");
				}
				break;
				case VT_ARRAY | VT_I8:
				{
					strcpy(iec_type, "not_supported");
					strcpy(opc_type, "VT_ARRAY | VT_I8");
				}
				break;
				case VT_ARRAY | VT_UI8:
				{
					strcpy(iec_type, "not_supported");
					strcpy(opc_type, "VT_ARRAY | VT_UI8");
				}
				break;
				case VT_ARRAY | VT_R4:
				{
					strcpy(iec_type, "not_supported");
					strcpy(opc_type, "VT_ARRAY | VT_R4");
				}
				break;
				case VT_ARRAY | VT_R8:
				{
					strcpy(iec_type, "not_supported");
					strcpy(opc_type, "VT_ARRAY | VT_R8");
				}
				break;
				case VT_ARRAY | VT_CY:
				{
					strcpy(iec_type, "not_supported");
					strcpy(opc_type, "VT_ARRAY | VT_CY");
				}
				break;
				case VT_ARRAY | VT_BOOL:
				{
					strcpy(iec_type, "not_supported");
					strcpy(opc_type, "VT_ARRAY | VT_BOOL");
				}
				break;
				case VT_ARRAY | VT_DATE:
				{
					strcpy(iec_type, "not_supported");
					strcpy(opc_type, "VT_ARRAY | VT_DATE");
				}
				break;
				case VT_ARRAY | VT_BSTR:
				{
					strcpy(iec_type, "not_supported");
					strcpy(opc_type, "VT_ARRAY | VT_BSTR");
				}
				break;
				case VT_ARRAY | VT_VARIANT:
				{
					strcpy(iec_type, "not_supported");
					strcpy(opc_type, "VT_ARRAY | VT_VARIANT");
				}
				break;
				default:
				{
					//OPC type NOT suported
				}
				break;
			}

			//#define GET_PROPERTIES

			#ifdef GET_PROPERTIES
			//////get properties of the item////////////////////////////////////////////////////////////
			DWORD noProperties = 0;
			DWORD *pPropertyIDs = NULL;
			LPWSTR *pDescriptions = NULL;
			VARTYPE *pvtDataTypes = NULL;
			HRESULT *pErrorsProps = NULL;
			
			hr = g_iOpcProperties->QueryAvailableProperties(pszName, &noProperties, &pPropertyIDs, &pDescriptions, &pvtDataTypes);

			if(FAILED(hr))
			{
				LogMessage(hr, "Failed to restrieve properties");
			}
				
			VARIANT *pValues = NULL;
			
			hr = g_iOpcProperties->GetItemProperties(pszName, noProperties, pPropertyIDs, &pValues, &pErrorsProps);

			if(FAILED(hr))
			{
				LogMessage(hr, "Failed to restrieve property values");
			}

			for(unsigned i = 0; i < noProperties; i++)
			{
				//printf("PropertyID = %d, Description = %ls, DataType = %d ", pPropertyIDs[i], pDescriptions[i], pvtDataTypes[i]);

				max = 0.0;
				min = 0.0;

				if (!FAILED(pErrorsProps[i]))
				{
					if(pPropertyIDs[i] == 307  ||
					   pPropertyIDs[i] == 308  ||
					   pPropertyIDs[i] == 309  ||
					   pPropertyIDs[i] == 310)
					{
						switch(V_VT(&pValues[i]))
						{
							case VT_BOOL:
							{
								//printf("Value = %x\n", V_BOOL(&pValues[i]));

								if(pPropertyIDs[i] == 307)
								{
									max = V_BOOL(&pValues[i]);
								}
								else if(pPropertyIDs[i] == 310)
								{
									min = V_BOOL(&pValues[i]);
								}
							}
							break;
							case VT_I2:
							{
								//printf("Value = %d\n", V_I2(&pValues[i]));

								if(pPropertyIDs[i] == 307)
								{
									max = V_I2(&pValues[i]);
								}
								else if(pPropertyIDs[i] == 310)
								{
									min = V_I2(&pValues[i]);
								}
							}
							break;
							case VT_I4:
							{
								//printf("Value = %ld\n", V_I4(&pValues[i]));

								if(pPropertyIDs[i] == 307)
								{
									max = V_I4(&pValues[i]);
								}
								else if(pPropertyIDs[i] == 310)
								{
									min = V_I4(&pValues[i]);
								}
							}
							break;
							case VT_R4:
							{
								//printf("Value = %f\n", V_R4(&pValues[i]));

								if(pPropertyIDs[i] == 307)
								{
									max = V_R4(&pValues[i]);
								}
								else if(pPropertyIDs[i] == 310)
								{
									min = V_R4(&pValues[i]);
								}
							}
							break;
							case VT_R8:
							{
								//printf("Value = %lf\n", V_R8(&pValues[i]));

								if(pPropertyIDs[i] == 307)
								{
									max = V_R8(&pValues[i]);
								}
								else if(pPropertyIDs[i] == 310)
								{
									min = V_R8(&pValues[i]);
								}
							}
							break;
							case VT_BSTR:
							{
								//printf("Value = %ls\n", V_BSTR(&pValues[i]));
							}
							break;
							case VT_DATE:
							{
								DATE d; //same as R8
								d = V_DATE(&pValues[i]);

								//printf("Value = %lf\n", d);
							}
							break;
							default:
							{
								printf("Property type not supported\n");
							}
							break;
						}

						if(pPropertyIDs[i] != 310)
						{
							//fprintf(dump, ";");
							//fflush(dump);
						}
					}
				}
			}

			::CoTaskMemFree(pPropertyIDs);
			::CoTaskMemFree(pErrorsProps);
			::CoTaskMemFree(pDescriptions);

			for (i = 0; i < noProperties; i++)
			{
				VariantClear(&(pValues[i]));	
			}

			::CoTaskMemFree(pValues);
			::CoTaskMemFree(pvtDataTypes);
			////////////////end get properties/////////////////////////////////////////
			#endif //GET_PROPERTIES

			//opc_server_item_id varchar(150), ioa_control_center varchar(150), iec_type varchar(150), readable varchar(150), writeable varchar(150), HiHiLimit varchar(150), LoLoLimit varchar(150)

			int readable = ((Item[nTestItem].dwAccessRights | OPC_READABLE) ? 1: 0);
			int writeable = ((Item[nTestItem].dwAccessRights | OPC_WRITEABLE) ? 1 : 0);

			fprintf(dump, "insert into opc_client_da_table values('%s', '%d', '%s', '%d', '%d', '%lf', '%lf', '%s');\n", 
			Item[nTestItem].spname, nTestItem + 1, iec_type, readable,	writeable,	max, min, opc_type);
			fflush(dump);
			
			////////////////////////////end dumping one record/////////////////////////////////////////////

			nTestItem++;
			
			::CoTaskMemFree(pItemResult);
			::CoTaskMemFree(pErrors);
			::CoTaskMemFree(pszName);

			if(nTestItem >= MAX_CONFIGURABLE_OPC_ITEMIDS)
			{ 
				printf("Warning! Increase ""MAX_CONFIGURABLE_OPC_ITEMIDS"" items\n");
				break;
			}
		}

		pEnumString->Release();

		OpcStop();

		if(dump)
		{
			fclose(dump);
			dump = NULL;
		}

		fprintf(stderr,"Server browsing is complete!\n");
		fflush(stderr);
	}
	
	IT_EXIT;
}
