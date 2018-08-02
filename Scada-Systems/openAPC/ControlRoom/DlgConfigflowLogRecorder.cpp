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
#include <wx/bookctrl.h>
#include <wx/spinctrl.h>

#include "globals.h"
#include "LogDefPanel.h"
#include "hmiObject.h"
#include "DlgConfigflowLogRecorder.h"
#include "flowLogRecorder.h"



IMPLEMENT_CLASS(DlgConfigflowLogRecorder, wxDialog)

BEGIN_EVENT_TABLE(DlgConfigflowLogRecorder, wxDialog)
  EVT_BUTTON  (wxID_ANY,DlgConfigflowLogRecorder::OnButtonClick)
END_EVENT_TABLE()


DlgConfigflowLogRecorder::DlgConfigflowLogRecorder(flowLogRecorder *object,wxWindow* parent, const wxString& title,bool hideISConfig)
                         :DlgConfigflowName(object,parent,title,hideISConfig)
{  
   wxInt32  i;
   wxString txt;
   
   m_object=object;

   for (i=0; i<6; i++)
   {
      logPanel[i]=new LogDefPanel(object->logData.data[i],object->logData.logFlags[i],m_book);
      txt=wxString::Format(_T("IN%d"),i);
      m_book->AddPage(logPanel[i],txt,(i==0));
   }
   SetSize(DIALOG_X_SIZE,m_bSizer->Fit(this).y);
   Center();
   returnOK=1;
}



DlgConfigflowLogRecorder::~DlgConfigflowLogRecorder()
{
}



void DlgConfigflowLogRecorder::OnButtonClick(wxCommandEvent &event)
{
   if (event.GetId()==wxID_OK)
   {
      wxInt32 i;

      for (i=0; i<6; i++)
      {
         logPanel[i]->getConfigData(m_object->logData.data[i]);
      }
      handleStdElements();
      Destroy();
   }
   else if (event.GetId()==wxID_CANCEL)
   {
      Destroy();
   }
}


