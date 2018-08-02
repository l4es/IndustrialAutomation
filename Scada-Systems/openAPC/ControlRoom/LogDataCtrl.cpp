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

#include "globals.h"
#include "flowObject.h"
#include "LogDataCtrl.h"



LogDataCtrl::LogDataCtrl(struct hmiObjectLogData logData[MAX_LOG_TYPES])
{
   wxInt32 i;

   SDBG
   for (i=0; i<MAX_LOG_TYPES; i++)
    data[i]=logData[i];
   digiValid=false;
   numValid=false;
   charValid=false;
}



LogDataCtrl::~LogDataCtrl()
{
    SDBG
}



void LogDataCtrl::logDigi(wxByte val,flowObject *object)
{
   wxInt32 i;

   if (!g_flowLog) return;
   for (i=0; i<MAX_LOG_TYPES; i++)
   {
      if ((!digiValid) || (prevDigiVal!=val))
      {
         if (data[i].flags & LOG_FLAG_ON_VALUE_CHANGED)
          g_flowLog->setLogInfo(_("Value changed"),i,object);
      }
      if ((val==1) && (data[i].flags & LOG_FLAG_ON_DIGI_VALUE_HI))
       g_flowLog->setLogInfo(_("Value set to HIGH"),i,object);
      if ((val==0) && (data[i].flags & LOG_FLAG_ON_DIGI_VALUE_LO))
       g_flowLog->setLogInfo(_("Value set to LOW"),i,object);
   }
   prevDigiVal=val;
   digiValid=true;
}



void LogDataCtrl::logNum(wxFloat64 val,flowObject *object)
{
   wxInt32 i;
   bool    allowNormalRangeMessage,doNormalRangeMessage;

   if (!g_flowLog) return;
   for (i=0; i<MAX_LOG_TYPES; i++)
   {
      allowNormalRangeMessage=true;
      doNormalRangeMessage=false;
      if ((!numValid) || (prevNumVal!=val))
      {
         if (data[i].flags & LOG_FLAG_ON_VALUE_CHANGED)
          g_flowLog->setLogInfo(_("Value changed"),i,object);
      }

      if (data[i].flags & LOG_FLAG_ON_NUM_VALUE_BELOW)
      {
         if ((val<data[i].mBelowVal/1000.0) && ((!numValid) || (prevNumVal>=data[i].mBelowVal/1000.0)))
         {
            g_flowLog->setLogInfo(_("Value smaller than limit"),i,object);
            allowNormalRangeMessage=false;
         }
         else if ((val>data[i].mBelowVal/1000.0) && (prevNumVal<=data[i].mBelowVal/1000.0))
          doNormalRangeMessage=true;
      }

      if (data[i].flags & LOG_FLAG_ON_NUM_VALUE_BETWEEN)
      {
         if ((val>=data[i].mFromVal/1000.0) && (val<=data[i].mToVal/1000.0) && 
             ((!numValid) || (prevNumVal<data[i].mFromVal/1000.0) || (prevNumVal>data[i].mToVal/1000.0)))
         {
            g_flowLog->setLogInfo(_("Value in limited range"),i,object);
            allowNormalRangeMessage=false;
         }
         else if (((val<data[i].mFromVal/1000.0) || (val>data[i].mToVal/1000.0)) && 
             ((prevNumVal>=data[i].mFromVal/1000.0) && (prevNumVal<=data[i].mToVal/1000.0)))
          doNormalRangeMessage=true;
      }
      if (data[i].flags & LOG_FLAG_ON_NUM_VALUE_ABOVE)
      {
         if ((val>data[i].mAboveVal/1000.0) && ((!numValid) || (prevNumVal<=data[i].mAboveVal/1000.0)))
         {
            allowNormalRangeMessage=false;
         }
         else if ((val<data[i].mAboveVal/1000.0) && (prevNumVal>=data[i].mAboveVal/1000.0))
          doNormalRangeMessage=true;
      }
      if ((doNormalRangeMessage) && (allowNormalRangeMessage)) g_flowLog->setLogInfo(_("Value in normal range"),i,object);

      if (data[i].flags & LOG_FLAG_ON_NUM_VALUE_ABOVE)
      {
         if ((val>data[i].mAboveVal/1000.0) && ((!numValid) || (prevNumVal<=data[i].mAboveVal/1000.0)))
         {
            g_flowLog->setLogInfo(_("Value greater than limit"),i,object);
            allowNormalRangeMessage=false;
         }
      }
   }
   prevNumVal=val;
   numValid=true;
}



void LogDataCtrl::logChar(wxString val,flowObject *object)
{
   wxInt32 i;

   if (!g_flowLog) return;
   for (i=0; i<MAX_LOG_TYPES; i++)
   {
      if ((!charValid) || (prevCharVal!=val))
      {
         if (data[i].flags & LOG_FLAG_ON_VALUE_CHANGED)
          g_flowLog->setLogInfo(_("Value changed"),i,object);
      }
   }
   prevCharVal=val;
   charValid=true;
}


