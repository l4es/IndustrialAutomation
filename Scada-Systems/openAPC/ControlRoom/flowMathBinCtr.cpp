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
#include "flowMathBinCtr.h"
#include "oapc_libio.h"


flowMathBinCtr::flowMathBinCtr():flowLogic()
{
   this->data.type=flowObject::FLOW_TYPE_MATH_BINCTR;
   this->data.stdIN= OAPC_DIGI_IO0|OAPC_BIN_IO1;
   this->data.stdOUT=             OAPC_NUM_IO1;
#ifdef ENV_PLAYER
   cnt=0;
#endif
}



flowMathBinCtr::~flowMathBinCtr()
{
}



wxString flowMathBinCtr::getDefaultName()
{
   return _T("Binary Counter");
}



#ifdef ENV_EDITOR
wxInt32 flowMathBinCtr::saveDATA(wxFile *FHandle,char chunkName[4],bool isCompiled)
{
   return flowObject::saveDATA(FHandle,chunkName,isCompiled);
}
#else



wxUint64 flowMathBinCtr::getAssignedOutput(wxUint64 input)
{
   if (input==OAPC_BIN_IO1) return OAPC_NUM_IO1;
   return 0;
}




wxByte flowMathBinCtr::setDigitalInput(FlowConnection *connection,wxByte value,wxUint32 *WXUNUSED(flowThreadID),flowObject *WXUNUSED(object))
{
#ifdef ENV_DEBUGGER
   if (connection->targetInputNum>0)
   {
      wxASSERT(0);
      g_debugWin->setDebugInformation(this,DEBUG_STOP_COND_ILLEGAL_IO,_T(""));
      return 0;
   }
#else
   connection=connection;
#endif
   if ((connection->targetInputNum==0) && (value==1)) cnt=0;
   return 1;
}



wxByte flowMathBinCtr::setBinInput(FlowConnection *connection,oapcBinHeadSp &value,wxUint32 *flowThreadID,flowObject *WXUNUSED(object))
{
   if (!threadIDOK(flowThreadID,false)) return 0;
#ifdef ENV_DEBUGGER
   if (connection->targetInputNum>1)
   {
      wxASSERT(0);
      g_debugWin->setDebugInformation(this,DEBUG_STOP_COND_ILLEGAL_IO,_T(""));
      return 0;
   }
#else
   connection=connection;
#endif
   if (value) cnt++;
   return 1;
}



wxFloat64 flowMathBinCtr::getNumOutput(FlowConnection *connection,wxInt32 *rcode,wxUint64 WXUNUSED(lastInput),wxLongLong WXUNUSED(origCreationTime))
{
#ifdef ENV_DEBUGGER
   if ((connection->sourceOutputNum<1) || (connection->sourceOutputNum>3))
   {
      wxASSERT(0);
      g_debugWin->setDebugInformation(this,DEBUG_STOP_COND_ILLEGAL_IO,_T(""));
      return 0;
   }
#else
   connection=connection;
#endif
  *rcode=OAPC_OK;
   return cnt;
}



#endif


wxInt32 flowMathBinCtr::loadDATA(wxFile *FHandle,wxUint32 chunkSize,wxUint32 IDOffset,bool isCompiled)
{
   return flowObject::loadDATA(FHandle,chunkSize,IDOffset,isCompiled);
}


