#ifndef flowLogicDigiTFF_H
#define flowLogicDigiTFF_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include "flowConnection.h"
#include "flowLogic.h"


class flowLogicDigiTFF : public flowLogic
{
public:
   flowLogicDigiTFF();
   virtual ~flowLogicDigiTFF();
   virtual wxString getDefaultName();
#ifdef ENV_EDITOR
   virtual wxInt32  saveDATA(wxFile *FHandle,char chunkName[4],bool isCompiled);
#endif
   virtual wxInt32  loadDATA(wxFile *FHandle,wxUint32 chunkSize,wxUint32 IDOffset,bool isCompiled);
#ifdef ENV_PLAYER
   virtual wxUint64 getAssignedOutput(wxUint64 input);
   virtual wxByte   setDigitalInput(FlowConnection *connection,wxByte value,wxUint32 *flowThreadID,flowObject *object);
#endif

private:
   bool digiChanged[MAX_NUM_IOS/2];
};

#endif
