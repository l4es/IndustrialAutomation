#ifndef FLOWCONVERTERNUM2CHAR_H
#define FLOWCONVERTERNUM2CHAR_H

#pragma pack(8)

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include "flowConnection.h"
#include "flowConverter.h"



struct flowConverterNum2CharOutputData
{
   wxUint32  flags,enablePattern;
   wxInt32   mInNum[CONVERTER_MAX_ASSIGNMENTS];
   wxUint8   compareFlag[CONVERTER_MAX_ASSIGNMENTS]; //OUTPUTS]; // used to compare the inputs
   wxUint16  pad2;
   wxUint32  pad4;
   char      store_outChar[CONVERTER_MAX_ASSIGNMENTS][MAX_NAME_LENGTH*2];
   char      store_outFormat[MAX_NAME_LENGTH*2];
};



struct flowConverterNum2CharData
{
   struct hmiFlowData                      flowData;
   wxInt32                                 flowX,flowY;
   char                                    store_name[MAX_NAME_LENGTH*2];
   struct flowConverterNum2CharOutputData outData[CONVERTER_MAX_OUTPUTS];
};



class flowConverterNum2Char : public flowConverter
{
public:
   flowConverterNum2Char();
   virtual ~flowConverterNum2Char();
   virtual wxString  getDefaultName();
#ifndef ENV_PLAYER
   virtual void      doDataFlowDialog(bool hideISConfig);
   virtual wxInt32   saveDATA(wxFile *FHandle,char chunkName[4],bool isCompiled);
#else
   virtual wxUint64  getAssignedOutput(wxUint64 input);
   virtual wxString  getCharOutput(FlowConnection *connection,wxInt32 *rcode,wxUint64 lastInput);
   virtual wxByte    setNumInput(FlowConnection *connection,wxFloat64 value,wxUint32 *flowThreadID,flowObject *object);

#endif
   virtual wxInt32   loadDATA(wxFile *FHandle,wxUint32 chunkSize,wxUint32 IDOffset,bool isCompiled);

   struct flowConverterNum2CharData convData;

   wxString          outChar[MAX_NUM_IOS][CONVERTER_MAX_ASSIGNMENTS];
   wxString          outFormat[MAX_NUM_IOS];

private:
#ifdef ENV_PLAYER
   wxFloat64        num[MAX_NUM_IOS];
   wxString         txtD[MAX_NUM_IOS],txtF[MAX_NUM_IOS];
#endif
};

#endif
