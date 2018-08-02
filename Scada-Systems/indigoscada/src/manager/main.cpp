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

#include <qt.h>
#include "IndentedTrace.h"
#include "general_defines.h"
#include "utilities.h"
#include "buttongroups.h"
#include "IndentedTrace.h"

extern void SetScadaHomeDirectory(const QString &s);

int main( int argc, char **argv )
{
	IT_IT("main - MANAGER");

	int stat = -1; 
		
	if(!IsSingleInstance(""SYSTEM_NAME_MANAGER""))
	{
		IT_COMMENT("Another instance of the manager is already running!");//error message
		return stat;
	}

	SetScadaHomeDirectory(argv[0]);		
	// 
	// if the DISPLAY variable is empty or not set then we go into none-GUI mode
	// this application can run GUI mode or non-GUI mode
	// it is expected that the GUI mode is started when we want to do debugging
	//   
	#ifdef UNIX
	bool useGUI = (getenv( "DISPLAY" ) != 0) && (strlen(getenv( "DISPLAY" )) > 0);
	//
	//
	if(!useGUI)
	{
		setenv("DISPLAY","localhost:0",1); 
	};
	//
	QApplication a(argc, argv,useGUI);
	#else
	QApplication a(argc, argv);
	#endif
	//
	//
	//
//		if(!chdir(QSFilename(""))) // change directory   
//		{
		//
		// connect to the databases uid = 0 for root 
		//if(getuid() > QS_MIN_UID)
		{
			ButtonsGroups buttonsgroups;
			buttonsgroups.resize( 500, 250 );
			buttonsgroups.setCaption( "IndigoSCADA task manager" );
			a.setMainWidget( &buttonsgroups );
			buttonsgroups.show();
			//
			//
			//
			#ifdef UNIX
			signal(SIGTERM,SigTermHandler);						
			#endif

			stat = a.exec();
			//
			return stat;
			//
		}
		//else
		//{
		//      cerr << "Must Not Run As Root" << endl;
		//}
//		}
//		else
//		{
//			cerr << "User Directory Not Accessible:" << (const char *) QSFilename("") << endl;
//			
//			QString err_msg;
//			err_msg = "User Directory Not Accessible:" + QSFilename("")+ "\n";
//			IT_COMMENT((const char *)err_msg);
//		}

	return stat;
}
