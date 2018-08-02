#ifndef FLOWGROUP_H
#define FLOWGROUP_H

#pragma pack(8)

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ObjectList.h"



struct groupFlowData
{
    struct   hmiFlowData flowData;
    wxInt32  flowX,flowY;
    wxUint32 type,flags,custIN,custOUT,dataFlowFlags,categoryFlag,reserved1,reserved2,reserved3,reserved4;
    char     store_name[MAX_NAME_LENGTH*2];
    char     store_descr[MAX_TEXT_LENGTH*2];
};



struct hmiGroupProjectData
{
   wxInt32 version,pad;
   wxInt32 flowW,flowH;
   wxInt32 res1,res2,res3,res4;
   char    store_descr[MAX_TEXT_LENGTH*2];
};



class IOElementDefinition;

class flowGroup : public flowObject
{
public:
   flowGroup(flowGroup *parent,ObjectList *list,wxPoint pos);
   flowGroup(flowGroup *parent,bool display);
   virtual ~flowGroup();
#ifdef ENV_EDITOR
   virtual wxString        getDefaultName();
//           wxInt32         saveEXIO(wxFile *FHandle,bool isCompiled);
   virtual wxInt32         getFlowWidth();
   virtual wxInt32         getFlowHeight();
   virtual wxUint64        getOutConnectorType(wxPoint pos);
   virtual IOElementDefinition *getInConnectorObject(wxPoint pos);
   virtual wxUint32        getInConnectorType(wxPoint pos);
           void            createFlowBitmap();
   virtual void            setFlowPos(ObjectList *parentList,wxPoint pos,wxByte forcePosition,wxByte checkIncoming);
           wxInt32         saveFGRP(wxFile *FHandle,wxByte saveOutgoing);
           flowGroup      *getParentObject();
   virtual bool            removeFlowConnection(FlowConnection *connection);
   virtual void            drawFlowConnections(wxAutoBufferedPaintDC *dc,FlowConnection *ignoreFlowCon,wxInt32 x, wxInt32 y);
   virtual void            drawFlowConnections(wxAutoBufferedPaintDC *dc,wxUint32 ignoreOutConnector,wxInt32 x, wxInt32 y);
   virtual FlowConnection *getFlowConnection(wxPoint pos,wxInt32 tolerance);
           void            getMinMaxIDs(wxUint32 &minID,wxUint32 &maxID);
   virtual wxPoint         getInConnectorPos(wxPoint pos);
   virtual void            doDataFlowDialog(bool hideISConfig);
           bool            isOutputUsed(flowObject *object,wxUint64 bit);
#endif
   virtual wxPoint         getOutConnectorPos(wxPoint pos);
   virtual void            deleteFlowConnection(FlowConnection *connection);

           ObjectList     *getObjectList();
           ObjectList     *getINObjectList();
           ObjectList     *getOUTObjectList();
   virtual flowObject     *getObject(wxUint32 id);
   virtual wxNode         *getObject(wxNode *node);
           void            addFlowConnection(FlowConnection *connection);
   virtual wxNode         *getFlowConnection(wxNode *node);

           wxInt32         loadFGRP(wxFile *FHandle,wxInt32 topChunkSize,wxUint32 IDOffset,bool display,ObjectList *topLevelObjectList);

           wxByte          returnOK;
#ifdef ENV_EDITOR
           wxUint32        m_categoryFlag;
#endif

protected:

private:
#ifdef ENV_EDITOR
   wxByte           setINConnections(flowObject* groupObject, IOElementDefinition *IOdef,wxPoint groupedObjectPos);
   wxByte           setOUTConnections(flowObject* groupedObject, IOElementDefinition *IOdef,wxPoint groupedObjectPos);
   wxString         descr;
#endif
   ObjectList      *list;
   ObjectList       INList,OUTList;
   flowGroup       *m_parent;
};

#endif
