#ifndef DLGDATAFLOWHMIHSLIDER_H
#define DLGDATAFLOWHMIHSLIDER_H

#include <wx/spinctrl.h>

#include "DlgConfigflowBase.h"

class DlgDataFlowhmiHSlider : public DlgConfigflowBase  
{
public:
	DlgDataFlowhmiHSlider(hmiObject *object,wxWindow* parent,wxUint32 *cycleMicros,const wxString& title,wxUint32 noFlowFlags,bool hideISConfig);
	virtual ~DlgDataFlowhmiHSlider();

private:
    void                 OnButtonClick(wxCommandEvent &event);
    void                 OnCheckBoxCheck(wxCommandEvent &event);
    void                 updateUI();

    wxUint32             m_noFlowFlags;
    wxUint32            *m_cycleMicros;
    struct hmiObjectData objectData;
    wxSpinCtrl          *cycleTimeField;
    wxCheckBox          *checkOnDigi1hi6,*checkOnDigi1lo6,*checkOnChanging6,*checkOnChanged6;
    wxCheckBox          *checkOnDigi1hi7,*checkOnDigi1lo7,*checkOnChanging7,*checkOnChanged7;

    DECLARE_EVENT_TABLE()
};

#endif
