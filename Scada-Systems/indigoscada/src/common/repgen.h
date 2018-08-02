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
*Header For: report generator
*Purpose: the report generator is a basic results extraction and display application
*it generates a tabular report (one per sample point), one graph per point and a statistics summary
*on a per sample point per tag basis 
*/

#ifndef include_repgen_hpp 
#define include_repgen_hpp 
// get the 
#include "realtimedb.h"
#include "graph.h"

#define REP_GRAPH_SIZE 500
class QSEXPORT ReportGenerator : public QObject
{
	Q_OBJECT
	//  
	enum {tItem = 1, tSample, tResults};
	//
	QString Name; // report name
	QString Title; // the title
	//
	QDateTime From; // from time frame
	QDateTime To;   // to time frame
	//
	QString FromStr; // in datetime
	QString ToStr;
	//
	int   nGraphs;      // number of graphs per page
	QStringList Points; // sample points
	//
	QString Dir; // the report directory
	//
	QString Front; // the front sheet template file
	QString Back; // the back sheet template file
	QString Table; // the table template file
	QString GraphStats; // the graph and stats template table file
	unsigned AlarmFilter; // 0 = All, 1 = Alarms and Warnings, 2 = Alarms only
	//
	unsigned nDone; // number of tables read
	QString BatchName; // the name of the batch if invoked as a bathc report
	//
	public:
	ReportGenerator(const QString & name = "",const QString & from = "",const QString & to = "",const QString & batchname = "");
	~ReportGenerator();
	void WriteTableHeader(QTextStream &os, QStringList &heading, const QString &units, const QString &name, const QString &comment);
	//
	public slots:
	//
	// Database response functions
	//
	void ConfigQueryResponse (QObject *,const QString &, int, QObject*);  // handles configuration responses
	void ResultsQueryResponse (QObject *,const QString &, int, QObject*); // results responses
};
#endif

