#ifndef flowConverterPair2Bin_H
#define flowConverterPair2Bin_H

#pragma pack(8)

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "oapc_libio.h"
#include "flowConnection.h"
#include "flowConverter.h"
#include "flowConverterPair2.h"



class flowConverterPair2Bin : public flowConverterPair2
{
public:
   flowConverterPair2Bin();
   virtual ~flowConverterPair2Bin();
   virtual wxString       getDefaultName();
#ifdef ENV_PLAYER
   virtual wxUint64       getAssignedOutput(wxUint64 input);
   virtual wxByte         setBinInput(FlowConnection *connection,oapcBinHeadSp &value,wxUint32 *flowThreadID,flowObject *WXUNUSED(object));
   virtual oapcBinHeadSp getBinOutput(FlowConnection *connection,wxInt32 *rcode,wxUint64 WXUNUSED(lastInput));
#endif

private:
#ifdef ENV_PLAYER
   std::queue<oapcBinHeadSp> m_binQ;
#endif
};

#endif
