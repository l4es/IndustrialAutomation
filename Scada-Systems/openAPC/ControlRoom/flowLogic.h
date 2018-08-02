#ifndef flowLogic_H
#define flowLogic_H

#pragma pack(8)

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include "flowConnection.h"


struct flowLogicOutputData
{
   wxByte    inputMask;
   wxByte    pad1;
   wxUint16  pad2;
   wxUint32  pad4;
   wxUint32  flags;      // includes operation mode;
   wxInt32   mConstValue_off; // constant value to calculate with
};


struct flowLogicData
{
   struct hmiFlowData          flowData;
   wxInt32                     flowX,flowY;
   char                        store_name[MAX_NAME_LENGTH*2];
   struct flowLogicOutputData  outData[MAX_NUM_IOS];
   struct oapc_num_value_block ConstValue[MAX_NUM_IOS];
};



class flowLogic : public flowObject
{
public:
   flowLogic();
   virtual ~flowLogic();
   virtual wxString getDefaultName();
#ifdef ENV_EDITOR
   virtual wxInt32  saveDATA(wxFile *FHandle,char chunkName[4],bool isCompiled);
#endif
   virtual wxInt32  loadDATA(wxFile *FHandle,wxUint32 chunkSize,wxUint32 IDOffset,bool isCompiled);

   static wxUint32      LOGIC_FLAGS_USE_CLOCK;
   static wxUint32      LOGIC_FLAGS_INVERT;
   static wxUint32      LOGIC_FLAGS_ALLOW_LOOPBACK;

   struct flowLogicData convData;                // used by config dialogue, therefore it is public
   wxFloat64            ConstValue[MAX_NUM_IOS]; // used by config dialogue, therefore it is public

protected:

private:
};

#endif
