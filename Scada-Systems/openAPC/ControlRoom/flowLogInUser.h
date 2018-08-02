#ifndef flowLogInUser_H
#define flowLogInUser_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "flowConnection.h"

#ifdef ENV_WINDOWS
#pragma warning (disable: 4018)
#pragma warning (disable: 4663)
#endif

#include <queue>

class flowLogInUser : public flowObject
{
public:
   flowLogInUser();
   virtual ~flowLogInUser();
   virtual wxString  getDefaultName();
#ifdef ENV_PLAYER
   virtual wxUint64 getAssignedOutput(wxUint64 input);
   virtual wxByte   setCharInput(FlowConnection *connection,wxString value,wxUint32 *flowThreadID,flowObject *object);
   virtual wxString getCharOutput(FlowConnection *connection,wxInt32 *rcode,wxUint64 lastInput);
   virtual wxByte   setDigitalInput(FlowConnection *connection,wxByte value,wxUint32 *flowThreadID,flowObject *object);
   virtual wxByte   getDigitalOutput(FlowConnection *connection,wxInt32 *rcode,wxUint64 lastInput,wxLongLong origCreationTime);
#endif

protected:

private:
#ifdef ENV_PLAYER
#ifndef ENV_HPLAYER
   UserData *m_userData;
#endif
   bool      m_loggedIn;
#endif
};

#endif
