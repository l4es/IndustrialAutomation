/******************************************************************************

This file is part of ControlRoom process control/HMI software.

ControlRoom is free software: you can redistribute it and/or modify it under
the terms of the GNU General Public License as published by the Free
Software Foundation, either version 3 of the License, or (at your option) any
later version.

ControlRoom is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more
details.

You should have received a copy of the GNU General Public License along with
ControlRoom. If not, see http://www.gnu.org/licenses/

*******************************************************************************

For different licensing and/or usage of the sources apart from GPL or any other
open source license, please contact us at https://openapc.com/contact.php

*******************************************************************************/

#include <wx/wx.h>
#include <wx/dir.h>
#include <wx/filename.h>

#ifndef ENV_BEAM
 #include "globals.h"
#endif
#include "common.h"
#include "ExternalIOLibs.h"
#include "ExternalIOLib.h"
#include "globals.h"


ExternalIOLibs::ExternalIOLibs(wxString libDir,wxUint32 type,wxWindow *parentSplash)
{
   SDBG
   m_libDir=libDir;
   wxDir dir(_T(LIB_PATH)+m_libDir);

#ifdef ENV_BEAM
 #ifdef _DEBUG
  #ifdef ENV_WINDOWS
   // Windows debug version of BeamConstruct has to look for the plugins at a different location
   dir.Open(_T("..\\OpenAPC\\"+libDir));
  #endif
 #endif
#endif

   if (dir.IsOpened())
   {
      wxString filename;

      bool cont = dir.GetFirst(&filename, wxEmptyString, wxDIR_FILES);
      while ( cont )
      {
#ifdef ENV_WINDOWS
         if (!(filename.Lower().Mid(filename.Length()-4,4).CompareTo(_T(".dll")))) importPlugin(filename,type,parentSplash);
#else
 #if defined (ENV_LINUX) || defined (ENV_QNX)
         if (!(filename.Lower().Mid(filename.Length()-3,3).CompareTo(_T(".so")))) importPlugin(filename,type,parentSplash);
 #else
  #error Not implemented!
 #endif
#endif
         cont = dir.GetNext(&filename);
      }
   }
}

#ifdef ENV_BEAM
ExternalIOLibs::ExternalIOLibs(wxString libDir)
{
   SDBG
   m_libDir=libDir;
}
#endif

ExternalIOLibs::ExternalIOLibs()
{
   SDBG
}


ExternalIOLibs::~ExternalIOLibs()
{
   ExternalIOLib *extIOLib;

   SDBG
   extIOLib=getLib(true);
   while (extIOLib)
   {
      delete extIOLib;
      extIOLib=getLib(false);
   }
   m_libList.clear();
}


bool ExternalIOLibs::contains(wxString libname)
{
   ExternalIOLib *extIOLib;

   extIOLib=getLib(true);
   while (extIOLib)
   {
      if (extIOLib->compare(libname)) return true;
      extIOLib=getLib(false);
   }
   return false;
}


#ifdef ENV_BEAM
void ExternalIOLibs::removeLib(ExternalIOLib *lib)
{
   m_libList.remove(lib);
}
#endif


ExternalIOLib *ExternalIOLibs::addLib(ExternalIOLib *lib)
{
   static list<ExternalIOLib*>::iterator it;

   if (m_libList.empty())
   {
      m_libList.push_back(lib);
      return lib;
   }
   else
   {
      list<ExternalIOLib*>::iterator it;

      it=m_libList.begin();
      while (it!=m_libList.end())
      {
         if ((*it)->name>lib->name)
         {
            m_libList.insert(it,lib);
            return lib;
         }
         it++;
      }
      m_libList.push_back(lib);
   }
   return lib;
}


ExternalIOLib *ExternalIOLibs::importPlugin(wxString libname,wxUint32 type,wxWindow *parentSplash)
{
   ExternalIOLib *lib;

   SDBG
   if (m_libDir.Length()>0) lib=new ExternalIOLib(wxString()+m_libDir+wxFileName::GetPathSeparator()+libname,type,parentSplash);
   else lib=new ExternalIOLib(libname,type,parentSplash);
   if (lib->isValid) return addLib(lib);
#ifndef _DEBUG
   else delete lib; // causes some problems with debug CRT
#endif
   return NULL;
}



ExternalIOLib *ExternalIOLibs::getLib(bool start)
{
   static list<ExternalIOLib*>::iterator it;

   SDBG
   if (m_libList.empty()) return NULL;
   if (start)
   {
      it=m_libList.begin();
      return *it;
   }
   else
   {
      it++;
      if (it!=m_libList.end()) return *it;
   }
   return NULL;
}


