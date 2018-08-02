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
#include "flowConverterDigi2Char.h"
#ifdef ENV_EDITOR
#include "DlgConfigflowConvertDigi2Char.h"
#endif
#include "oapc_libio.h"

#ifdef ENV_WINDOWS
 #ifdef ENV_WINDOWSCE
  #include "Winsock2.h"
 #endif
#endif



flowConverterDigi2Char::flowConverterDigi2Char():flowConverter()
{
#ifdef ENV_EDITOR
   wxUint32 i,j,bit=1;
#endif

   this->data.type=flowObject::FLOW_TYPE_CONV_DIGI2CHAR;
   this->data.stdIN=OAPC_DIGI_IO0|OAPC_DIGI_IO1|OAPC_DIGI_IO2|OAPC_DIGI_IO3|OAPC_DIGI_IO4|OAPC_DIGI_IO5|OAPC_DIGI_IO6|OAPC_DIGI_IO7;
   this->data.stdOUT=OAPC_CHAR_IO1|OAPC_CHAR_IO2|OAPC_CHAR_IO3|OAPC_CHAR_IO4|OAPC_CHAR_IO5|OAPC_CHAR_IO6|OAPC_CHAR_IO7;
#ifdef ENV_EDITOR
   wxString low[CONVERTER_MAX_INPUTS] ={_T("a"),_T("b"),_T("c"),_T("d"),_T("e"),_T("f"),_T("g"),_T("h")};
   wxString high[CONVERTER_MAX_INPUTS]={_T("A"),_T("B"),_T("C"),_T("D"),_T("E"),_T("F"),_T("G"),_T("H")};

   for (i=0; i<CONVERTER_MAX_OUTPUTS; i++)
   {
      convData.outData[i].inputMask=bit;
      convData.outData[i].flags=CONVERTER_FLAGS_DIRECT_MODE;
      for (j=0; j<CONVERTER_MAX_INPUTS; j++)
      {
         outValueLow[i][j]=low[j];
         outValueHigh[i][j]=high[j];
      }
      if (i==1)
      {
         convData.outData[i].inputMask=0x0F;
         convData.outData[i].flags=CONVERTER_FLAGS_BINARY_MODE;
      }
      bit=bit<<1;
   }
#endif
}



flowConverterDigi2Char::~flowConverterDigi2Char()
{
}



wxString flowConverterDigi2Char::getDefaultName()
{
   return _T("Convert Digi2Char");
}



#ifndef ENV_PLAYER
void flowConverterDigi2Char::doDataFlowDialog(bool hideISConfig)
{
   DlgConfigflowConvertDigi2Char dlg(this,(wxWindow*)g_hmiCanvas,_("Definition"),hideISConfig);

   dlg.ShowModal();
   if (dlg.returnOK)
   {
   }
   dlg.Destroy();
}



wxInt32 flowConverterDigi2Char::saveDATA(wxFile *FHandle,char chunkName[4],bool isCompiled)
{
   wxInt32                           length;
   struct hmiObjectHead              convHead;
   struct flowConverterDigi2CharData saveData;
   wxInt32                           i,j,len;
   wxNode                           *node;
   FlowConnection                   *connection;
   wxMBConvUTF16BE                   strConv;


   if (!FHandle) return 0;
   checkConnections();
   FHandle->Write(chunkName,4);
   if (isCompiled) length=sizeof(struct hmiObjectHead)+sizeof(struct flowConverterDigi2CharData)+(flowList.GetCount()*FLOW_CONNECTION_HEAD_SIZE);
   else length=sizeof(struct hmiObjectHead)+sizeof(struct flowConverterDigi2CharData)+(flowList.GetCount()*sizeof(struct flowConnectionData));
   length=htonl(length);
   FHandle->Write(&length,4);

   convHead.version=htonl(1);
   if (isCompiled) convHead.size=htonl(sizeof(struct flowConverterDigi2CharData)+(flowList.GetCount()*FLOW_CONNECTION_HEAD_SIZE));
   else convHead.size=htonl(sizeof(struct flowConverterDigi2CharData)+(flowList.GetCount()*sizeof(struct flowConnectionData)));
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
      saveData.outData[i].inputMask=convData.outData[i].inputMask; // it is a byte
      saveData.outData[i].flags    =htonl(convData.outData[i].flags);
      for (j=0; j<CONVERTER_MAX_INPUTS; j++)
      {
         strConv.WC2MB(saveData.outData[i].store_outValueLow[j], outValueLow[i][j], sizeof(saveData.outData[i].store_outValueLow[j]));
         strConv.WC2MB(saveData.outData[i].store_outValueHigh[j],outValueHigh[i][j],sizeof(saveData.outData[i].store_outValueHigh[j]));
      }
   }
   len+=FHandle->Write(&saveData,sizeof(struct flowConverterDigi2CharData));

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



wxUint64 flowConverterDigi2Char::getAssignedOutput(wxUint64 input)
{
   if ((input & OAPC_DIGI_IO0) && (digi[0]==1) &&
       ((convData.outData[1].flags & CONVERTER_FLAGS_USE_CLOCK) ||
        (convData.outData[2].flags & CONVERTER_FLAGS_USE_CLOCK) ||
        (convData.outData[3].flags & CONVERTER_FLAGS_USE_CLOCK) ||
        (convData.outData[4].flags & CONVERTER_FLAGS_USE_CLOCK) ||
        (convData.outData[5].flags & CONVERTER_FLAGS_USE_CLOCK) ||
        (convData.outData[6].flags & CONVERTER_FLAGS_USE_CLOCK) ||
        (convData.outData[7].flags & CONVERTER_FLAGS_USE_CLOCK)))
   {
      wxInt32  i,j,bit;
      wxUint32 outFlags=0;

      for (i=1; i<MAX_NUM_IOS; i++)
      {
         bit=2;
         for (j=1; j<MAX_NUM_IOS; j++)
         {
            if ((convData.outData[i].inputMask & bit) && (convData.outData[i].flags & CONVERTER_FLAGS_USE_CLOCK))
            {
               if (i==1) outFlags|=OAPC_CHAR_IO1;
               else if (i==2) outFlags|=OAPC_CHAR_IO2;
               else if (i==3) outFlags|=OAPC_CHAR_IO3;
               else if (i==4) outFlags|=OAPC_CHAR_IO4;
               else if (i==5) outFlags|=OAPC_CHAR_IO5;
               else if (i==6) outFlags|=OAPC_CHAR_IO6;
               else if (i==7) outFlags|=OAPC_CHAR_IO7;
            }
            bit=bit<<1;
         }
      }
      return outFlags;
   }
   else if ((input==OAPC_DIGI_IO0) || (input==OAPC_DIGI_IO1) || (input==OAPC_DIGI_IO2) || (input==OAPC_DIGI_IO3) ||
            (input==OAPC_DIGI_IO4) || (input==OAPC_DIGI_IO5) || (input==OAPC_DIGI_IO6) || (input==OAPC_DIGI_IO7))
   {
      wxInt32  i;
      wxUint32 outFlags=0;

      for (i=1; i<MAX_NUM_IOS; i++)
      {
         if ((convData.outData[i].inputMask & input) && !(convData.outData[i].flags & CONVERTER_FLAGS_USE_CLOCK))
         {
            if (i==1) outFlags|=OAPC_CHAR_IO1;
            else if (i==2) outFlags|=OAPC_CHAR_IO2;
            else if (i==3) outFlags|=OAPC_CHAR_IO3;
            else if (i==4) outFlags|=OAPC_CHAR_IO4;
            else if (i==5) outFlags|=OAPC_CHAR_IO5;
            else if (i==6) outFlags|=OAPC_CHAR_IO6;
            else if (i==7) outFlags|=OAPC_CHAR_IO7;
         }
      }
      return outFlags;
   }
   return 0;
}



wxString flowConverterDigi2Char::byteToBinary(wxByte x)
{
   wxString out;

   int z;

   for (z =0x80; z > 0; z >>= 1)
   {
      if ((x & z) == z) out << _T("1");
      else out << _T("0");
   }

   return out;
}



wxString flowConverterDigi2Char::getCharOutput(FlowConnection *connection,wxInt32 *rcode,wxUint64 lastInput)
{
   wxString retValue;

#ifdef ENV_DEBUGGER
   if ((connection->sourceOutputNum<0) || (connection->sourceOutputNum>=MAX_NUM_IOS))
   {
      g_debugWin->setDebugInformation(this,DEBUG_STOP_COND_ILLEGAL_IO,_T(""));
      *rcode=OAPC_ERROR_NO_SUCH_IO;
      return _T("");
   }
#endif
   if (convData.outData[connection->sourceOutputNum].flags & CONVERTER_FLAGS_USE_CLOCK)
   {
      if ((convData.outData[connection->sourceOutputNum].flags & CONVERTER_FLAGS_OUTPUT_MASK)==CONVERTER_FLAGS_BINARY_MODE)
       retValue=byteToBinary((wxByte)(digiBits & convData.outData[connection->sourceOutputNum].inputMask)>>1);
      else
      {
         wxUint32 inputNum=connection->getDigiIndexFromFlag(convData.outData[connection->sourceOutputNum].flags);
         if (digi[inputNum]) retValue=outValueHigh[connection->sourceOutputNum][inputNum];
         else retValue=outValueLow[connection->sourceOutputNum][inputNum];
      }
   }
   else
   {
      if ((convData.outData[connection->sourceOutputNum].flags & CONVERTER_FLAGS_OUTPUT_MASK)==CONVERTER_FLAGS_BINARY_MODE)
       retValue=byteToBinary((wxByte)(digiBits & convData.outData[connection->sourceOutputNum].inputMask));
      else
      {
         wxUint32 lastInputNum=connection->getDigiIndexFromFlag(lastInput);
         if (digi[lastInputNum]) retValue=outValueHigh[connection->sourceOutputNum][lastInputNum];
         else retValue=outValueLow[connection->sourceOutputNum][lastInputNum];
      }
   }
   *rcode=OAPC_OK;
   return retValue;
}
#endif



wxInt32 flowConverterDigi2Char::loadDATA(wxFile *FHandle,wxUint32 chunkSize,wxUint32 IDOffset,bool isCompiled)
{
   struct hmiObjectHead              convHead;
   struct flowConverterDigi2CharData loadData;
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

   loaded+=FHandle->Read(&loadData,sizeof(struct flowConverterDigi2CharData));

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
      convData.outData[i].inputMask=loadData.outData[i].inputMask; // it is a byte
      convData.outData[i].flags    =ntohl(loadData.outData[i].flags);
      for (j=0; j<CONVERTER_MAX_INPUTS; j++)
      {
         strConv.MB2WC(buf,loadData.outData[i].store_outValueLow[j],sizeof(buf));
         outValueLow[i][j]=buf;
         strConv.MB2WC(buf,loadData.outData[i].store_outValueHigh[j],sizeof(buf));
         outValueHigh[i][j]=buf;
      }
   }
   loaded+=flowObject::loadFlow(FHandle,&convData.flowData,IDOffset,false,isCompiled);
   return loaded;
}



