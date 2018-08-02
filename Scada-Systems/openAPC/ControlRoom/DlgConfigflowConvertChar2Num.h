#ifndef DLGCONFIGFLOWCONVERTCHAR2NUM_H
#define DLGCONFIGFLOWCONVERTCHAR2NUM_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "flowConverter.h"
#include "DlgConfigflowName.h"
#include "DlgConfigflowName.h"

class flowConverterChar2Num;

class DlgConfigflowConvertChar2Num : public DlgConfigflowName  
{
public:
	DlgConfigflowConvertChar2Num(flowConverterChar2Num *object,wxWindow* parent, const wxString& title,bool hideISConfig);
	virtual ~DlgConfigflowConvertChar2Num();

   DECLARE_CLASS(DlgConfigflowConvertChar2Num)

private:
    void                 OnButtonClick(wxCommandEvent &event);
    void                 OnCheckBoxCheck(wxCommandEvent &event);
    void                 OnRadioButtonCheck(wxCommandEvent &event);
    void                 updateUI();

    flowConverterChar2Num *m_object;
    wxRadioButton         *m_modeBinaryRB[CONVERTER_MAX_OUTPUTS],*m_modeDirectRB[CONVERTER_MAX_OUTPUTS];
    wxTextCtrl            *m_inChar[CONVERTER_MAX_OUTPUTS][CONVERTER_MAX_ASSIGNMENTS],*m_outNum[CONVERTER_MAX_OUTPUTS][CONVERTER_MAX_ASSIGNMENTS];
    wxCheckBox            *m_caseBox[CONVERTER_MAX_OUTPUTS][CONVERTER_MAX_ASSIGNMENTS],*m_enableBox[CONVERTER_MAX_OUTPUTS][CONVERTER_MAX_ASSIGNMENTS],
                          *m_modeOnClock[MAX_NUM_IOS];
    wxStaticText          *m_inText[CONVERTER_MAX_OUTPUTS],*m_caseText[CONVERTER_MAX_OUTPUTS],*m_outText[CONVERTER_MAX_OUTPUTS];
    wxButton              *m_cmpButton[MAX_NUM_IOS][CONVERTER_MAX_ASSIGNMENTS];

    DECLARE_EVENT_TABLE()

};

#endif
