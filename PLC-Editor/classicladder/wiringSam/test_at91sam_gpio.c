
/*
   Little test program to test GPIOs access of an Atmel AT91SAM
   using the wiringSam library.
   28 august 2015
   Tested on an Arietta G25 card (AcmeSystems) with following pins :
   - Push button on board : Port_C pin 17 (as input)
   - Led connected on Port_C pin 0 (as output).
*/

#include <stdio.h>
#include "wiringSam.h"

int main( int argc, char * argv[] )
{
	printf("Little GPIO test on at91sam with wiringSam !\n");
	
	if ( wiringSamSetup( )<0 )
	{
		printf("wiringSam lib init error !!!\n");
	}
	else
	{
		int Loop;
		pinMode( WSAM_PIO_C, 17, INPUT );
		printf( "Value of bit_17/PIO_C (Arietta Push Button on board) : %s\n", digitalRead( WSAM_PIO_C, 17 )?"HIGH":"LOW" );

		printf( "Now will flash 5 times, output bit_0/PIO_C...\n" );
		pinMode( WSAM_PIO_C, 0, OUTPUT );
		for( Loop=0; Loop<5; Loop++ )
		{
			digitalWrite( WSAM_PIO_C, 0, HIGH );
			delay( 500 ); //ms
			digitalWrite( WSAM_PIO_C, 0, LOW );
			delay( 1000 ); //ms
		}

/*		printf( "Now will flash 10 times, output bit_14/PIO_B...\n" );
		pinMode( WSAM_PIO_B, 14, OUTPUT );
		for( Loop=0; Loop<5; Loop++ )
		{
			digitalWrite( WSAM_PIO_B, 14, HIGH );
			delay( 750 ); //ms
			digitalWrite( WSAM_PIO_B, 14, LOW );
			delay( 250  ); //ms
		}*/
		
		wiringSamEnd( );
		printf("End of wiringSam test.\n");
	}
	return 0;
}
