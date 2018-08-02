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
#include "flowLogic.h"
#include "flowLogicNumShift.h"
#include "oapc_libio.h"


flowLogicNumShift::flowLogicNumShift():flowLogic()
{
#ifndef ENV_EDITOR
   wxUint32 i;
#endif

   this->data.type=flowObject::FLOW_TYPE_LOGI_NUMSHIFT;
   this->data.stdIN= OAPC_DIGI_IO0|OAPC_DIGI_IO1|OAPC_NUM_IO2;
   this->data.stdOUT=OAPC_NUM_IO0|OAPC_NUM_IO1|OAPC_NUM_IO2|OAPC_NUM_IO3|OAPC_NUM_IO4|OAPC_NUM_IO5|OAPC_NUM_IO6|OAPC_NUM_IO7|FLOW_TYPE_FLAG_NUM_OVERFLOW_OUT;
#ifndef ENV_EDITOR
   for (i=0; i<MAX_NUM_IOS; i++) num[i]=0;
#endif

}



flowLogicNumShift::~flowLogicNumShift()
{
}



wxString flowLogicNumShift::getDefaultName()
{
   return _T("Numeric Shift Register");
}



#ifdef ENV_PLAYER
wxUint64 flowLogicNumShift::getAssignedOutput(wxUint64 input)
{
   if (input & OAPC_DIGI_IO0) return OAPC_NUM_IO_MASK;
   return 0;
}



wxByte flowLogicNumShift::setDigitalInput(FlowConnection *connection,wxByte value,wxUint32 *flowThreadID,flowObject *WXUNUSED(object))
{
   wxInt32 i;

   if (!threadIDOK(flowThreadID,false)) return 0;
#ifdef ENV_DEBUGGER
   if ((connection->targetInputNum!=0) && (connection->targetInputNum!=1))
   {
      wxASSERT(0);
      g_debugWin->setDebugInformation(this,DEBUG_STOP_COND_ILLEGAL_IO,_T(""));
      return 0;
   }
#else
   connection=connection;
#endif

   if (value==0) return 1;
   if (connection->targetInputNum==1) for (i=0; i<MAX_NUM_IOS; i++) num[i]=0.0;
   return 1;
}


/**
Sets an numerical input
@param[in] input the flag that specifies the input
@param[in] value the value that has to be set for that input
@param[in] flowThreadID the Id of the thread that wants to set this value
@return 0; this method has to be overriden
*/
wxByte flowLogicNumShift::setNumInput(FlowConnection *connection,wxFloat64 value,wxUint32 *flowThreadID,flowObject *WXUNUSED(object))
{
   wxFloat64 overflow;
   wxInt32   i;

   if (!threadIDOK(flowThreadID,false)) return 0;
#ifdef ENV_DEBUGGER
   if (connection->targetInputNum!=2)
   {
      wxASSERT(0);
      g_debugWin->setDebugInformation(this,DEBUG_STOP_COND_ILLEGAL_IO,_T(""));
      return 0;
   }
#else
   connection=connection;
#endif
   overflow=num[MAX_NUM_IOS-1];
   g_flowPool.startOverflowFlows(this,FLOW_TYPE_FLAG_NUM_OVERFLOW_OUT,0,overflow,_T(""),oapcBinHeadSp(),wxGetLocalTimeMillis()); // TODO: fetch creation time from original thread

   for (i=MAX_NUM_IOS-1; i>0; i--) num[i]=num[i-1];
   num[0]=value;
   return 1;
}



wxFloat64 flowLogicNumShift::getNumOutput(FlowConnection *connection,wxInt32 *rcode,wxUint64 WXUNUSED(lastInput),wxLongLong WXUNUSED(origCreationTime))
{
   *rcode=OAPC_OK;
   return num[connection->sourceOutputNum];
}
#endif






