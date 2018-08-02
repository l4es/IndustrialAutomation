#ifndef DLGPROJECTSETTINGS_H
#define DLGPROJECTSETTINGS_H

#include <wx/dialog.h>
#include <wx/spinctrl.h>


#if _MSC_VER > 1000
#pragma once
#endif

class oapcFloatCtrl;

class DlgProjectSettings : public wxDialog  
{
public:
    DlgProjectSettings(wxWindow* parent, const wxString& title);
	virtual ~DlgProjectSettings();

    DECLARE_EVENT_TABLE()

private:
    void OnButtonClick(wxCommandEvent &event);
    void updateUI();

    wxSpinCtrl       *visW,*visH,*visGridW,*visGridH,*flowT,*timerRes;
    oapcFloatCtrl    *touchFactor;
    wxTextCtrl       *remSingleIP,*remDoubleIP1,*remDoubleIP2;
    wxButton         *visbgCol,*touchFont;
    wxCheckBox       *snapGridCB,*locSingleIS,*remSingleIS,*locDoubleIS,*remDoubleIS,*touchCB;
    wxStaticText     *touchFontText,*touchFactorText,*remSingleIPText,*remDoubleIP1Text,*remDoubleIP2Text;
};

#endif
