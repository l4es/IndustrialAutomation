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
#include <wx/mstream.h>

#include "iff.h"
#include "globals.h"
#include "flowLog.h"
#include "oapc_libio.h"


flowLog::flowLog():flowObject(NULL)
{
   this->data.type=FLOW_TYPE_MISC_LOG;
   this->data.stdIN= 0;
   this->data.stdOUT=OAPC_CHAR_IO0|OAPC_NUM_IO1|OAPC_CHAR_IO2|OAPC_NUM_IO3|OAPC_CHAR_IO4|OAPC_NUM_IO5|OAPC_CHAR_IO6|OAPC_NUM_IO7;
#ifndef ENV_EDITOR
   for (wxByte i=0; i<MAX_LOG_TYPES; i++)
   {
      m_logEnabled[i]=false;
      m_idLogEnabled[i]=false;
   }
#endif

}



flowLog::~flowLog()
{
}



#ifdef ENV_PLAYER
wxInt32 flowLog::loadFlow(wxFile *FHandle,struct hmiFlowData *flowData,wxUint32 IDOffset,bool usesOwnHead,bool isCompiled)
{
   wxInt32         ret;
   wxNode         *node;
   FlowConnection *connection;

   ret=flowObject::loadFlow(FHandle,flowData,IDOffset,usesOwnHead,isCompiled);

   node=getFlowConnection((wxNode*)NULL);
   while (node)
   {
      connection=(FlowConnection*)node->GetData();
      if (connection->data.sourceOutput & OAPC_CHAR_IO0)      m_logEnabled[0]=true;
      else if (connection->data.sourceOutput & OAPC_CHAR_IO2) m_logEnabled[1]=true;
      else if (connection->data.sourceOutput & OAPC_CHAR_IO4) m_logEnabled[2]=true;
      else if (connection->data.sourceOutput & OAPC_CHAR_IO6) m_logEnabled[3]=true;
      else if (connection->data.sourceOutput & OAPC_NUM_IO1) m_idLogEnabled[0]=true;
      else if (connection->data.sourceOutput & OAPC_NUM_IO3) m_idLogEnabled[1]=true;
      else if (connection->data.sourceOutput & OAPC_NUM_IO5) m_idLogEnabled[2]=true;
      else if (connection->data.sourceOutput & OAPC_NUM_IO7) m_idLogEnabled[3]=true;
      node=getFlowConnection(node);
   }

   return ret;
}
#endif



wxString flowLog::getDefaultName()
{
   return _("Log Output");
}



#ifndef ENV_EDITOR
void flowLog::setLogInfo(wxString logText,wxInt32 logNum,flowObject *object)
{
   wxString       log;
   FlowWalkerPool flowPool;

   if ((!m_logEnabled[logNum]) && (!m_idLogEnabled[logNum])) return;
   wxCriticalSectionLocker locker(m_logSection);
   m_dateTime=wxDateTime::Now();
#if wxCHECK_VERSION(2,9,0)
   log=log.Format(_T("%s - %s: %s"),m_dateTime.Format(_T("%c")),object->name,logText);
#else
   log=log.Format(_T("%s - %s: %s"),m_dateTime.Format(_T("%c")).c_str(),object->name.c_str(),logText.c_str());
#endif
   wxASSERT((logNum>=0) && (logNum<MAX_LOG_TYPES));
   if ((logNum>=0) && (logNum<MAX_LOG_TYPES))
   {
      g_flowPool.sendLogMessageToIS(logNum,log);

      m_logQ[logNum].push(log);
      m_idQ[logNum].push(object->data.id);
      if (logNum==0) // error
      {
         if (m_logEnabled[logNum])   flowPool.startFlows(this,NULL,OAPC_CHAR_IO0,0,wxGetLocalTimeMillis(),0);
         if (m_idLogEnabled[logNum]) flowPool.startFlows(this,NULL,OAPC_NUM_IO1,0,wxGetLocalTimeMillis(),0);
      }
      else if (logNum==1) // warning
      {
         if (m_logEnabled[logNum])   flowPool.startFlows(this,NULL,OAPC_CHAR_IO2,0,wxGetLocalTimeMillis(),0);
         if (m_idLogEnabled[logNum]) flowPool.startFlows(this,NULL,OAPC_NUM_IO3,0,wxGetLocalTimeMillis(),0);
      }
      else if (logNum==2) // info
      {
         if (m_logEnabled[logNum])   flowPool.startFlows(this,NULL,OAPC_CHAR_IO4,0,wxGetLocalTimeMillis(),0);
         if (m_idLogEnabled[logNum]) flowPool.startFlows(this,NULL,OAPC_NUM_IO5,0,wxGetLocalTimeMillis(),0);
      }
      else if (logNum==3) // event
      {
         if (m_logEnabled[logNum])   flowPool.startFlows(this,NULL,OAPC_CHAR_IO6,0,wxGetLocalTimeMillis(),0);
         if (m_idLogEnabled[logNum]) flowPool.startFlows(this,NULL,OAPC_NUM_IO7,0,wxGetLocalTimeMillis(),0);
      }
   }
}



wxFloat64 flowLog::getNumOutput(FlowConnection *connection,wxInt32 *rcode,wxUint64 WXUNUSED(lastInput),wxLongLong WXUNUSED(origCreationTime))
{
   wxByte    logNum;
   wxFloat32 val;

   logNum=connection->sourceOutputNum/2;

   wxASSERT(!m_idQ[logNum].empty());
   if (m_idQ[logNum].empty())
   {
      *rcode=OAPC_ERROR_NO_DATA_AVAILABLE;
      return 0.0;
   }
   *rcode=OAPC_OK;
   val=m_idQ[logNum].front();
   m_idQ[logNum].pop();
   return val;
}



wxString flowLog::getCharOutput(FlowConnection *connection,wxInt32 *rcode,wxUint64 WXUNUSED(lastInput))
{
   wxByte   logNum;
   wxString val;

   logNum=connection->sourceOutputNum/2;

   wxASSERT(!m_logQ[logNum].empty());
   if (m_logQ[logNum].empty())
   {
      *rcode=OAPC_ERROR_NO_DATA_AVAILABLE;
      return wxEmptyString;
   }
   *rcode=OAPC_OK;
   val=m_logQ[logNum].front();
   m_logQ[logNum].pop();
   return val;
}


#endif

