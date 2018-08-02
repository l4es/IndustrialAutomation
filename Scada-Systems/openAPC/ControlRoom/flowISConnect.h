#ifndef flowISConnect_H
#define flowISConnect_H

#include "flowConnection.h"


struct flowISConnectData
{
   struct hmiFlowData flowData;
   wxInt32            flowX,flowY;
   char               store_name[(MAX_NAME_LENGTH*2)+4];
   char               inNode[MAX_NUM_IOS][MAX_NAME_LENGTH+4],outNode[MAX_NUM_IOS][MAX_NAME_LENGTH+4];
   wxInt8             inNodeNum[MAX_NUM_IOS],outNodeNum[MAX_NUM_IOS];
};


class flowISConnect : public flowObject
{
public:
   flowISConnect();
   virtual ~flowISConnect();
   virtual wxString  getDefaultName();
   virtual wxInt32   loadDATA(wxFile *FHandle,wxUint32 chunkSize,wxUint32 IDOffset,bool isCompiled);
           bool      containsOutput(wxString name);
#ifdef ENV_EDITOR
   virtual wxInt32   saveDATA(wxFile *FHandle,char chunkName[4],bool isCompiled);
   virtual void      doDataFlowDialog(bool hideISConfig);
#else
           flowObject    *getTargetObjectInfo(wxString objectName,wxInt32 outputNum,wxUint64 *outputFlag);
   virtual wxByte         setDigitalInput(FlowConnection *connection,wxByte value,wxUint32 *flowThreadID,flowObject *object);
   virtual wxByte         getDigitalOutput(FlowConnection *connection,wxInt32 *rcode,wxUint64 lastInput,wxLongLong origCreationTime);
   virtual wxByte         setNumInput(FlowConnection *connection,wxFloat64 value,wxUint32 *flowThreadID,flowObject *object);
   virtual wxFloat64      getNumOutput(FlowConnection *connection,wxInt32 *rcode,wxUint64 lastInput,wxLongLong origCreationTime);
   virtual wxByte         setCharInput(FlowConnection *connection,wxString value,wxUint32 *flowThreadID,flowObject *object);
   virtual wxString       getCharOutput(FlowConnection *connection,wxInt32 *rcode,wxUint64 lastInput);
   virtual oapcBinHeadSp getBinOutput(FlowConnection *connection,wxInt32 *rcode,wxUint64 lastInput);
   virtual wxByte         setBinInput(FlowConnection *connection,oapcBinHeadSp &value,wxUint32 *flowThreadID,flowObject *object);
#endif

#ifndef ENV_EDITOR
   bool          m_digiValid[MAX_NUM_IOS],m_numValid[MAX_NUM_IOS],m_charValid[MAX_NUM_IOS];
   wxFloat64     m_num[MAX_NUM_IOS];
   wxString      m_txt[MAX_NUM_IOS];
   oapcBinHeadSp m_bin[MAX_NUM_IOS];
#endif

protected:

private:
   struct flowISConnectData m_ISConnectData;
   wxString                 m_outNode[MAX_NUM_IOS];
   bool                     m_outNodesConverted;
};

#endif
