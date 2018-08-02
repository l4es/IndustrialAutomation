#ifndef DlgConfigflowName_H
#define DlgConfigflowName_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <wx/wx.h>
#include <wx/bookctrl.h>

#include "DlgConfigflowBase.h"
#include "oapcNoSpaceCtrl.h"

class flowObject;

class DlgConfigflowName : public DlgConfigflowBase  
{
public:
	DlgConfigflowName(flowObject *object,wxWindow* parent, const wxString& title,bool hideISConfig);
	virtual ~DlgConfigflowName();

   wxByte   returnOK;

protected:
   void             handleStdElements();

   wxBookCtrl      *m_book;
   wxBoxSizer      *m_bSizer;
   oapcNoSpaceCtrl *m_nameField;

private:
   void             createStdElements();
   void             OnButtonClick(wxCommandEvent &event);
   void             updateUI();

   DECLARE_EVENT_TABLE()
};

#endif
