#ifndef HMISTACKEDPANE_H
#define HMISTACKEDPANE_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class hmiStackedPane:public hmiTabbedPane
{
public:
	hmiStackedPane(BasePanel *parent,flowObject *obj);
	virtual ~hmiStackedPane();

   virtual wxWindow  *createUIElement();
#ifndef ENV_PLAYER
   virtual wxString  getDefaultName();
#endif

protected:
#ifdef ENV_EDITOR
   virtual wxInt32     save(wxFile *FHandle);
#endif
};

#endif
