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

#ifndef IEC_103_DRIVER__INSTANCE
#define IEC_103_DRIVER__INSTANCE

#include "iec103driver.h"
#include "IndentedTrace.h"
#include "clear_crc_eight.h"
#include "iec104types.h"
#include "iec_item.h"
////////Middleware////////////
#include "RIPCThread.h"
#include "RIPCFactory.h"
#include "RIPCSession.h"
#include "RIPCServerFactory.h"
#include "RIPCClientFactory.h"
#include "ripc.h"
//////////////////////////////

/////////////////////////fifo///////////////////////////////////////////
extern void iec_call_exit_handler(int line, char* file, char* reason);
#include "fifoc.h"
#define MAX_FIFO_SIZE 65535
////////////////////////////////////////////////////////////////////////

////////////////////////////Middleware//////////////////////////////////
struct subs_args{
	RIPCQueue* queue_monitor_dir;
	fifo_h fifo_monitor_direction;
};

void consumer(void* pParam);
extern int exit_consumer;
////////////////////////////Middleware//////////////////////////////////

class Iec103DriverThread;

class IEC_103_DRIVERDRV Iec103driver_Instance : public DriverInstance 
{
	Q_OBJECT
	//
	//
	enum
	{
		tUnitProperties = 1,tList, tSamplePointProperties, tListUnits, 
		tGetSamplePointNamefromIOA, tGetIOAfromSamplePointName
	};
	//
	//
	
//	QStringList SampleList; // list of sample points
	bool fFail;
	QTimer *pTimer; // timer object for driving state machine
	int Retry; // the retry count
	int Countdown; // the countdown track
	int State; // the state machine's state 
	
	//  
	int Sp; //Current sample point index under measurement
	bool InTick; //tick sentinal
	int IecItems;
	
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
	//
	Track* Values;

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

	enum // states for the state machine
	{
		STATE_IDLE = 0,
		STATE_RESET,
		STATE_ASK_GENERAL_INTERROGATION,
		STATE_GENERAL_INTERROGATION_DONE,
		STATE_FAIL,
		STATE_RUNNING
	};

	public:
	Iec103DriverThread *pConnect;
	unsigned int msg_sent_in_control_direction;
	//
	Iec103driver_Instance(Driver *parent, const QString &name, int instance_id) : 
	DriverInstance(parent,name),fFail(0), Countdown(1),
	State(STATE_RESET),InTick(0),Retry(0),Sp(0),IecItems(1), Values(NULL),
	ParentDriver(parent),msg_sent_in_control_direction(0), instanceID(instance_id),
    pConnect(NULL)
	{
		IT_IT("Iec103driver_Instance::Iec103driver_Instance");
		connect (GetConfigureDb (),
		SIGNAL (TransactionDone (QObject *, const QString &, int, QObject*)), this,
		SLOT (QueryResponse (QObject *, const QString &, int, QObject*)));	// connect to the database

		pTimer = new QTimer(this);
		connect(pTimer,SIGNAL(timeout()),this,SLOT(Tick()));
		pTimer->start(1000); // start with a 1 second timer

		/////////////////////Middleware/////////////////////////////////////////////////////////////////
		char fifo_control_name[150];
		char str_instance_id[20];
        itoa(instance_id + 1, str_instance_id, 10);
		strcpy(fifo_control_name,"fifo_control_direction");
        strcat(fifo_control_name, str_instance_id);
        strcat(fifo_control_name, "iec103");
		
		char fifo_monitor_name[150];
		itoa(instance_id + 1, str_instance_id, 10);
		strcpy(fifo_monitor_name,"fifo_monitor_direction");
        strcat(fifo_monitor_name, str_instance_id);
        strcat(fifo_monitor_name, "iec103");

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

		/////////////////////////////////////local fifo//////////////////////////////////////////////////////////
		const size_t max_fifo_queue_size = MAX_FIFO_SIZE;
		
		strcat(fifo_monitor_name, "_fifo_");

		fifo_monitor_direction = fifo_open(fifo_monitor_name, max_fifo_queue_size, iec_call_exit_handler);

		arg.fifo_monitor_direction = fifo_monitor_direction;
		///////////////////////////////////////////////////////////////////////////////////////////////////

		/////////////////////Middleware/////////////////////////////////////////////////////////////////
		unsigned long threadid;
	
		CreateThread(NULL, 0, LPTHREAD_START_ROUTINE(consumer), (void*)&arg, 0, &threadid);
		/////////////////////Middleware/////////////////////////////////////////////////////////////////
	};

	~Iec103driver_Instance()
	{    
		IT_IT("Iec103driver_Instance::~Iec103driver_Instance");

		if(Values)
		{
			delete[] Values;
			Values = NULL;
		}

		///////////////////////////////////Middleware//////////////////////////////////////////////////
		exit_consumer = 1;
//		Sleep(3000);
		fifo_close(fifo_monitor_direction);
		queue_monitor_dir->close();
		queue_control_dir->close();
		session1->close();
		session2->close();
		delete session1;
		delete session2;
		///////////////////////////////////Middleware//////////////////////////////////////////////////
	};
	//
	void Fail(const QString &s)
	{
		FailUnit(s);
		fFail = true;
	};

	InstanceCfg Cfg; // the cacheable stuff
	Driver* ParentDriver;
	QString unit_name;
    int instanceID; //Equals to "line concept" of a SCADA driver

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
	//
};

#endif