//-< STDTP.H >-------------------------------------------------------*--------*
// GigaBASE                  Version 1.0         (c) 1999  GARRET    *     ?  *
// (Post Relational Database Management System)                      *   /\|  *
//                                                                   *  /  \  *
//                          Created:     20-Nov-98    K.A. Knizhnik  * / [] \ *
//                          Last update: 10-Dec-98    K.A. Knizhnik  * GARRET *
//-------------------------------------------------------------------*--------*
// Standart type and macro definitions
//-------------------------------------------------------------------*--------*

#ifndef __STDTP_H__
#define __STDTP_H__

#include "config.h"

#ifdef GIGABASE_DLL
#ifdef INSIDE_GIGABASE
#define GIGABASE_DLL_ENTRY __declspec(dllexport)
#else
#define GIGABASE_DLL_ENTRY __declspec(dllimport)
#endif
#else
#define GIGABASE_DLL_ENTRY
#endif

#ifdef USE_NAMESPACES
#define BEGIN_GIGABASE_NAMESPACE namespace gigabase {
#define END_GIGABASE_NAMESPACE }
#define USE_GIGABASE_NAMESPACE using namespace gigabase;
#else
#define BEGIN_GIGABASE_NAMESPACE
#define END_GIGABASE_NAMESPACE
#define USE_GIGABASE_NAMESPACE 
#endif

#if defined(__MINGW32__) && !defined(_WIN32)
#define _WIN32
#endif

#if defined(_WIN32_WCE) && !defined(_WINCE)
#define _WINCE
#endif

#if defined(__APPLE__) && !defined(__FreeBSD__)
// MAC OS X is Free BSD
#define __FreeBSD__ 4
#endif

#if defined(USE_MFC_STRING) && !defined(USE_MFC) && !defined(USE_ATL)
#define USE_ATL 1
#endif

#if defined(USE_MFC_STRING)
#ifndef MFC_STRING
#define MFC_STRING CString
#endif
#endif

#ifdef USE_STD_STRING
#include <string>
#endif

#if defined(_WIN32)

#ifdef USE_MFC
#include <afx.h>
#include <winsock2.h>
#else
#include <windows.h>
#if defined(USE_ATL) && defined(USE_MFC_STRING)
#include <atlstr.h>
#endif
#endif

#ifdef _MSC_VER
#pragma warning(disable:4800 4355 4146 4251)
#endif
#else
#ifdef _AIX
#define INT8_IS_DEFINED
#endif
#ifndef NO_PTHREADS
#ifndef _REENTRANT
#define _REENTRANT
#endif
#endif
#endif



#if defined(__VACPP_MULTI__) // IBM compiler produce a lot of stupid warnings
#pragma report(disable, "CPPC1608")
#pragma report(disable, "CPPC1281")
#endif /* __VACPP_MULTI__ */


#ifdef _WINCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <stdarg.h>
#include <tchar.h>
#include "wince.h"

#else

#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <assert.h>
#include <stdarg.h>
#include <time.h>
#ifdef UNICODE
#include <wchar.h>
#include <wctype.h>
#else
#include <ctype.h>
#endif

#endif

#ifndef __IBMCPP__
BEGIN_GIGABASE_NAMESPACE
#endif

//#ifndef __BCPLUSPLUS__
#if !defined(_WIN32) && !defined(__cdecl)
#define __cdecl
#endif


// Align value 'x' to boundary 'b' which should be power of 2
#define DOALIGN(x,b)   (((x) + (b) - 1) & ~((b) - 1))


#ifdef UNICODE
typedef wchar_t char_t;
#define STRLEN(x)     wcslen(x)
#define STRCMP(x, y)  wcscmp(x, y)
#define STRCHR(x, y)  wcschr(x, y)
#define STRCPY(x, y)  wcscpy(x, y)
#define STRNCPY(x,y,z) wcsncpy(x, y, z)
#define STRCOLL(x, y) wcscoll(x, y)
#define STRNCMP(x,y,z) wcsncmp(x, y, z)
#define STRLITERAL(x) (wchar_t*)L##x
#define PRINTF        wprintf
#define FPRINTF       fwprintf
#define SPRINTF       swprintf
#define VSPRINTF      vswprintf
#define FSCANF        fwscanf
#define SSCANF        swscanf
#define GETC(x)       getwc(x)
#define UNGETC(x, y)  ungetwc(x, y)
#define TOLOWER(x)    towlower((x) & 0xFFFF)
#define TOUPPER(x)    towlower((x) & 0xFFFF)
#define ISALNUM(x)    iswalnum((x) & 0xFFFF)
#define STRSTR(x, y)  wcsstr(x, y)
#define STRXFRM(x,y,z) wcsxfrm(x, y, z)
#define STRFTIME(a,b,c,d) wcsftime(a,b,c,d)
#ifdef _WINCE
#define REMOVE_FILE(s) DeleteFile(s)
#define RENAME_FILE(p, q) MoveFile(p, q)
#else
#define REMOVE_FILE(s) _wremove(s)
#define RENAME_FILE(p, q) _wrename(p, q)
#endif
#define GETENV(x)     _wgetenv(x)
#define T_EOF         WEOF
#ifndef STD_STRING
#define STD_STRING    std::wstring
#endif
#ifndef _T
#define _T(x) L##x
#endif
#else
typedef char char_t;
#define STRLEN(x)     strlen(x)
#define STRCHR(x, y)  strchr(x, y)
#define STRCMP(x, y)  strcmp(x, y)
#define STRCPY(x, y)  strcpy(x, y)
#define STRNCPY(x,y,z) strncpy(x, y, z)
#define STRCOLL(x, y) strcoll(x, y)
#define STRNCMP(x,y,z) strncmp(x, y, z)
#define STRLITERAL(x) (char*)x
#define PRINTF        printf
#define FPRINTF       fprintf
#define SPRINTF       sprintf
#define VSPRINTF      vsprintf
#define FSCANF        fscanf
#define SSCANF        sscanf
#define GETC(x)       getc(x)
#define UNGETC(x, y)  ungetc(x, y)
#define TOLOWER(x)    tolower((x) & 0xFF)
#define TOUPPER(x)    toupper((x) & 0xFF)
#define ISALNUM(x)    isalnum((x) & 0xFF)
#define STRSTR(x, y)  strstr(x, y)
#define STRXFRM(x,y,z) strxfrm(x, y, z)
#define STRFTIME(a,b,c,d) strftime(a,b,c,d)
#define REMOVE_FILE(s)     remove(s)
#define RENAME_FILE(p, q)  rename(p, q)
#define GETENV(x)     getenv(x)
#ifndef STD_STRING
#define STD_STRING    std::string
#endif
#define T_EOF         EOF
#ifdef _T
#undef _T
#endif
#define _T(x) x
#endif

inline void strlower(char_t* dst, const char_t* src)
{
    int ch;
    do {
        ch = *src++;
        *dst++ = TOLOWER(ch);
    } while (ch != 0);
}

typedef signed char    db_int1;
typedef unsigned char  db_nat1;

typedef signed short   db_int2;
typedef unsigned short db_nat2;

typedef signed int     db_int4;
typedef unsigned int   db_nat4;

typedef unsigned char  db_byte;

typedef float  db_real4;
typedef double db_real8;

#if defined(_WIN32) && !defined(__MINGW32__)
typedef unsigned __int64 db_nat8;
typedef __int64          db_int8;
#if defined(__IBMCPP__)
#define INT8_FORMAT "%lld"
#define T_INT8_FORMAT _T("%lld")
#else
#define INT8_FORMAT "%I64d"
#define T_INT8_FORMAT _T("%I64d")
#endif
#define CONST64(c)  c
#else
#if SIZEOF_LONG == 8
typedef unsigned long db_nat8;
typedef signed   long db_int8;
#define INT8_FORMAT "%ld"
#define T_INT8_FORMAT _T("%ld")
#define CONST64(c)  c##L
#else
typedef unsigned long long db_nat8;
typedef signed   long long db_int8;
#ifdef __MINGW32__
#define INT8_FORMAT "%I64d"
#define T_INT8_FORMAT _T("%I64d")
#else
#define INT8_FORMAT "%lld"
#define T_INT8_FORMAT _T("%lld")
#endif
#define CONST64(c)  c##LL
#endif
#endif

// This mapping is done for compatibility reasons
typedef db_int1 int1;
typedef db_nat1 nat1;
typedef db_int2 int2;
typedef db_nat2 nat2;
typedef db_int4 int4;
typedef db_nat4 nat4;

typedef db_byte byte;

typedef db_real4 real4;
typedef db_real8 real8;

typedef db_nat8 nat8;

#ifndef INT8_IS_DEFINED
typedef db_int8 int8;
#endif


#if !defined(bool) && ((defined(__SUNPRO_CC) && __SUNPRO_CC_COMPAT < 5) || defined(__IBMCPP__))
#define bool  char
#define true  (1)
#define false (0)
#endif

#define nat8_low_part(x)  ((db_nat4)(x))
#define int8_low_part(x)  ((db_int4)(x))
#if defined(_MSC_VER) // bug in MVC 6.0
#define nat8_high_part(x) (sizeof(x) < 8 ? 0 : ((db_nat4)((db_nat8)(x)>>32)))
#define int8_high_part(x) (sizeof(x) < 8 ? 0 : ((db_int4)((db_int8)(x)>>32)))
#else
#define nat8_high_part(x) ((db_nat4)((db_nat8)(x)>>32))
#define int8_high_part(x) ((db_int4)((db_int8)(x)>>32))
#endif

#define cons_nat8(hi, lo) ((((db_nat8)(hi)) << 32) | (db_nat4)(lo))
#define cons_int8(hi, lo) ((((db_int8)(hi)) << 32) | (db_nat4)(lo))

#define MAX_NAT8  db_nat8(-1)

#ifndef BIG_ENDIAN
#define BIG_ENDIAN      4321    /* most-significant byte first (IBM, net) */
#endif
#ifndef LITTLE_ENDIAN
#define LITTLE_ENDIAN   1234
#endif

#ifndef BYTE_ORDER
#if defined(__sparc__) || defined(__m68k__)
#define BYTE_ORDER      BIG_ENDIAN
#else
#define BYTE_ORDER      LITTLE_ENDIAN
#endif
#endif

#if defined(_WIN32)
typedef HANDLE descriptor_t;
#else
typedef int descriptor_t;
#endif

#if !defined(_fastcall) && (!defined(_WIN32) || defined(__IBMCPP__) || defined(__MINGW32__))
#define _fastcall
#endif

#if defined(_WIN32) || !defined(NO_PTHREADS)
#define THREADS_SUPPORTED 1
#else
#define THREADS_SUPPORTED 0
#endif

#define itemsof(array) (sizeof(array)/sizeof*(array))

extern GIGABASE_DLL_ENTRY byte* dbMalloc(size_t size);
extern GIGABASE_DLL_ENTRY void  dbFree(void* ptr);

#if defined(__BCPLUSPLUS__)
#define qsort(arr, n, s, cmp) std::qsort(arr, n, s, cmp)
#endif

#ifndef __IBMCPP__
END_GIGABASE_NAMESPACE
#endif

#endif


