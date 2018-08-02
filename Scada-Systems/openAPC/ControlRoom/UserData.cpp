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

#include "UserData.h"



UserData::UserData()
{
   init();
}



UserData::UserData(wxString login,wxString fullname,wxString pwd,wxUint32 canDo)
{
   init();
   m_canDo=canDo;
   m_login=login;
   m_fullname=fullname;
   m_pwd=pwd;
}



void UserData::init()
{
   m_canDo=0;
   m_state=0;
   m_res1=0; m_res2=0; m_res3=0;
}



UserData::~UserData()
{
}


