#ifndef flowConverterDigi2Pair_H
#define flowConverterDigi2Pair_H

#pragma pack(8)

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include "flowConnection.h"
#include "flowConverter.h"
#include "flowConverter2Pair.h"



class flowConverterDigi2Pair : public flowConverter2Pair
{
public:
   flowConverterDigi2Pair();
   virtual ~flowConverterDigi2Pair();
   virtual wxString  getDefaultName();
#ifdef ENV_PLAYER
   virtual wxUint64  getAssignedOutput(wxUint64 input);
   virtual wxByte    setDigitalInput(FlowConnection *connection,wxByte value,wxUint32 *flowThreadID,flowObject *object);
   virtual wxByte    getDigitalOutput(FlowConnection *connection,wxInt32 *rcode,wxUint64 lastInput,wxLongLong origCreationTime);
#endif

private:
};

#endif
