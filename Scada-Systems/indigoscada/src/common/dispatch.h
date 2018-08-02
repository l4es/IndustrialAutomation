/*
 *                         IndigoSCADA
 *
 *   This software and documentation are Copyright 2002 to 2009 Enscada 
 *   Limited and its licensees. All rights reserved. See file:
 *
 *                     $HOME/LICENSE 
 *
 *   for full copyright notice and license terms. 
 *
 */

#ifndef DISPATCH_H
#define DISPATCH_H

#include "common.h"
#include "general_defines.h"
#include "IndentedTrace.h"

#define BUF 25

struct QSEXPORT NotificationData{

	char* buf;
	char Databuf[BUF];

	public:

	int NotificationCode;
	char *Data;
	int Datalength;
	time_t Delay;

	NotificationData(int not = 0, const char* data = NULL, int datalength = 0, time_t delay = 0) :
	NotificationCode(not),buf(NULL),Data(NULL),Datalength(0),Delay(delay)
	{
		IT_IT("NotificationData::NotificationData");
		
		if(data)
		{
			if(datalength)
			{
				Datalength = datalength;
			}
			else
			{
				Datalength = strlen(data) + 1;
			}
			
			if (Datalength <= BUF) 
			{
				memcpy(Databuf, data, Datalength);
				Data = Databuf;
			}
			else
			{
				buf = new char[Datalength];
				memcpy(buf, data, Datalength);
				Data = buf;
			} 
		}
		else
		{
			*Databuf = '\0';
			Data = Databuf;
		}
	};
	//
	virtual ~NotificationData()
	{
		IT_IT("NotificationData::~NotificationData");

		if (buf) 
		{
			delete[] buf;
			buf = NULL;
		}
	};
};


#define END_READ_NOTIF 373
#define END_WRITE_NOTIF 737
#define END_WAIT_NOTIF 773

#include "iec104types.h"

//sizeof(dispatcher_extra_params) must be < string1 octects (100) in struct indigoscada_dispatch_message
typedef struct _dispatcher_extra_params{
	char string1[30];
	char string2[30];
	char string3[20];
	cp56time2a time_stamp;
	double value;
}dispatcher_extra_params;

class QSEXPORT NotificationEvent : public QEvent
{
	void *p; // who posted this event
	int o; 		// the op code
	
	char* buf;
	char Databuf[BUF];
	char *Data;

//	union
//	{
		int   d;	// data associated with this message
		void *pData;    // data
//	};
	//
	public:

	enum
	{
		//System commands
		CMD_LOGON = 100, // user has connected //parte da 100 per evitare duplicazioni con i cli_cmd
		CMD_LOGOUT, // user is logging off
		CMD_LOCK_SYSMGR, // lock as system manger
		CMD_UNLOCK_SYSMGR, // unlock as system manager
		CMD_ACK_ALARM, // ack an alarm on a single point
		CMD_ACK_ALL, // ack all alarms
		CMD_SET_RECEIPE, // set a receipe and restart
		CMD_CMD,	  // action a command
		CMD_GET_RECEIPE, // get the current receipe
		CMD_UPDATE_VALUE, // update a sample points value
		CMD_NOTIFY_ALARM, // we have a new alarm
		CMD_UPDATE_ALARM_GROUP, // alarm group has been updated
		//
		CMD_LAST,
		CMD_TRACE_ON,
		CMD_TRACE_OFF,
		CMD_SHUTDOWN_MONITOR,
		CMD_SHUTDOWN_SQLSERVER,
		CMD_SHUTDOWN_CLIENTS,
		
		//System notification
		CURRENT_NOTIFY,
		ACK_NOTIFY,
		ALARM_NOTIFY,
		UPDATE_NOTIFY,
		ALARMGROUP_NOTIFY,
		EVENT_NOTIFY,

		//serial notifications
		SERIAL_DRIVER_ERROR_NOTIFY,
		
		//Monitor
		//commands
		CMD_MONITOR_STOP,
		CMD_MONITOR_START,
		//notifications
		MONITOR_STARTED_NOTIFY,
		MONITOR_TICK_NOTIFY,
		MONITOR_STOPPED_NOTIFY,
		//sistema
		UI_EXE_DATA,
		//Archiver
		ARCHIVER_TICK_NOTIFY,
		CMD_ARCHIVER_RESTART, 
		CMD_ARCHIVER_STOP,
		CMD_ARCHIVER_START,
		ARCHIVER_STARTED_NOTIFY,
		ARCHIVER_STOPPED_NOTIFY,
		CMD_SHUTDOWN_ARCHIVER,
		CMD_SEND_COMMAND_TO_UNIT
	};
	//

	public:

	NotificationEvent(void *thread = NULL,unsigned op = 0, int i = 0, const char* data = NULL, int datalength = 0) 
	: QEvent(QEvent::User),p(thread),o(op),pData(NULL),d(i),Data(NULL),buf(NULL)
	{
		if(data)
		{
			if(datalength == 0)
			{
				datalength = strlen(data) + 1;
			}

			if (datalength <= BUF) 
			{
				memcpy(Databuf, data, datalength);
				Data = Databuf;
			}
			else
			{
				buf = new char[datalength];
				memcpy(buf, data, datalength);
				Data = buf;
			} 
		}
		else
		{
			*Databuf = '\0';
			Data = Databuf;
		}
	};

	virtual ~NotificationEvent()
	{
		if (buf) 
		{
			delete[] buf;
			buf = NULL;
		}
	};

	// 
	//  access functions
	void * caller() const { return p;};
	int opcode() const { return o;};
	const char* text() const { return Data;};
	int data() const { return d;};
	void SetPointer(void *pd) { pData = pd;}; // sometimes we want to set pointer data
	void *Pointer() const { return pData;};
};

class Dispatcher_Thread;
class Dispatcher_Reader_Thread;
class Dispatcher_Thread_Wait;

class QSEXPORT Dispatcher : public QObject 
{	
	Q_OBJECT	
	
	public:
	//
	QQueue <NotificationData> WriteQueue;  //  write queue, e' usata solo dal thread principale
	QQueue <NotificationData> WaitQueue;  //  wait queue, e' usata solo dal thread principale
	//
	Dispatcher_Thread *pConnect; 
	Dispatcher_Thread_Wait *pConnectWait; 
	Dispatcher_Reader_Thread *pConnectReader; 

	int pending_writes;
	int pending_reads;
	int pending_waits;

	Dispatcher() : pConnect(NULL),pConnectReader(0), pConnectWait(0),pending_writes(0),pending_reads(0),pending_waits(0)
	{
		IT_IT("Dispatcher::Dispatcher()");
		
		WriteQueue.setAutoDelete(true);
		WriteQueue.clear();

		WaitQueue.setAutoDelete(true);
		WaitQueue.clear();
	};
	virtual ~Dispatcher()
	{
		Disconnect();
	};
	virtual bool Connect(const QString &host, const QString &user, const QString &password = "");
	virtual bool Disconnect();
	void notificationEvent(NotificationEvent *); // message from thread to parent
	bool event(QEvent *e);
	virtual bool DoExec(int not = 0, const char *data = NULL, int datalength = 0, time_t delay = 0);
	void close();
	bool Ok();
	bool IsInRetry();
	bool IsConnected();
	
	signals:	
	
	void ReceivedNotify(int, const char *);
};

QSEXPORT Dispatcher * GetDispatcher(); //on primary host
QSEXPORT Dispatcher * GetSpareDispatcher(); //on spare host
//void SetDispatcher(Dispatcher *);

#include "stdtp.h"
#include "sync.h"

#ifndef QSFASTDB_CPP

USE_GIGABASE_NAMESPACE

#endif

class Dispatcher_Thread : QThread
{
	void run(); // thread main routine
	bool Done; // has the last transaction completed
	bool Trigger; // triggers a transaction
	int  dispatcher_handle;	
	bool fFail;
	NotificationData* notif;

	dbMutex mutex;
	dbEvent m_hevtSinc;
	dbEvent m_hevtEnd;
	//
	QString Host,User,Password;
	dbThread connThread;
	//
	Dispatcher *Parent;
	bool fExit;

	int rc; //return code
	bool server_is_connected;
	bool retry_to_connect_to_server_pending;
	bool is_started_dispatcher_thread_wait;
				
	public:
	Dispatcher_Thread(Dispatcher *parent,const QString &host, const QString &user, const QString &password);

	~Dispatcher_Thread();
	
	bool Ok()
	{
		IT_IT("Dispatcher_Thread::Ok");
		
		return !fFail; // 
	};

	bool IsInRetry()
	{
		return retry_to_connect_to_server_pending;
	}

	bool IsConnected()
	{
		return server_is_connected;
	}

	void SetFlagThreadWaitStarted()
	{
		is_started_dispatcher_thread_wait = true;
	}

	bool SetNotif(NotificationData* n);

	int GetHandle()
	{
		IT_IT("Dispatcher_Thread::GetHandle");
		
		return dispatcher_handle; // 
	};

	static void thread_proc ConnectorToDispatcherServer(void* arg) 
	{ 
        IT_IT("Dispatcher_Thread::ConnectorToDispatcherServer");
		
		((Dispatcher_Thread*)arg)->ConnectToServer();
    }

	void ConnectToServer();

	friend class Dispatcher;
};

class Dispatcher_Thread_Wait : QThread
{
	void run(); // thread main routine
	bool Done; // has the last transaction completed
	bool Trigger; // triggers a transaction
	bool fFail;
	NotificationData* notif;

	dbMutex mutex;
	dbEvent m_hevtSinc;
	dbEvent m_hevtEnd;
	dbEvent m_hevtWait;
	//
	Dispatcher *Parent;
	bool fExit;
			
	public:
	Dispatcher_Thread_Wait(Dispatcher *parent);

	~Dispatcher_Thread_Wait();
	
	bool Ok()
	{
		IT_IT("Dispatcher_Thread_Wait::Ok");
		
		return !fFail; // 
	};

	bool SetNotif(NotificationData* n);

	friend class Dispatcher;
};

class Dispatcher_Reader_Thread : QThread
{
	void run(); // thread main routine
	bool Done; // has the last transaction completed
	bool Trigger; // triggers a transaction
	int  dispatcher_handle;
	bool fFail;

	dbMutex mutex;
	dbEvent m_hevtEnd;
	//
	Dispatcher *Parent;
	bool fExit;
	bool dsp_connection;
				
	public:
	Dispatcher_Reader_Thread(Dispatcher *parent);

	~Dispatcher_Reader_Thread();

	bool Ok()
	{
		IT_IT("Dispatcher_Reader_Thread::Ok");
		
		return !fFail; // 
	};

	friend class Dispatcher;
};

// open dispatcher on primary host
QSEXPORT bool OpenDispatcherConnection();
QSEXPORT bool ConnectToDispatcher();
// close dispatcher 
QSEXPORT void CloseDispatcherConnection();		
//disconnect it
QSEXPORT void DisconnectFromDispatcher();

// open dispatcher on spare host
QSEXPORT bool OpenSpareDispatcherConnection();
QSEXPORT bool ConnectToSpareDispatcher();
// close dispatcher 
QSEXPORT void CloseSpareDispatcherConnection();		
//disconnect it
QSEXPORT void DisconnectFromSpareDispatcher();


#endif