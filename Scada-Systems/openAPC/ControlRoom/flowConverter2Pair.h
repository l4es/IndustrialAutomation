#ifndef flowConverter2Pair_H
#define flowConverter2Pair_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include "flowConnection.h"
#include "flowConverter.h"

struct flowConverter2PairData
{
   struct hmiFlowData flowData;
   wxInt32            flowX,flowY;
   wxUint32           flags,reserved1;
   char               store_name[MAX_NAME_LENGTH*2];
   char               store_cmdName[MAX_NUM_IOS][MAX_NAME_LENGTH*2];
};


class flowConverter2Pair : public flowConverter
{
public:
   flowConverter2Pair();
   virtual ~flowConverter2Pair();

   virtual wxString getDefaultName();
#ifdef ENV_EDITOR
   virtual void     doDataFlowDialog(bool hideISConfig);
   virtual wxInt32  saveDATA(wxFile *FHandle,char chunkName[4],bool isCompiled);
#else
   wxString         getCharOutput(FlowConnection *connection,wxInt32 *rcode,wxUint64 lastInput);
#endif
   virtual wxInt32  loadDATA(wxFile *FHandle,wxUint32 chunkSize,wxUint32 IDOffset,bool isCompiled);


   struct flowConverter2PairData convData;
   wxString         cmdName[MAX_NUM_IOS];


protected:
#ifdef ENV_PLAYER
   wxInt32          m_lastUsedInput;
#endif

private:
};

#endif
