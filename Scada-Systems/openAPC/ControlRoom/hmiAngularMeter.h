#ifndef hmiAngularMeter_H
#define hmiAngularMeter_H

#pragma pack(8)

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <wx/spinctrl.h>

#include "kwxCtrl/AngularMeter.h"

struct hmiAngularMeterData
{
   wxInt32     numSectors,numTicks;
   wxInt32     startAngle,endAngle; // 1/10 degrees
   wxUint32    reserved1,reserved2;
   wxUint32    sectorColour[2][MAX_NUM_ANGULARMETER_SECTORS];
};



class hmiAngularMeter : public hmiHGauge
{
public:
	hmiAngularMeter(BasePanel *parent,flowObject *obj);
	virtual ~hmiAngularMeter();

#ifdef ENV_EDITOR
    virtual wxString  getDefaultName();
    virtual wxPanel  *getConfigPanel(wxString *name,wxWindow *parent,wxInt32 num);
    virtual void      setConfigData();
    virtual wxPoint   getMinSize();
    virtual void      setData(flowObject *object);

#else
#endif
    virtual wxWindow *createUIElement();
    virtual void      doApplyData(const wxByte all);

protected:
#ifdef ENV_EDITOR
    virtual wxInt32   save(wxFile *FHandle);
#endif
    virtual wxInt32   load(wxFile *FHandle,wxUint32 chunkSize,wxUint32 IDOffset);

private:
#ifdef ENV_EDITOR
    void              OnButtonClick(wxCommandEvent &event);

    wxButton         *bgLow[MAX_NUM_ANGULARMETER_SECTORS],*bgHigh[MAX_NUM_ANGULARMETER_SECTORS];
    wxSpinCtrl       *m_numSectors,*m_numTicks;
    wxTextCtrl       *m_sAngle,*m_eAngle;
    wxStaticText     *bgText[MAX_NUM_ANGULARMETER_SECTORS];
#endif

   struct hmiAngularMeterData meterData;
};

#endif
