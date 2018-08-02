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
#include "flowLogic.h"
#include "oapc_libio.h"

#ifdef ENV_WINDOWS
 #ifdef ENV_WINDOWSCE
  #include "Winsock2.h"
 #endif
#endif

#define FLOWLOGIC_ACT_VERSION 2

wxUint32 flowLogic::LOGIC_FLAGS_USE_CLOCK     =0x0010;
wxUint32 flowLogic::LOGIC_FLAGS_INVERT        =0x0020;
wxUint32 flowLogic::LOGIC_FLAGS_ALLOW_LOOPBACK=0x0040;



flowLogic::flowLogic():flowObject(NULL)
{
   this->data.type=0; // this is a base-class that should not be used directly
}



flowLogic::~flowLogic()
{
}



wxString flowLogic::getDefaultName()
{
   wxASSERT(0);
   return _T("##invalid###");
}



#ifdef ENV_EDITOR
wxInt32 flowLogic::saveDATA(wxFile *FHandle,char chunkName[4],bool isCompiled)
{
   wxInt32               length;
   struct hmiObjectHead  convHead;
   struct flowLogicData  saveData;
   wxInt32               i,len;
   wxNode               *node;
   FlowConnection       *connection;
   wxMBConvUTF16BE       strConv;


   if (!FHandle) return 0;
   checkConnections();
   FHandle->Write(chunkName,4);
   if (isCompiled) length=sizeof(struct hmiObjectHead)+sizeof(struct flowLogicData)+sizeof(struct hmiObjectHead)+(flowList.GetCount()*FLOW_CONNECTION_HEAD_SIZE);
   else length=sizeof(struct hmiObjectHead)+sizeof(struct flowLogicData)+sizeof(struct hmiObjectHead)+(flowList.GetCount()*sizeof(struct flowConnectionData));
   length=htonl(length);
   FHandle->Write(&length,4);

   convHead.version=htonl(FLOWLOGIC_ACT_VERSION);
   convHead.size=htonl(sizeof(struct flowLogicData));
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
      saveData.outData[i].inputMask=convData.outData[i].inputMask; // it is a byte
      saveData.outData[i].flags    =htonl(convData.outData[i].flags);
      saveData.outData[i].mConstValue_off=0;
      oapc_util_dbl_to_block(ConstValue[i],&saveData.ConstValue[i]);
   }
   len+=FHandle->Write(&saveData,sizeof(struct flowLogicData));

   convHead.version=htonl(1);
   if (isCompiled) convHead.size=htonl(FLOW_CONNECTION_HEAD_SIZE); // size of ONE structure element!
   else convHead.size=htonl(sizeof(struct flowConnectionData)); // size of ONE structure element!
   convHead.reserved1=0;
   convHead.reserved2=0;
   len=FHandle->Write(&convHead,sizeof(struct hmiObjectHead));

   node=flowList.GetFirst();
   while (node)
   {
      connection=(FlowConnection*)node->GetData();
      len+=connection->saveFlow(FHandle,isCompiled);
      node=node->GetNext();
   }
   return len;
}
#endif



wxInt32 flowLogic::loadDATA(wxFile *FHandle,wxUint32 WXUNUSED(chunkSize),wxUint32 IDOffset,bool isCompiled)
{
   struct hmiObjectHead  convHead;
   struct flowLogicData  loadData;
   wxInt32               loaded,i;
   wxMBConvUTF16BE       strConv;
   wchar_t               buf[MAX_TEXT_LENGTH];

   if (!FHandle) return 0;
//   if (chunkSize>sizeof(struct hmiObjectHead)) chunkSize=sizeof(struct hmiObjectHead);
   loaded=FHandle->Read(&convHead,sizeof(struct hmiObjectHead));

   convHead.size     =ntohl(convHead.size);
   convHead.version  =ntohl(convHead.version);
   convHead.reserved1=ntohl(convHead.reserved1);
   convHead.reserved2=ntohl(convHead.reserved2);
#ifdef ENV_EDITOR
   if (convHead.version==1) convHead.size=416;
   else convHead.size=sizeof(struct flowLogicData);
   if (convHead.version>FLOWLOGIC_ACT_VERSION)
#else
   if (convHead.version!=FLOWLOGIC_ACT_VERSION)
#endif
   {
      g_fileVersionError();
      return loaded;
   }   

   loaded+=FHandle->Read(&loadData,convHead.size);
   data.id                    =ntohl(loadData.flowData.id);
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
      convData.outData[i].inputMask=loadData.outData[i].inputMask; // it is a byte
      convData.outData[i].flags    =ntohl(loadData.outData[i].flags);
   }
#ifdef ENV_EDITOR
   if (convHead.version==1)
   {
      for (i=0; i<MAX_NUM_IOS; i++)
       ConstValue[i]=ntohl(loadData.outData[i].mConstValue_off)/1000.0;
   }
   else
#endif
   {
      for (i=0; i<MAX_NUM_IOS; i++)
       ConstValue[i]=oapc_util_block_to_dbl(&loadData.ConstValue[i]);
   }
   if (convHead.version==1) loaded+=flowObject::loadFlow(FHandle,&convData.flowData,IDOffset,false,isCompiled);
   else loaded+=flowObject::loadFlow(FHandle,&convData.flowData,IDOffset,true,isCompiled);
   return loaded;
}
