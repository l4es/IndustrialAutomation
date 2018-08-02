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
#include "flowConverterDigi2Pair.h"
#include "oapc_libio.h"

#ifdef ENV_WINDOWS
 #ifdef ENV_WINDOWSCE
  #include "Winsock2.h"
 #endif
#endif



flowConverterDigi2Pair::flowConverterDigi2Pair():flowConverter2Pair()
{
   this->data.type=flowObject::FLOW_TYPE_CONV_DIGI2PAIR;
   this->data.stdIN=OAPC_DIGI_IO0|OAPC_DIGI_IO1|OAPC_DIGI_IO2|OAPC_DIGI_IO3|OAPC_DIGI_IO4|OAPC_DIGI_IO5|OAPC_DIGI_IO6|OAPC_DIGI_IO7;
   this->data.stdOUT=OAPC_CHAR_IO0|OAPC_DIGI_IO1;
#ifdef ENV_PLAYER
#endif
#ifdef ENV_EDITOR

#endif
}



flowConverterDigi2Pair::~flowConverterDigi2Pair()
{
}



wxString flowConverterDigi2Pair::getDefaultName()
{
   return _T("Convert Digi2Pair");
}



#ifndef ENV_PLAYER



#else



wxUint64 flowConverterDigi2Pair::getAssignedOutput(wxUint64 WXUNUSED(input))
{
   return OAPC_CHAR_IO0|OAPC_DIGI_IO1;
}



/**
Sets an digital input and checks if the same thread has set that input before
@param[in] input the flag that specifies the input
@param[in] value the value that has to be set for that input (0 or 1)
@param[in] flowThreadID the Id of the thread that wants to set this value
@return 0 if the same thread already has set a value; the thread has to finish
        in this case to avoid endless loops; 1 if the input value was set correctly
*/
wxByte flowConverterDigi2Pair::setDigitalInput(FlowConnection *connection,wxByte value,wxUint32 *flowThreadID,flowObject *object)
{
   m_lastUsedInput=connection->targetInputNum;
   return flowConverter2Pair::setDigitalInput(connection,value,flowThreadID,object);
}



wxByte flowConverterDigi2Pair::getDigitalOutput(FlowConnection *connection,wxInt32 *rcode,wxUint64 WXUNUSED(lastInput),wxLongLong WXUNUSED(origCreationTime))
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
   return digi[m_lastUsedInput];
}



#endif



