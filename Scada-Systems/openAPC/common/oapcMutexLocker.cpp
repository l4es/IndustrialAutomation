/********************************************************************************************

These sources are distributed in the hope that they will be useful, but WITHOUT ANY WARRANTY;
without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. All
the information given here, within the interface descriptions and within the specification
are subject to change without notice. Errors and omissions excepted.

These sources can be used freely according to the OpenAPC Dual License: As long as the
sources and the resulting applications/libraries/Plug-Ins are used together with the OpenAPC
software, they are licensed as freeware. When you use them outside the OpenAPC software they
are licensed under the terms of the GNU General Public License.

For more information please refer to http://www.openapc.com/oapc_license.php

*********************************************************************************************/

#include "liboapc.h"
#include "oapcMutexLocker.h"

#include <stdio.h>
#ifdef _DEBUG
 #include <string.h>
// #define ENABLE_MUTEX_LOG
#endif

oapcMutexLocker::oapcMutexLocker(void *mutex,const char *file,int line):
    m_mutex(mutex)
{
#ifdef ENABLE_MUTEX_LOG
#ifndef NDEBUG
//   printf("Lock Mutex   @ %s %d\n",file,line);
   strcpy(m_file,file);
   m_line=line;
#ifdef ENV_WINDOWS
   FILE *FHandle=fopen("C:\\tmp\\mutex.lock","a");
   if (FHandle)
   {
      fprintf(FHandle,"Lock Mutex   @ %s %d\n",file,line);
      fclose(FHandle);
   }
#endif
#endif
#endif //ENABLE_MUTEX_LOG

   if (mutex)
    oapc_thread_mutex_lock(mutex);

#ifdef ENABLE_MUTEX_LOG
#ifndef NDEBUG
   printf("Locked\n");
#ifdef ENV_WINDOWS
   FHandle=fopen("C:\\tmp\\mutex.lock","a");
   if (FHandle)
   {
       fprintf(FHandle,"Locked\n");
       fclose(FHandle);
   }
#endif
#endif
#endif //ENABLE_MUTEX_LOG
   file=file;
   line=line;
}



oapcMutexLocker::~oapcMutexLocker()
{
   if (m_mutex)
    oapc_thread_mutex_unlock((void*)m_mutex);

#ifdef ENABLE_MUTEX_LOG
#ifndef NDEBUG
   printf("UnLock Mutex\n");
#ifdef ENV_WINDOWS
   FILE *FHandle=fopen("C:\\tmp\\mutex.lock","a");
   if (FHandle)
   {
       fprintf(FHandle,"UnLock Mutex @ %s %d\n",m_file,m_line);
       fclose(FHandle);
   }
#endif
#endif
#endif //ENABLE_MUTEX_LOG
}
