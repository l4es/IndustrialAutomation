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

#ifndef __WXMSW__
#include <arpa/inet.h>
#endif

#include "iff.h"
#include "globals.h"
#include "oapc_libio.h"
#include "IOElementDefinition.h"
#include "hmiObject.h"

#ifdef ENV_WINDOWS
 #ifdef ENV_WINDOWSCE
  #include "Winsock2.h"
 #endif
#endif



wxUint32 IOElementDefinition::FLAG_NOT_EXTERNALLY_VISIBLE=0x0001;

#ifdef ENV_EDITOR
IOElementDefinition::IOElementDefinition(wxByte isInConnector,ObjectList *parentList,wxPoint pos):flowObject(NULL)
#else
IOElementDefinition::IOElementDefinition():flowObject(NULL)
#endif
{
   data.type=hmiObject::FLOW_TYPE_GROUP_IO_DEFINITION;
   ((flowObject*)this)->data.type=hmiObject::FLOW_TYPE_GROUP_IO_DEFINITION; // dirty hack: here we assign a value to the hidden field of the base class, it is used by the flow walker threas
   data.IOFlag=0;
   data.flags=0;
   data.id=0;
   name=_T("");
   data.posx=-1;    data.posy=-1;
   data.conPosx=-1; data.conPosy=-1;
#ifdef ENV_EDITOR
   m_isInConnector=isInConnector;
   setFlowPos(parentList,pos,1,0);
   m_nameField=NULL;
#else
   binStorage=oapcBinHeadSp();
#endif
}



#ifdef ENV_EDITOR
void IOElementDefinition::doDataFlowDialog(bool WXUNUSED(hideISConfig))
{
   wxString result;

#if wxCHECK_VERSION(2,9,0)
   result=result.Format(_T("%s (%d)"),_("I/O Definition"),data.id);
#else
   result=result.Format(_T("%s (%d)"),_("I/O Definition").c_str(),data.id);
#endif
   result=wxGetTextFromUser(result,_("Name"),name,NULL);
   if (result.Length()>0)
   {
      if (result.Length()>10) result=result.Mid(0,10);
      name=result;
      g_isSaved=false;
   }
}



/**
Gets the type of the out-connector. Here a reverse logic is true: the out-connector is an incoming
connector from the groups point of view but it is outgoing of the connector object
@param[in] pos the position where we have to look for a connector
@return the type of the connector or 0 if there is none at this position
*/
wxUint32 IOElementDefinition::getOutConnectorType(wxPoint pos)
{
   if (!m_isInConnector) return 0;

   if ((pos.x>=data.conPosx-3) && (pos.x<=data.conPosx+6) &&
       (pos.y>=data.conPosy+getFlowHeight()-5) && (pos.y<=data.conPosy+getFlowHeight())) return data.IOFlag;
   return 0; 
}



/**
Gets the type of the in-connector. Here a reverse logic is true: the in-connector is an
outgoing connector from the groups point of view but it is incoming on the connector object
@param[in] pos the position where we have to look for a connector
@return the type of the connector or 0 if there is none at this position
*/
wxUint32 IOElementDefinition::getInConnectorType(wxPoint pos)
{
   if (m_isInConnector) return 0;

   if ((pos.x>=data.conPosx-3) && (pos.x<=data.conPosx+6) &&
       (pos.y>=data.conPosy) &&   (pos.y<=data.conPosy+5)) return data.IOFlag;
   return 0; 
}



/**
Gets the exact position of the out-connector. Here a reverse logic is true: the out-connector
is an incoming connector from the groups point of view but it is outgoing of the connector object
@param[in] pos the position where we have to look for a connector
@return the exact position (relative to the top left position of the element itself) where the
        flow line has to start or -1,-1 if no connector at this position
*/
wxPoint IOElementDefinition::getOutConnectorPos(wxPoint pos)
{
   if (!m_isInConnector) return wxPoint(-1,-1);

   if ((pos.x>=data.conPosx-3) && (pos.x<=data.conPosx+6) &&
       (pos.y>=data.conPosy+getFlowHeight()-5) && (pos.y<=data.conPosy+getFlowHeight())) return wxPoint(getFlowWidth()/2,getFlowHeight());
   return wxPoint(-1,-1);
}



/**
Gets the type of the in-connector. Here a reverse logic is true: the in-connector is an
outgoing connector from the groups point of view but it is incoming on the connector object
@param[in] pos the position where we have to look for a connector
@return the exact position (relative to the top left position of the element itself) where
        the flow line has to end or -1,-1 if no connector at this position
*/
wxPoint IOElementDefinition::getInConnectorPos(wxPoint pos)
{
   if (m_isInConnector) return wxPoint(-1,-1);

   if ((pos.x>=data.conPosx-3) && (pos.x<=data.conPosx+6) &&
       (pos.y>=data.conPosy) &&   (pos.y<=data.conPosy+5)) return wxPoint(getFlowWidth()/2,0);
   return wxPoint(-1,-1);
}



/**
Sets the position of the incoming flow lines to keep then in-place during an flow element is dragged
@param[in] pos the new position of the element
@param[in] forcePosition the given position is used without any calculation
*/
void IOElementDefinition::setConnectorPos(ObjectList *parentList,wxPoint pos,wxByte forcePosition,wxByte checkIncoming)
{
    wxNode         *node;
    FlowConnection *connection;
    wxPoint         linePos0,linePos1,linePos2;

    if (forcePosition)
    {
        data.conPosx=pos.x;
        data.conPosy=pos.y;
        return;
    }
    if (checkIncoming)
    {
       node=flowList.GetFirst();
       while (node)
       {
           connection=(FlowConnection*)node->GetData();
           linePos0=connection->getPosAt(0);
           linePos1=connection->getPosAt(1);
           linePos0.x+=pos.x-data.conPosx;
           linePos0.y+=pos.y-data.conPosy;
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
              linePos1.x+=pos.x-data.conPosx;
              connection->setPosAt(1,linePos1);
           }
           else
           {
              linePos1.y+=pos.y-data.conPosy;
              connection->setPosAt(1,linePos1);
           }

          node=node->GetNext();
       }
    }
//    flowObject::setFlowPos(parentList,pos,forcePosition);
    setIncomingConnectionPos(parentList,pos-wxPoint(data.conPosx,data.conPosy));
    data.conPosx=pos.x;
    data.conPosy=pos.y;
}



void IOElementDefinition::setIncomingConnectionPos(ObjectList *parentList,wxPoint delta)
{
    wxNode              *node,*objNode;
    IOElementDefinition *srcObject;
    FlowConnection      *connection;
    wxPoint              linePos;

    if (!parentList) return;
    //TODO: chache these information after the first call from DRAG and flush the chache at the end
    objNode=parentList->GetFirst();
    while (objNode)
    {
       srcObject=(IOElementDefinition*)objNode->GetData();
       if (srcObject!=this)
       {
          node=srcObject->getFlowConnection(NULL);
          while (node)
          {
             connection=(FlowConnection*)node->GetData();
             if (connection->data.targetID==data.id)
             {
                linePos=connection->getLastPos();
                linePos+=delta;
                connection->setLastPos(linePos);
   
                linePos=connection->getPrevPos();
                linePos.x+=delta.x;
                connection->setPrevPos(linePos);
             }
             node=srcObject->getFlowConnection(node);
          }
       }
       objNode=objNode->GetNext();
    }
}


wxPoint IOElementDefinition::getFlowPos()
{
    return wxPoint(data.conPosx,data.conPosy);
}



wxInt32 IOElementDefinition::getFlowWidth()
{
    return FLOW_IO_ELEMENT_WIDTH;
}



wxInt32 IOElementDefinition::getFlowHeight()
{
    return FLOW_IO_ELEMENT_HEIGHT;
}



/**
Saves a chunk for an IOElementDefinition object in an open IFF file
@param[in] chunk name of the chunk that has to be used for identifying this object - it is different
           for incoming and outgoing connection objects
@param[in] FHandle handle to an opened file to save the data into
@param[in] saveOutgoing specifies if the outgoing flow connections have to be saved for this object (1)
           or not (0)
@return the number of bytes saved
*/
wxInt32 IOElementDefinition::saveFlowChunk(char *chunkName,wxFile *FHandle,wxByte saveOutgoing)
{
   struct groupIOFlowData flowData;
   wxInt32                length;
   wxMBConvUTF16BE        conv;

   if (!FHandle) return 0;

   FHandle->Write(chunkName,4);
   if (saveOutgoing) length=sizeof(struct groupIOFlowData)+(flowList.GetCount()*sizeof(struct flowConnectionData));
   else length=sizeof(struct groupIOFlowData);
   length=htonl(length);
   FHandle->Write(&length,4);

   flowData.flowData.id       =htonl(data.id);
   flowData.flowData.version  =htonl(1);
   if (saveOutgoing) flowData.flowData.usedFlows=htonl(flowList.GetCount());
   else flowData.flowData.usedFlows=htonl(0);
   flowData.flowData.maxEdges =htonl(MAX_CONNECTION_EDGES);
   flowData.id                =htonl(data.id);
   flowData.type              =htonl(data.type);
   flowData.IOFlag            =htonl(data.IOFlag);
   flowData.flags             =htonl(data.flags);
   flowData.posx              =htonl(data.posx);
   flowData.posy              =htonl(data.posy);
   flowData.conPosx           =htonl(data.conPosx);
   flowData.conPosy           =htonl(data.conPosy);

   conv.WC2MB(flowData.store_name,name,sizeof(flowData.store_name));
   length=FHandle->Write(&flowData,sizeof(struct groupIOFlowData));

   if (saveOutgoing)
   {
      wxNode         *node;
      FlowConnection *connection;

      node=flowList.GetFirst();
      while (node)
      {
         connection=(FlowConnection*)node->GetData();
         length+=connection->saveFlow(FHandle,false);
         node=node->GetNext();
      }
   }
   return length;
}



void IOElementDefinition::drawFlowConnections(wxAutoBufferedPaintDC *dc,FlowConnection *ignoreFlowCon,wxPoint pos,wxBitmap *bm)
{
    wxNode         *node;
    wxSize          textSize;
    FlowConnection *connection;

    dc->DrawBitmap(*bm,data.conPosx,data.conPosy);

    textSize=dc->GetTextExtent(name);
    dc->DrawRotatedText(name,getFlowPos().x-textSize.y+4,getFlowPos().y-1+getFlowHeight()+textSize.x,90);

    node=flowList.GetFirst();
    while (node)
    {
        connection=(FlowConnection*)node->GetData();
        if (connection!=ignoreFlowCon) connection->drawFlowConnection(dc,NULL,pos.x,pos.y);

        node=node->GetNext();
    }
}



void IOElementDefinition::drawFlowConnections(wxAutoBufferedPaintDC *dc,wxUint32 ignoreOutputConnector,wxPoint pos,wxBitmap *bm)
{
    wxNode         *node;
    wxSize          textSize;
    FlowConnection *connection;

    dc->DrawBitmap(*bm,data.conPosx,data.conPosy);

    textSize=dc->GetTextExtent(name);
    dc->DrawRotatedText(name,getFlowPos().x-textSize.y+4,getFlowPos().y-1+getFlowHeight()+textSize.x,90);

    node=flowList.GetFirst();
    while (node)
    {
        connection=(FlowConnection*)node->GetData();
        if (connection->data.sourceOutput!=ignoreOutputConnector) connection->drawFlowConnection(dc,NULL,pos.x,pos.y);

        node=node->GetNext();
    }
}
#endif



wxInt32 IOElementDefinition::loadFlowChunk(wxFile *FHandle,wxUint32 IDOffset,bool display,bool isCompiled)
{
   struct groupIOFlowData flowData;
   wxInt32                loaded;
   wxMBConvUTF16BE        conv;
   wchar_t                buf[MAX_NAME_LENGTH];

   wxASSERT(!isCompiled);
   isCompiled=isCompiled;
   loaded=FHandle->Read(&flowData,sizeof(struct groupIOFlowData));

//   data.id=ntohl(flowData.flowData.id);
   flowData.flowData.usedFlows     =ntohl(flowData.flowData.usedFlows);
   flowData.flowData.maxEdges      =ntohl(flowData.flowData.maxEdges);
   if (IDOffset==0)
   {
      data.id        =ntohl(flowData.id);
#ifdef ENV_EDITOR
      if (display) g_objectList.updateUniqueID(data.id);
#else
      display=display;
#endif
   }
   else
    data.id          =ntohl(flowData.id)-IDOffset+g_objectList.currentUniqueID()+1;
   data.type         =ntohl(flowData.type);
   data.IOFlag       =ntohl(flowData.IOFlag);
   data.flags        =ntohl(flowData.flags);
   data.posx         =ntohl(flowData.posx);
   data.posy         =ntohl(flowData.posy);
   data.conPosx      =ntohl(flowData.conPosx);
   data.conPosy      =ntohl(flowData.conPosy);

   conv.MB2WC(buf,flowData.store_name,sizeof(buf));
   name=buf;

   loaded+=flowObject::loadFlow(FHandle,&flowData.flowData,IDOffset,false,false);
   return loaded;
}



#ifdef ENV_PLAYER
wxUint64 IOElementDefinition::getAssignedOutput(wxUint64 WXUNUSED(input))
{
   return data.IOFlag;
}



wxByte IOElementDefinition::setDigitalInput(FlowConnection *WXUNUSED(connection),wxByte value,wxUint32 *WXUNUSED(flowThreadID),flowObject *WXUNUSED(object))
{
   if (value) digiStorage=1;
   else digiStorage=0;
   return 1;
}



wxByte IOElementDefinition::getDigitalOutput(FlowConnection *WXUNUSED(connection),wxInt32 *rcode,wxUint64 WXUNUSED(lastInput),wxLongLong WXUNUSED(origCreationTime))
{
   *rcode=OAPC_OK;
   return digiStorage;
}



wxByte IOElementDefinition::setNumInput(FlowConnection *WXUNUSED(connection),wxFloat64 value,wxUint32 *WXUNUSED(flowThreadID),flowObject *WXUNUSED(object))
{
   numStorage=value;
   return 1;
}



wxFloat64 IOElementDefinition::getNumOutput(FlowConnection *WXUNUSED(connection),wxInt32 *rcode,wxUint64 WXUNUSED(lastInput),wxLongLong WXUNUSED(origCreationTime))
{
   *rcode=OAPC_OK;
   return numStorage;
}



wxByte IOElementDefinition::setCharInput(FlowConnection *WXUNUSED(connection),wxString value,wxUint32 *WXUNUSED(flowThreadID),flowObject *WXUNUSED(object))
{
   charStorage=value;
   return 1;
}



wxString IOElementDefinition::getCharOutput(FlowConnection *WXUNUSED(connection),wxInt32 *rcode,wxUint64 WXUNUSED(lastInput))
{
   *rcode=OAPC_OK;
   return charStorage;
}



wxByte IOElementDefinition::setBinInput(FlowConnection *WXUNUSED(connection),oapcBinHeadSp &value,wxUint32 *WXUNUSED(flowThreadID),flowObject *WXUNUSED(object))
{
   binStorage=value;
   return 1;
}



oapcBinHeadSp IOElementDefinition::getBinOutput(FlowConnection *WXUNUSED(connection),wxInt32 *rcode,wxUint64 WXUNUSED(lastInput))
{
   *rcode=OAPC_OK;
   return binStorage;
}



#endif
