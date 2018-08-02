#ifndef DlgConfigflowGroup_H
#define DlgConfigflowGroup_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class oapcNoSpaceCtrl;

class DlgConfigflowGroup : public wxDialog  
{
public:
	DlgConfigflowGroup(flowGroup *group,wxWindow* parent, const wxString& title, const wxPoint& pos = wxDefaultPosition,const wxString& name = _T(""));
	virtual ~DlgConfigflowGroup();

   wxByte   returnOK;

   DECLARE_CLASS(DlgConfigflowGroup)

private:
    void                 OnButtonClick(wxCommandEvent &event);
    void                 updateUI();

    flowGroup           *group;
    oapcNoSpaceCtrl     *m_nameField;
    wxComboBox          *m_catComboBox;

    DECLARE_EVENT_TABLE()

};

#endif
