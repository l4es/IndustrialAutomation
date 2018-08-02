#ifndef FLOWCONVERTERCHAR2DIGI_H
#define FLOWCONVERTERCHAR2DIGI_H

#pragma pack(8)

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include "flowConnection.h"
#include "flowConverter.h"



struct flowConverterChar2DigiOutputData
{
   wxUint8   compareFlag[CONVERTER_MAX_ASSIGNMENTS]; //OUTPUTS]; // used to compare the input value
   wxUint32  pad4;
   wxUint16  pad2;
   wxUint32  enablePattern,casePattern;
   wxUint32  outOperation[CONVERTER_MAX_ASSIGNMENTS]; // specifies what has to be done in case of a match: high/low/pulsed output
   char      store_inChar[CONVERTER_MAX_ASSIGNMENTS][MAX_NAME_LENGTH*2];
};



struct flowConverterChar2DigiData
{
   struct hmiFlowData                      flowData;
   wxInt32                                 flowX,flowY;
   char                                    store_name[MAX_NAME_LENGTH*2];
   struct flowConverterChar2DigiOutputData outData[CONVERTER_MAX_OUTPUTS];
};



class flowConverterChar2Digi : public flowConverter
{
public:
   flowConverterChar2Digi();
   virtual ~flowConverterChar2Digi();
   virtual wxString  getDefaultName();
#ifdef ENV_EDITOR
   virtual void      doDataFlowDialog(bool hideISConfig);
   virtual wxInt32   saveDATA(wxFile *FHandle,char chunkName[4],bool isCompiled);
#else
   virtual wxUint64  getAssignedOutput(wxUint64 input);
   virtual wxByte    getDigitalOutput(FlowConnection *connection,wxInt32 *rcode,wxUint64 lastInput,wxLongLong origCreationTime);
   virtual wxByte    setCharInput(FlowConnection *connection,wxString value,wxUint32 *flowThreadID,flowObject *object);

#endif
   virtual wxInt32   loadDATA(wxFile *FHandle,wxUint32 chunkSize,wxUint32 IDOffset,bool isCompiled);

   struct flowConverterChar2DigiData convData;

   wxString         inChar[CONVERTER_MAX_OUTPUTS][CONVERTER_MAX_ASSIGNMENTS];

protected:
#ifdef ENV_PLAYER
   wxString         txt[MAX_NUM_IOS];
#endif

private:
};

#endif
