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
#include "flowNumTrigGate.h"
#include "oapc_libio.h"


flowNumTrigGate::flowNumTrigGate():flowLogic()
{
   this->data.type=flowObject::FLOW_TYPE_FLOW_NUMTGATE;
   this->data.stdIN=OAPC_NUM_IO0|OAPC_DIGI_IO1|OAPC_DIGI_IO2|OAPC_DIGI_IO3|OAPC_DIGI_IO4|OAPC_DIGI_IO5|OAPC_DIGI_IO6|OAPC_DIGI_IO7;
   this->data.stdOUT=           OAPC_NUM_IO1| OAPC_NUM_IO2| OAPC_NUM_IO3| OAPC_NUM_IO4| OAPC_NUM_IO5| OAPC_NUM_IO6| OAPC_NUM_IO7;
#ifdef ENV_PLAYER
   num=0.0;
   m_numValid=false;
#endif
}



flowNumTrigGate::~flowNumTrigGate()
{
}



wxString flowNumTrigGate::getDefaultName()
{
   return _T("Numeric Triggered Gate");
}



#ifdef ENV_EDITOR
wxInt32 flowNumTrigGate::saveDATA(wxFile *FHandle,char chunkName[4],bool isCompiled)
{
   return flowObject::saveDATA(FHandle,chunkName,isCompiled);
}
#else



wxUint64 flowNumTrigGate::getAssignedOutput(wxUint64 input)
{
   if ((m_numValid) && (input!=OAPC_NUM_IO0))
   {
      if (input==OAPC_DIGI_IO1) return OAPC_NUM_IO1;
      if (input==OAPC_DIGI_IO2) return OAPC_NUM_IO2;
      if (input==OAPC_DIGI_IO3) return OAPC_NUM_IO3;
      if (input==OAPC_DIGI_IO4) return OAPC_NUM_IO4;
      if (input==OAPC_DIGI_IO5) return OAPC_NUM_IO5;
      if (input==OAPC_DIGI_IO6) return OAPC_NUM_IO6;
      if (input==OAPC_DIGI_IO7) return OAPC_NUM_IO7;
   }
   return 0;
}


/**
Sets an numerical input
@param[in] input the flag that specifies the input
@param[in] value the value that has to be set for that input
@param[in] flowThreadID the Id of the thread that wants to set this value
@return 0; this method has to be overriden
*/
wxByte flowNumTrigGate::setNumInput(FlowConnection *connection,wxFloat64 value,wxUint32 *flowThreadID,flowObject *WXUNUSED(object))
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
   num=value;
   m_numValid=true;
   return 1;
}



wxFloat64 flowNumTrigGate::getNumOutput(FlowConnection *connection,wxInt32 *rcode,wxUint64 WXUNUSED(lastInput),wxLongLong WXUNUSED(origCreationTime))
{
#ifdef ENV_DEBUGGER
   if (connection->sourceOutputNum==0)
   {
      g_debugWin->setDebugInformation(this,DEBUG_STOP_COND_ILLEGAL_IO,_T(""));
      *rcode=OAPC_ERROR_NO_SUCH_IO;
      return 0;
   }
#endif
   if ((!m_numValid) || (digi[connection->sourceOutputNum]==0)) *rcode=OAPC_ERROR_NO_DATA_AVAILABLE;
   else *rcode=OAPC_OK;
   return num;
}

#endif


wxInt32 flowNumTrigGate::loadDATA(wxFile *FHandle,wxUint32 chunkSize,wxUint32 IDOffset,bool isCompiled)
{
   return flowObject::loadDATA(FHandle,chunkSize,IDOffset,isCompiled);
}


