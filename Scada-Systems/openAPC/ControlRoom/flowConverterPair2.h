#ifndef flowConverterPair2_H
#define flowConverterPair2_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include "flowConnection.h"
#include "flowConverter2Pair.h"

class flowConverterPair2 : public flowConverter2Pair
{
public:
#ifdef ENV_EDITOR
   virtual void     doDataFlowDialog(bool hideISConfig);
#else
   wxByte           setCharInput(FlowConnection *connection,wxString value,wxUint32 *flowThreadID,flowObject *object);

#endif
protected:
   wxString         cmd;
};

#endif
