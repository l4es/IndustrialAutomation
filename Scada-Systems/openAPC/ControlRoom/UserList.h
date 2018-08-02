#ifndef USERLIST_H
#define USERLIST_H

#include "UserData.h"

#include <list>

class UserList : public wxObject
{
public:
   UserList();
   ~UserList();

   UserData *getUserData(bool fromStart);
   UserData *getUserDataByLogin(wxString login);
   void      addUserData(UserData *userData);
   void      deleteAll();
   void      deleteUser(UserData *userData);
   wxInt32   getCount();

private:
#ifdef ENV_EDITOR
#ifndef ENV_BEAM
   void menuEnable(bool doEnable);
#endif
#endif

   std::list<UserData*> m_userList;
};


#endif
