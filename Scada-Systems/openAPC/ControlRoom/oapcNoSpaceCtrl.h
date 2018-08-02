#ifndef WX_NOSPACECTRL_H
#define WX_NOSPACECTRL_H

#include <wx/textctrl.h>
#include <wx/valtext.h>

class oapcNoSpaceCtrl : public wxTextCtrl
{
public:
   oapcNoSpaceCtrl();
   oapcNoSpaceCtrl(wxWindow *parent, wxWindowID id,wxString initial);
   virtual ~oapcNoSpaceCtrl();

   virtual bool     Create(wxWindow *parent, wxWindowID id,
                           const wxPoint& pos = wxDefaultPosition,
                           const wxSize& size = wxDefaultSize,
                           long style = 0,
                           const wxString& name = wxTextCtrlNameStr);
           void     SetValue(wxString val);
           wxString GetValue();

private:

   wxTextValidator  textValidator;
   wxArrayString    charExcludes;

};



#endif
