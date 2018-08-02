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

#ifndef __WXMSW__
#include <arpa/inet.h>
#endif

#include "iff.h"
#include "globals.h"
#include "flowConverter.h"
#include "flowConverter2Pair.h"
#include "oapc_libio.h"
#ifdef ENV_EDITOR
#include "DlgConfigflowConvert2Pair.h"
#endif


flowConverter2Pair::flowConverter2Pair():flowConverter()
{
#ifdef ENV_EDITOR
   wxInt32 i;
#endif

   this->data.type=0; // this is a base-class that should not be used directly
#ifdef ENV_EDITOR
   for (i=0; i<MAX_NUM_IOS; i++)
    cmdName[i]=wxString::Format(_T("CMD%d"),i);
#endif
}



flowConverter2Pair::~flowConverter2Pair()
{
}



wxString flowConverter2Pair::getDefaultName()
{
   wxASSERT(0);
   return _T("##invalid###");
}



#ifdef ENV_EDITOR
void flowConverter2Pair::doDataFlowDialog(bool hideISConfig)
{
   DlgConfigflowConvert2Pair dlg(this,true,(wxWindow*)g_hmiCanvas,_("Definition"),hideISConfig);

   dlg.ShowModal();
   if (dlg.returnOK)
   {
   }
   dlg.Destroy();
}



wxInt32 flowConverter2Pair::saveDATA(wxFile *FHandle,char chunkName[4],bool isCompiled)
{
   wxInt32                       length,i;
   struct hmiObjectHead          convHead;
   struct flowConverter2PairData saveData;
   wxInt32                       len;
   wxNode                       *node;
   FlowConnection               *connection;
   wxMBConvUTF16BE               strConv;


   if (!FHandle) return 0;
   checkConnections();
   FHandle->Write(chunkName,4);
   if (isCompiled) length=sizeof(struct hmiObjectHead)+sizeof(struct flowConverter2PairData)+(flowList.GetCount()*FLOW_CONNECTION_HEAD_SIZE);
   else length=sizeof(struct hmiObjectHead)+sizeof(struct flowConverter2PairData)+(flowList.GetCount()*sizeof(struct flowConnectionData));
   length=htonl(length);
   FHandle->Write(&length,4);

   convHead.version=htonl(1);
   if (isCompiled) convHead.size=htonl(sizeof(struct flowConverter2PairData)+(flowList.GetCount()*FLOW_CONNECTION_HEAD_SIZE));
   else convHead.size=htonl(sizeof(struct flowConverter2PairData)+(flowList.GetCount()*sizeof(struct flowConnectionData)));
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

   for (i=0; i<MAX_NUM_IOS; i++)
    strConv.WC2MB(saveData.store_cmdName[i],cmdName[i], sizeof(saveData.store_cmdName[i]));

   len+=FHandle->Write(&saveData,sizeof(struct flowConverter2PairData));

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

wxString flowConverter2Pair::getCharOutput(FlowConnection *connection,wxInt32 *rcode,wxUint64 WXUNUSED(lastInput))
{
   if (connection->sourceOutputNum!=0)
   {
      wxASSERT(0);
#ifdef ENV_DEBUGGER
      g_debugWin->setDebugInformation(this,DEBUG_STOP_COND_ILLEGAL_IO,_T(""));
#endif
      *rcode=OAPC_ERROR_NO_SUCH_IO;
      return _T("");
   }
   *rcode=OAPC_OK;
   return cmdName[m_lastUsedInput];
}
#endif



wxInt32 flowConverter2Pair::loadDATA(wxFile *FHandle,wxUint32 chunkSize,wxUint32 IDOffset,bool isCompiled)
{
   struct hmiObjectHead           convHead;
   struct flowConverter2PairData  loadData;
   wxInt32                        loaded,i;
   wxMBConvUTF16BE                strConv;
   wchar_t                        buf[MAX_TEXT_LENGTH];

   if (!FHandle) return 0;
   if (chunkSize>sizeof(struct hmiObjectHead)) chunkSize=sizeof(struct hmiObjectHead);
   loaded=FHandle->Read(&convHead,sizeof(struct hmiObjectHead));

   convHead.size     =ntohl(convHead.size);
   convHead.version  =ntohl(convHead.version);
   convHead.reserved1=ntohl(convHead.reserved1);
   convHead.reserved2=ntohl(convHead.reserved2);

   loaded+=FHandle->Read(&loadData,sizeof(struct flowConverter2PairData));

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
   for (i=0; i<MAX_NUM_IOS; i++)
   {
      strConv.MB2WC(buf,loadData.store_cmdName[i],sizeof(buf));
      cmdName[i]=buf;
   }
   loaded+=flowObject::loadFlow(FHandle,&convData.flowData,IDOffset,false,isCompiled);
   return loaded;
}




