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

#include "DlgConfigflowExternalIOLib.h"


DlgConfigflowExternalIOLib::DlgConfigflowExternalIOLib(wxString *objectName,XMLDecoder *xmlDecoder,wxWindow* parent,wxUint64 stdOUT,wxUint32 *cycleMicros, const wxString& title,bool hideISConfig)
             :wxDialog(parent,wxID_ANY,title,wxDefaultPosition,wxDefaultSize,wxRESIZE_BORDER|wxCAPTION)
{
   m_xmlDecoder=xmlDecoder;
   xmlDecoder->reInit();
   returnOK=0;
   m_xmlDecoder->constructUI(this,objectName,&returnOK,stdOUT,cycleMicros,hideISConfig);
   Connect(wxID_OK,wxEVT_COMMAND_BUTTON_CLICKED,wxCommandEventHandler(DlgConfigflowExternalIOLib::OnButtonClick),NULL,this);
   Connect(wxID_CANCEL,wxEVT_COMMAND_BUTTON_CLICKED,wxCommandEventHandler(DlgConfigflowExternalIOLib::OnButtonClick),NULL,this);
}


DlgConfigflowExternalIOLib::~DlgConfigflowExternalIOLib()
{
}


wxByte DlgConfigflowExternalIOLib::getNextPair(wxString *name,wxString *value,wxFont *font)
{
   return m_xmlDecoder->getNextPair(name,value,font);
}


void DlgConfigflowExternalIOLib::OnButtonClick(wxCommandEvent &event)
{
   m_xmlDecoder->OnButtonClick(event);
}
