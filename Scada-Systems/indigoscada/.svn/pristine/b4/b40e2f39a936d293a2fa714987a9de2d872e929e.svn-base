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


/*
*Header For: driver main definitions
*Purpose:
*/   

#ifndef include_driver_hpp 
#define include_driver_hpp 

#include "realtimedb.h"
#include "common.h"
#include "results.h"
#include "IndentedTrace.h"

#ifdef USE_RIPC_MIDDLEWARE
////////Middleware////////////
#include "RIPCThread.h"
#include "RIPCFactory.h"
#include "RIPCSession.h"
#include "RIPCServerFactory.h"
#include "RIPCClientFactory.h"
#include "ripc.h"
//////////////////////////////
#endif
#include "iec_item_type.h"
//
// base class for driver class
// this class has two functions
// to handle the user interface and configuration bits
// to handle the hardware control and interface
// 
//
// This is the message structure for driver threads to talk to other threads (usually parents)
//
class DriverThread;
class DriverEvent : public QEvent
{
	DriverThread *p; // who posted this event
	int o; 		// the op code
	QString t;  // text associated with message
	QString t1; // 2nd text data string
	union
	{
		int   d;	// data associated with this message
		void *pData;    // data
	};
	//
	public:
	enum
	{
		OpTrace = 0, // trace message
		OpUnitFail, // we have a failure of a unit
		OpUnitUnFail, // failure has ended on the unit
		OpSampleFail, // a sample point has failed
		OpSampleUnFail, // a sample point has stopped failing
		OpTerminate, // the thread wants to terminate
		OpPostList, // a list of results from RTU is being posted
		OpNext,	// extensions start from here
		OpEndTransaction, // end of transaction
		OpReceiveCommand, // read command
		OpEndCommand, //Result received from RTU after command execution, e' come OpEndTransaction
		OpSendAlarmString, //driver thread has an alarm string
		OpSendEventString //driver thread has an event string
	};
	//
	DriverEvent(DriverThread *thread,unsigned op, int i, const QString &s = QString::null,const QString &s1 = QString::null) 
	: QEvent(QEvent::User),p(thread),o(op),t(s),t1(s1),d(i),pData(NULL)
	{
	};
	// 
	//  access functions
	DriverThread * caller() const { return p;};
	int opcode() const { return o;};
	const QString & text() const { return t;};
	int data() const { return d;};
	const QString & text1() { return t1;};
	void SetPointer(void *pd) { pData = pd;}; // sometimes we want to set pointer data
	void *Pointer() const { return pData;};
};
// 
// derive from QObject so we have slots and signals
// 
//
class QSEXPORT Driver : public QObject //it implements the user interface of a driver
{
	Q_OBJECT
	QString Name;
	public:
	Driver(QObject *parent,const QString &name);
	virtual ~Driver();
	//
	// user interface stuff
	// 
	// this is called to configure the named unit for the named receipe
	virtual void UnitConfigure(QWidget *parent, const QString &name, const QString &receipe="(default)"); // configure a unit
	// this returns the names of the types supported by a given unit
	virtual void SetTypeList(QComboBox *pCombo, const QString &unitname); // set the type list for unit type
	// this returns the input ids / indeices for the given unit
	virtual void GetInputList(const QString &type, QStringList &,const QString &unit, const QString &name); // get the permitted input IDs
	// this returns the sample point additional configuration (eg 4-20mA upper and lower scale values)
	virtual QWidget * GetSpecificConfig(QWidget *parent,const QString &spname, const QString &sptype); //specific config for sample point of type
	const QString &GetName() const { return Name;}; // driver name
	virtual void GetTagList(const QString &type, QStringList &list,const QString &unit, const QString &name = ""); // returns the permitted tags for a given type for this unit
	virtual void CreateNewUnit(QWidget *parent, const QString &, int) {}; // create a new unit - quick configure
	static void CreateSamplePoint(const QString &name, QStringList &, const QString &init); // helper function to create a sample point's
	virtual void CheckTags(const QString &Name, const QStringList &currentList, const QStringList &list); // check a tag list set
	//
	// if this returns true then the driver has only one instance the same name as the driver - this is implicitly created
	virtual bool AmUnique() {return false;};
	virtual void DropAllSpecTables(QStringList &list); // drop all specific tables
	virtual void DropOneSpecTable(const QString &name); // drop one specific tables
	//  
	public slots:
	virtual void Start(); // start everything under this driver's control
	virtual void Stop(); // stop everything under this driver's control
	virtual void Command(const QString &, BYTE, LPVOID, DWORD, DWORD); // process a command for a named unit 
	virtual void CommandDlg(QWidget *parent, const QString &name); // command dialog
	void Trace(const QString &, const QString &);

	signals:
	void TraceOut(const QString &, const QString &);
};
//
class Results; // results and curent value interface objects
struct SendRecePacket;
//
//
class QSEXPORT DriverInstance : public QObject // the actual driver bits
{
	Q_OBJECT
	protected:
	//
	QString Name;      // unittype name. i.e "Simulator"
	QString UnitName;  //the unit name given by the creator of the system. i.e "Sim" or "Tintom"
	Results *pResults; // results processing object
	//
	bool fTrace; // unit has trace enabled
	bool fTest;  // unit is in test mode
	//
	//
	QQueue <SendRecePacket> InQueue; //e' usata solo dal thread principale

	int pending_transactions;

	static QMutex Lock; // the mutex to control access to shared data
	//
	//
	public:
	DriverInstance(Driver *parent, const QString &name);
	virtual ~DriverInstance(); 
	//
	void PostList(const QString &name, IECValueList &list); // send a list of values to the results database
	void PostValue(const QString &name, const QString &tag, double value); // post a single value to the results database
	//
	// find a sample point given the unit name, the type and input index (ie we donot know the name)
	const QString  FindSamplePoint(const QString &unit, const QString &type, const QString &ip);
	//
	// Set all points in the unit to a given (alarm) state with a comment
	void SetAllInUnit(const QString &comment, int state); // put all sample points associated with a unit into a state
	// fail this unit with comment 
	void FailUnit(const QString &comment)
	{
		IT_IT("DriverInstance::FailUnit");

		SetAllInUnit(comment,FailureLevel);
		QSLogAlarm("Monitor",tr("Unit ") + Name + tr(" has failed : ") + comment );

		if(fTrace)
		{
			Trace(tr("Unit ") + Name + tr(" has failed : ") + comment);
		}
	};
	//
	// un fail the unit - set it to idle while it recovers
	void UnFailUnit(const QString &comment) 
	{
		IT_IT("DriverInstance::UnFailUnit");

		SetAllInUnit(comment,NoLevel);
		QSLogAlarm("Monitor",tr("Unit ") + Name + tr(" has stopped failing : ") + comment );
		
		if(fTrace)
		{
			Trace(tr("Unit ") + Name + tr(" has stopped failing : ") + comment);
		}
	};
	//
	void FailSamplePoint(const QString &name, const QString &comment); // fail the sample point with comment
	void UnFailSamplePoint(const QString &name, const QString &comment); //un fail the sample point with comment
	void SetSamplePointState(const QString &name, const QString &comment,int state); // set the state of a sample point
	void OverWriteSamplePointState(const QString &name, const QString &comment,int state); // overwrite the state of a sample point
	//
	//
	static QStringList EnabledUnits; // this is the list of enabled units
	//
	// We need the current receipe name for filtering configurations
	static QString FormUnitList(); // build the list of enabled units for SQL transaction filtering NAME in(...)
	static QString FormSamplePointList();// build the list of enabled sample points for SQL transactions
	//
	typedef std::map<QString,QString, std::less<QString> > DriverProps; // driver properties dictionary - ashared values/flags
	static DriverProps Props; // the properites and semaphores
	//
	////////////////gloabal fifo///////////
	#ifdef USE_RIPC_MIDDLEWARE
	/////////////Middleware///////////////////////////////
    static RIPCFactory* global_factory1;
	static RIPCFactory* global_factory2;
	static RIPCSession* global_session1;
	static RIPCSession* global_session2;
	static RIPCQueue* fifo_global_monitor_direction;
	static RIPCQueue* fifo_global_control_direction;
	#endif
	///////////////////////////////////////
	//////Middleware/////////////
	static ORTEPublication *global_publisher;
	static iec_item_type    global_instanceSend;
	/////////////////////////////
	//
	void AddProp(const QString &k,const QString &v) // add a property
	{
		IT_IT("DriverInstance::AddProp");
		Lock.lock();
		DriverProps::value_type pr(k,v);
		Props.insert(pr);
		Lock.unlock();
	};
	const QString FindProp(const QString &s) // look for a property one
	{
		IT_IT("DriverInstance::FindProp");
		Lock.lock();
		DriverProps::iterator i = Props.find(s);
		if(!(i == Props.end()))
		{
			Lock.unlock();
			return (*i).second;
		};
		Lock.unlock();
		return QString::null; // return null value
	};
	void DelProp(const QString &s) // delete a property
	{
		IT_IT("DriverInstance::DelProp");
		Lock.lock();
		DriverProps::iterator i = Props.find(s);
		if(!(i == Props.end())) Props.erase(i);
		Lock.unlock();
	};
	void Trace(const QString &s)
	{  
		IT_IT("DriverInstance::Trace");
		if(fTrace)
		{
			// parent must be a Driver object 
			Lock.lock(); // one update at a time
			((Driver *)parent())->Trace(Name,s);
			Lock.unlock();
		};
	};
	//
	bool InTest() const { IT_IT("DriverInstance::InTest"); return fTest;}; // are we in test mode
	virtual void driverEvent(DriverEvent *); // message from thread to parent
	virtual void threadDelete(DriverThread *){}; // a thread is a about to be killed - thread may or may not have exitted
	//
	// The event handler
	virtual bool event(QEvent *e)
	{
		IT_IT("DriverInstance::event");

		if(e->type() == QEvent::User)
		{
			DriverEvent *d = ((DriverEvent *)e); // handle the driver event
			driverEvent(d);
			return true;
		};
		return QObject::event(e);
	};

	SendRecePacket & CurrentTransaction();
	unsigned GetTransQueueSize();
	//
	//
	public slots:
	//
	virtual void Start(); // start the driver for this unit control
	virtual void Stop(); // stop the driver for this unit
	virtual void Command(const QString &, BYTE, LPVOID, DWORD, DWORD); // process a command for a named unit
	virtual void Tick();
};

class SerialConnect;

#define INT_BUF_SEND_RECE 25


struct SendRecePacket{

	private:
	BYTE	bufferparam[INT_BUF_SEND_RECE];
	BYTE	answer[INT_BUF_SEND_RECE];

	public:
	
	SerialConnect* port;	// serial port of the destination device
    BYTE	Dest;			// destination device
    DWORD	CommandType;	// command type
	BYTE*	lpParams;		// command parameters 
	BYTE*	lpAnswerData;	// returned data, the user of the SendRecePacket can allocate memory in it
							//and the deallocation is automatic by the destructor
	DWORD	dwAnswerType;	// type of answer 

	DWORD	ActualAnswerLen;//actual response length
	DWORD	ActualParamLen;	//actual command parameters length

	//Extra info
	DWORD	IpIndex;		//Index of sample point
	DWORD	State;			//State associated with this packet
	DWORD   Client;	//the ui client, if any, who generated this packet, Client is a pointer to a QObject, use cast it bufore use : Client(QObject*) 
	DWORD   clientOperation; //it is the client operation that initiated the command

	SendRecePacket(SerialConnect *p = NULL, BYTE de = 0, DWORD co = 0, void* lpPa = NULL, DWORD param_lenght = 0, DWORD ipindex = 0, DWORD state = 0, DWORD client = 0, DWORD operation = 0) : 
	port(p),Dest(de),CommandType(co),dwAnswerType(0),lpParams(0),lpAnswerData(answer),
	ActualAnswerLen(0),ActualParamLen(0),IpIndex(ipindex),State(state),Client(client),
	clientOperation(operation)
	{
		IT_IT("SendRecePacket::SendRecePacket");

		if(lpPa)
		{
			if(param_lenght)
			{
				ActualParamLen = param_lenght;
			}
			else
			{
				ActualParamLen = strlen((const char *)lpPa) + 1;
			}
							
			if(ActualParamLen <= INT_BUF_SEND_RECE)
			{
				memcpy(bufferparam, lpPa, ActualParamLen);
				lpParams = bufferparam;
			}
			else
			{
				lpParams = new BYTE[ActualParamLen];
				memcpy(lpParams, lpPa, ActualParamLen);
			}
		}
		else
		{
			lpParams = bufferparam;
			memset(lpParams, 0x00,sizeof(lpParams));
		}

		memset(lpAnswerData, 0x00,sizeof(lpAnswerData));
		
	};
	//
	
	SendRecePacket(const SendRecePacket &t) : 
	port(t.port),Dest(t.Dest),CommandType(t.CommandType),dwAnswerType(t.dwAnswerType),ActualAnswerLen(t.ActualAnswerLen),
	ActualParamLen(t.ActualParamLen),IpIndex(t.IpIndex),State(t.State),Client(t.Client),clientOperation(t.clientOperation)
	{
		if(t.lpParams)
		{
			if(ActualParamLen == 0)
			{
				ActualParamLen = strlen((const char *)t.lpParams) + 1;
			}
			
			if(ActualParamLen <= INT_BUF_SEND_RECE)
			{
				memcpy(bufferparam, t.lpParams, ActualParamLen);
				lpParams = bufferparam;
			}
			else
			{
				lpParams = new BYTE[ActualParamLen];
				memcpy(lpParams, t.lpParams, ActualParamLen);
			}
		}
		else
		{
			lpParams = bufferparam;
			memset(lpParams, 0x00,sizeof(lpParams));
		}
		
		if(t.lpAnswerData)
		{
			if(t.ActualAnswerLen <= INT_BUF_SEND_RECE)
			{
				memcpy(answer, t.lpAnswerData, t.ActualAnswerLen);
				lpAnswerData = answer;
			}
			else
			{
				lpAnswerData = new BYTE[t.ActualAnswerLen];
				memcpy(lpAnswerData, t.lpAnswerData, t.ActualAnswerLen);
			}
		}
		else
		{
			lpAnswerData = answer;
			memset(lpAnswerData, 0x00,sizeof(lpAnswerData));
		}
		
	};
	//
	virtual ~SendRecePacket()
	{
		IT_IT("SendRecePacket::~SendRecePacket");

		if(lpParams != bufferparam)
		{
			delete[] lpParams;
		}
		if(lpAnswerData != answer) //it means that the user of the packet has allocated a memory
		{							//buffer whose pointer is contained in lpAnswerData
		   delete[] lpAnswerData;
		}
	};
};

#include <stdtp.h>
#include <sync.h>

class QSEXPORT DriverThread : public QThread // some base functions for a driver thread object
{
	protected:
	DriverInstance *Parent; 		// the parent object
	bool            fRequestHalt;	// set true if the parent wants the object to halt
	
	bool Done; // has the last transaction completed
	bool Trigger; // triggers a transaction
	bool fFail;
	bool fExit;
	SendRecePacket* pinternal_packet;
	//
	dbMutex mutex;
	dbEvent m_hevtSinc;
	dbEvent m_hevtEnd;

	public:
	DriverThread(DriverInstance *p) : 
	Parent(p), fRequestHalt(0),fFail(false),Done(true),Trigger(false),fExit(false)
	{ 
		IT_IT("DriverThread::DriverThread");
		m_hevtSinc.open();
		m_hevtEnd.open();
	};

	~DriverThread()
	{
		Parent = 0;
		IT_IT("DriverThread::~DriverThread");
		usleep(1000); // task switch 
		fExit = true; 
		m_hevtSinc.signal();

		{
			dbCriticalSection cs(mutex); //enter critical section
			m_hevtEnd.wait(mutex, 5);

		}//exit critical section
		
		if(running())
		{
			while(!finished()); //ristabilita il 05-12-09
			Sleep(50);
		}

		m_hevtSinc.close();
		m_hevtEnd.close();
	}

	bool SetCommand(SendRecePacket * packet);
	
	bool Ok()
	{
		IT_IT("DriverThread::Ok");
		
		return !fFail; // 
	}

	void Terminate(); // parent requests the thread close
	protected:	
	void Trace(const QString &s);
	void UnitFail(const QString &s); 
	void UnitUnFail(const QString &s); // failure has ended on the unit
	void SampleFail(const QString &name, const QString &s); // a sample point has failed
	void SampleUnFail(const QString &name, const QString &s); // a sample point has stopped failing
	void Terminating(); // the thread wants to terminate - fatal type of error - by the time the parent receives this the thread has exited
	void PostList(const QString &name, IECValueList &list); // send a list of values to the results database
	void PostValue(const QString &name, const QString &tag, double value); // post a single value to the results database

};
//
//

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//enum  {  tConfigLoad = 1, tConfigSave }; // transaction codes for specific configuratations

//
//
//
// the type of the entry point
// 
typedef Driver * (_cdecl * FncPtrDriverEntry)(QObject *); 
//
//
//
inline Driver * FindDriver(const QString &name) // finds a driver object
{
	IT_IT("FindDriver");

	#ifdef UNIX
	QString s = QSDRIVER_DIR + "/" + name + DLL_EXT; 
	#endif

	#ifdef WIN32
	QString s = QSDRIVER_DIR + QString("\\") + name + DLL_EXT; 
	#endif

	FncPtrDriverEntry pDriver = (FncPtrDriverEntry)GetDllEntry(s, DRIVER_ENTRY);
	if(pDriver)
	{
		IT_COMMENT("Driver FOUND");

		return (pDriver)(qApp); // create the driver interface (NB DO NOT delete when finished) - child of application
		// all instances deleted in the driver's _Unload function
	}

	IT_COMMENT("Driver NOT found");
	return 0;
}



#endif //include_driver_hpp
	
