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

#ifndef COMMON_HPP
	#define COMMON_HPP

	#ifdef UNIX
		#include <unistd.h>
		#include <stdio.h>
		#include <stdlib.h>
		#include <fcntl.h>
	#endif

	#ifdef WIN32
		#pragma warning (disable : 4355)
		#pragma warning (disable : 4786)
		#pragma warning (disable : 4390)
	#endif

	#include <errno.h>
	#include <signal.h>
	#include <map>
	#include <vector>

	//Qt library
	#include <qt.h>

	#define R_SUCCESS (0)
	#define R_FAILURE (-1)
	typedef long rcode;
	// handy strings
	#define NONE_STR "(***)"
	// size of UDP message
	#define MAX_EVENT_SIZE 0x2000
	//
	// class configuration functions
	// 
	#ifdef UNIX
		// Linux DLL stuff
		#include <dlfcn.h>
		#define DLL_EXT ".so"
		#define DLL_EXT_NODOT "so"
	#endif

	#ifdef WIN32
		#include <windows.h>
		#include <process.h>

		#ifndef _WINDOWS
			#define _WINDOWS
		#endif

		#define DLL_EXT ".dll"
		#define DLL_EXT_NODOT "dll"
	#endif
	//
	// dll entry point function
	// 
	#define DRIVER_ENTRY "_GetDriverEntry"
	//
	// dll exit point function
	// 
	#define DRIVER_EXIT "_Unload"
	//
	// the type of the exit point
	// 
	typedef void (_cdecl * FncPtrDriverExit)(); 
	//
	//
	//
	#include <time.h>
	#include <signal.h>

	#ifdef WIN32
		#include "scada_dll.h"
	#else
		// under UNIX we have no such thing as exports
		#define QSEXPORT 
	#endif

	QSEXPORT void QSMessage(const QString &s);
	QSEXPORT void QSTrace(const QString &s);

	QSEXPORT void * GetDllEntry(const QString &dllName, const QString &funcName);
	QSEXPORT void UnloadAllDlls(); // unload all Dlls 
	QSEXPORT int SetComboItem(QComboBox *pCombo, const QString &item); // set the items in a combo box
	QSEXPORT int SetListItem(QListBox *pCombo, const QString &item); // set the items in a list box
	QSEXPORT int SetListViewItem(QListView *pCombo, const QString &item); // set the items in a list box
	QSEXPORT void SetTabOrder(QWidget *Parent, QWidget **pL); // set tab order from a widget list
	QSEXPORT void SetReceipeName(const QString &s);
	QSEXPORT const QString & GetReceipeName();

	QSEXPORT void SetScadaHomeDirectory(const QString &s); //without final slach or backslash
	QSEXPORT const QString & GetScadaHomeDirectory(); //without final slach or backslash
	//
	QSEXPORT QString GetAlarmStateName(int state); // get the alarm state names and colours
	QSEXPORT QColor GetAlarmStateBkColour(int state);
	QSEXPORT QColor GetAlarmStateFgColour(int state);
	//
	QSEXPORT QDateTime IsoDateQDateTime(const QString &tm); // convert string into date/time
	QSEXPORT QDateTime IsoDateMsToQDateTime(const QString &tm); // convert string into date/time with milliseconds;
	QSEXPORT QString QDateTimeToIsoDateInSeconds(const QDateTime &dt); // and in seconds ...
	QSEXPORT QString QDateTimeToIsoDateInMilliSeconds(const QDateTime &dt); // and in milliseconds ...

	#define QDATE_TIME_ISO_DATE(str)	QDateTimeToIsoDateInMilliSeconds(str)

	QSEXPORT QDateTime ReportFromTime(int mode, const QString &Date, const QString &Time);
	QSEXPORT QDateTime ReportToTime(int mode, const QString &Date, const QString &Time);
	//
	QSEXPORT QString EncodePassword(const QString &); // encode a sting into password format
	//
	// Packet encode and decode functions
	//
	QSEXPORT int KermitEncodePacket(int Address, const QString &In,QString &Out);
	QSEXPORT int KermitDecodePacket(const QString &P, QString &O, int &Address);
	QSEXPORT BOOL RunningAsAdministrator();
	//
	//
	//
	inline QString QDateTimeString(const QDateTime &dt)
	{
		QString t;
		t.sprintf("%04d-%02d-%02d %02d:%02d:%02d.%03d",
		dt.date().year(), dt.date().month(), dt.date().day(),
		dt.time().hour(), dt.time().minute(),dt.time().second(), dt.time().msec()); 
		return t;
	};
	//
	//
	// this is the minimum uid for the process
	// we want to trap out where we run as root
	// Running as root (uid = 0) can spell disaster when things fail
	// 
	#define QS_MIN_UID (10)
	//
	//
	//
	// Paths
	//
	// Where to find files:

	//local to the user dir
	#define QSCONFIG_DIR QSFilename("Config")
	#define QSTEMP_DIR QSFilename("Temp")
	#define QSREPORT_DIR QSFilename("Reports")

	//global to the default directory
	#define QSDRIVER_DIR QSDefaultFilename("Drivers")
	#define QSHELP_DIR QSDefaultFilename("Help")
	#define QSTRANS_DIR QSDefaultFilename("Translations")
	#define QSBIN_DIR QSDefaultFilename("bin") 
	#define QSTEMPLATE_DIR QSDefaultFilename("Template")
	#define QSBMP_DIR QSDefaultFilename("Bitmaps")

	#ifdef UNIX
		#define QSHELP_FILE(s) QSHELP_DIR + "/" + s + ".html"
	#else
		#define QSHELP_FILE(s) QSHELP_DIR + "\\" + s + ".html"
	#endif
	//
	//
	inline QString QSFilename(const QString &name)
	{
		#ifdef UNIX
		QString f = GetScadaHomeDirectory() + "/" + name;
		#else
		QString f = GetScadaHomeDirectory() + "\\" + name;
		#endif

		return f;
	};

	inline QString QSDefaultFilename(const QString &name)
	{
		#ifdef UNIX
		QString f = GetScadaHomeDirectory() + "/" + name;
		#else
		QString f = GetScadaHomeDirectory() + "\\" + name;
		#endif

		return f;
	};

	inline QString QSTempFilename() // generate a temp file name - or a stem for a set of temp files
	{
		QString s = QSTEMP_DIR;
		s = QString(tempnam((const char *)s,"REP"));
		return s;
	};
	//
	// convert a string (00:00:00) into interval in seconds
	inline long QStringToInterval(const QString &s) 
	{
		const char *p = (const char *)s;
		long l = (atoi(p) * 3600L) + (atoi(p+3) * 60L) + atoi(p + 6);
		return l;
	};
	// convert an interval into a QString
	inline QString IntervalToString(long l)
	{
		QString s;
		s.sprintf("%02d:%02d:%02d",(int)(l / 3600L), (int)((l / 60L) % 60L), (int)(l % 60L));
		return s;
	};

	// a lot of things use the help window
	#include "helpwindow.h"

	//
	// Report time frame modes
	//
	enum // from time frame
	{
		LastHour = 0,
		LastFourHours,
		LastTwelveHours,
		LastDay,
		LastWeek,
		FromDateTime,
		FromToday,
		FromYesterday
	};
	enum // To time frame
	{
		Now = 0,
		ToYesterday,
		ToToday,
		ToDateTime
	};
	//
	//
	// Schedule Events
	// these are the enums for the frequencies
	enum
	{
		EveryMonday = 0,
		EveryTuesday,
		EveryWednesday,
		EveryThursday,
		EveryFriday,
		EverySaturday,
		EverySunday,
		EveryDay, // 7
		PeriodicEvent, // 8
		//
		// first days of month
		// 
		FirstMonday,
		FirstTuesday,
		FirstWednesday,
		FirstThursday,
		FirstFriday,
		FirstSaturday,
		FirstSunday,
		FirstWorkingDay, // (Assume Mon - Fri Working Days - Change for Arabic and Israel)
		//
		LastMonday, // last days of month
		LastTuesday,
		LastWednesday,
		LastThursday,
		LastFriday,
		LastSaturday,
		LastSunday,
		LastWorkingDay // (Assume Mon - Fri Working Days - Change for Arabic and Israel)
		//
	};
	enum // alarm levels
	{
		NoLevel = 0,
		OkLevel,
		WarningLevel,
		AlarmLevel,
		FailureLevel
	};
	enum // message types (levels)
	{
		MessageTrace = 0, // diagnostic trace 
		MessageEvent,     // system events and information
		MessageWarning,   // a warning message
		MessageAlarm,     // an alarm message
		MessageFailure
	};
	enum //priviledges mask
	{
		PRIVS_ALL =				0xFFFFFFFF,	// same as system programmer
		PRIVS_ACK_ALARMS =		0x00000001, // acknowledge alarms
		PRIVS_REPORTS =			0x00000002, // design reports
		PRIVS_REPORT_MAKE =		0x00000004, // run the report generator 
		PRIVS_AGE_PASSWORD =	0x00000008, // password does not age 
		PRIVS_CAN_EXIT =		0x00000010, // the user can exit the application
		PRIVS_AUTOLOGOUT =		0x00000020, // the user is auto logged out 
		PRIVS_FACTORY_ADMIN =	0x00000080, // factory administrator
		//PRIVS_SEND_COMMANDS =	0x00000100, // send commands
		PRIVS_NONE = 0
	};

	//
	// operators for handling bool
	// 
	inline QDataStream & operator >> (QDataStream &is , bool &b)
	{
		int i;
		is >> i;
		b = (i?true:false);
		return is;
	};
	inline QDataStream & operator << (QDataStream &os, bool &b)
	{
		os << (b?1:0);
		return os;
	};

	#include "inifile.h"

	// open a help window
	inline void QSHelp(const QString &s)
	{
//		QString home;
//		home = QSHELP_FILE(s);
//		HelpWindow* helpw = new HelpWindow(home, QSHELP_DIR, 0, QObject::tr("help viewer"));
//		helpw->show();
//		QObject::connect( helpw, SIGNAL(lastWindowClosed()),	helpw, SLOT(quit()) );

		#ifdef UNIX
		QString p = QSBIN_DIR + "/helpviewer " +  QSHELP_FILE(s) + "&";
		system((const char *)p);
		#else
		
		QString ini_file = GetScadaHomeDirectory() + "\\project\\scada.ini";
		Inifile iniFile((const char*)ini_file);

		if(iniFile.find("path","internet_browser"))
		{
			QString path;
			path = iniFile.find("path","internet_browser");

			QString cmd = path + " " +  QSHELP_FILE(s);
			
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
		}
 		
		#endif
	}

	//
	// ***********************************************************************

	#ifdef WIN32
	QSEXPORT char *winver();
	QSEXPORT BOOL RunningAsAdministrator();
	#endif

	QSEXPORT QString GetNames();

	
	// yes/no response
	#define YESNO(c,m)  (QMessageBox::information(this,c,m,QObject::tr("Yes"),QObject::tr("No"),0,1))
	// message
	#define MSG(c,m) QMessageBox::information(this,c,m,QObject::tr("Ok"));

	// Bits...

	#define BIT_0  (0x1)
	#define BIT_1  (0x2)
	#define BIT_2  (0x4)
	#define BIT_3  (0x8)
	#define BIT_4  (0x10)
	#define BIT_5  (0x20)
	#define BIT_6  (0x40)
	#define BIT_7  (0x80)
	#define BIT_8  (0x100)
	#define BIT_9  (0x200)
	#define BIT_10 (0x400)
	#define BIT_11 (0x800)
	#define BIT_12 (0x1000)
	#define BIT_13 (0x2000)
	#define BIT_14 (0x4000)
	#define BIT_15 (0x8000)
	#define BIT_16 (0x10000)
	#define BIT_17 (0x20000)
	#define BIT_18 (0x40000)
	#define BIT_19 (0x80000)
	#define BIT_20 (0x100000)
	#define BIT_21 (0x200000)
	#define BIT_22 (0x400000)
	#define BIT_23 (0x800000)
	#define BIT_24 (0x1000000)
	#define BIT_25 (0x2000000)
	#define BIT_26 (0x4000000)
	#define BIT_27 (0x8000000)
	#define BIT_28 (0x10000000)
	#define BIT_29 (0x20000000)
	#define BIT_30 (0x40000000)
	#define BIT_31 (0x80000000)

#endif //COMMON_HPP

