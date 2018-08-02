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
#include "flowLogicCharNOP.h"
#include "oapc_libio.h"


flowLogicCharNOP::flowLogicCharNOP():flowLogic()
{
   this->data.type=flowObject::FLOW_TYPE_LOGI_CHARNOP;
   this->data.stdIN=OAPC_CHAR_IO0|OAPC_CHAR_IO1|OAPC_CHAR_IO2|OAPC_CHAR_IO3|OAPC_CHAR_IO4|OAPC_CHAR_IO5|OAPC_CHAR_IO6|OAPC_CHAR_IO7;
   this->data.stdOUT=OAPC_CHAR_IO0|OAPC_CHAR_IO1|OAPC_CHAR_IO2|OAPC_CHAR_IO3|OAPC_CHAR_IO4|OAPC_CHAR_IO5|OAPC_CHAR_IO6|OAPC_CHAR_IO7;
}



flowLogicCharNOP::~flowLogicCharNOP()
{
}



wxString flowLogicCharNOP::getDefaultName()
{
   return _T("Character-NOP");
}



#ifdef ENV_EDITOR
wxInt32 flowLogicCharNOP::saveDATA(wxFile *FHandle,char chunkName[4],bool isCompiled)
{
   return flowObject::saveDATA(FHandle,chunkName,isCompiled);
}
#else



wxUint64 flowLogicCharNOP::getAssignedOutput(wxUint64 input)
{
   return input;
}



wxByte flowLogicCharNOP::setCharInput(FlowConnection *connection,wxString value,wxUint32 *flowThreadID,flowObject *WXUNUSED(object))
{
   if (!threadIDOK(flowThreadID,false)) return 0;
#ifdef ENV_DEBUGGER
   if (connection->targetInputNum!=0)
   {
      g_debugWin->setDebugInformation(this,DEBUG_STOP_COND_ILLEGAL_IO,_T(""));
      return 0;
   }
#else
   connection=connection;
#endif
   txt[connection->targetInputNum]=value;
   return 1;
}



wxString flowLogicCharNOP::getCharOutput(FlowConnection *connection,wxInt32 *rcode,wxUint64 WXUNUSED(lastInput))
{
   *rcode=OAPC_OK;
   return txt[connection->sourceOutputNum];
}


#endif


wxInt32 flowLogicCharNOP::loadDATA(wxFile *FHandle,wxUint32 chunkSize,wxUint32 IDOffset,bool isCompiled)
{
   return flowObject::loadDATA(FHandle,chunkSize,IDOffset,isCompiled);
}


