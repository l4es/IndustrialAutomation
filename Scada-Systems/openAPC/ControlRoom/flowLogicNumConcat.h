#ifndef flowLogicNumConcat_H
#define flowLogicNumConcat_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include "flowConnection.h"
#include "flowLogic.h"



class flowLogicNumConcat : public flowLogic
{
public:
   flowLogicNumConcat(wxUint32 type);
   virtual ~flowLogicNumConcat();
   virtual wxString  getDefaultName();
#ifdef ENV_EDITOR
   virtual void      doDataFlowDialog(bool hideISConfig);
#else
   virtual wxUint64  getAssignedOutput(wxUint64 input);
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
