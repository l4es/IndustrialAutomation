#ifndef flowConverterPair2Char_H
#define flowConverterPair2Char_H

#pragma pack(8)

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include "flowConnection.h"
#include "flowConverter.h"
#include "flowConverterPair2.h"



class flowConverterPair2Char : public flowConverterPair2
{
public:
   flowConverterPair2Char();
   virtual ~flowConverterPair2Char();
   virtual wxString  getDefaultName();
#ifdef ENV_PLAYER
   virtual wxUint64  getAssignedOutput(wxUint64 input);
   virtual wxByte    setCharInput(FlowConnection *connection,wxString value,wxUint32 *flowThreadID,flowObject *object);
   virtual wxString  getCharOutput(FlowConnection *connection,wxInt32 *rcode,wxUint64 lastInput);
#endif

private:
#ifdef ENV_PLAYER
   std::queue<wxString> m_charQ;
#endif
};

#endif
