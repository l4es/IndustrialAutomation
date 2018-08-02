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
#include "flowConverter2Pair.h"
#include "flowConverterBin2Pair.h"
#include "oapc_libio.h"

#ifdef ENV_WINDOWS
 #ifdef ENV_WINDOWSCE
  #include "Winsock2.h"
 #endif
#endif



flowConverterBin2Pair::flowConverterBin2Pair():flowConverter2Pair()
{
#ifdef ENV_PLAYER
   wxInt32 i;
#endif

   this->data.type=flowObject::FLOW_TYPE_CONV_BIN2PAIR;
   this->data.stdIN=OAPC_BIN_IO0|OAPC_BIN_IO1|OAPC_BIN_IO2|OAPC_BIN_IO3|OAPC_BIN_IO4|OAPC_BIN_IO5|OAPC_BIN_IO6|OAPC_BIN_IO7;
   this->data.stdOUT=OAPC_CHAR_IO0|OAPC_BIN_IO1;
#ifdef ENV_PLAYER
   for (i=0; i<MAX_NUM_IOS; i++)
    bin[i]=oapcBinHeadSp();
   m_lastUsedInput=-1;
#endif
}



flowConverterBin2Pair::~flowConverterBin2Pair()
{
}



wxString flowConverterBin2Pair::getDefaultName()
{
   return _T("Convert Bin2Pair");
}



#ifndef ENV_PLAYER



#else



wxUint64 flowConverterBin2Pair::getAssignedOutput(wxUint64 WXUNUSED(input))
{
   return OAPC_CHAR_IO0|OAPC_BIN_IO1;
}



wxByte flowConverterBin2Pair::setBinInput(FlowConnection *connection,oapcBinHeadSp &value,wxUint32 *flowThreadID,flowObject *WXUNUSED(object))
{
   if (!threadIDOK(flowThreadID,false)) return 0;
   m_lastUsedInput=connection->targetInputNum;
   bin[connection->targetInputNum]=value;
   return 1;
}



oapcBinHeadSp flowConverterBin2Pair::getBinOutput(FlowConnection *connection,wxInt32 *rcode,wxUint64 WXUNUSED(lastInput))
{
   if (connection->sourceOutputNum!=1)
   {
      wxASSERT(0);
#ifdef ENV_DEBUGGER
      g_debugWin->setDebugInformation(this,DEBUG_STOP_COND_ILLEGAL_IO,_T(""));
#endif
      *rcode=OAPC_ERROR_NO_SUCH_IO;
      return oapcBinHeadSp();
   }
   *rcode=OAPC_OK;
   return bin[m_lastUsedInput];
}


#endif



