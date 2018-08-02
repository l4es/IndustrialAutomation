#ifdef ENV_INT
#include "../hmiImage.h"
#endif

#ifndef HMIIMAGE_H
#define HMIIMAGE_H

#pragma pack(8)

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000



struct hmiImageData
{
   char store_imageNormal[MAX_PATH_LENGTH*2];
};



class hmiImage : public hmiObject
{
public:
	hmiImage(BasePanel *parent,flowObject *obj);
	virtual ~hmiImage();

   virtual wxWindow *createUIElement();
   virtual void      drawObject(wxAutoBufferedPaintDC *dc);
#ifndef ENV_PLAYER
   virtual wxString  getDefaultName();
   virtual wxPanel  *getConfigPanel(wxString *name,wxWindow *parent,wxInt32 num);
   virtual void      setConfigData();
   virtual wxPoint   getMinSize();
   virtual wxPoint   getMaxSize();
   virtual void      setData(flowObject *object);
#else
   virtual void      setUIElementState(wxInt32 state);
   virtual wxByte    setBinInput(FlowConnection *connection,oapcBinHeadSp &value,wxUint32 *flowThreadID,flowObject *object);
#endif
   wxBitmap         *bitmap;
   bool              m_isVisible;
   wxString          imageNormal;

protected:
#ifdef ENV_EDITOR
    virtual wxInt32   save(wxFile *FHandle);
#endif
    virtual wxInt32   load(wxFile *FHandle,wxUint32 chunkSize,wxUint32 IDOffset);
    virtual void      doApplyData(const wxByte all);

    struct hmiImageData imageData;

private:
#ifdef ENV_EDITOR
    void              OnButtonClick(wxCommandEvent &event);
#endif
    void              setBitmapData();
    wxTextCtrl       *normalPath;
    wxButton         *normalFCButton;
};

#endif
