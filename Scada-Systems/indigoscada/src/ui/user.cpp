/*
 *                         IndigoSCADA
 *
 *   This software and documentation are Copyright 2002 to 2012 Enscada 
 *   Limited and its licensees. All rights reserved. See file:
 *
 *                     $HOME/LICENSE 
 *
 *   for full copyright notice and license terms. 
 *
 */
/*
*Purpose: this is the user frame work. Most of the menu handling is done here
*This must be the top level widget
*/
#include "user.h"
// get the digital clock widget
#include "dclock.h"
#include "main.h"
#include "helpicon.xpm" 
#include "alarmdisplay.h"
#include "messages.h"
#include "statusdisplay.h"
#include "helpwindow.h"
#include "multitrace.h"
#include "realtime_browsedb.h"
#include "historic_browsedb.h"
//**************************************************************

// ******************** The dialogs we use *********************
// 
#include "ReportCfg.h"
#include "ReceipeCfg.h"
#include "BatchCfg.h"
#include "ReportSelect.h"
#include "ReportDir.h"
#include "ReceipeSelect.h"
#include "TimeFrameDlg.h"
#include "AckAlarmDlg.h"
#include "SelectPrinterDlg.h"
#include "printrep.h"
#include "rintro.h"
// 
//******************loggen support********************************
#include "loggen.h"
//******************repgen support********************************
#include "repgen.h"
//****************************************************************
#include "IndentedTrace.h"
#include "utilities.h"
//****************************************************************
//designer .ui widgets
#include <qwidgetfactory.h>
#include "hmi_mng.h"

//xpm
#include "scada.xpm"
#include "alarmsreport.xpm"
#include "batches.xpm"
#include "configreport.xpm"
#include "eventsreport.xpm"
#include "auditsreport.xpm"
#include "makereport.xpm"
#include "printreport.xpm"
#include "reviewreport.xpm"
#include "pentrace.xpm"
#include "server.xpm"
#include "../monitor/monitor.xpm"
#include "sysmgr.xpm"
#include "receipe.xpm"
//#include "keygold.xpm"
//#include "keysilver.xpm"
#include "unlock.xpm"
#include "open.xpm"
#include "action_exit.xpm"
#include "pencil.xpm"
#include "userInfo.xpm"
#include "info.xpm"
#include "logout.xpm"
#include "start.xpm"
#include "quit.xpm"

//suppliers logi
#include "logo_enscada_30.xpm"
#include "logo_blank_30.xpm"

//customer logi

//*****************************************************************

// interval between monitor ticks 
//#define MONITOR_TICK_TIME 2000 //ms
#define MONITOR_TICK_TIME 100 //ms

/*-Function:UserFrameWork
*Inputs:none
*Outputs:none
*Returns:none
*/																	
UserFrameWork::UserFrameWork():pToolBar(0),pToolBarLogo(0),pSys(0),translation(0),fInLogin(0),
MonitorRunning(true),MonitorTimeout(MONITOR_TICK_TIME),DataUITimeout(10),
InitialMemory(0.0),fRequestFetch(0),pMessage(0),prtrep(NULL),firmAddress(QString::null),iscadaAddress(QString::null),
logrepgen(NULL),repgen(NULL),intro(NULL),
MaxRetryReconnectToDispatcher(0),
MaxRetryReconnectToRealTimeDb(0),
MaxRetryReconnectToHistoricDb(0),
MaxRetryReconnectToSpareDispatcher(0),
MaxRetryReconnectToSpareRealTimeDb(0),
statusBar_state(WHITE_STATE)
{
	IT_IT("UserFrameWork::UserFrameWork");
	
	// 
	pMessage = 0;
	AutoLogoffTime = 0; // auto log off by default
	AutoLogOffCount = 0;
	//
	pLogoffTimer = new QTimer(this);
	connect (pLogoffTimer, SIGNAL (timeout ()), this, SLOT (AutoLogOut()));
	{
		QTimer *pTimer = new QTimer(this);
		connect (pTimer, SIGNAL (timeout ()), this, SLOT(Tick()));
		pTimer->start(100);
	};
	//
	menuBar()->clear();
	//	
	statusBar()->clear(); // create a status bar
	//  
	//
	move(20,20);
	resize(700,400); // start with a sensible size
	//
	setCaption(tr(SYSTEM_NAME_OPERATOR));
	//
	//
	// We want to listen to CURRENT and ALARM notifications
	// the frame work does not do much but the various client windows
	// will handle these
	// 
	connect (GetResultDb (),
	SIGNAL (TransactionDone (QObject *, const QString &, int, QObject*)), this,
	SLOT (QueryResponse (QObject *, const QString &, int, QObject*)));	// connect to the database
	//
	connect (GetCurrentDb (),
	SIGNAL (TransactionDone (QObject *, const QString &, int, QObject*)), this,
	SLOT (QueryResponse (QObject *, const QString &, int, QObject*)));	// connect to the database
	//
	connect (GetDispatcher (),
	SIGNAL (ReceivedNotify(int, const char *)), this,
	SLOT (ReceivedNotify(int, const char *)));	
	//
	connect (GetConfigureDb (),
	SIGNAL (TransactionDone (QObject *, const QString &, int, QObject*)), this,
	SLOT (QueryResponse (QObject *, const QString &, int, QObject*)));	// connect to the database
	//
	QString cmd = "select IKEY,DVAL from PROPS where SKEY='System';"; // get the system properties
	GetConfigureDb ()->DoExec (this, cmd, tGet);	// make the request
	//
	cmd = "select IKEY,DVAL from PROPS where SKEY='Semaphore';"; // get the hardware Semaphore properties
	GetConfigureDb ()->DoExec (this, cmd, tSemaphore);	// make the request
	//
	QPixmap icon((const char **)scada);
	setIcon(icon);
	//
	statusBar_state = WHITE_STATE;
	statusBar()->setBackgroundColor(Qt::white);
	statusBar()->message(tr(SYSTEM_NAME_STARTING));
	statusBar()->setSizeGripEnabled(false);
	//
	GetUserDetails().privs = 0;
	GetUserDetails().Name = tr(NONE_STR);
	//
	LastSeqNo = "0";
	LastFetchNo = "0";
	//
	iscadaAddress = SUPPLIER_NAME_LONG;
	firmAddress = CUSTOMER_NAME_LONG + QString("\n");

};
/*-Function:~UserFrameWork
*Inputs:none
*Outputs:none
*Returns:none
*/
UserFrameWork::~UserFrameWork()
{
	IT_IT("UserFrameWork::~UserFrameWork");

//	for(HMIDict::iterator it = hmi_database.begin(); !(it == hmi_database.end()); it++)
///	{
///		delete ((*it).second);
//	}
};
/*
*Function:void AutoLogOut(); // automatic logout 
* handles automatic log out after a period of inactivity
*Inputs:none
*Outputs:none
*Returns:none
*/
void UserFrameWork::AutoLogOut() // automatic logout 
{
	IT_IT("UserFrameWork::AutoLogOut");
	
	if(QSApplication::fKeyMouseEvent)
	{
		AutoLogOffCount--;
		if(AutoLogOffCount < 1)
		{
			//
			QSApplication::fKeyMouseEvent = 0; // acts a sentinal
			//
			// unstack any open dialogs or popups 
			// 
			QWidget *w;
			QKeyEvent kd(QEvent::KeyPress,1,27,0); // post an escape character - this will close the menu with a cancel exit code
			//
			//  << "Trying to Auto Close Popups" << endl;
			// close all popup widgets
			while((w = QApplication::activePopupWidget()) != 0)
			{
				w->close(FALSE);
				qApp->processEvents(100);
			};
			//
			// close all dialogs
			//cerr << "Trying to Auto Close Dialogs" << endl;
			while((w = QApplication::activeModalWidget()) != 0)
			{
				w->close(FALSE);
				qApp->processEvents(100);
			};
			//   
			qApp->processEvents(100);
			//cerr << "Trying to Auto Close System Manager" << endl;
			//
			// we cannot close the system manager here - a bit too tangled
			//if(SysMgrFrameWork::Active && pSys)
			//{
			//	QTimer::singleShot(50,pSys,SLOT(CloseSysMgr()));
			//	pSys = 0;
			//};
			QTimer::singleShot(150,this,SLOT(Logout()));
			QSLogEvent(GetUserDetails().Name,tr("Auto Logged Off"));
		};
	}
	else
	{
		QSApplication::fKeyMouseEvent = 1; // arm it
		AutoLogOffCount = AutoLogoffTime;
	};
};
/*
*Function: Logout
*clear the user details and goto login
*Inputs:none
*Outputs:none
*Returns:none
*/
void UserFrameWork::Logout()
{
	IT_IT("UserFrameWork::Logout");
	
	GetDispatcher()->DoExec(NotificationEvent::CMD_LOGOUT); //apa+++ 24-07-2012
	//
	pLogoffTimer->stop(); // halt the auto log 
	AutoLogOffCount = AutoLogoffTime;
	//
	if(GetUserDetails().Name != tr(NONE_STR))
	{
	#ifdef WIN32

		char computer_name[ MAX_PATH ];
		ZeroMemory( computer_name, sizeof( computer_name ) );
		DWORD size = MAX_PATH;
		::GetComputerName( computer_name, &size );

		QString msg = GetUserDetails().Name + tr(" has logged off computer ")
		+ QString(computer_name) + QString(" - ") + QString(winver());
		QSLogEvent("HMI",msg);

		msg = QString(""SYSTEM_NAME"") + QString(" - ")
		#ifdef NDEBUG //Release build
		+ QString("Release build ");
		#else
		+ QString("Debug build ");
		#endif
		//+ tr("Build Date:") + QString(GetScadaDateBuild()) + " " + QString(GetScadaTimeBuild());
			QSLogEvent("HMI",msg);
	#else
		char hn[32];
		hn[0] = 0;
		//
		gethostname(hn,sizeof(hn));

		QString msg = GetUserDetails().Name + tr(" has logged off computer ") + QString(hn) + " display=" + QString(getenv("DISPLAY"));
		QSLogEvent("HMI",msg);
	#endif

	}

	GetUserDetails().privs = 0;
	GetUserDetails().Name = tr(NONE_STR);
	menuBar()->clear(); // remove the menu
	QPopupMenu *file = new QPopupMenu(this);
	//
	file->insertItem(QPixmap((const char **)start_xpm),tr("&Login..."),this,SLOT(Login()));
	//
	if(GetUserDetails().privs & PRIVS_CAN_EXIT)
	{ 
		file->insertItem(QPixmap((const char **)quit_xpm),tr("E&xit"),qApp,SLOT(quit()),CTRL+Key_X); // does the user have the priviledge to exit
	}
	//
	menuBar()->insertItem(QString(tr("&File")),file);
	//
	#ifdef USE_STATUS_TAB
	if(centralWidget())
	{
		centralWidget()->show();
		((QTabWidget *)centralWidget())->showPage(pStatus);
		pStatus->setFocus();
	}
	#endif
	//
	if(pToolBar)
	{
		removeToolBar(pToolBar); // clear the tool bar
		delete pToolBar;
	}

	pToolBar = new QToolBar("UserToolBar",this,QMainWindow::Top);
	//show only the firm logo
	pToolBar->setGeometry(0, 0, 0, 0);
	pToolBar->setVerticalStretchable(false);
	pToolBar->setHorizontalStretchable(false);
	//pToolBar->setFixedWidth(300);
	QLabel* logo = new QLabel(pToolBar,"logo");
	logo->setPixmap(LOGO_SUPPLIER);
	QToolTip::add(logo, iscadaAddress);
	//logo->setFixedSize(150,30);

	QLabel* customer_logo = new QLabel(pToolBar,"customer_logo");
	customer_logo->setPixmap(LOGO_CUSTOMER);
		
	QToolTip::add(customer_logo, firmAddress);
	//customer_logo->setFixedSize(150,30);

	addToolBar(pToolBar);
	pToolBar->raise();
	pToolBar->show();

	//if(SysMgrFrameWork::Active && pSys)
	//{
	//	QTimer::singleShot(50,pSys,SLOT(CloseSysMgr()));
	//	pSys = 0;
	//}
	//
	Login();
};
/*
*Function:SetTabs
*set up the tabs
*Inputs:none
*Outputs:none
*Returns:none
*/

static HMIDict	hmi_database;
#define MAP_FILE_EXT		"ui"

void UserFrameWork::SetTabs()
{
	IT_IT("UserFrameWork::SetTabs");
	
	//
	if(!pMessage)
	{
		setCentralWidget(new QTabWidget(this)); // create the work area	
		        
		pAlarms = new AlarmDisplay(centralWidget());
		((QTabWidget *)centralWidget())->addTab(pAlarms,tr("&Alarms"));
		
		#ifdef USE_STATUS_TAB
		pStatus = new StatusDisplay(centralWidget());
		((QTabWidget *)centralWidget())->addTab(pStatus,tr("&Status"));
		#endif

		pMessage = new MessageDisplay(centralWidget()); // we need a trace / message window from the off
		((QTabWidget *)centralWidget())->addTab(pMessage,tr("&Messages"));

		///////////////Create an HMI dictionary//////////////////////////////////
		//Dinamic dialog loading from .ui designer file//////////////////////////////////////////
		char library_path[MAX_PATH];
		strcpy(library_path, (const char*)GetScadaHomeDirectory());
		strcat(library_path,"\\plugins");
		QApplication::addLibraryPath(library_path); //apa+++ 21-07-2012 So we can load qwtplugin.dll from directory C:\scada\plugins\designer
				
		QString hmi_index;
		
		///////////////find .ui files//////////////////////
		char buffer01[_MAX_PATH];
		char buffer02[_MAX_PATH];
		HANDLE HRicerca;
		WIN32_FIND_DATA DescrittoreF;
		char file_ext[50];
		int rt;

		strcpy(buffer01, (const char*)GetScadaHomeDirectory());
		strcat(buffer01, "\\project\\*.*");
		
		HRicerca = FindFirstFile(buffer01, &DescrittoreF);

		if(HRicerca == INVALID_HANDLE_VALUE)
		{
			
		}
		else
		{
			for(; rt = FindNextFile(HRicerca, &DescrittoreF); )
			{
				if(rt)
				{
					if((DescrittoreF.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != FILE_ATTRIBUTE_DIRECTORY)
					{
						strcpy(buffer02, (const char*)GetScadaHomeDirectory());
						strcat(buffer02, "\\");
						strcat(buffer02, DescrittoreF.cFileName);

						char *pdest = strrchr(buffer02, '.');

						if(pdest)
						{
							strncpy(file_ext, pdest + 1, 4);
							file_ext[4] = '\0';

							if(!strcmp(file_ext, MAP_FILE_EXT))
							{
								hmi_index = QString(DescrittoreF.cFileName);	
								HMIDict::value_type pr(hmi_index, new HMI_manager);
								hmi_database.insert(pr); // put in the dictionary					
							}
						}
					}
				}
			}

			FindClose(HRicerca);
		}
		///////////////////////////////////////////////////
				
		HMI_manager     *hmi_mng;
						
		for(HMIDict::iterator it = hmi_database.begin(); !(it == hmi_database.end()); it++)
		{
			hmi_mng = (*it).second;
		
			QWidgetFactory::loadImages("../Bitmaps");
			QString map_path = QString("../project/") + (*it).first;
			QDialog *designerHMI = (QDialog *)QWidgetFactory::create(map_path, hmi_mng);

			if(designerHMI)
			{
				hmi_mng->setParent(designerHMI); 

				((QTabWidget *)centralWidget())->addTab(designerHMI, (*it).first);

				QSLogEvent("HMI", QString((*it).first) + QString(" displayed"));
			}
			else
			{
				QSLogEvent("HMI", QString((*it).first) + QString(" not displayed"));
			}
		}
		//////////////////////////////////////////////////////////////////////////////////////////
		//
		centralWidget()->show();
		((QTabWidget *)centralWidget())->showPage(pAlarms);
		pAlarms->setFocus();
		pAlarms->show();
		//
		fRequestFetch = true;
		showMaximized();
		//QSMessage("Starting");
	};
};
/*-Function: Login
*show the Login screen 
*Inputs:none
*Outputs:none
*Returns:none
*/
#include "LoginDlg.h"
#include "general_defines.h"

///////start apa added on 15-07-2012///////////////////////////////////
//
// ***************** dialogs *************************
// 
#include "AlarmGroupCfg.h"
#include "BatchCfg.h"
#include "ReceipeCfg.h"
#include "ReportCfg.h"
#include "SampleCfg.h"
#include "ScheduleCfg.h"
#include "SerialCfg.h"
#include "SystemCfg.h"
#include "TagCfg.h"
#include "UnitCfg.h"
#include "UserCfgDlg.h"
#include "driver.h"
//xpm
//#include "fileopen.xpm"
//#include "filesave.xpm"
#include "datasourcesuser.xpm"
#include "driver.xpm"
#include "computergreen.xpm"
#include "column.xpm"
#include "connecttool.xpm"
///////end apa added on 15-07-2012///////////////////////////////////
 
void UserFrameWork::Login()
{
	IT_IT("UserFrameWork::Login");

	//
	if(!fInLogin)
	{
		fInLogin = true;
		SetTabs(); // set up the screens
		//
		LoginDlg dlg(this);
		if(dlg.exec())
		{
			GetDispatcher()->DoExec(NotificationEvent::CMD_LOGON); //apa+++ 24-07-2012

			// on success we load up the full menu based on the user' s privileges
			removeToolBar(pToolBar);
			delete pToolBar;
			pToolBar = new QToolBar("UserToolBar",this,QMainWindow::Top);
			//
			menuBar()->clear();
			QPopupMenu *file = new QPopupMenu(this);
			file->insertItem(QPixmap((const char **)logout_xpm),tr("Logout..."),this,SLOT(Logout()));
			file->insertSeparator();
			file->insertItem(QPixmap((const char **)pencil_xpm),tr("User Entry..."),this,SLOT(UserEntry()));
			file->insertSeparator();
			//
			if(GetUserDetails().privs & PRIVS_CAN_EXIT)
			{ 
				file->insertItem(QPixmap((const char **)quit_xpm),tr("E&xit"),qApp,SLOT(quit()),CTRL+Key_X); // does the user have the priviledge to exit
				file->insertItem(QPixmap((const char **)action_exit),tr("Exit &all clients and monitor"),this,SLOT(closeAll()),CTRL+Key_A); // close all programs
			};
			//
			menuBar()->insertItem(QString(tr("&File")),file);
			//
			//
			// ***************************************************************************** 
			if(GetUserDetails().privs & PRIVS_REPORTS)
			{
				QPopupMenu *reportMenu = new QPopupMenu(this);
				reportMenu->insertItem(QPixmap((const char **)makereport),tr("&Make..."),this,SLOT(makeReport()));
				reportMenu->insertItem(QPixmap((const char **)reviewreport),tr("&Review..."),this,SLOT(reviewReport()));
				reportMenu->insertItem(QPixmap((const char **)printreport),tr("&Print..."),this,SLOT(printReport()));
				reportMenu->insertItem(QPixmap((const char **)alarmsreport),tr("&Alarms Report..."),this,SLOT(alarmsReport()));
				reportMenu->insertItem(QPixmap((const char **)eventsreport),tr("&Events Report..."),this,SLOT(eventsReport()));
				//
				if(GetUserDetails().privs == PRIVS_ALL)
				{
					reportMenu->insertItem(QPixmap((const char **)auditsreport),tr("A&udit Report..."),this,SLOT(AuditReport()));
				}

				reportMenu->insertItem(QPixmap((const char **)configreport),tr("&Report Configure"),this,SLOT(configureReport()));
				//		
				reportMenu->insertSeparator();
				reportMenu->insertItem(QPixmap((const char **)Batches),tr("&Batch Editor..."),this,SLOT(batches()));
				reportMenu->insertSeparator();
				reportMenu->insertItem(QPixmap((const char **)pentrace),tr("&Pen Trace..."),this,SLOT(penTrace()));
								
				menuBar()->insertItem(tr("&Reports"),reportMenu);	
			};
			//
			// *****************************************************************************
			//
			// add the tool bar bits
			if(GetUserDetails().privs & PRIVS_REPORTS)
			{
				removeToolBar(pToolBar);
				delete pToolBar;
				pToolBar = new QToolBar("Customer-Supplier",this,QMainWindow::Top);
				pToolBar->setVerticalStretchable(false);
				pToolBar->setHorizontalStretchable(false);
				(void)  new QToolButton(QPixmap((const char **)configreport),tr("Report Configure"),0, this,SLOT(configureReport()),pToolBar, "Report Configure");
				(void)  new QToolButton(QPixmap((const char **)makereport),tr("Report Make"),0,	this,SLOT(makeReport()),pToolBar, "Report Design");
				(void)  new QToolButton(QPixmap((const char **)reviewreport),tr("Report Review"),0,	this,SLOT(reviewReport()),pToolBar, "Report Review");
				(void)  new QToolButton(QPixmap((const char **)printreport),tr("Report Print"),0, this,SLOT(printReport()),pToolBar, "Report Print");
				(void)  new QToolButton(QPixmap((const char **)alarmsreport),tr("Report Alarm"),0, this,SLOT(alarmsReport()),pToolBar, "Report Alarms");
				(void)  new QToolButton(QPixmap((const char **)eventsreport),tr("Report Events"),0,	this,SLOT(eventsReport()),pToolBar, "Report Events");
				(void)  new QToolButton(QPixmap((const char **)Batches),tr("Edit Batches"),0, this,SLOT(batches()),pToolBar, "Edit Batches");
			};
			//
			// System Administration Functions
			//
			if(GetUserDetails().privs == PRIVS_ALL)
			{
                // add the various configuration options
	            QPopupMenu *configMenu = new QPopupMenu(this);
	            configMenu->insertItem(QPixmap((const char **)computergreen_xpm),tr("&Configure System..."),this,SLOT(configureSystem()));
	            configMenu->insertItem(QPixmap((const char **)datasourcesuser_xpm),tr("&Configure Users..."),this,SLOT(configureUser()));
	            configMenu->insertItem(QPixmap((const char **)driver_xpm),tr("&Configure Units..."),this,SLOT(configureUnits()));
	            configMenu->insertItem(QPixmap((const char **)column_xpm),tr("&Configure Sample Points..."),this,SLOT(configureSamplePoints()));
	            //configMenu->insertItem(QPixmap((const char **)receipe_xpm),tr("&Configure Receipes..."),this,SLOT(configureReceipes()));
	            configMenu->insertItem(QPixmap((const char **)eventsreport),tr("&Configure Scheduled Events..."),this,SLOT(configureSchedule()));
	            //configMenu->insertItem(QPixmap((const char **)magick),tr("&Configure Serial Ports..."),this,SLOT(configurePorts()));
	            configMenu->insertItem(QPixmap((const char **)alarmsreport),tr("&Configure Alarm Groups..."),this,SLOT(configureAlarmGroups()));
	            //configMenu->insertItem(QPixmap((const char **)configreport),tr("&Configure Report..."),this,SLOT(configureReport()));

				configMenu->insertItem(QPixmap((const char **)scada),tr("&Configure HMI ..."),this,SLOT(configureHMI()));

				configMenu->insertItem(QPixmap((const char **)scada),tr("&Configure Protocol ..."),this,SLOT(configureProtocol()));
				
	            menuBar()->insertItem(tr("&Configure"),configMenu);

				QPopupMenu *control = new QPopupMenu(this);
				//control->insertItem(QPixmap((const char **)sysmgr), tr("&System Manager..."),this,SLOT(systemManager()));
				//control->insertSeparator();
				control->insertItem(QPixmap((const char **)monitor),tr("&Restart Monitor..."),this,SLOT(restart()));
				control->insertSeparator();
                //Receipes are NOT working...
				//control->insertItem(QPixmap((const char **)receipe_xpm),tr("&Configure Receipes..."),this,SLOT(configureReceipes()));
				//control->insertItem(QPixmap((const char **)open_xpm),tr("&Load Receipe..."),this,SLOT(loadReceipe()));
				//control->insertSeparator();
				//Database management...

				if(GetConfigureDb()->Ok() && GetResultDb()->Ok() && GetCurrentDb()->Ok())
				{
					control->insertItem(QPixmap((const char **)server_xpm),tr("&Real time database management..."),this,SLOT(realtimeDbManagement()));
				}

				if((GetSpareConfigureDb() != NULL) && (GetSpareCurrentDb() != NULL)&&(GetSpareResultDb() != NULL))
				{
					if(GetSpareConfigureDb()->Ok() && GetSpareResultDb()->Ok() && GetSpareCurrentDb()->Ok())
					{
						control->insertItem(QPixmap((const char **)server_xpm),tr("&Spare real time database management..."),this,SLOT(spareRealtimeDbManagement()));
					}
				}

				if(GetHistoricResultDb() != NULL)
				{
					if(GetHistoricResultDb()->Ok())
					{
						control->insertItem(QPixmap((const char **)server_xpm),tr("&Historical database management..."),this,SLOT(historicDbManagement()));
					}
				}

				control->insertSeparator();

				//control->insertItem(QPixmap((const char **)unlock_xpm),tr("&Clear System Manager Lock..."),this,SLOT(ClearSystemMgrLock()));
				menuBar()->insertItem(tr("System C&ontrol"),control);
			}
			else if(GetUserDetails().privs & PRIVS_FACTORY_ADMIN)
			{
				QPopupMenu *control = new QPopupMenu(this);
				
				control->insertItem(QPixmap((const char **)monitor),tr("&Restart Monitor..."),this,SLOT(restart()));
				//control->insertSeparator();
				//control->insertItem(QPixmap((const char **)receipe_xpm),tr("&Configure Receipes..."),this,SLOT(configureReceipes()));
				//control->insertSeparator();
				//control->insertItem(QPixmap((const char **)open_xpm),tr("&Load Receipe..."),this,SLOT(loadReceipe()));
				menuBar()->insertItem(tr("System C&ontrol"),control);
			}

			//
			// Acknowledge alarms
			//
			if(GetUserDetails().privs & PRIVS_ACK_ALARMS)
			{
				#include "ackall.xpm"
				pToolBar->addSeparator();
				(void)  new QToolButton(QPixmap((const char **)ackall),tr("Acknowledge All Alarms"),0, this,SLOT(ackAllAlarms()),pToolBar, "Ack. All Alarms");

			};
			//
			// add the help option
			{
				pToolBar->addSeparator();
				(void)  new QToolButton(QPixmap((const char **)helpicon),tr("Help"),0, this,SLOT(Help()),pToolBar, "Help");
			};
			QPopupMenu *help = new QPopupMenu(this);
			help->insertItem(QPixmap((const char **)helpicon), tr("Help..."),this,SLOT(Help()));
			help->insertItem(QPixmap((const char **)info_xpm),tr("About..."),this,SLOT(About()));
			menuBar()->insertItem(tr("&Help"),help); 
			//
			DigitalClock *pL = new DigitalClock(pToolBar); // Add the digital Clock to the tool bar
			//pToolBar->setStretchableWidget(pL);
			//
			//logos
			pToolBar->addSeparator();
			QLabel* logo = new QLabel(pToolBar,"logo");
			logo->setPixmap(LOGO_SUPPLIER);
			QToolTip::add(logo, iscadaAddress);
			pToolBar->addSeparator();

			QLabel* customer_logo = new QLabel(pToolBar,"customer_logo");
			customer_logo->setPixmap(LOGO_CUSTOMER);
			QToolTip::add(customer_logo, firmAddress);
			//end logos

			addToolBar(pToolBar);
			setRightJustification(TRUE);
			//   
			pToolBar->raise();
			pToolBar->show();
			//
			if(AutoLogoffTime > 0)
			{
				pLogoffTimer->start(60000); // auto log off timer - set to 1 minute ticks
				QSApplication::fKeyMouseEvent = 1; 
				AutoLogOffCount = AutoLogoffTime;
			};
		};
		//
		// if we fail then the user can exit / login from the menu
		// 
		fInLogin = 0;
		//
	};
};

/*
*Function: configureSystem
*system configuration dialog
*Inputs:none
*Outputs:none
*Returns:none
*/
void UserFrameWork::configureSystem()
{
	IT_IT("UserFrameWork::configureSystem");
	
	SystemCfg dlg(this);
	dlg.exec();
};
/*
*Function: configureUnits
*configure units
*Inputs:none
*Outputs:none
*Returns:none
*/
void UserFrameWork::configureUnits()
{
	IT_IT("UserFrameWork::configureUnits");
	
	UnitCfg dlg(this);
	dlg.exec();
};
/*
*Function: configure sample points
*Inputs:none
*Outputs:none
*Returns:none
*/
void UserFrameWork::configureSamplePoints()
{
	IT_IT("UserFrameWork::configureSamplePoints");
	
	SampleCfg dlg(this);
	dlg.exec();
};

/*
*Function: configureSchedule
*Inputs:none
*Outputs:none
*Returns:none
*/
void UserFrameWork::configureSchedule()
{
	IT_IT("UserFrameWork::configureSchedule");
	
	ScheduleCfg dlg(this);
	dlg.exec();
};
/*
*Function: configurePorts
*Inputs:none
*Outputs:none
*Returns:none
*/
void UserFrameWork::configurePorts()
{
	IT_IT("UserFrameWork::configurePorts");
	
	SerialCfg dlg(this);
	dlg.exec();
};

/*
*Function:configureAlarmGroups
*Inputs:none
*Outputs:none
*Returns:none
*/
void UserFrameWork::configureAlarmGroups()
{
	IT_IT("UserFrameWork::configureAlarmGroups");
	
	AlarmGroupCfg dlg(this);
	dlg.exec();
};

/*
*Function:configureHMI
*Inputs:none
*Outputs:none
*Returns:none
*/
void UserFrameWork::configureHMI()
{
	IT_IT("UserFrameWork::configureHMI");

	qApp->processEvents();
	QString path;

	#ifdef UNIX
	//QString cmd = QSBIN_DIR + "/helpviewer " + QSREPORT_DIR + "/" + dlg.List->currentText() + "/index.html&";
	system((const char *)cmd);
	#endif

	#ifdef WIN32
	QString cmd = GetScadaHomeDirectory() + "\\bin\\hmi_designer.exe";
	
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

	#endif
};


/*
*Function:configureProtocol
*Inputs:none
*Outputs:none
*Returns:none
*/
void UserFrameWork::configureProtocol()
{
	IT_IT("UserFrameWork::configureProtocol");

	qApp->processEvents();
	QString path;

	#ifdef UNIX
	//QString cmd = QSBIN_DIR + "/helpviewer " + QSREPORT_DIR + "/" + dlg.List->currentText() + "/index.html&";
	system((const char *)cmd);
	#endif

	#ifdef WIN32
	QString cmd = GetScadaHomeDirectory() + "\\bin\\protocol_configurator.exe";
	
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

	#endif
};

/*
*Function:configureUser
*configure users
*Inputs:none
*Outputs:none
*Returns:none
*/
void UserFrameWork::configureUser()
{
	IT_IT("UserFrameWork::configureUser");
	
	UserCfgDlg dlg(this);
	dlg.exec();
};

/*
*Function: ClearSystemMgrLock()
*clear the system manager's lock flag
*Inputs:none
*Outputs:none
*Returns:none
*/
void UserFrameWork::ClearSystemMgrLock()
{
	IT_IT("UserFrameWork::ClearSystemMgrLock");
	
	if(!YESNO(tr("Clear System Manager Lock"),tr("Clear The Lock - Are You Sure?")))
	{
		GetCurrentDb()->DoExec(0,"update PROPS set DVAL='0' where SKEY='System' and IKEY='Lock';",0);
	};
};


/*-Function: makeReport
*Inputs:none
*Outputs:none
*Returns:none
*/
void UserFrameWork::makeReport() // generate a report
{
	IT_IT("UserFrameWork::makeReport");
	
	ReportSelect dlg(this);
	if(dlg.exec())
	{
		if(!dlg.List->currentText().isEmpty())
		{
			qApp->processEvents();
			// invoke the report generator and then the viewer 
			QString r = dlg.List->currentText();

			#ifdef UNIX
			//
			// we must specify the X-server to use incase we are running from a remote client
			// 
			QString cmd = QSBIN_DIR + "/repgen -display localhost:0 ";
			for(int i = 1; i < 7; i++)
			{
				cmd +=  QString(" \"") + qApp->argv()[i] + "\" ";  
			};
			//
			// add the report name
			cmd += r;
			//
			// add the report viewer startup and put the whole lot out as a detached task
			// Win32 ports will have to do this differently as non-console tasks detach immediately
			// 
			cmd += "; " + QSBIN_DIR + "/helpviewer " + QSREPORT_DIR + "/" + r + "/index.html&";
			//
			// spawn it
			system((const char *)cmd); 
			//
			#endif

			#ifdef WIN32
			if(repgen) delete repgen;
			repgen = new ReportGenerator(r);
			#endif //WIN32
		}
	}
}

/*-Function: reviewReport
*Inputs:none
*Outputs:none
*Returns:none
*/
void UserFrameWork::reviewReport() // review an existing report
{
	IT_IT("UserFrameWork::reviewReport");

	ReportDir dlg(this);

	if(dlg.exec())
	{
		if(!dlg.List->currentText().isEmpty())
		{
			qApp->processEvents();

			QString ini_file = GetScadaHomeDirectory() + "\\project\\scada.ini";
			Inifile iniFile((const char*)ini_file);

			if(iniFile.find("path","internet_browser"))
			{
				QString path;
				path = iniFile.find("path","internet_browser");

				#ifdef UNIX
				QString cmd = QSBIN_DIR + "/helpviewer " + QSREPORT_DIR + "/" + dlg.List->currentText() + "/index.html&";
				system((const char *)cmd);
				#endif

				#ifdef WIN32
				QString cmd = path + " " + QSREPORT_DIR + "\\" + dlg.List->currentText() + "\\index.html";
				
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

				#endif

	//			QString home = QSREPORT_DIR + "\\" + dlg.List->currentText() + "\\index.html";
	//			HelpWindow* helpw = new HelpWindow(home, QSREPORT_DIR, 0, QObject::tr("help viewer"));
	//			helpw->show();
	//			QObject::connect( helpw, SIGNAL(lastWindowClosed()), helpw, SLOT(quit()) );

			}
		};
	};
};
/*-Function: alarmsReport
*Inputs:none
*Outputs:none
*Returns:none
*/
void UserFrameWork::alarmsReport() // generate an alarms report
{
	IT_IT("UserFrameWork::alarmsReport");
	
	TimeFrameDlg dlg(this);
	dlg.setCaption(tr("Select Alarm Report Time Frame"));
	if(dlg.exec())
	{
		#ifdef UNIX
		// spawn off the alarm log report generator
		QString cmd = QSBIN_DIR + "/loggen ";
		cmd += " \"" + QString(qApp->argv()[1]) + "\"";
		cmd += " \"" + QString(qApp->argv()[3]) + "\"";
		cmd += " \"" + QString(qApp->argv()[5]) + "\"";
		cmd += " \"" + QString(qApp->argv()[6]) + "\"";
		cmd += " ALARMS ";
		// add the time frame
		cmd += " \"" + dlg.FromDate->text() + " " + dlg.FromTime->text() + "\"";
		cmd += " \"" + dlg.ToDate->text() + " " + dlg.ToTime->text() + "\"";
		cmd += "; " + QSBIN_DIR + "/helpviewer " + QSREPORT_DIR + "/ALARMS.html&";
		//
		// spawn it
		system((const char *)cmd); 
		#endif

		#ifdef WIN32
		QString From_cmd = dlg.FromDate->text() + " " + dlg.FromTime->text();
		QString To_cmd = dlg.ToDate->text() + " " + dlg.ToTime->text();
		if(logrepgen) delete logrepgen;
		logrepgen = new LogReportGenerator("ALARMS", From_cmd, To_cmd);
		#endif //WIN32
	}
};
/*-Function: AuditReport
*Inputs:none
*Outputs:none
*Returns:none
*/
void UserFrameWork::AuditReport() // generate an alarms report
{
	IT_IT("UserFrameWork::AuditReport");
	
	TimeFrameDlg dlg(this);
	dlg.setCaption(tr("Select Audit Report Time Frame"));
	if(dlg.exec())
	{
		#ifdef UNIX
		// spawn off the alarm log report generator
		QString cmd = QSBIN_DIR + "/loggen -display localhost:0 ";
		cmd += " \"" + QString(qApp->argv()[1]) + "\"";
		cmd += " \"" + QString(qApp->argv()[3]) + "\"";
		cmd += " \"" + QString(qApp->argv()[5]) + "\"";
		cmd += " \"" + QString(qApp->argv()[6]) + "\"";
		cmd += " AUDIT ";
		// add the time frame
		cmd += " \"" + dlg.FromDate->text() + " " + dlg.FromTime->text() + "\"";
		cmd += " \"" + dlg.ToDate->text() + " " + dlg.ToTime->text() + "\"";
		// add the report viewer startup and put the whole lot out as a detached task
		// Win32 ports will have to do this differently as non-console tasks detach immediately
		cmd += "; " + QSBIN_DIR + "/helpviewer " + QSREPORT_DIR + "/AUDIT.html&";
		//
		// spawn it
		qApp->processEvents();
		system((const char *)cmd); 
		#endif

		#ifdef WIN32
		QString From_cmd = dlg.FromDate->text() + " " + dlg.FromTime->text();
		QString To_cmd = dlg.ToDate->text() + " " + dlg.ToTime->text();
		if(logrepgen) delete logrepgen;
		logrepgen = new LogReportGenerator("AUDIT", From_cmd, To_cmd);
		#endif //WIN32
	}
}
/*-Function:eventsReport
*Inputs:none
*Outputs:none
*Returns:none
*/
void UserFrameWork::eventsReport() // generate an events report
{
	IT_IT("UserFrameWork::eventsReport");
	
	TimeFrameDlg dlg(this);
	dlg.setCaption(tr("Select Events Report Time Frame"));
	if(dlg.exec())
	{
		#ifdef UNIX
		// spawn off the event log report generator
		// spawn off the alarm log report generator
		QString cmd = QSBIN_DIR + "/loggen -display localhost:0 ";
		cmd += " \"" + QString(qApp->argv()[1]) + "\"";
		cmd += " \"" + QString(qApp->argv()[3]) + "\"";
		cmd += " \"" + QString(qApp->argv()[5]) + "\"";
		cmd += " \"" + QString(qApp->argv()[6]) + "\"";
		cmd += " EVENTS ";
		// add the time frame
		cmd += " \"" + dlg.FromDate->text() + " " + dlg.FromTime->text() + "\"";
		cmd += " \"" + dlg.ToDate->text() + " " + dlg.ToTime->text() + "\"";
		// add the report viewer startup and put the whole lot out as a detached task
		// Win32 ports will have to do this differently as non-console tasks detach immediately
		cmd += "; " + QSBIN_DIR + "/helpviewer " + QSREPORT_DIR + "/EVENTS.html&";
		//
		// spawn it
		qApp->processEvents();
		system((const char *)cmd); 

		#endif

		#ifdef WIN32
		QString From_cmd = dlg.FromDate->text() + " " + dlg.FromTime->text();
		QString To_cmd = dlg.ToDate->text() + " " + dlg.ToTime->text();
		if(logrepgen) delete logrepgen;
		logrepgen = new LogReportGenerator("EVENTS", From_cmd, To_cmd);
		#endif //WIN32
	}
}
/*-Function:configureReport
*Inputs:none
*Outputs:none
*Returns:none
*/
void UserFrameWork::configureReport() // report design
{
	IT_IT("UserFrameWork::configureReport");
	
	ReportCfg dlg(this);
	dlg.exec();
};

/*
*Function: configureReceipes
*Inputs:none
*Outputs:none
*Returns:none
*/

void UserFrameWork::configureReceipes()
{
	IT_IT("UserFrameWork::configureReceipes");
	
	ReceipeCfg dlg(this);
	dlg.exec();
};


/*-Function: restart
*Inputs:none
*Outputs:none
*Returns:none
*/
void UserFrameWork::restart() // stop and start monitoring
{
	IT_IT("UserFrameWork::restart");

	if(!YESNO(tr("Restart Monitoring"),tr("Restart Monitoring Are You Sure?")))
	{
		QString msg = "All messages acknowledged automatically by restarting the monitor"; //apa+++ 26-07-2012
		AckAllAlarms(msg);
		//broadcast
		GetDispatcher()->DoExec(NotificationEvent::CMD_MONITOR_STOP);
		DOAUDIT(tr("Stop Monitor"));
		GetDispatcher()->DoExec(NotificationEvent::CMD_MONITOR_START);
		DOAUDIT(tr("Start Monitor"));
	};
};


/*-Function: loadReceipe
*Inputs:none
*Outputs:none
*Returns:none
*/
void UserFrameWork::loadReceipe() // stop monitoring and set the receipe
{
	IT_IT("UserFrameWork::loadReceipe");
	
	ReceipeSelect dlg(this);
	if(dlg.exec())
	{
		if(!YESNO(tr("Switch Receipe"), tr("Switch Receipe - Are You Sure ?")))
		{
			if(!dlg.List->currentText().isEmpty())
			{
				// stop monitoring
				//broadcast
				GetDispatcher()->DoExec(NotificationEvent::CMD_MONITOR_STOP);
				//
				// set the current receipe
				// 
				GetConfigureDb()->DoExec(0,
				"update PROPS set DVAL='"
				+ dlg.List->currentText()
				+ "' where SKEY='System' and IKEY='Receipe';",0);
				//
				//
				// and start it again
				//broadcast
				GetDispatcher()->DoExec(NotificationEvent::CMD_MONITOR_START);
				// 
				DOAUDIT(tr("Load Receipe:") + dlg.List->currentText());
			};
		};
	};
};
/*-Function: startTrace
*Inputs:none
*Outputs:none
*Returns:none
*/
void UserFrameWork::startTrace() // trace on
{
	IT_IT("UserFrameWork::startTrace");
	
	if(!YESNO(tr("Start Trace"), tr("Start Trace")))
	{
		//broadcast
		GetDispatcher()->DoExec(NotificationEvent::CMD_TRACE_ON);
		DOAUDIT(tr("Start Trace"));
	};
};
/*-Function: stopTrace
*Inputs:none
*Outputs:none
*Returns:none
*/
void UserFrameWork::stopTrace() // trace off
{
	IT_IT("UserFrameWork::stopTrace");

};
/*-Function: Help
*Inputs:none
*Outputs:none
*Returns:none
*/
void UserFrameWork::Help() // help
{
	IT_IT("UserFrameWork::Help");
	
	QSHelp("index");
};
/*-Function: systemManager
*Inputs:none
*Outputs:none
*Returns:none
*/
void UserFrameWork::systemManager() // start the system manager
{
	IT_IT("UserFrameWork::systemManager");
	
	//
	// we only get this option if we have all priviledges
	// first we must try and lock the database against other people being 
	// System Manager at the same time
	// 
	//if(!SysMgrFrameWork::Active)
	//{
		// Try and lock
		GetCurrentDb()->DoExec(this,"select DVAL from PROPS where SKEY='System' and IKEY='Lock';",tLockSysMgr); // get the system manager lock 
	//}
	//else
	///{
	//	if(pSys)
	//	{
	//		pSys->raise();
	//	};
	//};
};
/*-Function: printReport
*Inputs:none
*Outputs:none
*Returns:none
*/
void UserFrameWork::printReport() // print an existing report
{
	IT_IT("UserFrameWork::printReport");
	
	ReportDir dlg(this);
	if(dlg.exec())
	{
		if(!dlg.List->currentText().isEmpty())
		{
			SelectPrinterDlg pdlg(this);
			if(pdlg.exec())
			{
				#ifdef UNIX
				// spawn the report process
				if(!dlg.List->currentText().isEmpty())
				{
					QString cmd = QSBIN_DIR + "/printrep -display localhost:0 " + dlg.List->currentText() + 
					" " + pdlg.GetSelection()+ "&";
					system((const char *)cmd);
				};
				#endif

				#ifdef WIN32
				
				if(!dlg.List->currentText().isEmpty())
				{
					if(prtrep) delete prtrep;                          
					prtrep = new PrintReport(dlg.List->currentText(), pdlg.GetSelection());
				}

				#endif
			};
		};
	};
};
/*-Function: batches
*Inputs:none
*Outputs:none
*Returns:none
*/
void UserFrameWork::batches() // manage batches
{
	IT_IT("UserFrameWork::batches");
	
	BatchCfg dlg(this);
	dlg.exec();
};
/*-Function: ackAllAlarms
*Inputs:none
*Outputs:none
*Returns:none
*/
void UserFrameWork::ackAllAlarms() // acknowledge all alarms
{
	IT_IT("UserFrameWork::ackAllAlarms");
	
	AckAlarmDlg dlg(this);
	dlg.SetHeading(tr("Acknowledge All Alarms"));
	if(dlg.exec())
	{
		AckAllAlarms(dlg.GetComment());
	};
};
/*
*Function:QueryResponse
*Inputs:client object, command string, transaction id
*Outputs:none
*Returns:none
*/
void UserFrameWork::QueryResponse (QObject *p,const QString &c, int id, QObject*caller) // current value responses
{
	
	if(p != this) return;

	IT_IT("UserFrameWork::QueryResponse");

	switch(id)
	{
		case tGet: // we have the system properties
		{
			for(unsigned i = 0; i < GetConfigureDb()->GetNumberResults(); i++,GetConfigureDb()->FetchNext())
			{
				if(GetConfigureDb()->GetString("IKEY") == "Language")
				{
					if(UndoEscapeSQLText(GetConfigureDb()->GetString("DVAL")) != "English")
					{
						//
						// we have the language - default is English (sorry) 
						// 
						if(translation.load(UndoEscapeSQLText(GetConfigureDb()->GetString("DVAL")),QSTRANS_DIR))
						{
							qApp->installTranslator(&translation); // install the translator
						};
					};
				}
				else if(GetConfigureDb()->GetString("IKEY") == "Comment")
				{
					setCaption(UndoEscapeSQLText(GetConfigureDb()->GetString("DVAL")));
				}
				else if(GetConfigureDb()->GetString("IKEY") == "Autologoff")
				{
					AutoLogoffTime = GetConfigureDb()->GetInt("DVAL");
				}
				else if(GetConfigureDb()->GetString("IKEY") == "AuditLevel")
				{
					UserDetails &u = GetUserDetails();
					u.AuditLevel = GetConfigureDb()->GetInt("DVAL");
				}
				else if(GetConfigureDb()->GetString("IKEY") == "Receipe")
				{
					SetReceipeName (UndoEscapeSQLText(GetConfigureDb()->GetString("DVAL")));
				}	
				else if(GetConfigureDb()->GetString("IKEY") == "AutoLogOut")
				{
					AutoLogoffTime = GetConfigureDb()->GetInt("DVAL");//60000;
				};	
			};
			//
			// now ask for everything
			//
			QString cmd = "select * from TAGS_DB order by NAME asc;"; // get all current values
			GetCurrentDb()->DoExec(this,cmd,tList); // start by getting everything
			cmd = "select * from CVAL_DB order by NAME asc;"; // get all current values
			GetCurrentDb()->DoExec(this,cmd,tListSp); // start by getting everything
			emit Restart(); // notify a restart
			//
			QPopupMenu *file = new QPopupMenu(this);
			file->insertItem(QPixmap((const char **)start_xpm),tr("Login..."),this,SLOT(Login()));
			menuBar()->insertItem(QString(tr("&File")),file);
			//
//			statusBar()->clear(); // create a status bar
			QTimer::singleShot(100,this,SLOT(Logout())); // trigger the login by forcing logout
		};
		break;
		case tSemaphore:
		{
			int n = GetConfigureDb()->GetNumberResults();
			if(n > 0)
			{
				QString comport = GetConfigureDb()->GetString("DVAL");
			}
		}
		break;
		case tListSp: // list of updated sample points
		{
			//int n = GetCurrentDb()->GetNumberResults();
			//cerr << "List Sample Points nhits = " << n << endl;
			//for(int i = 0; i < n; i++)
			//{
			//	QString x = GetCurrentDb()->DumpTuple(i);
			//	cerr << (const char *)x << endl;
			//};
			emit UpdateSamplePoint(); // tell every one
		};
		break;
		case tList:
		{
			// list of updated tags
			emit UpdateTags(); // some windows want all tags that have been updated
		};
		break;
		case tLog:
		{
			int n = GetResultDb()->GetNumberResults();
			if(n > 0)
			{
				for(int i = 0; i < n; i++,GetResultDb()->FetchNext())
				{
					QString s = GetResultDb()->GetIsoDateString("TIMEDATE") + ":" + UndoEscapeSQLText(GetResultDb()->GetString("SOURCE")) + " " +
					UndoEscapeSQLText(GetResultDb()->GetString("QMESSAGE"));
					QSMessage(s);
				};
			};
		};
		break;
		case tAlarms:
		{
			int n = GetResultDb()->GetNumberResults();
			if(n > 0)
			{
				for(int i = 0; i < n; i++,GetResultDb()->FetchNext())
				{
					QString s = GetResultDb()->GetIsoDateString("TIMEDATE") + ":" + UndoEscapeSQLText(GetResultDb()->GetString("SOURCE")) + " " +
					UndoEscapeSQLText(GetResultDb()->GetString("QMESSAGE"));
					QSTrace(s); // put into the trace pane
				};
			};
		};
		break;
		case tLockSysMgr:
		{
			if(GetCurrentDb()->GetInt("DVAL") == 0)
			{
				DOAUDIT(tr("Start System Manager"));
				GetCurrentDb()->DoExec(0,"update PROPS set DVAL='1' where SKEY='System' and IKEY='Lock';",0);
				//pSys = new SysMgrFrameWork(0, "System Manager");
				//pSys->show();
				//pSys->raise();
			}
			else
			{
				QMessageBox::information(this,tr("System Manager"), tr("Someone is alreadly Logged on as System Manager"));
			};
		};
		break;
		default:
		break;
	};
};
/*
*Function:CurrentNotify
*Inputs:notification code
*Outputs:none
*Returns:none
*/
void UserFrameWork::ReceivedNotify(int ntf, const char * data)
{
	IT_IT("UserFrameWork::ReceivedNotify");
	
	switch(ntf)
	{
		case NotificationEvent::CURRENT_NOTIFY:
		{
			IT_COMMENT("CURRENT_NOTIFY");
			// request all new current values
			fRequestFetch = true;
		}
		break;
		case  NotificationEvent::MONITOR_STOPPED_NOTIFY:
		{
			IT_COMMENT("MONITOR_STOPPED_NOTIFY");
			//
			// the monitoring has stopped
			//
			statusBar_state = RED_STATE;
			QSMessage(QDateTimeString(QDateTime::currentDateTime()) + ":" + QString(tr("HMI -> Monitoring has stopped")));
			statusBar()->setBackgroundColor(Qt::red);
			statusBar()->message(tr("Monitoring has stopped"));
			MonitorRunning = false;
		}
		break;
		case  NotificationEvent::MONITOR_STARTED_NOTIFY:
		{
			IT_COMMENT("MONITOR_STARTED_NOTIFY");

			//
			// the system has started monitoring
			// reload all panes
			QString cmd = "select IKEY,DVAL from PROPS where SKEY='System';"; // get the system properties - we need the receipe before anything
			GetConfigureDb ()->DoExec (this, cmd, tGet);	// make the request
			//
			QSMessage(QDateTimeString(QDateTime::currentDateTime()) + ":" + QString(tr("HMI -> Monitoring has started")));
		}
		break;
		case  NotificationEvent::EVENT_NOTIFY:
		{
			IT_COMMENT("EVENT_NOTIFY");
			
			QSMessage(QDateTimeString(QDateTime::currentDateTime()) + ":" + QString(data));
		}
		break;
		case  NotificationEvent::ACK_NOTIFY:
		{ 
			IT_COMMENT("ACK_NOTIFY");
			
			// we have had a change in the ack states - get the latest ack's alarms
			fRequestFetch = true;
			QString id = data;
			if(id ==  ACK_ALL_TOKEN)
			{
				QString cmd = "select * from CVAL_DB;";   // easy thing is to reload all sample point states
				GetCurrentDb()->DoExec(this,cmd,tListSp); // start by getting everything
			}
			else
			{
				QString cmd = "select * from CVAL_DB where NAME='" + id + "';";  
				GetCurrentDb()->DoExec(this,cmd,tListSp); // only one point has changed
			}
		}
		break;
		case  NotificationEvent::ALARM_NOTIFY:
		{
			IT_COMMENT("ALARM_NOTIFY");
			statusBar_state = RED_STATE;
			statusBar()->setBackgroundColor(Qt::red);

			QSTrace(QDateTimeString(QDateTime::currentDateTime()) + ":" + QString(data));
		}
		break;
		case  NotificationEvent::MONITOR_TICK_NOTIFY:
		{
			IT_COMMENT("MONITOR_TICK_NOTIFY");
			
			// this is a monitor tick 
			MonitorRunning = true;

			LastSeqNo = data; // get the sequence number
			
			if(statusBar_state != GREEN_STATE)
			{
				statusBar_state = GREEN_STATE;
				statusBar()->setBackgroundColor(Qt::green);
				QString message = tr("Monitor is Running -- ") + tr("Active receipe: ") + GetReceipeName() + tr(" -- User: ") + GetUserDetails().Name;
				statusBar()->message(message);
			}

			MonitorTimeout = MONITOR_TICK_TIME + 2000;			
		}
		break;
		case  NotificationEvent::UPDATE_NOTIFY:
		{
			IT_COMMENT("UPDATE_NOTIFY");

			fRequestFetch = true; //apa+++ 16-06-2014

			LastSeqNo = data; // get the sequence number

			//DoFetch();
			
			MonitorTimeout = MONITOR_TICK_TIME + 2000;
		}
		break;
		case NotificationEvent::SERIAL_DRIVER_ERROR_NOTIFY:
		{
			statusBar_state = RED_STATE;
			statusBar()->setBackgroundColor(Qt::red);
			statusBar()->message(tr("Serial driver error - SERIOUS FAILURE"));
		}
		break;
		case  NotificationEvent::CMD_SHUTDOWN_CLIENTS:
		{
			QTimer::singleShot(3000,qApp,SLOT(quit())); // quit in 3 seconds
		}
		break;
		default:
		break;
	}
};


/*
*Function: Tick
*one second timer for timeouts 
*Inputs:none
*Outputs:none
*Returns:none
*/
void UserFrameWork::Tick()
{
	IT_IT("UserFrameWork::Tick");

	MonitorTimeout = MonitorTimeout - 100;

	if(MonitorTimeout < 0) // have we heard from the montoring task yet
	{
		if(MonitorRunning)
		{
			statusBar_state = RED_STATE;
			statusBar()->setBackgroundColor(Qt::red);
			statusBar()->message(tr("No connection with monitor"));
			QSMessage(QDateTimeString(QDateTime::currentDateTime()) + ":" + QString("HMI -> No connection with monitor"));
			MonitorRunning = false;
		}

		MonitorTimeout = MONITOR_TICK_TIME + 2000;
	}

	if(fRequestFetch)
	{
		DoFetch();
		fRequestFetch = false;
	}

	if(GetSpareDispatcher() != NULL)
	{
		//if((!GetSpareDispatcher()->Ok()) && (MaxRetryReconnectToSpareDispatcher <= 5))
		if(!GetSpareDispatcher()->Ok())
		{
			statusBar_state = RED_STATE;
			statusBar()->setBackgroundColor(Qt::red);
			statusBar()->message(tr("Spare Dispatcher Client Failure"));

			QSLogEvent("HMI", "Spare dispatcher client connection error");
			QSLogEvent("HMI", "Attempt to restore connection with spare dispatcher server");
			
			if(GetSpareDispatcher()->IsConnected())
			{
				if(!GetSpareDispatcher()->Ok())
				{
					DisconnectFromSpareDispatcher();
				}
			}

			if(!GetSpareDispatcher()->IsInRetry())
			{
				ConnectToSpareDispatcher();
			}

			++MaxRetryReconnectToSpareDispatcher;
		}
		else
		{
			MaxRetryReconnectToSpareDispatcher = 0;
		}
	}


	//if((!GetDispatcher()->Ok()) && (MaxRetryReconnectToDispatcher <= 5))
	if(!GetDispatcher()->Ok())
	{
		statusBar_state = RED_STATE;
		statusBar()->setBackgroundColor(Qt::red);
		statusBar()->message(tr("Dispatcher Client Failure"));

		QSLogEvent("HMI", "Dispatcher client connection error");
		QSLogEvent("HMI", "Attempt to restore connection with dispatcher server");
		
		if(GetDispatcher()->IsConnected())
		{
			if(!GetDispatcher()->Ok())
			{
				DisconnectFromDispatcher();
			}
		}

		if(!GetDispatcher()->IsInRetry())
		{
			ConnectToDispatcher();
		}

		++MaxRetryReconnectToDispatcher;
	}
	else
	{
		MaxRetryReconnectToDispatcher = 0;
	}

	if(!GetConfigureDb()->Ok() || !GetResultDb()->Ok() || !GetCurrentDb()->Ok())
	{
		//if(MaxRetryReconnectToRealTimeDb <= 5)
		{
		
			statusBar_state = RED_STATE;
			statusBar()->setBackgroundColor(Qt::red);
			statusBar()->message(tr("Realtime database Client Failure"));

			if(!GetConfigureDb()->Ok())
			{
				QString msg = QString("Real time client error: ") + GetConfigureDb()->GetErrorMessage();
				QSLogEvent("HMI", msg);
				GetConfigureDb()->AcnoledgeError();
			}

			if(!GetResultDb()->Ok())
			{
				QString msg = QString("Real time client error: ") + GetResultDb()->GetErrorMessage();
				QSLogEvent("HMI", msg);	
				GetResultDb()->AcnoledgeError();
			}
			
			if(!GetCurrentDb()->Ok())
			{
				QString msg = QString("Real time client error: ") + GetCurrentDb()->GetErrorMessage();
				QSLogEvent("HMI", msg);	
				GetCurrentDb()->AcnoledgeError();
			}

			QSLogEvent("HMI", "Attempt to restore connection with realtime database server");

			//if(GetConfigureDb()->IsConnected() || GetCurrentDb()->IsConnected() || GetResultDb()->IsConnected())
			{
				if(!GetConfigureDb()->Ok() || !GetResultDb()->Ok() || !GetCurrentDb()->Ok())
				{
					DisconnectFromRealTimeDatabases();
				}
			}

			//if()
			{
				ConnectToRealTimeDatabases();
			}
			
			++MaxRetryReconnectToRealTimeDb;
		}
	}
	else
	{
		MaxRetryReconnectToRealTimeDb = 0;
	}

	if((GetSpareConfigureDb() != NULL) && (GetSpareCurrentDb() != NULL)&&(GetSpareResultDb() != NULL))
	{
		if(!GetSpareConfigureDb()->Ok() || !GetSpareResultDb()->Ok() || !GetSpareCurrentDb()->Ok())
		{
			//if(MaxRetryReconnectToSpareRealTimeDb <= 5)
			{
				if(!GetSpareConfigureDb()->Ok())
				{
					QString msg = QString("Spare real time client error: ") + GetSpareConfigureDb()->GetErrorMessage();
					QSLogEvent("HMI", msg);
					GetSpareConfigureDb()->AcnoledgeError();
				}

				if(!GetSpareResultDb()->Ok())
				{
					QString msg = QString("Spare real time client error: ") + GetSpareResultDb()->GetErrorMessage();
					QSLogEvent("HMI", msg);	
					GetSpareResultDb()->AcnoledgeError();
				}
				
				if(!GetSpareCurrentDb()->Ok())
				{
					QString msg = QString("Spare real time client error: ") + GetSpareCurrentDb()->GetErrorMessage();
					QSLogEvent("HMI", msg);	
					GetSpareCurrentDb()->AcnoledgeError();
				}

				QSLogEvent("HMI", "Attempt to restore connection with spare realtime database server");

				if(GetSpareConfigureDb()->IsConnected())
				{
					if(!GetSpareConfigureDb()->Ok())
					{
						DisconnectFromSpareRealTimeDatabases();
					}
				}

				ConnectToSpareRealTimeDatabases();

				++MaxRetryReconnectToSpareRealTimeDb;
			}
		}
		else
		{
			MaxRetryReconnectToSpareRealTimeDb = 0;
		}
	}

	if(GetHistoricResultDb() != NULL)
	{
		//if((!GetHistoricResultDb()->Ok()) && (MaxRetryReconnectToHistoricDb <= 5))
		if(!GetHistoricResultDb()->Ok())
		{
			statusBar_state = RED_STATE;
			statusBar()->setBackgroundColor(Qt::red);
			statusBar()->message(tr("Historical Database Client Failure"));

			QString msg = QString("Historical client error: ") + GetHistoricResultDb()->GetErrorMessage();
			QSLogEvent("HMI", msg);
			QSLogEvent("HMI", "Attempt to restore connection with historical database server");
			GetHistoricResultDb()->AcnoledgeError();

			//if(MaxRetryReconnectToHistoricDb == 0)
			{
				DisconnectFromHistoricDatabases();
			}

			ConnectToHistoricDatabases();

			++MaxRetryReconnectToHistoricDb;
		}
		else
		{
			MaxRetryReconnectToHistoricDb = 0;
		}
	}

/*
	if((MaxRetryReconnectToHistoricDb > 2) || 
		(MaxRetryReconnectToRealTimeDb > 2) ||
		(MaxRetryReconnectToDispatcher > 2) || 
		(MaxRetryReconnectToSpareRealTimeDb > 2) ||
		(MaxRetryReconnectToSpareDispatcher > 2) 
		)
	{
		statusBar_state = RED_STATE;
		statusBar()->setBackgroundColor(Qt::red);
		statusBar()->message(tr("Too much reconnect retries .."));
		
		QSLogEvent("HMI", "Too much reconnect retries ..");
	}
*/

	
	--DataUITimeout;

	if(DataUITimeout < 0) 
	{
		
		//global and calling process memory
		MEMORYSTATUS lpBuffer;
		lpBuffer.dwLength = sizeof(MEMORYSTATUS);
		GlobalMemoryStatus(&lpBuffer);
				
		double UIMemory = (double)(lpBuffer.dwTotalVirtual - lpBuffer.dwAvailVirtual);
		if(InitialMemory == 0.0)
		{
			InitialMemory = (double)(lpBuffer.dwTotalVirtual - lpBuffer.dwAvailVirtual);
		}

		double deltaUImem = 100.0*(UIMemory - InitialMemory)/InitialMemory;
				
		//dispatch without feedback
		char parametri[sizeof(dispatcher_extra_params)];
		memset(parametri, 0, sizeof(dispatcher_extra_params));

		dispatcher_extra_params* params = (dispatcher_extra_params *) parametri;
					
		params->value = deltaUImem;
		
		//dispatch without feedback
		GetDispatcher()->DoExec(NotificationEvent::UI_EXE_DATA, parametri, sizeof(dispatcher_extra_params));
				
		DataUITimeout = 10;
	}
};
/*
*Function:DoFetch()
*Inputs:none
*Outputs:none
*Returns:none
*/
void UserFrameWork::DoFetch()
{
	IT_IT("UserFrameWork::DoFetch");

	// get all updated points
	QString cmd = "select * from TAGS_DB where SEQNO >= " + LastFetchNo  + " order by NAME asc;";  
	GetCurrentDb()->DoExec(this,cmd,tList); 	  
	//
	cmd = "select * from CVAL_DB where SEQNO >= " + LastFetchNo + " order by NAME asc;";  
	GetCurrentDb()->DoExec(this,cmd,tListSp); 	  
	LastFetchNo = LastSeqNo;
};
/*
*Function: penTrace
*Open a pen trace window
*Inputs:none
*Outputs:none
*Returns:none
*/
void UserFrameWork::penTrace()
{
	IT_IT("UserFrameWork::penTrace");
	
	Multitrace *p = new Multitrace; // create the top level window
	connect(this,SIGNAL(UpdateTags()),p,SLOT(UpdateTags())); 
	//
	p->show();
	//p->setGeometry(QApplication::desktop()->geometry());// maximise to full screen 
	p->setGeometry(3, 20, QApplication::desktop()->width() - 3, QApplication::desktop()->height()-50);
	p->raise();
	//
};

/*
*Function: realtimeDbManagement
*Open a database management window
*Inputs:none
*Outputs:none
*Returns:none
*/
void UserFrameWork::realtimeDbManagement()
{
	IT_IT("UserFrameWork::realtimeDbManagement");
	
	RealTimeBrowsedb *p = new RealTimeBrowsedb(GetRealTimeDbDict()); // create the top level window
	//
	p->show();
	p->setGeometry(3, 20, QApplication::desktop()->width() - 3, QApplication::desktop()->height()-50);
	p->raise();
	//
};

/*
*Function: spareRealtimeDbManagement
*Open a database management window
*Inputs:none
*Outputs:none
*Returns:none
*/
void UserFrameWork::spareRealtimeDbManagement()
{
	IT_IT("UserFrameWork::spareRealtimeDbManagement");
	
	RealTimeBrowsedb *p = new RealTimeBrowsedb(GetSpareRealTimeDbDict()); // create the top level window
	//
	p->show();
	p->setGeometry(3, 20, QApplication::desktop()->width() - 3, QApplication::desktop()->height()-50);
	p->raise();
	//
};


/*
*Function: historicDbManagement
*Open a database management window
*Inputs:none
*Outputs:none
*Returns:none
*/
void UserFrameWork::historicDbManagement()
{
	IT_IT("UserFrameWork::historicDbManagement");
	
	HistoricBrowsedb *p = new HistoricBrowsedb(GetHistoricDbDict()); // create the top level window
	//
	p->show();
	p->setGeometry(3, 20, QApplication::desktop()->width() - 3, QApplication::desktop()->height()-50);
	p->raise();
	//
};


/*
*Function:closeEvent
*Inputs:event structure
*Outputs:none
*Returns:none
*/
void UserFrameWork::closeEvent(QCloseEvent *e)
{
	IT_IT("UserFrameWork::closeEvent");
	
	if(GetUserDetails().privs & PRIVS_CAN_EXIT)
	{
		// we must allow enough time for the database queues to finish their stuff
		// do we have the system manager active or not ?
		//if(SysMgrFrameWork::Active)
		//{
		//	// clear the semaphore
		//	GetCurrentDb()->DoExec(0,"update PROPS set DVAL='0' where SKEY='System' and IKEY='Lock';",0);
//
//			pSys->hide();			
//		};		
		this->hide(); // hide the application
		QTimer::singleShot(3000,qApp,SLOT(quit())); // quit in 3 seconds
		e->ignore();
	}
	else
	{  
		e->ignore(); // only people who can exit can close this application
	};
};
/*
*Function: UserEntry
*make a usr entry
*Inputs:none
*Outputs:none
*Returns:none
*/
void UserFrameWork::UserEntry()
{
	IT_IT("UserFrameWork::UserEntry");
	
	AckAlarmDlg dlg(this);
	dlg.SetHeading(tr("User Message"));
	if(dlg.exec())
	{
		QSLogEvent(GetUserDetails().Name,dlg.GetComment()); 
	};
};

/*
*Function: About
*Inputs:none
*Outputs:none
*Returns:none
*/
void UserFrameWork::About()
{
	IT_IT("UserFrameWork::About");
	
	#ifdef WIN32
		int compiler = _MSC_VER;
		QString comp;

		switch(compiler)
		{
			case 1200:
			comp = QString("VC++ 6.0 SP6 ");
			break;
			case 1300:
			comp = QString("VC++ 7.0 ");
			break;
			case 1310:
			comp = QString("VC++ 7.1 ");
			break;
			default:
			comp = tr("Unknown ");
			break;
		}

		QDateTime dt = QDateTime::currentDateTime();

		QMessageBox::about(this,tr(SYSTEM_NAME),
								tr("Version: ") + QString(GetScadaDateBuild()) + " " + QString(GetScadaTimeBuild())
								+ QString("\n")
								+ tr("Compiler version: ") + comp
								+ QString("\n")
								+ tr("OS version: ") + QString(winver())
								+ QString("\n")
								+ tr("UI version: ") + QString(QT_VERSION_STR)   //Qt toolkit version
								+ QString("\n")
								+ tr("Real time database version: ") + QString::number(354)  //FastDB version used
								+ QString("\n")
								+ tr("Historical database version: ") + QString::number(337)  //GigaBASE versions used
								+ QString("\n")
								#ifdef NDEBUG
								+ tr("Build configuration: ") + QString("Release")
								#else
								+ tr("Build configuration: ") + QString("Debug")
								#endif
   							    + QString(" at enscada.com\n")
								+ QString("\nIndigoSCADA (\"Software\") is distributed under a license agreement (\"Agreement\") between you,\n\
(\"Licensee\") and Enscada Limited (\"Licensor\"). By downloading\n\
the Software electronically, installing, copying, or otherwise using the\n\
Software, Licensee agrees to be bound by the terms and conditions of this\n\
Agreement. If Licensee does not agree to the terms and conditions of this\n\
agreement then do not download the Software, install or use the Software, and\n\
immediately destroy the Software.\n\
IndigoSCADA is free software and may be used in accordance\n\
with the terms of the GNU LESSER GENERAL PUBLIC LICENSE v3 as published by the Free\n\
Software Foundation <http://www.gnu.org/licenses/lgpl-3.0-standalone.html>.\n\
\n\
LICENSEE ACKNOWLEDGES THAT THE SOFTWARE IS SUPPLIED FREE, WITHOUT LICENSE FEES\n\
AND IS THEREFORE PROVIDED WITH NO WARRANTIES OF ANY KIND INCLUDING THE\n\
WARRANTIES OF DESIGN, MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE,\n\
NONINFRINGEMENT, OR ARISING FROM A COURSE OF DEALING, USAGE OR TRADE PRACTICE.\n\
EXCEPT TO THE EXTENT PROHIBITED BY APPLICABLE LAW LICENSOR SHALL HAVE NO\n\
LIABILITY FOR DAMAGES OF ANY KIND INCLUDING ANY INDIRECT OR CONSEQUENTIAL LOSS\n\
(INCLUDING WITHOUT LIMITATION, LOSS OF USE; DATA; INFORMATION; BUSINESS;\n\
PRODUCTION OR GOODWILL), EXEMPLARY OR INCIDENTAL DAMAGES, LOST PROFITS OR OTHER\n\
SPECIAL OR PUNITIVE DAMAGES WHATSOEVER, WHETHER IN CONTRACT, TORT, (INCLUDING\n\
NEGLIGENCE, STRICT LIABILITY AND ALL OTHERS), WARRANTY, INDEMNITY OR UNDER\n\
STATUTE, EVEN IF LICENSOR HAS BEEN ADVISED OF THE LIKELIHOOD OF SAME.\n\
")
							);
	#endif
};

void UserFrameWork::closeAll()
{
	if(!YESNO(tr("Question"), tr("Do you really want to close all clients and monitor?")))
	{
		GetDispatcher()->DoExec(NotificationEvent::CMD_SHUTDOWN_MONITOR);
		//GetDispatcher()->DoExec(NotificationEvent::CMD_SHUTDOWN_SQLSERVER); //not implemented yet
		GetDispatcher()->DoExec(NotificationEvent::CMD_SHUTDOWN_CLIENTS);
	}
}

/*! Shows the Splash window (start up)
    \sa closeIntro()
*/
void UserFrameWork::showIntro() 
{
	intro = new RIntro(GetScadaTimeBuild(), this, "intro");
	intro->show();
}


/*! Closes the Splash window
    \sa showIntro()
*/
void UserFrameWork::closeIntro() 
{
	if(intro)
	{
		delete intro;
	}
}
