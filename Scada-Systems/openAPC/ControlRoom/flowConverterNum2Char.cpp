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
#include "flowConverterNum2Char.h"
#ifdef ENV_EDITOR
#include "DlgConfigflowConvertNum2Char.h"
#endif
#include "oapc_libio.h"

#ifdef ENV_WINDOWS
 #ifdef ENV_WINDOWSCE
  #include "Winsock2.h"
 #endif
#endif



flowConverterNum2Char::flowConverterNum2Char():flowConverter()
{
   wxUint32 i;
#ifdef ENV_EDITOR
   wxUint32 j,bit=1;
#endif

   this->data.type=flowObject::FLOW_TYPE_CONV_NUM2CHAR;
   this->data.stdIN=OAPC_DIGI_IO0|OAPC_NUM_IO1|OAPC_NUM_IO2|OAPC_NUM_IO3|OAPC_NUM_IO4|OAPC_NUM_IO5|OAPC_NUM_IO6|OAPC_NUM_IO7;
   this->data.stdOUT=OAPC_DIGI_IO0|OAPC_CHAR_IO1|OAPC_CHAR_IO2|OAPC_CHAR_IO3|OAPC_CHAR_IO4|OAPC_CHAR_IO5|OAPC_CHAR_IO6|OAPC_CHAR_IO7|FLOW_TYPE_FLAG_NUM_OVERFLOW_OUT;
#ifdef ENV_PLAYER
   for (i=0; i<MAX_NUM_IOS; i++)
   {
      num[i]=0.0;
      txtD[i]=_T("0");
      txtF[i]=_T("0.000000");
   }
#endif
#ifdef ENV_EDITOR
   wxString val[CONVERTER_MAX_ASSIGNMENTS]={_T("A"),_T("B"),_T("C"),_T("D"),_T("E"),_T("F"),_T("G"),_T("H"),_T("I"),_T("J")};
   for (i=0; i<MAX_NUM_IOS; i++)
   {
      convData.outData[i].enablePattern=1;
      outFormat[i]=wxString::Format(_T("%%d%d %%f%d"),i,i);
      for (j=0; j<CONVERTER_MAX_ASSIGNMENTS; j++)
      {
         convData.outData[i].mInNum[j]=(j+1)*1000;
         convData.outData[i].compareFlag[j]=CONVERTER_FLAGS_CMP_EQUAL;
         outChar[i][j]=val[j];
      }
      if (i==1) convData.outData[i].flags=CONVERTER_FLAGS_BINARY_MODE|CONVERTER_FLAGS_USE_CLOCK;
      else convData.outData[i].flags=CONVERTER_FLAGS_DIRECT_MODE|CONVERTER_FLAGS_USE_CLOCK;
      bit=bit<<1;
   }
#endif
}



flowConverterNum2Char::~flowConverterNum2Char()
{
}



wxString flowConverterNum2Char::getDefaultName()
{
   return _T("Convert Num2Char");
}



#ifndef ENV_PLAYER
void flowConverterNum2Char::doDataFlowDialog(bool hideISConfig)
{
   DlgConfigflowConvertNum2Char dlg(this,(wxWindow*)g_hmiCanvas,_("Definition"),hideISConfig);

   dlg.ShowModal();
   if (dlg.returnOK)
   {
   }
   dlg.Destroy();
}



wxInt32 flowConverterNum2Char::saveDATA(wxFile *FHandle,char chunkName[4],bool isCompiled)
{
   wxInt32                            length;
   struct hmiObjectHead               convHead;
   struct flowConverterNum2CharData   saveData;
   wxInt32                            i,j,len;
   wxNode                            *node;
   FlowConnection                    *connection;
   wxMBConvUTF16BE                    strConv;


   if (!FHandle) return 0;
   checkConnections();
   FHandle->Write(chunkName,4);
   if (isCompiled) length=sizeof(struct hmiObjectHead)+sizeof(struct flowConverterNum2CharData)+(flowList.GetCount()*FLOW_CONNECTION_HEAD_SIZE);
   else length=sizeof(struct hmiObjectHead)+sizeof(struct flowConverterNum2CharData)+(flowList.GetCount()*sizeof(struct flowConnectionData));
   length=htonl(length);
   FHandle->Write(&length,4);

   convHead.version=htonl(1);
   if (isCompiled) convHead.size=htonl(sizeof(struct flowConverterNum2CharData)+(flowList.GetCount()*FLOW_CONNECTION_HEAD_SIZE));
   else convHead.size=htonl(sizeof(struct flowConverterNum2CharData)+(flowList.GetCount()*sizeof(struct flowConnectionData)));
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
      strConv.WC2MB(saveData.outData[i].store_outFormat,outFormat[i],sizeof(saveData.outData[i].store_outFormat));
      for (j=0; j<CONVERTER_MAX_ASSIGNMENTS; j++)
      {
         saveData.outData[i].mInNum[j]=htonl(convData.outData[i].mInNum[j]);
         saveData.outData[i].compareFlag[j]=convData.outData[i].compareFlag[j];
         strConv.WC2MB(saveData.outData[i].store_outChar[j],outChar[i][j],sizeof(saveData.outData[i].store_outChar[j]));
      }
   }
   len+=FHandle->Write(&saveData,sizeof(struct flowConverterNum2CharData));

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



wxUint64 flowConverterNum2Char::getAssignedOutput(wxUint64 input)
{
   if ((input & OAPC_DIGI_IO0) && (digi[0]==1))
   {
      wxUint32 outFlags=OAPC_DIGI_IO0;

      digi[0]=1;
      if (convData.outData[1].flags & CONVERTER_FLAGS_USE_CLOCK) outFlags|=OAPC_CHAR_IO1;
      if (convData.outData[2].flags & CONVERTER_FLAGS_USE_CLOCK) outFlags|=OAPC_CHAR_IO2;
      if (convData.outData[3].flags & CONVERTER_FLAGS_USE_CLOCK) outFlags|=OAPC_CHAR_IO3;
      if (convData.outData[4].flags & CONVERTER_FLAGS_USE_CLOCK) outFlags|=OAPC_CHAR_IO4;
      if (convData.outData[5].flags & CONVERTER_FLAGS_USE_CLOCK) outFlags|=OAPC_CHAR_IO5;
      if (convData.outData[6].flags & CONVERTER_FLAGS_USE_CLOCK) outFlags|=OAPC_CHAR_IO6;
      if (convData.outData[7].flags & CONVERTER_FLAGS_USE_CLOCK) outFlags|=OAPC_CHAR_IO7;

      return outFlags;
   }
   else if ((input==OAPC_NUM_IO1) && !(convData.outData[1].flags & CONVERTER_FLAGS_USE_CLOCK)) return OAPC_CHAR_IO1;
   else if ((input==OAPC_NUM_IO2) && !(convData.outData[2].flags & CONVERTER_FLAGS_USE_CLOCK)) return OAPC_CHAR_IO2;
   else if ((input==OAPC_NUM_IO3) && !(convData.outData[3].flags & CONVERTER_FLAGS_USE_CLOCK)) return OAPC_CHAR_IO3;
   else if ((input==OAPC_NUM_IO4) && !(convData.outData[4].flags & CONVERTER_FLAGS_USE_CLOCK)) return OAPC_CHAR_IO4;
   else if ((input==OAPC_NUM_IO5) && !(convData.outData[5].flags & CONVERTER_FLAGS_USE_CLOCK)) return OAPC_CHAR_IO5;
   else if ((input==OAPC_NUM_IO6) && !(convData.outData[6].flags & CONVERTER_FLAGS_USE_CLOCK)) return OAPC_CHAR_IO6;
   else if ((input==OAPC_NUM_IO7) && !(convData.outData[7].flags & CONVERTER_FLAGS_USE_CLOCK)) return OAPC_CHAR_IO7;
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
wxByte flowConverterNum2Char::setNumInput(FlowConnection *connection,wxFloat64 value,wxUint32 *flowThreadID,flowObject *WXUNUSED(object))
{
   if (!threadIDOK(flowThreadID,false)) return 0;
   num[connection->targetInputNum]=value;
   txtD[connection->targetInputNum]=wxString::Format(_T("%d"),(wxInt32)num[connection->targetInputNum]);
   txtF[connection->targetInputNum]=wxString::Format(_T("%f"),num[connection->targetInputNum]);
   return 1;
}



wxString flowConverterNum2Char::getCharOutput(FlowConnection *connection,wxInt32 *rcode,wxUint64 WXUNUSED(lastInput))
{
#ifdef ENV_DEBUGGER
   if ((connection->sourceOutputNum<0) || (connection->sourceOutputNum>=MAX_NUM_IOS))
   {
      g_debugWin->setDebugInformation(this,DEBUG_STOP_COND_ILLEGAL_IO,_T(""));
      *rcode=OAPC_ERROR_NO_SUCH_IO;
      return _T("");
   }
#endif
   if ((convData.outData[connection->sourceOutputNum].flags & CONVERTER_FLAGS_OUTPUT_MASK)==CONVERTER_FLAGS_BINARY_MODE)
   {
      *rcode=OAPC_OK;
      return wxString::Format(_T("%d"),(wxInt32)num[connection->sourceOutputNum]);
   }
   else if ((convData.outData[connection->sourceOutputNum].flags & CONVERTER_FLAGS_OUTPUT_MASK)==CONVERTER_FLAGS_FORMAT_MODE)
   {
      wxString retValue=outFormat[connection->sourceOutputNum];

      retValue.Replace(_T("%d0"),txtD[0]); retValue.Replace(_T("%f0"),txtF[0]);
      retValue.Replace(_T("%d1"),txtD[1]); retValue.Replace(_T("%f1"),txtF[1]);
      retValue.Replace(_T("%d2"),txtD[2]); retValue.Replace(_T("%f2"),txtF[2]);
      retValue.Replace(_T("%d3"),txtD[3]); retValue.Replace(_T("%f3"),txtF[3]);
      retValue.Replace(_T("%d4"),txtD[4]); retValue.Replace(_T("%f4"),txtF[4]);
      retValue.Replace(_T("%d5"),txtD[5]); retValue.Replace(_T("%f5"),txtF[5]);
      retValue.Replace(_T("%d6"),txtD[6]); retValue.Replace(_T("%f6"),txtF[6]);
      retValue.Replace(_T("%d7"),txtD[7]); retValue.Replace(_T("%f7"),txtF[7]);
      *rcode=OAPC_OK;
      return retValue;
   }
   else if ((convData.outData[connection->sourceOutputNum].flags & CONVERTER_FLAGS_OUTPUT_MASK)==CONVERTER_FLAGS_DIRECT_MODE)
   {
      wxInt32 i;
      wxUint32 bit=1;

      for (i=0; i<CONVERTER_MAX_ASSIGNMENTS; i++)
      {
         if ((convData.outData[connection->sourceOutputNum].enablePattern & bit)==bit)
         {
            if (
                ((convData.outData[connection->sourceOutputNum].mInNum[i]/1000.0==num[connection->sourceOutputNum]) && (convData.outData[connection->sourceOutputNum].compareFlag[i]==CONVERTER_FLAGS_CMP_EQUAL)) ||
                ((convData.outData[connection->sourceOutputNum].mInNum[i]/1000.0!=num[connection->sourceOutputNum]) && (convData.outData[connection->sourceOutputNum].compareFlag[i]==CONVERTER_FLAGS_CMP_NOTEQUAL)) ||
                ((convData.outData[connection->sourceOutputNum].mInNum[i]/1000.0>num[connection->sourceOutputNum]) && (convData.outData[connection->sourceOutputNum].compareFlag[i]==CONVERTER_FLAGS_CMP_GREATER)) ||
                ((convData.outData[connection->sourceOutputNum].mInNum[i]/1000.0<num[connection->sourceOutputNum]) && (convData.outData[connection->sourceOutputNum].compareFlag[i]==CONVERTER_FLAGS_CMP_SMALLER)))
            {
               *rcode=OAPC_OK;
               return outChar[connection->sourceOutputNum][i];
            }
         }
         bit=bit<<1;
      }
   }
   else wxASSERT(0);
   // start new threads to handle the overflow data
   g_flowPool.startOverflowFlows(this,FLOW_TYPE_FLAG_NUM_OVERFLOW_OUT,0,num[connection->sourceOutputNum],_T(""),oapcBinHeadSp(),wxGetLocalTimeMillis()); // TODO: fetch creation time from original thread

   *rcode=OAPC_ERROR_NO_DATA_AVAILABLE|OAPC_INT_FLOW_FORKEDi;
   return _T("");
}
#endif



wxInt32 flowConverterNum2Char::loadDATA(wxFile *FHandle,wxUint32 chunkSize,wxUint32 IDOffset,bool isCompiled)
{
   struct hmiObjectHead              convHead;
   struct flowConverterNum2CharData  loadData;
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

   loaded+=FHandle->Read(&loadData,sizeof(struct flowConverterNum2CharData));

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
      convData.outData[i].flags        =ntohl(loadData.outData[i].flags);
      convData.outData[i].enablePattern=ntohl(loadData.outData[i].enablePattern);
      strConv.MB2WC(buf,loadData.outData[i].store_outFormat,sizeof(buf));
      outFormat[i]=buf;
      for (j=0; j<CONVERTER_MAX_ASSIGNMENTS; j++)
      {
         convData.outData[i].compareFlag[j]=loadData.outData[i].compareFlag[j];
         convData.outData[i].mInNum[j]=ntohl(loadData.outData[i].mInNum[j]);
         strConv.MB2WC(buf,loadData.outData[i].store_outChar[j],sizeof(buf));
         outChar[i][j]=buf;
      }
   }
   loaded+=flowObject::loadFlow(FHandle,&convData.flowData,IDOffset,false,isCompiled);
   return loaded;
}



