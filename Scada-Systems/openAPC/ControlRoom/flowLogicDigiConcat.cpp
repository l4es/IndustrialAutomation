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
#include "flowLogicDigiConcat.h"
#ifdef ENV_EDITOR
#include "DlgConfigflowLogic.h"
#endif
#include "oapc_libio.h"


flowLogicDigiConcat::flowLogicDigiConcat(wxUint32 type):flowLogic()
{
#ifdef ENV_EDITOR
   wxUint32 i;
#endif

   this->data.type=type;
   this->data.stdIN= OAPC_DIGI_IO0|OAPC_DIGI_IO1|OAPC_DIGI_IO2|OAPC_DIGI_IO3|OAPC_DIGI_IO4|OAPC_DIGI_IO5|OAPC_DIGI_IO6|OAPC_DIGI_IO7;
   this->data.stdOUT=OAPC_DIGI_IO0|OAPC_DIGI_IO1|OAPC_DIGI_IO2|OAPC_DIGI_IO3|OAPC_DIGI_IO4|OAPC_DIGI_IO5|OAPC_DIGI_IO6|OAPC_DIGI_IO7;
#ifdef ENV_EDITOR
   for (i=0; i<MAX_NUM_IOS; i++)
   {
      convData.outData[i].inputMask=0;
      convData.outData[i].flags=0;
      convData.outData[i].inputMask=0;
   }
   convData.outData[0].inputMask=0x1E;
#endif
}



flowLogicDigiConcat::~flowLogicDigiConcat()
{
}



wxString flowLogicDigiConcat::getDefaultName()
{
   if (data.type==FLOW_TYPE_LOGI_DIGIOR) return _T("Digital (N)OR");
   else if (data.type==FLOW_TYPE_LOGI_DIGIXOR) return _T("Digital X(N)OR");
   else if (data.type==FLOW_TYPE_LOGI_DIGIAND) return _T("Digital (N)AND");
   else wxASSERT(0);
   return _T("##invalid##");
}



#ifndef ENV_PLAYER
void flowLogicDigiConcat::doDataFlowDialog(bool hideISConfig)
{
   DlgConfigflowLogic dlg(this,(wxWindow*)g_hmiCanvas,_("Definition"),hideISConfig);

   dlg.ShowModal();
   if (dlg.returnOK)
   {
   }
   dlg.Destroy();
}



#else



wxUint64 flowLogicDigiConcat::getAssignedOutput(wxUint64 input)
{
   if ((input & OAPC_DIGI_IO0) && (digi[0]==1) &&
       ((convData.outData[0].flags & LOGIC_FLAGS_USE_CLOCK) ||
        (convData.outData[1].flags & LOGIC_FLAGS_USE_CLOCK) ||
        (convData.outData[2].flags & LOGIC_FLAGS_USE_CLOCK) ||
        (convData.outData[3].flags & LOGIC_FLAGS_USE_CLOCK) ||
        (convData.outData[4].flags & LOGIC_FLAGS_USE_CLOCK) ||
        (convData.outData[5].flags & LOGIC_FLAGS_USE_CLOCK) ||
        (convData.outData[6].flags & LOGIC_FLAGS_USE_CLOCK) ||
        (convData.outData[7].flags & LOGIC_FLAGS_USE_CLOCK)))
   {
      wxInt32  i,j,bit;
      wxUint32 outFlags=0;

      for (i=0; i<MAX_NUM_IOS; i++)
      {
         bit=1;
         for (j=0; j<MAX_NUM_IOS; j++)
         {
            if ((convData.outData[i].inputMask & bit) && (convData.outData[i].flags & LOGIC_FLAGS_USE_CLOCK))
            {
               if (i==0) outFlags|=OAPC_DIGI_IO0;
               else if (i==1) outFlags|=OAPC_DIGI_IO1;
               else if (i==2) outFlags|=OAPC_DIGI_IO2;
               else if (i==3) outFlags|=OAPC_DIGI_IO3;
               else if (i==4) outFlags|=OAPC_DIGI_IO4;
               else if (i==5) outFlags|=OAPC_DIGI_IO5;
               else if (i==6) outFlags|=OAPC_DIGI_IO6;
               else if (i==7) outFlags|=OAPC_DIGI_IO7;
            }
            bit=bit<<1;
         }
      }
      return outFlags;
   }
   else if ((input==OAPC_DIGI_IO0) || (input==OAPC_DIGI_IO1) || (input==OAPC_DIGI_IO2) || (input==OAPC_DIGI_IO3) ||
            (input==OAPC_DIGI_IO4) || (input==OAPC_DIGI_IO5) || (input==OAPC_DIGI_IO6) || (input==OAPC_DIGI_IO7))
   {
      wxInt32  i;
      wxUint32 outFlags=0;

      for (i=0; i<MAX_NUM_IOS; i++)
      {
         if ((convData.outData[i].inputMask & input) && !(convData.outData[i].flags & LOGIC_FLAGS_USE_CLOCK))
         {
            if (i==0) outFlags|=OAPC_DIGI_IO0;
            else if (i==1) outFlags|=OAPC_DIGI_IO1;
            else if (i==2) outFlags|=OAPC_DIGI_IO2;
            else if (i==3) outFlags|=OAPC_DIGI_IO3;
            else if (i==4) outFlags|=OAPC_DIGI_IO4;
            else if (i==5) outFlags|=OAPC_DIGI_IO5;
            else if (i==6) outFlags|=OAPC_DIGI_IO6;
            else if (i==7) outFlags|=OAPC_DIGI_IO7;
         }
      }
      return outFlags;
   }
   return 0;
}



wxByte flowLogicDigiConcat::getDigitalOutput(FlowConnection *connection,wxInt32 *rcode,wxUint64 WXUNUSED(lastInput),wxLongLong WXUNUSED(origCreationTime))
{
   wxByte    retValue=0,xorCnt=0;
   wxInt32   i,bit=1;

#ifdef ENV_DEBUGGER
   if ((connection->sourceOutputNum<0) || (connection->sourceOutputNum>=MAX_NUM_IOS))
   {
      g_debugWin->setDebugInformation(this,DEBUG_STOP_COND_ILLEGAL_IO,_T(""));
      *rcode=OAPC_ERROR_NO_SUCH_IO;
      return 0;
   }
#endif
   if (data.type==FLOW_TYPE_LOGI_DIGIAND) retValue=1;
   for (i=0; i<MAX_NUM_IOS; i++)
   {
      if (convData.outData[connection->sourceOutputNum].inputMask & bit)
      {
         if (data.type==FLOW_TYPE_LOGI_DIGIOR) retValue|=digi[i];
         else if (data.type==FLOW_TYPE_LOGI_DIGIXOR)
         {
            if (digi[i]) xorCnt++;
         }
         else if (data.type==FLOW_TYPE_LOGI_DIGIAND)
          retValue&=digi[i];
         else wxASSERT(0);
      }
      bit=bit<<1;
   }
   if (data.type==FLOW_TYPE_LOGI_DIGIXOR)
   {
      if (xorCnt==1) retValue=1;
      else retValue=0;
   }
   if (convData.outData[connection->sourceOutputNum].flags & LOGIC_FLAGS_INVERT)
   {
      if (retValue) retValue=0;
      else retValue=1;
   }

   *rcode=OAPC_OK;
   return retValue;
}
#endif






