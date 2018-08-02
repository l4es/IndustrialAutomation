#ifndef HMIPRIMITIVE_H
#define HMIPRIMITIVE_H

#include "oapcSpinCtrl.h"

#pragma pack(8)

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000



struct hmiPrimitiveData
{
   unsigned char m_mode,m_arrow[2],m_lineSize,m_arrowSize[2],res1,res2;
};



class hmiPrimitive : public hmiObject
{
public:
	hmiPrimitive(BasePanel *parent,flowObject *obj,wxUint32 type);
	virtual ~hmiPrimitive();

   virtual wxWindow *createUIElement();
   virtual void      drawObject(wxAutoBufferedPaintDC *dc);
#ifndef ENV_PLAYER
   virtual wxString  getDefaultName();
   virtual wxPoint   getMinSize();
   virtual wxPanel  *getConfigPanel(wxString *name,wxWindow *parent,wxInt32 num);
   virtual void      setConfigData();
   virtual void      setData(flowObject *object);
#else
   virtual void      setUIElementState(wxInt32 state);
#endif
   bool              m_isVisible;

protected:
#ifdef ENV_EDITOR
    virtual wxInt32        save(wxFile *FHandle);
#endif
    virtual wxInt32        load(wxFile *FHandle,wxUint32 chunkSize,wxUint32 IDOffset);
    virtual void           doApplyData(const wxByte all);

    struct hmiPrimitiveData freeLineData;

private:
#ifdef ENV_EDITOR
    void OnCommandEvent(wxCommandEvent &event);

    wxSpinCtrl *m_lineSizeField;
    wxComboBox *m_modeCombo;
    wxComboBox *m_arrowCombo[2];
    wxSpinCtrl *m_arrowSizeField[2];
#endif
};

#endif //HMIPRIMITIVE_H
