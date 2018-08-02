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
#include "flowLogicBinNOP.h"
#include "oapc_libio.h"


flowLogicBinNOP::flowLogicBinNOP():flowLogic()
{
   this->data.type=flowObject::FLOW_TYPE_LOGI_BINNOP;
   this->data.stdIN=OAPC_NUM_IO0|OAPC_NUM_IO1|OAPC_NUM_IO2|OAPC_NUM_IO3|OAPC_NUM_IO4|OAPC_NUM_IO5|OAPC_NUM_IO6|OAPC_NUM_IO7;
   this->data.stdOUT=OAPC_NUM_IO0|OAPC_NUM_IO1|OAPC_NUM_IO2|OAPC_NUM_IO3|OAPC_NUM_IO4|OAPC_NUM_IO5|OAPC_NUM_IO6|OAPC_NUM_IO7;
#ifdef ENV_PLAYER
   wxInt32 i;

   for (i=0; i<MAX_NUM_IOS; i++) bin[i]=oapcBinHeadSp();
#endif
}



flowLogicBinNOP::~flowLogicBinNOP()
{
}



wxString flowLogicBinNOP::getDefaultName()
{
   return _T("Binary NOP");
}



#ifdef ENV_EDITOR
wxInt32 flowLogicBinNOP::saveDATA(wxFile *FHandle,char chunkName[4],bool isCompiled)
{
   return flowObject::saveDATA(FHandle,chunkName,isCompiled);
}
#else



wxUint64 flowLogicBinNOP::getAssignedOutput(wxUint64 input)
{
   return input;
}



wxByte flowLogicBinNOP::setBinInput(FlowConnection *connection,oapcBinHeadSp &value,wxUint32 *flowThreadID,flowObject *WXUNUSED(object))
{
   if (!threadIDOK(flowThreadID,false)) return 0;
   bin[connection->targetInputNum]=value;
   return 1;
}



oapcBinHeadSp flowLogicBinNOP::getBinOutput(FlowConnection *connection,wxInt32 *rcode,wxUint64 WXUNUSED(lastInput))
{
   *rcode=OAPC_OK|OAPC_INT_BIN_NOT_UNLOCKEDi;
   return bin[connection->sourceOutputNum];
}


#endif


wxInt32 flowLogicBinNOP::loadDATA(wxFile *FHandle,wxUint32 chunkSize,wxUint32 IDOffset,bool isCompiled)
{
   return flowObject::loadDATA(FHandle,chunkSize,IDOffset,isCompiled);
}


