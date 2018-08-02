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
#include "flowLogicNumConcat.h"
#ifdef ENV_EDITOR
#include "DlgConfigflowLogic.h"
#endif
#include "oapc_libio.h"


flowLogicNumConcat::flowLogicNumConcat(wxUint32 type):flowLogic()
{
#ifdef ENV_EDITOR
   wxUint32 i;
#endif

   this->data.type=type;
   this->data.stdIN= OAPC_DIGI_IO0|OAPC_NUM_IO1|OAPC_NUM_IO2|OAPC_NUM_IO3|OAPC_NUM_IO4|OAPC_NUM_IO5|OAPC_NUM_IO6|OAPC_NUM_IO7;
   this->data.stdOUT=             OAPC_NUM_IO1|OAPC_NUM_IO2|OAPC_NUM_IO3|OAPC_NUM_IO4|OAPC_NUM_IO5|OAPC_NUM_IO6|OAPC_NUM_IO7;
#ifdef ENV_EDITOR
   for (i=0; i<MAX_NUM_IOS; i++)
   {
      convData.outData[i].flags=0; //LOGIC_FLAGS_USE_CLOCK;
      convData.outData[i].inputMask=0x00;
      if ((data.type==hmiObject::FLOW_TYPE_LOGI_NUMMUL) || (data.type==hmiObject::FLOW_TYPE_LOGI_NUMDIV)) ConstValue[i]=1;
      else ConstValue[i]=0;
   }
   convData.outData[0].inputMask=0x1E;
#else
   wxInt32 i;

   for (i=0; i<MAX_NUM_IOS; i++) num[i]=0;
#endif

}



flowLogicNumConcat::~flowLogicNumConcat()
{
}



wxString flowLogicNumConcat::getDefaultName()
{
   if (data.type==FLOW_TYPE_LOGI_NUMOR) return _T("Numeric (N)OR");
   else if (data.type==FLOW_TYPE_LOGI_NUMXOR) return _T("Numeric X(N)OR");
   else if (data.type==FLOW_TYPE_LOGI_NUMAND) return _T("Numeric (N)AND");
   else if (data.type==FLOW_TYPE_LOGI_NUMADD) return _T("Numeric Add");
   else if (data.type==FLOW_TYPE_LOGI_NUMSUB) return _T("Numeric Sub");
   else if (data.type==FLOW_TYPE_LOGI_NUMMUL) return _T("Numeric Mul");
   else if (data.type==FLOW_TYPE_LOGI_NUMDIV) return _T("Numeric Div");
   else wxASSERT(0);
   return _T("##invalid##");
}



#ifndef ENV_PLAYER
void flowLogicNumConcat::doDataFlowDialog(bool hideISConfig)
{
   DlgConfigflowLogic dlg(this,(wxWindow*)g_hmiCanvas,_("Definition"),hideISConfig);

   dlg.ShowModal();
   if (dlg.returnOK)
   {
   }
   dlg.Destroy();
}



#else



wxUint64 flowLogicNumConcat::getAssignedOutput(wxUint64 input)
{
   if ((input & OAPC_DIGI_IO0) && (digi[0]==1) &&
       ((convData.outData[1].flags & LOGIC_FLAGS_USE_CLOCK) ||
        (convData.outData[2].flags & LOGIC_FLAGS_USE_CLOCK) ||
        (convData.outData[3].flags & LOGIC_FLAGS_USE_CLOCK) ||
        (convData.outData[4].flags & LOGIC_FLAGS_USE_CLOCK) ||
        (convData.outData[5].flags & LOGIC_FLAGS_USE_CLOCK) ||
        (convData.outData[6].flags & LOGIC_FLAGS_USE_CLOCK) ||
        (convData.outData[7].flags & LOGIC_FLAGS_USE_CLOCK)))
   {
      wxInt32  i,j,bit;
      wxUint32 outFlags=0;

      for (i=1; i<MAX_NUM_IOS; i++)
      {
         bit=2;

         for (j=1; j<MAX_NUM_IOS; j++)
         {
            if ((convData.outData[i].inputMask & bit) && (convData.outData[i].flags & LOGIC_FLAGS_USE_CLOCK))
            {
               if (i==1) outFlags|=OAPC_NUM_IO1;
               else if (i==2) outFlags|=OAPC_NUM_IO2;
               else if (i==3) outFlags|=OAPC_NUM_IO3;
               else if (i==4) outFlags|=OAPC_NUM_IO4;
               else if (i==5) outFlags|=OAPC_NUM_IO5;
               else if (i==6) outFlags|=OAPC_NUM_IO6;
               else if (i==7) outFlags|=OAPC_NUM_IO7;
            }
            bit=bit<<1;
         }
      }
      return outFlags;;
   }
   else if ((input==OAPC_NUM_IO1) || (input==OAPC_NUM_IO2) || (input==OAPC_NUM_IO3) ||
            (input==OAPC_NUM_IO4) || (input==OAPC_NUM_IO5) || (input==OAPC_NUM_IO6) || (input==OAPC_NUM_IO7))
   {
      wxInt32  i;
      wxUint32 outFlags=0;

      for (i=1; i<MAX_NUM_IOS; i++)
      {
         if ((convData.outData[i].inputMask & (input>>8)) && !(convData.outData[i].flags & LOGIC_FLAGS_USE_CLOCK))
         {
            if (i==1) outFlags|=OAPC_NUM_IO1;
            else if (i==2) outFlags|=OAPC_NUM_IO2;
            else if (i==3) outFlags|=OAPC_NUM_IO3;
            else if (i==4) outFlags|=OAPC_NUM_IO4;
            else if (i==5) outFlags|=OAPC_NUM_IO5;
            else if (i==6) outFlags|=OAPC_NUM_IO6;
            else if (i==7) outFlags|=OAPC_NUM_IO7;
         }
      }
      return outFlags;
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
wxByte flowLogicNumConcat::setNumInput(FlowConnection *connection,wxFloat64 value,wxUint32 *flowThreadID,flowObject *WXUNUSED(object))
{
   if (!threadIDOK(flowThreadID,(convData.outData[connection->targetInputNum].flags & LOGIC_FLAGS_ALLOW_LOOPBACK)==LOGIC_FLAGS_ALLOW_LOOPBACK)) return 0;
   num[connection->targetInputNum]=value;
   return 1;
}



wxFloat64 flowLogicNumConcat::getNumOutput(FlowConnection *connection,wxInt32 *rcode,wxUint64 WXUNUSED(lastInput),wxLongLong WXUNUSED(origCreationTime))
{
   wxInt32   retValue=0;
   wxFloat64 retValueF=0;
   wxInt32   i,bit=2;
   bool      firstValue=true;

#ifdef ENV_DEBUGGER
   if ((connection->sourceOutputNum<0) || (connection->sourceOutputNum>=MAX_NUM_IOS))
   {
      g_debugWin->setDebugInformation(this,DEBUG_STOP_COND_ILLEGAL_IO,_T(""));
      *rcode=OAPC_ERROR_NO_SUCH_IO;
      return 0;
   }
#endif
   if (data.type==FLOW_TYPE_LOGI_NUMAND) retValue=0xFFFFFFFF;
   for (i=1; i<MAX_NUM_IOS; i++)
   {
      if (convData.outData[connection->sourceOutputNum].inputMask & bit)
      {
         if (data.type==FLOW_TYPE_LOGI_NUMOR) retValue|=(wxInt32)OAPC_ROUND(num[i],0);
         else if (data.type==FLOW_TYPE_LOGI_NUMXOR) retValue^=(wxInt32)OAPC_ROUND(num[i],0);
         else if (data.type==FLOW_TYPE_LOGI_NUMAND) retValue&=(wxInt32)OAPC_ROUND(num[i],0);
         else if (data.type==FLOW_TYPE_LOGI_NUMADD)
          retValueF+=num[i];
         else if (data.type==FLOW_TYPE_LOGI_NUMSUB)
         {
            if (firstValue) retValueF=num[i];
            else retValueF-=num[i];
            firstValue=false;
         }
         else if (data.type==FLOW_TYPE_LOGI_NUMMUL)
         {
            if (firstValue) retValueF=num[i];
            else retValueF*=num[i];
            firstValue=false;
         }
         else if (data.type==FLOW_TYPE_LOGI_NUMDIV)
         {
            if (firstValue) retValueF=num[i];
            else retValueF/=num[i];
            firstValue=false;
         }
         else wxASSERT(0);
      }
      bit=bit<<1;
   }

   if (data.type==FLOW_TYPE_LOGI_NUMADD) retValueF+=ConstValue[connection->sourceOutputNum];
   else if (data.type==FLOW_TYPE_LOGI_NUMSUB) retValueF-=ConstValue[connection->sourceOutputNum];
   else if (data.type==FLOW_TYPE_LOGI_NUMMUL) retValueF*=ConstValue[connection->sourceOutputNum];
   else if (data.type==FLOW_TYPE_LOGI_NUMDIV) retValueF/=ConstValue[connection->sourceOutputNum];
   
   if (convData.outData[connection->sourceOutputNum].flags & LOGIC_FLAGS_INVERT)
    retValue^=0xFFFFFFFF;

   *rcode=OAPC_OK;
   if ((data.type==FLOW_TYPE_LOGI_NUMADD) || (data.type==FLOW_TYPE_LOGI_NUMSUB) || 
       (data.type==FLOW_TYPE_LOGI_NUMMUL) || (data.type==FLOW_TYPE_LOGI_NUMDIV)) return retValueF;
   return (wxFloat64)retValue;
}
#endif






