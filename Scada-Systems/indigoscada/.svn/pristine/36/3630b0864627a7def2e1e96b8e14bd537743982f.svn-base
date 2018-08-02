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
#include "mqtt_client_app.h"
#include "iec104types.h"
#include "iec_item.h"
#include "clear_crc_eight.h"
#include "GeneralHashFunctions.h"

#define MAX_KEYLEN 256
#define MAX_COMMAND_SEND_TIME 60

static DWORD g_dwSleepInLoop = 1000;

#include <sys/stat.h>

/////////////////////////////////////Middleware////////////////////////////////
static void iec_call_exit_handler(int line, char* file, char* reason);

#ifdef USE_RIPC_MIDDLEWARE
///////commands
void control_dir_consumer(void* pParam)
{
	struct subs_args* arg = (struct subs_args*)pParam;
	struct iec_item item;
	RIPCObject objDesc(&item, sizeof(struct iec_item));

	MQTT_client_imp* parent = (MQTT_client_imp*)arg->parent;

	while(1)
	{
		if(parent->exit_threads)
		{
			break;
		}

		parent->queue_control_dir->get(objDesc);

		parent->check_for_commands(&item);
	}
}
////////////////////////////////Middleware/////////////////
#endif

/////////////////////////////////////Middleware///////////////////////////////////////////
Boolean  quite = ORTE_FALSE;
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

//	fprintf(stderr,"new checksum = %u\n", checksum);

	//if checksum is 0 then there are no errors
	if(checksum != 0)
	{
		//log error message
		ExitProcess(0);
	}

//	fprintf(stderr,"iec_type = %u\n", item2->iec_type);
//	fprintf(stderr,"iec_obj = %x\n", item2->iec_obj);
//	fprintf(stderr,"cause = %u\n", item2->cause);
//	fprintf(stderr,"msg_id =%u\n", item2->msg_id);
//	fprintf(stderr,"ioa_control_center = %u\n", item2->ioa_control_center);
//	fprintf(stderr,"casdu =%u\n", item2->casdu);
//	fprintf(stderr,"is_neg = %u\n", item2->is_neg);
//	fprintf(stderr,"checksum = %u\n", item2->checksum);
}

void recvCallBack(const ORTERecvInfo *info,void *vinstance, void *recvCallBackParam) 
{
	MQTT_client_imp * cl = (MQTT_client_imp*)recvCallBackParam;
	iec_item_type *item1 = (iec_item_type*)vinstance;

	switch (info->status) 
	{
		case NEW_DATA:
		{
		  if(!quite)
		  {
			  struct iec_item item2;
			  rebuild_iec_item_message(&item2, item1);
			  cl->check_for_commands(&item2);
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
////////////////////////////////Middleware/////////////////////////////////////

extern int gl_timeout_connection_with_parent;

bool MQTT_client_imp::fExit = false;
struct structItem* MQTT_client_imp::Item = NULL;
double MQTT_client_imp::dead_band_percent = 0.0;
#ifdef USE_RIPC_MIDDLEWARE
RIPCQueue*  MQTT_client_imp::queue_monitor_dir = NULL;
#endif

static u_int n_msg_sent = 0;

MQTT_client_imp::MQTT_client_imp(char* broker_host_name, char* line_number)
{ 
	IT_IT("MQTT_client_imp::MQTT_client_imp");

	strcpy(BrokerHostName, broker_host_name);

	Config_db = NULL;
	g_dwNumItems = 0;
	g_dwUpdateRate = 60000; //in milliseconds
	nThreads = 1;

	dump = NULL;

	#ifdef USE_RIPC_MIDDLEWARE
	/////////////////////Middleware/////////////////////////////////////////////////////////////////
    factory1 = NULL;
	factory2 = NULL;
	session1 = NULL;
	session2 = NULL;
	#endif

	char fifo_monitor_name[150];
	char fifo_control_name[150];

	strcpy(fifo_monitor_name,"fifo_monitor_direction");
	strcat(fifo_monitor_name, line_number);
	strcat(fifo_monitor_name, "mqtt");
		
	strcpy(fifo_control_name,"fifo_control_direction");
	strcat(fifo_control_name, line_number);
	strcat(fifo_control_name, "mqtt");

	#ifdef USE_RIPC_MIDDLEWARE
	port = 6000;
	hostname = "localhost";

	factory1 = RIPCClientFactory::getInstance();
	factory2 = RIPCClientFactory::getInstance();

	session1 = factory1->create(hostname, port);
	session2 = factory2->create(hostname, port);
	queue_monitor_dir = session1->createQueue(fifo_monitor_name);
	queue_control_dir = session2->createQueue(fifo_control_name);

	arg.parent = this;
	#endif

	/////////////////////Middleware/////////////////////////////////////////////////////////////////
	int32_t                 strength = 1;
	NtpTime                 persistence, deadline, minimumSeparation, delay;
	IPAddress				smIPAddress = IPADDRESS_INVALID;
	ORTEDomainProp          dp; 
	ORTEDomainAppEvents     events;
	
	publisher = NULL;
	subscriber = NULL;

	ORTEInit();
	ORTEDomainPropDefaultGet(&dp);
	NTPTIME_BUILD(minimumSeparation,0); 
	NTPTIME_BUILD(delay,1); //1s

	//initiate event system
	ORTEDomainInitEvents(&events);

	events.onRegFail = onRegFail;

	//Create application     
	domain = ORTEDomainAppCreate(ORTE_DEFAULT_DOMAIN,&dp,&events,ORTE_FALSE);

	iec_item_type_type_register(domain);

	//Create publisher
	NTPTIME_BUILD(persistence,5);

	publisher = ORTEPublicationCreate(
	domain,
	fifo_monitor_name,
	"iec_item_type",
	&instanceSend,
	&persistence,
	strength,
	NULL,
	NULL,
	NULL);

	//if(publisher == NULL){} //check this error
			
	//Create subscriber
	NTPTIME_BUILD(deadline,3);

	subscriber = ORTESubscriptionCreate(
	domain,
	IMMEDIATE,
	BEST_EFFORTS,
	fifo_control_name,
	"iec_item_type",
	&instanceRecv,
	&deadline,
	&minimumSeparation,
	recvCallBack,
	this,
	smIPAddress);

	//if(subscriber == NULL){} //check this error
	///////////////////////////////////Middleware//////////////////////////////////////////////////

	#ifdef USE_RIPC_MIDDLEWARE
	///////////fifo//////////////////
	strcat(fifo_control_name, "_fifo_client");

	#define MAX_FIFO_SIZE 65535
	fifo_control_direction = fifo_open(fifo_control_name, MAX_FIFO_SIZE, iec_call_exit_handler);
		
	unsigned long threadid;
	CreateThread(NULL, 0, LPTHREAD_START_ROUTINE(control_dir_consumer), (void*)&arg, 0, &threadid);
	///////////////////////////////////Middleware//////////////////////////////////////////////////
	#endif
	/////////////////////Middleware/////////////////////////////////////////////////////////////////
		
	IT_EXIT;
}
		
MQTT_client_imp::~MQTT_client_imp()
{
	IT_IT("MQTT_client_imp::~MQTT_client_imp");
	
	fExit = true;
	
	#ifdef USE_RIPC_MIDDLEWARE
	////////Middleware/////////////
	exit_threads = 1;
	fifo_close(fifo_control_direction);
	queue_monitor_dir->close();
	queue_control_dir->close();
	session1->close();
	delete session1;
	session2->close();
	delete session2;
	////////Middleware/////////////
	#endif
	///////////////////////////////////Middleware//////////////////////////////////////////////////
	ORTEDomainAppDestroy(domain);
    domain = NULL;
	////////////////////////////////////Middleware//////////////////////////////////////////////////
	IT_EXIT;
}

int MQTT_client_imp::Update()
{
	IT_IT("MQTT_client_imp::Update");
	
	int rc = 0;
	
	while(true)
	{
		rc = mqttclient(&mqttCtx);

		if(rc != MQTT_CODE_CONTINUE)
		{
			fprintf(stderr,"MQTT_client_imp exiting...., due to lack of connection with server\n");
			fflush(stderr);
			IT_COMMENT("MQTT_client_imp exiting...., due to lack of connection with server");
			
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
			#ifdef USE_RIPC_MIDDLEWARE
			////////Middleware/////////////
			//publishing data
			queue_monitor_dir->put(&item_to_send, sizeof(struct iec_item));
			////////Middleware/////////////
			#endif

			//Send in monitor direction
			//prepare published data
			memset(&instanceSend,0x00, sizeof(iec_item_type));

			instanceSend.iec_type = item_to_send.iec_type;
			memcpy(&(instanceSend.iec_obj), &(item_to_send.iec_obj), sizeof(struct iec_object));
			instanceSend.cause = item_to_send.cause;
			instanceSend.msg_id = item_to_send.msg_id;
			instanceSend.ioa_control_center = item_to_send.ioa_control_center;
			instanceSend.casdu = item_to_send.casdu;
			instanceSend.is_neg = item_to_send.is_neg;
			instanceSend.checksum = item_to_send.checksum;

			ORTEPublicationSend(publisher);

			Sleep(200);

			break;
		}
		
		if(fExit)
		{
			IT_COMMENT("Terminate mqtt loop!");
			break;
		}

		gl_timeout_connection_with_parent++;

		if(gl_timeout_connection_with_parent > 60)
		{
			break; //exit loop for timeout of connection with parent
		}
				
		::Sleep(g_dwSleepInLoop);
	}
	
	IT_EXIT;
	return 0;
}

int MQTT_client_imp::MQTTStart(char* SubscribeTopicName, char*UserName, char* Password, int Port, char* ClientID)
{
	IT_IT("MQTT_client_imp::MQTTStart");

    /* init defaults */
    mqtt_init_ctx(&mqttCtx);
    mqttCtx.app_name = "mqttclient";
	mqttCtx.host = BrokerHostName;
	mqttCtx.topic_name = SubscribeTopicName;
	mqttCtx.dump_mode = 0;
	mqttCtx.username = UserName;
	mqttCtx.password = Password;
	mqttCtx.port = Port;
	mqttCtx.client_id = ClientID;

	IT_EXIT;
    return(0);
}

int MQTT_client_imp::MQTTStop()
{
	IT_IT("MQTT_client_imp::MQTTStop");

	if(dump)
	{
		fclose(dump);
		dump = NULL;

		fprintf(stderr,"Server browsing is complete!\n");
		fflush(stderr);
	}
		
	if(Item)
	{
		free(Item);
		Item = NULL;
	}

	IT_EXIT;
	return 1;
}

#include <time.h>
#include <sys/timeb.h>

void MQTT_client_imp::get_local_host_time(struct cp56time2a* time)
{
	struct timeb tb;
	struct tm	*ptm;

    ftime (&tb);
	ptm = localtime(&tb.time);
		
	time->hour = ptm->tm_hour;					//<0.23>
	time->min = ptm->tm_min;					//<0..59>
	time->msec = ptm->tm_sec*1000 + tb.millitm; //<0.. 59999>
	time->mday = ptm->tm_mday; //<1..31>
	time->wday = (ptm->tm_wday == 0) ? ptm->tm_wday + 7 : ptm->tm_wday; //<1..7>
	time->month = ptm->tm_mon + 1; //<1..12>
	time->year = ptm->tm_year - 100; //<0.99>
	time->iv = 0; //<0..1> Invalid: <0> is valid, <1> is invalid
	time->su = (u_char)tb.dstflag; //<0..1> SUmmer time: <0> is standard time, <1> is summer time

    return;
}

void MQTT_client_imp::get_utc_host_time(struct cp56time2a* time)
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

time_t MQTT_client_imp::epoch_from_cp56time2a(const struct cp56time2a* time)
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

void MQTT_client_imp::epoch_to_cp56time2a(cp56time2a *time, signed __int64 epoch_in_millisec)
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

#define _EPSILON_ ((double)(2.220446E-16))

#define DO_NOT_RESCALE

short MQTT_client_imp::rescale_value(double V, double Vmin, double Vmax, int* error)
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

double MQTT_client_imp::rescale_value_inv(double A, double Vmin, double Vmax, int* error)
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


void MQTT_client_imp::check_for_commands(struct iec_item *queued_item)
{
	DWORD dw = 0;
	char topic_to_write[200];
	int item = 0;
        
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
			// select from Item table spname where ioa is equal to ioa of packet arriving (command) from monitor.exe
			///////////////////////////////////////////////////////////////////////////////////////
			int found = 0;
			
			for(dw = 0; dw < g_dwNumItems; dw++) 
			{ 
				if(queued_item->iec_obj.ioa == Item[dw].ioa_control_center)
				{
					found = 1;
					strcpy(topic_to_write, Item[dw].spname); //Found topic to write
					item = dw;
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
			
			//Receive a write command
								
			fprintf(stderr,"Receiving command for topic %s, ioa %d\n", topic_to_write, queued_item->iec_obj.ioa);
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
				char command_string[20];

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
						double Vmin = Item[item].min_measure;
						double Vmax = Item[item].max_measure;
						double A = (double)queued_item->iec_obj.o.type61.sv;
						int error = 0;

						val_to_write = rescale_value_inv(A, Vmin, Vmax, &error);
						if(error){ return;}

						sprintf(command_string, "%lf", val_to_write);
					}
					break;
					case C_SE_TB_1:
					{
						double Vmin = Item[item].min_measure;
						double Vmax = Item[item].max_measure;
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
						memset(command_string, 0x00, 20);
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
						double Vmin = Item[item].min_measure;
						double Vmax = Item[item].max_measure;
						double A = (double)queued_item->iec_obj.o.type48.sv;
						int error = 0;

						val_to_write = rescale_value_inv(A, Vmin, Vmax, &error);
						if(error){ return;}
					
						sprintf(command_string, "%lf", val_to_write);
					}
					break;
					case C_SE_NB_1:
					{
						double Vmin = Item[item].min_measure;
						double Vmax = Item[item].max_measure;
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
						memset(command_string, 0x00, 20);
						memcpy(command_string, &(queued_item->iec_obj.o.type51.stcd), sizeof(struct iec_stcd));
					}
					break;
					default:
					{
						return;
					}
					break;
				}
				
				printf("Command for sample point %s, value: %s\n", Item[item].spname, command_string);

				//write MQTT command///////////////////////////////////////////////////
				/* Publish Topic */
				int rc;

				XMEMSET(&mqttCtx.publish, 0, sizeof(MqttPublish));
				mqttCtx.publish.retain = 0;
				mqttCtx.publish.qos = mqttCtx.qos;
				mqttCtx.publish.duplicate = 0;
				mqttCtx.publish.topic_name = topic_to_write;
				mqttCtx.publish.packet_id = mqtt_get_packetid();
				mqttCtx.publish.buffer = (byte*)command_string;
				mqttCtx.publish.total_len = (word16)XSTRLEN(command_string);
						
				rc = MqttClient_Publish(&mqttCtx.client, &mqttCtx.publish);
				
				printf("MQTT Publish: Topic %s, %s (%d)\n",
					mqttCtx.publish.topic_name, MqttClient_ReturnCodeToString(rc), rc);
				////////////////////////////////////////////////////////////////////////
			}
			else
			{
				printf("Rejeced command for sample point %s, aged for %ld s; max aging time %d s\n", Item[item].spname, delta, MAX_COMMAND_SEND_TIME);
				return;
			}
		}
		else if(queued_item->iec_type == C_EX_IT_1)
		{
			//Receiving EXIT process command from monitor.exe
			//exit the thread, and stop the process
			fExit = true;
		}
	}

	return;
}

#include <signal.h>

static char* get_date_time()
{
	static char sz[128];
	time_t t = time(NULL);
	struct tm *ptm = localtime(&t);
	
	strftime(sz, sizeof(sz)-2, "%m/%d/%y %H:%M:%S", ptm);

	strcat(sz, "|");
	return sz;
}

static void iec_call_exit_handler(int line, char* file, char* reason)
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
	strcat(log_file, "\\logs\\mqtt.log");
#elif __unix__
	strcat(log_file, "/logs/mqtt.log");	
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

//MQTT specific/////////////
/* locals */
static int mPacketIdLast;
static unsigned int message_hash_key = 0;
static int nTestItem = 0;

void mqtt_init_ctx(MQTTCtx* mqttCtx)
{
    XMEMSET(mqttCtx, 0, sizeof(MQTTCtx));
    //mqttCtx->host = DEFAULT_MQTT_HOST;
	mqttCtx->qos = DEFAULT_MQTT_QOS;
    mqttCtx->clean_session = 1;
    mqttCtx->keep_alive_sec = DEFAULT_KEEP_ALIVE_SEC;
    //mqttCtx->client_id = DEFAULT_CLIENT_ID;
    //mqttCtx->topic_name = DEFAULT_TOPIC_NAME;
    mqttCtx->cmd_timeout_ms = DEFAULT_CMD_TIMEOUT_MS;
}

int err_sys(const char* msg)
{
    if (msg) {
        PRINTF("error: %s", msg);
    }
	exit(EXIT_FAILURE);
}

word16 mqtt_get_packetid(void)
{
    mPacketIdLast = (mPacketIdLast >= MAX_PACKET_ID) ?
        1 : mPacketIdLast + 1;
    return (word16)mPacketIdLast;
}

int mqtt_tls_cb(MqttClient* client)
{
    (void)client;
    return 0;
}

/* Configuration */
#define MAX_BUFFER_SIZE         1024    /* Maximum size for network read/write callbacks */
//#define TEST_MESSAGE            "test"


static int mqtt_message_cb(MqttClient *client, MqttMessage *msg,
    byte msg_new, byte msg_done)
{
    byte buf[PRINT_BUFFER_SIZE+1];
    word32 len;
    MQTTCtx* mqttCtx = (MQTTCtx*)client->ctx;

	cp56time2a time;
	//signed __int64 epoch_in_millisec;
	struct iec_item item_to_send;

	MQTT_client_imp* parent_class;
	    	
	memset(&item_to_send,0x00, sizeof(struct iec_item));
	item_to_send.cause = 3;

	(void)mqttCtx;

	parent_class = (MQTT_client_imp*)mqttCtx->parent_class;

    if (msg_new) 
	{
        /* Determine min size to dump */
        len = msg->topic_name_len;
        if (len > PRINT_BUFFER_SIZE) {
            len = PRINT_BUFFER_SIZE;
        }
        XMEMCPY(buf, msg->topic_name, len);
        buf[len] = '\0'; /* Make sure its null terminated */

        /* Print incoming message */
        PRINTF("MQTT Message: Topic %s, Qos %d, Len %u\n",
            buf, msg->qos, msg->total_len);

		if (mqttCtx->dump_mode) 
		{
			char iec_type[100];
			char opc_type[100];
			double max = 0.0;
			double min = 0.0;
			//All Topics are mapped to floating point
			strcpy(iec_type, "M_ME_TF_1");
			strcpy(opc_type, "VT_R4");
			
			//topic_name varchar(150), ioa_control_center varchar(150), iec_type varchar(150), readable varchar(150), writeable varchar(150), HiHiLimit varchar(150), LoLoLimit varchar(150)

			int readable = 1;
			int writeable = 1;

			fprintf(parent_class->dump, "insert into mqtt_client_table values('%s', '%d', '%s', '%d', '%d', '%lf', '%lf', '%s');\n", 
			buf, nTestItem + 1, iec_type, readable,	writeable,	max, min, opc_type);
			fflush(parent_class->dump);
			
			nTestItem++;
			////////////////////////////end dumping one record/////////////////////////////////////////////
		}
		else
		{
			//////////////////////////////////////////////////////////////////////////
			//Prepare message in monitoring direction
			message_hash_key = APHash((char *)buf, len);

			for(unsigned int dw = 0; dw < parent_class->g_dwNumItems; dw++) 
			{ 
				if(parent_class->Item[dw].hash_key == message_hash_key)
				{
					item_to_send.iec_obj.ioa = parent_class->Item[dw].ioa_control_center;

					//printf("Found topic %s, ioa= %d\n",parent_class->Item[dw].spname, parent_class->Item[dw].ioa_control_center);
					break;
				}
			}
			//////////////////////////////////////////////////////////////////////////
			
			/* Print message payload */
			len = msg->buffer_len;
			if (len > PRINT_BUFFER_SIZE) {
				len = PRINT_BUFFER_SIZE;
			}
			XMEMCPY(buf, msg->buffer, len);
			buf[len] = '\0'; /* Make sure its null terminated */
			PRINTF("Payload (%d - %d): %s\n",
				msg->buffer_pos, msg->buffer_pos + len, buf);

			if (msg_done) {
				PRINTF("MQTT Message: Done\n");
			}

			//////////////////////////////////////////////////////////////////////////
			//Prepare message in monitoring direction
			item_to_send.iec_type = M_ME_TF_1;
			//parent_class->epoch_to_cp56time2a(&time, epoch_in_millisec);
			parent_class->get_local_host_time(&time);
			item_to_send.iec_obj.o.type36.time = time;
			item_to_send.iec_obj.o.type36.iv = 0;
			item_to_send.iec_obj.o.type36.mv = (float)atof((char *)buf);
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
			
			#ifdef USE_RIPC_MIDDLEWARE
			////////Middleware/////////////
			//publishing data
			parent_class->queue_monitor_dir->put(&item_to_send, sizeof(struct iec_item));
			////////Middleware/////////////
			#endif

			//Send in monitor direction
			//prepare published data
			memset(&(parent_class->instanceSend),0x00, sizeof(iec_item_type));

			parent_class->instanceSend.iec_type = item_to_send.iec_type;
			memcpy(&(parent_class->instanceSend.iec_obj), &(item_to_send.iec_obj), sizeof(struct iec_object));
			parent_class->instanceSend.cause = item_to_send.cause;
			parent_class->instanceSend.msg_id = item_to_send.msg_id;
			parent_class->instanceSend.ioa_control_center = item_to_send.ioa_control_center;
			parent_class->instanceSend.casdu = item_to_send.casdu;
			parent_class->instanceSend.is_neg = item_to_send.is_neg;
			parent_class->instanceSend.checksum = item_to_send.checksum;

			ORTEPublicationSend(parent_class->publisher);

			n_msg_sent++;
		}
	}

    /* Return negative to terminate publish processing */
    return MQTT_CODE_SUCCESS;
}

int mqttclient(MQTTCtx *mqttCtx)
{
    int rc = MQTT_CODE_SUCCESS, i;

    switch (mqttCtx->stat) {
        case WMQ_BEGIN:
        {
            PRINTF("MQTT Client: QoS %d, Use TLS %d\n", mqttCtx->qos, mqttCtx->use_tls);
        }

        case WMQ_NET_INIT:
        {
            mqttCtx->stat = WMQ_NET_INIT;

            /* Initialize Network */
            rc = MqttClientNet_Init(&mqttCtx->net);
            if (rc == MQTT_CODE_CONTINUE) {
                return rc;
            }
            PRINTF("MQTT Net Init: %s (%d)\n",
                MqttClient_ReturnCodeToString(rc), rc);
            if (rc != MQTT_CODE_SUCCESS) {
                goto exit;
            }

            /* setup tx/rx buffers */
            mqttCtx->tx_buf = (byte*)WOLFMQTT_MALLOC(MAX_BUFFER_SIZE);
            mqttCtx->rx_buf = (byte*)WOLFMQTT_MALLOC(MAX_BUFFER_SIZE);
        }

        case WMQ_INIT:
        {
            mqttCtx->stat = WMQ_INIT;

            /* Initialize MqttClient structure */
            rc = MqttClient_Init(&mqttCtx->client, &mqttCtx->net,
                mqtt_message_cb,
                mqttCtx->tx_buf, MAX_BUFFER_SIZE,
                mqttCtx->rx_buf, MAX_BUFFER_SIZE,
                mqttCtx->cmd_timeout_ms);
            if (rc == MQTT_CODE_CONTINUE) {
                return rc;
            }
            PRINTF("MQTT Init: %s (%d)\n",
                MqttClient_ReturnCodeToString(rc), rc);
            if (rc != MQTT_CODE_SUCCESS) {
                goto exit;
            }
            mqttCtx->client.ctx = mqttCtx;
        }

        case WMQ_TCP_CONN:
        {
            mqttCtx->stat = WMQ_TCP_CONN;

            /* Connect to broker */
            rc = MqttClient_NetConnect(&mqttCtx->client, mqttCtx->host, mqttCtx->port,
                DEFAULT_CON_TIMEOUT_MS, mqttCtx->use_tls, mqtt_tls_cb);
            if (rc == MQTT_CODE_CONTINUE) {
                return rc;
            }
            PRINTF("MQTT Socket Connect: %s (%d)\n",
                MqttClient_ReturnCodeToString(rc), rc);
            if (rc != MQTT_CODE_SUCCESS) {
                goto exit;
            }
        }

        case WMQ_MQTT_CONN:
        {
            mqttCtx->stat = WMQ_MQTT_CONN;

            XMEMSET(&mqttCtx->connect, 0, sizeof(MqttConnect));
            mqttCtx->connect.keep_alive_sec = mqttCtx->keep_alive_sec;
            mqttCtx->connect.clean_session = mqttCtx->clean_session;
            mqttCtx->connect.client_id = mqttCtx->client_id;

            /* Last will and testament sent by broker to subscribers
                of topic when broker connection is lost */
            XMEMSET(&mqttCtx->lwt_msg, 0, sizeof(mqttCtx->lwt_msg));
            mqttCtx->connect.lwt_msg = &mqttCtx->lwt_msg;
            mqttCtx->connect.enable_lwt = mqttCtx->enable_lwt;
            if (mqttCtx->enable_lwt) {
                /* Send client id in LWT payload */
                mqttCtx->lwt_msg.qos = mqttCtx->qos;
                mqttCtx->lwt_msg.retain = 0;
                mqttCtx->lwt_msg.topic_name = WOLFMQTT_TOPIC_NAME"lwttopic";
                mqttCtx->lwt_msg.buffer = (byte*)mqttCtx->client_id;
                mqttCtx->lwt_msg.total_len = (word16)XSTRLEN(mqttCtx->client_id);
            }
            /* Optional authentication */
            mqttCtx->connect.username = mqttCtx->username;
            mqttCtx->connect.password = mqttCtx->password;

            /* Send Connect and wait for Connect Ack */
            rc = MqttClient_Connect(&mqttCtx->client, &mqttCtx->connect);
            if (rc == MQTT_CODE_CONTINUE) {
                return rc;
            }
            PRINTF("MQTT Connect: %s (%d)\n",
                MqttClient_ReturnCodeToString(rc), rc);
            if (rc != MQTT_CODE_SUCCESS) {
                goto disconn;
            }

            /* Validate Connect Ack info */
            PRINTF("MQTT Connect Ack: Return Code %u, Session Present %d\n",
                mqttCtx->connect.ack.return_code,
                (mqttCtx->connect.ack.flags &
                    MQTT_CONNECT_ACK_FLAG_SESSION_PRESENT) ?
                    1 : 0
            );

            /* Build list of topics */
            mqttCtx->topics[0].topic_filter = mqttCtx->topic_name;
            mqttCtx->topics[0].qos = mqttCtx->qos;

            /* Subscribe Topic */
            XMEMSET(&mqttCtx->subscribe, 0, sizeof(MqttSubscribe));
            mqttCtx->subscribe.packet_id = mqtt_get_packetid();
            mqttCtx->subscribe.topic_count = sizeof(mqttCtx->topics)/sizeof(MqttTopic);
            mqttCtx->subscribe.topics = mqttCtx->topics;
        }

        case WMQ_SUB:
        {
            mqttCtx->stat = WMQ_SUB;

            rc = MqttClient_Subscribe(&mqttCtx->client, &mqttCtx->subscribe);
            if (rc == MQTT_CODE_CONTINUE) {
                return rc;
            }
            PRINTF("MQTT Subscribe: %s (%d)\n",
                MqttClient_ReturnCodeToString(rc), rc);
            if (rc != MQTT_CODE_SUCCESS) {
                goto disconn;
            }

            /* show subscribe results */
            for (i = 0; i < mqttCtx->subscribe.topic_count; i++) {
                mqttCtx->topic = &mqttCtx->subscribe.topics[i];
                PRINTF("  Topic %s, Qos %u, Return Code %u\n",
                    mqttCtx->topic->topic_filter,
                    mqttCtx->topic->qos, mqttCtx->topic->return_code);
            }

            /* Publish Topic */
			/*
            XMEMSET(&mqttCtx->publish, 0, sizeof(MqttPublish));
            mqttCtx->publish.retain = 0;
            mqttCtx->publish.qos = mqttCtx->qos;
            mqttCtx->publish.duplicate = 0;
            mqttCtx->publish.topic_name = mqttCtx->topic_name;
            mqttCtx->publish.packet_id = mqtt_get_packetid();
            mqttCtx->publish.buffer = (byte*)TEST_MESSAGE;
            mqttCtx->publish.total_len = (word16)XSTRLEN(TEST_MESSAGE);
			*/
        }

        case WMQ_PUB:
        {
            mqttCtx->stat = WMQ_PUB;

			/*
            rc = MqttClient_Publish(&mqttCtx->client, &mqttCtx->publish);
            if (rc == MQTT_CODE_CONTINUE) {
                return rc;
            }
            PRINTF("MQTT Publish: Topic %s, %s (%d)\n",
                mqttCtx->publish.topic_name, MqttClient_ReturnCodeToString(rc), rc);
            if (rc != MQTT_CODE_SUCCESS) {
                goto disconn;
            }
			*/

            /* Read Loop */
            PRINTF("MQTT Waiting for message...\n");
            MqttClientNet_CheckForCommand_Enable(&mqttCtx->net);
        }

        case WMQ_WAIT_MSG:
        {
            mqttCtx->stat = WMQ_WAIT_MSG;

            do {
                /* Try and read packet */
                rc = MqttClient_WaitMessage(&mqttCtx->client,
                                                    mqttCtx->cmd_timeout_ms);

                /* check for test mode */
//                if (mStopRead) {
//                    rc = MQTT_CODE_SUCCESS;
//                    break;
//                }

                /* check return code */
                if (rc == MQTT_CODE_CONTINUE) {
                    return rc;
                }
                else if (rc == MQTT_CODE_ERROR_TIMEOUT) {
                    /* Check to see if command data (stdin) is available */
                    
					//rc = MqttClientNet_CheckForCommand(&mqttCtx->net,
                    //    mqttCtx->rx_buf, MAX_BUFFER_SIZE);
                    //if (rc > 0) {
                        /* Publish Topic */
                    //    mqttCtx->stat = WMQ_PUB;
                    //    XMEMSET(&mqttCtx->publish, 0, sizeof(MqttPublish));
                    //    mqttCtx->publish.retain = 0;
                    //    mqttCtx->publish.qos = mqttCtx->qos;
                    //    mqttCtx->publish.duplicate = 0;
                    //    mqttCtx->publish.topic_name = mqttCtx->topic_name;
                    //    mqttCtx->publish.packet_id = mqtt_get_packetid();
                    //    mqttCtx->publish.buffer = mqttCtx->rx_buf;
                    //    mqttCtx->publish.total_len = (word16)rc;
                    //    rc = MqttClient_Publish(&mqttCtx->client, &mqttCtx->publish);
                    //    PRINTF("MQTT Publish: Topic %s, %s (%d)\n",
                    //        mqttCtx->publish.topic_name,
                    //        MqttClient_ReturnCodeToString(rc), rc);
                    //}
                    /* Keep Alive */
                    //else {
					    printf("Send ping\n");
                        rc = MqttClient_Ping(&mqttCtx->client);
                        if (rc == MQTT_CODE_CONTINUE) {
                            return rc;
                        }
                        else if (rc != MQTT_CODE_SUCCESS) {
                            PRINTF("MQTT Ping Keep Alive Error: %s (%d)\n",
                                MqttClient_ReturnCodeToString(rc), rc);
                            break;
                        }
                    //}
                }
                else if (rc != MQTT_CODE_SUCCESS) {
					//rc = MQTT_CODE_SUCCESS; //apa+++
					//return rc; //apa+++
                    /* There was an error */
                    PRINTF("MQTT Message Wait: %s (%d)\n",
                        MqttClient_ReturnCodeToString(rc), rc);
                    break;
                }
            } while (1);

            /* Check for error */
            if (rc != MQTT_CODE_SUCCESS) {
                goto disconn;
            }

            /* Unsubscribe Topics */
            XMEMSET(&mqttCtx->unsubscribe, 0, sizeof(MqttUnsubscribe));
            mqttCtx->unsubscribe.packet_id = mqtt_get_packetid();
            mqttCtx->unsubscribe.topic_count =
                sizeof(mqttCtx->topics) / sizeof(MqttTopic);
            mqttCtx->unsubscribe.topics = mqttCtx->topics;
        }

        case WMQ_UNSUB:
        {
            mqttCtx->stat = WMQ_UNSUB;
            PRINTF("MQTT Exiting...");

            /* Unsubscribe Topics */
            rc = MqttClient_Unsubscribe(&mqttCtx->client, &mqttCtx->unsubscribe);
            if (rc == MQTT_CODE_CONTINUE) {
                return rc;
            }
            PRINTF("MQTT Unsubscribe: %s (%d)\n",
                MqttClient_ReturnCodeToString(rc), rc);
            if (rc != MQTT_CODE_SUCCESS) {
                goto disconn;
            }
            mqttCtx->return_code = rc;
        }

        case WMQ_DISCONNECT:
        {
            /* Disconnect */
            rc = MqttClient_Disconnect(&mqttCtx->client);
            if (rc == MQTT_CODE_CONTINUE) {
                return rc;
            }
            PRINTF("MQTT Disconnect: %s (%d)\n",
                MqttClient_ReturnCodeToString(rc), rc);
            if (rc != MQTT_CODE_SUCCESS) {
                goto disconn;
            }
        }

        case WMQ_NET_DISCONNECT:
        {
            mqttCtx->stat = WMQ_NET_DISCONNECT;

            rc = MqttClient_NetDisconnect(&mqttCtx->client);
            if (rc == MQTT_CODE_CONTINUE) {
                return rc;
            }
            PRINTF("MQTT Socket Disconnect: %s (%d)\n",
                MqttClient_ReturnCodeToString(rc), rc);
        }

        case WMQ_DONE:
        {
            mqttCtx->stat = WMQ_DONE;
            rc = mqttCtx->return_code;
            goto exit;
        }

        default:
            rc = MQTT_CODE_ERROR_STAT;
            goto exit;
    } /* switch */

disconn:
    mqttCtx->stat = WMQ_NET_DISCONNECT;
    mqttCtx->return_code = rc;
    return MQTT_CODE_CONTINUE;

exit:

    /* Free resources */
    if (mqttCtx->tx_buf) WOLFMQTT_FREE(mqttCtx->tx_buf);
    if (mqttCtx->rx_buf) WOLFMQTT_FREE(mqttCtx->rx_buf);

    /* Cleanup network */
    MqttClientNet_DeInit(&mqttCtx->net);

    return rc;
}
