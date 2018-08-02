/******************************************************************************

This file is part of ControlRoom process control/HMI software.

ControlRoom is free software: you can redistribute it and/or modify it under
the terms of the GNU General Public License as published by the Free
Software Foundation, either version 3 of the License, or (at your option) any
later version.

ControlRoom is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more
details.

You should have received a copy of the GNU General Public License along with
ControlRoom. If not, see http://www.gnu.org/licenses/

*******************************************************************************

For different licensing and/or usage of the sources apart from GPL or any other
open source license, please contact us at https://openapc.com/contact.php

*******************************************************************************/

#include <wx/wx.h>
#include <wx/app.h>
#include <wx/filename.h>

#include "oapcMutexLocker.h"
#include "flowWalkerPool.h"
#include "globals.h"
#include "FlowDispatcherThread.h"


FlowDispatcherThread::FlowDispatcherThread()
                     :wxThread()
{
   std::list<int>::iterator it;

   m_signal=oapc_thread_signal_create();
   m_listMutex=oapc_thread_mutex_create();
   if (g_objectList.m_projectData->flags & PROJECT_FLAG_ISPACE_MASK)
   {
      if (((g_objectList.m_projectData->flags & PROJECT_FLAG_ISPACE_MASK)==PROJECT_FLAG_ISPACE_SINGLE_REMOTE) ||
          ((g_objectList.m_projectData->flags & PROJECT_FLAG_ISPACE_MASK)==PROJECT_FLAG_ISPACE_DOUBLE_REMOTE))
      {
         g_flowPool.openISConnection(false); // do not try to start the server locally in case it is located somewhere else
      }
      else
      {
         g_flowPool.openISConnection(true);
         if (!g_pluggerIDList.empty())
          for ( it=g_pluggerIDList.begin() ; it!=g_pluggerIDList.end(); it++)
         {
#ifdef ENV_WINDOWS
             wxExecute(wxGetCwd()+wxFileName::GetPathSeparator()+wxString::Format(_T("OpenPlugger.exe -p %s -i %d -w %s"),g_projectPath,*it,wxGetCwd()));
#else
            char *pcall;
            char  path[300+4],cwd[250+4];

            oapc_unicode_stringToCharUTF8(g_projectPath,path,250);
            oapc_unicode_stringToCharUTF8(wxGetCwd(),cwd,300);
            (void)asprintf(&pcall,"OpenPlugger -p %s -i %d -w %s &",path,*it,cwd);
            if (pcall)
            {
               system(pcall);
               free(pcall);
            }
#endif
         }
      }
   }
}



FlowDispatcherThread::~FlowDispatcherThread()
{
   oapc_thread_signal_release(m_signal);
   oapc_thread_mutex_release(m_listMutex);
}



#ifdef ENV_DEBUGGER
wxInt32 FlowDispatcherThread::getThreadNum()
{
   static wxInt32         numPrev=1;
          wxInt32         num,numRet;
          oapcMutexLocker locker(m_listMutex,__FILE__,__LINE__);

   num=m_threadList.size();

   // a small hack to keep the thread number in debugger display a bit higher for some time (it is only for the user, so this inaccuracy is ok)
   if (num<numPrev) numRet=numPrev;
   else numRet=num;
   numPrev=num;
   return numRet;
}
#endif



void FlowDispatcherThread::appendNewThread(FlowWalkerThread *newThread)
{
   oapcMutexLocker locker(m_listMutex,__FILE__,__LINE__);
   m_threadAppendList.push_back(newThread);
   oapc_thread_signal_send(m_signal);
}



void *FlowDispatcherThread::Entry()
{
   wxLongLong sTime,eTime;

   std::list<FlowWalkerThread*>::iterator it,eraseIt;
   bool                                   eraseItValid;
#ifdef ENV_DEBUGGER
   bool                                   checkExecTime;
   flowObject                            *lastObject;
#endif

   while (!g_breakThreads)
   {
#ifdef ENV_DEBUGGER
      if (g_threadsDisabled)
      {
         FlowWalkerThread *thread;

         oapc_thread_sleep(100);
         for (it=m_threadList.begin(); it!=m_threadList.end(); it++)
         {
            thread=(*it);
            thread->creationTime=wxGetLocalTimeMillis();
         }
      }
      else
#endif
      oapc_thread_sleep(0);
      if (!m_threadAppendList.empty())
      {
         oapcMutexLocker locker(m_listMutex,__FILE__,__LINE__);

         it=m_threadAppendList.begin();
         while (it!=m_threadAppendList.end())
         {
            m_threadList.push_back(*it);
            it++;
         }
         m_threadAppendList.clear();
      }
      if (!m_threadList.empty())
      {
         eraseItValid=false;
         it=m_threadList.begin();
         while (it!=m_threadList.end())
         {
/*            if (eraseIt!=(std::list<FlowWalkerThread*>::iterator)NULL)
            {
               delete (*eraseIt);
               m_threadList.erase(eraseIt); 
               eraseIt=(std::list<FlowWalkerThread*>::iterator)NULL;
               m_threadNum--;
            }*/
#ifdef ENV_DEBUGGER
            sTime=wxGetLocalTimeMillis();
            lastObject=(*it)->object;
            if (((*it)->object->data.type & FLOW_TYPE_MASK)==FLOW_TYPE_EXTERNAL_LIB)
             checkExecTime=true;
            else checkExecTime=false;
#endif
#ifndef ENV_HPLAYER
//#ifndef ENV_WINDOWS
            wxMutexGuiEnter();
//#endif
#endif
// m�rps         
			if ((*it)->execute()==0)
            {
               eraseIt=it;
               eraseItValid=true;
            }
#ifndef ENV_HPLAYER
//#ifndef ENV_WINDOWS
            wxMutexGuiLeave();
//#endif
#endif
#ifdef ENV_DEBUGGER
            if (checkExecTime)
            {
               eTime=wxGetLocalTimeMillis();
               if (eTime-sTime>MAX_RUNTIME_MILLIS)
               {
                 if (g_debugWin)
                  g_debugWin->setDebugInformation(lastObject,DEBUG_ERROR_EXECUTION_LIMIT,_T(""));
               }
            }
#endif
            if (((*it)->connection) && ((*it)->connection->isTempConnection))
            {
            	delete (*it)->connection;
            }
            if (eraseItValid)
            {
               delete (*eraseIt);
               it=m_threadList.erase(eraseIt);
               eraseItValid=false;
            }
            else it++;
         }
      }
      else
      {
         oapc_thread_signal_wait(m_signal,10);
      }
      g_allowInputPolling=true;
   }
   m_threadList.clear();

   if (g_objectList.m_projectData->flags & PROJECT_FLAG_ISPACE_MASK)
   {
   	  if (g_ispaceHandle) 
   	  {
         struct oapc_digi_value_block digiValue;
   	
      	 digiValue.digiValue=1;
   	     oapc_ispace_set_value(g_ispaceHandle,"/system/exit",OAPC_DIGI_IO0,&digiValue,&g_auth); // change the value again
      	 oapc_ispace_disconnect(g_ispaceHandle);
   	  }
   	  g_ispaceHandle=NULL;
   }

   printf("FlowDispatcherThread::Entry() exiting!\n");

#ifndef _DEBUG
   exit(0);
#endif

   g_flowPool.exitApplication();
#ifndef ENV_WINDOWS
   wxMutexGuiEnter();
#endif
#ifdef ENV_DEBUGGER
   g_debugWin->Close(true);
   g_debugWin=NULL;
#else
//   g_mainWin->Close(true);
   delete g_mainWin;
#endif
#ifndef ENV_WINDOWS
   wxMutexGuiLeave();
#endif
#ifdef ENV_PLAYER
#ifndef ENV_HPLAYER
//   wxGetApp().ExitMainLoop();
#endif
#endif
   
   return 0;
}
