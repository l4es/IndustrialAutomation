/**********************************************************************
--- Qt Architect generated file ---
File: CheckTagEntries.cpp
Last generated: Thu Nov 30 08:47:27 2000
*********************************************************************/
#include "CheckTagEntries.h"
#define Inherited CheckTagEntriesData

#pragma warning (disable : 4060) 

CheckTagEntries::CheckTagEntries
(
QWidget* parent,
const QString &name, const QString &unit_type
)
:
Inherited( parent, name ),Name(name)
{
	setCaption(tr("Checking Tag Entries and Database"));
	//
	connect (GetConfigureDb (),
	SIGNAL (TransactionDone (QObject *, const QString &, int, QObject*)), this,
	SLOT (QueryResponse (QObject *, const QString &, int, QObject*)));	// connect to the database
	connect (GetResultDb (),
	SIGNAL (TransactionDone (QObject *, const QString &, int, QObject*)), this,
	SLOT (ResultQueryResponse (QObject *, const QString &, int, QObject*)));	// connect to the database
	pDriver = FindDriver(unit_type); // we have the driver
	if(!pDriver) 
	{
		reject();
	}
	else
	{
		QString cmd = "select NAME,TAG from TAGS where NAME in (select NAME from SAMPLE where UNIT ='" + Name + "') order by NAME;";
	};
}
CheckTagEntries::~CheckTagEntries()
{
}

void CheckTagEntries::QueryResponse (QObject *p, const QString &c, int State, QObject*caller) // notify transaction completerequestrt needs the name record addedarttod text,startdate text,endtod text,enddate text);
{
	if(this == p)
	{
		switch (State)
		{
		}
	}
}

void CheckTagEntries::ResultQueryResponse (QObject *p, const QString &c, int State, QObject*caller) // notify transaction completerequestrt needs the name record addedarttod text,startdate text,endtod text,enddate text);
{
	if(this == p)
	{
		switch (State)
		{
		}
	}
}

