#ifndef flowLogicNumShift_H
#define flowLogicNumShift_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include "flowConnection.h"
#include "flowLogic.h"



class flowLogicNumShift : public flowLogic
{
public:
   flowLogicNumShift();
   virtual ~flowLogicNumShift();
   virtual wxString  getDefaultName();
#ifndef ENV_EDITOR
   virtual wxUint64  getAssignedOutput(wxUint64 input);
   virtual wxByte    setDigitalInput(FlowConnection *connection,wxByte value,wxUint32 *flowThreadID,flowObject *object);
   virtual wxByte    setNumInput(FlowConnection *connection,wxFloat64 value,wxUint32 *flowThreadID,flowObject *object);
   virtual wxFloat64 getNumOutput(FlowConnection *connection,wxInt32 *rcode,wxUint64 lastInput,wxLongLong origCreationTime);
#endif

protected:

private:
#ifdef ENV_PLAYER
   wxFloat64 num[MAX_NUM_IOS];
#endif
};

#endif
