#ifndef hmiUserMgmntPanel_H
#define hmiUserMgmntPanel_H

#include <wx/panel.h>
#include <wx/radiobut.h>
#include <wx/button.h>
#include <wx/checkbox.h>
#include <wx/listbox.h>
#include <wx/statbox.h>
#include <wx/textctrl.h>
#include <wx/combobox.h>
#include <wx/stattext.h>

#ifndef ENV_BEAM
 #include "globals.h"
#else
 #include "../CNConstruct/cnco_globals.h"
#endif
#include "oapcTextCtrl.h"

class UserData;

class hmiUserMgmntPanel : public wxPanel
{
public:
   hmiUserMgmntPanel(wxWindow* parent);
   ~hmiUserMgmntPanel(void);
   static const wxUint32 priviFlagEnabled[MAX_PRIVI_NUM+1];
   static const wxUint32 priviFlagDisabled[MAX_PRIVI_NUM+1];
   static const wxUint32 priviFlagInvisible[MAX_PRIVI_NUM+1];
   static const wxUint32 priviFlagMask[MAX_PRIVI_NUM+1];
   static const wxUint32 priviFlagIgnore[MAX_PRIVI_NUM+1];

           wxUint32 getPriviFlags();
           void     updateUI();
   virtual bool     Enable( bool enable = true );
#ifdef ENV_PLAYER
   static void    saveUserData(wxWindow *parent);
#endif
#ifdef ENV_BEAM
   static void    deleteUserData();
#endif

private:
   void           OnListBoxSelected(wxCommandEvent &event);
   void           OnButtonClicked(wxCommandEvent &event);
   void           OnCheckBoxChecked(wxCommandEvent &event);

   wxCheckBox   *priviCB[MAX_PRIVI_NUM+1];
   wxListBox    *m_userList;
   wxButton     *m_addUser,*m_delUser,*m_setUserPwd,*m_applyUserData;
   wxStaticBox  *m_userBox;
   oapcTextCtrl *m_loginField,*m_nameField,*m_commentField;
   wxComboBox   *m_stateCombo;
   wxStaticText *m_loginText,*m_nameText,*m_commentText,*m_stateText;
   wxStaticText *m_userText,*m_privilegeText;
   UserData     *m_userData;
   bool          m_wasChanged;


   DECLARE_EVENT_TABLE()
};

#endif
