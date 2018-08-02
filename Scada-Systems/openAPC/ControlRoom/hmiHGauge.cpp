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

#ifndef __WXMSW__
#include <arpa/inet.h>
#endif

#include "iff.h"
#include "oapc_libio.h"
#include "HMICanvas.h"
#include "hmiObject.h"
#include "hmiHSlider.h"
#include "hmiVSlider.h"
#include "hmiHGauge.h"
#include "globals.h"
#ifdef ENV_DEBUGGER
 #include "DebugWin.h"
#endif

hmiHGauge::hmiHGauge(BasePanel *parent,flowObject *obj)
          :hmiHSlider(parent,obj)
{
   this->data.type=HMI_TYPE_HGAUGE;
   this->data.stdIN =OAPC_DIGI_IO0|OAPC_NUM_IO7;
   this->data.stdOUT=OAPC_DIGI_IO0|OAPC_NUM_IO7;
#ifdef ENV_EDITOR
   noUIFlags=OAPC_HMI_NO_UI_TEXT|OAPC_HMI_NO_UI_FONT;
   noFlowFlags=HMI_NOFLOWFLAG_ON_DIGI_IN1|HMI_NOFLOWFLAG_OUT6;
   if (!obj)
   {
       this->data.fgcolor[0]=0xBB0000; this->data.fgcolor[1]=0xBB0000;
       this->data.msizex=getDefSize().x*1000;
       this->data.msizey=getDefSize().y*1000;
       num=35;
       MinValue=0;
       MaxValue=100.0;
   }
#endif
}

hmiHGauge::~hmiHGauge()
{

}



#ifndef ENV_PLAYER
wxString hmiHGauge::getDefaultName()
{
    return _T("Horizontal Gauge");
}



wxInt32 hmiHGauge::save(wxFile *FHandle)
{
   wxInt32 length;

   if (!FHandle) return 0;

   FHandle->Write(CHUNK_HOBL_HGAG,4);
   length=sizeof(struct hmiObjectData)+sizeof(struct hmiObjectHead);
   length=htonl(length);
   FHandle->Write(&length,4);
   length=hmiObject::save(FHandle);

   return length;
}


wxPoint hmiHGauge::getDefSize()
{
   return wxPoint(200,30);
}

#endif



wxWindow *hmiHGauge::createUIElement()
{
   uiElement=new wxGauge(m_parent,wxID_ANY,MaxValue-MinValue,wxDefaultPosition,wxDefaultSize,wxGA_SMOOTH);
   hmiObject::createUIElement();
   return uiElement;
}



void hmiHGauge::doApplyData(const wxByte all)
{
    hmiObject::doApplyData(all);
    if (all)
    {
       if (!applyLayoutData())
       {
          setSize(wxRealPoint(data.msizex,data.msizey));
          setPos(wxRealPoint(data.mposx,data.mposy));
       }
    }
    uiElement->SetBackgroundColour(wxColour(data.bgcolor[digi[0]]));
    uiElement->SetForegroundColour(wxColour(data.fgcolor[digi[0]]));
    if (((wxGauge*)uiElement)->GetRange()!=MaxValue-MinValue) ((wxGauge*)uiElement)->SetRange(MaxValue-MinValue);
    ((wxGauge*)uiElement)->SetValue(num);
}



#ifdef ENV_PLAYER
/**
Sets an digital input, checks if the same thread has set that input before and uses the
input as trigger for sending the own value (if configured)
@param[in] input the flag that specifies the input
@param[in] value the value that has to be set for that input (0 or 1)
@param[in] flowThreadID the Id of the thread that wants to set this value
@return 0 if the same thread already has set a value; the thread has to finish
        in this case to avoid endless loops; 1 if the input value was set correctly
*/
wxByte hmiHGauge::setDigitalInput(FlowConnection *connection,wxByte value,wxUint32 *flowThreadID,flowObject *object)
{
   return hmiObject::setDigitalInput(connection,value,flowThreadID,object);
}



/**
Sets an numerical input and checks if the same thread has set that input before
@param[in] input the flag that specifies the input
@param[in] value the value that has to be set for that input
@param[in] flowThreadID the Id of the thread that wants to set this value
@return 0 if the same thread already has set a value; the thread has to finish
        in this case to avoid endless loops; 1 if the input value was set correctly
*/
wxByte hmiHGauge::setNumInput(FlowConnection *WXUNUSED(connection),wxFloat64 value,wxUint32 *flowThreadID,flowObject *WXUNUSED(object))
{
   if (!threadIDOK(flowThreadID,false)) return 0;
   num=value-MinValue;
   applyData(0);
   return 1;
}



wxFloat64 hmiHGauge::getNumOutput(FlowConnection *connection,wxInt32 *rcode,wxUint64 WXUNUSED(lastInput),wxLongLong WXUNUSED(origCreationTime))
{
#ifdef ENV_DEBUGGER
   if ((connection->sourceOutputNum<0) || (connection->sourceOutputNum>=MAX_NUM_IOS))
   {
      g_debugWin->setDebugInformation(this,DEBUG_STOP_COND_ILLEGAL_IO,_T(""));
      *rcode=OAPC_ERROR_NO_SUCH_IO;
      return 0;
   }
#else
   connection=connection;
#endif

   logDataCtrl->logNum(num,this);

   *rcode=OAPC_OK;
   return num+MinValue;
}
#endif

