#ifndef hmiSpecialPanel_H
#define hmiSpecialPanel_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class hmiSpecialPanel:public hmiObject
{
public:
	hmiSpecialPanel(BasePanel *parent,flowObject *obj,wxInt32 type);
	virtual ~hmiSpecialPanel();

   virtual wxWindow *createUIElement();
   virtual void      applyFont(wxWindow *ui);
#ifdef ENV_EDITOR
   virtual wxString  getDefaultName();
#else
#endif

protected:
#ifdef ENV_EDITOR
   virtual wxInt32   save(wxFile *FHandle);
#endif
   virtual void      doApplyData(const wxByte all);

private:
           void      enable(wxWindow *ui,bool enable);

};

#endif
