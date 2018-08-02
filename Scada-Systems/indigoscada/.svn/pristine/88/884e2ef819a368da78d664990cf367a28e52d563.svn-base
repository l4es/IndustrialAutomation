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
*
* Purpose: Print A Report 
* This does something like html2ps does but with a lot fewer features
* We print index.html then the graphs and stats summaries then the 
* tables. set PRINT_TO_FILE for writing everything to file (debugging)
* 
* 
*/
#include "printrep.h"
#include "inifile.h"
#include "IndentedTrace.h"
/*
*Function:PrintReport
*Inputs:none
*Outputs:none
*Returns:none
*/
PrintReport::PrintReport(const QString &name, const QString &printerName) : 
Name(name),PrinterName(printerName), nState(STATE_START), PageNo(1),PrintToFile(1), // the report to print
NumberPages(1)
{
	IT_IT("PrintReport::PrintReport");
};
/*
*Function: setup
*sets up everything - allows for other constructors
*Inputs:none
*Outputs:none
*Returns:none
*/
void PrintReport::Setup()
{
	IT_IT("PrintReport::Setup");
	
	// form the printing object
	// form the timer object - we drive the printing process as a state machine
	QTimer  *pTimer = new QTimer(this);
	pTimer->connect(pTimer,SIGNAL(timeout()),this,SLOT(Process()));
	pTimer->start(100); 
	//
	QMimeSourceFactory::setDefaultFactory(&mimeSourceFactory); // initalise it
	mimeSourceFactory.setFilePath(  QSREPORT_DIR + "/" + Name);
	//
	normal.setColor( QColorGroup::Foreground, QColor( QRgb( 0 ) ) );
	normal.setColor( QColorGroup::Button, QColor( QRgb( 10526884 ) ) );
	normal.setColor( QColorGroup::Light, QColor( QRgb( 15790326 ) ) );
	normal.setColor( QColorGroup::Midlight, QColor( QRgb( 13158605 ) ) );
	normal.setColor( QColorGroup::Dark, QColor( QRgb( 5263442 ) ) );
	normal.setColor( QColorGroup::Mid, QColor( QRgb( 6974061 ) ) );
	normal.setColor( QColorGroup::Text, QColor( QRgb( 0 ) ) );
	normal.setColor( QColorGroup::BrightText, QColor( QRgb( 16777215 ) ) );
	normal.setColor( QColorGroup::ButtonText, QColor( QRgb( 0 ) ) );
	normal.setColor( QColorGroup::Base, QColor( QRgb( 16777215 ) ) );
	normal.setColor( QColorGroup::Background, QColor( QRgb( 10526884 ) ) );
	normal.setColor( QColorGroup::Shadow, QColor( QRgb( 0 ) ) );
	normal.setColor( QColorGroup::Highlight, QColor( QRgb( 128 ) ) );
	normal.setColor( QColorGroup::HighlightedText, QColor( QRgb( 16777215 ) ) );
};
/*
*Function:~PrintReport
*Inputs:none
*Outputs:none
*Returns:none
*/
PrintReport::~PrintReport()
{
	IT_IT("PrintReport::~PrintReport");
};
#define PRINT_MARGIN 10
#define NEW_PAGE "<!--NewPage-->"
/*
*Function: ReadPage
*Read until we get a new page marker
*Inputs:TextStream
*Outputs:none
*Returns:page as text - returns empty string on eol
*/
QString PrintReport::ReadPage(QTextStream &is)
{
	IT_IT("PrintReport::ReadPage");
	
	QString text;
	do
	{
		is >> ws; // strip leading white space
		QString line = is.readLine();	
		if(line != QString::null)
		{
			if(line == NEW_PAGE)
			{
				break; // end of page
			}
			else if(line == "<!GRAPH>")
			{
				is >> ws; // skip the next line - this is the image map - that is the graph - we have problems rendering PNG files
				is.readLine();
			}
			else
			{
				text += line + "\n";
			};
		}
		else
		{
			break;
		};
	} while(!is.atEnd());
	//
	return text;
};
/*
*Function:PrintGraph
*Inputs:name of graph file to load
*Outputs:none
*Returns:none
*/
void PrintReport::PrintGraph( const QString &filename)
{
	IT_IT("PrintReport::PrintGraph");
	
	//cerr << "Printing Graph" << endl;
	QFile f( QSREPORT_DIR + "/" + Name + "/" + filename + ".grf");
	if(f.open(IO_ReadOnly))
	{
		//
		QDataStream ts(&f);
		// we now load all the graph data
		// 
		int nf = 0;
		int logMode;
		QString Title;
		QString SPName;
		QString Comment;
		QString Units;
		Graph graph; // the graph
		// 
		ts >> nf; // number of tags
		ts >> logMode; // are we in log mode
		ts >> Title; // title of the report
		ts >> SPName; // sample point name
		ts >> Comment;
		ts >> Units; // the units
		//
		graph.SetLogMode(logMode);

		QString ini_file = GetScadaHomeDirectory() + "\\project\\scada.ini";
		Inifile iniFile((const char*)ini_file);

		QString str;
		if((str = iniFile.find("maxItems","report_generation")) != NULL)
		{
			int max_i = atoi(str);
			graph.SetMaxPoints(max_i); 
		}

		//
		for(int i = 0;i < nf; i++)
		{
			QString tag;
			QDateTime d;
			double v;
			int n;
			ts >>  tag >> n; // tag name and number of points
			for(int j = 0; j < n; j++)
			{
				ts >> d >> v; // get the value pair
				graph.Update(tag, d, v); // add the point to the graph
			};
		};
		//
		// we read the text until we get a new page marker
		//
		#ifdef PRINT_TO_FILE
		printer.setOutputFileName("x" + QString::number(PageNo) + ".ps"); // for debugging
		#endif
		printer.setOrientation(QPrinter::Landscape);
		//
		QPainter p( &printer ); // painter's in
		//
		QPaintDeviceMetrics metrics(p.device());
		//
		int dpix = metrics.logicalDpiX() / 2; // half inch border
		int dpiy = metrics.logicalDpiY() / 2;
		//
		QRect body(dpix, dpiy,	metrics.width()  -  dpix*4,	metrics.height() -  dpiy*2);
		//
		QFont font("times", 8); 
		p.setFont(font);
		//
		graph.ScaleGraph(); // get the limits and steps
		graph.Plot(p,body,Qt::white); // draw it
		//
		p.setFont(font);
		//
		QString s =  tr("Page") + " " +  QString::number(PageNo) + tr(" of ") + QString::number(NumberPages);
		//
		int dy = p.fontMetrics().height() + 2;
		int y = dy;
		p.drawText( body.left() ,y, Title);
		y += dy;
		p.drawText( body.left() ,y, Comment);
		y += dy;
		QString s1 = SPName + " (" + Units + ")";
		p.drawText( body.left(),y,s1);
		//
		p.drawText( body.right() - p.fontMetrics().width( s ),dy,s);
		//
		PageNo++;
	};
}
/*
*Function:Print()
*print the current text buffer
*Inputs:file name
*Outputs:none
*Returns:none
*/
void PrintReport::Print( const QString &filename)
{
	IT_IT("PrintReport::Print");
	
	QString Text; // where we load the text to 
	//
	QString context =  QSREPORT_DIR + "/" + Name; // the context for the document
	QFile f(context + "/" + filename);
	if(f.open(IO_ReadOnly))
	{
		//
		QTextStream ts(&f);
		//
		// we read the text until we get a new page marker
		//
		#ifdef PRINT_TO_FILE
		printer.setOutputFileName("x" + QString::number(PageNo) + ".ps");
		#endif
		printer.setOrientation(QPrinter::Portrait);
		QPainter p( &printer ); // painter's in
		//
		QPaintDeviceMetrics metrics(p.device());
		int dpix = metrics.logicalDpiX() / 2; // the margins
		int dpiy = metrics.logicalDpiY() / 2;
		//
		QRect body(dpix, dpiy,	metrics.width()  -  dpix*2,	metrics.height() -  dpiy*2);
		//
		QFont font("times", 8); 
		//
		while(!ts.atEnd())
		{
			Text = ReadPage(ts);  // get the text
			//cerr << "Got Page Length:" << Text.length() << endl;
			//
			if(Text != QString::null)
			{			
				//	
				QSimpleRichText richText( Text, font, context,0,&mimeSourceFactory);
				richText.setWidth( &p, body.width() );
				//
				QRect view( body );
				richText.draw( &p, body.left(), body.top(), view, normal );
				//
				p.setFont( font );
				//
				QString s =  tr("Page") + " " +  QString::number(PageNo) + tr(" of ") + QString::number(NumberPages);
				//
				p.drawText( view.right() - p.fontMetrics().width( s ),
				view.bottom() + p.fontMetrics().ascent() + 5, s ); // add the page number
				//
				PageNo++;
				//
				if(!ts.atEnd())	printer.newPage(); // ask for new page
			};
		};
	};
}
/*
*Function: Process
*printing state machine. We do it this way to let the system be responsive
*regardles of the operating system
*Inputs:none
*Outputs:none
*Returns:none
*/
void PrintReport::Process()
{
	IT_IT("PrintReport::Process");
	//
	switch(nState)
	{
		// load the 
		//
		case STATE_START:
		{
			// set up the printer object
			//
			printer.setPrinterName(PrinterName); // say which queue we are using
			printer.setOutputToFile(PrintToFile);
			printer.setPageSize(QPrinter::A4);
			//
			if(PrintToFile)
			{
				printer.setOutputFileName(Name + ".ps"); // temp file for 1st pass
			}
			else
			{
				printer.setOutputFileName(QString::null); //no file for 2nd pass
			};
			nState = STATE_INDEX;
		};
		break;
		case STATE_INDEX:
		{
			// 
			// print it
			Print("index.html"); // print the index
			nState = STATE_STATS;
		};
		break;
		case STATE_STATS:
		{
			//
			// get the list of stats pages
			// those ending in _stats.html
			//
			QDir d(QSREPORT_DIR + "/" + Name ,"*.txt");
			//
			for(unsigned i = 0; i < d.count();i++)
			{
				QFileInfo fi(d[i]); // we want to parse this up
				QString sf = fi.baseName() + "_stats.html";
				Print(sf); // load and print the stats page
				//
				// load and print the graph objects
				//cerr << "Printing Graph " << (const char *)fi.baseName() << endl;
				PrintGraph(fi.baseName());				
				//
				QString tf = fi.baseName()+"_tab.html"; // load the table files
				Print(tf); // load and print 
			};
			//
			nState = STATE_DONE;
		};
		break;
		case STATE_DONE:
		{
			//
			// finish with a back sheet
			//
			//
			NumberPages = PageNo - 1; // number of pages
			if(!PrintToFile)
			{
				emit Done();
			}
			else
			{
				PrintToFile = false;
				nState = STATE_START;
				PageNo = 1;
				#ifdef PRINT_TO_FILE
				emit Done();
				#endif
			};
		};
		break;
		default:
		{
			// problem
			emit Done();
		};
		break;
		//
	};
};

