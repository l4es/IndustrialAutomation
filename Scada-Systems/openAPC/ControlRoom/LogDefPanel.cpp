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
#include <wx/spinctrl.h>

#include "MainWin.h"
#include "LogDefPanel.h"
#include "globals.h"
#include "oapc_libio.h"



IMPLEMENT_CLASS(LogDefPanel, wxPanel)

BEGIN_EVENT_TABLE(LogDefPanel, wxPanel)
  EVT_CHECKBOX(wxID_ANY,LogDefPanel::OnCheckBoxCheck)
END_EVENT_TABLE()



LogDefPanel::LogDefPanel(struct hmiObjectLogData logData[MAX_LOG_TYPES],wxUint32 logFlags,wxWindow* parent)
            :wxPanel(parent)
{
   wxInt32 i;

   for (i=0; i<MAX_LOG_TYPES; i++)
   {
      useAboveValue[i]=NULL;   useBetweenValue[i]=NULL; useBelowValue[i]=NULL;
      useChangeValue[i]=NULL; 
      useHiValue[i]=NULL;      useLoValue[i]=NULL;

      aboveIntField[i]=NULL;   fromIntField[i]=NULL;    toIntField[i]=NULL;    belowIntField[i]=NULL;
      aboveFloatField[i]=NULL; fromFloatField[i]=NULL;  toFloatField[i]=NULL;  belowFloatField[i]=NULL;
   }
    
   if (logFlags!=0)
   {
      if ((logFlags==LOG_TYPE_INTNUM) ||
          (logFlags==LOG_TYPE_FLOATNUM))
      {
         wxFlexGridSizer *fSizer=new wxFlexGridSizer(3,1,4);
         SetSizer(fSizer);

         for (i=0; i<MAX_LOG_TYPES; i++)
         {
            wxStaticText *text;

            if (i==0) text=new wxStaticText(this,wxID_ANY,_("Error")+_T(":"));
            else if (i==1) text=new wxStaticText(this,wxID_ANY,_("Warning")+_T(":"));
            else if (i==2) text=new wxStaticText(this,wxID_ANY,_("Information")+_T(":"));
            else text=new wxStaticText(this,wxID_ANY,_("Event")+_T(":"));
            fSizer->Add(text,0,wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL);
            text=new wxStaticText(this,wxID_ANY,wxEmptyString); fSizer->Add(text);
            text=new wxStaticText(this,wxID_ANY,wxEmptyString); fSizer->Add(text);

            if (logFlags==LOG_TYPE_INTNUM)
            {
               useAboveValue[i]=new wxCheckBox(this,wxID_ANY,_("On value greater than")+_T(":"));
               fSizer->Add(useAboveValue[i],0,wxEXPAND);
               aboveIntField[i]=new wxSpinCtrl(this,wxID_ANY,wxEmptyString,wxDefaultPosition,wxDefaultSize,wxSP_ARROW_KEYS,MIN_NUM_VALUE,MAX_NUM_VALUE,logData[i].mAboveVal/1000.0);
               fSizer->Add(aboveIntField[i],0,wxEXPAND);
               text=new wxStaticText(this,wxID_ANY,wxEmptyString); fSizer->Add(text);

               useBetweenValue[i]=new wxCheckBox(this,wxID_ANY,_("On value between")+_T(":"));
               fSizer->Add(useBetweenValue[i],0,wxEXPAND);
               fromIntField[i]=new wxSpinCtrl(this,wxID_ANY,wxEmptyString,wxDefaultPosition,wxDefaultSize,wxSP_ARROW_KEYS,MIN_NUM_VALUE,MAX_NUM_VALUE,logData[i].mFromVal/1000.0);
               fSizer->Add(fromIntField[i],0,wxEXPAND);
               toIntField[i]=new wxSpinCtrl(this,wxID_ANY,wxEmptyString,wxDefaultPosition,wxDefaultSize,wxSP_ARROW_KEYS,MIN_NUM_VALUE,MAX_NUM_VALUE,logData[i].mToVal/1000.0);
               fSizer->Add(toIntField[i],0,wxEXPAND);

               useBelowValue[i]=new wxCheckBox(this,wxID_ANY,_("On value smaller than")+_T(":"));
               fSizer->Add(useBelowValue[i],0,wxEXPAND);
               belowIntField[i]=new wxSpinCtrl(this,wxID_ANY,wxEmptyString,wxDefaultPosition,wxDefaultSize,wxSP_ARROW_KEYS,MIN_NUM_VALUE,MAX_NUM_VALUE,logData[i].mBelowVal/1000.0);
               fSizer->Add(belowIntField[i],0,wxEXPAND);
               text=new wxStaticText(this,wxID_ANY,wxEmptyString); fSizer->Add(text);
            }
            else
            {          
               useAboveValue[i]=new wxCheckBox(this,wxID_ANY,_("On value greater than")+_T(":"));
               fSizer->Add(useAboveValue[i],0,wxEXPAND);

               aboveFloatField[i]=new oapcFloatCtrl(this,wxID_ANY,wxDefaultPosition,wxDefaultSize,wxTE_RIGHT,logData[i].mAboveVal/1000.0);
               Connect(aboveFloatField[i]->GetId(),wxEVT_COMMAND_TEXT_ENTER,wxCommandEventHandler(LogDefPanel::OnNumberChanged));
               aboveFloatField[i]->Connect(aboveFloatField[i]->GetId(),wxEVT_KILL_FOCUS,wxFocusEventHandler(LogDefPanel::OnNumberFocusChanged),NULL,this);
               fSizer->Add(aboveFloatField[i],0,wxEXPAND);
               text=new wxStaticText(this,wxID_ANY,wxEmptyString); fSizer->Add(text);

               useBetweenValue[i]=new wxCheckBox(this,wxID_ANY,_("On value between")+_T(":"));
               fSizer->Add(useBetweenValue[i],0,wxEXPAND);
               fromFloatField[i]=new oapcFloatCtrl(this,wxID_ANY,wxDefaultPosition,wxDefaultSize,wxTE_RIGHT,logData[i].mFromVal/1000.0);
               Connect(fromFloatField[i]->GetId(),wxEVT_COMMAND_TEXT_ENTER,wxCommandEventHandler(LogDefPanel::OnNumberChanged));
               fromFloatField[i]->Connect(fromFloatField[i]->GetId(),wxEVT_KILL_FOCUS,wxFocusEventHandler(LogDefPanel::OnNumberFocusChanged),NULL,this);
               fSizer->Add(fromFloatField[i],0,wxEXPAND);

               toFloatField[i]=new oapcFloatCtrl(this,wxID_ANY,wxDefaultPosition,wxDefaultSize,wxTE_RIGHT,logData[i].mToVal/1000.0);
               Connect(toFloatField[i]->GetId(),wxEVT_COMMAND_TEXT_ENTER,wxCommandEventHandler(LogDefPanel::OnNumberChanged));
               toFloatField[i]->Connect(toFloatField[i]->GetId(),wxEVT_KILL_FOCUS,wxFocusEventHandler(LogDefPanel::OnNumberFocusChanged),NULL,this);
               fSizer->Add(toFloatField[i],0,wxEXPAND);

               useBelowValue[i]=new wxCheckBox(this,wxID_ANY,_("On value smaller than")+_T(":"));
               fSizer->Add(useBelowValue[i],0,wxEXPAND);
               belowFloatField[i]=new oapcFloatCtrl(this,wxID_ANY,wxDefaultPosition,wxDefaultSize,wxTE_RIGHT,logData[i].mBelowVal/1000.0);
               Connect(belowFloatField[i]->GetId(),wxEVT_COMMAND_TEXT_ENTER,wxCommandEventHandler(LogDefPanel::OnNumberChanged));
               belowFloatField[i]->Connect(belowFloatField[i]->GetId(),wxEVT_KILL_FOCUS,wxFocusEventHandler(LogDefPanel::OnNumberFocusChanged),NULL,this);
               fSizer->Add(belowFloatField[i],0,wxEXPAND);
               text=new wxStaticText(this,wxID_ANY,wxEmptyString); fSizer->Add(text);
            }
 
            useChangeValue[i]=new wxCheckBox(this,wxID_ANY,_("On value changed"));
            fSizer->Add(useChangeValue[i],0,wxEXPAND);
            text=new wxStaticText(this,wxID_ANY,wxEmptyString); fSizer->Add(text);
            text=new wxStaticText(this,wxID_ANY,wxEmptyString); fSizer->Add(text);

            if (logData[i].flags & LOG_FLAG_ON_NUM_VALUE_ABOVE)   useAboveValue[i]->SetValue(1);
            if (logData[i].flags & LOG_FLAG_ON_NUM_VALUE_BELOW)   useBelowValue[i]->SetValue(1);
            if (logData[i].flags & LOG_FLAG_ON_NUM_VALUE_BETWEEN) useBetweenValue[i]->SetValue(1);
            if (logData[i].flags & LOG_FLAG_ON_VALUE_CHANGED)     useChangeValue[i]->SetValue(1);

//            fSizer->AddGrowableCol(1,2);
//            fSizer->AddGrowableCol(2,2);
         }
      }
      else // digi and string log types
      {
         wxBoxSizer *fSizer=new wxBoxSizer( wxVERTICAL );
         this->SetSizer(fSizer);

         for (i=0; i<MAX_LOG_TYPES; i++)
         {
            wxStaticText *text;

            if (i==0) text=new wxStaticText(this,wxID_ANY,_("Error")+_T(":"));
            else if (i==1) text=new wxStaticText(this,wxID_ANY,_("Warning")+_T(":"));
            else if (i==2) text=new wxStaticText(this,wxID_ANY,_("Information")+_T(":"));
            else text=new wxStaticText(this,wxID_ANY,_("Event")+_T(":"));
            fSizer->Add(text,0,wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL);
            fSizer->AddSpacer(4);

            if (logFlags==LOG_TYPE_DIGI)
            {
               useLoValue[i]=new wxCheckBox(this,wxID_ANY,_("On value set to LOW"));
               fSizer->Add(useLoValue[i],0,wxEXPAND);
               fSizer->AddSpacer(4);
               useHiValue[i]=new wxCheckBox(this,wxID_ANY,_("On value set to HIGH"));
               fSizer->Add(useHiValue[i],0,wxEXPAND);
               fSizer->AddSpacer(4);
            }
 
            useChangeValue[i]=new wxCheckBox(this,wxID_ANY,_("On value changed"));
            fSizer->Add(useChangeValue[i],0,wxEXPAND);
            fSizer->AddSpacer(8);

            if (logData[i].flags & LOG_FLAG_ON_VALUE_CHANGED)     useChangeValue[i]->SetValue(1);
            if (logData[i].flags & LOG_FLAG_ON_DIGI_VALUE_HI)     useHiValue[i]->SetValue(1);
            if (logData[i].flags & LOG_FLAG_ON_DIGI_VALUE_LO)     useLoValue[i]->SetValue(1);
            
         }
      }
   }
   updateUI();
}



LogDefPanel::~LogDefPanel()
{
}



void LogDefPanel::getConfigData(struct hmiObjectLogData logData[MAX_LOG_TYPES])
{
   wxInt32 i;

   // *** Logging Panel *******************
   for (i=0; i<MAX_LOG_TYPES; i++)
   {
      if ((useAboveValue[i]) && (useAboveValue[i]->GetValue()))
      {
         logData[i].flags|=LOG_FLAG_ON_NUM_VALUE_ABOVE;
         if (aboveIntField[i])        logData[i].mAboveVal=aboveIntField[i]->GetValue()*1000.0;
         else if (aboveFloatField[i]) logData[i].mAboveVal=aboveFloatField[i]->GetValue(MIN_NUM_VALUE,MAX_NUM_VALUE)*1000.0;
      }
      else logData[i].flags&=~LOG_FLAG_ON_NUM_VALUE_ABOVE;
      if ((useBelowValue[i]) && (useBelowValue[i]->GetValue()))
      {
         logData[i].flags|=LOG_FLAG_ON_NUM_VALUE_BELOW;
         if (belowIntField[i])        logData[i].mBelowVal=belowIntField[i]->GetValue()*1000.0;
         else if (belowFloatField[i]) logData[i].mBelowVal=belowFloatField[i]->GetValue(MIN_NUM_VALUE,MAX_NUM_VALUE)*1000.0;
      }
      else logData[i].flags&=~LOG_FLAG_ON_NUM_VALUE_BELOW;
      if ((useBetweenValue[i]) && (useBetweenValue[i]->GetValue()))
      {
         logData[i].flags|=LOG_FLAG_ON_NUM_VALUE_BETWEEN;
         if (fromIntField[i])        logData[i].mFromVal=fromIntField[i]->GetValue()*1000.0;
         else if (fromFloatField[i]) logData[i].mFromVal=fromFloatField[i]->GetValue(MIN_NUM_VALUE,MAX_NUM_VALUE)*1000.0;
         if (toIntField[i])          logData[i].mToVal=toIntField[i]->GetValue()*1000.0;
         else if (toFloatField[i])   logData[i].mToVal=toFloatField[i]->GetValue(MIN_NUM_VALUE,MAX_NUM_VALUE)*1000.0;
      }
      else logData[i].flags&=~LOG_FLAG_ON_NUM_VALUE_BETWEEN;
      if ((useChangeValue[i]) && (useChangeValue[i]->GetValue())) logData[i].flags|=LOG_FLAG_ON_VALUE_CHANGED;
      else logData[i].flags&=~LOG_FLAG_ON_VALUE_CHANGED;
      if ((useHiValue[i]) && (useHiValue[i]->GetValue())) logData[i].flags|=LOG_FLAG_ON_DIGI_VALUE_HI;
      else logData[i].flags&=~LOG_FLAG_ON_DIGI_VALUE_HI;
      if ((useLoValue[i]) && (useLoValue[i]->GetValue())) logData[i].flags|=LOG_FLAG_ON_DIGI_VALUE_LO;
      else logData[i].flags&=~LOG_FLAG_ON_DIGI_VALUE_LO;
   }
}



void LogDefPanel::updateUI()
{
   wxInt32 i;

   for (i=0; i<MAX_LOG_TYPES; i++)
   {
      if (useAboveValue[i])
      {
         if (aboveIntField[i])   aboveIntField[i]->Enable(useAboveValue[i]->GetValue());
         if (aboveFloatField[i]) aboveFloatField[i]->Enable(useAboveValue[i]->GetValue());
      }
      if (useBelowValue[i])
      {
         if (belowIntField[i])   belowIntField[i]->Enable(useBelowValue[i]->GetValue());
         if (belowFloatField[i]) belowFloatField[i]->Enable(useBelowValue[i]->GetValue());
      }
      if (useBetweenValue[i])
      {
         if (fromIntField[i])    fromIntField[i]->Enable(useBetweenValue[i]->GetValue());
         if (fromFloatField[i])  fromFloatField[i]->Enable(useBetweenValue[i]->GetValue());
         if (toIntField[i])      toIntField[i]->Enable(useBetweenValue[i]->GetValue());
         if (toFloatField[i])    toFloatField[i]->Enable(useBetweenValue[i]->GetValue());
      }
   }
}




void LogDefPanel::OnNumberChanged(wxCommandEvent &event)
{
   wxInt32 i;
   double d;

   for (i=0; i<MAX_LOG_TYPES; i++)
   {
      if ((aboveFloatField[i]) && (event.GetId()==aboveFloatField[i]->GetId()))
      {
         aboveFloatField[i]->SetValue(aboveFloatField[i]->GetValue(MIN_NUM_VALUE,MAX_NUM_VALUE));
         return;
      }
      else if ((belowFloatField[i]) && (event.GetId()==belowFloatField[i]->GetId()))
      {
         belowFloatField[i]->SetValue(belowFloatField[i]->GetValue(MIN_NUM_VALUE,MAX_NUM_VALUE));
         return;
      }
      else if ((fromFloatField[i]) && (event.GetId()==fromFloatField[i]->GetId()))
      {
         fromFloatField[i]->SetValue(fromFloatField[i]->GetValue(MIN_NUM_VALUE,MAX_NUM_VALUE));
         if (fromFloatField[i]->GetValue(MIN_NUM_VALUE,MAX_NUM_VALUE)>toFloatField[i]->GetValue(MIN_NUM_VALUE,MAX_NUM_VALUE))
         {
            d=fromFloatField[i]->GetValue(MIN_NUM_VALUE,MAX_NUM_VALUE);
            fromFloatField[i]->SetValue(toFloatField[i]->GetValue(MIN_NUM_VALUE,MAX_NUM_VALUE));
            toFloatField[i]->SetValue(d);
         }
         return;
      }
      else if ((toFloatField[i]) && (event.GetId()==toFloatField[i]->GetId()))
      {
         toFloatField[i]->SetValue(toFloatField[i]->GetValue(MIN_NUM_VALUE,MAX_NUM_VALUE));
         if (fromFloatField[i]->GetValue(MIN_NUM_VALUE,MAX_NUM_VALUE)>toFloatField[i]->GetValue(MIN_NUM_VALUE,MAX_NUM_VALUE))
         {
            d=fromFloatField[i]->GetValue(MIN_NUM_VALUE,MAX_NUM_VALUE);
            fromFloatField[i]->SetValue(toFloatField[i]->GetValue(MIN_NUM_VALUE,MAX_NUM_VALUE));
            toFloatField[i]->SetValue(d);
         }
         return;
      }
   }
}



void LogDefPanel::OnNumberFocusChanged(wxFocusEvent &event)
{
   wxCommandEvent cevent;

   cevent.SetId(event.GetId());
   OnNumberChanged(cevent);
   event.Skip(true);
}



void LogDefPanel::OnCheckBoxCheck(wxCommandEvent &WXUNUSED(event))
{
   updateUI();
}


