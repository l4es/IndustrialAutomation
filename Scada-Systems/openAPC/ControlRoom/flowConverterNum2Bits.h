#ifndef FLOWCONVERTERNum2Bits_H
#define FLOWCONVERTERNum2Bits_H

#pragma pack(8)

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include "flowConnection.h"
#include "flowConverter.h"



class flowConverterNum2Bits : public flowConverter
{
public:
   flowConverterNum2Bits();
   virtual ~flowConverterNum2Bits();
#ifdef ENV_EDITOR
   virtual wxString  getDefaultName();
#else
   virtual wxUint64  getAssignedOutput(wxUint64 input);
   virtual wxByte    getDigitalOutput(FlowConnection *connection,wxInt32 *rcode,wxUint64 lastInput,wxLongLong origCreationTime);
   virtual wxByte    setNumInput(FlowConnection *connection,wxFloat64 value,wxUint32 *flowThreadID,flowObject *object);
   virtual wxFloat64 getNumOutput(FlowConnection *connection,wxInt32 *rcode,wxUint64 lastInput,wxLongLong origCreationTime);
#endif

private:
#ifdef ENV_PLAYER
   wxFloat64         num[MAX_NUM_IOS];
#endif
};

#endif
