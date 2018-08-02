#ifndef flowBinTrigGate_H
#define flowBinTrigGate_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include "flowConnection.h"
#include "flowLogic.h"


class flowBinTrigGate : public flowLogic
{
public:
   flowBinTrigGate();
   virtual ~flowBinTrigGate();
   virtual wxString       getDefaultName();
#ifdef ENV_EDITOR
   virtual wxInt32        saveDATA(wxFile *FHandle,char chunkName[4],bool isCompiled);
#endif
   virtual wxInt32        loadDATA(wxFile *FHandle,wxUint32 chunkSize,wxUint32 IDOffset,bool isCompiled);
#ifdef ENV_PLAYER
   virtual wxUint64       getAssignedOutput(wxUint64 input);
   virtual wxByte         setBinInput(FlowConnection *connection,oapcBinHeadSp &value,wxUint32 *flowThreadID,flowObject *object);
   virtual oapcBinHeadSp getBinOutput(FlowConnection *connection,wxInt32 *rcode,wxUint64 lastInput);
#endif

private:
#ifdef ENV_PLAYER
   oapcBinHeadSp bin;
   bool          m_binValid;
#endif
};

#endif
