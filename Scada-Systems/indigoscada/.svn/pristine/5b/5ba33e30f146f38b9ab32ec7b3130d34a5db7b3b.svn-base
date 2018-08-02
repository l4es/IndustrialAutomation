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
*Header For: monitoring engine
*Purpose:
*/
#ifndef include_monitor_hpp 
#define include_monitor_hpp

#include "realtimedb.h"
#include "historicdb.h"
#include "fifoc.h"

#ifdef USE_RIPC_MIDDLEWARE
////////Middleware////////////
#include "RIPCThread.h"
#include "RIPCFactory.h"
#include "RIPCSession.h"
#include "RIPCServerFactory.h"
#include "RIPCClientFactory.h"
#include "ripc.h"

typedef class Monitor* par;

struct subs_args{
	par parent;
};

//////////////////////////////
#endif

////////////////////////////Middleware///////////////////////////////////////////////////////
#include "iec_item_type.h"
extern void onRegFail(void *param);
extern void recvCallBack(const ORTERecvInfo *info,void *vinstance, void *recvCallBackParam); 
/////////////////////////////////////////////////////////////////////////////////////////////

#include <map>
class SamplePoint;
class Schedule;
class MessageDisplay;
class Driver;
class Dispatcher;
struct NotificationData;

class Monitor : public QObject
{
	Q_OBJECT
	//
	bool fStarted;               // started flag
	int MidnightReset;
	int lastHour; // used to track midnight resets
	int MaxRetryReconnectToDispatcher;
	int MaxRetryReconnectToRealTimeDb;
	int MaxRetryReconnectToHistoricDb;
	int MaxRetryReconnectToSpareDispatcher;
	int MaxRetryReconnectToSpareRealTimeDb;
	//
	public:
	//
	static Monitor * Instance; // instance of monitor object 
	Monitor(QObject *parent = NULL, RealTimeDbDict *db_dct = NULL, Dispatcher *dsp = NULL);
	~Monitor();
	//
	enum 
	{	
		tUnitTypes = 1,tAlarmGroups,tReceipe,tReceipeRecord,tUnits,
		tSamples,tSerialPorts,tTags,tTable,tUpdateDone,tAllUpdated,
		tTagsCurrent,tGet,tSamplesCurrent,tGetUnitfromIOA
	};

	QTranslator translation;
	//
	typedef std::map<QString, Driver *, std::less<QString> > DDict; // the driver dictionary
	DDict drivers;
	//
	Schedule *pSchedule; // the event schedular object
	//
	unsigned long SequenceNumber; // monitor's idea of a sort of time 
	bool fHalt; // halt flag
	//
	void UpdateCurrentValue( const QString &name, SamplePoint &sp);

	void Command(const QString & str, BYTE cmd, LPVOID lpPa, DWORD pa_length, DWORD ipindex)
	{
		emit DoCommand(str, cmd, lpPa, pa_length, ipindex);
	};

	void ResetStatistics(); // reset the stats
	void ResetTables(); // reset the tables
	//
	Dispatcher *dispatcher;
	RealTimeDbDict db_dictionary;
	QSDatabase *CfgDb;
	QSDatabase *CurDb;
	QSDatabase *ResDb;
	/////////////////middleware//////////////
	fifo_h fifo_control_direction;

	#ifdef USE_RIPC_MIDDLEWARE
	struct subs_args arg;
	int exit_command_thread;
	RIPCQueue* queue_control_dir;
	#endif
	/////////////////////////////////////////
	//////Middleware/////////////
    ORTEDomain *domain;
	ORTESubscription *subscriber;
	iec_item_type    instanceRecv;
	/////////////////////////////
	//
	public slots:
	//
	
	//Real time database on Host A 	(Primary)
	void ConfigQueryResponse (QObject *,const QString &, int, QObject*);  // handles configuration responses
	void CurrentQueryResponse (QObject *,const QString &, int, QObject*); // current value responses
	void ResultsQueryResponse (QObject *,const QString &, int, QObject*); // results responses

	//Real time database on Host B (Spare)
	//void SpareConfigQueryResponse (QObject *,const QString &, int, QObject*);  // handles configuration responses
	//void SpareCurrentQueryResponse (QObject *,const QString &, int, QObject*); // current value responses
	//void SpareResultsQueryResponse (QObject *,const QString &, int, QObject*); // results responses

	//Historic database
	void HistoricResultsQueryResponse (QObject *,const QString &, int, QObject*); // results responses

	void ReceivedNotify(int, const char *);
	//void SpareReceivedNotify(int, const char *);

	void Start(); // start it all off
	void Stop(); // stop everything
	void Tick();
	void Trace(const QString &, const QString &);
	void get_items_from_global_fifo(void);
	void get_utc_host_time(struct cp56time2a* time);
	//
	signals:
	void DoCommand(const QString &, BYTE cmd, LPVOID lpPa, DWORD pa_length, DWORD ipindex);
	void TraceOut(const  QString &, const QString &); // trace out to listeners
};

#endif

