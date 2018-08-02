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

#include "hmiObject.h"
#include "oapc_libio.h"
#include "globals.h"
#include "hmiHSlider.h"
#include "hmiNumField.h"
#include "hmiTextField.h"
#include "flowWalkerPool.h"
#include "flowConnection.h"
#include "flowWalkerThread.h"
#include "flowExternalIOLib.h"
#include "flowGroup.h"
#include "IOElementDefinition.h"
#include "hmiTabbedPane.h"
#include "hmiAdditionalPane.h"


#ifndef ENV_PLUGGER
DECLARE_APP(PlayerMainApp)
#endif

const wxUint32 FlowWalkerThread::THREAD_MODE_DIGI=1;
const wxUint32 FlowWalkerThread::THREAD_MODE_NUM =2;
const wxUint32 FlowWalkerThread::THREAD_MODE_CHAR=3;
const wxUint32 FlowWalkerThread::THREAD_MODE_BIN =4;



FlowWalkerThread::FlowWalkerThread(flowObject *object,FlowConnection *connection,wxUint32 mode,wxLongLong creationTime,wxUint64 lastInput)
{
   this->flowThreadID=g_flowPool.getFlowThreadID();
   this->object=object;
    
   this->lastInput=lastInput;
   if (connection) this->connection=connection;
   else
   {
      if (object->getFlowConnection(NULL))
       this->connection=(FlowConnection*)object->getFlowConnection(NULL)->GetData();
   }
   this->creationTime=creationTime;
   this->mode=mode;
   firstCall=true;
}



FlowWalkerThread::~FlowWalkerThread()
{
}



void FlowWalkerThread::setMode(wxUint32 mode)
{
   this->mode=mode;
}



/**
This method has to be called directly after thread creation. It has to be used, when an output already
has been used and the returned value is available: in this case the value is set here and the thread
starts with walking down the flow connection and setting the next input value instead of checking the
output again
@param[in] val the digital value that has to be used for the next objects input
*/
void FlowWalkerThread::setFirstOutValue(wxByte val,flowObject *object)
{
   digiValue=val;
   m_sourceObject=object;
}



/**
This method has to be called directly after thread creation. It has to be used, when an output already
has been used and the returned value is available: in this case the value is set here and the thread
starts with walking down the flow connection and setting the next input value instead of checking the
output again
@param[in] val the numerical value that has to be used for the next objects input
*/
void FlowWalkerThread::setFirstOutValue(wxFloat64 val,flowObject *object)
{
   numValue=val;
   m_sourceObject=object;
}



/**
This method has to be called directly after thread creation. It has to be used, when an output already
has been used and the returned value is available: in this case the value is set here and the thread
starts with walking down the flow connection and setting the next input value instead of checking the
output again
@param[in] val the character value that has to be used for the next objects input
*/
void FlowWalkerThread::setFirstOutValue(wxString val,flowObject *object)
{
   charValue=val;
   m_sourceObject=object;
}



/**
This method has to be called directly after thread creation. It has to be used, when an output already
has been used and the returned value is available: in this case the value is set here and the thread
starts with walking down the flow connection and setting the next input value instead of checking the
output again
@param[in] val the binary value that has to be used for the next objects input
*/
void FlowWalkerThread::setFirstOutValue(const oapcBinHeadSp &val,flowObject *object)
{
   binValue=val;
   firstCall=false;
   m_sourceObject=object;
}



void FlowWalkerThread::setNextConnection(FlowConnection *connection)
{
   this->connection=connection;
}



void FlowWalkerThread::setFlowThreadID(wxUint32 flowThreadID)
{
    this->flowThreadID=flowThreadID;
}



wxInt32 FlowWalkerThread::exitThread()
{
   return 0;
}



wxInt32 FlowWalkerThread::execute()
{
   if ((!connection) || (g_breakThreads)) return exitThread();

   if (connection->data.targetID==0) return exitThread();
   object=g_objectList.getObject(connection->data.targetID);
//   wxASSERT(object);
   if (!object) return exitThread();
   if (mode==THREAD_MODE_DIGI)
   {
      if (!connection->isTempConnection) g_flowPool.sendDigiValueToIS(connection->data.targetInput,digiValue,object,false);
      if (!object->setDigitalInput(connection,digiValue,&flowThreadID,m_sourceObject)) return exitThread();
      if (g_breakThreads) return exitThread();
   }
   else if (mode==THREAD_MODE_NUM)
   {
      if (!connection->isTempConnection) g_flowPool.sendNumValueToIS(connection->data.targetInput,numValue,object,false);
      if (!object->setNumInput(connection,numValue,&flowThreadID,m_sourceObject)) return exitThread();
   }
   else if (mode==THREAD_MODE_CHAR)
   {
      if (!connection->isTempConnection) g_flowPool.sendCharValueToIS(connection->data.targetInput,charValue,object,false);
      if (!object->setCharInput(connection,charValue,&flowThreadID,m_sourceObject)) return exitThread();
   }
   else if (mode==THREAD_MODE_BIN)
   {
      if (!connection->isTempConnection) g_flowPool.sendBinValueToIS(connection->data.targetInput,binValue,object,false);
      if (!object->setBinInput(connection,binValue,&flowThreadID,m_sourceObject)) return exitThread();
   }
   lastInput=connection->data.targetInput;
   g_flowPool.startFlows(object,this,connection->data.targetInput,0,creationTime,lastInput); // after this call this threads owns the new connection that has to be processed next

#ifndef _DEBUG      
   if (creationTime+g_objectList.m_projectData->flowTimeout<wxGetLocalTimeMillis())
   {
#ifdef ENV_DEBUGGER
      if (g_debugWin) g_debugWin->setDebugInformation(object,DEBUG_STOP_COND_RUN_TIMEOUT,_T(""));
#endif
      printf("Terminating thread due to timeout\n");
      return exitThread();
   }
#endif
   if (!connection) return exitThread();
   return 1;
}
