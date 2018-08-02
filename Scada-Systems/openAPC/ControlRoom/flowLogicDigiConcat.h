#ifndef flowLogicDigiConcat_H
#define flowLogicDigiConcat_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include "flowConnection.h"
#include "flowLogic.h"



class flowLogicDigiConcat : public flowLogic
{
public:
   flowLogicDigiConcat(wxUint32 type);
   virtual ~flowLogicDigiConcat();
   virtual wxString  getDefaultName();
#ifdef ENV_EDITOR
   virtual void      doDataFlowDialog(bool hideISConfig);
#else
   virtual wxUint64  getAssignedOutput(wxUint64 input);
   virtual wxByte    getDigitalOutput(FlowConnection *connection,wxInt32 *rcode,wxUint64 lastInput,wxLongLong origCreationTime);
#endif

protected:

private:
};

#endif
