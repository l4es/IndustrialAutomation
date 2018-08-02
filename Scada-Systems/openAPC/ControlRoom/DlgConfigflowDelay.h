#ifndef DlgConfigflowDelay_H
#define DlgConfigflowDelay_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <wx/spinctrl.h>

#include "oapcNoSpaceCtrl.h"
#include "flowDelay.h"
#include "DlgConfigflowName.h"


class flowConverterDigi2Num;

class DlgConfigflowDelay : public DlgConfigflowName
{
public:
	DlgConfigflowDelay(flowDelay *object,wxWindow* parent, const wxString& title,bool hideISConfig);
	virtual ~DlgConfigflowDelay();

   DECLARE_CLASS(DlgConfigflowDelay)

private:
    void                 OnButtonClick(wxCommandEvent &event);

    flowDelay           *m_object;
    wxSpinCtrl          *m_delayField[MAX_NUM_IOS];


    DECLARE_EVENT_TABLE()

};

#endif
