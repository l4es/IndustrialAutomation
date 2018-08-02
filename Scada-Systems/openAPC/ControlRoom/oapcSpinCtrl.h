#ifndef oapcSpinCtrl_H
#define oapcSpinCtrl_H

#include <wx/spinbutt.h>
#include "oapcFloatCtrl.h"

class OSNumpad;
class wxSpinButton;

class oapcSpinCtrl : public wxPanel
{
public:
   oapcSpinCtrl(wxWindow* parent, wxWindowID id = -1,const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize,int min = 0, int max = 100, int initial = 0);
   virtual ~oapcSpinCtrl();

   virtual wxInt32 GetValue();
   virtual void    SetValue(wxInt32 value);
   virtual bool    Enable(bool enable);
   virtual void    SetEditable(bool editable);

   oapcFloatCtrl *m_numField;
   wxSpinButton  *m_spinButton;

protected:

private:
   void OnSpinEvent(wxSpinEvent& event);
   void OnTextChanged(wxCommandEvent& event);
   wxInt32        m_min,m_max;

   DECLARE_EVENT_TABLE()
};



#endif
