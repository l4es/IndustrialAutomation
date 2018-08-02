/******************  Universal EventLogging utility  ****************
 **                  Advancrd initialization method                **
 *                                                                  *
 *                    Copyright (c) 1996,1999 by Timofei Bondarenko *
 ********************************************************************/
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
//#include <winnt.h>
#include "unilog.h"

#if 0
static char hoheho[64] = "\n*    ";
#define DBG_OUT (sprintf(hoheho + 3, "%d **", __LINE__), OutputDebugString(hoheho))
#endif
#if 0
#include <stdarg.h>
static void DBGBOX(const char *fmt, ...)
  {
    char buf[512];
    va_list ap;
    va_start(ap, fmt);
    wvsprintf(buf, fmt, ap);
    va_end(ap);
    MessageBox(NULL, buf, "unilog", MB_OK);
  }
#endif 

#define MAX_ADV (32)
#define ENTRYNAME_MAX (32)

typedef struct LogEntry
   {
    unilog *log;
    long used;
    char name[ENTRYNAME_MAX];
    char redir[ENTRYNAME_MAX];
   } LogEntry;

static LogEntry entries[MAX_ADV];

static CRITICAL_SECTION adv_lk;
static LONG adv_lk_init;
static const char levenum[8][8] = {
   "FATAL", "ERROR",  "WARNING", "MESSAGE",
   "INFO",  "NOTICE", "TRACE",   "DEBUG" };
#if 0
#define CHECK_LEVEL(level) \
       if (level > ll_DEBUG) level = ll_DEBUG;  \
       else if (level < ll_FATAL) level = ll_FATAL;
#else
#define CHECK_LEVEL(level) ;
#endif

void unilog_advinit(void)
{
 if (0 <= adv_lk_init &&
     0 == InterlockedExchange(&adv_lk_init, 1))
   InitializeCriticalSection(&adv_lk);
}

void unilog_advdestroy(void)
{
 if (0 < InterlockedExchange(&adv_lk_init, -1))
   {
    int ii;
    LogEntry *ent;
    EnterCriticalSection(&adv_lk);
    for(ii = SIZEOF_ARRAY(entries), ent = entries; ii--; ent++)
      if (ent->log)
        {
         unilog *log;
         ent->name[0] = '\0';
         ent->redir[0] = '\0';
         ent->used = 0;
         log = ent->log; ent->log = 0;
         unilog_Close(log);
        }
    DeleteCriticalSection(&adv_lk);
   }
}

void unilog_Delete(unilog *log)
{
 unsigned klen;
 LogEntry *ent;

 if (!log || 0 > adv_lk_init) return;
 unilog_advinit();

 EnterCriticalSection(&adv_lk);

 for(klen = SIZEOF_ARRAY(entries), ent = entries; klen--; ent++)
   if (log == ent->log)
     {
      if (0 == InterlockedDecrement(&ent->used))
        {
         ent->log = 0;
         if (!ent->redir[0]) ent->name[0] = '\0';
         //ent->redir[0] = '\0';
        }
      else log = 0;
      break;
     }
 LeaveCriticalSection(&adv_lk);
 if (log) unilog_Close(log);
}

static int reg_query(HKEY hkey, const char *name, unsigned rtype,
                     void *buf, unsigned size)
{
 char *tbuf, ibuf[FILENAME_MAX + 16];
 DWORD tlen, tlen0, type;
 LONG rv;

 tbuf = buf; tlen = tlen0 = size;
 if (REG_EXPAND_SZ == rtype ||
     REG_DWORD == rtype && sizeof(ibuf) > size)
   {
    tbuf = ibuf; tlen = tlen0 = sizeof(ibuf);
   }

 rv = RegQueryValueEx(hkey, name, 0, &type, (LPBYTE)tbuf, &tlen);
 if (ERROR_SUCCESS != rv)
   return ERROR_MORE_DATA == rv ? (0 < (long)tlen? tlen: 1): -1;

 if (REG_EXPAND_SZ == type)
   {
    char *ebuf = ibuf;
    DWORD elen = sizeof(ibuf), el;
/* make sure ebuf and tbuf pointing not to the same space */
    if (tbuf == ibuf) ebuf = (char*)buf, elen = size;
    tlen++;
    if (tlen0 > tlen && tlen0 - tlen > elen)
         ebuf = tbuf + tlen, elen = tlen0 - tlen;
    if (0 != (el = ExpandEnvironmentStrings(tbuf, ebuf, elen)) &&
        elen >= el) tbuf = ebuf, tlen = elen;
   }

 if (REG_DWORD == rtype)
   {
    if (rtype != type)
      {
       char *end;
       if (REG_SZ != type && REG_EXPAND_SZ != type || !*tbuf) return 1;
       tlen = strtoul(tbuf, &end, 0);
     /*  if (tlen == ~0) bogus MSVCRT.dll returns 0... */
       if (end == tbuf)
         {
          unsigned ii;
//          while(*end && ' ' >= (unsigned char)*end) end++;
          for(ii = 0; ii < SIZEOF_ARRAY(levenum); ii++)
            if (!stricmp(levenum[ii], end)) { tlen = ii; break; }
         }
       tbuf = (char*)&tlen;
      }
    if (sizeof(DWORD) < size) size = sizeof(DWORD);
    memmove(buf, tbuf, size);
   }
 else if (rtype != type &&
         (rtype != REG_SZ || type != REG_EXPAND_SZ)) return 1;
 else if (buf != tbuf && size)
   {
    if (size > tlen) size = tlen;
    else tbuf[size - 1] = 0;
    memmove(buf, tbuf, size);
   }
 
 return 0;
}

unilog *unilog_Create(const char *entryname,
   /* name of the description in the UNILOG_PATH.
     Following parameters are defaults for notexisting entries: */
                      const char *fname, /* destination name in form:
                            NULL - don't change
                            ServerName|EventSource
                            |EventSource
                            FileName
                            FileName*title
                            FileName?      - use file locking
                            FileName?title - use file locking */
                       const char *title, /* optional string */
                       int max_kb, /* Max filesize: -1 unlimited,
                                                 -2 -don't change */
                       int level) /* level [ll_FATAL...ll_DEBUG] */
{
 char ltitle[32];
 char key[FILENAME_MAX+16];
 unsigned klen;
 unilog *log = 0;
 LogEntry *ent, *free_ent = 0;
 HKEY hkey;

 if (!entryname || !*entryname || 0 > adv_lk_init) return 0;
 unilog_advinit();

 EnterCriticalSection(&adv_lk);

 for(klen = SIZEOF_ARRAY(entries), ent = entries; klen--; ent++)
   if (!strnicmp(ent->name, entryname, ENTRYNAME_MAX))
     {
      if (ent->log && ent->used) /* already in use */
        {
         InterlockedIncrement(&ent->used);
         log = ent->log;
         goto Unlock;
        }
      else if (*ent->redir) /* is redirected before opening */
        {
         entryname = ent->redir;
         free_ent = ent; break;
         //goto Reload;
        }
     }
   else if (!*ent->name && !ent->used && !free_ent) free_ent = ent;
// ent = free_ent;
//Reload:
 klen = strlen(strcpy(key, UNILOG_PATH));
 key[klen++] = '\\';
 strncpy(key + klen, entryname, ENTRYNAME_MAX);
 key[klen + ENTRYNAME_MAX] = '\0';

 if (RegCreateKey(HKEY_LOCAL_MACHINE, key, &hkey) == ERROR_SUCCESS)
   {
    DWORD val;
    if (!reg_query(hkey, "size", REG_DWORD, &val, sizeof(val)))
      {
       max_kb = val;
      }
    else
      {
       sprintf(ltitle, "%d", max_kb);
       RegSetValueEx(hkey, "size", 0, REG_SZ,
                     (CONST BYTE*)ltitle, strlen(ltitle) + 1);
      }

    if (!reg_query(hkey, "level", REG_DWORD, &val, sizeof(val)))
      {
       level = val;
       CHECK_LEVEL(level);
      }
    else
      {
#if 0
       sprintf(ltitle, "0x%x", level);
       RegSetValueEx(hkey, "level", 0, REG_SZ,
                     (CONST BYTE*)ltitle, strlen(ltitle) + 1);
#else
# if 1
      if (level <= ll_DEBUG && level >= ll_FATAL)
         RegSetValueEx(hkey, "level", 0, REG_SZ,
                      (CONST BYTE*)levenum[level], strlen(levenum[level])+1);
      else
# endif
        {
         val = level;
         RegSetValueEx(hkey, "level", 0, REG_DWORD,
                       (CONST BYTE*)&val, sizeof(val));
         CHECK_LEVEL(level);
        }
#endif
      }

    if (!reg_query(hkey, 0, REG_SZ, ltitle, sizeof(ltitle)) && *ltitle)
      {
        title = ltitle;
        while(*title && ' ' >= (unsigned char)*title) title++;
      }
    else if (title)
      {
        RegSetValueEx(hkey, 0, 0, REG_SZ,
                     (CONST BYTE*)title, strlen(title) + 1);
      }

    if (!reg_query(hkey, "path", REG_SZ, key, sizeof(key)) /*&& *key*/)
      fname = key;
    else
      {
       if (!fname) fname = "";
       RegSetValueEx(hkey, "path", 0, REG_EXPAND_SZ,
                     (CONST BYTE*)fname, strlen(fname) + 1);
      }

    RegCloseKey(hkey);
   }
 
 if (fname != key && 
    (klen = ExpandEnvironmentStrings(fname, key, sizeof(key))) &&
     klen <= sizeof(key)) fname = key;

 if (log = unilog_Open(fname, title, max_kb, level))
   {
    if (free_ent)
      {
       if (!*free_ent->name)
         {
          strncpy(free_ent->name, entryname, ENTRYNAME_MAX);
          /*free_ent->name[ENTRYNAME_MAX] = '\0';*/
         }
       free_ent->log = log;
       InterlockedIncrement(&free_ent->used);
      }
   }

Unlock:
 LeaveCriticalSection(&adv_lk);
 return log;
}

static unilog *log_refresh(LogEntry *ent, const char *entryname)
{
 unsigned klen;
 HKEY hkey;
 char key[FILENAME_MAX+16];
 char ltitle[32];
 int max_kb, level;

 klen = strlen(strcpy(key, UNILOG_PATH));
 key[klen++] = '\\';
 if (!entryname) entryname = *ent->redir? ent->redir: ent->name;
 strncpy(key + klen, entryname, ENTRYNAME_MAX);
 key[klen + ENTRYNAME_MAX] = '\0';

 klen = 0;
 if (ent->log && RegOpenKey(HKEY_LOCAL_MACHINE, key, &hkey) == ERROR_SUCCESS)
   {
    DWORD val;
    if (reg_query(hkey, "size", REG_DWORD, &val, sizeof(val))) goto Close;
    max_kb = val;

    if (reg_query(hkey, "level", REG_DWORD, &val, sizeof(val))) goto Close;
    level = val;
    CHECK_LEVEL(level);

    if (reg_query(hkey, 0, REG_SZ, ltitle, sizeof(ltitle))) goto Close;

    if (reg_query(hkey, "path", REG_SZ, key, sizeof(key)) /*|| !*key*/) goto Close;

    klen = 1;
Close:
    RegCloseKey(hkey);
   }

 return klen? unilog_ReOpen(ent->log, key, ltitle, max_kb, level): 0;
}

unilog *unilog_Refresh(const char *entryname)
{
 unsigned klen;
 unilog *log = 0;
 LogEntry *ent;

 if (entryname && !*entryname) entryname = 0;
 if (0 > adv_lk_init) return 0;
 unilog_advinit();

 EnterCriticalSection(&adv_lk);

 for(klen = SIZEOF_ARRAY(entries), ent = entries; klen--; ent++)
   if (ent->used)
     {
      int cmp;
      if (!entryname) cmp = 1;
      else if (cmp = !strnicmp(ent->name, entryname, ENTRYNAME_MAX))
        log = ent->log;
      if (cmp || !strnicmp(ent->redir, entryname, ENTRYNAME_MAX))
        log_refresh(ent, 0);
     }
 LeaveCriticalSection(&adv_lk);

// if (klen) unilog_ReOpen(log, key, ltitle, max_kb, level);
 return log;
}

void unilog_Redirect(const char *dst, const char *src, int glob)
{
 int found = 0;
 LogEntry *free_ent = 0, *ent;
 unsigned klen;

 if (!dst) dst = "";
 if (src && !*src) src = 0;

 if (0 > adv_lk_init) return;
 unilog_advinit();

 EnterCriticalSection(&adv_lk);

 for(klen = SIZEOF_ARRAY(entries), ent = entries; klen--; ent++)
   if (ent->name[0])
     {
      if (!src ||
          !strnicmp(ent->name, src, ENTRYNAME_MAX) && glob >= 0 ||
          !strnicmp(ent->redir, src, ENTRYNAME_MAX) && glob <= 0)
        {
         if (strnicmp(ent->redir, dst, ENTRYNAME_MAX))
           {
            if (!strnicmp(ent->name, dst, ENTRYNAME_MAX))
              ent->redir[0] = '\0';
            else strncpy(ent->redir, dst, ENTRYNAME_MAX);
            if (ent->used) log_refresh(ent, 0);
            else if (!*dst) ent->name[0] = '\0';
           }
         found++;
        }
     }
   else
     {
      if (!free_ent && !ent->used) free_ent = ent;
     }

 if (!found && src && *dst && free_ent && strnicmp(src, dst, ENTRYNAME_MAX))
   {
    strncpy(free_ent->name, src, ENTRYNAME_MAX);
    strncpy(free_ent->redir, dst, ENTRYNAME_MAX);
   }

 LeaveCriticalSection(&adv_lk);
}

/* end of advini.c */
