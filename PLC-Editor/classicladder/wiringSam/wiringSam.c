/*
   wiringSam : a library to easily access to GPIOs pins of an Atmel AT91SAM
   like on "Arduino" cards
   Copyright (C) 2015 Marc Le Douarain
   Times functions taken directly from wiringPi library by Gordon Henderson. 
   
   This library is free software: you can redistribute it and/or modify
   it under the terms of the GNU Lesser General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU Lesser  General Public License for more details.

   You should have received a copy of the GNU Lesser  General Public License
   along with this library.  If not, see <http://www.gnu.org/licenses/>
   
   wiringSam v0.1 - 28 august 2015
*/

#include <stdio.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>
#include <sys/time.h>

#include "wiringSam.h"

#define MAP_SIZE 4096   /* needs to be a power of 2! */
#define MAP_MASK (MAP_SIZE - 1)

uint32_t SamPioControllerBaseAddress[ WSAM_NBR_PIOS ] = { 0xFFFFF400, 0xFFFFF600, 0xFFFFF800, 0xFFFFFA00 };

int mem_fd = -1;
void * MapBase = NULL;
static StructSamGpio * MapPioController[ WSAM_NBR_PIOS ];

// Time for easy calculations
static uint64_t epochMilli, epochMicro ;
static void initialiseEpoch( void );

int  wiringSamSetup( void )
{
	if ((mem_fd = open( "/dev/mem", O_RDWR | O_SYNC)) < 0)
	{
        printf("wiringSam: Failed to open '/dev/mem'.\n");
		return -1;
	}
	else
	{
		int ScanPio;
		initialiseEpoch( );
		
		MapBase = (StructSamGpio *)mmap( 0, MAP_SIZE, PROT_READ|PROT_WRITE, MAP_SHARED, mem_fd, SamPioControllerBaseAddress[WSAM_PIO_A] & ~MAP_MASK );
		if ((int)MapBase == -1)
		{
			close( mem_fd );
			printf( "wiringSam: mmap() call failed.\n");
			return -1;
		}
		
		for( ScanPio=0; ScanPio<WSAM_NBR_PIOS; ScanPio++ )
			MapPioController[ ScanPio ] = MapBase + (SamPioControllerBaseAddress[ ScanPio ] & MAP_MASK);

	}
	return 0;
}

void wiringSamEnd( void )
{
	if ( munmap( MapBase, MAP_SIZE ) == -1)
	{
		printf( "wiringSam: munmap() call for end failed.\n");
	}
	else
	{
		close( mem_fd ); 
	}
}

void pinMode( int controller, int pin, int mode )
{
	if ( MapBase && controller>=0 && controller<WSAM_NBR_PIOS )
	{
		if ( mode==INPUT )
		{
			MapPioController[ controller ]->OutputDisableRegister = 1<<pin;
		}
		else if ( mode==OUTPUT )
		{
			MapPioController[ controller ]->OutputEnableRegister = 1<<pin;
		}
	}
}

void pullUpDnControl( int controller, int pin, int pud )
{
	if ( MapBase && controller>=0 && controller<WSAM_NBR_PIOS )
	{
		 if ( pud==PUD_UP )
		 {
			MapPioController[ controller ]->PadPulldownDisableRegister = 1<<pin;
			MapPioController[ controller ]->PullupEnableRegister = 1<<pin;
		 }
		 else if ( pud==PUD_DOWN )
		 {
			MapPioController[ controller ]->PullupDisableRegister = 1<<pin;
			MapPioController[ controller ]->PadPulldownEnableRegister = 1<<pin;
		 }
		 else if ( pud==PUD_OFF )
		 {
			MapPioController[ controller ]->PullupDisableRegister = 1<<pin;
			MapPioController[ controller ]->PadPulldownDisableRegister = 1<<pin;
		 }
	}
}

int digitalRead( int controller, int pin )
{
	if ( MapBase && controller>=0 && controller<WSAM_NBR_PIOS )
	{
		if ( MapPioController[ controller ]->PinDataStatusRegister & (1<<pin ) )
			return HIGH;
		else
			return LOW;
	}
	return LOW;
}

void digitalWrite( int controller, int pin, int value )
{
	if ( MapBase && controller>=0 && controller<WSAM_NBR_PIOS )
	{
		if ( value==HIGH )
			MapPioController[ controller ]->SetOutputDataRegister = 1<<pin;
		else
			MapPioController[ controller ]->ClearOutputDataRegister = 1<<pin;
	}
}



/* ----------------------------------------------------------------------------------------------
   Following time functions taken from wiringPi library (for the Raspberry Pi) by Gordon Henderson
   Thanks to him for its nice work
   ---------------------------------------------------------------------------------------------- */

/*
 * initialiseEpoch:
 *	Initialise our start-of-time variable to be the current unix
 *	time in milliseconds and microseconds.
 *********************************************************************************
 */
static void initialiseEpoch (void)
{
  struct timeval tv ;

  gettimeofday (&tv, NULL) ;
  epochMilli = (uint64_t)tv.tv_sec * (uint64_t)1000    + (uint64_t)(tv.tv_usec / 1000) ;
  epochMicro = (uint64_t)tv.tv_sec * (uint64_t)1000000 + (uint64_t)(tv.tv_usec) ;
}

/*
 * delay:
 *	Wait for some number of milliseconds
 *********************************************************************************
 */
void delay (unsigned int howLong)
{
  struct timespec sleeper, dummy ;

  sleeper.tv_sec  = (time_t)(howLong / 1000) ;
  sleeper.tv_nsec = (long)(howLong % 1000) * 1000000 ;

  nanosleep (&sleeper, &dummy) ;
}

/*
 * delayMicroseconds:
 *	This is somewhat intersting. It seems that on the Pi, a single call
 *	to nanosleep takes some 80 to 130 microseconds anyway, so while
 *	obeying the standards (may take longer), it's not always what we
 *	want!
 *
 *	So what I'll do now is if the delay is less than 100uS we'll do it
 *	in a hard loop, watching a built-in counter on the ARM chip. This is
 *	somewhat sub-optimal in that it uses 100% CPU, something not an issue
 *	in a microcontroller, but under a multi-tasking, multi-user OS, it's
 *	wastefull, however we've no real choice )-:
 *
 *      Plan B: It seems all might not be well with that plan, so changing it
 *      to use gettimeofday () and poll on that instead...
 *********************************************************************************
 */
void delayMicrosecondsHard (unsigned int howLong)
{
  struct timeval tNow, tLong, tEnd ;

  gettimeofday (&tNow, NULL) ;
  tLong.tv_sec  = howLong / 1000000 ;
  tLong.tv_usec = howLong % 1000000 ;
  timeradd (&tNow, &tLong, &tEnd) ;

  while (timercmp (&tNow, &tEnd, <))
    gettimeofday (&tNow, NULL) ;
}
void delayMicroseconds (unsigned int howLong)
{
  struct timespec sleeper ;
  unsigned int uSecs = howLong % 1000000 ;
  unsigned int wSecs = howLong / 1000000 ;

  /**/ if (howLong ==   0)
    return ;
  else if (howLong  < 100)
    delayMicrosecondsHard (howLong) ;
  else
  {
    sleeper.tv_sec  = wSecs ;
    sleeper.tv_nsec = (long)(uSecs * 1000L) ;
    nanosleep (&sleeper, NULL) ;
  }
}

/*
 * millis:
 *	Return a number of milliseconds as an unsigned int.
 *********************************************************************************
 */
unsigned int millis (void)
{
  struct timeval tv ;
  uint64_t now ;

  gettimeofday (&tv, NULL) ;
  now  = (uint64_t)tv.tv_sec * (uint64_t)1000 + (uint64_t)(tv.tv_usec / 1000) ;

  return (uint32_t)(now - epochMilli) ;
}

/*
 * micros:
 *	Return a number of microseconds as an unsigned int.
 *********************************************************************************
 */
unsigned int micros (void)
{
  struct timeval tv ;
  uint64_t now ;

  gettimeofday (&tv, NULL) ;
  now  = (uint64_t)tv.tv_sec * (uint64_t)1000000 + (uint64_t)tv.tv_usec ;

  return (uint32_t)(now - epochMicro) ;
}

