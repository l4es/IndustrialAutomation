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
#include "flowConverterChar2Digi.h"
#ifdef ENV_EDITOR
 #include "DlgConfigflowConvertChar2Digi.h"
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


flowConverterChar2Digi::flowConverterChar2Digi():flowConverter()
{
#ifdef ENV_EDITOR
   wxUint32 i,j,bit=1;
#endif

   this->data.type=flowObject::FLOW_TYPE_CONV_CHAR2DIGI;
   this->data.stdIN=OAPC_DIGI_IO0|OAPC_CHAR_IO1|OAPC_CHAR_IO2|OAPC_CHAR_IO3|OAPC_CHAR_IO4|OAPC_CHAR_IO5|OAPC_CHAR_IO6|OAPC_CHAR_IO7;
   this->data.stdOUT=OAPC_DIGI_IO0|OAPC_DIGI_IO1|OAPC_DIGI_IO2|OAPC_DIGI_IO3|OAPC_DIGI_IO4|OAPC_DIGI_IO5|OAPC_DIGI_IO6|OAPC_DIGI_IO7|FLOW_TYPE_FLAG_CHAR_OVERFLOW_OUT;
#ifdef ENV_EDITOR
   wxString sval[CONVERTER_MAX_ASSIGNMENTS]={_T("A"),_T("B"),_T("C"),_T("D"),_T("E"),_T("F"),_T("G"),_T("H"),_T("I"),_T("J")};
   for (i=0; i<MAX_NUM_IOS; i++)
   {
      if (i==1) convData.outData[i].enablePattern=1;
      else convData.outData[i].enablePattern=0;
      convData.outData[i].casePattern=0;
      for (j=0; j<CONVERTER_MAX_ASSIGNMENTS; j++)
      {
         convData.outData[i].outOperation[j]=CONVERTER_FLAGS_OUTPUT_HIGH; // only outOperation[0] manages the use_clock flag
         convData.outData[i].compareFlag[j]=CONVERTER_FLAGS_CMP_EQUAL;
         inChar[i][j]=sval[j];
      }
      bit=bit<<1;
   }
#endif
}



flowConverterChar2Digi::~flowConverterChar2Digi()
{
}



wxString flowConverterChar2Digi::getDefaultName()
{
   return _T("Convert Char2Digi");
}



#ifndef ENV_PLAYER
void flowConverterChar2Digi::doDataFlowDialog(bool hideISConfig)
{
   DlgConfigflowConvertChar2Digi dlg(this,(wxWindow*)g_hmiCanvas,_("Definition"),hideISConfig);

   dlg.ShowModal();
   if (dlg.returnOK)
   {
   }
   dlg.Destroy();
}



wxInt32 flowConverterChar2Digi::saveDATA(wxFile *FHandle,char chunkName[4],bool isCompiled)
{
   wxInt32                            length;
   struct hmiObjectHead               convHead;
   struct flowConverterChar2DigiData  saveData;
   wxInt32                            i,j,len;
   wxNode                            *node;
   FlowConnection                    *connection;
   wxMBConvUTF16BE                    strConv;

   if (!FHandle) return 0;
   checkConnections();
   FHandle->Write(chunkName,4);
   if (isCompiled) length=sizeof(struct hmiObjectHead)+sizeof(struct flowConverterChar2DigiData)+(flowList.GetCount()*FLOW_CONNECTION_HEAD_SIZE);
   else length=sizeof(struct hmiObjectHead)+sizeof(struct flowConverterChar2DigiData)+(flowList.GetCount()*sizeof(struct flowConnectionData));
   length=htonl(length);
   FHandle->Write(&length,4);

   convHead.version=htonl(1);
   if (isCompiled) convHead.size=htonl(sizeof(struct flowConverterChar2DigiData)+(flowList.GetCount()*FLOW_CONNECTION_HEAD_SIZE));
   else convHead.size=htonl(sizeof(struct flowConverterChar2DigiData)+(flowList.GetCount()*sizeof(struct flowConnectionData)));
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
      saveData.outData[i].enablePattern=htonl(convData.outData[i].enablePattern);
      saveData.outData[i].casePattern  =htonl(convData.outData[i].casePattern);
      for (j=0; j<CONVERTER_MAX_ASSIGNMENTS; j++)
      {
         saveData.outData[i].compareFlag[j]=convData.outData[i].compareFlag[j];
         saveData.outData[i].outOperation[j]=htonl(convData.outData[i].outOperation[j]);
         strConv.WC2MB(saveData.outData[i].store_inChar[j],inChar[i][j], sizeof(saveData.outData[i].store_inChar[j]));
      }
   }
   len+=FHandle->Write(&saveData,sizeof(struct flowConverterChar2DigiData));

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



wxUint64 flowConverterChar2Digi::getAssignedOutput(wxUint64 input)
{
   if ((input & OAPC_DIGI_IO0) && (digi[0]==1))
   {
      wxUint32 outFlags=OAPC_DIGI_IO0;

      digi[0]=1;
      if ((txt[1].Length()>0) && (convData.outData[1].outOperation[0] & CONVERTER_FLAGS_USE_CLOCK)) outFlags|=OAPC_DIGI_IO1;
      if ((txt[1].Length()>0) && (convData.outData[2].outOperation[0] & CONVERTER_FLAGS_USE_CLOCK)) outFlags|=OAPC_DIGI_IO2;
      if ((txt[1].Length()>0) && (convData.outData[3].outOperation[0] & CONVERTER_FLAGS_USE_CLOCK)) outFlags|=OAPC_DIGI_IO3;
      if ((txt[1].Length()>0) && (convData.outData[4].outOperation[0] & CONVERTER_FLAGS_USE_CLOCK)) outFlags|=OAPC_DIGI_IO4;
      if ((txt[1].Length()>0) && (convData.outData[5].outOperation[0] & CONVERTER_FLAGS_USE_CLOCK)) outFlags|=OAPC_DIGI_IO5;
      if ((txt[1].Length()>0) && (convData.outData[6].outOperation[0] & CONVERTER_FLAGS_USE_CLOCK)) outFlags|=OAPC_DIGI_IO6;
      if ((txt[1].Length()>0) && (convData.outData[7].outOperation[0] & CONVERTER_FLAGS_USE_CLOCK)) outFlags|=OAPC_DIGI_IO7;

      return outFlags;
   }
   else if ((input==OAPC_CHAR_IO1) && (txt[1].Length()>0) && !(convData.outData[1].outOperation[0] & CONVERTER_FLAGS_USE_CLOCK))
    return OAPC_DIGI_IO1;
   else if ((input==OAPC_CHAR_IO2) && (txt[2].Length()>0) && !(convData.outData[2].outOperation[0] & CONVERTER_FLAGS_USE_CLOCK))
    return OAPC_DIGI_IO2;
   else if ((input==OAPC_CHAR_IO3) && (txt[3].Length()>0) && !(convData.outData[3].outOperation[0] & CONVERTER_FLAGS_USE_CLOCK))
    return OAPC_DIGI_IO3;
   else if ((input==OAPC_CHAR_IO4) && (txt[4].Length()>0) && !(convData.outData[4].outOperation[0] & CONVERTER_FLAGS_USE_CLOCK))
    return OAPC_DIGI_IO4;
   else if ((input==OAPC_CHAR_IO5) && (txt[5].Length()>0) && !(convData.outData[5].outOperation[0] & CONVERTER_FLAGS_USE_CLOCK))
    return OAPC_DIGI_IO5;
   else if ((input==OAPC_CHAR_IO6) && (txt[6].Length()>0) && !(convData.outData[6].outOperation[0] & CONVERTER_FLAGS_USE_CLOCK))
    return OAPC_DIGI_IO6;
   else if ((input==OAPC_CHAR_IO7) && (txt[7].Length()>0) && !(convData.outData[7].outOperation[0] & CONVERTER_FLAGS_USE_CLOCK))
    return OAPC_DIGI_IO7;
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
wxByte flowConverterChar2Digi::setCharInput(FlowConnection *connection,wxString value,wxUint32 *flowThreadID,flowObject *WXUNUSED(object))
{
   if (!threadIDOK(flowThreadID,false)) return 0;
   txt[connection->targetInputNum]=value;

   return 1;
}



wxByte flowConverterChar2Digi::getDigitalOutput(FlowConnection *connection,wxInt32 *rcode,wxUint64 WXUNUSED(lastInput),wxLongLong origCreationTime)
{
#ifdef ENV_DEBUGGER
   if ((connection->sourceOutputNum<0) || (connection->sourceOutputNum>=MAX_NUM_IOS))
   {
      g_debugWin->setDebugInformation(this,DEBUG_STOP_COND_ILLEGAL_IO,_T(""));
      *rcode=OAPC_ERROR_NO_SUCH_IO;
      return 0;
   }
#endif
   wxUint32 bit=1,i;

   for (i=0; i<CONVERTER_MAX_ASSIGNMENTS; i++)
   {
      if ((convData.outData[connection->sourceOutputNum].enablePattern & bit)==bit)
      {
         if ((convData.outData[connection->sourceOutputNum].casePattern & bit)==bit)
         {
            if (
                ((inChar[connection->sourceOutputNum][i].Cmp(txt[connection->sourceOutputNum])==0) && (convData.outData[connection->sourceOutputNum].compareFlag[i]==CONVERTER_FLAGS_CMP_EQUAL)) ||
                ((inChar[connection->sourceOutputNum][i].Cmp(txt[connection->sourceOutputNum])!=0) && (convData.outData[connection->sourceOutputNum].compareFlag[i]==CONVERTER_FLAGS_CMP_NOTEQUAL)))
                                                                                                                                    
            {
               *rcode=OAPC_OK;
               if ((convData.outData[connection->sourceOutputNum].outOperation[i] & CONVERTER_FLAGS_OUTPUT_MASK)==CONVERTER_FLAGS_OUTPUT_LOW) return 0;
               else if ((convData.outData[connection->sourceOutputNum].outOperation[i] & CONVERTER_FLAGS_OUTPUT_MASK)==CONVERTER_FLAGS_OUTPUT_HIGH) return 1;
               else if ((convData.outData[connection->sourceOutputNum].outOperation[i] & CONVERTER_FLAGS_OUTPUT_MASK)==CONVERTER_FLAGS_OUTPUT_PULSE_LOHI)
               {
                  FlowWalkerThread *dataThread=new FlowWalkerThread(this,connection,FlowWalkerThread::THREAD_MODE_DIGI,origCreationTime,0);
                  dataThread->setFirstOutValue((wxByte)0,this);
                  g_flowDispatcher->appendNewThread(dataThread);
                  wxMilliSleep(100);
                  *rcode|=OAPC_INT_FLOW_FORKEDi;
                  return 1;
               }
               else if ((convData.outData[connection->sourceOutputNum].outOperation[i] & CONVERTER_FLAGS_OUTPUT_MASK)==CONVERTER_FLAGS_OUTPUT_PULSE_HILO)
               {
                  FlowWalkerThread *dataThread=new FlowWalkerThread(this,connection,FlowWalkerThread::THREAD_MODE_DIGI,origCreationTime,0);
                  dataThread->setFirstOutValue((wxByte)1,this);
                  g_flowDispatcher->appendNewThread(dataThread);
                  wxMilliSleep(100);
                  *rcode|=OAPC_INT_FLOW_FORKEDi;
                  return 0;
               }
               else wxASSERT(0);
            }
         }
         else
         {
            if (
                ((inChar[connection->sourceOutputNum][i].CmpNoCase(txt[connection->sourceOutputNum])==0) && (convData.outData[connection->sourceOutputNum].compareFlag[i]==CONVERTER_FLAGS_CMP_EQUAL)) ||
                ((inChar[connection->sourceOutputNum][i].CmpNoCase(txt[connection->sourceOutputNum])!=0) && (convData.outData[connection->sourceOutputNum].compareFlag[i]==CONVERTER_FLAGS_CMP_NOTEQUAL)))
                // compare case insensitive
            {
               *rcode=OAPC_OK;
               if ((convData.outData[connection->sourceOutputNum].outOperation[i] & CONVERTER_FLAGS_OUTPUT_MASK)==CONVERTER_FLAGS_OUTPUT_LOW)
                return 0;
               else if ((convData.outData[connection->sourceOutputNum].outOperation[i] & CONVERTER_FLAGS_OUTPUT_MASK)==CONVERTER_FLAGS_OUTPUT_HIGH)
                return 1;
               else if ((convData.outData[connection->sourceOutputNum].outOperation[i] & CONVERTER_FLAGS_OUTPUT_MASK)==CONVERTER_FLAGS_OUTPUT_PULSE_LOHI)
               {
                  FlowWalkerThread *dataThread=new FlowWalkerThread(this,connection,FlowWalkerThread::THREAD_MODE_DIGI,origCreationTime,0);
                  dataThread->setFirstOutValue((wxByte)0,this);
                  g_flowDispatcher->appendNewThread(dataThread);
                  wxMilliSleep(100);
                  *rcode|=OAPC_INT_FLOW_FORKEDi;
                  return 1;
               }
               else if ((convData.outData[connection->sourceOutputNum].outOperation[i] & CONVERTER_FLAGS_OUTPUT_MASK)==CONVERTER_FLAGS_OUTPUT_PULSE_HILO)
               {
                  FlowWalkerThread *dataThread=new FlowWalkerThread(this,connection,FlowWalkerThread::THREAD_MODE_DIGI,origCreationTime,0);
                  dataThread->setFirstOutValue((wxByte)1,this);
                  g_flowDispatcher->appendNewThread(dataThread);
                  wxMilliSleep(100);
                  *rcode|=OAPC_INT_FLOW_FORKEDi;
                  return 0;
               }
// ???               else wxASSERT(0);
            }
         }
      }
      bit=bit<<1;
   }
   // start new threads to handle the overflow data and the overflow clock
   g_flowPool.startOverflowFlows(this,FLOW_TYPE_FLAG_CHAR_OVERFLOW_OUT,0,0.0,txt[connection->sourceOutputNum],oapcBinHeadSp(),origCreationTime);

   *rcode=OAPC_ERROR_NO_DATA_AVAILABLE|OAPC_INT_FLOW_FORKEDi;
   return 0;
}
#endif



wxInt32 flowConverterChar2Digi::loadDATA(wxFile *FHandle,wxUint32 chunkSize,wxUint32 IDOffset,bool isCompiled)
{
   struct hmiObjectHead              convHead;
   struct flowConverterChar2DigiData loadData;
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

   loaded+=FHandle->Read(&loadData,sizeof(struct flowConverterChar2DigiData));

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
   for (i=0; i<CONVERTER_MAX_OUTPUTS; i++)
   {
      convData.outData[i].enablePattern=ntohl(loadData.outData[i].enablePattern);
      convData.outData[i].casePattern  =ntohl(loadData.outData[i].casePattern);
      for (j=0; j<CONVERTER_MAX_ASSIGNMENTS; j++)
      {
         convData.outData[i].compareFlag[j]=loadData.outData[i].compareFlag[j];
         convData.outData[i].outOperation[j]=ntohl(loadData.outData[i].outOperation[j]);
         strConv.MB2WC(buf,loadData.outData[i].store_inChar[j],sizeof(buf));
         inChar[i][j]=buf;
      }
   }
   loaded+=flowObject::loadFlow(FHandle,&convData.flowData,IDOffset,false,isCompiled);
   return loaded;
}
