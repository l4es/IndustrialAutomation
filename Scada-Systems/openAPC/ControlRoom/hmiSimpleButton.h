#ifndef HMISIMPLEBUTTON_H
#define HMISIMPLEBUTTON_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class hmiSimpleButton:public hmiObject
{
public:
	hmiSimpleButton(BasePanel *parent,flowObject *obj);
	virtual ~hmiSimpleButton();

   virtual wxWindow *createUIElement();
#ifdef ENV_EDITOR
   virtual wxString  getDefaultName();
#else
   virtual wxByte    getDigitalOutput(FlowConnection *connection,wxInt32 *rcode,wxUint64 lastInput,wxLongLong origCreationTime);
   virtual wxByte    setDigitalInput(FlowConnection *connection,wxByte value,wxUint32 *flowThreadID,flowObject *object);
   virtual wxByte    setCharInput(FlowConnection *connection,wxString value,wxUint32 *flowThreadID,flowObject *object);
   virtual wxUint64  getAssignedOutput(wxUint64 input);
#endif

protected:
#ifdef ENV_EDITOR
   virtual wxInt32    save(wxFile *FHandle);
#endif
   virtual void       doApplyData(const wxByte all);
   virtual wxInt32    load(wxFile *FHandle,wxUint32 chunkSize,wxUint32 IDOffset);
};

#endif
