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
#include <wx/dcbuffer.h>
#include <wx/filename.h>
#include <wx/window.h>

#include <wx/foldbar/foldpanelbar.h>

#ifdef ENV_DEBUGGER
#include "DebugWin.h"
#endif

#include "globals.h"
#include "MainApp.h"

#ifndef ENV_PLAYER
#include "MainWin.h"
#include "DlgObjectProp.h"
#endif

#include "ChangePwdDlg.h"
#include "oapc_libio.h"
#include "BasePanel.h"
#include "PCompiler.h"
#include "hmiObject.h"
#include "flowObject.h"
#include "hmiSimpleButton.h"
#include "hmiImageButton.h"
#include "hmiToggleImageButton.h"
#include "hmiToggleButton.h"
#include "hmiRadioButton.h"
#include "hmiHSlider.h"
#include "hmiVSlider.h"
#include "hmiHGauge.h"
#include "hmiVGauge.h"
#include "hmiNumField.h"
#include "hmiTextField.h"
#include "hmiFloatField.h"
#include "hmiTextLabel.h"
#include "hmiLine.h"
#include "hmiImage.h"
#include "hmiPrimitive.h"
#include "hmiTabbedPane.h"
#include "hmiStackedPane.h"
#include "hmiAdditionalPane.h"
#include "hmiSinglePanel.h"
#include "hmiAngularMeter.h"
#include "hmiAngularRegulator.h"
#include "hmiLCDNumber.h"
#include "HMICanvas.h"
#include "ObjectList.h"
#include "ExternalIOLib.h"
#include "hmiSpecialPanel.h"
#include "hmiExternalIOLib.h"
#include "hmiUserMgmntPanel.h"
#ifdef ENV_EDITOR
 #include "DlgControlSelector.h"
#else
 #include "flowWalkerPool.h"
#endif


//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

#ifndef ENV_PLAYER
 #define POPUP_NEW_SIMPLE_BUTTON        0x1001
 #define POPUP_NEW_TOGGLE_BUTTON        0x1002
 #define POPUP_NEW_VSLIDER              0x1003
 #define POPUP_NEW_HSLIDER              0x1004
 #define POPUP_NEW_IMAGE_BUTTON         0x1005
 #define POPUP_NEW_VGAUGE               0x1006
 #define POPUP_NEW_HGAUGE               0x1007
 #define POPUP_NEW_NUMFIELD             0x1008
 #define POPUP_NEW_FLOATFIELD           0x1009
 #define POPUP_NEW_TEXTFIELD            0x100A
 #define POPUP_NEW_TABBEDPANE           0x100B
 #define POPUP_NEW_STACKEDPANE          0x100C
 #define POPUP_NEW_SINGLEPANEL          0x100D
 #define POPUP_NEW_TOGGLE_IMAGE_BUTTON  0x100E
 #define POPUP_NEW_AMETER               0x100F
 #define POPUP_NEW_LCDNUMBER            0x1010
 #define POPUP_NEW_AREGULATOR           0x1011
 #define POPUP_NEW_ADDITIONALPANE       0x1012
 #define POPUP_NEW_RADIO_BUTTON         0x1013
 #define POPUP_NEW_CHECKBOX             0x1014
 #define POPUP_NEW_USERMANAGEMENT_PANEL 0x1015
 #define POPUP_NEW_PASSWORDFIELD        0x1016

 #define POPUP_NEW_LINE          0x1101
 #define POPUP_NEW_TEXTLABEL     0x1102
 #define POPUP_NEW_IMAGE         0x1103
 #define POPUP_NEW_FREELINE      0x1104
 #define POPUP_NEW_FRAME         0x1105
 #define POPUP_NEW_ELLIPSE       0x1106

 #define POPUP_DEL_CONTROL          wxID_DELETE
 #define POPUP_EDIT_CONTROL         wxID_EDIT
 #define POPUP_SNAP_CONTROL_TO_GRID wxID_JUSTIFY_CENTER
 #define POPUP_DUP_CONTROL          wxID_DUPLICATE
 #define POPUP_MAX_CONTROL          wxID_ZOOM_100
 #define POPUP_SELECT_CONTROL       0x1201
 #define POPUP_NEXT_PANE            wxID_FORWARD
 #define POPUP_PREV_PANE            wxID_BACKWARD
 #define POPUP_CUT_CONTROL          wxID_CUT
 #define POPUP_INS_CONTROL          wxID_PASTE
 #define POPUP_ELEMENT_TO_TOP       0x1202
 #define POPUP_ELEMENT_TO_BOTTOM    0x1203
#endif

#define BUTTON_FOLDBAR          0x2001

//IMPLEMENT_CLASS(HMICanvas, BasePanel)

BEGIN_EVENT_TABLE(HMICanvas, BasePanel)
#ifndef ENV_PLAYER
  EVT_CONTEXT_MENU(HMICanvas::ShowContextMenu)
  EVT_MOUSE_EVENTS(HMICanvas::OnMouseEvent)
  EVT_MENU(POPUP_NEW_SIMPLE_BUTTON,HMICanvas::OnNewControl)
  EVT_MENU(POPUP_NEW_IMAGE_BUTTON,HMICanvas::OnNewControl)
  EVT_MENU(POPUP_NEW_TOGGLE_IMAGE_BUTTON,HMICanvas::OnNewControl)
  EVT_MENU(POPUP_NEW_IMAGE,HMICanvas::OnNewControl)
  EVT_MENU(POPUP_NEW_FREELINE,HMICanvas::OnNewControl)
  EVT_MENU(POPUP_NEW_FRAME,HMICanvas::OnNewControl)
  EVT_MENU(POPUP_NEW_ELLIPSE,HMICanvas::OnNewControl)
  EVT_MENU(POPUP_NEW_TOGGLE_BUTTON,HMICanvas::OnNewControl)
  EVT_MENU(POPUP_NEW_RADIO_BUTTON,HMICanvas::OnNewControl)
  EVT_MENU(POPUP_NEW_CHECKBOX,HMICanvas::OnNewControl)
  EVT_MENU(POPUP_NEW_VSLIDER,HMICanvas::OnNewControl)
  EVT_MENU(POPUP_NEW_HSLIDER,HMICanvas::OnNewControl)
  EVT_MENU(POPUP_NEW_VGAUGE,HMICanvas::OnNewControl)
  EVT_MENU(POPUP_NEW_AMETER,HMICanvas::OnNewControl)
  EVT_MENU(POPUP_NEW_AREGULATOR,HMICanvas::OnNewControl)
  EVT_MENU(POPUP_NEW_LCDNUMBER,HMICanvas::OnNewControl)
  EVT_MENU(POPUP_NEW_NUMFIELD,HMICanvas::OnNewControl)
  EVT_MENU(POPUP_NEW_TEXTFIELD,HMICanvas::OnNewControl)
  EVT_MENU(POPUP_NEW_PASSWORDFIELD,HMICanvas::OnNewControl)
  EVT_MENU(POPUP_NEW_TABBEDPANE,HMICanvas::OnNewControl)
  EVT_MENU(POPUP_NEW_STACKEDPANE,HMICanvas::OnNewControl)
  EVT_MENU(POPUP_NEW_SINGLEPANEL,HMICanvas::OnNewControl)
  EVT_MENU(POPUP_NEW_ADDITIONALPANE,HMICanvas::OnNewControl)
  EVT_MENU(POPUP_NEW_FLOATFIELD,HMICanvas::OnNewControl)
  EVT_MENU(POPUP_NEW_HGAUGE,HMICanvas::OnNewControl)
  EVT_MENU(POPUP_NEW_TEXTLABEL,HMICanvas::OnNewControl)
  EVT_MENU(POPUP_NEW_LINE,HMICanvas::OnNewControl)
  EVT_MENU(POPUP_NEW_USERMANAGEMENT_PANEL,HMICanvas::OnNewControl)
  EVT_MENU(POPUP_DEL_CONTROL,HMICanvas::OnDelControl)
  EVT_MENU(POPUP_DUP_CONTROL,HMICanvas::OnDupControl)
  EVT_MENU(POPUP_EDIT_CONTROL,HMICanvas::OnEditControl)
  EVT_MENU(POPUP_MAX_CONTROL,HMICanvas::OnMaxControl)
  EVT_MENU(POPUP_SELECT_CONTROL,HMICanvas::OnSelectControl)
  EVT_MENU(POPUP_SNAP_CONTROL_TO_GRID,HMICanvas::OnSnapControlToGrid)
  EVT_MENU(POPUP_ELEMENT_TO_TOP,HMICanvas::OnPopupMenu)
  EVT_MENU(POPUP_ELEMENT_TO_BOTTOM,HMICanvas::OnPopupMenu)
  EVT_MENU(POPUP_PREV_PANE,HMICanvas::OnPopupMenu)
  EVT_MENU(POPUP_NEXT_PANE,HMICanvas::OnPopupMenu)
  EVT_MENU(POPUP_CUT_CONTROL,HMICanvas::OnPopupMenu)
  EVT_MENU(POPUP_INS_CONTROL,HMICanvas::OnPopupMenu)
#else
  EVT_MENU(FLOW_LOGIN_USER_CHANGE_PWD,HMICanvas::OnExternalEvent)
 #ifdef ENV_WINDOWS
  EVT_HOTKEY(1,HMICanvas::OnHotKey)
  EVT_HOTKEY(2,HMICanvas::OnHotKey)
  EVT_HOTKEY(3,HMICanvas::OnHotKey)
  EVT_HOTKEY(4,HMICanvas::OnHotKey)
  EVT_HOTKEY(5,HMICanvas::OnHotKey)
  EVT_HOTKEY(6,HMICanvas::OnHotKey)
 #endif
#endif
END_EVENT_TABLE()


#ifndef ENV_PLAYER
#define SELECT_MODE_NONE     0
#define SELECT_MODE_DRAG     1
#define SELECT_MODE_TOPLEFT  2
#define SELECT_MODE_TOPRIGHT 3
#define SELECT_MODE_BOTLEFT  4
#define SELECT_MODE_BOTRIGHT 5
#define SELECT_MODE_BOTTOM   6
#define SELECT_MODE_RIGHT    7
#endif



HMICanvas::HMICanvas(wxScrolledWindow *parent,
            wxWindowID winid,
            const wxPoint& pos,
            const wxSize& size,
            wxInt32 style,
            const wxString& name)
          :BasePanel(parent, winid,pos,size,style,name)
{
    SDBG
#ifdef ENV_EDITOR
   m_lastElementID=0;
#endif
   m_owner=parent;
   m_myID=0;
   init();
}



HMICanvas::HMICanvas(wxWindow *parent,wxUint32 myID,wxInt32 style)
          :BasePanel(parent,myID,wxDefaultPosition,wxDefaultSize,style)
{
    SDBG
#ifdef ENV_EDITOR
   m_lastElementID=0;
#endif
   m_owner=NULL;
   m_myID=myID;
   init();
}

#ifndef ENV_HPLAYER
 #if wxCHECK_VERSION(2,9,0)
   void HMICanvas::AddPendingEvent(const wxEvent& event)
   {
       BasePanel::AddPendingEvent(event);
   }
 #endif
#endif


void HMICanvas::init()
{
   SDBG
   if (m_myID==0) g_objectList.setParent(this);
#ifdef ENV_PLAYER
   if (!g_externalIOLibs) g_externalIOLibs=new ExternalIOLibs();
   if (!g_externalHMILibs) g_externalHMILibs=new ExternalIOLibs();
 #ifdef ENV_DEBUGGER
   displayNames=0;
 #endif
 #ifdef ENV_WINDOWS
   RegisterHotKey(1,wxMOD_ALT,WXK_TAB);
   RegisterHotKey(2,wxMOD_ALT|wxMOD_CONTROL,WXK_DELETE);
   RegisterHotKey(3,wxMOD_ALT|wxMOD_CONTROL,WXK_BACK);
   RegisterHotKey(4,wxMOD_WIN,0x45); // explorer
   RegisterHotKey(5,wxMOD_CONTROL|wxMOD_SHIFT,WXK_ESCAPE); // taskmanager
   RegisterHotKey(6,wxMOD_WIN,0x4C); // log off
  #endif
#else
   if (!g_externalIOLibs) g_externalIOLibs=new ExternalIOLibs(_T("flowplugins"),FLOW_TYPE_MASK,NULL);
   if (!g_externalHMILibs) g_externalHMILibs=new ExternalIOLibs(_T("hmiplugins"),HMI_TYPE_MASK,NULL);
   refreshProject();
   selectMode=SELECT_MODE_NONE;
   mainWin=NULL;
   rectStart.x=-1;
   rectEnd.x=-1;
   g_isEmpty=true;
   highlightObject=NULL;
#endif
   SetFocus();
//   SetBackgroundStyle(wxBG_STYLE_CUSTOM);
}



HMICanvas::~HMICanvas()
{
    SDBG
#ifndef ENV_PLAYER
   g_selectedList.deleteAll(0);
#endif
}



#ifdef ENV_EDITOR
int HMICanvas::OnKeyDown(wxKeyEvent& event)
{
   wxCommandEvent cmdEvent;

   if ((event.m_keyCode==WXK_F2) && (m_lastElementID!=0))
   {
      cmdEvent.SetId(m_lastElementID);
      OnNewControl(cmdEvent);
      return 1;
   }
   else if (event.ShiftDown())
   {
      if (event.m_keyCode==WXK_INSERT)
      {
         cmdEvent.SetId(POPUP_DUP_CONTROL);
         OnDupControl(cmdEvent);
         return 1;
      }
      else if (event.m_keyCode==WXK_DELETE)
      {
         cmdEvent.SetId(POPUP_DEL_CONTROL);
         OnDelControl(cmdEvent);
         return 1;
      }
   }
   return -1;
}



void HMICanvas::displayName(wxString name)
{
   displayedName=name;
}



void HMICanvas::showElement(hmiObject *object)
{
   if (object->data.isChildOf!=m_myID)
   {
      hmiTabbedPane *basePane=(hmiTabbedPane*)g_objectList.getObject(object->data.isChildOf);
      wxASSERT(basePane);
      if (!basePane) return;
//      m_owner->Scroll((basePane->getPos().x-160)/10,(basePane->getPos().y-160)/10);
      highlightObject=NULL;
      ((HMICanvas*)basePane->getPanel())->showElement(object);
   }
   else
   {
      if (m_owner) m_owner->Scroll((object->getPos().x/1000.0-160)/10,(object->getPos().y/1000.0-160)/10);
      highlightObject=object;
   }
   Refresh();
}



void HMICanvas::OnChildMouseEvent(wxMouseEvent& event)
{
    hmiObject *object;
    wxInt32 id=event.GetId();

    object=g_objectList.getUIElement(id,m_myID,event.GetPosition());
    if (object)
    {
       wxRealPoint pos;

       if (((event.Moving()) || (event.Dragging())) &&
           (m_myID!=0) &&
           (object->data.isChildOf==m_myID)) return; // do not forward mouse positions with offset when the position data come from the dragged element

       pos.x=event.GetPosition().x;
       pos.y=event.GetPosition().y;
       event.m_x=pos.x+object->getPos().x/1000.0;
       event.m_y=pos.y+object->getPos().y/1000.0;
       OnMouseEvent(event);
    }
}



void HMICanvas::OnMouseEvent(wxMouseEvent& event)
{
   static bool lastWasLeftDown=false;

   lastMousePos=event.GetPosition();
   if ((lastMousePos.x<0) || (lastMousePos.y<0)) return;
   if (event.Dragging())
   {
      lastWasLeftDown=false;
      if ((g_selectedList.GetCount()>0) && (g_selectedList.getFirstObject()->data.isChildOf!=m_myID))
       g_selectedList.deleteAll(0);
      if (g_selectedList.GetCount()>0) switch (selectMode)
      {
         case SELECT_MODE_TOPLEFT:
         case SELECT_MODE_BOTRIGHT:
         case SELECT_MODE_TOPRIGHT:
         case SELECT_MODE_BOTLEFT:
         case SELECT_MODE_BOTTOM:
         case SELECT_MODE_RIGHT:
         {
             wxRealPoint size,prevSize;
             wxPoint     minSize,maxSize;
             wxRealPoint pos;
             wxInt32     dx,dy;

             size=g_selectedList.getSingleSize();
             minSize=g_selectedList.getSingleMinSize();
             maxSize=g_selectedList.getSingleMaxSize();
             prevSize=g_selectedList.getSingleSize();
             pos=g_selectedList.getTotalPos();
             pos.x/=1000.0;
             pos.y/=1000.0;
             dx=lastMousePos.x-startMousePos.x;
             dy=lastMousePos.y-startMousePos.y;

             switch (selectMode)
             {
                case SELECT_MODE_TOPLEFT:
                    size.x-=dx;
                    if (size.x<minSize.x) size.x=minSize.x;
                    else if (size.x>maxSize.x) size.x=maxSize.x;
                    size.y-=dy;
                    if (size.y<minSize.y) size.y=minSize.y;
                    else if (size.y>maxSize.y) size.y=maxSize.y;
                    pos.x-=size.x-prevSize.x;
                    pos.y-=size.y-prevSize.y;
                    break;
                case SELECT_MODE_BOTRIGHT:
                    wxFloat32 factor;

                    factor=(size.x+dx)/size.x;

                    size.x+=dx;
                    if (size.x<minSize.x) size.x=minSize.x;
                    else if (size.x>maxSize.x) size.x=minSize.x;
                    size.y*=factor;
                    if (size.y<minSize.y) return;
                    if (size.y>maxSize.y) return;
                    break;
                case SELECT_MODE_TOPRIGHT:
                    size.x+=dx;
                    if (size.x<minSize.x) size.x=minSize.x;
                    else if (size.x>maxSize.x) size.x=maxSize.x;
                    size.y-=dy;
                    if (size.y<minSize.y) size.y=minSize.y;
                    else if (size.y>maxSize.y) size.y=maxSize.y;
                    pos.y-=size.y-prevSize.y;
                    break;
                case SELECT_MODE_BOTLEFT:
                    size.x-=dx;
                    if (size.x<minSize.x) size.x=minSize.x;
                    else if (size.x>maxSize.x) size.x=maxSize.x;
                    size.y+=dy;
                    if (size.y<minSize.y) size.y=minSize.y;
                    else if (size.y>maxSize.y) size.y=maxSize.y;
                    pos.x-=size.x-prevSize.x;
                    break;
                case SELECT_MODE_BOTTOM:
                    size.y+=dy;
                    if (size.y<minSize.y) size.y=minSize.y;
                    else if (size.y>maxSize.y) size.y=maxSize.y;
                    break;
                case SELECT_MODE_RIGHT:
                    size.x+=dx;
                    if (size.x<minSize.x) size.x=minSize.x;
                    else if (size.x>maxSize.x) size.x=maxSize.x;
                    break;
             }
             g_selectedList.setSingleSize(size);
             g_selectedList.setTotalPos(pos);
             Refresh();
             g_isSaved=false;
             break;
          }
          case SELECT_MODE_DRAG:
          {
             wxRealPoint pos,size;

             pos=g_selectedList.getTotalPos();
             size=g_selectedList.getTotalSize();
             pos.x/=1000.0;  pos.y/=1000.0;
             size.x/=1000.0; size.y/=1000.0;
             pos.x+=(lastMousePos.x-startMousePos.x);
             pos.y+=(lastMousePos.y-startMousePos.y);

             if (pos.x<0) pos.x=0;
             else if (pos.x+size.x>g_objectList.m_projectData->totalW) pos.x=g_objectList.m_projectData->totalW-size.x;
             if (pos.y<0) pos.y=0;
             else if (pos.y+size.y>g_objectList.m_projectData->totalH) pos.y=g_objectList.m_projectData->totalH-size.y;

             g_selectedList.setTotalPos(pos);
             Refresh();
             g_isSaved=false;
             break;
          }
          default:
             if (rectStart.x>=0)
             {
                rectEnd=lastMousePos;
                Refresh();
             }
             break;
       }
       else if (rectStart.x>=0)
       {
          rectEnd=lastMousePos;
          Refresh();
       }

       startMousePos=lastMousePos;
    }
    else if (event.Moving())
    {
        if ((g_selectedList.GetCount()>0) && (g_selectedList.getFirstObject()->data.msizex>=0))
        {
           wxRect     rect;
           hmiObject *firstObject;

           firstObject=(hmiObject*)g_selectedList.getFirstObject();

           rect=g_selectedList.getBorder(9);
           if (((firstObject->noUIFlags & OAPC_HMI_NO_FREE_SIZE_RATIO)==0) &&
               ((firstObject->noUIFlags & OAPC_HMI_NO_SIZE)==0) &&
               (lastMousePos.x>=rect.x) &&                     (lastMousePos.x<=rect.x+10) &&
               (lastMousePos.y>=rect.y) &&                     (lastMousePos.y<=rect.y+10) &&
               (g_selectedList.GetCount()==1))
           {
              wxSetCursor(wxCursor(wxCURSOR_SIZENWSE));
              selectMode=SELECT_MODE_TOPLEFT;
           }
           else if (((firstObject->noUIFlags & OAPC_HMI_NO_SIZE)==0) &&
                    (lastMousePos.x>=rect.x+rect.width-10) &&  (lastMousePos.x<=rect.x+rect.width) &&
                    (lastMousePos.y>=rect.y+rect.height-10) && (lastMousePos.y<=rect.y+rect.height) && (g_selectedList.GetCount()==1)) // scale while keeping aspect ratios
           {
              wxSetCursor(wxCursor(wxCURSOR_SIZENWSE));
              selectMode=SELECT_MODE_BOTRIGHT;
           }

           else if (((firstObject->noUIFlags & OAPC_HMI_NO_SIZE)==0) &&
                    ((firstObject->noUIFlags & OAPC_HMI_NO_FREE_SIZE_RATIO)==0) &&
                    (lastMousePos.x>=rect.x) &&                (lastMousePos.x<=rect.x+10) &&
                    (lastMousePos.y>=rect.y+rect.height-10) && (lastMousePos.y<=rect.y+rect.height) &&
                    (g_selectedList.GetCount()==1))
           {
              wxSetCursor(wxCursor(wxCURSOR_SIZENESW));
              selectMode=SELECT_MODE_BOTLEFT;
           }
           else if (((firstObject->noUIFlags & OAPC_HMI_NO_SIZE)==0) &&
                    ((firstObject->noUIFlags & OAPC_HMI_NO_FREE_SIZE_RATIO)==0) &&
                    (lastMousePos.x>=rect.x+rect.width-10) &&  (lastMousePos.x<=rect.x+rect.width) &&
                    (lastMousePos.y>=rect.y) &&                (lastMousePos.y<=rect.y+10) &&
                    (g_selectedList.GetCount()==1))
           {
              wxSetCursor(wxCursor(wxCURSOR_SIZENESW));
              selectMode=SELECT_MODE_TOPRIGHT;
           }
           else if (((firstObject->noUIFlags & OAPC_HMI_NO_SIZE)==0) &&
                    ((firstObject->noUIFlags & OAPC_HMI_NO_FREE_SIZE_RATIO)==0) &&
                    (lastMousePos.x>=rect.x+rect.width-10) &&   (lastMousePos.x<=rect.x+rect.width) &&
                    (lastMousePos.y>=rect.y+rect.height/2-5) && (lastMousePos.y<=rect.y+rect.height/2+5) &&
                    (g_selectedList.GetCount()==1))
           {
              wxSetCursor(wxCursor(wxCURSOR_SIZEWE));
              selectMode=SELECT_MODE_RIGHT;
           }
           else if (((firstObject->noUIFlags & OAPC_HMI_NO_SIZE)==0) &&
                    ((firstObject->noUIFlags & OAPC_HMI_NO_FREE_SIZE_RATIO)==0) &&
                    (lastMousePos.x>=rect.x+rect.width/2-5) &&  (lastMousePos.x<=rect.x+rect.width/2+5) &&
                    (lastMousePos.y>=rect.y+rect.height-10) &&  (lastMousePos.y<=rect.y+rect.height) &&
                    (g_selectedList.GetCount()==1))
           {
              wxSetCursor(wxCursor(wxCURSOR_SIZENS));
              selectMode=SELECT_MODE_BOTTOM;
           }


           else if ((lastMousePos.x>=rect.x) && (lastMousePos.x<=rect.x+rect.width) &&
               (lastMousePos.y>=rect.y) && (lastMousePos.y<=rect.y+rect.height))
           {
              wxSetCursor(wxCursor(wxCURSOR_HAND));
              selectMode=SELECT_MODE_DRAG;
           }
           else
           {
              wxSetCursor(wxCursor(wxCURSOR_ARROW));
              selectMode=SELECT_MODE_NONE;
           }
        }
        else
        {
           wxSetCursor(wxCursor(wxCURSOR_ARROW));
           selectMode=SELECT_MODE_NONE;
        }
    }
    else if (event.RightUp())
    {
       wxInt32    id=event.GetId();
       hmiObject *newObject;

       highlightObject=NULL;
       newObject=g_objectList.getUIElement(id,m_myID,event.GetPosition());
       if ((newObject) && (!g_selectedList.Find(newObject)))
       {
          if ((!event.ShiftDown()) && (!event.MetaDown())) g_selectedList.deleteAll(0);
          if (!g_selectedList.getFirstObject()) g_selectedList.addObject(newObject,true,false); // list is empty so adding is not a problem
          else if (g_selectedList.getFirstObject()->data.isChildOf==m_myID) // object belongs to same panel so adding should be ok
          {
             if ((newObject->data.msizex>=0) && // a non-layouted object, it can be added to a group of selected elements
                 (g_selectedList.getFirstObject()->data.msizex>=0)) // the first element is not a layouted object so grouping can be done in general
              g_selectedList.addObject(newObject,true,false);
          }
          if (newObject) Refresh();
       }
       if ((newObject) && (newObject->data.type==HMI_TYPE_ADDITIONALPANE))
       {
          lastMousePos.y-=newObject->getPos().y/1000.0;
          lastMousePos.x-=newObject->getPos().x/1000.0;
       }
//       ShowContextMenu();
    }
    else if (event.LeftDown())
    {
       startMousePos=lastMousePos;
       wxInt32    id=event.GetId();
       hmiObject *newObject=NULL;

       lastWasLeftDown=true;
       highlightObject=NULL;
       rectStart.x=-1;
       rectEnd.x=-1;
       newObject=g_objectList.getUIElement(id,m_myID,event.GetPosition());
       if ((newObject) && (!g_selectedList.Find(newObject)))
       {
          if (newObject)
          {
             if ((!event.ShiftDown()) && (!event.MetaDown())) g_selectedList.deleteAll(0); // flush the current selection when no shift/ctrl-key is pressed
             if (!g_selectedList.getFirstObject()) g_selectedList.addObject(newObject,true,false); // list is empty so adding is not a problem
             else if (g_selectedList.getFirstObject()->data.isChildOf==m_myID) // object belongs to same panel so adding should be ok
             {
                if ((newObject->data.msizex>=0) && // a non-layouted object, it can be added to a group of selected elements
                    (g_selectedList.getFirstObject()->data.msizex>=0)) // the first element is not a layouted object so grouping can be done in general
                 g_selectedList.addObject(newObject,true,false);
             }
             if (newObject) Refresh();
             lastWasLeftDown=false;
          }
       }
       if ((!newObject) && 
           ((selectMode==SELECT_MODE_NONE) || (selectMode==SELECT_MODE_DRAG))) rectStart=lastMousePos;
       if ((newObject) && (newObject->data.type==HMI_TYPE_TABBEDPANE)) event.Skip();
       if ((newObject) && (newObject->data.type==HMI_TYPE_STACKEDPANE)) event.Skip();
       switch (selectMode)
       {
          case SELECT_MODE_TOPLEFT:
          case SELECT_MODE_BOTRIGHT:
          case SELECT_MODE_TOPRIGHT:
          case SELECT_MODE_BOTLEFT:
          case SELECT_MODE_BOTTOM:
          case SELECT_MODE_RIGHT:
             g_mainWin->addUndoStep(_("Scale"));
             break;
          case SELECT_MODE_DRAG:
             g_mainWin->addUndoStep(_("Move"));
             break;
          default:
             break;
       }
    }
    else if (event.LeftUp())
    {
       if (g_objectList.m_projectData->flags & PROJECT_FLAG_SNAPTOGRID)
       {
          wxFloat32  x,y;

          x=g_selectedList.getTotalPos().x/1000.0;
          y=g_selectedList.getTotalPos().y/1000.0;
          x=OAPC_ROUND(x/g_objectList.m_projectData->gridW,0)*g_objectList.m_projectData->gridW;
          y=OAPC_ROUND(y/g_objectList.m_projectData->gridH,0)*g_objectList.m_projectData->gridH;
          g_selectedList.setTotalPos(wxRealPoint(x,y));

/*          wxNode    *node;
          hmiObject *object;
          wxFloat32  x,y;

          node=g_selectedList.getObject((wxNode*)NULL);
          while (node)
          {
             object=(hmiObject*)node->GetData();
             x=object->getPos().x;
             y=object->getPos().y;
             x=OAPC_ROUND(x/g_objectList.m_projectData->gridW,0)*g_objectList.m_projectData->gridW;
             y=OAPC_ROUND(y/g_objectList.m_projectData->gridH,0)*g_objectList.m_projectData->gridH;
             object->setPos(wxRealPoint(x,y));

             node=g_selectedList.getObject(node);
          }*/
       }
       if ((g_selectedList.GetCount()==1)) g_selectedList.getFirstObject()->applyData(0); //1);
       if ((rectStart.x>=0) && (rectEnd.x>=0))
       {
          wxList::compatibility_iterator node;
          flowObject                    *object;
          wxRect                         selRect,objRect;
          wxInt32 xchg;

          if (rectEnd.x<rectStart.x)
          {
             xchg=rectEnd.x;
             rectEnd.x=rectStart.x;
             rectStart.x=xchg;
          }
          if (rectEnd.y<rectStart.y)
          {
             xchg=rectEnd.y;
             rectEnd.y=rectStart.y;
             rectStart.y=xchg;
          }

          if ((!event.ShiftDown()) && (!event.MetaDown())) g_selectedList.deleteAll(0); // flush the current selection when no shift/ctrl-key is pressed
          // find all elements that are inside the rectangle and add them to the selected objects list

          selRect=wxRect(rectStart.x*1000,rectStart.y*1000,(rectEnd.x-rectStart.x)*1000,(rectEnd.y-rectStart.y)*1000);
          node=g_objectList.getObject((wxList::compatibility_iterator)NULL);
          while (node)
          {
             object=(flowObject*)node->GetData();
             if ((object->data.isChildOf==m_myID) &&
                 (object->data.type & HMI_TYPE_MASK) &&
                 (object->data.type!=HMI_TYPE_ADDITIONALPANE))
             {
                objRect=wxRect(((hmiObject*)object)->getPos().x,((hmiObject*)object)->getPos().y,
                               ((hmiObject*)object)->getSize().x,((hmiObject*)object)->getSize().y);
                if (objRect.width==0) objRect.width=1;
                if (objRect.height==0) objRect.height=1;
                if ((selRect.Intersects(objRect)) && (object->data.msizex>=0)) // don't group-select layouted elements
                 g_selectedList.addObject(object,true,false);
             }
             node=g_objectList.getObject(node);
          }
          rectStart.x=-1;
          rectEnd.x=-1;
       }
       else if ((lastWasLeftDown) && (g_nextHMILeftClickOperation) && (selectMode==SELECT_MODE_NONE))
       {
          wxCommandEvent cmdEvent;

          cmdEvent.SetId(g_nextHMILeftClickOperation);
          OnNewControl(cmdEvent);
       }
       lastWasLeftDown=false;
       Refresh();
    }
    else if (event.LeftDClick())
    {
       wxInt32    id=event.GetId();
       hmiObject *newObject;

       highlightObject=NULL;
       newObject=g_objectList.getUIElement(id,m_myID,event.GetPosition());
       if ((!newObject) && (m_myID>0)) newObject=(hmiObject*)g_objectList.getObject(m_myID); // double-click on tabbed/stacked/single pane(l)
       if (newObject)
       {
          g_selectedList.deleteAll(0);
          if ((newObject->data.type!=HMI_TYPE_ADDITIONALPANE) &&
              (newObject->data.type!=HMI_TYPE_TABBEDPANE) &&
              (newObject->data.type!=HMI_TYPE_SINGLEPANEL) &&
              (newObject->data.type!=HMI_TYPE_STACKEDPANE))
          {
             g_selectedList.addObject(newObject,true,false);
             Refresh();
          }
          DlgObjectProp dlg(newObject,g_mainWin,_("Properties"));
          dlg.ShowModal();
          g_selectedList.deleteAll(0);
          if ((newObject->data.type!=HMI_TYPE_ADDITIONALPANE) &&
              (newObject->data.type!=HMI_TYPE_TABBEDPANE) &&
              (newObject->data.type!=HMI_TYPE_SINGLEPANEL) &&
              (newObject->data.type!=HMI_TYPE_STACKEDPANE))
           g_selectedList.addObject(newObject,true,false);
          Refresh();
       }
    }
    event.Skip(true);
}
#endif



#ifdef ENV_DEBUGGER
void HMICanvas::displayControlNames(wxByte enable)
{
   displayNames=enable;
   Refresh();
}
#endif



void HMICanvas::OnPaint(wxPaintEvent &WXUNUSED(event))
{
#ifdef ENV_DEBUGGER
   flowObject *object;
   wxNode     *node;
#endif
#ifdef ENV_EDITOR
   flowObject *object;

#endif
   
   wxAutoBufferedPaintDC dc( this );
   BasePanel::paintBackground(dc);
   BasePanel::paintContents(dc);

#ifdef ENV_EDITOR
   if (displayedName.Length()>0)
   {
      dc.SetFont(*wxSWISS_FONT);
      dc.SetBrush(*wxGREY_BRUSH);
      dc.DrawText(_T("(")+displayedName+_T(")"),2,10);
   }

   if (highlightObject)
   {
      dc.SetBrush(*wxLIGHT_GREY_BRUSH);
      dc.SetPen(*wxLIGHT_GREY_PEN);
      dc.DrawRectangle(highlightObject->getPos().x/1000.0-13,highlightObject->getPos().y/1000.0-13,highlightObject->getSize().x/1000.0+26,highlightObject->getSize().y/1000.0+26);
   }

   if (g_selectedList.GetCount()>0)
   {
      wxNode *node;

      node=g_selectedList.getObject((wxNode*)NULL);
      object=(hmiObject*)node->GetData();
      if ((object->data.isChildOf==m_myID) && (object->data.type!=HMI_TYPE_ADDITIONALPANE))
      {
         wxRect rect;

         dc.SetPen(BLUE_PEN);
         dc.SetBrush(*wxTRANSPARENT_BRUSH);
         rect=g_selectedList.getBorder(9);
         dc.DrawRectangle(rect.x,              rect.y,rect.width,rect.height);
         if  (g_selectedList.getFirstObject()->data.msizex>0)
         {
            dc.DrawRectangle(rect.x+9,            rect.y+9,rect.width-18,rect.height-18);
            if (g_selectedList.GetCount()==1)
            {
               if ((((hmiObject*)object)->noUIFlags & OAPC_HMI_NO_SIZE)==0)
               {
                  dc.SetBrush(*wxMEDIUM_GREY_BRUSH);
                  dc.DrawRectangle(rect.x+rect.width-10, rect.y+rect.height-10,10,10);
               }
               if (((((hmiObject*)object)->noUIFlags & OAPC_HMI_NO_FREE_SIZE_RATIO)==0) &&
                   ((((hmiObject*)object)->noUIFlags & OAPC_HMI_NO_SIZE)==0))
               {
                  dc.SetBrush(*wxGREY_BRUSH);
                  dc.DrawRectangle(rect.x+rect.width-10, rect.y+rect.height/2-5,10,10);
                  dc.DrawRectangle(rect.x+rect.width/2-5,rect.y+rect.height-10,10,10);
                   dc.SetBrush(*wxLIGHT_GREY_BRUSH);
                  dc.DrawRectangle(rect.x,               rect.y,10,10);
                  dc.DrawRectangle(rect.x+rect.width-10, rect.y,10,10);
                   dc.DrawRectangle(rect.x,               rect.y+rect.height-10,10,10);
               }
            }
         }
      }
   }
   if ((rectStart.x>=0) && (rectEnd.x>=0))
   {
      dc.SetBrush(*wxTRANSPARENT_BRUSH);
      dc.SetPen(BLUE_PEN_DOT);
      dc.DrawRectangle(rectStart.x,rectStart.y,rectEnd.x-rectStart.x,rectEnd.y-rectStart.y);
   }
#else
#endif
#ifdef ENV_DEBUGGER
   dc.SetFont(*wxSWISS_FONT);
   dc.SetTextBackground(wxColour(g_objectList.m_projectData->bgCol));
   dc.SetTextForeground(*wxBLACK);
   if (displayNames)
   {
      node=g_objectList.getObject((wxList::compatibility_iterator)NULL);
      while (node)
      {
         object=(flowObject*)node->GetData();
         if (object->data.type & HMI_TYPE_MASK)
          dc.DrawText(((hmiObject*)object)->name,((hmiObject*)object)->getPos().x/1000.0+1,((hmiObject*)object)->getPos().y/1000.0+((hmiObject*)object)->getSize().y/1000.0+2);
         node=g_objectList.getObject(node);
      }
   }
#endif
   BasePanel::paintBorder(dc);
}



#ifndef ENV_PLAYER
void HMICanvas::OnFoldbarClicked(wxMouseEvent& event)
{
   if (event.GetId()>=BUTTON_FOLDBAR)
   {
      std::list<wxStaticText*>::iterator it;
      wxStaticText                      *text;

      for ( it=m_foldBarList.begin() ; it != m_foldBarList.end(); it++ )
      {
         text=*it;
         if (event.GetId()==text->GetId())
         {
#ifdef __x86_64__
            if (g_nextHMILeftClickOperation==(wxInt64)text->GetClientData())
#else
            if (g_nextHMILeftClickOperation==(wxInt32)text->GetClientData())
#endif
            {
               text->SetForegroundColour(*wxBLACK);
               g_nextHMILeftClickOperation=0;
            }
            else
            {
               text->SetForegroundColour(BLUE_COLOUR);
#ifdef __x86_64__
               g_nextHMILeftClickOperation=(wxInt64)text->GetClientData();
#else
               g_nextHMILeftClickOperation=(wxInt32)text->GetClientData();
#endif
            }
         }
         else text->SetForegroundColour(*wxBLACK);
         text->Refresh();
      }
   }
}



wxInt32 HMICanvas::addFoldBarItem(wxFoldPanelBar *bar,wxFoldPanel *foldItem,wxInt32 id,wxString name)
{
   wxStaticText   *text;
   static wxInt32  buttonCtr=0;

   text=new wxStaticText(foldItem->GetParent(),BUTTON_FOLDBAR+buttonCtr,name,wxDefaultPosition,wxDefaultSize,wxALIGN_LEFT);
   if (id==POPUP_NEW_USERMANAGEMENT_PANEL)
   {
      g_userMgmntButton=text;
      g_userMgmntButton->Enable(false);
   }
   text->Connect(wxEVT_LEFT_DOWN,wxMouseEventHandler(HMICanvas::OnFoldbarClicked),NULL,this);
   if (id==wxID_ANY) id=BUTTON_FOLDBAR+buttonCtr;
   text->SetClientData((void*)id);
   m_foldBarList.push_back(text);
   buttonCtr++;

   bar->AddFoldPanelWindow(*foldItem,text,wxFPB_ALIGN_WIDTH,0,0,0);
   return BUTTON_FOLDBAR+buttonCtr-1;
}



wxFoldPanelBar *HMICanvas::createFoldBar(wxWindow *parent)
{
   ExternalIOLib *extIOLib;
   wxFoldPanel   foldItem(NULL);

   m_hmiBar = new wxFoldPanelBar(parent,wxID_ANY,wxDefaultPosition,wxSize(200,500),wxFPB_DEFAULT_STYLE |wxFPB_VERTICAL,0);

   foldItem=m_hmiBar->AddFoldPanel(_("Control"),true);
   addFoldBarItem(m_hmiBar,&foldItem,POPUP_NEW_SIMPLE_BUTTON,_("Simple Button"));
   addFoldBarItem(m_hmiBar,&foldItem,POPUP_NEW_IMAGE_BUTTON,_("Image Button"));
   addFoldBarItem(m_hmiBar,&foldItem,POPUP_NEW_TOGGLE_BUTTON,_("Toggle Button"));
   addFoldBarItem(m_hmiBar,&foldItem,POPUP_NEW_RADIO_BUTTON,_("Radio Button"));
   addFoldBarItem(m_hmiBar,&foldItem,POPUP_NEW_CHECKBOX,_("Checkbox"));
   addFoldBarItem(m_hmiBar,&foldItem,POPUP_NEW_HSLIDER,_("Horizontal Slider"));
   addFoldBarItem(m_hmiBar,&foldItem,POPUP_NEW_VSLIDER,_("Vertical Slider"));
   addFoldBarItem(m_hmiBar,&foldItem,POPUP_NEW_AREGULATOR,_("Angular Regulator"));
   addFoldBarItem(m_hmiBar,&foldItem,POPUP_NEW_NUMFIELD,_("Number Field"));
   addFoldBarItem(m_hmiBar,&foldItem,POPUP_NEW_FLOATFIELD,_("Floating Number Field"));
   addFoldBarItem(m_hmiBar,&foldItem,POPUP_NEW_TEXTFIELD,_("Text Field"));
   addFoldBarItem(m_hmiBar,&foldItem,POPUP_NEW_PASSWORDFIELD,_("Password Field"));
   m_hmiBar->AddFoldPanelSeperator(foldItem,wxColour(200,200,210),0,0,0);
   extIOLib=g_externalHMILibs->getLib(true);
   while (extIOLib)
   {
      if (((extIOLib->getLibCapabilities() & OAPC_IS_DEPRECATED)==0) &&
          ((extIOLib->getLibCapabilities() & OAPC_HMICAT_MASK)==OAPC_HMICAT_CONTROL))
      {
         extIOLib->foldUiID=addFoldBarItem(m_hmiBar,&foldItem,wxID_ANY,extIOLib->name);
      }
      extIOLib=g_externalHMILibs->getLib(false);
   }

   foldItem=m_hmiBar->AddFoldPanel(_("Display"),true);
   addFoldBarItem(m_hmiBar,&foldItem,POPUP_NEW_HGAUGE,_("Horizontal Gauge"));
   addFoldBarItem(m_hmiBar,&foldItem,POPUP_NEW_VGAUGE,_("Vertical Gauge"));
   addFoldBarItem(m_hmiBar,&foldItem,POPUP_NEW_AMETER,_("Angular Meter"));
   addFoldBarItem(m_hmiBar,&foldItem,POPUP_NEW_LCDNUMBER,_("LC Numeric Display"));
   m_hmiBar->AddFoldPanelSeperator(foldItem,wxColour(200,200,210),0,0,0);
   extIOLib=g_externalHMILibs->getLib(true);
   while (extIOLib)
   {
      if (((extIOLib->getLibCapabilities() & OAPC_IS_DEPRECATED)==0) &&
          ((extIOLib->getLibCapabilities() & OAPC_HMICAT_MASK)==OAPC_HMICAT_DISPLAY))
      {
         extIOLib->foldUiID=addFoldBarItem(m_hmiBar,&foldItem,wxID_ANY,extIOLib->name);
      }
      extIOLib=g_externalHMILibs->getLib(false);
   }

   foldItem=m_hmiBar->AddFoldPanel(_("Static"),true);
   addFoldBarItem(m_hmiBar,&foldItem,POPUP_NEW_LINE,_("Rectangle"));
   addFoldBarItem(m_hmiBar,&foldItem,POPUP_NEW_FREELINE,_("Free Line"));
   addFoldBarItem(m_hmiBar,&foldItem,POPUP_NEW_TEXTLABEL,_("Text Label"));
   addFoldBarItem(m_hmiBar,&foldItem,POPUP_NEW_ELLIPSE,_("Ellipse"));
   addFoldBarItem(m_hmiBar,&foldItem,POPUP_NEW_FRAME,_("Frame"));
   addFoldBarItem(m_hmiBar,&foldItem,POPUP_NEW_IMAGE,_("Image"));
   m_hmiBar->AddFoldPanelSeperator(foldItem,wxColour(200,200,210),0,0,0);
   extIOLib=g_externalHMILibs->getLib(true);
   while (extIOLib)
   {
      if (((extIOLib->getLibCapabilities() & OAPC_IS_DEPRECATED)==0) &&
          ((extIOLib->getLibCapabilities() & OAPC_HMICAT_MASK)==OAPC_HMICAT_STATIC))
      {
         extIOLib->foldUiID=addFoldBarItem(m_hmiBar,&foldItem,wxID_ANY,extIOLib->name);
      }
      extIOLib=g_externalHMILibs->getLib(false);
   }

   foldItem=m_hmiBar->AddFoldPanel(_("Container"),true);
   addFoldBarItem(m_hmiBar,&foldItem,POPUP_NEW_TABBEDPANE,_("Tabbed Pane"));
   addFoldBarItem(m_hmiBar,&foldItem,POPUP_NEW_STACKEDPANE,_("Stacked Pane"));
   addFoldBarItem(m_hmiBar,&foldItem,POPUP_NEW_ADDITIONALPANE,_("Additional Pane"));
   addFoldBarItem(m_hmiBar,&foldItem,POPUP_NEW_SINGLEPANEL,_("Single Panel"));

   foldItem=m_hmiBar->AddFoldPanel(_("Miscellaneous"),true);
   addFoldBarItem(m_hmiBar,&foldItem,POPUP_NEW_USERMANAGEMENT_PANEL,_("User Management Panel"));
   m_hmiBar->AddFoldPanelSeperator(foldItem,wxColour(200,200,210),0,0,0);


   return m_hmiBar;
}



void HMICanvas::ShowContextMenu(wxContextMenuEvent &WXUNUSED(event))
{
   flowObject *singleSelectedObject,*relatedObject;
   wxMenu      menu;
   wxMenu     *elementsMenu,*uiMenu,*displayMenu,*containerMenu,*controlMenu,*miscMenu=new wxMenu();
   wxMenuItem *item;
   bool        uiHasCustom=false,displayHasCustom=false,elementsHasCustom=false;

#ifdef __WXDEBUG__
//   menu.Append(wxID_ANY,wxString::Format(_T("%d"),m_myID));
#endif

   // fetch the selected object if only one is selected
   if (g_selectedList.GetCount()==1) singleSelectedObject=g_selectedList.getFirstObject();
   else singleSelectedObject=NULL;

   // fetch the object that contains this HMICanvas
   if (m_myID!=0) relatedObject=g_objectList.getObject(m_myID);
   else relatedObject=NULL;

   containerMenu=new wxMenu();
   containerMenu->Append(POPUP_NEW_TABBEDPANE,_("Tabbed Pane"));
   containerMenu->Append(POPUP_NEW_STACKEDPANE,_("Stacked Pane"));
   item=new wxMenuItem(containerMenu,POPUP_NEW_ADDITIONALPANE, _("Additional Pane"));
   containerMenu->Append(item);
   if (m_myID==0)
   {
      if ((!singleSelectedObject) ||
         ((singleSelectedObject->data.type!=HMI_TYPE_TABBEDPANE) && (singleSelectedObject->data.type!=HMI_TYPE_STACKEDPANE)))
      item->Enable(0); // it is the base canvas and no tabbed/stacked pane is selected, so no panel to be added here
   }
   containerMenu->Append(POPUP_NEW_SINGLEPANEL,_("Single Panel"));

   elementsMenu=new wxMenu();
   elementsMenu->Append(POPUP_NEW_LINE,_("Line"));
   elementsMenu->Append(POPUP_NEW_TEXTLABEL,_("Text Label"));
   elementsMenu->Append(POPUP_NEW_IMAGE,_("Image"));

   uiMenu=new wxMenu();
   uiMenu->Append(POPUP_NEW_SIMPLE_BUTTON,_("Simple Button"));
   uiMenu->Append(POPUP_NEW_IMAGE_BUTTON,_("Image Button"));
   uiMenu->Append(POPUP_NEW_TOGGLE_BUTTON,_("Toggle Button"));
//   basicMenu->Append(POPUP_NEW_TOGGLE_IMAGE_BUTTON,_("Image Toggle Button"));
   uiMenu->Append(POPUP_NEW_RADIO_BUTTON,_("Radio Button"));
   uiMenu->Append(POPUP_NEW_CHECKBOX,_("Checkbox"));
   uiMenu->Append(POPUP_NEW_HSLIDER,_("Horizontal Slider"));
   uiMenu->Append(POPUP_NEW_VSLIDER,_("Vertical Slider"));
   uiMenu->Append(POPUP_NEW_AREGULATOR,_("Angular Regulator"));
   uiMenu->Append(POPUP_NEW_NUMFIELD,_("Number Field"));
   uiMenu->Append(POPUP_NEW_FLOATFIELD,_("Floating Number Field"));
   uiMenu->Append(POPUP_NEW_TEXTFIELD,_("Text Field"));
   uiMenu->Append(POPUP_NEW_PASSWORDFIELD,_("Password Field"));

   displayMenu=new wxMenu();
   displayMenu->Append(POPUP_NEW_HGAUGE,_("Horizontal Gauge"));
   displayMenu->Append(POPUP_NEW_VGAUGE,_("Vertical Gauge"));
   displayMenu->Append(POPUP_NEW_AMETER,_("Angular Meter"));
   displayMenu->Append(POPUP_NEW_LCDNUMBER,_("LC Numeric Display"));

   item=new wxMenuItem(miscMenu,POPUP_NEW_USERMANAGEMENT_PANEL,_("User Management Panel"));
   miscMenu->Append(item);
   if ((!g_userPriviData.enabled) && (!g_hmiUserMgmntPanel)) item->Enable(0);

   controlMenu=new wxMenu();

   item=new wxMenuItem(controlMenu,POPUP_DUP_CONTROL, _("Duplicate")+_T("\tShift-INS"));
   controlMenu->Append(item);
   if (((singleSelectedObject) &&
       ((singleSelectedObject->data.type==HMI_TYPE_ADDITIONALPANE) ||
        (singleSelectedObject->data.type==HMI_TYPE_TABBEDPANE) ||
        (singleSelectedObject->data.type==HMI_TYPE_SINGLEPANEL) ||
        (singleSelectedObject->data.type==HMI_TYPE_STACKEDPANE))) ||
       (g_selectedList.GetCount()<=0)) item->Enable(0);

   item=new wxMenuItem(controlMenu,POPUP_CUT_CONTROL, _("Cut"));
   controlMenu->Append(item);
   if (g_selectedList.GetCount()<=0) item->Enable(0);

   item=new wxMenuItem(controlMenu,POPUP_EDIT_CONTROL, _("Edit"));
   controlMenu->Append(item);
   if ((relatedObject) &&
       ((relatedObject->data.type==HMI_TYPE_ADDITIONALPANE) ||
        (relatedObject->data.type==HMI_TYPE_TABBEDPANE) ||
        (relatedObject->data.type==HMI_TYPE_SINGLEPANEL) ||
        (relatedObject->data.type==HMI_TYPE_STACKEDPANE)))
   {
      // keep the item enabled
   }
   else if (g_selectedList.GetCount()!=1) item->Enable(0);

   item=new wxMenuItem(controlMenu,POPUP_PREV_PANE, _("Previous Pane"));
   controlMenu->Append(item);
   if ((singleSelectedObject) &&
       ((singleSelectedObject->data.type==HMI_TYPE_ADDITIONALPANE) ||
        (singleSelectedObject->data.type==HMI_TYPE_TABBEDPANE) ||
        (singleSelectedObject->data.type==HMI_TYPE_STACKEDPANE)))
   {
      // keep the item enabled
   }
   else item->Enable(0);

   item=new wxMenuItem(controlMenu,POPUP_NEXT_PANE, _("Next Pane"));
   controlMenu->Append(item);
   if ((singleSelectedObject) &&
       ((singleSelectedObject->data.type==HMI_TYPE_ADDITIONALPANE) ||
        (singleSelectedObject->data.type==HMI_TYPE_TABBEDPANE) ||
        (singleSelectedObject->data.type==HMI_TYPE_STACKEDPANE)))
   {
      // keep the item enabled
   }
   else item->Enable(0);

   item=new wxMenuItem(controlMenu,POPUP_MAX_CONTROL, _("Maximize"));
   controlMenu->Append(item);
   if ((relatedObject) && (singleSelectedObject) &&
       ((relatedObject->data.type==HMI_TYPE_TABBEDPANE) ||
        (relatedObject->data.type==HMI_TYPE_STACKEDPANE)))
   {
      // keep the item enabled
   }
   else if ((g_selectedList.GetCount()!=1) || (!singleSelectedObject) ||
            ((singleSelectedObject) &&
            (singleSelectedObject->data.type!=HMI_TYPE_TABBEDPANE) &&
            (singleSelectedObject->data.type!=HMI_TYPE_STACKEDPANE)))
    item->Enable(0);

   item=new wxMenuItem(controlMenu,POPUP_DEL_CONTROL, _("Delete")+_T("\tShift-DEL"));
   controlMenu->Append(item);
   if ((relatedObject) &&
       ((relatedObject->data.type==HMI_TYPE_ADDITIONALPANE) ||
        (relatedObject->data.type==HMI_TYPE_TABBEDPANE) ||
        (relatedObject->data.type==HMI_TYPE_SINGLEPANEL) ||
        (relatedObject->data.type==HMI_TYPE_STACKEDPANE)))
   {
      // keep the item enabled
   }
   else if (g_selectedList.GetCount()<=0) item->Enable(0);

   item=new wxMenuItem(controlMenu,POPUP_SNAP_CONTROL_TO_GRID, _("Snap To Grid"));
   controlMenu->Append(item);
   if ((relatedObject) &&
       ((relatedObject->data.type==HMI_TYPE_TABBEDPANE) ||
        (relatedObject->data.type==HMI_TYPE_SINGLEPANEL) ||
        (relatedObject->data.type==HMI_TYPE_STACKEDPANE)))
   {
      // keep the item enabled
   }
   else if ((g_selectedList.GetCount()!=1) ||
            ((singleSelectedObject) &&
             (singleSelectedObject->data.type==HMI_TYPE_ADDITIONALPANE)))
    item->Enable(0);

   item=new wxMenuItem(controlMenu,POPUP_ELEMENT_TO_TOP, _("Move To Top"));
   controlMenu->Append(item);
   if (g_selectedList.GetCount()!=1) item->Enable(0);
   item=new wxMenuItem(controlMenu,POPUP_ELEMENT_TO_BOTTOM, _("Move To Bottom"));
   controlMenu->Append(item);
   if (g_selectedList.GetCount()!=1) item->Enable(0);

   item=new wxMenuItem(&menu,m_lastElementID, _("Last element again")+_T("\tF2"));
   menu.Append(item);
   if (m_lastElementID==0) item->Enable(false);

   menu.AppendSubMenu(uiMenu,_("Control"));
   menu.AppendSubMenu(displayMenu,_("Display"));
   menu.AppendSubMenu(elementsMenu,_("Static"));
   menu.AppendSubMenu(containerMenu,_("Container"));
   menu.AppendSubMenu(miscMenu,_("Miscellaneous"));
   menu.AppendSeparator();
   menu.Append(POPUP_SELECT_CONTROL,_("Select"));

   if ((relatedObject) &&
       ((relatedObject->data.type==HMI_TYPE_ADDITIONALPANE) ||
        (relatedObject->data.type==HMI_TYPE_TABBEDPANE) ||
        (relatedObject->data.type==HMI_TYPE_SINGLEPANEL) ||
        (relatedObject->data.type==HMI_TYPE_STACKEDPANE)))// possibly it is an additional pane?
    menu.AppendSubMenu(controlMenu,_("Selected Control(s)"));
   else
   {
      if (g_selectedList.GetCount()>1) menu.AppendSubMenu(controlMenu,_("Selected Control(s)"));
      else if (singleSelectedObject)
       menu.AppendSubMenu(controlMenu,_("Selected Control(s)"));
      else
      {
         item=new wxMenuItem(&menu,wxID_ANY,_("Selected Control(s)"));
         delete controlMenu;
         menu.Append(item);
         item->Enable(0);
      }
   }

   item=new wxMenuItem(&menu,POPUP_INS_CONTROL, _("Insert"));
   menu.Append(item);
   if (g_cuttedList.GetCount()<=0) item->Enable(0);

   ExternalIOLib *extIOLib=g_externalHMILibs->getLib(true);
   while (extIOLib)
   {
      wxMenuItem *item;

      if ((extIOLib->getLibCapabilities() & OAPC_IS_DEPRECATED)==0)
      {
         if ((extIOLib->getLibCapabilities() & OAPC_HMICAT_MASK)==OAPC_HMICAT_CONTROL)
         {
            if (!uiHasCustom) uiMenu->AppendSeparator();
            uiHasCustom=true;
            item=new wxMenuItem(uiMenu,wxID_ANY,extIOLib->name);
            uiMenu->Append(item);
         }
         else if ((extIOLib->getLibCapabilities() & OAPC_HMICAT_MASK)==OAPC_HMICAT_DISPLAY)
         {
            if (!displayHasCustom) displayMenu->AppendSeparator();
            displayHasCustom=true;
            item=new wxMenuItem(displayMenu,wxID_ANY,extIOLib->name);
            displayMenu->Append(item);
         }
         else if ((extIOLib->getLibCapabilities() & OAPC_HMICAT_MASK)==OAPC_HMICAT_STATIC)
         {
            if (!elementsHasCustom) elementsMenu->AppendSeparator();
            elementsHasCustom=true;
            item=new wxMenuItem(elementsMenu,wxID_ANY,extIOLib->name);
            elementsMenu->Append(item);
         }
         else
         {
            item=new wxMenuItem(miscMenu,wxID_ANY,extIOLib->name);
            miscMenu->Append(item);
         }
         extIOLib->uiID=item->GetId();
         Connect(extIOLib->uiID,wxEVT_COMMAND_MENU_SELECTED,wxCommandEventHandler(HMICanvas::OnNewControl));
      }
      extIOLib=g_externalHMILibs->getLib(false);
   }

   PopupMenu(&menu);
}



void HMICanvas::OnNewControl(wxCommandEvent& event)
{
   hmiObject   *button=NULL;
   wxBusyCursor wait;

   g_mainWin->addUndoStep(_("Add Control"));
   m_lastElementID=event.GetId();
   if (event.GetId()==POPUP_NEW_SIMPLE_BUTTON)             button=new hmiSimpleButton(this,NULL);
   else if (event.GetId()==POPUP_NEW_IMAGE_BUTTON)         button=new hmiImageButton(this,NULL);
   else if (event.GetId()==POPUP_NEW_TOGGLE_IMAGE_BUTTON)  button=new hmiToggleImageButton(this,NULL);
   else if (event.GetId()==POPUP_NEW_TOGGLE_BUTTON)        button=new hmiToggleButton(this,NULL);
   else if (event.GetId()==POPUP_NEW_RADIO_BUTTON)         button=new hmiRadioButton(this,NULL,HMI_TYPE_RADIOBUTTON);
   else if (event.GetId()==POPUP_NEW_CHECKBOX)             button=new hmiRadioButton(this,NULL,HMI_TYPE_CHECKBOX);
   else if (event.GetId()==POPUP_NEW_HSLIDER)              button=new hmiHSlider(this,NULL);
   else if (event.GetId()==POPUP_NEW_VSLIDER)              button=new hmiVSlider(this,NULL);
   else if (event.GetId()==POPUP_NEW_HGAUGE)               button=new hmiHGauge(this,NULL);
   else if (event.GetId()==POPUP_NEW_VGAUGE)               button=new hmiVGauge(this,NULL);
   else if (event.GetId()==POPUP_NEW_NUMFIELD)             button=new hmiNumField(this,NULL);
   else if (event.GetId()==POPUP_NEW_TEXTFIELD)            button=new hmiTextField(this,NULL,HMI_TYPE_TEXTFIELD);
   else if (event.GetId()==POPUP_NEW_PASSWORDFIELD)        button=new hmiTextField(this,NULL,HMI_TYPE_PASSWORDFIELD);
   else if (event.GetId()==POPUP_NEW_FLOATFIELD)           button=new hmiFloatField(this,NULL);
   else if (event.GetId()==POPUP_NEW_TEXTLABEL)            button=new hmiTextLabel(this,NULL);
   else if (event.GetId()==POPUP_NEW_LINE)                 button=new hmiLine(this,NULL);
   else if (event.GetId()==POPUP_NEW_IMAGE)                button=new hmiImage(this,NULL);
   else if (event.GetId()==POPUP_NEW_FREELINE)             button=new hmiPrimitive(this,NULL,HMI_TYPE_FREELINE);
   else if (event.GetId()==POPUP_NEW_FRAME)                button=new hmiPrimitive(this,NULL,HMI_TYPE_FRAME);
   else if (event.GetId()==POPUP_NEW_ELLIPSE)              button=new hmiPrimitive(this,NULL,HMI_TYPE_ELLIPSE);
   else if (event.GetId()==POPUP_NEW_TABBEDPANE)           button=new hmiTabbedPane(this,NULL);
   else if (event.GetId()==POPUP_NEW_STACKEDPANE)          button=new hmiStackedPane(this,NULL);
   else if (event.GetId()==POPUP_NEW_ADDITIONALPANE)
   {
      flowObject *object;

      object=g_objectList.getObject(m_myID);
      if (!object) return;
      if (object->data.type==HMI_TYPE_ADDITIONALPANE)
       button=new hmiAdditionalPane(((hmiAdditionalPane*)object)->m_parent,NULL);
      else button=new hmiAdditionalPane(this,NULL);
   }
   else if (event.GetId()==POPUP_NEW_SINGLEPANEL)          button=new hmiSinglePanel(this,NULL);
   else if (event.GetId()==POPUP_NEW_AMETER)               button=new hmiAngularMeter(this,NULL);
   else if (event.GetId()==POPUP_NEW_AREGULATOR)           button=new hmiAngularRegulator(this,NULL);
   else if (event.GetId()==POPUP_NEW_LCDNUMBER)            button=new hmiLCDNumber(this,NULL);
   else if (event.GetId()==POPUP_NEW_USERMANAGEMENT_PANEL) button=new hmiSpecialPanel(this,NULL,HMI_TYPE_SP_USERMANAGEMENT);
   else
   {
      ExternalIOLib *extIOLib;

      extIOLib=g_externalHMILibs->getLib(true);
      while (extIOLib)
      {
         if ((extIOLib->uiID==event.GetId()) || (extIOLib->foldUiID==event.GetId()))
         {
            button=new hmiExternalIOLib(extIOLib,this,NULL,0);
            if (!button->returnOK)
            {
               delete button;
               return;
            }
            break;
         }
         extIOLib=g_externalHMILibs->getLib(false);
      }
   }
   if (button)
   {
      button->createUIElement();
      button->setPos(wxRealPoint(lastMousePos.x*1000.0,lastMousePos.y*1000.0));
      g_objectList.addObject(button,true,false);
      g_selectedList.deleteAll(0);
      if (button->data.type!=HMI_TYPE_ADDITIONALPANE)
       g_selectedList.addObject(button,true,false);
      else ((BasePanel*)button->uiElement)->Refresh();
      Refresh();
      g_isSaved=false;
      g_isEmpty=false;
   }
   else wxASSERT(0);
}



#endif



void HMICanvas::refreshProject()
{
    SDBG
   if (!g_objectList.m_projectData) return;
//   SetBackgroundColour(wxColour(g_objectList.m_projectData->bgCol));
#ifndef ENV_PLAYER
   wxInt32 x,y;

   if (m_owner)
   {
      m_owner->GetViewStart(&x,&y);
      if ((g_objectList.m_projectData->totalW>0) && (g_objectList.m_projectData->totalH>0))
       m_owner->SetScrollbars(HMI_SCROLL_UNIT,HMI_SCROLL_UNIT,g_objectList.m_projectData->totalW/HMI_SCROLL_UNIT,g_objectList.m_projectData->totalH/HMI_SCROLL_UNIT,x,y);
   }
#else
   GetParent()->SetClientSize(g_objectList.m_projectData->totalW,g_objectList.m_projectData->totalH);
#endif
   SetClientSize(wxSize(g_objectList.m_projectData->totalW,g_objectList.m_projectData->totalH));
   SetBackgroundColour(g_objectList.m_projectData->bgCol);
   Refresh();
}



#ifndef ENV_PLAYER
void HMICanvas::saveProject(wxString file)
{
   g_objectList.saveProject(file,false);
   g_isSaved=true;
}
#endif


wxByte HMICanvas::loadProject(wxString projectPath)
{
   wxString error;

   SDBG
   error=g_objectList.loadProject(projectPath,true,false);
   if (error.Length()==0)
   {
      refreshProject();
#ifdef ENV_PLAYER
      g_flowPool.startTimer();
#endif
#ifdef ENV_EDITOR
      g_isEmpty=false;
      g_isSaved=true;
#endif
      return 1;
   }
   else wxMessageBox(projectPath+_T(":\n")+error,_("Error"),wxICON_ERROR|wxOK|wxCENTRE);
   return 0;
}


#if defined ENV_DEBUGGER || ENV_EDITOR
bool HMICanvas::loadProject(wxString *m_projectDir, wxString *m_projectFile)
{
#ifdef ENV_EDITOR
   wxInt32 res=0;

   if ((!g_isEmpty) && (!g_isSaved))
   {
      wxMessageDialog dialog(this,_("Really load a new project?\nThis will delete all the existing data!"),_T("Question"), wxYES_NO|wxICON_QUESTION);
      res=dialog.ShowModal();
   }
   if ((res==wxID_YES) || (g_isEmpty) || (g_isSaved))
#endif
   {
      wxFileDialog* openFileDialog=new wxFileDialog( this, _("Load new project"),this->m_projectDir,this->m_projectFile,PROJECT_FILETYPES,wxFD_OPEN, wxDefaultPosition);

      if ( openFileDialog->ShowModal() == wxID_OK )
      {
         wxString path;

         path=openFileDialog->GetPath();
         oapc_path_split(&path,&this->m_projectDir,&this->m_projectFile,_T(".apcp"));
         *m_projectDir=this->m_projectDir;
         *m_projectFile=this->m_projectFile;
         newProject();
         loadProject(path);
         delete openFileDialog;
         return true;
      }
   }
   return false;
}


void HMICanvas::newProject()
{
#ifdef ENV_EDITOR
   g_flowCanvas->resetPointers();
   g_selectedList.deleteAll(0);
   g_isEmpty=true;
#endif
   g_objectList.deleteAll(1);
   g_objectList.init();
   refreshProject();
}
#endif



#ifndef ENV_PLAYER
void HMICanvas::OnDelControl(wxCommandEvent& WXUNUSED(event))
{
   g_mainWin->addUndoStep(_("Delete"));
   g_isSaved=false;
   if ((g_selectedList.GetCount()<=0) && (m_myID!=0)) // child-HMICanvas which is deleted by following operation
   {
      g_selectedList.deleteAll(0);
      g_objectList.deleteObject(g_objectList.getObject(m_myID),true,true);
   }
   else // base-HMI-canvas
   {
      if (g_selectedList.GetCount()<=0)
      {
         wxMessageBox(_("No element selected to perform this operation with!"),_("Error"),wxICON_ERROR|wxOK|wxCENTRE);
         return;
      }
      g_objectList.deleteObjects(g_selectedList,1,1);
      g_selectedList.deleteAll(0);
      g_hmiCanvas->Layout();
      g_hmiCanvas->Refresh();
   }
}



void HMICanvas::OnDupControl(wxCommandEvent& WXUNUSED(event))
{
   hmiObject *object,*copyObject;
   ObjectList copyList((HMICanvas*)NULL/*,0*/);
   wxNode    *node;
   hmiMutex  *mutex;

   g_mainWin->addUndoStep(_("Duplicate"));
   node=g_selectedList.getObject((wxNode*)NULL);
   if (!node) return;
   while (node)
   {
      object=(hmiObject*)node->GetData();
      copyObject=object->duplicate();
      copyObject->setFlowPos(NULL,wxPoint(-1,-1),1,0);
      copyObject->data.id=0;
      copyList.addObject(copyObject,true,false);
      mutex=g_objectList.getMutexByObjectID(object->data.id);
      if (mutex) mutex->addObjectID(copyObject->data.id);
      copyObject->name=_T("");
      node=g_selectedList.getObject(node);
   }
   copyList.setTotalPos(wxRealPoint(copyList.getTotalPos().x/1000.0+20,copyList.getTotalPos().y/1000.0+20));
   g_selectedList.deleteAll(0);
   g_objectList.Append(&copyList);
   g_selectedList.Append(&copyList);
   copyList.deleteAll(0);
   g_isSaved=false;
   Refresh();
}



void HMICanvas::OnEditControl(wxCommandEvent& WXUNUSED(event))
{
   hmiObject *object;

   g_mainWin->addUndoStep(_("Edit"));
   if ((g_selectedList.GetCount()<=0) && (m_myID!=0)) object=(hmiObject*)g_objectList.getObject(m_myID);
   else object=(hmiObject*)g_selectedList.getFirstObject();
   if (!object)
   {
      wxMessageBox(_("No element selected to perform this operation with!"),_("Error"),wxICON_ERROR|wxOK|wxCENTRE);
      return;
   }
   DlgObjectProp dlg(object,g_mainWin,_("Properties"));
   dlg.ShowModal();
   g_selectedList.deleteAll(0);
   if (object->data.type!=HMI_TYPE_ADDITIONALPANE)
    g_selectedList.addObject(object,true,false);
   Refresh();
}



void HMICanvas::OnMaxControl(wxCommandEvent& WXUNUSED(event))
{
   hmiObject *object;
   wxNode    *node;

   node=g_selectedList.GetFirst();
   wxASSERT(node);
   if (!node) return;
   object=(hmiObject*)node->GetData();
   object->setPos(wxRealPoint(0.0,0.0));
   if (object->data.isChildOf==0)
    object->setSize(wxRealPoint(g_hmiCanvas->GetSize().x*1000.0,g_hmiCanvas->GetSize().y*1000.0));
   else
   {
      hmiObject *parentObject;

      parentObject=(hmiObject*)g_objectList.getObject(object->data.isChildOf);
      wxASSERT(parentObject);
      if (!parentObject) return;
      wxASSERT(parentObject->data.type==HMI_TYPE_TABBEDPANE);
      if (parentObject->data.type!=HMI_TYPE_TABBEDPANE) return;
      wxASSERT(parentObject->data.type==HMI_TYPE_STACKEDPANE);
      if (parentObject->data.type!=HMI_TYPE_STACKEDPANE) return;
      object->setSize(wxRealPoint(((BasePanel*)parentObject->uiElement)->GetSize().x*1000.0,((BasePanel*)parentObject->uiElement)->GetSize().y*1000.0));
   }
   Refresh();
}



void HMICanvas::OnSelectControl(wxCommandEvent& WXUNUSED(event))
{
    DlgControlSelector  dlg(this,_T("Select a Control"),DLGCONTROLSELECTOR_FLAG_ALLOWSORT);//0,false);
    flowObject         *object;

    dlg.ShowModal();
    object=dlg.getSelectedControl();
    if (object)
    {
//       if (object->data.type==HMI_TYPE_ADDITIONALPANE) object=g_objectList.getObject(object->data.isChildOf);
       if (object)
       {
          g_selectedList.deleteAll(0);
          g_selectedList.addObject(object,true,false);
          Refresh();
       }
    }
}



void HMICanvas::OnSnapControlToGrid(wxCommandEvent& WXUNUSED(event))
{
    hmiObject *object;

    if ((g_selectedList.GetCount()<=0) && (m_myID!=0)) object=(hmiObject*)g_objectList.getObject(m_myID);
    else object=(hmiObject*)g_selectedList.getFirstObject();
    if ((object) && (g_selectedList.GetCount()>0))
    {
        g_selectedList.setSingleSize(wxRealPoint(OAPC_ROUND(object->getSize().x/1000.0/g_objectList.m_projectData->gridW,0)*g_objectList.m_projectData->gridW,
                                   OAPC_ROUND(object->getSize().y/1000.0/g_objectList.m_projectData->gridH,0)*g_objectList.m_projectData->gridH));
        g_selectedList.setTotalPos(wxRealPoint(OAPC_ROUND(object->getPos().x/1000.0/g_objectList.m_projectData->gridW,0)*g_objectList.m_projectData->gridW,
                                 OAPC_ROUND(object->getPos().y/1000.0/g_objectList.m_projectData->gridH,0)*g_objectList.m_projectData->gridH));
        Refresh();
    }
    else
    {
        object->setSize(wxRealPoint(OAPC_ROUND(object->getSize().x/1000.0/g_objectList.m_projectData->gridW,0)*g_objectList.m_projectData->gridW*1000.0,
                                    OAPC_ROUND(object->getSize().y/1000.0/g_objectList.m_projectData->gridH,0)*g_objectList.m_projectData->gridH*1000.0));
        object->setPos(wxRealPoint(OAPC_ROUND(object->getPos().x/1000.0/g_objectList.m_projectData->gridW,0)*g_objectList.m_projectData->gridW*1000.0,
                                   OAPC_ROUND(object->getPos().y/1000.0/g_objectList.m_projectData->gridH,0)*g_objectList.m_projectData->gridH*1000.0));
        Refresh();
    }
}



void HMICanvas::OnPopupMenu(wxCommandEvent& event)
{
   hmiObject *object;

   if ((g_selectedList.GetCount()<=0) && (m_myID!=0)) object=(hmiObject*)g_objectList.getObject(m_myID);
   else object=(hmiObject*)g_selectedList.getFirstObject();
   {
      if (event.GetId()==POPUP_CUT_CONTROL)
      {
         wxNode     *node;
         hmiObject  *object;
         wxRealPoint pos;

         g_cuttedList.deleteAll(true);
         g_objectList.deleteObjects(g_selectedList,false,false);
         g_cuttedList.Append(&g_selectedList);
         node=g_cuttedList.getObject((wxNode*)NULL);
         while (node)
         {
            object=(hmiObject*)node->GetData();
            pos=object->getPos();
            pos.x-=g_selectedList.getTotalPos().x;
            pos.y-=g_selectedList.getTotalPos().y;
            object->setPos(pos);
            if (object->uiElement) delete object->uiElement;
            object->uiElement=NULL;

            node=g_cuttedList.getObject(node);
         }
         g_selectedList.deleteAll(false);
         Refresh();
      }
      else if (event.GetId()==POPUP_INS_CONTROL)
      {
         wxNode     *node;
         hmiObject  *object;
         BasePanel  *containerObject;
         wxRealPoint pos;

         if (m_myID!=0)
         {
            object=((hmiObject*)g_objectList.getObject(m_myID));
            if ((object->data.type==HMI_TYPE_ADDITIONALPANE) ||
                (object->data.type==HMI_TYPE_TABBEDPANE) ||
                (object->data.type==HMI_TYPE_STACKEDPANE)) containerObject=((hmiTabbedPane*)object)->getPanel();
            else containerObject=(BasePanel*)object->uiElement;
         }
         else containerObject=g_hmiCanvas;

         node=g_cuttedList.getObject((wxNode*)NULL);
         while (node)
         {
            object=(hmiObject*)node->GetData();
            object->setParent(containerObject);
            pos=object->getPos();
            pos.x+=lastMousePos.x*1000;
            pos.y+=lastMousePos.y*1000;
            object->setPos(pos);
            object->createUIElement();

            node=g_cuttedList.getObject(node);
         }
         g_objectList.Append(&g_cuttedList);
         Refresh();
         g_cuttedList.deleteAll(false);
      }
      else if (event.GetId()==POPUP_ELEMENT_TO_TOP)
      {
         g_objectList.deleteObject(object,false,false);
         g_objectList.addObject(object,true,true);
         Refresh();
      }
      else if (event.GetId()==POPUP_ELEMENT_TO_BOTTOM)
      {
          wxNode *first;

          first=g_objectList.getObject((wxNode*)NULL);
          if ((first) && (first->GetData()!=object))
          {
             g_objectList.deleteObject(object,false,false);
             g_objectList.InsertBefore(first,object);
             Refresh();
          }
      }
      else if ((object->data.type==HMI_TYPE_ADDITIONALPANE) ||
          (object->data.type==HMI_TYPE_TABBEDPANE) ||
          (object->data.type==HMI_TYPE_STACKEDPANE))
      {
   	     wxInt32 sel;
   	
      	 sel=((hmiTabbedPane*)object)->getPageSelected();
         if (event.GetId()==POPUP_NEXT_PANE)
         {
      	    if (sel<((hmiTabbedPane*)object)->getPanelCount()-1) sel++;
         }
         else if (event.GetId()==POPUP_PREV_PANE)
         {
      	    if (sel>0) sel--;
         }
   	     ((hmiTabbedPane*)object)->setPageSelected(sel);      
      }
   }
}


#endif



#ifndef ENV_PLAYER
void HMICanvas::setMainWin(MainWin *mainWin)
{
    this->mainWin=mainWin;
}



MainWin *HMICanvas::getMainWin()
{
    return mainWin;
}
#endif



#ifdef ENV_PLAYER
void HMICanvas::OnExternalEvent(wxCommandEvent& event)
{
   if (event.GetId()==FLOW_LOGIN_USER_CHANGE_PWD)
   {
      ChangePwdDlg dlg(g_mainWin,_("Change Password"),(UserData*)event.GetEventObject());

      dlg.ShowModal();
      if (dlg.m_ok) hmiUserMgmntPanel::saveUserData(g_mainWin);
   }
   else assert(0);
}


void HMICanvas::OnHotKey(wxKeyEvent &event)
{
   SDBG
   event.Skip(true);
}



void HMICanvas::OnButtonClicked(wxCommandEvent &event)
{
   hmiObject *object;

   SDBG
   object=g_objectList.getObjectByUIElementID(event.GetId());
   if (object)
   {
       g_flowPool.startFlows(object,NULL,OAPC_DIGI_IO1,0,wxGetLocalTimeMillis(),0);
       object->updateSelectionState();
/*       if ((object->data.type & HMI_TYPE_SIMPLEBUTTON)==HMI_TYPE_SIMPLEBUTTON)
       {
           wxMilliSleep(200);
           g_flowPool.startFlows(object,NULL,OAPC_DIGI_IO1,0,wxGetLocalTimeMillis(),0);
       }*/
/*       else if ((object->data.type & HMI_TYPE_TOGGLEBUTTON)==HMI_TYPE_TOGGLEBUTTON)
        ((hmiToggleButton*)object)->applyToState();*/
   }
}



void HMICanvas::OnSliderScrolled(wxScrollEvent &event)
{
   hmiObject *object;

   SDBG
   object=g_objectList.getObjectByUIElementID(event.GetId());
   if (object)
   {
      if (object->data.dataFlowFlags & HMI_FLOWFLAG_ON_CHANGED_OUT6) DoOnSliderScrolled(object,OAPC_NUM_IO6,0,wxGetLocalTimeMillis());
      if (object->data.dataFlowFlags & HMI_FLOWFLAG_ON_CHANGED_OUT7) DoOnSliderScrolled(object,OAPC_NUM_IO7,0,wxGetLocalTimeMillis());
   }
}



void HMICanvas::DoOnSliderScrolled(flowObject *object,wxUint32 outflag,wxUint32 inheritThreadID,wxLongLong inheritCreationTime)
{
   SDBG
   g_flowPool.startFlows(object,NULL,outflag,inheritThreadID,inheritCreationTime,0);
}



void HMICanvas::OnSliderScrolling(wxScrollEvent &event)
{
   hmiObject *object;

   object=g_objectList.getObjectByUIElementID(event.GetId());
   if (object)
   {
      if (object->data.dataFlowFlags & HMI_FLOWFLAG_ON_CHANGING_OUT6)
      {
//         if (((hmiHSlider*)object)->lastThreadTime+100<wxGetLocalTimeMillis())
         {
//            ((hmiHSlider*)object)->lastThreadTime=wxGetLocalTimeMillis();
            g_flowPool.startFlows(object,NULL,OAPC_NUM_IO6,0,wxGetLocalTimeMillis(),0);
         }
      }
      else if (object->data.dataFlowFlags & HMI_FLOWFLAG_ON_CHANGING_OUT7)
      {
//         if (((hmiHSlider*)object)->lastThreadTime+100<wxGetLocalTimeMillis())
         {
//            ((hmiHSlider*)object)->lastThreadTime=wxGetLocalTimeMillis();
            g_flowPool.startFlows(object,NULL,OAPC_NUM_IO7,0,wxGetLocalTimeMillis(),0);
         }
      }
   }
}



void HMICanvas::OnNumberChanged(wxCommandEvent &event)
{
   hmiObject *object;

   SDBG
   object=g_objectList.getObjectByUIElementID(event.GetId());
   if (object)
   {
      if (object->data.dataFlowFlags & HMI_FLOWFLAG_ON_CHANGED_OUT6) DoOnSliderScrolled(object,OAPC_NUM_IO6,0,wxGetLocalTimeMillis());
      if (object->data.dataFlowFlags & HMI_FLOWFLAG_ON_CHANGED_OUT7) DoOnSliderScrolled(object,OAPC_NUM_IO7,0,wxGetLocalTimeMillis());
   }
}



void HMICanvas::OnNumberFocusChanged(wxFocusEvent &event)
{
   hmiObject *object;

   SDBG
   object=g_objectList.getObjectByUIElementID(event.GetId());
   if (object)
   {
      if (object->data.dataFlowFlags & HMI_FLOWFLAG_ON_CHANGED_OUT6)
      {
#ifdef ENV_DEBUGGER
         if (g_debugWin) g_debugWin->setDebugInformation(object,DEBUG_INFO_FOCUS_LOST,_T(""));
#endif
		 if (object->valueHasChanged())
          DoOnSliderScrolled(object,OAPC_NUM_IO6,0,wxGetLocalTimeMillis());
      }
      if (object->data.dataFlowFlags & HMI_FLOWFLAG_ON_CHANGED_OUT7)
      {
#ifdef ENV_DEBUGGER
         if (g_debugWin) g_debugWin->setDebugInformation(object,DEBUG_INFO_FOCUS_LOST,_T(""));
#endif
		 if (object->valueHasChanged())
          DoOnSliderScrolled(object,OAPC_NUM_IO7,0,wxGetLocalTimeMillis());
      }
   }
   event.Skip(true);
}



void HMICanvas::OnCharChanged(wxCommandEvent &event)
{
   hmiObject *object;

   SDBG
   object=g_objectList.getObjectByUIElementID(event.GetId());
   if (object)
   {
      if (object->data.dataFlowFlags & HMI_FLOWFLAG_ON_CHANGED_OUT6) DoOnSliderScrolled(object,OAPC_CHAR_IO6,0,wxGetLocalTimeMillis());
      if (object->data.dataFlowFlags & HMI_FLOWFLAG_ON_CHANGED_OUT7) DoOnSliderScrolled(object,OAPC_CHAR_IO7,0,wxGetLocalTimeMillis());
   }
}



void HMICanvas::OnCharFocusChanged(wxFocusEvent &event)
{
   hmiObject *object;

   SDBG
   object=g_objectList.getObjectByUIElementID(event.GetId());
   if (object)
   {
      if (object->data.dataFlowFlags & HMI_FLOWFLAG_ON_CHANGED_OUT6)
      {
#ifdef ENV_DEBUGGER
         if (g_debugWin) g_debugWin->setDebugInformation(object,DEBUG_INFO_FOCUS_LOST,_T(""));
#endif
		 if (object->valueHasChanged())
          DoOnSliderScrolled(object,OAPC_CHAR_IO6,0,wxGetLocalTimeMillis());
      }
      if (object->data.dataFlowFlags & HMI_FLOWFLAG_ON_CHANGED_OUT7)
      {
#ifdef ENV_DEBUGGER
         if (g_debugWin) g_debugWin->setDebugInformation(object,DEBUG_INFO_FOCUS_LOST,_T(""));
#endif
		 if (object->valueHasChanged())
          DoOnSliderScrolled(object,OAPC_CHAR_IO7,0,wxGetLocalTimeMillis());
      }
   }
   event.Skip(true);
}



void HMICanvas::OnNumberChanging(wxCommandEvent &event)
{
   hmiObject *object;

   SDBG
   object=g_objectList.getObjectByUIElementID(event.GetId());
   if (object)
   {
      if (object->data.dataFlowFlags & HMI_FLOWFLAG_ON_CHANGING_OUT6)
      {
//         if (((hmiHSlider*)object)->lastThreadTime+100<wxGetLocalTimeMillis())
         {
//            ((hmiHSlider*)object)->lastThreadTime=wxGetLocalTimeMillis();
            g_flowPool.startFlows(object,NULL,OAPC_NUM_IO6,0,wxGetLocalTimeMillis(),0);
         }
      }
      else if (object->data.dataFlowFlags & HMI_FLOWFLAG_ON_CHANGING_OUT7)
      {
//         if (((hmiHSlider*)object)->lastThreadTime+100<wxGetLocalTimeMillis())
         {
//            ((hmiHSlider*)object)->lastThreadTime=wxGetLocalTimeMillis();
            g_flowPool.startFlows(object,NULL,OAPC_NUM_IO7,0,wxGetLocalTimeMillis(),0);
         }
      }
   }
}


void HMICanvas::OnSpinNumberChanging(wxSpinEvent& event)
{
   wxCommandEvent cmdEvent;

   SDBG
   cmdEvent.SetId(event.GetId());
   OnNumberChanging(cmdEvent);
}

#endif
