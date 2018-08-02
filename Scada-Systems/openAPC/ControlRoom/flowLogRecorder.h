#ifndef flowLogRecorder_H
#define flowLogRecorder_H

#pragma pack(8)

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include "flowConnection.h"
#ifdef ENV_PLAYER
#include "LogDataCtrl.h"
#endif


struct flowLogRecorderData
{
   struct hmiFlowData      flowData;
   wxInt32                 flowX,flowY;
   struct hmiObjectLogData data[MAX_NUM_IOS][MAX_LOG_TYPES];
   wxUint32                logFlags[MAX_NUM_IOS];
   char                    store_name[MAX_NAME_LENGTH*2];
};



class flowLogRecorder : public flowObject
{
public:
   flowLogRecorder();
   virtual ~flowLogRecorder();
   virtual wxString getDefaultName();
#ifdef ENV_EDITOR
   virtual void     doDataFlowDialog(bool hideISConfig);
   virtual wxInt32  saveDATA(wxFile *FHandle,char chunkName[4],bool isCompiled);
#else
   virtual wxUint64 getAssignedOutput(wxUint64 input);
   virtual wxByte   setDigitalInput(FlowConnection *connection,wxByte value,wxUint32 *flowThreadID,flowObject *object);
   virtual wxByte   setCharInput(FlowConnection *connection,wxString value,wxUint32 *flowThreadID,flowObject *object);
   virtual wxByte   setNumInput(FlowConnection *connection,wxFloat64 value,wxUint32 *flowThreadID,flowObject *object);
#endif
   virtual wxInt32  loadDATA(wxFile *FHandle,wxUint32 chunkSize,wxUint32 IDOffset,bool isCompiled);

   struct flowLogRecorderData logData;

protected:

private:
#ifdef ENV_PLAYER
   LogDataCtrl *logDataCtrl[MAX_NUM_IOS];
#endif
};

#endif
