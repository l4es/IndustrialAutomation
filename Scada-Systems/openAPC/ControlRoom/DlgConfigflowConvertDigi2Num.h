#ifndef DLGCONFIGFLOWCONVERTDIGI2NUM_H
#define DLGCONFIGFLOWCONVERTDIGI2NUM_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "flowConverter.h"
#include "DlgConfigflowName.h"
#include "DlgConfigflowName.h"

class flowConverterDigi2Num;

class DlgConfigflowConvertDigi2Num : public DlgConfigflowName  
{
public:
	DlgConfigflowConvertDigi2Num(flowConverterDigi2Num *object,wxWindow* parent, const wxString& title,bool hideISConfig);
	virtual ~DlgConfigflowConvertDigi2Num();

   DECLARE_CLASS(DlgConfigflowConvertDigi2Num)

private:
    void                 OnButtonClick(wxCommandEvent &event);
    void                 OnCheckBoxCheck(wxCommandEvent &event);
    void                 OnRadioButtonCheck(wxCommandEvent &event);
    void                 updateUI();

    flowConverterDigi2Num *m_object;
    wxCheckBox            *m_maskBox[CONVERTER_MAX_OUTPUTS][CONVERTER_MAX_INPUTS],
                          *m_modeOnClock[MAX_NUM_IOS];
    wxRadioButton         *m_modeBinaryRB[CONVERTER_MAX_OUTPUTS],*m_modeDirectRB[CONVERTER_MAX_OUTPUTS];
    wxTextCtrl            *m_lowValueField[CONVERTER_MAX_OUTPUTS][CONVERTER_MAX_INPUTS],*m_highValueField[CONVERTER_MAX_OUTPUTS][CONVERTER_MAX_INPUTS];
    wxStaticText          *m_ValueText[CONVERTER_MAX_OUTPUTS][CONVERTER_MAX_INPUTS],*m_lowText[CONVERTER_MAX_OUTPUTS],*m_highText[CONVERTER_MAX_OUTPUTS];

    DECLARE_EVENT_TABLE()

};

#endif
