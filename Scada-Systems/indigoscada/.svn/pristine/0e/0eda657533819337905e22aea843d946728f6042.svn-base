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

#include "opc_client_da_instance.h"
#include "opc_client_dadriverthread.h"

#ifdef USE_RIPC_MIDDLEWARE
////////////////////Middleware/////////////////////////////////////////////
int exit_consumer = 0;

void consumer(void* pParam)
{
	struct subs_args* arg = (struct subs_args*)pParam;
	struct iec_item item;
	RIPCObject objDesc(&item, sizeof(struct iec_item));

	while(1)
	{
		if(exit_consumer)
		{
			break;
		}

		arg->queue_monitor_dir->get(objDesc);

		fifo_put(arg->fifo_monitor_direction, (char *)&item, sizeof(struct iec_item));
	}
}
////////////////////Middleware/////////////////////////////////////////////
#endif

/////////////////////////////////////Middleware///////////////////////////////////////////
Boolean  quite=ORTE_FALSE;
int	regfail=0;

//event system
void onRegFail(void *param) 
{
  printf("registration to a manager failed\n");
  regfail = 1;
}

void rebuild_iec_item_message(struct iec_item *item2, iec_item_type *item1)
{
	unsigned char checksum;

	///////////////Rebuild struct iec_item//////////////////////////////////
	item2->iec_type = item1->iec_type;
	memcpy(&(item2->iec_obj), &(item1->iec_obj), sizeof(struct iec_object));
	item2->cause = item1->cause;
	item2->msg_id = item1->msg_id;
	item2->ioa_control_center = item1->ioa_control_center;
	item2->casdu = item1->casdu;
	item2->is_neg = item1->is_neg;
	item2->checksum = item1->checksum;
	///////and check the 1 byte checksum////////////////////////////////////
	checksum = clearCrc((unsigned char *)item2, sizeof(struct iec_item));

	//fprintf(stderr,"new checksum = %u\n", checksum);

	//if checksum is 0 then there are no errors
	if(checksum != 0)
	{
		//log error message
		ExitProcess(0);
	}

	/*
	fprintf(stderr,"iec_type = %u\n", item2->iec_type);
	fprintf(stderr,"iec_obj = %x\n", item2->iec_obj);
	fprintf(stderr,"cause = %u\n", item2->cause);
	fprintf(stderr,"msg_id =%u\n", item2->msg_id);
	fprintf(stderr,"ioa_control_center = %u\n", item2->ioa_control_center);
	fprintf(stderr,"casdu =%u\n", item2->casdu);
	fprintf(stderr,"is_neg = %u\n", item2->is_neg);
	fprintf(stderr,"checksum = %u\n", item2->checksum);
	*/
}

void recvCallBack(const ORTERecvInfo *info,void *vinstance, void *recvCallBackParam) 
{
	Opc_client_da_Instance * cl = (Opc_client_da_Instance*)recvCallBackParam;
	iec_item_type *item1 = (iec_item_type*)vinstance;

	switch (info->status) 
	{
		case NEW_DATA:
		{
		  if(!quite)
		  {
			  struct iec_item item2;
			  rebuild_iec_item_message(&item2, item1);
			  //TODO: detect losts messages when item2.msg_id are NOT consecutive
			  fifo_put(cl->fifo_monitor_direction, (char *)&item2, sizeof(struct iec_item));
		  }
		}
		break;
		case DEADLINE:
		{
			//printf("deadline occurred\n");
		}
		break;
	}
}
////////////////////////////////Middleware//////////////////////////////////////////////


/////////////////////////////////////////////////////////////
//This global data is not a problem bucause each
//instance of Opc_client_da_Instance class
//load the sqlite database at a different time
static gl_row_counter = 0;
static gl_column_counter = 0;
static struct local_structItem* gl_Config_db = 0;
/////////////////////////////////////////////////////////////

struct local_structItem
{
	char indigo_scada_name[200];
	CHAR spname[200]; //Item ID of opc server, i.e. Simulated Card.Simulated Node.Random.R8 as C string
	char opc_type[100];
	unsigned int ioa_control_center;//unique inside CASDU
	unsigned int io_list_iec_type; //IEC 104 type
	int readable;
	int writeable;
	float min_measure;
	float max_measure;
};

/*
*Function:
*Inputs:none
*Outputs:none
*Returns:none
*/

void Opc_client_da_Instance::Start() 
{
	IT_IT("Opc_client_da_Instance::Start");

	State = STATE_RESET;
	QString cmd = "select * from UNITS where UNITTYPE='opc_client_da_driver' and NAME in(" + DriverInstance::FormUnitList()+ ");";
	GetConfigureDb()->DoExec(this,cmd,tListUnits);
};
/*
*Function: Stop
*Inputs:none
*Outputs:none
*Returns:none
*/
void Opc_client_da_Instance::Stop() // stop everything under this driver's control
{	
	IT_IT("Opc_client_da_Instance::Stop");

	pTimer->stop();

	Disconnect(); //Stop consumer thread
}
/*
*Function: QueryResponse
*Inputs:none
*Outputs:none
*Returns:none
*/
void Opc_client_da_Instance::QueryResponse(QObject *p, const QString &c, int id, QObject*caller) // handles database responses
{
	IT_IT("Opc_client_da_Instance::QueryResponse");
	
	if(p != this) return;
	switch(id)
	{
		case tListUnits:
		{
			//
			// get the properties - need to be receipe aware 
			//
			unit_name = GetConfigureDb()->GetString("NAME");

			QString pc = 
			"select * from PROPS where SKEY='" + Name + 
			"' and (IKEY = '(default)' or IKEY='"+ GetReceipeName() + "') order by IKEY desc;"; 
			//
			// get the properties SKEY = unit name IKEY = receipe name
			// 
			GetConfigureDb()->DoExec(this,pc,tUnitProperties);

			
			if(GetReceipeName() == "(default)")
			{
				// get the properties for this receipe  - fall back to default
				QString cmd = "select * from SAMPLE where UNIT='" + GetConfigureDb()->GetString("NAME") + "' and ENABLED=1;"; 
				GetConfigureDb()->DoExec(this,cmd,tList);
			}
			else
			{
				QString cmd = 
				"select * from SAMPLE where UNIT='" + GetConfigureDb()->GetString("NAME") + 
				"' and NAME in (" + DriverInstance::FormSamplePointList() + ");"; 
				// only the enabled sample points
				GetConfigureDb()->DoExec(this,cmd,tList);
			};
		}
		break;
		case tList: // list of sample points controlled by the unit
		{
			//  
			// copy the list to local dictionary 
			// now get the properties for each of the sample points
			QString nl; // name list for getting properties
			// 
			for(int ii = 1; ii <= OpcItems; ii++) // initialise
			{
				Values[ii].Name = "";
				Values[ii].clear();
			};
			//  
			// 
			int n = GetConfigureDb()->GetNumberResults();
			for(int i = 0; i < n; i++,GetConfigureDb()->FetchNext())
			{
				int idx = GetConfigureDb()->GetInt("IPINDEX");
				if (idx > 0 && idx <= OpcItems)
				{
					Values[idx].Name = GetConfigureDb()->GetString("NAME"); // save the name
				};

				if(i)
				{
					nl += ",";
				};

				nl += "'" + GetConfigureDb()->GetString("NAME") + "'";
			};
			
			QString cmd = "select * from PROPS where SKEY='SAMPLEPROPS' and IKEY in ("+nl+");";
			GetConfigureDb()->DoExec(this,cmd,tSamplePointProperties);
		};
		break;
		case tSamplePointProperties://properties specific to a sample point
		{
			int n = GetConfigureDb()->GetNumberResults();
			for(int j = 0; j < n; j++,GetConfigureDb()->FetchNext()) // may get a boat load of properties back
			{  
				// look for the entry in the table
				for(int k = 1; k <= OpcItems;k++)
				{
					if(Values[k].Name == GetConfigureDb()->GetString("IKEY"))
					{
//						QString s = UndoEscapeSQLText(GetConfigureDb()->GetString("DVAL"));
//						QTextIStream is(&s); // extract the values
//						QString a;  
//						is >> a; // interval
//						Values[k].SamplePeriod = QStringToInterval(a);
//						Values[k].clear(); // set up
//						is >> a; // now get the 
//						Values[k].Low  = a.toDouble(); // 4 - 20 low value 
//						is >> a;
//						Values[k].High = a.toDouble(); // 4 - 20 high value
//						is >> a;
//						Values[k].fSpotValue = a.toInt(); 
						break;                    
					};
				};
			};
		};
		break;
		case tUnitProperties:
		{
			if(GetConfigureDb()->GetNumberResults() > 0)
			{
				// 
				QString s = UndoEscapeSQLText(GetConfigureDb()->GetString("DVAL")); // the top one is either the receipe or (default)
				QTextIStream is(&s); // extract the values
				//
				is >> OpcItems;	  // how many OPC items there are in the RTU or PLC
				is >> Cfg.SampleTime; // how long we sample for in milliseconds
				is >> Cfg.OpcServerProgID;    // Opc Server ProgID
				is >> Cfg.OpcServerIPAddress; // Opc Server IP Address
				is >> Cfg.OpcclassId; // Opc Server Class ID
				
				Countdown = 1;

				if(Values)
				{
					delete[] Values;
					Values = NULL;
				}

				Values = new Track[OpcItems+1];
				//
				if(InTest())
				{
					Cfg.SampleTime = 1000; // override sampling period
				};
				//

				//Start OPC client DA driver
				if(!Connect())
				{
					QSLogAlarm(Name,tr("Failed to start OPC client driver"));
				}
			}
		}
		break;
		case tGetIOAfromSamplePointName:
		{
			QSTransaction &t = GetConfigureDb()->CurrentTransaction();

			if(GetConfigureDb()->GetNumberResults() > 0)
			{
				// 
				int IOACommand = GetConfigureDb()->GetInt("IOA");
				
				double command_value = 0.0;

				if(strlen((const char*)t.Data1) > 0)
				{
					command_value = atof((const char*)t.Data1);
				}

				QString cmd_time_stamp = t.Data3;

				__int64 epoch_in_millisec = _atoi64((const char*)cmd_time_stamp);

				struct cp56time2a iec_cmd_time;
				
				epoch_to_cp56time2a(&iec_cmd_time, epoch_in_millisec);

				printf("Command from %s, IOA = %d, value = %lf\n", (const char*)t.Data2, IOACommand, command_value);

				//Send C_SE_TC_1//////////////////////////////////////////////////////////////////////////
				struct iec_item item_to_send;
				memset(&item_to_send,0x00, sizeof(struct iec_item));
				item_to_send.iec_type = C_SE_TC_1;
				item_to_send.iec_obj.ioa = IOACommand;
				item_to_send.iec_obj.o.type63.sv = (float)command_value;
				
				item_to_send.iec_obj.o.type63.time = iec_cmd_time;

				item_to_send.msg_id = msg_sent_in_control_direction++;
				item_to_send.checksum = clearCrc((unsigned char *)&item_to_send, sizeof(struct iec_item));
				///////////////////////////////////////////////////////////////////////////////////////////

				#ifdef USE_RIPC_MIDDLEWARE
				////////////////////Middleware/////////////////////////////////////////////
				//publishing data
				queue_control_dir->put(&item_to_send, sizeof(struct iec_item));
				//////////////////////////Middleware/////////////////////////////////////////
				#endif
				////////////////////Middleware/////////////////////////////////////////////
				//prepare published data
				memset(&instanceSend,0x00, sizeof(iec_item_type));
				instanceSend.iec_type = item_to_send.iec_type;
				memcpy(&(instanceSend.iec_obj), &(item_to_send.iec_obj), sizeof(struct iec_object));
				instanceSend.msg_id = item_to_send.msg_id;
				instanceSend.checksum = item_to_send.checksum;

				ORTEPublicationSend(publisher);
				//////////////////////////Middleware/////////////////////////////////////////
			}
		}
		break;
		case tSetTAgsParams:
		{
			QSTransaction &t = GetConfigureDb()->CurrentTransaction();
			
			QString cmd = "select NAME from TAGS where IOA="+ t.Data1 + " and UNIT='"+ Name + "';";
		
			GetConfigureDb()->DoExec(this, cmd, tSetSamplePointNamefromIOA, t.Data1);
		}
		break;
		case tSetSamplePointNamefromIOA:
		{
			QSTransaction &t = GetConfigureDb()->CurrentTransaction();

			if(GetConfigureDb()->GetNumberResults() > 0)
			{
				QString ioa = t.Data1;

				int ioa_i = atoi((const char*)ioa);
				//
				QString SamplePointName = UndoEscapeSQLText(GetConfigureDb()->GetString("NAME"));

				for(int j = 0; j < n_rows; j ++)
				{
					if(Config_db)
					{
						if(Config_db[j].ioa_control_center == ioa_i)
						{
							strcpy(Config_db[j].indigo_scada_name, (const char*)SamplePointName);

							ioa_name_map.insert(ioa_i, SamplePointName);
							break;
						}
					}
				}
			}					
		}
		break;
		default:
		break;
	}
}

/*
*Function: driverEvent
*handles asynchronous notifications from a child driver thread
*Inputs:opcode , data as int, data as QString
*Outputs:none
*Returns:none
*/
//Realtime method
void Opc_client_da_Instance::driverEvent(DriverEvent *p)
{
	IT_IT("Opc_client_da_Instance::driverEvent");
	
	switch(p->opcode())
	{
		case DriverEvent::OpEndTransaction:
		{
			//Gestire evento di comando eseguito
		}
		break;
		case DriverEvent::OpSendEventString:
		{
			//
			QSLogEvent(Name, p->text());
			//
		};
		break;
		case DriverEvent::OpSendAlarmString:
		{
			//
			QSLogAlarm(Name, p->text());
			//
		};
		break;
		default:
		break;
	}

	DriverInstance::driverEvent(p);
}

/*
*Function: expect
*checks a given answer
*Inputs:none
*Outputs:none
*Returns:none
*/

//Realtime method
bool Opc_client_da_Instance::expect(unsigned int cmd)
{
	IT_IT("Opc_client_da_Instance::expect");

    if(InQueue.count())
	{
		SendRecePacket *packet = InQueue.head(); // get the current transaction
		if(packet->dwAnswerType != cmd) 
		{
			IT_COMMENT2("Error: Get answer %d, instead of %d", packet->dwAnswerType, cmd);
			return false;
		}
		return true;
	}

	IT_COMMENT("Error: No answer yet!");
	return false;
}

//Realtime method
void Opc_client_da_Instance::removeTransaction()
{
	IT_IT("Opc_client_da_Instance::removeTransaction");

	if(InQueue.count() > 0 && pConnect)
	{
		InQueue.remove(); //end of the life cycle of InQueue SendRecePacket
		pending_transactions--;
		IT_COMMENT1("PENDING COMMANDS %d", pending_transactions);

		if(InQueue.count() > 0)
		{
//			pConnect->SetCommand(InQueue.head()); // send the next transaction
		}
	}
}

void strip_white_space(char *dst, const char *src, int len)
{
    int i, j, n = strlen(src);
    
    memset(dst, 0x00, len);

    for (i = 0, j = 0; i < n; i++)
	{
		if (src[i] != ' ')
		{
			dst[j++] = src[i];
		}
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////

#define MAX_OPC_ITEM_PER_LINE 1000

#include <sqlite3.h>

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
				strcpy(gl_Config_db[gl_row_counter].spname, argv[i]);
			}
			break;
			case 1:
			{
				//column 2 in table opc_client_da_table
				//ioa_control_center Unstructured
				gl_Config_db[gl_row_counter].ioa_control_center = atoi(argv[i]);
			}
			break;
			case 2:
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
			break;
			case 3:
			{
				//column 4 in table opc_client_da_table
				//readable
				gl_Config_db[gl_row_counter].readable = atoi(argv[i]);
			}
			break;
			case 4:
			{
				//column 5 in table opc_client_da_table
				//writeable
				gl_Config_db[gl_row_counter].writeable = atoi(argv[i]);
			}
			break;
			case 5:
			{
				//column 6 in table opc_client_da_table
				//HiHiLimit
				gl_Config_db[gl_row_counter].max_measure = (float)atof(argv[i]);
			}
			break;
			case 6:
			{
				//column 7 in table opc_client_da_table
				//LoLoLimit
				gl_Config_db[gl_row_counter].min_measure = (float)atof(argv[i]);				
			}
			break;
			case 7:
			{
				//column 8 in table opc_client_da_table
				//opc_type in OPC format 
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

/*
*Function: Tick
*checks for triggered events - run every second or so
*Inputs:none
*Outputs:none
*Returns:none
*/


void Opc_client_da_Instance::Tick()
{
	IT_IT("Opc_client_da_Instance::Tick");

	//This code runs inside main monitor.exe thread
	
	switch(State)
	{
		case STATE_RESET:
		{
			fprintf(stderr, "State = STATE_RESET\n");
			fflush(stderr);
			State = STATE_INIT_DB;
		}
		break;
		case STATE_INIT_DB:
		{
			sqlite3 *db;
			char *zErrMsg = 0;
			int rc;
			n_rows = 0;
			m_columns = 0;

			char db_name[100];

			strcpy(db_name, GetScadaHomeDirectory());
			strcat(db_name, "\\project\\");
			strcat(db_name, Cfg.OpcServerProgID);
			strcat(db_name, ".db");
			
			rc = sqlite3_open(db_name, &db);

			if(rc)
			{
			  fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
			  fflush(stderr);
			  sqlite3_close(db);

			  QString msg;
			  msg.sprintf("OPC DA client on line %d failed", instanceID + 1); 
			  FailUnit(msg);

			  State = STATE_FAIL;
			  break;
			}

			gl_Config_db = (struct local_structItem*)calloc(1, MAX_OPC_ITEM_PER_LINE*sizeof(struct local_structItem));

			gl_row_counter = 0;

			rc = sqlite3_exec(db, "select * from opc_client_da_table;", db_callback, 0, &zErrMsg);

			if(rc != SQLITE_OK)
			{
			  fprintf(stderr, "SQL error: %s\n", zErrMsg);
			  fflush(stderr);
			  sqlite3_free(zErrMsg);

			  QString msg;
			  msg.sprintf("OPC DA client on line %d failed", instanceID + 1); 
			  FailUnit(msg);

			  State = STATE_FAIL;
			  break;
			}

			sqlite3_close(db);

			n_rows = gl_row_counter;
			m_columns = gl_column_counter;

			Config_db = (struct local_structItem*)calloc(1, MAX_OPC_ITEM_PER_LINE*sizeof(struct local_structItem));
			memcpy(Config_db, gl_Config_db, MAX_OPC_ITEM_PER_LINE*sizeof(struct local_structItem));

			if(gl_Config_db)
			{
				free(gl_Config_db);
			}

			gl_Config_db = NULL;
			
			if(n_rows > OpcItems)
			{
				QString msg;
			    msg.sprintf("OPC DA client on line %d failed", instanceID + 1); 
			    FailUnit(msg);

				State = STATE_FAIL;
				break;
			}

			for(int i = 0; i < n_rows; i ++)
			{
				if(Config_db)
				{
					char str[10];
					// update the tags
					QString cmd;
					
					cmd = QString("update TAGS set PARAMS='");
					
					char dst[150];
					strip_white_space(dst, Config_db[i].spname, 150);
					
					cmd += QString(dst);
					cmd += QString(" ");
					cmd += QString(Config_db[i].opc_type);
					cmd += QString(" ");
					cmd += QString(itoa(Config_db[i].writeable, str, 10));
					cmd += "' where IOA=" + QString(itoa(Config_db[i].ioa_control_center, str, 10)) + " and UNIT='"+ Name + "';";

					GetConfigureDb()->DoExec(this, cmd , tSetTAgsParams, QString(itoa(Config_db[i].ioa_control_center, str, 10)));

					printf("%s\n", (const char*)cmd);
				}
			}

			State = STATE_INIT_DB_DONE;
		}
		break;
		case STATE_INIT_DB_DONE:
		{
			fprintf(stderr, "State = STATE_INIT_DB_DONE\n");
			fflush(stderr);
			State = STATE_ASK_GENERAL_INTERROGATION;
		}
		break;
		case STATE_ASK_GENERAL_INTERROGATION:
		{
			//Send C_IC_NA_1//////////////////////////////////////////////////////////////////////////
			struct iec_item item_to_send;
			memset(&item_to_send,0x00, sizeof(struct iec_item));
			item_to_send.iec_type = C_IC_NA_1;
			item_to_send.iec_obj.ioa = 0;
			item_to_send.iec_obj.o.type100.qoi = 1;

			//struct cp56time2a actual_time;
			//get_utc_host_time(&actual_time);
			//item_to_send.iec_obj.o.type58.time = actual_time;
			item_to_send.msg_id = msg_sent_in_control_direction++;
			item_to_send.checksum = clearCrc((unsigned char *)&item_to_send, sizeof(struct iec_item));
			///////////////////////////////////////////////////////////////////////////////////////////

			#ifdef USE_RIPC_MIDDLEWARE
			//////////////Middleware///////////////////////////////////////
			//publishing data
			queue_control_dir->put(&item_to_send, sizeof(struct iec_item));
			//////////////Middleware///////////////////////////////////////
			#endif

			////////////////////Middleware/////////////////////////////////////////////
			//prepare published data
			memset(&instanceSend,0x00, sizeof(iec_item_type));
			instanceSend.iec_type = item_to_send.iec_type;
			memcpy(&(instanceSend.iec_obj), &(item_to_send.iec_obj), sizeof(struct iec_object));
			instanceSend.msg_id = item_to_send.msg_id;
			instanceSend.checksum = item_to_send.checksum;

			ORTEPublicationSend(publisher);
			//////////////////////////Middleware/////////////////////////////////////////

			fprintf(stderr, "State = STATE_ASK_GENERAL_INTERROGATION\n");
			fflush(stderr);
			State = STATE_GENERAL_INTERROGATION_DONE;
		}
		break;
		case STATE_GENERAL_INTERROGATION_DONE:
		{
			fprintf(stderr, "State = STATE_GENERAL_INTERROGATION_DONE\n");
			fflush(stderr);
			State = STATE_RUNNING;
		}
		break;
		case STATE_FAIL:
		{
			get_items_from_local_fifo();
		}
		break;
		case STATE_RUNNING:
		{
			get_items_from_local_fifo();
		}
		break;
		default:
		break;
	}
}

void Opc_client_da_Instance::get_items_from_local_fifo(void)
{
	unsigned char buf[sizeof(struct iec_item)];
	int len;
	const unsigned wait_limit_ms = 1;
	struct iec_item* p_item;

	for(int i = 0; (len = fifo_get(fifo_monitor_direction, (char*)buf, sizeof(struct iec_item), wait_limit_ms)) >= 0; i += 1)	
	{ 
		p_item = (struct iec_item*)buf;

		if(State == STATE_FAIL)
		{
			if(p_item->iec_type != C_LO_ST_1)
			{
				QString msg;
				msg.sprintf("OPC DA client on line %d is now connected to OPC DA server.", instanceID + 1); 
				UnFailUnit(msg);
				State = STATE_ASK_GENERAL_INTERROGATION;
			}
		}
			
		//fprintf(stderr,"Receiving %d th opc da message \n", p_item->msg_id);
		//fflush(stderr);
		fprintf(stderr,"Receiving %d th opc da message from line = %d\n", p_item->msg_id, instanceID + 1);
		fflush(stderr);

		//for (int j = 0; j < len; j++) 
		//{ 
			//assert((unsigned char)buf[i] == len);
			//unsigned char c = *((unsigned char*)buf + j);
			//printf("rx <--- 0x%02x-\n", c);
			//fprintf(fp,"rx <--- 0x%02x-\n", c);
			//fflush(fp);

			//IT_COMMENT1("rx <--- 0x%02x-\n", c);
		//}

		//printf("---------------\n");

		unsigned char rc = clearCrc((unsigned char *)buf, sizeof(struct iec_item));

		if(rc != 0)
		{
			fprintf(stderr, "checksum error rc =%d\n", rc);
			fflush(stderr);
			ExitProcess(1);
		}

		////////////////////////////////Send in gloabal monitor direction/////////////////
		if(global_publisher)
		{
			//Send in monitor direction
			//prepare published data
			memset(&global_instanceSend,0x00, sizeof(iec_item_type));

			global_instanceSend.iec_type = p_item->iec_type;
			memcpy(&(global_instanceSend.iec_obj), &(p_item->iec_obj), sizeof(struct iec_object));
			global_instanceSend.cause = p_item->cause;
			global_instanceSend.msg_id = p_item->msg_id;
			global_instanceSend.ioa_control_center = p_item->ioa_control_center;
			global_instanceSend.casdu = p_item->casdu;
			global_instanceSend.is_neg = p_item->is_neg;
			global_instanceSend.checksum = p_item->checksum;

			ORTEPublicationSend(global_publisher);
		}
		//////////////////////////////////////////////////////////////////////////////////
		
		QString sp_name;

		IOANameMap::Iterator it;

		it = ioa_name_map.find(p_item->iec_obj.ioa);

		switch(p_item->iec_type)
		{
			case M_SP_NA_1:
			{
				if(it != ioa_name_map.end())
				{
					sp_name = it.data();

					iec_type1 var = p_item->iec_obj.o.type1;
				
					IECValue v(VALUE_TAG, &var, M_SP_NA_1);
									
					IECValueList l;

					l.insert(l.end(),v);

					PostList(sp_name, l);
				}
				else
				{
					fprintf(stderr, "Error: not found name of item with ioa = %d\n", p_item->iec_obj.ioa);
					fflush(stderr);
				}
			}
			break;
			case M_DP_NA_1:
			{
				if(it != ioa_name_map.end())
				{
					sp_name = it.data();

					iec_type3 var = p_item->iec_obj.o.type3;
				
					IECValue v(VALUE_TAG, &var, M_DP_NA_1);
				
					IECValueList l;

					l.insert(l.end(),v);

					PostList(sp_name, l);

				}
				else
				{
					fprintf(stderr, "Error: not found name of item with ioa = %d\n", p_item->iec_obj.ioa);
					fflush(stderr);
				}
			}
			break;
			case M_ME_NA_1:
			{
				if(it != ioa_name_map.end())
				{
					sp_name = it.data();

					iec_type9 var = p_item->iec_obj.o.type9;
				
					IECValue v(VALUE_TAG, &var, M_ME_NA_1);
				
					IECValueList l;

					l.insert(l.end(),v);

					PostList(sp_name, l);

				}
				else
				{
					fprintf(stderr, "Error: not found name of item with ioa = %d\n", p_item->iec_obj.ioa);
					fflush(stderr);
				}
			}
			break;
			case M_ME_NB_1:
			{
				if(it != ioa_name_map.end())
				{
					sp_name = it.data();

					iec_type11 var = p_item->iec_obj.o.type11;
				
					IECValue v(VALUE_TAG, &var, M_ME_NB_1);
				
					IECValueList l;

					l.insert(l.end(),v);

					PostList(sp_name, l);

				}
				else
				{
					fprintf(stderr, "Error: not found name of item with ioa = %d\n", p_item->iec_obj.ioa);
					fflush(stderr);
				}
			}
			break;
			case M_ME_NC_1:
			{
				if(it != ioa_name_map.end())
				{
					sp_name = it.data();

					iec_type13 var = p_item->iec_obj.o.type13;

					IECValue v(VALUE_TAG, &var, M_ME_NC_1);
				
					IECValueList l;

					l.insert(l.end(),v);

					PostList(sp_name, l);

				}
				else
				{
					fprintf(stderr, "Error: not found name of item with ioa = %d\n", p_item->iec_obj.ioa);
					fflush(stderr);
				}
			}
			break;
			case M_SP_TB_1:
			{
				if(it != ioa_name_map.end())
				{
					sp_name = it.data();

					iec_type30 var = p_item->iec_obj.o.type30;
				
					IECValue v(VALUE_TAG, &var, M_SP_TB_1);
				
					IECValueList l;

					l.insert(l.end(),v);

					PostList(sp_name, l);

				}
				else
				{
					fprintf(stderr, "Error: not found name of item with ioa = %d\n", p_item->iec_obj.ioa);
					fflush(stderr);
				}
			}
			break;
			case M_DP_TB_1:
			{
				if(it != ioa_name_map.end())
				{
					sp_name = it.data();

					iec_type31 var = p_item->iec_obj.o.type31;
				
					IECValue v(VALUE_TAG, &var, M_DP_TB_1);
				
					IECValueList l;

					l.insert(l.end(),v);

					PostList(sp_name, l);

				}
				else
				{
					fprintf(stderr, "Error: not found name of item with ioa = %d\n", p_item->iec_obj.ioa);
					fflush(stderr);
				}
			}
			break;
			case M_BO_TB_1:
			{
				//value.sprintf("%d", p_item->iec_obj.o.type33.stcd);
			}
			break;
			case M_ME_TD_1:
			{
				if(it != ioa_name_map.end())
				{
					sp_name = it.data();
	
					iec_type34 var = p_item->iec_obj.o.type34;
				
					IECValue v(VALUE_TAG, &var, M_ME_TD_1);
				
					IECValueList l;

					l.insert(l.end(),v);

					PostList(sp_name, l);

				}
				else
				{
					fprintf(stderr, "Error: not found name of item with ioa = %d\n", p_item->iec_obj.ioa);
					fflush(stderr);
				}
			}
			break;
			case M_ME_TE_1:
			{
				if(it != ioa_name_map.end())
				{
					sp_name = it.data();

					iec_type35 var = p_item->iec_obj.o.type35;
				
					IECValue v(VALUE_TAG, &var, M_ME_TE_1);
				
					IECValueList l;

					l.insert(l.end(),v);

					PostList(sp_name, l);
				}
				else
				{
					fprintf(stderr, "Error: not found name of item with ioa = %d\n", p_item->iec_obj.ioa);
					fflush(stderr);
				}
			}
			break;
			case M_ME_TF_1:
			{
				if(it != ioa_name_map.end())
				{
					sp_name = it.data();

					iec_type36 var = p_item->iec_obj.o.type36;
				
					IECValue v(VALUE_TAG, &var, M_ME_TF_1);
				
					IECValueList l;

					l.insert(l.end(),v);

					PostList(sp_name, l);
				}
				else
				{
					fprintf(stderr, "Error: not found name of item with ioa = %d\n", p_item->iec_obj.ioa);
					fflush(stderr);
				}
			}
			break;
			case M_ME_TN_1:
			{
				if(it != ioa_name_map.end())
				{
					sp_name = it.data();

					is_type150 var = p_item->iec_obj.o.type150;
				
					IECValue v(VALUE_TAG, &var, M_ME_TN_1);
				
					IECValueList l;

					l.insert(l.end(),v);

					PostList(sp_name, l);

				}
				else
				{
					fprintf(stderr, "Error: not found name of item with ioa = %d\n", p_item->iec_obj.ioa);
					fflush(stderr);
				}
			}
			break;
			case M_IT_TB_1:
			{
				if(it != ioa_name_map.end())
				{
					sp_name = it.data();

					iec_type37 var = p_item->iec_obj.o.type37;
				
					IECValue v(VALUE_TAG, &var, M_IT_TB_1);
				
					IECValueList l;

					l.insert(l.end(),v);

					PostList(sp_name, l);

				}
				else
				{
					fprintf(stderr, "Error: not found name of item with ioa = %d\n", p_item->iec_obj.ioa);
					fflush(stderr);
				}
			}
			break;
			case C_EX_IT_1:
			{
				printf("Child process is exiting...\n");
			}
			break;
			case C_LO_ST_1:
			{
				if(State != STATE_FAIL)
				{
					printf("OPC DA client on line %d has lost connection with OPC DA server...\n", instanceID + 1);

					QString msg;
					msg.sprintf("OPC DA client on line %d has lost connection with OPC DA server...", instanceID + 1); 
					FailUnit(msg);

					State = STATE_FAIL;
				}
			}
			break;
			default:
			{
				fprintf(stderr, "Not supported type%d \n", p_item->iec_type);
				fflush(stderr);
			}
			break;
		}

		if(i > 50)
		{
			break;
		}
	}
}


/*
*Function:event
*event handler
*Inputs:none
*Outputs:none
*Returns:none
*/

//Realtime method
bool Opc_client_da_Instance::event(QEvent *e)
{
	IT_IT("Opc_client_da_Instance::event");

	if(e->type() == QEvent::User)
	{
		// handle it
		// add the next item from the queue 
					
		//if(InQueue.count() > 0 && pConnect)
		//{
			//SendRecePacket *p = InQueue.head(); // get the current transaction
			//DriverEvent *d = ((DriverEvent *)e); // handle the driver event
			//driverEvent(d);

			//InQueue.remove(); // delete current transaction
			//pending_transactions--;
			//IT_COMMENT1("PENDING COMMANDS %d", pending_transactions);

			//if(InQueue.count() > 0)
			//{
			//	pConnect->SetCommand(InQueue.head()); // send the next transaction
			//}
		//}
		
		DriverEvent *d = ((DriverEvent *)e); // handle the driver event
		driverEvent(d);

		return true;
	}
	return QObject::event(e);
};

/*
*Function: Connect
*Inputs:none
*Outputs:none
*Returns:none
*/

bool Opc_client_da_Instance::Connect() 
{	
	IT_IT("Opc_client_da_Instance::Connect");
	//return 0 on fail
	//retunr 1 on success

	if(pConnect) delete pConnect;
	
	pConnect = new Opc_client_da_DriverThread(this); // create the connection

	if(pConnect->Ok()) 
	{
		pConnect->start();
	}

	return true;
}

/*
*Function:Disconnect
*Inputs:none
*Outputs:none
*Returns:none
*/
bool  Opc_client_da_Instance::Disconnect()        
{
	IT_IT("Opc_client_da_Instance::Disconnect");
	
	bool res = true;
	
	InQueue.clear();

	if(pConnect) delete pConnect;	//added on 27-11-09
	pConnect = NULL;	//added on 27-11-09

	return res;
};

/*
*Function:DoExec
*Inputs:clien tobject, command string,transaction id,data1 and data 2
*Outputs:none
*Returns:none
*/
//Realtime method
bool  Opc_client_da_Instance::DoExec(SendRecePacket *t)
{
	IT_IT("Opc_client_da_Instance::DoExec");
	
	bool res = false;
/*
	if(pConnect)
	{
		IT_COMMENT3("OPC TRANSACTION de %d, cmd %d, lpPa %s", t->Dest, t->CommandType, (char*)(t->lpParams));

		if(!InQueue.count()) 
		{
			// we have a zero count so must trigger the send receive loop
			pConnect->SetCommand(t);
		}
		
		pending_transactions++;

		IT_COMMENT1("PENDING COMMANDS %d", pending_transactions);

		InQueue.enqueue(t); 
	}
*/
	return res;
};

/*
*Function:Command
*Inputs:none
*Outputs:none
*Returns:none
*/
void Opc_client_da_Instance::Command(const QString & name, BYTE cmd, LPVOID lpPa, DWORD pa_length, DWORD ipindex) // process a command for a named unit 
{
	IT_IT("Opc_client_da_Instance::Command");

	if(pConnect)
	{
		dispatcher_extra_params* params = (dispatcher_extra_params *)lpPa;

		QString sample_point_name = QString(params->string2);

		IT_COMMENT3("Received command for instance %s, sample point: %s, value: %lf", (const char*)name, (const char*)sample_point_name, params->value);

		QString pc = "select * from TAGS where NAME='" + sample_point_name + "';";

		QString value_for_command;
		value_for_command.sprintf("%lf", params->value);

		/////////////////////////////////command time stamp/////////////////////////////////////////////////
		__int64 command_arrive_time_in_ms = Epoch_in_millisec_from_cp56time2a(&(params->time_stamp));

		char buffer[20];
		_i64toa(command_arrive_time_in_ms, buffer, 10);
		QString cmd_epoch_in_ms = QString(buffer);
		////////////////////////////////////////////////////////////////////////////////////////////////////

		GetConfigureDb()->DoExec(this, pc, tGetIOAfromSamplePointName, value_for_command, sample_point_name, cmd_epoch_in_ms);
	}
}

/////////////////////////////////////Middleware///////////////////////////////////////////

#include <time.h>
#include <sys/timeb.h>

void Opc_client_da_Instance::get_utc_host_time(struct cp56time2a* time)
{
	struct timeb tb;
	struct tm	*ptm;
		
	IT_IT("get_utc_host_time");

    ftime (&tb);
	ptm = gmtime(&tb.time);
		
	time->hour = ptm->tm_hour;					//<0..23>
	time->min = ptm->tm_min;					//<0..59>
	time->msec = ptm->tm_sec*1000 + tb.millitm; //<0..59999>
	time->mday = ptm->tm_mday; //<1..31>
	time->wday = (ptm->tm_wday == 0) ? ptm->tm_wday + 7 : ptm->tm_wday; //<1..7>
	time->month = ptm->tm_mon + 1; //<1..12>
	time->year = ptm->tm_year - 100; //<0..99>
	time->iv = 0; //<0..1> Invalid: <0> is valid, <1> is invalid
	time->su = (u_char)tb.dstflag; //<0..1> SUmmer time: <0> is standard time, <1> is summer time

	IT_EXIT;
    return;
}

void Opc_client_da_Instance::epoch_to_cp56time2a(cp56time2a *time, signed __int64 epoch_in_millisec)
{
	struct tm	*ptm;
	int ms = (int)(epoch_in_millisec%1000);
	time_t seconds;
	
	memset(time, 0x00,sizeof(cp56time2a));
	seconds = (long)(epoch_in_millisec/1000);
	ptm = localtime(&seconds);
		
    if(ptm)
	{
		time->hour = ptm->tm_hour;					//<0.23>
		time->min = ptm->tm_min;					//<0..59>
		time->msec = ptm->tm_sec*1000 + ms; //<0.. 59999>
		time->mday = ptm->tm_mday; //<1..31>
		time->wday = (ptm->tm_wday == 0) ? ptm->tm_wday + 7 : ptm->tm_wday; //<1..7>
		time->month = ptm->tm_mon + 1; //<1..12>
		time->year = ptm->tm_year - 100; //<0.99>
		time->iv = 0; //<0..1> Invalid: <0> is valid, <1> is invalid
		time->su = (u_char)ptm->tm_isdst; //<0..1> SUmmer time: <0> is standard time, <1> is summer time
	}

    return;
}

/////////////////////////////////////Middleware/////////////////////////////////////////////

#include <signal.h>

char* get_date_time()
{
	static char sz[128];
	time_t t = time(NULL);
	struct tm *ptm = localtime(&t);
	
	strftime(sz, sizeof(sz)-2, "%m/%d/%y %H:%M:%S", ptm);

	strcat(sz, "|");
	return sz;
}

void iec_call_exit_handler(int line, char* file, char* reason)
{
	FILE* fp;
	char program_path[_MAX_PATH];
	char log_file[_MAX_FNAME+_MAX_PATH];
	IT_IT("iec_call_exit_handler");

	program_path[0] = '\0';
#ifdef WIN32
	if(GetModuleFileName(NULL, program_path, _MAX_PATH))
	{
		*(strrchr(program_path, '\\')) = '\0';        // Strip \\filename.exe off path
		*(strrchr(program_path, '\\')) = '\0';        // Strip \\bin off path
    }
#elif __unix__
	if(getcwd(program_path, _MAX_PATH))
	{
		*(strrchr(program_path, '/')) = '\0';        // Strip \\filename.exe off path
		*(strrchr(program_path, '/')) = '\0';        // Strip \\bin off path
    }
#endif

	strcpy(log_file, program_path);

#ifdef WIN32
	strcat(log_file, "\\logs\\fifo.log");
#elif __unix__
	strcat(log_file, "/logs/fifo.log");	
#endif

	fp = fopen(log_file, "a");

	if(fp)
	{
		if(line && file && reason)
		{
			fprintf(fp, "PID:%d time:%s exit process at line: %d, file %s, reason:%s\n", GetCurrentProcessId, get_date_time(), line, file, reason);
		}
		else if(line && file)
		{
			fprintf(fp, "PID:%d time:%s exit process at line: %d, file %s\n", GetCurrentProcessId, get_date_time(), line, file);
		}
		else if(reason)
		{
			fprintf(fp, "PID:%d time:%s exit process for reason %s\n", GetCurrentProcessId, get_date_time(), reason);
		}

		fflush(fp);
		fclose(fp);
	}

	//raise(SIGABRT);   //raise abort signal which in turn starts automatically a separete thread and call exit SignalHandler
	ExitProcess(0);

	IT_EXIT;
}