#ifdef ENV_INT
#include "../hmiImageButton.h"
#endif

#ifndef HMIIMAGEBUTTON_H
#define HMIIMAGEBUTTON_H

#pragma pack(8)

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


struct hmiImageButtonData
{
   char store_imageNormal[MAX_PATH_LENGTH*2];
   char store_imageSelected[MAX_PATH_LENGTH*2];
   char store_imageDisabled[MAX_PATH_LENGTH*2];
};



class hmiImageButton : public hmiSimpleButton
{
public:
	hmiImageButton(BasePanel *parent,flowObject *obj);
	virtual ~hmiImageButton();

#ifdef ENV_EDITOR
   virtual wxString  getDefaultName();
   virtual wxPanel  *getConfigPanel(wxString *name,wxWindow *parent,wxInt32 num);
   virtual void      setConfigData();
   virtual void      setData(flowObject *object);
#else
   virtual wxByte    setBinInput(FlowConnection *connection,oapcBinHeadSp &value,wxUint32 *flowThreadID,flowObject *object);
   virtual wxUint64  getAssignedOutput(wxUint64 input);
#endif
   virtual wxWindow *createUIElement();

protected:
#ifdef ENV_EDITOR
    virtual wxInt32     save(wxFile *FHandle);
#endif
    virtual wxInt32     load(wxFile *FHandle,wxUint32 chunkSize,wxUint32 IDOffset);
    virtual void        doApplyData(const wxByte all);

    struct hmiImageButtonData imageButtonData;

    wxString             imageNormal,imageSelected,imageDisabled;
    wxStaticText        *normalText,*disabledText,*selectedText;
    wxTextCtrl          *normalPath,*disabledPath,*selectedPath;
    wxButton            *normalFCButton,*disabledFCButton,*selectedFCButton;
    wxComboBox          *m_predefComboBox;

private:
#ifdef ENV_EDITOR
    void                 OnButtonClick(wxCommandEvent &event);
    void                 updateUI();
#endif
    void                 setBitmapData(bool all);

    wxBitmap             bitmap;
};

#endif

