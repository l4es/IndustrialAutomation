#ifndef flowConverterPair2Digi_H
#define flowConverterPair2Digi_H

#pragma pack(8)

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include "flowConnection.h"
#include "flowConverter.h"
#include "flowConverterPair2.h"



class flowConverterPair2Digi : public flowConverterPair2
{
public:
   flowConverterPair2Digi();
   virtual ~flowConverterPair2Digi();
   virtual wxString  getDefaultName();
#ifdef ENV_PLAYER
   virtual wxUint64  getAssignedOutput(wxUint64 input);
   virtual wxByte    setDigitalInput(FlowConnection *connection,wxByte value,wxUint32 *flowThreadID,flowObject *object);
   virtual wxByte    getDigitalOutput(FlowConnection *connection,wxInt32 *rcode,wxUint64 lastInput,wxLongLong origCreationTime);
#endif

private:
#ifdef ENV_PLAYER
   std::queue<wxByte> m_digiQ;
#endif
};

#endif
