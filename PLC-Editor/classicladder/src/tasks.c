/* Classic Ladder Project */
/* Copyright (C) 2001-2017 Marc Le Douarain */
/* http://www.sourceforge.net/projects/classicladder */
/* http://sites.google.com/site/classicladder */
/* January 2012 */
/* ------------------------------------------------------- */
/* Tasks management (create/delete/period)                 */
/* For Posix (Linux / real-time Xenomai 2 and 3) & Windows */
/* ------------------------------------------------------- */
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

//put in comment if your glibc is older than version 2.12,
//and didn't know 'pthread_setname_np()' function...
#define USE_PTHREAD_SETNAME_NP

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#ifdef __WIN32__
#include <windows.h>
#else
#if defined(USE_PTHREAD_SETNAME_NP) && !defined(__USE_GNU)
#define __USE_GNU
#endif
#include <pthread.h>
#include <syslog.h>
#include <unistd.h>
#include <stdint.h>
#include <errno.h>
#include <sys/mman.h>
#include <sys/timerfd.h>
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
int TimerFdForThread[ NBR_TASKS ];
pthread_mutex_t MutexArray[ NBR_MUTEXES ];
#endif

//char * LaunchScriptParamList[ 30 ]; // First = NULL if no parameters to command to launch...
//////char LaunchExternalCommandBuff[ 3000 ];
//////int LaunchExternalCommandBuffIndex = 0;
//////char * LaunchExternalCommandArgv[ 30 ];
//////int LaunchExternalCommandArgvIndex = 0;

void InitTasks( void )
{
	int ScanTask;
	for( ScanTask=0; ScanTask<NBR_TASKS; ScanTask++ )
	{
		ThreadRunning[ ScanTask ] = 0;
		PeriodThreadValue[ ScanTask ] = 0;
#ifdef __WIN32__
		ThreadHandle[ ScanTask ] = NULL;
#else
		TimerFdForThread[ ScanTask ] = -1;
#endif
	}
}

#ifdef __WIN32__
char CreateTaskWindows( int IdTask, char * TaskName, int Priority, int StackSizeInKo, unsigned int PeriodMicroSecs, void * (*pTaskFunction)(void *) )
{
	char InitOk = 0;
	printf("Creating thread%d(%s)...\n",IdTask,TaskName);
	ThreadRunning[ IdTask ] = 1;
	PeriodThreadValue[ IdTask ] = PeriodMicroSecs;
	ThreadHandle[ IdTask ] = CreateThread( NULL/*no security attributes*/, StackSizeInKo*1024L/*default stack size*/,                                                   
			(LPTHREAD_START_ROUTINE)/* thread function*/pTaskFunction, 
			NULL/*argument to thread function*/,                
			THREAD_QUERY_INFORMATION/*use default creation flags*/,                           
			&ThreadId[ IdTask ]/* returns the thread identifier*/);                
	if ( ThreadHandle[ IdTask ]==NULL )
	{
		printf("Failed to create thread%d(%s)... !!!!!\n",IdTask,TaskName);
	}
	else
	{
		if ( PeriodMicroSecs!=0 )
			SetPeriodicValueForThread( IdTask, PeriodMicroSecs );
		printf( "Created thread%d(%s) ok.\n", IdTask, TaskName );
		InitOk = 1;
	}
	return InitOk;
}
#endif

#ifndef __WIN32__
// Period=-1 if not a periodic task
char CreateTaskLinux( int IdTask, char * TaskName, int Priority, int StackSizeInKo, unsigned int PeriodMicroSecs, void * (*pTaskFunction)(void *) )
{
	pthread_attr_t ThreadAttributes;
	char InitOk = 0;

	ThreadRunning[ IdTask ] = 1;
	PeriodThreadValue[ IdTask ] = PeriodMicroSecs;
	printf("Creating thread%d(%s)...\n",IdTask,TaskName);
	
	int err = pthread_attr_init(&ThreadAttributes);
	if ( err )
	{
		printf("pthread attr_init() failed for thread '%s' with err=%d\n", TaskName, err );
		return InitOk;
	}
#ifdef __COBALT__
	err = pthread_attr_setinheritsched( &ThreadAttributes, PTHREAD_EXPLICIT_SCHED );
	if ( err )
	{
		printf("pthread set explicit sched failed for thread '%s' with err=%d\n", TaskName, err );
		return InitOk;
	}
#endif
	err = pthread_attr_setdetachstate(&ThreadAttributes, PTHREAD_CREATE_DETACHED /*PTHREAD_CREATE_JOINABLE*/);
	if ( err )
	{
		printf("pthread set detach state failed for thread '%s' with err=%d\n", TaskName, err );
		return InitOk;
	}
#if defined(__XENO__) || defined(__COBALT__)
	err = pthread_attr_setschedpolicy(&ThreadAttributes, SCHED_FIFO);
	if ( err )
	{
		printf("pthread set scheduling policy failed for thread '%s' with err=%d\n", TaskName, err );
		return -13;
	}
	struct sched_param paramA = { .sched_priority = Priority };
	err = pthread_attr_setschedparam(&ThreadAttributes, &paramA);
	if ( err )
	{
		printf("pthread set priority failed for thread '%s' with err=%d\n", TaskName, err );
		return InitOk;
	}
#endif
	if ( StackSizeInKo>0 )
	{
		err = pthread_attr_setstacksize(&ThreadAttributes, StackSizeInKo*1024);
		if ( err )
		{
			printf("pthread set stack size failed for thread '%s' with err=%d\n", TaskName, err );
			return InitOk;
		}
	}

	// if a timerfd is used to make thread periodic (Linux / Xenomai 3),
	// initialize it before launching thread (timer is read in the loop)
#ifndef __XENO__ 
	if ( PeriodMicroSecs>0 )
		SetPeriodicValueForThread( IdTask, PeriodMicroSecs );
#endif

	err = pthread_create( &PosixThread[ IdTask ], &ThreadAttributes, (void *(*)(void *))pTaskFunction, (void *)NULL );
	if ( err )
	{
		printf( "Failed to create thread '%s' with err=%d !!!!!\n", TaskName, err );
		return InitOk;
	}
	else
	{
#ifdef __XENO__ 
		if ( PeriodMicroSecs>0 )
			SetPeriodicValueForThread( IdTask, PeriodMicroSecs );
#endif
		pthread_attr_destroy(&ThreadAttributes);
#ifdef __XENO__ 
		err = pthread_set_name_np( PosixThread[ IdTask ], TaskName );
#elif defined( USE_PTHREAD_SETNAME_NP )
		err = pthread_setname_np( PosixThread[ IdTask ], TaskName );
#endif
		if ( err )
		{
			printf("pthread set name failed for thread '%s', err=%d\n", TaskName, err );
			return InitOk;
		}
		printf( "Created thread%d(%s) ok.\n", IdTask, TaskName );
		InitOk = 1;
	}
	return InitOk;
}
#endif

// Period=-1 if not a periodic task
char CreateTask( int IdTask, char * TaskName, int Priority, int StackSizeInKo, unsigned int PeriodMicroSecs, void * (*pTaskFunction)(void *) )
{
	if( IdTask>=0 && IdTask<NBR_TASKS )
	{
#ifdef __WIN32__
		return CreateTaskWindows( IdTask, TaskName, Priority, StackSizeInKo, PeriodMicroSecs, pTaskFunction );
#else
		return CreateTaskLinux( IdTask, TaskName, Priority, StackSizeInKo, PeriodMicroSecs, pTaskFunction );
#endif
	}
	else
	{
		printf("Task creation overflow !!!!!\n");
		return 0;
	}
}

// to change period of a task later... after reading parameter in file project for example!
void SetPeriodicValueForThread( int IdTask, unsigned int PeriodMicroSecs )
{
	if( IdTask>=0 && IdTask<NBR_TASKS )
	{
		PeriodThreadValue[ IdTask ] = PeriodMicroSecs;
		
#ifndef __WIN32__
		// calc start time of the periodic thread
		struct timespec start_time;
#ifdef __XENO__
		if ( clock_gettime( CLOCK_REALTIME, &start_time ) )
#else
		if ( clock_gettime( CLOCK_MONOTONIC, &start_time ) )
#endif
		{
			printf( "Failed to call clock_gettime\n" );
			return;
		}
		/* Start one seconde later from now. */
		start_time.tv_sec += 1;
		
#ifndef __XENO__ 
		struct itimerspec period_timer_conf;
		// not already created ?
		if ( TimerFdForThread[ IdTask ]==-1 )
			TimerFdForThread[ IdTask ] = timerfd_create(CLOCK_MONOTONIC, 0);
		if ( TimerFdForThread[ IdTask ]==-1 )
		{
			printf( "Failed to create timerfd for thread%d\n", IdTask);
			return;
		}
		period_timer_conf.it_value = start_time;
		period_timer_conf.it_interval.tv_sec = 0;
		period_timer_conf.it_interval.tv_nsec = PeriodMicroSecs*1000;
		if ( timerfd_settime(TimerFdForThread[ IdTask ], TFD_TIMER_ABSTIME, &period_timer_conf, NULL) )
		{
			printf( "Failed to set periodic tor thread%d with errno=%d\n", IdTask, errno);
			return;
		}
		else
		{
			printf( "Set periodic thread%d period=%dusecs ok.\n", IdTask, PeriodMicroSecs );
		}
#else
		struct timespec period_timespec;
		period_timespec.tv_sec = 0;
		period_timespec.tv_nsec = PeriodMicroSecs*1000;
		if ( pthread_make_periodic_np(PosixThread[ IdTask ], &start_time, &period_timespec)!=0 )
		{
			printf("Xenomai make_periodic failed for thread%d.\n", IdTask);
			return;
		}
		else
		{
			printf( "Set periodic thread%d period=%dusecs ok.\n", IdTask, PeriodMicroSecs );
		}
#endif

#endif
	}
}

// to be called at a start of a periodic task
#if defined(__XENO__) || defined(__COBALT__)
struct timespec start_logic_task;
#endif
void PeriodicTaskCycleStart( int IdTask )
{
#ifndef __WIN32__
	int err = 0;
#endif
#ifdef __XENO__
	unsigned long overruns;
	err = pthread_wait_np(&overruns);
	if (err || overruns) {
		printf( "Xenomai wait_period failed for thread%d: err %d, overruns: %lu\n", IdTask, err, overruns);
		InfosGene->NbrTicksMissed += (unsigned int)overruns;
//			exit(EXIT_FAILURE);
	}
#elif defined(__WIN32__)
	int ValPauseMilliSecs = PeriodThreadValue[ IdTask ]/1000;
	if ( ValPauseMilliSecs<1 )
		ValPauseMilliSecs = 1;
	DoPauseMilliSecs( ValPauseMilliSecs );
#else
	uint64_t ticks;
	err = read(TimerFdForThread[ IdTask ], &ticks, sizeof(ticks));
	if ( err<0 )
	{
		printf( "TimerFd wait period failed for thread%d with errno=%d\n", IdTask, errno );
	}
	if ( ticks>1 )
	{
#ifdef __COBALT__
		printf( "TimerFd wait period missed for thread%d: overruns=%lu\n", IdTask, (long unsigned int)ticks );
#endif
		InfosGene->NbrTicksMissed += (unsigned int)ticks;
	}
#endif

#if defined(__XENO__) || defined(__COBALT__)
	if ( IdTask==ID_TASK_LOGIC )
	{
#ifdef __XENO__
		clock_gettime( CLOCK_REALTIME, &start_logic_task );
#else
		clock_gettime( CLOCK_MONOTONIC, &start_logic_task );
#endif
	}
#endif
}
void PeriodicTaskCycleEnd( int IdTask )
{
#if defined(__XENO__) || defined(__COBALT__)
	if ( IdTask==ID_TASK_LOGIC )
	{
		struct timespec now;
#ifdef __XENO__
		clock_gettime( CLOCK_REALTIME, &now );
#else
		clock_gettime( CLOCK_MONOTONIC, &now );
#endif
		if ( now.tv_sec==start_logic_task.tv_sec )
		{
			InfosGene->DurationOfLastScan = now.tv_nsec-start_logic_task.tv_nsec;
			if ( InfosGene->DurationOfLastScan>InfosGene->MaxScanDuration )
				InfosGene->MaxScanDuration = InfosGene->DurationOfLastScan;
		}
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


// if Oneargv ends with '\t' => executing !
void LaunchExternalCommand( char * Oneargv )
{
#ifndef __WIN32__
//////	write( PipeLaunchExternalCmd[ 1 ], Oneargv, strlen( Oneargv )+1 );
	write( PipeLaunchExternalCmd, Oneargv, strlen( Oneargv )+1 );
#endif
}


#ifdef AAAAAAAAAAAAAAA
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
#endif


// trouble with Xeno3 (when fork to launch execv), so now done an external 'ClassicLauncher' application to do the work!
#ifdef AAAAAAAAAAAAAAA
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
printf("%s(): adding argv[%d]: %s (lgt=%d)\n", __FUNCTION__, LaunchExternalCommandArgvIndex, PtrBuffArgv,LengthBuffArgv);
					LaunchExternalCommandArgv[ LaunchExternalCommandArgvIndex++ ] = &LaunchExternalCommandBuff[ LaunchExternalCommandBuffIndex ];
					strcpy( &LaunchExternalCommandBuff[ LaunchExternalCommandBuffIndex ], PtrBuffArgv );
					LaunchExternalCommandBuffIndex = LaunchExternalCommandBuffIndex+LengthBuffArgv+1;
					
					if ( CommandListComplete )
					{
printf("%s(): COMMAND COMPLETE TO LAUNCH (ArgvIdx=%d)!\n", __FUNCTION__, LaunchExternalCommandArgvIndex);
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
#endif
