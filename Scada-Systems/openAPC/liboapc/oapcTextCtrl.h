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

#ifndef OAPCTEXTCTRL_H
#define OAPCTEXTCTRL_H

#include "liboapc.h"

#if defined ENV_EDITOR || ENV_PLAYER
#include "OSKeyboard.h"
#endif

class OAPC_EXT_API oapcTextCtrl : public wxTextCtrl
{
public:
   oapcTextCtrl(wxWindow *parent, wxWindowID id,wxString value=wxEmptyString,
              const wxPoint& pos = wxDefaultPosition,
              const wxSize& size = wxDefaultSize,
              wxInt32 style = 0);
   virtual ~oapcTextCtrl();

protected:
#ifdef ENV_PLAYER
#ifndef ENV_BEAM
   virtual void handleOnScreenInput();
#endif
#endif

private:
#ifdef ENV_PLAYER
#ifndef ENV_BEAM
   void OnMouseEvent(wxMouseEvent& event);
#endif
#endif

};



#endif
