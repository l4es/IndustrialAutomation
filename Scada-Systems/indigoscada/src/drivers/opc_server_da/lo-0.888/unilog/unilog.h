/******************  Universal EventLogging utility  ****************
 **                                                                **
 *                    Copyright (c) 1996,1999 by Timofei Bondarenko *
 ********************************************************************/
#ifndef UNILOG_H
#define UNILOG_H  (0x0545)
#ifdef __cplusplus
extern "C" {
#endif

#ifndef SIZEOF_ARRAY
#define SIZEOF_ARRAY(x) (sizeof(x)/sizeof(x[0]))
#endif
#ifndef USE_LOG
#define USE_LOG ll_DEBUG
#endif
#define UNILOG_DLL_NAME "unilog.dll"
#ifndef UNILOG_PATH
#define UNILOG_PATH "Software\\" UNILOG_DLL_NAME
#endif

#ifndef  UL_PUBLIC
# define UL_PUBLIC  /*extern*/
/* This silly thing is not required when we use .DEF file 
# define UL_PUBLIC __declspec(dllexport) 
# define UL_PUBLIC __attribute__((dllexport)) */
/* This one may be useful for caller optimization
# define UL_PUBLIC __declspec(dllimport) */
#endif /*UL_PUBLIC*/

/* level: */
#define ll_FATAL         ( 0)
#define ll_ERROR         ( 1)
#define ll_WARNING       ( 2)
#define ll_MESSAGE       ( 3)
#define ll_INFO          ( 4)
#define ll_NOTICE        ( 5)
#define ll_TRACE         ( 6)
#define ll_DEBUG         ( 7)
#define UL_EV_ERROR      (ll_ERROR)   /* EVENTLOG_ERROR_TYPE    */
#define UL_EV_WARNING    (ll_WARNING) /* EVENTLOG_WARNING_TYPE  */
#define UL_EV_INFO       (ll_MESSAGE) /* EVENTLOG_INFORMATION_TYPE */
#define UL_EV_SUCCESS    ( 8) /* EVENTLOG_SUCCESS       */
#define UL_EV_AUDIT_SUCC ( 9) /* EVENTLOG_AUDIT_SUCCESS */
#define UL_EV_AUDIT_FAIL (10) /* EVENTLOG_AUDIT_FAILURE */

struct unilog;
typedef struct unilog unilog;

/* Advanced control functions *******************************/

UL_PUBLIC
unilog *unilog_Create(const char *entryname,
   /* name of the description in the UNILOG_PATH.
     Following parameters are defaults for notexisting entries: */
                      const char *fname, /* destination name in form:
                            ServerName|EventSource
                            |EventSource
                            FileName
                            FileName*title
                            FileName?      - use file locking
                            FileName?title - use file locking 
                            ?
                            ?title         - use dubugging console */
                       const char *title, /* optional string */
                       int max_kb, /* Max filesize: -1 unlimited,
                                                 -2 -don't change */
                       int level); /* level [ll_FATAL...ll_DEBUG] */
/* parameters are loaded from regitry : UNILOG_PATH\entryname
   if there are no such enty the entry will be created using 
   specified parameters. 
   The <fname> may contain environment strings, such as:
    "%TEMP%\mylog.txt"
   Though it is not acceptable for unilog_Open()! */

UL_PUBLIC
void unilog_Delete(unilog *log);
/* close the log opened by unilog_Create().
   The registry entry is preserved */

UL_PUBLIC
unilog *unilog_Refresh(const char *entryname);
 /* reload parameters from the registry, if entryname is 0 the all
    configured entries will be refreshed */

UL_PUBLIC
void unilog_Redirect(const char *dst, const char *src, int glob);
 /* Redirect the log under src entry to the dst entry.
    The both enties are not required to be opened or even exist.
    if dst = 0 then redirection will be revoked.
    src = 0 mean "all redirections".

    dst src glob

     *   A    1   redirect src only;
             -1   redirect all entries already directed to src,
                  except src entry itself
              0   redirect all entries already directed to src.

     A   0    *   redirect all entries to dst

     0   0    *   cancel all redirections
 */
/* Raw control functions ************************************/

UL_PUBLIC
unilog *unilog_Open(const char *fname, /* destination name in form:
                            NULL - don't change
                            ServerName|EventSource
                            |EventSource
                            FileName
                            FileName*title
                            FileName?      - use file locking
                            FileName?title - use file locking 
                            ?
                            ?title         - use dubugging console */
                    const char *title, /* optional string */
                    int max_kb, /* Max filesize: -1 unlimited,
                                                 -2 -don't change */
                    int level); /* level [ll_FATAL...ll_DEBUG] */

UL_PUBLIC
unilog *unilog_ReOpen(unilog *base, /* modify an existing log */
                      const char *fname,
                      const char *title,
                      int max_kb, int level);

UL_PUBLIC
void unilog_Close(unilog *log);

UL_PUBLIC
int  unilog_Check(unilog *log);
/* Returns:
   0 - log is valid and connected to a storage
   1 - log is valid and not connected to a storage;
       it can be used for unilog_v/ReOpen/Close;
  -1 - log is invalid */

/* MAIN utilites **************************************************/

#ifdef va_start
UL_PUBLIC
void unilog_v(int level, unilog *log, const char *title, const char *fmt, va_list arg);
#endif
/* if fmt begins from "%!" then following format chars allowed:
     e - print errno from arglist;
     E - print current errno
     l - print last error from arglist;
     L - print GetLastError();
     t - print thread ID from arglist;
     T - print GetCurrentThreadId();
     p - print PID from arglist;
     P - print _getpid();
     r - print RAS error from arglist;
     s - print socket's error from arglist;
     S - print WSAGetLastError();
     U - print UID;
     y - print SSP error from arglist;
     V - print level of the message (E1 mean ll_ERROR===1);
     Y - don't print level of the message;
  EXAMPLE: "%!ELSP General Error %s", "string"
  NOTE: T, P, V and Y also allowed in the title 
  Any lowercase caharacter following "%!" will cause removing 
      of one argument from arglist.
 */

UL_PUBLIC
void unilog_l(int level, unilog *log, const char *title, const char *fmt, ...);

UL_PUBLIC
int unilog_Register(const char* ServiceName, const char* FilePath);
UL_PUBLIC
int unilog_UnRegister(const char* ServiceName);
UL_PUBLIC
int unilog_IsRegistered(const char* ServiceName);

const char *sockerror(int ecode);
const char *ssperror(int ecode);
unsigned raserror(char *buf, unsigned size, int ecode);

/***********************************************************************/
#ifdef  UL_ifLEVEL
/* You may define it as either:
              #define UL_ifLEVEL (&ul_level)
              #include <unilog.h>
              int ul_level = ll_INFO;
   OR
              #define UL_ifLEVEL (ul_log)
              #include <unilog.h>
              unilog *ul_log; 
  And see what's happen:
*/      

#define UL_L(level,ARG) do { if (0 == (UL_ifLEVEL) || \
                               *((int*)(UL_ifLEVEL)) >= ll_##level) \
                                   unilog_##level ARG; } while(0)
#else
#define UL_L(level,ARG) unilog_##level ARG
#endif
/***********************************************************************/

#if ll_FATAL <= USE_LOG
UL_PUBLIC
void unilog_FATAL(unilog *log, const char *title, const char *fmt, ...);
# define UL_FATAL(ARG) UL_L(FATAL,ARG)
#else
# define UL_FATAL(ARG)
#endif

#if ll_ERROR <= USE_LOG
UL_PUBLIC
void unilog_ERROR(unilog *log, const char *title, const char *fmt, ...);
# define UL_ERROR(ARG) UL_L(ERROR,ARG)
#else
# define UL_ERROR(ARG)
#endif

#if ll_WARNING <= USE_LOG
UL_PUBLIC
void unilog_WARNING(unilog *log, const char *title, const char *fmt, ...);
# define UL_WARNING(ARG) UL_L(WARNING,ARG)
#else
# define UL_WARNING(ARG)
#endif

#if ll_MESSAGE <= USE_LOG
UL_PUBLIC
void unilog_MESSAGE(unilog *log, const char *title, const char *fmt, ...);
# define UL_MESSAGE(ARG) UL_L(MESSAGE,ARG)
#else
# define UL_MESSAGE(ARG)
#endif

#if ll_INFO <= USE_LOG
UL_PUBLIC
void unilog_INFO(unilog *log, const char *title, const char *fmt, ...);
# define UL_INFO(ARG) UL_L(INFO,ARG)
#else
# define UL_INFO(ARG)
#endif

#if ll_NOTICE <= USE_LOG
UL_PUBLIC
void unilog_NOTICE(unilog *log, const char *title, const char *fmt, ...);
# define UL_NOTICE(ARG) UL_L(NOTICE,ARG)
#else
# define UL_NOTICE(ARG)
#endif

#if ll_TRACE <= USE_LOG
UL_PUBLIC
void unilog_TRACE(unilog *log, const char *title, const char *fmt, ...);
# define UL_TRACE(ARG) UL_L(TRACE,ARG)
#else
# define UL_TRACE(ARG)
#endif

#if ll_DEBUG <= USE_LOG
UL_PUBLIC
void unilog_DEBUG(unilog *log, const char *title, const char *fmt, ...);
# define UL_DEBUG(ARG) UL_L(DEBUG,ARG)
#else
# define UL_DEBUG(ARG)
#endif

#if       0 > USE_LOG
#define unilog_Create(a,b,c,d,e)  (0)
#define unilog_Refresh(a)         (0)
#define unilog_Delete(a)
#define unilog_Redirect(a,b,c)

#define unilog_Open(a,b,c,d)      (0)
#define unilog_ReOpen(a,b,c,d,e)  (0)
#define unilog_Close(a)
#define unilog_Check(a)           (0)

#define unilog_Register(a,b)
#define unilog_UnRegister(a)
#define unilog_IsRegistered(a)    (0)

#define unilog_v(a,b,c,d,e)
#endif /* 0 > USE_LOG */

#ifdef __cplusplus
          }
#endif
#endif /*UNILOG_H*/
