#ifndef DlgUserpriviSettings_H
#define DlgUserpriviSettings_H

#include <wx/dialog.h>
#include <wx/spinctrl.h>
#include <wx/textctrl.h>
#include <wx/bmpbuttn.h>
#include <wx/stattext.h>

#include "PrivilegePanel.h"

#if _MSC_VER > 1000
#pragma once
#endif

class DlgUserpriviSettings : public wxDialog  
{
public:
    DlgUserpriviSettings(wxWindow* parent, const wxString& title,struct userprivi_data *userPriviData);
	virtual ~DlgUserpriviSettings();

    DECLARE_EVENT_TABLE()

private:
    void     OnButtonClick(wxCommandEvent &event);
    void     OnCheckboxCheck(wxCommandEvent &event);
    void     updateUI();
    void     exchangePrivis(wxInt32 privi1,wxInt32 privi2);
    wxUint32 exchangeUserPriviFlags(wxUint32 privisEnabled,wxInt32 privi1,wxInt32 privi2);

    wxTextCtrl           *nameField[MAX_PRIVI_NUM];
    wxBitmapButton       *upBtn[MAX_PRIVI_NUM],*downBtn[MAX_PRIVI_NUM];
    wxCheckBox           *enableCB[MAX_PRIVI_NUM];
    wxCheckBox           *enableGlobalCB;
    wxStaticText         *title1,*title2;

    struct userprivi_data m_userPriviData;
};

#endif
