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
#include "flowStart.h"
#include "oapc_libio.h"
#ifdef ENV_EDITOR
#include "DlgConfigflowStart.h"
#endif

#ifdef ENV_WINDOWS
 #ifdef ENV_WINDOWSCE
  #include "Winsock2.h"
 #endif
#endif



flowStart::flowStart():flowObject(NULL)
{
   this->data.type=FLOW_TYPE_FLOW_START;
   this->data.stdIN =0;
   this->data.stdOUT=OAPC_DIGI_IO1|OAPC_DIGI_IO2|OAPC_NUM_IO3|OAPC_NUM_IO4|OAPC_CHAR_IO5|OAPC_CHAR_IO6;
#ifdef ENV_EDITOR
   convData.digi[0]=1;
   convData.digi[1]=0;
   convData.mnum[0]=1000*1000;
   convData.mnum[1]=-1000*1000;
   txt[0]=_T("START");
   txt[1]=_T("A");
#endif
}



flowStart::~flowStart()
{
}



#ifdef ENV_EDITOR
wxString flowStart::getDefaultName()
{
   return _T("Initial Flow Start");
}



void flowStart::doDataFlowDialog(bool hideISConfig)
{
   DlgConfigflowStart dlg(this,(wxWindow*)g_hmiCanvas,_("Definition"),hideISConfig);

   dlg.ShowModal();
   if (dlg.returnOK)
   {
   }
   dlg.Destroy();
}



wxInt32 flowStart::saveDATA(wxFile *FHandle,char chunkName[4],bool isCompiled)
{
   wxInt32               length;
   struct hmiObjectHead  convHead;
   struct flowStartData  saveData;
   wxInt32               i,len;
   wxNode               *node;
   FlowConnection       *connection;
   wxMBConvUTF16BE       strConv;


   if (!FHandle) return 0;
   checkConnections();
   FHandle->Write(chunkName,4);
   if (isCompiled) length=sizeof(struct hmiObjectHead)+sizeof(struct flowStartData)+(flowList.GetCount()*FLOW_CONNECTION_HEAD_SIZE);
   else length=sizeof(struct hmiObjectHead)+sizeof(struct flowStartData)+(flowList.GetCount()*sizeof(struct flowConnectionData));
   length=htonl(length);
   FHandle->Write(&length,4);

   convHead.version=htonl(1);
   if (isCompiled) convHead.size=htonl(sizeof(struct flowStartData)+(flowList.GetCount()*FLOW_CONNECTION_HEAD_SIZE));
   else convHead.size=htonl(sizeof(struct flowStartData)+(flowList.GetCount()*sizeof(struct flowConnectionData)));
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
   for (i=0; i<2; i++)
   {
      saveData.digi[i]=convData.digi[i];
      saveData.mnum[i]=htonl(convData.mnum[i]);
      strConv.WC2MB(saveData.store_txt[i],txt[i], sizeof(saveData.store_txt[i]));
   }
   len+=FHandle->Write(&saveData,sizeof(struct flowStartData));

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



wxUint64 flowStart::getAssignedOutput(wxUint64 WXUNUSED(input))
{
   return OAPC_DIGI_IO1|OAPC_DIGI_IO2|OAPC_NUM_IO3|OAPC_NUM_IO4|OAPC_CHAR_IO5|OAPC_CHAR_IO6;
}



wxString flowStart::getCharOutput(FlowConnection *connection,wxInt32 *rcode,wxUint64 WXUNUSED(lastInput))
{
   if ((connection->sourceOutputNum<5) || (connection->sourceOutputNum>6))
   {
#ifdef ENV_DEBUGGER
      g_debugWin->setDebugInformation(this,DEBUG_STOP_COND_ILLEGAL_IO,_T(""));
#endif
      *rcode=OAPC_ERROR_NO_SUCH_IO;
      return _T("");
   }
   *rcode=OAPC_OK;
   return txt[connection->sourceOutputNum-5];
}



wxFloat64 flowStart::getNumOutput(FlowConnection *connection,wxInt32 *rcode,wxUint64 WXUNUSED(lastInput),wxLongLong WXUNUSED(origCreationTime))
{
   if ((connection->sourceOutputNum<3) || (connection->sourceOutputNum>4))
   {
#ifdef ENV_DEBUGGER
      g_debugWin->setDebugInformation(this,DEBUG_STOP_COND_ILLEGAL_IO,_T(""));
#endif
      *rcode=OAPC_ERROR_NO_SUCH_IO;
      return 0;
   }
   *rcode=OAPC_OK;
   return (convData.mnum[connection->sourceOutputNum-3]/1000.0);
}



wxByte flowStart::getDigitalOutput(FlowConnection *connection,wxInt32 *rcode,wxUint64 WXUNUSED(lastInput),wxLongLong WXUNUSED(origCreationTime))
{
   if ((connection->sourceOutputNum<1) || (connection->sourceOutputNum>2))
   {
#ifdef ENV_DEBUGGER
      g_debugWin->setDebugInformation(this,DEBUG_STOP_COND_ILLEGAL_IO,_T(""));
#endif
      *rcode=OAPC_ERROR_NO_SUCH_IO;
      return 0;
   }
   *rcode=OAPC_OK;
   return convData.digi[connection->sourceOutputNum-1];
}
#endif



wxInt32 flowStart::loadDATA(wxFile *FHandle,wxUint32 chunkSize,wxUint32 IDOffset,bool isCompiled)
{
   struct hmiObjectHead  convHead;
   struct flowStartData  loadData;
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

   loaded+=FHandle->Read(&loadData,sizeof(struct flowStartData));

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
   for (i=0; i<2; i++)
   {
      convData.digi[i]=loadData.digi[i];
      convData.mnum[i]=ntohl(loadData.mnum[i]);
      strConv.MB2WC(buf,loadData.store_txt[i],sizeof(buf));
      txt[i]=buf;
   }
   loaded+=flowObject::loadFlow(FHandle,&convData.flowData,IDOffset,false,isCompiled);
   return loaded;
}



