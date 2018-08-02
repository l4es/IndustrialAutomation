#ifndef MAINAPP_H
#define MAINAPP_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "MainWin.h"
#include "common/oapcResourceBundle.h"

class MainApp : public wxApp
{
public:
	virtual bool OnInit();
	virtual int  OnExit();
   virtual bool OnCmdLineParsed(wxCmdLineParser& parser);
   virtual void OnInitCmdLine(wxCmdLineParser& parser);
	        int  FilterEvent(wxEvent& event);

private:
    wxString          file;
};



#endif
