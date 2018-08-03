/* Classic Ladder Project */
/* Copyright (C) 2001-2018 Marc Le Douarain */
/* http://www.sourceforge.net/projects/classicladder */
/* http://sites.google.com/site/classicladder */
/* February 2001 */
/* ------------------------------ */
/* The main for Linux Application */
/* ------------------------------ */
/* This library is free software; you can redistribute it and/or */
/* modify it under the terms of the GNU Lesser General Public */
/* License as published by the Free Software Foundation; either */
/* version 2.1 of the License, or (at your option) any later version. */

/* This library is distributed in the hope that it will be useful, */
/* but WITHOUT ANY WARRANTY; without even the implied warranty of */
/* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU */
/* Lesser General Public License for more details. */

/* You should have received a copy of the GNU Lesser General Public */
/* License along with this library; if not, write to the Free Software */
/* Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA */

#ifdef MODULE
#include <linux/string.h>
#else
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#endif

#include "classicladder.h"
#include "global.h"
#include "files_project.h"
#include "calc.h"
#include "vars_access.h"
#include "manager.h"
#include "calc_sequential.h"
#include "files_sequential.h"
//#include "config.h"
#include "hardware.h"
#include "socket_server.h"
#include "socket_modbus_master.h"
#include "vars_system.h"
#include "time_and_rtc.h"
#include "frames_log_buffers.h"

#if !defined( MODULE )
#include <sys/types.h>
#include <dirent.h>
#include <errno.h>
#include <signal.h>
#include <getopt.h>
#ifdef __WIN32__
#include <windows.h>
#else
#include <pthread.h>
#include <syslog.h>
#include <sys/wait.h>
#include <fcntl.h>
#endif
#ifdef __XENO__
#include <sys/mman.h>
#endif
#endif

#ifdef GTK_INTERFACE
#include <gtk/gtk.h>
#include <libintl.h>
#include <locale.h>
#include "classicladder_gtk.h"
#include "manager_gtk.h"
#include "menu_and_toolbar_gtk.h"
#include "search.h"
#include "spy_vars_gtk.h"
#endif

#ifdef MAT_CONNECTION
#include "../../lib/plc.h"
#endif

#ifdef COMPLETE_PLC
#include "monitor_threads.h"
#include "log_events.h"
#endif
#include "preferences.h"
#include "tasks.h"
#include "monitor_transfer.h"

#ifdef NO_SEGV_TRAP
#warning "#######################################################"
#warning "#### Beware this version is compiled in DEBUG !!! #####"
#warning "#######################################################"
#endif

int cl_remote;
int nogui = 0;
int ModbusServerPort = 9502; // Standard "502" requires root privileges...

#if !defined( MODULE )

char TheProject[400] = "";
//////#ifdef GTK_INTERFACE
//char TheProject[400] = "projects_examples/example.clprj";
//////#else
//////char TheProject[400] = "projects_examples/parallel_port_direct.clprj";
//////#endif

char QuitClassicLadder = FALSE;

int LifeCounterPeriod = 0;

//////int PipeLaunchExternalCmd[ 2 ];
int PipeLaunchExternalCmd;

void ClassicLadderEndOfAppli( void )
{
	debug_printf("EndOfAppli function...\n");
//////	write( PipeLaunchExternalCmd[ 1 ], "*", 2 ); // TO ASK END CHILD !
	if ( PipeLaunchExternalCmd!=-1 )
	{
		write( PipeLaunchExternalCmd, "*", 2 ); // TO ASK END CHILD !
		close( PipeLaunchExternalCmd );
		PipeLaunchExternalCmd = -1;
	}
	QuitClassicLadder = TRUE;
	StopRunIfRunning( );
	EndTaskWanted( ID_TASK_LOGIC );
	EndTaskWanted( ID_TASK_SCAN_INPUTS );
	EndTaskWanted( ID_TASK_GET_TIME );
printf("wait a little before continuing end...\n");
DoPauseMilliSecs(1000);
//done before	StopRunIfRunning( );
#ifdef COMPLETE_PLC
	EndGeneralMonitor( );
#endif
#ifdef MODBUS_IO_MASTER
	CloseSocketModbusMaster( );
#endif
	CloseSocketServer( );
#ifdef GTK_INTERFACE
	SavePreferences( );
#endif
	ClassicLadder_FreeAllInfos( TRUE/*CleanAndRemoveTmpDir*/ );
	EndHardware( );
	EndGeneralMutex( );
	if ( Preferences.UseRtcDevice )
		CloseRtcDevice( );
	debug_printf("Normal end of appli.\n");
#ifndef __WIN32__
	if ( Preferences.UseSysLog )
		syslog( LOG_USER|LOG_DEBUG, "Normal end of application.\n" );
#endif
}

void HandlerSignalInterrupt( int signal_id )
{
	debug_printf("End of appli asked (signal=%d)\n", signal_id);
#ifndef __WIN32__
	if ( Preferences.UseSysLog )
		syslog( LOG_USER|LOG_DEBUG, "End of application asked with signal=%d.\n",signal_id );
#endif
	// security if ctrl-c loop received ??!!
	if ( !QuitClassicLadder )
		ClassicLadderEndOfAppli( );
	else
		debug_printf("Already under quit...!!!???\n");
//////v0.9.10	exit( 0 );
//v0.9.11, in fact seems to be usefull for ctrl-c with gtk+ interface...?
	exit( 0 );
}
void HandlerSignalSegfault( int signal_id )
{
	debug_printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
	debug_printf("!!! ABNORMAL END OF APPLI (SEGV) !!!\n");
	debug_printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
#ifndef __WIN32__
	if ( Preferences.UseSysLog )
		syslog( LOG_USER|LOG_DEBUG, "ABNORMAL END OF APPLI (SEGV) !!!\n" );
#endif
	ClassicLadderEndOfAppli( );
	exit( 0 );
}
#ifndef __WIN32__
// balayette à zombies
void HandlerSignalSigChld( int signal_id )
{
	while( 0<waitpid(-1, NULL, WNOHANG ) );
}
#endif

void display_help (void)
{
	printf("Usage: classicladder [OPTIONS] [PATH]\n"
			"Start classicladder PLC with an optional project path e.g. myplc\n"
			"\n"
			"           --help     	        display this help and exit\n"
			"           --version  	        output version information and exit\n"
			"           --nogui             do not create the GUI\n"
			"-p port    --modbus_port=port   port to use for modbus server\n"
//			"-c file    --config=file        read PLC configuration from file\n"
			);
//	exit(0);
}

void display_version (void)
{
	printf( CL_PRODUCT_NAME " v" CL_RELEASE_VER_STRING "\n" CL_RELEASE_DATE_STRING "\n"
	       "\n"
               "Copyright (C) " CL_RELEASE_COPYRIGHT_YEARS " Marc Le Douarain\nmarc . le - douarain /At\\ laposte \\DoT/ net\n"
	       "\n"
	       "ClassicLadder comes with NO WARRANTY\n"
	       "to the extent permitted by law.\n"
	       "\n"
	       "You may redistribute copies of ClassicLadder\n"
	       "under the terms of the GNU Lesser General Public Licence.\n"
	       "See the file `lesserGPL.txt' for more information.\n");
//	exit(0);
}

// returns TRUE if must end !
char process_options (int argc, char *argv[])
{
	char ExitProg = FALSE;
	int error = 0;

	for (;;)
	{
		int option_index = 0;
		static const char *short_options = "c:";
		static const struct option long_options[] = {
			{"help", no_argument, 0, 0},
			{"version", no_argument, 0, 0},
			{"nogui", no_argument, 0, 'n'},
//			{"config", required_argument, 0, 'c'},
			{"modbus_port", required_argument, 0, 'p'},
			{0, 0, 0, 0},
		};

		int c = getopt_long(argc, argv, short_options,
				    long_options, &option_index);
		if (c == EOF)
			break;

		switch (c)
		{
			case 0:
				switch (option_index) {
				case 0:
					display_help();
					ExitProg = TRUE;
					break;
				case 1:
					display_version();
					ExitProg = TRUE;
					break;
				}
				break;
			case 'n':
				nogui = 1;
				break;
/*			case 'c':
	#ifndef RT_SUPPORT
				read_config (optarg);
	#else
				printf("Config file is used by the RT module in RTLinux version !!!\n");
	#endif
				break;*/
			case 'p':
				ModbusServerPort = atoi( optarg );
				break;
			case '?':
				error = 1;
				ExitProg = TRUE;
				break;
		}
	}

	if (error)
	{
		display_help();
		ExitProg = TRUE;
	}

	if ((argc - optind) != 0)
		VerifyDirectorySelected( TheProject, argv[optind] );
	return ExitProg;
}

#ifdef GTK_INTERFACE
gboolean RunStopStateForGtk;
#endif
void DoFlipFlopRunStop( void )
{
#ifdef GTK_INTERFACE
	printf("|-> %s() called (connected=%s)\n",__FUNCTION__,InfosGUI->TargetMonitor.RemoteConnected?"yes":"no");
	if ( InfosGUI->TargetMonitor.RemoteConnected )
	{
		// Send order to switch run/stop the target !!!
		InfosGUI->TargetMonitor.AskTargetToSetState = (InfosGUI->TargetMonitor.LatestTargetState==STATE_RUN)?STATE_STOP:STATE_RUN;
printf("STATE WANTED FOR TARGET = %s (current=%s)\n", _ConvStateInString(InfosGUI->TargetMonitor.AskTargetToSetState),_ConvStateInString(InfosGUI->TargetMonitor.LatestTargetState) );
		return; // end of function here...
	}
#endif
	if (InfosGene->LadderState==STATE_RUN)
	{
		printf("In %s(), actually 'RUNNING' => flip in 'STOP' !\n",__FUNCTION__);
		InfosGene->LadderState = STATE_STOP;
		InfosGene->DurationOfLastScan = -1;
#ifdef GTK_INTERFACE
		MessageInStatusBar(_("Stopped program - press run button to continue."));
		RunStopStateForGtk = FALSE;
		SetGtkMenuStateForRunStopSwitch( &RunStopStateForGtk );
#endif
	}
	else
	{
		int ScanSlaveList;
		for( ScanSlaveList=0; ScanSlaveList<NBR_MODBUS_SLAVES ;ScanSlaveList++ )
			InitStatsForSlave( ScanSlaveList );
		printf("In %s(), not 'RUNNING' (state=%d)=> flip in 'RUN' !\n",__FUNCTION__, InfosGene->LadderState);
		InfosGene->LadderState = STATE_RUN;
#ifdef GTK_INTERFACE
		MessageInStatusBar(_("Started program - press stop to pause.")); 
		RunStopStateForGtk = TRUE;
		SetGtkMenuStateForRunStopSwitch( &RunStopStateForGtk );
#endif
	}
	// save new state selected in preferences
	Preferences.LatestStateSaved = InfosGene->LadderState;
	RemountFileSystemRW( );
	SavePreferences( );
	RemountFileSystemRO( );
}
void DoFlipFlopRunOneCycleFreeze( void )
{
#ifdef GTK_INTERFACE
	printf("|-> %s() called (connected=%s)\n",__FUNCTION__,InfosGUI->TargetMonitor.RemoteConnected?"yes":"no");
	if ( InfosGUI->TargetMonitor.RemoteConnected )
	{
		// Send order to switch run/stop the target !!!
		InfosGUI->TargetMonitor.AskTargetToSetState = (InfosGUI->TargetMonitor.LatestTargetState==STATE_RUN || InfosGene->LadderState==STATE_RUN_FOR_ONE_CYCLE)?STATE_RUN_FREEZE:STATE_RUN_FOR_ONE_CYCLE;
printf("STATE WANTED FOR TARGET = %s (current=%s)\n", _ConvStateInString(InfosGUI->TargetMonitor.AskTargetToSetState),_ConvStateInString(InfosGUI->TargetMonitor.LatestTargetState) );
		return; // end of function here...
	}
#endif
	if ( InfosGene->LadderState==STATE_RUN || InfosGene->LadderState==STATE_RUN_FOR_ONE_CYCLE )
	{
		printf("In %s(), actually 'RUNNING' => flip in 'FREEZE' !\n",__FUNCTION__);
		InfosGene->LadderState = STATE_RUN_FREEZE;
#ifdef GTK_INTERFACE
		MessageInStatusBar(_("Freezed program - select run or run one cycle to continue."));
		RunStopStateForGtk = FALSE;
		SetGtkMenuStateForRunStopSwitch( &RunStopStateForGtk );
#endif
	}
	else
	{
		printf("In %s(), not 'RUNNING' => flip in 'RUN FOR ONE CYCLE' !\n",__FUNCTION__);
		InfosGene->LadderState = STATE_RUN_FOR_ONE_CYCLE;
#ifdef GTK_INTERFACE
		MessageInStatusBar(_("Started program for one cycle...")); 
		RunStopStateForGtk = TRUE;
		SetGtkMenuStateForRunStopSwitch( &RunStopStateForGtk );
#endif
	}
}

void StopRunIfRunning( void )
{
	if (InfosGene->LadderState==STATE_RUN)
	{
		debug_printf("Stopping...");
		InfosGene->LadderStoppedToRunBack = TRUE;
		InfosGene->LadderState = STATE_STOP;
		InfosGene->DurationOfLastScan = -1;
		while( InfosGene->UnderCalculationPleaseWait==TRUE )
		{
			DoPauseMilliSecs( 100 );
		}
		debug_printf("done.\n");
	}
}
void RunBackIfStopped( void )
{
	if ( InfosGene->LadderStoppedToRunBack )
	{
		int ScanSlaveList;
		for( ScanSlaveList=0; ScanSlaveList<NBR_MODBUS_SLAVES ;ScanSlaveList++ )
			InitStatsForSlave( ScanSlaveList );
		debug_printf("Start running!\n");
		InfosGene->LadderState = STATE_RUN;
		InfosGene->LadderStoppedToRunBack = FALSE;
	}
}

void DoReset( void )
{
	// reset with monitor only possible on an embedded!
#ifdef GTK_INTERFACE
	if ( InfosGUI->TargetMonitor.RemoteConnected )
	{
		// Send order to switch run/stop the target !!!
		InfosGUI->TargetMonitor.AskTargetToReset = 1;
printf("RESET WANTED OF THE TARGET\n" );
		return;
	}
#endif

	InitVars( TRUE/*DoLogEvents*/ );

//////	int StateBefore = InfosGene->LadderState;
//////	InfosGene->LadderState = STATE_STOP;
//////	// wait, to be sure calcs have ended...
//////	usleep( 100000 );
	StopRunIfRunning( );

//v0.9.4, moved before to log bools reset!!!	InitVars( );
	InitSystemVars( FALSE );
	PrepareAllDatasBeforeRun( );

//////	if ( StateBefore==STATE_RUN )
//////		InfosGene->LadderState = STATE_RUN;
	RunBackIfStopped( );
#ifdef GTK_INTERFACE
	MessageInStatusBar( (InfosGene->LadderState==STATE_RUN)?(_("Reset logic data - Now running.")):(_("Reset logic data done.")) );
#endif
}

void LifeLed( )
{
	int CounterMax = ( ( InfosGene->LadderState==STATE_RUN || InfosGene->LadderState==STATE_RUN_FOR_ONE_CYCLE ) && !InfosGene->AskToConfHardInputs)?500:100;
	if ( LifeCounterPeriod>=CounterMax )
	{
		LifeCounterPeriod = 0;
		if( InfosGene->LifeLedState )
			InfosGene->LifeLedState = FALSE;
		else
			InfosGene->LifeLedState = TRUE;
	}
	LifeCounterPeriod = LifeCounterPeriod+GeneralParamsMirror./*InfosGene->GeneralParams.*/PeriodMilliSecsTaskLogic;
//printf("life led, state=%s, period=%d(+%d), max=%d\n", InfosGene->LifeLedState?"ON":"OFF", LifeCounterPeriod, GeneralParamsMirror./*InfosGene->GeneralParams.*/PeriodMilliSecsTaskLogic, CounterMax);
}

// ============================
// === INPUTS PERIODIC TASK ===
// ============================
void CyclicCalcRefreshOfScanInputsTask( void )
{
	do
	{

		PeriodicTaskCycleStart( ID_TASK_SCAN_INPUTS );
		if ( TaskIsRunning( ID_TASK_SCAN_INPUTS ) )
		{
			
			if ( InfosGene->LadderState==STATE_RUN || InfosGene->LadderState==STATE_RUN_FOR_ONE_CYCLE )
			{
#ifdef GTK_INTERFACE
				if ( !GeneralParamsMirror.RealInputsOutputsOnlyOnTarget )
#endif
				{
					// called here, because ioperm seems requiring to be defined in this thread using them.
					if ( InfosGene->AskToConfHardInputs )
					{
						ConfigHardware( 0/*ForOutputs*/ );
						InfosGene->AskToConfHardInputs = FALSE;
					}

					ReadPhysicalInputs();
				}
#ifdef GTK_INTERFACE
				else
				{
					InfosGene->AskToConfHardInputs = FALSE;
				}
#endif
			}
		}
		PeriodicTaskCycleEnd( ID_TASK_SCAN_INPUTS );
	}
	while( TaskIsRunning( ID_TASK_SCAN_INPUTS ) );
printf("====> INPUTS TASK EXIT.\n");
	TaskExit( ID_TASK_SCAN_INPUTS );
}
// ==================================
// === LOGIC/OUTPUTS PERIODIC TASK ==
// ==================================
void CyclicCalcRefreshOfLogicTask( void )
{
	ConfigHardwareForLifeUsersLeds( );

	do
	{

		PeriodicTaskCycleStart( ID_TASK_LOGIC );
		if ( TaskIsRunning( ID_TASK_LOGIC ) )
		{
			
			LifeLed( );
			
			if ( ( InfosGene->LadderState==STATE_RUN || InfosGene->LadderState==STATE_RUN_FOR_ONE_CYCLE ) && !InfosGene->AskToConfHardInputs )
			{
#ifdef GTK_INTERFACE
				if ( !GeneralParamsMirror.RealInputsOutputsOnlyOnTarget )
#endif
				{
					// called here, because ioperm seems requiring to be defined in this thread using them.
					if ( InfosGene->AskToConfHardOutputs )
					{
						ConfigHardware( 1/*ForOutputs*/ );
						InfosGene->AskToConfHardOutputs = FALSE;
					}
				}
#ifdef GTK_INTERFACE
				else
				{
					InfosGene->AskToConfHardOutputs = FALSE;
				}
#endif

				InfosGene->UnderCalculationPleaseWait = TRUE;
//////				ReadPhysicalInputs();
				CopyInputsStatesToInputsVars( );
				UpdateSystemVars( );
				ClassicLadder_RefreshAllSections();
#ifdef GTK_INTERFACE
				if ( !GeneralParamsMirror.RealInputsOutputsOnlyOnTarget )
#endif
				{
					WritePhysicalOutputs( FALSE/*OnlyForLifeLed*/ );
				}
				InfosGene->UnderCalculationPleaseWait = FALSE;
				
				// it was just for one cycle, so freeze now !
				if ( InfosGene->LadderState==STATE_RUN_FOR_ONE_CYCLE )
				{
					DoFlipFlopRunOneCycleFreeze( );
				}

			}//if (InfosGene->LadderState==STATE_RUN
			else
			{
#ifndef GTK_INTERFACE
				WritePhysicalOutputs( TRUE/*OnlyForLifeLed*/ );
#endif
			}

			SecurityForAbortedTransferToSlave( );
		}
		PeriodicTaskCycleEnd( ID_TASK_LOGIC );
	}
	while( TaskIsRunning( ID_TASK_LOGIC ) );
printf("====> LOGIC TASK EXIT.\n");
	TaskExit( ID_TASK_LOGIC );
}

// ===========================
// === GET TIME DATAS TASK ===
// ===========================
// (periodic thread created to not be real-time...)
void CyclicCalcGetTimeDatasTask( void )
{
	do
	{

		if ( TaskIsRunning( ID_TASK_GET_TIME ) )
		{
			GetTimeDatasInThread( Preferences.UseRtcDevice );
			if ( tm_copy_for_vars_sys.tm_wday!=-1 )
			{
//printf("%s(): %d/%d/%d %02d:%02d:%02d (%d)\n", __FUNCTION__, tm_copy_for_vars_sys.tm_year%100,tm_copy_for_vars_sys.tm_mon+1,tm_copy_for_vars_sys.tm_mday, tm_copy_for_vars_sys.tm_hour,tm_copy_for_vars_sys.tm_min,tm_copy_for_vars_sys.tm_sec, (int)CurrentIntTimeCopy );
				if ( GeneralParamsMirror.AutomaticallyAdjustSummerWinterTime )
				{
					VerifyAutoAdjustSummerWinterTime( );
				}
			}
			
RemoteAlarmsTransmitForEmails( );
		}
		DoPauseMilliSecs( 250 );
	}
	while( TaskIsRunning( ID_TASK_GET_TIME ) );
printf("====> TIME TASK EXIT.\n");
	TaskExit( ID_TASK_GET_TIME );
}

void HardwareActionsAfterProjectLoaded( void )
{
#ifndef RT_SUPPORT
#ifdef GTK_INTERFACE
	if ( !GeneralParamsMirror.RealInputsOutputsOnlyOnTarget )
#endif
	{
		OpenHardware( 0 );
		InfosGene->AskToConfHardInputs = TRUE;
		InfosGene->AskToConfHardOutputs = TRUE;
	}
	// added here in v0.9.9
	ConfigSerialModbusMaster( );
#endif
	CleanupEventsLog( );
	//added in 0.9.101
	WriteBoolVarsTo1FromPendingEventsLog( );
	//added in 0.9.20
	AdjustPeriodicValuesForTasks( );
	//moved here in 0.9.20 after project datas allocated !
	LoadSetsVarsList( );
}

void AdjustPeriodicValuesForTasks( void )
{
	printf("Set periodic times values for threads: inputs=%dms , logic=%dms\n",GeneralParamsMirror.PeriodMilliSecsTaskScanInputs, GeneralParamsMirror.PeriodMilliSecsTaskLogic);
	SetPeriodicValueForThread( ID_TASK_SCAN_INPUTS, GeneralParamsMirror.PeriodMilliSecsTaskScanInputs*1000 );
	SetPeriodicValueForThread( ID_TASK_LOGIC, GeneralParamsMirror.PeriodMilliSecsTaskLogic*1000 );
}

int ClassicLadderMain( int argc, char *argv[] )
{
	int ReturnCode = 0;
	char AllocMemoryOk = TRUE;
	LogEventsDataFile[ 0 ] = '\0';
	// moved here in v0.9.20 to avoir test everywhere after... and cases not tested in monitor_tranfer for TmpDirectoryRoot, so...
	InitTempDir( );
	InitLogDatas( );
	InitPreferences( );
	LoadPreferences( );
#ifndef __WIN32__
	if ( Preferences.UseSysLog )
	{
		openlog( "ClassicLadder", LOG_CONS|LOG_PERROR, LOG_USER );
		syslog( LOG_USER|LOG_DEBUG, "Starting... (default project=%s)\n", Preferences.DefaultProjectFileNameToLoadAtStartup );
	}
	if ( Preferences.Daemonize )
		daemon( 1, 1 );
#endif
#ifdef __XENO__
	mlockall(MCL_CURRENT | MCL_FUTURE);
#endif

	if ( Preferences.UseRtcDevice )
	{
		if ( !OpenRtcDevice( ) )
		{
//Dec.2016, seen with MSYS2/Win32!!!
#ifndef __WIN32__
			if ( Preferences.UseSysLog )
				syslog( LOG_USER|LOG_DEBUG, "Failed to open RTC device !!!" );
#endif
		}
	}
	InitTimeAndRtc( );
	InitGeneralMutex( );
	InitTasks( );
	if ( !InitHardware( ) )
	{
#ifndef __WIN32__
		if ( Preferences.UseSysLog )
			syslog( LOG_USER|LOG_DEBUG, "Init hardware error !!!\n");
#endif
		return -1;
	}
#ifdef MODBUS_IO_MASTER
	InitModbusMasterBeforeReadConf( );
#endif
#ifdef COMPLETE_PLC
	InitAllFramesLogBuffers( );
#endif
	if ( Preferences.DefaultProjectFileNameToLoadAtStartup[ 0 ]!='\0' )
		strcpy( TheProject, Preferences.DefaultProjectFileNameToLoadAtStartup );
	if ( process_options (argc, argv) )
	{
		return -1;
	}

//v0.9.20	InitGeneralParamsMirror( );
	// here we get the sizes defined in the project...
	// (before doing the allocations !)
//v0.9.20	LoadGeneralParamsOnlyFromProject( TheProject );

	AllocMemoryOk = ClassicLadder_AllocAllInfos() /*v0.9.20 && ClassicLadder_AllocProjectDatas()*/;
	if ( AllocMemoryOk )
	{
		// now InfosGene allocated...
		strcpy( InfosGene->CurrentProjectFileName, TheProject );
		
//v0.9.20 project datas not yet allocated here...		LoadSetsVarsList( );
		LoadLogEventsData( );

		InitSocketServer( 0/*UseUdpMode*/, ModbusServerPort/*PortNbr*/ );
#ifdef MODBUS_IO_MASTER
		InitSocketModbusMaster( );
#endif
#ifdef COMPLETE_PLC
		InitGeneralMonitor( /*ListenPortForSlave*/9000, /*InitOnlyTheSlave*/FALSE );
#endif

		signal( SIGINT /*SIGTERM*/, HandlerSignalInterrupt );
		signal( SIGTERM, HandlerSignalInterrupt );
#ifndef NO_SEGV_TRAP
		signal( SIGSEGV, HandlerSignalSegfault );
#endif

//printf( "tasks periods (GeneralParams): inputs=%dms , logic=%dms\n", InfosGene->GeneralParams.PeriodMilliSecsTaskScanInputs, InfosGene->GeneralParams.PeriodMilliSecsTaskLogic );
printf( "Tasks periods (GeneralParams): inputs=%dms , logic=%dms\n", GeneralParamsMirror.PeriodMilliSecsTaskScanInputs, GeneralParamsMirror.PeriodMilliSecsTaskLogic );
// cyclic thread for real-time is in the kernel (RTLinux/RTAI case, not for Xenomai easily also done here!)
#ifndef RT_SUPPORT
		if ( CreateTask( ID_TASK_SCAN_INPUTS, "CL_INPUTS", 97/*Priority*/, 16/*StackSizeInKo*/, GeneralParamsMirror./*InfosGene->GeneralParams.*/PeriodMilliSecsTaskScanInputs*1000, ( void * (*)(void *) )CyclicCalcRefreshOfScanInputsTask )
			&& CreateTask( ID_TASK_LOGIC, "CL_LOGIC", 98/*Priority*/, 16/*StackSizeInKo*/, GeneralParamsMirror./*InfosGene->GeneralParams.*/PeriodMilliSecsTaskLogic*1000, ( void * (*)(void *) )CyclicCalcRefreshOfLogicTask )
			&& CreateTask( ID_TASK_GET_TIME, "CL_GET_TIME", 99/*Priority*/, 16/*StackSizeInKo*/, 0, ( void * (*)(void *) )CyclicCalcGetTimeDatasTask ) )
#endif
		{
			char WaitHere = TRUE;
			char ProjectLoadedOk = FALSE;
			// even if project loaded ok, run it only if saved as 'run' in preferences (latest state wanted)
			char ProjectLoadedToRun = FALSE;

#ifdef GTK_INTERFACE
			if ( nogui==0 )
			{
				InitSearchDatas( );
				InitGtkWindows( argc, argv );
#ifndef __WIN32__
				if ( Preferences.UseSysLog )
					syslog( LOG_USER|LOG_DEBUG, "Init Gtk Windows Done\n" );
#endif
			}
#endif

			if ( TheProject[ 0 ]!='\0' )
			{
				//TODO: perhaps pass here TheProject as parameter, and is copied internally
				//into InfosGene->CurrentProjectFileName in the function LoadProjectFiles() ?!
				//lot of works to do in the gtk part for LoadProjectFiles & SaveProjectFiles...
				ProjectLoadedOk = LoadProjectFiles( InfosGene->CurrentProjectFileName );
				ProjectLoadedToRun = ProjectLoadedOk && Preferences.LatestStateSaved==STATE_RUN;
				if ( !ProjectLoadedOk )
					AllocMemoryOk = ClassicLadder_AllocProjectDatas();

#ifdef GTK_INTERFACE
				if ( nogui==0 )
				{
					UpdateAllGtkWindows( );
					MessageInStatusBar( ProjectLoadedOk?(_("Project loaded and running")):(_("Project failed to load...")) );
					RunStopStateForGtk = ProjectLoadedToRun;
					SetGtkMenuStateForRunStopSwitch( &RunStopStateForGtk );
				}
#endif

				if ( ProjectLoadedOk )
				{
					HardwareActionsAfterProjectLoaded( );
				}
			}//if ( TheProject[ 0 ]!='\0' )
			else
			{
				AllocMemoryOk = ClassicLadder_AllocProjectDatas();
#ifdef GTK_INTERFACE
				ManagerDisplaySections( TRUE/*ForgetSectionSelected*/ );
#endif
			}
			if ( AllocMemoryOk )
			{
				// even if project not loaded / not to run, important to set project in 'stop' (to allow refresh, etc...)
				InfosGene->LadderState = ProjectLoadedToRun?STATE_RUN:STATE_STOP;

#ifdef GTK_INTERFACE
				//ProblemWithPrint		gdk_threads_enter( );
				if ( nogui==0 )
				{
					UpdateWindowTitleWithProjectName( );
					UpdateAllLabelsFreeVars( -1/*OnlyThisOne*/, NULL );
					//added in v0.9.20, because not done during windows init (project datas not allocated now...)
					UpdateAllLabelsBoolsVars( -1/*OnlyThisColumn*/ );
					gtk_main();
//Dec.2016, seen during MSYS2/Win32!!! gdk_threads_leave();
					WaitHere = FALSE;
				}
				//ProblemWithPrint		gdk_threads_leave( );
#endif
				if ( WaitHere )
				{
					debug_printf("Press CTRL-C or send 'kill' signal to end...!\n");
					while( !QuitClassicLadder )
					{
						DoPauseMilliSecs( 100 );
						if ( InfosGene->BackgroundSaveSetVarsList )
						{
							InfosGene->BackgroundSaveSetVarsList = FALSE;
debug_printf("Asked to save set vars list...\n");
							if ( !InfosGene->DoNotSaveDuringLoadingSetVarsList )
							{
								RemountFileSystemRW( );
								SaveSetsVarsList( );
								RemountFileSystemRO( );
							}
							else
							{
								InfosGene->DoNotSaveDuringLoadingSetVarsList = FALSE;
							}
						}
						if ( InfosGene->BackgroundSaveLogEventsData )
						{
							InfosGene->BackgroundSaveLogEventsData = FALSE;
debug_printf("Asked to save log events data...\n");
							RemountFileSystemRW( );
							SaveLogEventsData( );
							RemountFileSystemRO( );
						}
					}
					debug_printf("Go out of wait main.\n");
				}//if ( WaitHere )
			}//if ( AllocMemoryOk )

		}
		else
		{
#ifndef __WIN32__
			if ( Preferences.UseSysLog )
				syslog( LOG_USER|LOG_DEBUG, "Failed to init tasks !!!\n" );
#endif
			ClassicLadder_FreeProjectDatas( );
			ClassicLadder_FreeAllInfos( TRUE/*CleanAndRemoveTmpDir*/ );
			ReturnCode = -1;
		}
	}
	
	if ( !AllocMemoryOk )
	{
#ifndef __WIN32__
		if ( Preferences.UseSysLog )
			syslog( LOG_USER|LOG_DEBUG, "Failed to alloc memory required !!!\n" );
#endif
		ClassicLadder_FreeProjectDatas( );
		ClassicLadder_FreeAllInfos( TRUE/*CleanAndRemoveTmpDir*/ );
		ReturnCode = -1;
	}

#ifndef __WIN32__
	if ( Preferences.UseSysLog )
		syslog( LOG_USER|LOG_DEBUG, "End of wait main.\n" );
#endif
	return ReturnCode;
}
#endif


// usefull under Linux to create a new process listening on a pipe external commands to launch...
// trouble with Xeno3 (when fork to launch execv), so now done an external 'ClassicLauncher' application to do the work!
int main( int argc, char *argv[] )
{
#ifdef AAAAAAAAAAAAAAAAAAAAAAAA

#if !defined( __WIN32__ ) && !defined( NO_SEGV_TRAP )
	int pid;
	if ( pipe( PipeLaunchExternalCmd )!=0 )
	{
		printf("Failed to init pipe for external launch command !\n");
		exit(1);
	}
	switch (pid = fork())
	{
		case 0: /* child */
			//dup2(PipeLaunchExternalCmd[0], 0);
			close(PipeLaunchExternalCmd[1]); /* the child does not need this end of the pipe (write) */
#ifndef __WIN32__
			signal( SIGCHLD, HandlerSignalSigChld );
#endif
printf("MAIN: CHILD, calling TreatExternalCmd()\n");
			TreatExternalCommandToLaunch( );
printf("MAIN: CHILD, end for calling TreatExternalCmd()\n");
			break;
		default: /* parent */
			//dup2(PipeLaunchExternalCmd[1], 1);
			close(PipeLaunchExternalCmd[0]); /* the parent does not need this end of the pipe (read) */
printf("MAIN: PARENT, calling Main()\n");
			if ( ClassicLadderMain( argc, argv )==-1 )
			{
printf("MAIN: PARENT, Main() return an error or request an exit...\n");
				write( PipeLaunchExternalCmd[ 1 ], "*", 2 ); // TO ASK END CHILD !
			}
printf("MAIN: PARENT, end for calling Main()\n");
			break;
		case -1:
			printf("Failed to fork for external launch command !\n");
			exit(1);
	}
	return 0;
#else
#endif

#endif

// will block if can not open ?!
#if !defined( GTK_INTERFACE ) && !defined( __WIN32__ ) && !defined( NO_SEGV_TRAP )
	PipeLaunchExternalCmd = open( FIFO_CLASSICLAUNCHER, O_WRONLY );
	if ( PipeLaunchExternalCmd==-1 )
	{
		printf("Failed to open fifo-pipe %s for external launch command !\n", FIFO_CLASSICLAUNCHER);
	}
#endif
	return ClassicLadderMain( argc, argv );
//#endif
}
