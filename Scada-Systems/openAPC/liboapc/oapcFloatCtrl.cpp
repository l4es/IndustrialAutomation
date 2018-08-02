/********************************************************************************************

This program and source file is free software: you can redistribute it and/or modify it under
the terms of the GNU General Public License as published by the Free Software Foundation,
either version 3 of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with this program.
If not, see <http://www.gnu.org/licenses/>.

*********************************************************************************************/

#include <wx/wx.h>

#include "oapcFloatCtrl.h"
#include "oapc_libio.h"
#ifdef ENV_PLAYER
 #include "OSNumpad.h"
#endif


/**
Constructor for a oapcFloatCtrl object, here the GUI element is created immediately, no
additional call to Create() is necessary.
@param[in] parent the parent window where this control has to be added at
@param[in] id the ID for this element or wxID_ANY when an ID has to be assigned by
           wxWidgets automatically
@param[in] pos the position where the control has to be added within its parent window
@param[in] size the size of the oapcFloatCtrl
@param[in] style the style parameters that influence the behaviour and design of this
           control, please refer to the description of wxTextCtrl for the available
           style flags
@param[in] initial the initial value that has to be set to this control
@param[in] name the symbolic name for this GUI element
*/
oapcFloatCtrl::oapcFloatCtrl(wxWindow *parent, wxWindowID id,const wxPoint& pos,const wxSize& size,wxInt32 style,
                             double initial)
              :oapcTextCtrl(parent,id,wxEmptyString,pos,size,style)
{
   m_accuracy=4;
   SetValidator(wxTextValidator(wxFILTER_NUMERIC));
   SetValue(initial);
   m_me=this;
}



oapcFloatCtrl::~oapcFloatCtrl()
{
}



void oapcFloatCtrl::SetAccuracy(wxInt32 accuracy)
{
   m_accuracy=accuracy;
}



wxInt32 oapcFloatCtrl::GetAccuracy()
{
   return m_accuracy;
}



/**
Sets a new value for the control
@param[in] val the new value to be set
*/
void oapcFloatCtrl::SetValue(double val)
{
   wxString str;

   if (m_accuracy==0)
   {
      str.Printf(wxT("%d"),(wxInt32)OAPC_ROUND(val,0));
   }
   else
   {
      wxString str2;

      str2.Printf(wxT("%%.%df"),m_accuracy);
      str.Printf(str2,val);
/*      while (str.Right(1).Cmp(_T("0"))==0)
       str=str.Mid(0,str.Length()-1);*/
      if ((str.Right(1).Cmp(_T(","))==0) || (str.Right(1).Cmp(_T("."))==0))
       str=str+_T("0");
   }
   wxTextCtrl::SetValue(str);
}



/**
Retrieves the current value that is displayed and checks its bounds
@param[in] minVal the minimum value that is allowed for the returned value, if
           the value entered in the control is smaller than minVal it is set
           to this value
@param[in] maxVal the maximum value that is allowed for the returned value, if
           the value entered in the control is greater than maxVal it is set
           to this value
@return the value that was entered into the control as long as it not exceeds the
           given minimum and maximum value or the minimum or maximum allowed value
           otherwise
*/
double oapcFloatCtrl::GetValue(double minVal,double maxVal)
{
   double d;
   bool   changed=false;

   d=GetValueUnlimited();
   if (d<minVal)
   {
	   d=minVal;
	   changed=true;
   }
   else if (d>maxVal)
   {
	   d=maxVal;
	   changed=true;
   }
   if (oapcTextCtrl::GetValue().Length()<=0) changed=true;
   if (changed) SetValue(d);
   return d;
}




/**
Retrieves the current value that is displayed without checking its bounds
@return the value that was entered into the control
*/
double oapcFloatCtrl::GetValueUnlimited()
{
   wxString text;
   double d;

   text=wxTextCtrl::GetValue();
   if (!text.ToDouble(&d))
   {
      text.Replace(_T("."),_T(","));
      if (!text.ToDouble(&d))
      {
         text.Replace(_T(","),_T("."));
         text.ToDouble(&d);
      }
   }

   return d;
}



int oapcFloatCtrl::overflow(int WXUNUSED(i))
{
   return 0;
}



#ifdef ENV_PLAYER
#ifndef ENV_BEAM
void oapcFloatCtrl::handleOnScreenInput()
{
   if (!g_numpad)
   {
      g_numpad=new OSNumpad(this,wxID_ANY,GetValue(MIN_NUM_VALUE,MAX_NUM_VALUE));
      g_numpad->setOptPosition(m_me);
      g_numpad->ShowModal();
      delete g_numpad;
      g_numpad=NULL;
   }
}
#endif
#endif
