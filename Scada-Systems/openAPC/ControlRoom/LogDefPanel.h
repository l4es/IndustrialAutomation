#ifndef LogDefPanel_H
#define LogDefPanel_H

#include <wx/dialog.h>
#include <wx/bookctrl.h>

#include "oapcFloatCtrl.h"
#include "hmiObject.h"
#include "XMLDecoder.h"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class LogDefPanel:public wxPanel
{
public:
   LogDefPanel(struct hmiObjectLogData logData[MAX_LOG_TYPES],wxUint32 logFlags,wxWindow* parent);
	virtual ~LogDefPanel();

           void getConfigData(struct hmiObjectLogData logData[MAX_LOG_TYPES]);

   DECLARE_CLASS(LogDefPanel)

private:
    void        OnNumberChanged(wxCommandEvent &event);
    void        OnNumberFocusChanged(wxFocusEvent &event);
    void        OnCheckBoxCheck(wxCommandEvent &event);
    void        updateUI();

    wxCheckBox          *disabledLow,*disabledHigh,*roLow,*roHigh;
    wxCheckBox          *useAboveValue[MAX_LOG_TYPES],*useBetweenValue[MAX_LOG_TYPES],*useBelowValue[MAX_LOG_TYPES],
                        *useChangeValue[MAX_LOG_TYPES],*useHiValue[MAX_LOG_TYPES],*useLoValue[MAX_LOG_TYPES];
    wxSpinCtrl          *aboveIntField[MAX_LOG_TYPES],*fromIntField[MAX_LOG_TYPES],*toIntField[MAX_LOG_TYPES],
                        *belowIntField[MAX_LOG_TYPES];
    oapcFloatCtrl         *aboveFloatField[MAX_LOG_TYPES],*fromFloatField[MAX_LOG_TYPES],*toFloatField[MAX_LOG_TYPES],
                        *belowFloatField[MAX_LOG_TYPES];
    wxSpinCtrl          *posX,*posY,*sizeX,*sizeY;

    DECLARE_EVENT_TABLE()

};

#endif // !defined(AFX_LogDefPanel_H__2C31260D_1348_4C26_981D_0A86D15A00FB__INCLUDED_)
