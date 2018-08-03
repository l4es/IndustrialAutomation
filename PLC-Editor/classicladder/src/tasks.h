
//added for time_t...
#include <time.h>

#define NBR_TASKS 10

#define ID_TASK_SCAN_INPUTS 0
#define ID_TASK_LOGIC 1
#define ID_TASK_GET_TIME 2


#define NBR_MUTEXES 5

#define MUTEX_LOG_EVENTS 0
#define MUTEX_LOG_TRANSMIT_FLAGS 1
#define MUTEX_TIME_COPY_FOR_VARS_SYS 2

void InitTasks( void );
char CreateTask( int IdTask, char * TaskName, int Priority, int StackSizeInKo, unsigned int Period, void * (*pTaskFunction)(void *) );
void SetPeriodicValueForThread( int IdTask, unsigned int PeriodMicroSecs );

void PeriodicTaskCycleStart( int IdTask );
void PeriodicTaskCycleEnd( int IdTask );
void EndTaskWanted( int IdTask );
void TaskExit( int IdTask );
char TaskIsRunning( int IdTask );

void InitGeneralMutex( void );
void EndGeneralMutex( void );
int CreateTheMutex( int IdMutex );
void EndTheMutex( int IdMutex );
void LockTheMutex( int IdMutex );
void UnlockTheMutex( int IdMutex );

void LaunchExternalCommand( char * Oneargv );
void TreatExternalCommandToLaunch( void );

