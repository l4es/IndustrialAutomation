#ifndef FLOWWALKERTHREAD_H
#define FLOWWALKERTHREAD_H

#include "oapcBinHead.h"

class FlowWalkerThread
{
public:
	FlowWalkerThread(flowObject *object,FlowConnection *connection,wxUint32 mode,wxLongLong creationTime,wxUint64 lastInput);
	virtual ~FlowWalkerThread();

   virtual wxInt32 execute();

   void          setNextConnection(FlowConnection *connection);
   void          setFlowThreadID(wxUint32 flowThreadID);
   void          setFirstOutValue(wxByte val,flowObject *object);
   void          setFirstOutValue(wxFloat64 val,flowObject *object);
   void          setFirstOutValue(wxString val,flowObject *object);
   void          setFirstOutValue(const oapcBinHeadSp &val,flowObject *object);
   void          setMode(wxUint32 mode);

   static const wxUint32 THREAD_MODE_DIGI;
   static const wxUint32 THREAD_MODE_NUM;
   static const wxUint32 THREAD_MODE_CHAR;
   static const wxUint32 THREAD_MODE_BIN;

   wxLongLong            creationTime;
   flowObject           *object;
   FlowConnection       *connection;

protected:
   wxUint32              flowThreadID,mode;
   wxByte                digiValue;
   wxFloat64             numValue;
   wxString              charValue;
   oapcBinHeadSp         binValue;

private:
   wxInt32         exitThread();

   wxUint64        lastInput;
   flowObject     *m_sourceObject;
   bool            firstCall;
};

#endif
