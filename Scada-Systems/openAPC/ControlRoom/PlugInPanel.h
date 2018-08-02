#ifndef PLUGINPANEL_H
#define PLUGINPANEL_H

#include <wx/wx.h>
#include <wx/panel.h>
#include <wx/listctrl.h>
#include <wx/imaglist.h>

#include <vector>

class flowExternalIOLib;
class wxFoldPanelBar;

class PlugInPanel : public wxPanel
{
public:
   PlugInPanel(wxWindow *parent);
   ~PlugInPanel();

   void            addItem(flowExternalIOLib *object,wxUint32 capabilities);
   void            deleteItem(wxInt32 id);
   void            setItemName(wxInt32 id,wxString name);
   void            newProject();
   void            setEnabled(bool enabled);
   wxFoldPanelBar *createFoldBar(wxWindow *parent);
   void            OnFoldbarClicked(wxMouseEvent& event);
private:
   void            ShowContextMenu(wxContextMenuEvent &event);
   void            DoShowContextMenu(bool isNew);
   void            OnListEditEvent(wxListEvent& event);
   void            OnPopupMenu(wxCommandEvent &event);
   
   wxListCtrl          *m_item_list;
   wxImageList         *m_pImageList;
   wxInt32              m_imgCnt;
   std::vector<wxInt32> itemID;
   wxFoldPanelBar      *m_plugBar;

   DECLARE_EVENT_TABLE()
};
 
#endif
