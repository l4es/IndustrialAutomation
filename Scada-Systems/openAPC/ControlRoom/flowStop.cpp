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
#include "flowStop.h"
#include "oapc_libio.h"

#ifdef ENV_WINDOWS
 #ifdef ENV_WINDOWSCE
  #include "Winsock2.h"
 #endif
#endif



flowStop::flowStop():flowObject(NULL)
{
   this->data.type=FLOW_TYPE_FLOW_STOP;
   this->data.stdIN =OAPC_DIGI_IO2;
   this->data.stdOUT=0;
}



flowStop::~flowStop()
{
}



#ifdef ENV_EDITOR
wxString flowStop::getDefaultName()
{
   return _T("Exit Application");
}



#else



/**
Sets an digital input and checks if the same thread has set that input before
@param[in] input the flag that specifies the input
@param[in] value the value that has to be set for that input (0 or 1)
@param[in] flowThreadID the Id of the thread that wants to set this value
@return 0 if the same thread already has set a value; the thread has to finish
        in this case to avoid endless loops; 1 if the input value was set correctly
*/
wxByte flowStop::setDigitalInput(FlowConnection *connection,wxByte value,wxUint32 *flowThreadID,flowObject *WXUNUSED(object))
{
   if (!threadIDOK(flowThreadID,false)) return 0;
   if ((connection->targetInputNum==2) && (value==1))
   {
      g_breakThreads=1;
      return 1;
   }
   else return 0;
}
#endif

