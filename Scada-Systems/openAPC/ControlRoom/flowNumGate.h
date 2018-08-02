#ifndef flowNumGate_H
#define flowNumGate_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include "flowConnection.h"
#include "flowLogic.h"
#include "flowNumTrigGate.h"


class flowNumGate : public flowNumTrigGate
{
public:
   flowNumGate();
   virtual ~flowNumGate();
   virtual wxString  getDefaultName();
#ifdef ENV_PLAYER
   virtual wxUint64  getAssignedOutput(wxUint64 input);
   virtual wxByte    setNumInput(FlowConnection *connection,wxFloat64 value,wxUint32 *flowThreadID,flowObject *object);
   virtual wxFloat64 getNumOutput(FlowConnection *connection,wxInt32 *rcode,wxUint64 lastInput,wxLongLong origCreationTime);
#endif

protected:
#ifdef ENV_PLAYER
   wxFloat32 num[MAX_NUM_IOS];
   bool      m_numValid[MAX_NUM_IOS];
#endif
};

#endif
