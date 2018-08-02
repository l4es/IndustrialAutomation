#ifndef PLAYERMAINAPP_H
#define PLAYERMAINAPP_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifdef ENV_DEBUGGER
class DebugWin;
#endif

#include "oapcResourceBundle.h"

class PlayerMainApp : public wxApp
{
public:
   virtual bool OnInit();
   virtual int  OnExit();
   virtual bool OnCmdLineParsed(wxCmdLineParser& parser);
   virtual void OnInitCmdLine(wxCmdLineParser& parser);

private:
   void     exitApp();

   wxString file;
#ifndef ENV_DEBUGGER
   long     m_posX,m_posY;
   bool     m_titleBar;
#endif
};


#endif
