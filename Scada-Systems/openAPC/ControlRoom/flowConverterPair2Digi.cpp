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
#include "flowConverterPair2Digi.h"
#include "oapc_libio.h"

#ifdef ENV_WINDOWS
#ifdef ENV_WINDOWSCE
#include "Winsock2.h"
#endif
#endif



flowConverterPair2Digi::flowConverterPair2Digi():flowConverterPair2()
{
   this->data.type=flowObject::FLOW_TYPE_CONV_PAIR2DIGI;
   this->data.stdIN=OAPC_CHAR_IO0|OAPC_DIGI_IO1;
   this->data.stdOUT=OAPC_DIGI_IO0|OAPC_DIGI_IO1|OAPC_DIGI_IO2|OAPC_DIGI_IO3|OAPC_DIGI_IO4|OAPC_DIGI_IO5|OAPC_DIGI_IO6|OAPC_DIGI_IO7;
#ifdef ENV_PLAYER
#endif
#ifdef ENV_EDITOR

#endif
}



flowConverterPair2Digi::~flowConverterPair2Digi()
{
}



wxString flowConverterPair2Digi::getDefaultName()
{
   return _T("Convert Pair2Digi");
}



#ifndef ENV_PLAYER



#else



wxUint64 flowConverterPair2Digi::getAssignedOutput(wxUint64 input)
{
   wxInt32 i,flag;

   if (input!=OAPC_CHAR_IO0) return 0;
   flag=OAPC_DIGI_IO0;
   for (i=0; i<MAX_NUM_IOS; i++)
   {
      if (cmdName[i].CompareTo(cmd)==0) return flag;
      flag=flag<<1;
   }
   return 0;
}



wxByte flowConverterPair2Digi::setDigitalInput(FlowConnection *WXUNUSED(connection),wxByte value,wxUint32 *flowThreadID,flowObject *WXUNUSED(object))
{
   if (!threadIDOK(flowThreadID,false)) return 0;
   m_digiQ.push(value);
   return 1;
}



wxByte flowConverterPair2Digi::getDigitalOutput(FlowConnection *WXUNUSED(connection),wxInt32 *rcode,wxUint64 WXUNUSED(lastInput),wxLongLong WXUNUSED(origCreationTime))
{
   wxByte val;

   if (m_digiQ.empty())
   {
      *rcode=OAPC_ERROR_NO_DATA_AVAILABLE;
      return 0.0;
   }
   *rcode=OAPC_OK;
   val=m_digiQ.front();
   m_digiQ.pop();
   return val;
}



#endif



