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
#include "flowBinGate.h"
#include "oapc_libio.h"


flowBinGate::flowBinGate():flowBinTrigGate()
{
#ifdef ENV_PLAYER
   wxInt32 i;

   for (i=0; i<MAX_NUM_IOS; i++)
   {
      bin[i]=oapcBinHeadSp();
      m_binValid[i]=false;
   }
#endif
   this->data.type=flowObject::FLOW_TYPE_FLOW_BINGATE;
   this->data.stdIN=OAPC_DIGI_IO0|OAPC_BIN_IO1| OAPC_BIN_IO2| OAPC_BIN_IO3| OAPC_BIN_IO4| OAPC_BIN_IO5| OAPC_BIN_IO6| OAPC_BIN_IO7;
   this->data.stdOUT=            OAPC_BIN_IO1| OAPC_BIN_IO2| OAPC_BIN_IO3| OAPC_BIN_IO4| OAPC_BIN_IO5| OAPC_BIN_IO6| OAPC_BIN_IO7;
}



flowBinGate::~flowBinGate()
{
}



wxString flowBinGate::getDefaultName()
{
   return _T("Binary Gate");
}



#ifndef ENV_EDITOR
wxUint64 flowBinGate::getAssignedOutput(wxUint64 input)
{
   wxByte inputNum;

   if (input==OAPC_DIGI_IO0) return 0;
   inputNum=FlowConnection::getBinIndexFromFlag(input);
   if ((m_binValid[inputNum]) && (digi[0]==1)) return input;
   return 0;
}



wxByte flowBinGate::setBinInput(FlowConnection *connection,oapcBinHeadSp &value,wxUint32 *flowThreadID,flowObject *WXUNUSED(object))
{
   if (!threadIDOK(flowThreadID,false)) return 0;
   bin[connection->targetInputNum]=value;
   m_binValid[connection->targetInputNum]=true;
   return 1;
}



oapcBinHeadSp flowBinGate::getBinOutput(FlowConnection *connection,wxInt32 *rcode,wxUint64 WXUNUSED(lastInput))
{
   if ((!m_binValid[connection->sourceOutputNum]) || (digi[0]==0)) *rcode=OAPC_ERROR_NO_DATA_AVAILABLE;
   else *rcode=OAPC_OK;
   return bin[connection->sourceOutputNum];
}
#endif

