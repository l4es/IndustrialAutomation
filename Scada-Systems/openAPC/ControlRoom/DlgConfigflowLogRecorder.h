#ifndef DlgConfigflowLogRecorder_H
#define DlgConfigflowLogRecorder_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <wx/spinctrl.h>

#include "LogDefPanel.h"
#include "DlgConfigflowName.h"

class flowLogRecorder;

class DlgConfigflowLogRecorder : public DlgConfigflowName  
{
public:
	DlgConfigflowLogRecorder(flowLogRecorder *object,wxWindow* parent, const wxString& title,bool hideISConfig);
	virtual ~DlgConfigflowLogRecorder();

   DECLARE_CLASS(DlgConfigflowLogRecorder)

private:
   void                 OnButtonClick(wxCommandEvent &event);

   flowLogRecorder     *m_object;
   LogDefPanel         *logPanel[6];

   DECLARE_EVENT_TABLE()
};

#endif
