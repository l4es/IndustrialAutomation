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
*Purpose: Event schedular
*/
#include "schedule.h"
#include "printrep.h"
#include "repgen.h"
#include "IndentedTrace.h"
/*
*Function:Schedule
*Inputs:parent object, name
*Outputs:none
*Returns:none
*/
Schedule::Schedule(QObject *parent, const char *name) :
QObject(parent,name), prtrep(NULL), repgen(NULL)
{
	IT_IT("Schedule::Schedule");
	
	connect (GetConfigureDb (),
	SIGNAL (TransactionDone (QObject *, const QString &, int, QObject*)), this,
	SLOT (ConfigQueryResponse (QObject *, const QString &, int, QObject*)));	// connect to the database
	//
	QTimer * pTimer = new QTimer(this);
	pTimer->connect(pTimer,SIGNAL(timeout()),this,SLOT(Tick()));
	pTimer->start(60000); // tick every minute - no need to go much faster  - maybe should be a system property
	//
	// cache the periodic events
	GetConfigureDb()->DoExec(this,"select * from SCHEDULE where FREQMODE=8;",tPeriodic);
	//
	LastPoll = QDateTime::currentDateTime();
	//
};
/*
*Function:~Schedule
*Inputs:none
*Outputs:none
*Returns:none
*/
Schedule::~Schedule()
{
	IT_IT("Schedule::~Schedule");
};
/*
*Function: MakeSelectString
*builds the select string
*Inputs:current time of day, last time we polled
*Outputs:none
*Returns:the select string to query the database with
*/
QString Schedule::MakeSelectString(const QDateTime &dt, const QDateTime &lastPoll)
{
	IT_IT("Schedule::MakeSelectString");
	
	QString res; 
	QString fmode = "(" + QString::number(EveryDay); // add in the every day commands
	//
	// get the time of day
	//
	// look for events to run on this day of the week
	// 
	QString d;
	d.setNum(dt.date().dayOfWeek() - 1);
	fmode += "," + d;
	//
	// look for first day of month 
	// 
	if(dt.date().day() <= 7)
	{
		fmode += "," + QString::number(dt.date().dayOfWeek() - 1 + FirstMonday);
	};
	//
	// last day in month
	// 
	if((dt.date().daysInMonth() - dt.date().day()) < 7)
	{
		fmode += "," + QString::number(dt.date().dayOfWeek() - 1 + LastMonday);
	};
	//
	// test for first working day
	// 
	if((dt.date().day() <= 3) && (dt.date().dayOfWeek() <=5))
	{
		//
		// the first working day must be the first of the month Tue-Fri 
		// or Mon where the date is 1st 2nd or 3rd of the month
		// 
		if((dt.date().day() == 1) || (dt.date().dayOfWeek() == 1)) // first working day of the month
		{
			fmode += "," + QString::number(FirstWorkingDay);
		}
	};
	//
	// test for last working day
	// 
	if((dt.date().dayOfWeek() <= 5) && (dt.date().day() >= (dt.date().daysInMonth() - 2))) 
	// Mon - Fri
	{
		if((dt.date().daysInMonth() == dt.date().day()) || // it is the last day in the month
		(dt.date().dayOfWeek() == 5)) // it is a Friday and within 2 days of the end of the month
		// is this the last friday with the last day on a sat or a sun
		{
			fmode += "," + QString::number(LastWorkingDay);
		};
	};
	//
	fmode += ")";
	//
	// trap midnight roll over
	// 
	if(lastPoll.date().day() < dt.date().day())
	{
		QString parc, parc1;
		
		// stradles mid night
		res = "select * from SCHEDULE where FREQMODE in " + fmode +  //" and TOD > '" + parc.sprintf("%ld",QStringToInterval(lastPoll.time().toString())) +
		" and  TOD < " + parc1.sprintf("%ld",QStringToInterval(dt.time().toString())) + ";"; 
	}
	else
	{
		QString parc, parc1;
		
		res = "select * from SCHEDULE where FREQMODE in " + fmode + " and TOD between " + parc.sprintf("%ld",QStringToInterval(lastPoll.time().toString())) +
		" and " + parc1.sprintf("%ld",QStringToInterval(dt.time().toString())) + ";"; 
	};
	return res;
};
/*
*Function:Tick
*State machine - queries for events that are runable now
*we run about once per minute
*Inputs:none
*Outputs:none
*Returns:none
*/
void Schedule::Tick() // state machine
{
	IT_IT("Schedule::Tick");
	
	//
	// look for events to run today
	QDateTime dt = QDateTime::currentDateTime();
	QString cmd = MakeSelectString(dt, LastPoll);
	GetConfigureDb()->DoExec(this,cmd,tEvents); // select these events
	//
	// look for periodic events - 
	// scan the cache action any that are due
	//
	if(Periodics.size() > 0)
	{
		//
		unsigned t = (unsigned)LastPoll.secsTo(dt);
		SchedList::iterator i = Periodics.begin();
		//
		for(; i != Periodics.end();i++)
		{
			if((*i).Countdown <= t)
			{
				(*i).Countdown = (*i).Interval; // reload
				// trigger the event - we load from the db and then do the trigger handling
				GetConfigureDb()->DoExec(this,"select * from SCHEDULE where NAME='"+(*i).Name+"';",tEvents);
			}
			else
			{
				(*i).Countdown -= t;
			};
		};
	};
	// that is it
	//
	LastPoll = dt.addSecs(-1); // go back one second 
};
/*
*Function:ConfigQueryResponse
*Inputs:clinet , command, transaction id
*Outputs:none
*Returns:none
*/
void Schedule::ConfigQueryResponse (QObject *p,const QString &c, int id, QObject* caller)  // handles configuration responses
{
	if(p != this) return;

	IT_IT("Schedule::ConfigQueryResponse");

	switch(id)
	{
		case tEvents:
		{
			IT_COMMENT("tEvents");
			
			// selected events
			int n = GetConfigureDb()->GetNumberResults();
			for(int k = 0; k < n; k++, GetConfigureDb()->FetchNext())
			{
				//
				//cerr << " Running Event " << (const char *)GetConfigureDb()->GetString("NAME") << endl;
				//  do we have trigger point
				//  
				bool fTriggered = true;
				if(GetConfigureDb()->GetString("TRIGGERPOINT") != tr(NONE_STR))
				{
					// 
					// get the tag value
					fTriggered = false;  // default to not triggered
					SamplePointDictWrap &d = Results::GetEnabledPoints();
					SamplePointDictWrap::iterator i = d.find(GetConfigureDb()->GetString("TRIGGERPOINT"));
					if(!(i == d.end()))
					{
						//
						SamplePoint::TagDict::iterator j = (*i).second.Tags.find(GetConfigureDb()->GetString("TRIGGERTAG"));
						if(!(j == (*i).second.Tags.end()))
						{
							if((*j).second.enabled)
							{
								//
								// found the tag
								// now test the trigger condition
								//  
								switch( GetConfigureDb()->GetInt("RELOP"))
								{
									case 0: // >
									fTriggered = (get_value_iec_value((*j).second.value) > GetConfigureDb()->GetDouble("TRIGGERVAL"));
									break;
									case 1: // <
									fTriggered = (get_value_iec_value((*j).second.value) < GetConfigureDb()->GetDouble("TRIGGERVAL"));
									break;
									case 2: // =
									fTriggered = (get_value_iec_value((*j).second.value) == GetConfigureDb()->GetDouble("TRIGGERVAL"));
									break;
									case 3: // !=
									fTriggered = (get_value_iec_value((*j).second.value) != GetConfigureDb()->GetDouble("TRIGGERVAL"));
									break;        
									default: // something wrong - will skip by default
									break;                        
								};
							}
						};
					};
				};
				//    
				if(fTriggered)
				{
					//
					// spawn report ?
					// 
					if(GetConfigureDb()->GetString("REPNAME") != tr(NONE_STR))
					{ 
						#ifdef UNIX
						
						QString cmd = QSBIN_DIR + "/repgen -display localhost:0 ";
						for(int i = 1; i < 7; i++)
						{
							// requires the arguments to set up properly  
							cmd +=  QString(" \"") + qApp->argv()[i] + "\" ";  
						};
						// add the report name
						cmd += GetConfigureDb()->GetString("REPNAME") ;
						if(GetConfigureDb()->GetBool("REPPRINT"))
						{
							// spawn the report with print to the default printer
							cmd += ";" + QSBIN_DIR + "/printrep -display localhost:0 " + GetConfigureDb()->GetString("REPNAME") + " lp &";
						}
						else
						{
							cmd += "&"; // put in the background
						};  
						//cerr << " Running |" << (const char *)cmd << "|" << endl;
						system((const char *)cmd); // spawn 

						#endif
						
						#ifdef WIN32
						if(repgen) delete repgen;
						repgen = new ReportGenerator(GetConfigureDb()->GetString("REPNAME"));
						
						if(GetConfigureDb()->GetBool("REPPRINT"))
						{
							if(prtrep) delete prtrep;                                       //LPT1 va portato fuori!
							prtrep = new PrintReport(GetConfigureDb()->GetString("REPNAME"),"LPT1");
						}
						#endif //WIN32
					};
					// 
					// trigger receipe
					// 
					if(GetConfigureDb()->GetString("RECEIPE") != tr(NONE_STR))
					{ 
						// stop monitoring
						//broadcast
						GetDispatcher()->DoExec(NotificationEvent::CMD_MONITOR_STOP);
						//
						// set the receipe property
						//
						GetConfigureDb()->DoExec(0,
						"update PROPS set DVAL='"
						+ GetConfigureDb()->GetString("RECEIPE")
						+ "' where SKEY='System' and IKEY='Receipe';",0);
						//
						//
						// and start it again
						//broadcast
						GetDispatcher()->DoExec(NotificationEvent::CMD_MONITOR_START);
						//
						QSLogEvent("Schedule", "Switched to Receipe " + GetConfigureDb()->GetString("RECEIPE"));
						//
					};
					// 
					// do action - NEVER EVER RUN AS ROOT FOR REAL  - YOU HAVE BEEN WARNED !!!
					// it might be a good idea to do a chroot after startup and to abort if the UID is 0
					// one could schedule an xterm session to a remote system and ....
					// 
					QString s = GetConfigureDb()->GetString("ACTIONS");
					if(!s.isEmpty())
					{
						//
						// if the first character is ! then this is a shell script
						if(s[0] == '!')
						{    
							QString cmd = s.mid(1) + "&";
							system((const char *)cmd); // runs in the background
						}   
						else
						{
							// it is an SQL transaction on the current values database
							// one would expect things like calculated values and so forth, changing the state of alarm outputs
							// to be done here
							GetCurrentDb()->DoExec(0,s,0); // we do the SQL transaction 
						};
					};            
				};
				//
			};
		};
		break;
		case tPeriodic:
		{
			IT_COMMENT("tPeriodic");
			
			// build table of periodic tasks - when one becomes runable fetch it
			int n = GetConfigureDb()->GetNumberResults();
			for(int i = 0; i < n; i++, GetConfigureDb()->FetchNext())
			{
				SchedItem s(GetConfigureDb()->GetString("NAME"),GetConfigureDb()->GetInt("TOD"));
				Periodics.insert(Periodics.end(),s);
			};
		};
		break;
		default:
		break;
	};
};

