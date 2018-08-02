/******************  Universal EventLogging utility  ****************
 **                         read/write locks                       **
 *                                                                  *
 *                            Copyright (c) 1996 by Kostya Volovich *
 ********************************************************************/
#include <windows.h>
#include <winnt.h>
#include "unilog.h"

HMODULE unilog_dll_module = 0;

static const char regpath[] =
   "SYSTEM\\CurrentControlSet\\Services\\EventLog\\Application\\";

int unilog_IsRegistered(const char* ServiceName)
{
    char szKey[256];
    HKEY hKey = NULL;
    strcpy(szKey, regpath);
    strcat(szKey, ServiceName);
    if (RegOpenKey(HKEY_LOCAL_MACHINE, szKey, &hKey) != ERROR_SUCCESS) {
        return 0;
    }
	RegCloseKey (hKey);
   return 1;
}

int unilog_Register(const char* ServiceName, const char* FilePath)
{
    char szKey[256];
    char szFile[260];
    HKEY hKey = NULL;
    DWORD dwData = EVENTLOG_ERROR_TYPE |
                   EVENTLOG_WARNING_TYPE |
                   EVENTLOG_INFORMATION_TYPE;
    strcpy(szKey, regpath);
    strcat(szKey, ServiceName);
    if (!FilePath)
      {
#if defined(UNILOG_DLL_NAME) && (!defined(UNILOG_STATIC) || !UNILOG_STATIC) /* dll */
       HMODULE hm = unilog_dll_module;
       if (!hm) hm = GetModuleHandle(UNILOG_DLL_NAME);
       FilePath = (hm && GetModuleFileName(hm,
                         szFile, sizeof(szFile)-1))?
                         szFile: UNILOG_DLL_NAME;
#else /* static */
       if (!GetModuleFileName(unilog_dll_module, szFile, sizeof(szFile)-1))
         return -1;
       else FilePath = szFile;
#endif
      }

    if (RegCreateKey(HKEY_LOCAL_MACHINE, szKey, &hKey) != ERROR_SUCCESS)
      return -1;

    RegSetValueEx(hKey,
                    "EventMessageFile",
                    0,
                    REG_EXPAND_SZ,
                    (CONST BYTE*)FilePath,
                    strlen(FilePath) + 1);


    RegSetValueEx(hKey,
                    "TypesSupported",
                    0,
                    REG_DWORD,
                    (CONST BYTE*)&dwData,
                     sizeof(DWORD));
    RegCloseKey(hKey);
   return 0;
}

int unilog_UnRegister(const char* ServiceName)
{
    char szKey[256];
/*
   if (!unilog_IsRegistered(ServiceName))
      return 0;
*/
    strcpy(szKey, regpath);
    strcat(szKey, ServiceName);
    if (RegDeleteKey(HKEY_LOCAL_MACHINE, szKey) != ERROR_SUCCESS)
        return -1;

   return 0;
}

/* end of reglog.c */
