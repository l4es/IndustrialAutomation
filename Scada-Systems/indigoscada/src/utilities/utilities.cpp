/*
 *                         IndigoSCADA
 *
 *   This software and documentation are Copyright 2002 to 2009 Enscada 
 *   Limited and its licensees. All rights reserved. See file:
 *
 *                     $HOME/LICENSE 
 *
 *   for full copyright notice and license terms. 
 *
 */

#include "utilities.h"
#ifdef WIN32
#include <windows.h>
#include "isnan.h"
#include "fcmp.h"
#include "const.h"

bool IsSingleInstance (const char* Name)
{

   HANDLE hMutex = CreateMutex (NULL, TRUE, Name);
   if (GetLastError() == ERROR_ALREADY_EXISTS)
   {
	  CloseHandle(hMutex);
	  return false;
   }
   return true;
}

const char* GetScadaDateBuild()
{
   return __DATE__;
}

const char* GetScadaTimeBuild()
{
   return __TIME__;
}

int IsNAN_Double(double dbl)
{
	return ISNAND(dbl);
}

int IsNAN_Float(float flt)
{
	return ISNANF(flt);
}

int flp_cmp(double x1, double x2)
{
	return fcmp(x1, x2, MACHEP);
}


#endif