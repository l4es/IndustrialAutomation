#ifndef flowStop_H
#define flowStop_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include "flowConnection.h"



class flowStop : public flowObject
{
public:
   flowStop();
   virtual ~flowStop();
#ifdef ENV_EDITOR
   virtual wxString getDefaultName();
#else
   virtual wxByte   setDigitalInput(FlowConnection *connection,wxByte value,wxUint32 *flowThreadID,flowObject *object);
#endif

protected:

private:
};

#endif
