/* Copyright (c) 1996-2017, OPC Foundation. All rights reserved.

   The source code in this file is covered under a dual-license scenario:
     - RCL: for OPC Foundation members in good-standing
     - GPL V2: everybody else

   RCL license terms accompanied with this source code. See http://opcfoundation.org/License/RCL/1.00/

   GNU General Public License as published by the Free Software Foundation;
   version 2 of the License are accompanied with this source code. See http://opcfoundation.org/License/GPLv2

   This source code is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
*/

/******************************************************************************************************/
/* Platform Portability Layer                                                                         */
/* Modify the content of this file according to the event implementation on your system.              */
/* This is the win32 implementation                                                                   */
/******************************************************************************************************/

/* System Headers */
#include <windows.h>
#include <time.h>

/* UA platform definitions */
#include <opcua_p_internal.h>

/* own headers */
#include <opcua_datetime.h>
#include <opcua_p_datetime.h>

static const OpcUa_Int64 SECS_BETWEEN_EPOCHS = 11644473600;
static const OpcUa_Int64 SECS_TO_100NS = 10000000; /* 10^7 */
static const OpcUa_Int64 MSECS_TO_100NS = 10000; /* 10^4 */
static const OpcUa_Int64 MICROSECS_TO_100NS = 10; /* 10 */
static int daysInMonth[2][12] = {
    {31,28,31,30,31,30,31,31,30,31,30,31},
    {31,29,31,30,31,30,31,31,30,31,30,31}
};
#define IS_LEAP(n)      ((!((n) % 400) || (!((n) % 4) && ((n) % 100))) != 0)
#define LEAP_YEARS_SINCE_1601(year) (((year)-1601) / 4) - (((year)-1601) / 100) + (((year)-1601) / 400)

/*============================================================================
 * The OpcUa_GetTimeOfDay function (returns the time in OpcUa_TimeVal format)
 *===========================================================================*/
OpcUa_Void OPCUA_DLLCALL OpcUa_P_DateTime_GetTimeOfDay(OpcUa_TimeVal* a_pTimeVal)
{
    OpcUa_DateTime dateTime = OpcUa_P_DateTime_UtcNow();
    OpcUa_Int64 unixtime = dateTime.dwHighDateTime;

    unixtime <<= 32;
    unixtime += dateTime.dwLowDateTime;
    unixtime /= MICROSECS_TO_100NS;
    a_pTimeVal->uintMicroSeconds = (OpcUa_UInt32)(unixtime % 1000000);
    unixtime /= 1000000;
    unixtime -= SECS_BETWEEN_EPOCHS;
    a_pTimeVal->uintSeconds = unixtime;
}

/*============================================================================
 * The OpcUa_UtcNow function (returns the time in OpcUa_DateTime format)
 *===========================================================================*/
OpcUa_DateTime OPCUA_DLLCALL OpcUa_P_DateTime_UtcNow()
{
    FILETIME ftTime;
#ifdef _WIN32_WCE
    SYSTEMTIME sysTime;
#endif /* _WIN32_WCE */
    OpcUa_DateTime tmpDateTime;

#ifdef _WIN32_WCE
    GetSystemTime(&sysTime);
    SystemTimeToFileTime(&sysTime, &ftTime);
#else /* _WIN32_WCE */
    GetSystemTimeAsFileTime(&ftTime);
#endif /* _WIN32_WCE */

    tmpDateTime.dwHighDateTime = (OpcUa_UInt32)ftTime.dwHighDateTime;
    tmpDateTime.dwLowDateTime  = (OpcUa_UInt32)ftTime.dwLowDateTime;

    return tmpDateTime;
}

/*============================================================================
 * Convert DateTime into String
 *===========================================================================*/
OpcUa_StatusCode OPCUA_DLLCALL OpcUa_P_DateTime_GetStringFromDateTime(  OpcUa_DateTime a_DateTime,
                                                                        OpcUa_StringA  a_pBuffer,
                                                                        OpcUa_UInt32   a_uLength)
{
    const char*         formatString = "%04d-%02d-%02dT%02d:%02d:%02d.%03dZ";
    OpcUa_UInt64 unixtime = a_DateTime.dwHighDateTime;
    int ms;
    int tm_sec, tm_min, tm_hour;
    int tm_year, tm_mon, tm_mday;
    int apiResult;
    int leapYears;

    unixtime <<= 32;
    unixtime += a_DateTime.dwLowDateTime;
    unixtime /= MSECS_TO_100NS;
    ms = (int)(unixtime % 1000);
    unixtime /= 1000;
    tm_sec = (int)(unixtime % 60);
    unixtime /= 60;
    tm_min = (int)(unixtime % 60);
    unixtime /= 60;
    tm_hour = (int)(unixtime % 24);
    unixtime /= 24;
    tm_mday = (int)unixtime; /* days never cause overflow */

    /* calculate years and remaining days in year according to leap years */
    /* first assumption, assume every year has 365 days */
    tm_year = 1601 + tm_mday / 365;
    tm_mday = tm_mday % 365;

    leapYears = LEAP_YEARS_SINCE_1601(tm_year);
    /* correct remaining days according to "used" leap days */
    tm_mday -= leapYears;
    /* handle possible remaining days underflow
      A loop can be implemented here since the correction has to be executed maximal 5 times */
    while(tm_mday < 0)
    {
        tm_year--;
        if(IS_LEAP(tm_year))
        {
            tm_mday += 366;
        }
        else
        {
            tm_mday += 365;
        }
    }

    if(tm_year > 9999)
    {
        return OpcUa_Bad;
    }

    tm_mon = 0;
    while(tm_mon < 11)
    {
        if(tm_mday < daysInMonth[IS_LEAP(tm_year)][tm_mon])
        {
            break;
        }
        tm_mday -= daysInMonth[IS_LEAP(tm_year)][tm_mon];
        tm_mon++;
    }

#if OPCUA_USE_SAFE_FUNCTIONS
    apiResult = OpcUa_SPrintfA(a_pBuffer, a_uLength, formatString,
        tm_year, tm_mon+1, tm_mday+1,
        tm_hour, tm_min, tm_sec, ms);
#else /* OPCUA_USE_SAFE_FUNCTIONS */
    OpcUa_ReferenceParameter(a_uLength);
    if(a_uLength < 25)
    {
        return OpcUa_BadInvalidArgument;
    }
    apiResult = OpcUa_SPrintfA(a_pBuffer, formatString,
        tm_year, tm_mon+1, tm_mday+1,
        tm_hour, tm_min, tm_sec, ms);
#endif /* OPCUA_USE_SAFE_FUNCTIONS */

    if(apiResult < 20)
    {
        return OpcUa_Bad;
    }

    return OpcUa_Good;
}

/*============================================================================
 * Convert String into DateTime
 *===========================================================================*/
OpcUa_StatusCode OPCUA_DLLCALL OpcUa_P_DateTime_GetDateTimeFromString(OpcUa_StringA   a_pchDateTimeString,
                                                                OpcUa_DateTime* a_pDateTime)
{
    int         milliSet    = 0;
    int         tm_sec, tm_min, tm_hour;
    int         tm_year, tm_mon, tm_mday;
    int         ms          = 0;
    size_t      stringLength;
    size_t      maxStringLength;
    char        years[5]    = "YYYY";
    char        months[3]   = "MM";
    char        days[3]     = "DD";
    char        hours[3]    = "HH";
    char        minutes[3]  = "MM";
    char        seconds[3]  = "SS";
    char        millis[4]   = "000";
    char        timeZone[4] = "000";
    int         zoneValue   = 0;
    int         signPosition;
    int         tmpVar;
    OpcUa_Int64 unixtime;

    /***************************************************************
     *  ToDo:
     *  Timezone can have values from -12 to +14
     *  At the moment only timezones from -12 to +12 are expected
     *  timezones can also have minutes
     *  at the moment minutes are ignored
     ***************************************************************/

    if(    a_pchDateTimeString  == OpcUa_Null
        || a_pDateTime          == OpcUa_Null)
    {
        return OpcUa_BadInvalidArgument;
    }

    /* ToDo: set max stringlength we accept */
    maxStringLength = 50;

    stringLength = strlen(a_pchDateTimeString);

    /*  check length of string -> there can be any number of digits behind ms */
    /*  we'll ignore anything beyond 3 */
    if(stringLength < 20 || stringLength > maxStringLength)
    {
        return OpcUa_BadSyntaxError;
    }

    /* simple syntax check */
    /* ToDo: we might add some syntax checks here */
    if(a_pchDateTimeString[4] == '-' && a_pchDateTimeString[7] == '-' && (a_pchDateTimeString[10] == 'T' || a_pchDateTimeString[10] == 't') && a_pchDateTimeString[13] == ':' &&  a_pchDateTimeString[16] == ':' )
    {
        /* copy strings */
#if OPCUA_USE_SAFE_FUNCTIONS
        strncpy_s(years, 5, a_pchDateTimeString, 4);
        strncpy_s(months, 3, a_pchDateTimeString+5, 2);
        strncpy_s(days, 3, a_pchDateTimeString+8, 2);
        strncpy_s(hours, 3, a_pchDateTimeString+11, 2);
        strncpy_s(minutes, 3, a_pchDateTimeString+14, 2);
        strncpy_s(seconds, 3, a_pchDateTimeString+17, 2);
#else /* OPCUA_USE_SAFE_FUNCTIONS */
        strncpy(years, a_pchDateTimeString, 5);
        strncpy(months, a_pchDateTimeString+5, 2);
        strncpy(days, a_pchDateTimeString+8, 2);
        strncpy(hours, a_pchDateTimeString+11, 2);
        strncpy(minutes, a_pchDateTimeString+14, 2);
        strncpy(seconds, a_pchDateTimeString+17, 2);
#endif /* OPCUA_USE_SAFE_FUNCTIONS */

        /* parse date and time */
        tm_year = strtol(years, 0, 10);
        if(tm_year < 1601 || tm_year > 9999)
        {
            return OpcUa_BadOutOfRange;
        }
        tm_mon = strtol(months, 0, 10) - 1;
        if(tm_mon < 0 || tm_mon > 11)
        {
            return OpcUa_BadOutOfRange;
        }
        tm_mday = strtol(days, 0, 10) - 1;
        if(tm_mday < 0 || tm_mday >= daysInMonth[IS_LEAP(tm_year)][tm_mon])
        {
            return OpcUa_BadOutOfRange;
        }
        tm_hour = strtol(hours, 0, 10);
        if(tm_hour < 0 || tm_hour > 23)
        {
            return OpcUa_BadOutOfRange;
        }
        tm_min = strtol(minutes, 0, 10);
        if(tm_min < 0 || tm_min > 59)
        {
            return OpcUa_BadOutOfRange;
        }
        tm_sec = strtol(seconds, 0, 10);
        if(tm_sec < 0 || tm_sec > 59)
        {
            return OpcUa_BadOutOfRange;
        }

        signPosition = 19;

        /* check if ms are set */
        if(a_pchDateTimeString[signPosition] == '.')
        {
            milliSet = 1;
        }

        /* find sign for timezone or Z (we accept 'z' and 'Z' here) */
        while(a_pchDateTimeString[signPosition] != '\0' && a_pchDateTimeString[signPosition] != '+' && a_pchDateTimeString[signPosition] != '-' && a_pchDateTimeString[signPosition] != 'Z' && a_pchDateTimeString[signPosition] != 'z')
        {
            ++signPosition;
        }

        if(a_pchDateTimeString[signPosition] == 'z' ||a_pchDateTimeString[signPosition] == 'Z')
        {
            /* utc time */
            if(milliSet)
            {
                /* be careful we can have more or less than 3 digits of milliseconds */
                tmpVar = signPosition - 20;
                if(tmpVar > 3)
                {
                    tmpVar = 3;
                }

#if OPCUA_USE_SAFE_FUNCTIONS
                strncpy_s(millis, 4, a_pchDateTimeString+20, tmpVar);
#else /* OPCUA_USE_SAFE_FUNCTIONS */
                strncpy(millis, a_pchDateTimeString+20, tmpVar);
#endif /* OPCUA_USE_SAFE_FUNCTIONS */

                ms = strtol(millis, 0, 10);
            }
        }
        else if(a_pchDateTimeString[signPosition] == '+' || a_pchDateTimeString[signPosition] == '-')
        {
            /* copy timezone */
#if OPCUA_USE_SAFE_FUNCTIONS
            strncpy_s(timeZone, 4, a_pchDateTimeString+signPosition, 3);
#else /* OPCUA_USE_SAFE_FUNCTIONS */
            strncpy(timeZone, a_pchDateTimeString+signPosition, 3);
#endif /* OPCUA_USE_SAFE_FUNCTIONS */

            /* strtol will take care of the sign */
            zoneValue = strtol(timeZone, 0, 10);

            if(zoneValue < -12 || zoneValue > 12)
            {
                return OpcUa_BadOutOfRange;
            }

            if(milliSet)
            {
                /* be careful we can have more or less than 3 digits of milliseconds */
                tmpVar = signPosition - 20;
                if(tmpVar > 3)
                {
                    tmpVar = 3;
                }

                /* copy timezone */
#if OPCUA_USE_SAFE_FUNCTIONS
                strncpy_s(millis, 4, a_pchDateTimeString+20, tmpVar);
#else /* OPCUA_USE_SAFE_FUNCTIONS */
                strncpy(millis, a_pchDateTimeString+20, tmpVar);
#endif /* OPCUA_USE_SAFE_FUNCTIONS */

                ms = strtol(millis, 0, 10);
            }
        }
        else
        {
            /* error -> no timezone specified */
            /* a time without timezone is not an absolute time but a time span */
            /* we might handle this as UTC */
            return OpcUa_BadSyntaxError;
        }

        /* correct time to UTC */
        tmpVar = tm_hour - zoneValue;
        if(tmpVar > 23)
        {
            tm_hour = tmpVar - 24;
            tm_mday++;     /* add one day to date */
        }
        else if(tmpVar < 0)
        {
            tm_hour = tmpVar + 24;
            tm_mday--;    /* substract one day from date */
        }
        else
        {
            tm_hour = tm_hour - zoneValue;
        }
    }
    else /* if(strchr(a_pchDateTimeString, ':')) */
    {
        /* other formats are not supported at the moment */
        /* 20060606T06:48:48Z */
        /* 20060606T064848Z */
        return OpcUa_BadSyntaxError;
    }

    /* compute days in year */
    for(tmpVar = 0; tmpVar < tm_mon; tmpVar++)
    {
        tm_mday += daysInMonth[IS_LEAP(tm_year)][tmpVar];
    }

    /* compute days since 1.1.1601, (including leap days) */
    unixtime = 365*(tm_year-1601) + tm_mday + LEAP_YEARS_SINCE_1601(tm_year);

    /* convert to seconds */
    unixtime *= 24*3600;
    /* add day time to 64 bit value */
    unixtime += 3600*tm_hour + 60*tm_min + tm_sec;
    /* convert to FILETIME */
    unixtime *= SECS_TO_100NS;
    /* add the milliseconds */
    unixtime += ms * MSECS_TO_100NS;

    a_pDateTime->dwHighDateTime = unixtime >> 32;
    a_pDateTime->dwLowDateTime  = unixtime & 0xffffffff;

    return OpcUa_Good;
}
