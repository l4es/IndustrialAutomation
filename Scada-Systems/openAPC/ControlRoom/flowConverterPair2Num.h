#ifndef flowConverterPair2Num_H
#define flowConverterPair2Num_H

#pragma pack(8)

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include "flowConnection.h"
#include "flowConverter.h"
#include "flowConverterPair2.h"



class flowConverterPair2Num : public flowConverterPair2
{
public:
   flowConverterPair2Num();
   virtual ~flowConverterPair2Num();
   virtual wxString  getDefaultName();
#ifdef ENV_PLAYER
   virtual wxUint64  getAssignedOutput(wxUint64 input);
   virtual wxByte    setNumInput(FlowConnection *connection,wxFloat64 value,wxUint32 *flowThreadID,flowObject *object);
   virtual wxFloat64 getNumOutput(FlowConnection *connection,wxInt32 *rcode,wxUint64 lastInput,wxLongLong origCreationTime);
#endif

private:
#ifdef ENV_PLAYER
   std::queue<wxFloat32> m_numQ;
#endif
};

#endif
