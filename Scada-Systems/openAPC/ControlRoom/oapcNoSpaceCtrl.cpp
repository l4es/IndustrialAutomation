#include <wx/wx.h>
#include <wx/valtext.h>
#include <wx/arrstr.h>

#include "oapcNoSpaceCtrl.h"
#include "globals.h"


/**
This class implements a very specific text input field that does not allow whitespaces
or tabs, it replaces the mby an underscore automatically
*/
oapcNoSpaceCtrl::oapcNoSpaceCtrl() : wxTextCtrl()
{ 
}




oapcNoSpaceCtrl::oapcNoSpaceCtrl(wxWindow *parent, wxWindowID id,wxString initial)
{
   Create(parent, id,wxDefaultPosition,wxDefaultSize,0,wxEmptyString);
   SetValue(initial);
}



oapcNoSpaceCtrl::~oapcNoSpaceCtrl()
{
}




bool oapcNoSpaceCtrl::Create(wxWindow *parent, wxWindowID id,const wxPoint& pos,const wxSize& size,long style,
                             const wxString& name)
{
   charExcludes.Add(_T(" "));
   textValidator.SetStyle(wxFILTER_EXCLUDE_LIST);
   textValidator.SetExcludes(charExcludes);
   return wxTextCtrl::Create(parent,id,wxEmptyString,pos,size,style,textValidator,name);
}



void oapcNoSpaceCtrl::SetValue(wxString val)
{
   g_checkString(val);
   wxTextCtrl::SetValue(val);
}



wxString oapcNoSpaceCtrl::GetValue()
{
   wxString val;

   val=wxTextCtrl::GetValue();
   g_checkString(val);
   return val;
}



