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

#ifdef ENV_DEBUGGER
#include "DebugWin.h"
#endif

#include "iff.h"
#include "globals.h"
#include "flowLogRecorder.h"
#include "oapc_libio.h"
#ifdef ENV_PLAYER
#include "LogDataCtrl.h"
#else
#include "DlgConfigflowLogRecorder.h"
#endif

#ifdef ENV_WINDOWS
 #ifdef ENV_WINDOWSCE
  #include "Winsock2.h"
 #endif
#endif



flowLogRecorder::flowLogRecorder():flowObject(NULL)
{
   this->data.type=FLOW_TYPE_MISC_LOG_REC;
   this->data.stdIN =OAPC_DIGI_IO0|OAPC_DIGI_IO1|OAPC_NUM_IO2|OAPC_NUM_IO3|OAPC_CHAR_IO4|OAPC_CHAR_IO5;
   this->data.stdOUT=0;
#ifdef ENV_EDITOR
   memset(&logData.data,0,sizeof(struct hmiObjectLogData)*MAX_NUM_IOS*MAX_LOG_TYPES);
   logData.logFlags[0]=LOG_TYPE_DIGI;     logData.logFlags[1]=LOG_TYPE_DIGI;
   logData.logFlags[2]=LOG_TYPE_FLOATNUM; logData.logFlags[3]=LOG_TYPE_FLOATNUM;
   logData.logFlags[4]=LOG_TYPE_CHAR;     logData.logFlags[5]=LOG_TYPE_CHAR;
#else
   for (wxInt32 i=0; i<6; i++) logDataCtrl[i]=NULL;
#endif
}



flowLogRecorder::~flowLogRecorder()
{
#ifdef ENV_PLAYER
   wxInt32 i;

   for (i=0; i<MAX_NUM_IOS; i++)
   {
      if (logDataCtrl[i]) delete logDataCtrl[i];
   }
#endif
}



wxString flowLogRecorder::getDefaultName()
{
   return _("Log Recorder");
}



#ifdef ENV_EDITOR
void flowLogRecorder::doDataFlowDialog(bool hideISConfig)
{
   DlgConfigflowLogRecorder dlg(this,(wxWindow*)g_hmiCanvas,_("Definition"),hideISConfig);

   dlg.ShowModal();
   if (dlg.returnOK)
   {
   }
   dlg.Destroy();
}



wxInt32 flowLogRecorder::saveDATA(wxFile *FHandle,char chunkName[4],bool isCompiled)
{
   wxInt32                     length;
   struct hmiObjectHead        logHead;
   struct flowLogRecorderData  saveData;
   wxInt32                     i,l,len;
   wxNode                     *node;
   FlowConnection             *connection;
   wxMBConvUTF16BE             strConv;


   if (!FHandle) return 0;
   checkConnections();
   FHandle->Write(chunkName,4);
   if (isCompiled) length=sizeof(struct hmiObjectHead)+sizeof(struct flowLogRecorderData)+(flowList.GetCount()*FLOW_CONNECTION_HEAD_SIZE);
   else length=sizeof(struct hmiObjectHead)+sizeof(struct flowLogRecorderData)+(flowList.GetCount()*sizeof(struct flowConnectionData));
   length=htonl(length);
   FHandle->Write(&length,4);

   logHead.version=htonl(1);
   if (isCompiled) logHead.size=htonl(sizeof(struct flowLogRecorderData)+(flowList.GetCount()*FLOW_CONNECTION_HEAD_SIZE));
   else logHead.size=htonl(sizeof(struct flowLogRecorderData)+(flowList.GetCount()*sizeof(struct flowConnectionData)));
   logHead.reserved1=0;
   logHead.reserved2=0;
   len=FHandle->Write(&logHead,sizeof(struct hmiObjectHead));

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
      saveData.logFlags[i]=htonl(logData.logFlags[i]);
      for (l=0; l<MAX_LOG_TYPES; l++)
      {
         saveData.data[i][l].mBelowVal=htonl(logData.data[i][l].mBelowVal);
         saveData.data[i][l].mAboveVal=htonl(logData.data[i][l].mAboveVal);
         saveData.data[i][l].mFromVal =htonl(logData.data[i][l].mFromVal);
         saveData.data[i][l].mToVal   =htonl(logData.data[i][l].mToVal);
         saveData.data[i][l].flags    =htonl(logData.data[i][l].flags);
         saveData.data[i][l].reserved1=htonl(logData.data[i][l].reserved1);
      }
   }
   
   len+=FHandle->Write(&saveData,sizeof(struct flowLogRecorderData));

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
wxUint64 flowLogRecorder::getAssignedOutput(wxUint64 WXUNUSED(input))
{
   return 0;
}



wxByte flowLogRecorder::setDigitalInput(FlowConnection *connection,wxByte value,wxUint32 *flowThreadID,flowObject *object)
{
   if (!threadIDOK(flowThreadID,false)) return 0;
   wxASSERT((connection->targetInputNum==0) || (connection->targetInputNum==1));
   if ((connection->targetInputNum!=0) && (connection->targetInputNum!=1)) return 0;
   logDataCtrl[connection->targetInputNum]->logDigi(value,object);
   return 1;
}



wxByte flowLogRecorder::setCharInput(FlowConnection *connection,wxString value,wxUint32 *flowThreadID,flowObject *object)
{
   if (!threadIDOK(flowThreadID,false)) return 0;
   wxASSERT((connection->targetInputNum==4) || (connection->targetInputNum==5));
   if ((connection->targetInputNum!=4) && (connection->targetInputNum!=5)) return 0;
   logDataCtrl[connection->targetInputNum]->logChar(value,object);   
   return 1;
}



wxByte flowLogRecorder::setNumInput(FlowConnection *connection,wxFloat64 value,wxUint32 *flowThreadID,flowObject *object)
{
   if (!threadIDOK(flowThreadID,false)) return 0;
   wxASSERT((connection->targetInputNum==2) || (connection->targetInputNum==3));
   if ((connection->targetInputNum!=2) && (connection->targetInputNum!=3)) return 0;
   logDataCtrl[connection->targetInputNum]->logNum(value,object);
   return 1;
}



#endif



wxInt32 flowLogRecorder::loadDATA(wxFile *FHandle,wxUint32 chunkSize,wxUint32 IDOffset,bool isCompiled)
{
   struct hmiObjectHead        logHead;
   struct flowLogRecorderData  loadData;
   wxInt32                     loaded,i,l;
   wxMBConvUTF16BE             strConv;
   wchar_t                     buf[MAX_TEXT_LENGTH];

   if (!FHandle) return 0;
   if (chunkSize>sizeof(struct hmiObjectHead)) chunkSize=sizeof(struct hmiObjectHead);
   loaded=FHandle->Read(&logHead,sizeof(struct hmiObjectHead));

   logHead.size     =ntohl(logHead.size);
   logHead.version  =ntohl(logHead.version);
   logHead.reserved1=ntohl(logHead.reserved1);
   logHead.reserved2=ntohl(logHead.reserved2);

   loaded+=FHandle->Read(&loadData,sizeof(struct flowLogRecorderData));

   data.id                    =ntohl(loadData.flowData.id);
   logData.flowData.usedFlows=ntohl(loadData.flowData.usedFlows);
   logData.flowData.maxEdges =ntohl(loadData.flowData.maxEdges);
   m_flowFlags               =ntohl(loadData.flowData.flowFlags);
   logData.flowX=             ntohl(loadData.flowX);
   logData.flowY=             ntohl(loadData.flowY);
   strConv.MB2WC(buf,loadData.store_name,sizeof(buf));
   name=buf;
#ifdef ENV_EDITOR
   setFlowPos(NULL,wxPoint(logData.flowX,logData.flowY),1,1);
#else
   createNodeNames();
#endif

   for (i=0; i<MAX_NUM_IOS; i++)
   {
      logData.logFlags[i]=ntohl(loadData.logFlags[i]);
      for (l=0; l<MAX_LOG_TYPES; l++)
      {
         logData.data[i][l].mBelowVal=ntohl(loadData.data[i][l].mBelowVal);
         logData.data[i][l].mAboveVal=ntohl(loadData.data[i][l].mAboveVal);
         logData.data[i][l].mFromVal =ntohl(loadData.data[i][l].mFromVal);
         logData.data[i][l].mToVal   =ntohl(loadData.data[i][l].mToVal);
         logData.data[i][l].flags    =ntohl(loadData.data[i][l].flags);
         logData.data[i][l].reserved1=ntohl(loadData.data[i][l].reserved1);
      }
#ifdef ENV_PLAYER
      logDataCtrl[i]=new LogDataCtrl(logData.data[i]);
#endif
   }
   loaded+=flowObject::loadFlow(FHandle,&logData.flowData,IDOffset,false,isCompiled);
   return loaded;
}


