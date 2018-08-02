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
*Header For: implimentation of inspect classes
*Purpose: these provide historic graphs and tables of results
*/
#include "historic_inspect.h"
#include "inspect.h"
#include "AckAlarmDlg.h"
#include "inifile.h"
#include "IndentedTrace.h"
#include "historicdb.h"
#include "inspect.xpm"
#include "quit.xpm"
#include "alm.xpm"
#include "pencil.xpm"

//Con query di percorrimento database  Last First      |<  <   >  >|

/*
*Function:HistoricInspect
*Inputs:sample point name
*Outputs:none
*Returns:none
*/
HistoricInspect::HistoricInspect(const QString &name, int t) : 
QMainWindow(0,name,WDestructiveClose),
Name(name),LastTime(QDateTime::currentDateTime()),maxItems(25),
pSm(NULL),pS(NULL),pHistory(NULL),pChart(NULL), MoreOldRecord(0),
MoreRecentRecord(0)
{
	
	IT_IT("HistoricInspect::HistoricInspect");
	//  
	// connect to the databases
	// 
	connect (GetConfigureDb (),
	SIGNAL (TransactionDone (QObject *, const QString &, int, QObject*)), this,
	SLOT (ConfigQueryResponse (QObject *, const QString &, int, QObject*)));	// connect to the database
	//   
	connect (GetHistoricResultDb (),
	SIGNAL (TransactionDone (QObject *, const QString &, int, QObject*)), this,
	SLOT (ResultsQueryResponse (QObject *, const QString &, int, QObject*)));	// connect to historic database
	//
	connect (GetCurrentDb (),
	SIGNAL (TransactionDone (QObject *, const QString &, int, QObject*)), this,
	SLOT (CurrentQueryResponse (QObject *, const QString &, int, QObject*)));	// connect to the database

	//////////////Toolbar//////////////////////////////////////////////
	#include "first_cmp.xpm"
	#include "before_cmp.xpm"
	#include "next_cmp.xpm"
	#include "last_cmp.xpm"
	pToolBarBrowse = new QToolBar("Browse",this,QMainWindow::Top);
	(void)  new QToolButton(QPixmap((const char **)first_cmp),tr("First"),0,this,SLOT(first()),pToolBarBrowse, "First");
	(void)  new QToolButton(QPixmap((const char **)before_cmp),tr("Previous"),0,this,SLOT(before()),pToolBarBrowse, "Previous");
	(void)  new QToolButton(QPixmap((const char **)next_cmp),tr("Next"),0,this,SLOT(next()),pToolBarBrowse, "Next");
	(void)  new QToolButton(QPixmap((const char **)last_cmp),tr("Last"),0,this,SLOT(last()),pToolBarBrowse, "Last");
	// and the LineEdit
    lined2 = new QLineEdit(pToolBarBrowse);
	lined2->setValidator( new QIntValidator(lined2));
	
	connect (lined2, SIGNAL (textChanged (const QString &)),
	SLOT (Changed (const QString &)));
	
	///////////end////////Toolbar//////////////////////////////////////

	//
	pSm =  new QSplitter(QSplitter::Vertical,this); // vertical split, => main horizontal bar
	//
	// add the search widget
	// 
	Status.SetName(name); 
	Status.Create(pSm,Qt::Horizontal); //=> vertical sub bar 1
	// connect to the go button
	//    
	pS = new QSplitter(pSm); //=> vertical sub bar 2
	
	///////////////////////////////////////////////////////////////////

	QString ini_file = GetScadaHomeDirectory() + "\\project\\scada.ini";
	Inifile iniFile((const char*)ini_file);

	QValueList<int> l;
	const char* str;
	if((str = iniFile.find("pSmSize1","inspect_window")) != NULL)
	{
		l.append(atoi(str));
	}
	if((str = iniFile.find("pSmSize2","inspect_window")) != NULL)
	{
		l.append(atoi(str));
	}
		
	pSm->setSizes(l);

	setCentralWidget(pSm);

	//
	this->setCursor(QCursor(WaitCursor));
	//
	statusBar()->message(tr("Fetching Data"));
	//
	pHistory = new StatusListView(pS); // build the header time, tags
	pHistory->addColumn(tr("Date/Time")); // historical tabular
	pHistory->setItemMargin(4);
	pHistory->setSorting(0);
	pHistory->setShowSortIndicator(true);
	//pHistory->resize(pHistory->size());

	// Disallow sorting of the table by clicking on headers
	QHeader *qh = pHistory->header();
	disconnect(qh, SIGNAL(sectionClicked(int)),pHistory, SLOT(changeSortColumn(int)));


	pS->setResizeMode (pHistory, QSplitter::Stretch);
	
	//
	pChart = new GraphPane(pS); // line chart 
	if((str = iniFile.find("maxItems","inspect_window")) != NULL)
	{
		maxItems = atoi(str);
	}

	lined2->setText(QString::number(maxItems));

	const char* str2;
	if((str = iniFile.find("minY","inspect_window")) != NULL 
		&& (str2 = iniFile.find("maxY","inspect_window")) != NULL)
	{
		double miny = atoi(str);
		double maxy = atoi(str2);
		pChart->SetMinMaxY(miny, maxy);
	}

	pChart->SetMaxPoints(maxItems); 
	pChart->SetTitle(tr("Plot of Sample Point") + " " + Name);

	pS->setResizeMode (pChart, QSplitter::Stretch);
	
	l.clear();

	if((str = iniFile.find("pSSize1","inspect_window")) != NULL)
	{
		l.append(atoi(str));
	}
	if((str = iniFile.find("pSSize2","inspect_window")) != NULL)
	{
		l.append(atoi(str));
	}
	
	pS->setSizes(l);

	//
	// retrive the results
	QDateTime dt = QDateTime::currentDateTime().addSecs(-t);

	QString cmd = "select * from " + Name + " where  (TIMEDATE > " + QDATE_TIME_ISO_DATE(dt) + ") order by TIMEDATE asc limit " + QString::number(maxItems) + ";"; 
	
	GetHistoricResultDb()->DoExec(this,cmd,tResults);
	//
	// we need the configuration details 	     
	cmd = "select * from SAMPLE where NAME = '"+Name+"';";
	GetConfigureDb()->DoExec(this,cmd,tItem);
	//   
	connect(pHistory,SIGNAL(rightButtonClicked(QListViewItem *, const QPoint &, int)),SLOT(PointMenu(QListViewItem *, const QPoint &, int))); 
	connect(pChart,SIGNAL(RightClick()), SLOT(GraphMenu()));

	//resize(400,300); 
	QPopupMenu *file = new QPopupMenu(this);
	file->insertItem(QPixmap((const char **)quit_xpm),tr("Close"),this,SLOT(Close()));
	menuBar()->insertItem(tr("&File"),file);
	//file->insertSeparator();
	//file->insertItem(QPixmap((const char **)pencil_xpm),tr("Points ia a window"),this,SLOT(getMaxItems()));
	//
	//
	QPixmap icon((const char **)inspect);
	setIcon(icon);
	//
};
/*
*Function:ConfigQueryResponse
*Inputs:client object, command string, transaction id
*Outputs:none
*Returns:none
*/
void HistoricInspect::ConfigQueryResponse (QObject *p,const QString &, int id, QObject* caller)  // handles configuration responses
{
	if(p != this) return;

	IT_IT("HistoricInspect::ConfigQueryResponse");

	switch(id)
	{
		case tItem: // the point's configuration - comment and units in particular
		{
			if(GetConfigureDb()->GetNumberResults() > 0)
			{
				//
				// set the title and units
				QString s =  tr("Historical data for ") + Name + " [" + UndoEscapeSQLText(GetConfigureDb()->GetString("COMMENT")) + "] " +
				GetConfigureDb()->GetString("UNITS");
				setCaption(s);
				//
				pChart->SetLogMode(GetConfigureDb()->GetBool("LOGMODE"));  
				pChart->Update();     
				// 
				// update the graph
				//
				Status.UpdateHeader(); // update the header
				QString cmd = "select * from TAGS_DB where NAME='"+  Name +"';"; // update the tag values for the selected SP
				GetCurrentDb()->DoExec(this,cmd,tTags);
				cmd = "select * from CVAL_DB where NAME='"+ Name +"';"; // update the tag values for the selected SP
				GetCurrentDb()->DoExec(this,cmd,tSamplePoint);
				//
				// get the alarm limits
				cmd = "select * from TAGS where NAME='"+Name+"' and RECEIPE='"+GetReceipeName()+"';";
				GetConfigureDb()->DoExec(this,cmd,tTagLimits);
				//
			};
		};
		break;
		case tTagLimits:
		{
			Status.UpdateTagsLimits();
		};
		break;
		default:
		break;
	};
};
/*
*Function:
*Inputs:client object, command string, transaction id
*Outputs:none
*Returns:none
*/
void HistoricInspect::ResultsQueryResponse (QObject *p, const QString &c, int id, QObject* caller) // results responses
{
	if(p != this) return;
	
	IT_IT("HistoricInspect::ResultsQueryResponse");

	switch(id)
	{
		case tResults: //graph initialization
		{
			int n = GetHistoricResultDb()->GetNumberResults();

			if(n > 0)
			{
				pHistory->clear();
				pChart->Clear();
				
				if(n > maxItems)
				{
					QString message = tr("Fetching Data; The graph is truncated for old values; Fetched records = ") + 
									QString::number(n) + ", max num of records = " + QString::number(maxItems);
					statusBar()->message(message);				
				}
				// first field is date/time
				// then state 
				// then list of tags
				//
				QStringList l;
				StateListItem *p;
				int j = GetHistoricResultDb()->GetFieldNames(l);

				if(j > 1)
				{ 
					QHeader *qh = pHistory->header();
					int ncol_in_header = qh->count();

					if(ncol_in_header == 1)
					{
						for(int i = 2; i < j; i++)
						{    
							// add the tag headings
							pHistory->addColumn(l[i]); // historical tabular
						};
					}
						// now add the data
					for(int k = 0; k < n; k++,GetHistoricResultDb()->FetchNext())
					{
						timestamp = _atoi64((const char*)(GetHistoricResultDb()->GetString("TIMEDATE"))); 

						if(k == 0)
						{
							MoreOldRecord = timestamp;
							MoreRecentRecord = MoreOldRecord;
						}
						else
						{
							if(timestamp > MoreRecentRecord)
							{
								MoreRecentRecord = timestamp;
							}

							if(timestamp < MoreOldRecord)
							{
								MoreOldRecord = timestamp;
							}
						}

						QString ds =  GetHistoricResultDb()->GetIsoDateString("TIMEDATE");

						//////////////////
						//QString a = QDATE_TIME_ISO_DATE(IsoDateQDateTime(ds));
						//IT_COMMENT2("TIMEDATELIST = %s %s", (const char*)ds, (const char*)a);
						//////////////////

						LastTime =  IsoDateQDateTime(ds);
						p = new StateListItem(pHistory, 
						ds, // the time stamp as ISO
						"",
						GetHistoricResultDb()->GetInt("STATE")
						);
						// 
						for(unsigned i = 2; i < l.count(); i++)
						{
							double v = atof((const char*)(GetHistoricResultDb()->GetString(l[i])));
							QString val = QString::number(v,'f',2); //two decimal points of precision
							p->setText(i - 1, val); // fill out the data value
							pChart->Add(l[i], LastTime, GetHistoricResultDb()->GetDouble(l[i])); // add to the graph
						};
						// 
					};
				};

				pChart->Update();
				pHistory->triggerUpdate();
				pHistory->ensureItemVisible(p);
			};

			statusBar()->message(tr(""));
			this->setCursor(QCursor(ArrowCursor));
		};
		break;
		default:
		break;
	};
};
/*
*Function:
*Inputs:client object, command string, transaction id
*Outputs:none
*Returns:none
*/
void HistoricInspect::CurrentQueryResponse (QObject *p,const QString &c, int id, QObject* caller) // current value responses
{
	if(p != this) return;

	IT_IT("HistoricInspect::CurrentQueryResponse");

	switch(id)
	{
		case tTags:
		{
			Status.UpdateTagsPane();
		};
		break;
		case tSamplePoint:
		{
			Status.UpdateSamplePoint();
		};
		break;
		default:
		break;
	};
};
/*
*Function:PointMenu
*Inputs:nothing useful
*Outputs:none
*Returns:none
*/
void HistoricInspect::PointMenu(QListViewItem *, const QPoint &, int) // handle right click on history pane
{
	IT_IT("HistoricInspect::PointMenu");

	#if 0
	// put up a menu - print / cancel 
	QPopupMenu m(pHistory);
	//m.insertItem(tr("Print..."),0); // print the graph
	//m.insertItem(QPixmap((const char **)quit_xpm),tr("Cancel"),-1);
	//
	switch(m.exec(QCursor::pos()))
	{
		case 0: 
		{
		};
		break;
		default:
		break;
	};
	#endif
};
/*
*Function:GraphMenu
*Inputs:none
*Outputs:none
*Returns:none
*/
void HistoricInspect::GraphMenu() // handle right click on graph menu
{
	IT_IT("HistoricInspect::GraphMenu");
	
	// put up a print / cancel menu
	QPopupMenu m(pChart);
	//m.insertItem(tr("Print..."),0); // print the graph
	//m.insertItem(QPixmap((const char **)quit_xpm),tr("Cancel"),-1);
	//
	switch(m.exec(QCursor::pos()))
	{
		case 0: 
		{
			pChart->Print();
		};
		break;
		default:
		break;
	};
};
/*
*Function:UpdateTags
*update tags for this inspector
*Inputs:none
*Outputs:none
*Returns:none
*/
void HistoricInspect::UpdateTags() 
{
	IT_IT("HistoricInspect::UpdateTags");
	
	//Here we have set of records from the query
	//select * from TAGS_DB where SEQNO >= " + LastFetchNo  + " order by NAME asc;" in user.cpp Dofetch()

	int n = GetCurrentDb()->GetNumberResults();
	GetCurrentDb()->GotoBegin();
	for(int i = 0; i < n; i++,GetCurrentDb()->FetchNext())
	{
		if(GetCurrentDb()->GetString("NAME") == Name)  //sample point name
		{
			QDateTime dt =  IsoDateQDateTime(GetCurrentDb()->GetIsoDateString("UPDTIME"));
			if(dt > LastTime) // filter out duplicates
			{
				LastTime = dt;
				//
				//if(pChart->HasLine(GetCurrentDb()->GetString("TAGNAME")))
				//{
					pHistory->setSorting(0);
					pItem = new StateListItem(pHistory, GetCurrentDb()->GetIsoDateString("UPDTIME"),"", GetCurrentDb()->GetInt("STATE")); 
					//
					// now add the tag values
					// 
					for(int j = i; j < n; j++,GetCurrentDb()->FetchNext())
					{
						if(GetCurrentDb()->GetString("NAME") == Name)
						{
							//
							// find the correct tag heading or create a new one
							//
							pChart->Add(GetCurrentDb()->GetString("TAGNAME"),dt,GetCurrentDb()->GetDouble("VAL")); // update graph
							//
							bool Done = false;

							for(int k = 1; k < pHistory->columns(); k++)
							{
								if(pHistory->columnText(k) == GetCurrentDb()->GetString("TAGNAME"))
								{
									double v = atof((const char*)(GetCurrentDb()->GetString("VAL")));
									QString val = QString::number(v,'f',2); //two decimal points of precision
									pItem->setText(k, val);
									Done = true;
									break;
								}
							}

							if(!Done)
							{
								int k = pHistory->addColumn(GetCurrentDb()->GetString("TAGNAME"));

								double v = atof((const char*)(GetCurrentDb()->GetString("VAL")));
								QString val = QString::number(v,'f',2); //two decimal points of precision
								pItem->setText(k, val);
							}

							//
							if(pItem->GetState() < GetCurrentDb()->GetInt("STATE"))
							{
								pItem->SetState(GetCurrentDb()->GetInt("STATE"));
							}
							//
							//
							if(pHistory->childCount() > maxItems)
							{
								QListViewItem* p = pHistory->firstChild();
								pHistory->takeItem(p);
								delete p;
							}
						}
						else
						{
							break;
						}    
					}
					//
					pChart->Update(); // update the graph  
					pHistory->triggerUpdate(); // all done
					pHistory->ensureItemVisible(pItem);
					//
					return; // no point looking any further  
				//}
			};
		};
	};
};
/*
*Function: Close
*Inputs:none
*Outputs:none
*Returns:none
*/
void HistoricInspect::Close()
{
	IT_IT("HistoricInspect::Close");

	QString ini_file = GetScadaHomeDirectory() + "\\project\\scada.ini";
	Inifile iniFile((const char*)ini_file);

	QValueList<int> vl;
	vl = pS->sizes();

	for(int i = 0; i<(int)vl.count(); ++i) 
	{
		int size = *vl.at(i);
	}

	vl = pSm->sizes();

	for(i = 0; i<(int)vl.count(); ++i) 
	{
		int size = *vl.at(i);
	}

	if(pHistory)
	{
		delete pHistory;
		pHistory = NULL;
	}

	if(pChart)
	{
		delete pChart;
		pChart = NULL;
	}

	if(pS)
	{
		delete pS;
		pS = NULL;
	}

	if(pSm)
	{
		delete pSm;
		pSm = NULL;
	}


//	delete file;

	close(true); // close and delete the window
};

HistoricInspect::~HistoricInspect()
{
	if(pHistory)
	{
		delete pHistory;
		pHistory = NULL;
	}
	if(pChart)
	{
		delete pChart;
		pChart = NULL;
	}
	if(pS)
	{
		delete pS;
		pS = NULL;
	}
	if(pSm)
	{
		delete pSm;
		pSm = NULL;
	}

	ScadaWindowsDict* inspect_win = GetInpectWindowsDict();

	ScadaWindowsDict::iterator j =  inspect_win->find(Name);

	if(!(j == inspect_win->end()))
	{
		inspect_win->erase(j);
	}
}

void HistoricInspect::first()
{
	IT_IT("HistoricInspect::first");

	QString cmd = "select * from " + Name + " order by TIMEDATE asc limit " + QString::number(maxItems) + ";"; 

	GetHistoricResultDb()->DoExec(this,cmd,tResults);
}

void HistoricInspect::before()
{
	IT_IT("HistoricInspect::before");

	char buffer[20];
	_i64toa(MoreOldRecord, buffer, 10);
	
	QString cmd = "select * from " + Name + " where (TIMEDATE <" +
	QString(buffer) + ") order by TIMEDATE desc limit " + QString::number(maxItems) + ";";

	GetHistoricResultDb()->DoExec(this,cmd,tResults);
}

void HistoricInspect::next()
{
	IT_IT("HistoricInspect::next");

	char buffer[20];
	_i64toa(MoreRecentRecord, buffer, 10);

	QString cmd = "select * from " + Name + " where (TIMEDATE >" +
	QString(buffer) + ") order by TIMEDATE asc limit " + QString::number(maxItems) + ";"; 

	GetHistoricResultDb()->DoExec(this,cmd,tResults);
}

void HistoricInspect::last()
{
	IT_IT("HistoricInspect::last");

	QString cmd = "select * from " + Name + " order by TIMEDATE desc limit " + QString::number(maxItems) + ";"; 

	GetHistoricResultDb()->DoExec(this,cmd,tResults);
}

/*
*Function: getMaxItems
*make a usr entry
*Inputs:none
*Outputs:none
*Returns:none
*/
void HistoricInspect::Changed(const QString &)
{
	IT_IT("HistoricInspect::Changed");
	
	QString maxit = lined2->text().stripWhiteSpace();

	int m = atoi((const char*)maxit);

	if(m)
	{
		maxItems = m;
	}

	//AckAlarmDlg dlg(this);

	//dlg.SetHeading(tr("Points in a window"));

	//if(dlg.exec())
	//{
	//	maxItems = (dlg.GetComment()).toLong();
	//};
};

