#ifndef DlgConfigflowLogic_H
#define DlgConfigflowLogic_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "flowLogic.h"
#include "oapcFloatCtrl.h"
#include "DlgConfigflowName.h"

class flowConverterDigi2Num;

class DlgConfigflowLogic : public DlgConfigflowName  
{
public:
	DlgConfigflowLogic(flowLogic *object,wxWindow* parent, const wxString& title,bool hideISConfig);
	virtual ~DlgConfigflowLogic();

   DECLARE_CLASS(DlgConfigflowLogic)

private:
    void                 OnButtonClick(wxCommandEvent &event);
    void                 OnCheckBoxCheck(wxCommandEvent &event);
    void                 updateUI();

    flowLogic           *m_object;
    wxCheckBox          *m_maskBox[MAX_NUM_IOS][MAX_NUM_IOS],
                        *m_modeOnClock[MAX_NUM_IOS],*m_modeInvert[MAX_NUM_IOS],*m_modeAllowLoop[MAX_NUM_IOS];
    oapcFloatCtrl       *m_constNum[MAX_NUM_IOS];
    wxByte               m_isNumericMode;
    bool                 m_isMathMode;


    DECLARE_EVENT_TABLE()

};

#endif
