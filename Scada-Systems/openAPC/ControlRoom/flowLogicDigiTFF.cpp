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
#include "flowLogicDigiTFF.h"
#include "oapc_libio.h"


flowLogicDigiTFF::flowLogicDigiTFF():flowLogic()
{
#ifdef ENV_PLAYER
   wxInt32 i;

   for (i=0; i<MAX_NUM_IOS/2; i++) digiChanged[i]=false;
   digi[0]=0;   digi[1]=1;
   digi[2]=0;   digi[3]=1;
   digi[4]=0;   digi[5]=1;
   digi[6]=0;   digi[7]=1;
#endif
   this->data.type=flowObject::FLOW_TYPE_LOGI_DIGITFF;
   this->data.stdIN= OAPC_DIGI_IO0|             OAPC_DIGI_IO2|             OAPC_DIGI_IO4|             OAPC_DIGI_IO6;
   this->data.stdOUT=OAPC_DIGI_IO0|OAPC_DIGI_IO1|OAPC_DIGI_IO2|OAPC_DIGI_IO3|OAPC_DIGI_IO4|OAPC_DIGI_IO5|OAPC_DIGI_IO6|OAPC_DIGI_IO7;
}



flowLogicDigiTFF::~flowLogicDigiTFF()
{
}



wxString flowLogicDigiTFF::getDefaultName()
{
   return _T("Toggle Flip-Flop");
}



#ifdef ENV_EDITOR
wxInt32 flowLogicDigiTFF::saveDATA(wxFile *FHandle,char chunkName[4],bool isCompiled)
{
   return flowObject::saveDATA(FHandle,chunkName,isCompiled);
}


#else



wxUint64 flowLogicDigiTFF::getAssignedOutput(wxUint64 input)
{
   if ((input & OAPC_DIGI_IO0) && (digiChanged[0]))
   {
      digiChanged[0]=false;
      return OAPC_DIGI_IO0|OAPC_DIGI_IO1;
   }
   else if ((input & OAPC_DIGI_IO2) && (digiChanged[1]))
   {
      digiChanged[1]=false;
      return OAPC_DIGI_IO2|OAPC_DIGI_IO3;
   }
   else if ((input & OAPC_DIGI_IO4) && (digiChanged[2]))
   {
      digiChanged[2]=false;
      return OAPC_DIGI_IO4|OAPC_DIGI_IO5;
   }
   else if ((input & OAPC_DIGI_IO6) && (digiChanged[3]))
   {
      digiChanged[3]=false;
      return OAPC_DIGI_IO6|OAPC_DIGI_IO7;
   }
   return 0;
}



wxByte flowLogicDigiTFF::setDigitalInput(FlowConnection *connection,wxByte value,wxUint32 *flowThreadID,flowObject *WXUNUSED(object))
{
   if (!threadIDOK(flowThreadID,false)) return 0;

   if (value==0) return 0;

   digiChanged[connection->targetInputNum/2]=true;
   digi[connection->targetInputNum-1]=1-digi[connection->targetInputNum-1];
   digi[connection->targetInputNum]=1-digi[connection->targetInputNum];
   return 1;
}
#endif


wxInt32 flowLogicDigiTFF::loadDATA(wxFile *FHandle,wxUint32 chunkSize,wxUint32 IDOffset,bool isCompiled)
{
   return flowObject::loadDATA(FHandle,chunkSize,IDOffset,isCompiled);
}


