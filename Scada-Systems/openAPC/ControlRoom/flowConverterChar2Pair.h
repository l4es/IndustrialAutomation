#ifndef flowConverterChar2Pair_H
#define flowConverterChar2Pair_H

#pragma pack(8)

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include "flowConnection.h"
#include "flowConverter.h"
#include "flowConverter2Pair.h"



class flowConverterChar2Pair : public flowConverter2Pair
{
public:
   flowConverterChar2Pair();
   virtual ~flowConverterChar2Pair();
   virtual wxString  getDefaultName();
#ifdef ENV_PLAYER
   virtual wxUint64  getAssignedOutput(wxUint64 input);
   virtual wxByte    setCharInput(FlowConnection *connection,wxString value,wxUint32 *flowThreadID,flowObject *object);
   virtual wxString  getCharOutput(FlowConnection *connection,wxInt32 *rcode,wxUint64 lastInput);
#endif

private:
#ifdef ENV_PLAYER
   wxString          txt[MAX_NUM_IOS];
#endif
};

#endif
