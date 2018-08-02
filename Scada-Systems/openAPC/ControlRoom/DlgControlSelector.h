
#ifndef DLGCONTROLSELECTOR_H
#define DLGCONTROLSELECTOR_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "hmiObject.h"

#define DLGCONTROLSELECTOR_FLAG_SHOWFLOWSTATE 0x0001
#define DLGCONTROLSELECTOR_FLAG_SHOWALL       0x0002
#ifdef ENV_EDITOR
 #define DLGCONTROLSELECTOR_FLAG_ALLOWSORT    0x0004
#endif

class DlgControlSelector : public wxDialog  
{
public:
	DlgControlSelector(wxWindow* parent, const wxString& title,wxUint32 flags);
	virtual ~DlgControlSelector();

    flowObject *getSelectedControl();

    DECLARE_CLASS(DlgControlSelector)

private:
    void OnButtonClick(wxCommandEvent &event);
    void OnDoubleClick(wxCommandEvent &event);
#ifdef ENV_EDITOR
    void updateUI();
    void changePos(wxInt32 change);

          wxBitmapButton *m_upBtn,*m_downBtn;
#endif

          flowObject     *selectedObject;
          wxListBox      *m_list;
          wxArrayString   m_items;
    const wxUint32        m_flags;

    DECLARE_EVENT_TABLE()
};

#endif
