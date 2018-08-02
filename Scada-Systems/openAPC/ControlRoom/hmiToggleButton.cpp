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
#include <wx/tglbtn.h>

#ifndef __WXMSW__
#include <arpa/inet.h>
#endif

#include "iff.h"
#include "globals.h"
#include "oapc_libio.h"
#include "HMICanvas.h"
#include "hmiObject.h"
#include "hmiToggleButton.h"


hmiToggleButton::hmiToggleButton(BasePanel *parent,flowObject *obj)
                :hmiSimpleButton(parent,obj)
{
   this->data.type=HMI_TYPE_TOGGLEBUTTON;
   this->data.stdIN=OAPC_DIGI_IO0|OAPC_DIGI_IO1|OAPC_CHAR_IO3;
   this->data.stdOUT=OAPC_DIGI_IO0|OAPC_DIGI_IO1;
#ifdef ENV_EDITOR
   noUIFlags=OAPC_HMI_NO_UI_MINMAX|OAPC_HMI_NO_UI_RO;
   if (obj==NULL)
   {
       this->data.msizey=40*1000;
       text[0]=getDefaultName();
       text[1]=getDefaultName();
   }
#else
   this->m_mutexID=-1;
#endif
}



wxWindow *hmiToggleButton::createUIElement()
{
   uiElement=new wxToggleButton(m_parent,wxID_ANY,g_res->getString(text[0]));
   hmiObject::createUIElement();
#ifdef ENV_PLAYER
   g_hmiCanvas->Connect(uiElement->GetId(),wxEVT_COMMAND_TOGGLEBUTTON_CLICKED,wxCommandEventHandler(HMICanvas::OnButtonClicked));
#endif
   return uiElement;
}



hmiToggleButton::~hmiToggleButton()
{
}



#ifdef ENV_EDITOR
wxString hmiToggleButton::getDefaultName()
{
   return _("Toggle Button");
}



wxInt32 hmiToggleButton::save(wxFile *FHandle)
{
   wxInt32 length;

   if (!FHandle) return 0;

   FHandle->Write(CHUNK_HOBL_TOBU,4);
   length=sizeof(struct hmiObjectHead)+sizeof(struct hmiObjectData);
   length=htonl(length);
   FHandle->Write(&length,4);
   length=hmiObject::save(FHandle);

   return length;
}



void hmiToggleButton::OnCombobox(wxCommandEvent &WXUNUSED(event))
{
   wxInt32 sel;

   sel=comboBox->GetSelection();
   if (sel==(wxInt32)(comboBox->GetCount()-1))
   {
      wxString name;

      name=wxGetTextFromUser(_("Please enter a name for the new mutex"),_("Enter mutex name"));
      if (name.Length()>0)
      {
         hmiMutex *mutex;

         mutex=new hmiMutex(name);
         g_objectList.m_mutexList.push_back(mutex);
         mutex->addObjectID(data.id);
         comboBox->Insert(name,sel);
         comboBox->Select(sel);
      }
   }
   else
   {
      hmiMutex *mutex;

      do
      {
         mutex=g_objectList.getMutexByObjectID(data.id);
         if (mutex) mutex->removeObjectID(data.id);
      }
      while (mutex); // remove the object ID from all mutexes that (accidentially) may manage it
      mutex=g_objectList.getMutexByName(comboBox->GetValue());
      mutex->addObjectID(data.id);
   }
}



wxPanel* hmiToggleButton::getConfigPanel(wxString *name,wxWindow *parent,wxInt32 num)
{
   list<hmiMutex*>::iterator it;
   hmiMutex                 *mutex;
   wxInt32                   cnt;

   if (num>0) return NULL;

   wxPanel *panel=new wxPanel(parent);

   wxFlexGridSizer *fSizer=new wxFlexGridSizer(2,4,4);

   wxStaticText *text=new wxStaticText(panel,wxID_ANY,_("Mutual Exclusion")+_T(":"));
   fSizer->Add(text,0,wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL);

   comboBox=new wxComboBox(panel,wxID_ANY,_T(""),wxDefaultPosition,wxDefaultSize,0,NULL,wxCB_DROPDOWN|wxCB_READONLY);

   comboBox->SetSelection(wxNOT_FOUND);
   cnt=0;
//�-���
   for ( it=g_objectList.m_mutexList.begin(); it!=g_objectList.m_mutexList.end(); it++)
   {
      comboBox->Append((*it)->m_name);
      mutex=(*it);
      if (mutex->hasObjectID(data.id)) comboBox->SetSelection(cnt);
      cnt++;
   }

   comboBox->Append(_("Create new Mutex")+_T("..."));
   comboBox->Connect(wxEVT_COMMAND_COMBOBOX_SELECTED,wxCommandEventHandler(hmiToggleButton::OnCombobox),NULL,this);
//   comboBox->Select(labelData.alignment);
   fSizer->Add(comboBox,1,wxEXPAND);

   fSizer->AddGrowableCol(1,1);
   panel->SetSizer(fSizer);

   *name=_("Mutual Exclusion");
   return panel;
}



void hmiToggleButton::setConfigData()
{
    if (uiElement) ((wxToggleButton*)uiElement)->SetValue(false);
//    labelData.alignment=comboBox->GetSelection();
}
#endif // ENV_EDITOR



void hmiToggleButton::doApplyData(const wxByte all)
{
    hmiSimpleButton::doApplyData(all);
//    if (data.state[digi[0]] & hmiObject::OAPC_STATE_FLAG_SELECTED) ((wxToggleButton*)uiElement)->SetValue(true);
//    else ((wxToggleButton*)uiElement)->SetValue(false);
    uiElement->Refresh();
}



#ifdef ENV_PLAYER
/**
Sets an digital input and checks if the same thread has set that input before
@param[in] input the flag that specifies the input
@param[in] value the value that has to be set for that input (0 or 1)
@param[in] flowThreadID the Id of the thread that wants to set this value
@return 0 if the same thread already has set a value; the thread has to finish
        in this case to avoid endless loops; 1 if the input value was set correctly
*/
wxByte hmiToggleButton::setDigitalInput(FlowConnection *connection,wxByte value,wxUint32 *flowThreadID,flowObject *object)
{
//   if (!threadIDOK(flowThreadID,false)) return 0;
   if (connection->targetInputNum==0)
   {
      switch (data.type)
      {
         case HMI_TYPE_RADIOBUTTON:
            ((wxRadioButton*)uiElement)->SetLabel(g_res->getString(text[value]));
            break;
         case HMI_TYPE_CHECKBOX:
            ((wxCheckBox*)uiElement)->SetLabel(g_res->getString(text[value]));
            break;
         case HMI_TYPE_TOGGLEBUTTON:
            ((wxToggleButton*)uiElement)->SetLabel(g_res->getString(text[value]));
            break;
         default:
            assert(0);
            break;
      }
   }
   else if (connection->targetInputNum==1) setSelected(value!=0);
   return hmiObject::setDigitalInput(connection,value,flowThreadID,object);
}



void hmiToggleButton::updateSelectionState()
{
   if (data.type==HMI_TYPE_TOGGLEBUTTON)
   {
      if (((wxToggleButton*)uiElement)->GetValue())
       setSelected(true);
   }
}



void hmiToggleButton::setSelected(bool selected)
{
   if (data.type==HMI_TYPE_TOGGLEBUTTON)
   {
      if (!selected)
      {
         wxCommandEvent event;

         ((wxToggleButton*)uiElement)->SetValue(selected);
         event.SetId(uiElement->GetId());
         g_hmiCanvas->OnButtonClicked(event);
      }
      if ((selected) && (m_mutexID>0))
      {
         hmiMutex                *mutex;
         hmiObject               *object;
         list<wxUint32>::iterator it;

         mutex=g_objectList.getMutexByObjectID(data.id);
         if (mutex)
         {
            for (it=mutex->m_idQ.begin(); it!=mutex->m_idQ.end(); it++)
            {
               if ((*it)!=data.id)
               {
                  object=(hmiObject*)g_objectList.getObject(*it);
                  if (object) object->setSelected(false);
               }
            }
         }
      }
   
   }
   else
   {
      switch (data.type)
      {
         case HMI_TYPE_RADIOBUTTON:
            ((wxRadioButton*)uiElement)->SetValue(selected);
            break;
         case HMI_TYPE_CHECKBOX:
            ((wxCheckBox*)uiElement)->SetValue(selected);
            break;
         default:
            assert(0);
            break;
      }
   }
}



wxByte hmiToggleButton::getDigitalOutput(FlowConnection *connection,wxInt32 *rcode,wxUint64 lastInput,wxLongLong origCreationTime)
{
   wxByte   val;

#ifdef ENV_DEBUGGER
   if ((connection->sourceOutputNum<0) || (connection->sourceOutputNum>=MAX_NUM_IOS))
   {
      wxASSERT(0);
      g_debugWin->setDebugInformation(this,DEBUG_STOP_COND_ILLEGAL_IO,_T(""));
      *rcode=OAPC_ERROR_NO_SUCH_IO;
      return 0;
   }
#endif
   if (connection->sourceOutputNum==1)
   {
      *rcode=OAPC_OK;
      switch (data.type)
      {
         case HMI_TYPE_RADIOBUTTON:
            val=((wxRadioButton*)uiElement)->GetValue();
            break;
         case HMI_TYPE_CHECKBOX:
            val=((wxCheckBox*)uiElement)->GetValue();
            break;
         case HMI_TYPE_TOGGLEBUTTON:
            val=((wxToggleButton*)uiElement)->GetValue();
            break;
         default:
            assert(0);
            val=0;
            break;
      }
      logDataCtrl->logDigi(val,this);
      return val;
   }
   return hmiObject::getDigitalOutput(connection,rcode,lastInput,origCreationTime);
}



wxUint64 hmiToggleButton::getAssignedOutput(wxUint64 input)
{
   if (input==OAPC_DIGI_IO1) return OAPC_DIGI_IO1;
   else return hmiObject::getAssignedOutput(input);
}



/*void hmiToggleButton::applyToState()
{
   digi[0]=((wxToggleButton*)uiElement)->GetValue();
   applyData(0);
}*/
#endif


