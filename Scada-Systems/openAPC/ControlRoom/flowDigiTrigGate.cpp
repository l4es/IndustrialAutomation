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
#include <wx/checkbox.h>

#ifdef ENV_DEBUGGER
#include "DebugWin.h"
#endif
#include "iff.h"
#include "globals.h"
#include "flowObject.h"
#include "flowDigiTrigGate.h"
#include "oapc_libio.h"


flowDigiTrigGate::flowDigiTrigGate():flowLogic()
{
   this->data.type=flowObject::FLOW_TYPE_FLOW_DIGITGATE;
   this->data.stdIN=OAPC_DIGI_IO0|OAPC_DIGI_IO1|OAPC_DIGI_IO2|OAPC_DIGI_IO3|OAPC_DIGI_IO4|OAPC_DIGI_IO5|OAPC_DIGI_IO6|OAPC_DIGI_IO7;
   this->data.stdOUT=            OAPC_DIGI_IO1|OAPC_DIGI_IO2|OAPC_DIGI_IO3|OAPC_DIGI_IO4|OAPC_DIGI_IO5|OAPC_DIGI_IO6|OAPC_DIGI_IO7;
#ifdef ENV_PLAYER
   m_digiValid=false;
#endif
}



flowDigiTrigGate::~flowDigiTrigGate()
{
}



wxString flowDigiTrigGate::getDefaultName()
{
   return _T("Digital Triggered Gate");
}



#ifdef ENV_EDITOR
wxInt32 flowDigiTrigGate::saveDATA(wxFile *FHandle,char chunkName[4],bool isCompiled)
{
   return flowObject::saveDATA(FHandle,chunkName,isCompiled);
}
#else



wxUint64 flowDigiTrigGate::getAssignedOutput(wxUint64 input)
{
   if ((m_digiValid) && (input!=OAPC_DIGI_IO0))
   {
      return input;
   }
   return 0;
}



wxByte flowDigiTrigGate::setDigitalInput(FlowConnection *connection,wxByte value,wxUint32 *flowThreadID,flowObject *object)
{
   if (connection->targetInputNum==0) m_digiValid=true;
   return flowObject::setDigitalInput(connection,value,flowThreadID,object);
}



wxByte flowDigiTrigGate::getDigitalOutput(FlowConnection *connection,wxInt32 *rcode,wxUint64 WXUNUSED(lastInput),wxLongLong WXUNUSED(origCreationTime))
{
#ifdef ENV_DEBUGGER
   if (connection->sourceOutputNum==0)
   {
      wxASSERT(0);
      g_debugWin->setDebugInformation(this,DEBUG_STOP_COND_ILLEGAL_IO,_T(""));
      *rcode=OAPC_ERROR_NO_SUCH_IO;
      return 0;
   }
#endif
   if ((!m_digiValid) || (digi[connection->sourceOutputNum]==0)) *rcode=OAPC_ERROR_NO_DATA_AVAILABLE;
   else *rcode=OAPC_OK;
   return digi[0];
}



#endif


wxInt32 flowDigiTrigGate::loadDATA(wxFile *FHandle,wxUint32 chunkSize,wxUint32 IDOffset,bool isCompiled)
{
   return flowObject::loadDATA(FHandle,chunkSize,IDOffset,isCompiled);
}


