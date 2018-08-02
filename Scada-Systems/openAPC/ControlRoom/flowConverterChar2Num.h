#ifndef FLOWCONVERTERCHAR2NUM_H
#define FLOWCONVERTERCHAR2NUM_H

#pragma pack(8)

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include "flowConnection.h"
#include "flowConverter.h"



struct flowConverterChar2NumOutputData
{
   wxUint8   compareFlag[CONVERTER_MAX_ASSIGNMENTS];//OUTPUTS]; // used to compare the input value
   wxUint32  pad4;
   wxUint16  pad2;
   wxUint32  flags,reserved1; // includes operation mode;
   wxUint32  enablePattern,casePattern;
   wxInt32   mOutNum[CONVERTER_MAX_ASSIGNMENTS];
   char      store_inChar[CONVERTER_MAX_ASSIGNMENTS][MAX_NAME_LENGTH*2];
};



struct flowConverterChar2NumData
{
   struct hmiFlowData                     flowData;
   wxInt32                                flowX,flowY;
   char                                   store_name[MAX_NAME_LENGTH*2];
   struct flowConverterChar2NumOutputData outData[CONVERTER_MAX_OUTPUTS];
};



class flowConverterChar2Num : public flowConverter
{
public:
   flowConverterChar2Num();
   virtual ~flowConverterChar2Num();
   virtual wxString  getDefaultName();
#ifndef ENV_PLAYER
   virtual void      doDataFlowDialog(bool hideISConfig);
   virtual wxInt32   saveDATA(wxFile *FHandle,char chunkName[4],bool isCompiled);
#else
   virtual wxUint64  getAssignedOutput(wxUint64 input);
   virtual wxFloat64 getNumOutput(FlowConnection *connection,wxInt32 *rcode,wxUint64 lastInput,wxLongLong origCreationTime);
   virtual wxByte    setCharInput(FlowConnection *connection,wxString value,wxUint32 *flowThreadID,flowObject *object);

#endif
   virtual wxInt32   loadDATA(wxFile *FHandle,wxUint32 chunkSize,wxUint32 IDOffset,bool isCompiled);

   struct flowConverterChar2NumData convData;

   wxString          inChar[CONVERTER_MAX_OUTPUTS][CONVERTER_MAX_ASSIGNMENTS];

protected:
#ifdef ENV_PLAYER
   wxString         txt[MAX_NUM_IOS];
#endif

private:
};

#endif
