#ifndef flowLogicCharShift_H
#define flowLogicCharShift_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include "flowConnection.h"
#include "flowLogic.h"



class flowLogicCharShift : public flowLogic
{
public:
   flowLogicCharShift();
   virtual ~flowLogicCharShift();
   virtual wxString  getDefaultName();
#ifndef ENV_EDITOR
   virtual wxUint64  getAssignedOutput(wxUint64 input);
   virtual wxByte    setDigitalInput(FlowConnection *connection,wxByte value,wxUint32 *flowThreadID,flowObject *object);
   virtual wxByte    setCharInput(FlowConnection *connection,wxString value,wxUint32 *flowThreadID,flowObject *object);
   virtual wxString  getCharOutput(FlowConnection *connection,wxInt32 *rcode,wxUint64 lastInput);
#endif

protected:

private:
#ifdef ENV_PLAYER
   wxString txt[MAX_NUM_IOS];
#endif
};

#endif
