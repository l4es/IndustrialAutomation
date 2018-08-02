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
*User interface main routine
*Purpose:
*/
#include "dispatch.h"
#include "user.h"
#include "realtimedb.h"
#include "messages.h"
#include "main.h"
#include "general_defines.h"
#include "utilities.h"
#include "IndentedTrace.h"
#include "historicdb.h"
//
//
bool QSApplication::fKeyMouseEvent = 1;
bool QSApplication::notify(QObject *o, QEvent *e) // notification handler 
{
	switch(e->type())
	{
		case QEvent::MouseMove:
		case QEvent::KeyPress:
		{
			fKeyMouseEvent = 0;
		};
		break;
		default:
		break;
	};
	return QApplication::notify(o,e);
};

//
//
int main(int argc, char **argv)
{
	IT_IT("main - UI");
	
	int stat = -1;  

	char version[100];
	//version control///////////////////////////////////////////////////////////////
    sprintf(version, "ui.exe - Build: %s %s at enscada.com",__DATE__,__TIME__);
    fprintf(stdout, "%s\n", version);
	SYSTEMTIME oT;
	::GetLocalTime(&oT);
	fprintf(stdout,"%02d/%02d/%04d, %02d:%02d:%02d Starting ... %s\n",oT.wMonth,oT.wDay,oT.wYear,oT.wHour,oT.wMinute,oT.wSecond,SYSTEM_NAME_HMI); 
	fflush(stdout);
		
	if(!IsSingleInstance(""SYSTEM_NAME_HMI""))
	{
		IT_COMMENT("Another instance of the HMInterface is already running!");//error message
		return stat;
	}
	
	SetScadaHomeDirectory(argv[0]);
	
	QSApplication a(argc, argv);
	  
	#ifdef UNIX
	a.setFont(QFont("helvetica", 12));
	#endif
	#ifdef WIN32
	a.setFont(QFont("Arial", 10));
	#endif
	//
	// Set to Windows Style 
	//  
	a.setStyle( new QWindowsStyle);
	//
	// open connections
	// 
	if(OpenRealTimeConnections() && 
	   OpenDispatcherConnection())
	{
		UserFrameWork *mdiframework=new UserFrameWork();
		a.setMainWidget(mdiframework);
		//mdiframework->show();
		//mdiframework->showIntro();
		//QTimer::singleShot(50000, mdiframework, SLOT(closeIntro()));
		mdiframework->setGeometry(QApplication::desktop()->geometry());// maximise to full screen 
		//DeskW = a.desktop()->width();
		//DeskH = a.desktop()->height();

		if(OpenSpareDispatcherConnection())
		{
			//do somethig
		}

		if(OpenSpareRealTimeConnections())
		{
		
		}

		OpenHistoricConnections();

		stat = a.exec();
		//
		//
		#ifdef STL_BUG_FIXED
		CloseRealTimeConnections();
		#endif

		#ifdef STL_BUG_FIXED
		CloseDispatcherConnection();
		#endif

		if(GetHistoricResultDb() != NULL)
		{
			#ifdef STL_BUG_FIXED
			CloseHistoricConnections();
			#endif
		}
		//
		if(GetSpareDispatcher() != NULL)
		{
			#ifdef STL_BUG_FIXED
			CloseSpareDispatcherConnection();
			#endif
		}

		if((GetSpareConfigureDb() != NULL) && (GetSpareCurrentDb() != NULL)&&(GetSpareResultDb() != NULL))
		{
			#ifdef STL_BUG_FIXED
			CloseSpareRealTimeConnections();
			#endif
		}

		//
		UnloadAllDlls(); // unload Dlls

#ifdef WIN32
#ifdef NDEBUG
		if(mdiframework)
			delete mdiframework;
#endif
#endif
		
		return stat;
	}
	else
	{
		//cerr << "Failed to connect to database(s) and (or) dispatcher" << endl;
		MessageBox(NULL,"Failed to connect to database(s) and (or) dispatcher","UI", MB_OK|MB_ICONSTOP);
		IT_COMMENT("Failed to connect to database(s) and (or) dispatcher");//error messag
	};
	
	return stat;
}

