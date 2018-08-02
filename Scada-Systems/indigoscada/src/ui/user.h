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
*Header For: user framework
*Purpose: the user frame work is the application's main window
*/
#ifndef include_user_hpp 
#define include_user_hpp
#include "dbase.h"
#include "dispatch.h"
//
class AlarmDisplay;
class MessageDisplay;
class StatusDisplay;
class SysMgrFrameWork;
class PrintReport;
class LogReportGenerator;
class ReportGenerator;
class RIntro;
class HMI_manager;

#ifdef USE_STD_MAP
typedef std::map<QString, HMI_manager*, std::less<QString> > HMIDict; // the HMI dictionary 26-11-2012
#endif

#define USE_STATUS_TAB

class UserFrameWork : public QMainWindow
{
	Q_OBJECT
	//
	QToolBar       *pToolBar; // the current tool bar
	QToolBar       *pToolBarLogo; // the current tool bar logo
	AlarmDisplay   *pAlarms; // the alarms window
	#ifdef USE_STATUS_TAB
	StatusDisplay  *pStatus; // the status window
	#endif	
	SysMgrFrameWork *pSys;   // system manager framework
	//
	unsigned AutoLogoffTime; // number of seconds to wait before forcing a log off
	int AutoLogOffCount; // count down for auto log off
	QTimer * pLogoffTimer; // auto log off timer
	void DoFetch();
	void SetTabs();
	//
	enum { tList = 1, tListSp, tLog, tGet, tAlarms, tLockSysMgr, tSemaphore};
	//
	QTranslator translation;
	bool fInLogin; // login dialog sentinal
	bool MonitorRunning; // is the monitor task running or not 
	int MonitorTimeout; // timeout track for detecting monitor failures
	int DataUITimeout;  // timeout track for detecting ui process operatig system data
	double InitialMemory; //initial memory of ui process
	bool fRequestFetch; // true if we should issue a fetch
	QString LastSeqNo; // the last sequence number broadcast by monitor
	QString LastFetchNo; // the last "time" when a fetch was done 
	//

	////////////support for PrintReport//////////////////////
	PrintReport *prtrep;
	////////////support for LogReportGeneretor///////////////
	LogReportGenerator *logrepgen;
	////////////support for ReportGeneretor//////////////////
	ReportGenerator *repgen;
	/////////////////////////////////////////////////////////
	//logo support
	QString firmAddress;
	QString iscadaAddress;
	/////////////////////////////////////////////////////////

	//Intro widget
	RIntro*       intro;

	////////////////////////////////////////////////////////////
	int MaxRetryReconnectToDispatcher;
	int MaxRetryReconnectToRealTimeDb;
	int MaxRetryReconnectToHistoricDb;
	int MaxRetryReconnectToSpareDispatcher;
	int MaxRetryReconnectToSpareRealTimeDb;
	////////////////////////////////////////////////////////////

	int statusBar_state;

	enum{
	 GREEN_STATE,
	 RED_STATE,
	 WHITE_STATE
	};

	public:
	UserFrameWork();
	~UserFrameWork();
	//
	MessageDisplay *pMessage; // the message window
	//
	void closeEvent(QCloseEvent *);
	//
	//
	// menu item handlers
	//
	public slots:
	void Login(); // handle the login
	void showIntro();
	void closeIntro();
	void makeReport(); // generate a report
	void reviewReport(); // review an existing report
	void alarmsReport(); // genrate an alarms report
	void eventsReport(); // generate an events report
	void configureReport(); // report design
	void configureReceipes(); //Receipes configure
    void configureSystem();
    void configureUnits();
    void configureSamplePoints();
    void configureSchedule();
    void configureAlarmGroups();
	void configureHMI(); //Start the hmi_designer.exe tool
	void configureProtocol(); //Start the protocol_configurator.exe tool
    void configurePorts();
    void configureUser();
	void restart(); // stop and start monitoring
	void loadReceipe(); // stop montoring and set the receipe
	void startTrace(); // trace on
	void stopTrace(); // trace off
	void Help(); // help
	void systemManager(); // start the system manager
	void printReport(); // print an existing report
	void batches(); // manage batches
	void ackAllAlarms(); // acknowledge all alarms
	//
	void QueryResponse (QObject *,const QString &, int, QObject*); //database responses
	void ReceivedNotify(int, const char *);
	void Logout(); // logout
	void AutoLogOut(); // automatic logout 
	void penTrace(); // pen trace
	void realtimeDbManagement(); //real time database management
	void spareRealtimeDbManagement(); //spare real time database management
	void historicDbManagement(); //historical database management
	void UserEntry(); // user makes message entries
	void About(); // about and version number
	void AuditReport(); // generate an Audit Log Report
	void ClearSystemMgrLock(); // clear the system manager lock - may be set after a crash
	void closeAll();
	//
	void Tick();
	signals:
	void UpdateTags(); // update current tag value
	void UpdateSamplePoint(); // update the sample point
	void Restart(); // started monitoring
	void CloseDialog();
	//
};
#endif

