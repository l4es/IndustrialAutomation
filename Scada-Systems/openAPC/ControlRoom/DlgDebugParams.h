#ifndef DlgDebugParams_H
#define DlgDebugParams_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class DlgDebugParams : public wxDialog  
{
public:
	DlgDebugParams(wxUint32 flags,wxWindow* parent, const wxString& title, const wxPoint& pos = wxDefaultPosition,const wxString& name = _T(""));
	virtual ~DlgDebugParams();

    wxUint32 flags;

    DECLARE_CLASS(DlgDebugParams)

private:
    void                 OnButtonClick(wxCommandEvent &event);
    void                 OnCheckBoxCheck(wxCommandEvent &event);
    void                 updateUI();

    wxCheckBox          *stopOnThreadID,*stopOnIllegalIO,*stopOnRunTimeout,*stopOnExtLibError,*stopOnDataConvError,*stopOnIncompData;

    DECLARE_EVENT_TABLE()

};

#endif
