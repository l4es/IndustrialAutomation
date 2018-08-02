#ifndef flowDigiGate_H
#define flowDigiGate_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include "flowConnection.h"
#include "flowLogic.h"


class flowDigiGate : public flowDigiTrigGate
{
public:
   flowDigiGate();
   virtual ~flowDigiGate();
   virtual wxString              getDefaultName();
#ifdef ENV_PLAYER
   virtual wxUint64              getAssignedOutput(wxUint64 input);
   virtual wxByte                setDigitalInput(FlowConnection *connection,wxByte value,wxUint32 *flowThreadID,flowObject *object);
   virtual wxByte                getDigitalOutput(FlowConnection *connection,wxInt32 *rcode,wxUint64 lastInput,wxLongLong origCreationTime);
#endif

private:
#ifdef ENV_PLAYER
   bool                  m_digiValid[MAX_NUM_IOS];
#endif
};

#endif
