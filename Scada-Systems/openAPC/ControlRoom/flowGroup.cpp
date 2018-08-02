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
#include "oapc_libio.h"
#include "flowObject.h"
#include "flowGroup.h"
#include "hmiObject.h"
#ifdef ENV_EDITOR
#include "flowConnection.h"
#include "DlgConfigflowGroup.h"
#endif
#include "IOElementDefinition.h"
#include "common.h"

#ifdef ENV_WINDOWS
 #ifdef ENV_WINDOWSCE
  #include "Winsock2.h"
 #endif
#endif



flowGroup::flowGroup(flowGroup *parent,ObjectList *list,wxPoint pos):flowObject(NULL)
{
   returnOK=0;
   this->data.type=hmiObject::FLOW_TYPE_GROUP;
   this->data.id=g_objectList.getUniqueID();
#ifndef ENV_PLAYER
   this->name=wxString::Format(_T("%s %d"),getDefaultName().GetData(),this->data.id); // name is added only when the ID is assigned
   m_categoryFlag=0;
#endif
   this->m_parent=parent;
   this->list=list;
   this->data.flowposx=pos.x;
   this->data.flowposy=pos.y;
   returnOK=1;
}



flowGroup::flowGroup(flowGroup *parent,bool display):flowObject(NULL)
{
   returnOK=0;
   this->data.type=hmiObject::FLOW_TYPE_GROUP;
   if (display) this->data.id=g_objectList.getUniqueID();
#ifndef ENV_PLAYER
   this->name=wxString::Format(_T("%s %d"),getDefaultName().GetData(),this->data.id); // name is added only when the ID is assigned
   m_categoryFlag=0;
#endif
   this->m_parent=parent;
   list=new ObjectList();
   returnOK=1;
}



flowGroup::~flowGroup()
{
   if (list)
   {
      list->deleteAll(1);
      delete list;
   }
#ifdef ENV_EDITOR
   if (flowBitmap) delete flowBitmap;
#endif
   INList.DeleteContents(1);
   //TODO: replace by own delete mechanism, DeleteContents() seems not to work
   OUTList.DeleteContents(1);
   //TODO: replace by own delete mechanism, DeleteContents() seems not to work
}



/**
Checks if an object with the given ID exists within the scope of this group. Comparing
to ObjectList->getObject() this method does not return the object with the ID but a pointer
to itself. That's necessary because the flowGroup itself has to handle all following calls.
@param[in] id identifier of the object to look for
@return pointer to this in case the object was found, NULL otherwise
*/
flowObject *flowGroup::getObject(wxUint32 id)
{
   wxNode              *node;
   IOElementDefinition *IOdef;
   wxByte               i;

   node=INList.GetFirst();
   for (i=0; i<2; i++)
   {
      while (node)
      {
         IOdef=(IOElementDefinition*)node->GetData();
         if (IOdef->data.id==id) return IOdef;
         node=node->GetNext();
      }
      node=OUTList.GetFirst();
   }
   wxASSERT(0); // it should never happe nthat there is no IO definition available for an ID
   return NULL;
}



/** different to the preceding getObject() method this one handles only the list ob grouped objects*/
wxNode *flowGroup::getObject(wxNode *node)
{
   if (!node) return list->GetFirst();
   return node->GetNext();
}



ObjectList *flowGroup::getObjectList()
{
   return list;
}



ObjectList *flowGroup::getINObjectList()
{
   return &INList;
}



ObjectList *flowGroup::getOUTObjectList()
{
   return &OUTList;
}



wxPoint flowGroup::getOutConnectorPos(wxPoint pos)
{
   wxNode              *node;
   IOElementDefinition *IOdef;

   node=OUTList.GetFirst();
   while (node)
   {
      IOdef=(IOElementDefinition*)node->GetData();

      if ((pos.x-getFlowPos().x>=IOdef->data.posx-5)  && (pos.x-getFlowPos().x<=IOdef->data.posx+5) &&
          (pos.y-getFlowPos().y>=IOdef->data.posy-10) && (pos.y-getFlowPos().y<=IOdef->data.posy)) return wxPoint(IOdef->data.posx,IOdef->data.posy);

      node=node->GetNext();
   }
   return wxPoint(-1,-1);
}



#ifdef ENV_EDITOR
wxPoint flowGroup::getInConnectorPos(wxPoint pos)
{
   wxNode              *node;
   IOElementDefinition *IOdef;

   node=INList.GetFirst();
   while (node)
   {
      IOdef=(IOElementDefinition*)node->GetData();

      if ((pos.x-getFlowPos().x>=IOdef->data.posx-5)  && (pos.x-getFlowPos().x<=IOdef->data.posx+5) &&
          (pos.y-getFlowPos().y>=IOdef->data.posy) && (pos.y-getFlowPos().y<=IOdef->data.posy+10)) return wxPoint(IOdef->data.posx,IOdef->data.posy);

      node=node->GetNext();
   }
   return wxPoint(-1,-1);
}



void flowGroup::getMinMaxIDs(wxUint32 &minID,wxUint32 &maxID)
{
   wxInt32     i;
   ObjectList *list=NULL;
   wxNode     *node;

   for (i=0; i<3; i++)
   {
      if (i==0) list=getObjectList();
      else if (i==1) list=getINObjectList();
      else if (i==2) list=getOUTObjectList();

      node=list->getObject((wxNode*)NULL);
      while (node)
      {
         if (i==0)
         {
            flowObject *object;

            object=(flowObject*)node->GetData();
            if (object->data.id<minID) minID=object->data.id;
            if (object->data.id>maxID) maxID=object->data.id;
            if (object->data.type==hmiObject::FLOW_TYPE_GROUP) ((flowGroup*)object)->getMinMaxIDs(minID,maxID);
         }
         else
         {
            IOElementDefinition *object;

            object=(IOElementDefinition*)node->GetData();
            if (object->data.id<minID) minID=object->data.id;
            if (object->data.id>maxID) maxID=object->data.id;
         }
         node=node->GetNext();
      }
   }
}



FlowConnection *flowGroup::getFlowConnection(wxPoint pos,wxInt32 tolerance)
{
   wxNode              *node;
   IOElementDefinition *IOdef;
   FlowConnection      *flowCon;

   node=OUTList.GetFirst();
   while (node)
   {
      IOdef=(IOElementDefinition*)node->GetData();
      flowCon=IOdef->getFlowConnection(pos,tolerance);
      if (flowCon) return flowCon;
      node=node->GetNext();
   }
   return flowObject::getFlowConnection(pos,tolerance);
}



flowGroup *flowGroup::getParentObject()
{
   return m_parent;
}



void flowGroup::setFlowPos(ObjectList *parentList,wxPoint pos,wxByte forcePosition,wxByte checkIncoming)
{
   wxNode              *node;
   IOElementDefinition *IOdef;

   node=OUTList.GetFirst();
   while (node)
   {
      IOdef=(IOElementDefinition*)node->GetData();
  	   IOdef->setFlowPos(list,pos,forcePosition,0);
      node=node->GetNext();
   }
   flowObject::setFlowPos(parentList,pos,forcePosition,checkIncoming);
}




wxInt32 flowGroup::getFlowWidth()
{
    return flowBitmap->GetWidth();
}



wxInt32 flowGroup::getFlowHeight()
{
    return flowBitmap->GetHeight();
}



wxByte flowGroup::setINConnections(flowObject* groupedObject, IOElementDefinition *IOdef,wxPoint groupedObjectPos)
{
   wxNode         *node,*flownode;
   flowObject     *object;
   FlowConnection *connection;
   FlowConnection *continueConnection; // this is the flow connection that is added newly from the IOElement to the original flow target
   wxByte          res=0,loopState=1; // 1 - main objects, 2 - input connectors

   if (m_parent==NULL) node=g_objectList.getObject((wxNode*)NULL);
   else node=((flowGroup*)m_parent)->getObject((wxNode*)NULL);
   while (node)
   {
      object=(flowObject*)node->GetData();
      if (object!=this) // do not compare myself
      {
         flownode=object->getFlowConnection((wxNode*)NULL);
         while (flownode)
         {
            connection=(FlowConnection*)flownode->GetData();
            // check if the current FlowConnection points to my object and input
            if ((connection->data.targetID==groupedObject->data.id) &&
                (connection->data.targetInput & IOdef->data.IOFlag))
            {
               wxPoint pos;

               pos=connection->getLastPos();

               IOdef->data.conPosx=pos.x-3;
               IOdef->data.conPosy=pos.y-100-FLOW_IO_ELEMENT_HEIGHT;

               continueConnection=new FlowConnection();
               wxASSERT(continueConnection);
               if (continueConnection)
               {
                  continueConnection->data.targetInput=connection->data.targetInput;
                  continueConnection->data.targetID=connection->data.targetID;
                  continueConnection->data.sourceOutput=IOdef->data.IOFlag;
                  continueConnection->data.usedEdges=3;
                  continueConnection->data.linex[0]=pos.x; continueConnection->data.liney[0]=pos.y-100;
                  continueConnection->data.linex[1]=pos.x; continueConnection->data.liney[1]=pos.y-50;
                  continueConnection->data.linex[2]=pos.x; continueConnection->data.liney[2]=pos.y-50;
                  continueConnection->data.linex[3]=pos.x; continueConnection->data.liney[3]=pos.y;
                  IOdef->addFlowConnection(continueConnection);
                  res=1;
               }

               connection->data.targetInput=IOdef->data.id; // let the targetInput point to the IOElementDefinition identifier
               connection->data.targetID=this->data.id;     // modify the id of the FlowConnection to point to the group
               pos.x=groupedObjectPos.x;
               pos.y=groupedObjectPos.y;
               connection->setLastPos(pos);
               pos=connection->getPrevPos();
               pos.x=groupedObjectPos.x;
               connection->setPrevPos(pos);

               // add connection from IOdef to flowObject
            }
            flownode=flownode->GetNext();
         }
      }
      node=node->GetNext();
      if ((!node) && (loopState==1) && (m_parent)) // in case we're already within a group we also have to check for incoming input connector connections
      {
         loopState=2;
         node=((flowGroup*)m_parent)->getINObjectList()->getObject((wxNode*)NULL);
      }
   }
   return res;
}



/**
This method creates the out connection data structures
@param[in] groupedObject the object that is put into the group and where the outgoing connections have to be processed
@param[in] IODef the input/output definition intermediate object for that object and its connections to outside of the group
@param[in] groupedObjectPos the position of the group symbol
@return 0 if no outgoing connector object was created for the output that is specified by the given IODef object or 1
        if there has been an outgoing connection from this output
*/
wxByte flowGroup::setOUTConnections(flowObject* groupedObject, IOElementDefinition *IOdef,wxPoint groupedObjectPos)
{
   FlowConnection *connection;
   wxNode         *node;
   wxPoint         pos,pos0;

   node=groupedObject->getFlowConnection(NULL);
   while (node)
   {
      connection=(FlowConnection*)node->GetData();

      if ((!list->getObject(IOdef->data.id)) && 
          (!list->getObject(connection->data.targetID)) && 
          (connection->data.sourceOutput==IOdef->data.IOFlag))
      {
         pos=connection->getPosAt(0);
         pos0=pos;
         pos.x=groupedObjectPos.x;
         pos.y=groupedObjectPos.y;
         connection->setPosAt(0,pos);
         pos=connection->getPosAt(1);
         pos.x=groupedObjectPos.x;
         connection->setPosAt(1,pos);

         // move the outgoing flow connection from the grouped object to the group
         groupedObject->removeFlowConnection(connection);
         IOdef->addFlowConnection(connection);
         node=groupedObject->getFlowConnection(NULL);
      }
      else node=groupedObject->getFlowConnection(node);
   }

   if ((pos0.x>0) && (pos0.y>0)) // add connectors only for existing connections, not for unused outputs
   {
      connection=new FlowConnection();
      wxASSERT(connection);
      if (connection) 
      {
         connection->data.targetID=IOdef->data.id;
         connection->data.targetInput=IOdef->data.IOFlag; // this flag doesn't matters, the IODef object has a non-ambiguous assignment to an output
         connection->data.sourceOutput=IOdef->data.IOFlag;

         connection->data.usedEdges=3;
         connection->data.linex[0]=pos0.x; connection->data.liney[0]=pos0.y;
         connection->data.linex[1]=pos0.x; connection->data.liney[1]=pos0.y+50;
         connection->data.linex[2]=pos0.x; connection->data.liney[2]=pos0.y+50;
         connection->data.linex[3]=pos0.x; connection->data.liney[3]=pos0.y+100;

         IOdef->data.conPosx=pos0.x-3;
         IOdef->data.conPosy=pos0.y+100;

         groupedObject->addFlowConnection(connection);
      }
      return 1;
   }
   return 0;
}



static int flowGroup_PosSortCompareFunction(const void *elem1, const void *elem2)
{
   if ((**(flowObject**)elem1).getFlowPos().x<(**(flowObject**)elem2).getFlowPos().x) return -1;
   else if ((**(flowObject**)elem1).getFlowPos().x>(**(flowObject**)elem2).getFlowPos().x) return 1;
   else return 0;
}



static int flowGroup_OutputSortCompareFunction(const void *elem1, const void *elem2)
{
   if ((**(flowObject**)elem1).data.stdOUT!=0) return -1;
   else if ((**(flowObject**)elem2).data.stdOUT!=0) return 1;
   else return 0;
}



bool flowGroup::isOutputUsed(flowObject *object,wxUint64 bit)
{
   FlowConnection *connection;
   wxNode         *node=NULL;

   node=object->getFlowConnection(node);
   while (node)
   {
      connection=(FlowConnection*)node->GetData();

      if ((connection->data.sourceOutput & bit)==bit) return true;
      node=object->getFlowConnection(node);
   }
   return false;
}


void flowGroup::createFlowBitmap()
{
   flowObject          *object;
   wxNode              *node;
   wxUint64             bit;
   wxUint32             num=0,posCtrIN=1,posCtrOUT=1;
   wxBitmap            *IObm;
   IOElementDefinition *IOdef;
   wxSize               textSize;
   wxInt32              numIN,numOUT,width,correctPosX;
   wxFont               font;

   // count the number of IOs
   numIN=INList.GetCount();
   numOUT=OUTList.GetCount();
   if ((numIN==0) && (numOUT==0))
   {
      node=list->getObject((wxNode*)NULL);
      wxASSERT(node);
      while (node)
      {
         object=(flowObject*)node->GetData();
         bit=0;
#ifdef ENV_WINDOWS
         while (bit<0x8000000000000000)
#else
         while (bit<0x8000000000000000LL)
#endif
         {
            if (bit==0) bit=1;
            else bit=bit<<1;

            if ((object->data.stdIN & bit)==bit) numIN++;
            if (((object->data.stdOUT & bit)==bit) && (isOutputUsed(object,bit))) numOUT++;
         }
         node=list->getObject(node);
      }
   }

   // create an empty bitmap
   if (numIN>numOUT) width=2+(numIN*13);
   else width=2+(numOUT*13);
   if (width<FLOW_OBJECT_WIDTH) width=FLOW_OBJECT_WIDTH;
   if (flowBitmap) delete flowBitmap;
   flowBitmap=new wxBitmap(width,FLOW_OBJECT_HEIGHT*3,-1);

   if ((OUTList.GetCount()==0) && (INList.GetCount()==0)) // re-create the bitmap in case all data are already available
   {
      list->Sort(flowGroup_PosSortCompareFunction);
      list->Sort(flowGroup_OutputSortCompareFunction);
      node=list->getObject((wxList::compatibility_iterator)NULL);
      wxASSERT(node);
      while (node)
      {
        object=(flowObject*)node->GetData();
        bit=0;
#ifdef ENV_WINDOWS
        while (bit<0x8000000000000000)
#else
        while (bit<0x8000000000000000LL)
#endif
        {
           if (bit==0) bit=1;
           else bit=bit<<1;

           IOdef=NULL;
           if ((object->data.stdIN & bit)==bit)
           {
              if (m_parent) IOdef=new IOElementDefinition(1,m_parent->getObjectList(),getFlowPos());
              else IOdef=new IOElementDefinition(1,&g_objectList,getFlowPos());
              IOdef->data.IOFlag=bit;
              IOdef->data.id=g_objectList.getUniqueID();
#if wxCHECK_VERSION(2,9,0)
              IOdef->name=wxString::Format(_T("%s %d"),object->name.Mid(0,6),num);
#else
              IOdef->name=wxString::Format(_T("%s %d"),object->name.Mid(0,6).c_str(),num);
#endif
              IOdef->data.posx=posCtrIN+6;
              IOdef->data.posy=0;

              if (setINConnections(object,IOdef,getFlowPos()+wxPoint(posCtrIN+6,0)))
              {
                 INList.addObject(IOdef,true,false);
                 posCtrIN+=13;
              }
              else delete (IOdef);

           }
           if ((object->data.stdOUT & bit)==bit)
           {
              if (m_parent) IOdef=new IOElementDefinition(0,m_parent->getObjectList(),getFlowPos());
              else IOdef=new IOElementDefinition(0,&g_objectList,getFlowPos());
              IOdef->data.IOFlag=bit;
              IOdef->data.id=g_objectList.getUniqueID();
#if wxCHECK_VERSION(2,9,0)
              IOdef->name=wxString::Format(_T("%s %d"),object->name.Mid(0,6),num);
#else
              IOdef->name=wxString::Format(_T("%s %d"),object->name.Mid(0,6).c_str(),num);
#endif
              IOdef->data.posx=posCtrOUT+6;
              IOdef->data.posy=getFlowHeight();
              
              if (setOUTConnections(object,IOdef,getFlowPos()+wxPoint(posCtrOUT+6,getFlowHeight())))
              {
                 OUTList.addObject(IOdef,true,false);
                 posCtrOUT+=13;
              }
              else delete IOdef;

           }
           num++;
           if (num==8) num=0;
        }
        node=list->getObject(node);
      }
   }

   // fill the bitmap
   wxASSERT(flowBitmap);
   if (!flowBitmap) return;
   wxMemoryDC dc(*flowBitmap);
   dc.SetBrush(*wxWHITE_BRUSH);
   dc.SetPen(*wxBLACK);
   dc.SetFont(*wxSWISS_FONT);
   font=dc.GetFont();
#ifdef ENV_WINDOWS
   font.SetPointSize(7);
#else
   font.SetPointSize(8);
#endif
   dc.SetFont(font);

   dc.DrawRectangle(0,0,getFlowWidth(),getFlowHeight());

   correctPosX=7;
   node=INList.GetFirst();
   while (node)
   {
      IOdef=(IOElementDefinition*)node->GetData();

      IObm=NULL;
      if (IOdef->data.posx!=correctPosX)
      {
         wxNode         *inNode,*conNode;
         flowObject     *inObject;
         FlowConnection *inConnection;
         wxPoint         pos;

         if (m_parent==NULL) inNode=g_objectList.getObject((wxNode*)NULL);
         else inNode=m_parent->getObject((wxNode*)NULL);
         while (inNode)
         {
            inObject=(flowObject*)inNode->GetData();

            conNode=inObject->getFlowConnection((wxNode*)NULL);
            while (conNode)
            {
               inConnection=(FlowConnection*)conNode->GetData();
               if ((inConnection->data.targetID==data.id) && (inConnection->data.targetInput==IOdef->data.id))
               {
                  pos=inConnection->getLastPos(); pos.x-=(IOdef->data.posx-correctPosX); inConnection->setLastPos(pos);
                  pos=inConnection->getPrevPos(); pos.x-=(IOdef->data.posx-correctPosX); inConnection->setPrevPos(pos);
               }
               conNode=inObject->getFlowConnection(conNode);
            }
            if (m_parent==NULL) inNode=g_objectList.getObject(inNode);
            else inNode=m_parent->getObject(inNode);
         }
         IOdef->data.posx=correctPosX;
      }
      if (IOdef->data.IOFlag & OAPC_DIGI_IO_MASK) IObm=new wxBitmap(_T(DATA_PATH"img/group_digiin.png"),wxBITMAP_TYPE_PNG);
      else if (IOdef->data.IOFlag & OAPC_NUM_IO_MASK) IObm=new wxBitmap(_T(DATA_PATH"img/group_numin.png"),wxBITMAP_TYPE_PNG);
      else if (IOdef->data.IOFlag & OAPC_CHAR_IO_MASK) IObm=new wxBitmap(_T(DATA_PATH"img/group_charin.png"),wxBITMAP_TYPE_PNG);
      else if (IOdef->data.IOFlag & OAPC_BIN_IO_MASK) IObm=new wxBitmap(_T(DATA_PATH"img/group_binin.png"),wxBITMAP_TYPE_PNG);
      dc.DrawBitmap(*IObm,IOdef->data.posx-6,IOdef->data.posy,0);
      if (IObm) delete (IObm);

      textSize=dc.GetTextExtent(IOdef->name);
#ifdef ENV_WINDOWS
      dc.DrawRotatedText(IOdef->name,IOdef->data.posx-7,IOdef->data.posy+12+textSize.x,90);
#else
      dc.DrawRotatedText(IOdef->name,IOdef->data.posx-5,IOdef->data.posy+12+textSize.x,90);
#endif
      correctPosX+=13;
      node=node->GetNext();
   }

   correctPosX=7;
   node=OUTList.GetFirst();
   while (node)
   {
      IOdef=(IOElementDefinition*)node->GetData();

      IObm=NULL;
      if (IOdef->data.posx!=correctPosX)
      {
         wxNode         *conNode;
         FlowConnection *outConnection;
         wxPoint         pos;

         conNode=IOdef->getFlowConnection((wxNode*)NULL);
         while (conNode)
         {
            outConnection=(FlowConnection*)conNode->GetData();
            pos=outConnection->getPosAt(0); pos.x-=(IOdef->data.posx-correctPosX); outConnection->setPosAt(0,pos);
            pos=outConnection->getPosAt(1); pos.x-=(IOdef->data.posx-correctPosX); outConnection->setPosAt(1,pos);
            conNode=IOdef->getFlowConnection(conNode);
         }

         IOdef->data.posx=correctPosX;
      }
      if (IOdef->data.IOFlag & OAPC_DIGI_IO_MASK) IObm=new wxBitmap(_T(DATA_PATH"img/group_digiout.png"),wxBITMAP_TYPE_PNG);
      else if (IOdef->data.IOFlag & OAPC_NUM_IO_MASK) IObm=new wxBitmap(_T(DATA_PATH"img/group_numout.png"),wxBITMAP_TYPE_PNG);
      else if (IOdef->data.IOFlag & OAPC_CHAR_IO_MASK) IObm=new wxBitmap(_T(DATA_PATH"img/group_charout.png"),wxBITMAP_TYPE_PNG);
      else if (IOdef->data.IOFlag & OAPC_BIN_IO_MASK) IObm=new wxBitmap(_T(DATA_PATH"img/group_binout.png"),wxBITMAP_TYPE_PNG);
      else wxASSERT(0);


      wxASSERT(IObm);
      if (IObm)
      {
         dc.DrawBitmap(*IObm,IOdef->data.posx-6,IOdef->data.posy-8,0);
         delete (IObm);
      }

#ifdef ENV_WINDOWS
      dc.DrawRotatedText(IOdef->name,IOdef->data.posx-7,IOdef->data.posy-10,90);
#else
      dc.DrawRotatedText(IOdef->name,IOdef->data.posx-5,IOdef->data.posy-10,90);
#endif

      correctPosX+=13;
      node=node->GetNext();
   }
}




wxUint64 flowGroup::getOutConnectorType(wxPoint pos)
{
   wxNode              *node;
   IOElementDefinition *IOdef;

   node=OUTList.GetFirst();
   while (node)
   {
      IOdef=(IOElementDefinition*)node->GetData();

      if ((pos.x-getFlowPos().x>=IOdef->data.posx-5)  && (pos.x-getFlowPos().x<=IOdef->data.posx+5) &&
          (pos.y-getFlowPos().y>=IOdef->data.posy-10) && (pos.y-getFlowPos().y<=IOdef->data.posy)) return IOdef->data.IOFlag;

      node=node->GetNext();
   }
   return 0;
}



wxUint32 flowGroup::getInConnectorType(wxPoint pos)
{
   wxNode              *node;
   IOElementDefinition *IOdef;

   node=INList.GetFirst();
   while (node)
   {
      IOdef=(IOElementDefinition*)node->GetData();

      if ((pos.x-getFlowPos().x>=IOdef->data.posx-5)  && (pos.x-getFlowPos().x<=IOdef->data.posx+5) &&
          (pos.y-getFlowPos().y>=IOdef->data.posy) && (pos.y-getFlowPos().y<=IOdef->data.posy+10)) return IOdef->data.IOFlag;

      node=node->GetNext();
   }
   return 0;
}



IOElementDefinition *flowGroup::getInConnectorObject(wxPoint pos)
{
   wxNode              *node;
   IOElementDefinition *IOdef;

   node=INList.GetFirst();
   while (node)
   {
      IOdef=(IOElementDefinition*)node->GetData();

      if ((pos.x-getFlowPos().x>=IOdef->data.posx-5)  && (pos.x-getFlowPos().x<=IOdef->data.posx+5) &&
          (pos.y-getFlowPos().y>=IOdef->data.posy) && (pos.y-getFlowPos().y<=IOdef->data.posy+10)) return IOdef;

      node=node->GetNext();
   }
   return NULL;
}




wxString flowGroup::getDefaultName()
{
    return _T("Group");
}



/**
Saves a complete flow group object in IFF format
@param[in] FHandle handle to the open file to save the data into
@param[in] saveOutgoing specifies if the outgoing flow connections have to be saved (e.g. for project data) or
           not (e.g. when only a single group object is saved)
@return the number of saved data
*/
wxInt32  flowGroup::saveFGRP(wxFile *FHandle,wxByte saveOutgoing)
{
   wxFileOffset            lengthPos,lastFlowChunkPos;
   struct groupFlowData    flowData;
   wxInt32                 length,l;
   wxNode                 *node;
   IOElementDefinition    *IOdef;
   wxMBConvUTF16BE         conv;

   if (!FHandle) return 0;

   FHandle->Write(CHUNK_FLOW_FGRP,4); // FlowGRouP this is not a standard chunk, it consists of several subchunks
   length=0;
   FHandle->Write(&length,4);
   lengthPos=FHandle->Tell()-4;

   // save general group data
   FHandle->Write(CHUNK_FLOW_FGRP_GRPD,4); // GRouPData
   length=sizeof(struct groupFlowData)/*+(flowList.GetCount()*sizeof(struct flowConnectionData))*/;
   length=htonl(length);
   FHandle->Write(&length,4);

   wxASSERT(data.id>0);
   flowData.flowData.length   =htonl(sizeof(struct groupFlowData));
   flowData.flowData.id       =htonl(data.id);
   flowData.flowData.version  =htonl(1);
   flowData.flowData.usedFlows=htonl(flowList.GetCount());
   flowData.flowData.maxEdges =htonl(MAX_CONNECTION_EDGES);
   flowData.flowX             =htonl(getFlowPos().x);
   flowData.flowY             =htonl(getFlowPos().y);
   flowData.custIN=0;
   flowData.custOUT=0;
   flowData.dataFlowFlags     =htonl(data.dataFlowFlags);
   flowData.flags             =htonl(data.hmiFlags);
   flowData.type              =htonl(data.type);
   flowData.categoryFlag      =htonl(m_categoryFlag);
   flowData.reserved1=0;
   flowData.reserved2=0;
   flowData.reserved3=0;
   flowData.reserved4=0;
   conv.WC2MB(flowData.store_name,name,sizeof(flowData.store_name));
   conv.WC2MB(flowData.store_descr,descr,sizeof(flowData.store_descr));

   FHandle->Write(&flowData,sizeof(struct groupFlowData));

/*   node=flowList.GetFirst();
   while (node)
   {
      connection=(FlowConnection*)node->GetData();
      connection->saveFlow(FHandle);
      node=node->GetNext();
   }*/

   //save input/output conector data
   node=INList.GetFirst();
   while (node)
   {
      IOdef=(IOElementDefinition*)node->GetData();
      IOdef->saveFlowChunk((char*)CHUNK_FLOW_FGRP_GRPI,FHandle,1); // GRouPInputs
      node=node->GetNext();
   }
   node=OUTList.GetFirst();
   while (node)
   {
      IOdef=(IOElementDefinition*)node->GetData();
      IOdef->saveFlowChunk((char*)CHUNK_FLOW_FGRP_GRPO,FHandle,saveOutgoing); // GRouPOutputs
      node=node->GetNext();
   }

   FHandle->Write(CHUNK_FLOW,4); //flow connections list ***************************************************
   length=0;
   FHandle->Write(&length,4);
   lastFlowChunkPos=FHandle->Tell()-4;

   list->saveFlowObjectData(FHandle,false);

   l=htonl(FHandle->Tell()-lastFlowChunkPos-4); // write length of FLOW chunk
   FHandle->Seek(lastFlowChunkPos,wxFromStart);
   FHandle->Write(&l,4);
   FHandle->SeekEnd();

   length=htonl(FHandle->Tell()-lengthPos-4); // write length of FGRP chunk
   FHandle->Seek(lengthPos);
   FHandle->Write(&length,4);
   FHandle->SeekEnd();
   return ntohl(length);
}
#endif



wxInt32 flowGroup::loadFGRP(wxFile *FHandle,wxInt32 topChunkSize,wxUint32 IDOffset,bool display,ObjectList *topLevelObjectList)
{
   wxInt32               loaded=0;
   char                  rawData[5];
   wxInt32               chunkSize,readSize;
   struct groupFlowData  flowData;
   IOElementDefinition  *IOdef=NULL;
#if defined ENV_EDITOR || defined ENV_DEBUGGER
   wchar_t               buf[MAX_TEXT_LENGTH];
#endif
   rawData[4]=0;

   while (topChunkSize>0)
   {
      readSize=FHandle->Read(rawData,4);
      readSize+=FHandle->Read(&chunkSize,4);
      if (readSize!=8) return 0;
      topChunkSize-=8;
      loaded+=8;

      chunkSize=ntohl(chunkSize);
      topChunkSize-=chunkSize;
      loaded+=chunkSize;

      if (chunkSize>0)
      {
         if (!strncmp(rawData,CHUNK_FLOW_FGRP_GRPD,4))
         {
            wxMBConvUTF16BE             conv;

            chunkSize-=FHandle->Read(&flowData,sizeof(struct groupFlowData));
            flowData.flowData.version=  ntohl(flowData.flowData.version);
            if (IDOffset==0) data.id   =ntohl(flowData.flowData.id);
            else
            {
               wxASSERT(display);
               if (display) data.id    =ntohl(flowData.flowData.id)-IDOffset+g_objectList.currentUniqueID()+1;
            }
            flowData.flowData.usedFlows=ntohl(flowData.flowData.usedFlows);
            flowData.flowData.maxEdges= ntohl(flowData.flowData.maxEdges);
#ifdef ENV_EDITOR
            if (m_parent) setFlowPos(m_parent->getObjectList(),wxPoint(ntohl(flowData.flowX),ntohl(flowData.flowY)),1,1);
            else
            {
//               wxASSERT(display);
               if (display) setFlowPos(&g_objectList,wxPoint(ntohl(flowData.flowX),ntohl(flowData.flowY)),1,1);
            }
            m_categoryFlag             =ntohl(flowData.categoryFlag);
#endif
            data.dataFlowFlags         =ntohl(flowData.dataFlowFlags);
            data.hmiFlags              =ntohl(flowData.flags);
            data.type                  =ntohl(flowData.type);
#if defined ENV_EDITOR || defined ENV_DEBUGGER
            conv.MB2WC(buf,flowData.store_name,sizeof(buf));
            name=buf;
#endif
#ifdef ENV_EDITOR
            conv.MB2WC(buf,flowData.store_descr,sizeof(buf));
            descr=buf;
#endif
            chunkSize-=flowObject::loadFlow(FHandle,&flowData.flowData,IDOffset,false,false);
            wxASSERT(chunkSize==0);
         }
         else if (!strncmp(rawData,CHUNK_FLOW_FGRP_GRPI,4))
         {
#ifdef ENV_EDITOR
            if (m_parent) IOdef=new IOElementDefinition(1,m_parent->getObjectList(),getFlowPos());
            else
            {
               IOdef=new IOElementDefinition(1,topLevelObjectList,getFlowPos());
            }
#else
            IOdef= new IOElementDefinition();
#endif
            chunkSize-=IOdef->loadFlowChunk(FHandle,IDOffset,display,false);
            INList.addObject(IOdef,display,IDOffset>0);
            wxASSERT(chunkSize==0);
         }
         else if (!strncmp(rawData,CHUNK_FLOW_FGRP_GRPO,4))
         {
#ifdef ENV_EDITOR
            if (m_parent) IOdef=new IOElementDefinition(0,m_parent->getObjectList(),getFlowPos());
            else
            {
               IOdef=new IOElementDefinition(0,topLevelObjectList,getFlowPos());
            }
#else
            IOdef= new IOElementDefinition();
#endif
            chunkSize-=IOdef->loadFlowChunk(FHandle,IDOffset,display,false);
            OUTList.addObject(IOdef,display,IDOffset>0);
            wxASSERT(chunkSize==0);
         }
         else if (!strncmp(rawData,CHUNK_FLOW,4))
         {
            bool hasISCOElement=false; //unused here but required by loadFlowObjectData

            chunkSize=list->loadFlowObjectData(FHandle,chunkSize,IDOffset,NULL,display,false,topLevelObjectList,OBJECT_MODE_FLOWELEMENT,&hasISCOElement); // all other chunks must belong to the object list
            wxASSERT(chunkSize==0);
         }
         else wxASSERT(0);
         FHandle->Seek(chunkSize,wxFromCurrent);
      }
      else return -1;
   }
   return loaded;
}



void flowGroup::addFlowConnection(FlowConnection *connection)
{
   wxNode *node;
   IOElementDefinition *IOdef;

   node=OUTList.GetFirst();
   while (node)
   {
      IOdef=(IOElementDefinition*)node->GetData();
      if (getOutConnectorPos(wxPoint(connection->data.linex[0]/*-data.flowposx*/,connection->data.liney[0]/*-data.flowposy*/))==wxPoint(IOdef->data.posx,IOdef->data.posy))
      {
         IOdef->addFlowConnection(connection);
         return;
      }
      node=node->GetNext();
   }
}



wxNode *flowGroup::getFlowConnection(wxNode *node)
{
   static wxNode              *outlistnode=NULL;
          wxNode              *connectionnode;
   static IOElementDefinition *IOdef=NULL;

   if (!node)
   {
      outlistnode=OUTList.GetFirst();
      if (!outlistnode) return NULL;
      IOdef=(IOElementDefinition*)outlistnode->GetData();
      connectionnode=IOdef->getFlowConnection(NULL);
   }
   else connectionnode=IOdef->getFlowConnection(node);
   while (!connectionnode)
   {
      outlistnode=outlistnode->GetNext();
      if (!outlistnode) return NULL;
      IOdef=(IOElementDefinition*)outlistnode->GetData();
      connectionnode=IOdef->getFlowConnection(NULL);
   }

   return connectionnode;
}





void flowGroup::deleteFlowConnection(FlowConnection *connection)
{
    INList.DeleteObject(connection);
    OUTList.DeleteObject(connection);
    delete connection;
}



#ifdef ENV_EDITOR
bool flowGroup::removeFlowConnection(FlowConnection *connection)
{
   INList.DeleteObject(connection);
   OUTList.DeleteObject(connection);
   return true;
}




void flowGroup::drawFlowConnections(wxAutoBufferedPaintDC *dc,FlowConnection *ignoreFlowCon,wxInt32 x, wxInt32 y)
{
    wxNode         *node;
    FlowConnection *connection;

    node=getFlowConnection(NULL);
    while (node)
    {
        connection=(FlowConnection*)node->GetData();
        if (connection!=ignoreFlowCon) connection->drawFlowConnection(dc,NULL,x,y);
        node=getFlowConnection(node);
    }
}



void flowGroup::drawFlowConnections(wxAutoBufferedPaintDC *dc,wxUint32 ignoreOutConnector,wxInt32 x, wxInt32 y)
{
    wxNode         *node;
    FlowConnection *connection;

    node=getFlowConnection(NULL);
    while (node)
    {
        connection=(FlowConnection*)node->GetData();
        if (connection->data.sourceOutput!=ignoreOutConnector) connection->drawFlowConnection(dc,NULL,x,y);
        node=getFlowConnection(node);
    }
}



void flowGroup::doDataFlowDialog(bool WXUNUSED(hideISConfig))
{
   DlgConfigflowGroup dlg(this,NULL,_("Group Definitions"));
   dlg.ShowModal();
   if (dlg.returnOK)
   {
   }
   dlg.Destroy();
}
#endif






