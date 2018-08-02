#ifndef HMITEXTFIELD_H
#define HMITEXTFIELD_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000



class hmiTextField : public hmiNumField
{
public:
   hmiTextField(BasePanel *parent,flowObject *obj,wxInt32 type);
   virtual ~hmiTextField();

   virtual wxWindow *createUIElement();

#ifdef ENV_EDITOR
   virtual wxString  getDefaultName();
   virtual void      doDataFlowDialog(bool hideISConfig);
#else
   virtual bool      valueHasChanged();
   virtual wxByte    setCharInput(FlowConnection *connection,wxString value,wxUint32 *flowThreadID,flowObject *object);
   virtual wxString  getCharOutput(FlowConnection *connection,wxInt32 *rcode,wxUint64 lastInput);
   virtual wxByte    setDigitalInput(FlowConnection *connection,wxByte value,wxUint32 *flowThreadID,flowObject *object);
#endif

protected:
#ifdef ENV_EDITOR
   virtual wxInt32   save(wxFile *FHandle);
#endif
   virtual void      doApplyData(const wxByte all);


private:
   wxString          txt[MAX_NUM_IOS];
   wxString          m_lastVal;
};

#endif
