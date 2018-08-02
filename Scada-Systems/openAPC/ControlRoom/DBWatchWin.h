#ifndef DBWatchWin_H
#define DBWatchWin_H

#include <wx/wx.h>
#include <wx/frame.h>
#include <wx/grid.h>


class DBWatchWin : public wxFrame
{
public:
   DBWatchWin(wxString projectName);
   ~DBWatchWin();

   void OnClose(wxCloseEvent& event);
   void setNodeInfo(char *nodeName,wxUint32 ios);

private:
   wxTextCtrl *m_list;
   wxString    m_projectName;
   void       *m_ispaceHandle;

   DECLARE_EVENT_TABLE()
};


#endif // griddemo_h

