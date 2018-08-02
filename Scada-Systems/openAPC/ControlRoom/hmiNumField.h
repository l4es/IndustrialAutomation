#ifndef HMINUMFIELD_H
#define HMINUMFIELD_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "hmiHSlider.h"

class hmiNumField : public hmiHSlider
{
public:
   hmiNumField(BasePanel *parent,flowObject *obj);
   virtual ~hmiNumField();

#ifndef ENV_PLAYER
   virtual wxString  getDefaultName();
#else
   virtual wxFloat64 getNumOutput(FlowConnection *connection,wxInt32 *rcode,wxUint64 lastInput,wxLongLong origCreationTime);
   virtual bool                           valueHasChanged();
#endif
   virtual wxWindow *createUIElement();
   virtual void      doApplyData(const wxByte all);

protected:
#ifdef ENV_EDITOR
   virtual wxInt32   save(wxFile *FHandle);
#endif
   wxInt32           m_lastVal;
};

#endif
