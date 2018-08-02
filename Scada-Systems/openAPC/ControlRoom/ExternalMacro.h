#ifndef ExternalMacro_h
#define ExternalMacro_h

#include <wx/wx.h>

class ExternalMacro : public wxObject
{
public:
   ExternalMacro(wxString filename);
   ~ExternalMacro();

   wxString m_filename,m_name;
   wxInt32  m_uiID;
   wxUint32 m_category;
};

#endif
