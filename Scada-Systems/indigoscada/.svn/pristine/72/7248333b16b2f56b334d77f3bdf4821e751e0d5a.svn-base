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
* Header For: log report generator
* Purpose: the log file report generator 
* it generates a tabular report 
*/
#ifndef include_loggen_hpp 
#define include_loggen_hpp 
// get the 
#include <qt.h>
#include "realtimedb.h"
#include "graph.h"

class QSEXPORT LogReportGenerator : public QWidget
{
	Q_OBJECT
	//  
	enum {tItem = 1, tSample, tResults};
	//
	QString Name; // report name
	//
	QDateTime From; // from time frame
	QDateTime To;   // to time frame
	//
	//
	QString Dir; // the report directory
	//
	public:
	LogReportGenerator(const QString &name = "", const QString &from = "", const QString &to = "", const QString &source = "");
	~LogReportGenerator();
	//
	public slots:
	//
	// Database response functions
	void ResultsQueryResponse (QObject *,const QString &, int, QObject*); // results responses
};
#endif

