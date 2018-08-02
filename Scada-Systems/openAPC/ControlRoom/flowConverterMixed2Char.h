#ifndef FLOWCONVERTERMixed2CHAR_H
#define FLOWCONVERTERMixed2CHAR_H

#pragma pack(8)

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include "flowConnection.h"
#include "flowConverter.h"



struct flowConverterMixed2CharData
{
   struct hmiFlowData flowData;
   wxInt32            flowX,flowY;
   wxUint32           flags,reserved1;
   char               store_name[MAX_NAME_LENGTH*2];
   char               store_outFormat[MAX_NAME_LENGTH*2];
};



class flowConverterMixed2Char : public flowConverter
{
public:
   flowConverterMixed2Char();
   virtual ~flowConverterMixed2Char();
   virtual wxString  getDefaultName();
#ifndef ENV_PLAYER
   virtual void      doDataFlowDialog(bool hideISConfig);
   virtual wxInt32   saveDATA(wxFile *FHandle,char chunkName[4],bool isCompiled);
#else
   virtual wxUint64  getAssignedOutput(wxUint64 input);
   virtual wxString  getCharOutput(FlowConnection *connection,wxInt32 *rcode,wxUint64 lastInput);
   virtual wxByte    setNumInput(FlowConnection *connection,wxFloat64 value,wxUint32 *flowThreadID,flowObject *object);
   virtual wxByte    setCharInput(FlowConnection *connection,wxString value,wxUint32 *flowThreadID,flowObject *object);

#endif
   virtual wxInt32   loadDATA(wxFile *FHandle,wxUint32 chunkSize,wxUint32 IDOffset,bool isCompiled);

   struct flowConverterMixed2CharData convData;

   wxString          outFormat;

private:
#ifdef ENV_PLAYER
   wxFloat64        num[MAX_NUM_IOS];
   wxString         txtD[MAX_NUM_IOS],txtF[MAX_NUM_IOS];
#endif
};

#endif
