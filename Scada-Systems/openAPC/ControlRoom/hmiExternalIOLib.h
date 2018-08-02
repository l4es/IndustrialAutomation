#ifndef hmiExternalIOLib_H
#define hmiExternalIOLib_H

#pragma pack(8)

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include "flowConnection.h"
#include "ExternalIOLib.h"
#include "hmiHSlider.h"
#ifdef ENV_EDITOR
#include "XMLDecoder.h"
#endif



struct externalHMIData
{
    wxInt32  version,length;
    wxUint32 reserved1,reserved2,reserved3,reserved4;
    wxInt32  cycleMicros;
    wxInt32  extLibDataSize; // size of the data that come from/go to the external lib
    char     store_libname[MAX_LIBNAME_LENGTH];
};



class hmiExternalIOLib : public hmiHSlider
{
public:
   hmiExternalIOLib(ExternalIOLib *lib,BasePanel *parent,flowObject *obj,wxInt32 dataID);
   hmiExternalIOLib(BasePanel *parent,flowObject *obj);
   virtual ~hmiExternalIOLib();

   bool                          initExternal(ExternalIOLib *lib,bool getDefaultValues,wxInt32 dataID);
#ifndef ENV_PLAYER
   virtual wxString              getDefaultName();
   virtual void                  doDataFlowDialog(bool hideISConfig);
#endif
   virtual wxWindow             *createUIElement();
   virtual void                  doApplyData(const wxByte all);

   void                         *m_pluginData;

#ifdef ENV_EDITOR
   virtual wxPoint               getMinSize();
   virtual wxPoint               getMaxSize();
   virtual wxPoint               getDefSize();
   void                          getLibConfigData();
   virtual wxPanel              *getConfigPanel(wxString *name,wxWindow *parent,wxInt32 num);
   virtual void                  setConfigData();
   virtual wxInt32               save(wxFile *FHandle);
   virtual void                  setData(flowObject *object);
#else
   lib_oapc_mouseevent           oapc_mouseevent; // public function is called from the wxWindows object that will hold the external graphical data
//   lib_oapc_set_io_callback oapc_set_io_callback;

   virtual void           setUIElementState(wxInt32 state);
   virtual wxUint64       getAssignedOutput(wxUint64 input);
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
   virtual wxInt32        load(wxFile *FHandle,wxUint32 chunkSize,wxUint32 IDOffset);

#ifdef ENV_EDITOR
   XMLDecoder                   *xmlHMIDecoder,*xmlFlowDecoder;
#else
   wxFloat64                     prevNum[MAX_NUM_IOS];
   wxByte                        prevDigi[MAX_NUM_IOS];
   wxString                      prevTxt[MAX_NUM_IOS],txt[MAX_NUM_IOS];
#endif
   ExternalIOLib                *lib;

protected:

private:
#ifdef ENV_EDITOR
   void                          parseXMLDescr(wxXmlNode *node);
#endif

   wxByte               digiIn1,prevDigiIn1;
   wxFloat64            num[MAX_NUM_IOS];
   void                *m_accessMutex;
};



/********************************************************************************/

class hmiExtLibContainer : public wxPanel
{
public:
    hmiExtLibContainer(hmiExternalIOLib *lib,wxWindow *parent);
	virtual ~hmiExtLibContainer();


private:

    void    OnPaint(wxPaintEvent& event);
#ifdef ENV_PLAYER
    void    OnMouseEvent(wxMouseEvent& event);
#endif

    hmiExternalIOLib        *m_lib;
};





#endif
