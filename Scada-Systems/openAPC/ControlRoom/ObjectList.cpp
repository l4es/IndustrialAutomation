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
 #include "../ObjectList.cpp"
#else //ENV_INT

#include <wx/wx.h>
#include <wx/file.h>
#include <wx/mstream.h>
#include <wx/zipstrm.h>
#include <wx/stdpaths.h>

#ifdef ENV_LINUX
#include <arpa/inet.h>
#endif

#include "iff.h"
#include "oapc_libio.h"
#include "globals.h"
#include "ObjectList.h"
#include "MainWin.h"
#include "flowExternalIOLib.h"
#include "flowGroup.h"
#include "IOElementDefinition.h"
#include "UserList.h"
#include "PlugInPanel.h"

#include "flowConverterDigi2Num.h"
#include "flowConverterDigi2Char.h"
#include "flowConverterChar2Num.h"
#include "flowConverterChar2Digi.h"
#include "flowConverterNum2Digi.h"
#include "flowConverterNum2Bits.h"
#include "flowConverterNum2Char.h"
#include "flowConverterMixed2Char.h"
#include "flowConverterDigi2Pair.h"
#include "flowConverterNum2Pair.h"
#include "flowConverterChar2Pair.h"
#include "flowConverterPair2Char.h"
#include "flowConverterPair2Digi.h"
#include "flowConverterPair2Num.h"
#include "flowConverterPair2Bin.h"
#include "flowConverterBin2Pair.h"
#include "flowLogicDigiRSFF.h"
#include "flowLogicDigiTFF.h"
#include "flowLogicDigiNOT.h"
#include "flowLogicNumNOT.h"
#include "flowLogicDigiNOP.h"
#include "flowLogicNumNOP.h"
#include "flowLogicCharNOP.h"
#include "flowLogicBinNOP.h"
#include "flowLogicDigiConcat.h"
#include "flowLogicNumConcat.h"
#include "flowLogicDigiShift.h"
#include "flowLogicNumShift.h"
#include "flowLogicCharShift.h"
#include "flowLogicBinShift.h"
#include "flowMathDigiCmp.h"
#include "flowMathNumCmp.h"
#include "flowMathCharCmp.h"
#include "flowMathDigiCtr.h"
#include "flowMathNumCtr.h"
#include "flowMathCharCtr.h"
#include "flowMathBinCtr.h"
#include "flowStop.h"
#include "flowDelay.h"
#include "flowStart.h"
#include "flowTimer.h"
#include "flowLog.h"
#include "flowLogRecorder.h"
#include "flowLogInUser.h"
#include "flowNumTrigGate.h"
#include "flowNumGate.h"
#include "flowCharTrigGate.h"
#include "flowCharGate.h"
#include "flowBinTrigGate.h"
#include "flowBinGate.h"
#include "flowDigiTrigGate.h"
#include "flowDigiGate.h"
#include "flowISConnect.h"
#include "PCompiler.h"
#ifndef ENV_HPLAYER
 #include "hmiObject.h"
 #include "hmiSimpleButton.h"
 #include "hmiImageButton.h"
 #include "hmiToggleImageButton.h"
 #include "hmiToggleButton.h"
 #include "hmiRadioButton.h"
 #include "hmiHSlider.h"
 #include "hmiVSlider.h"
 #include "hmiHGauge.h"
 #include "hmiVGauge.h"
 #include "hmiNumField.h"
 #include "hmiTextField.h"
 #include "hmiFloatField.h"
 #include "hmiTextLabel.h"
 #include "hmiLine.h"
 #include "hmiImage.h"
 #include "hmiPrimitive.h"
 #include "hmiTabbedPane.h"
 #include "hmiStackedPane.h"
 #include "hmiAdditionalPane.h"
 #include "hmiSinglePanel.h"
 #include "hmiAngularMeter.h"
 #include "hmiAngularRegulator.h"
 #include "hmiLCDNumber.h"
 #include "hmiSpecialPanel.h"
 #include "hmiExternalIOLib.h"
 #include "hmiUserMgmntPanel.h"

 #include "oapcSpinCtrl.h"
#endif

#ifdef ENV_WINDOWS
 #ifdef ENV_WINDOWSCE
  #include "Winsock2.h"
 #endif
#endif


ObjectList::ObjectList(flowGroup *owner)
           :wxList()
{
   init();
//   handleID=0;
   m_owner=owner;
#ifndef ENV_HPLAYER
   parent=NULL;
#endif
   m_projectData=NULL;
}



ObjectList::ObjectList()
           :wxList()
{
   init();
//   handleID=0;
#ifndef ENV_HPLAYER
   parent=NULL;
#endif
   m_owner=NULL;
   m_projectData=NULL;
}



ObjectList::~ObjectList()
{
#ifndef ENV_HPLAYER
   hmiMutex *mutex;

   while ((this==&g_objectList) && (g_objectList.m_mutexList.size()>0))
   {
      mutex=g_objectList.m_mutexList.front();
      g_objectList.m_mutexList.pop_front();
      delete mutex;
   }
#endif
   if (m_projectData) free (m_projectData);
}



void ObjectList::init()
{
   pos.x=-1;  pos.y=-1;
   size.x=-1; size.y=-1;
   name.Clear();
   m_lastID=0;
}


#ifndef ENV_HPLAYER
ObjectList::ObjectList(HMICanvas *parent/*,wxByte handleID*/)
           :wxList()
{
   init();
   this->parent=parent;
//   this->handleID=handleID;
   m_owner=NULL;
   m_projectData=NULL;
}



#ifdef ENV_PLAYER
void ObjectList::setUserPrivileges(UserData *userData)
{
   wxNode     *node;
   flowObject *object;
   wxInt32     i;
   bool        priviFound;

   if (!g_userPriviData.enabled) return; // for backward compatibility, lett all be enabled in case no user privis are used
   node=getObject((wxNode*)NULL);
   while (node)
   {
      object=(flowObject*)node->GetData();
      if ((object->data.type & HMI_TYPE_MASK)!=0)
      {
         priviFound=false;
         if ((userData) && ((userData->m_canDo & hmiUserMgmntPanel::priviFlagMask[0])==hmiUserMgmntPanel::priviFlagEnabled[0]))
         {
            // supervisor logged in
            ((hmiObject*)object)->setUIElementState(2);
            priviFound=true;
         }
         else if (userData)
         {
            for (i=1; i<MAX_PRIVI_NUM; i++)
            {
               if ((userData->m_canDo & hmiUserMgmntPanel::priviFlagMask[i])==hmiUserMgmntPanel::priviFlagEnabled[i])
               {
                  if ((object->data.userPriviFlags & hmiUserMgmntPanel::priviFlagMask[i])==hmiUserMgmntPanel::priviFlagEnabled[i])
                  {
                     ((hmiObject*)object)->setUIElementState(2);
                     priviFound=true;
                     break;
                  }
                  else if ((object->data.userPriviFlags & hmiUserMgmntPanel::priviFlagMask[i])==hmiUserMgmntPanel::priviFlagDisabled[i])
                  {
                     ((hmiObject*)object)->setUIElementState(1);
                     priviFound=true;
                     break;
                  }
                  else if ((object->data.userPriviFlags & hmiUserMgmntPanel::priviFlagMask[i])==hmiUserMgmntPanel::priviFlagInvisible[i])
                  {
                     ((hmiObject*)object)->setUIElementState(0);
                     priviFound=true;
                     break;
                  }
               }
            }
         }
         if (!priviFound)
         {
            // default user / logged out
            if ((object->data.userPriviFlags & hmiUserMgmntPanel::priviFlagMask[0])==hmiUserMgmntPanel::priviFlagEnabled[0])
            {
               ((hmiObject*)object)->setUIElementState(2);
               priviFound=true;
            }
            else if ((object->data.userPriviFlags & hmiUserMgmntPanel::priviFlagMask[0])==hmiUserMgmntPanel::priviFlagDisabled[0])
            {
               ((hmiObject*)object)->setUIElementState(1);
               priviFound=true;
            }
            else if ((object->data.userPriviFlags & hmiUserMgmntPanel::priviFlagMask[0])==hmiUserMgmntPanel::priviFlagInvisible[0])
            {
               ((hmiObject*)object)->setUIElementState(0);
               priviFound=true;
            }
            if (!priviFound) ((hmiObject*)object)->setUIElementState(2); // enable the elemets by default in case all is set to ignored or user management is disabled
         }
      }
      node=node->GetNext();
   }
   g_hmiCanvas->Refresh();
}
#endif



hmiMutex *ObjectList::getMutexByName(wxString name)
{
   list<hmiMutex*>::iterator it;

   for ( it=g_objectList.m_mutexList.begin(); it!=g_objectList.m_mutexList.end(); it++)
   {
      if (!(*it)->m_name.compare(name)) return *it;
   }
   return NULL;
}



/**
* Find a hmiMutex object according to the ID of the object that is managed
* by this mutex
*/
hmiMutex *ObjectList::getMutexByObjectID(wxInt32 id)
{
   list<hmiMutex*>::iterator it;

   for ( it=g_objectList.m_mutexList.begin(); it!=g_objectList.m_mutexList.end(); it++)
   {
      if ((*it)->hasObjectID(id)) return *it;
   }
   return NULL;
}
#endif


/** replacement for the original function which doesn't seems to work...*/
void ObjectList::DeleteContents(bool destroy)
{
   wxNode     *node;
   flowObject *object;

   wxASSERT(!d_isCompiling || (this!=&g_objectList));
   node=GetFirst();
   while (node)
   {
      object=(flowObject*)node->GetData();
      DeleteNode(node);
      if (destroy)
      {
         if (object->data.isChildOf!=0) object->m_doDestroy=0; // child objects are deleted by their parents
         delete object;
      }
      node=GetFirst();
   }
}


#ifndef ENV_HPLAYER
void ObjectList::setParent(BasePanel *parent)
{
   wxASSERT(!d_isCompiling || (this!=&g_objectList));
   this->parent=parent;
}



hmiObject *ObjectList::getUIElement(wxInt32 wxId,wxUint32 parentID,wxPoint pos)
{
   wxNode *node;

   wxASSERT(!d_isCompiling || (this!=&g_objectList));
   pos.x*=1000;
   pos.y*=1000;
   node = GetFirst();
   while (node)
   {
        hmiObject *current =(hmiObject*)node->GetData();

        if ((current->data.type & HMI_TYPE_MASK) && ((current)->uiElement) && ((current)->uiElement->GetId()==wxId))
        {
           if ((parentID==current->data.isChildOf) && (current->data.type!=HMI_TYPE_ADDITIONALPANE)) return current;
        }
        node = node->GetNext();
    }
    node = GetFirst();
    while (node)
    {
        hmiObject *current =(hmiObject*)node->GetData();

        if (current->data.type & HMI_TYPE_MASK)
        {
           if ((current->getPos().x<=pos.x) && (current->getPos().x+current->getSize().x>=pos.x) &&
               (current->getPos().y<=pos.y) && (current->getPos().y+current->getSize().y>=pos.y))
           {
            if ((parentID==current->data.isChildOf) && (current->data.type!=HMI_TYPE_ADDITIONALPANE)) return current;
           }
        }
        node = node->GetNext();
    }
    return NULL;
}


wxRealPoint ObjectList::getSingleSize()
{
   if (getFirstObject()) return wxRealPoint(((hmiObject*)getFirstObject())->getSize().x/1000.0,((hmiObject*)getFirstObject())->getSize().y/1000.0);
   return wxRealPoint();
}
#endif



#ifdef ENV_EDITOR
wxPoint ObjectList::getSingleMinSize()
{
   if (getFirstObject()) return ((hmiObject*)getFirstObject())->getMinSize();
   return wxPoint();
}



wxPoint ObjectList::getSingleMaxSize()
{
   if (getFirstObject()) return ((hmiObject*)getFirstObject())->getMaxSize();
   return wxPoint();
}
#endif



void ObjectList::setSingleSize(wxRealPoint newSize)
{
   wxNode *node;

   newSize.x*=1000.0;
   newSize.y*=1000.0;
   node = GetFirst();
   if (node)
   {
      hmiObject *current =(hmiObject*)node->GetData();
      current->setSize(newSize);

      size=newSize;
   }
}



wxRealPoint ObjectList::getTotalSize()
{
   return size;
}



/*void ObjectList::setTotalSize(wxRealPoint newSize,wxInt32 anchor)
{
    wxFloat32   factorX,factorY;
    wxRealPoint oldSize,oldPos,setSize;

    factorX=1.0*newSize.x/size.x;
    factorY=1.0*newSize.y/size.y;

    wxList::compatibility_iterator node = GetFirst();
    while (node)
    {
        hmiObject *current =(hmiObject*)node->GetData();

        oldSize=current->getSize();
        setSize.x=oldSize.x*factorX;
        setSize.y=oldSize.y*factorY;
        current->setSize(setSize);

        oldPos=current->getPos();
        if ((anchor & TOTAL_SIZE_ANCHOR_LEFT) && ((wxInt32)oldPos.x>pos.x)) oldPos.x=((oldPos.x-pos.x)*factorX)+pos.x;
        else if ((anchor & TOTAL_SIZE_ANCHOR_RIGHT) && ((wxInt32)(oldPos.x+oldSize.x)<(pos.x+size.x)))
                                                                            oldPos.x=(pos.x+size.x)-(((pos.x+size.x)-oldPos.x)*factorX);
        if ((anchor & TOTAL_SIZE_ANCHOR_TOP) &&  ((wxInt32)oldPos.y>pos.y)) oldPos.y=((oldPos.y-pos.y)*factorY)+pos.y;
        else if ((anchor & TOTAL_SIZE_ANCHOR_BOTTOM) && ((wxInt32)(oldPos.y+oldSize.y)<(pos.y+size.y)))
                                                                            oldPos.y=(pos.y+size.y)-(((pos.y+size.y)-oldPos.y)*factorY);
        current->setPos(oldPos);

        node = node->GetNext();
    }
    size=newSize;
}*/



wxRealPoint ObjectList::getTotalPos()
{
    return pos;
}



void ObjectList::setTotalPos(wxRealPoint newPos)
{
    wxNode *node;

    newPos.x*=1000.0;
    newPos.y*=1000.0;
    node = GetFirst();
    while (node)
    {
        hmiObject *current =(hmiObject*)node->GetData();
        current->setPos(current->getPos()+newPos-pos);
        node = node->GetNext();
    }
    pos=newPos;
}



void ObjectList::deleteAll(bool deleteData)
{
#ifndef ENV_HPLAYER
   hmiMutex                 *mutex;

   if (this==&g_objectList) hmiMutex::s_lastID=0;
   while ((this==&g_objectList) && (g_objectList.m_mutexList.size()>0))
   {
      mutex=g_objectList.m_mutexList.front();
      g_objectList.m_mutexList.pop_front();
      delete mutex;
   }
#endif
   wxASSERT(!d_isCompiling || (this!=&g_objectList));
   if (deleteData) DeleteContents(true);
   else wxList::Clear();
   pos.x=-1;  pos.y=-1;
   size.x=-1; size.y=-1;
}



void ObjectList::Clear()
{
    deleteAll(0);
}



void ObjectList::Append(ObjectList *wlist)
{
   wxList::compatibility_iterator node;

   wxASSERT(!d_isCompiling || (this!=&g_objectList));
   node=wlist->getObject((wxList::compatibility_iterator)NULL);
   while (node)
   {
       addObject((hmiObject*)node->GetData(),true,false);
       node=wlist->getObject(node);
   }
}



void ObjectList::addObject(flowObject* object,bool display,bool keepID)
{
   wxASSERT(!d_isCompiling || (this!=&g_objectList));
   if (!object) return;
   wxList::Append(object);
#ifdef ENV_EDITOR
   if (display) handleObjectSize(object,keepID);
#else
   keepID=keepID;
   display=display;
#endif
}



void ObjectList::Insert(flowObject* object)
{
   wxASSERT(!d_isCompiling || (this!=&g_objectList));
   wxASSERT(object);
   if (!object) return;
   wxList::Insert(object);
#ifdef ENV_EDITOR
   handleObjectSize(object,false);
#endif
}


wxUint32 ObjectList::getUniqueID()
{
   wxASSERT(!d_isCompiling);
   m_lastID++;
   return m_lastID;
}



wxUint32 ObjectList::currentUniqueID()
{
   wxASSERT(!d_isCompiling);
   return m_lastID;
}



#ifdef ENV_EDITOR
void ObjectList::InsertBefore(wxNode *posNode,flowObject* object)
{
   wxNode *node;

   node=GetFirst();
   while (node)
   {
      if (node->GetData()==posNode->GetData())
      {
         wxList::Insert(node,object);
         return;
      }
      node=node->GetNext();
   }
   assert(0);
}


void ObjectList::InsertAfter(wxNode *posNode,flowObject* object)
{
   wxNode *node;

   node=GetFirst();
   while (node)
   {
      if (node->GetData()==posNode->GetData())
      {
         node=node->GetNext();
         if (node) wxList::Insert(node,object);
         else wxList::Append(object);
         return;
      }
      node=node->GetNext();
   }
   assert(0);
}


wxInt32 ObjectList::getCount(wxUint32 mask,wxUint32 mode)
{
   wxUint32 cnt=0;

   wxASSERT(!d_isCompiling || (this!=&g_objectList));
   wxList::compatibility_iterator node = GetFirst();
   while (node)
   {
       flowObject *current =(hmiObject*)node->GetData();

       if ((current->data.type & mask)!=0)
       {
          if ((mode==0) || (current->getMode()==mode))
           cnt++;
       }
       node = node->GetNext();
   }
   return cnt;
}



wxRect ObjectList::getBorder(wxInt32 dist)
{
    return wxRect(pos.x/1000-dist,pos.y/1000-dist,size.x/1000+dist+dist,size.y/1000+dist+dist);
}



void ObjectList::moveFlowPos(wxPoint newPos,wxPoint beyondPos)
{
    wxNode  *node,*connectionNode;
    wxInt32  i;
    bool     skipFlows=false;

    node = GetFirst();
    while (node)
    {
        flowObject *current =(flowObject*)node->GetData();
        if ((current->data.flowposx>=0) && (current->data.flowposy>=0))
        {
           if (current->data.type==flowObject::FLOW_TYPE_GROUP_IO_DEFINITION)
           {
              IOElementDefinition *IODef=(IOElementDefinition*)current;

              if (IODef->data.conPosx>beyondPos.x) IODef->data.conPosx+=newPos.x;
              if (IODef->data.conPosy>beyondPos.y) IODef->data.conPosy+=newPos.y;
              if (!IODef->m_isInConnector) skipFlows=true;
           }
           else
           {
              if (current->data.flowposx>beyondPos.x) current->data.flowposx+=newPos.x;
              if (current->data.flowposy>beyondPos.y) current->data.flowposy+=newPos.y;
           }
           if (!skipFlows)
           {
              connectionNode=current->getFlowConnection(NULL);
              while (connectionNode)
              {
          	     FlowConnection *connection=(FlowConnection*)connectionNode->GetData();
           	     for (i=0; i<=connection->data.usedEdges; i++)
                 {
          	        if (connection->data.linex[i]>beyondPos.x) connection->data.linex[i]+=newPos.x;
                    if (connection->data.liney[i]>beyondPos.y) connection->data.liney[i]+=newPos.y;
                 }
                 connectionNode=current->getFlowConnection(connectionNode);
              }
           }
        }
        node = node->GetNext();
    }
}



void ObjectList::updateUniqueID(wxUint32 newID)
{
   wxASSERT(!d_isCompiling);
   if (newID>m_lastID) m_lastID=newID+1;
}



void ObjectList::handleObjectSize(flowObject* object,bool keepID)
{
//    if (handleID)
    {
       if (object->data.type!=flowObject::FLOW_TYPE_GROUP_IO_DEFINITION)
       {
          if (object->data.id<=0) object->data.id=g_objectList.getUniqueID();
          else if (!keepID) g_objectList.updateUniqueID(object->data.id);
       }
       else
       {
          if (((IOElementDefinition*)object)->data.id<=0) ((IOElementDefinition*)object)->data.id=g_objectList.getUniqueID();
          else if (!keepID) g_objectList.updateUniqueID(((IOElementDefinition*)object)->data.id);
       }
#ifdef ENV_EDITOR
       if (object->name.IsEmpty())
       {
          wxString name;
          if (object->data.type!=flowObject::FLOW_TYPE_GROUP_IO_DEFINITION)
          {
             name=object->getDefaultName();
             name.Replace(_T(" "),_T("_"));
#if wxCHECK_VERSION(2,9,0)
             if (object->name.IsEmpty()) object->name=wxString::Format(_T("%s_%d"),name,object->data.id);
#else
             if (object->name.IsEmpty()) object->name=wxString::Format(_T("%s_%d"),name.c_str(),object->data.id);
#endif
          }
       }
#endif
    }
    if (object->data.type & HMI_TYPE_MASK)
    {
       if (object->data.type==HMI_TYPE_ADDITIONALPANE) object=g_objectList.getObject(object->data.isChildOf);
       if (size.x<0)
       {
          size.x=((hmiObject*)object)->getSize().x;
          size.y=((hmiObject*)object)->getSize().y;
          pos.x=((hmiObject*)object)->getPos().x;
          pos.y=((hmiObject*)object)->getPos().y;
       }
       else
       {
          if (((hmiObject*)object)->getPos().x<pos.x)
          {
             size.x+=pos.x-((hmiObject*)object)->getPos().x;
             pos.x=((hmiObject*)object)->getPos().x;
          }
          if (((hmiObject*)object)->getPos().y<pos.y)
          {
             size.y+=pos.y-((hmiObject*)object)->getPos().y;
             pos.y=((hmiObject*)object)->getPos().y;
          }
          if (((hmiObject*)object)->getPos().x+((hmiObject*)object)->getSize().x>pos.x+size.x)
           size.x=(((hmiObject*)object)->getPos().x+((hmiObject*)object)->getSize().x)-pos.x;
          if (((hmiObject*)object)->getPos().y+((hmiObject*)object)->getSize().y>pos.y+size.y)
           size.y=(((hmiObject*)object)->getPos().y+((hmiObject*)object)->getSize().y)-pos.y;
       }
    }
}
#endif



void ObjectList::deleteObjects(ObjectList list,bool unlinkConnections,bool destroyObjects)
{
   flowObject     *object;
   wxNode         *node;

   wxASSERT(!d_isCompiling || (this!=&g_objectList));
   node= list.GetFirst();
   while (node)
   {
      object=(flowObject*)node->GetData();
      deleteObject(object,unlinkConnections,destroyObjects);
      node = node->GetNext();
   }
}



void ObjectList::deleteObject(flowObject *object,bool unlinkConnections,bool destroyObject)
{
#ifdef ENV_EDITOR
   wxNode         *flowNode;
   FlowConnection *flowCon;
#endif
   flowObject     *cmpObject;
   wxNode         *cmpNode;

   wxASSERT(!d_isCompiling || (this!=&g_objectList));
   if (!object) return;
   DeleteObject(object);
#ifdef ENV_EDITOR
   if (unlinkConnections)
   {
      cmpNode=getObject((wxNode*)NULL);
      while (cmpNode)
      {
         cmpObject=(flowObject*)cmpNode->GetData();
         if (cmpObject!=object)
         {
            flowNode=cmpObject->getFlowConnection(NULL);
            while (flowNode)
            {
               flowCon=(FlowConnection*)flowNode->GetData();
               if (flowCon->data.targetID==object->data.id)
                flowCon->data.targetID=0;
               flowNode=cmpObject->getFlowConnection(flowNode);
            }
         }
         cmpNode=getObject(cmpNode);
      }
   }
#else
   unlinkConnections=unlinkConnections;
#endif
   if (destroyObject)
   {
#ifndef ENV_HPLAYER
      if (object->data.type==HMI_TYPE_ADDITIONALPANE)
      {
         flowObject *parentPane;

         parentPane=(flowObject*)getObject(object->data.isChildOf);
         if (parentPane)
         {
            if (parentPane->data.type==HMI_TYPE_TABBEDPANE)
             ((hmiTabbedPane*)parentPane)->deletePanel(((hmiAdditionalPane*)object)->uiElement);
            else if (parentPane->data.type==HMI_TYPE_STACKEDPANE)
             ((hmiStackedPane*)parentPane)->deletePanel(((hmiAdditionalPane*)object)->uiElement);
         }
      }
#endif
      cmpNode=getObject((wxNode*)NULL);
      while (cmpNode)
      {
         cmpObject=(flowObject*)cmpNode->GetData();
         if (cmpObject->data.isChildOf==object->data.id)
         {
            deleteObject(cmpObject,unlinkConnections,destroyObject);
            cmpNode=getObject((wxNode*)NULL);
         }
         else cmpNode=getObject(cmpNode);
      }
      delete object;
   }
}



wxNode *ObjectList::getDrawableObject(wxList::compatibility_iterator node)
{
   flowObject *object;

   if (!node) node = GetFirst();
   else node=node->GetNext();
   while (node)
   {
      object=(flowObject*)node->GetData();
      if (((object->data.type & HMI_TYPE_MASK)!=0) /*&& ((hmiObject*)object)->uiElement*/) return node; // hmiImage does not make use of a uiElement but is a drawable object
      node = node->GetNext();
   }
   return NULL;
}



void ObjectList::deleteObjects(wxUint32 withParentID,bool destroyObject)
{
   wxNode *node = GetFirst();

   wxASSERT(!d_isCompiling || (this!=&g_objectList));
   while (node)
   {
      flowObject *current =(flowObject*)node->GetData();
      if (current->data.isChildOf==withParentID)
      {
         DeleteNode(node);
         if (destroyObject) delete current;
         node = GetFirst();
      }
      else node = node->GetNext();
    }
}



#ifdef ENV_EDITOR
wxNode *ObjectList::getDrawableFlowObject(wxList::compatibility_iterator node)
{
   if (!node) node = GetFirst();
   else node=node->GetNext();
   while (node)
   {
      hmiObject *object;

      object=(hmiObject*)node->GetData();
      wxASSERT(object);
      if (!object) return NULL;
      if (object->data.flowposx>-1) return node;
      node = node->GetNext();
   }
   return NULL;
}



flowObject *ObjectList::getDrawableFlowObject(wxPoint pos,wxByte includeFlowLines)
{
   wxList::compatibility_iterator node;
   flowObject                    *object;
   wxInt32                        posx,posy;

   node = GetFirst();
   while (node)
   {
      object=((flowObject*)node->GetData());
      if (object->data.type==hmiObject::FLOW_TYPE_GROUP_IO_DEFINITION)
      {
         posx=((IOElementDefinition*)object)->data.conPosx;
         posy=((IOElementDefinition*)object)->data.conPosy;
      }
      else
      {
         posx=object->data.flowposx;
         posy=object->data.flowposy;
      }
      if ((posx>=0) && (posx<=pos.x) && (posx+object->getFlowWidth()>=pos.x) && 
          (posy>=0) && (posy<=pos.y) && (posy+object->getFlowHeight()>=pos.y))
       return object;
      else if (includeFlowLines)
      {
          if (object->getFlowConnection(pos,5))
           return object;
      }
      node = node->GetNext();
   }
   return NULL;
}
#endif



wxNode *ObjectList::getObject(wxNode *node)
{
   wxASSERT(!d_isCompiling || (this!=&g_objectList));
   if (!node) node = GetFirst();
   else node=node->GetNext();
   return node;
}



flowObject *ObjectList::getObject(wxUint32 id)
{
   flowObject *object;

   wxASSERT(!d_isCompiling || (this!=&g_objectList));
   wxNode *node= GetFirst();
   while (node)
   {
      object=((flowObject*)node->GetData());
      if (object->data.type==hmiObject::FLOW_TYPE_GROUP_IO_DEFINITION) //TODO: remove when using APCX files only in player
      {
         if (((IOElementDefinition*)object)->data.id==id) return object;
      }
      else
      {
         if (object->data.id==id) return object;
         if (object->data.type==hmiObject::FLOW_TYPE_GROUP)
         {
            flowObject *innerObject;

            innerObject=((flowGroup*)object)->getObjectList()->getObject(id);
            if (innerObject) return innerObject;
            innerObject=((flowGroup*)object)->getINObjectList()->getObject(id);
            if (innerObject) return innerObject;
            innerObject=((flowGroup*)object)->getOUTObjectList()->getObject(id);
            if (innerObject) return innerObject;
         }
      }
      node = node->GetNext();
   }
   return NULL;
}



/**
 * Finds a flow object based on its (unique) name
 * @param[in] name the unique name of the object
 * @param[in] includeDeviceObjects when this parameter is set to true also object with mode
 *            OBJECT_MODE_DEVICE are included into search, elsewhere they are ignored and not returned
 * @return the object with this name or NULL in case it could not be found
 */
flowObject *ObjectList::getObject(wxString name,bool includeDeviceObjects)
{
   flowObject *object;

   wxASSERT(!d_isCompiling || (this!=&g_objectList));
   wxNode *node= GetFirst();
   while (node)
   {
      object=((flowObject*)node->GetData());
      if (object->data.type==hmiObject::FLOW_TYPE_GROUP_IO_DEFINITION) // TODO: remove for Player when using APCX fileso nly
      {
         if (name.Cmp(((IOElementDefinition*)object)->name)==0)
         {
            if ((includeDeviceObjects) || (object->getMode()!=OBJECT_MODE_DEVICE))
             return object;
         }
      }
      else
      {
         if (name.Cmp(object->name)==0)
         {
            if ((includeDeviceObjects) || (object->getMode()!=OBJECT_MODE_DEVICE))
             return object;
         }
         if (object->data.type==hmiObject::FLOW_TYPE_GROUP)
         {
            flowObject *innerObject;

            innerObject=((flowGroup*)object)->getObjectList()->getObject(name,includeDeviceObjects);
            if (innerObject)
            {
               if ((includeDeviceObjects) || (object->getMode()!=OBJECT_MODE_DEVICE))
                return innerObject;
            }
            innerObject=((flowGroup*)object)->getINObjectList()->getObject(name,includeDeviceObjects);
            if (innerObject)
            {
               if ((includeDeviceObjects) || (object->getMode()!=OBJECT_MODE_DEVICE))
                return innerObject;
            }
            innerObject=((flowGroup*)object)->getOUTObjectList()->getObject(name,includeDeviceObjects);
            if (innerObject)
            {
               if ((includeDeviceObjects) || (object->getMode()!=OBJECT_MODE_DEVICE))
                return innerObject;
            }
         }
      }
      node = node->GetNext();
   }
   return NULL;
}



/**
 * Finds a flowISConnect object that handles the given device node name
 * @param[in] name the unique name of the device node
 * @return the object that handles this device node or NULL in case it could not be found
 */
flowObject *ObjectList::getParentISCOObject(wxString name)
{
   flowObject *object;

   wxASSERT(!d_isCompiling || (this!=&g_objectList));
   wxNode *node= GetFirst();
   while (node)
   {
      object=((flowObject*)node->GetData());
      if (object->data.type==hmiObject::FLOW_TYPE_MISC_ISCONNECT)
      {
         if (((flowISConnect*)object)->containsOutput(name)) return object;
      }
      else if (object->data.type==hmiObject::FLOW_TYPE_GROUP)
      {
         flowObject *innerObject;

         innerObject=((flowGroup*)object)->getObjectList()->getParentISCOObject(name);
         if (innerObject) return innerObject;
      }
      node = node->GetNext();
   }
   return NULL;
}



flowObject *ObjectList::getFirstObject()
{
   wxASSERT(!d_isCompiling || (this!=&g_objectList));
   wxNode *node= GetFirst();
   if (node) return (flowObject*)node->GetData();
   return NULL;
}


#ifndef ENV_HPLAYER
hmiObject *ObjectList::getObjectByUIElementID(wxInt32 id)
{
   hmiObject *object;

   wxASSERT(!d_isCompiling || (this!=&g_objectList));
   wxList::compatibility_iterator node= GetFirst();
   while (node)
   {
      object=((hmiObject*)node->GetData());
      if ((object->uiElement) && (object->uiElement->GetId()==id)) return object;
      else if (object->data.type==HMI_TYPE_NUMFIELD)
      {
         if (((oapcSpinCtrl*)object->uiElement)->m_numField->GetId()==id) return object;
         if (((oapcSpinCtrl*)object->uiElement)->m_spinButton->GetId()==id) return object;
      }
      node = node->GetNext();
   }
   return NULL;
}
#endif


wxInt32 ObjectList::loadFlowObjectData(wxFile *FHandle,wxInt32 chunkSize,wxInt32 IDOffset,wxUint32 *loadedGroupID,bool display,bool isCompiled,ObjectList *topLevelObjectList,wxInt32 mode,bool *hasISCOElement)
{
   wxInt32      subChunkSize;
   char         rawData[5];
   size_t       readSize;
#ifndef ENV_HPLAYER
   flowObject  *loadObject;
#endif

#ifdef ENV_EDITOR
   g_isEmpty=0; // something is loaded either by load project or by load group, so the project is no longer empty
#endif
   rawData[4]=0;
   while (chunkSize>0)
   {
      readSize=FHandle->Read(rawData,4);
      readSize+=FHandle->Read(&subChunkSize,4);
      if (readSize!=8) break;
      chunkSize-=8;
      subChunkSize=ntohl(subChunkSize);
      wxASSERT(subChunkSize>0);
      if (subChunkSize<=0) return chunkSize;
      wxASSERT(subChunkSize<=1000000);
      if (subChunkSize>1000000) return chunkSize;

      chunkSize-=subChunkSize; // the following code ensures that exactly the size of subChunkSize is read so we can substract it here
      if (!strncmp(rawData,CHUNK_FLOW_SOBJ,4)) // hmi objects
      {
#ifndef ENV_HPLAYER
         struct hmiFlowData flowData;

         wxASSERT(mode!=OBJECT_MODE_DEVICE);
         subChunkSize-=FHandle->Read(&flowData,sizeof(struct hmiFlowData));
         flowData.version=  ntohl(flowData.version);
         if (IDOffset)
         {
            wxASSERT(display); // add-loading is not supported for background-loaded jobs
            flowData.id=ntohl(flowData.id)-IDOffset+g_objectList.currentUniqueID()+1;
         }
         else flowData.id  =ntohl(flowData.id);
         flowData.usedFlows=ntohl(flowData.usedFlows);

         flowData.maxEdges= ntohl(flowData.maxEdges);
         flowData.flowFlags= ntohl(flowData.flowFlags);
         loadObject=(hmiObject*)this->getObject(flowData.id);
         wxASSERT(loadObject);
         if (loadObject)
         {
            subChunkSize-=loadObject->loadFlow(FHandle,&flowData,0,false,isCompiled);
            ((hmiObject*)loadObject)->m_flowFlags=flowData.flowFlags;
         }
#endif
      }
      else if (!strncmp(rawData,CHUNK_FLOW_EXIO,4)) // external IO flow objects
      {
         struct externalFlowData        flowData;
         flowExternalIOLib             *flow=NULL;
         ExternalIOLib                 *extIOLib;

         subChunkSize-=FHandle->Read(&flowData,sizeof(struct externalFlowData));
         flowData.flowData.version=  ntohl(flowData.flowData.version);
         if (IDOffset)
         {
            wxASSERT(display); // add-loading is not supported for background-loaded jobs
            flowData.flowData.id=ntohl(flowData.flowData.id)-IDOffset+g_objectList.currentUniqueID()+1;
         }
         else flowData.flowData.id  =ntohl(flowData.flowData.id);
         if ((*hasISCOElement) || (mode!=OBJECT_MODE_DEVICE)) // load and create the plug in only in case there is a ISConnection
                                                              // available or in case we're loading locally managed objects
         {
            flowData.flowData.usedFlows=ntohl(flowData.flowData.usedFlows);
            flowData.flowData.maxEdges= ntohl(flowData.flowData.maxEdges);
            flowData.flowX=             ntohl(flowData.flowX);
            flowData.flowY=             ntohl(flowData.flowY);
            flowData.extLibDataSize=    ntohl(flowData.extLibDataSize);
            flowData.userPriviFlags    =oapc_util_ntohll(flowData.userPriviFlags);
            flowData.dataFlowFlags     =ntohl(flowData.dataFlowFlags);
            flowData.flowFlags         =ntohl(flowData.flowFlags);
            flowData.type              =ntohl(flowData.type);
            flowData.cycleMicros       =ntohl(flowData.cycleMicros);
            wxString libname;
            
            oapc_unicode_charToStringUTF16BE(flowData.store_libname,sizeof(flowData.store_libname),&libname);
   #ifdef ENV_LINUX
            libname.Replace(_T(".dll"),_T(".so"));
            libname.Replace(_T(".DLL"),_T(".so"));
            libname.Replace(_T("\\"),_T("/"));
   #endif
   #ifdef ENV_WINDOWS
            libname.Replace(_T(".so"),_T(".dll"));
            libname.Replace(_T("/"),_T("\\"));
   #endif

            if (!g_externalIOLibs->contains(libname))
             g_externalIOLibs->importPlugin(libname,FLOW_TYPE_MASK,NULL);
            extIOLib=g_externalIOLibs->getLib(true);
            while (extIOLib)
            {
               if (extIOLib->libname.Cmp(libname)==0)
               {
                  flow=new flowExternalIOLib(extIOLib,flowData.flowData.id);
                  if (flow)
                  {
                     if (flow->returnOK)
                     {
                        oapc_unicode_charToStringUTF16BE(flowData.store_name,sizeof(flowData.store_name),&flow->name);
   #ifndef ENV_EDITOR
                        flow->createNodeNames();
   #endif
                        addObject(flow,display,IDOffset>0);
                        flow->setMode(mode);
   #ifdef ENV_EDITOR
                        if (mode==OBJECT_MODE_FLOWELEMENT)
                         flow->setFlowPos(this,wxPoint(flowData.flowX,flowData.flowY),1,1);
                        else
                         g_plugInPanel->addItem(flow,extIOLib->getLibCapabilities());
   #endif
                     }
                     else
                     {
                        delete flow;
                        flow=NULL;
                     }
                  }
                  break;
               }
               extIOLib=g_externalIOLibs->getLib(false);
            }

            if (flow)
            {
               subChunkSize-=flow->loadEXIO(FHandle,&flowData,IDOffset,isCompiled);
               if (!flow->returnOK)
               {
                  this->DeleteObject(flow);
                  delete flow;
               }
            }
            else
#ifndef ENV_HPLAYER
             wxMessageBox(_("Could not find required Plug-In")+_T(": ")+libname,_("Error"),wxICON_ERROR|wxOK);
#else
             printf("Error: Could not find required Plug-In!\n");
#endif
         }
#ifdef ENV_PLAYER
         if ((mode==OBJECT_MODE_DEVICE) &&
             (((g_objectList.m_projectData->flags & PROJECT_FLAG_ISPACE_MASK)==PROJECT_FLAG_ISPACE_SINGLE_LOCAL) ||
              ((g_objectList.m_projectData->flags & PROJECT_FLAG_ISPACE_MASK)==PROJECT_FLAG_ISPACE_DOUBLE_LOCAL)))
         {
            // store identifiers for automatic execution of OpenPlugger with these elements
            g_pluggerIDList.push_back(flowData.flowData.id);
         }
#endif
      }
      else if (!strncmp(rawData,CHUNK_FLOW_FGRP,4)) // flowgroup
      {
         wxASSERT(!isCompiled);
         wxASSERT(mode!=OBJECT_MODE_DEVICE);
         flowGroup *group=new flowGroup(m_owner,display);
         wxASSERT(group);
         if (group)
         {
            subChunkSize-=group->loadFGRP(FHandle,subChunkSize,IDOffset,display,topLevelObjectList);
#ifdef ENV_EDITOR
            if (display) group->createFlowBitmap();
#endif
            wxASSERT(group->returnOK);
            if (!group->returnOK)
            {
               delete group;
               group=NULL;
               if (loadedGroupID) *loadedGroupID=0;
            }
            else
            {
               addObject(group,display,IDOffset>0);
               if (loadedGroupID) *loadedGroupID=group->data.id;
            }
         }
      }
      else if ((!strncmp(rawData,CHUNK_FLOW_CVDN,4)) || (!strncmp(rawData,CHUNK_FLOW_CVDC,4)) ||
               (!strncmp(rawData,CHUNK_FLOW_CVCN,4)) || (!strncmp(rawData,CHUNK_FLOW_CVCD,4)) ||
               (!strncmp(rawData,CHUNK_FLOW_CVND,4)) || (!strncmp(rawData,CHUNK_FLOW_CVNC,4)) || (!strncmp(rawData,CHUNK_FLOW_CVNB,4)) ||
               (!strncmp(rawData,CHUNK_FLOW_CVMC,4)) ||
               (!strncmp(rawData,CHUNK_FLOW_CVDP,4)) || (!strncmp(rawData,CHUNK_FLOW_CVNP,4)) ||
               (!strncmp(rawData,CHUNK_FLOW_CVCP,4)) || (!strncmp(rawData,CHUNK_FLOW_CVBP,4)) ||
               (!strncmp(rawData,CHUNK_FLOW_CVPC,4)) || (!strncmp(rawData,CHUNK_FLOW_CVPD,4)) ||
               (!strncmp(rawData,CHUNK_FLOW_CVPN,4)) || (!strncmp(rawData,CHUNK_FLOW_CVPB,4)) ||
               (!strncmp(rawData,CHUNK_FLOW_LRSF,4)) || (!strncmp(rawData,CHUNK_FLOW_LTFF,4)) || 
               (!strncmp(rawData,CHUNK_FLOW_LGDN,4)) || (!strncmp(rawData,CHUNK_FLOW_LGNN,4)) ||
               (!strncmp(rawData,CHUNK_FLOW_LGDP,4)) || (!strncmp(rawData,CHUNK_FLOW_LGNP,4)) ||
               (!strncmp(rawData,CHUNK_FLOW_LDSF,4)) || (!strncmp(rawData,CHUNK_FLOW_LNSF,4)) ||
               (!strncmp(rawData,CHUNK_FLOW_LCSF,4)) || (!strncmp(rawData,CHUNK_FLOW_LBSF,4)) ||
               (!strncmp(rawData,CHUNK_FLOW_LGCP,4)) || (!strncmp(rawData,CHUNK_FLOW_LGBP,4)) ||
               (!strncmp(rawData,CHUNK_FLOW_LGDO,4)) || (!strncmp(rawData,CHUNK_FLOW_LGNO,4)) ||
               (!strncmp(rawData,CHUNK_FLOW_LGDA,4)) || (!strncmp(rawData,CHUNK_FLOW_LGNA,4)) ||
               (!strncmp(rawData,CHUNK_FLOW_LGDX,4)) || (!strncmp(rawData,CHUNK_FLOW_LGNX,4)) ||
               (!strncmp(rawData,CHUNK_FLOW_DCMP,4)) ||
               (!strncmp(rawData,CHUNK_FLOW_NCMP,4)) || (!strncmp(rawData,CHUNK_FLOW_CCMP,4)) ||
               (!strncmp(rawData,CHUNK_FLOW_DCTR,4)) || (!strncmp(rawData,CHUNK_FLOW_NCTR,4)) || 
               (!strncmp(rawData,CHUNK_FLOW_CCTR,4)) || (!strncmp(rawData,CHUNK_FLOW_BCTR,4)) || 
               (!strncmp(rawData,CHUNK_FLOW_FSTP,4)) || (!strncmp(rawData,CHUNK_FLOW_FDLY,4)) ||
               (!strncmp(rawData,CHUNK_FLOW_FSTA,4)) || (!strncmp(rawData,CHUNK_FLOW_FTMR,4)) ||
               (!strncmp(rawData,CHUNK_FLOW_MAAD,4)) || (!strncmp(rawData,CHUNK_FLOW_MAMU,4)) ||
               (!strncmp(rawData,CHUNK_FLOW_MASU,4)) || (!strncmp(rawData,CHUNK_FLOW_MADI,4)) ||
               (!strncmp(rawData,CHUNK_MISC_LOGO,4)) || (!strncmp(rawData,CHUNK_MISC_LOGR,4)) ||
               (!strncmp(rawData,CHUNK_MISC_USLI,4)) || (!strncmp(rawData,CHUNK_MISC_ISCO,4)) ||
               (!strncmp(rawData,CHUNK_FLOW_NTGT,4)) || (!strncmp(rawData,CHUNK_FLOW_CTGT,4)) ||
               (!strncmp(rawData,CHUNK_FLOW_BTGT,4)) || (!strncmp(rawData,CHUNK_FLOW_DTGT,4)) ||
               (!strncmp(rawData,CHUNK_FLOW_NGTE,4)) || (!strncmp(rawData,CHUNK_FLOW_CGTE,4)) ||
               (!strncmp(rawData,CHUNK_FLOW_BGTE,4)) || (!strncmp(rawData,CHUNK_FLOW_DGTE,4)))
      {
         flowObject *loadObject=NULL;

         wxASSERT(mode!=OBJECT_MODE_DEVICE);
         if (!strncmp(rawData,CHUNK_FLOW_CVDN,4))
          loadObject=new flowConverterDigi2Num();
         else if (!strncmp(rawData,CHUNK_FLOW_CVDC,4))
          loadObject=new flowConverterDigi2Char();
         else if (!strncmp(rawData,CHUNK_FLOW_CVCN,4))
          loadObject=new flowConverterChar2Num();
         else if (!strncmp(rawData,CHUNK_FLOW_CVCD,4))
          loadObject=new flowConverterChar2Digi();
         else if (!strncmp(rawData,CHUNK_FLOW_CVND,4))
          loadObject=new flowConverterNum2Digi();
         else if (!strncmp(rawData,CHUNK_FLOW_CVNB,4))
          loadObject=new flowConverterNum2Bits();
         else if (!strncmp(rawData,CHUNK_FLOW_CVNC,4))
          loadObject=new flowConverterNum2Char();
         else if (!strncmp(rawData,CHUNK_FLOW_CVMC,4))
          loadObject=new flowConverterMixed2Char();
         else if (!strncmp(rawData,CHUNK_FLOW_CVDP,4))
          loadObject=new flowConverterDigi2Pair();
         else if (!strncmp(rawData,CHUNK_FLOW_CVNP,4))
          loadObject=new flowConverterNum2Pair();
         else if (!strncmp(rawData,CHUNK_FLOW_CVCP,4))
          loadObject=new flowConverterChar2Pair();
         else if (!strncmp(rawData,CHUNK_FLOW_CVPC,4))
          loadObject=new flowConverterPair2Char();
         else if (!strncmp(rawData,CHUNK_FLOW_CVPD,4))
          loadObject=new flowConverterPair2Digi();
         else if (!strncmp(rawData,CHUNK_FLOW_CVPN,4))
          loadObject=new flowConverterPair2Num();
         else if (!strncmp(rawData,CHUNK_FLOW_CVPB,4))
          loadObject=new flowConverterPair2Bin();
         else if (!strncmp(rawData,CHUNK_FLOW_CVBP,4))
          loadObject=new flowConverterBin2Pair();

         else if (!strncmp(rawData,CHUNK_FLOW_LRSF,4))
          loadObject=new flowLogicDigiRSFF();
         else if (!strncmp(rawData,CHUNK_FLOW_LTFF,4))
          loadObject=new flowLogicDigiTFF();
         else if (!strncmp(rawData,CHUNK_FLOW_LGDN,4))
          loadObject=new flowLogicDigiNOT();
         else if (!strncmp(rawData,CHUNK_FLOW_LGNN,4))
          loadObject=new flowLogicNumNOT();
         else if (!strncmp(rawData,CHUNK_FLOW_LGDP,4))
          loadObject=new flowLogicDigiNOP();
         else if (!strncmp(rawData,CHUNK_FLOW_LGNP,4))
          loadObject=new flowLogicNumNOP();
         else if (!strncmp(rawData,CHUNK_FLOW_LGCP,4))
          loadObject=new flowLogicCharNOP();
         else if (!strncmp(rawData,CHUNK_FLOW_LGBP,4))
          loadObject=new flowLogicBinNOP();
         else if (!strncmp(rawData,CHUNK_FLOW_NTGT,4))
          loadObject=new flowNumTrigGate();
         else if (!strncmp(rawData,CHUNK_FLOW_NGTE,4))
          loadObject=new flowNumGate();
         else if (!strncmp(rawData,CHUNK_FLOW_CTGT,4))
          loadObject=new flowCharTrigGate();
         else if (!strncmp(rawData,CHUNK_FLOW_CGTE,4))
          loadObject=new flowCharGate();
         else if (!strncmp(rawData,CHUNK_FLOW_BTGT,4))
          loadObject=new flowBinTrigGate();
         else if (!strncmp(rawData,CHUNK_FLOW_BGTE,4))
          loadObject=new flowBinGate();
         else if (!strncmp(rawData,CHUNK_FLOW_DTGT,4))
          loadObject=new flowDigiTrigGate();
         else if (!strncmp(rawData,CHUNK_FLOW_DGTE,4))
          loadObject=new flowDigiGate();
         else if (!strncmp(rawData,CHUNK_FLOW_LDSF,4))
          loadObject=new flowLogicDigiShift();
         else if (!strncmp(rawData,CHUNK_FLOW_LNSF,4))
          loadObject=new flowLogicNumShift();
         else if (!strncmp(rawData,CHUNK_FLOW_LCSF,4))
          loadObject=new flowLogicCharShift();
         else if (!strncmp(rawData,CHUNK_FLOW_LBSF,4))
          loadObject=new flowLogicBinShift();

         else if (!strncmp(rawData,CHUNK_FLOW_DCMP,4))
          loadObject=new flowMathDigiCmp();
         else if (!strncmp(rawData,CHUNK_FLOW_NCMP,4))
          loadObject=new flowMathNumCmp();
         else if (!strncmp(rawData,CHUNK_FLOW_CCMP,4))
          loadObject=new flowMathCharCmp();
         else if (!strncmp(rawData,CHUNK_FLOW_DCTR,4))
          loadObject=new flowMathDigiCtr();
         else if (!strncmp(rawData,CHUNK_FLOW_NCTR,4))
          loadObject=new flowMathNumCtr();
         else if (!strncmp(rawData,CHUNK_FLOW_CCTR,4))
          loadObject=new flowMathCharCtr();
         else if (!strncmp(rawData,CHUNK_FLOW_BCTR,4))
          loadObject=new flowMathBinCtr();

         else if (!strncmp(rawData,CHUNK_FLOW_LGDO,4))
          loadObject=new flowLogicDigiConcat(hmiObject::FLOW_TYPE_LOGI_DIGIOR);
         else if (!strncmp(rawData,CHUNK_FLOW_LGNO,4))
          loadObject=new flowLogicNumConcat(hmiObject::FLOW_TYPE_LOGI_NUMOR);
         else if (!strncmp(rawData,CHUNK_FLOW_LGDX,4))
          loadObject=new flowLogicDigiConcat(hmiObject::FLOW_TYPE_LOGI_DIGIXOR);
         else if (!strncmp(rawData,CHUNK_FLOW_LGNX,4))
          loadObject=new flowLogicNumConcat(hmiObject::FLOW_TYPE_LOGI_NUMXOR);
         else if (!strncmp(rawData,CHUNK_FLOW_LGDA,4))
          loadObject=new flowLogicDigiConcat(hmiObject::FLOW_TYPE_LOGI_DIGIAND);
         else if (!strncmp(rawData,CHUNK_FLOW_LGNA,4))
          loadObject=new flowLogicNumConcat(hmiObject::FLOW_TYPE_LOGI_NUMAND);

         else if (!strncmp(rawData,CHUNK_FLOW_MAAD,4))
          loadObject=new flowLogicNumConcat(hmiObject::FLOW_TYPE_LOGI_NUMADD);
         else if (!strncmp(rawData,CHUNK_FLOW_MASU,4))
          loadObject=new flowLogicNumConcat(hmiObject::FLOW_TYPE_LOGI_NUMSUB);
         else if (!strncmp(rawData,CHUNK_FLOW_MAMU,4))
          loadObject=new flowLogicNumConcat(hmiObject::FLOW_TYPE_LOGI_NUMMUL);
         else if (!strncmp(rawData,CHUNK_FLOW_MADI,4))
          loadObject=new flowLogicNumConcat(hmiObject::FLOW_TYPE_LOGI_NUMDIV);

         else if (!strncmp(rawData,CHUNK_FLOW_FSTP,4))
          loadObject=new flowStop();
         else if (!strncmp(rawData,CHUNK_FLOW_FSTA,4))
          loadObject=new flowStart();
         else if (!strncmp(rawData,CHUNK_FLOW_FDLY,4))
          loadObject=new flowDelay();
         else if (!strncmp(rawData,CHUNK_FLOW_FTMR,4))
          loadObject=new flowTimer();

         else if (!strncmp(rawData,CHUNK_MISC_LOGO,4))
         {
            if (g_flowLog==NULL)
            {
               loadObject=new flowLog();
               g_flowLog=(flowLog*)loadObject;
            }
         }
         else if (!strncmp(rawData,CHUNK_MISC_LOGR,4))
          loadObject=new flowLogRecorder();
         else if (!strncmp(rawData,CHUNK_MISC_USLI,4))
          loadObject=new flowLogInUser();
         else if (!strncmp(rawData,CHUNK_MISC_ISCO,4))
         {
            loadObject=new flowISConnect();
            *hasISCOElement=true;
         }

         else wxASSERT(0);
         if (loadObject)
         {
            subChunkSize-=loadObject->loadDATA(FHandle,subChunkSize,IDOffset,isCompiled);
            addObject(loadObject,display,IDOffset>0);
            FHandle->Seek(subChunkSize,wxFromCurrent);
         }
         wxASSERT(subChunkSize==0);
      }

      FHandle->Seek(subChunkSize,wxFromCurrent);
   }
   return chunkSize;
}



wxString ObjectList::loadProject(wxString file,bool display,bool winsizeOnly)
{
   wxFile      *FHandle;
   wxString     data;
   char         rawData[5];
   wxInt32      chunkSize;
#ifndef ENV_HPLAYER
   wxInt32      subChunkSize=-1,m_lastLoadError=OAPC_OK;
   hmiObject   *loadObject;
#endif
   size_t       readSize;
   bool         isCompiled=false,hasISCOElement=false;
#ifdef ENV_PLAYER
   PCompiler    compiler;
#endif

#ifndef ENV_HPLAYER
   if (this==&g_objectList) hmiMutex::s_lastID=0;
   m_imageList.clear();
#endif
   wxASSERT(!d_isCompiling || (this!=&g_objectList));
   g_flowLog=NULL;
   rawData[4]=0;
   FHandle=new wxFile(file,wxFile::read);
   if (FHandle->IsOpened())
   {
        FHandle->Read(rawData,4);
        if (strncmp(rawData,CHUNK_FORM,4))
        {
           FHandle->Close();
           delete FHandle;
           return _("No valid IFF file!");
        }
        FHandle->Seek(4,wxFromCurrent); // TODO: evaluate length information

        FHandle->Read(rawData,4);
        if (strncmp(rawData,CHUNK_APCP,4))
        {
#if defined(ENV_PLAYER) | defined(_DEBUG) // APCX can be loaded by player only
           if (strncmp(rawData,CHUNK_APCX,4))
           {
              FHandle->Close();
              delete FHandle;
              return _("No valid APCP project file or APCX compiled project!");
           }
           else isCompiled=true;
#else
           FHandle->Close();
           delete FHandle;
           return _("No valid APCP project file!");
#endif
        }
        while (1)
        {
           readSize=FHandle->Read(rawData,4);
           readSize+=FHandle->Read(&chunkSize,4);
           if (readSize!=8) break;
           chunkSize=ntohl(chunkSize);

           wxASSERT(chunkSize>0);
           if (chunkSize>0)
           {
              if (!strncmp(rawData,CHUNK_ANNO,4))
              {
              }
              else if (!strncmp(rawData,CHUNK_VERS,4))
              {
              }
              else if (!strncmp(rawData,CHUNK_PROJ,4))
              {
                 wxUint32        l;

                 if (m_projectData) free (m_projectData);
                 m_projectData=NULL;
                 l=chunkSize;
                 if (l>sizeof(struct hmiProjectData)) l=sizeof(struct hmiProjectData);
                 m_projectData=g_initProjectSettings(false);
                 if (!m_projectData)
                 {
                    FHandle->Close();
                    delete (FHandle);
                    return _("Not enough memory!");
                 }
                 chunkSize-=FHandle->Read(m_projectData,l);
                 m_projectData->version        =ntohl(m_projectData->version);
                 m_projectData->flags          =ntohl(m_projectData->flags);
                 m_projectData->bgCol          =ntohl(m_projectData->bgCol);
                 m_projectData->gridW          =ntohl(m_projectData->gridW);
                 m_projectData->gridH          =ntohl(m_projectData->gridH);
                 m_projectData->totalW         =ntohl(m_projectData->totalW);
                 m_projectData->totalH         =ntohl(m_projectData->totalH);
                 m_projectData->flowW          =ntohl(m_projectData->flowW);
                 m_projectData->flowH          =ntohl(m_projectData->flowH);
                 m_projectData->flowTimeout    =ntohl(m_projectData->flowTimeout);
                 m_projectData->timerResolution=ntohl(m_projectData->timerResolution);
                 m_projectData->editorX        =ntohl(m_projectData->editorX);
                 if (m_projectData->editorX<0) m_projectData->editorX=0;
                 else if (m_projectData->editorX>2000) m_projectData->editorX=2000;
                 m_projectData->editorY        =ntohl(m_projectData->editorY);
                 if (m_projectData->editorY<0) m_projectData->editorY=0;
                 else if (m_projectData->editorY>1000) m_projectData->editorY=1000;
                 m_projectData->editorW        =ntohl(m_projectData->editorW);
                 if (m_projectData->editorW<200) m_projectData->editorW=200;
                 else if (m_projectData->editorW>2000) m_projectData->editorW=2000;
                 m_projectData->editorH        =ntohl(m_projectData->editorH);
                 if (m_projectData->editorH<100) m_projectData->editorH=100;
                 else if (m_projectData->editorH>2000) m_projectData->editorH=2000;
                 if (m_projectData->version>=2)
                 {
                    // new since 1.1
                    m_projectData->touchFactor    =ntohl(m_projectData->touchFactor);
                    if (m_projectData->touchFactor<1000) m_projectData->touchFactor=1000;
                    else if (m_projectData->touchFactor>20000) m_projectData->touchFactor=20000;
                    oapc_unicode_charToStringUTF16BE(m_projectData->font.store_faceName,sizeof(m_projectData->font.store_faceName),&g_projectTouchFontFaceName);
                    m_projectData->font.pointSize=ntohl(m_projectData->font.pointSize);
                    m_projectData->font.style=ntohl(m_projectData->font.style);
                    m_projectData->font.weight=ntohl(m_projectData->font.weight);
                 }
                 if (m_projectData->version>=3)
                 {
                    // new since 1.3
                    //m_projectData->m_remSingleIP
                 }
                 if (m_projectData->version>=4)
                 {
                    // new since 1.4
                    //m_projectData->m_remDoubleIP
                 }
#ifdef ENV_EDITOR
                 g_plugInPanel->setEnabled((g_objectList.m_projectData->flags & PROJECT_FLAG_ISPACE_MASK)!=0);
#endif
                 if (winsizeOnly) return wxEmptyString;
              }
              else if (!strncmp(rawData,CHUNK_USPR,4))
              {
#ifndef ENV_HPLAYER
                 wxInt32         priviNum,l,i;
                 wxMBConvUTF16BE conv;
                 wchar_t         buf[154];
                 char            store_name[304];

                 for (i=0; i<MAX_PRIVI_NUM; i++) g_userPriviData.priviName[i]=_T("");
                 FHandle->Read(&l,4); chunkSize-=4;
                 g_userPriviData.enabled=ntohl(l)!=0;
                 if (g_userPriviData.enabled) g_userList=new UserList();

                 FHandle->Read(&l,4); chunkSize-=4;
                 g_userPriviData.privisEnabled=ntohl(l);

                 if (chunkSize>0)
                 {
                    // available only in non-compiled files
                    FHandle->Read(&l,4); chunkSize-=4;
                    priviNum=ntohl(l);
                    if (priviNum>MAX_PRIVI_NUM) priviNum=MAX_PRIVI_NUM;
                    for (i=0; i<priviNum; i++)
                    {
                       if (chunkSize<300) break;
                       FHandle->Read(store_name,300);     
                       conv.MB2WC(buf,store_name,sizeof(buf));
                       g_userPriviData.priviName[i]=buf;
                       chunkSize-=300;
                    }
                    // end of available only in non-compiled files
                 }
#endif
              }
              else if (!strncmp(rawData,CHUNK_USDA,4))
              {
#ifndef ENV_HPLAYER
                 chunkSize=loadUserData(FHandle,chunkSize,true);
#endif
              }
              else if (!strncmp(rawData,CHUNK_HOBL,4))
              {
#ifndef ENV_HPLAYER
                 while (chunkSize>0)
                 {
                    readSize=FHandle->Read(rawData,4);
                    readSize+=FHandle->Read(&subChunkSize,4);
                    if (readSize!=8) break;
                    chunkSize-=8;
                    subChunkSize=ntohl(subChunkSize);
                    chunkSize-=subChunkSize; // the following code ensures that exactly the size of subChunjkSize is read so we can substract it here
                    loadObject=NULL;
                    if (!strncmp(rawData,CHUNK_HOBL_MTXL,4))
                    {
                       if (subChunkSize>=200)
                       {
                          wxMBConvUTF16BE conv;
                          wchar_t         buf[104];
                          char            store_name[204];
                          hmiMutex       *mutex;
                          wxUint32        id;

                          FHandle->Read(store_name,200);     
                          conv.MB2WC(buf,store_name,sizeof(buf));
                          mutex=new hmiMutex(buf);
                          subChunkSize-=200;
                          if (mutex) m_mutexList.push_back(mutex);
                          while ((mutex) && (subChunkSize>=4))
                          {
                             FHandle->Read(&id,4);     
                             mutex->addObjectID(ntohl(id));
                             subChunkSize-=4;
                          }
                       }
                    }
#ifndef ENV_HPLAYER
                    else if (!strncmp(rawData,CHUNK_HOBL_FLTF,4))      
                     loadObject=new hmiFloatField(parent,NULL);
                    else if (!strncmp(rawData,CHUNK_HOBL_HGAG,4)) loadObject=new hmiHGauge(parent,NULL);
                    else if (!strncmp(rawData,CHUNK_HOBL_HSLD,4)) loadObject=new hmiHSlider(parent,NULL);
                    else if (!strncmp(rawData,CHUNK_HOBL_IMAG,4)) loadObject=new hmiImage(parent,NULL);
                    else if (!strncmp(rawData,CHUNK_HOBL_FLIN,4)) loadObject=new hmiPrimitive(parent,NULL,HMI_TYPE_FREELINE);
                    else if (!strncmp(rawData,CHUNK_HOBL_FRAM,4)) loadObject=new hmiPrimitive(parent,NULL,HMI_TYPE_FRAME);
                    else if (!strncmp(rawData,CHUNK_HOBL_ELIP,4)) loadObject=new hmiPrimitive(parent,NULL,HMI_TYPE_ELLIPSE);
                    else if (!strncmp(rawData,CHUNK_HOBL_IMBU,4)) loadObject=new hmiImageButton(parent,NULL);
                    else if (!strncmp(rawData,CHUNK_HOBL_TOIB,4)) loadObject=new hmiToggleImageButton(parent,NULL);
                    else if (!strncmp(rawData,CHUNK_HOBL_LINE,4)) loadObject=new hmiLine(parent,NULL);
                    else if (!strncmp(rawData,CHUNK_HOBL_NUMF,4)) loadObject=new hmiNumField(parent,NULL);
                    else if (!strncmp(rawData,CHUNK_HOBL_SIBU,4))
                     loadObject=new hmiSimpleButton(parent,NULL);
                    else if (!strncmp(rawData,CHUNK_HOBL_TELA,4))
                     loadObject=new hmiTextLabel(parent,NULL);
                    else if (!strncmp(rawData,CHUNK_HOBL_TAPA,4)) loadObject=new hmiTabbedPane(parent,NULL);
                    else if (!strncmp(rawData,CHUNK_HOBL_STPA,4)) loadObject=new hmiStackedPane(parent,NULL);
                    else if (!strncmp(rawData,CHUNK_HOBL_ADPA,4)) loadObject=new hmiAdditionalPane(parent,NULL);
                    else if (!strncmp(rawData,CHUNK_HOBL_SIPA,4))
                     loadObject=new hmiSinglePanel(parent,NULL);
                    else if (!strncmp(rawData,CHUNK_HOBL_TOBU,4))
                     loadObject=new hmiToggleButton(parent,NULL);
                    else if (!strncmp(rawData,CHUNK_HOBL_RABU,4))
                     loadObject=new hmiRadioButton(parent,NULL,HMI_TYPE_RADIOBUTTON);
                    else if (!strncmp(rawData,CHUNK_HOBL_CKBX,4))
                     loadObject=new hmiRadioButton(parent,NULL,HMI_TYPE_CHECKBOX);
                    else if (!strncmp(rawData,CHUNK_HOBL_TXTF,4))
                     loadObject=new hmiTextField(parent,NULL,HMI_TYPE_TEXTFIELD);
                    else if (!strncmp(rawData,CHUNK_HOBL_PWDF,4))
                     loadObject=new hmiTextField(parent,NULL,HMI_TYPE_PASSWORDFIELD);
                    else if (!strncmp(rawData,CHUNK_HOBL_VGAG,4)) loadObject=new hmiVGauge(parent,NULL);
                    else if (!strncmp(rawData,CHUNK_HOBL_VSLD,4))
                     loadObject=new hmiVSlider(parent,NULL);
                    else if (!strncmp(rawData,CHUNK_HOBL_ANME,4))
                     loadObject=new hmiAngularMeter(parent,NULL);
                    else if (!strncmp(rawData,CHUNK_HOBL_ANRE,4)) loadObject=new hmiAngularRegulator(parent,NULL);
                    else if (!strncmp(rawData,CHUNK_HOBL_LCDN,4)) loadObject=new hmiLCDNumber(parent,NULL);
                    else if (!strncmp(rawData,CHUNK_HOBL_UMGM,4))
                     loadObject=new hmiSpecialPanel(parent,NULL,HMI_TYPE_SP_USERMANAGEMENT);
                    else if (!strncmp(rawData,CHUNK_HOBL_EXHM,4))
                    {
                       loadObject=new hmiExternalIOLib(parent,NULL);
                       if (!((hmiExternalIOLib*)loadObject)->returnOK)
                       {
                          delete loadObject;
                          loadObject=NULL;
                       }
                    }
#endif
                    else wxASSERT(0);
                    if (loadObject)
                    {
                       subChunkSize-=loadObject->load(FHandle,subChunkSize,0);
                       if (!loadObject->returnOK)
                       {
                          if (m_lastLoadError!=loadObject->getLoadError())
                          {
                             m_lastLoadError=loadObject->getLoadError();
                             g_OAPCMessageBox(m_lastLoadError,NULL,NULL,NULL,wxEmptyString);
                          }
                          delete loadObject;
                          loadObject=NULL;
                       }
                       if (loadObject)
                       {
#ifdef ENV_PLAYER
                          hmiMutex *mutex;

                          // store the mutex info within the object that is identified by the mutex list loaded above
                          mutex=getMutexByObjectID(loadObject->data.id);
                          if (mutex) loadObject->m_mutexID=mutex->m_ID;
                          // end of store the mutex info within the object that is identified by the mutex list loaded above
#endif
                          if (loadObject->data.isChildOf!=0)
                          {
                             hmiObject *object;

                             object=(hmiObject*)getObject(loadObject->data.isChildOf);
                             wxASSERT(object);
                             if (object)
                             {
                                wxASSERT((object->data.type==HMI_TYPE_TABBEDPANE) ||
                                         (object->data.type==HMI_TYPE_STACKEDPANE) ||
                                         (object->data.type==HMI_TYPE_ADDITIONALPANE) ||
                                         (object->data.type==HMI_TYPE_SINGLEPANEL));
                                if ((object->data.type==HMI_TYPE_TABBEDPANE) ||
                                    (object->data.type==HMI_TYPE_STACKEDPANE) ||
                                    (object->data.type==HMI_TYPE_ADDITIONALPANE))
                                 loadObject->setParent(((hmiTabbedPane*)object)->getPanel());
                                else if (object->data.type==HMI_TYPE_SINGLEPANEL)
                                 loadObject->setParent((BasePanel*)((hmiObject*)object)->uiElement);
                             }
                          }
                          if (display)
                          {
                             loadObject->createUIElement();
                             loadObject->applyData(1);
                          }
                          addObject(loadObject,display,false);
                       }
                    }
                    FHandle->Seek(subChunkSize,wxFromCurrent);
                 }
                 wxASSERT(subChunkSize==0);
#endif
              }
              else if (!strncmp(rawData,CHUNK_FLOW,4))
               chunkSize=loadFlowObjectData(FHandle,chunkSize,0,NULL,display,isCompiled,this,OBJECT_MODE_FLOWELEMENT,&hasISCOElement);
              else if (!strncmp(rawData,CHUNK_DEVL,4))
              {
#ifdef ENV_EDITOR
                 hasISCOElement=true;
#endif
                 chunkSize=loadFlowObjectData(FHandle,chunkSize,0,NULL,display,isCompiled,this,OBJECT_MODE_DEVICE,&hasISCOElement);
              }
              else wxASSERT(0);
              FHandle->Seek(chunkSize,wxFromCurrent);
           }
        }
#ifdef ENV_EDITOR
        wxNode     *node;
        wxUint32    minID,maxID=0;
        flowObject *object;

        node=getObject((wxNode*)NULL);
        while (node)
        {
           object=(flowObject*)node->GetData();
           if (object->data.id==hmiObject::FLOW_TYPE_GROUP) ((flowGroup*)object)->getMinMaxIDs(minID,maxID);
           else if (object->data.id>maxID) maxID=object->data.id;
           node=this->getObject(node);
        }
        if (display)
        {
           g_objectList.updateUniqueID(maxID-1); // will be incremented when an ID is fetched
        }
#endif
        FHandle->Close();
    }
    else
    {
        delete FHandle;
        return _("Could not open file!");
    }
    delete FHandle;

    // TODO: check if this part is really necessary
    // check for problems / optimize job
    {
       wxNode         *node,*connectionNode=NULL;
       flowObject     *object;
       FlowConnection *connection;

       node=getObject((wxNode*)NULL);
       while (node)
       {
          object=(flowObject*)node->GetData();
          connectionNode=object->getFlowConnection(connectionNode);
          while (connectionNode)
          {
             connection=(FlowConnection*)connectionNode->GetData();
             if ((connection->data.targetID==0) || (!getObject(connection->data.targetID)))
             {
#if defined ENV_DEBUGGER || ENV_EDITOR
                connection->data.targetID=0; // set to 0 to avoid problems with objects that have the same ID
#else
                object->deleteFlowConnection(connection); // delete unused flow connection
                connectionNode=NULL;
#endif
             }
             connectionNode=object->getFlowConnection(connectionNode);
          }
          node=getObject(node);
       }
    }
    // end of check for problems / optimize job
#ifdef ENV_PLAYER
    if (!isCompiled) compiler.compile(&g_objectList);
#endif
    return wxEmptyString;
}



#ifndef ENV_HPLAYER
wxInt32 ObjectList::loadUserData(wxFile *FHandle,wxInt32 chunkSize,bool checkForRuntimeData)
{
   wxInt32         l,cnt;
   wchar_t         buf[154];
   char            store_data[304];
   UserData       *userData;
   wxMBConvUTF16BE conv;
#ifdef ENV_PLAYER

   if (checkForRuntimeData)
   {
      bool     rtUserDataLoaded=false;
      wxString sTmp;      

      // in case there are runtime data stored including user data we do load the misntead of the default ones from
      // the project files
#ifndef ENV_WINDOWSCE
      sTmp=wxStandardPaths::Get().GetUserDataDir()+wxFileName::GetPathSeparator()+g_projectFile+_T(".rtbak");
      if (!wxFileName::FileExists(sTmp)) sTmp=wxStandardPaths::Get().GetUserDataDir()+wxFileName::GetPathSeparator()+g_projectFile+_T(".rtdat");
#else
      sTmp=_T("oapchome")+wxFileName::GetPathSeparator()+g_projectFile+_T(".rtbak");
      if (!wxFileName::FileExists(sTmp)) sTmp=_T("oapchome")+wxFileName::GetPathSeparator()+g_projectFile+_T(".rtdat");
#endif
      if (wxFile::Exists(sTmp))
      {
         wxFile      *rtHandle;
         char         rawData[5];
         wxInt32      chunkSize;
         size_t       readSize;

         rawData[4]=0;
         rtHandle=new wxFile(sTmp,wxFile::read);
         if (rtHandle->IsOpened())
         {
            rtHandle->Read(rawData,4);
            if (!strncmp(rawData,CHUNK_FORM,4))
            {
               rtHandle->Seek(4,wxFromCurrent); // TODO: evaluate length information

               rtHandle->Read(rawData,4);
               if (!strncmp(rawData,CHUNK_APRT,4))
               { 
                  while (1)
                  {
                     readSize=rtHandle->Read(rawData,4);
                     readSize+=rtHandle->Read(&chunkSize,4);
                     if (readSize!=8) break;
                     chunkSize=ntohl(chunkSize);

                     wxASSERT(chunkSize>0);
                     if (chunkSize>0)
                     {
                        if (!strncmp(rawData,CHUNK_USDA,4))
                        {  
                           if (loadUserData(rtHandle,chunkSize,false)==0)
                            rtUserDataLoaded=true;
                           rtHandle->Close();
                           delete rtHandle;
                           break;
                        }
                     }
                  } 
               }
            }
         }
      }
      if (rtUserDataLoaded) return chunkSize;
   }
#else
   checkForRuntimeData=checkForRuntimeData; // used in player only
#endif

   if (g_userList) g_userList->deleteAll();
   else g_userList=new UserList();
   FHandle->Read(&l,4); chunkSize-=4;               // version
   FHandle->Read(&l,4); cnt=ntohl(l); chunkSize-=4; // number of user data

   while ((chunkSize>=520) && (cnt>0))
   {
      userData=new UserData();
      wxASSERT(userData);
      if (!userData) return chunkSize;
      g_userList->addUserData(userData);

      FHandle->Read(store_data,50); chunkSize-=50;
      conv.MB2WC(buf,store_data,sizeof(buf));
      userData->m_login=buf;

      FHandle->Read(store_data,50); chunkSize-=50;
      conv.MB2WC(buf,store_data,sizeof(buf));
      userData->m_pwd=buf;

      FHandle->Read(store_data,200); chunkSize-=200;
      conv.MB2WC(buf,store_data,sizeof(buf));
      userData->m_fullname=buf;

      FHandle->Read(store_data,200); chunkSize-=200;
      conv.MB2WC(buf,store_data,sizeof(buf));
      userData->m_comment=buf;

      FHandle->Read(&l,4); userData->m_canDo=ntohl(l); chunkSize-=4;
      FHandle->Read(&l,4); userData->m_state=ntohl(l); chunkSize-=4;
      FHandle->Read(&l,4); userData->m_res1=ntohl(l);  chunkSize-=4;
      FHandle->Read(&l,4); userData->m_res2=ntohl(l);  chunkSize-=4;
      FHandle->Read(&l,4); userData->m_res3=ntohl(l);  chunkSize-=4;
      cnt--;
   }
   return chunkSize;
}



void ObjectList::saveUserData(wxFile *FHandle)
{
   wxUint32        l,lastPos;
   UserData       *userData;
   wxMBConvUTF16BE conv;
   char            store_data[304];

   if ((!g_userList) || (g_userList->getCount()<=0)) return;

   FHandle->Write(CHUNK_USDA,4); // user privileges ***************************************************
   l=htonl(0); FHandle->Write(&l,4);
   lastPos=FHandle->Tell()-4;

   l=htonl(1);                      // version of chunk
   FHandle->Write(&l,4);
   l=htonl(g_userList->getCount()); // number of users
   FHandle->Write(&l,4);

   userData=g_userList->getUserData(true);
   while (userData)
   {
      conv.WC2MB(store_data,userData->m_login,50);
      FHandle->Write(store_data,50);
      conv.WC2MB(store_data,userData->m_pwd,50);
      FHandle->Write(store_data,50);
      conv.WC2MB(store_data,userData->m_fullname,200);
      FHandle->Write(store_data,200);
      conv.WC2MB(store_data,userData->m_comment,200);
      FHandle->Write(store_data,200);
      l=htonl(userData->m_canDo);
      FHandle->Write(&l,4);
      l=htonl(userData->m_state);
      FHandle->Write(&l,4);
      l=htonl(userData->m_res1);
      FHandle->Write(&l,4);
      l=htonl(userData->m_res2);
      FHandle->Write(&l,4);
      l=htonl(userData->m_res3);
      FHandle->Write(&l,4);
      userData=g_userList->getUserData(false);
   }

   l=FHandle->Tell()-lastPos-4; // write length of chunk
   l=htonl(l);
   FHandle->Seek(lastPos,wxFromStart);
   FHandle->Write(&l,4);
   FHandle->Seek(0,wxFromEnd);  // end of write length of chunk / user privileges ********************
}
#endif


#ifdef ENV_EDITOR
wxNode *ObjectList::findObjectByName(wxString name,wxNode *previous,wxByte hmiObjectOnly)
{
   flowObject *object;
   wxString    cmpName;

   wxASSERT(!d_isCompiling || (this!=&g_objectList));
   name.MakeUpper();
   if (!previous)
   {
      previous=GetFirst();
      if (!previous) return NULL;
      object=(flowObject*)previous->GetData();
      if (((hmiObjectOnly) && (object->data.type & HMI_TYPE_MASK)) || (!hmiObjectOnly))
      {
         cmpName=object->name;
         cmpName.MakeUpper();
         if (cmpName.Contains(name)) return previous;
      }
   }
   previous=previous->GetNext();
   while (previous)
   {
      object=(flowObject*)previous->GetData();
      if (((hmiObjectOnly) && (object->data.type & HMI_TYPE_MASK)) || (!hmiObjectOnly))
      {
         cmpName=object->name;
         cmpName.MakeUpper();
         if (cmpName.Contains(name)) return previous;
      }
      previous=previous->GetNext();
   }

   return NULL;
}



void ObjectList::saveFlowObjectData(wxFile *FHandle,bool isCompiled)
{
   wxNode *node = GetFirst();
   while (node)
   {
      flowObject *current=(flowObject*)node->GetData();
      if ((current->data.type & HMI_TYPE_MASK)!=0)         ((hmiObject*)current)->saveSOBJ(FHandle,isCompiled);
      else if (current->data.type==FLOW_TYPE_EXTERNAL_LIB)
      {
    	  if (current->getMode()==OBJECT_MODE_FLOWELEMENT) ((flowExternalIOLib*)current)->saveEXIO(FHandle,isCompiled);
      }
      else if (current->data.type==hmiObject::FLOW_TYPE_GROUP)
      {
         wxASSERT(!isCompiled); // compiled projects never have groups
         ((flowGroup*)current)->saveFGRP(FHandle,1);
      }
      else if (current->data.type & FLOW_TYPE_MASK)
      {
         if ((current->data.flowposx>=0) && (current->data.flowposy>=0))
         {
            if (current->data.type==hmiObject::FLOW_TYPE_CONV_CHAR2DIGI)       current->saveDATA(FHandle,(char*)CHUNK_FLOW_CVCD,isCompiled);
            else if (current->data.type==hmiObject::FLOW_TYPE_CONV_CHAR2NUM)   current->saveDATA(FHandle,(char*)CHUNK_FLOW_CVCN,isCompiled);
            else if (current->data.type==hmiObject::FLOW_TYPE_CONV_DIGI2CHAR)  current->saveDATA(FHandle,(char*)CHUNK_FLOW_CVDC,isCompiled);
            else if (current->data.type==hmiObject::FLOW_TYPE_CONV_DIGI2NUM)   current->saveDATA(FHandle,(char*)CHUNK_FLOW_CVDN,isCompiled);
            else if (current->data.type==hmiObject::FLOW_TYPE_CONV_MIXED2CHAR) current->saveDATA(FHandle,(char*)CHUNK_FLOW_CVMC,isCompiled);
            else if (current->data.type==hmiObject::FLOW_TYPE_CONV_DIGI2PAIR)  current->saveDATA(FHandle,(char*)CHUNK_FLOW_CVDP,isCompiled);
            else if (current->data.type==hmiObject::FLOW_TYPE_CONV_NUM2PAIR)   current->saveDATA(FHandle,(char*)CHUNK_FLOW_CVNP,isCompiled);
            else if (current->data.type==hmiObject::FLOW_TYPE_CONV_CHAR2PAIR)  current->saveDATA(FHandle,(char*)CHUNK_FLOW_CVCP,isCompiled);
            else if (current->data.type==hmiObject::FLOW_TYPE_CONV_PAIR2CHAR)  current->saveDATA(FHandle,(char*)CHUNK_FLOW_CVPC,isCompiled);
            else if (current->data.type==hmiObject::FLOW_TYPE_CONV_PAIR2DIGI)  current->saveDATA(FHandle,(char*)CHUNK_FLOW_CVPD,isCompiled);
            else if (current->data.type==hmiObject::FLOW_TYPE_CONV_PAIR2NUM)   current->saveDATA(FHandle,(char*)CHUNK_FLOW_CVPN,isCompiled);
            else if (current->data.type==hmiObject::FLOW_TYPE_CONV_PAIR2BIN)   current->saveDATA(FHandle,(char*)CHUNK_FLOW_CVPB,isCompiled);
            else if (current->data.type==hmiObject::FLOW_TYPE_CONV_BIN2PAIR)   current->saveDATA(FHandle,(char*)CHUNK_FLOW_CVBP,isCompiled);
            else if (current->data.type==hmiObject::FLOW_TYPE_CONV_NUM2BITS)
             current->saveDATA(FHandle,(char*)CHUNK_FLOW_CVNB,isCompiled);
            else if (current->data.type==hmiObject::FLOW_TYPE_CONV_NUM2CHAR)   current->saveDATA(FHandle,(char*)CHUNK_FLOW_CVNC,isCompiled);
            else if (current->data.type==hmiObject::FLOW_TYPE_CONV_NUM2DIGI)   current->saveDATA(FHandle,(char*)CHUNK_FLOW_CVND,isCompiled);
            else if (current->data.type==hmiObject::FLOW_TYPE_LOGI_NUMNOP)     current->saveDATA(FHandle,(char*)CHUNK_FLOW_LGNP,isCompiled);
            else if (current->data.type==hmiObject::FLOW_TYPE_LOGI_DIGINOP)    current->saveDATA(FHandle,(char*)CHUNK_FLOW_LGDP,isCompiled);
            else if (current->data.type==hmiObject::FLOW_TYPE_LOGI_CHARNOP)    current->saveDATA(FHandle,(char*)CHUNK_FLOW_LGCP,isCompiled);
            else if (current->data.type==hmiObject::FLOW_TYPE_LOGI_BINNOP)     current->saveDATA(FHandle,(char*)CHUNK_FLOW_LGBP,isCompiled);
            else if (current->data.type==hmiObject::FLOW_TYPE_LOGI_DIGISHIFT)  current->saveDATA(FHandle,(char*)CHUNK_FLOW_LDSF,isCompiled);
            else if (current->data.type==hmiObject::FLOW_TYPE_LOGI_NUMSHIFT)   current->saveDATA(FHandle,(char*)CHUNK_FLOW_LNSF,isCompiled);
            else if (current->data.type==hmiObject::FLOW_TYPE_LOGI_CHARSHIFT)  current->saveDATA(FHandle,(char*)CHUNK_FLOW_LCSF,isCompiled);
            else if (current->data.type==hmiObject::FLOW_TYPE_LOGI_BINSHIFT)   current->saveDATA(FHandle,(char*)CHUNK_FLOW_LBSF,isCompiled);
            else if (current->data.type==hmiObject::FLOW_TYPE_LOGI_DIGIRSFF)   current->saveDATA(FHandle,(char*)CHUNK_FLOW_LRSF,isCompiled);
            else if (current->data.type==hmiObject::FLOW_TYPE_LOGI_DIGITFF)    current->saveDATA(FHandle,(char*)CHUNK_FLOW_LTFF,isCompiled);
            else if (current->data.type==hmiObject::FLOW_TYPE_LOGI_DIGINOT)    current->saveDATA(FHandle,(char*)CHUNK_FLOW_LGDN,isCompiled);
            else if (current->data.type==hmiObject::FLOW_TYPE_LOGI_NUMNOT)     current->saveDATA(FHandle,(char*)CHUNK_FLOW_LGNN,isCompiled);
            else if (current->data.type==hmiObject::FLOW_TYPE_LOGI_DIGIOR)     current->saveDATA(FHandle,(char*)CHUNK_FLOW_LGDO,isCompiled);
            else if (current->data.type==hmiObject::FLOW_TYPE_LOGI_NUMOR)      current->saveDATA(FHandle,(char*)CHUNK_FLOW_LGNO,isCompiled);
            else if (current->data.type==hmiObject::FLOW_TYPE_LOGI_DIGIXOR)    current->saveDATA(FHandle,(char*)CHUNK_FLOW_LGDX,isCompiled);
            else if (current->data.type==hmiObject::FLOW_TYPE_LOGI_NUMXOR)     current->saveDATA(FHandle,(char*)CHUNK_FLOW_LGNX,isCompiled);
            else if (current->data.type==hmiObject::FLOW_TYPE_LOGI_DIGIAND)    current->saveDATA(FHandle,(char*)CHUNK_FLOW_LGDA,isCompiled);
            else if (current->data.type==hmiObject::FLOW_TYPE_LOGI_NUMAND)     current->saveDATA(FHandle,(char*)CHUNK_FLOW_LGNA,isCompiled);
            else if (current->data.type==hmiObject::FLOW_TYPE_LOGI_NUMADD)     current->saveDATA(FHandle,(char*)CHUNK_FLOW_MAAD,isCompiled);
            else if (current->data.type==hmiObject::FLOW_TYPE_LOGI_NUMSUB)     current->saveDATA(FHandle,(char*)CHUNK_FLOW_MASU,isCompiled);
            else if (current->data.type==hmiObject::FLOW_TYPE_LOGI_NUMMUL)     current->saveDATA(FHandle,(char*)CHUNK_FLOW_MAMU,isCompiled);
            else if (current->data.type==hmiObject::FLOW_TYPE_LOGI_NUMDIV)     current->saveDATA(FHandle,(char*)CHUNK_FLOW_MADI,isCompiled);
            else if (current->data.type==hmiObject::FLOW_TYPE_MATH_DIGICMP)    current->saveDATA(FHandle,(char*)CHUNK_FLOW_DCMP,isCompiled);
            else if (current->data.type==hmiObject::FLOW_TYPE_MATH_NUMCMP)     current->saveDATA(FHandle,(char*)CHUNK_FLOW_NCMP,isCompiled);
            else if (current->data.type==hmiObject::FLOW_TYPE_MATH_CHARCMP)    current->saveDATA(FHandle,(char*)CHUNK_FLOW_CCMP,isCompiled);
            else if (current->data.type==hmiObject::FLOW_TYPE_MATH_DIGICTR)    current->saveDATA(FHandle,(char*)CHUNK_FLOW_DCTR,isCompiled);
            else if (current->data.type==hmiObject::FLOW_TYPE_MATH_NUMCTR)     current->saveDATA(FHandle,(char*)CHUNK_FLOW_NCTR,isCompiled);
            else if (current->data.type==hmiObject::FLOW_TYPE_MATH_CHARCTR)    current->saveDATA(FHandle,(char*)CHUNK_FLOW_CCTR,isCompiled);
            else if (current->data.type==hmiObject::FLOW_TYPE_MATH_BINCTR)     current->saveDATA(FHandle,(char*)CHUNK_FLOW_BCTR,isCompiled);
            else if (current->data.type==hmiObject::FLOW_TYPE_FLOW_STOP)       current->saveDATA(FHandle,(char*)CHUNK_FLOW_FSTP,isCompiled);
            else if (current->data.type==hmiObject::FLOW_TYPE_FLOW_START)      current->saveDATA(FHandle,(char*)CHUNK_FLOW_FSTA,isCompiled);
            else if (current->data.type==hmiObject::FLOW_TYPE_FLOW_DELAY)      current->saveDATA(FHandle,(char*)CHUNK_FLOW_FDLY,isCompiled);
            else if (current->data.type==hmiObject::FLOW_TYPE_FLOW_TIMER)      current->saveDATA(FHandle,(char*)CHUNK_FLOW_FTMR,isCompiled);
            else if (current->data.type==hmiObject::FLOW_TYPE_FLOW_NUMTGATE)   current->saveDATA(FHandle,(char*)CHUNK_FLOW_NTGT,isCompiled);
            else if (current->data.type==hmiObject::FLOW_TYPE_FLOW_NUMGATE)    current->saveDATA(FHandle,(char*)CHUNK_FLOW_NGTE,isCompiled);
            else if (current->data.type==hmiObject::FLOW_TYPE_FLOW_CHARTGATE)  current->saveDATA(FHandle,(char*)CHUNK_FLOW_CTGT,isCompiled);
            else if (current->data.type==hmiObject::FLOW_TYPE_FLOW_CHARGATE)   current->saveDATA(FHandle,(char*)CHUNK_FLOW_CGTE,isCompiled);
            else if (current->data.type==hmiObject::FLOW_TYPE_FLOW_BINTGATE)   current->saveDATA(FHandle,(char*)CHUNK_FLOW_BTGT,isCompiled);
            else if (current->data.type==hmiObject::FLOW_TYPE_FLOW_BINGATE)    current->saveDATA(FHandle,(char*)CHUNK_FLOW_BGTE,isCompiled);
            else if (current->data.type==hmiObject::FLOW_TYPE_FLOW_DIGITGATE)  current->saveDATA(FHandle,(char*)CHUNK_FLOW_DTGT,isCompiled);
            else if (current->data.type==hmiObject::FLOW_TYPE_FLOW_DIGIGATE)   current->saveDATA(FHandle,(char*)CHUNK_FLOW_DGTE,isCompiled);
            else if (current->data.type==hmiObject::FLOW_TYPE_MISC_LOG)        current->saveDATA(FHandle,(char*)CHUNK_MISC_LOGO,isCompiled);
            else if (current->data.type==hmiObject::FLOW_TYPE_MISC_LOG_REC)    current->saveDATA(FHandle,(char*)CHUNK_MISC_LOGR,isCompiled);
            else if (current->data.type==hmiObject::FLOW_TYPE_MISC_LOGIN)      current->saveDATA(FHandle,(char*)CHUNK_MISC_USLI,isCompiled);
            else if (current->data.type==hmiObject::FLOW_TYPE_MISC_ISCONNECT)  current->saveDATA(FHandle,(char*)CHUNK_MISC_ISCO,isCompiled);
         }
      }
      else wxASSERT(0);
      node = node->GetNext();
   }
}



void ObjectList::saveDeviceObjectData(wxFile *FHandle,bool isCompiled)
{
   wxNode *node = GetFirst();
   while (node)
   {
      flowObject *current=(flowObject*)node->GetData();
      if (current->data.type==FLOW_TYPE_EXTERNAL_LIB)
      {
         if (current->getMode()==OBJECT_MODE_DEVICE) ((flowExternalIOLib*)current)->saveEXIO(FHandle,isCompiled);
      }
      node = node->GetNext();
   }
}


void ObjectList::saveProject(wxString file,bool isCompiled)
{
   wxFile      *FHandle;
   wxInt32      length,l,i;
   wxFileOffset lastPos;

   wxASSERT(!d_isCompiling || (this!=&g_objectList));
   FHandle=new wxFile();
   FHandle->Create(file,true);
   if (FHandle->IsOpened())
   {
        struct hmiProjectData convData;
        wxMBConvUTF16BE       conv;
        char                  data[100+1];

        if (isCompiled) FHandle->Write(CHUNK_FORM"    "CHUNK_APCX,12); /********************************************************/
        else FHandle->Write(CHUNK_FORM"    "CHUNK_APCP,12); /********************************************************/

        strncpy(data,"ControlRoom",100);
        length=htonl(strlen(data));
        FHandle->Write(CHUNK_ANNO,4);  /****************************************************************/
        FHandle->Write(&length,4);
        FHandle->Write(data,strlen(data));

        FHandle->Write(CHUNK_VERS,4);  /****************************************************************/
        length=htonl(4); FHandle->Write(&length,4);
        l=htonl(1);      FHandle->Write(&l,4);

        FHandle->Write(CHUNK_PROJ,4);  /****************************************************************/
        length=htonl(sizeof(struct hmiProjectData));
        FHandle->Write(&length,4);
        convData.version        =htonl(4); // version
        convData.flags          =htonl(m_projectData->flags);
        convData.bgCol          =htonl(m_projectData->bgCol);
        convData.gridW          =htonl(m_projectData->gridW);
        convData.gridH          =htonl(m_projectData->gridH);
        convData.totalW         =htonl(m_projectData->totalW);
        convData.totalH         =htonl(m_projectData->totalH);
        convData.flowW          =htonl(m_projectData->flowW);
        convData.flowH          =htonl(m_projectData->flowH);
        convData.flowTimeout    =htonl(m_projectData->flowTimeout);
        convData.timerResolution=htonl(m_projectData->timerResolution);
        convData.editorX        =htonl(m_projectData->editorX);
        convData.editorY        =htonl(m_projectData->editorY);
        convData.editorW        =htonl(m_projectData->editorW);
        convData.editorH        =htonl(m_projectData->editorH);
        // new since 1.1
        convData.touchFactor    =htonl(m_projectData->touchFactor);
        conv.WC2MB(convData.font.store_faceName,g_projectTouchFontFaceName,sizeof(convData.font.store_faceName));
        convData.font.pointSize=htonl(m_projectData->font.pointSize);
        convData.font.style=htonl(m_projectData->font.style);
        convData.font.weight=htonl(m_projectData->font.weight);
        // new since 1.3
        strcpy(convData.m_remSingleIP,m_projectData->m_remSingleIP);
        // new since 1.4
        strcpy(convData.m_remDoubleIP,m_projectData->m_remDoubleIP);
        // end of new since

        FHandle->Write(&convData,sizeof(struct hmiProjectData));

        //TODO: save only the enabled-flags and no names when a project was compiled
        FHandle->Write(CHUNK_USPR,4); // user privileges ***************************************************
        length=htonl(0); FHandle->Write(&length,4);
        lastPos=FHandle->Tell()-4;

        l=htonl(g_userPriviData.enabled);
        FHandle->Write(&l,4);
        l=htonl(g_userPriviData.privisEnabled);
        FHandle->Write(&l,4);
        if (!isCompiled)
        {
           l=htonl(MAX_PRIVI_NUM);
           FHandle->Write(&l,4);
           for (i=0; i<MAX_PRIVI_NUM; i++)
           {  
              char store_name[300+4];
              
              oapc_unicode_stringToCharUTF16BE(g_userPriviData.priviName[i],store_name,300);
              FHandle->Write(store_name,300);
           }
        }
        l=FHandle->Tell()-lastPos-4; // write length of chunk

        l=htonl(l);
        FHandle->Seek(lastPos,wxFromStart);
        FHandle->Write(&l,4);
        FHandle->Seek(0,wxFromEnd);  // end of write length of chunk / user privileges ********************

        saveUserData(FHandle);

        if (g_objectList.getCount(HMI_TYPE_MASK,0)>0)
        {
			FHandle->Write(CHUNK_HOBL,4); // object list ******************************************************
			length=htonl(0); 
            FHandle->Write(&length,4);
			lastPos=FHandle->Tell()-4;

			if (!m_mutexList.empty())
			{
			   hmiMutex                 *mutex;
			   list<hmiMutex*>::iterator it;
			   list<wxUint32>::iterator  idIt;
			   wxUint32                  id;
			   wxMBConvUTF16BE           conv;
			   char                      store_name[204];

			   for ( it=m_mutexList.begin() ; it !=m_mutexList.end(); it++ )
			   {
				  mutex=*it;
				  FHandle->Write(CHUNK_HOBL_MTXL,4); // object list ***************************************************
				  length=htonl(200+(mutex->m_idQ.size()*4));
				  FHandle->Write(&length,4);
				  conv.WC2MB(store_name,mutex->m_name,200);
				  FHandle->Write(store_name,200);
				  for ( idIt=mutex->m_idQ.begin(); idIt!=mutex->m_idQ.end(); idIt++ )
				  {
					 id=htonl(*idIt);
					 FHandle->Write(&id,4);
				  }
			   }
			}

			wxNode *node = GetFirst();
			while (node)
			{
			   flowObject *current =(flowObject*)node->GetData();
			   if ((current->data.type & FLOW_TYPE_MASK)==0)
				current->save(FHandle);
			   node = node->GetNext();
			}

			l=htonl(FHandle->Tell()-lastPos-4); // write length of chunk
			FHandle->Seek(lastPos,wxFromStart);
			FHandle->Write(&l,4);
			FHandle->Seek(0,wxFromEnd);
        }

        FHandle->Write(CHUNK_FLOW,4); //flow connections list ***************************************************
        length=htonl(0); FHandle->Write(&length,4);
        lastPos=FHandle->Tell()-4;
        saveFlowObjectData(FHandle,isCompiled);
        l=FHandle->Tell()-lastPos-4; // write length of chunk
        l=htonl(l);
        FHandle->Seek(lastPos,wxFromStart);
        FHandle->Write(&l,4);
        FHandle->Seek(0,wxFromEnd);

        if (g_objectList.m_projectData->flags & PROJECT_FLAG_ISPACE_MASK)
        {
           if (g_objectList.getCount(FLOW_TYPE_MASK,OBJECT_MODE_DEVICE)>0)
           {
              FHandle->Write(CHUNK_DEVL,4); //device plugin list list ***************************************************
              length=htonl(0); FHandle->Write(&length,4);
              lastPos=FHandle->Tell()-4;
              saveDeviceObjectData(FHandle,isCompiled);
              l=FHandle->Tell()-lastPos-4; // write length of chunk
              l=htonl(l);
              FHandle->Seek(lastPos,wxFromStart);
              FHandle->Write(&l,4);
              FHandle->Seek(0,wxFromEnd);
           }
        }

        l=htonl(FHandle->Tell()-8); // total size of file data
        FHandle->Seek(4,wxFromStart);
        FHandle->Write(&l,4);

        FHandle->Close();

    }
    else wxMessageBox(_("Could not save file!"),_T("Error"),wxOK|wxCENTRE|wxICON_ERROR);
    delete FHandle;
}
#endif

#endif //ENV_INT
