#ifndef DlgConfigflowTimer_H
#define DlgConfigflowTimer_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <wx/spinctrl.h>

#include "flowTimer.h"
#include "DlgConfigflowName.h"


class flowConverterDigi2Num;

class DlgConfigflowTimer : public DlgConfigflowName  
{
public:
	DlgConfigflowTimer(flowTimer *object,wxWindow* parent, const wxString& title,bool hideISConfig);
	virtual ~DlgConfigflowTimer();

   DECLARE_CLASS(DlgConfigflowTimer)

private:
   void                 OnButtonClick(wxCommandEvent &event);

   flowTimer           *m_object;
   wxSpinCtrl          *m_digiField[2],*m_timeField;
   wxTextCtrl          *m_numField[2],*m_textField[2];

   DECLARE_EVENT_TABLE()
};

#endif
