#ifndef flowBinGate_H
#define flowBinGate_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include "flowConnection.h"
#include "flowLogic.h"


class flowBinGate : public flowBinTrigGate
{
public:
   flowBinGate();
   virtual ~flowBinGate();
   virtual wxString       getDefaultName();
#ifdef ENV_PLAYER
   virtual wxUint64       getAssignedOutput(wxUint64 input);
   virtual wxByte         setBinInput(FlowConnection *connection,oapcBinHeadSp &value,wxUint32 *flowThreadID,flowObject *object);
   virtual oapcBinHeadSp getBinOutput(FlowConnection *connection,wxInt32 *rcode,wxUint64 lastInput);
#endif

private:
#ifdef ENV_PLAYER
   oapcBinHeadSp bin[MAX_NUM_IOS];
   bool          m_binValid[MAX_NUM_IOS];
#endif
};

#endif
