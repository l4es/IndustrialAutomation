/* Classic Ladder Project */
/* Copyright (C) 2001-2015 Marc Le Douarain */
/* http://www.sourceforge.net/projects/classicladder */
/* http://sites.google.com/site/classicladder */
/* January 2012 */
/* ----------------------------------------------- */
/* Tasks management (create/delete/period)         */
/* For Posix (Linux / real-time Xenomai) & Windows */
/* ----------------------------------------------- */
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



#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#ifdef __WIN32__
#include <windows.h>
#else
#include <pthread.h>
#include <syslog.h>
#include <unistd.h>
#include <errno.h>
#endif

#include "tasks.h"
#include "classicladder.h"
#include "global.h"

char ThreadRunning[ NBR_TASKS ];
unsigned int PeriodThreadValue[ NBR_TASKS ]; // in micro-secs units
#ifdef __WIN32__
HANDLE ThreadHandle[ NBR_TASKS ];
DWORD ThreadId[ NBR_TASKS ];
CRITICAL_SECTION MutexArray[ NBR_MUTEXES ];
#else
pthread_t PosixThread[ NBR_TASKS ];
pthread_mutex_t MutexArray[ NBR_MUTEXES ];
#endif

//char * LaunchScriptParamList[ 30 ]; // First = NULL if no parameters to command to launch...
char LaunchExternalCommandBuff[ 3000 ];
int LaunchExternalCommandBuffIndex = 0;
char * LaunchExternalCommandArgv[ 30 ];
int LaunchExternalCommandArgvIndex = 0;

void InitTasks( void )
{
	int ScanTask;
	for( ScanTask=0; ScanTask<NBR_TASKS; ScanTask++ )
	{
		ThreadRunning[ ScanTask ] = 0;
		PeriodThreadValue[ ScanTask ] = 0;
#ifdef __WIN32__
		ThreadHandle[ ScanTask ] = NULL;
#endif
	}
}


// Period=-1 if not a periodic task
char CreateTask( int IdTask, char * TaskName, int Priority, int StackSizeInKo, unsigned int PeriodMicroSecs, void * (*pTaskFunction)(void *) )
{
	char InitOk = 0;
	if( IdTask>=0 && IdTask<NBR_TASKS )
	{
#ifndef __WIN32__
		int Error = 0;
#endif
		printf("Creating thread%d(%s)...\n",IdTask,TaskName);
		ThreadRunning[ IdTask ] = 1;
		PeriodThreadValue[ IdTask ] = PeriodMicroSecs;
#ifdef __WIN32__
		ThreadHandle[ IdTask ] = CreateThread( NULL/*no security attributes*/, StackSizeInKo*1024L/*default stack size*/,                                                   
				(LPTHREAD_START_ROUTINE)/* thread function*/pTaskFunction, 
				NULL/*argument to thread function*/,                
				THREAD_QUERY_INFORMATION/*use default creation flags*/,                           
				&ThreadId[ IdTask ]/* returns the thread identifier*/);                
		if ( ThreadHandle[ IdTask ]==NULL )
#else
		pthread_attr_t ThreadAttributes;
		pthread_attr_init(&ThreadAttributes);
		pthread_attr_setdetachstate(&ThreadAttributes, PTHREAD_CREATE_DETACHED /*PTHREAD_CREATE_JOINABLE*/);
#ifdef __XENO__
		pthread_attr_setschedpolicy(&ThreadAttributes, SCHED_FIFO);
		struct sched_param paramA = { .sched_priority = Priority };
		pthread_attr_setschedparam(&ThreadAttributes, &paramA);
#endif
		pthread_attr_setstacksize(&ThreadAttributes, StackSizeInKo*1024);
		Error = pthread_create( &PosixThread[ IdTask ], &ThreadAttributes, (void *(*)(void *))pTaskFunction, (void *)NULL );
		if (Error) // pthread_create returns 0 on success
#endif
		{
			printf("Failed to create thread%d(%s)... !!!!!\n",IdTask,TaskName);
		}
		else
		{
#ifdef __XENO__
//			pthread_set_name_np( pthread_self(), __FUNCTION__ );
			pthread_set_name_np( PosixThread[ IdTask ], TaskName );
#endif
			if ( PeriodMicroSecs!=0 )
				SetPeriodicValueForThread( IdTask, PeriodMicroSecs );
			printf( "Created thread%d(%s) ok.\n", IdTask, TaskName );
			InitOk = 1;
		}
	}
	else
	{
		printf("Task creation overflow !!!!!\n");
	}
	return InitOk;
}

// to change period of a task later... after reading parameter in file project for example!
void SetPeriodicValueForThread( int IdTask, unsigned int PeriodMicroSecs )
{
	if( IdTask>=0 && IdTask<NBR_TASKS )
	{
		PeriodThreadValue[ IdTask ] = PeriodMicroSecs;
#ifdef __XENO__
		struct timespec start, period;
		int err = 0;
//TODO: support PeriodicRefreshMilliSecs instead of PERIODIC_REFRESH_MS_DEF...
		clock_gettime( CLOCK_REALTIME, &start );
		start.tv_sec += 1;	/* Start in one second from now. */
		period.tv_sec = 0;
		period.tv_nsec = PeriodMicroSecs*1000;
//		if ( pthread_make_periodic_np(pthread_self(), &start, &period)!=0 )
		if ( pthread_make_periodic_np(PosixThread[ IdTask ], &start, &period)!=0 )
		{
			printf("Xenomai make_periodic failed for thread%d: err %d\n", IdTask, err);
			exit(EXIT_FAILURE);
		}
		else
		{
			printf( "Set periodic thread%d period=%dusecs ok.\n", IdTask, PeriodMicroSecs );
		}
#endif
	}
}

// to be called at a start of a periodic task
#ifdef __XENO__
struct timespec start_logic_task;
#endif
void PeriodicTaskCycleStart( int IdTask )
{
#ifdef __XENO__
	int err = 0;
	unsigned long overruns;
	err = pthread_wait_np(&overruns);
	if (err || overruns) {
		printf( "Xenomai wait_period failed for thread%d: err %d, overruns: %lu\n", IdTask, err, overruns);
//			exit(EXIT_FAILURE);
	}
	if ( IdTask==ID_TASK_LOGIC )
		clock_gettime( CLOCK_REALTIME, &start_logic_task );
#else
	int ValPauseMilliSecs = PeriodThreadValue[ IdTask ]/1000;
	if ( ValPauseMilliSecs<1 )
		ValPauseMilliSecs = 1;
	DoPauseMilliSecs( ValPauseMilliSecs );
#endif
}
void PeriodicTaskCycleEnd( int IdTask )
{
#ifdef __XENO__
	if ( IdTask==ID_TASK_LOGIC )
	{
		struct timespec now;
		clock_gettime( CLOCK_REALTIME, &now );
		if ( now.tv_sec==start_logic_task.tv_sec )
			InfosGene->DurationOfLastScan = now.tv_nsec-start_logic_task.tv_nsec;
	}
#endif
}
void EndTaskWanted( int IdTask )
{
	if( IdTask>=0 && IdTask<NBR_TASKS )
	{
		ThreadRunning[ IdTask ] = 0;
#ifdef __WIN32__
		if ( ThreadHandle[ IdTask ] )
			TerminateThread( ThreadHandle[ IdTask ], 0);
#endif
	}
}

void TaskExit( int IdTask )
{
	if( IdTask>=0 && IdTask<NBR_TASKS )
	{
#ifndef __WIN32__
		pthread_exit(NULL);
#endif
	}
}

char TaskIsRunning( int IdTask )
{
	if( IdTask>=0 && IdTask<NBR_TASKS )
	{
		return ThreadRunning[ IdTask ];
	}
	return 0;
}


void DoPauseMilliSecs( int MilliSecsTime )
{
#ifdef __WIN32__
	Sleep( MilliSecsTime );
#else
	struct timespec time;
	int NbrSecs =0;
	int NbrNanos = MilliSecsTime*1000000;
	if ( MilliSecsTime>=1000 )
	{
		NbrSecs = MilliSecsTime/1000;
		NbrNanos = (MilliSecsTime%1000)*1000000;
	}
	time.tv_sec = NbrSecs;
	time.tv_nsec = NbrNanos;
	nanosleep( &time, NULL );
	//usleep( Time*1000 );
#endif
}


void InitGeneralMutex( void )
{
	int ScanMutex;
	for( ScanMutex=0; ScanMutex<NBR_MUTEXES; ScanMutex++ )
		CreateTheMutex( ScanMutex );
}
void EndGeneralMutex( void )
{
	int ScanMutex;
	for( ScanMutex=0; ScanMutex<NBR_MUTEXES; ScanMutex++ )
		EndTheMutex( ScanMutex );
}

int CreateTheMutex( int IdMutex )
{
	if( IdMutex>=0 && IdMutex<NBR_MUTEXES )
	{
#ifdef __WIN32__
		InitializeCriticalSection( &MutexArray[ IdMutex ]);
#else
		pthread_mutex_init( &MutexArray[ IdMutex ], NULL );
#endif
	}
	return 1;
}
void EndTheMutex( int IdMutex )
{
	if( IdMutex>=0 && IdMutex<NBR_MUTEXES )
	{
#ifdef __WIN32__
		DeleteCriticalSection( &MutexArray[ IdMutex ] );
#else
		pthread_mutex_destroy( &MutexArray[ IdMutex ] );
#endif
	}
}
void LockTheMutex( int IdMutex )
{
	if( IdMutex>=0 && IdMutex<NBR_MUTEXES )
	{
#ifdef __WIN32__
		EnterCriticalSection( &MutexArray[ IdMutex ] );
#else
		pthread_mutex_lock( &MutexArray[ IdMutex ] );
#endif
	}
}
void UnlockTheMutex( int IdMutex )
{
	if( IdMutex>=0 && IdMutex<NBR_MUTEXES )
	{
#ifdef __WIN32__
		LeaveCriticalSection( &MutexArray[ IdMutex ] );
#else
		pthread_mutex_unlock( &MutexArray[ IdMutex ] );
#endif
	}
}

// do not call under Xenomai to avoid mode switch...
time_t GetCurrentIntTime( void )
{
	return time( NULL );
}

void ConvertIntTimeToAsc( time_t IntTime, char * Buff, char WithDate )
{
	struct tm * tm_return;
#ifdef __WIN32__
	tm_return = localtime( &IntTime );
#else
	struct tm tm_result;
//	 tm_return = gmtime_r( &IntTime, &tm_result );
	tm_return = localtime_r( &IntTime, &tm_result );
#endif
	if ( tm_return )
	{
#ifdef __WIN32__
		if ( WithDate )
			sprintf( Buff, "%02d/%02d/%02d %02d:%02d:%02d", tm_return->tm_year%100, tm_return->tm_mon+1, tm_return->tm_mday,
					tm_return->tm_hour, tm_return->tm_min, tm_return->tm_sec );
		else
			sprintf( Buff, "%02d:%02d:%02d", tm_return->tm_hour, tm_return->tm_min, tm_return->tm_sec );
#else
		if ( WithDate )
			sprintf( Buff, "%02d/%02d/%02d %02d:%02d:%02d", tm_result.tm_year%100, tm_result.tm_mon+1, tm_result.tm_mday,
					tm_result.tm_hour, tm_result.tm_min, tm_result.tm_sec );
		else
			sprintf( Buff, "%02d:%02d:%02d", tm_result.tm_hour, tm_result.tm_min, tm_result.tm_sec );
#endif
	}
	else
	{
		Buff[ 0 ] = '\0';
	}
}
void GetCurrentAscTime( char * Buff )
{
	time_t CurrIntTime = GetCurrentIntTime( );
	ConvertIntTimeToAsc( CurrIntTime, Buff, TRUE/*WithDate*/ );
}
time_t ConvertAscToIntTime( char * Buff )
{
	time_t time;
	struct tm tm_val;
	int year,month,day,hour,min,sec;
	sscanf( Buff, "%02d/%02d/%02d %02d:%02d:%02d", &year,&month,&day,&hour,&min,&sec );
	tm_val.tm_mday = day;
	tm_val.tm_mon = month -1;
	tm_val.tm_year = 100+year;
	tm_val.tm_hour = hour;
	tm_val.tm_min = min;
	tm_val.tm_sec = sec;
	tm_val.tm_isdst = -1;
	time = mktime( &tm_val );
	return time;
}


void ForkAndExecvExternalCommandPrepared( )
{
#ifndef __WIN32__
	pid_t pid;
	pid = fork( );
	if ( pid==-1 )
	{
		printf("Failed to fork to launch external command!\n");
		if ( Preferences.UseSysLog )
			syslog( LOG_USER|LOG_DEBUG, "Failed to fork to launch external command..." );
	}
	else if ( pid==0 )
	{
		char LaunchResult = -1;
		printf("Launch the external command: %s\n",LaunchExternalCommandArgv[0]);
		if ( Preferences.UseSysLog )
			syslog( LOG_USER|LOG_DEBUG, "Launch the external command: %s", LaunchExternalCommandArgv[0] );
		LaunchResult = execv( LaunchExternalCommandArgv[0], LaunchExternalCommandArgv );
		if ( LaunchResult!=-1 )
		{
			// can not display anything here...
		}
		else
		{
			printf("Failed to launch external command !!! (errno=%d)\n", errno);
			if ( Preferences.UseSysLog )
				syslog( LOG_USER|LOG_DEBUG, "Failed to launch external command ! (errno=%d)", errno );
		}
		exit( 0 );
	}
//TODO, where? in father (for next call) without parameter inited: 	LaunchScriptParamList[ 0 ] = NULL; // no parameter
#endif
}

// if Oneargv ends with '\n' => executing !
void LaunchExternalCommand( char * Oneargv )
{
#ifndef __WIN32__
	write( PipeLaunchExternalCmd[ 1 ], Oneargv, strlen( Oneargv )+1 );
#endif
}

#ifndef __WIN32__
// called in loop by child
void TreatExternalCommandToLaunch( void )
{
	char BuffPipeRead[ 500 ];
	char EndForChildSeen = FALSE;
	do
	{
printf("%s(): Reading pipe...\n", __FUNCTION__);
		int BuffSize = read( PipeLaunchExternalCmd[ 0 ], BuffPipeRead, 500 );
		if ( BuffSize>0 )
		{
			char * PtrBuffArgv = BuffPipeRead;
printf("%s(): pipe read: %s (total=%d)\n", __FUNCTION__, PtrBuffArgv,BuffSize);
			while( BuffSize>0 )
			{
				// beware we can receive many argv in one read() !
				int LengthBuffArgv = strlen( PtrBuffArgv );
				BuffSize = BuffSize-(LengthBuffArgv+1);
printf("%s(): (total_rest=%d)!\n", __FUNCTION__,BuffSize);
				if ( LengthBuffArgv==1 && PtrBuffArgv[ 0 ]=='*')
				{
printf("%s(): ASK TO END PROCESS PIPE SEEN!\n", __FUNCTION__);
					EndForChildSeen = TRUE;
				}
				else
				{
					char CommandListComplete = FALSE;

					if( PtrBuffArgv[ LengthBuffArgv-1 ]=='\t' )
					{
						CommandListComplete = TRUE;
						PtrBuffArgv[ LengthBuffArgv-1 ] = '\0';
					}
printf("%s(): adding argv: %s (lgt=%d)\n", __FUNCTION__, PtrBuffArgv,LengthBuffArgv);
					LaunchExternalCommandArgv[ LaunchExternalCommandArgvIndex++ ] = &LaunchExternalCommandBuff[ LaunchExternalCommandBuffIndex ];
					strcpy( &LaunchExternalCommandBuff[ LaunchExternalCommandBuffIndex ], PtrBuffArgv );
					LaunchExternalCommandBuffIndex = LaunchExternalCommandBuffIndex+LengthBuffArgv+1;
					
					if ( CommandListComplete )
					{
						LaunchExternalCommandArgv[ LaunchExternalCommandArgvIndex ] = NULL; // end of argv list.
						ForkAndExecvExternalCommandPrepared( );
						LaunchExternalCommandBuffIndex = 0;
						LaunchExternalCommandArgvIndex = 0;
					}
					
				}
				PtrBuffArgv = PtrBuffArgv+LengthBuffArgv+1;
			}
		}
		else if ( BuffSize==-1 )
		{
printf("%s(): Reading pipe error...!\n", __FUNCTION__);
		}
		else
		{
printf("%s(): Reading pipe - NO DATA!\n", __FUNCTION__);
sleep(1);
		}
	}
	while( !EndForChildSeen );
}
#endif
