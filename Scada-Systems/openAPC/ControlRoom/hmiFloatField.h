#ifndef HMIFLOATFIELD_H
#define HMIFLOATFIELD_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <wx/spinctrl.h>
#include "hmiNumField.h"

class hmiFloatField : public hmiNumField
{
public:
   hmiFloatField(BasePanel *parent,flowObject *obj);
   virtual ~hmiFloatField();

#ifndef ENV_PLAYER
   virtual wxString  getDefaultName();
#endif
   virtual wxWindow *createUIElement();

protected:
#ifdef ENV_EDITOR
   virtual wxInt32   save(wxFile *FHandle);
#endif
   virtual void      doApplyData(const wxByte all);

#ifdef ENV_PLAYER
   virtual wxFloat64 getNumOutput(FlowConnection *connection,wxInt32 *rcode,wxUint64 lastInput,wxLongLong origCreationTime);
   virtual bool      valueHasChanged();
#endif

private:
#ifdef ENV_PLAYER
	wxFloat64        m_lastVal;
#else
	virtual wxPanel* getConfigPanel(wxString *name,wxWindow *parent,wxInt32 num);
	virtual void     setConfigData();

	wxSpinCtrl      *m_numDecPlaces;
#endif
};

#endif
