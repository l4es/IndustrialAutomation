#ifndef CHANGEPWDDLG_H
#define CHANGEPWDDLG_H

#include <wx/dialog.h>
#include "oapcTextCtrl.h"

class UserData;

class ChangePwdDlg:public wxDialog
{
public:
   ChangePwdDlg(wxWindow *parent,wxString title,UserData *userData);
   ~ChangePwdDlg();

   bool m_ok;

private:
   void OnButtonClick(wxCommandEvent &event);

   oapcTextCtrl *oldPwd,*newPwd,*repPwd;
   UserData     *m_userData;

   DECLARE_EVENT_TABLE()
};

#endif
