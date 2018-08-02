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

#ifdef ENV_DEBUGGER
#include "DebugWin.h"
#endif

#include "iff.h"
#include "globals.h"
#include "flowDelay.h"
#include "oapc_libio.h"
#ifdef ENV_EDITOR
#include "DlgConfigflowDelay.h"
#endif

#ifdef ENV_WINDOWS
 #ifdef ENV_WINDOWSCE
  #include "Winsock2.h"
 #endif
#endif



flowDelay::flowDelay():flowObject(NULL)
{
#ifdef ENV_EDITOR
   wxInt32 i;

   for (i=0; i<MAX_NUM_IOS; i++) convData.delay[i]=150;
#endif
   this->data.type=FLOW_TYPE_FLOW_DELAY;
   this->data.stdIN =OAPC_DIGI_IO0|OAPC_DIGI_IO1|OAPC_NUM_IO2|OAPC_NUM_IO3|OAPC_CHAR_IO4|OAPC_CHAR_IO5|OAPC_NUM_IO7;
   this->data.stdOUT=OAPC_DIGI_IO0|OAPC_DIGI_IO1|OAPC_NUM_IO2|OAPC_NUM_IO3|OAPC_CHAR_IO4|OAPC_CHAR_IO5;
}



flowDelay::~flowDelay()
{
}



wxString flowDelay::getDefaultName()
{
   return _T("Delay");
}



#ifdef ENV_EDITOR
void flowDelay::doDataFlowDialog(bool hideISConfig)
{
   wxString title;

   title=_("Definition")+title.Format(_T(" (%d)"),data.id);
   DlgConfigflowDelay dlg(this,(wxWindow*)g_hmiCanvas,title,hideISConfig);

   dlg.ShowModal();
   if (dlg.returnOK)
   {
   }
   dlg.Destroy();
}



wxInt32 flowDelay::saveDATA(wxFile *FHandle,char chunkName[4],bool isCompiled)
{
   wxInt32               length;
   struct hmiObjectHead  convHead;
   struct flowDelayData  saveData;
   wxInt32               i,len;
   wxNode               *node;
   FlowConnection       *connection;
   wxMBConvUTF16BE       strConv;


   if (!FHandle) return 0;
   checkConnections();
   FHandle->Write(chunkName,4);
   if (isCompiled) length=sizeof(struct hmiObjectHead)+sizeof(struct flowDelayData)+(flowList.GetCount()*FLOW_CONNECTION_HEAD_SIZE);
   else length=sizeof(struct hmiObjectHead)+sizeof(struct flowDelayData)+(flowList.GetCount()*sizeof(struct flowConnectionData));
   length=htonl(length);
   FHandle->Write(&length,4);

   convHead.version=htonl(1);
   if (isCompiled) convHead.size=htonl(sizeof(struct flowDelayData)+(flowList.GetCount()*FLOW_CONNECTION_HEAD_SIZE));
   else convHead.size=htonl(sizeof(struct flowDelayData)+(flowList.GetCount()*sizeof(struct flowConnectionData)));
   convHead.reserved1=0;
   convHead.reserved2=0;
   len=FHandle->Write(&convHead,sizeof(struct hmiObjectHead));

   saveData.flowData.id       =htonl(data.id);
   saveData.flowData.version  =htonl(1);
   saveData.flowData.usedFlows=htonl(flowList.GetCount());
   saveData.flowData.maxEdges =htonl(MAX_CONNECTION_EDGES);
   saveData.flowData.flowFlags=htonl(m_flowFlags);
   saveData.flowX             =htonl(getFlowPos().x);
   saveData.flowY             =htonl(getFlowPos().y);
   strConv.WC2MB(saveData.store_name,name, sizeof(saveData.store_name));
   for (i=0; i<MAX_NUM_IOS; i++)
   {
      saveData.delay[i]=htonl(convData.delay[i]);
   }
   len+=FHandle->Write(&saveData,sizeof(struct flowDelayData));

   node=flowList.GetFirst();
   while (node)
   {
      connection=(FlowConnection*)node->GetData();
      len+=connection->saveFlow(FHandle,isCompiled);
      node=node->GetNext();
   }
   return len;
}


#else
wxUint64 flowDelay::getAssignedOutput(wxUint64 WXUNUSED(input))
{
   return 0;
}



static void *delayThread(void *data)
{
   flowDelayThreadData *tData;

   tData=(flowDelayThreadData*)data;
   if (g_breakThreads) return NULL;
   oapc_thread_sleep(tData->m_delay);
   if (g_breakThreads) return NULL;

   if (tData->m_output & OAPC_DIGI_IO_MASK)
    g_flowPool.startOverflowFlows(tData->m_sourceObject,tData->m_output,tData->m_digi,0.0,_T(""),oapcBinHeadSp(),wxGetLocalTimeMillis());
   else if (tData->m_output & OAPC_NUM_IO_MASK)
    g_flowPool.startOverflowFlows(tData->m_sourceObject,tData->m_output,0,tData->m_num,_T(""),oapcBinHeadSp(),wxGetLocalTimeMillis());
   else if (tData->m_output & OAPC_CHAR_IO_MASK)
    g_flowPool.startOverflowFlows(tData->m_sourceObject,tData->m_output,0,0.0,tData->m_txt,oapcBinHeadSp(),wxGetLocalTimeMillis());

   delete tData;
   return NULL;
}



wxByte flowDelay::setDigitalInput(FlowConnection *connection,wxByte value,wxUint32 *flowThreadID,flowObject *WXUNUSED(object))
{
   flowDelayThreadData *tData;

   if (!threadIDOK(flowThreadID,false)) return 0;
   tData=new flowDelayThreadData();
   if (!tData) return 0;
   tData->m_output=connection->data.targetInput;
   tData->m_digi=value;
   tData->m_delay=convData.delay[connection->targetInputNum];
   tData->m_sourceObject=this;
   if (!oapc_thread_create(delayThread,(void*)tData))
   {
      delete tData;
      return 0;
   }
   return 1;
}



wxByte flowDelay::setNumInput(FlowConnection *connection,wxFloat64 value,wxUint32 *flowThreadID,flowObject *WXUNUSED(object))
{
   flowDelayThreadData *tData;

   if (!threadIDOK(flowThreadID,false)) return 0;
   if (connection->targetInputNum==7)
   {
      // set new delay values
      int i;

      for (i=0; i<MAX_NUM_IOS; i++) convData.delay[i]=(wxUint32)OAPC_ROUND(value,0);
   }
   else
   {
      tData=new flowDelayThreadData();
      if (!tData) return 0;
      tData->m_output=connection->data.targetInput;
      tData->m_num=value;
      tData->m_delay=convData.delay[connection->targetInputNum];
      tData->m_sourceObject=this;
      if (!oapc_thread_create(delayThread,(void*)tData))
      {
         delete tData;
         return 0;
      }
   }
   return 1;
}



wxByte flowDelay::setCharInput(FlowConnection *connection,wxString value,wxUint32 *flowThreadID,flowObject *WXUNUSED(object))
{
   flowDelayThreadData *tData;

   if (!threadIDOK(flowThreadID,false)) return 0;
   tData=new flowDelayThreadData();
   if (!tData) return 0;
   tData->m_output=connection->data.targetInput;
   tData->m_txt=value;
   tData->m_delay=convData.delay[connection->targetInputNum];
   tData->m_sourceObject=this;
   if (!oapc_thread_create(delayThread,(void*)tData))
   {
      delete tData;
      return 0;
   }
   return 1;
}



#endif



wxInt32 flowDelay::loadDATA(wxFile *FHandle,wxUint32 chunkSize,wxUint32 IDOffset,bool isCompiled)
{
   struct hmiObjectHead  convHead;
   struct flowDelayData  loadData;
   wxInt32               loaded,i;
   wxMBConvUTF16BE       strConv;
   wchar_t               buf[MAX_TEXT_LENGTH];

   if (!FHandle) return 0;
   if (chunkSize>sizeof(struct hmiObjectHead)) chunkSize=sizeof(struct hmiObjectHead);
   loaded=FHandle->Read(&convHead,sizeof(struct hmiObjectHead));

   convHead.size     =ntohl(convHead.size);
   convHead.version  =ntohl(convHead.version);
   convHead.reserved1=ntohl(convHead.reserved1);
   convHead.reserved2=ntohl(convHead.reserved2);

   loaded+=FHandle->Read(&loadData,sizeof(struct flowDelayData));

   if (IDOffset==0)
    data.id                   =ntohl(loadData.flowData.id);
   else
    data.id                   =ntohl(loadData.flowData.id)-IDOffset+g_objectList.currentUniqueID()+1;
   convData.flowData.usedFlows=ntohl(loadData.flowData.usedFlows);
   convData.flowData.maxEdges =ntohl(loadData.flowData.maxEdges);
   m_flowFlags                =ntohl(loadData.flowData.flowFlags);
   convData.flowX=             ntohl(loadData.flowX);
   convData.flowY=             ntohl(loadData.flowY);
   strConv.MB2WC(buf,loadData.store_name,sizeof(buf));
   name=buf;
#ifdef ENV_EDITOR
   setFlowPos(NULL,wxPoint(convData.flowX,convData.flowY),1,1);
#else
   createNodeNames();
#endif
   for (i=0; i<MAX_NUM_IOS; i++)
   {
      convData.delay[i]=ntohl(loadData.delay[i]);
   }
   loaded+=flowObject::loadFlow(FHandle,&convData.flowData,IDOffset,false,isCompiled);
   return loaded;
}


