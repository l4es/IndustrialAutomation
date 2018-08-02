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
#include "stdint.h"
#include "iec104types.h"
#include "iec_item.h"
#include "client.h"
#include "clear_crc_eight.h"
#include <stdlib.h>

#define MAX_COMMAND_SEND_TIME 60 //seconds

#include <sys/stat.h>

extern int gl_timeout_connection_with_parent;

#ifdef __cplusplus
extern "C" {
#endif

int32_t MmsValue_toInt32(MmsValue* value);
uint32_t MmsValue_toUInt32(MmsValue* value);

#ifdef __cplusplus
}
#endif

/////////////////////////////////////Middleware///////////////////////////////////////////
////////////////////////////////Middleware/////////////////////////////////////

static u_int n_msg_sent = 0;

int IEC61850_client_imp::sendEvents()
{
	IT_IT("IEC61850_client_imp::sendEvents");
	
	int rc = 0;
	int check_server = 0;

	while(true)
	{
		//check connection every g_dwUpdateRate*10 (about 10 or 30 secondi)
		if((check_server%10) == 0)
		{
			rc = check_connection_to_server();
			fprintf(stderr,"check for server connection...\n");
			fflush(stderr);
		}

		check_server++;

		if(rc)
		{ 
			//fprintf(stderr,"IEC61850_client_imp exiting...., due to lack of connection with server\n");
			//fflush(stderr);
			IT_COMMENT("IEC61850_client_imp exiting...., due to lack of connection with server");
			
			//Send LOST message to parent (monitor.exe)
			struct iec_item item_to_send;
			struct cp56time2a actual_time;
			get_utc_host_time(&actual_time);

			memset(&item_to_send,0x00, sizeof(struct iec_item));

			item_to_send.iec_obj.ioa = 0;

			item_to_send.cause = 0x03;
			item_to_send.iec_type = C_LO_ST_1;
			item_to_send.iec_obj.o.type30.sp = 0;
			item_to_send.iec_obj.o.type30.time = actual_time;
			item_to_send.iec_obj.o.type30.iv = 0;
			item_to_send.msg_id = n_msg_sent;
			item_to_send.checksum = clearCrc((unsigned char *)&item_to_send, sizeof(struct iec_item));

			//Send in monitor direction
			//prepare published data
		
			break; 
		}

		if(fExit)
		{
			IT_COMMENT("Terminate client thread!");
			break;
		}

		gl_timeout_connection_with_parent++;

		if(gl_timeout_connection_with_parent > 60)
		{
			break; //exit loop for timeout of connection with parent
		}

		pollServer();
				
		::Sleep(pollingTime);
	}
		
	IT_EXIT;
	return 0;
}

int IEC61850_client_imp::Start()
{
	IT_IT("IEC61850_client_imp::Start");
	
	char show_msg[200];
	sprintf(show_msg, " IndigoSCADA IEC61850 client start\n");
	LogMessage(NULL, show_msg);
	
	con = MmsConnection_create();

	// use authentication
	auth = (struct sAcseAuthenticationParameter*)calloc(1, sizeof(struct sAcseAuthenticationParameter));
	auth->mechanism = AUTH_PASSWORD;
	auth->value.password.string = "indigoscada";

	connectionParams = (IsoConnectionParameters *)calloc(1, sizeof(IsoConnectionParameters));
	connectionParams->acseAuthParameter = auth;

	MmsConnection_setIsoConnectionParameters(con, connectionParams);

	indication = MmsConnection_connect(con, ServerIPAddress, tcpPort);

	if (indication != MMS_OK) 
	{
		printf("MMS connect failed!\n");
		return(1);
	}
	else
	{
		printf("MMS connected.\n\n");
	}

	printf("Domains present on server:\n--------------------------\n");

	nameList = MmsConnection_getDomainNames(con);
	
	LinkedList element = nameList;

	int elementCount = 0;
	char* str;
	while ((element = LinkedList_getNext(element)) != NULL) {
		str = (char*) (element->data);
		strcpy(mmsDomain, str);
		printf("%s\n", str);
		elementCount++;
	}

	LinkedList_destroy(nameList);
	printf("\n");

	printf("Data sets for domain %s:\n--------------------------------------------\n", mmsDomain);
	nameList = MmsConnection_getDomainVariableListNames(con, mmsDomain);
	LinkedList_printStringList(nameList);
	LinkedList_destroy(nameList);
	printf("\n");

	nameList = MmsConnection_getDomainVariableNames(con, mmsDomain);

	IT_EXIT;
	return(0);
}

int IEC61850_client_imp::Stop()
{
	IT_IT("IEC61850_client_imp::Stop");

	fprintf(stderr,"Entering Stop()\n");
	fflush(stderr);

	if(nameList)
	{
		LinkedList_destroy(nameList);
		nameList = NULL;
	}
		
	if(Item)
	{
		free(Item);
		Item = NULL;
	}

	MmsConnection_destroy(con);

	char show_msg[200];
	sprintf(show_msg, " IndigoSCADA IEC61850 client end\n");
	LogMessage(NULL, show_msg);

	IT_EXIT;
	return 1;
}

int IEC61850_client_imp::check_connection_to_server(void)
{
	IT_IT("IEC61850_client_imp::check_connection_to_server");
	//TODO: is there any way to check the connection with server?
	return 0;
}

struct log_message{

	int ioa;
	char message[150];
};

void IEC61850_client_imp::LogMessage(HRESULT hr, LPCSTR pszError, const char* name)
{
	//TODO: send message to monitor.exe as a single point

	/*
	struct iec_item item_to_send;
	struct cp56time2a actual_time;
	get_utc_host_time(&actual_time);

	memset(&item_to_send,0x00, sizeof(struct iec_item));

	//item_to_send.iec_obj.ioa =  Find ioa given the message in a vector of log_message

	item_to_send.cause = 0x03;
	item_to_send.iec_type = M_SP_TB_1;
	item_to_send.iec_obj.o.type30.sp = 0;
	item_to_send.iec_obj.o.type30.time = actual_time;
	item_to_send.iec_obj.o.type30.iv = 0;
	*/
}

#include <time.h>
#include <sys/timeb.h>

void IEC61850_client_imp::get_utc_host_time(struct cp56time2a* time)
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
	time->year = ptm->tm_year - 100; //<0.99>
	time->iv = 0; //<0..1> Invalid: <0> is valid, <1> is invalid
	time->su = (u_char)tb.dstflag; //<0..1> SUmmer time: <0> is standard time, <1> is summer time

	IT_EXIT;
    return;
}

time_t IEC61850_client_imp::epoch_from_cp56time2a(const struct cp56time2a* time)
{
	struct tm	t;
	time_t epoch = 0;
	int ms;
	
	memset(&t, 0x00, sizeof(struct tm));
	
	t.tm_hour = time->hour;
	t.tm_min = time->min;
	t.tm_sec = time->msec/1000;
	ms = time->msec%1000; //not used
	t.tm_mday = time->mday;
	t.tm_mon = time->month - 1;	  //from <1..12> to	<0..11>				
	t.tm_year = time->year + 100; //from <0..99> to <years from 1900>
	t.tm_isdst = time->su;
	
	epoch = mktime(&t);

	if((epoch == -1) || (time->iv == 1))
	{
		epoch = 0;
	}

	return epoch;
}

void IEC61850_client_imp::epoch_to_cp56time2a(cp56time2a *time, signed __int64 epoch_in_millisec)
{
	struct tm	*ptm;
	int ms = (int)(epoch_in_millisec%1000);
	time_t seconds;

	IT_IT("epoch_to_cp56time2a");
	
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

	IT_EXIT;
    return;
}

void IEC61850_client_imp::pollServer(void)
{
	IT_IT("IEC61850_client_imp::pollServer");

	cp56time2a time;
	signed __int64 epoch_in_millisec;
	struct iec_item item_to_send;
	unsigned int index_inside_db;
	int send_item;
				
	if(Item == NULL)
	{
		//print error message
		return;
	}
	
	LinkedList element = nameList;
	int elementCount = 0;
	char* str;

	///////////////////////initialize packet to send/////////
	send_item = 0;
	memset(&item_to_send,0x00, sizeof(struct iec_item));
	item_to_send.cause = 0x03; //spontaneous
	/////////////////////////////////////////////////////////
			
	while((element = LinkedList_getNext(element)) != NULL) 
	{
		str = (char*) (element->data);

		typeSpec = MmsConnection_getVariableAccessAttributes(con, mmsDomain, str);

		if(typeSpec->type == MMS_ARRAY ||
		   typeSpec->type == MMS_STRUCTURE ||
		   typeSpec->type == MMS_STRING ||
		   typeSpec->type == MMS_VISIBLE_STRING ||
		   typeSpec->type == MMS_OCTET_STRING ||
		   typeSpec->type == MMS_OBJ_ID)
		{
			elementCount++;
			continue;
		}

		value = MmsConnection_readVariable(con, mmsDomain, str);

		if(value == NULL)
		{
			elementCount++;
			continue;
		}
	
		int found = 0;
		//linear search, TODO improve
		for(index_inside_db = 0; index_inside_db < g_dwNumItems; index_inside_db++)
		{
			if(strcmp(str, Item[index_inside_db].spname) == 0)
			{
				found = 1;
				break;
			}
		}

		if(!found)
		{
			elementCount++;
			continue;
		}

		item_to_send.iec_obj.ioa = Item[index_inside_db].ioa_control_center;

		switch(typeSpec->type) 
		//switch(MmsValue_getType(value))
		{
			case MMS_UNSIGNED:
			{
				switch(Item[index_inside_db].iec_104_type)
				{
					case M_IT_TB_1:
					{
						item_to_send.iec_type = M_IT_TB_1;
						item_to_send.iec_obj.o.type37.counter = MmsValue_toUInt32(value);
						send_item++;

						fprintf(stderr, "Read %s with value: %u\n", Item[index_inside_db].spname, MmsValue_toUInt32(value));
						fflush(stderr);
					}
					break;
					case M_ME_TE_1:
					{
						int error = 0;
						item_to_send.iec_type = M_ME_TE_1;
						item_to_send.iec_obj.o.type35.mv = rescale_value(MmsValue_toUInt32(value),
						Item[index_inside_db].min_measure, 
						Item[index_inside_db].max_measure, &error);
						send_item++;

						fprintf(stderr, "Read %s with value: %u\n", Item[index_inside_db].spname, MmsValue_toUInt32(value));
						fflush(stderr);
					}
					break;
					default:
					{
					  fprintf(stderr,"IEC type %d is NOT supported for MMS_UNSIGNED\n", Item[index_inside_db].iec_104_type);
					  fflush(stderr);
					}
					break;
				}
			}
			break;
			case MMS_INTEGER:
			{
				switch(Item[index_inside_db].iec_104_type)
				{
					case M_IT_TB_1:
					{
						item_to_send.iec_type = M_IT_TB_1;
						item_to_send.iec_obj.o.type37.counter = MmsValue_toInt32(value);
						send_item++;

						fprintf(stderr, "Read %s with value: %d\n", Item[index_inside_db].spname, MmsValue_toInt32(value));
						fflush(stderr);
					}
					break;
					case M_ME_TE_1:
					{
						int error = 0;
						item_to_send.iec_type = M_ME_TE_1;
						item_to_send.iec_obj.o.type35.mv = rescale_value(MmsValue_toInt32(value),
						Item[index_inside_db].min_measure, 
						Item[index_inside_db].max_measure, &error);
						send_item++;

						fprintf(stderr, "Read %s with value: %d\n", Item[index_inside_db].spname, MmsValue_toInt32(value));
						fflush(stderr);
					}
					break;
					default:
					{
					  fprintf(stderr,"IEC type %d is NOT supported for MMS_INTEGER\n", Item[index_inside_db].iec_104_type);
					  fflush(stderr);
					}
					break;
				}
			}
			break;
			case MMS_FLOAT:
			{
				if(typeSpec->typeSpec.floatingpoint.formatWidth == 64) 
				{
					fprintf(stderr, "MMS_FLOAT 64\n");
					fflush(stderr);

					fprintf(stderr, "Read %s with value: %lf\n", Item[index_inside_db].spname, MmsValue_toDouble(value));
					fflush(stderr);

					item_to_send.iec_type = M_ME_TN_1;
					item_to_send.iec_obj.o.type150.mv = MmsValue_toDouble(value);
					send_item++;
				}
				else 
				{
					fprintf(stderr, "MMS_FLOAT 32\n");
					fflush(stderr);

					fprintf(stderr, "Read %s with value: %f\n", Item[index_inside_db].spname, MmsValue_toFloat(value));
					fflush(stderr);

					item_to_send.iec_type = M_ME_TF_1;
					item_to_send.iec_obj.o.type36.mv = MmsValue_toFloat(value);
					send_item++;
				}
			}
			break;
			case MMS_BOOLEAN:
			{
				item_to_send.iec_type = M_SP_TB_1;
				item_to_send.iec_obj.o.type30.sp = value->value.boolean;
				send_item++;

				fprintf(stderr, "Read %s with value: %d\n", Item[index_inside_db].spname, value->value.boolean);
				fflush(stderr);
			}
			break;
			case MMS_BIT_STRING:
			{
				//quality
				switch(Item[index_inside_db].iec_104_type)
				{
					case M_SP_TB_1:
					item_to_send.iec_obj.o.type30.iv = *(value->value.bitString.buf);
					send_item++;
					break;
					case M_ME_TF_1:
					item_to_send.iec_obj.o.type36.iv = *(value->value.bitString.buf);
					send_item++;
					break;
					case M_ME_TN_1:
					item_to_send.iec_obj.o.type150.iv = *(value->value.bitString.buf);
					send_item++;
					break;
					case M_ME_TE_1:
					item_to_send.iec_obj.o.type35.iv = *(value->value.bitString.buf);
					send_item++;
					break;
					case M_IT_TB_1:
					item_to_send.iec_obj.o.type37.iv = *(value->value.bitString.buf);
					send_item++;
					break;
					default:
					break;
				}

				fprintf(stderr, "Quality of %s is: %d\n", Item[index_inside_db].spname, *(value->value.bitString.buf));
				fflush(stderr);
			}
			break;
			case MMS_GENERALIZED_TIME:
			case MMS_BINARY_TIME:
			case MMS_UTC_TIME:
			{
				//time stamp
				uint32_t epoc = MmsValue_toUnixTimestamp(value);
				epoch_in_millisec = epoc;
				epoch_in_millisec = 1000*epoch_in_millisec;
				epoch_to_cp56time2a(&time, epoch_in_millisec);
				
				switch(Item[index_inside_db].iec_104_type)
				{
					case M_SP_TB_1:
					item_to_send.iec_obj.o.type30.time = time;
					send_item++;
					break;
					case M_ME_TF_1:
					item_to_send.iec_obj.o.type36.time = time;
					send_item++;
					break;
					case M_ME_TN_1:
					item_to_send.iec_obj.o.type150.time = time;
					send_item++;
					break;
					case M_ME_TE_1:
					item_to_send.iec_obj.o.type35.time = time;
					send_item++;
					break;
					case M_IT_TB_1:
					item_to_send.iec_obj.o.type37.time = time;
					send_item++;
					break;
					default:
					break;
				}
			}
			break;
			case MMS_BCD:
			{
				//TODO: implement this
			}
			break;
			default:
			{
				//fprintf(stderr, "MMS type not supported\n");
				//fflush(stderr);
				item_to_send.iec_type = 0;
			}
			break;
		}

		elementCount++;

		if(item_to_send.iec_type == 0)
		{
			continue;
		}

		if(send_item >= 3)
		{
			item_to_send.msg_id = n_msg_sent;
			item_to_send.checksum = clearCrc((unsigned char *)&item_to_send, sizeof(struct iec_item));

			//unsigned char buf[sizeof(struct iec_item)];
			//int len = sizeof(struct iec_item);
			//memcpy(buf, &item_to_send, len);
			//	for(j = 0;j < len; j++)
			//	{
			//	  unsigned char c = *(buf + j);
				//fprintf(stderr,"tx ---> 0x%02x\n", c);
				//fflush(stderr);
				//IT_COMMENT1("tx ---> 0x%02x\n", c);
			//	}

			//Send in monitor direction
			fprintf(stderr,"Sending message %u th\n", n_msg_sent);
			fflush(stderr);
			IT_COMMENT1("Sending message %u th\n", n_msg_sent);

			//TODO: publish data
			
			n_msg_sent++;

			///////////////////////initialize packet to send/////////
			send_item = 0;
			memset(&item_to_send,0x00, sizeof(struct iec_item));
			item_to_send.cause = 0x03; //spontaneous
			/////////////////////////////////////////////////////////
		}
	}

	IT_EXIT;
}

#define _EPSILON_ ((double)(2.220446E-16))

#define DO_NOT_RESCALE

short IEC61850_client_imp::rescale_value(double V, double Vmin, double Vmax, int* error)
{
	#ifdef DO_SCALE
	double Amin;
	double Amax;
	double r;
	//double V; //Observed value in ingegneristic unit
	double A = 0.0; //Calculate scaled value between Amin = -32768 and Amax = 32767
	double denomin;

	IT_IT("rescale_value");

	*error = 0;

	Amin = -32768.0;
	Amax = 32767.0;

	if(((V - Vmin) > 0.0) && ((V - Vmax) < 0.0))
	{
		denomin = Vmax - Vmin;

		if(denomin > 0.0)
		{
			r = (Amax - Amin)/denomin;
			A = r*(V - Vmin) + Amin;
		}
		else
		{
			*error = 1;
		}
	}
	else if(((V - Vmin) < 0.0))
	{
		A = Amin;
	}
	else if(!fcmp(V, Vmin, _EPSILON_))
	{
		A = Amin;
	}
	else if(((V - Vmax) > 0.0))
	{
		A = Amax;
	}
	else if(!fcmp(V, Vmax, _EPSILON_))
	{
		A = Amax;
	}
	
	IT_COMMENT4("V = %lf, Vmin = %lf, Vmax = %lf, A = %lf", V, Vmin, Vmax, A);

	IT_EXIT;

	return (short)A;

	#endif

	#ifdef DO_NOT_RESCALE

	return (short)V;

	#endif //DO_NOT_RESCALE
}

double IEC61850_client_imp::rescale_value_inv(double A, double Vmin, double Vmax, int* error)
{
	#ifdef DO_SCALE
	double Amin;
	double Amax;
	double r;
	double V; //Calculated value in ingegneristic unit
	//double A = 0.0; //Given a scaled value between Amin = -32768 and Amax = 32767
	double denomin;

	IT_IT("rescale_value_inv");

	*error = 0;

	Amin = -32768.0;
	Amax = 32767.0;

	denomin = Vmax - Vmin;

	if(denomin > 0.0)
	{
		r = (Amax - Amin)/denomin;
		V = Vmin + (A - Amin)/r;
	}
	else
	{
		*error = 1;
	}
		
	IT_COMMENT4("V = %lf, Vmin = %lf, Vmax = %lf, A = %lf", V, Vmin, Vmax, A);

	IT_EXIT;

	return V;

	#endif

	#ifdef DO_NOT_RESCALE

	return A;

	#endif //DO_NOT_RESCALE
}

void IEC61850_client_imp::check_for_commands(struct iec_item *queued_item)
{
	DWORD dw = 0;
	MmsValue* value_to_write;
		    
	if(!fExit)
	{ 
		fprintf(stderr,"Receiving %d th message \n", queued_item->msg_id);
		fflush(stderr);
					
		/////////////////////write command///////////////////////////////////////////////////////////
		if(queued_item->iec_type == C_SC_TA_1
			|| queued_item->iec_type == C_DC_TA_1
			|| queued_item->iec_type == C_SE_TA_1
			|| queued_item->iec_type == C_SE_TB_1
			|| queued_item->iec_type == C_SE_TC_1
			|| queued_item->iec_type == C_BO_TA_1
			|| queued_item->iec_type == C_SC_NA_1
			|| queued_item->iec_type == C_DC_NA_1
			|| queued_item->iec_type == C_SE_NA_1 
			|| queued_item->iec_type == C_SE_NB_1
			|| queued_item->iec_type == C_SE_NC_1
			|| queued_item->iec_type == C_BO_NA_1)
		{
			Sleep(100); //Delay between one command and the next one

			/////////Here we make the QUERY:////////////////////////////////////////// /////////////////////////////
			// select from Item table hClient where ioa is equal to ioa of packet arriving (command) from monitor.exe
			///////////////////////////////////////////////////////////////////////////////////////
			int found = 0;
			DWORD hClient = -1;

			for(dw = 0; dw < g_dwNumItems; dw++) 
			{ 
				if(queued_item->iec_obj.ioa == Item[dw].ioa_control_center)
				{
					hClient = Item[dw].hClient;
					found = 1;
					break;
				}
			}

			if(found == 0)
			{
				fprintf(stderr,"Error: Command with IOA %d not found in I/O list\n", queued_item->iec_obj.ioa);
				fflush(stderr);
				fprintf(stderr,"Command NOT executed\n");
				fflush(stderr);
				return;
			}
			/////////////////////////////////////////////////////////////////////
			#ifdef CHECK_TYPE
			//check iec type of command
			if(Item[hClient - 1].iec_104_type != queued_item->iec_type)
			{
				//error
				fprintf(stderr,"Error: Command with IOA %d has iec_type %d, different from IO list type %d\n", queued_item->iec_obj.ioa, queued_item->iec_type, Item[hClient - 1].iec_104_type);
				fflush(stderr);
				fprintf(stderr,"Command NOT executed\n");
				fflush(stderr);
				return;
			}
			#endif

			//Receive a write command
								
			fprintf(stderr,"Receiving command for hClient %d, ioa %d\n", hClient, queued_item->iec_obj.ioa);
			fflush(stderr);
			
			//Check the life time of the command/////////////////////////////////////////////////////////////////
			//If life time > MAX_COMMAND_SEND_TIME seconds => DO NOT execute the command

			time_t command_generation_time_in_seconds = 0;

			switch(queued_item->iec_type)
			{
				case C_SC_TA_1:
				case C_SC_NA_1:
				{
					//time contains the UTC time
					command_generation_time_in_seconds = epoch_from_cp56time2a(&(queued_item->iec_obj.o.type58.time));

					fprintf(stderr,"Command generation at UTC time: h:%i m:%i s:%i ms:%i %02i-%02i-%02i, iv %i, su %i\n",
					queued_item->iec_obj.o.type58.time.hour,
					queued_item->iec_obj.o.type58.time.min,
					queued_item->iec_obj.o.type58.time.msec/1000,
					queued_item->iec_obj.o.type58.time.msec%1000,
					queued_item->iec_obj.o.type58.time.mday,
					queued_item->iec_obj.o.type58.time.month,
					queued_item->iec_obj.o.type58.time.year,
					queued_item->iec_obj.o.type58.time.iv,
					queued_item->iec_obj.o.type58.time.su);
					fflush(stderr);
				}
				break;
				case C_DC_TA_1:
				case C_DC_NA_1:
				{
					//time contains the UTC time
					command_generation_time_in_seconds = epoch_from_cp56time2a(&(queued_item->iec_obj.o.type59.time));

					fprintf(stderr,"Command generation at UTC time: h:%i m:%i s:%i ms:%i %02i-%02i-%02i, iv %i, su %i\n",
					queued_item->iec_obj.o.type59.time.hour,
					queued_item->iec_obj.o.type59.time.min,
					queued_item->iec_obj.o.type59.time.msec/1000,
					queued_item->iec_obj.o.type59.time.msec%1000,
					queued_item->iec_obj.o.type59.time.mday,
					queued_item->iec_obj.o.type59.time.month,
					queued_item->iec_obj.o.type59.time.year,
					queued_item->iec_obj.o.type59.time.iv,
					queued_item->iec_obj.o.type59.time.su);
					fflush(stderr);
				}
				break;
				case C_SE_TA_1:
				case C_SE_NA_1:
				{
					//time contains the UTC time
					command_generation_time_in_seconds = epoch_from_cp56time2a(&(queued_item->iec_obj.o.type61.time));

					fprintf(stderr,"Command generation at UTC time: h:%i m:%i s:%i ms:%i %02i-%02i-%02i, iv %i, su %i\n",
					queued_item->iec_obj.o.type61.time.hour,
					queued_item->iec_obj.o.type61.time.min,
					queued_item->iec_obj.o.type61.time.msec/1000,
					queued_item->iec_obj.o.type61.time.msec%1000,
					queued_item->iec_obj.o.type61.time.mday,
					queued_item->iec_obj.o.type61.time.month,
					queued_item->iec_obj.o.type61.time.year,
					queued_item->iec_obj.o.type61.time.iv,
					queued_item->iec_obj.o.type61.time.su);
					fflush(stderr);
				}
				break;
				case C_SE_TB_1:
				case C_SE_NB_1:
				{
					//time contains the UTC time
					command_generation_time_in_seconds = epoch_from_cp56time2a(&(queued_item->iec_obj.o.type62.time));

					fprintf(stderr,"Command generation at UTC time: h:%i m:%i s:%i ms:%i %02i-%02i-%02i, iv %i, su %i\n",
					queued_item->iec_obj.o.type62.time.hour,
					queued_item->iec_obj.o.type62.time.min,
					queued_item->iec_obj.o.type62.time.msec/1000,
					queued_item->iec_obj.o.type62.time.msec%1000,
					queued_item->iec_obj.o.type62.time.mday,
					queued_item->iec_obj.o.type62.time.month,
					queued_item->iec_obj.o.type62.time.year,
					queued_item->iec_obj.o.type62.time.iv,
					queued_item->iec_obj.o.type62.time.su);
					fflush(stderr);
				}
				break;
				case C_SE_TC_1:
				case C_SE_NC_1:
				{
					//time contains the UTC time
					command_generation_time_in_seconds = epoch_from_cp56time2a(&(queued_item->iec_obj.o.type63.time));

					fprintf(stderr,"Command generation at UTC time: h:%i m:%i s:%i ms:%i %02i-%02i-%02i, iv %i, su %i\n",
					queued_item->iec_obj.o.type63.time.hour,
					queued_item->iec_obj.o.type63.time.min,
					queued_item->iec_obj.o.type63.time.msec/1000,
					queued_item->iec_obj.o.type63.time.msec%1000,
					queued_item->iec_obj.o.type63.time.mday,
					queued_item->iec_obj.o.type63.time.month,
					queued_item->iec_obj.o.type63.time.year,
					queued_item->iec_obj.o.type63.time.iv,
					queued_item->iec_obj.o.type63.time.su);
					fflush(stderr);
				}
				break;
				case C_BO_TA_1:
				case C_BO_NA_1:
				{
					//time contains the UTC time
					command_generation_time_in_seconds = epoch_from_cp56time2a(&(queued_item->iec_obj.o.type64.time));

					fprintf(stderr,"Command generation at UTC time: h:%i m:%i s:%i ms:%i %02i-%02i-%02i, iv %i, su %i\n",
					queued_item->iec_obj.o.type64.time.hour,
					queued_item->iec_obj.o.type64.time.min,
					queued_item->iec_obj.o.type64.time.msec/1000,
					queued_item->iec_obj.o.type64.time.msec%1000,
					queued_item->iec_obj.o.type64.time.mday,
					queued_item->iec_obj.o.type64.time.month,
					queued_item->iec_obj.o.type64.time.year,
					queued_item->iec_obj.o.type64.time.iv,
					queued_item->iec_obj.o.type64.time.su);
					fflush(stderr);
				}
				break;
				default:
				{
					//error
					//fprintf(stderr,"Error %d, %s\n",__LINE__, __FILE__);
					//fflush(stderr);

					char show_msg[200];
					sprintf(show_msg, "Error %d, %s\n",__LINE__, __FILE__);
					IEC61850_client_imp::LogMessage(0, show_msg);
				
					return;
				}
				break;
			}

			struct cp56time2a actual_time;
			get_utc_host_time(&actual_time);

			time_t command_execution_time_in_seconds = epoch_from_cp56time2a(&actual_time);

			fprintf(stderr,"Command execution UTC time: h:%i m:%i s:%i ms:%i %02i-%02i-%02i, iv %i, su %i\n",
			actual_time.hour,
			actual_time.min,
			actual_time.msec/1000,
			actual_time.msec%1000,
			actual_time.mday,
			actual_time.month,
			actual_time.year,
			actual_time.iv,
			actual_time.su);
			fflush(stderr);

			time_t delta = command_execution_time_in_seconds  - command_generation_time_in_seconds;

			fprintf(stderr,"Aged delta time= %d\n", delta);
			fflush(stderr);

			if(delta < MAX_COMMAND_SEND_TIME && delta >= 0)
			{
				switch(Item[hClient - 1].iec61850_type)
				{
					case MMS_VISIBLE_STRING:
					{
						#define COMMAND_STR_LEN 20
						char command_string[COMMAND_STR_LEN];

						double val_to_write = 0.0;
						
						switch(queued_item->iec_type)
						{
							case C_SC_TA_1:
							{
								val_to_write = queued_item->iec_obj.o.type58.scs;
								sprintf(command_string, "%f", val_to_write);
							}
							break;
							case C_DC_TA_1:
							{
								val_to_write = queued_item->iec_obj.o.type59.dcs;
								sprintf(command_string, "%f", val_to_write);
							}
							break;
							case C_SE_TA_1:
							{
								double Vmin = Item[hClient - 1].min_measure;
								double Vmax = Item[hClient - 1].max_measure;
								double A = (double)queued_item->iec_obj.o.type61.sv;
								int error = 0;

								val_to_write = rescale_value_inv(A, Vmin, Vmax, &error);
								if(error){ return;}

								sprintf(command_string, "%lf", val_to_write);
							}
							break;
							case C_SE_TB_1:
							{
								double Vmin = Item[hClient - 1].min_measure;
								double Vmax = Item[hClient - 1].max_measure;
								double A = (double)queued_item->iec_obj.o.type62.sv;
								int error = 0;

								val_to_write = rescale_value_inv(A, Vmin, Vmax, &error);
								if(error){ return;}
								
								sprintf(command_string, "%lf", val_to_write);
							}
							break;
							case C_SE_TC_1:
							{
								val_to_write = queued_item->iec_obj.o.type63.sv;
								sprintf(command_string, "%lf", val_to_write);
							}
							break;
							case C_BO_TA_1:
							{
								memset(command_string, 0x00, COMMAND_STR_LEN);
								memcpy(command_string, &(queued_item->iec_obj.o.type64.stcd), sizeof(struct iec_stcd));
							}
							break;
							case C_SC_NA_1:
							{
								val_to_write = queued_item->iec_obj.o.type45.scs;
								sprintf(command_string, "%lf", val_to_write);
							}
							break;
							case C_DC_NA_1:
							{
								val_to_write = queued_item->iec_obj.o.type46.dcs;
								sprintf(command_string, "%lf", val_to_write);
							}
							break;
							case C_SE_NA_1:
							{
								double Vmin = Item[hClient - 1].min_measure;
								double Vmax = Item[hClient - 1].max_measure;
								double A = (double)queued_item->iec_obj.o.type48.sv;
								int error = 0;

								val_to_write = rescale_value_inv(A, Vmin, Vmax, &error);
								if(error){ return;}
							
								sprintf(command_string, "%lf", val_to_write);
							}
							break;
							case C_SE_NB_1:
							{
								double Vmin = Item[hClient - 1].min_measure;
								double Vmax = Item[hClient - 1].max_measure;
								double A = (double)queued_item->iec_obj.o.type49.sv;
								int error = 0;

								val_to_write = rescale_value_inv(A, Vmin, Vmax, &error);
								if(error){ return;}
								
								sprintf(command_string, "%lf", val_to_write);
							}
							break;
							case C_SE_NC_1:
							{
								val_to_write = queued_item->iec_obj.o.type50.sv;
								sprintf(command_string, "%lf", val_to_write);
							}
							break;
							case C_BO_NA_1:
							{
								memset(command_string, 0x00, COMMAND_STR_LEN);
								memcpy(command_string, &(queued_item->iec_obj.o.type51.stcd), sizeof(struct iec_stcd));
							}
							break;
							default:
							{
								//error
								//fprintf(stderr,"Error %d, %s\n",__LINE__, __FILE__);
								//fflush(stderr);

								char show_msg[200];
								sprintf(show_msg, "Error %d, %s\n",__LINE__, __FILE__);
								IEC61850_client_imp::LogMessage(0, show_msg);
								return;
							}
							break;
						}
						
						//fprintf(stderr,"Command for sample point %s, value: %s\n", Item[hClient - 1].spname, );
						//fflush(stderr);

						char show_msg[450];
						sprintf(show_msg, "Command for sample point %s, value: %s\n", Item[hClient - 1].spname, command_string);
						LogMessage(NULL, show_msg);

						value_to_write = MmsConnection_readVariable(con, mmsDomain, Item[hClient - 1].spname);
						//value_to_write->type = MMS_VISIBLE_STRING;
						MmsValue_setVisibleString(value_to_write, command_string);

						MmsConnection_writeVariable(con, mmsDomain, Item[hClient - 1].spname , value_to_write);

						MmsValue_delete(value_to_write); //delete after write
					}
					break;
					default:
					{
						unsigned int v = 0;
						double cmd_val = 0.0;

						switch(queued_item->iec_type)
						{
							case C_SC_TA_1:
							{
								v = queued_item->iec_obj.o.type58.scs;
								cmd_val = (double)v;
							}
							break;
							case C_DC_TA_1:
							{
								v = queued_item->iec_obj.o.type59.dcs;
								cmd_val = (double)v;
							}
							break;
							case C_SE_TA_1:
							{
								double Vmin = Item[hClient - 1].min_measure;
								double Vmax = Item[hClient - 1].max_measure;
								double A = (double)queued_item->iec_obj.o.type61.sv;
								int error = 0;

								cmd_val = rescale_value_inv(A, Vmin, Vmax, &error);
								if(error){ return;}
							}
							break;
							case C_SE_TB_1:
							{
								double Vmin = Item[hClient - 1].min_measure;
								double Vmax = Item[hClient - 1].max_measure;
								double A = (double)queued_item->iec_obj.o.type62.sv;
								int error = 0;

								cmd_val = rescale_value_inv(A, Vmin, Vmax, &error);
								if(error){ return;}
							}
							break;
							case C_SE_TC_1:
							{
								cmd_val = queued_item->iec_obj.o.type63.sv;
							}
							break;
							case C_BO_TA_1:
							{
								memcpy(&v, &(queued_item->iec_obj.o.type64.stcd), sizeof(struct iec_stcd));
								cmd_val = (double)v;
							}
							break;
							case C_SC_NA_1:
							{
								v = queued_item->iec_obj.o.type45.scs;
								cmd_val = (double)v;
							}
							break;
							case C_DC_NA_1:
							{
								v = queued_item->iec_obj.o.type46.dcs;
								cmd_val = (double)v;
							}
							break;
							case C_SE_NA_1:
							{
								double Vmin = Item[hClient - 1].min_measure;
								double Vmax = Item[hClient - 1].max_measure;
								double A = (double)queued_item->iec_obj.o.type48.sv;
								int error = 0;

								cmd_val = rescale_value_inv(A, Vmin, Vmax, &error);
								if(error){ return;}
							}
							break;
							case C_SE_NB_1:
							{
								double Vmin = Item[hClient - 1].min_measure;
								double Vmax = Item[hClient - 1].max_measure;
								double A = (double)queued_item->iec_obj.o.type49.sv;
								int error = 0;

								cmd_val = rescale_value_inv(A, Vmin, Vmax, &error);
								if(error){ return;}
							}
							break;
							case C_SE_NC_1:
							{
								cmd_val = queued_item->iec_obj.o.type50.sv;
							}
							break;
							case C_BO_NA_1:
							{
								memcpy(&v, &(queued_item->iec_obj.o.type51.stcd), sizeof(struct iec_stcd));
								cmd_val = (double)v;
							}
							break;
							default:
							{
								//error
								//fprintf(stderr,"Error %d, %s\n",__LINE__, __FILE__);
								//fflush(stderr);

								char show_msg[200];
								sprintf(show_msg, "Error %d, %s\n",__LINE__, __FILE__);
								IEC61850_client_imp::LogMessage(0, show_msg);
								
								return;
							}
							break;
						}
						
						switch(Item[hClient - 1].iec61850_type)
						{
							case MMS_FLOAT:
							{
								value_to_write = MmsConnection_readVariable(con, mmsDomain, Item[hClient - 1].spname);
								//value_to_write->type = MMS_FLOAT;
								MmsValue_setFloat(value_to_write, (float)cmd_val);

								fprintf(stderr, "Write variable %s with value: %f\n", Item[hClient - 1].spname, MmsValue_toFloat(value_to_write));
								fflush(stderr);

								MmsConnection_writeVariable(con, mmsDomain, Item[hClient - 1].spname, value_to_write);

								MmsValue_delete(value_to_write); //delete after write
							}
							break;
							//TODO: add other MMS types
							default:
							break;
						}
						
						//fprintf(stderr,"Command for sample point %s, value: %lf\n", Item[hClient - 1].spname, cmd_val);
						//fflush(stderr);

						char show_msg[450];
						sprintf(show_msg, " Command for sample point %s, value: %lf\n", Item[hClient - 1].spname, cmd_val);
						LogMessage(NULL, show_msg);

						//IT_COMMENT2("Command for sample point %s, value: %lf", Item[hClient - 1].spname, cmd_val);
					}
					break;
				}
			}
			else
			{
				IT_COMMENT3("Rejeced command for sample point %s, aged for %ld s; max aging time %d s\n", Item[hClient - 1].spname, delta, MAX_COMMAND_SEND_TIME);
				//fprintf(stderr,"Rejeced command for sample point %s, aged for %ld s; max aging time %d s\n", Item[hClient - 1].spname, delta, MAX_COMMAND_SEND_TIME);
				//fflush(stderr);

				char show_msg[200];
				sprintf(show_msg, "Rejeced command for sample point %s, aged for %ld s; max aging time %d s\n", Item[hClient - 1].spname, delta, MAX_COMMAND_SEND_TIME);
				IEC61850_client_imp::LogMessage(0, show_msg);
			
				return;
			}
		}
		else if(queued_item->iec_type == C_EX_IT_1)
		{
			//Receiving EXIT process command from monitor.exe
			//exit the thread, and stop the process
			fExit = true;
		}
		else if(queued_item->iec_type == C_IC_NA_1)
		{
			//Receiving general interrogation command from monitor.exe
			IT_COMMENT("Receiving general interrogation command from monitor.exe");
			fprintf(stderr,"Receiving general interrogation command from monitor.exe\n");
			fflush(stderr);

			//TODO: implement this
			
			/////////end General interrogation command
		}
	}

	return;
}

void IEC61850_client_imp::alloc_command_resources(void)
{
	//Empty
}

void IEC61850_client_imp::free_command_resources(void)
{
	//Empty
}
