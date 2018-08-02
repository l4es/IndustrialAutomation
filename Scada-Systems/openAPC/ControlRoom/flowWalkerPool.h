#ifndef FLOWWALKERPOOL_H
#define FLOWWALKERPOOL_H

#include "../common/oapcBinHead.h"

class FlowWalkerThread;
class flowObject;
class ObjectList;



void FlowWalkerPool_oapc_io_callback(unsigned long outputs,unsigned long callbackID); // global callback function

class FlowWalkerPool : public wxEvtHandler
{
public:
	FlowWalkerPool();
	virtual ~FlowWalkerPool();

   void     startTimer();
   void     stopTimer();
   wxUint32 getFlowThreadID();
#ifndef ENV_PLUGGER
   void     exitApplication();
   void     startInitialFlows(ObjectList *list);
   void     startTimerFlows(ObjectList *list);
   void     startFlows(flowObject *object,FlowWalkerThread *thread,wxUint64 usedOutput,wxUint32 inheritThreadID,wxLongLong inheritCreationTime,wxUint64 lastInput);
   void     startOverflowFlows(flowObject *object,wxUint64 assignedOutputs,wxByte firstDigiVal,wxFloat64 firstNumVal,wxString firstCharVal,const oapcBinHeadSp &firstBinVal,wxLongLong inheritCreationTime);
#endif
   bool     checkObjectForInput(flowObject *object,wxUint64 outputs,wxLongLong currentTime,bool force);
   bool     sendLogMessageToIS(wxInt32 logID,wxString message);
   bool     sendDigiValueToIS(wxUint64 targetInput,wxByte digiValue,flowObject *object,bool isOutput);
   bool     sendNumValueToIS(wxUint64 targetInput,wxFloat64 numValue,flowObject *object,bool isOutput);
   bool     sendCharValueToIS(wxUint64 targetInput,wxString charValue,flowObject *object,bool isOutput);
   bool     sendBinValueToIS(wxUint64 targetInput,oapcBinHeadSp &binValue,flowObject *object,bool isOutput);
   bool     openISConnection(bool executeServer);
   bool     checkObjectsForInput();
   bool     outputPollTimer;

private:
   bool              checkObjectsForInput(ObjectList *list,wxLongLong currentTime);
   bool              tryOpenISConnection(bool executeServer,char *host);
#ifdef ENV_DEBUGGER
   void              setNewValue(wxInt32 outputNum,wxByte digi,flowObject *object);
   void              setNewValue(wxInt32 outputNum,wxFloat64 num,flowObject *object);
   void              setNewValue(wxInt32 outputNum,wxString txt,flowObject *object);
   void              setNewValue(wxInt32 outputNum,oapcBinHeadSp &bin,flowObject *object);
#endif

   wxUint32          currFlowThreadID;
   void             *idSection;

};

#endif
