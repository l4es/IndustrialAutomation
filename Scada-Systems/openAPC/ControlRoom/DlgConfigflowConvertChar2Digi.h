#ifndef DLGCONFIGFLOWCONVERTCHAR2DIGI_H
#define DLGCONFIGFLOWCONVERTCHAR2DIGI_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "flowConverter.h"
#include "DlgConfigflowName.h"
#include "DlgConfigflowName.h"

class flowConverterChar2Digi;

class DlgConfigflowConvertChar2Digi : public DlgConfigflowName  
{
public:
	DlgConfigflowConvertChar2Digi(flowConverterChar2Digi *object,wxWindow* parent, const wxString& title,bool hideISConfig);
	virtual ~DlgConfigflowConvertChar2Digi();

   DECLARE_CLASS(DlgConfigflowConvertChar2Digi)

private:
   void                 OnButtonClick(wxCommandEvent &event);
   void                 OnCheckBoxCheck(wxCommandEvent &event);
   void                 OnRadioButtonCheck(wxCommandEvent &event);
   void                 updateUI();

   flowConverterChar2Digi *m_object;
   wxTextCtrl             *m_inChar[CONVERTER_MAX_OUTPUTS][CONVERTER_MAX_ASSIGNMENTS];
   wxRadioButton          *m_outLow[CONVERTER_MAX_OUTPUTS][CONVERTER_MAX_ASSIGNMENTS],
                          *m_outHigh[CONVERTER_MAX_OUTPUTS][CONVERTER_MAX_ASSIGNMENTS],
                          *m_outPulseLoHi[CONVERTER_MAX_OUTPUTS][CONVERTER_MAX_ASSIGNMENTS],
                          *m_outPulseHiLo[CONVERTER_MAX_OUTPUTS][CONVERTER_MAX_ASSIGNMENTS];                           ;
   wxCheckBox             *m_caseBox[CONVERTER_MAX_OUTPUTS][CONVERTER_MAX_ASSIGNMENTS],*m_enableBox[CONVERTER_MAX_OUTPUTS][CONVERTER_MAX_ASSIGNMENTS],
                          *m_modeOnClock[MAX_NUM_IOS];
   wxStaticText           *m_inText[CONVERTER_MAX_OUTPUTS],*m_caseText[CONVERTER_MAX_OUTPUTS],*m_outText[CONVERTER_MAX_OUTPUTS];
   wxButton               *m_cmpButton[MAX_NUM_IOS][CONVERTER_MAX_ASSIGNMENTS];

   DECLARE_EVENT_TABLE()
};

#endif
