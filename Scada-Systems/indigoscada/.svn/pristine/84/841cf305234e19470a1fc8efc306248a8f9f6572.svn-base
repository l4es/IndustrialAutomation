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
*Multipen trace.
*Purpose:Several sample points on a pen trace
*/
#include "multitrace.h"
#include "IndentedTrace.h"
#include "pentrace.xpm"
#include "quit.xpm"
//
//
/*
*Function:Multitrace
*Inputs:none
*Outputs:none
*Returns:none
*/
Multitrace::Multitrace() : QMainWindow(0,0,WDestructiveClose),maxItems(2000),back_in_time(7200),
pChart(NULL),List(NULL),pSm(NULL)
{
	IT_IT("Multitrace::Multitrace");
	
	connect (GetConfigureDb (),
	SIGNAL (TransactionDone (QObject *, const QString &, int, QObject*)), this,
	SLOT (ConfigQueryResponse (QObject *, const QString &, int, QObject*)));	// connect to the database
	//   
	connect (GetResultDb (),
	SIGNAL (TransactionDone (QObject *, const QString &, int, QObject*)), this,
	SLOT (ResultsQueryResponse (QObject *, const QString &, int, QObject*)));	// connect to the database
	//
	connect (GetCurrentDb (),
	SIGNAL (TransactionDone (QObject *, const QString &, int, QObject*)), this,
	SLOT (CurrentQueryResponse (QObject *, const QString &, int, QObject*)));	// connect to the database
	//
	//
	// create the splitter
	pSm =  new QSplitter(QSplitter::Horizontal,this); // vertical split
	pSm->setFrameStyle(QFrame::Panel | QFrame::Sunken);
	//
	//
	// Add the selection dialog
	List = new QListView(pSm);
	List->setFrameStyle(QFrame::Panel | QFrame::Sunken);
	List->addColumn(tr("Sample Points"));
	//
	// Add the Graph Pane
	#ifdef QWT_TRACE
	pChart = new GraphPaneQwt(pSm); // line chart 
	#else
	pChart = new GraphPane(pSm); // line chart 
	#endif

	QString ini_file = GetScadaHomeDirectory() + "\\project\\scada.ini";
	Inifile iniFile((const char*)ini_file);

	const char* str;
	if((str = iniFile.find("maxItems","trace_window")) != NULL)
	{
		maxItems = atoi(str);
	}
	if((str = iniFile.find("back_in_time","trace_window")) != NULL)
	{
		back_in_time = atoi(str);
	}

	pChart->SetMaxPoints(maxItems); 
	pChart->SetTitle(tr("Pen Trace"));
	pChart->Clear();
	//
	//
	QValueList<int> l;
	l.append(width()  / 4);
	l.append(width()*3 / 4);
	pSm->setSizes(l);
	setCentralWidget(pSm);
	//
	QPopupMenu *file = new QPopupMenu(this);
	file->insertItem(tr("Open Configuration..."),this,SLOT(Load()));
	file->insertItem(tr("Save Configuration..."),this,SLOT(Save()));
	//TODO: fix the printing of trace 14-08-03
	//file->insertItem(tr("Print Plot..."),this,SLOT(Print())); 
	file->insertItem(QPixmap((const char **)quit_xpm), tr("Close"),this,SLOT(Close()));
	menuBar()->insertItem(tr("&File"),file);
	//
	//
	//
	QPopupMenu *options = new QPopupMenu(this);
	options->insertItem(tr("Use Log Scales"),this,SLOT(LogScalesOn()));
	options->insertItem(tr("Use Linear Scales"),this,SLOT(LinScalesOn()));
	menuBar()->insertItem(tr("&Options"),options);
	//
	setCaption(tr("Pen Trace"));
	//
	move(20,20);
	resize(700,400); // start with a sensible size
	GetCurrentDb()->DoExec(this,"select NAME,TAGNAME from TAGS_DB;",tList); // get the list of sample points and tags
	//
	QPixmap icon((const char **)pentrace);
	setIcon(icon);
	//
	QTimer *pT = new QTimer(this);
	connect (pT, SIGNAL (timeout ()), this, SLOT (Check()));
	pT->start(5000); // check for changes every 5 seconds
	//
};
/*
*Function:Print
*Print the plot
*Inputs:none
*Outputs:none
*Returns:none
*/
void Multitrace::Print()
{
	IT_IT("Multitrace::Print");
	// print the graph
	pChart->Print();
};
/*
*Function:
*Inputs:none
*Outputs:none
*Returns:none
*/
void Multitrace::ConfigQueryResponse (QObject *p,const QString &, int id, QObject* caller)  // handles configuration responses
{
	if(p != this) return;

	IT_IT("Multitrace::ConfigQueryResponse");
};
/*
*Function:
*Inputs:none
*Outputs:none
*Returns:none
*/
void Multitrace::ResultsQueryResponse (QObject *p,const QString &, int id, QObject* caller) // results responses
{
	if(p != this) return;

	IT_IT("Multitrace::ResultsQueryResponse");

	switch(id)
	{
		case tData: //graph initialization
		{
			// we have got some data for a sample point - add it to the graph 
			// find the node in the list - get the list of tags to load from record
			QSTransaction &t = GetResultDb()->CurrentTransaction();
			//
			int n = GetResultDb()->GetNumberResults();

			if(n > maxItems)
			{
				QString message = tr("Fetching Data; The graph is truncated for old values; Fetched records = ") + 
								QString::number(n) + ", max num of records = " + QString::number(maxItems);
				statusBar()->message(message);				
			}

			for(int i = 0; i < n; i++, GetResultDb()->FetchNext())
			{
				// which tags do we want
				QDateTime dt =  IsoDateQDateTime(GetResultDb()->GetIsoDateString("TIMEDATE"));  // time stamp
				SpDict::iterator j = Items.find(t.Data1);
				if(!(j == Items.end()))
				{
					ItemDict::iterator k = (*j).second.begin();
					//  	   
					for(;!(k == (*j).second.end());k++) // is it in the selected points dictionary
					{
						if((*k).second->isOn()) // enabled ?
						{
							// if so update the graph
							QString s = (*j).first + "[" + (*k).first + "]";
							//IT_COMMENT1("the tag %s isOn and is added", (const char*)s);
							//cerr << i << " " << (const char *)(*k).first << " " << (const char *)GetResultDb()->GetString((*k).first,i) << endl;
							//
							bool ok = 0;
							double v = GetResultDb()->GetDouble((*k).first,&ok);
							if(ok)
							{
								pChart->Add(s,dt,v); // update graph
							};
							//
						};
					};
				};
			};
			pChart->Update(); // update the graph  
			statusBar()->message(tr(""));
			//this->setCursor(QCursor(ArrowCursor));
		};
		break;
		default:
		break;
	};
};
/*
*Function:
*Inputs:none
*Outputs:none
*Returns:none
*/
void Multitrace::CurrentQueryResponse (QObject *p,const QString &, int id, QObject* caller) // current value responses
{
	if(p != this) return;

	IT_IT("Multitrace::CurrentQueryResponse");

	switch(id)
	{
		case tList:
		{
			//
			// build the sample point tag list - build as a tree
			int n = GetCurrentDb()->GetNumberResults();
			QString last = QString::null; 
			QCheckListItem *item = 0;
			//
			Items.clear();
			//
			for(int i = 0; i < n; i++, GetCurrentDb()->FetchNext())
			{
				if(last != GetCurrentDb()->GetString("NAME"))
				{
					//
					last = GetCurrentDb()->GetString("NAME");   
					ItemDict d;
					SpDict::value_type pr(last,d);
					Items.insert(pr);
					//     
					item = new QCheckListItem(List,last); // created as a controller
				};
				//
				//
				QCheckListItem*pi =  new QCheckListItem(item, GetCurrentDb()->GetString("TAGNAME"),QCheckListItem::CheckBox);
				SpDict::iterator j = Items.find(last);
				//
				if(!(j == Items.end()))
				{
					ItemDict::value_type pr(GetCurrentDb()->GetString("TAGNAME"),pi);     
					(*j).second.insert(pr);
				};
				//
				//
			};
			List->triggerUpdate();
		};
		break;
		default:
		break;
	};
};
/*
*Function:UpdateTags
*Inputs:none
*Outputs:none
*Returns:none
*/
void Multitrace::UpdateTags() 
{
	IT_IT("Multitrace::UpdateTags");
	
	//Here we have set of records from the query
	//select * from TAGS_DB where SEQNO >= " + LastFetchNo  + " order by NAME asc;" in user.cpp Dofetch()

	bool Changed = false;
	int n = GetCurrentDb()->GetNumberResults();
	GetCurrentDb()->GotoBegin(); // restart from the beginning
	for(int i = 0; i < n; i++, GetCurrentDb()->FetchNext())
	{
		SpDict::iterator j = Items.find(GetCurrentDb()->GetString("NAME"));
		if(!(j == Items.end()))
		{
			ItemDict::iterator k = (*j).second.find(GetCurrentDb()->GetString("TAGNAME"));
			//  	   
			if(!(k == (*j).second.end())) // is it in the selected points dictionary
			{
				if((*k).second->isOn())
				{
					QString s = (*j).first + "[" + (*k).first + "]";
					
					if(pChart->HasLine(s)) // is the line in the graph NOTE: the line il added to the graph the firs time by Check()
					{
						// if so update the graph
						QDateTime dt =  IsoDateQDateTime(GetCurrentDb()->GetIsoDateString("UPDTIME")); 
						pChart->Add(s,dt,GetCurrentDb()->GetDouble("VAL")); // update graph
						Changed = true;  
					}
					//else
					//{
					//	IT_COMMENT1("UpdateTags corsa %s", (const char*)s);
					//}
				};
			};
		};
	};
	//
	if(Changed)
	{
		// trigger a graph redraw
		pChart->Update(); // update the graph
	};
};
/*
*Function: Close
*Inputs:none
*Outputs:none
*Returns:none
*/
void Multitrace::Close()
{
	IT_IT("Multitrace::Close");

	if(List)
	{
		delete List;
		List = NULL;
	}
	if(pChart)
	{
		delete pChart;
		pChart = NULL;
	}
	if(pSm)
	{
		delete pSm;
		pSm = NULL;
	}
	
	close(true); // close and delete the window
};
/*
*Function: Load
*load a plot set up
*Inputs:none
*Outputs:none
*Returns:none
*/
void  Multitrace::Load()
{
	IT_IT("Multitrace::Load");
	
	QString s = QFileDialog::getOpenFileName(QSFilename(""),"*.plt");
	if(s.isEmpty())
	{
		return;
	};
	// clear all checks
	// load magic - valid ?
	// load the rest
	QFile fs(s);
	if(fs.open(IO_ReadOnly))
	{
		//
		QDataStream is(&fs);
		QString sp; // sample point
		QString tg; // tag
		//
		is >> sp;
		if(sp == "MULTITRACE") // magic phrase
		{
			//
			pChart->Clear(); // clean the display
			pChart->Update();
			//
			SpDict::iterator i = Items.begin();
			for(; !(i == Items.end()); i++)
			{
				ItemDict::iterator j = (*i).second.begin();
				for(; !(j == (*i).second.end()); j++)
				{   
					(*j).second->setOn(0); // un check                    
				};
			};
			//
			while(!is.atEnd()) // look for selected items
			{
				is >> sp >> tg;
				SpDict::iterator i = Items.find(sp);
				if(!(i == Items.end()))
				{
					ItemDict::iterator j = (*i).second.find(tg);
					if(!(j == (*i).second.end()))
					{
						(*j).second->setOn(true);
					};
				};
			};
			setCaption(tr("Pen Trace") + "[" + s + "]");
		};
	};
};
/*
*Function: Save
*Save a plot set up
*Inputs:none
*Outputs:none
*Returns:none
*/
void  Multitrace::Save()
{
	IT_IT("Multitrace::Save");
	
	//
	// save the magic number
	// save name / tags of items that are checked
	// 
	QString s = QFileDialog::getSaveFileName(QSFilename(""),"*.plt",this);
	if(s.isEmpty())
	{
		return;
	};
	//
	QFile fs(s);
	if(fs.open(IO_WriteOnly))
	{
		//
		QDataStream os(&fs);
		os << QString("MULTITRACE"); // magic number
		SpDict::iterator i = Items.begin();
		//
		for(; !(i == Items.end()); i++)
		{
			ItemDict::iterator j = (*i).second.begin();
			for(; !(j == (*i).second.end()); j++)
			{   
				if((*j).second->isOn())                     
				{
					os << (*i).first << (*j).first; // write out the check items only
				};
			};
		};
	};
	setCaption(tr("Pen Trace") + "[" + s + "]");		
};
/*
*Function:LogScalesOn
*Inputs:none
*Outputs:none
*Returns:none
*/
void Multitrace::LogScalesOn()
{
	IT_IT("Multitrace::LogScalesOn");

	pChart->SetLogMode(true);	
	pChart->Update();
};
/*
*Function:LinScalesOn
*Inputs:none
*Outputs:none
*Returns:none
*/
void Multitrace::LinScalesOn()
{
	IT_IT("Multitrace::LinScalesOn");
	
	pChart->SetLogMode(false);	
	pChart->Update();
};
/*
*Function: Check
*check for new lines
*Inputs:none
*Outputs:none
*Returns:none
*/
void Multitrace::Check()
{
	IT_IT("Multitrace::Check");
	
	//bool Changed = false;
	SpDict::iterator j = Items.begin();
	for(; !(j == Items.end());j++)
	{
		ItemDict::iterator k = (*j).second.begin(); // iterate tags
		//  	   
		for(; !(k == (*j).second.end());k++) // is it in the selected points dictionary
		{
			if((*k).second->isOn())
			{
				QString s = (*j).first + "[" + (*k).first + "]";

				//IT_COMMENT1("the tag %s isOn", (const char*)s);
				if(!pChart->HasLine(s)) // is the line in the graph
				{
					//
					//this->setCursor(QCursor(WaitCursor));
					statusBar()->message(tr("Fetching Data"));
					//IT_COMMENT1("the tag %s isOn and the graph has no line", (const char*)s);
					
					// request the data
					QDateTime t = QDateTime::currentDateTime().addSecs(-back_in_time);
					
					QString cmd = "select TIMEDATE," +  (*k).first + " from " + (*j).first + " where (TIMEDATE > "
					+ QDATE_TIME_ISO_DATE(t) + ") order by TIMEDATE asc;";
					
					GetResultDb()->DoExec(this,cmd,tData,(*j).first); // get the data from the last half hour to kick off
				};
			};
		};
	};
	//if(Changed)
	//{
		// trigger a graph redraw
	//	pChart->Update(); // update the graph
	//};
};

