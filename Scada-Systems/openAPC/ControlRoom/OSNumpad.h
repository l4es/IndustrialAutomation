#ifndef OSNumpad_H
#define OSNumpad_H

#include <wx/wx.h>
#include <wx/dialog.h>
#include <wx/textctrl.h>
#include <wx/spinctrl.h>
#include <wx/button.h>
#include <wx/tglbtn.h>

#include "OSKeyboard.h"

class oapcSpinCtrl;
class oapcTextCtrl;

class OSNumpad : public OSKeyboard
{
public:
   OSNumpad(oapcTextCtrl* textInput,wxWindowID id,double value);
   ~OSNumpad();

private:
   void OnButtonClicked(wxCommandEvent &event);
   void init(wxFloat64 value);

   wxString        m_row;
   wxButton       *m_keyButton[4][3];
   wxButton       *m_minus;
   wxBitmapButton *m_backspace,*m_enter;

   DECLARE_EVENT_TABLE()
};

#endif
