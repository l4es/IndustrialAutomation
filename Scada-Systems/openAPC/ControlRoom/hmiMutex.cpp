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

#include "hmiMutex.h"



wxUint32 hmiMutex::s_lastID=0;



hmiMutex::hmiMutex(wxString name)
{
   m_name=name;
   m_ID=hmiMutex::getNextID();
}


hmiMutex::~hmiMutex()
{
}



void hmiMutex::addObjectID(wxUint32 id)
{
   m_idQ.push_front(id);
}



void hmiMutex::removeObjectID(wxUint32 id)
{
   m_idQ.remove(id);
}



bool hmiMutex::hasObjectID(wxUint32 id)
{
   std::list<wxUint32>::iterator it;

   for (it=m_idQ.begin(); it!=m_idQ.end(); it++)
    if ((*it)==id) return true;
   return false;
}



wxUint32 hmiMutex::getNextID()
{
   s_lastID++;
   return s_lastID;
}


