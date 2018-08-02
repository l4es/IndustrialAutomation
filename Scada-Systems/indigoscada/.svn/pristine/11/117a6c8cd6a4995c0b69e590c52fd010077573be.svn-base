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
#ifndef PRINTREP_HPP
#define PRINTREP_HPP
//
// This is the print report application 
// This application walks the report and prints all files in the report
// plus a header and a page number
//
#include "realtimedb.h"
#include "graph.h"

class QSEXPORT PrintReport : public QObject
{
	Q_OBJECT
	QColorGroup normal; // the colour group for printing
	QString Name; // report name
	QString PrinterName; // the name of the printer
	QPrinter printer; // the printer object
	QMimeSourceFactory mimeSourceFactory; // factory
	//
	int nState; // state machine
	int PageNo; // page number
	bool PrintToFile; // do we print to file (1st or 2nd pass)
	int NumberPages;
	//
	enum
	{
		STATE_START = 0, // states
		STATE_INDEX,
		STATE_STATS,
		STATE_TABLES,
		STATE_DONE
	};
	public:
	//
	PrintReport(const QString &name, const QString &printername = QString::null); // the report to print
	~PrintReport();
	void Print(const QString &);
	void Setup();
	QString ReadPage(QTextStream &is);
	void PrintGraph( const QString &filename);
	//
	public slots:
	void Process();
	QPrinter &GetPrinter() { return printer;};
	signals:
	void Done();
};
#endif

