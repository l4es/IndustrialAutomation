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
#include <wx/menu.h>

#ifndef ENV_BEAM
 #include "globals.h"
#else
 #include "../CNConstruct/cnco_globals.h"
#endif
#include "UserList.h"
#include "UserData.h"
#include "hmiUserMgmntPanel.h"



UserList::UserList()
{
   UserData       *userData;

   userData=new UserData(_T("supervisor"),wxEmptyString,_T("supervisor"),
                         hmiUserMgmntPanel::priviFlagEnabled[0]|
                         hmiUserMgmntPanel::priviFlagEnabled[1]|
                         hmiUserMgmntPanel::priviFlagEnabled[2]|
                         hmiUserMgmntPanel::priviFlagEnabled[3]|
                         hmiUserMgmntPanel::priviFlagEnabled[4]|
                         hmiUserMgmntPanel::priviFlagEnabled[5]|
                         hmiUserMgmntPanel::priviFlagEnabled[6]|
                         hmiUserMgmntPanel::priviFlagEnabled[7]|
                         hmiUserMgmntPanel::priviFlagEnabled[8]);
   addUserData(userData);
   userData=new UserData(_T("admin"),_("Administrator"),_T("admin"),
                         hmiUserMgmntPanel::priviFlagEnabled[1]|
                         hmiUserMgmntPanel::priviFlagEnabled[2]|
                         hmiUserMgmntPanel::priviFlagEnabled[3]|
                         hmiUserMgmntPanel::priviFlagEnabled[4]|
                         hmiUserMgmntPanel::priviFlagEnabled[5]|
                         hmiUserMgmntPanel::priviFlagEnabled[6]|
                         hmiUserMgmntPanel::priviFlagEnabled[7]|
                         hmiUserMgmntPanel::priviFlagEnabled[8]);
   addUserData(userData);
   userData=new UserData(_T("service"),_("Service"),_T("service"),
                         hmiUserMgmntPanel::priviFlagEnabled[2]|
                         hmiUserMgmntPanel::priviFlagEnabled[3]|
                         hmiUserMgmntPanel::priviFlagEnabled[4]|
                         hmiUserMgmntPanel::priviFlagEnabled[5]|
                         hmiUserMgmntPanel::priviFlagEnabled[6]|
                         hmiUserMgmntPanel::priviFlagEnabled[7]|
                         hmiUserMgmntPanel::priviFlagEnabled[8]);
   addUserData(userData);
   userData=new UserData(_T("operator"),_("System Operator"),_T("operator"),
                         hmiUserMgmntPanel::priviFlagEnabled[6]|
                         hmiUserMgmntPanel::priviFlagEnabled[7]|
                         hmiUserMgmntPanel::priviFlagEnabled[8]);
   addUserData(userData);

/*      g_userPriviData.priviName[0]=_("Manage Users");
   g_userPriviData.priviName[1]=_("Exit Application");
   g_userPriviData.priviName[2]=_("Manual Control");
   g_userPriviData.priviName[3]=_("Modify Parameters");
   g_userPriviData.priviName[4]=_("Edit Recipe");
   g_userPriviData.priviName[5]=_("Select Recipe");
   g_userPriviData.priviName[6]=_("Start Process");
   g_userPriviData.priviName[7]=_("Stop Process");*/


#ifdef ENV_EDITOR
#ifndef ENV_BEAM
   menuEnable(true);
#endif //ENV_BEAM
#endif //ENV_EDITOR
}



UserList::~UserList()
{
   deleteAll();
#ifdef ENV_EDITOR
#ifndef ENV_BEAM
   menuEnable(false);
#endif //ENV_BEAM
#endif //ENV_EDITOR
}



void UserList::deleteAll()
{
   std::list<UserData*>::iterator it;
   UserData                      *userData;

   for (it=m_userList.begin(); it!=m_userList.end(); it++)
   {
      userData=*it;
      delete userData;
   }
   m_userList.clear();
}



#ifdef ENV_EDITOR
#ifndef ENV_BEAM
void UserList::menuEnable(bool doEnable)
{
   wxMenuItem    *item;
   wxMenuItemList menuItemList;
   wxNode        *node;

   menuItemList=g_mainMenuFile->GetMenuItems();
   node=(wxNode*)menuItemList.GetFirst();
   while (node)
   {
      item=(wxMenuItem*)node->GetData();
      if (item->GetId()==HMI_ID_USERS)
      {
         item->Enable(doEnable);
         break;
      }
      node=node->GetNext();
   }
}
#endif //ENV_BEAM
#endif



wxInt32 UserList::getCount()
{
   return m_userList.size();
}



void UserList::deleteUser(UserData *userData)
{
   m_userList.remove(userData);
}



UserData *UserList::getUserData(bool fromStart)
{
   static std::list<UserData*>::iterator it;

   if (fromStart)
   {
      if (m_userList.empty()) return NULL;
      it=m_userList.begin();
   }
   else
   {
      it++;
      if (it==m_userList.end()) return NULL;
   }
   return *it;
}


UserData *UserList::getUserDataByLogin(wxString login)
{
   std::list<UserData*>::iterator it;

   for (it=m_userList.begin(); it!=m_userList.end(); it++)
   {
      if ((*it)->m_login.Cmp(login)==0) return *it;
   }
   return NULL;
}



void UserList::addUserData(UserData *userData)
{
   m_userList.push_back(userData);
}


