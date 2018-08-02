#ifndef PRIVILEGEPANEL_H
#define PRIVILEGEPANEL_H

#include <wx/panel.h>
#include <wx/radiobut.h>
#include <wx/button.h>
#include <wx/checkbox.h>

#include "userprivi.h"


class PrivilegePanel : public wxPanel
{
public:
   PrivilegePanel(wxWindow* parent, wxWindowID id,wxUint32 priviFlags,wxUint32 priviFlagsAvailable);
   ~PrivilegePanel(void);

   wxUint32 getPriviFlags();

private:
   void ButtonClicked(wxCommandEvent &event);
   void RadiobuttonSelected(wxCommandEvent &event);

   wxRadioButton *rb[4][MAX_PRIVI_NUM+1];
   wxButton      *setButton[4];
   wxCheckBox    *depSelectCB;

   DECLARE_EVENT_TABLE()
};

#endif
