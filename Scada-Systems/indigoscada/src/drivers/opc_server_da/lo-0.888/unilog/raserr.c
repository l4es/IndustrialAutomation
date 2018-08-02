/******************  Universal EventLogging utility  ****************
 **                     SSP errors translation                     **
 *                                                                  *
 *                         Copyright (c) 2002  Timofei Bondarenko   *
 ********************************************************************/
 
#include <windows.h>
//#include <ras.h>
#include "unilog.h"


unsigned raserror(char *buf, unsigned size, int ecode)
{
 HMODULE hm;
 unsigned rv = 0;
 hm = LoadLibrary("rasapi32.dll");
 if (hm)
   {
    FARPROC fp;
    fp = GetProcAddress(hm, "RasGetErrorStringA");
    if (fp)
      {
       rv = ((DWORD (APIENTRY *)(UINT, LPSTR, DWORD))fp)(ecode, buf, size);
       rv = rv? 0: strlen(buf);
      }
    FreeLibrary(hm);
   }
 return rv;
}

/* end of raserr.c */
