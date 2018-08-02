#ifndef HMIHSLIDER_H
#define HMIHSLIDER_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000



class hmiHSlider : public hmiObject
{
public:
	hmiHSlider(BasePanel *parent,flowObject *obj);
	virtual ~hmiHSlider();

#ifndef ENV_PLAYER
    virtual wxString  getDefaultName();
#endif
    virtual wxWindow *createUIElement();

#ifdef ENV_PLAYER
    virtual wxByte    setNumInput(FlowConnection *connection,wxFloat64 value,wxUint32 *flowThreadID,flowObject *object);
    virtual wxFloat64 getNumOutput(FlowConnection *connection,wxInt32 *rcode,wxUint64 lastInput,wxLongLong origCreationTime);
    virtual wxByte    setDigitalInput(FlowConnection *connection,wxByte value,wxUint32 *flowThreadID,flowObject *object);
    virtual wxUint64  getAssignedOutput(wxUint64 input);

//    wxLongLong        lastThreadTime; // this is used to ensure threads are created not to fast for this object; it has to be used for permanently changing events only but not for one-time events
#else
    virtual void      doDataFlowDialog(bool hideISConfig);
#endif

protected:
#ifdef ENV_EDITOR
    virtual wxInt32     save(wxFile *FHandle);
#endif
    virtual void        doApplyData(const wxByte all);

    wxFloat64           num;
};

#endif
