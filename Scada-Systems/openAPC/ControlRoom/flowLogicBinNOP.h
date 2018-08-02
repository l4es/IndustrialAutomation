#ifndef flowLogicBinNOP_H
#define flowLogicBinNOP_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include "flowConnection.h"
#include "flowLogic.h"


class flowLogicBinNOP : public flowLogic
{
public:
   flowLogicBinNOP();
   virtual ~flowLogicBinNOP();
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
   oapcBinHeadSp bin[MAX_NUM_IOS];
#endif
};

#endif
