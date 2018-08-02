#ifndef flowCharGate_H
#define flowCharGate_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include "flowConnection.h"
#include "flowLogic.h"


class flowCharGate : public flowCharTrigGate
{
public:
   flowCharGate();
   virtual ~flowCharGate();
   virtual wxString  getDefaultName();
#ifdef ENV_PLAYER
   virtual wxUint64  getAssignedOutput(wxUint64 input);
   virtual wxByte    setCharInput(FlowConnection *connection,wxString value,wxUint32 *flowThreadID,flowObject *object);
   virtual wxString  getCharOutput(FlowConnection *connection,wxInt32 *rcode,wxUint64 lastInput);
#endif

private:
#ifdef ENV_PLAYER
   wxString  txt[MAX_NUM_IOS];
   bool      m_txtValid[MAX_NUM_IOS];
#endif
};

#endif
