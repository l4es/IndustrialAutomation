#ifndef EXTERNALIOLIBS_H
#define EXTERNALIOLIBS_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

using namespace std;

#include <list>
#include "ExternalIOLib.h"

class ExternalIOLibs  
{
public:
   ExternalIOLibs(wxString libDir,wxUint32 type,wxWindow *parentSplash);
#ifdef ENV_BEAM
   ExternalIOLibs(wxString libDir);
#endif
   ExternalIOLibs();
   virtual ~ExternalIOLibs();

   ExternalIOLib *getLib(bool start);
   ExternalIOLib *importPlugin(wxString libname,wxUint32 type,wxWindow *parentSplash);
   bool           contains(wxString libname);
   ExternalIOLib *addLib(ExternalIOLib *lib);
#ifdef ENV_BEAM
   void           removeLib(ExternalIOLib *lib);
#endif

private:

    wxString             m_libDir;
    list<ExternalIOLib*> m_libList;
};

#endif
