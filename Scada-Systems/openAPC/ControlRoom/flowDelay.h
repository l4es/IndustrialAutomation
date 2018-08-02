#ifndef flowDelay_H
#define flowDelay_H

#pragma pack(8)

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include "flowConnection.h"



struct flowDelayData
{
   struct hmiFlowData  flowData;
   wxInt32             flowX,flowY;
   wxUint32            delay[MAX_NUM_IOS]; // msec
   char                store_name[MAX_NAME_LENGTH*2];
};



class flowDelayThreadData
{
public:
   wxUint64    m_output;
   wxUint32    m_delay;
   wxByte      m_digi;
   wxFloat64   m_num;
   wxString    m_txt;
   flowObject *m_sourceObject;
};


class flowDelay : public flowObject
{
public:
   flowDelay();
   virtual ~flowDelay();
   virtual wxString getDefaultName();
#ifdef ENV_EDITOR
   virtual void     doDataFlowDialog(bool hideISConfig);
   virtual wxInt32  saveDATA(wxFile *FHandle,char chunkName[4],bool isCompiled);

#else
   virtual wxUint64  getAssignedOutput(wxUint64 input);
   virtual wxByte    setDigitalInput(FlowConnection *connection,wxByte value,wxUint32 *flowThreadID,flowObject *object);
   virtual wxByte    setCharInput(FlowConnection *connection,wxString value,wxUint32 *flowThreadID,flowObject *object);
   virtual wxByte    setNumInput(FlowConnection *connection,wxFloat64 value,wxUint32 *flowThreadID,flowObject *object);
#endif
   virtual wxInt32   loadDATA(wxFile *FHandle,wxUint32 chunkSize,wxUint32 IDOffset,bool isCompiled);

   struct flowDelayData convData;

protected:

private:

};

#endif
