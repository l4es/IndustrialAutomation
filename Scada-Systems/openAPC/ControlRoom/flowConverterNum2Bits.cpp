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
#include "flowConverterNum2Bits.h"
#ifdef ENV_PLAYER
 #include "flowWalkerThread.h"
#endif
#include "oapc_libio.h"


flowConverterNum2Bits::flowConverterNum2Bits():flowConverter()
{
   this->data.type=flowObject::FLOW_TYPE_CONV_NUM2BITS;
   this->data.stdIN=OAPC_NUM_IO0;
   this->data.stdOUT=OAPC_DIGI_IO0|OAPC_DIGI_IO1|OAPC_DIGI_IO2|OAPC_DIGI_IO3|OAPC_DIGI_IO4|OAPC_DIGI_IO5|OAPC_DIGI_IO6|OAPC_DIGI_IO7|FLOW_TYPE_FLAG_NUM_OVERFLOW_OUT;
}



flowConverterNum2Bits::~flowConverterNum2Bits()
{
}



#ifdef ENV_EDITOR
wxString flowConverterNum2Bits::getDefaultName()
{
   return _T("Convert Num2Bits");
}

#else

wxUint64 flowConverterNum2Bits::getAssignedOutput(wxUint64 input)
{
   if (input & OAPC_NUM_IO0) return OAPC_DIGI_IO_MASK|FLOW_TYPE_FLAG_NUM_OVERFLOW_OUT;
   return 0;
}



/**
Sets an numerical input and checks if the same thread has set that input before
@param[in] input the flag that specifies the input
@param[in] value the value that has to be set for that input
@param[in] flowThreadID the Id of the thread that wants to set this value
@return 0 if the same thread already has set a value; the thread has to finish
        in this case to avoid endless loops; 1 if the input value was set correctly
*/
wxByte flowConverterNum2Bits::setNumInput(FlowConnection *connection,wxFloat64 value,wxUint32 *flowThreadID,flowObject *WXUNUSED(object))
{
   if (!threadIDOK(flowThreadID,false)) return 0;
   num[connection->targetInputNum]=value;

   return 1;
}



wxByte flowConverterNum2Bits::getDigitalOutput(FlowConnection *connection,wxInt32 *rcode,wxUint64 WXUNUSED(lastInput),wxLongLong WXUNUSED(origCreationTime))
{
   wxUint64    retVal;

#ifdef ENV_DEBUGGER
   if ((connection->sourceOutputNum<0) || (connection->sourceOutputNum>=MAX_NUM_IOS))
   {
      g_debugWin->setDebugInformation(this,DEBUG_STOP_COND_ILLEGAL_IO,_T(""));
      *rcode=OAPC_ERROR_NO_SUCH_IO;
      return 0;
   }
#endif
   // mode CONVERTER_Num2Bits_FLAGS_DIRECT_MODE
   wxUint32 bit=0;

   if (connection->data.sourceOutput & OAPC_DIGI_IO0) bit=1;
   else if (connection->data.sourceOutput & OAPC_DIGI_IO1) bit=2;
   else if (connection->data.sourceOutput & OAPC_DIGI_IO2) bit=4;
   else if (connection->data.sourceOutput & OAPC_DIGI_IO3) bit=8;
   else if (connection->data.sourceOutput & OAPC_DIGI_IO4) bit=16;
   else if (connection->data.sourceOutput & OAPC_DIGI_IO5) bit=32;
   else if (connection->data.sourceOutput & OAPC_DIGI_IO6) bit=64;
   else if (connection->data.sourceOutput & OAPC_DIGI_IO7) bit=128;

   if (((wxUint32)OAPC_ROUND(num[0],0)) & bit) retVal=1;
   else retVal=0;
   *rcode=OAPC_OK;
   return retVal;
}



wxFloat64 flowConverterNum2Bits::getNumOutput(FlowConnection *connection,wxInt32 *rcode,wxUint64 WXUNUSED(lastInput),wxLongLong WXUNUSED(origCreationTime))
{
#ifdef ENV_DEBUGGER
   if (connection->sourceOutputNum!=MAX_NUM_IOS)
   {
      g_debugWin->setDebugInformation(this,DEBUG_STOP_COND_ILLEGAL_IO,_T(""));
      *rcode=OAPC_ERROR_NO_SUCH_IO;
      return 0;
   }
#else
   connection=connection;
#endif
   *rcode=OAPC_OK;
   return (wxFloat64)(((wxInt64)OAPC_ROUND(num[0],0))>>8);
}

#endif









