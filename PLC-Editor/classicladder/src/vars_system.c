/* Classic Ladder Project */
/* Copyright (C) 2001-2017 Marc Le Douarain */
/* http://www.sourceforge.net/projects/classicladder */
/* http://sites.google.com/site/classicladder */
/* July 2009 */
/* ----------- */
/* System vars */
/* ----------- */
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
#include "time_and_rtc.h"
#include "tasks.h"
#include "vars_system.h"

int FlashSecs;
int FlashMins;
int ColdStart;
int HotStart;

int SummerWinterYearBak = -1;
int SummerWinterHourBak = -1;
int SummerDayAndMonth = -1;
int WinterDayAndMonth = -1;
char FlagWinterSwitch = FALSE;

void InitSystemVars( char HardwareStart )
{
	FlashSecs = 0;
	FlashMins = 0;
	if ( HardwareStart )
		ColdStart = 1;
	HotStart = 1;
}

void UpdateSystemVars( void )
{
	FlashSecs = FlashSecs+GeneralParamsMirror./*InfosGene->GeneralParams.*/PeriodMilliSecsTaskLogic;
	if ( FlashSecs>=1000 )
		FlashSecs = FlashSecs-1000;
	//1hz
	WriteVar( VAR_SYSTEM, 0, FlashSecs<500 );
	//~1Hz
	WriteVar( VAR_SYSTEM, 1, FlashSecs>=500 );

	FlashMins = FlashMins+GeneralParamsMirror./*InfosGene->GeneralParams.*/PeriodMilliSecsTaskLogic;
	if ( FlashMins>=60000 )
		FlashMins = FlashMins-60000;
	//1Min
	WriteVar( VAR_SYSTEM, 2, FlashSecs<30000 );
	//~1Min
	WriteVar( VAR_SYSTEM, 3, FlashSecs>=30000 );

	//boots
	WriteVar( VAR_SYSTEM, 4, ColdStart );
	WriteVar( VAR_SYSTEM, 5, HotStart );
	ColdStart = 0;
	HotStart = 0;

	//time vars
	//copied from a 'struct tm' var updated in another task (not real-time)
	LockTheMutex( MUTEX_TIME_COPY_FOR_VARS_SYS );
	//verify if valid values in it ?
	if ( tm_copy_for_vars_sys.tm_wday!=-1 )
	{
		WriteVar( VAR_WORD_SYSTEM, 0, tm_copy_for_vars_sys.tm_hour*10000+tm_copy_for_vars_sys.tm_min*100+tm_copy_for_vars_sys.tm_sec );
		WriteVar( VAR_WORD_SYSTEM, 1, (tm_copy_for_vars_sys.tm_year)%100*10000+(tm_copy_for_vars_sys.tm_mon+1)*100+tm_copy_for_vars_sys.tm_mday );
		WriteVar( VAR_WORD_SYSTEM, 2, tm_copy_for_vars_sys.tm_wday );
	}
	UnlockTheMutex( MUTEX_TIME_COPY_FOR_VARS_SYS );

//debug
WriteVar( VAR_WORD_SYSTEM, 9, FlashSecs );

	// modem
	if ( Modem.ModemUsed )
	{
		WriteVar( VAR_SYSTEM, 40, GetIfModemInitAndConfigOk( ) );
		WriteVar( VAR_SYSTEM, 41, GetIfModemIsConnected( ) );
	}
}



// can work for 31 days month only, nbr of years since 1900.
int CalcLastSundayDayInAMonth( int MonthSearch, int YearSearch )
{
	struct tm tm_val;
	tm_val.tm_mday = 31;
	tm_val.tm_mon = MonthSearch-1;
	tm_val.tm_year = YearSearch;
	tm_val.tm_hour = 0;
	tm_val.tm_min = 0;
	tm_val.tm_sec = 0;
	tm_val.tm_isdst = -1;
	// this call correctly set usefull tm_wday field for us
	mktime( &tm_val );
	// now find first sunday day starting from end of month
	while ( tm_val.tm_wday!=0/*sunday*/ )
	{
		tm_val.tm_mday--;
		tm_val.tm_wday--;
		if ( tm_val.tm_wday==-1 )
			tm_val.tm_wday = 6;
	}
printf("###> %s(), for year=%d and month=%d, return day=%d\n",__FUNCTION__, YearSearch, MonthSearch, tm_val.tm_mday);
	return tm_val.tm_mday;
}
void VerifyAutoAdjustSummerWinterTime( void )
{
	int ForThisYear = tm_copy_for_vars_sys.tm_year;
	if ( SummerWinterYearBak==-1 || SummerWinterYearBak!=ForThisYear )
	{
		SummerDayAndMonth = CalcLastSundayDayInAMonth( 3/*march*/, ForThisYear )+(2/*feb*/*31) ;
		WinterDayAndMonth = CalcLastSundayDayInAMonth( 10/*oct*/, ForThisYear )+(9/*sept*/*31);
printf("###> CALC WINTER=%d / SUMMER=%d DAYS\n",WinterDayAndMonth,SummerDayAndMonth);
		SummerWinterYearBak = ForThisYear;
	}
	if ( SummerWinterHourBak==-1 || tm_copy_for_vars_sys.tm_hour!=SummerWinterHourBak )
	{
		int DayAndMonthToday = tm_copy_for_vars_sys.tm_mday+tm_copy_for_vars_sys.tm_mon*31;
printf("###> CALC FOR WINTER/SUMMER TODAY=%d\n",DayAndMonthToday);
		if ( DayAndMonthToday==SummerDayAndMonth && tm_copy_for_vars_sys.tm_hour==2 )
		{
			// switch to summer time... oh it will be hard this morning... :-(
printf("###> SWITCHING TO SUMMER (hour+1)\n");
			tm_copy_for_vars_sys.tm_hour++;
			FlagWinterSwitch = FALSE;
			WriteVar( VAR_SYSTEM, 9, 1 );
printf("###> SET SYSVAR9 to 1\n");
			time_t IntTimeToSet = mktime( &tm_copy_for_vars_sys );
			if ( IntTimeToSet!=-1 )
				SetTimeClock( IntTimeToSet, Preferences.UseRtcDevice );
		}
		else if ( DayAndMonthToday==WinterDayAndMonth && tm_copy_for_vars_sys.tm_hour==3 && !FlagWinterSwitch)
		{
			// switch to winter time.
printf("###> SWITCHING TO WINTER (hour-1)\n");
			tm_copy_for_vars_sys.tm_hour--;
			FlagWinterSwitch = TRUE;
			WriteVar( VAR_SYSTEM, 9, 0 );
printf("###> SET SYSVAR9 to 0\n");
			time_t IntTimeToSet = mktime( &tm_copy_for_vars_sys );
			if ( IntTimeToSet!=-1 )
				SetTimeClock( IntTimeToSet, Preferences.UseRtcDevice );
		}
		else
		{
			WriteVar( VAR_SYSTEM, 9, (SummerDayAndMonth<=DayAndMonthToday && DayAndMonthToday<WinterDayAndMonth )?1:0);
printf("###> RECALC SYSVAR9 = %d\n", ReadVar( VAR_SYSTEM, 9 ));
		}
		SummerWinterHourBak = tm_copy_for_vars_sys.tm_hour;
	}
}

