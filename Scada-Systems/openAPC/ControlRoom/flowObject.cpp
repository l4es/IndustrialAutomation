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

#ifdef ENV_INT
#include "../flowObject.cpp"
#else //ENV_INT

#include <wx/wx.h>

#ifndef __WXMSW__
#include <arpa/inet.h>
#endif

#ifdef ENV_DEBUGGER
#include "DebugWin.h"
#endif
#ifdef ENV_EDITOR
#include "DlgConfigflowName.h"
#endif
#include "oapc_libio.h"
#include "globals.h"
#include "flowObject.h"
#include "hmiUserMgmntPanel.h"
#include "hmiObject.h"

#ifdef ENV_WINDOWS
#ifdef ENV_WINDOWSCE
#include "Winsock2.h"
#endif
#endif

#define FLOWOBJECT_ACT_VERSION 2

/*
wxUint32 flowObject::LOG_TYPE_DIGI                =1;
wxUint32 flowObject::LOG_TYPE_INTNUM              =2;
wxUint32 flowObject::LOG_TYPE_FLOATNUM            =3;
wxUint32 flowObject::LOG_TYPE_CHAR                =4;

wxUint32 flowObject::LOG_FLAG_ON_NUM_VALUE_BELOW  =0x00000001;
wxUint32 flowObject::LOG_FLAG_ON_NUM_VALUE_ABOVE  =0x00000002;
wxUint32 flowObject::LOG_FLAG_ON_NUM_VALUE_BETWEEN=0x00000004;
wxUint32 flowObject::LOG_FLAG_ON_DIGI_VALUE_HI    =0x00000008;
wxUint32 flowObject::LOG_FLAG_ON_DIGI_VALUE_LO    =0x00000010;
wxUint32 flowObject::LOG_FLAG_ON_VALUE_CHANGED    =0x00000020;
wxUint32 flowObject::LOG_FLAG_MASK                =0x0000003F;

wxUint32 flowObject::FLAG_MAP_OUTPUT_TO_ISERVER   =0x00000001; // send changed output values to iserver
wxUint32 flowObject::FLAG_ALLOW_MOD_FROM_ISERVER  =0x00000002; // accept new values from iserver for modifying in/outputs of this object

#ifdef ENV_WINDOWS
wxUint64 flowObject::FLOW_TYPE_FLAG_DIGI_OVERFLOW_OUT =0x0000000100000000;
wxUint64 flowObject::FLOW_TYPE_FLAG_NUM_OVERFLOW_OUT  =0x0000000200000000;
wxUint64 flowObject::FLOW_TYPE_FLAG_CHAR_OVERFLOW_OUT =0x0000000400000000;
wxUint64 flowObject::FLOW_TYPE_FLAG_BIN_OVERFLOW_OUT  =0x0000000800000000;
#else
wxUint64 flowObject::FLOW_TYPE_FLAG_DIGI_OVERFLOW_OUT =0x0000000100000000LL;
wxUint64 flowObject::FLOW_TYPE_FLAG_NUM_OVERFLOW_OUT  =0x0000000200000000LL;
wxUint64 flowObject::FLOW_TYPE_FLAG_CHAR_OVERFLOW_OUT =0x0000000400000000LL;
wxUint64 flowObject::FLOW_TYPE_FLAG_BIN_OVERFLOW_OUT  =0x0000000800000000LL;
#endif
*/

//wxUint32 flowObject::FLOW_TYPE_EXTERNAL_LIB       =0x00000100;
wxUint32 flowObject::FLOW_TYPE_GROUP              =0x00000200;
wxUint32 flowObject::FLOW_TYPE_GROUP_IO_DEFINITION=0x00000300;
wxUint32 flowObject::FLOW_TYPE_CONV_DIGI2NUM      =0x00000400;
wxUint32 flowObject::FLOW_TYPE_CONV_DIGI2CHAR     =0x00000500;
wxUint32 flowObject::FLOW_TYPE_CONV_NUM2DIGI      =0x00000600;
wxUint32 flowObject::FLOW_TYPE_CONV_NUM2CHAR      =0x00000700;
wxUint32 flowObject::FLOW_TYPE_CONV_CHAR2DIGI     =0x00000800;
wxUint32 flowObject::FLOW_TYPE_CONV_CHAR2NUM      =0x00000900;
wxUint32 flowObject::FLOW_TYPE_CONV_MIXED2CHAR    =0x00000A00;
wxUint32 flowObject::FLOW_TYPE_CONV_NUM2BITS      =0x00000B00;
wxUint32 flowObject::FLOW_TYPE_LOGI_DIGINOT       =0x00000C00;
wxUint32 flowObject::FLOW_TYPE_LOGI_NUMNOT        =0x00000D00;
wxUint32 flowObject::FLOW_TYPE_LOGI_DIGIOR        =0x00000E00;
wxUint32 flowObject::FLOW_TYPE_LOGI_NUMOR         =0x00000F00;
wxUint32 flowObject::FLOW_TYPE_LOGI_DIGIAND       =0x00001000;
wxUint32 flowObject::FLOW_TYPE_LOGI_NUMAND        =0x00001100;
wxUint32 flowObject::FLOW_TYPE_LOGI_DIGIXOR       =0x00001200;
wxUint32 flowObject::FLOW_TYPE_LOGI_NUMXOR        =0x00001300;
wxUint32 flowObject::FLOW_TYPE_LOGI_NUMADD        =0x00001400;
wxUint32 flowObject::FLOW_TYPE_LOGI_NUMSUB        =0x00001500;
wxUint32 flowObject::FLOW_TYPE_LOGI_NUMMUL        =0x00001600;
wxUint32 flowObject::FLOW_TYPE_LOGI_NUMDIV        =0x00001700;
wxUint32 flowObject::FLOW_TYPE_FLOW_START         =0x00001800;
wxUint32 flowObject::FLOW_TYPE_FLOW_STOP          =0x00001900;
wxUint32 flowObject::FLOW_TYPE_FLOW_TIMER         =0x00001A00;
wxUint32 flowObject::FLOW_TYPE_FLOW_DELAY         =0x00001B00;
wxUint32 flowObject::FLOW_TYPE_MISC_LOG           =0x00001C00;
wxUint32 flowObject::FLOW_TYPE_MISC_LOG_REC       =0x00001D00;
wxUint32 flowObject::FLOW_TYPE_CONV_DIGI2PAIR     =0x00001E00;
wxUint32 flowObject::FLOW_TYPE_CONV_NUM2PAIR      =0x00001F00;
wxUint32 flowObject::FLOW_TYPE_CONV_CHAR2PAIR     =0x00002000;
wxUint32 flowObject::FLOW_TYPE_CONV_BIN2PAIR      =0x00002100;
wxUint32 flowObject::FLOW_TYPE_FLOW_NUMTGATE      =0x00002200;
wxUint32 flowObject::FLOW_TYPE_FLOW_CHARTGATE     =0x00002300;
wxUint32 flowObject::FLOW_TYPE_FLOW_BINTGATE      =0x00002400;
wxUint32 flowObject::FLOW_TYPE_FLOW_DIGITGATE     =0x00002500;
wxUint32 flowObject::FLOW_TYPE_CONV_PAIR2CHAR     =0x00002600;
wxUint32 flowObject::FLOW_TYPE_CONV_PAIR2DIGI     =0x00002700;
wxUint32 flowObject::FLOW_TYPE_CONV_PAIR2NUM      =0x00002800;
wxUint32 flowObject::FLOW_TYPE_CONV_PAIR2BIN      =0x00002900;
wxUint32 flowObject::FLOW_TYPE_LOGI_DIGINOP       =0x00002A00;
wxUint32 flowObject::FLOW_TYPE_LOGI_NUMNOP        =0x00002B00;
wxUint32 flowObject::FLOW_TYPE_LOGI_CHARNOP       =0x00002C00;
wxUint32 flowObject::FLOW_TYPE_LOGI_BINNOP        =0x00002D00;
wxUint32 flowObject::FLOW_TYPE_LOGI_DIGISHIFT     =0x00002E00;
wxUint32 flowObject::FLOW_TYPE_LOGI_NUMSHIFT      =0x00002F00;
wxUint32 flowObject::FLOW_TYPE_LOGI_CHARSHIFT     =0x00003000;
wxUint32 flowObject::FLOW_TYPE_LOGI_BINSHIFT      =0x00003100;
wxUint32 flowObject::FLOW_TYPE_FLOW_NUMGATE       =0x00003200;
wxUint32 flowObject::FLOW_TYPE_FLOW_CHARGATE      =0x00003300;
wxUint32 flowObject::FLOW_TYPE_FLOW_BINGATE       =0x00003400;
wxUint32 flowObject::FLOW_TYPE_FLOW_DIGIGATE      =0x00003500;
wxUint32 flowObject::FLOW_TYPE_LOGI_DIGIRSFF      =0x00003600;
wxUint32 flowObject::FLOW_TYPE_MATH_NUMCMP        =0x00003700;
wxUint32 flowObject::FLOW_TYPE_MATH_DIGICMP       =0x00003800;
wxUint32 flowObject::FLOW_TYPE_MATH_CHARCMP       =0x00003900;
wxUint32 flowObject::FLOW_TYPE_MATH_DIGICTR       =0x00003A00;
wxUint32 flowObject::FLOW_TYPE_MATH_NUMCTR        =0x00003B00;
wxUint32 flowObject::FLOW_TYPE_MATH_CHARCTR       =0x00003C00;
wxUint32 flowObject::FLOW_TYPE_MATH_BINCTR        =0x00003D00;
wxUint32 flowObject::FLOW_TYPE_LOGI_DIGITFF       =0x00003E00;
wxUint32 flowObject::FLOW_TYPE_MISC_LOGIN         =0x00003F00;
wxUint32 flowObject::FLOW_TYPE_MISC_ISCONNECT     =0x00004000;

flowObject::flowObject(flowObject *obj):wxEvtHandler()
{
   wxInt32 i;

   m_doDestroy=1;
   for (i=0; i<MAX_NUM_IOS; i++) digi[i]=0;
#ifdef ENV_PLAYER
   lastPollTime=0;
   lastFlowThreadID=0;
   digiBits=0;
   for (i=0; i<MAX_NUM_IOS; i++) dataValid[i]=false;
   nodeNameIn=NULL;
   nodeNameOut=NULL;
   m_flowFlags=0;
#else
   m_mode=OBJECT_MODE_FLOWELEMENT;
   flowBitmap=NULL;
#endif
   if (obj!=NULL)
   {
       memcpy(&this->data,&obj->data,sizeof(struct hmiObjectData));
       this->data.id=0; // do not use the ID of the copied object because they have to be unique (ID is set by ObjectList::Append())
       this->fontFaceName[0]=obj->fontFaceName[0];
       this->fontFaceName[1]=obj->fontFaceName[1];
       this->text[0]=obj->text[0];
       this->text[1]=obj->text[1];
   }
   else
   {
      this->data.type=0;
      this->m_flowFlags=0; //FLAG_MAP_OUTPUT_TO_ISERVER|FLAG_ALLOW_MOD_FROM_ISERVER;
      this->data.logFlags=0;
      this->data.isChildOf=0;
      this->data.stdIN=OAPC_DIGI_IO0;
      this->data.stdOUT=OAPC_DIGI_IO0;
#ifndef ENV_HPLAYER
#ifdef ENV_WINDOWS
      this->data.userPriviFlags=(0xFFFFFFFFFFFFFFFF & ~hmiUserMgmntPanel::priviFlagMask[0]) | hmiUserMgmntPanel::priviFlagEnabled[0];
#else
      this->data.userPriviFlags=(0xFFFFFFFFFFFFFFFFLL & ~hmiUserMgmntPanel::priviFlagMask[0]) | hmiUserMgmntPanel::priviFlagEnabled[0];
#endif
#endif
      this->data.dataFlowFlags=HMI_FLOWFLAG_ON_CHANGED_OUT7;
      this->data.id=0;
      this->data.flowposx=-1;         this->data.flowposy=-1;
      this->data.bgcolor[0]=0xDDDDDD; this->data.bgcolor[1]=0xBBBBBB;
      this->data.fgcolor[0]=0x000000; this->data.fgcolor[1]=0x222222;
      this->data.state[0]=0;          this->data.state[1]=0;

#ifndef ENV_HPLAYER
      wxWindow win;
      wxFont   font;

      font=win.GetFont();
      for (i=0; i<2; i++)
      {
         this->data.font[i].pointSize=font.GetPointSize();
         this->data.font[i].style=font.GetStyle();
         this->data.font[i].weight=font.GetWeight();
         fontFaceName[i]=_T("");
      }
#endif
      for (i=0; i<MAX_LOG_TYPES; i++)
      {
         data.log[i].mBelowVal=0;
         data.log[i].mAboveVal=0;
         data.log[i].mFromVal=0;
         data.log[i].mToVal=0;
         data.log[i].flags=0;
      }
      MinValue=0;
      MaxValue=100;
   }
}



flowObject::~flowObject()
{
   wxNode *node;

   node=flowList.GetFirst();
   while (node)
   {
      if (node->GetData()) delete node->GetData();
      node=node->GetNext();
   }
#ifdef ENV_PLAYER
   if (nodeNameIn) free(nodeNameIn);
   if (nodeNameOut) free(nodeNameOut);
#endif
}



wxPoint flowObject::getFlowPos()
{
    return wxPoint(data.flowposx,data.flowposy);
}



wxUint32 flowObject::getMode()
{
   return m_mode;
}



void flowObject::setMode(wxUint32 mode)
{
	m_mode=mode;
}



#ifdef ENV_EDITOR
wxString flowObject::getDefaultName()
{
    wxASSERT(0);
    return _T("##invalid flowObject##");
}



/**
Sets the position of the incoming flow lines to keep then in-place during an flow element is dragged
@param[in] pos the new position of the element
@param[in] forcePosition the given position is used without any calculation
@param[in] checkIncoming defines if the incoming flow connection positions have to be checked and
           adapted or not; this option has to be set to 0 in case of the outgoing IO list of a group,
           here the incoming connections are group-internal and don't have to be modified from outside
*/
void flowObject::setFlowPos(ObjectList *parentList,wxPoint pos,wxByte forcePosition,wxByte checkIncoming)
{
    wxNode         *node;
    FlowConnection *connection;
    wxPoint         linePos0,linePos1,linePos2;

    if (forcePosition)
    {
        data.flowposx=pos.x;
        data.flowposy=pos.y;
        return;
    }
    node=flowList.GetFirst();
    while (node)
    {
        connection=(FlowConnection*)node->GetData();
        linePos0=connection->getPosAt(0);
        linePos1=connection->getPosAt(1);
        linePos0.x+=pos.x-data.flowposx;
        linePos0.y+=pos.y-data.flowposy;
        connection->setPosAt(0,linePos0);
        if ((connection->data.sourceOutput & (OAPC_DIGI_IO_MASK|OAPC_NUM_IO_MASK|
                                              OAPC_CHAR_IO_MASK|OAPC_BIN_IO_MASK))!=0)
        {
           if (linePos0.y+2>linePos1.y)
           {
              linePos1.y=linePos0.y+2;
              linePos2=connection->getPosAt(2);
              linePos2.y=linePos0.y+2;
              connection->setPosAt(2,linePos2);
           }
           linePos1.x+=pos.x-data.flowposx;
           connection->setPosAt(1,linePos1);
        }
        else
        {
           linePos1.y+=pos.y-data.flowposy;
           connection->setPosAt(1,linePos1);
        }

        node=node->GetNext();
    }
    if (checkIncoming) setIncomingConnectionPos(parentList,pos-wxPoint(data.flowposx,data.flowposy));
    data.flowposx=pos.x;
    data.flowposy=pos.y;
}



void flowObject::setIncomingConnectionPos(ObjectList *parentList,wxPoint delta)
{
    wxNode         *node,*objNode;
    flowObject     *srcObject;
    FlowConnection *connection;
    wxPoint         linePos0,linePos1,linePos2;

    //TODO: chache these information after the first call from DRAG and flush the chache at the end
    objNode=parentList->GetFirst();
    while (objNode)
    {
       srcObject=(flowObject*)objNode->GetData();
//       if (srcObject!=this) // also compare same object, a FlowConnection can loop back to itself!
       {
          node=srcObject->getFlowConnection(NULL);
          while (node)
          {
             connection=(FlowConnection*)node->GetData();
             if (connection->data.targetID==data.id)
             {
                linePos0=connection->getLastPos();
                linePos1=connection->getPrevPos();
                linePos0+=delta;
                linePos1.x+=delta.x;
                if (linePos1.y>linePos0.y-2)
                {
                   linePos1.y=linePos0.y-2;
                   linePos2=connection->getPosAt(connection->data.usedEdges-2);
                   linePos2.y=linePos0.y-2;
                   connection->setPosAt(connection->data.usedEdges-2,linePos2);
                }

                connection->setLastPos(linePos0);
                connection->setPrevPos(linePos1);
             }
             node=srcObject->getFlowConnection(node);
          }
       }
       objNode=objNode->GetNext();
    }
}




wxInt32 flowObject::getFlowWidth()
{
    return FLOW_OBJECT_WIDTH;
}



wxInt32 flowObject::getFlowHeight()
{
    return FLOW_OBJECT_HEIGHT;
}



void flowObject::checkConnections()
{
   wxNode                   *node,*node2;
   FlowConnection           *connection,*connection2,*remConnection=NULL;

   // check for double connections - this is a workaround for a bug and a helper function
   // for the end user so that no unwanted, double lines are created
   node=flowList.GetFirst();
   while (node)
   {
      connection=(FlowConnection*)node->GetData();

      node2=flowList.GetFirst();
      while (node2)
      {
         connection2=(FlowConnection*)node2->GetData();      
         if (connection!=connection2)
         {
            if ((connection->data.sourceOutput==connection2->data.sourceOutput) &&
                (connection->data.targetID    ==connection2->data.targetID) &&
                (connection->data.targetInput ==connection2->data.targetInput))
            {
               if (connection->data.usedEdges<MAX_CONNECTION_EDGES) // keep the connection only in case it is not damaged
               {
                  wxASSERT(0);
                  wxASSERT(d_isCompiling);
                  remConnection=connection2;
               }
               else wxASSERT(0);
            }

         }
         node2=node2->GetNext();
         if (remConnection)
         {
            wxASSERT(d_isCompiling);
            flowList.DeleteObject(remConnection);
         }
      } 
      
      node=node->GetNext();
   }
   // end of check for double connections
}



wxInt32 flowObject::saveDATA(wxFile *FHandle,char chunkName[4],bool isCompiled)
{
   wxInt32                   length;
   struct hmiObjectHead      convHead;
   struct flowObjectData     saveData;
   wxInt32                   len;
   wxNode                   *node;
   FlowConnection           *connection;
   wxMBConvUTF16BE           strConv;

   if (!FHandle) return 0;
   checkConnections();

   FHandle->Write(chunkName,4);
   if (isCompiled) length=sizeof(struct hmiObjectHead)+sizeof(struct flowObjectData)+(flowList.GetCount()*FLOW_CONNECTION_HEAD_SIZE);
   else length=sizeof(struct hmiObjectHead)+sizeof(struct flowObjectData)+(flowList.GetCount()*sizeof(struct flowConnectionData));
   length=htonl(length);
   FHandle->Write(&length,4);

   convHead.version=htonl(1);
   if (isCompiled) convHead.size=htonl(sizeof(struct flowObjectData)+(flowList.GetCount()*FLOW_CONNECTION_HEAD_SIZE));
   else convHead.size=htonl(sizeof(struct flowObjectData)+(flowList.GetCount()*sizeof(struct flowConnectionData)));
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
   len+=FHandle->Write(&saveData,sizeof(struct flowObjectData));

   node=flowList.GetFirst();
   while (node)
   {
      connection=(FlowConnection*)node->GetData();
      len+=connection->saveFlow(FHandle,isCompiled);
      node=node->GetNext();
   }
   return len;
}



wxInt32 flowObject::save(wxFile *FHandle)
{
    struct hmiObjectData convData;
    struct hmiObjectHead convHead;
    wxInt32              i,len;
    wxMBConvUTF16BE      conv;

    if (!FHandle) return 0;

    convHead.version=htonl(FLOWOBJECT_ACT_VERSION);
    convHead.size=htonl(sizeof(struct hmiObjectData));
    convHead.reserved1=0;
    convHead.reserved2=0;
    len=FHandle->Write(&convHead,sizeof(struct hmiObjectHead));

    convData.type           =htonl(data.type);
    convData.hmiFlags       =htonl(data.hmiFlags);
    convData.logFlags       =htonl(data.logFlags);
    convData.isChildOf      =htonl(data.isChildOf);
    convData.stdIN          =oapc_util_htonll(data.stdIN);  // to be removed some time, they are static
    convData.stdOUT         =oapc_util_htonll(data.stdOUT); // to be removed some time, they are static
    convData.userPriviFlags =oapc_util_htonll(data.userPriviFlags);
    convData.id             =htonl(data.id);
    convData.dataFlowFlags  =htonl(data.dataFlowFlags);
    convData.floatAccuracy  =data.floatAccuracy;
    convData.reservedLayoutAlignment=data.reservedLayoutAlignment;
    convData.custom3        =htons(data.custom3);
    convData.mMaxValue_off  =0;
    convData.mposx          =htonl(data.mposx);    convData.mposy=htonl(data.mposy);
    convData.msizex         =htonl(data.msizex);   convData.msizey=htonl(data.msizey);
    convData.flowposx       =htonl(data.flowposx); convData.flowposy=htonl(data.flowposy);
    conv.WC2MB(convData.store_name,name,sizeof(convData.store_name));

    for (i=0; i<=1; i++)
    {
       conv.WC2MB(convData.store_text[i],text[i],sizeof(convData.store_text[i]));

       convData.bgcolor[i]=htonl(data.bgcolor[i]);
       convData.fgcolor[i]=htonl(data.fgcolor[i]);
       convData.state[i]=htonl(data.state[i]);

       conv.WC2MB(convData.font[i].store_faceName,fontFaceName[i],sizeof(convData.font[i].store_faceName));
       convData.font[i].pointSize=htonl(data.font[i].pointSize);
       convData.font[i].style=htonl(data.font[i].style);
       convData.font[i].weight=htonl(data.font[i].weight);
    }
    for (i=0; i<MAX_LOG_TYPES; i++)
    {
       convData.log[i].mBelowVal=htonl(data.log[i].mBelowVal);
       convData.log[i].mAboveVal=htonl(data.log[i].mAboveVal);
       convData.log[i].mFromVal=htonl(data.log[i].mFromVal);
       convData.log[i].mToVal=htonl(data.log[i].mToVal);
       convData.log[i].flags=htonl(data.log[i].flags);
    }
    oapc_util_dbl_to_block(MinValue,&convData.MinValue);
    oapc_util_dbl_to_block(MaxValue,&convData.MaxValue);

    return len+FHandle->Write(&convData,sizeof(struct hmiObjectData));
}
#endif



void flowObject::applyData(wxByte WXUNUSED(all))
{
}



#ifndef ENV_EDITOR
void flowObject::createNodeNames()
{
#ifdef ENV_PLAYER
   wxMBConvUTF8              conv8;
   wxString                  nodeName;
#endif

   nodeName=_T("/")+name+_T("/in");
   nodeNameIn=(char*)malloc((nodeName.Length()*2)+1);
   wxASSERT(nodeNameIn);
   if (nodeNameIn) conv8.WC2MB(nodeNameIn,nodeName,nodeName.Length()*2);

   nodeName=_T("/")+name+_T("/out");
   nodeNameOut=(char*)malloc((nodeName.Length()*2)+1);
   wxASSERT(nodeNameOut);
   if (nodeNameOut) conv8.WC2MB(nodeNameOut,nodeName,nodeName.Length()*2);
}
#endif




#ifndef ENV_PLUGGER
wxInt32 flowObject::loadDATA(wxFile *FHandle,wxUint32 chunkSize,wxUint32 IDOffset,bool isCompiled)
{
   struct hmiObjectHead      convHead;
   struct flowObjectData     loadData;
   wxInt32                   loaded;
   wxMBConvUTF16BE           strConv;
   wchar_t                   buf[MAX_TEXT_LENGTH];

   if (!FHandle) return 0;
   if (chunkSize>sizeof(struct hmiObjectHead)) chunkSize=sizeof(struct hmiObjectHead);
   loaded=FHandle->Read(&convHead,sizeof(struct hmiObjectHead));

   convHead.size     =ntohl(convHead.size);
   convHead.version  =ntohl(convHead.version);
   convHead.reserved1=ntohl(convHead.reserved1);
   convHead.reserved2=ntohl(convHead.reserved2);

   loaded+=FHandle->Read(&loadData,sizeof(struct flowObjectData));

#ifdef ENV_ISERVER
   data.id=ntohl(loadData.flowData.id);
#else
   if (IDOffset==0) data.id=ntohl(loadData.flowData.id);
   else data.id=ntohl(loadData.flowData.id)-IDOffset+g_objectList.currentUniqueID()+1;
#endif

   convData.flowData.usedFlows=ntohl(loadData.flowData.usedFlows);
   convData.flowData.maxEdges =ntohl(loadData.flowData.maxEdges);
   m_flowFlags                =ntohl(loadData.flowData.flowFlags);
   convData.flowX=             ntohl(loadData.flowX);
   convData.flowY=             ntohl(loadData.flowY);
   strConv.MB2WC(buf,loadData.store_name,sizeof(buf));
   name=buf;
   g_checkString(name);
#ifdef ENV_EDITOR
   setFlowPos(NULL,wxPoint(convData.flowX,convData.flowY),1,1);
#else
   createNodeNames();
#endif
   loaded+=loadFlow(FHandle,&convData.flowData,IDOffset,false,isCompiled);
   return loaded;
}


wxInt32 flowObject::getLoadError()
{
   return OAPC_OK;
}


wxInt32 flowObject::load(wxFile *FHandle,wxUint32 chunkSize,wxUint32 IDOffset)
{
    struct hmiObjectData convData;
    struct hmiObjectHead convHead;
    wxInt32              i;
    wxMBConvUTF16BE      conv;
    wchar_t              buf[MAX_TEXT_LENGTH];

    if (!FHandle) return 0;
    chunkSize=FHandle->Read(&convHead,sizeof(struct hmiObjectHead));

    convHead.size     =ntohl(convHead.size);
    convHead.version  =ntohl(convHead.version);
    convHead.reserved1=ntohl(convHead.reserved1);
    convHead.reserved2=ntohl(convHead.reserved2);
//    if (chunkSize>convHead.size) chunkSize=convHead.size;

#ifdef ENV_EDITOR
    if (convHead.version>FLOWOBJECT_ACT_VERSION)
#else
    if (convHead.version!=FLOWOBJECT_ACT_VERSION)
#endif
    {
#ifndef ENV_ISERVER
       g_fileVersionError();
#else
   IDOffset=IDOffset;
 #ifdef ENV_LINUX
  #warning implement error message handling via IServer
 #endif
#endif
       return chunkSize;
    }

    chunkSize+=FHandle->Read(&convData,convHead.size);

    data.type          =ntohl(convData.type);
    data.hmiFlags      =ntohl(convData.hmiFlags);
    data.logFlags      =ntohl(convData.logFlags);
    data.isChildOf     =ntohl(convData.isChildOf);

//    do not load these data, they depend on the code and therefore are static
//    data.stdIN         =oapc_util_ntohll(convData.stdIN);
//    data.stdOUT        =oapc_util_ntohll(convData.stdOUT);

    data.userPriviFlags=oapc_util_ntohll(convData.userPriviFlags);
#ifndef ENV_ISERVER
    if (IDOffset==0) data.id=ntohl(convData.id);
    else data.id=ntohl(convData.id)-IDOffset+g_objectList.currentUniqueID()+1;
#else
    data.id=ntohl(convData.id);
#endif
    data.dataFlowFlags  =ntohl(convData.dataFlowFlags);
    data.mposx          =ntohl(convData.mposx);    data.mposy=ntohl(convData.mposy);
    data.msizex         =ntohl(convData.msizex);   data.msizey=ntohl(convData.msizey);
    data.flowposx       =ntohl(convData.flowposx); data.flowposy=ntohl(convData.flowposy);
    data.floatAccuracy  =convData.floatAccuracy;
    data.reservedLayoutAlignment=convData.reservedLayoutAlignment;
    data.custom3        =ntohs(convData.custom3);
    conv.MB2WC(buf,convData.store_name,sizeof(buf));
    name=buf;
    for (i=0; i<=1; i++)
    {
       conv.MB2WC(buf,convData.store_text[i],sizeof(buf));
       text[i]=buf;

       data.bgcolor[i]=ntohl(convData.bgcolor[i]);
       data.fgcolor[i]=ntohl(convData.fgcolor[i]);
       data.state[i]=ntohl(convData.state[i]);

       conv.MB2WC(buf,convData.font[i].store_faceName,sizeof(buf));
       fontFaceName[i]=buf;
       data.font[i].pointSize=ntohl(convData.font[i].pointSize);
       data.font[i].style=ntohl(convData.font[i].style);
       data.font[i].weight=ntohl(convData.font[i].weight);
    }
    for (i=0; i<MAX_LOG_TYPES; i++)
    {
       data.log[i].mBelowVal=ntohl(convData.log[i].mBelowVal);
       data.log[i].mAboveVal=ntohl(convData.log[i].mAboveVal);
       data.log[i].mFromVal=ntohl(convData.log[i].mFromVal);
       data.log[i].mToVal=ntohl(convData.log[i].mToVal);
       data.log[i].flags=ntohl(convData.log[i].flags);
    }
#ifdef ENV_EDITOR
   if (convHead.version==1)
   {
      MinValue=-1000;
      MaxValue=ntohl(convData.mMaxValue_off)/1000.0;
   }
   else
#endif
   {
      MinValue=oapc_util_block_to_dbl(&convData.MinValue);
      MaxValue=oapc_util_block_to_dbl(&convData.MaxValue);
   }
#ifndef ENV_EDITOR
   createNodeNames();
#endif
   return chunkSize;
}



wxInt32 flowObject::loadFlow(wxFile *FHandle,struct hmiFlowData *flowData,wxUint32 IDOffset,bool usesOwnHead,bool isCompiled)
{
   wxInt32              length=0;
#ifndef ENV_ISERVER
   FlowConnection      *flowCon;
   wxInt32              i;
   wxByte               successful;
#endif
   struct hmiObjectHead convHead;

   wxASSERT(FHandle);
   if (!FHandle) return 0;

   if (usesOwnHead)
   {
      length=FHandle->Read(&convHead,sizeof(struct hmiObjectHead));

      convHead.size     =ntohl(convHead.size);
      convHead.version  =ntohl(convHead.version);
      convHead.reserved1=ntohl(convHead.reserved1);
      convHead.reserved2=ntohl(convHead.reserved2);
   }

#ifndef ENV_ISERVER
   for (i=0; i<flowData->usedFlows; i++)
   {
      flowCon=new FlowConnection();
      length+=flowCon->loadFlow(FHandle,flowData,IDOffset,successful,isCompiled);
      if (successful) this->addFlowConnection(flowCon);
      else delete flowCon;
   }
#else
   isCompiled=isCompiled;
   IDOffset=IDOffset;
   wxASSERT(flowData->usedFlows==0); // the IServer part of ap roject file does not contain any flow information
#endif
   return length;
}
#endif



void flowObject::setData(struct hmiObjectData *newdata)
{
   memcpy(&data,newdata,sizeof(struct hmiObjectData));
   applyData(1);
}



void flowObject::setData(flowObject *object)
{
   MaxValue=object->MaxValue;
   MinValue=object->MinValue;
   fontFaceName[0]=object->fontFaceName[0];
   fontFaceName[1]=object->fontFaceName[1];
   setData(&object->data);
}



void flowObject::addFlowConnection(FlowConnection *connection)
{
    flowList.Append(connection);
}



wxNode *flowObject::getFlowConnection(wxNode *node)
{
    if (!node) return flowList.GetFirst();
    return node->GetNext();
}



void flowObject::deleteFlowConnection(FlowConnection *connection)
{
    flowList.DeleteObject(connection);
    delete connection;
}



#ifdef ENV_EDITOR
FlowConnection *flowObject::getFlowConnection(wxPoint pos,wxInt32 tolerance)
{
    wxList::compatibility_iterator node;
    FlowConnection                *connection;

    node=flowList.GetFirst();
    while (node)
    {
        connection=(FlowConnection*)node->GetData();
        if (connection->getFlowLine(pos,tolerance)>-1) return connection;
        node=node->GetNext();
    }
    return NULL;
}



bool flowObject::removeFlowConnection(FlowConnection *connection)
{
    return flowList.DeleteObject(connection);
}




void flowObject::drawFlowConnections(wxAutoBufferedPaintDC *dc,FlowConnection *ignoreFlowCon,wxInt32 x, wxInt32 y)
{
    wxList::compatibility_iterator node;
    FlowConnection                *connection;

    node=flowList.GetFirst();
    while (node)
    {
        connection=(FlowConnection*)node->GetData();
        if (connection!=ignoreFlowCon) connection->drawFlowConnection(dc,NULL,x,y);
        node=node->GetNext();
    }
}



void flowObject::drawFlowConnections(wxAutoBufferedPaintDC *dc,wxUint32 ignoreOutConnector,wxInt32 x, wxInt32 y)
{
    wxList::compatibility_iterator node;
    FlowConnection                *connection;

    node=flowList.GetFirst();
    while (node)
    {
        connection=(FlowConnection*)node->GetData();
        if (connection->data.sourceOutput!=ignoreOutConnector) connection->drawFlowConnection(dc,NULL,x,y);
        node=node->GetNext();
    }
}



void flowObject::doDataFlowDialog(bool hideISConfig)
{
   DlgConfigflowName dlg(this,(wxWindow*)g_hmiCanvas,_("Definition"),hideISConfig);

   dlg.ShowModal();
   if (dlg.returnOK)
   {
   }
   dlg.Destroy();
}
#endif



#ifdef ENV_PLAYER
#ifndef ENV_PLUGGER
bool flowObject::threadIDOK(wxUint32 *flowThreadID,bool allowSameID)
{
#ifdef ENV_ISERVER
   flowThreadID=flowThreadID;
   allowSameID=allowSameID;
#else
   if (lastFlowThreadID==*flowThreadID)
   {
      if (allowSameID)
      {
        *flowThreadID=g_flowPool.getFlowThreadID();
         lastFlowThreadID=*flowThreadID;
         return true;
      }
#ifdef ENV_DEBUGGER
      g_debugWin->setDebugInformation(this,DEBUG_STOP_COND_THREAD_ID,_T(""));
#endif
      return false;
   }
   lastFlowThreadID=*flowThreadID;
#endif
   return true;
}
#endif



wxUint64 flowObject::getAssignedOutput(wxUint64 WXUNUSED(input))
{
   return 0;
}



/**
Sets an digital input and checks if the same thread has set that input before
@param[in] input the flag that specifies the input
@param[in] value the value that has to be set for that input (0 or 1)
@param[in] flowThreadID the Id of the thread that wants to set this value
@return 0 if the same thread already has set a value; the thread has to finish
        in this case to avoid endless loops; 1 if the input value was set correctly
*/
wxByte flowObject::setDigitalInput(FlowConnection *connection,wxByte value,wxUint32 *flowThreadID,flowObject *WXUNUSED(object))
{
   wxByte   bit=0;

#ifndef ENV_PLUGGER
   if (!threadIDOK(flowThreadID,false)) return 0;
#endif
   switch (connection->targetInputNum)
   {
      case 0:
         bit=1;
         break;
      case 1:
         bit=2;
         break;
      case 2:
         bit=4;
         break;
      case 3:
         bit=8;
         break;
      case 4:
         bit=16;
         break;
      case 5:
         bit=32;
         break;
      case 6:
         bit=64;
         break;
      case 7:
         bit=128;
         break;
      default:
         wxASSERT(0);
         return 0;
         break;
   }
   if (value)
   {
      digi[connection->targetInputNum]=1;
      digiBits|=bit;
   }
   else
   {
      digi[connection->targetInputNum]=0;
      digiBits&=~bit;
   }
   applyData(0);
   return 1;
}



wxByte flowObject::getDigitalOutput(FlowConnection *connection,wxInt32 *rcode,wxUint64 WXUNUSED(lastInput),wxLongLong WXUNUSED(origCreationTime))
{
#ifdef ENV_DEBUGGER
   if ((connection->sourceOutputNum<0) || (connection->sourceOutputNum>=MAX_NUM_IOS))
   {
      wxASSERT(0);
      g_debugWin->setDebugInformation(this,DEBUG_STOP_COND_ILLEGAL_IO,_T(""));
      *rcode=OAPC_ERROR_NO_SUCH_IO;
      return 0;
   }
#endif
   *rcode=OAPC_OK;
   return digi[connection->sourceOutputNum];
}



bool flowObject::valueHasChanged()
{
	return false;
}


/**
Sets an numerical input
@param[in] input the flag that specifies the input
@param[in] value the value that has to be set for that input
@param[in] flowThreadID the Id of the thread that wants to set this value
@return 0; this method has to be overriden
*/
wxByte flowObject::setNumInput(FlowConnection *WXUNUSED(connection),wxFloat64 WXUNUSED(value),wxUint32 *WXUNUSED(flowThreadID),flowObject *WXUNUSED(object))
{
   wxASSERT(0);
   return 0;
}



wxFloat64 flowObject::getNumOutput(FlowConnection *WXUNUSED(connection),wxInt32 *rcode,wxUint64 WXUNUSED(lastInput),wxLongLong WXUNUSED(origCreationTime))
{
   wxASSERT(0);
   *rcode=OAPC_ERROR_NO_SUCH_IO;
   return 0.0;
}



/**
Sets an character input
@param[in] input the flag that specifies the input
@param[in] value the value that has to be set for that input
@param[in] flowThreadID the Id of the thread that wants to set this value
@return 0; this method has to be overridden
*/
wxByte flowObject::setCharInput(FlowConnection *WXUNUSED(connection),wxString WXUNUSED(value),wxUint32 *WXUNUSED(flowThreadID),flowObject *WXUNUSED(object))
{
   wxASSERT(0);
   return 0;
}



wxString flowObject::getCharOutput(FlowConnection *WXUNUSED(connection),wxInt32 *rcode,wxUint64 WXUNUSED(lastInput))
{
   wxASSERT(0);
   *rcode=OAPC_ERROR_NO_SUCH_IO;
   return _T("X-(");
}



wxByte flowObject::setBinInput(FlowConnection *WXUNUSED(connection),oapcBinHeadSp& WXUNUSED(value),wxUint32 *WXUNUSED(flowThreadID),flowObject *WXUNUSED(object))
{
   wxASSERT(0);
   return 0;
}



void flowObject::releaseBinData(FlowConnection *WXUNUSED(connection))
{
   wxASSERT(0);
}



oapcBinHeadSp flowObject::getBinOutput(FlowConnection *WXUNUSED(connection),wxInt32 *WXUNUSED(rcode),wxUint64 WXUNUSED(lastInput))
{
   wxASSERT(0);
   return oapcBinHeadSp();
}

#endif

#endif //ENV_INT
