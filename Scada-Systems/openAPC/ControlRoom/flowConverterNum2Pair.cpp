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
#include "flowConverter2Pair.h"
#include "flowConverterNum2Pair.h"
#include "oapc_libio.h"

#ifdef ENV_WINDOWS
 #ifdef ENV_WINDOWSCE
  #include "Winsock2.h"
 #endif
#endif



flowConverterNum2Pair::flowConverterNum2Pair():flowConverter2Pair()
{
   this->data.type=flowObject::FLOW_TYPE_CONV_NUM2PAIR;
   this->data.stdIN=OAPC_NUM_IO0|OAPC_NUM_IO1|OAPC_NUM_IO2|OAPC_NUM_IO3|OAPC_NUM_IO4|OAPC_NUM_IO5|OAPC_NUM_IO6|OAPC_NUM_IO7;
   this->data.stdOUT=OAPC_CHAR_IO0|OAPC_NUM_IO1;
#ifdef ENV_PLAYER
#endif
#ifdef ENV_EDITOR

#endif
}



flowConverterNum2Pair::~flowConverterNum2Pair()
{
}



wxString flowConverterNum2Pair::getDefaultName()
{
   return _T("Convert Num2Pair");
}



#ifndef ENV_PLAYER



#else



wxUint64 flowConverterNum2Pair::getAssignedOutput(wxUint64 WXUNUSED(input))
{
   return OAPC_CHAR_IO0|OAPC_NUM_IO1;
}



wxByte flowConverterNum2Pair::setNumInput(FlowConnection *connection,wxFloat64 value,wxUint32 *flowThreadID,flowObject *WXUNUSED(object))
{
   if (!threadIDOK(flowThreadID,false)) return 0;
   m_lastUsedInput=connection->targetInputNum;
   num[connection->targetInputNum]=value;
   return 1;
}



wxFloat64 flowConverterNum2Pair::getNumOutput(FlowConnection *connection,wxInt32 *rcode,wxUint64 WXUNUSED(lastInput),wxLongLong WXUNUSED(origCreationTime))
{
#ifdef ENV_DEBUGGER
   if (connection->sourceOutputNum!=1)
   {
      wxASSERT(0);
      g_debugWin->setDebugInformation(this,DEBUG_STOP_COND_ILLEGAL_IO,_T(""));
      *rcode=OAPC_ERROR_NO_SUCH_IO;
      return 0;
   }
#else
   connection=connection;
#endif
   *rcode=OAPC_OK;
   return num[m_lastUsedInput];
}



#endif



