#ifndef DLGCONFIGFLOWCONVERTNUM2Char_H
#define DLGCONFIGFLOWCONVERTNUM2Char_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "flowConverter.h"
#include "oapcNoSpaceCtrl.h"
#include "DlgConfigflowName.h"


class flowConverterNum2Char;

class DlgConfigflowConvertNum2Char : public DlgConfigflowName
{
public:
	DlgConfigflowConvertNum2Char(flowConverterNum2Char *object,wxWindow* parent, const wxString& title,bool hideISConfig);
	virtual ~DlgConfigflowConvertNum2Char();

   DECLARE_CLASS(DlgConfigflowConvertNum2Char)

private:
    void                 OnButtonClick(wxCommandEvent &event);
    void                 OnCheckBoxCheck(wxCommandEvent &event);
    void                 OnRadioButtonCheck(wxCommandEvent &event);
    void                 updateUI();

    flowConverterNum2Char *m_object;
    wxRadioButton         *m_modeBinaryRB[CONVERTER_MAX_OUTPUTS],*m_modeDirectRB[CONVERTER_MAX_OUTPUTS],*m_modeFormatRB[CONVERTER_MAX_OUTPUTS];
    wxTextCtrl            *m_inNum[CONVERTER_MAX_OUTPUTS][CONVERTER_MAX_ASSIGNMENTS],*m_outChar[CONVERTER_MAX_OUTPUTS][CONVERTER_MAX_ASSIGNMENTS],
                          *m_fmtChar[CONVERTER_MAX_OUTPUTS];
    wxCheckBox            *m_enableBox[CONVERTER_MAX_OUTPUTS][CONVERTER_MAX_ASSIGNMENTS],
                          *m_modeOnClock[MAX_NUM_IOS];
    wxStaticText          *m_inText[CONVERTER_MAX_OUTPUTS],
                          *m_outText[CONVERTER_MAX_OUTPUTS];
    wxButton              *m_cmpButton[MAX_NUM_IOS][CONVERTER_MAX_ASSIGNMENTS];

    DECLARE_EVENT_TABLE()

};

#endif
