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


#include <wx/panel.h>
#include <wx/window.h>
#include <wx/dcbuffer.h>

#include "globals.h"
#include "BasePanel.h"
#include "hmiImage.h"
#include "hmiPrimitive.h"

BasePanel::BasePanel(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, wxInt32 style,wxString name)
          :wxPanel(parent,id,pos,size,style,name)
{
   SDBG
   m_myID=0;
   SetBackgroundStyle(wxBG_STYLE_CUSTOM);
   Connect(wxEVT_PAINT,wxPaintEventHandler(BasePanel::OnPaint),NULL,this);
   m_borderNum=-1;
   m_gbSizer=new wxGridBagSizer(1,1);
   m_gbSizer->SetDimension(0,0,size.x,size.y);
   m_gbSizer->SetMinSize(size.x,size.y);
   SetSizer(m_gbSizer);
}



BasePanel::BasePanel(wxWindow *parent,wxUint32 myID)
          :wxPanel(parent)
{
    SDBG
   m_myID=myID;
   SetBackgroundStyle(wxBG_STYLE_CUSTOM);
   Connect(wxEVT_PAINT,wxPaintEventHandler(BasePanel::OnPaint),NULL,this);
   m_borderNum=-1;
}



BasePanel::~BasePanel()
{
    SDBG
   if (m_myID==0) g_objectList.deleteObjects(m_myID,1);
}



void BasePanel::OnPaint(wxPaintEvent &WXUNUSED(event))
{
    wxAutoBufferedPaintDC dc( this );

    paintBackground(dc);
    paintContents(dc);
    paintBorder(dc);
}



void BasePanel::paintBackground(wxAutoBufferedPaintDC &dc)
{
#ifndef ENV_PLAYER
    wxInt32    cx,cy;
#endif

    dc.SetBackground(GetBackgroundColour());
    dc.Clear();
    m_size_x = 0;
    m_size_y = 0;
    GetClientSize(&m_size_x,&m_size_y);
#ifndef ENV_PLAYER
    dc.SetPen(*wxLIGHT_GREY_PEN);

    if (g_objectList.m_projectData)
    {
       for (cx=10; cx<m_size_x; cx+=g_objectList.m_projectData->gridW)
        for (cy=10; cy<m_size_y; cy+=g_objectList.m_projectData->gridH)
         dc.DrawPoint(cx,cy);
    }
    else
    {
       for (cx=10; cx<m_size_x; cx+=10)
        for (cy=10; cy<m_size_y; cy+=10)
         dc.DrawPoint(cx,cy);
    }
#endif
}



void BasePanel::paintContents(wxAutoBufferedPaintDC &dc)
{
   wxNode    *node;
   hmiObject *object;

   node=g_objectList.getDrawableObject(NULL);
   while (node)
   {
      object=(hmiObject*)node->GetData();
      if (object->data.isChildOf==m_myID) object->drawObject(&dc);
/*      if ((object->data.type==HMI_TYPE_IMAGE) && (object->data.isChildOf==m_myID))
      {
#ifdef ENV_PLAYER
         oapc_thread_mutex_lock(g_imagePaintMutex);
#endif
         if ((((hmiImage*)object)->bitmap->Ok()) && (((hmiImage*)object)->m_isVisible))
          dc.DrawBitmap(*((hmiImage*)object)->bitmap,((hmiImage*)object)->getPos().x/1000.0,((hmiImage*)object)->getPos().y/1000.0,true);
#ifdef ENV_PLAYER
         oapc_thread_mutex_unlock(g_imagePaintMutex);
#endif
      }
      else if ((object->data.type==HMI_TYPE_FREELINE) && (object->data.isChildOf==m_myID))
      {
         hmiPrimitive *line;

         line=dynamic_cast<hmiPrimitive*>(object);
         assert(line);
         if (line) line->drawObject(&dc);
      }*/
      node=g_objectList.getDrawableObject(node);
   }
}



wxColor BasePanel::getLighterColour(wxColor colour,wxInt32 offset)
{
   wxInt32 r,g,b;

   r=colour.Red()+offset;   if (r>255) r=255;
   g=colour.Green()+offset; if (g>255) g=255;
   b=colour.Blue()+offset;  if (b>255) b=255;
   return wxColor(r,g,b);
}



void BasePanel::paintBorder(wxAutoBufferedPaintDC &dc)
{
   if (m_borderNum==0) return;
   dc.SetPen(GetForegroundColour());
   dc.SetBrush(*wxTRANSPARENT_BRUSH);

   if (m_borderNum==1)
   {
      dc.DrawLine(m_size_x,0,0,0);      dc.DrawLine(0,0,0,m_size_y);
      dc.DrawLine(m_size_x-3,1,1,1);    dc.DrawLine(1,1,1,m_size_y-2);

      dc.SetPen(wxColour(getLighterColour(GetForegroundColour(),150)));
      dc.DrawLine(1,m_size_y-1,m_size_x-1,m_size_y-1); dc.DrawLine(m_size_x-1,m_size_y-1,m_size_x-1,0);
      dc.DrawLine(1,m_size_y-2,m_size_x-2,m_size_y-2); dc.DrawLine(m_size_x-2,m_size_y-2,m_size_x-2,0);
   }
   else if (m_borderNum==2)
   {
      dc.DrawLine(1,m_size_y-1,m_size_x-1,m_size_y-1); dc.DrawLine(m_size_x-1,m_size_y-1,m_size_x-1,0);
      dc.DrawLine(1,m_size_y-2,m_size_x-2,m_size_y-2); dc.DrawLine(m_size_x-2,m_size_y-2,m_size_x-2,0);

      dc.SetPen(wxColour(getLighterColour(GetForegroundColour(),150)));
      dc.DrawLine(m_size_x,0,0,0);      dc.DrawLine(0,0,0,m_size_y);
      dc.DrawLine(m_size_x-3,1,1,1);    dc.DrawLine(1,1,1,m_size_y-2);
   } 
   else if (m_borderNum==3) dc.DrawRectangle(0,0,m_size_x,m_size_y);
   else if (m_borderNum==4)
   {
      dc.DrawRectangle(0,0,m_size_x,m_size_y);
      dc.DrawRectangle(2,2,m_size_x-4,m_size_y-4);
   }
   else if (m_borderNum==5)
   {
#if wxCHECK_VERSION(2,9,0)
      dc.SetPen(wxPen(GetForegroundColour(),1,wxPENSTYLE_DOT));
#else
      dc.SetPen(wxPen(GetForegroundColour(),1,wxDOT));
#endif
      dc.DrawRectangle(0,0,m_size_x,m_size_y);
   }
   else if (m_borderNum==6)
   {
#if wxCHECK_VERSION(2,9,0)
      dc.SetPen(wxPen(GetForegroundColour(),1,wxPENSTYLE_SHORT_DASH));
#else
      dc.SetPen(wxPen(GetForegroundColour(),1,wxSHORT_DASH));
#endif
      dc.DrawRectangle(0,0,m_size_x,m_size_y);
   }
   else if (m_borderNum==7)
   {
#if wxCHECK_VERSION(2,9,0)
      dc.SetPen(wxPen(GetForegroundColour(),1,wxPENSTYLE_DOT_DASH));
#else
      dc.SetPen(wxPen(GetForegroundColour(),1,wxDOT_DASH));
#endif
      dc.DrawRectangle(0,0,m_size_x,m_size_y);
   }
   else if (m_borderNum==8)
   {
#if wxCHECK_VERSION(2,9,0)
      dc.SetPen(wxPen(GetForegroundColour(),1,wxPENSTYLE_LONG_DASH));
#else
      dc.SetPen(wxPen(GetForegroundColour(),1,wxLONG_DASH));
#endif
      dc.DrawRectangle(0,0,m_size_x,m_size_y);
   }
}




void BasePanel::setBorderNum(wxInt32 borderNum)
{
    SDBG
   m_borderNum=borderNum;
}

