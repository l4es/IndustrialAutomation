#ifndef DlgConfigflowBase_H
#define DlgConfigflowBase_H

#include <wx/wx.h>

class flowObject;

class DlgConfigflowBase : public wxDialog  
{
public:
	DlgConfigflowBase(flowObject *object,wxWindow* parent, const wxString& title,bool hideISConfig);
	virtual ~DlgConfigflowBase();

   wxByte   returnOK;

protected:
#ifndef ENV_BEAM
   void             addStdElements(wxSizer *fSizerBasic,wxPanel *panelBasic,bool buttonsOnly,bool threeRows);
#endif

   flowObject      *m_object;
#ifndef ENV_BEAM
   wxCheckBox      *mapOutputCB,*ioModCB;
   bool             m_hideISConfig;
#endif

private:


};

#endif
