#ifndef FLOWCONVERTER_H
#define FLOWCONVERTER_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include "flowConnection.h"

#define CONVERTER_MAX_INPUTS      MAX_NUM_IOS
#define CONVERTER_MAX_OUTPUTS     MAX_NUM_IOS
#define CONVERTER_MAX_ASSIGNMENTS 10



class flowConverter : public flowObject
{
public:
   flowConverter();
   virtual ~flowConverter();
   virtual wxString getDefaultName();

   static wxUint32 CONVERTER_FLAGS_OUTPUT_LOW;
   static wxUint32 CONVERTER_FLAGS_OUTPUT_HIGH;
   static wxUint32 CONVERTER_FLAGS_OUTPUT_PULSE_LOHI;
   static wxUint32 CONVERTER_FLAGS_OUTPUT_PULSE_HILO;
   static wxUint32 CONVERTER_FLAGS_BINARY_MODE;
   static wxUint32 CONVERTER_FLAGS_DIRECT_MODE;
   static wxUint32 CONVERTER_FLAGS_FORMAT_MODE;
   static wxUint32 CONVERTER_FLAGS_OUTPUT_MASK;

   static wxUint8 CONVERTER_FLAGS_CMP_EQUAL;
   static wxUint8 CONVERTER_FLAGS_CMP_NOTEQUAL;
   static wxUint8 CONVERTER_FLAGS_CMP_GREATER;
   static wxUint8 CONVERTER_FLAGS_CMP_SMALLER;
   static wxUint8 CONVERTER_FLAGS_CMP_MASK;

   static wxUint32 CONVERTER_FLAGS_USE_CLOCK;
   static wxUint32 CONVERTER_FLAGS_FORCE_TO_DOT;


protected:

private:
};

#endif
