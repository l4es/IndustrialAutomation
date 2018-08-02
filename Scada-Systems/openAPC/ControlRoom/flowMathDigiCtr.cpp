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
#include "flowMathDigiCtr.h"
#include "oapc_libio.h"


flowMathDigiCtr::flowMathDigiCtr():flowLogic()
{
   this->data.type=flowObject::FLOW_TYPE_MATH_DIGICTR;
   this->data.stdIN= OAPC_DIGI_IO0|OAPC_DIGI_IO1|OAPC_DIGI_IO2|OAPC_DIGI_IO3;
   this->data.stdOUT=             OAPC_NUM_IO1| OAPC_NUM_IO2| OAPC_NUM_IO3;
#ifdef ENV_PLAYER
   wxInt32 i;

   for (i=0; i<3; i++) cnt[i]=0;
#endif
}



flowMathDigiCtr::~flowMathDigiCtr()
{
}



wxString flowMathDigiCtr::getDefaultName()
{
   return _T("Digital Counter");
}



#ifdef ENV_EDITOR
wxInt32 flowMathDigiCtr::saveDATA(wxFile *FHandle,char chunkName[4],bool isCompiled)
{
   return flowObject::saveDATA(FHandle,chunkName,isCompiled);
}
#else



wxUint64 flowMathDigiCtr::getAssignedOutput(wxUint64 input)
{
   if (input==OAPC_DIGI_IO1) return OAPC_NUM_IO1;
   else if (input==OAPC_DIGI_IO2) return OAPC_NUM_IO2;
   else if (input==OAPC_DIGI_IO3) return OAPC_NUM_IO3;
   return 0;
}




wxByte flowMathDigiCtr::setDigitalInput(FlowConnection *connection,wxByte value,wxUint32 *flowThreadID,flowObject *WXUNUSED(object))
{
   wxInt32 i;

#ifdef ENV_DEBUGGER
   if (connection->targetInputNum>3)
   {
      wxASSERT(0);
      g_debugWin->setDebugInformation(this,DEBUG_STOP_COND_ILLEGAL_IO,_T(""));
      return 0;
   }
#else
   connection=connection;
#endif
   if (connection->targetInputNum!=0)
   {
      if (!threadIDOK(flowThreadID,false)) return 0;
   }
   if (connection->targetInputNum==0) for (i=0; i<3; i++) cnt[i]=0;
   else if ((connection->targetInputNum==1) && (value==1)) cnt[0]++;
   else if ((connection->targetInputNum==2) && (value==0)) cnt[1]++;
   else if (connection->targetInputNum==3) cnt[2]++;
   return 1;
}



wxFloat64 flowMathDigiCtr::getNumOutput(FlowConnection *connection,wxInt32 *rcode,wxUint64 WXUNUSED(lastInput),wxLongLong WXUNUSED(origCreationTime))
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
   return cnt[connection->sourceOutputNum-1];
}



#endif


wxInt32 flowMathDigiCtr::loadDATA(wxFile *FHandle,wxUint32 chunkSize,wxUint32 IDOffset,bool isCompiled)
{
   return flowObject::loadDATA(FHandle,chunkSize,IDOffset,isCompiled);
}


