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

#include "PCompiler.h"
#include "flowObject.h"
#include "flowConnection.h"
#include "ObjectList.h"
#include "IOElementDefinition.h"
#include "flowGroup.h"
#include "globals.h"



PCompiler::PCompiler()
{
    SDBG
}



PCompiler::~PCompiler()
{
    SDBG
}



FlowConnection *PCompiler::findValidTarget(FlowConnection *connection,flowObject *nextObject,FlowConnection *resetConnection)
{
   FlowConnection *iodefConnection;
   wxNode         *ioNode;

   SDBG
   wxASSERT(nextObject);
   if (!nextObject) return NULL;
   SDBG
   while ((nextObject->data.type==flowObject::FLOW_TYPE_GROUP) || 
          (nextObject->data.type==flowObject::FLOW_TYPE_GROUP_IO_DEFINITION))
   {
      if (nextObject->data.type==flowObject::FLOW_TYPE_GROUP)
      {
         nextObject=m_objectList->getObject(connection->data.targetInput);
         wxASSERT(nextObject);
         if (!nextObject) return NULL;
      }
      if (nextObject->data.type==flowObject::FLOW_TYPE_GROUP_IO_DEFINITION)
      {
         ioNode=((IOElementDefinition*)nextObject)->getFlowConnection((wxNode*)NULL);
         while (ioNode)
         {
            iodefConnection=(FlowConnection*)ioNode->GetData();
            if (iodefConnection->data.targetID!=0)
            {
               FlowConnection *resConnection;
               flowObject     *resObject;

               resObject=m_objectList->getObject(iodefConnection->data.targetID);
               wxASSERT(resObject);
               if (!resObject) return NULL;
               if ((resObject->data.type==flowObject::FLOW_TYPE_GROUP) || 
                   (resObject->data.type==flowObject::FLOW_TYPE_GROUP_IO_DEFINITION))
               {
                  resConnection=findValidTarget(iodefConnection,resObject,resetConnection);
                  if (resConnection)
                  {
                     if (!resetConnection) resetConnection=iodefConnection; // pointer to the last connector connection - to be reseted
                     return resConnection;
                  }
               }
               else return iodefConnection;
            }
            ioNode=((IOElementDefinition*)nextObject)->getFlowConnection(ioNode);
         }
         return NULL;
      }
   }
   wxASSERT(0);
   wxASSERT(1);
   return NULL;
}



void PCompiler::shortenConnections(flowObject *srcObject,FlowConnection *srcConnection,FlowConnection *parentConnection)
{
   wxNode              *cNode;
   flowObject          *trgObject;
   FlowConnection      *newConnection;

   SDBG
   trgObject=m_objectList->getObject(parentConnection->data.targetID);
   if (!trgObject) return;
   if (trgObject->data.type==flowObject::FLOW_TYPE_GROUP)
   {
      // fetch the IOElementDefinition that belongs to the group
      trgObject=((flowGroup*)trgObject)->getINObjectList()->getObject(parentConnection->data.targetInput);
      cNode=trgObject->getFlowConnection((wxNode*)NULL);
      while (cNode)
      {
         shortenConnections(srcObject,srcConnection,(FlowConnection*)cNode->GetData());
         cNode=trgObject->getFlowConnection(cNode);
      }
   }
   else if (trgObject->data.type==flowObject::FLOW_TYPE_GROUP_IO_DEFINITION)
   {
      cNode=trgObject->getFlowConnection((wxNode*)NULL);
      while (cNode)
      {
         shortenConnections(srcObject,srcConnection,(FlowConnection*)cNode->GetData());
         cNode=trgObject->getFlowConnection(cNode);
      }
   }
   else
   {
      if (parentConnection!=srcConnection)
      {
         newConnection=new FlowConnection();
         newConnection->data.targetID=trgObject->data.id;
         newConnection->data.targetInput=parentConnection->data.targetInput;
         newConnection->data.sourceOutput=srcConnection->data.sourceOutput;
#ifdef ENV_PLAYER
         newConnection->setFlowIONumbers();
#endif
         srcObject->addFlowConnection(newConnection);
         srcConnection->data.targetID=0; // this is the connection object of the origin, so we can disable it here
      }
   }
}



void PCompiler::removeIOElementDefinitions(ObjectList *olist)
{
   wxNode              *node;
   flowObject          *object;
   
   SDBG
   node=olist->getObject((wxNode*)NULL);
   while (node)
   {
      object=(flowObject*)node->GetData();
      if (object->data.type==flowObject::FLOW_TYPE_GROUP)
      {
         ((flowGroup*)object)->getINObjectList()->deleteAll(true);
         ((flowGroup*)object)->getOUTObjectList()->deleteAll(true);
         removeIOElementDefinitions(((flowGroup*)object)->getObjectList());
      }
      node=olist->getObject(node);
   }
}



void PCompiler::removeOrphanFlowConnections(ObjectList *olist)
{
   wxNode              *node,*cnode;
   flowObject          *object;
   FlowConnection      *connection,*delConnection=NULL;
   
   SDBG
   node=olist->getObject((wxNode*)NULL);
   while (node)
   {
      object=(flowObject*)node->GetData();
      if (object->data.type==flowObject::FLOW_TYPE_GROUP)
      {
         wxASSERT(0); // should never happen because groups are resolved before
         removeOrphanFlowConnections(((flowGroup*)object)->getObjectList());
      }
      cnode=object->getFlowConnection((wxNode*)NULL);
      while (cnode)
      {
         connection=(FlowConnection*)cnode->GetData();
         if (connection->data.targetID==0) delConnection=connection;
         cnode=object->getFlowConnection(cnode);
         if (delConnection)
         {
            object->deleteFlowConnection(delConnection);
            delConnection=NULL;
         }
      }
      node=olist->getObject(node);
   }
}



void PCompiler::resolveGroupStructure(ObjectList *olist)
{
   wxNode              *node;
   flowObject          *object,*resObject=NULL,*delObject=NULL;
   
   SDBG
   node=olist->getObject((wxNode*)NULL);
   while (node)
   {
      object=(flowObject*)node->GetData();
      if (object->data.type==flowObject::FLOW_TYPE_GROUP)
      {
         resolveGroupStructure(((flowGroup*)object)->getObjectList());
         ((flowGroup*)object)->getObjectList()->deleteAll(false); // flush the list because their objects already have been linked to other lists
         delObject=object;
      }
      else if (olist!=m_objectList) resObject=object;
      node=olist->getObject(node);
      if (resObject)
      {
         m_objectList->addObject(resObject,false,false);
         resObject=NULL;
      }
      if (delObject) // remove nested groups
      {
         olist->deleteObject(delObject,false,true);
         delObject=NULL;
      }
   }
}



void PCompiler::recurseStartingObjects(ObjectList *olist)
{
   wxNode              *node;
   flowObject          *object;
   wxNode              *cNode;

   SDBG
   node=olist->getObject((wxNode*)NULL);
   while (node)
   {
      object=(flowObject*)node->GetData();
      if (object->data.type==flowObject::FLOW_TYPE_GROUP)
      {
         recurseStartingObjects(((flowGroup*)object)->getObjectList());
      }
      else
      {
         cNode=object->getFlowConnection((wxNode*)NULL);
         while (cNode)
         {
            shortenConnections(object,(FlowConnection*)cNode->GetData(),(FlowConnection*)cNode->GetData());
            cNode=object->getFlowConnection(cNode);
         }
      }
      node=olist->getObject(node);
   }
}



void PCompiler::compile(ObjectList *olist)
{
    SDBG
   m_objectList=olist;
   recurseStartingObjects(olist);     // to shorten the connections
   removeIOElementDefinitions(olist); // not necessary because groups are deleted fully in a later step?
   resolveGroupStructure(olist);
   removeOrphanFlowConnections(olist);
}
