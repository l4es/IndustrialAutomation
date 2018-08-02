#ifndef UserData_H
#define UserData_H

class UserData : public wxObject
{
public:
   UserData(wxString login,wxString fullname,wxString pwd,wxUint32 canDo);
   UserData();
   ~UserData();

   wxString m_login,m_fullname,m_pwd,m_comment;
   wxUint32 m_canDo;              // flags describing what this user can do
   wxUint32 m_state;              // enabled/disabled state of the user itself
   wxUint32 m_res1,m_res2,m_res3; // reserved

private:
   void init();
};


#endif
