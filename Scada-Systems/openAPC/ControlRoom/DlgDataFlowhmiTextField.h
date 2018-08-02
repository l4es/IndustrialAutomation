#ifndef DLGDATAFLOWHMIHTEXTFIELD_H
#define DLGDATAFLOWHMIHTEXTFIELD_H

#include "DlgConfigflowBase.h"

class DlgDataFlowhmiTextField : public DlgConfigflowBase
{
public:
	DlgDataFlowhmiTextField(hmiObject *object,wxWindow* parent,wxUint32 *cycleMicros, const wxString& title,wxUint32 noFlowFlags,bool hideISConfig);
	virtual ~DlgDataFlowhmiTextField();

private:
    void                 OnButtonClick(wxCommandEvent &event);
    void                 OnCheckBoxCheck(wxCommandEvent &event);
    void                 updateUI();
    wxUint32             m_noFlowFlags;
    wxUint32            *m_cycleMicros;
    struct hmiObjectData objectData;
    wxSpinCtrl          *cycleTimeField;
    wxCheckBox          *checkOnDigi1hi6,*checkOnDigi1lo6,*checkOnChanged6;
    wxCheckBox          *checkOnDigi1hi7,*checkOnDigi1lo7,*checkOnChanged7;

    DECLARE_EVENT_TABLE()
};

#endif
