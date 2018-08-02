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
#include "flowLogicDigiShift.h"
#include "oapc_libio.h"


flowLogicDigiShift::flowLogicDigiShift():flowLogic()
{
   this->data.type=flowObject::FLOW_TYPE_LOGI_DIGISHIFT;
   this->data.stdIN= OAPC_DIGI_IO0|OAPC_DIGI_IO1|OAPC_DIGI_IO2;
   this->data.stdOUT=OAPC_DIGI_IO0|OAPC_DIGI_IO1|OAPC_DIGI_IO2|OAPC_DIGI_IO3|OAPC_DIGI_IO4|OAPC_DIGI_IO5|OAPC_DIGI_IO6|OAPC_DIGI_IO7|FLOW_TYPE_FLAG_DIGI_OVERFLOW_OUT;
}



flowLogicDigiShift::~flowLogicDigiShift()
{
}



wxString flowLogicDigiShift::getDefaultName()
{
   return _T("Digital Shift Register");
}



#ifdef ENV_PLAYER
wxUint64 flowLogicDigiShift::getAssignedOutput(wxUint64 input)
{
   if (input & OAPC_DIGI_IO0) return OAPC_DIGI_IO_MASK;
   return 0;
}



wxByte flowLogicDigiShift::setDigitalInput(FlowConnection *connection,wxByte value,wxUint32 *flowThreadID,flowObject *WXUNUSED(object))
{
   wxInt32 i;

   if (!threadIDOK(flowThreadID,false)) return 0;
#ifdef ENV_DEBUGGER
   if ((connection->targetInputNum!=0) && (connection->targetInputNum!=1) && (connection->targetInputNum!=2))
   {
      wxASSERT(0);
      g_debugWin->setDebugInformation(this,DEBUG_STOP_COND_ILLEGAL_IO,_T(""));
      return 0;
   }
#else
   connection=connection;
#endif

   if ((connection->targetInputNum==1) && (value==1)) for (i=0; i<MAX_NUM_IOS; i++) digi[i]=0;
   else if (connection->targetInputNum==2)
   {
      wxByte overflow;

      overflow=digi[MAX_NUM_IOS-1];
      g_flowPool.startOverflowFlows(this,FLOW_TYPE_FLAG_DIGI_OVERFLOW_OUT,overflow,0.0,_T(""),oapcBinHeadSp(),wxGetLocalTimeMillis()); // TODO: fetch creation time from original thread

      for (i=MAX_NUM_IOS-1; i>0; i--) digi[i]=digi[i-1];
      digi[0]=value;
   }
   return 1;
}

#endif
