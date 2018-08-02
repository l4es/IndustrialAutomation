#ifndef DLGCONFIGFLOWCONVERTNUM2DIGI_H
#define DLGCONFIGFLOWCONVERTNUM2DIGI_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "flowConverter.h"
#include "oapcNoSpaceCtrl.h"
#include "DlgConfigflowName.h"

class flowConverterNum2Digi;

class DlgConfigflowConvertNum2Digi : public DlgConfigflowName
{
public:
	DlgConfigflowConvertNum2Digi(flowConverterNum2Digi *object,wxWindow* parent, const wxString& title,bool hideISConfig);
	virtual ~DlgConfigflowConvertNum2Digi();

   DECLARE_CLASS(DlgConfigflowConvertNum2Digi)

private:
    void                 OnButtonClick(wxCommandEvent &event);
    void                 OnCheckBoxCheck(wxCommandEvent &event);
    void                 OnRadioButtonCheck(wxCommandEvent &event);
    void                 updateUI();

    flowConverterNum2Digi *m_object;
    wxTextCtrl            *m_inNum[CONVERTER_MAX_OUTPUTS][CONVERTER_MAX_ASSIGNMENTS];
    wxRadioButton         *m_outLow[CONVERTER_MAX_OUTPUTS][CONVERTER_MAX_ASSIGNMENTS],
                          *m_outHigh[CONVERTER_MAX_OUTPUTS][CONVERTER_MAX_ASSIGNMENTS],
                          *m_outPulseLoHi[CONVERTER_MAX_OUTPUTS][CONVERTER_MAX_ASSIGNMENTS],
                          *m_outPulseHiLo[CONVERTER_MAX_OUTPUTS][CONVERTER_MAX_ASSIGNMENTS];                           ;
    wxCheckBox            *m_enableBox[CONVERTER_MAX_OUTPUTS][CONVERTER_MAX_ASSIGNMENTS],
                          *m_modeOnClock[MAX_NUM_IOS];
    wxStaticText          *m_inText[CONVERTER_MAX_OUTPUTS],
                          *m_caseText[CONVERTER_MAX_OUTPUTS],
                          *m_outText1[CONVERTER_MAX_OUTPUTS],
                          *m_outText2[CONVERTER_MAX_OUTPUTS],
                          *m_outText3[CONVERTER_MAX_OUTPUTS],
                          *m_outText4[CONVERTER_MAX_OUTPUTS];
    wxButton              *m_cmpButton[MAX_NUM_IOS][CONVERTER_MAX_ASSIGNMENTS];

    DECLARE_EVENT_TABLE()

};

#endif
