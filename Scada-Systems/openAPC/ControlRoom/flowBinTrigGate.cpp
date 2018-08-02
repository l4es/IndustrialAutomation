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

#ifndef __WXMSW__
#include <arpa/inet.h>
#endif

#ifdef ENV_DEBUGGER
#include "DebugWin.h"
#endif
#include "iff.h"
#include "globals.h"
#include "flowObject.h"
#include "flowBinTrigGate.h"
#include "oapc_libio.h"


flowBinTrigGate::flowBinTrigGate():flowLogic()
{
   this->data.type=flowObject::FLOW_TYPE_FLOW_BINTGATE;
   this->data.stdIN=OAPC_BIN_IO0|OAPC_DIGI_IO1|OAPC_DIGI_IO2|OAPC_DIGI_IO3|OAPC_DIGI_IO4|OAPC_DIGI_IO5|OAPC_DIGI_IO6|OAPC_DIGI_IO7;
   this->data.stdOUT=           OAPC_BIN_IO1| OAPC_BIN_IO2| OAPC_BIN_IO3| OAPC_BIN_IO4| OAPC_BIN_IO5| OAPC_BIN_IO6| OAPC_BIN_IO7;
#ifdef ENV_PLAYER
   m_binValid=false;
#endif
}



flowBinTrigGate::~flowBinTrigGate()
{
}



wxString flowBinTrigGate::getDefaultName()
{
   return _T("Binary Triggered Gate");
}



#ifdef ENV_EDITOR
wxInt32 flowBinTrigGate::saveDATA(wxFile *FHandle,char chunkName[4],bool isCompiled)
{
   return flowObject::saveDATA(FHandle,chunkName,isCompiled);
}
#else



wxUint64 flowBinTrigGate::getAssignedOutput(wxUint64 input)
{
   if ((m_binValid) && (input!=OAPC_BIN_IO0))
   {
      if (input==OAPC_DIGI_IO1) return OAPC_BIN_IO1;
      if (input==OAPC_DIGI_IO2) return OAPC_BIN_IO2;
      if (input==OAPC_DIGI_IO3) return OAPC_BIN_IO3;
      if (input==OAPC_DIGI_IO4) return OAPC_BIN_IO4;
      if (input==OAPC_DIGI_IO5) return OAPC_BIN_IO5;
      if (input==OAPC_DIGI_IO6) return OAPC_BIN_IO6;
      if (input==OAPC_DIGI_IO7) return OAPC_BIN_IO7;
   }
   return 0;
}



wxByte flowBinTrigGate::setBinInput(FlowConnection *WXUNUSED(connection),oapcBinHeadSp &value,wxUint32 *flowThreadID,flowObject *WXUNUSED(object))
{
   if (!threadIDOK(flowThreadID,false)) return 0;
   bin=value;
   m_binValid=true;
   return 1;
}



oapcBinHeadSp flowBinTrigGate::getBinOutput(FlowConnection *connection,wxInt32 *rcode,wxUint64 WXUNUSED(lastInput))
{
   if ((!m_binValid) || (digi[connection->sourceOutputNum]==0)) *rcode=OAPC_ERROR_NO_DATA_AVAILABLE;
   else *rcode=OAPC_OK;
   return bin;
}



#endif


wxInt32 flowBinTrigGate::loadDATA(wxFile *FHandle,wxUint32 chunkSize,wxUint32 IDOffset,bool isCompiled)
{
   return flowObject::loadDATA(FHandle,chunkSize,IDOffset,isCompiled);
}


