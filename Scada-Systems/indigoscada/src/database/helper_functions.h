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

#ifndef helper_function_h
#define helper_function_h

#include "dispatch.h"
#include "dbase.h"
#include "realtimedb.h"
#include "historicdb.h"
#include "general_defines.h"
//
//
//
// These are helper functions to move the dependencies into a single file 
//
//
#define DATETIME_NOW  GetDateTimeNowInMilliSeconds()
#define DATE_TIME_FORMAT GetDateTimeFormat()
// time field SQL name
#define DATE_TIME_COL_TYPE GetDateTimeColType()
#define DATETIME_EPOCH GetDateTimeEpoch()
//
//
QSEXPORT const char * GetDateTimeFormat();
QSEXPORT QString GetDateTimeColType();
QSEXPORT QString GetDateTimeEpoch();
QSEXPORT QString GetDateTimeNowInSeconds();
QSEXPORT QString GetDateTimeNowInMilliSeconds();
//
//
struct UserDetails
{	
	QString Name;		// user name	
	QString Language;	// what language we are using	
	unsigned int privs;// permitted priviledges	
	bool     AmSystem;		// track if system manager	
	unsigned AuditLevel;    // what audit level are we at - 0 = none, 1 = Changes (New, Apply, Delete, Rename), 2 = Changes need comment	
	enum	
	{		
		AuditLevelNone = 0,		
		AuditChanges,		
		AuditChangesComment	
	};	
	UserDetails() :Name(QObject::tr(NONE_STR)),Language("English"),privs(0),AmSystem(0),AuditLevel(0)	
	{	
	}
};

QSEXPORT UserDetails & GetUserDetails ();	// return the user setup

//
// Some helper functions and macros
// 
inline void  QSLogAlarm(const QString &src, const QString &msg)
{	
	QString cmd = "insert into ALARMS values(" +  DATETIME_NOW  + ",'" + EscapeSQLText(src).left(MAX_LENGHT_OF_STRING) +"','"+ EscapeSQLText(msg).left(MAX_LENGHT_OF_STRING) +"');";	
	GetResultDb()->DoExec(0,cmd,0);
	if(GetHistoricResultDb() != NULL)
	{
		GetHistoricResultDb()->DoExec(0,cmd,0);
	}

	QString alarm =  src + " -> " + msg;
	//broadcast
	GetDispatcher()->DoExec(NotificationEvent::ALARM_NOTIFY, (const char*) alarm); 	// say we have an alarm
};

inline void  QSLogEvent(const QString &src, const QString &msg)
{
	QString cmd = "insert into EVENTS values("+ DATETIME_NOW + ",'" + EscapeSQLText(src).left(MAX_LENGHT_OF_STRING)+"','"+ EscapeSQLText(msg).left(MAX_LENGHT_OF_STRING)+"');";	
	GetResultDb()->DoExec(0,cmd,0);

	if(GetHistoricResultDb() != NULL)
	{
		GetHistoricResultDb()->DoExec(0,cmd,0);
	}

	QString event =  src + " -> " + msg;	
	//broadcast
	GetDispatcher()->DoExec(NotificationEvent::EVENT_NOTIFY,  (const char*) event);

};

inline void  QSEvent(const QString &src, const QString &msg)
{
	//No logging to database...
	QString event =  src + " -> " + msg;	
	//broadcast
	GetDispatcher()->DoExec(NotificationEvent::EVENT_NOTIFY,  (const char*) event); 
};

inline void  QSAudit(const QString &src, const QString &msg) // audit events - we keep quiet about these
{	
	QString cmd = "insert into AUDIT values("+ DATETIME_NOW+ ",'" + EscapeSQLText(src).left(MAX_LENGHT_OF_STRING)+"','"+ EscapeSQLText(msg).left(MAX_LENGHT_OF_STRING)+"');";	
	GetResultDb()->DoExec(0,cmd,0);
	
	if(GetHistoricResultDb() != NULL)
	{
		GetHistoricResultDb()->DoExec(0,cmd,0);
	}
};

void  QSEXPORT QSAuditTrail(QWidget *parent,const QString &src, const QString &msg);

#define DOAUDIT(m) QSAuditTrail(this,caption(),m)

#define ACK_ALL_TOKEN "_ALL_"

inline void AckAllAlarms(const QString &comment = "")
{	
	QString cmd = "update CVAL_DB set ACKFLAG=0,ACKTIME=" +  DATETIME_NOW + " where ACKFLAG=1;";	
	GetCurrentDb()->DoExec(0,cmd,0);	
	//	
	QString to_tr = QObject::tr(" has acknowledged all alarms");
	QSLogAlarm("Client",  GetUserDetails().Name + to_tr + ":" + comment);	
	//	
	cmd = "update ALM_GRP_STATE set ACKSTATE=0;";	
	GetCurrentDb()->DoExec(0,cmd,0); // do the transaction	
	cmd = "update ALM_GRP set ACKSTATE=0;";	
	GetCurrentDb()->DoExec(0,cmd,0); // do the transaction

	//broadcast
	GetDispatcher()->DoExec(NotificationEvent::ALARMGROUP_NOTIFY);	
	GetDispatcher()->DoExec(NotificationEvent::ACK_NOTIFY, ACK_ALL_TOKEN);	
	//
};

inline void AckAlarm(const QString &name, const QString &comment="")
{	
	QString cmd = "update CVAL_DB set ACKFLAG=0,ACKTIME=" +  DATETIME_NOW + " where ACKFLAG=1 and NAME='"+name+"';";
	QString to_tr = QObject::tr(" has acknowledged alarm on ");
	QSLogAlarm("Client",  GetUserDetails().Name + to_tr + name + ": " + comment);	
	GetCurrentDb()->DoExec(0,cmd,0);	
	cmd = "update ALM_GRP_STATE set ACKSTATE=0 where SPNAME='"+name+"';" ; // update all members of the alarm groups	
	GetCurrentDb()->DoExec(0,cmd,0); // do the transaction
	//
	//	//// recalculate the ackstate of the group
	//
	//		
	// following two lines tolto commento by APA!
	cmd = "update ALM_GRP set ACKSTATE=0 where NAME in (select NAME from ALM_GRP_STATE where SPNAME='" + name + "');";	
	GetCurrentDb()->DoExec(0,cmd,0); // do the transaction
	//
	//
	//broadcast
	GetDispatcher()->DoExec(NotificationEvent::ACK_NOTIFY, (const char*)name);
	GetDispatcher()->DoExec(NotificationEvent::ALARMGROUP_NOTIFY);

};

#endif