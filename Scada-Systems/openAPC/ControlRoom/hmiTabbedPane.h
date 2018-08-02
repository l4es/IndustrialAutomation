#ifndef HMITABBEDPANE_H
#define HMITABBEDPANE_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class hmiTabbedPane:public hmiObject
{
public:
	hmiTabbedPane(BasePanel *parent,flowObject *obj);
	virtual ~hmiTabbedPane();

    virtual BasePanel *getPanel();
    virtual void       addPanel(wxWindow *panel);
    virtual void       deletePanel(wxWindow *panel);
    virtual wxInt32    getPanelCount();
    virtual void       setPageSelected(wxWindow *panel);
#ifdef ENV_EDITOR
    virtual void       setPageSelected(wxInt32 panelNum);
    virtual wxInt32    getPageSelected();
#endif
    virtual void       setPageEnabled(wxWindow *panel,wxByte enable);
    virtual wxWindow  *createUIElement();
#ifndef ENV_PLAYER
    virtual wxString   getDefaultName();
#else
    virtual wxByte     setNumInput(FlowConnection *connection,wxFloat64 value,wxUint32 *flowThreadID,flowObject *object);
    virtual void       setUIElementState(wxInt32 state);
#endif

protected:
#ifdef ENV_EDITOR
   virtual wxInt32     save(wxFile *FHandle);
#endif
   virtual void        doApplyData(const wxByte all);
   BasePanel          *m_firstPanel;

};

#endif
