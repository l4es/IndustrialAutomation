#ifndef WATCHWIN_H
#define WATCHWIN_H

#include <wx/wx.h>
#include <wx/frame.h>
#include <wx/grid.h>
#include "oapcBinHead.h"

class flowObject;


class WatchListElement : public wxObject
{
public:
   WatchListElement() {m_id=-1; m_object=NULL;}
	wxInt32     m_id;
   flowObject *m_object;
};



class WatchWin : public wxFrame
{
public:
   WatchWin(wxString projectName);
   ~WatchWin();

   void OnClose(wxCloseEvent& event);
   void OnCellLeftDClick( wxGridEvent& );
   void OnCellValueChanged( wxGridEvent& );
   void OnCellLeftClick( wxGridEvent& );
   void OnEditorShown( wxGridEvent& ev );

   void setNewValue(wxInt32 outputNum,wxByte digi,wxInt32 id);
   void setNewValue(wxInt32 outputNum,wxFloat64 num,wxInt32 id);
   void setNewValue(wxInt32 outputNum,wxString txt,wxInt32 id);
   void setNewValue(wxInt32 outputNum,oapcBinHeadSp &bin,wxInt32 id);

   DECLARE_EVENT_TABLE()
private:
   void    createNewCol(wxInt32 id,wxInt32 height);
   void    updateRowData(wxInt32 rowNum,bool highlightChanges);
   wxInt32 getRowFromID(wxInt32 id); 
   
   wxList   m_watchList;
   wxInt32  m_nextNewRowNum;
   wxGrid  *grid;
   wxString m_projectName;
};


#endif // griddemo_h

