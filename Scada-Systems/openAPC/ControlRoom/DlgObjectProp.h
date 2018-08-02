#ifndef DLGOBJECTPROP_H
#define DLGOBJECTPROP_H

#include <wx/dialog.h>
#include <wx/bookctrl.h>

#include "oapcFloatCtrl.h"
#include "hmiObject.h"
#include "XMLDecoder.h"
#include "LogDefPanel.h"
#include "PrivilegePanel.h"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class DlgObjectProp:public wxDialog
{
public:
    DlgObjectProp(hmiObject *object,wxWindow* parent,const wxString& title, const wxPoint& pos = wxDefaultPosition,const wxString& name = _T(""));
	virtual ~DlgObjectProp();

    DECLARE_CLASS(DlgObjectProp)

private:
    void        OnButtonClick(wxCommandEvent &event);
    void        OnNumberChanged(wxCommandEvent &event);
    void        OnNumberFocusChanged(wxFocusEvent &event);
    void        OnSpinEvent(wxSpinEvent &event);
    void        updateUI();

    wxBookCtrl          *book;
    LogDefPanel         *logPanel;
    PrivilegePanel      *priviPanel;
    wxTextCtrl          *textLow,*textHigh;
    oapcFloatCtrl       *minField,*maxField;
    wxButton            *bgLow,*bgHigh,*fgLow,*fgHigh;
    wxButton            *fontLow,*fontHigh;
    wxComboBox          *stateHigh,*stateLow;
    hmiObject           *object;
    oapcNoSpaceCtrl     *nameField;
    wxTextCtrl          *textLowField,*textHighField;
    wxCheckBox          *disabledLow,*disabledHigh,*roLow,*roHigh;
    wxStaticText        *m_absPosText,*m_absSizeText;
/*    wxCheckBox          *useAboveValue[MAX_LOG_TYPES],*useBetweenValue[MAX_LOG_TYPES],*useBelowValue[MAX_LOG_TYPES],
                        *useChangeValue[MAX_LOG_TYPES],*useHiValue[MAX_LOG_TYPES],*useLoValue[MAX_LOG_TYPES];
    wxSpinCtrl          *aboveIntField[MAX_LOG_TYPES],*fromIntField[MAX_LOG_TYPES],*toIntField[MAX_LOG_TYPES],
                        *belowIntField[MAX_LOG_TYPES];
    oapcFloatCtrl         *aboveFloatField[MAX_LOG_TYPES],*fromFloatField[MAX_LOG_TYPES],*toFloatField[MAX_LOG_TYPES],
                        *belowFloatField[MAX_LOG_TYPES];*/
    wxSpinCtrl          *posX,*posY,*sizeX,*sizeY;
    struct hmiObjectData objectData;
    wxFloat32            m_sizeRatio;

    DECLARE_EVENT_TABLE()

};

#endif // !defined(AFX_DLGOBJECTPROP_H__2C31260D_1348_4C26_981D_0A86D15A00FB__INCLUDED_)
