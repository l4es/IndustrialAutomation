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
*Header For: Report genrator implimentation
*Purpose:
*/

#include "repgen.h"
#include "common.h"
#include "smplstat.h"
#include "inifile.h"
#include "IndentedTrace.h"
#ifdef UNIX
#include <unistd.h>
#endif

#include <fcntl.h>
// how to indicate a new page
#define NEW_PAGE "<!--NewPage-->"
// number of lines per page
// 
#define N_LINES_PAGE 30
//
// number tags after which we insert a page break in the stats graph page
#define N_TAGS_FOR_STATS_PAGE_BREAK 3
//

struct Stats : public SampleStatistic
{
	int nAlarms; // number of alarms
	int nWarnings; // number of warnings
	Stats() : nAlarms(0),nWarnings(0) {};
};
//struct GraphData
//{
//	#define N_GRAPH_POINTS 500
//	SampleStatistic Data[N_GRAPH_POINTS]; // we reduce the data displayed to something useful
//};
#define START_TOKEN "<!**START**>"
/*
*Function:ReportGenerator
*Inputs:report name
*Outputs:none
*Returns:none
*/
ReportGenerator::ReportGenerator(const QString & name,const QString & from,const QString & to,const QString & batchname)
: Name(name),From(IsoDateQDateTime(from)),To(IsoDateQDateTime(to)),BatchName(batchname),AlarmFilter(0),nDone(0)
{
	
	IT_IT("ReportGenerator::ReportGenerator");
	//
	connect (GetConfigureDb (),
	SIGNAL (TransactionDone (QObject *, const QString &, int, QObject*)), this,
	SLOT (ConfigQueryResponse (QObject *, const QString &, int, QObject*)));	// connect to the database
	//
	connect (GetResultDb (),
	SIGNAL (TransactionDone (QObject *, const QString &, int, QObject*)), this,
	SLOT (ResultsQueryResponse (QObject *, const QString &, int, QObject*)));	// connect to the database
	// start by getting the report from the database
	// 
	QString cmd = "select * from REPORTS where NAME='"+Name+"';";
	GetConfigureDb()->DoExec(this,cmd,tItem); // kick it off
	//
	IT_COMMENT1("Report name: %s", (const char *)Name);
};
/*
*Function:~ReportGenerator
*Inputs:none
*Outputs:none
*Returns:none
*/
ReportGenerator::~ReportGenerator()
{
	IT_IT("ReportGenerator::~ReportGenerator");
};
/*
*Function:WriteTableHeader
*Inputs:output stream, headings list, units
*Outputs:none
*Returns:none
*/
void ReportGenerator::WriteTableHeader(QTextStream &os, QStringList &heading, const QString &Units, 
const QString &sName, const QString &Comment)
{
	IT_IT("ReportGenerator::WriteTableHeader");
	
	//
	//
	os << "<H3>";
	if(sName != QString::null)
	{
		os << tr("Results Table For ") << sName << "<br>" << endl;
	};
	if(Comment != QString::null)
	{	
		os <<  Comment << "<br>" << endl;
	};
	//
	// qualify with the alarm level filter
	// 
	switch(AlarmFilter)
	{
		case 0:
		os <<  tr("All Results") << endl;
		break;
		//
		case 1:
		os << tr("Alarms And Warnings Only In Tables") << endl;
		break;
		//
		case 2:
		os <<  tr("Alarms Only In Tables") << endl;
		break;
		//
		default:
		break;
	};
	//
	os << "</H3><br>" << endl;
	//
	// Table header
	// 
	os << "<TABLE BORDER=\"1\" CLASS=\"CALSTABLE\">\n<TR>"; 
	os << "<TH ALIGN=\"LEFT\" VALIGN=\"TOP\">"<< tr("Date") << "</TH>";
	os << "<TH ALIGN=\"LEFT\" VALIGN=\"TOP\">"<< tr("Time") << "</TH>";
	os << "<TH ALIGN=\"LEFT\" VALIGN=\"TOP\">"<< tr("State") << "</TH>";
	//
	for(unsigned k = 0; k < heading.count(); k++)
	{
		os << "<TH ALIGN=\"LEFT\" VALIGN=\"TOP\">"<< heading[k] << " " << Units <<"</TH>";
	};
	//            
	os << "</TR>\n";
};
/*
*Function:ConfigQueryResponse 
*Inputs:client, command, transaction id
*Outputs:none
*Returns:none
*/
void ReportGenerator::ConfigQueryResponse (QObject *p,const QString &, int id, QObject* caller)  // handles configuration responses
{
	if(p != this) return;

	IT_IT("ReportGenerator::ConfigQueryResponse");

	switch(id)
	{
		case tItem:
		{
			if(GetConfigureDb()->GetNumberResults() > 0)
			{
				//
				// we have the report
				//
				Title = UndoEscapeSQLText(UndoEscapeSQLText(GetConfigureDb()->GetString("COMMENT")));
				nGraphs = GetConfigureDb()->GetInt("GRAPHPAGE");
				QString s = GetConfigureDb()->GetString("SAMPLES");
				AlarmFilter = GetConfigureDb()->GetInt("ALARMFILTER"); // get the alarm filter level
				//
				QTextIStream is (&s);
				int n = 0;		// how many names
				is >> n;
				if (n > 0)
				{
					for (int i = 0; i < n; i++)
					{
						QString a;
						is >> a;
						Points << a.stripWhiteSpace();
					};
				};
				//
				// build the date time strings
				// this depends on the modes
				//
				if(BatchName.isEmpty())
				{
					From =  ReportFromTime(
					GetConfigureDb()->GetInt("FROMMODE"), 
					GetConfigureDb()->GetString("FROMDATE"), 
					GetConfigureDb()->GetString("FROMTIME"));
					To = ReportToTime(
					GetConfigureDb()->GetInt("TOMODE"), 
					GetConfigureDb()->GetString("TODATE"), 
					GetConfigureDb()->GetString("TOTIME"));
				}
				//
				// Get the front and back sheet and templates
				// 
				Front = GetConfigureDb()->GetString("FRONT");
				Back = GetConfigureDb()->GetString("BACK");
				Table = GetConfigureDb()->GetString("TABLESHEET");
				GraphStats = GetConfigureDb()->GetString("GRAPHSTATS");
				//
				//
				if((From >= To) || (Points.count() < 1))
				{
					QMessageBox::information(NULL,QObject::tr("Time Frame Error"),QObject::tr("From Time is After or Equal the To Time"),QObject::tr("Ok"));
				}
				else
				{
					//
					// Now build the report SQL transactions
					// we want to generate 
					// Stats min, max, ave - could do with SD - does do it ?
					// 
					
					FromStr = QDATE_TIME_ISO_DATE(From);
					ToStr = QDATE_TIME_ISO_DATE(To);
					
					//
					#ifdef UNIX
					Dir = QSREPORT_DIR + "/" + Name;
					#else if WIN32
					Dir = QSREPORT_DIR + "\\" + Name;
					#endif


					QDir dir;
					dir.mkdir(Dir); // create the report directory
					//
					//  We can construct reports with PSQL easily but that lacks portablity
					for(unsigned i = 0; i < Points.count(); i++)
					{
						QString cmd = "select * from "+ Points[i] + " where TIMEDATE between " + FromStr + " and " + ToStr + ";";
						GetResultDb()->DoExec(this,cmd,tResults,Points[i]); //ask for the table

						IT_COMMENT1("Query: %s", (const char *)cmd);
					}
				} 
			}
		}
		break;
		case tSample:
		{
			//    
			// get each of the csv files 
			// convert to HTML
			// plot graph object
			// accumulate stats
			//
			for(unsigned iSp = 0; iSp < GetConfigureDb()->GetNumberResults(); iSp++,GetConfigureDb()->FetchNext())
			{
				QString file = Dir + "/" + GetConfigureDb()->GetString("NAME") + ".txt";
				QFile f(file);
				if(f.exists())
				{
					if(f.open(IO_ReadOnly))
					{
						QTextStream is(&f);
						int nrecs, nf;
						//
						QString ofile(Dir + "/" + GetConfigureDb()->GetString("NAME") + "_tab.html");
						QFile of(ofile);
						if(of.open(IO_WriteOnly))
						{
							QTextStream os(&of);
							// put out the HTML header bits
							is >> nrecs >> nf; //number of records, number of fields
							is >> ws; 
							nf -= 3; // get the number of data fields (ex date, time and state)
							//
							QString heading = is.readLine(); // get the heading tags
							int nline = 0; // count the numebr of lines per page
							int npage = 1; // count the number of pages
							//
							//
							// set up the header
							QString TableBuf; // buffer for table template
							//
							os << "<HTML> <HEAD> <TITLE> " << tr("Results Table For ") << GetConfigureDb()->GetString("NAME") << "</TITLE>\n";
							os << "</HEAD>\n";
							os << "<BODY BGCOLOR=\"#FFFFFF\" TEXT=\"#000000\" LINK=\"#0000FF\" VLINK=\"#840084\" ALINK=\"#0000FF\" >\n";
							//
							if(Table != tr(NONE_STR))
							{
								// 
								// open the table template file
								// write out until we get to START_TOKEN
								// 
								QFile tf(QSTEMPLATE_DIR + "/" + Table + ".html");
								if(tf.open(IO_ReadOnly))
								{
									char *b = new char[tf.size() + 1]; // these should be only a few k
									tf.readBlock(b,tf.size());
									b[tf.size()] = 0; // null terminate
									//
									TableBuf = QString(b); // turn into a QString
									//
									// look for the START_TOKEN
									int i = TableBuf.find(START_TOKEN);
									if(i > 0)
									{
										const char *p = (const char *)TableBuf;
										os.writeRawBytes(p,i); // write the leading part
									};
								};
							};
							//
							//
							// ID the samplepoint, report and time frame
							//    
							os << "<H3>" << tr("Results Table For ") << GetConfigureDb()->GetString("NAME") << "<br>" << endl;
							os <<  UndoEscapeSQLText(UndoEscapeSQLText(GetConfigureDb()->GetString("COMMENT"))) << "<br>" << endl;
							//
							if(!BatchName.isEmpty())
							{
								os << "<B>" << tr("Batch ID:") << BatchName << "</B><br>" << endl;
							};
							//
							os << tr("Report From ") << From.toString() << tr(" To ") << To.toString() << "</H3>" << endl;
							//
							// Table header
							// 
							//
							QTextIStream hs(&heading);  
							//
							QString hd; 
							hs >> hd; // miss the date time
							hs >> hd; // miss the state
							hs >> hd;
							//
							QStringList tags; // list of tag names
							QString Units =  GetConfigureDb()->GetString("UNITS");
							//          
							for(int k = 0; k < nf; k++)
							{
								hs >> hd;
								tags << hd;
							};
							//
							nline = 5; // first page has 5 lines less to allow for header
							WriteTableHeader(os, tags, Units,QString::null,QString::null);
							//GetConfigureDb()->GetString("NAME",iSp),
							//UndoEscapeSQLText(GetConfigureDb()->GetString("COMMENT",iSp)));  // put in the table header
							// add links for graph and stats page
							//
							//
							// ************************************
							//
							Stats* stats = new Stats[nf]; // the stats tracks
							Graph graphData; // the graph data
							//
							//
							while(!is.atEnd())
							{
								//
								QString s = is.readLine();
								QTextIStream ts(&s); // parse it all up
								//
								QString d; // date 
								QString t; // time
								int state = 0; // state
								//
								ts >> d >> t >> state; // date time and state
								//
								bool fRow = true; // flag if we write out the table record
								//
								switch(AlarmFilter)
								{
									case 0:
									break;
									case 1:
									if(state < WarningLevel) fRow = false;
									break;
									case 2:
									if(state < AlarmLevel) fRow = false;
									break;
									default:
									break;
								};
								//
								QDateTime tval = IsoDateQDateTime(d + " " + t); // the time stamp
								//
								//
								if(fRow)
								{
									os << "<TR> " ;
									os << "<TD> " << d << "</TD> <TD>" << t << "</TD><TD>" << GetAlarmStateName(state) << "</TD>";
								};
								for(int j = 0; j < nf; j++)
								{
									QString s;
									ts >> s;
									if(s != "***")
									{
										//
										// accumulate stats
										// 
										double v = s.toDouble();
										stats[j] += v;
										//
										switch(state)
										{
											case WarningLevel:
											stats[j].nWarnings++;
											break;
											case AlarmLevel:
											stats[j].nAlarms++;
											break;
											default:
											break;
										};
										//
										// plot on graph
										graphData.Update(tags[j],tval,v);
										// 
									}
									if(fRow)
									{
										os << "<TD>" << s << "</TD>";    
									};
								};
								if(fRow)
								{
									os << "</TR>" << endl; // break up for human readers of the html
									//
									// Do we start a new page ?
									//
									nline++;
								};
								//
								if(nline > N_LINES_PAGE)
								{
									os << "</TABLE>" << endl;
									os << NEW_PAGE << endl; // add a page break
									//
									WriteTableHeader(os, tags, Units,  // put in the table header
									GetConfigureDb()->GetString("NAME"),
									UndoEscapeSQLText(GetConfigureDb()->GetString("COMMENT")));  // put in the table header
									//
									nline = 0;
									npage++;
								};
								//
							};// read line loop
							//
							os << "</TABLE> " << endl;
							if(Table != tr(NONE_STR))
							{
								// finish writing out the table file
								// look for the START_TOKEN
								int i = TableBuf.find(START_TOKEN);
								if(i > 0)
								{
									const char *p = (const char *)TableBuf;
									os.writeRawBytes(p + i,TableBuf.length() - i); // write the trailing  part
								};
							};
							os << "</HTML>" << endl;
							//
							// Now generate the stats table 
							QString sfile(Dir + "/" + GetConfigureDb()->GetString("NAME") + "_stats" + ".html");
							QFile sf(sfile);
							if(sf.open(IO_WriteOnly))
							{
								//
								QString GBuffer; // the graph template file buffer
								//
								QTextStream os(&sf);
								os << "<HTML> <HEAD> <TITLE> " << tr("Graph and Statistics Table For ") 
								<< GetConfigureDb()->GetString("NAME") << "</TITLE>\n";
								os << "</HEAD>\n";
								os << "<BODY BGCOLOR=\"#FFFFFF\" TEXT=\"#000000\" LINK=\"#0000FF\" VLINK=\"#840084\" ALINK=\"#0000FF\" >\n";
								if(GraphStats != tr(NONE_STR))
								{
									//
									// open the stats / graph template 
									// write out until we get to start
									// write out until we get to START_TOKEN
									// 
									QFile tf(QSTEMPLATE_DIR + "/" + GraphStats + ".html");
									if(tf.open(IO_ReadOnly))
									{
										char *b = new char[tf.size() + 1]; // these should be only a few k
										tf.readBlock(b,tf.size());
										b[tf.size()] = 0; // null terminate
										//
										GBuffer = QString(b); // turn into a QString
										//
										// look for the START_TOKEN
										int i = GBuffer.find(START_TOKEN);
										if(i > 0)
										{
											const char *p = (const char *)GBuffer;
											os.writeRawBytes(p,i); // write the leading part
										};
									};
								};
								//
								os << "<H3>"<< tr("Graph and Statistics Table For ") 
								<< GetConfigureDb()->GetString("NAME") << " (" << Units << ")"
								<< "<br>" << endl;
								os <<  UndoEscapeSQLText(GetConfigureDb()->GetString("COMMENT")) << "<br>" << endl;
								//
								if(!BatchName.isEmpty())
								{
									os <<  tr("Batch ID:") << BatchName << "<br>" << endl;
								};
								os <<  tr("Report From ") << From.toString() << tr(" To ") << To.toString() << "<br></H3>" << endl;
								//
								// Table header
								// 
								os << "<TABLE BORDER=\"1\" CLASS=\"CALSTABLE\">\n<TR>"; 
								os << "<TH ALIGN=\"LEFT\" VALIGN=\"TOP\">"<< tr("Tag") << "</TH>";
								os << "<TH ALIGN=\"LEFT\" VALIGN=\"TOP\">"<< tr("Mean") << "</TH>";
								os << "<TH ALIGN=\"LEFT\" VALIGN=\"TOP\">"<< tr("Maximum") << "</TH>";
								os << "<TH ALIGN=\"LEFT\" VALIGN=\"TOP\">"<< tr("Minimum") << "</TH>";
								os << "<TH ALIGN=\"LEFT\" VALIGN=\"TOP\">"<< tr("Std. Dev.") << "</TH>";
								os << "<TH ALIGN=\"LEFT\" VALIGN=\"TOP\">"<< tr("Samples") << "</TH>";
								os << "<TH ALIGN=\"LEFT\" VALIGN=\"TOP\">"<< tr("No. Alarms") << "</TH>";
								os << "<TH ALIGN=\"LEFT\" VALIGN=\"TOP\">"<< tr("No. Warnings") << "</TH>";
								os << "<TH ALIGN=\"LEFT\" VALIGN=\"TOP\">"<< tr("% Alarms") << "</TH>";
								os << "<TH ALIGN=\"LEFT\" VALIGN=\"TOP\">"<< tr("% Warnings") << "</TH>";
								os << "<TH ALIGN=\"LEFT\" VALIGN=\"TOP\">"<< tr("% Not Alarm") << "</TH>";
								os << "</TR>" << endl;
								//
								// tuples out - all out
								// 
								// 
								//
								for(int i = 0; i < nf; i++)
								{
									//  
									os << "<TR>";
									os << "<TD>" << tags[i] << "</TD>";
									//
									if(stats[i].samples() > 0)
									{
										os << "<TD>" << stats[i].mean() << "</TD>";
										os << "<TD>" << stats[i].Max() << "</TD>";
										os << "<TD>" << stats[i].Min() << "</TD>";
										os << "<TD>" << stats[i].stdDev() << "</TD>";
										os << "<TD>" << stats[i].samples() << "</TD>";
										os << "<TD>" << stats[i].nAlarms << "</TD>";
										os << "<TD>" << stats[i].nWarnings << "</TD>";
										double a = (double)(stats[i].nAlarms)/(double)stats[i].samples() * 100.0;
										os << "<TD>" << floor(a) << "</TD>"; // round down to an integer
										a = (double)(stats[i].nWarnings)/(double)stats[i].samples() * 100.0;
										os << "<TD>" << floor(a) << "</TD>"; // round down to an integer
										a = (double)(stats[i].nAlarms)/(double)stats[i].samples() * 100.0;
										os << "<TD>" << floor(100.0 - a) << "</TD>"; // round down to an integer
									}
									else
									{
										// empty
										os << "<TD>***</TD>";
										os << "<TD>***</TD>";
										os << "<TD>***</TD>";
										os << "<TD>***</TD>";
										os << "<TD>0</TD>";
										os << "<TD>***</TD>";
										os << "<TD>***</TD>";
										os << "<TD>***</TD>";
										os << "<TD>***</TD>";
										os << "<TD>***</TD>";
									};
									os << "</TR>" << endl;  
								};  
								// 
								//  Scale Graph
								//  
								os << "</TABLE><P><br></P>" << endl; // end of table
								//
								graphData.SetLogMode(GetConfigureDb()->GetInt("LOGMODE"));

								QString ini_file = GetScadaHomeDirectory() + "\\project\\scada.ini";
								Inifile iniFile((const char*)ini_file);
								
								QString str;
								if((str = iniFile.find("maxItems","report_generation")) != NULL)
								{
									int max_i = atoi(str);
									graphData.SetMaxPoints(max_i); 
								}

								graphData.ScaleGraph(); // get the limits and steps
								//
								if( nf > N_TAGS_FOR_STATS_PAGE_BREAK )
								{
									os << NEW_PAGE << endl; // add page break - only affects printing
									os << "<H2>"<< tr("Graph For:");
									os << GetConfigureDb()->GetString("NAME") << " (" << Units << ")"<< "</H2><br>" << endl;
								};
								// 
								QPixmap graph(REP_GRAPH_SIZE,REP_GRAPH_SIZE); // plotting area
								QPainter DC; // dc to the pixmap
								graph.fill(Qt::white);
								//
								DC.begin(&graph);
								graphData.Plot(DC,QRect(0,0,REP_GRAPH_SIZE,REP_GRAPH_SIZE),Qt::white); // draw it
								DC.end(); 
								//
								//  write as a PNG file
								//  
								graph.save(Dir + "/" + GetConfigureDb()->GetString("NAME") + ".png","PNG"); // save as a PNG file
								// insert the hyper link
								os << "<!GRAPH>" << endl; // mark this a s a graph object 
								os << "<img width=\"" << REP_GRAPH_SIZE << "\" height=\"" 
								<< REP_GRAPH_SIZE << "\" src=\"" << (GetConfigureDb()->GetString("NAME") + ".png") 
								<< "\" alt=\"Home\" align=\"left\" border=\"0\">" << endl;
								// 
								// 
								// 
								// 
								if(GraphStats != tr(NONE_STR))
								{
									// finish writing out
									int i = GBuffer.find(START_TOKEN);
									if(i > 0)
									{
										const char *p = (const char *)GBuffer;
										os << endl;
										os.writeRawBytes(p + i,GBuffer.length() - i); // write the trailing  part
									};
								};
								//
								//
								// now we write out the graph for the report printer
								// we write out the raw data
								// 
								{
									QString ofile(Dir + "/" + GetConfigureDb()->GetString("NAME") + ".grf");
									QFile of(ofile);
									if(of.open(IO_WriteOnly))
									{
										QDataStream os(&of); // must be a data stream 
										os << graphData.plotMap.size(); // number of tags
										os << graphData.logMode; // are we in log mode
										os << Title; // title of the report
										os << GetConfigureDb()->GetString("NAME") ; // sample point name
										os << UndoEscapeSQLText(GetConfigureDb()->GetString("COMMENT"));
										os << Units; // the units
										//
										Graph::DATAMAP::iterator i = graphData.plotMap.begin();
										for(;!(i == graphData.plotMap.end()); i++)
										{
											os << (*i).first << (*i).second.size(); // tag name and number of points
											Graph::DataMap::iterator j = (*i).second.begin();
											for(;!( j == (*i).second.end()); j++)
											{
												os << (*j).first << (*j).second;
											};
										};
									};
								};
								//
								//
								os << "</HTML>" << endl;
							};
						};
					};
				};
			};
			//
			// Write the index file
			//
			//
			QFile idxf(Dir + "/index.html");
			if(idxf.open(IO_WriteOnly))
			{
				QTextStream os(&idxf);
				QString FrontBuf;
				//
				os << "<HTML> <HEAD> <TITLE> " << tr("Index For Report ") << Name << "</TITLE>\n";
				os << "</HEAD>\n";
				os << "<BODY BGCOLOR=\"#FFFFFF\" TEXT=\"#000000\" LINK=\"#0000FF\" VLINK=\"#840084\" ALINK=\"#0000FF\" >\n";
				//
				if(Front != tr(NONE_STR))
				{
					// 
					QFile tf(QSTEMPLATE_DIR + "/" + Front + ".html");
					if(tf.open(IO_ReadOnly))
					{
						char* b = new char[tf.size() + 1]; // these should be only a few k
						tf.readBlock(b,tf.size());
						b[tf.size()] = 0; // null terminate
						//
						FrontBuf = QString(b); // turn into a QString
						//
						// look for the START_TOKEN
						int i = FrontBuf.find(START_TOKEN);
						if(i > 0)
						{
							const char *p = (const char *)FrontBuf;
							os.writeRawBytes(p,i); // write the leading part
						};
					};
				};
				//    
				// write out the list of points
				//
				os << "<H1>" << tr("Report: ") << Title << "</H1><br>" << endl; 
				if(!BatchName.isEmpty())
				{
					os << "<B>" << tr("Batch ID:") << BatchName << "</B><br>" << endl;
				};
				os << "<P>" << tr("Report From ") << From.toString() << tr(" To ") << To.toString() << "</P>" << endl;
				//
				switch(AlarmFilter)
				{
					case 0:
					os << "<br><P>" << tr("All Results") << "</P>" << endl;
					break;
					case 1:
					os << "<br><P>" << tr("Alarms And Warnings Only In Tables") << "</P>" << endl;
					break;
					case 2:
					os << "<br><P>" << tr("Alarms Only In Tables") << "</P>" << endl;
					break;
					default:
					break;
				};
				//
				os << "<H3>" << endl;
				os << "<TABLE BORDER=\"1\" CLASS=\"CALSTABLE\" ALIGN=\"LEFT\">\n<TR>"; 
				os << "<TH ALIGN=\"LEFT\" VALIGN=\"TOP\">"<< tr("Sample Point") << "</TH>";
				os << "<TH ALIGN=\"LEFT\" VALIGN=\"TOP\">"<< tr("Tables") << "</TH>";
				os << "<TH ALIGN=\"LEFT\" VALIGN=\"TOP\">"<< tr("Statistics and Graph") << "</TH>";
				os << "<TH ALIGN=\"LEFT\" VALIGN=\"TOP\">"<< tr("Comment") << "</TH>";
				os << "</TR>" << endl;
				//
				GetConfigureDb()->GotoBegin();
				//
				for(unsigned ip = 0; ip < GetConfigureDb()->GetNumberResults(); ip++,GetConfigureDb()->FetchNext())
				{
					os << "<TR>";
					os << "<TD>" << GetConfigureDb()->GetString("NAME") << "</TD>"; // the sample point
					//
					os << "<TD> <a href=\"" 
					<< (GetConfigureDb()->GetString("NAME") + "_tab.html") << "\">" << tr("Table") << "</a> </TD>";
					//
					os << "<TD> <a href=\"" 
					<< (GetConfigureDb()->GetString("NAME") + "_stats.html") << "\">" << tr("Statisics and Graph") << "</a></TD>";
					//
					os << "<TD>" << UndoEscapeSQLText(GetConfigureDb()->GetString("COMMENT")) << "</TD>" ;
					os << "</TR>" << endl;
				};
				if(Back != tr(NONE_STR))
				{
					// when printed this will come out last
					os << "<BR><BR>" << endl << "<a href=\"" 
					<< (QSTEMPLATE_DIR + "/" + Back + ".html") << "\">" << tr("End") << "</a>";
				};
				//
				os << "</TABLE>" << "</H3>" << endl;
				//
				if(Front != tr(NONE_STR))
				{  
					// finish writing out
					// finish writing out
					int i = FrontBuf.find(START_TOKEN);
					if(i > 0)
					{
						const char *p = (const char *)FrontBuf;
						os.writeRawBytes(p + i,FrontBuf.length() - i); // write the trailing  part
					};
				};
				os << "</HTML>" << endl;
			};
			//
			//
			//cerr << "Report All Complete" << endl;
			//qApp->quit(); // all done 

			QString ini_file = GetScadaHomeDirectory() + "\\project\\scada.ini";
			Inifile iniFile((const char*)ini_file);

			if(iniFile.find("path","internet_browser"))
			{
				QString path;
				path = iniFile.find("path","internet_browser");

				QString cmd = path + " " + QSREPORT_DIR + "\\" + Name + "\\index.html";
				
				// create new process
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


	//			QString home = QSREPORT_DIR + "\\" + Name + "\\index.html";
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
/*
*Function::ResultsQueryResponse
*Inputs:client, command, id
*Outputs:none
*Returns:none
*/
void ReportGenerator::ResultsQueryResponse (QObject *p,const QString &, int id, QObject* caller)
{
	if(p != this) return;

	IT_IT("ReportGenerator::ResultsQueryResponse");

	switch(id)
	{
		case tResults: // results table
		{
			//
			// 
			QSTransaction &t = GetResultDb()->CurrentTransaction();
			//
			//cerr << "Got Results Table For " << (const char *)t.Data1 << endl;
			//
			#ifdef UNIX
			QFile file(Dir + "/" + t.Data1 + ".txt");
			#else
			QFile file(Dir + "\\" + t.Data1 + ".txt");
			#endif

			if(file.open(IO_WriteOnly))
			{
				QTextStream os(&file);
				//
				// write out the header
				//
				int n = GetResultDb()->GetNumberResults();
				//
				if(n)
				{
					// write the results set as space separated 
					QStringList l;
					if(GetResultDb()->GetFieldNames(l) > 2)
					{
						os << n << " " << (l.count() + 1) << endl; // date-time field ends up as two fields, space separated
						os << tr("Date") << " " << tr("Time") << " " << tr("State") << " ";
						for(unsigned k = 2; k < l.count();k++)
						{
							os << l[k] << " ";
						};
						os << endl;
						//
						for(int i = 0; i < n; i++,GetResultDb()->FetchNext())
						{
							for(unsigned j = 0; j < l.count();j++)
							{
								//
								// it is here we could do date translation into local format
								//
								QString s;
								if(l[j] == "TIMEDATE")
								{
									s = GetResultDb()->GetIsoDateString(l[j]);
								}
								else
								{
									s = GetResultDb()->GetString(l[j]);
								}
								
								if(s.isEmpty())
								{
									os << "*** "; // null value
								}
								else
								{
									os << s << " ";
								};
							};
							os << endl;
						};
						//            
					};
				};
			};
			//  
			// if this is the last transaction then we have to query for the sample point configurations
			//
			nDone++;
			//cerr << " Response: Results Transaction Queue Size " << nDone << endl;
			if(nDone == Points.count()) // last item in the queue 
			{
				// fetch the sample configurations
				QString cmd = "select * from SAMPLE where NAME in ("; // request the list
				//
				for(unsigned i = 0; i < Points.count(); i++)
				{
					if(i >0) cmd+= ",";
					cmd += "'"+Points[i]+"'";
				};    
				cmd += ");";
				GetConfigureDb()->DoExec(this,cmd,tSample);

				IT_COMMENT1("Query: %s", (const char *)cmd);
			};
		};
		break;
		//
		default:
		break;
		//
	};
	//
	// if there are no more transactions left (ie only one) then quit
	// 
};
