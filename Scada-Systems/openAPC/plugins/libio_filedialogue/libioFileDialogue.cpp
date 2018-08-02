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



#include "libioFileDialogue.h"

BEGIN_EVENT_TABLE(libioFileDialogue, wxFileDialog)
    EVT_BUTTON(wxID_CANCEL,libioFileDialogue::OnCancel)
END_EVENT_TABLE()


libioFileDialogue::libioFileDialogue(wxWindow* parent, const wxString& message, const wxString& defaultDir, const wxString& defaultFile, const wxString& wildcard, long style, const wxPoint& pos)
                  :wxFileDialog(parent,message,defaultDir,defaultFile,wildcard,style,pos)
{
   m_cancelled=false;
}



void libioFileDialogue::OnCancel(wxCommandEvent& event)
{
   m_cancelled=true;
   event.Skip(true);
}



bool libioFileDialogue::wasCancelled()
{
   return m_cancelled;
}

