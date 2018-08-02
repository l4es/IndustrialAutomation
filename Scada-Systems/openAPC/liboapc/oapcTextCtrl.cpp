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

#include "oapcTextCtrl.h"
#ifdef ENV_PLAYER
#include "OSKeyboard.h"
#endif



oapcTextCtrl::oapcTextCtrl(wxWindow *parent,wxWindowID id,wxString value,const wxPoint& pos,const wxSize& size,wxInt32 style)
             :wxTextCtrl(parent,id,value,pos,size,style)
{
#ifdef ENV_PLAYER
#ifndef ENV_BEAM
   if ((!this->IsEnabled()) || (!this->IsEditable())) return;
   if ((g_objectList.m_projectData->flags & PROJECT_FLAG_TOUCHSCREEN_SUPPORT)==0) return;
   Connect(wxEVT_LEFT_DOWN,wxMouseEventHandler(oapcTextCtrl::OnMouseEvent), NULL, this );
#endif
#endif
}



oapcTextCtrl::~oapcTextCtrl()
{
}


#ifdef ENV_PLAYER
#ifndef ENV_BEAM
void oapcTextCtrl::handleOnScreenInput()
{
   if ((!this->IsEnabled()) || (!this->IsEditable())) return;
   if ((g_objectList.m_projectData->flags & PROJECT_FLAG_TOUCHSCREEN_SUPPORT)==0) return;
   OSKeyboard dlg(this,wxID_ANY,GetValue());
   dlg.ShowModal();
}


void oapcTextCtrl::OnMouseEvent(wxMouseEvent& WXUNUSED(event))
{
   handleOnScreenInput();
}
#endif
#endif





