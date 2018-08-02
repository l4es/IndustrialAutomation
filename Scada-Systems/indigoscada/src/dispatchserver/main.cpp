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

#ifdef WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#undef WIN32_LEAN_AND_MEAN
#endif
#include <qt.h>
#include "IndentedTrace.h"
#include "general_defines.h"
#include "dispatcher.h"
#include "link.xpm"
#include "utilities.h"

int main(int argc, char **argv)
{
	IT_IT("main - DISPATCHER");

	int stat = -1;

	char version[100];
	//version control///////////////////////////////////////////////////////////////
    sprintf(version, "dspserver.exe - Build: %s %s at enscada.com",__DATE__,__TIME__);
    fprintf(stdout, "%s\n", version);
	SYSTEMTIME oT;
	::GetLocalTime(&oT);
	fprintf(stdout,"%02d/%02d/%04d, %02d:%02d:%02d Starting ... %s\n",oT.wMonth,oT.wDay,oT.wYear,oT.wHour,oT.wMinute,oT.wSecond,SYSTEM_NAME_DISPATCHER); 
	fflush(stdout);
		
	if(!IsSingleInstance(""SYSTEM_NAME_DISPATCHER""))
	{
		IT_COMMENT("Another instance of the dispatcher server is already running!");//error message
		return stat;
	}

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
				//
				//
				//
				#ifdef UNIX
				signal(SIGTERM,SigTermHandler);						
				#endif

				{
					DispatcherServer dsp;
					dsp.run(argc, argv);

					stat = a.exec();
				}
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