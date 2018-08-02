#ifndef HMIHGAUGE_H
#define HMIHGAUGE_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "hmiHSlider.h"

class hmiHGauge : public hmiHSlider
{
public:
	hmiHGauge(BasePanel *parent,flowObject *obj);
	virtual ~hmiHGauge();
#ifndef ENV_PLAYER
    virtual wxString  getDefaultName();
    virtual wxPoint   getDefSize();
#endif
    virtual wxWindow *createUIElement();

protected:
#ifdef ENV_EDITOR
    virtual wxInt32   save(wxFile *FHandle);
#endif
    virtual void      doApplyData(const wxByte all);

#ifdef ENV_PLAYER
    virtual wxByte    setDigitalInput(FlowConnection *connection,wxByte value,wxUint32 *flowThreadID,flowObject *object);
    virtual wxByte    setNumInput(FlowConnection *connection,wxFloat64 value,wxUint32 *flowThreadID,flowObject *object);
    virtual wxFloat64 getNumOutput(FlowConnection *connection,wxInt32 *rcode,wxUint64 lastInput,wxLongLong origCreationTime);
#endif

};

#endif
