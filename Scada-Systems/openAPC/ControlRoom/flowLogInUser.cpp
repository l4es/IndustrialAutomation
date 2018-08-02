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

#include "iff.h"
#include "globals.h"
#include "flowLogInUser.h"
#include "oapc_libio.h"
#ifndef ENV_HPLAYER
 #include "UserList.h"
 #include "HMICanvas.h"
#endif

flowLogInUser::flowLogInUser():flowObject(NULL)
{
   this->data.type=FLOW_TYPE_MISC_LOGIN;
   this->data.stdIN= OAPC_CHAR_IO0|OAPC_CHAR_IO1|OAPC_DIGI_IO2|OAPC_DIGI_IO3;
   this->data.stdOUT=OAPC_CHAR_IO0|OAPC_DIGI_IO1;
#ifndef ENV_EDITOR
#ifndef ENV_HPLAYER
   m_userData=NULL;
#endif
   m_loggedIn=false;
#endif
}



flowLogInUser::~flowLogInUser()
{
}



wxString flowLogInUser::getDefaultName()
{
   return _("Log In User");
}


#ifdef ENV_PLAYER
wxUint64 flowLogInUser::getAssignedOutput(wxUint64 input)
{
   if (input & OAPC_CHAR_IO1)
   {
#ifndef ENV_HPLAYER
      if ((m_userData) && (m_loggedIn)) return (OAPC_CHAR_IO0|OAPC_DIGI_IO1);
      else
#endif
       return OAPC_DIGI_IO1;
   }
   else if ((input & OAPC_DIGI_IO2) && (digi[2]==1))
   {
      return 0;
   }
   else if ((input & OAPC_DIGI_IO3) && (digi[3]==1))
   {
      return (OAPC_CHAR_IO0|OAPC_DIGI_IO1);
   }
   return 0;
}



wxByte flowLogInUser::setCharInput(FlowConnection *connection,wxString value,wxUint32 *flowThreadID,flowObject *WXUNUSED(object))
{
   if (!threadIDOK(flowThreadID,false)) return 0;
   if (connection->targetInputNum==0)
   {
#ifndef ENV_HPLAYER
      m_userData=g_userList->getUserDataByLogin(value);
#endif
      m_loggedIn=false;
   }
   else if (connection->targetInputNum==1)
   {
#ifndef ENV_HPLAYER
      if (m_userData)
      {
         if ((m_userData->m_pwd.Cmp(value)==0) && (m_userData->m_state==0))
          m_loggedIn=true;
         else m_loggedIn=false;
      }
      else
#endif
       m_loggedIn=false;
      if (m_loggedIn)
      {
#ifndef ENV_HPLAYER
         g_objectList.setUserPrivileges(m_userData);
         // switch all ui elements states
#endif
      }
   }
   else return 0;
   return 1;
}



wxString flowLogInUser::getCharOutput(FlowConnection *connection,wxInt32 *rcode,wxUint64 lastInput)
{
   wxString retValue;

   if (connection->sourceOutputNum!=0)
   {
#ifdef ENV_DEBUGGER
      g_debugWin->setDebugInformation(this,DEBUG_STOP_COND_ILLEGAL_IO,_T(""));
#endif
      *rcode=OAPC_ERROR_NO_SUCH_IO;
      return _T("");
   }
   *rcode=OAPC_OK;
#ifndef ENV_HPLAYER
   if (m_userData) return m_userData->m_fullname;
   else
#endif
   if (lastInput == OAPC_DIGI_IO3) return _("default");
   return _T("");
}



wxByte flowLogInUser::setDigitalInput(FlowConnection *connection,wxByte value,wxUint32 *flowThreadID,flowObject *object)
{
   if (!flowObject::setDigitalInput(connection,value,flowThreadID,object)) return 0;
   if (connection->targetInputNum==2)
   {
      if (digi[2]==0) return 1;
#ifndef ENV_HPLAYER
      if (m_userData)
      {
         wxCommandEvent* evt = new wxCommandEvent();

         evt->SetEventType(wxEVT_COMMAND_MENU_SELECTED);
         evt->SetId(FLOW_LOGIN_USER_CHANGE_PWD);
         evt->SetEventObject(m_userData);
         g_hmiCanvas->AddPendingEvent(*evt);
         return 1;
      }
#endif
      return 0;
   }
   else if (connection->targetInputNum==3)
   {
      // log out the current user and set all elements to default state
      if (digi[3]==0) return 1;
#ifndef ENV_HPLAYER
      // m_userData=NULL; keep the user data since the user name input field may still contain it
      g_objectList.setUserPrivileges(NULL);
#endif
   }
   else return 0;
   return 1;
}



wxByte flowLogInUser::getDigitalOutput(FlowConnection *connection,wxInt32 *rcode,wxUint64 lastInput,wxLongLong WXUNUSED(origCreationTime))
{
   if (connection->sourceOutputNum!=1)
   {
#ifdef ENV_DEBUGGER
      g_debugWin->setDebugInformation(this,DEBUG_STOP_COND_ILLEGAL_IO,_T(""));
#endif
      *rcode=OAPC_ERROR_NO_SUCH_IO;
      return 0;
   }
   *rcode=OAPC_OK;
   if (lastInput == OAPC_DIGI_IO3) return 1; // in case of log out we return an OK
   if (m_loggedIn) return 1;
   else return 0;
}

#endif




