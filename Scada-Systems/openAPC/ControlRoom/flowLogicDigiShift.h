#ifndef flowLogicDigiShift_H
#define flowLogicDigiShift_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include "flowConnection.h"
#include "flowLogic.h"



class flowLogicDigiShift : public flowLogic
{
public:
   flowLogicDigiShift();
   virtual ~flowLogicDigiShift();
   virtual wxString  getDefaultName();
#ifndef ENV_EDITOR
   virtual wxUint64  getAssignedOutput(wxUint64 input);
   virtual wxByte    setDigitalInput(FlowConnection *connection,wxByte value,wxUint32 *flowThreadID,flowObject *object);
#endif

protected:

private:

};

#endif
