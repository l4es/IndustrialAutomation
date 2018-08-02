#ifndef flowStart_H
#define flowStart_H

#pragma pack(8)

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include "flowConnection.h"

struct flowStartData
{
   struct hmiFlowData  flowData;
   wxInt32             flowX,flowY;
   wxUint8             digi[2],pad1_1,pad1_2;
   wxUint32            pad4;
   wxInt32             mnum[2];
   char                store_name[MAX_NAME_LENGTH*2];
   char                store_txt[2][MAX_NAME_LENGTH*2];
};



class flowStart : public flowObject
{
public:
   flowStart();
   virtual ~flowStart();
#ifdef ENV_EDITOR
   virtual wxString getDefaultName();
   virtual void     doDataFlowDialog(bool hideISConfig);
   virtual wxInt32  saveDATA(wxFile *FHandle,char chunkName[4],bool isCompiled);
#else
   virtual wxUint64  getAssignedOutput(wxUint64 input);
   virtual wxString  getCharOutput(FlowConnection *connection,wxInt32 *rcode,wxUint64 lastInput);
   virtual wxFloat64 getNumOutput(FlowConnection *connection,wxInt32 *rcode,wxUint64 lastInput,wxLongLong origCreationTime);
   virtual wxByte    getDigitalOutput(FlowConnection *connection,wxInt32 *rcode,wxUint64 lastInput,wxLongLong origCreationTime);
#endif
   virtual wxInt32   loadDATA(wxFile *FHandle,wxUint32 chunkSize,wxUint32 IDOffset,bool isCompiled);

   struct flowStartData convData;

   wxString          txt[2];

protected:

private:
};

#endif
