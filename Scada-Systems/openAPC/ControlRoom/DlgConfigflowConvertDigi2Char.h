#ifndef DLGCONFIGFLOWCONVERTDIGI2CHAR_H
#define DLGCONFIGFLOWCONVERTDIGI2CHAR_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "flowConverter.h"
#include "DlgConfigflowName.h"
#include "DlgConfigflowName.h"

class flowConverterDigi2Char;

class DlgConfigflowConvertDigi2Char : public DlgConfigflowName
{
public:
	DlgConfigflowConvertDigi2Char(flowConverterDigi2Char *object,wxWindow* parent, const wxString& title,bool hideISConfig);
	virtual ~DlgConfigflowConvertDigi2Char();

   DECLARE_CLASS(DlgConfigflowConvertDigi2Char)

private:
   void                 OnButtonClick(wxCommandEvent &event);
   void                 OnCheckBoxCheck(wxCommandEvent &event);
   void                 OnRadioButtonCheck(wxCommandEvent &event);
   void                 updateUI();

   flowConverterDigi2Char *m_object;
   wxCheckBox             *m_maskBox[MAX_NUM_IOS][MAX_NUM_IOS],
                          *m_modeOnClock[MAX_NUM_IOS];
   wxRadioButton          *m_modeBinaryRB[MAX_NUM_IOS],*m_modeDirectRB[MAX_NUM_IOS];
   wxTextCtrl             *m_lowValueField[MAX_NUM_IOS][MAX_NUM_IOS],*m_highValueField[MAX_NUM_IOS][MAX_NUM_IOS];
   wxStaticText           *m_ValueText[MAX_NUM_IOS][MAX_NUM_IOS],*m_lowText[MAX_NUM_IOS],*m_highText[MAX_NUM_IOS];

   DECLARE_EVENT_TABLE()

};

#endif
