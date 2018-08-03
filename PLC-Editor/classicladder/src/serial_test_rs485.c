
/* Simple test of serial with rs485 (half-duplex) */
/* Using serial low-level of ClassicLadder project */
/* Marc Le Douarain, 14 october 2012 */

#include "serial_common.h"

#include <stdio.h>
#include <string.h>
#include <time.h>

#define PORT_IDX 0
void DoPauseMilliSecs( int MilliSecsTime );

int main( int argc, char *argv[] )
{
	char BuffRx[ 50 ];
	int LgtRx;
	SerialGeneralInit( );
	
	if ( SerialOpen( PORT_IDX, "/dev/ttyS0", 9600, 8, 0, 1 ) )
	{
		char * StringToSend = "Hello world with rs485 !\n";
		int Count = 0;
		printf("RS485 test...\n");
		SerialSetRTS( PORT_IDX, 0 );
		while( 1 )
		{

			if ( Count==0 )
			{
				printf("Send: %s", StringToSend );
				SerialSetRTS( PORT_IDX, 1 );
				SerialSend( PORT_IDX, StringToSend, strlen( StringToSend ) );
				DoPauseMilliSecs( 1000 );
				SerialSetRTS( PORT_IDX, 0 );
			}
			LgtRx = SerialReceive( PORT_IDX, BuffRx, 49 );
			if ( LgtRx>0 )
			{
				BuffRx[ LgtRx ] = '\0';
				printf("Received: %s\n", BuffRx );
			}
			DoPauseMilliSecs( 500 );
			Count++;
			if ( Count>10 )
				Count = 0;
			
		}
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
