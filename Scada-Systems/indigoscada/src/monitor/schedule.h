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
*Header For: schedular object
*Purpose: this handles the running of events
*/
#ifndef include_schedule_hpp 
#define include_schedule_hpp
#include "common.h"
#include "realtimedb.h" 
#include "driver.h"

class PrintReport;
class ReportGenerator;

class Schedule : public QObject
{
	Q_OBJECT
	QDateTime LastPoll; // when we last looked at the schedular
	struct SchedItem // track for periodic scheduled items
	{
		QString Name;
		unsigned Interval;
		unsigned Countdown;
		SchedItem(const QString &name,unsigned i) : Name(name),Interval(i),Countdown(0){};
		SchedItem(const SchedItem &s) : Name(s.Name), Interval(s.Interval), Countdown(s.Countdown){};
	};
	enum{tEvents = 1,tPeriodic}; // transaction codes
	//
	typedef std::vector<SchedItem> SchedList;
	SchedList Periodics; // list of periodic items
	//
	////////////support for PrintReport///////////////////////
	PrintReport *prtrep;
	////////////support for ReportGeneretor////////////////
	ReportGenerator *repgen;
	//////////////////////////////////////////////////////////
	public:
	Schedule(QObject *parent, const char *name = "schedule");
	~Schedule();

	void Restart() // reschedule
	{
		Periodics.clear();  
		GetConfigureDb()->DoExec(this,"select * from SCHEDULE where FREQMODE=8;",tPeriodic);
	};
	static QString MakeSelectString(const QDateTime &dt, const QDateTime &lastPoll); // generates the select string for events
	public slots:
	void ConfigQueryResponse (QObject *,const QString &, int, QObject*);  // handles configuration responses
	void Tick(); // state machine
};
#endif

