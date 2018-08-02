#ifndef DLGCONFIGFLOWEXTERNALIOLIB_H
#define DLGCONFIGFLOWEXTERNALIOLIB_H

#include <wx/dialog.h>
#include <wx/bookctrl.h>
#include <wx/xml/xml.h>

//#include "hmiObject.h"
#include "XMLDecoder.h"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000



//class flowExternalIOLib;

class DlgConfigflowExternalIOLib:public wxDialog
{
public:
    DlgConfigflowExternalIOLib(wxString *objectName,XMLDecoder *xmlDecoder,wxWindow* parent,wxUint64 stdOUT,wxUint32 *cycleMicros,const wxString& title,bool hideISConfig);
	virtual ~DlgConfigflowExternalIOLib();

    wxByte      getNextPair(wxString *name,wxString *value,wxFont *font);

    bool        returnOK;
    wxUint32    m_cycleMicros;

private:
    void        OnButtonClick(wxCommandEvent &event);

    XMLDecoder *m_xmlDecoder;
};

#endif
