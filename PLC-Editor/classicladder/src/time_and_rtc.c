/* Classic Ladder Project */
/* Copyright (C) 2001-2018 Marc Le Douarain */
/* http://www.sourceforge.net/projects/classicladder */
/* http://sites.google.com/site/classicladder */
/* August 2017 */
/* ------------------------------------------------------------ */
/* Time clock & RTC access, and functions conversions utilities */
/* ------------------------------------------------------------ */
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

#ifndef MODULE
#include <stdio.h>
#include <string.h>
#include <time.h>
#ifndef __WIN32__
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/rtc.h>
#include <sys/time.h>
#include <syslog.h>
#endif
#endif

#include "classicladder.h"
#include "global.h"
#include "vars_access.h"
#include "modem.h"
#include "tasks.h"
#include "time_and_rtc.h"

int DeviceRTC;

struct tm tm_copy_for_vars_sys;
time_t CurrentIntTimeCopy = (time_t)-1;

void InitTimeAndRtc( void )
{
	tm_copy_for_vars_sys.tm_wday = -1; // to indicate not initialized for now...
}

// called in a task not real-time
// "tm_copy_for_vars_sys" values then copied in the logic task (that can be real-time)
void GetTimeDatasInThread( char DoUseRtcDevice )
{
//TODO: perhaps to optimize both timegm() (in ReadRtc()) & localtime(), not necessary to do that every call...
	if ( DoUseRtcDevice )
	{
		CurrentIntTimeCopy = ReadRtc( );
	}
	else
	{
//		struct timeval tv;
		CurrentIntTimeCopy = time(NULL);
//		if ( gettimeofday( &tv, NULL )==0 )
		{
//			CurrentIntTimeCopy = tv.tv_sec;
		}
	}
	if ( CurrentIntTimeCopy!=(time_t)-1 )
	{
		struct tm * ptm_for_vars_sys = localtime( &CurrentIntTimeCopy );
		// to avoid too much (long) block real-time task, it will use this copy...<
		// (and problematic under Windows with some fields at -1 a little time !!!)
		LockTheMutex( MUTEX_TIME_COPY_FOR_VARS_SYS );
		memcpy( (void *)&tm_copy_for_vars_sys, (void *)ptm_for_vars_sys, sizeof( struct tm ) );
		UnlockTheMutex( MUTEX_TIME_COPY_FOR_VARS_SYS );
	}
}

// usefull to avoid mode switch in Xenomai !
time_t GetCopyCurrentIntTime( void )
{
	return CurrentIntTimeCopy;
}

char OpenRtcDevice( void )
{
#ifndef __WIN32__
	DeviceRTC = open( "/dev/rtc", O_RDONLY);
	if ( DeviceRTC<0 )
	{
		DeviceRTC = open( "/dev/rtc0", O_RDONLY);
	}
	if ( DeviceRTC>=0 )
	{
		printf("RTC device opened.\n");
	}
	else
	{
		printf("Failed to open RTC device !!!\n");
		return FALSE;
	}
#endif
	return TRUE;
}
void CloseRtcDevice( void )
{
#ifndef __WIN32__
	if ( DeviceRTC>=0 )
	{
		close( DeviceRTC );
		printf("RTC device closed.\n");
	}
#endif
}

time_t ReadRtc( void )
{
	time_t TimeResult = (time_t)-1;
#ifndef __WIN32__
	if ( DeviceRTC>=0 )
	{
		//use a "copy" to avoid to have seconds to 0 sometimes ???!!!
		struct tm rtc_tm_read;
		memset( &rtc_tm_read, 0, sizeof(rtc_tm_read) );
		ioctl( DeviceRTC, RTC_RD_TIME, &rtc_tm_read );
//printf("rtc raw read %d %d/%d/%d %02d:%02d:%02d (time=%d)\n", rtc_tm_read.tm_wday, rtc_tm_read.tm_mday, rtc_tm_read.tm_mon+1, rtc_tm_read.tm_year+1900, rtc_tm_read.tm_hour, rtc_tm_read.tm_min, rtc_tm_read.tm_sec, TimeResult );
		rtc_tm_read.tm_isdst = -1; /*unknown*/
//RTC in UTC time (and not local) !		TimeResult = mktime( &copy_tm );
		TimeResult = timegm( &rtc_tm_read );
//printf("rtc read (after timegm) %d %d/%d/%d %02d:%02d:%02d (time=%d)\n", rtc_tm_read.tm_wday, rtc_tm_read.tm_mday, rtc_tm_read.tm_mon+1, rtc_tm_read.tm_year+1900, rtc_tm_read.tm_hour, rtc_tm_read.tm_min, rtc_tm_read.tm_sec, TimeResult );
	}
#endif
	return TimeResult;
}

char WriteRtc( time_t time_to_set )
{
#ifndef __WIN32__
	if ( DeviceRTC>=0 )
	{
		struct tm * tm_return;
		struct tm tm_result;
		tm_return = gmtime_r( &time_to_set, &tm_result );
		//tm_return = localtime_r( &IntTime, &tm_result );
		if ( tm_return )
			ioctl( DeviceRTC, RTC_SET_TIME, /*tm_return*/&tm_result );
		return 1;
	}
	else
#endif
	{
		return 0;
	}
}

char SetTimeClock( time_t IntTimeValueToSet, char DoUseRtcDevice )
{
#ifndef __WIN32__
	char Done = 1;
	struct timeval tv;
	tv.tv_sec = IntTimeValueToSet;
	tv.tv_usec = 0;
	if ( settimeofday( &tv, NULL )!=0 )
	{
		printf("Failed to set Linux clock time!!!\n");
		Done = 0;
	}
	if ( DoUseRtcDevice )
	{
		if ( !WriteRtc( IntTimeValueToSet ) )
		{
			printf("Failed to set RTC !!!\n");
			Done = 0;
		}
	}
	return Done;
#else
	return 0;
#endif
}


// do not call under Xenomai to avoid mode switch...
time_t GetCurrentIntTime( void )
{
	return time( NULL );
}

void ConvertIntTimeToAsc( time_t IntTime, char * Buff, char WithDate, char InUTC )
{
	struct tm * tm_return;
#ifdef __WIN32__
	if ( InUTC )
		tm_return = gmtime( &IntTime );
	else
		tm_return = localtime( &IntTime );
#else
	struct tm tm_result;
	if ( InUTC )
		tm_return = gmtime_r( &IntTime, &tm_result );
	else
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
// local time
void GetCurrentAscTime( char * Buff )
{
	time_t CurrIntTime = GetCurrentIntTime( );
	ConvertIntTimeToAsc( CurrIntTime, Buff, TRUE/*WithDate*/, FALSE/*InUTC*/ );
}
// UTC time (used for monitor protocol "set time")
void GetCurrentAscTimeUTC( char * Buff )
{
	time_t CurrIntTime = GetCurrentIntTime( );
	ConvertIntTimeToAsc( CurrIntTime, Buff, TRUE/*WithDate*/, TRUE/*InUTC*/ );
}
//Dec.2016, seen with MSYS2/Win32, timegm() not available... but only usefull for Linux embedded targets ! ;-)
#ifndef __WIN32__
// used for monitor protocol "set time" (string in UTC)
time_t ConvertAscUtcToIntTime( char * Buff )
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
//SwitchedInUTC	time = mktime( &tm_val );
	time = timegm( &tm_val );
	return time;
}
#endif
