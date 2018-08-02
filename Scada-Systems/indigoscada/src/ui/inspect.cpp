/*
 *                         IndigoSCADA
 *
 *   This software and documentation are Copyright 2002 to 2014 Enscada 
 *   Limited and its licensees. All rights reserved. See file:
 *
 *                     $HOME/LICENSE 
 *
 *   for full copyright notice and license terms. 
 *
 */
/*
*Header For: implimentation of inspect classes
*Purpose: these provide real time graphs and tables of results
*/
#include "inspect.h"
#include "AckAlarmDlg.h"
#include "inifile.h"
#include "IndentedTrace.h"
#include "inspect.xpm"
#include "quit.xpm"
#include "alm.xpm"
#include "button.xpm"
/*
*Function:Inspect
*Inputs:sample point name
*Outputs:none
*Returns:none
*/
Inspect::Inspect(const QString &name, int t) : 
QMainWindow(0,name,WDestructiveClose),
Name(name),LastTime(QDateTime::currentDateTime()),maxItems(2000),
pSm(NULL),pS(NULL),pHistory(NULL),pChart(NULL)
{
	
	IT_IT("Inspect::Inspect");
	//  
	// connect to the databases
	// 
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

	QString cmd = "select * from " + Name + " where  (TIMEDATE > " + QDATE_TIME_ISO_DATE(dt) + ") order by TIMEDATE asc;";

	GetResultDb()->DoExec(this,cmd,tResults);
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
void Inspect::ConfigQueryResponse (QObject *p,const QString &, int id, QObject* caller)  // handles configuration responses
{
	if(p != this) return;

	IT_IT("Inspect::ConfigQueryResponse");

	switch(id)
	{
		case tItem: // the point's configuration - comment and units in particular
		{
			if(GetConfigureDb()->GetNumberResults() > 0)
			{
				//
				// set the title and units
				QString s =  Name + " [" + UndoEscapeSQLText(GetConfigureDb()->GetString("COMMENT")) + "] " +
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
void Inspect::ResultsQueryResponse (QObject *p, const QString &c, int id, QObject* caller) // results responses
{
	if(p != this) return;
	
	IT_IT("Inspect::ResultsQueryResponse");

	switch(id)
	{
		case tResults: //graph initialization
		{
			int n = GetResultDb()->GetNumberResults();
			if(n > 0)
			{
				pHistory->clear();
				
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
				int j = GetResultDb()->GetFieldNames(l);
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
					for(int k = 0; k < n; k++,GetResultDb()->FetchNext())
					{
						QString ds =  GetResultDb()->GetIsoDateString("TIMEDATE");

						//////////////////
						//QString a = QDATE_TIME_ISO_DATE(IsoDateQDateTime(ds));
						//IT_COMMENT2("TIMEDATELIST = %s %s", (const char*)ds, (const char*)a);
						//////////////////

						LastTime =  IsoDateQDateTime(ds);
						p = new StateListItem(pHistory, 
						ds, // the time stamp as ISO
						"",
						GetResultDb()->GetInt("STATE")
						);
						// 
						for(unsigned i = 2; i < l.count(); i++)
						{
							double v = atof((const char*)(GetResultDb()->GetString(l[i])));
							QString val = QString::number(v,'f',2); //two decimal points of precision
							p->setText(i - 1, val); // fill out the data value
							pChart->Add(l[i], LastTime, GetResultDb()->GetDouble(l[i])); // add to the graph
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
void Inspect::CurrentQueryResponse (QObject *p,const QString &c, int id, QObject* caller) // current value responses
{
	if(p != this) return;

	IT_IT("Inspect::CurrentQueryResponse");

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
void Inspect::PointMenu(QListViewItem *, const QPoint &, int) // handle right click on history pane
{
	IT_IT("Inspect::PointMenu");

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
void Inspect::GraphMenu() // handle right click on graph menu
{
	IT_IT("Inspect::GraphMenu");
	
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
void Inspect::UpdateTags() 
{
	IT_IT("Inspect::UpdateTags");
	
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
void Inspect::Close()
{
	IT_IT("Inspect::Close");

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

Inspect::~Inspect()
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

static ScadaWindowsDict inspect_windows;

ScadaWindowsDict* GetInpectWindowsDict()
{
	return &inspect_windows;
};

static ScadaWindowsDict acknowledge_alarm_windows;

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#include "historic_inspect.h"
#include "historicdb.h"

inspectPopupMenu::inspectPopupMenu( class QWidget *parent, const char *name)
: QPopupMenu( parent, name )
{
	// connect to the databases
	// 
	connect (GetConfigureDb (),
	SIGNAL (TransactionDone (QObject *, const QString &, int, QObject*)), this,
	SLOT (ConfigQueryResponse (QObject *, const QString &, int, QObject*)));	// connect to configuration database
};

void inspectPopupMenu::get_unit(QWidget* parent, QString name)
{
	// get the UNIT
	char parent_string[15];
	ultoa((unsigned long)parent, parent_string, 10);

	QString par = QString(parent_string);
	QString cmd = "select UNIT from TAGS where NAME='"+name+"';";
	GetConfigureDb()->DoExec(this,cmd,tTagUnit, par, name);			
};

/*
*Function:ConfigQueryResponse
*Inputs:client object, command string, transaction id
*Outputs:none
*Returns:none
*/
void inspectPopupMenu::ConfigQueryResponse (QObject *p,const QString &, int id, QObject* caller)  // handles configuration responses
{
	if(p != this) return;

	IT_IT("inspectPopupMenu::ConfigQueryResponse");

	switch(id)
	{
		case tTagUnit:
		{
			QSTransaction &t = GetConfigureDb()->CurrentTransaction();

			int res = GetConfigureDb()->GetNumberResults();

			if(res > 0)
			{
				if(res == 1)
				{
					QString parent = t.Data1;
					QString samplePointName = t.Data2;

					QString unit_name = GetConfigureDb()->GetString("UNIT");

					QString cmd = "select UNITTYPE from UNITS where NAME='"+unit_name+"';";
					GetConfigureDb()->DoExec(this,cmd,tUnitType, parent, samplePointName);
				}
				else
				{
					QMessageBox::warning(this,tr("Command Failed"),tr("The sample point name is not unique in the TAGS configuration database table"));
				}
			}
			else
			{
				QMessageBox::warning(this,tr("Command Failed"),tr("No sample point name found"));
			}
		}
		break;
		case tUnitType:
		{
			QSTransaction &t = GetConfigureDb()->CurrentTransaction();

			if(GetConfigureDb()->GetNumberResults() > 0)
			{
				command_parent = t.Data1;
				command_samplePointName = t.Data2;
				command_unit_type = GetConfigureDb()->GetString("UNITTYPE");

				QTimer::singleShot(10,this,SLOT(DoCommandDialog()));
			}
		}
		break;
		default:
		break;
	};
};


void inspectPopupMenu::DoCommandDialog()
{
	//Send command trough dispatcher to monitor.exe
	//In monitor.exe call the ::Command method of the drive of unit_type
	//Note: dll drivers should be installed on the client PC, where ui.exe is running
	//in C:\scada\Drivers 

	QWidget* parent = (QWidget*) atol((const char*)command_parent);
	
	
	Driver *dp = FindDriver(command_unit_type);

	if(dp)
	{
		dp->CommandDlg(parent, command_samplePointName);
	}			
};

static inspectPopupMenu* g_m = NULL;

/*
*Function:
*Inputs:none
*Outputs:none
*Returns:none
*/
void InspectMenu(QWidget *parent, const QString &name, bool AckState)
{
	if(GetUserDetails().Name == QObject::tr(NONE_STR)) return;
	// 
	if(g_m)
	{
		delete g_m;
		g_m = NULL;
	}

	inspectPopupMenu* m = new inspectPopupMenu(parent);

	g_m = m;

	ScadaWindowsDict::iterator j =  inspect_windows.find(name);

	if(j == inspect_windows.end())
	{
		m->insertItem(QPixmap((const char **)inspect), QObject::tr("Inspect (Last 2 hours)..."),0); // open a real time device inspector
		m->insertItem(QPixmap((const char **)inspect), QObject::tr("Inspect (Last day)..."),1); // open a real time device inspector
		m->insertItem(QPixmap((const char **)inspect), QObject::tr("Inspect (Last 2 days)..."),2); // open a real time device inspector
		m->insertItem(QPixmap((const char **)inspect), QObject::tr("Inspect historical data..."),3); // open an historic device inspector
	}

	if(GetUserDetails ().privs & PRIVS_ACK_ALARMS)
	{
		m->insertItem(QPixmap((const char **)button),QObject::tr("Send command..."),5); // Send command
	}
	
	if(GetUserDetails ().privs &  PRIVS_ACK_ALARMS)
	{
		ScadaWindowsDict::iterator j =  acknowledge_alarm_windows.find(name);

		if(j == acknowledge_alarm_windows.end())
		{
			if(AckState)
			{
				m->insertItem(QPixmap((const char **)alm_xpm),QObject::tr("Acknowledge Alarm..."),4); // acknowledge alarm
			};
		}
	};

	//m->insertItem(QPixmap((const char **)quit_xpm),QObject::tr("Cancel"),-1);
	//
	switch(m->exec(QCursor::pos()))
	{
		case 0: // open a sample point inspector
		{
			ScadaWindowsDict::iterator j =  inspect_windows.find(name);
			if(j == inspect_windows.end())
			{
				// create the inspector for this sample point
				ScadaWindowsDict::value_type pr(name,(QObject*)Inspect::Create(name, (2 * 3600)));
				inspect_windows.insert(pr); // put in the dictionary
			}
		};
		break;
		case 1: // open a sample point inspector
		{
			ScadaWindowsDict::iterator j =  inspect_windows.find(name);
			if(j == inspect_windows.end())
			{

				// create the inspector for this sample point
				ScadaWindowsDict::value_type pr(name,(QObject*)Inspect::Create(name, (24 * 3600)));
				inspect_windows.insert(pr); // put in the dictionary
			}
		};
		break;
		case 2: // open a sample point inspector
		{
			ScadaWindowsDict::iterator j =  inspect_windows.find(name);
			if(j == inspect_windows.end())
			{
				// create the historic inspector for this sample point
				ScadaWindowsDict::value_type pr(name,(QObject*)Inspect::Create(name, (48 * 3600)));
				inspect_windows.insert(pr); // put in the dictionary
			}
		};
		break;
		case 3: // open a sample point historic inspector
		{
			if(GetHistoricResultDb() != NULL)
			{
				ScadaWindowsDict::iterator j =  inspect_windows.find(name);
				if(j == inspect_windows.end())
				{
					// create the inspector for this sample point. We show the last two hours
					ScadaWindowsDict::value_type pr(name,(QObject*)HistoricInspect::Create(name, (2 * 3600)));
					inspect_windows.insert(pr); // put in the dictionary
				}
			}
		};
		break;
		case 4: // acknowledge the alarm
		{
			ScadaWindowsDict::iterator j =  acknowledge_alarm_windows.find(name);

			if(j == acknowledge_alarm_windows.end())
			{
				AckAlarmDlg dlg(qApp->mainWidget());

				ScadaWindowsDict::value_type pr(name,(QObject*)&dlg);
				acknowledge_alarm_windows.insert(pr); // put in the dictionary

				dlg.SetHeading(QObject::tr("Acknowledge Alarm For Sample Point ") + name);
				//
				if(dlg.exec())
				{
					AckAlarm(name,dlg.GetComment());
				};

				ScadaWindowsDict::iterator jj =  acknowledge_alarm_windows.find(name);

				if(!(jj == acknowledge_alarm_windows.end()))
				{
					acknowledge_alarm_windows.erase(jj);
				}
			}
		};
		break;
		case 5: // send command to UNIT driver
		{
			m->get_unit(parent, name);
		};
		break;
		default:
		break;
	};
};

