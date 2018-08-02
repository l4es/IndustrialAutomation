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

#ifndef WX_FLOATCTRL_H
#define WX_FLOATCTRL_H

#include "oapcTextCtrl.h"

class OAPC_EXT_API oapcFloatCtrl : public oapcTextCtrl
{
public:
   oapcFloatCtrl(wxWindow *parent, wxWindowID id,
              const wxPoint& pos = wxDefaultPosition,
              const wxSize& size = wxDefaultSize,
              wxInt32 style = 0,
              double initial=0.0);
   virtual ~oapcFloatCtrl();

           void    SetAccuracy(wxInt32 val);
           wxInt32 GetAccuracy();
           void    SetValue(double val);
           double  GetValue(double minVal/*=LONG_MIN*/,double maxVal/*=LONG_MAX*/);
           double  GetValueUnlimited();

   wxWindow      *m_me;
   wxInt32        m_accuracy;

protected:
   int overflow(int);
#ifdef ENV_PLAYER
#ifndef ENV_BEAM
   virtual void handleOnScreenInput();
#endif
#endif

private:

};



#endif
