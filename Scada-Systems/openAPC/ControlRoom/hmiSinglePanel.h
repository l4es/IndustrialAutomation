#ifndef hmiSinglePanel_H
#define hmiSinglePanel_H

#pragma pack(8)

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000



struct hmiSinglePanelData
{
   wxByte   borderNum,pad1_1;
   wxUint16 pad2;
   wxUint32 reserved;
};



class hmiSinglePanel:public hmiObject
{
public:
	hmiSinglePanel(BasePanel *parent,flowObject *obj);
	virtual ~hmiSinglePanel();

   virtual wxWindow  *createUIElement();
   virtual wxInt32    load(wxFile *FHandle,wxUint32 chunkSize,wxUint32 IDOffset);
#ifdef ENV_EDITOR
   virtual wxString   getDefaultName();
   virtual wxPanel   *getConfigPanel(wxString *name,wxWindow *parent,wxInt32 num);
   virtual void       setConfigData();
   virtual void       setData(flowObject *object);
#endif

protected:
#ifdef ENV_EDITOR
   virtual wxInt32     save(wxFile *FHandle);
#endif
   virtual void        doApplyData(const wxByte all);

private:
   void                       updateBorder();

#ifdef ENV_EDITOR
   wxComboBox                *comboBox;
#endif
   struct hmiSinglePanelData  panelData;
};

#endif
