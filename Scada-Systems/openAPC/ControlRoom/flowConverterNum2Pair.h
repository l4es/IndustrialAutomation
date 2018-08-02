#ifndef flowConverterNum2Pair_H
#define flowConverterNum2Pair_H

#pragma pack(8)

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include "flowConnection.h"
#include "flowConverter.h"
#include "flowConverter2Pair.h"



class flowConverterNum2Pair : public flowConverter2Pair
{
public:
   flowConverterNum2Pair();
   virtual ~flowConverterNum2Pair();
   virtual wxString  getDefaultName();
#ifdef ENV_PLAYER
   virtual wxUint64  getAssignedOutput(wxUint64 input);
   virtual wxByte    setNumInput(FlowConnection *connection,wxFloat64 value,wxUint32 *flowThreadID,flowObject *object);
   virtual wxFloat64 getNumOutput(FlowConnection *connection,wxInt32 *rcode,wxUint64 lastInput,wxLongLong origCreationTime);
#endif

private:
   wxFloat64 num[MAX_NUM_IOS];
};

#endif
