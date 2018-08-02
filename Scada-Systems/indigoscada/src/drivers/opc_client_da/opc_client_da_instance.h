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

#ifndef OPC_CLIENT_DA__INSTANCE
#define OPC_CLIENT_DA__INSTANCE

#include "opc_client_da.h"
#include "IndentedTrace.h"
#include "clear_crc_eight.h"
#include "iec104types.h"
#include "iec_item.h"

#ifdef USE_RIPC_MIDDLEWARE
////////////////////////////Middleware/////////////////////////////////////////////////////////////
#include "RIPCThread.h"
#include "RIPCFactory.h"
#include "RIPCSession.h"
#include "RIPCServerFactory.h"
#include "RIPCClientFactory.h"
#include "ripc.h"
/////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////Middleware/////////////////////////////////////////////////////////////
struct subs_args{
	RIPCQueue* queue_monitor_dir;
	fifo_h fifo_monitor_direction;
};

void consumer(void* pParam);
extern int exit_consumer;
/////////////////////////////////////////////////////////////////////////////////////////////
#endif

////////////////////////////Middleware/////////////////////////////////////////////////////////////
#include "iec_item_type.h"
extern void onRegFail(void *param);
extern Boolean  quite;
extern void recvCallBack(const ORTERecvInfo *info,void *vinstance, void *recvCallBackParam); 
/////////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////fifo///////////////////////////////////////////
extern void iec_call_exit_handler(int line, char* file, char* reason);
#include "fifoc.h"
#define MAX_FIFO_SIZE 65535
////////////////////////////////////////////////////////////////////////

typedef QMap<int, QString> IOANameMap;

class Opc_client_da_DriverThread;

class OPC_CLIENT_DADRV Opc_client_da_Instance : public DriverInstance 
{
	Q_OBJECT

	enum
	{
		tUnitProperties = 1,tList, tSamplePointProperties, tListUnits,
		tGetIOAfromSamplePointName, tSetTAgsParams,
		tSetSamplePointNamefromIOA
	};
	
	bool fFail;
	QTimer *pTimer; // timer object for driving state machine
	int Retry; // the retry count
	int Countdown; // the countdown track
	int State; // the state machine's state
	//  
	int Sp; //Current sample point index under measurement
	bool InTick; //tick sentinal
	int OpcItems;
	int n_rows; //sqlite_rows
	int m_columns;//sqlite_columns
	struct local_structItem* Config_db;
	IOANameMap ioa_name_map;
	
	struct  Track
	{
		QString Name;           // name of sample point
		SampleStatistic Stats;  // we track the stats  
		double LastValue;       // the last value
		bool   fSpotValue;        // do we report the last value or the mean of the values over the sample period
		unsigned  SamplePeriod; // how often we sample 
		QDateTime NextSample;
		bool fFailed; // flag if the sample point is in a failed state
		void clear()
		{
			LastValue = 0.0; fSpotValue = false;
			NextSample = QDateTime::currentDateTime();
			fFailed = false;
			Stats.reset();
		}; 
	};

	Track* Values;

	#ifdef USE_RIPC_MIDDLEWARE
	/////////////Middleware///////////////////////////////
    int          port;
    char const*  hostname;
    RIPCFactory* factory1;
	RIPCFactory* factory2;
	RIPCSession* session1;
	RIPCSession* session2;
	RIPCQueue*   queue_monitor_dir;
	RIPCQueue*   queue_control_dir;
	struct subs_args arg;
	//////////////////////////////////////////////////////
	#endif

	enum // states for the state machine
	{
		STATE_IDLE = 0,
		STATE_RESET,
		STATE_INIT_DB,
		STATE_INIT_DB_DONE,
		STATE_ASK_GENERAL_INTERROGATION,
		STATE_GENERAL_INTERROGATION_DONE,
		STATE_FAIL,
		STATE_RUNNING
	};

	public:
	Opc_client_da_DriverThread *pConnect;
	unsigned int msg_sent_in_control_direction;
	//
	Opc_client_da_Instance(Driver *parent, const QString &name, int instance_id) : 
	DriverInstance(parent,name),fFail(0), Countdown(1), pConnect(NULL),Config_db(NULL),
	State(STATE_RESET),InTick(0),Retry(0),Sp(0),OpcItems(1), Values(NULL),
	ParentDriver(parent),msg_sent_in_control_direction(0), instanceID(instance_id)
	{
		IT_IT("Opc_client_da_Instance::Opc_client_da_Instance");
		connect (GetConfigureDb (),
		SIGNAL (TransactionDone (QObject *, const QString &, int, QObject*)), this,
		SLOT (QueryResponse (QObject *, const QString &, int, QObject*)));	// connect to the database

		pTimer = new QTimer(this);
		connect(pTimer,SIGNAL(timeout()),this,SLOT(Tick()));
		//pTimer->start(1000); // start with a 1 second timer
		pTimer->start(100); // start with a 100 ms timer

		/////////////////////Middleware/////////////////////////////////////////////////////////////////
		char fifo_control_name[150];
		char str_instance_id[20];
        itoa(instance_id + 1, str_instance_id, 10);
		strcpy(fifo_control_name,"fifo_control_direction");
        strcat(fifo_control_name, str_instance_id);
        strcat(fifo_control_name, "da");
		
		char fifo_monitor_name[150];
		itoa(instance_id + 1, str_instance_id, 10);
		strcpy(fifo_monitor_name,"fifo_monitor_direction");
        strcat(fifo_monitor_name, str_instance_id);
        strcat(fifo_monitor_name, "da");

		#ifdef USE_RIPC_MIDDLEWARE
		port = 6000;
		hostname = "localhost";

		factory1 = RIPCClientFactory::getInstance();
		factory2 = RIPCClientFactory::getInstance();
		session1 = factory1->create(hostname, port);
		session2 = factory2->create(hostname, port);
		queue_monitor_dir = session1->createQueue(fifo_monitor_name);
		queue_control_dir = session2->createQueue(fifo_control_name);

		arg.queue_monitor_dir = queue_monitor_dir;
		///////////////////////////////////Middleware//////////////////////////////////////////////////
		#endif

		/////////////////////Middleware/////////////////////////////////////////////////////////////////
		ORTEDomainProp          dp; 
		ORTESubscription        *s = NULL;
		int32_t                 strength = 1;
		NtpTime                 persistence,deadline,minimumSeparation,delay;
		Boolean                 havePublisher = ORTE_FALSE;
		Boolean                 haveSubscriber = ORTE_FALSE;
		IPAddress				smIPAddress = IPADDRESS_INVALID;
		ORTEDomainAppEvents     events;

		ORTEInit();
		ORTEDomainPropDefaultGet(&dp);
		NTPTIME_BUILD(minimumSeparation, 0); //0 s
		NTPTIME_BUILD(delay, 1); //1 s

		//initiate event system
		ORTEDomainInitEvents(&events);

		events.onRegFail = onRegFail;

		//Create application     
		domain = ORTEDomainAppCreate(ORTE_DEFAULT_DOMAIN,&dp,&events,ORTE_FALSE);

		iec_item_type_type_register(domain);

		//Create publisher
		NTPTIME_BUILD(persistence, 5); //5 s
		
		publisher = ORTEPublicationCreate(
		domain,
		fifo_control_name,
		"iec_item_type",
		&instanceSend,
		&persistence,
		strength,
		NULL,
		NULL,
		NULL);

		//Create subscriber
		NTPTIME_BUILD(deadline,3);

		subscriber = ORTESubscriptionCreate(
		domain,
		IMMEDIATE,
		BEST_EFFORTS,
		fifo_monitor_name,
		"iec_item_type",
		&instanceRecv,
		&deadline,
		&minimumSeparation,
		recvCallBack,
		this,
		smIPAddress);
		///////////////////////////////////Middleware//////////////////////////////////////////////////


		/////////////////////////////////////local fifo//////////////////////////////////////////////////////////
		const size_t max_fifo_queue_size = MAX_FIFO_SIZE;
		
		strcat(fifo_monitor_name, "_fifo_");

		fifo_monitor_direction = fifo_open(fifo_monitor_name, max_fifo_queue_size, iec_call_exit_handler);

		#ifdef USE_RIPC_MIDDLEWARE
		arg.fifo_monitor_direction = fifo_monitor_direction;
		///////////////////////////////////////////////////////////////////////////////////////////////////

		/////////////////////Middleware/////////////////////////////////////////////////////////////////
		unsigned long threadid;
	
		CreateThread(NULL, 0, LPTHREAD_START_ROUTINE(consumer), (void*)&arg, 0, &threadid);
		/////////////////////Middleware/////////////////////////////////////////////////////////////////
		#endif
	};

	~Opc_client_da_Instance()
	{    
		IT_IT("Opc_client_da_Instance::~Opc_client_da_Instance");

		if(Values)
		{
			delete[] Values;
			Values = NULL;
		}

		if(Config_db)
		{
			free(Config_db);
		}

		#ifdef USE_RIPC_MIDDLEWARE
		///////////////////////////////////Middleware//////////////////////////////////////////////////
		exit_consumer = 1;
		fifo_close(fifo_monitor_direction);
		queue_monitor_dir->close();
		queue_control_dir->close();
		session1->close();
		session2->close();
		delete session1;
		delete session2;
		///////////////////////////////////Middleware//////////////////////////////////////////////////
		#endif

		///////////////////////////////////Middleware//////////////////////////////////////////////////
		ORTEDomainAppDestroy(domain);
        domain = NULL;
		///////////////////////////////////Middleware//////////////////////////////////////////////////
	};

	void Fail(const QString &s)
	{
		FailUnit(s);
		fFail = true;
	};

	InstanceCfg Cfg; // the cacheable stuff
	Driver* ParentDriver;
	QString unit_name;
	int instanceID; //Equals to "line concept" of a SCADA driver

	//////Middleware/////////////
    ORTEDomain *domain;
	ORTEPublication *publisher;
	ORTESubscription *subscriber;
	iec_item_type    instanceSend;
	iec_item_type    instanceRecv;
	/////////////////////////////

	////////////////local fifo///////////
	fifo_h fifo_monitor_direction;
	///////////////////////////////
	
	void driverEvent(DriverEvent *); // message from thread to parent
	bool event(QEvent *e);
	bool Connect();					//connect to the DriverThread
	bool Disconnect();              //disconnect from the DriverThread
	bool DoExec(SendRecePacket *t);
	bool expect(unsigned int cmd);
	void removeTransaction();
	//////Middleware//////////////////////////////////////
	void get_utc_host_time(struct cp56time2a* time);
	void epoch_to_cp56time2a(cp56time2a *time, signed __int64 epoch_in_millisec);
	//////////////////////////////////////////////////////
	////////////////local fifo////////////////////////////
	void get_items_from_local_fifo(void);
	//////////////////////////////////////////////////////
	public slots:
	//
	virtual void Start(); // start everything under this driver's control
	virtual void Stop(); // stop everything under this driver's control
	virtual void Command(const QString & name, BYTE cmd, LPVOID lpPa, DWORD pa_length, DWORD ipindex); // process a command for a named unit 
	virtual void QueryResponse (QObject *, const QString &, int, QObject*); // handles database responses
	virtual void Tick();
};

#endif