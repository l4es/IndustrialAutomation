/**************************************************************************
 *                                                                        *
 * Light OPC Server development library                                   *
 *                                                                        *
 *   Copyright (c) 2000  Timofei Bondarenko                               *
 **************************************************************************/

/**************************************************************************
 Compile-time adjustable parameters
 **************************************************************************/
#ifndef OPTIONS_H
#define OPTIONS_H

#ifndef LO_INIT_VARIANT
#define LO_INIT_VARIANT  (0)
#endif
/* 0 use simplified VARIANTs initialization: VARIANT::vt_type = 0
   1 use VariantInit() */

/* Handling of zero timestamps by loCacheUpdate(): */
#ifndef LO_FILL_TIMESTAMP
#define LO_FILL_TIMESTAMP (0)
#endif
/* 0 - don't replace zero timestamps
   1 - replace zero timestamps by current time
   2 - replace zero timestamps by specified timestamp */

#ifndef LO_EV_TIMESTAMP
#define LO_EV_TIMESTAMP (2)
#endif
/* 0 - don't replace zero timestamps
   1,2 - replace zero by specified timestamp at the request handling */
#if LO_EV_TIMESTAMP && LO_FILL_TIMESTAMP
#define LO_FILL_TIMESTAMP (0)
#endif


#ifndef LO_AUTO_ALLOC
#define LO_AUTO_ALLOC (4)
#endif
/* > 1 - the number of stack-allocated entries for short buffers */

#ifndef LO_KEEP_OLD_CACHE
#define LO_KEEP_OLD_CACHE (1)
/* 1 == Keep values in cache buffer (accessible through loCacheLock())
        from one update to another.
   0 == Preserving of these values is not guaranteed.

  see cacheupd.c/loUpdatePipe() for details. */
#endif

#ifndef LO_NONBASIC_PROPDESCR
#define LO_NONBASIC_PROPDESCR (1)
/* 1 == include descrioption strings for non-basic items' properties. */
#endif

#ifndef ENUM_EMPTY
/* Style of returning of empty enumerators.
   See enum.h for details. */
#if 0
#define ENUM_EMPTY(hr,flg,pUnk) (hr = ENUM_EMPTY_SFALSE(pUnk))
/* According the standard: S_FALSE & empty enumerator */
#elif 0
#define ENUM_EMPTY(hr,flg,pUnk) (hr = ENUM_EMPTY_RT(flg,pUnk))
/* Using run-time flags loDf_EE_XXX */
#elif 0
#define ENUM_EMPTY(hr,flg,pUnk) (hr = ENUM_EMPTY_SOK(pUnk)) /* S_OK & empty enumerator */
#elif 0
#define ENUM_EMPTY(hr,flg,pUnk) (hr = ENUM_EMPTY_NULL(pUnk)) /* S_FALSE & NULL enumerator */
#endif
#endif

#ifndef LO_CHECK_RIGHTS
#define LO_CHECK_RIGHTS   (1)
#endif
/* Do OPC access_right checking in Read()/Write() */

#ifndef LO_USE_PTHREADS
# ifdef _WIN32
#  define LO_USE_PTHREADS   (0)
# else
#  define LO_USE_PTHREADS   (1)
# endif
#endif
#if LO_USE_PTHREADS
# define LO_USE_BOTHMODEL      (0)
# define LO_USE_FREEMARSHALL   (0)
# define LO_USE_FREEMARSH_ENUM (0)
# define LO_USE_HRCLOCK        (0)
#endif
/* Use pthreads instead of native Win32/MSVC threads.
   See losynch.h/c for details.
   <pthread.h> should be in include path; and appropriate library linked.
   On Win32 the pthreads make no advantages
   (and adds some timing problems under W95/98). */

#ifndef LO_USE_BOTHMODEL
#define LO_USE_BOTHMODEL  (1)
#endif
/* 1 support for the "both" threading model and loDf_BOTHMODEL
   0 "free" threading only - usefull for out-of-proc servers and Windows CE */

#ifndef LO_USE_FREEMARSHALL
#define LO_USE_FREEMARSHALL  (1)
#endif
/* 1 use CoCreateFreeThreadedMarshaler() for OPC Server & Group objects
   and do support for the loDf_FREEMARSH */

#ifndef  LO_USE_FREEMARSH_ENUM
#define  LO_USE_FREEMARSH_ENUM (1)/*LO_USE_FREEMARSHALL*/
#endif
/* 1 use CoCreateFreeThreadedMarshaler() for various enums
   I don't see the way to use an enumerator across multiple threads.
   See enum.h for advanced IEnum options */


#ifndef   LO_TIMEBACK
# ifdef  _WIN32
#  define LO_TIMEBACK (-1/*10000*/)
# else
#  define LO_TIMEBACK (0)
# endif
#endif
/* Under some circumances (such as a running DOS application in W2k).
   the Win32 API WaitFor...() may return before the timer will show
   appropriate time.
   To prevent extra calls to cond_timedwait() we may detect
   this case and then increase all timeouts by appropriare
   timer's granularity within the next (LO_TIMEBACK) milliseconds.
   0 - don't detect such things;
  <0 - use another (new) compensation method. */


#ifndef LO_USE_HRCLOCK
#define LO_USE_HRCLOCK    (0)
#endif
/* Using of high resolution clock does provide
   smooth bandwidth calculation.
   On another hand, the QueryPerformanceCounter() itself is
   10...100 times slower than GetTickCount().
   (It works via rdtsc/rpcc on NT/W2k *Server* only).
   So we decided to use GetTickCount() by default. */


#ifndef LO_CO_DISCONNECT
#if 1
#define LO_USE_OBJTRACK (1)
#define LO_CO_DISCONNECT(x,xx) CoDisconnectObject((x),(xx))
#else
#define LO_USE_OBJTRACK (0)
#define LO_CO_DISCONNECT(x,xx) ((void)0)
#endif
#endif
/* LO_CO_DISCONNECT allows to disable CoDisconnectObject(x,xx)
   LO_USE_OBJTRACK enables tracking of returned enums
   to do disconnect on them. */

#ifndef LO_USE_IAM_CHECK
#define LO_USE_IAM_CHECK  (1)
#endif
/* 1 use silly interface pointer validation in Enums & ConnPoints */

#ifndef USE_LOG
#define USE_LOG ll_DEBUG
#endif
#define UL_ifLEVEL  lolog
#include <unilog.h>
extern
#ifdef __cplusplus
       "C"
#endif
            unilog *lolog;
#define LOGID  lolog,NULL
#define INIT_LOG() unilog_Create("LightOPC", "|LightOPC","%!T", \
                    0, /* Max filesize: -1 unlimited, -2 -don't change */\
                    ll_INFO) /* level [ll_FATAL...ll_DEBUG] */


#ifndef LO_USE_MALLOCX
# if defined(USE_LOG) && USE_LOG >= ll_TRACE
#  define LO_USE_MALLOCX (1)
# else
#  define LO_USE_MALLOCX (0)
# endif
/* The mallocX() & Co. wrappers. Allows to debug some memory leaks. */
/* 1 spy for unfreed memory. See xmalloc.cpp for malloc() tracing tools. */
/* It is not a GNU xmalloc() one! */
#endif

#ifndef LO_USE_OBJXREF
#define LO_USE_OBJXREF (1)
#endif
/* 1 spy for undeleted objects */

#ifndef OleErrorsH
# if defined(__MINGW32__) && defined (__cplusplus)
#  define LO_OLE_ERRORS_h <ole2.h>
# else
#  define LO_OLE_ERRORS_h <olectl.h>
# endif
/* Use #include LO_OLE_ERRORS_h in place of #include <olectl.h>
   make sure use this include AFTER a private LightOPC header included. */
#endif

#endif /*OPTIONS_H*/
