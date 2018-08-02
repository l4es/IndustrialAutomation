#ifndef PLAYERMAINAPP_H
#define PLAYERMAINAPP_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifdef ENV_DEBUGGER
class DebugWin;
#endif



class PlayerMainApp : public wxAppConsole
{
public:
   virtual int  OnRun();
   virtual bool OnInit();
   virtual int  OnExit();
   virtual bool OnCmdLineParsed(wxCmdLineParser& parser);
   virtual void OnInitCmdLine(wxCmdLineParser& parser);

private:
   void     exitApp();

   wxString file;
};


#endif
