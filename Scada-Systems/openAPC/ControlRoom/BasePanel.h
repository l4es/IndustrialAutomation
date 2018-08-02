#ifndef OAPC_BASEPANEL_H
#define OAPC_BASEPANEL_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <wx/panel.h>
#include <wx/dcbuffer.h>
#include <wx/gbsizer.h>

#ifndef ENV_EDITOR
#include "flowWalkerPool.h"
#endif

class BasePanel : public wxPanel
{
public:
   BasePanel(wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, wxInt32 style = wxTAB_TRAVERSAL,wxString name=_T(""));
   BasePanel(wxWindow *parent,wxUint32 myID);
   virtual ~BasePanel();

   virtual void OnPaint( wxPaintEvent &event);
           void paintBackground(wxAutoBufferedPaintDC &dc);
           void paintBorder(wxAutoBufferedPaintDC &dc);
           void paintContents(wxAutoBufferedPaintDC &dc);
           void setBorderNum(wxInt32 borderNum);

   wxUint32        m_myID;
   wxGridBagSizer *m_gbSizer;

private:
   wxColor getLighterColour(wxColor colour,wxInt32 offset);

   wxInt32 m_borderNum,m_size_x,m_size_y;
};


#endif // OAPC_BASEPANEL_H
