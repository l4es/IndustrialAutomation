/********************************************************************************************

These sources are distributed in the hope that they will be useful, but WITHOUT ANY WARRANTY;
without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. All
the information given here, within the interface descriptions and within the specification
are subject to change without notice. Errors and omissions excepted.

These sources demonstrate the usage of the OpenAPC Plug-In programming interface. They can be
used freely according to the OpenAPC Dual License: As long as the sources and the resulting
applications/libraries/Plug-Ins are used together with the OpenAPC software, they are
licensed as freeware. When you use them outside the OpenAPC software they are licensed under
the terms of the GNU General Public License.

For more information please refer to http://www.openapc.com/oapc_license.php

*********************************************************************************************/



#ifndef LIBIOFILEDIALOGUE_H
#define LIBIOFILEDIALOGUE_H

#include <wx/wx.h>

class libioFileDialogue : public wxFileDialog
{
public:
   libioFileDialogue(wxWindow* parent, const wxString& message =wxEmptyString, const wxString& defaultDir =wxEmptyString, const wxString& defaultFile =wxEmptyString, const wxString& wildcard =_("*.*"), long style = 0, const wxPoint& pos = wxDefaultPosition);
   void OnCancel(wxCommandEvent& event);
   bool wasCancelled();

private:
   bool m_cancelled;
   
   DECLARE_EVENT_TABLE()
};

#endif /* LEARNDIALOGUE_H_ */


