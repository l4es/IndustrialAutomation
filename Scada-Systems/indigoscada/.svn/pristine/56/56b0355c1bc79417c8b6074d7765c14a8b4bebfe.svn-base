/******************  Universal EventLogging utility  ****************
 **                                                                **
 *                    Copyright (c) 1996,1999 by Timofei Bondarenko *
 ********************************************************************/
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdarg.h>
#include <stdlib.h>
#include <io.h>
#include <fcntl.h>
#include <share.h>
#include <process.h>
#include <sys/stat.h>
#include <sys/locking.h>
#include <aclapi.h>

#include <windows.h>
#include "unilog.h"
#include "logger.h"

#ifndef EVENTLOG_SUCCESS
#define EVENTLOG_SUCCESS (0) /* for MINGW32 */
#endif
#if 0
#define MY_LANG  MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT)
#else
#define MY_LANG  (0x409) /*MAKELANGID(LANG_NEUTRAL, SUBLANG_NEUTRAL)*/
#endif

typedef struct {
                int level;
                int msgid;
                WORD type;
                char flag;
               } ulMsgType;

static const ulMsgType log_msgmap[] =
{
/*ll_FATAL          0*/ ll_FATAL,  EV_LOG_ERROR,  EVENTLOG_ERROR_TYPE,      'F',
/*ll_ERROR          1*/ ll_ERROR,  EV_LOG_ERROR,  EVENTLOG_ERROR_TYPE,      'E',
/*ll_WARNING        2*/ ll_WARNING,EV_LOG_WARNING,EVENTLOG_WARNING_TYPE,    'W',
/*ll_MESSAGE        3*/ ll_MESSAGE,EV_LOG_INFO,   EVENTLOG_INFORMATION_TYPE,'M',
/*ll_INFO           4*/ ll_INFO,   EV_LOG_INFO,   EVENTLOG_INFORMATION_TYPE,'I',
/*ll_NOTICE         5*/ ll_NOTICE, EV_LOG_INFO,   EVENTLOG_INFORMATION_TYPE,'N',
/*ll_TRACE          6*/ ll_TRACE,  EV_LOG_INFO,   EVENTLOG_INFORMATION_TYPE,'T',
/*ll_DEBUG          7*/ ll_DEBUG,  EV_LOG_INFO,   EVENTLOG_INFORMATION_TYPE,'D',
/*UL_EV_SUCCESS     8*/ ll_INFO,   EV_LOG_SUCCESS,EVENTLOG_SUCCESS,         'S',
/*UL_EV_AUDIT_SUCC  9*/ ll_MESSAGE,EV_LOG_SUCCESS,EVENTLOG_AUDIT_SUCCESS,   'a',
/*UL_EV_AUDIT_FAIL 10*/ ll_MESSAGE,EV_LOG_SUCCESS,EVENTLOG_AUDIT_FAILURE,   'f'
};

enum ulMODE { ulNOTUSED = 0,
              ulEVLOG   = 1,
              ulFILE    = 2,
              ulFILEL   = 3,
              ulDEBUG   = 4,
              ulLAST    = 4  };

struct unilog
      {
       int    level;
       HANDLE eh;
       int    fd;
       int    max_kb;
       enum ulMODE mode;
       char title[32];
       char storage[300];
       DWORD lang;
       struct unilog *iam;
       CRITICAL_SECTION lk; /* for future use */
      };

static struct unilog default_log =
  { ll_NOTICE, NULL, -1, -1, ulNOTUSED, "", "", MY_LANG, NULL, 0 };
static LONG log_lock;

static const struct
	{
	 char build[10];
	 char date[32];
	} identify =
#define sh(v,x) ('0' + (v >> x & 15))
#define stri(v) sh(v,28),sh(v,24),sh(v,20),sh(v,16),sh(v,12),sh(v,8),sh(v,4),sh(v,0)
	{ { '\r', '\n', stri(UNILOG_H) }, " " __DATE__ " " __TIME__ " sign @\r\n"	};

int unilog_Check(unilog *log)
{
 if (!log || log != log->iam) return -1;
 if (log->mode == ulNOTUSED) return 1;
 return 0;
}

unilog *unilog_Open(const char *fname, const char *title,
                    int max_kb, int level)
{
 unilog *log;

 if (!fname /*|| !*fname*/)
   {
    if (default_log.iam == &default_log ||
        InterlockedExchange(&log_lock, 1))
      return &default_log; /* Let's concurrent to do our Job */
    fname = "|" UNILOG_DLL_NAME;
    goto mkDef;
   }
 if (default_log.mode == ulNOTUSED && default_log.iam == NULL &&
     0 == InterlockedExchange(&log_lock, 1))
   {
mkDef:
    InitializeCriticalSection(&default_log.lk);
    default_log.iam = &default_log;
    log = &default_log;
   }
 else
   {
/*mkNew:*/
    log = (unilog*)malloc(sizeof(unilog));
    if (!log) return NULL;
    log->level = ll_NOTICE;
    log->eh = NULL;
    log->fd = -1;
    log->max_kb = -1;
    log->mode = ulNOTUSED;
    log->title[0] = '\0';
    log->storage[0] = '\0';
    log->lang = MY_LANG;
    InitializeCriticalSection(&log->lk);
    log->iam = log;
   }
 unilog_ReOpen(log, fname, title, max_kb, level);
 return log;
}

void unilog_Close(unilog *base)
{
 if (!base || base->iam != base) return;
 EnterCriticalSection(&base->lk);
 if (base->iam != base)
   LeaveCriticalSection(&base->lk);
 else
   {
    int    old_fd;
    HANDLE old_eh;
    base->iam = NULL;
    old_fd = base->fd;
    old_eh = base->eh;
    base->mode = ulNOTUSED;
    base->eh = NULL;
    base->fd = -1;
    if (old_eh) DeregisterEventSource(old_eh);
    if (old_fd != -1) close(old_fd);
    DeleteCriticalSection(&base->lk);
    if (base == &default_log)
      {
       base->level = ll_NOTICE;
       base->max_kb = -1;
       base->title[0] = '\0';
       base->storage[0] = '\0';
       base->lang = MY_LANG;
       InterlockedExchange(&log_lock, 0);
      }
    else free(base);
   }
}

unilog *unilog_ReOpen(unilog *base,
                      const char *fname, const char *title,
                      int max_kb, int level)
{
 char storage[FILENAME_MAX+16];
 const char *source = NULL;
 size_t len;
 enum ulMODE mode = ulNOTUSED;
 int   rc = 0;

 if (!base)
   {
    base = &default_log;
    if (base->iam != base ||
        base->mode < ulNOTUSED ||
        base->mode > ulLAST)
      return unilog_Open(fname, title, max_kb, level);
   }
 if (base->iam != base ||
     base->mode < ulNOTUSED || base->mode > ulLAST) return NULL;

 if (!fname)
   {
    fname = base->storage;
    EnterCriticalSection(&base->lk); rc = 1;
   }
 len = strcspn(fname, "?*|<>");
 switch(fname[len])
   {
 case  '?': mode = len? ulFILEL: ulDEBUG; break;
 case  '*': mode = ulFILE;  break;
 case '\0': mode = ulFILE;  break;
 case  '|': mode = ulEVLOG; break;
 default:   mode = ulEVLOG; break;
   }
 if (0 == len && 
     mode != ulEVLOG && 
     mode != ulDEBUG) 
     mode = ulNOTUSED;
 else if (fname[len])
   {
    source = fname + len + 1;
    if (*source && mode != ulEVLOG && !title) title = source;
   }
 if (len > sizeof(storage)-1) len = sizeof(storage)-1;
 memcpy(storage, fname, len); storage[len] = '\0';

 if (rc) rc = 0;
 else EnterCriticalSection(&base->lk); /***************************/

 if (mode != base->mode ||
     /*fname && */stricmp(fname, base->storage))
   {
    int    old_fd = base->fd;
    HANDLE old_eh = base->eh;

    switch(mode)
      {
    case ulEVLOG:
      { HANDLE eh;
       if (!source || !*source) source = UNILOG_DLL_NAME;
       if (/*!*storage &&*/ 0 == unilog_IsRegistered(source))
         unilog_Register(source, NULL);
       if (eh = RegisterEventSource(*storage? storage: NULL, source))
         {
          base->eh = eh;
          rc = 1;
         }
       else rc = -1;
      } break;
    case ulFILE:
    case ulFILEL:
      { int fd;
       if (0 <= (fd = sopen(storage, O_BINARY|O_RDWR|O_CREAT|
                            (max_kb? 0: O_TRUNC), SH_DENYNO,
                            S_IREAD|S_IWRITE)))
         {
          base->fd = fd;
          rc = 1;
         }
       else rc = -1;
      } break;
    case ulDEBUG:
    case ulNOTUSED:
       rc = 1;  break;
      } /* switch(mode) */
    if (rc > 0)
      {
       if (base->mode != mode)
         {
          base->mode = mode;
          switch(mode)
            {
          case ulEVLOG:   base->fd = -1;   break;
          case ulFILE:
          case ulFILEL:   base->eh = NULL; break;
          case ulDEBUG:
          case ulNOTUSED: base->fd = -1;
                          base->eh = NULL; break;
            }
         }
       base->storage[sizeof(base->storage)-1] = '\0';
       if (base->storage != fname)
         strncpy(base->storage, fname, sizeof(base->storage)-1);
       if (old_eh) DeregisterEventSource(old_eh);
       if (old_fd != -1) close(old_fd);
      }
   }
 if (rc >= 0)
   {
    if (level >= ll_FATAL)
      base->level = level <= ll_DEBUG? level: ll_DEBUG;
    if (max_kb >= -1) base->max_kb = max_kb;
    if (title && strncmp(title, base->title, sizeof(base->title)))
      {
       base->title[sizeof(base->title)-1] = '\0';
       strncpy(base->title, title, sizeof(base->title)-1);
      }
   }
 LeaveCriticalSection(&base->lk);  /******************************/

 return rc >= 0? base: NULL;
}

static void remove_np(char *buff, size_t size)
{
 while(size--)
   {
    if (' ' > (unsigned char)*buff) *buff = ' ';
    buff++;
   }
}

static size_t int_format_message(struct unilog *ul,
                                 DWORD flags, DWORD msgid,
                                 char *buff, DWORD maxsize)
{
 size_t size;
 size = FormatMessage(flags, NULL, msgid, ul->lang, buff, maxsize, NULL);
 if (0 == size && ul->lang)
   {
   /* && GetLastError() == ERROR_RESOURCE_LANG_NOT_FOUND)*/
    size = FormatMessage(flags, NULL, msgid, 0, buff, maxsize, NULL);
    if (0 != size) ul->lang = 0;
   }
 if (size > maxsize) size = maxsize;
 while(size && ' ' >= (unsigned char)buff[size-1]) size--;
 remove_np(buff, size);
 return size;
}

static const char *ul_strerror(int ecode)
{
 const char *err = 0;
 if (ecode >= 0 && ecode < sys_nerr) 
#if 0
   err = sys_errlist[ecode];
#else
   err = strerror(ecode);
#endif
 if (err == 0) err = "<Unknown errno>";
 return err;
}

#define ul_UNKNOWN_ERR(msg)  (!(msg) || !*(msg) || '<' == *(msg))

static size_t ul_snprintf(char *buf, size_t size, const char *fmt, ...)
{
 int len;
 va_list arg;
 va_start(arg, fmt);
 len = _vsnprintf(buf, size, fmt, arg);
 va_end(arg);
 return (0 > len || size <= (size_t)len) ? 0: (size_t)len;
}

void unilog_v(int level, unilog *log,
              const char *title, const char *fmt, va_list arg)
{
#if BUFSIZ > 4096
 char buff[BUFSIZ];
#else
 char buff[4096];
#endif
/* "1999/02/23 13:15:59.003 x"
    01234567891         2
              0123456789012345 */
#define MSG_TIMESTAMP (24)
#define MSG_START (MSG_TIMESTAMP /*+ 1*/)
 size_t size = MSG_START;
#define BUF_SIZE_ARG (sizeof(buff)-24-size)
#define BUF_LEN_ARG &buff[size],BUF_SIZE_ARG
 int errsv, p_errno, p_sockerr, p_pid, p_raserr;
 DWORD lasterr, p_lasterr, p_thrid, p_ssperr;
 SID *sid_owner = 0;
 SECURITY_DESCRIPTOR *secud = 0;
#define ecTHRID    (1) /* don't change!, see printing... */
#define ecPID      (2) /* don't change!, see printing... */
#define ecERRNO    (4)
#define ecLASTERR  (8)
#define ecSOCKERR (16)
#define ecSSPERR  (32)
#define ecRASERR  (64)
#define ecLEVEL  (128)
#define ecUID    (256)
 int ecodes = /*0*/ecLEVEL;
 int len;


 if (!fmt || level < ll_FATAL || level >= SIZEOF_ARRAY(log_msgmap)) return;

 errsv = errno;
 lasterr = GetLastError();

 if (!log || log != log->iam)
   {
    if (default_log.iam == &default_log) log = &default_log;
    else if (!(log = unilog_Open(NULL, NULL, -2, -1))) goto Exit;
    if (log != log->iam || log->mode == ulNOTUSED) goto Exit;
   }
 /*Level:*/
 if (log_msgmap[level].level > log->level && level != ll_FATAL) goto Exit;
 if (!title) title = log->title;
 if (log->mode == ulNOTUSED)
   {
    if (log == &default_log) goto Exit;
    if (default_log.iam == &default_log) log = &default_log;
    else if (!(log = unilog_Open(NULL, NULL, -2, -1))) goto Exit;
    if (log != log->iam || log->mode == ulNOTUSED) goto Exit;
   }

 if (title[0] == '%' && title[1] == '!')
   for(++title;;)
     switch(*(++title))
       {
     case 'P': p_pid  = GetCurrentProcessId(); ecodes |=  ecPID;   break;
     case 'T': p_thrid = GetCurrentThreadId(); ecodes |=  ecTHRID; break;
     case 'U':                                 ecodes |=  ecUID;   break;
     case 'V':                                 ecodes |=  ecLEVEL; break;
     case 'v':
     case 'Y':                                 ecodes &= ~ecLEVEL; break;
     case ' ': title++;
     default:  goto Break0;
       }
Break0:
 if (fmt[0] == '%' && fmt[1] == '!')
   for(++fmt;;)
     switch(*(++fmt))
       {
     case 'e': p_errno   = va_arg(arg, int);    ecodes |= ecERRNO;   break;
     case 'E': p_errno   = errsv;               ecodes |= ecERRNO;   break;

     case 'l': p_lasterr = va_arg(arg, DWORD);  ecodes |= ecLASTERR; break;
     case 'L': p_lasterr = lasterr;             ecodes |= ecLASTERR; break;

     case 't': p_thrid  = va_arg(arg, DWORD);   ecodes |= ecTHRID;   break;
     case 'T': p_thrid  = GetCurrentThreadId(); ecodes |= ecTHRID;   break;

     case 'p': p_pid   = va_arg(arg, int);      ecodes |= ecPID;     break;
     case 'P': p_pid   = GetCurrentProcessId(); ecodes |= ecPID;     break;

     case 's': p_sockerr = va_arg(arg, int);    ecodes |= ecSOCKERR; break;
     case 'S': p_sockerr = WSAGetLastError();   ecodes |= ecSOCKERR; break;

     case 'y': p_ssperr  = va_arg(arg, int);    ecodes |= ecSSPERR;  break;

     case 'r': p_raserr  = va_arg(arg, int);    ecodes |= ecRASERR;  break;
/*     case 'R': */

     case 'U':                                  ecodes |=  ecUID;    break;
     case 'V':                                  ecodes |=  ecLEVEL;  break;
     case 'Y':                                  ecodes &= ~ecLEVEL;  break;

     case ' ': ++fmt; goto Break;
     default:  
        if ('a' <= *fmt && *fmt <= 'z') va_arg(arg, int); /* unknown code */
        else if ('A' <= *fmt && *fmt <= 'Z') continue; 
        else { goto Break; }
       }
Break:

 if (ecodes & ecLEVEL)
   size += ul_snprintf(BUF_LEN_ARG, "%c%-2d",
           log_msgmap[level].flag, log_msgmap[level].level);
 if (ecodes & (ecPID | ecTHRID))
   {
    if (ecodes & ecLEVEL) size--; 
    if (ecodes & ecPID)
      size += ul_snprintf(BUF_LEN_ARG, 
       (ecodes & ecTHRID)? "% 03d% 04d ": "% 03d ", p_pid, p_thrid);
    else size += ul_snprintf(BUF_LEN_ARG, "% 04d ", p_thrid);
   }

#if 1
 if (ecodes & ecUID)
   {
    HMODULE hlib;
    FARPROC getsecinf;
    if (log->mode != ulEVLOG ||
        !(hlib = GetModuleHandle("advapi32.dll")) ||
        (getsecinf = GetProcAddress(hlib, "GetSecurityInfo")) ||
        ERROR_SUCCESS != ((DWORD (APIENTRY *)(HANDLE,
        SE_OBJECT_TYPE, SECURITY_INFORMATION, SID**, SID**,
        ACL**, ACL**, SECURITY_DESCRIPTOR**))getsecinf)(
        GetCurrentThread(), SE_KERNEL_OBJECT,
            OWNER_SECURITY_INFORMATION/*| GROUP_SECURITY_INFORMATION*/,
            &sid_owner, NULL/*&sid_group*/, NULL, NULL, &secud) )
      {
        DWORD lsize;
        buff[size++] = '[';
        lsize = (BUF_SIZE_ARG) / 2;
        if (GetUserName(&buff[size], &lsize) && 
            lsize < (BUF_SIZE_ARG) / 2)
          size += strlen(&buff[size]);
        buff[size++] = ']';
        buff[size++] = ' ';
      }
   }
#else
 if ((ecodes & ecUID) && ERROR_SUCCESS == 
      GetSecurityInfo(GetCurrentThread(), SE_KERNEL_OBJECT,
            OWNER_SECURITY_INFORMATION/*| GROUP_SECURITY_INFORMATION*/,
            &sid_owner, NULL/*&sid_group*/, NULL, NULL, &secud) &&
      log->mode != ulEVLOG)
   {
    len = (BUF_SIZE_ARG)/2-3;
    if (len > 5)
      {
        char *msg = buff + size;
        DWORD ulen = len, dlen = len;
        SID_NAME_USE siduse;
        *msg++ = '[';
        memset(msg, 0, 4);
        memset(msg + 1 + len, 0, 4);
        if (LookupAccountSid(NULL,  // local computer
              sid_owner, msg, &ulen,
                  msg + len + 1, &dlen, &siduse))
          {
            msg[ulen = strlen(msg)] = '\\';            
            memmove(msg + 1 + ulen, msg + 1 + len, 
                      dlen = strlen(msg + 1 + len));
            msg += 1 + ulen + dlen;
          }
       *msg++ = ']';
       *msg++ = ' ';
       size = msg - buff;
      }
   }
#endif

 if (*title)
   size += ul_snprintf(BUF_LEN_ARG, "%s ", title);

 len = _vsnprintf(BUF_LEN_ARG, fmt, arg);
 if (len > 0 && len < (int)BUF_SIZE_ARG) size += len;

 buff[size++] = '\r';
 buff[size++] = '\n';

 if (ecodes & ecERRNO)
   {
    const char *msg;
#if 1
    msg = ul_strerror(p_errno);
    if (ul_UNKNOWN_ERR(msg))
      {
       msg = sockerror(p_errno);
       if (ul_UNKNOWN_ERR(msg)) msg = "Unknown errno";
      }
#else
    msg = (p_errno < WSABASEERR? ul_strerror(p_errno)
                               : sockerror(p_errno);
    if (ul_UNKNOWN_ERR(msg)) msg = "Unknown errno";
#endif
    len = strlen(msg);
    while(len > 0 && ' ' >= (unsigned char)msg[len-1]) len--;
    size += ul_snprintf(BUF_LEN_ARG, "%.*s (%d)\r\n", len, msg, p_errno);
   }
 if (ecodes & ecLASTERR)
   {
    size += len = int_format_message(log,
                               FORMAT_MESSAGE_FROM_SYSTEM|
                               FORMAT_MESSAGE_IGNORE_INSERTS,
                               p_lasterr,  BUF_LEN_ARG);
    size += ul_snprintf(BUF_LEN_ARG, "%s(%u=0x%X)\r\n",
            len? "": "GetLastError() =", p_lasterr, p_lasterr);
   }

 if (ecodes & ecSOCKERR)
   {
    const char *msg = sockerror((int)p_sockerr);
    if (ul_UNKNOWN_ERR(msg)) 
      {
       msg = ul_strerror((int)p_sockerr);
       if (ul_UNKNOWN_ERR(msg)) msg = "<unknown WSA>";
      }
#if 1
    if (*msg == '<')
      {
       len = int_format_message(log,
                                FORMAT_MESSAGE_FROM_SYSTEM|
                                FORMAT_MESSAGE_IGNORE_INSERTS,
                                p_sockerr,  BUF_LEN_ARG);
       if (len) size += len, msg = " /WSA";
      }
#endif
    len = strlen(msg);
    while(len > 0 && ' ' >= (unsigned char)msg[len-1]) len--;
    size += ul_snprintf(BUF_LEN_ARG, "%.*s(%d=0x%X)\r\n", 
                                   len, msg, p_sockerr, p_sockerr);
   }
 if (ecodes & ecSSPERR)
   {
    const char *msg = ssperror((int)p_ssperr);
    if (ul_UNKNOWN_ERR(msg)) msg = "<unknown SSP>";
#if 1
    if (*msg == '<')
      {
       len = int_format_message(log,
                                FORMAT_MESSAGE_FROM_SYSTEM|
                                FORMAT_MESSAGE_IGNORE_INSERTS,
                                p_ssperr,  BUF_LEN_ARG);
       if (len) size += len, msg = " /SSP";
      }
#endif
    size += ul_snprintf(BUF_LEN_ARG, "%s(%u=0x%X)\r\n", msg, p_ssperr, p_ssperr);
   }
 if (ecodes & ecRASERR)
   {
    const char *msg = "<unknown RAS>";
    len = raserror(BUF_LEN_ARG, p_raserr);
    if (!len)
      {
       len = int_format_message(log,
                                FORMAT_MESSAGE_FROM_SYSTEM|
                                FORMAT_MESSAGE_IGNORE_INSERTS,
                                p_raserr,  BUF_LEN_ARG);
      }
    if (len) size += len, msg = " /RAS";
    size += ul_snprintf(BUF_LEN_ARG, "%s(%u=0x%X)\r\n", msg, p_raserr, p_raserr);
   }

 buff[sizeof(buff)-1] = '\0';
/*{
 for(len = MSG_START; len <= size; len++)
   printf(' ' > (unsigned char)buff[len]? "<%02X>": "%c", buff[len]);
 printf("\n");
}*/
 if (log->iam != log) goto Exit;
 if (log->mode == ulEVLOG)
   {
    HANDLE eh;
    const char *buf[3];
    char *msg;
    buf[0] = msg = buff + MSG_START;
    buf[1] = "";
    buf[2] = "";
    buff[size - 2] = '\0';
//    EnterCriticalSection(&log->lk);
    eh = (log->iam == log && log->mode == ulEVLOG)? log->eh: 0;
 //   LeaveCriticalSection(&log->lk);
    while(msg = strchr(msg, '%'))
      {
        msg++;
        if (*msg >= '0' && *msg <= '9') *msg++ = '?';
      }
    if (eh)
      ReportEvent(/*log->*/eh,
                log_msgmap[level].type,
                0, // event category - any value
                log_msgmap[level].msgid,
                sid_owner, // security identifier (optional)
                3, // number of strings to merge with message
                0, // size of raw (binary) data (in bytes)
                buf,   // array of strings to merge with message
                NULL); // address of binary data
   }
 else 
   {
    SYSTEMTIME ti;
    long pos_lk = -1, pos_e;

    memset(buff, ' ', MSG_START);
    GetLocalTime(&ti);
    len = ul_snprintf(buff, MSG_TIMESTAMP, "%04u/%02u/%02u %02u:%02u:%02u.%03u",
                    0x1fff & ti.wYear, 0x3f & ti.wMonth, 0x3f & ti.wDay,
                    0x3f & ti.wHour, 0x3f & ti.wMinute, 0x3f & ti.wSecond,
                    (0x3ff & ti.wMilliseconds) % 1000u);
    if ((int)len > 0) 
	  while((int)len < MSG_START) buff[len++] = ' ';
/*	else len = 0;*/
    
    remove_np(buff, size - 2);
//    buff[size++] = '\r'; buff[size++] = '\n';

    if (log->mode == ulDEBUG)
      {
       buff[size] = '\0';
       OutputDebugString(buff);
       goto Exit;
      }

    if (log->iam != log) goto Exit;

    EnterCriticalSection(&log->lk);
    if (log->iam != log) goto Leave;
    if (log->mode == ulFILEL)
      {
       pos_lk = lseek(log->fd, 1L<<16, SEEK_END);
       if (0 > pos_lk ||
           0 > _locking(log->fd, _LK_LOCK, -pos_lk)) goto Leave;
      }
    else if (log->mode != ulFILE) goto Leave;

    pos_e = lseek(log->fd, 0L, SEEK_END);
    write(log->fd, buff, size);

    if (log->max_kb > 0 &&
        ((long)log->max_kb << 10) < (pos_e += size))
      {
       char *nl_aligned = 0;
       long pos_s = 0L;
       pos_e = (long)log->max_kb << 9;

       if (log->mode == ulFILE) /* didn't locked yet */
         {
          pos_lk = lseek(log->fd, 1L<<16, SEEK_END);
          if (0 > pos_lk ||
              0 > _locking(log->fd, _LK_LOCK, -pos_lk)) goto Leave;
         }

       do {
           int wr_len;
           if (0 > lseek(log->fd, pos_e + pos_s, SEEK_SET) ||
               0 > (len = read(log->fd, buff, sizeof(buff))) ) break;
           wr_len = len;
           if (0 == nl_aligned)
             {
              if (nl_aligned = (char*)memchr(buff, '\n', len))
                {
                 nl_aligned++;
                 wr_len -= nl_aligned - buff;
                 pos_e += nl_aligned - buff;
                }
              else wr_len = 0, pos_e += len;
             }
           if (wr_len && (
               0 > lseek(log->fd, 0 + pos_s, SEEK_SET) ||
               0 > write(log->fd, buff + len - wr_len, wr_len) ) ) break; /*goto Exit;*/
           pos_s += wr_len;
          }
       while(len == sizeof(buff));
       chsize(log->fd, pos_s);
      }
    if (pos_lk >= 0 /*log->mode == ulFILEL*/)
      {
       lseek(log->fd, pos_lk, SEEK_SET);
       _locking(log->fd, _LK_UNLCK, -pos_lk);
      }
Leave:
    LeaveCriticalSection(&log->lk);
   }
Exit:
 if (secud) LocalFree(secud);
 if (ll_FATAL == log_msgmap[level].level) ExitProcess(3);
 SetLastError(lasterr);
 errno = errsv;
 return;
}

void unilog_l(int level, unilog *log, const char *title, const char *fmt, ...)
{
 va_list arg;
 va_start(arg, fmt);
 unilog_v(level, log, title, fmt, arg);
 va_end(arg);
}

#define GEN_ULOG(level)                                  \
void unilog_##level(unilog *log, const char *title, const char *fmt, ...)   \
{ va_list arg; va_start(arg, fmt);                       \
  unilog_v(ll_##level, log, title, fmt, arg);  va_end(arg); }

GEN_ULOG(FATAL)
GEN_ULOG(ERROR)
GEN_ULOG(WARNING)
GEN_ULOG(MESSAGE)
GEN_ULOG(INFO)
GEN_ULOG(NOTICE)
GEN_ULOG(TRACE)
GEN_ULOG(DEBUG)

void unilog_CloseDefault(void) { unilog_Close(&default_log); }

/* end of unilog.c */
