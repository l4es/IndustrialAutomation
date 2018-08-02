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
#include "flowConverterMixed2Char.h"
#ifdef ENV_EDITOR
#include "DlgConfigflowConvertMixed2Char.h"
#endif
#include "oapc_libio.h"

#ifdef ENV_WINDOWS
 #ifdef ENV_WINDOWSCE
  #include "Winsock2.h"
 #endif
#endif



flowConverterMixed2Char::flowConverterMixed2Char():flowConverter()
{
   this->data.type=flowObject::FLOW_TYPE_CONV_MIXED2CHAR;
   this->data.stdIN=OAPC_DIGI_IO0|OAPC_NUM_IO1|OAPC_NUM_IO2|OAPC_NUM_IO3|OAPC_NUM_IO4|OAPC_CHAR_IO5|OAPC_CHAR_IO6|OAPC_CHAR_IO7;
   this->data.stdOUT=OAPC_CHAR_IO1;
#ifdef ENV_PLAYER
   wxUint32 i;

   for (i=0; i<MAX_NUM_IOS; i++)
   {
      num[i]=0.0;
      txtD[i]=_T("0");
      txtF[i]=_T("0.000000");
   }
#endif
#ifdef ENV_EDITOR
   outFormat=_T("%d1 %d2 %f3 %f4 %s5 %s6 %s7");
   convData.flags=CONVERTER_FLAGS_USE_CLOCK;
#endif
}



flowConverterMixed2Char::~flowConverterMixed2Char()
{
}



wxString flowConverterMixed2Char::getDefaultName()
{
   return _T("Convert Mixed2Char");
}



#ifndef ENV_PLAYER
void flowConverterMixed2Char::doDataFlowDialog(bool hideISConfig)
{
   DlgConfigflowConvertMixed2Char dlg(this,(wxWindow*)g_hmiCanvas,_("Definition"),hideISConfig);

   dlg.ShowModal();
   if (dlg.returnOK)
   {
   }
   dlg.Destroy();
}



wxInt32 flowConverterMixed2Char::saveDATA(wxFile *FHandle,char chunkName[4],bool isCompiled)
{
   wxInt32                            length;
   struct hmiObjectHead               convHead;
   struct flowConverterMixed2CharData saveData;
   wxInt32                            len;
   wxNode                            *node;
   FlowConnection                    *connection;
   wxMBConvUTF16BE                    strConv;


   if (!FHandle) return 0;
   checkConnections();
   FHandle->Write(chunkName,4);
   if (isCompiled) length=sizeof(struct hmiObjectHead)+sizeof(struct flowConverterMixed2CharData)+(flowList.GetCount()*FLOW_CONNECTION_HEAD_SIZE);
   else length=sizeof(struct hmiObjectHead)+sizeof(struct flowConverterMixed2CharData)+(flowList.GetCount()*sizeof(struct flowConnectionData));
   length=htonl(length);
   FHandle->Write(&length,4);

   convHead.version=htonl(1);
   if (isCompiled) convHead.size=htonl(sizeof(struct flowConverterMixed2CharData)+(flowList.GetCount()*FLOW_CONNECTION_HEAD_SIZE));
   else convHead.size=htonl(sizeof(struct flowConverterMixed2CharData)+(flowList.GetCount()*sizeof(struct flowConnectionData)));
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
   saveData.flags             =htonl(convData.flags);
   strConv.WC2MB(saveData.store_name,name, sizeof(saveData.store_name));
   strConv.WC2MB(saveData.store_outFormat,outFormat, sizeof(saveData.store_outFormat));

   len+=FHandle->Write(&saveData,sizeof(struct flowConverterMixed2CharData));

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



wxUint64 flowConverterMixed2Char::getAssignedOutput(wxUint64 input)
{
   if (convData.flags & CONVERTER_FLAGS_USE_CLOCK)
   {
      if ((input & OAPC_DIGI_IO0) && (digi[0]==1)) return OAPC_CHAR_IO1;
      else return 0;
   }
   else return OAPC_CHAR_IO1;
}



/**
Sets an numerical input and checks if the same thread has set that input before
@param[in] input the flag that specifies the input
@param[in] value the value that has to be set for that input
@param[in] flowThreadID the Id of the thread that wants to set this value
@return 0 if the same thread already has set a value; the thread has to finish
        in this case to avoid endless loops; 1 if the input value was set correctly
*/
wxByte flowConverterMixed2Char::setNumInput(FlowConnection *connection,wxFloat64 value,wxUint32 *flowThreadID,flowObject *WXUNUSED(object))
{

   if (!threadIDOK(flowThreadID,false)) return 0;
#ifdef ENV_DEBUGGER
   if ((connection->data.targetInput & data.stdIN & OAPC_NUM_IO_MASK)==0)
   {
      wxASSERT(0);
      g_debugWin->setDebugInformation(this,DEBUG_STOP_COND_ILLEGAL_IO,_T(""));
      return 0;
   }
#endif
   num[connection->targetInputNum]=value;
   txtD[connection->targetInputNum]=wxString::Format(_T("%d"),(wxInt32)num[connection->targetInputNum]);
   txtF[connection->targetInputNum]=wxString::Format(_T("%f"),num[connection->targetInputNum]);

   if (convData.flags & flowConverterMixed2Char::CONVERTER_FLAGS_FORCE_TO_DOT)
    txtF[connection->targetInputNum].Replace(_T(","),_T("."));


   return 1;
}



/**
Sets an character input and checks if the same thread has set that input before
@param[in] input the flag that specifies the input
@param[in] value the value that has to be set for that input
@param[in] flowThreadID the Id of the thread that wants to set this value
@return 0 if the same thread already has set a value; the thread has to finish
        in this case to avoid endless loops; 1 if the input value was set correctly
*/
wxByte flowConverterMixed2Char::setCharInput(FlowConnection *connection,wxString value,wxUint32 *flowThreadID,flowObject *WXUNUSED(object))
{
   if (!threadIDOK(flowThreadID,false)) return 0;
#ifdef ENV_DEBUGGER
   if ((connection->data.targetInput & data.stdIN & OAPC_CHAR_IO_MASK)==0)
   {
      wxASSERT(0);
      g_debugWin->setDebugInformation(this,DEBUG_STOP_COND_ILLEGAL_IO,_T(""));
      return 0;
   }
#endif
   txtD[connection->targetInputNum]=value;
   return 1;
}



wxString flowConverterMixed2Char::getCharOutput(FlowConnection *connection,wxInt32 *rcode,wxUint64 WXUNUSED(lastInput))
{
   wxString  retValue;

#ifdef ENV_DEBUGGER
   if (connection->data.sourceOutput!=OAPC_CHAR_IO1)
   {
      g_debugWin->setDebugInformation(this,DEBUG_STOP_COND_ILLEGAL_IO,_T(""));
      *rcode=OAPC_ERROR_NO_SUCH_IO;
      return _T("");
   }
#else
   connection=connection;
#endif
   retValue=outFormat;
   retValue.Replace(_T("%d1"),txtD[1]); retValue.Replace(_T("%f1"),txtF[1]);
   retValue.Replace(_T("%d2"),txtD[2]); retValue.Replace(_T("%f2"),txtF[2]);
   retValue.Replace(_T("%d3"),txtD[3]); retValue.Replace(_T("%f3"),txtF[3]);
   retValue.Replace(_T("%d4"),txtD[4]); retValue.Replace(_T("%f4"),txtF[4]);
   retValue.Replace(_T("%s5"),txtD[5]);
   retValue.Replace(_T("%s6"),txtD[6]);
   retValue.Replace(_T("%s7"),txtD[7]);
   *rcode=OAPC_OK;
   return retValue;
}
#endif



wxInt32 flowConverterMixed2Char::loadDATA(wxFile *FHandle,wxUint32 chunkSize,wxUint32 IDOffset,bool isCompiled)
{
   struct hmiObjectHead                convHead;
   struct flowConverterMixed2CharData  loadData;
   wxInt32                             loaded;
   wxMBConvUTF16BE                     strConv;
   wchar_t                             buf[MAX_TEXT_LENGTH];

   if (!FHandle) return 0;
   if (chunkSize>sizeof(struct hmiObjectHead)) chunkSize=sizeof(struct hmiObjectHead);
   loaded=FHandle->Read(&convHead,sizeof(struct hmiObjectHead));

   convHead.size     =ntohl(convHead.size);
   convHead.version  =ntohl(convHead.version);
   convHead.reserved1=ntohl(convHead.reserved1);
   convHead.reserved2=ntohl(convHead.reserved2);

   loaded+=FHandle->Read(&loadData,sizeof(struct flowConverterMixed2CharData));

   if (IDOffset==0)
    data.id                   =ntohl(loadData.flowData.id);
   else
    data.id                   =ntohl(loadData.flowData.id)-IDOffset+g_objectList.currentUniqueID()+1;
   convData.flowData.usedFlows=ntohl(loadData.flowData.usedFlows);
   convData.flowData.maxEdges =ntohl(loadData.flowData.maxEdges);
   m_flowFlags                =ntohl(loadData.flowData.flowFlags);
   convData.flowX=             ntohl(loadData.flowX);
   convData.flowY=             ntohl(loadData.flowY);
   convData.flags=             ntohl(loadData.flags);
   strConv.MB2WC(buf,loadData.store_name,sizeof(buf));
   name=buf;
#ifdef ENV_EDITOR
   setFlowPos(NULL,wxPoint(convData.flowX,convData.flowY),1,1);
#else
   createNodeNames();
#endif
   strConv.MB2WC(buf,loadData.store_outFormat,sizeof(buf));
   outFormat=buf;
   loaded+=flowObject::loadFlow(FHandle,&convData.flowData,IDOffset,false,isCompiled);
   return loaded;
}



