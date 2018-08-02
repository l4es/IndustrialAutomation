#ifndef flowLogicBinShift_H
#define flowLogicBinShift_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include "flowConnection.h"
#include "flowLogic.h"



class flowLogicBinShift : public flowLogic
{
public:
   flowLogicBinShift();
   virtual ~flowLogicBinShift();
   virtual wxString       getDefaultName();
#ifndef ENV_EDITOR
   virtual wxUint64       getAssignedOutput(wxUint64 input);
   virtual wxByte         setDigitalInput(FlowConnection *connection,wxByte value,wxUint32 *flowThreadID,flowObject *object);
   virtual wxByte         setBinInput(FlowConnection *connection,oapcBinHeadSp &value,wxUint32 *flowThreadID,flowObject *object);
   virtual oapcBinHeadSp getBinOutput(FlowConnection *connection,wxInt32 *rcode,wxUint64 lastInput);
#endif

protected:

private:
#ifdef ENV_PLAYER
   oapcBinHeadSp bin[MAX_NUM_IOS];
#endif
};

#endif
