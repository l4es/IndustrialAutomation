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
#include "flowConverter.h"
#include "flowConverterPair2.h"
#include "flowConverterPair2Num.h"
#include "oapc_libio.h"

#ifdef ENV_WINDOWS
#ifdef ENV_WINDOWSCE
#include "Winsock2.h"
#endif
#endif



flowConverterPair2Num::flowConverterPair2Num():flowConverterPair2()
{
   this->data.type=flowObject::FLOW_TYPE_CONV_PAIR2NUM;
   this->data.stdIN=OAPC_CHAR_IO0|OAPC_NUM_IO1;
   this->data.stdOUT=OAPC_NUM_IO0|OAPC_NUM_IO1|OAPC_NUM_IO2|OAPC_NUM_IO3|OAPC_NUM_IO4|OAPC_NUM_IO5|OAPC_NUM_IO6|OAPC_NUM_IO7;
#ifdef ENV_PLAYER
#endif
#ifdef ENV_EDITOR

#endif
}



flowConverterPair2Num::~flowConverterPair2Num()
{
}



wxString flowConverterPair2Num::getDefaultName()
{
   return _T("Convert Pair2Num");
}



#ifndef ENV_PLAYER



#else



wxUint64 flowConverterPair2Num::getAssignedOutput(wxUint64 input)
{
   wxInt32 i,flag;

   if (input!=OAPC_CHAR_IO0) return 0;
   flag=OAPC_NUM_IO0;
   for (i=0; i<MAX_NUM_IOS; i++)
   {
      if (cmdName[i].CompareTo(cmd)==0) return flag;
      flag=flag<<1;
   }
   return 0;
}



wxByte flowConverterPair2Num::setNumInput(FlowConnection *WXUNUSED(connection),wxFloat64 value,wxUint32 *WXUNUSED(flowThreadID),flowObject *WXUNUSED(object))
{
   m_numQ.push(value);
   return 1;
}


wxFloat64 flowConverterPair2Num::getNumOutput(FlowConnection *WXUNUSED(connection),wxInt32 *rcode,wxUint64 WXUNUSED(lastInput),wxLongLong WXUNUSED(origCreationTime))
{
   wxFloat64 val;

   if (m_numQ.empty())
   {
      *rcode=OAPC_ERROR_NO_DATA_AVAILABLE;
      return 0.0;
   }
   *rcode=OAPC_OK;
   val=m_numQ.front();
   m_numQ.pop();
   return val;
}


#endif



