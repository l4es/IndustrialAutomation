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
*Header For: Alarm Log report generator implimentation
*
*Purpose:
*/

#include "loggen.h"
#include "common.h"
#include "smplstat.h"
#include "IndentedTrace.h"
#ifdef UNIX
#include <unistd.h>
#endif
#include <fcntl.h>
/*
*Function:LogReportGenerator
*Inputs:report name
*Outputs:none
*Returns:none
*/
LogReportGenerator::LogReportGenerator(const QString &name, const QString &from, const QString &to, const QString &source)  : Name(name)
{
	IT_IT("LogReportGenerator::LogReportGenerator");
	
	//
	From = IsoDateQDateTime(from);
	To = IsoDateQDateTime(to);
	//
	connect (GetResultDb (),
	SIGNAL (TransactionDone (QObject *, const QString &, int, QObject*)), this,
	SLOT (ResultsQueryResponse (QObject *, const QString &, int, QObject*)));	// connect to the database
	//
	// start by getting the report from the database
	//
	QString cmd;
	if(source.isEmpty())
	{
		cmd = "select * from " + Name + " where TIMEDATE between " + QDATE_TIME_ISO_DATE(From) + " and " + QDATE_TIME_ISO_DATE(To) + ";";
		//QString cmd = "select * from " + Name + " where TIMEDATE > " + QDATE_TIME_ISO_DATE(From) + " and TIMEDATE < " + QDATE_TIME_ISO_DATE(To) + ";";
	}
	else
	{
		cmd = "select * from " + Name + " where TIMEDATE > " + QDATE_TIME_ISO_DATE(From) + " and TIMEDATE < " + QDATE_TIME_ISO_DATE(To) + " and SOURCE='"+ source +"';";
	}

	GetResultDb()->DoExec(this,cmd,tResults); // kick it off
	//
};
/*
*Function:~LogReportGenerator
*Inputs:none
*Outputs:none
*Returns:none
*/
LogReportGenerator::~LogReportGenerator()
{
	IT_IT("LogReportGenerator::~LogReportGenerator");
}
/*
*Function::ResultsQueryResponse
*Inputs:client, command, id
*Outputs:none
*Returns:none
*/
void LogReportGenerator::ResultsQueryResponse(QObject *p,const QString &, int id, QObject* caller)
{
	if(p != this) return;

	IT_IT("LogReportGenerator::ResultsQueryResponse");

	switch(id)
	{
		case tResults: // results table
		{
			//
			// We now have the data
			//
			// open the output file
			// write the header
			QString ofile( QSREPORT_DIR + "/" + Name + ".html");
			QFile of(ofile);
			if(of.open(IO_WriteOnly))
			{
				QTextStream os(&of);
				//
				//
				// set up the header
				// 
				os << "<HTML> <HEAD> <TITLE> " << tr("Results Table For ") << Name << "</TITLE>\n";
				os << "</HEAD>\n";
				os << "<BODY BGCOLOR=\"#FFFFFF\" TEXT=\"#000000\" LINK=\"#0000FF\" VLINK=\"#840084\" ALINK=\"#0000FF\" >\n";
				// 
				os << "<P>";
				os << "<B>" << tr("Log ") << Name << "</B><br>" << endl;
				os << "</P>" << endl;
				os << "<P>" << tr("Report From ") << From.toString() << tr(" To ") << To.toString() << "</P>" << endl;
				//
				// Table header
				// 
				os << "<TABLE BORDER=\"1\" CLASS=\"CALSTABLE\">\n<TR>"; 
				os << "<TH ALIGN=\"LEFT\" VALIGN=\"TOP\">"<< tr("Date/Time") << "</TH>";
				os << "<TH ALIGN=\"LEFT\" VALIGN=\"TOP\">"<< tr("Source") << "</TH>";
				os << "<TH ALIGN=\"LEFT\" VALIGN=\"TOP\">"<< tr("Message") << "</TH>";
				os << "</TR>\n";
				//
				// write the data out as table cells
				// 
				for(unsigned i = 0; i < GetResultDb()->GetNumberResults(); i++, GetResultDb()->FetchNext())
				{
					// all event logs have the same format
					os << "<TR>";
					os << "<TD>" << GetResultDb()->GetIsoDateString("TIMEDATE") << "</TD>"; 
					os << "<TD>" << UndoEscapeSQLText(GetResultDb()->GetString("SOURCE")) << "</TD>"; 
					os << "<TD>" << UndoEscapeSQLText(GetResultDb()->GetString("QMESSAGE")) << "</TD>"; 
					os << "</TR>" << endl;
					//
					//qApp->processEvents(); // process X/Win events
					//
				};
				os << "</TABLE>" << endl;
				//
				os << "</HTML>" << endl;
				//
			};

			QString ini_file = GetScadaHomeDirectory() + "\\project\\scada.ini";
			Inifile iniFile((const char*)ini_file);

			if(iniFile.find("path","internet_browser"))
			{
				QString path;
				path = iniFile.find("path","internet_browser");

				//////////////create new process///////////////////////////////////////////////
				QString cmd = path + " " + QSREPORT_DIR + "\\" + Name +".html";
				//
				////////////create new process/////////////////////////////////////
				STARTUPINFO startup_information;

				ZeroMemory( &startup_information, sizeof( startup_information ) );
				startup_information.cb = sizeof( startup_information );

				PROCESS_INFORMATION process_information;
				ZeroMemory( &process_information, sizeof( process_information ) );

				if ( CreateProcess( NULL,
									  (char*)((const char*)cmd),
									  0,
									  0,
									  1,
									  NORMAL_PRIORITY_CLASS,
									  0,
									  0,
									 &startup_information,
									 &process_information ) != FALSE );

	//			QString home = QSREPORT_DIR + "\\" + Name +".html";
	//			HelpWindow* helpw = new HelpWindow(home, QSREPORT_DIR, 0, QObject::tr("help viewer"));
	//			helpw->show();
	//			QObject::connect( helpw, SIGNAL(lastWindowClosed()), helpw, SLOT(quit()) );
			}
		};
		break;
		default:
		break;
	};
	
};
