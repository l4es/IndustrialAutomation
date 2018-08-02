#ifndef ExternalMacroS_H
#define ExternalMacroS_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

using namespace std;

#include <list>
#include "ExternalMacro.h"

class ExternalMacros  
{
public:
	ExternalMacros();
	virtual ~ExternalMacros();

   ExternalMacro *getMacro(bool start);

private:
   void getMacroTitle(wxString file,wxString *title,wxUint32 *category);
   void importMacro(wxString libname);

   list<ExternalMacro*> m_macroList;
};

#endif
