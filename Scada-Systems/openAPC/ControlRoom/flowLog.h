#ifndef flowLog_H
#define flowLog_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "flowConnection.h"

#ifdef ENV_WINDOWS
#pragma warning (disable: 4018)
#pragma warning (disable: 4663)
#endif

#include <queue>

class flowLog : public flowObject
{
public:
   flowLog();
   virtual ~flowLog();
   virtual wxString  getDefaultName();
#ifndef ENV_EDITOR
   virtual wxInt32   loadFlow(wxFile *FHandle,struct hmiFlowData *flowData,wxUint32 IDOffset,bool usesOwnHead,bool isCompiled);
           void      setLogInfo(wxString logText,wxInt32 logNum,flowObject *object);
           wxFloat64 getNumOutput(FlowConnection *connection,wxInt32 *rcode,wxUint64 lastInput,wxLongLong origCreationTime);
           wxString  getCharOutput(FlowConnection *connection,wxInt32 *rcode,wxUint64 lastInput);
#endif

protected:

private:
#ifdef ENV_PLAYER
   std::queue<wxString> m_logQ[MAX_LOG_TYPES];
   std::queue<wxUint32> m_idQ[MAX_LOG_TYPES];
   bool                 m_logEnabled[MAX_LOG_TYPES],m_idLogEnabled[MAX_LOG_TYPES];
   wxCriticalSection    m_logSection;
   wxDateTime           m_dateTime;
#endif
};

#endif
