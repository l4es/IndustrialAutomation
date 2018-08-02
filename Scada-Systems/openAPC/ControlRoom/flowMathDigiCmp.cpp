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
#include "flowMathDigiCmp.h"
#include "oapc_libio.h"


flowMathDigiCmp::flowMathDigiCmp():flowLogic()
{
   this->data.type=flowObject::FLOW_TYPE_MATH_DIGICMP;
   this->data.stdIN= OAPC_DIGI_IO0|OAPC_DIGI_IO1|             OAPC_DIGI_IO3| OAPC_DIGI_IO4;
   this->data.stdOUT=OAPC_DIGI_IO0|OAPC_DIGI_IO1|OAPC_DIGI_IO2|OAPC_DIGI_IO3|OAPC_DIGI_IO4|OAPC_DIGI_IO5;
}



flowMathDigiCmp::~flowMathDigiCmp()
{
}



wxString flowMathDigiCmp::getDefaultName()
{
   return _T("Compare Digital");
}



#ifdef ENV_EDITOR
wxInt32 flowMathDigiCmp::saveDATA(wxFile *FHandle,char chunkName[4],bool isCompiled)
{
   return flowObject::saveDATA(FHandle,chunkName,isCompiled);
}
#else



wxUint64 flowMathDigiCmp::getAssignedOutput(wxUint64 input)
{
	wxUint32 rFlags=0;
	
   if ((input==OAPC_DIGI_IO0) || (input==OAPC_DIGI_IO1))
   {
      if (digi[0]<digi[1]) rFlags=OAPC_DIGI_IO0;
      if (digi[0]==digi[1]) rFlags|=OAPC_DIGI_IO1;
      if (digi[0]>digi[1]) rFlags|=OAPC_DIGI_IO2;
   }
   if ((input==OAPC_DIGI_IO3) || (input==OAPC_DIGI_IO4))
   {
      if (digi[3]<digi[4]) rFlags|=OAPC_DIGI_IO3;
      if (digi[3]==digi[4]) rFlags|=OAPC_DIGI_IO4;
      if (digi[3]>digi[4]) rFlags|=OAPC_DIGI_IO5;
   }
   return rFlags;
}



wxByte flowMathDigiCmp::getDigitalOutput(FlowConnection *connection,wxInt32 *rcode,wxUint64 WXUNUSED(lastInput),wxLongLong WXUNUSED(origCreationTime))
{
	if (connection->sourceOutputNum<=5)
	{
      *rcode=OAPC_OK;
      return 1;
	}
	else
	{
#ifdef ENV_DEBUGGER
      wxASSERT(0);
      g_debugWin->setDebugInformation(this,DEBUG_STOP_COND_ILLEGAL_IO,_T(""));
      *rcode=OAPC_ERROR_NO_SUCH_IO;
#endif
      return 0;
   }
}

#endif


wxInt32 flowMathDigiCmp::loadDATA(wxFile *FHandle,wxUint32 chunkSize,wxUint32 IDOffset,bool isCompiled)
{
   return flowObject::loadDATA(FHandle,chunkSize,IDOffset,isCompiled);
}


