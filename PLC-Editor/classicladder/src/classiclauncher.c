
/* Launcher for external applications to start from ClassicLadder */
/* Done after discovered that previous method not working when compiled for Xenomai 3 (when fork to launch execv) */
/* Marc Le Douarain, 29 july 2017 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "classicladder.h"

int PipeLaunchExternalCmd;

char LaunchExternalCommandBuff[ 3000 ];
int LaunchExternalCommandBuffIndex = 0;
char * LaunchExternalCommandArgv[ 30 ];
int LaunchExternalCommandArgvIndex = 0;

void HandlerSignalInterrupt( int signal_id )
{
	exit( 0 );
}

void ForkAndExecvExternalCommandPrepared( )
{
	pid_t pid;
	pid = fork( );
	if ( pid==-1 )
	{
		printf("Failed to fork to launch external command!\n");
//		if ( Preferences.UseSysLog )
//			syslog( LOG_USER|LOG_DEBUG, "Failed to fork to launch external command..." );
	}
	else if ( pid==0 )
	{
		char LaunchResult = -1;
		printf("Launch the external command: %s\n",LaunchExternalCommandArgv[0]);
//		if ( Preferences.UseSysLog )
//			syslog( LOG_USER|LOG_DEBUG, "Launch the external command: %s", LaunchExternalCommandArgv[0] );
		LaunchResult = execv( LaunchExternalCommandArgv[0], LaunchExternalCommandArgv );
		if ( LaunchResult!=-1 )
		{
			// can not display anything here...
		}
		else
		{
			printf("Failed to launch external command !!! (errno=%d)\n", errno);
//			if ( Preferences.UseSysLog )
//				syslog( LOG_USER|LOG_DEBUG, "Failed to launch external command ! (errno=%d)", errno );
		}
		exit( 0 );
	}
//TODO, where? in father (for next call) without parameter inited: 	LaunchScriptParamList[ 0 ] = NULL; // no parameter
}

// called in loop by child
void TreatExternalCommandToLaunch( void )
{
	char BuffPipeRead[ 500 ];
	char EndForChildSeen = FALSE;
	do
	{
printf("%s(): Reading pipe...\n", __FUNCTION__);
		int BuffSize = read( PipeLaunchExternalCmd, BuffPipeRead, 500 );
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

int main( int argc, char *argv[] )
{
	daemon( 1, 1 );
	
	if ( access( FIFO_CLASSICLAUNCHER, F_OK )==-1 )
	{
		if ( mkfifo( FIFO_CLASSICLAUNCHER, 0777 )==-1 )
		{
			printf("ClassicLauncher: Failed to init fifo-pipe %s for external launch command !\n", FIFO_CLASSICLAUNCHER);
			return 0;
		}
	}
	PipeLaunchExternalCmd = open( FIFO_CLASSICLAUNCHER, O_RDONLY );
	if ( PipeLaunchExternalCmd==-1 )
	{
		printf("Failed to open fifo-pipe %s for external launch command !\n", FIFO_CLASSICLAUNCHER);
	}
	else
	{
		TreatExternalCommandToLaunch( );
	}
	close( PipeLaunchExternalCmd );

	return 0;
}

