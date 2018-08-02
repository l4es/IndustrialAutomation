#ifndef IOELEMENTDEFINITION_H
#define IOELEMENTDEFINITION_H

#pragma pack(8)

#include "ObjectList.h"
#include "oapcNoSpaceCtrl.h"


struct groupIOFlowData
{
    struct   hmiFlowData flowData;
    wxUint32 IOFlag,id,flags,type;
    wxInt32  posx,posy,conPosx,conPosy;
    char     store_name[MAX_NAME_LENGTH*2];
};

#define FLOW_IO_ELEMENT_WIDTH   7
#define FLOW_IO_ELEMENT_HEIGHT 19

class IOElementDefinition:public flowObject
{
public:
#ifdef ENV_EDITOR
    IOElementDefinition(wxByte isInConnector,ObjectList *parentList,wxPoint pos);
#else
    IOElementDefinition();
#endif
#ifdef ENV_EDITOR
           wxInt32        getFlowWidth();
           wxInt32        getFlowHeight();
           wxInt32        saveFlowChunk(char *chunkName,wxFile *FHandle,wxByte saveOutgoing);
   virtual void           drawFlowConnections(wxAutoBufferedPaintDC *dc,FlowConnection *ignoreFlowCon,wxPoint pos,wxBitmap *bm);
   virtual void           drawFlowConnections(wxAutoBufferedPaintDC *dc,wxUint32 ignoreOutputConnector,wxPoint pos,wxBitmap *bm);
   virtual void           setConnectorPos(ObjectList *parentList,wxPoint pos,wxByte forcePosition,wxByte checkIncoming);
   virtual wxPoint        getFlowPos();
   virtual void           setIncomingConnectionPos(ObjectList *parentList,wxPoint delta);
   virtual wxUint32       getOutConnectorType(wxPoint pos);
   virtual wxUint32       getInConnectorType(wxPoint pos);
   virtual wxPoint        getOutConnectorPos(wxPoint pos);
   virtual wxPoint        getInConnectorPos(wxPoint pos);
   virtual void           doDataFlowDialog(bool hideISConfig);
#else
   virtual wxUint64       getAssignedOutput(wxUint64 input);
   virtual wxByte         setDigitalInput(FlowConnection *connection,wxByte value,wxUint32 *flowThreadID,flowObject *object);
   virtual wxByte         getDigitalOutput(FlowConnection *connection,wxInt32 *rcode,wxUint64 lastInput,wxLongLong origCreationTime);
   virtual wxByte         setNumInput(FlowConnection *connection,wxFloat64 value,wxUint32 *flowThreadID,flowObject *object);
   virtual wxFloat64      getNumOutput(FlowConnection *connection,wxInt32 *rcode,wxUint64 lastInput,wxLongLong origCreationTime);
   virtual wxByte         setCharInput(FlowConnection *connection,wxString value,wxUint32 *flowThreadID,flowObject *object);
   virtual wxString       getCharOutput(FlowConnection *connection,wxInt32 *rcode,wxUint64 lastInput);
   virtual oapcBinHeadSp getBinOutput(FlowConnection *connection,wxInt32 *rcode,wxUint64 lastInput);
   virtual wxByte         setBinInput(FlowConnection *connection,oapcBinHeadSp &value,wxUint32 *flowThreadID,flowObject *object);
#endif
    wxInt32               loadFlowChunk(wxFile *FHandle,wxUint32 IDOffset,bool display,bool isCompiled);

    struct groupIOFlowData data;
    wxString               name;
    wxByte                 m_isInConnector;
#ifdef ENV_EDITOR
    oapcNoSpaceCtrl       *m_nameField;

#endif
    static wxUint32 FLAG_NOT_EXTERNALLY_VISIBLE;

private:
#ifndef ENV_EDITOR
   wxByte        digiStorage;
   wxString      charStorage;
   wxFloat64     numStorage;
   oapcBinHeadSp binStorage;
#endif
};
#endif
