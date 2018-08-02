/**********************************************************************
--- Qt Architect generated file ---
File: CheckTagEntries.h
Last generated: Thu Nov 30 08:47:27 2000
*********************************************************************/
#ifndef CheckTagEntries_included
#define CheckTagEntries_included
#include "CheckTagEntriesData.h"
#include "dbase.h"
#include "realtimedb.h"
#include "driver.h"
#include "ReceipeCfg.h"
#include "common.h"
class Driver;
class CheckTagEntries : public CheckTagEntriesData
{
	Q_OBJECT
	QString Name; // unit name
	Driver  *pDriver;	
	public:
	CheckTagEntries(
	QWidget *parent,
	const QString &name,const QString &unit_type );
	virtual ~CheckTagEntries();
	void QueryResponse (QObject *p, const QString &c, int State, QObject*); 
	void ResultQueryResponse (QObject *p, const QString &c, int State, QObject*); 
};
#endif // CheckTagEntries_included

