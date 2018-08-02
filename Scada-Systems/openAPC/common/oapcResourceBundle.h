#ifndef oapcResourceBundle_H
#define oapcResourceBundle_H

#include <wx/wx.h>

class oapcResourceBundle;

#ifdef G_RES
extern oapcResourceBundle   *g_res;
#undef _
#define _(inStr) g_res->getString(_T(inStr))
#endif

class oapcResourceBundle : public wxObject  
{
public:
   oapcResourceBundle(const wxString resName,const wxString tryLocale=wxEmptyString);
   virtual ~oapcResourceBundle();

   virtual void      appendResource(const wxString resName,const wxString tryLocale=wxEmptyString);
   virtual void      flushResources();
   virtual wxString  getString(wxString key);
private:
   void     processLine(wxString ln);

//   wxByte   resLoaded;
};



#endif
