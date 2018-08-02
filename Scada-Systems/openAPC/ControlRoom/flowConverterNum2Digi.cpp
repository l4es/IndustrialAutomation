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
#include <wx/checkbox.h>

#ifndef __WXMSW__
#include <arpa/inet.h>
#endif

#ifdef ENV_DEBUGGER
#include "DebugWin.h"
#endif
#include "iff.h"
#include "globals.h"
#include "flowObject.h"
#include "flowConverter.h"
#include "flowConverterNum2Digi.h"
#ifdef ENV_EDITOR
#include "DlgConfigflowConvertNum2Digi.h"
#endif
#ifdef ENV_PLAYER
 #include "flowWalkerThread.h"
#endif
#include "oapc_libio.h"

#ifdef ENV_WINDOWS
 #ifdef ENV_WINDOWSCE
  #include "Winsock2.h"
 #endif
#endif



flowConverterNum2Digi::flowConverterNum2Digi():flowConverter()
{
#ifdef ENV_EDITOR
   wxUint32 i,j,bit=1;
#endif

   this->data.type=flowObject::FLOW_TYPE_CONV_NUM2DIGI;
   this->data.stdIN=OAPC_DIGI_IO0|OAPC_NUM_IO1|OAPC_NUM_IO2|OAPC_NUM_IO3|OAPC_NUM_IO4|OAPC_NUM_IO5|OAPC_NUM_IO6|OAPC_NUM_IO7;
   this->data.stdOUT=OAPC_DIGI_IO0|OAPC_DIGI_IO1|OAPC_DIGI_IO2|OAPC_DIGI_IO3|OAPC_DIGI_IO4|OAPC_DIGI_IO5|OAPC_DIGI_IO6|OAPC_DIGI_IO7|FLOW_TYPE_FLAG_NUM_OVERFLOW_OUT;
#ifdef ENV_EDITOR
   for (i=0; i<CONVERTER_MAX_OUTPUTS; i++)
   {
      convData.outData[i].enablePattern=1;
      for (j=0; j<CONVERTER_MAX_ASSIGNMENTS; j++)
      {
         convData.outData[i].outOperation[j]=CONVERTER_FLAGS_OUTPUT_HIGH|CONVERTER_FLAGS_USE_CLOCK;
         convData.outData[i].mInNum[j]=(j+1)*1000;
         convData.outData[i].compareFlag[j]=CONVERTER_FLAGS_CMP_EQUAL;
      }
      convData.outData[i].flags=CONVERTER_FLAGS_DIRECT_MODE|CONVERTER_FLAGS_USE_CLOCK;
      bit=bit<<1;
   }
#endif
}



flowConverterNum2Digi::~flowConverterNum2Digi()
{
}



wxString flowConverterNum2Digi::getDefaultName()
{
   return _T("Convert Num2Digi");
}



#ifndef ENV_PLAYER
void flowConverterNum2Digi::doDataFlowDialog(bool hideISConfig)
{
   DlgConfigflowConvertNum2Digi dlg(this,(wxWindow*)g_hmiCanvas,_("Definition"),hideISConfig);

   dlg.ShowModal();
   if (dlg.returnOK)
   {
   }
   dlg.Destroy();
}



wxInt32 flowConverterNum2Digi::saveDATA(wxFile *FHandle,char chunkName[4],bool isCompiled)
{
   wxInt32                            length;
   struct hmiObjectHead               convHead;
   struct flowConverterNum2DigiData  saveData;
   wxInt32                            i,j,len;
   wxNode                            *node;
   FlowConnection                    *connection;
   wxMBConvUTF16BE                    strConv;


   if (!FHandle) return 0;
   checkConnections();
   FHandle->Write(chunkName,4);
   if (isCompiled) length=sizeof(struct hmiObjectHead)+sizeof(struct flowConverterNum2DigiData)+(flowList.GetCount()*FLOW_CONNECTION_HEAD_SIZE);
   else length=sizeof(struct hmiObjectHead)+sizeof(struct flowConverterNum2DigiData)+(flowList.GetCount()*sizeof(struct flowConnectionData));
   length=htonl(length);
   FHandle->Write(&length,4);

   convHead.version=htonl(1);
   if (isCompiled) convHead.size=htonl(sizeof(struct flowConverterNum2DigiData)+(flowList.GetCount()*FLOW_CONNECTION_HEAD_SIZE));
   else convHead.size=htonl(sizeof(struct flowConverterNum2DigiData)+(flowList.GetCount()*sizeof(struct flowConnectionData)));
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
   for (i=0; i<CONVERTER_MAX_OUTPUTS; i++)
   {
      saveData.outData[i].flags        =htonl(convData.outData[i].flags);
      saveData.outData[i].enablePattern=htonl(convData.outData[i].enablePattern);
      for (j=0; j<CONVERTER_MAX_ASSIGNMENTS; j++)
      {
         saveData.outData[i].outOperation[j]=convData.outData[i].outOperation[j];
         saveData.outData[i].compareFlag[j]=convData.outData[i].compareFlag[j];
         saveData.outData[i].mInNum[j]=htonl(convData.outData[i].mInNum[j]);
      }
   }
   len+=FHandle->Write(&saveData,sizeof(struct flowConverterNum2DigiData));

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



wxUint64 flowConverterNum2Digi::getAssignedOutput(wxUint64 input)
{
   if ((input & OAPC_DIGI_IO0) && (digi[0]==1))
   {
      wxUint32 outFlags=OAPC_DIGI_IO0;

      digi[0]=1;
      if (convData.outData[1].flags & CONVERTER_FLAGS_USE_CLOCK) outFlags|=OAPC_DIGI_IO1;
      if (convData.outData[2].flags & CONVERTER_FLAGS_USE_CLOCK) outFlags|=OAPC_DIGI_IO2;
      if (convData.outData[3].flags & CONVERTER_FLAGS_USE_CLOCK) outFlags|=OAPC_DIGI_IO3;
      if (convData.outData[4].flags & CONVERTER_FLAGS_USE_CLOCK) outFlags|=OAPC_DIGI_IO4;
      if (convData.outData[5].flags & CONVERTER_FLAGS_USE_CLOCK) outFlags|=OAPC_DIGI_IO5;
      if (convData.outData[6].flags & CONVERTER_FLAGS_USE_CLOCK) outFlags|=OAPC_DIGI_IO6;
      if (convData.outData[7].flags & CONVERTER_FLAGS_USE_CLOCK) outFlags|=OAPC_DIGI_IO7;

      return outFlags;
   }
   else if ((input==OAPC_NUM_IO1) && !(convData.outData[1].flags & CONVERTER_FLAGS_USE_CLOCK)) return OAPC_DIGI_IO1;
   else if ((input==OAPC_NUM_IO2) && !(convData.outData[2].flags & CONVERTER_FLAGS_USE_CLOCK)) return OAPC_DIGI_IO2;
   else if ((input==OAPC_NUM_IO3) && !(convData.outData[3].flags & CONVERTER_FLAGS_USE_CLOCK)) return OAPC_DIGI_IO3;
   else if ((input==OAPC_NUM_IO4) && !(convData.outData[4].flags & CONVERTER_FLAGS_USE_CLOCK)) return OAPC_DIGI_IO4;
   else if ((input==OAPC_NUM_IO5) && !(convData.outData[5].flags & CONVERTER_FLAGS_USE_CLOCK)) return OAPC_DIGI_IO5;
   else if ((input==OAPC_NUM_IO6) && !(convData.outData[6].flags & CONVERTER_FLAGS_USE_CLOCK)) return OAPC_DIGI_IO6;
   else if ((input==OAPC_NUM_IO7) && !(convData.outData[7].flags & CONVERTER_FLAGS_USE_CLOCK)) return OAPC_DIGI_IO7;
   return 0;
}



/**
Sets an numerical input and checks if the same thread has set that input before
@param[in] input the flag that specifies the input
@param[in] value the value that has to be set for that input
@param[in] flowThreadID the Id of the thread that wants to set this value
@return 0 if the same thread already has set a value; the thread has to finish
        in this case to avoid endless loops; 1 if the input value was set correctly
*/
wxByte flowConverterNum2Digi::setNumInput(FlowConnection *connection,wxFloat64 value,wxUint32 *flowThreadID,flowObject *WXUNUSED(object))
{
   if (!threadIDOK(flowThreadID,false)) return 0;
   num[connection->targetInputNum]=value;

   return 1;
}



wxByte flowConverterNum2Digi::getDigitalOutput(FlowConnection *connection,wxInt32 *rcode,wxUint64 WXUNUSED(lastInput),wxLongLong origCreationTime)
{
#ifdef ENV_DEBUGGER
   if ((connection->sourceOutputNum<0) || (connection->sourceOutputNum>=MAX_NUM_IOS))
   {
      g_debugWin->setDebugInformation(this,DEBUG_STOP_COND_ILLEGAL_IO,_T(""));
      *rcode=OAPC_ERROR_NO_SUCH_IO;
      return 0;
   }
#endif
   // mode CONVERTER_NUM2DIGI_FLAGS_DIRECT_MODE
   wxUint32 bit=1,i;

   for (i=0; i<CONVERTER_MAX_ASSIGNMENTS; i++)
   {
      if ((convData.outData[connection->sourceOutputNum].enablePattern & bit)==bit)
      {
         if (
             ((convData.outData[connection->sourceOutputNum].mInNum[i]/1000.0==num[connection->sourceOutputNum]) && (convData.outData[connection->sourceOutputNum].compareFlag[i]==CONVERTER_FLAGS_CMP_EQUAL)) ||
             ((convData.outData[connection->sourceOutputNum].mInNum[i]/1000.0!=num[connection->sourceOutputNum]) && (convData.outData[connection->sourceOutputNum].compareFlag[i]==CONVERTER_FLAGS_CMP_NOTEQUAL)) ||
             ((convData.outData[connection->sourceOutputNum].mInNum[i]/1000.0<num[connection->sourceOutputNum]) && (convData.outData[connection->sourceOutputNum].compareFlag[i]==CONVERTER_FLAGS_CMP_GREATER)) || // output num[output] must be greater than internal compare value
             ((convData.outData[connection->sourceOutputNum].mInNum[i]/1000.0>num[connection->sourceOutputNum]) && (convData.outData[connection->sourceOutputNum].compareFlag[i]==CONVERTER_FLAGS_CMP_SMALLER)))   // output num[connection->sourceOutputNum] must be smaller than internal compare value
         {
            *rcode=OAPC_OK;
            if (convData.outData[connection->sourceOutputNum].outOperation[i]==CONVERTER_FLAGS_OUTPUT_LOW) return 0;
            else if (convData.outData[connection->sourceOutputNum].outOperation[i]==CONVERTER_FLAGS_OUTPUT_HIGH) return 1;
            else if (convData.outData[connection->sourceOutputNum].outOperation[i]==CONVERTER_FLAGS_OUTPUT_PULSE_LOHI)
            {
               FlowWalkerThread *dataThread=new FlowWalkerThread(this,connection,FlowWalkerThread::THREAD_MODE_DIGI,origCreationTime,0);
               dataThread->setFirstOutValue((wxByte)0,this);
               g_flowDispatcher->appendNewThread(dataThread);
               wxMilliSleep(100);
               *rcode|=OAPC_INT_FLOW_FORKEDi;
               return 1;
            }
            else if (convData.outData[connection->sourceOutputNum].outOperation[i]==CONVERTER_FLAGS_OUTPUT_PULSE_HILO)
            {
               FlowWalkerThread *dataThread=new FlowWalkerThread(this,connection,FlowWalkerThread::THREAD_MODE_DIGI,origCreationTime,0);
               dataThread->setFirstOutValue((wxByte)1,this);
               g_flowDispatcher->appendNewThread(dataThread);
               wxMilliSleep(100);
               *rcode|=OAPC_INT_FLOW_FORKEDi;
               return 0;
            }
         }
      }
      bit=bit<<1;
   }
   // start new threads to handle the overflow data
   g_flowPool.startOverflowFlows(this,FLOW_TYPE_FLAG_NUM_OVERFLOW_OUT,0,num[connection->sourceOutputNum],_T(""),oapcBinHeadSp(),origCreationTime);

   *rcode=OAPC_ERROR_NO_DATA_AVAILABLE|OAPC_INT_FLOW_FORKEDi;
   return 0;
}
#endif



wxInt32 flowConverterNum2Digi::loadDATA(wxFile *FHandle,wxUint32 chunkSize,wxUint32 IDOffset,bool isCompiled)
{
   struct hmiObjectHead              convHead;
   struct flowConverterNum2DigiData  loadData;
   wxInt32                           loaded,i,j;
   wxMBConvUTF16BE                   strConv;
   wchar_t                           buf[MAX_TEXT_LENGTH];

   if (!FHandle) return 0;
   if (chunkSize>sizeof(struct hmiObjectHead)) chunkSize=sizeof(struct hmiObjectHead);
   loaded=FHandle->Read(&convHead,sizeof(struct hmiObjectHead));

   convHead.size     =ntohl(convHead.size);
   convHead.version  =ntohl(convHead.version);
   convHead.reserved1=ntohl(convHead.reserved1);
   convHead.reserved2=ntohl(convHead.reserved2);

   loaded+=FHandle->Read(&loadData,sizeof(struct flowConverterNum2DigiData));

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
#endif
   for (i=0; i<CONVERTER_MAX_OUTPUTS; i++)
   {
      convData.outData[i].flags        =ntohl(loadData.outData[i].flags);
      convData.outData[i].enablePattern=ntohl(loadData.outData[i].enablePattern);
      for (j=0; j<CONVERTER_MAX_ASSIGNMENTS; j++)
      {
         convData.outData[i].outOperation[j]=loadData.outData[i].outOperation[j];
         convData.outData[i].compareFlag[j]=loadData.outData[i].compareFlag[j];
         convData.outData[i].mInNum[j]=ntohl(loadData.outData[i].mInNum[j]);
      }
   }
   loaded+=flowObject::loadFlow(FHandle,&convData.flowData,IDOffset,false,isCompiled);
   return loaded;
}
