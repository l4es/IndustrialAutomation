#ifndef flowConverterBin2Pair_H
#define flowConverterBin2Pair_H

#pragma pack(8)

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include "flowConnection.h"
#include "flowConverter.h"
#include "flowConverter2Pair.h"
#include "oapc_libio.h"


class flowConverterBin2Pair : public flowConverter2Pair
{
public:
   flowConverterBin2Pair();
   virtual ~flowConverterBin2Pair();
   virtual wxString       getDefaultName();
#ifdef ENV_PLAYER
   virtual wxUint64       getAssignedOutput(wxUint64 input);
   virtual wxByte         setBinInput(FlowConnection *connection,oapcBinHeadSp &value,wxUint32 *flowThreadID,flowObject *object);
   virtual oapcBinHeadSp getBinOutput(FlowConnection *connection,wxInt32 *rcode,wxUint64 lastInput);
#endif

private:
#ifndef ENV_EDITOR
   oapcBinHeadSp bin[MAX_NUM_IOS];
#endif
};

#endif
