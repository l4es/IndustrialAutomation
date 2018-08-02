/**************************************************************************
 *                                                                        *
 * Light OPC Server development library                                   *
 *                                                                        *
 *   Copyright (c) 2000 by Timofei Bondarenko                             *
                                                                          *
 loService timers & clocks
 **************************************************************************/

#include "loserv.h"
#include "losynch.h"
#include "util.h" /* lo_variant_to_filetime / lo_filetime_to_variant */

#ifdef __GNUC__
#define  LLINT(x)  (x##LL)
#define ULLINT(x)  (x##ULL)
#endif
#ifdef _MSC_VER
#define  LLINT(x)  (x##i64)
#define ULLINT(x)  (x##ui64) /* or i64u ...*/
#endif

#if   LO_USE_PTHREADS

#include <pthread.h>


unsigned long (*lo_millisecabs)(struct timespec *);

static unsigned long lo_millisecabs_ft(struct timespec *tn)
{
 FILETIME ft;
 ULONGLONG ll;
 GetSystemTimeAsFileTime(&ft);
 ll = ft.dwHighDateTime;
 ll = (ll << 32) + ft.dwLowDateTime;
 if (tn)
   {
    tn->tv_nsec = (long)(ll % 10000000) * 100; 
    tn->tv_sec  = (long)(ll / 10000000 - ULLINT(11644473600));
   } 
 return (unsigned long)(ll / 10000);
}

/* GetSystemTimeAsFileTime() on Win95/98 has too big granularity: 55ms.
   And it works about 300 times slower than GetTickCount().
   So we decided to use it only as timeshift for a *_timedwait() function. 
   Yes, the milliseconds returned and timespec are missynchronized, but 
   we don't carry about because the *_timedwait() will probably get the same
   timespec (due FILETIME granularity). */ 

static unsigned long lo_millisecabs_95(struct timespec *tn)
{
 unsigned long ms = GetTickCount();
 if (tn) 
   {
#if 0
    lo_millisecabs_ft(tn);
#elif 1
    FILETIME ft;
    ULONGLONG ll;
    GetSystemTimeAsFileTime(&ft);
    ll = ft.dwHighDateTime;
    ll = (ll << 32) + ft.dwLowDateTime;
    tn->tv_nsec = (long)(ll % 10000000) * 100;
    tn->tv_sec  = (long)(ll / 10000000 - ULLINT(11644473600));
#else 
    FILETIME ft;
    ULARGE_INTEGER li;
    GetSystemTimeAsFileTime(&ft);
    li.LowPart = ft.dwLowDateTime;
    li.HighPart = ft.dwHighDateTime;
    tn->tv_nsec = (long)( li.QuadPart % 10000000) * 100;
    tn->tv_sec  = (long)((li.QuadPart - ULLINT(116444736000000000))/ 10000000);
/* return (DWORD)(li.QuadPart / 10000u);*/     
#endif
   }
 return ms;
}

struct timespec *lo_absrel(struct timespec *tn, unsigned relms)
{
 ldiv_t ld;
/* struct timespec to = *tn; */
 ld = ldiv(relms, 1000);
 tn->tv_sec += ld.quot;
 ld = ldiv(ld.rem * 1000000 + tn->tv_nsec, 1000000000);
 tn->tv_sec += ld.quot;
 tn->tv_nsec = ld.rem;
#if 0
 UL_DEBUG((LOGID, "ABSREL: %03u.%07u + %u.%03u = %03u.%07u",
   to.tv_sec % 1000, to.tv_nsec / 100, 
   relms / 1000, relms % 1000,
   tn->tv_sec % 1000, tn->tv_nsec / 100));
#endif
 return tn;
}
#endif /*LO_USE_PTHREADS*/

void ld_current_time(const loCaller *cactx, FILETIME *ft)
{
 GetSystemTimeAsFileTime(ft);
}

loMilliSec    lo_timegran_ms;
unsigned long lo_timegran_100ns;
#if    LO_TIMEBACK  && LO_USE_PTHREADS
int lo_NO_TIMEBACK;
#endif

#if   LO_USE_HRCLOCK

DWORD (WINAPI *lo_millisec_clock)(void);

static ULONGLONG lo_hr_frequency;

static DWORD WINAPI lo_hr_millisec_clock(void)
{
 LARGE_INTEGER tq;
 QueryPerformanceCounter(&tq);
 return (DWORD)((ULONGLONG)tq.QuadPart / lo_hr_frequency);
}

#endif /* LO_USE_HRCLOCK */

void lo_setup_clock(void)
{
 static LONG init, initialized;

 if (0 != InterlockedExchange(&init, 1))
   {
    while(0 == initialized) Sleep(0);
    return;
   }

#if  LO_USE_HRCLOCK
 {
  LARGE_INTEGER freq;
  if (QueryPerformanceFrequency(&freq))
    {
     lo_hr_frequency = (freq.QuadPart + 500) / 1000;
     if (lo_hr_frequency <= 0x10000L) /* ~ 66 MHz */
       {
        /* It seems to be not a rdtsc/rpcc */
        lo_millisec_clock = GetTickCount;
       }
     else lo_millisec_clock = lo_hr_millisec_clock;
    }
  else lo_millisec_clock = GetTickCount;
  UL_INFO((LOGID, "HiRes timer has been %s (%ld.%03u MHz)",
      lo_millisec_clock == lo_hr_millisec_clock? "INITIALIZED": "REFUSED",
      (long)(lo_hr_frequency/1000), (int)(lo_hr_frequency%1000)));
 }
#endif /*LO_USE_HRCLOCK*/
 {
  DWORD ta, ti = 0;
  BOOL tb;
  if (GetSystemTimeAdjustment(&ta, &ti, &tb))
    {
     if (ti && ti < 100*10000)
       {
        UL_INFO((LOGID, "Timer granularity determined %u.%04ums; use: %ums", 
          ti / 10000, ti % 10000, (ti + 5000) / 10000));        
       }
     else /* WinE may report 0 */
       {        
        UL_INFO((LOGID, "Timer granularity is suspicious %u.%04ums: Assume 20ms", 
          ti / 10000, ti % 10000));        
        ti = 20 * 10000;
       }
     lo_timegran_100ns = ti;
     if (0 == (ti = (ti + 5000) / 10000)) ti = 1;
     lo_timegran_ms = ti;
#if     LO_USE_PTHREADS
     lo_millisecabs = lo_millisecabs_ft;
#endif
    }
  else /* Win95/98 do not support GetSystemTimeAdjustment(). */
    {
/* ftime() / GetSystemTime() in PTHREADS/Win95/98 have resolution 55ms
   therefore we can't use shorter sleeps safely. 
   ti = 60 * 10000; 549255;           */
     ti = 5 * 10000;
     UL_INFO((LOGID, "Timer granularity default %u.%04ums", 
              ti / 10000, ti % 10000));
     lo_timegran_100ns = ti;
     lo_timegran_ms = (ti + 5000) / 10000;
#if LO_USE_PTHREADS
     lo_millisecabs = lo_millisecabs_95;
#if LO_TIMEBACK
     lo_NO_TIMEBACK = 1;
#endif
#endif
    }
 }
 initialized = 1;
}

loMilliSec lo_default_timegran(loMilliSec *minwait)
{
 /* MSDN said about timer granularity:
    NT 3.5     16ms
    NT 4.0     10ms
    95/98      55ms

  I saw following (GetTickCount(), Sleep() and WaitFor*()):
    NT 4.0 AdvServer  15.625ms
    W2k Server        15.625ms *
    W2k Professional  10ms (10.0144ms) *
    Win95             13.73ms (54.9255ms**)
    Win98             5ms     (54.9255ms**)
    WinE/Linux        1ms     (10ms+10ms***)
  * - under W2k the granularity of Sleep()/WaitFor*() might drop to ~1ms 
      when an specefic application running.
  ** - For SystemTime/FILETIME.
  *** - Under WinE Wait*() have granularity 10ms and usally by 10ms 
        longer than requested (that's better than _shorter_ under W2k).

  Thus I use this _BOGUS_ code just for getting *default* granularity
 */
 if (minwait) 
   {
    *minwait = lo_timegran_ms < 10? 15: lo_timegran_ms;
   }
 return lo_timegran_ms;
#if 0
 {
  DWORD ti = 0;
  HANDLE ev;
  ev = CreateEvent(NULL, FALSE, FALSE, NULL);
  if (ev)
    {
     if (WAIT_TIMEOUT == WaitForSingleObject(ev, 1))
       {
        lo_millisec();
        WaitForSingleObject(ev, 1);
        ti = lo_millisec();
        WaitForSingleObject(ev, 1);
        ti = lo_millisec() - ti;
       }
     CloseHandle(ev);
     if (ti)
       {
        UL_INFO((LOGID, "Timer granularity determined is %dmS", ti));
        if (ti >= 10 && ti <= 100)
          {
           if (0 == timegran)
             InterlockedExchange(&timegran, ti);
           return ti;
          }
       }
    }
 }
#endif
#if 0
 {
  DWORD ti = 0;
  OSVERSIONINFO osvi;
  memset(&osvi, 0, sizeof(osvi));
  osvi.dwOSVersionInfoSize = sizeof(osvi);
  if (GetVersionEx(&osvi))
    {
     if (osvi.dwPlatformId == VER_PLATFORM_WIN32_NT)
       {
        ti = 25/*16*/;
        if (osvi.dwMajorVersion > 4) ti = 20/*10*/;
       }
     else ti = 50/*55*/;
     if (0 == timegran)
       {
        InterlockedExchange(&timegran, ti);
        UL_INFO((LOGID, "Timer granularity for the system is %dmS", ti));
       }
     return ti;
    }
 }
#endif
/* UL_NOTICE((LOGID, "Timer granularity default is %dmS", 25));
 return 25;
*/
}


/* Anyway this is more accurate than SystemTimeToVariantTime(); */

double lo_filetime_to_variant(const FILETIME *ft)
{
#if 0
 /* 109205 * 3600 * 24 == 9435312000 */
 int frac;
 LONGLONG ll = ft->dwHighDateTime;
 ll = (ll << 32) + ft->dwLowDateTime;
 ll -= LLINT(94353120000000000);
#if 0
 return (double)(ll / (24. * 3600. * 1e7));
#else
 frac = (int)(ll % 1000000000L);
 return (ll / 1000000000L + frac / 1e9) / (24. * 36.);
#endif
#elif 0
 /* 2 ^^ 32 == 4294967296
    109205 * 3600 * 24 * 1e7 / 4294967296 == 21968297
    109205 * 3600 * 24 * 1e7 % 4294967296 == 2836185088
 */
 return (-2836185088.
         +(ft->dwHighDateTime - 21968297.) * 4294967296.
         + ft->dwLowDateTime) / (24. * 3600. * 1e7);
#else
 return (-109205. * (24. * 3600. * 1e7)
         + ft->dwHighDateTime * 4294967296.
         + ft->dwLowDateTime) / (24. * 3600. * 1e7);
#endif
}

void lo_variant_to_filetime(FILETIME *ft, double vd)
{
 LONGLONG ll;
 ll = (LONGLONG)(vd * 24. * 3600. * 1e7 /*+0.5*/);
 ll += LLINT(94353120000000000);
 ft->dwLowDateTime = (DWORD)ll;
 ft->dwHighDateTime = (DWORD)(ll >> 32);
}

/* end of hr_clock.c */
