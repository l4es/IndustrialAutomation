#ifndef DLGCONFIGFLOWCONVERTMixed2Char_H
#define DLGCONFIGFLOWCONVERTMixed2Char_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "flowConverter.h"
#include "oapcNoSpaceCtrl.h"
#include "DlgConfigflowName.h"


class flowConverterMixed2Char;

class DlgConfigflowConvertMixed2Char : public DlgConfigflowName
{
public:
	DlgConfigflowConvertMixed2Char(flowConverterMixed2Char *object,wxWindow* parent, const wxString& title,bool hideISConfig);
	virtual ~DlgConfigflowConvertMixed2Char();

   DECLARE_CLASS(DlgConfigflowConvertMixed2Char)

private:
   void                 OnButtonClick(wxCommandEvent &event);

   flowConverterMixed2Char *m_object;
   wxCheckBox              *m_modeOnClock,*m_modeForceToDot;
   wxTextCtrl              *m_fmtChar;
   DECLARE_EVENT_TABLE()

};

#endif
