#ifndef hmiAdditionalPane_H
#define hmiAdditionalPane_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "hmiStackedPane.h"

class hmiAdditionalPane:public hmiStackedPane
{
public:
	hmiAdditionalPane(BasePanel *parent,flowObject *obj);
	virtual ~hmiAdditionalPane();

   virtual BasePanel *getPanel();
   virtual wxWindow  *createUIElement();
   virtual void       doApplyData(const wxByte all);
   virtual void       setPos(wxRealPoint pos);
   virtual wxInt32    getPanelCount();
   virtual void       setPageSelected(wxWindow *panel);
#ifdef ENV_EDITOR
   virtual void       setPageSelected(wxInt32 panelNum);
   virtual wxInt32    getPageSelected();
   virtual wxString   getDefaultName();
#else
   virtual void       setUIElementState(wxInt32 state);
#endif

protected:
#ifdef ENV_EDITOR
   virtual wxInt32     save(wxFile *FHandle);
#endif
private:
   hmiTabbedPane       *basePane;
   wxInt32              panelNumber;
};
#endif
