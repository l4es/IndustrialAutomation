#ifndef FLOWCONVERTERDIGI2CHAR_H
#define FLOWCONVERTERDIGI2CHAR_H

#pragma pack(8)

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include "flowConnection.h"
#include "flowConverter.h"



struct flowConverterDigi2CharOutputData
{
   wxByte   inputMask;
   wxByte   pad1;
   wxUint16 pad2;
   wxUint32 flags; // includes operation mode;
   char     store_outValueLow[CONVERTER_MAX_INPUTS][MAX_NAME_LENGTH*2],store_outValueHigh[CONVERTER_MAX_INPUTS][MAX_NAME_LENGTH*2];
};



struct flowConverterDigi2CharData
{
   struct hmiFlowData                      flowData;
   wxInt32                                 flowX,flowY;
   char                                    store_name[MAX_NAME_LENGTH*2];
   struct flowConverterDigi2CharOutputData outData[CONVERTER_MAX_OUTPUTS];
};



class flowConverterDigi2Char : public flowConverter
{
public:
   flowConverterDigi2Char();
   virtual ~flowConverterDigi2Char();
   virtual wxString  getDefaultName();
#ifndef ENV_PLAYER
   virtual void      doDataFlowDialog(bool hideISConfig);
   virtual wxInt32   saveDATA(wxFile *FHandle,char chunkName[4],bool isCompiled);
#else
   virtual wxUint64  getAssignedOutput(wxUint64 input);
   virtual wxString  getCharOutput(FlowConnection *connection,wxInt32 *rcode,wxUint64 lastInput);
#endif
   virtual wxInt32   loadDATA(wxFile *FHandle,wxUint32 chunkSize,wxUint32 IDOffset,bool isCompiled);

   struct flowConverterDigi2CharData convData;
   wxString                          outValueLow[CONVERTER_MAX_OUTPUTS][CONVERTER_MAX_INPUTS],outValueHigh[CONVERTER_MAX_OUTPUTS][CONVERTER_MAX_INPUTS];

protected:

private:
#ifdef ENV_PLAYER
   wxString          byteToBinary(wxByte x);
#endif
};

#endif
