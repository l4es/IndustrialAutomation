#ifndef DlgUsers_H
#define DlgUsers_H

#include <wx/dialog.h>
#include <wx/spinctrl.h>
#include <wx/textctrl.h>
#include <wx/bmpbuttn.h>
#include <wx/stattext.h>

#include "PrivilegePanel.h"

#if _MSC_VER > 1000
#pragma once
#endif

class DlgUsers : public wxDialog  
{
public:
    DlgUsers(wxWindow* parent, const wxString& title);
	virtual ~DlgUsers();

private:
};

#endif
