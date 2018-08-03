
/* Simple test of current time / RTC values */
/* Using "time_and_rtc" file of ClassicLadder project */
/* Marc Le Douarain, 18 august 2017 */

#include "time_and_rtc.h"

#include <stdio.h>
#include <string.h>
#include <time.h>

void DoPauseMilliSecs( int MilliSecsTime );

int main( int argc, char *argv[] )
{
	struct tm * ptr_tm_time_test;
	if ( OpenRtcDevice( ) )
	{
		int Pass;
		for( Pass=0; Pass<2; Pass++ )
		{
			ReadRtc( );
			
			time_t time_test = time( NULL );
			ptr_tm_time_test = gmtime( &time_test );
			printf("Time UTC read = %d %d/%d/%d %02d:%02d:%02d\n", ptr_tm_time_test->tm_wday, ptr_tm_time_test->tm_mday, ptr_tm_time_test->tm_mon+1, ptr_tm_time_test->tm_year+1900, ptr_tm_time_test->tm_hour, ptr_tm_time_test->tm_min, ptr_tm_time_test->tm_sec );
			ptr_tm_time_test = localtime( &time_test );
			printf("Time local read = %d %d/%d/%d %02d:%02d:%02d\n", ptr_tm_time_test->tm_wday, ptr_tm_time_test->tm_mday, ptr_tm_time_test->tm_mon+1, ptr_tm_time_test->tm_year+1900, ptr_tm_time_test->tm_hour, ptr_tm_time_test->tm_min, ptr_tm_time_test->tm_sec );
			if ( Pass<1 )
				DoPauseMilliSecs( 2000 );
		}
		CloseRtcDevice( );
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
