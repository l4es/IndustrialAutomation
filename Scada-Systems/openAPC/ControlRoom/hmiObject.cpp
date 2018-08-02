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
 #include "../hmiObject.cpp"
#else //ENV_INT

#include <wx/wx.h>
#include <wx/font.h>
#include <wx/thread.h>
#include <wx/dcbuffer.h>

#ifndef __WXMSW__
#include <arpa/inet.h>
#endif

#include "iff.h"
#include "globals.h"
#include "MainApp.h"
#include "flowObject.h"
#include "hmiObject.h"
#include "hmiSimpleButton.h"
#include "HMICanvas.h"
#include "hmiFloatField.h"
#include "hmiHGauge.h"
#include "hmiHSlider.h"
#include "hmiImage.h"
#include "hmiPrimitive.h"
#include "hmiImageButton.h"
#include "hmiToggleImageButton.h"
#include "hmiLine.h"
#include "hmiNumField.h"
#include "hmiSimpleButton.h"
#include "hmiTextLabel.h"
#include "hmiToggleButton.h"
#include "hmiRadioButton.h"
#include "hmiTextField.h"
#include "hmiVGauge.h"
#include "hmiVSlider.h"
#include "hmiTabbedPane.h"
#include "hmiStackedPane.h"
#include "hmiAdditionalPane.h"
#include "hmiSinglePanel.h"
#include "hmiAngularMeter.h"
#include "hmiAngularRegulator.h"
#include "hmiLCDNumber.h"
#include "hmiSpecialPanel.h"
#include "hmiExternalIOLib.h"


hmiObject::hmiObject(BasePanel *parent,flowObject *obj)
          :flowObject(obj)
          , uiElement(NULL)
          , returnOK(false)
#ifndef ENV_EDITOR
          , logDataCtrl(NULL)
#endif
          , lastFont_pointSize(-1)
          , lastFont_style(-1)
          , lastFont_weight(-1)
          , lastFont_faceName(_T(""))
          , m_loadError(OAPC_OK)
{
   m_parent=parent;
#ifdef ENV_EDITOR
   m_userPriviEnable=OAPC_USERPRIVI_DISABLE|OAPC_USERPRIVI_HIDE;
   noUIFlags=0xFFFFFFFF;
   noFlowFlags=0;
   data.reservedLayoutAlignment=0;
   data.hmiFlags=0;
#endif
   if (m_parent) data.isChildOf=m_parent->m_myID; // no parent in case of silent load
   returnOK=true;
}


hmiObject::~hmiObject()
{
   hmiMutex *mutex;

   mutex=g_objectList.getMutexByObjectID(this->data.id);
   if (mutex) mutex->removeObjectID(this->data.id);
#ifdef ENV_PLAYER
   if (logDataCtrl) delete logDataCtrl;
#endif
   if ((uiElement) && (m_doDestroy))
   {
#ifdef ENV_PLAYER
      uiElement->Close();
      uiElement->Destroy();
#else
      uiElement->Show(false); // just hide it, deletion out of an event handler would cause a crash
#endif
   }
}


void hmiObject::OnValueChanged(wxCommandEvent &event)
{
   if (event.GetId()==data.type)
    doApplyData(event.GetEventObject()!=NULL);

   if (event.GetEventObject()) delete event.GetEventObject();
}


void hmiObject::triggerGUIAction(const int id,const wxByte all)
{
   wxCommandEvent* evt = new wxCommandEvent();

   if (all) evt->SetEventObject(new wxObject());
   else evt->SetEventObject(NULL);
   evt->SetEventType(wxEVT_COMMAND_BUTTON_CLICKED);
   evt->SetId(id);
   AddPendingEvent(*evt);
}


void hmiObject::setParent(BasePanel *parent)
{
   m_parent=parent;
   data.isChildOf=parent->m_myID;
}


void hmiObject::doApplyData(const wxByte WXUNUSED(all))
{
}


void hmiObject::applyData(wxByte all)
{
   triggerGUIAction(data.type,all);
}


bool hmiObject::applyLayoutData()
{
   return false;
}

void hmiObject::drawObject(wxAutoBufferedPaintDC* /*dc*/)
{
}


wxWindow *hmiObject::createUIElement()
{
   if (!uiElement) return NULL;
   applyData(1);

#ifndef ENV_PLAYER
   uiElement->Connect(wxEVT_RIGHT_UP,wxMouseEventHandler(HMICanvas::OnChildMouseEvent),NULL,m_parent);
   uiElement->Connect(wxEVT_MOTION,wxMouseEventHandler(HMICanvas::OnChildMouseEvent),NULL,m_parent);
   uiElement->Connect(wxEVT_LEFT_DOWN,wxMouseEventHandler(HMICanvas::OnChildMouseEvent),NULL,m_parent);
   uiElement->Connect(wxEVT_LEFT_UP,wxMouseEventHandler(HMICanvas::OnChildMouseEvent),NULL,m_parent);
   uiElement->Connect(wxEVT_LEFT_DCLICK,wxMouseEventHandler(HMICanvas::OnChildMouseEvent),NULL,m_parent);
#endif

   return uiElement;
}



void hmiObject::applyFont(wxWindow *ui)
{
   if ((lastFont_pointSize!=data.font[digi[0]].pointSize) ||
       (lastFont_style    !=data.font[digi[0]].style) ||
       (lastFont_weight   !=data.font[digi[0]].weight) ||
       (lastFont_faceName.Cmp(fontFaceName[digi[0]])!=0))
   {
#if wxCHECK_VERSION(2,9,0)
      g_setFont(ui,wxFont(data.font[digi[0]].pointSize,
                          wxFONTFAMILY_DEFAULT,
                          (wxFontStyle)data.font[digi[0]].style,
                          (wxFontWeight)data.font[digi[0]].weight,
                          false,
                          fontFaceName[digi[0]]));
#else
      g_setFont(ui,wxFont(data.font[digi[0]].pointSize,
                          wxFONTFAMILY_DEFAULT,
                          data.font[digi[0]].style,
                          data.font[digi[0]].weight,
                          false,
                          fontFaceName[digi[0]]));
#endif

      lastFont_pointSize=data.font[digi[0]].pointSize;
      lastFont_style=data.font[digi[0]].style;
      lastFont_weight=data.font[digi[0]].weight;
      lastFont_faceName=fontFaceName[digi[0]];
   }
}


wxInt32 hmiObject::getLoadError()
{
   return m_loadError;
}


wxInt32 hmiObject::load(wxFile *FHandle,wxUint32 chunkSize,wxUint32 IDOffset)
{
   wxInt32 res;

   res=flowObject::load(FHandle,chunkSize,IDOffset);
#ifdef ENV_PLAYER
   logDataCtrl=new LogDataCtrl(data.log);
#endif

   if ((data.hmiFlags & 0xFFFFFFFE)!=0) data.hmiFlags=0; // for compatibility with old projects where hmiFlags does not has been initialised correctly
   return res;
}



#ifdef ENV_EDITOR
hmiObject *hmiObject::duplicate()
{
    hmiObject *dupObject=NULL;
    if (data.type==HMI_TYPE_FLOATFIELD)             dupObject=new hmiFloatField(m_parent,this);
    else if (data.type==HMI_TYPE_HGAUGE)            dupObject=new hmiHGauge(m_parent,this);
    else if (data.type==HMI_TYPE_HSLIDER)           dupObject=new hmiHSlider(m_parent,this);
    else if (data.type==HMI_TYPE_IMAGE)             dupObject=new hmiImage(m_parent,this);
    else if ((data.type==HMI_TYPE_FREELINE) || (data.type==HMI_TYPE_FRAME) || (data.type==HMI_TYPE_ELLIPSE))
                                                    dupObject=new hmiPrimitive(m_parent,this,data.type);
    else if (data.type==HMI_TYPE_IMAGEBUTTON)       dupObject=new hmiImageButton(m_parent,this);
    else if (data.type==HMI_TYPE_TOGGLEIMAGEBUTTON) dupObject=new hmiToggleImageButton(m_parent,this);
    else if (data.type==HMI_TYPE_LINE)              dupObject=new hmiLine(m_parent,this);
    else if (data.type==HMI_TYPE_NUMFIELD)          dupObject=new hmiNumField(m_parent,this);
    else if (data.type==HMI_TYPE_SIMPLEBUTTON)      dupObject=new hmiSimpleButton(m_parent,this);
    else if (data.type==HMI_TYPE_TEXTLABEL)         dupObject=new hmiTextLabel(m_parent,this);
    else if (data.type==HMI_TYPE_TOGGLEBUTTON)      dupObject=new hmiToggleButton(m_parent,this);
    else if (data.type==HMI_TYPE_RADIOBUTTON)       dupObject=new hmiRadioButton(m_parent,this,HMI_TYPE_RADIOBUTTON);
    else if (data.type==HMI_TYPE_CHECKBOX)          dupObject=new hmiRadioButton(m_parent,this,HMI_TYPE_CHECKBOX);
    else if (data.type==HMI_TYPE_TEXTFIELD)         dupObject=new hmiTextField(m_parent,this,HMI_TYPE_TEXTFIELD);
    else if (data.type==HMI_TYPE_PASSWORDFIELD)     dupObject=new hmiTextField(m_parent,this,HMI_TYPE_PASSWORDFIELD);
    else if (data.type==HMI_TYPE_VGAUGE)            dupObject=new hmiVGauge(m_parent,this);
    else if (data.type==HMI_TYPE_VSLIDER)           dupObject=new hmiVSlider(m_parent,this);
    else if (data.type==HMI_TYPE_TABBEDPANE)        dupObject=new hmiTabbedPane(m_parent,this);
    else if (data.type==HMI_TYPE_STACKEDPANE)       dupObject=new hmiStackedPane(m_parent,this);
    else if (data.type==HMI_TYPE_ADDITIONALPANE)    dupObject=new hmiAdditionalPane(m_parent,this);
    else if (data.type==HMI_TYPE_SINGLEPANEL)       dupObject=new hmiSinglePanel(m_parent,this);
    else if (data.type==HMI_TYPE_ANGULARMETER)      dupObject=new hmiAngularMeter(m_parent,this);
    else if (data.type==HMI_TYPE_LCDNUMBER)         dupObject=new hmiLCDNumber(m_parent,this);
    else if (data.type==HMI_TYPE_ANGULARREGULATOR)  dupObject=new hmiAngularRegulator(m_parent,this);
    else if (data.type==HMI_TYPE_SP_USERMANAGEMENT) dupObject=new hmiSpecialPanel(m_parent,this,HMI_TYPE_SP_USERMANAGEMENT);
    else if (data.type==HMI_TYPE_EXTERNAL_LIB)
    {
       dupObject=new hmiExternalIOLib(((hmiExternalIOLib*)this)->lib,m_parent,this,0);
    }
    wxASSERT(dupObject);
    if (!dupObject) return NULL;
    dupObject->createUIElement();
    dupObject->setData(this);

    return dupObject;
}



wxString hmiObject::getDefaultName()
{
    wxASSERT(0);
    return _T("##invalid hmiObject##");
}



wxPoint hmiObject::getMinSize()
{
   return wxPoint(10,10);
}


wxPoint hmiObject::getMaxSize()
{
   return wxPoint(10000,10000);
}


wxPoint hmiObject::getDefSize()
{
    return wxPoint(40,20);
}
#endif



void hmiObject::setSize(wxRealPoint msize)
{
   data.msizex=msize.x;
   data.msizey=msize.y;
   if ((uiElement) && (data.msizex>0))
    uiElement->SetSize(msize.x/1000.0,msize.y/1000.0);
}



wxRealPoint hmiObject::getSize()
{
   return wxRealPoint(data.msizex,data.msizey);
}



void hmiObject::setPos(wxRealPoint pos)
{
   data.mposx=pos.x;
   data.mposy=pos.y;
   if ((uiElement) && (data.msizex>0))
    uiElement->SetPosition(wxPoint(pos.x/1000.0,pos.y/1000.0));
}



wxRealPoint hmiObject::getPos()
{
   return wxRealPoint(data.mposx,data.mposy);
}



#ifdef ENV_PLAYER
void hmiObject::setUIElementState(wxInt32 state)
{
   wxASSERT(uiElement);
   if (!uiElement) return;
   if (state==2)
   {
      if (!uiElement->IsShown()) uiElement->Show(true);
      if (!uiElement->IsEnabled()) uiElement->Enable(true);
   }
   else if (state==1)
   {
      if (!uiElement->IsShown()) uiElement->Show(true);
      if (uiElement->IsEnabled()) uiElement->Enable(false);
   }
   else if (state==0) uiElement->Show(false);
}
#endif



#ifdef ENV_EDITOR
wxPanel* hmiObject::getConfigPanel(wxString *WXUNUSED(name),wxWindow *WXUNUSED(parent),wxInt32 WXUNUSED(num))
{
    return NULL;
}



void hmiObject::setConfigData()
{
}


wxInt32 hmiObject::saveSOBJ(wxFile *FHandle,bool isCompiled)
{
   struct hmiFlowData             flowData;
   wxList::compatibility_iterator node;
   FlowConnection                *connection;
   wxInt32                        length;

   if (!FHandle) return 0;
   if (flowList.GetCount()<0) return 0;
   checkConnections();
   FHandle->Write(CHUNK_FLOW_SOBJ,4);
   if (isCompiled) length=sizeof(struct hmiFlowData)+(flowList.GetCount()*FLOW_CONNECTION_HEAD_SIZE);
   else length=sizeof(struct hmiFlowData)+(flowList.GetCount()*sizeof(struct flowConnectionData));
   length=htonl(length);
   FHandle->Write(&length,4);

   wxASSERT(data.id>0);
   flowData.id=htonl(data.id);
   flowData.version=htonl(1);
   flowData.usedFlows=htonl(flowList.GetCount());
   flowData.maxEdges=htonl(MAX_CONNECTION_EDGES);
   flowData.flowFlags=htonl(m_flowFlags);
   length=FHandle->Write(&flowData,sizeof(struct hmiFlowData));

   node=flowList.GetFirst();
   while (node)
   {
      connection=(FlowConnection*)node->GetData();
      length+=connection->saveFlow(FHandle,isCompiled);
      node=node->GetNext();
   }
   return length;
}
#else



wxUint64 hmiObject::getAssignedOutput(wxUint64 input)
{
   if (input==OAPC_DIGI_IO0) return OAPC_DIGI_IO0;
   return 0;
}



void hmiObject::setSelected(bool WXUNUSED(selected))
{
}



void hmiObject::updateSelectionState()
{
}
#endif

#endif //ENV_INT