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
#include <wx/thread.h>
#include <wx/filename.h>

#include <stdlib.h>

#include "oapc_libio.h"
#include "globals.h"
#include "flowObject.h"
#include "flowExternalIOLib.h"
#include "flowWalkerPool.h"
#include "flowWalkerThread.h"
#include "flowConnection.h"
#include "flowGroup.h"
#include "flowLog.h"
#include "flowISConnect.h"
#include "IOElementDefinition.h"
#include "oapcMutexLocker.h"
#include "globals.h"
#include "hmiObject.h"
#ifndef ENV_HPLAYER
 #include "hmiExternalIOLib.h"
#endif
#ifdef ENV_DEBUGGER
 #include "WatchWin.h"
#endif

FlowWalkerPool::FlowWalkerPool()
{
   currFlowThreadID=0;
   outputPollTimer=false;
   idSection=oapc_thread_mutex_create();
}



FlowWalkerPool::~FlowWalkerPool()
{
   oapc_thread_mutex_release(idSection);
}



#ifndef ENV_PLUGGER
void FlowWalkerPool::exitApplication()
{
   g_breakThreads=1;
#ifdef ENV_DEBUGGER
   g_threadsDisabled=0;
#endif
#ifdef ENV_EDITOR
   g_plugInPanel->newProject();
#endif
//#if defined ENV_EDITOR || ENV_DEBUGGER
#ifdef ENV_EDITOR
   g_hmiCanvas->newProject();
#else
   g_objectList.deleteAll(true);
#endif
   g_flowDispatcher=NULL;
}
#endif



#ifdef ENV_DEBUGGER
void FlowWalkerPool::setNewValue(wxInt32 outputNum,wxByte digi,flowObject *object)
{
	oapcMutexLocker lock(g_watchWinLock,__FILE__,__LINE__);
	if (!g_watchWin) return;
	g_watchWin->setNewValue(outputNum,digi,object->data.id);
}



void FlowWalkerPool::setNewValue(wxInt32 outputNum,wxFloat64 num,flowObject *object)
   {
	oapcMutexLocker lock(g_watchWinLock,__FILE__,__LINE__);
	if (!g_watchWin) return;
	g_watchWin->setNewValue(outputNum,num,object->data.id);
}



void FlowWalkerPool::setNewValue(wxInt32 outputNum,wxString txt,flowObject *object)
{
	oapcMutexLocker lock(g_watchWinLock,__FILE__,__LINE__);
	if (!g_watchWin) return;
	g_watchWin->setNewValue(outputNum,txt,object->data.id);
}



void FlowWalkerPool::setNewValue(wxInt32 outputNum,oapcBinHeadSp &bin,flowObject *object)
{
	oapcMutexLocker lock(g_watchWinLock,__FILE__,__LINE__);
	if (!g_watchWin) return;
	g_watchWin->setNewValue(outputNum,bin,object->data.id);
}
#endif



/**
 * Starts to check object(s) for input data; this method is called out of a timer thread periodically
 * to poll plug-ins
 * @return true in case there are plug-ins that have to be polled, false in case no timer and no
 *         polling is necessary
 */
bool FlowWalkerPool::checkObjectsForInput()
{
   bool hasPollableObjects=false;

#ifdef ENV_DEBUGGER
   wxNode *node;

   if (g_threadsDisabled)
   {
      wxMilliSleep(10); //???
      
      node=g_objectList.getObject((wxNode*)NULL);
      while (node)
      {
          ((flowObject*)node->GetData())->lastPollTime+=11;// give some additional time to egalize timing inaccuracies
          node=g_objectList.getObject(node);
      }
   }
   else
#endif
   if (g_allowInputPolling)
   {
#ifndef ENV_PLUGGER
      hasPollableObjects=checkObjectsForInput(&g_objectList,wxGetLocalTimeMillis());
#else
      hasPollableObjects=checkObjectForInput(g_object,g_object->data.stdOUT,wxGetLocalTimeMillis(),false);
#endif
      g_allowInputPolling=false; // is reset by thread loop as soon as all connections are touched once; this avoids overflows
                                 // where data from external devices are fetched faster than then connection thread is able to
                                 // consume them
      return hasPollableObjects;
   }
   return true;
}


static void on_timer_flowWalkerPool(void *data,int /*timerID*/)
{
   FlowWalkerPool *object;

   object=(FlowWalkerPool*)data;
   if (object->checkObjectsForInput())
    object->outputPollTimer=oapc_thread_timer_start(on_timer_flowWalkerPool,g_objectList.m_projectData->timerResolution,object,1703);
}



void FlowWalkerPool::startTimer()
{
   if (outputPollTimer)
   {
      wxASSERT(0);
   }
   outputPollTimer=oapc_thread_timer_start(on_timer_flowWalkerPool,g_objectList.m_projectData->timerResolution,this,1703);
}




void FlowWalkerPool_oapc_io_callback(unsigned long outputs,unsigned long callbackID)
{
   flowObject *object;

#ifndef ENV_PLUGGER
   object=g_objectList.getObject(callbackID);
   if (object)
   {
//      object->lastPollTime=0;
      g_flowPool.checkObjectForInput(object,outputs,wxGetLocalTimeMillis(),true);
   }
#else
   g_flowPool.checkObjectForInput(g_object,outputs,wxGetLocalTimeMillis(),true);
#endif
}



#ifndef ENV_PLUGGER
bool FlowWalkerPool::checkObjectsForInput(ObjectList *olist,wxLongLong currentTime)
{
   wxNode *node;
   bool    hasPollableObjects=false; 

   if (!olist) return true;
   node=olist->getObject((wxNode*)NULL);
   while (node)
   {
      flowObject    *object;

      object=(flowObject*)node->GetData();
      wxASSERT(object);
      if (object)
      {
         if (object->data.type==hmiObject::FLOW_TYPE_GROUP) // TODO: remove this statement when using ACPX files only, it eats a nameable amount of CPU time
          hasPollableObjects=checkObjectsForInput(((flowGroup*)object)->getObjectList(),currentTime);
         else
         {
            if (object->data.type==FLOW_TYPE_EXTERNAL_LIB)
            {
               if ((((flowExternalIOLib*)object)->lib->getLibCapabilities() & OAPC_ACCEPTS_IO_CALLBACK)==0)
               {
                  if (checkObjectForInput(object,object->data.stdOUT,currentTime,false))
                   hasPollableObjects=true;
               }
            }
#ifndef ENV_HPLAYER
            else if (object->data.type==HMI_TYPE_EXTERNAL_LIB)
            {
               if ((((hmiExternalIOLib*)object)->lib->getLibCapabilities() & OAPC_ACCEPTS_IO_CALLBACK)==0)
               {
                  if (checkObjectForInput(object,object->data.stdOUT,currentTime,false))
                   hasPollableObjects=true;
               }
            }
#endif
         }
      }
      node=g_objectList.getObject(node);
   }
   return hasPollableObjects;
}
#endif



static void recv_callback(void*,char *nodeName,unsigned int,unsigned int ios,void *values[MAX_NUM_IOS])
{
   flowObject  *object;
   wxInt64      bitmask=0x01010101;
   wxInt32      i;
   wxString     objectName;

   if (strncmp(nodeName+strlen(nodeName)-4,"/out",4)==0)
   {
      FlowConnection *connection;
      wxNode         *node;
      
      nodeName[strlen(nodeName)-4]=0;
      oapc_unicode_charToStringUTF8(nodeName+1,MAX_NODENAME_LENGTH-1,&objectName);
      object=g_objectList.getObject(objectName,false);
      if (object)
      {

         if (!(object->m_flowFlags & FLAG_ALLOW_MOD_FROM_ISERVER))
         {
#ifdef ENV_DEBUGGER
            if (g_debugWin) g_debugWin->setDebugInformation(object,DEBUG_INFO_DATA_FROM_IS_REJECTED,_T(""));
#endif
            return; // object doesn't allows modification from outside -> warning for debugger!
         }
         node=object->getFlowConnection((wxNode*)NULL);
         while (node)
         {
            connection=(FlowConnection*)node->GetData();
            if (connection)
            {
               bitmask=0x01010101;
               for (i=0; i<MAX_NUM_IOS; i++)
               {
                  if (values[i])
                  {
                     if (ios & bitmask & OAPC_DIGI_IO_MASK & connection->data.sourceOutput)
                     {
                        struct oapc_digi_value_block *digiValue;
                        FlowWalkerThread             *digiThread;

                        digiValue=(struct oapc_digi_value_block*)values[i];
                        digiThread=new FlowWalkerThread(object,connection,FlowWalkerThread::THREAD_MODE_DIGI,wxGetLocalTimeMillis(),0);
                        digiThread->setFirstOutValue(digiValue->digiValue,object);
                        g_flowDispatcher->appendNewThread(digiThread);
                     }
                     else if (ios & bitmask & OAPC_NUM_IO_MASK & connection->data.sourceOutput)
                     {
                        struct oapc_num_value_block *numValue;
                        wxFloat64                    d;
                        FlowWalkerThread            *numThread;

                        numValue=(struct oapc_num_value_block*)values[i];
                        d=oapc_util_block_to_dbl(numValue);
                        numThread=new FlowWalkerThread(object,connection,FlowWalkerThread::THREAD_MODE_NUM,wxGetLocalTimeMillis(),0);
                        numThread->setFirstOutValue(d,object);
                        g_flowDispatcher->appendNewThread(numThread);
                     }
                     else if (ios & bitmask & OAPC_CHAR_IO_MASK & connection->data.sourceOutput)
                     {
                        struct oapc_char_value_block *charValue;
                        FlowWalkerThread             *charThread;
                        wxString                      s;

                        charValue=(struct oapc_char_value_block*)values[i];
                        charThread=new FlowWalkerThread(object,connection,FlowWalkerThread::THREAD_MODE_CHAR,wxGetLocalTimeMillis(),0);
                        oapc_unicode_charToStringUTF8(charValue->charValue,MAX_TEXT_LEN,&s);
                        charThread->setFirstOutValue(s,object);
                        g_flowDispatcher->appendNewThread(charThread);
                     }
                     else if (ios & bitmask & OAPC_BIN_IO_MASK & connection->data.sourceOutput)
                     {
                        FlowWalkerThread *binThread;
                        oapcBinHeadSp     binValue(new oapcBinHead((struct oapc_bin_head*)values[i]));

                        if (binValue)
                        {
                           binThread=new FlowWalkerThread(object,connection,FlowWalkerThread::THREAD_MODE_BIN,wxGetLocalTimeMillis(),0);
                           binThread->setFirstOutValue(binValue,object);
                           g_flowDispatcher->appendNewThread(binThread);
                        }
                     }
                  }
                  bitmask=bitmask<<1;
               }
            }
            node=object->getFlowConnection(node);
         }
      }
      else
      {
         flowISConnect  *ISCOobject;
         wxUint64        outputFlag;

         ISCOobject=(flowISConnect*)g_objectList.getParentISCOObject(objectName);
         if (ISCOobject)
         {
            bitmask=0x01010101;
            for (i=0; i<MAX_NUM_IOS; i++)
            {
               if (values[i])
               {
                  object=ISCOobject->getTargetObjectInfo(objectName,i,&outputFlag);
                  wxASSERT(object);
                  if (!object) return;
                  node=ISCOobject->getFlowConnection((wxNode*)NULL);
                  while (node)
                  {
                     connection=(FlowConnection*)node->GetData();
                     if (connection)
                     {
                        if (connection->data.sourceOutput==outputFlag)
                        {
                           if (OAPC_DIGI_IO_MASK & connection->data.sourceOutput)
                           {
                              struct oapc_digi_value_block *digiValue;
                              FlowWalkerThread             *digiThread;

                              digiValue=(struct oapc_digi_value_block*)values[i];
                              digiThread=new FlowWalkerThread(object,connection,FlowWalkerThread::THREAD_MODE_DIGI,wxGetLocalTimeMillis(),0);
                              digiThread->setFirstOutValue(digiValue->digiValue,object);
                              g_flowDispatcher->appendNewThread(digiThread);
                           }
                           else if (OAPC_NUM_IO_MASK & connection->data.sourceOutput)
                           {
                              struct oapc_num_value_block *numValue;
                              wxFloat64                    d;
                              FlowWalkerThread            *numThread;

                              numValue=(struct oapc_num_value_block*)values[i];
                              d=oapc_util_block_to_dbl(numValue);
                              numThread=new FlowWalkerThread(object,connection,FlowWalkerThread::THREAD_MODE_NUM,wxGetLocalTimeMillis(),0);
                              numThread->setFirstOutValue(d,object);
                              g_flowDispatcher->appendNewThread(numThread);
                           }
                           else if (OAPC_CHAR_IO_MASK & connection->data.sourceOutput)
                           {
                              struct oapc_char_value_block *charValue;
                              FlowWalkerThread             *charThread;
                              wxString                      s;

                              charValue=(struct oapc_char_value_block*)values[i];
                              charThread=new FlowWalkerThread(object,connection,FlowWalkerThread::THREAD_MODE_CHAR,wxGetLocalTimeMillis(),0);
                              oapc_unicode_charToStringUTF8(charValue->charValue,MAX_TEXT_LEN,&s);
                              charThread->setFirstOutValue(s,object);
                              g_flowDispatcher->appendNewThread(charThread);
                           }
                           else if (OAPC_BIN_IO_MASK & connection->data.sourceOutput)
                           {
                              oapcBinHeadSp     binValue(new oapcBinHead((struct oapc_bin_head*)values[i]));
                              FlowWalkerThread *binThread;

                              if (binValue)
                              {
                                 binThread=new FlowWalkerThread(object,connection,FlowWalkerThread::THREAD_MODE_BIN,wxGetLocalTimeMillis(),0);
                                 binThread->setFirstOutValue(binValue,object);
                                 g_flowDispatcher->appendNewThread(binThread);
                              }
                           }
                           break;
                        }
                     }
                     node=object->getFlowConnection(node);
                  }
               }
               bitmask=bitmask<<1;
            }
         }
      }
		return;
	}
	if (strncmp(nodeName+strlen(nodeName)-3,"/in",3)==0)
	{
      nodeName[strlen(nodeName)-3]=0;
      oapc_unicode_charToStringUTF8(nodeName+1,MAX_NODENAME_LENGTH-1,&objectName);
      object=g_objectList.getObject(objectName,false);
      if (!object) return; // value changed that doesn't exist within the scope of the player
      if (!(object->m_flowFlags & FLAG_ALLOW_MOD_FROM_ISERVER))
      {
#ifdef ENV_DEBUGGER
         if (g_debugWin) g_debugWin->setDebugInformation(object,DEBUG_INFO_DATA_FROM_IS_REJECTED,_T(""));
#endif
      	return; // object doesn't allows modification from outside -> warning for debugger!
      }
      for (i=0; i<MAX_NUM_IOS; i++)
      {
      	if (values[i])
      	{
         	if (ios & bitmask & OAPC_DIGI_IO_MASK)
         	{
         		struct oapc_digi_value_block *digiValue;
               FlowWalkerThread             *digiThread;
               FlowConnection               *connection;
      		
         		connection=new FlowConnection();
         		wxASSERT(connection);
         		if (connection)
      	   	{
         	   	digiValue=(struct oapc_digi_value_block*)values[i];
            		connection->isTempConnection=true;
            		connection->data.targetInput=ios & bitmask & OAPC_DIGI_IO_MASK;
            		connection->targetInputNum=connection->getDigiIndexFromFlag(connection->data.targetInput);
                  connection->data.targetID=object->data.id;

                  digiThread=new FlowWalkerThread(object,connection,FlowWalkerThread::THREAD_MODE_DIGI,wxGetLocalTimeMillis(),0);
                  digiThread->setFirstOutValue(digiValue->digiValue,object);
                  g_flowDispatcher->appendNewThread(digiThread);
         		}
         	}
         	else if (ios & bitmask & OAPC_NUM_IO_MASK)
         	{
         		struct oapc_num_value_block *numValue;
      	   	wxFloat64                    d;
               FlowWalkerThread            *numThread;
               FlowConnection              *connection;
      		
         		connection=new FlowConnection();
      	   	wxASSERT(connection);
      		   if (connection)
         		{
            		numValue=(struct oapc_num_value_block*)values[i];
            		d=oapc_util_block_to_dbl(numValue);
            		connection->isTempConnection=true;
         	   	connection->data.targetInput=ios & bitmask & OAPC_NUM_IO_MASK;
            		connection->targetInputNum=connection->getNumIndexFromFlag(connection->data.targetInput);
                  connection->data.targetID=object->data.id;

                  numThread=new FlowWalkerThread(object,connection,FlowWalkerThread::THREAD_MODE_NUM,wxGetLocalTimeMillis(),0);
                  numThread->setFirstOutValue(d,object);
                  g_flowDispatcher->appendNewThread(numThread);
         		}
         	}
         	else if (ios & bitmask & OAPC_CHAR_IO_MASK)
      	   {
         		struct oapc_char_value_block *charValue;
               FlowWalkerThread             *charThread;
               FlowConnection               *connection;
      		
      		   connection=new FlowConnection();
         		wxASSERT(connection);
         		if (connection)
         		{
                   wxString s;

            	   charValue=(struct oapc_char_value_block*)values[i];
         		   connection->isTempConnection=true;
            	   connection->data.targetInput=ios & bitmask & OAPC_CHAR_IO_MASK;
            	   connection->targetInputNum=connection->getCharIndexFromFlag(connection->data.targetInput);
                   connection->data.targetID=object->data.id;
 
                  charThread=new FlowWalkerThread(object,connection,FlowWalkerThread::THREAD_MODE_CHAR,wxGetLocalTimeMillis(),0);
                  oapc_unicode_charToStringUTF8(charValue->charValue,MAX_TEXT_LEN,&s);
                  charThread->setFirstOutValue(s,object);
                  g_flowDispatcher->appendNewThread(charThread);
      	   	   }
         	}
         	else if (ios & bitmask & OAPC_BIN_IO_MASK)
         	{
               FlowWalkerThread            *binThread;
               FlowConnection              *connection;
            
      	   	   connection=new FlowConnection();
         	   wxASSERT(connection);
         	   if (connection)
         	   {
                  oapcBinHeadSp binValue(new oapcBinHead((struct oapc_bin_head*)values[i]));
                   
           	      if (binValue)
            	  {
                     connection->isTempConnection=true;
                     connection->data.targetInput=ios & bitmask & OAPC_BIN_IO_MASK;
                     connection->targetInputNum=connection->getBinIndexFromFlag(connection->data.targetInput);
                     connection->data.targetID=object->data.id;

                     binThread=new FlowWalkerThread(object,connection,FlowWalkerThread::THREAD_MODE_BIN,wxGetLocalTimeMillis(),0);
                     binThread->setFirstOutValue(binValue,object);
                     g_flowDispatcher->appendNewThread(binThread);
                  }
               }
            }
         }
         bitmask=bitmask<<1;
      }
   }
   else if (strncmp(nodeName,"/system/exit",MAX_NODENAME_LENGTH)==0)
   {
      struct oapc_digi_value_block *digiValue;
      
      if ((ios & OAPC_DIGI_IO0) && (values[0]))
      {
         digiValue=(struct oapc_digi_value_block*)values[0];
         if (digiValue) g_breakThreads=true;
      }
   }
   else if ((g_flowLog) && (strncmp(nodeName,"/system/pluggererr",MAX_NODENAME_LENGTH)==0))
   {
      struct oapc_char_value_block *charNameValue,*charCustomValue;
      int                           errorCode=0;

      if ((ios & OAPC_NUM_IO0) && (values[0])) errorCode=oapc_util_block_to_dbl((struct oapc_num_value_block*)values[0]);
      if ((errorCode==OAPC_ERROR_CUSTOM) && (ios & OAPC_CHAR_IO1) && (values[1]))
      {
         flowObject object(NULL);

         object.data.id=0;
         charNameValue=(struct oapc_char_value_block*)values[1];
         oapc_unicode_charToStringUTF8(charNameValue->charValue,MAX_TEXT_LEN,&object.name);
         if ((ios & OAPC_CHAR_IO2) && (values[2]))
         {
            wxString s;

            charCustomValue=(struct oapc_char_value_block*)values[2];
            oapc_unicode_charToStringUTF8(charCustomValue->charValue,MAX_TEXT_LEN,&s);
            g_flowLog->setLogInfo(s,0,&object);
         }
         else g_flowLog->setLogInfo(g_OAPCMessage(errorCode,NULL,NULL,wxEmptyString,NULL),0,&object);
      }
   }
}



bool FlowWalkerPool::sendLogMessageToIS(wxInt32 logID,wxString message)
{
   struct oapc_char_value_block charMsgValue;
   char                         nodeName[MAX_NODENAME_LENGTH+4];
   wxInt32                      i;

   if ((g_objectList.m_projectData->flags & PROJECT_FLAG_ISPACE_MASK)==0) return true;
   if (!g_ispaceHandle) openISConnection(false);
   if (!g_ispaceHandle) return false;
   if (logID==0) strncpy(nodeName,"/system/errmsg",MAX_NODENAME_LENGTH);
   else if (logID==1) strncpy(nodeName,"/system/warnmsg",MAX_NODENAME_LENGTH);
   else if (logID==2) strncpy(nodeName,"/system/infomsg",MAX_NODENAME_LENGTH);
   else if (logID==3) strncpy(nodeName,"/system/evtmsg",MAX_NODENAME_LENGTH);
   else
   {
      wxASSERT(0);
      return false;
   }
#if wxCHECK_VERSION(2,9,0)
   memcpy(charMsgValue.charValue,(const char*)message.mb_str(),MAX_TEXT_LEN);
#else
   memcpy(charMsgValue.charValue,message.c_str(),MAX_TEXT_LEN);
#endif

   for (i=0; i<2; i++)
   {
      if (oapc_ispace_set_value(g_ispaceHandle,nodeName,OAPC_CHAR_IO1,&charMsgValue,&g_auth)!=OAPC_OK)
      {
         if ((i==1) || (!openISConnection(false)))
         {
            // TODO: implement break up/error reaction
            g_ispaceHandle=NULL;
            return false;
         }
      }
      else return true;
   }
   return true;
}



bool FlowWalkerPool::tryOpenISConnection(bool executeServer,char *host)
{
   if ((oapc_ispace_set_recv_callback(g_ispaceHandle,&recv_callback)!=OAPC_OK) ||
       (oapc_ispace_connect(g_ispaceHandle,host,0,&g_auth)!=OAPC_OK))
   {
      if (executeServer)
      {
         int i;

         if ((g_objectList.m_projectData->flags & PROJECT_FLAG_ISPACE_MASK)==PROJECT_FLAG_ISPACE_SINGLE_LOCAL)
         {
            #ifdef ENV_WINDOWS
            wxExecute(wxGetCwd()+wxFileName::GetPathSeparator()+_T("OpenIServer.exe"));
            #else
            system("OpenIServer &");
            #endif
         }
         else // PROJECT_FLAG_ISPACE_DOUBLE_LOCAL
         {
            #ifdef ENV_WINDOWS
            wxExecute(wxGetCwd()+wxFileName::GetPathSeparator()+_T("OpenIServer.exe -a"));
            #else
            system("OpenIServer -a &");
            #endif
         }
         oapc_thread_sleep(750);

         for (i=0;;i++)
         {
            if ((oapc_ispace_set_recv_callback(g_ispaceHandle,&recv_callback)!=OAPC_OK) ||
                (oapc_ispace_connect(g_ispaceHandle,NULL,0,&g_auth)!=OAPC_OK))
            {
               if (i>30) // wait for max 3 sec
               {
                  if (executeServer)
#ifndef ENV_HPLAYER
                   wxMessageBox(_("Could not establish connection to Interlock Server!"),_("Error"),wxOK|wxICON_ERROR);
#else
                   printf("Error: Could not establish connection to Interlock Server!");
#endif
                  g_ispaceHandle=NULL;
                  return false;
               }
               else oapc_thread_sleep(150);
            }
            else return true;
         }
      }
   }
   return false;
}


/**
 * @param[in] executeServer defines if the server has to be started for a (re)connection or
 *            not; this is not allowed when this function is called out of a thread context
 */
bool FlowWalkerPool::openISConnection(bool executeServer)
{
   char *host;
   bool  result=true;

   if ((g_objectList.m_projectData->flags & PROJECT_FLAG_ISPACE_MASK)==0) return true;
   if (((g_objectList.m_projectData->flags & PROJECT_FLAG_ISPACE_MASK)==PROJECT_FLAG_ISPACE_SINGLE_REMOTE) ||
       ((g_objectList.m_projectData->flags & PROJECT_FLAG_ISPACE_MASK)==PROJECT_FLAG_ISPACE_DOUBLE_REMOTE))
    host=g_objectList.m_projectData->m_remSingleIP;
   else host=NULL;
   g_ispaceHandle=oapc_ispace_get_instance();
   if (g_ispaceHandle)
   {
      if (!tryOpenISConnection(executeServer,host))
      {
         if ((g_objectList.m_projectData->flags & PROJECT_FLAG_ISPACE_MASK)==PROJECT_FLAG_ISPACE_DOUBLE_REMOTE)
         {
            host=g_objectList.m_projectData->m_remDoubleIP;
            result=tryOpenISConnection(executeServer,host);
         }
         if (!result)
         {
             if (executeServer)
#ifndef ENV_HPLAYER
              wxMessageBox(_("Could not establish connection to Interlock Server!"),_("Error"),wxOK|wxICON_ERROR);
#else
              printf("Error: Could not establish connection to Interlock Server!");
#endif
            g_ispaceHandle=NULL;
            return false;
         }
      }
   }
   else
   {
      if (executeServer)
#ifndef ENV_HPLAYER
       wxMessageBox(_("Not enough resources to communicate with Interlock Server!"),_("Error"),wxOK|wxICON_ERROR);
#else
       printf("Error: Not enough resources to communicate with Interlock Server!");
#endif
      g_ispaceHandle=NULL;
      return false;
   }
   return true;
}



bool FlowWalkerPool::sendDigiValueToIS(wxUint64 targetInput,wxByte digiValue,flowObject *object,bool isOutput)
{
   if ((!g_ispaceHandle) && (object->m_flowFlags & FLAG_MAP_OUTPUT_TO_ISERVER)) openISConnection(false);
   if ((g_ispaceHandle) && (object->m_flowFlags & FLAG_MAP_OUTPUT_TO_ISERVER))
  	{
  		wxInt32 i;
  		char   *nodeName;
  		
  		if (isOutput) nodeName=object->nodeNameOut;
  		else nodeName=object->nodeNameIn;
  		for (i=0; i<2; i++)
  		{
     		// TODO: put in queue and own thread to avoid blocking of data flow
  	   	struct oapc_digi_value_block digiValueBlock;
   		
  		   digiValueBlock.digiValue=digiValue;
         wxASSERT(targetInput & OAPC_DIGI_IO_MASK);
         wxASSERT(strlen(nodeName));
     		if (oapc_ispace_set_value(g_ispaceHandle,nodeName,targetInput,&digiValueBlock,&g_auth)!=OAPC_OK)
  	   	{
  		   	if ((i==1) || (!openISConnection(false)))
  		   	{
  			      // TODO: implement break up/error reaction
               g_ispaceHandle=NULL;
               return false;
  		   	}
  	   	}
         else return true;
  		}   		
  	}
  	return true;
}



bool FlowWalkerPool::sendNumValueToIS(wxUint64 targetInput,wxFloat64 numValue,flowObject *object,bool isOutput)
{
   if ((!g_ispaceHandle) && (object->m_flowFlags & FLAG_MAP_OUTPUT_TO_ISERVER)) openISConnection(false);
   if ((g_ispaceHandle) && (object->m_flowFlags & FLAG_MAP_OUTPUT_TO_ISERVER))
  	{
  		wxInt32 i;
  		char   *nodeName;
  		
  		if (isOutput) nodeName=object->nodeNameOut;
  		else nodeName=object->nodeNameIn;
  		for (i=0; i<2; i++)
  		{
    		// TODO: put in queue and own thread to avoid blocking of data flow
   		struct oapc_num_value_block numValueBlock;
   		
         wxASSERT(targetInput & OAPC_NUM_IO_MASK);
         wxASSERT(strlen(nodeName));
  	   	oapc_util_dbl_to_block(numValue,&numValueBlock);
         if (oapc_ispace_set_value(g_ispaceHandle,nodeName,targetInput,&numValueBlock,&g_auth)!=OAPC_OK)
  		   {
  		   	if ((i==1) || (!openISConnection(false)))
  		   	{
  			      // TODO: implement break up/error reaction
               g_ispaceHandle=NULL;
               return false;
  		   	}
  		   }
         else return true;
  		}   		
  	}
  	return true;
}



bool FlowWalkerPool::sendCharValueToIS(wxUint64 targetInput,wxString charValue,flowObject *object,bool isOutput)
{
   if ((!g_ispaceHandle) && (object->m_flowFlags & FLAG_MAP_OUTPUT_TO_ISERVER)) openISConnection(false);
   if ((g_ispaceHandle) && (object->m_flowFlags & FLAG_MAP_OUTPUT_TO_ISERVER))
  	{
  		wxInt32 i;
  		char   *nodeName;
  		
  		if (isOutput) nodeName=object->nodeNameOut;
  		else nodeName=object->nodeNameIn;
  		for (i=0; i<2; i++)
  		{
     		// TODO: put in queue and own thread to avoid blocking of data flow
  	   	struct oapc_char_value_block charValueBlock;
         wxMBConvUTF8  conv;

         wxASSERT(targetInput & OAPC_CHAR_IO_MASK);
         wxASSERT(strlen(nodeName));
         memset(&charValueBlock,0,sizeof(struct oapc_char_value_block));
         conv.WC2MB(charValueBlock.charValue,charValue,MAX_TEXT_LEN);
     		if (oapc_ispace_set_value(g_ispaceHandle,nodeName,targetInput,&charValueBlock,&g_auth)!=OAPC_OK)
  	   	{
  		   	if ((i==1) || (!openISConnection(false)))
  		   	{
  			      // TODO: implement break up/error reaction
               g_ispaceHandle=NULL;
               return false;
  		   	}
  	   	}
         else return true;
  		}   		
  	}
  	return true;
}



bool FlowWalkerPool::sendBinValueToIS(wxUint64 targetInput,oapcBinHeadSp &binValue,flowObject *object,bool isOutput)
{
   if ((!g_ispaceHandle) && (object->m_flowFlags & FLAG_MAP_OUTPUT_TO_ISERVER)) openISConnection(false);
   if ((g_ispaceHandle) && (object->m_flowFlags & FLAG_MAP_OUTPUT_TO_ISERVER))
  	{
  		wxInt32 i,ret;
  		char   *nodeName;
  		
  		if (isOutput) nodeName=object->nodeNameOut;
  		else nodeName=object->nodeNameIn;
  		for (i=0; i<2; i++)
  		{
     		// TODO: put in queue and own thread to avoid blocking of data flow
         wxASSERT(targetInput & OAPC_BIN_IO_MASK);
         wxASSERT(strlen(nodeName));
         ret=oapc_ispace_set_value(g_ispaceHandle,nodeName,targetInput,binValue->m_bin,&g_auth);
  	   	if (ret!=OAPC_OK)
  		   {
  		   	if ((i==1) || (!openISConnection(false)))
  		   	{
  			      // TODO: implement break up/error reaction
               g_ispaceHandle=NULL;
               return false;
  		   	}
  	   	}
         else return true;
  		}   		
  	}   	
  	return true;
}



bool FlowWalkerPool::checkObjectForInput(flowObject *object,wxUint64 outputs,wxLongLong currentTime,bool force)
{
   FlowConnection connection;
   bool           hasConnectedOutput=false;

   if (!g_flowDispatcher) return true; // is not available in case the license dialogue is still open
   if (((object->data.type==FLOW_TYPE_EXTERNAL_LIB) || (object->data.type==HMI_TYPE_EXTERNAL_LIB)) &&
            (outputs) &&
            ((object->lastPollTime+(object->m_cycleMicros/1000)<currentTime) || (force)))
   {
      wxUint64      bit=0x01010101;
      wxInt32       rcode,i;
      wxByte        digiValue=0;
      wxFloat64     numValue=0.0;
      wxString      charValue=_T("");
      oapcBinHeadSp binValue;
      bool          binValueUnused=true;

      for (i=0; i<MAX_NUM_IOS; i++)
      {
         if (outputs & bit)
         {
            rcode=-1;
            connection.data.sourceOutput=bit;
            if (bit & outputs & OAPC_DIGI_IO_MASK)
            {
               connection.sourceOutputNum=connection.getDigiIndexFromFlag(connection.data.sourceOutput);
               digiValue=object->getDigitalOutput(&connection,&rcode,0,currentTime);
               sendDigiValueToIS(bit & outputs & OAPC_DIGI_IO_MASK,digiValue,object,true);
#ifdef ENV_DEBUGGER
               setNewValue(connection.sourceOutputNum,digiValue,object);
#endif
            }
            if (bit & outputs & OAPC_NUM_IO_MASK)
            {
               connection.sourceOutputNum=connection.getNumIndexFromFlag(connection.data.sourceOutput);
               numValue=object->getNumOutput(&connection,&rcode,0,currentTime);
               sendNumValueToIS(bit & outputs & OAPC_NUM_IO_MASK,numValue,object,true);
#ifdef ENV_DEBUGGER
               setNewValue(connection.sourceOutputNum,numValue,object);
#endif
            }
            if (bit & outputs & OAPC_CHAR_IO_MASK)
            {
               connection.sourceOutputNum=connection.getCharIndexFromFlag(connection.data.sourceOutput);
               charValue=object->getCharOutput(&connection,&rcode,0);
               sendCharValueToIS(bit & outputs & OAPC_CHAR_IO_MASK,charValue,object,true);
#ifdef ENV_DEBUGGER
               setNewValue(connection.sourceOutputNum,charValue,object);
#endif
            }
            if (bit & outputs & OAPC_BIN_IO_MASK)
            {
               connection.sourceOutputNum=connection.getBinIndexFromFlag(connection.data.sourceOutput);
               binValue=object->getBinOutput(&connection,&rcode,0);
               if ((rcode==OAPC_OK) && (binValue))
               {
                  object->releaseBinData(&connection);
                  sendBinValueToIS(bit & outputs & OAPC_BIN_IO_MASK,binValue,object,true);
#ifdef ENV_DEBUGGER
                  setNewValue(connection.sourceOutputNum,binValue,object);
#endif
               }
             }

             if ((rcode & OAPC_ERROR_MASK)==OAPC_OK)
             {
                wxNode         *node;
                FlowConnection *connection;

                node=object->getFlowConnection(NULL);
                while (node)
                {
                   connection=(FlowConnection*)node->GetData();

                   if (connection->data.sourceOutput & bit) // we found the flow connection that belongs to the current output
                   {
                      FlowWalkerThread *dataThread=NULL;
                      
                      hasConnectedOutput=true;
                      if (bit & outputs & OAPC_DIGI_IO_MASK)
                      {
                         dataThread=new FlowWalkerThread(object,connection,FlowWalkerThread::THREAD_MODE_DIGI,currentTime,0);
                         dataThread->setFirstOutValue(digiValue,object);
                         g_flowDispatcher->appendNewThread(dataThread);
                      }
                      if (bit & outputs & OAPC_NUM_IO_MASK)
                      {
                         dataThread=new FlowWalkerThread(object,connection,FlowWalkerThread::THREAD_MODE_NUM,currentTime,0);
                         dataThread->setFirstOutValue(numValue,object);
                         g_flowDispatcher->appendNewThread(dataThread);
                      }
                      if (bit & outputs & OAPC_CHAR_IO_MASK)
                      {
                         dataThread=new FlowWalkerThread(object,connection,FlowWalkerThread::THREAD_MODE_CHAR,currentTime,0);
                         dataThread->setFirstOutValue(charValue,object);
                         g_flowDispatcher->appendNewThread(dataThread);
                      }
                      if (bit & outputs & OAPC_BIN_IO_MASK)
                      {
                         dataThread=new FlowWalkerThread(object,connection,FlowWalkerThread::THREAD_MODE_BIN,currentTime,0);
                         dataThread->setFirstOutValue(binValue,object);
                         rcode&=~OAPC_INT_BIN_NOT_UNLOCKEDi;
                         g_flowDispatcher->appendNewThread(dataThread);
                         binValueUnused=false;
                      }
                   }
                   node=object->getFlowConnection(node);
                }
             }
            else hasConnectedOutput=true; // may have a connected output but we don't have checked it now because there were no data available
            if ((binValue) && (binValueUnused))
            {
               binValue.reset();
            }
         }
         bit=bit<<1;
      }
      object->lastPollTime=wxGetLocalTimeMillis();
   }
   else return true;
   return hasConnectedOutput;
}



wxUint32 FlowWalkerPool::getFlowThreadID()
{
   oapcMutexLocker lock(idSection,__FILE__,__LINE__);
   currFlowThreadID++;
   return currFlowThreadID;
}



void FlowWalkerPool::startInitialFlows(ObjectList *olist)
{
   wxNode     *node;
   flowObject *object;

   // start the flows that run at program start
#ifndef ENV_HPLAYER
   g_objectList.setUserPrivileges(NULL);
#endif
   if (!olist) return;
   node=olist->getObject((wxNode*)NULL);
   while (node)
   {
      object=(flowObject*)node->GetData();
      if (object->data.type==flowObject::FLOW_TYPE_FLOW_START)
       startFlows(object,NULL,object->data.stdOUT,0,wxGetLocalTimeMillis(),0);
      if (object->data.type==flowObject::FLOW_TYPE_GROUP)
       startInitialFlows(((flowGroup*)object)->getObjectList());
      node=node->GetNext();
   }
}



void FlowWalkerPool::startTimerFlows(ObjectList *olist)
{
   wxNode     *node;
   flowObject *object;

   // start the flows that use timers and run periodically
   if (!olist) return;
   node=olist->getObject((wxNode*)NULL);
   while (node)
   {
      object=(flowObject*)node->GetData();
      if (object->data.type==flowObject::FLOW_TYPE_FLOW_TIMER) object->applyData(1);
      else if (object->data.type==flowObject::FLOW_TYPE_GROUP)
       startTimerFlows(((flowGroup*)object)->getObjectList());
      node=node->GetNext();
   }
}



/**
This method starts new control flows for an object
@param[in] object the object that has to be checked for valid outgoing connections and where
                  new flowWalkerThreads have to be created for
@param[in] thread an optionally existing flow thread, this one will continue with the first
                  flowConnection, therefore all new threads have to be created for all following
                  flowConnections only
@param[in] inheritThreadID ID of a thread that will die after this call but where its context has not to die
                  to avoid endless loops: if inheritThreadID is not equal 0 the first creadted thread will
                  get this ID
*/
void FlowWalkerPool::startFlows(flowObject *object,FlowWalkerThread *thread,wxUint64 usedInput,wxUint32 inheritThreadID,wxLongLong inheritCreationTime,wxUint64 lastInput)
{
   FlowConnection       *connection;
   FlowConnection        statConnection;
   wxUint64              assignedOutputs=0;
   wxInt32               rcode[MAX_NUM_IOS+1],i;
   wxInt32               statRcode;
   wxUint32              bitmask=0x01010101;

   if (g_breakThreads) return;
   if (!thread) // if there is no running thread this flow is started externally, not by a running flow
   {
      assignedOutputs=usedInput;
   }
   else
   {
      assignedOutputs=object->getAssignedOutput(usedInput);
      if (assignedOutputs==0) // if it is an external lib we have to exit here: their outputs are checked periodically or by callback
      {
         thread->setNextConnection(NULL);
         return;
      }
   }

   for (i=0; i<MAX_NUM_IOS+1; i++) object->dataValid[i]=false;
   
   wxNode *node=object->getFlowConnection(NULL);
   while (node)
   {
      connection=(FlowConnection*)node->GetData();
      rcode[connection->sourceOutputNum]=OAPC_OK;

      // prefetch the output data if not already done ***************************
      wxASSERT(connection);
      if (connection->data.sourceOutput & assignedOutputs)
      {
         if (connection->data.sourceOutput & (OAPC_DIGI_IO_MASK|FLOW_TYPE_FLAG_DIGI_OVERFLOW_OUT))
         {
            if (!object->dataValid[connection->sourceOutputNum])
             object->dataDigi[connection->sourceOutputNum]=object->getDigitalOutput(connection,&rcode[connection->sourceOutputNum],usedInput,inheritCreationTime);
#ifdef ENV_DEBUGGER
            setNewValue(connection->sourceOutputNum,object->dataDigi[connection->sourceOutputNum],object);
#endif
         }
         else if (connection->data.sourceOutput & (OAPC_NUM_IO_MASK|FLOW_TYPE_FLAG_NUM_OVERFLOW_OUT))
         {
            if (!object->dataValid[connection->sourceOutputNum])
             object->dataNum[connection->sourceOutputNum]=object->getNumOutput(connection,&rcode[connection->sourceOutputNum],usedInput,inheritCreationTime);
#ifdef ENV_DEBUGGER
            setNewValue(connection->sourceOutputNum,object->dataNum[connection->sourceOutputNum],object);
#endif
         }
         else if (connection->data.sourceOutput & (OAPC_CHAR_IO_MASK|FLOW_TYPE_FLAG_CHAR_OVERFLOW_OUT))
         {
            if (!object->dataValid[connection->sourceOutputNum])
             object->dataTxt[connection->sourceOutputNum]=object->getCharOutput(connection,&rcode[connection->sourceOutputNum],usedInput);
#ifdef ENV_DEBUGGER
            setNewValue(connection->sourceOutputNum,object->dataTxt[connection->sourceOutputNum],object);
#endif
         }
         else if (connection->data.sourceOutput & (OAPC_BIN_IO_MASK|FLOW_TYPE_FLAG_DIGI_OVERFLOW_OUT))
         {
            if (!object->dataValid[connection->sourceOutputNum])
             object->dataBin[connection->sourceOutputNum]=object->getBinOutput(connection,&rcode[connection->sourceOutputNum],usedInput);
#ifdef ENV_DEBUGGER
            setNewValue(connection->sourceOutputNum,object->dataBin[connection->sourceOutputNum],object);
#endif
            //TODO: decrease oapc_bin_head usage ctr -> really?
         }
         if ((rcode[connection->sourceOutputNum] & OAPC_ERROR_MASK)!=OAPC_OK)
         {
            if ((rcode[connection->sourceOutputNum] & OAPC_ERROR_MASK)==OAPC_ERROR_NO_DATA_AVAILABLE) // this message is sent in case of an overflow
            {
//            	   pthread_mutex_unlock(&flowSection);
//               return 0;
            }
            else
            {
               wxASSERT(0);
#ifdef ENV_DEBUGGER
#endif
//            	   pthread_mutex_unlock(&flowSection);
//               return 0;
            }
         }
         else object->dataValid[connection->sourceOutputNum]=true;
      }
      // end of prefetch the output data if not already done ********************
      node=node->GetNext();
   }


   for (i=0; i<MAX_NUM_IOS/*+1*/; i++)
   {
      statConnection.sourceOutputNum=i;
      if (assignedOutputs & bitmask & OAPC_DIGI_IO_MASK)
      {
         if (!object->dataValid[i])
          object->dataDigi[i]=object->getDigitalOutput(&statConnection,&statRcode,usedInput,inheritCreationTime);
         sendDigiValueToIS(assignedOutputs & bitmask & OAPC_DIGI_IO_MASK,object->dataDigi[i],object,true);
      }
      else if (assignedOutputs & bitmask & OAPC_NUM_IO_MASK)
      {
         if (!object->dataValid[i])
          object->dataNum[i]=object->getNumOutput(&statConnection,&statRcode,usedInput,inheritCreationTime);
         sendNumValueToIS(assignedOutputs & bitmask & OAPC_NUM_IO_MASK,object->dataNum[i],object,true);
      }
      else if (assignedOutputs & bitmask & OAPC_CHAR_IO_MASK)
      {
         if (!object->dataValid[i])
          object->dataTxt[i]=object->getCharOutput(&statConnection,&statRcode,usedInput);
         sendCharValueToIS(assignedOutputs & bitmask & OAPC_CHAR_IO_MASK,object->dataTxt[i],object,true);
      }
      else if (assignedOutputs & bitmask & OAPC_BIN_IO_MASK)
      {
         if (!object->dataValid[i])
          object->dataBin[i]=object->getBinOutput(&statConnection,&statRcode,usedInput);
         sendBinValueToIS(assignedOutputs & bitmask & OAPC_BIN_IO_MASK,object->dataBin[i],object,true);
      }
      bitmask=bitmask<<1;
   }


   node=object->getFlowConnection(NULL);
   if ((!node) && (thread)) thread->setNextConnection(NULL); // this flow ends here because there are no outgoing flow connections
   while (node)
   {
      connection=(FlowConnection*)node->GetData();

      wxASSERT(connection);
      if ((connection) && (connection->data.sourceOutput & assignedOutputs) && (object->dataValid[connection->sourceOutputNum]))
      {
         if (thread)
         {
            thread->setNextConnection(connection);
            if ((connection->data.sourceOutput & assignedOutputs & (OAPC_DIGI_IO_MASK|FLOW_TYPE_FLAG_DIGI_OVERFLOW_OUT))==connection->data.sourceOutput)
            {
               thread->setMode(FlowWalkerThread::THREAD_MODE_DIGI);
               thread->setFirstOutValue(object->dataDigi[connection->sourceOutputNum],object);
               thread=NULL;
            }
            else if ((connection->data.sourceOutput & assignedOutputs & (OAPC_NUM_IO_MASK|FLOW_TYPE_FLAG_NUM_OVERFLOW_OUT))==connection->data.sourceOutput)
            {
               thread->setMode(FlowWalkerThread::THREAD_MODE_NUM);
               thread->setFirstOutValue(object->dataNum[connection->sourceOutputNum],object);
               thread=NULL;
            }
            else if ((connection->data.sourceOutput & assignedOutputs & (OAPC_CHAR_IO_MASK|FLOW_TYPE_FLAG_CHAR_OVERFLOW_OUT))==connection->data.sourceOutput)
            {
               thread->setMode(FlowWalkerThread::THREAD_MODE_CHAR);
               thread->setFirstOutValue(object->dataTxt[connection->sourceOutputNum],object);
               thread=NULL;
            }
            else if ((connection->data.sourceOutput & assignedOutputs & (OAPC_BIN_IO_MASK|FLOW_TYPE_FLAG_BIN_OVERFLOW_OUT))==connection->data.sourceOutput)
            {
               thread->setMode(FlowWalkerThread::THREAD_MODE_BIN);
               thread->setFirstOutValue(object->dataBin[connection->sourceOutputNum],object);
               rcode[connection->sourceOutputNum]&=~OAPC_INT_BIN_NOT_UNLOCKEDi;
               thread=NULL;
            }
         }
         else if ((connection) &&
                  ((connection->data.sourceOutput & assignedOutputs & OAPC_DIGI_IO_MASK)==connection->data.sourceOutput))
         {
            FlowWalkerThread *digiThread;
             digiThread=new FlowWalkerThread(object,connection,FlowWalkerThread::THREAD_MODE_DIGI,inheritCreationTime,lastInput);
            if (digiThread)
            {
               if (inheritThreadID>0) digiThread->setFlowThreadID(inheritThreadID);
               digiThread->setFirstOutValue(object->dataDigi[connection->sourceOutputNum],object);
               inheritThreadID=0;
               g_flowDispatcher->appendNewThread(digiThread);
            }
            else
            {
#ifdef ENV_DEBUGGER
               g_debugWin->setDebugInformation(object,DEBUG_STOP_COND_NO_MEMORY,_T(""));
#endif
            }
         }
         else if ((connection) &&
                  ((connection->data.sourceOutput & assignedOutputs & OAPC_NUM_IO_MASK)==connection->data.sourceOutput))
         {
            FlowWalkerThread *numThread;
             numThread=new FlowWalkerThread(object,connection,FlowWalkerThread::THREAD_MODE_NUM,inheritCreationTime,lastInput);
            if (numThread)
            {
               if (inheritThreadID>0) numThread->setFlowThreadID(inheritThreadID);
               numThread->setFirstOutValue(object->dataNum[connection->sourceOutputNum],object);
               inheritThreadID=0;
               g_flowDispatcher->appendNewThread(numThread);
            }
            else
            {
#ifdef ENV_DEBUGGER
               g_debugWin->setDebugInformation(object,DEBUG_STOP_COND_NO_MEMORY,_T(""));
#endif
            }
         }
         else if ((connection) &&
                  ((connection->data.sourceOutput & assignedOutputs & OAPC_CHAR_IO_MASK)==connection->data.sourceOutput))
         {
            FlowWalkerThread *charThread;
             charThread=new FlowWalkerThread(object,connection,FlowWalkerThread::THREAD_MODE_CHAR,inheritCreationTime,lastInput);
            if (charThread)
            {
               if (inheritThreadID>0) charThread->setFlowThreadID(inheritThreadID);
               charThread->setFirstOutValue(object->dataTxt[connection->sourceOutputNum],object);
               inheritThreadID=0;
               g_flowDispatcher->appendNewThread(charThread);
            }
            else
            {
#ifdef ENV_DEBUGGER
               g_debugWin->setDebugInformation(object,DEBUG_STOP_COND_NO_MEMORY,_T(""));
#endif
            }
         }
         else if ((connection) &&
                  ((connection->data.sourceOutput & assignedOutputs & OAPC_BIN_IO_MASK)==connection->data.sourceOutput))
         {
            FlowWalkerThread *binThread;
             binThread=new FlowWalkerThread(object,connection,FlowWalkerThread::THREAD_MODE_BIN,inheritCreationTime,lastInput);
            if (binThread)
            {
               if (inheritThreadID>0) binThread->setFlowThreadID(inheritThreadID);
               binThread->setFirstOutValue(object->dataBin[connection->sourceOutputNum],object);
               inheritThreadID=0;
               g_flowDispatcher->appendNewThread(binThread);
            }
            else
            {
#ifdef ENV_DEBUGGER
               g_debugWin->setDebugInformation(object,DEBUG_STOP_COND_NO_MEMORY,_T(""));
#endif
            }
         }
      }
      node=node->GetNext();
   }
   if (thread) thread->setNextConnection(NULL); // we had a thread but there could no output be found where we could continue it
}



void FlowWalkerPool::startOverflowFlows(flowObject *object,wxUint64 assignedOutputs,wxByte firstDigiVal,wxFloat64 firstNumVal,wxString firstCharVal,const oapcBinHeadSp &firstBinVal,wxLongLong inheritCreationTime)
{
   FlowConnection       *connection;

//   oapcMutexLocker lock(&flowSection,__FILE__,__LINE__);
   wxNode *node=object->getFlowConnection(NULL);
   while (node)
   {
       connection=(FlowConnection*)node->GetData();
       wxASSERT(connection);
       if (connection->data.sourceOutput)
       {
          if ((connection) && (assignedOutputs & connection->data.sourceOutput & (FLOW_TYPE_FLAG_DIGI_OVERFLOW_OUT|OAPC_DIGI_IO_MASK)))
          {
             FlowWalkerThread *digiThread;

             digiThread=new FlowWalkerThread(object,connection,FlowWalkerThread::THREAD_MODE_DIGI,inheritCreationTime,0);
             digiThread->setFirstOutValue(firstDigiVal,object);
             g_flowDispatcher->appendNewThread(digiThread);
          }
          else if ((connection) && (assignedOutputs & connection->data.sourceOutput & (FLOW_TYPE_FLAG_NUM_OVERFLOW_OUT|OAPC_NUM_IO_MASK)))
          {
             FlowWalkerThread *numThread;

             numThread=new FlowWalkerThread(object,connection,FlowWalkerThread::THREAD_MODE_NUM,inheritCreationTime,0);
             numThread->setFirstOutValue(firstNumVal,object);
             g_flowDispatcher->appendNewThread(numThread);
          }
          else if ((connection) && (assignedOutputs & connection->data.sourceOutput & (FLOW_TYPE_FLAG_CHAR_OVERFLOW_OUT|OAPC_CHAR_IO_MASK)))
          {
             FlowWalkerThread *charThread;

             charThread=new FlowWalkerThread(object,connection,FlowWalkerThread::THREAD_MODE_CHAR,inheritCreationTime,0);
             charThread->setFirstOutValue(firstCharVal,object);
             g_flowDispatcher->appendNewThread(charThread);
          }
          else if ((connection) && (assignedOutputs & connection->data.sourceOutput & FLOW_TYPE_FLAG_BIN_OVERFLOW_OUT))
          {
             FlowWalkerThread *binThread;

             binThread=new FlowWalkerThread(object,connection,FlowWalkerThread::THREAD_MODE_BIN,inheritCreationTime,0);
             binThread->setFirstOutValue(firstBinVal,object);
             g_flowDispatcher->appendNewThread(binThread);
          }
       }
       node=node->GetNext();
   }
}
