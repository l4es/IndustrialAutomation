#ifndef OSKEYBOARD_H
#define OSKEYBOARD_H

#include <wx/wx.h>
#include <wx/dialog.h>
#include <wx/textctrl.h>
#include <wx/button.h>
#include <wx/tglbtn.h>

class oapcTextCtrl;

class OSKeyboard : public wxDialog
{
public:
   OSKeyboard(oapcTextCtrl* input,wxWindowID id, const wxString& value);
   OSKeyboard(oapcTextCtrl* input,wxWindowID id);
   ~OSKeyboard();

   virtual void setOptPosition(wxWindow *win);

protected:

   wxTextCtrl     *m_inputField;
   oapcTextCtrl   *m_input;

private:
   wxString        m_row[2][4];
   void OnButtonClicked(wxCommandEvent &event);
   void setKbdValues(bool shift);

   wxButton       *m_keyButton[4][13];
   wxButton       *m_backspace,*m_space,*m_enter;
   wxToggleButton *m_capsLock,*m_shiftOnce1,*m_shiftOnce2;

   DECLARE_EVENT_TABLE()
};

#endif
