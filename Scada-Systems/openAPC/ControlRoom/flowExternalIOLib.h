#ifndef FLOWEXTERNALIOLIB_H
#define FLOWEXTERNALIOLIB_H

#pragma pack(8)

#include "flowConnection.h"
#include "ExternalIOLib.h"
#include "common.h"
#ifdef ENV_EDITOR
#include "XMLDecoder.h"
#endif


class flowExternalIOLib : public flowObject
{
public:
   flowExternalIOLib(ExternalIOLib *lib,wxInt32 dataID);
   virtual ~flowExternalIOLib();
   virtual wxString getDefaultName();
#ifdef ENV_EDITOR
           wxInt32  saveEXIO(wxFile *FHandle,bool isCompiled);
   void             getLibConfigData();
#else
   virtual wxByte         setDigitalInput(FlowConnection *connection,wxByte value,wxUint32 *flowThreadID,flowObject *object);
   virtual wxByte         getDigitalOutput(FlowConnection *connection,wxInt32 *rcode,wxUint64 lastInput,wxLongLong origCreationTime);
   virtual wxByte         setNumInput(FlowConnection *connection,wxFloat64 value,wxUint32 *flowThreadID,flowObject *object);
   virtual wxFloat64      getNumOutput(FlowConnection *connection,wxInt32 *rcode,wxUint64 lastInput,wxLongLong origCreationTime);
   virtual wxByte         setCharInput(FlowConnection *connection,wxString value,wxUint32 *flowThreadID,flowObject *object);
   virtual wxString       getCharOutput(FlowConnection *connection,wxInt32 *rcode,wxUint64 lastInput);
   virtual oapcBinHeadSp getBinOutput(FlowConnection *connection,wxInt32 *rcode,wxUint64 lastInput);
   virtual void           releaseBinData(FlowConnection *connection);
   virtual wxByte         setBinInput(FlowConnection *connection,oapcBinHeadSp &value,wxUint32 *flowThreadID,flowObject *object);
#endif
           wxInt32        loadEXIO(wxFile *FHandle,struct externalFlowData *flowData,wxUint32 IDOffset,bool isCompiled);
   void                   doDataFlowDialog(bool hideISConfig);

#ifdef ENV_EDITOR
   XMLDecoder              *xmlDecoder;
#endif

   ExternalIOLib           *lib;
   wxByte                   returnOK;

protected:

private:
   void                *m_pluginData;
   void                *m_accessMutex;
};

#endif
