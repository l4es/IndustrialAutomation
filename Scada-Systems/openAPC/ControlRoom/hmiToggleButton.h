#ifndef HMITOGGLEBUTTON_H
#define HMITOGGLEBUTTON_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "hmiSimpleButton.h"

class hmiToggleButton : public hmiSimpleButton
{
public:
	hmiToggleButton(BasePanel *parent,flowObject *obj);
	virtual ~hmiToggleButton();

   virtual wxWindow *createUIElement();
#ifdef ENV_EDITOR
   virtual wxInt32   save(wxFile *FHandle);
   virtual wxString  getDefaultName();
   virtual wxPanel*  getConfigPanel(wxString *name,wxWindow *parent,wxInt32 num);
   virtual void      setConfigData();
   virtual void      OnCombobox(wxCommandEvent &event);

   wxComboBox       *comboBox;

#endif
   virtual void      doApplyData(const wxByte all);
#ifdef ENV_PLAYER
   virtual wxByte    setDigitalInput(FlowConnection *connection,wxByte value,wxUint32 *flowThreadID,flowObject *object);
   virtual wxByte    getDigitalOutput(FlowConnection *connection,wxInt32 *rcode,wxUint64 lastInput,wxLongLong origCreationTime);
   virtual wxUint64  getAssignedOutput(wxUint64 input);
   virtual void      setSelected(bool selected);
   virtual void      updateSelectionState();
#endif

};

#endif
