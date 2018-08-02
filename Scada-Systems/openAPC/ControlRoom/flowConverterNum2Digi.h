#ifndef FLOWCONVERTERNUM2DIGI_H
#define FLOWCONVERTERNUM2DIGI_H

#pragma pack(8)

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include "flowConnection.h"
#include "flowConverter.h"



struct flowConverterNum2DigiOutputData
{
   wxUint32  flags,enablePattern;
   wxInt32   mInNum[CONVERTER_MAX_ASSIGNMENTS];
   wxUint8   compareFlag[CONVERTER_MAX_ASSIGNMENTS]; //OUTPUTS]; // used to compare the inputs
   wxUint16  pad2_1;
   wxUint32  pad4_1;
   wxByte    outOperation[CONVERTER_MAX_ASSIGNMENTS]; // specifies what has to be done in case of a match: high/low/pulsed output
   wxUint16  pad2_2;
   wxUint32  pad4_2;
};



struct flowConverterNum2DigiData
{
   struct hmiFlowData                      flowData;
   wxInt32                                 flowX,flowY;
   char                                    store_name[MAX_NAME_LENGTH*2];
   struct flowConverterNum2DigiOutputData outData[CONVERTER_MAX_OUTPUTS];
};



class flowConverterNum2Digi : public flowConverter
{
public:
   flowConverterNum2Digi();
   virtual ~flowConverterNum2Digi();
   virtual wxString  getDefaultName();
#ifndef ENV_PLAYER
   virtual void      doDataFlowDialog(bool hideISConfig);
   virtual wxInt32   saveDATA(wxFile *FHandle,char chunkName[4],bool isCompiled);
#else
   virtual wxUint64  getAssignedOutput(wxUint64 input);
   virtual wxByte    getDigitalOutput(FlowConnection *connection,wxInt32 *rcode,wxUint64 lastInput,wxLongLong origCreationTime);
   virtual wxByte    setNumInput(FlowConnection *connection,wxFloat64 value,wxUint32 *flowThreadID,flowObject *object);

#endif
   virtual wxInt32   loadDATA(wxFile *FHandle,wxUint32 chunkSize,wxUint32 IDOffset,bool isCompiled);

   struct flowConverterNum2DigiData convData;

private:
#ifdef ENV_PLAYER
   wxFloat64        num[MAX_NUM_IOS];
#endif
};

#endif
