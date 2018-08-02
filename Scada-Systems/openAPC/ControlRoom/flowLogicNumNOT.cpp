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
#include "flowLogicNumNOT.h"
#include "oapc_libio.h"


flowLogicNumNOT::flowLogicNumNOT():flowLogic()
{
   this->data.type=flowObject::FLOW_TYPE_LOGI_NUMNOT;
   this->data.stdIN=OAPC_NUM_IO0|OAPC_NUM_IO1|OAPC_NUM_IO2|OAPC_NUM_IO3|OAPC_NUM_IO4|OAPC_NUM_IO5|OAPC_NUM_IO6|OAPC_NUM_IO7;
   this->data.stdOUT=OAPC_NUM_IO0|OAPC_NUM_IO1|OAPC_NUM_IO2|OAPC_NUM_IO3|OAPC_NUM_IO4|OAPC_NUM_IO5|OAPC_NUM_IO6|OAPC_NUM_IO7;
#ifdef ENV_PLAYER
   wxInt32 i;

   for (i=0; i<MAX_NUM_IOS; i++) num[i]=0;
#endif
}



flowLogicNumNOT::~flowLogicNumNOT()
{
}



wxString flowLogicNumNOT::getDefaultName()
{
   return _T("Numeric NOT");
}



#ifdef ENV_EDITOR
wxInt32 flowLogicNumNOT::saveDATA(wxFile *FHandle,char chunkName[4],bool isCompiled)
{
   return flowObject::saveDATA(FHandle,chunkName,isCompiled);
}
#else



wxUint64 flowLogicNumNOT::getAssignedOutput(wxUint64 input)
{
   if (input & OAPC_NUM_IO_MASK) return input;
   return 0;
}


/**
Sets an numerical input
@param[in] input the flag that specifies the input
@param[in] value the value that has to be set for that input
@param[in] flowThreadID the Id of the thread that wants to set this value
@return 0; this method has to be overriden
*/
wxByte flowLogicNumNOT::setNumInput(FlowConnection *connection,wxFloat64 value,wxUint32 *flowThreadID,flowObject *WXUNUSED(object))
{
   if (!threadIDOK(flowThreadID,false)) return 0;
   num[connection->targetInputNum]=value;
   return 1;
}



wxFloat64 flowLogicNumNOT::getNumOutput(FlowConnection *connection,wxInt32 *rcode,wxUint64 WXUNUSED(lastInput),wxLongLong WXUNUSED(origCreationTime))
{
   wxUint32 retVal;

#ifdef ENV_DEBUGGER
   if ((connection->sourceOutputNum<0) || (connection->sourceOutputNum>=MAX_NUM_IOS))
   {
      g_debugWin->setDebugInformation(this,DEBUG_STOP_COND_ILLEGAL_IO,_T(""));
      *rcode=OAPC_ERROR_NO_SUCH_IO;
      return 0;
   }
#endif
   *rcode=OAPC_OK;
   retVal=(wxInt32)OAPC_ROUND(num[connection->sourceOutputNum],0);
   retVal^=0xFFFFFFFF;
   return (wxFloat32)retVal;
}

#endif


wxInt32 flowLogicNumNOT::loadDATA(wxFile *FHandle,wxUint32 chunkSize,wxUint32 IDOffset,bool isCompiled)
{
   return flowObject::loadDATA(FHandle,chunkSize,IDOffset,isCompiled);
}


