#ifndef DlgConfigflowStart_H
#define DlgConfigflowStart_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <wx/spinctrl.h>

#include "flowStart.h"
#include "DlgConfigflowName.h"


class flowConverterDigi2Num;

class DlgConfigflowStart : public DlgConfigflowName  
{
public:
	DlgConfigflowStart(flowStart *object,wxWindow* parent, const wxString& title,bool hideISConfig);
	virtual ~DlgConfigflowStart();

   DECLARE_CLASS(DlgConfigflowStart)

private:
   void                 OnButtonClick(wxCommandEvent &event);

   flowStart           *m_object;
   wxSpinCtrl          *m_digiField[2];
   wxTextCtrl          *m_numField[2],*m_textField[2];


   DECLARE_EVENT_TABLE()
};

#endif
