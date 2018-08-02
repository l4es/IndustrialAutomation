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
#include "flowCharTrigGate.h"
#include "flowCharGate.h"
#include "oapc_libio.h"


flowCharGate::flowCharGate():flowCharTrigGate()
{
#ifdef ENV_PLAYER
   wxInt32 i;

   for (i=0; i<MAX_NUM_IOS; i++)
   {
      txt[i]=_T("");
      m_txtValid[i]=false;
   }
#endif
   this->data.type=flowObject::FLOW_TYPE_FLOW_CHARGATE;
   this->data.stdIN=OAPC_DIGI_IO0|OAPC_CHAR_IO1|OAPC_CHAR_IO2|OAPC_CHAR_IO3|OAPC_CHAR_IO4|OAPC_CHAR_IO5|OAPC_CHAR_IO6|OAPC_CHAR_IO7;
   this->data.stdOUT=            OAPC_CHAR_IO1|OAPC_CHAR_IO2|OAPC_CHAR_IO3|OAPC_CHAR_IO4|OAPC_CHAR_IO5|OAPC_CHAR_IO6|OAPC_CHAR_IO7;
}



flowCharGate::~flowCharGate()
{
}



wxString flowCharGate::getDefaultName()
{
   return _T("Character Gate");
}



#ifndef ENV_EDITOR
wxUint64 flowCharGate::getAssignedOutput(wxUint64 input)
{
   wxByte inputNum;

   if (input==OAPC_DIGI_IO0) return 0;
   inputNum=FlowConnection::getCharIndexFromFlag(input);
   if ((m_txtValid[inputNum]) && (digi[0]==1)) return input;
   return 0;
}



wxByte flowCharGate::setCharInput(FlowConnection *connection,wxString value,wxUint32 *flowThreadID,flowObject *WXUNUSED(object))
{
   if (!threadIDOK(flowThreadID,false)) return 0;
#ifdef ENV_DEBUGGER
   if (connection->targetInputNum==0)
   {
      g_debugWin->setDebugInformation(this,DEBUG_STOP_COND_ILLEGAL_IO,_T(""));
      return 0;
   }
#endif
   txt[connection->targetInputNum]=value;
   m_txtValid[connection->targetInputNum]=true;
   return 1;
}



wxString flowCharGate::getCharOutput(FlowConnection *connection,wxInt32 *rcode,wxUint64 WXUNUSED(lastInput))
{
#ifdef ENV_DEBUGGER
   if (connection->sourceOutputNum==0)
   {
      wxASSERT(0);
      g_debugWin->setDebugInformation(this,DEBUG_STOP_COND_ILLEGAL_IO,_T(""));
      *rcode=OAPC_ERROR_NO_SUCH_IO;
      return _T("");
   }
#endif
   if ((!m_txtValid[connection->sourceOutputNum]) || (digi[0]==0)) *rcode=OAPC_ERROR_NO_DATA_AVAILABLE;
   else *rcode=OAPC_OK;
   return txt[connection->sourceOutputNum];
}
#endif


