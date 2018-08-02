
#ifndef HMITEXTLABEL_H
#define HMITEXTLABEL_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000



struct hmiTextLabelData
{
   wxByte   alignment,pad1_1;
   wxUint16 pad2;
   wxUint32 reserved;
};



class hmiTextLabel : public hmiSimpleButton
{
public:
	hmiTextLabel(BasePanel *parent,flowObject *obj);
	virtual ~hmiTextLabel();

#ifdef ENV_EDITOR
   virtual wxString        getDefaultName();
   virtual wxPanel        *getConfigPanel(wxString *name,wxWindow *parent,wxInt32 num);
   virtual void            setConfigData();
#endif
   virtual wxInt32         load(wxFile *FHandle,wxUint32 chunkSize,wxUint32 IDOffset);
   virtual wxWindow       *createUIElement();

protected:
#ifdef ENV_EDITOR
   virtual wxInt32         save(wxFile *FHandle);
#endif

private:
   void                    updateAlignment();

#ifdef ENV_EDITOR
   wxComboBox             *comboBox;
#endif
   struct hmiTextLabelData labelData;

};

#endif
