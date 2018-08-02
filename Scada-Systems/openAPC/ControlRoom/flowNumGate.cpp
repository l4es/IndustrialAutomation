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

#ifdef ENV_DEBUGGER
#include "DebugWin.h"
#endif
#include "iff.h"
#include "globals.h"
#include "flowObject.h"
#include "flowNumGate.h"
#include "oapc_libio.h"
#include "flowConnection.h"


flowNumGate::flowNumGate():flowNumTrigGate()
{
#ifdef ENV_PLAYER
   wxInt32 i;

   for (i=0; i<MAX_NUM_IOS; i++)
   {
      num[i]=0.0;
      m_numValid[i]=false;
   }
#endif

   this->data.type=flowObject::FLOW_TYPE_FLOW_NUMGATE;
   this->data.stdIN=OAPC_DIGI_IO0|OAPC_NUM_IO1| OAPC_NUM_IO2| OAPC_NUM_IO3| OAPC_NUM_IO4| OAPC_NUM_IO5| OAPC_NUM_IO6| OAPC_NUM_IO7;
   this->data.stdOUT=            OAPC_NUM_IO1| OAPC_NUM_IO2| OAPC_NUM_IO3| OAPC_NUM_IO4| OAPC_NUM_IO5| OAPC_NUM_IO6| OAPC_NUM_IO7;
}



flowNumGate::~flowNumGate()
{
}



wxString flowNumGate::getDefaultName()
{
   return _T("Numeric Gate");
}



#ifndef ENV_EDITOR
wxUint64 flowNumGate::getAssignedOutput(wxUint64 input)
{
   wxByte inputNum;

   if (input==OAPC_DIGI_IO0) return 0;
   inputNum=FlowConnection::getNumIndexFromFlag(input);
   if ((m_numValid[inputNum]) && (digi[0]==1)) return input;
   return 0;
}



wxByte flowNumGate::setNumInput(FlowConnection *connection,wxFloat64 value,wxUint32 *flowThreadID,flowObject *WXUNUSED(object))
{
   if (!threadIDOK(flowThreadID,false)) return 0;
#ifdef ENV_DEBUGGER
   if (connection->targetInputNum==0)
   {
      g_debugWin->setDebugInformation(this,DEBUG_STOP_COND_ILLEGAL_IO,_T(""));
      return 0;
   }
#endif
   num[connection->targetInputNum]=value;
   m_numValid[connection->targetInputNum]=true;
   return 1;
}



wxFloat64 flowNumGate::getNumOutput(FlowConnection *connection,wxInt32 *rcode,wxUint64 WXUNUSED(lastInput),wxLongLong WXUNUSED(origCreationTime))
{
#ifdef ENV_DEBUGGER
   if (connection->sourceOutputNum==0)
   {
      g_debugWin->setDebugInformation(this,DEBUG_STOP_COND_ILLEGAL_IO,_T(""));
      *rcode=OAPC_ERROR_NO_SUCH_IO;
      return 0;
   }
#endif
   if ((!m_numValid[connection->sourceOutputNum]) || (digi[0]==0)) *rcode=OAPC_ERROR_NO_DATA_AVAILABLE;
   else *rcode=OAPC_OK;
   return num[connection->sourceOutputNum];
}
#endif
