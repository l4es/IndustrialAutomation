#ifndef FLOWCONVERTERDIGI2NUM_H
#define FLOWCONVERTERDIGI2NUM_H

#pragma pack(8)

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include "flowConnection.h"
#include "flowConverter.h"



struct flowConverterDigi2NumOutputData
{
   wxByte    inputMask;
   wxByte    pad1;
   wxUint16  pad2;
   wxUint32  flags; // includes operation mode;
   wxInt32   mOutValueLow[CONVERTER_MAX_INPUTS],mOutValueHigh[CONVERTER_MAX_INPUTS];
};



struct flowConverterDigi2NumData
{
   struct hmiFlowData                     flowData;
   wxInt32                                flowX,flowY;
   char                                   store_name[MAX_NAME_LENGTH*2];
   struct flowConverterDigi2NumOutputData outData[CONVERTER_MAX_OUTPUTS];
};



class flowConverterDigi2Num : public flowConverter
{
public:
   flowConverterDigi2Num();
   virtual ~flowConverterDigi2Num();
   virtual wxString  getDefaultName();
#ifndef ENV_PLAYER
   virtual void      doDataFlowDialog(bool hideISConfig);
   virtual wxInt32   saveDATA(wxFile *FHandle,char chunkName[4],bool isCompiled);
#else
   virtual wxUint64  getAssignedOutput(wxUint64 input);
   virtual wxFloat64 getNumOutput(FlowConnection *connection,wxInt32 *rcode,wxUint64 lastInput,wxLongLong origCreationTime);
#endif
   virtual wxInt32   loadDATA(wxFile *FHandle,wxUint32 chunkSize,wxUint32 IDOffset,bool isCompiled);

   struct flowConverterDigi2NumData convData;

protected:

private:
};

#endif
