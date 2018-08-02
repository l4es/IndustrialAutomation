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

#ifdef ENV_INT
 #include "../hmiExternalIOLib.cpp"
#else //ENV_INT

#include <wx/wx.h>

#ifdef ENV_DEBUGGER
#include "DebugWin.h"
#endif
#ifndef __WXMSW__
#include <arpa/inet.h>
#endif

#include "iff.h"
#include "globals.h"
#include "oapc_libio.h"
#include "liboapc.h"
#include "hmiExternalIOLib.h"
#include "common/oapcMutexLocker.h"
#include "HMICanvas.h"
#ifdef ENV_EDITOR
 #include "DlgConfigflowName.h"
 #include "DlgConfigflowExternalIOLib.h"
 #include "DlgDataFlowhmiHSlider.h"
 #include "DlgDataFlowhmiTextField.h"
#else
 #include "flowWalkerPool.h"
#endif

#ifdef ENV_WINDOWS
 #ifdef ENV_WINDOWSCE
  #include "Winsock2.h"
 #endif
#endif




hmiExtLibContainer::hmiExtLibContainer(hmiExternalIOLib *lib,wxWindow* parent)
	: wxPanel(parent,wxID_ANY, wxDefaultPosition, wxDefaultSize,wxNO_BORDER)
{
   m_lib=lib;
   SetBackgroundStyle(wxBG_STYLE_CUSTOM);
   Connect(wxEVT_PAINT,wxPaintEventHandler(hmiExtLibContainer::OnPaint),NULL,this);
#ifdef ENV_PLAYER
   Connect(wxEVT_LEFT_DOWN,wxMouseEventHandler(hmiExtLibContainer::OnMouseEvent),NULL,this);
   Connect(wxEVT_LEFT_UP,wxMouseEventHandler(hmiExtLibContainer::OnMouseEvent),NULL,this);
   Connect(wxEVT_MIDDLE_DOWN,wxMouseEventHandler(hmiExtLibContainer::OnMouseEvent),NULL,this);
   Connect(wxEVT_MIDDLE_UP,wxMouseEventHandler(hmiExtLibContainer::OnMouseEvent),NULL,this);
   Connect(wxEVT_RIGHT_DOWN,wxMouseEventHandler(hmiExtLibContainer::OnMouseEvent),NULL,this);
   Connect(wxEVT_RIGHT_UP,wxMouseEventHandler(hmiExtLibContainer::OnMouseEvent),NULL,this);
   Connect(wxEVT_MOTION,wxMouseEventHandler(hmiExtLibContainer::OnMouseEvent),NULL,this);
   Connect(wxEVT_ENTER_WINDOW,wxMouseEventHandler(hmiExtLibContainer::OnMouseEvent),NULL,this);
   Connect(wxEVT_LEAVE_WINDOW,wxMouseEventHandler(hmiExtLibContainer::OnMouseEvent),NULL,this);
   Connect(wxEVT_LEFT_DCLICK,wxMouseEventHandler(hmiExtLibContainer::OnMouseEvent),NULL,this);
   Connect(wxEVT_MIDDLE_DCLICK,wxMouseEventHandler(hmiExtLibContainer::OnMouseEvent),NULL,this);
   Connect(wxEVT_RIGHT_DCLICK,wxMouseEventHandler(hmiExtLibContainer::OnMouseEvent),NULL,this);
#endif
}



hmiExtLibContainer::~hmiExtLibContainer()
{
   oapc_canvas_release_data(this);
}



void hmiExtLibContainer::OnPaint(wxPaintEvent &WXUNUSED(event))
{
   wxAutoBufferedPaintDC dc( this );
   wxASSERT(m_lib->lib->oapc_paint);
   if (m_lib->lib->oapc_paint)
   {
      dc.SetFont(m_lib->uiElement->GetFont()); // TODO: call this only the first time
      m_lib->lib->oapc_paint(m_lib->m_pluginData,&dc,(wxPanel*)m_lib->uiElement);
   }
}



#ifdef ENV_PLAYER
void hmiExtLibContainer::OnMouseEvent(wxMouseEvent& event)
{
   if (!m_lib->oapc_mouseevent) return;
   if (((m_lib->lib->getLibCapabilities() & OAPC_ACCEPTS_MOUSEDRAGS)!=0) && (event.Dragging()))
   {
      m_lib->oapc_mouseevent(m_lib->m_pluginData,&event);
      Refresh();
   }
   else if (((m_lib->lib->getLibCapabilities() & OAPC_ACCEPTS_MOUSEMOVES)!=0) && (event.Moving()))
   {
      m_lib->oapc_mouseevent(m_lib->m_pluginData,&event);
      Refresh();
   }
   else if (((m_lib->lib->getLibCapabilities() & OAPC_ACCEPTS_MOUSECLICKS)!=0) && 
            ((event.LeftDClick()) ||   (event.LeftDown())   || (event.LeftUp()) || 
             (event.RightDClick()) ||  (event.RightDown())  || (event.RightUp()) ||
             (event.MiddleDClick()) || (event.MiddleDown()) || (event.MiddleUp())))
   {
      m_lib->oapc_mouseevent(m_lib->m_pluginData,&event);
      Refresh();
   }
}
#endif



/**********************************************************************************/



hmiExternalIOLib::hmiExternalIOLib(ExternalIOLib *lib,BasePanel *parent,flowObject *obj,wxInt32 dataID)
                 :hmiHSlider(parent,obj)
{
   if (!returnOK) return;
   returnOK=false;
#ifdef ENV_PLAYER
   oapc_mouseevent=NULL;
   m_accessMutex=oapc_thread_mutex_create();
#endif
   m_pluginData=NULL;
#ifdef ENV_EDITOR
   xmlFlowDecoder=new XMLDecoder(this,(wxWindow*)g_mainWin,lib,m_pluginData);
   xmlHMIDecoder=new XMLDecoder(NULL, (wxWindow*)g_mainWin,lib,m_pluginData);
#endif
   this->data.type=HMI_TYPE_EXTERNAL_LIB;
   if (!initExternal(lib,true,dataID)) return;
   returnOK=true;
}



hmiExternalIOLib::hmiExternalIOLib(BasePanel *parent,flowObject *obj):hmiHSlider(parent,obj)
{
   if (!returnOK) return;
   returnOK=false;
   this->data.type=HMI_TYPE_EXTERNAL_LIB;
   lib=NULL;
#ifdef ENV_PLAYER
   oapc_mouseevent=NULL;
   m_accessMutex=oapc_thread_mutex_create();
#else
   xmlFlowDecoder=NULL;
   xmlHMIDecoder=NULL;
#endif
   returnOK=true;
}



#ifdef ENV_EDITOR
bool hmiExternalIOLib::initExternal(ExternalIOLib *lib,bool getDefaultValues,wxInt32 dataID)
#else
bool hmiExternalIOLib::initExternal(ExternalIOLib *lib,bool WXUNUSED(getDefaultValues),wxInt32 dataID)
#endif
{
   wxInt32 i;
#ifdef ENV_PLAYER
   wxInt32 rcode;
#endif

   returnOK=false;
   m_cycleMicros=125000;
   this->lib=lib;
   this->data.id=dataID;
#ifdef ENV_EDITOR
   if (!xmlFlowDecoder) xmlFlowDecoder=new XMLDecoder(this,(wxWindow*)g_mainWin,lib,m_pluginData);
   if (!xmlHMIDecoder)  xmlHMIDecoder=new XMLDecoder(NULL,(wxWindow*)g_mainWin,lib,m_pluginData);
   this->data.logFlags=0;
   flowBitmap=NULL;
#endif
   for (i=0; i<MAX_NUM_IOS; i++)
   {
#ifdef ENV_PLAYER
      prevDigi[i]=0;
      prevNum[i]=0;
#endif
      num[i]=0;
   }
   num[7]=30;
   digiIn1=0;
   prevDigiIn1=0;

   if ((!lib->oapc_get_capabilities) ||
       (!lib->oapc_get_no_ui_flags) ||
       ((!lib->oapc_get_input_flags) && (!lib->oapc_get_output_flags)) ||
       (!lib->oapc_paint)
#ifdef ENV_EDITOR
       ||
       (!lib->oapc_get_defsize) || (!lib->oapc_get_minsize) || (!lib->oapc_get_maxsize) ||
       /*(!lib->oapc_get_numminmax) ||*/ (!lib->oapc_get_colours)
#endif
       )
   {
      wxMessageBox(lib->libname+_T(":\n")+_("Could not load required symbols, plugin seems to be corrupt!"),_("Warning"),wxICON_WARNING|wxOK);
      return false;
   }
   m_pluginData=NULL;
   if (lib->oapc_create_instance2)
#ifdef ENV_PLAYER
    m_pluginData=lib->oapc_create_instance2(OAPC_INSTANCE_OPERATION);
#else
    m_pluginData=lib->oapc_create_instance2(OAPC_INSTANCE_MINIMUM_INIT);
#endif
   else m_pluginData=lib->oapc_create_instance();
   if (!m_pluginData)
   {
      g_OAPCMessageBox(OAPC_ERROR_NO_MEMORY,lib,NULL,(wxWindow*)g_mainWin,wxEmptyString);
      returnOK=0;
      return false;
   }
#ifdef ENV_EDITOR
   xmlFlowDecoder->setInstanceData(m_pluginData);
   xmlHMIDecoder->setInstanceData(m_pluginData);
   getLibConfigData();
   noUIFlags=lib->oapc_get_no_ui_flags();
   if (getDefaultValues)
   {
      float f1,f2;

      lib->oapc_get_defsize(&f1,&f2);
      data.msizex=f1*1000;
      data.msizey=f2*1000;
      lib->oapc_get_colours(&data.bgcolor[0],&data.fgcolor[0]);
      lib->oapc_get_colours(&data.bgcolor[1],&data.fgcolor[1]);
   }
   m_userPriviEnable=lib->getLibCapabilities() & OAPC_USERPRIVI_MASK;
#else
   /* capability-dependent functions */
   if (lib->oapc_init)
   {
      rcode=lib->oapc_init(m_pluginData);
      if (rcode!=OAPC_OK)
      {
         g_OAPCMessageBox(rcode,lib,m_pluginData,g_mainWin,wxEmptyString);
         returnOK=0;
      }
   }

   if ((lib->getLibCapabilities()) & (OAPC_ACCEPTS_MOUSEDRAGS|OAPC_ACCEPTS_MOUSECLICKS|OAPC_ACCEPTS_MOUSEMOVES))
   {
      oapc_mouseevent=(lib_oapc_mouseevent)oapc_dlib_get_symbol(lib->m_dynLib,"oapc_mouseevent");
   }
   if ((lib->getLibCapabilities() & OAPC_ACCEPTS_IO_CALLBACK))
   {
      if (lib->oapc_set_io_callback) lib->oapc_set_io_callback(m_pluginData,FlowWalkerPool_oapc_io_callback,data.id); // oapc_io_callback is located in flowWalkerPool.cpp
   }   
#endif
   if (lib->getLibCapabilities() & OAPC_HAS_INPUTS) this->data.stdIN=lib->getLibInputFlags();
   else this->data.stdIN=0;
   if (lib->getLibCapabilities() & OAPC_HAS_OUTPUTS) this->data.stdOUT=lib->getLibOutputFlags();
   else this->data.stdOUT=0;
   if (lib->getLibCapabilities() & OAPC_HAS_LOG_TYPE_DIGI)          this->data.logFlags=LOG_TYPE_DIGI;
   else if (lib->getLibCapabilities() & OAPC_HAS_LOG_TYPE_INTNUM)   this->data.logFlags=LOG_TYPE_INTNUM;
   else if (lib->getLibCapabilities() & OAPC_HAS_LOG_TYPE_FLOATNUM) this->data.logFlags=LOG_TYPE_FLOATNUM;
   else if (lib->getLibCapabilities() & OAPC_HAS_LOG_TYPE_CHAR)     this->data.logFlags=LOG_TYPE_CHAR;
   returnOK=true;
   return true;
}



wxWindow *hmiExternalIOLib::createUIElement()
{
   uiElement=new hmiExtLibContainer(this,m_parent);
#ifdef ENV_PLAYER
   if ((lib->getLibCapabilities() & OAPC_USERPRIVI_MASK) && (g_userPriviData.enabled)) uiElement->Show(false);
#endif
   hmiObject::createUIElement();
   return uiElement;
}



hmiExternalIOLib::~hmiExternalIOLib()
{
#ifndef ENV_PLAYER
   if (flowBitmap!=NULL) delete flowBitmap;
#else
   if ((lib) && (lib->oapc_exit)) lib->oapc_exit(m_pluginData);
   oapc_thread_mutex_release(m_accessMutex);
#endif
   if ((lib) && (lib->oapc_delete_instance) && (m_pluginData)) lib->oapc_delete_instance(m_pluginData);
#ifdef ENV_EDITOR
   if (xmlFlowDecoder) delete xmlFlowDecoder;
   if (xmlHMIDecoder) delete xmlHMIDecoder;
#endif
}



void hmiExternalIOLib::doApplyData(const wxByte all)
{
   if (all)
   {
      if (!applyLayoutData())
      {
         setSize(wxRealPoint(data.msizex,data.msizey));
         setPos(wxRealPoint(data.mposx,data.mposy));
      }
   }
   if (lib->oapc_set_numminmax) lib->oapc_set_numminmax(m_pluginData,MinValue,MaxValue);
   uiElement->SetBackgroundColour(wxColour(data.bgcolor[digi[0]]));
   uiElement->SetForegroundColour(wxColour(data.fgcolor[digi[0]]));
   applyFont(uiElement);
   uiElement->SetLabel(g_res->getString(text[digi[0]]));
   oapc_canvas_set_enabled((wxPanel*)uiElement,(data.state[digi[0]] & HMI_STATE_FLAG_DISABLED)==0);
   oapc_canvas_set_readonly((wxPanel*)uiElement,(data.state[digi[0]] & HMI_STATE_FLAG_RO)!=0);
   if ((data.state[digi[0]] & HMI_STATE_FLAG_DISABLED)==0) uiElement->SetCursor(wxNullCursor);
   else uiElement->SetCursor(wxCursor(wxCURSOR_NO_ENTRY));

   uiElement->Refresh();
   uiElement->Update();
}



#ifndef ENV_EDITOR
void hmiExternalIOLib::setUIElementState(wxInt32 state)
{
   wxASSERT(uiElement);
   if (!uiElement) return;
   if (state==2)
   {
      if (!uiElement->IsShown()) uiElement->Show(true);
      oapc_canvas_set_enabled((wxPanel*)uiElement,true);
   }
   else if (state==1)
   {
      if (!uiElement->IsShown()) uiElement->Show(true);
      oapc_canvas_set_enabled((wxPanel*)uiElement,false);
   }
   else if (state==0) uiElement->Show(false);
}

#else

void hmiExternalIOLib::setData(flowObject *object)
{
   hmiExternalIOLib *extObject;
   unsigned long     extLibDataSize;
   void             *extLibData=NULL;

   hmiObject::setData(object);
   extObject=(hmiExternalIOLib*)object;
   if (extObject->lib->oapc_get_save_data)
   {
      extLibData=lib->oapc_get_save_data(extObject->m_pluginData,&extLibDataSize);
      if (lib->oapc_set_loaded_data) lib->oapc_set_loaded_data(m_pluginData,extLibDataSize,(char*)extLibData);
   }
}



wxPoint hmiExternalIOLib::getMinSize()
{
   wxFloat32 x,y;

   lib->oapc_get_minsize(m_pluginData,&x,&y);
   return wxPoint(x,y);
}



wxPoint hmiExternalIOLib::getMaxSize()
{
   wxFloat32 x,y;

   lib->oapc_get_maxsize(m_pluginData,&x,&y);
   return wxPoint(x,y);
}


wxPoint hmiExternalIOLib::getDefSize()
{
   wxFloat32 x,y;

   if (lib->oapc_get_defsize)
   {
      lib->oapc_get_defsize(&x,&y);
      return wxPoint(x,y);
   }
   else return hmiObject::getDefSize();
}


wxString hmiExternalIOLib::getDefaultName()
{
    return lib->name;
}



void hmiExternalIOLib::getLibConfigData()
{
   char             *data;

   if (lib->oapc_get_hmi_config_data)
   {
      data=lib->oapc_get_hmi_config_data(m_pluginData);
      xmlHMIDecoder->setData(data);
   }
   if (lib->oapc_get_config_data)
   {
      char         *p,*oldLoc=NULL;

      p=setlocale(LC_NUMERIC,NULL);
      if (p) 
      {
         oldLoc=strdup(p);
         setlocale(LC_NUMERIC,"C");
      }
      data=lib->oapc_get_config_data(m_pluginData);
      if (p) 
      {
         setlocale(LC_NUMERIC,oldLoc);
         free(oldLoc);
      }

      xmlFlowDecoder->setData(data);
      flowBitmap=xmlFlowDecoder->getFlowBitmap();
   }

}



wxPanel* hmiExternalIOLib::getConfigPanel(wxString *name,wxWindow *parent,wxInt32 num)
{
   bool tmp;

   if (num==0) getLibConfigData();
   return xmlHMIDecoder->getConfigPanel(&tmp,name,parent,num);
}



void hmiExternalIOLib::setConfigData()
{
   if ((lib->getLibCapabilities() & OAPC_ACCEPTS_PLAIN_CONFIGURATION) && ((lib->oapc_set_config_data) || (lib->oapc_set_config_data_const)))
   {
      wxString          name,value;
      wxFont            font;
      wxMBConvUTF8      conv;

      while (xmlHMIDecoder->getNextPair(&name,&value,&font))
      {
         char c1[50],c2[2000];

         conv.WC2MB(c1,name,50);
         conv.WC2MB(c2,value,2000);
         if (lib->oapc_set_config_data_const) lib->oapc_set_config_data_const(m_pluginData,c1,c2);
         else lib->oapc_set_config_data(m_pluginData,c1,c2);
      }
   }
}



void hmiExternalIOLib::doDataFlowDialog(bool hideISConfig)
{
   if ((lib->getLibCapabilities()) & OAPC_HAS_STANDARD_FLOW_CONFIGURATION)
   {
      wxUint32 *cycleMicros;

      noFlowFlags=0;
      cycleMicros=NULL;

      if (((lib->getLibCapabilities()) & OAPC_ACCEPTS_IO_CALLBACK)==0)
       cycleMicros=&m_cycleMicros;
      if ((data.stdIN & OAPC_DIGI_IO_MASK)==OAPC_DIGI_IO0) noFlowFlags|=HMI_NOFLOWFLAG_ON_DIGI_IN1;
      if (((data.stdOUT & OAPC_NUM_IO6)!=OAPC_NUM_IO6) &&
          ((data.stdOUT & OAPC_CHAR_IO6)!=OAPC_CHAR_IO6)) noFlowFlags|=HMI_NOFLOWFLAG_OUT6;
      if ((data.stdOUT & OAPC_NUM_IO7)==OAPC_NUM_IO7)
      {
         DlgDataFlowhmiHSlider dlg(this,m_parent,cycleMicros,_("Data flow definition"),noFlowFlags,hideISConfig);
         dlg.ShowModal();
      }
      else if ((data.stdOUT & OAPC_CHAR_IO7)==OAPC_CHAR_IO7)
      {
         DlgDataFlowhmiTextField dlg(this,m_parent,cycleMicros,_("Data flow definition"),noFlowFlags,hideISConfig);
         dlg.ShowModal();
      }
      else if ((data.stdIN & OAPC_DIGI_IO_MASK)==OAPC_DIGI_IO0)
      {
         DlgConfigflowName dlg(this,m_parent,_("Data flow definition"),hideISConfig);
         dlg.ShowModal();
      }
      else wxMessageBox(_("Plug-In error: a standard flow dialogue was requested by the Plug-In\nbut there is no standard IO configuration!"),_("Error"),wxICON_ERROR);
   }
   else
   {
      wxMBConvUTF8      conv;
      wxString          title;
      wxUint32          cycleMicros=0;

      getLibConfigData();
      title=name+_T(" ")+_("Definition")+name.Format(_T(" (%d)"),data.id);
      DlgConfigflowExternalIOLib dlg(&name,xmlFlowDecoder,NULL,data.stdOUT,&cycleMicros,title,hideISConfig);

      dlg.ShowModal();
      dlg.Destroy();
   }
}




wxInt32 hmiExternalIOLib::save(wxFile *FHandle)
{
   wxInt32                length;
   unsigned long          extLibDataSize;
   void                  *extLibData=NULL;
   struct externalHMIData saveData;
   wxMBConvUTF16BE         conv;

   if (!FHandle) return 0;

   FHandle->Write(CHUNK_HOBL_EXHM,4);
   if (lib->oapc_get_save_data) extLibData=lib->oapc_get_save_data(m_pluginData,&extLibDataSize);
   else extLibDataSize=0;
   length=sizeof(struct hmiObjectHead)+sizeof(struct hmiObjectData)+sizeof(struct externalHMIData)+extLibDataSize;
   length=htonl(length);
   FHandle->Write(&length,4);

   // *** HMI standard structure ***
   length=length+hmiObject::save(FHandle);

   // *** definitions for external data ***
   saveData.version=htonl(1);
   saveData.length=htonl(sizeof(struct externalHMIData));
   saveData.reserved1=0;
   saveData.reserved2=0;
   saveData.reserved3=0;
   saveData.reserved4=0;   
   saveData.cycleMicros=htonl(m_cycleMicros);
   saveData.extLibDataSize=htonl(extLibDataSize);
   conv.WC2MB(saveData.store_libname,lib->libname,sizeof(saveData.store_libname));
   length+=FHandle->Write(&saveData,sizeof(struct externalHMIData));

   // ** custom data of the external library ***
   if ((extLibDataSize>0) && (extLibData)) length+=FHandle->Write(extLibData,extLibDataSize);

   return length;
}
#endif



wxInt32 hmiExternalIOLib::load(wxFile *FHandle,wxUint32 chunkSize,wxUint32 IDOffset)
{
   wxUint32               loaded,extLibDataSize;
   void                  *extLibData;
   struct externalHMIData loadData;
   wxMBConvUTF16BE        conv;
   wchar_t                buf[MAX_LIBNAME_LENGTH];
   wxString               libname;
   ExternalIOLib         *extIOLib;
   bool                   ret=true;

   // *** HMI standard structure ***
   loaded=hmiObject::load(FHandle,chunkSize,IDOffset);

   // *** definitions for external data ***
   loaded+=FHandle->Read(&loadData,sizeof(struct externalHMIData));
   m_cycleMicros=ntohl(loadData.cycleMicros);
   extLibDataSize=ntohl(loadData.extLibDataSize);
   conv.MB2WC(buf,loadData.store_libname,sizeof(buf));
   libname=buf;

   // *** now find the plugin with that name

#ifdef ENV_LINUX
   libname.Replace(_T(".dll"),_T(".so"));
   libname.Replace(_T(".DLL"),_T(".so"));
   libname.Replace(_T("\\"),_T("/"));
#endif
#ifdef ENV_WINDOWS
   libname.Replace(_T(".so"),_T(".dll"));
   libname.Replace(_T("/"),_T("\\"));
#endif

   if (!g_externalHMILibs->contains(libname))
    g_externalHMILibs->importPlugin(libname,HMI_TYPE_MASK,NULL);
   extIOLib=g_externalHMILibs->getLib(true);
   while (extIOLib)
   {
      if (extIOLib->compare(libname))
      {
         ret=initExternal(extIOLib,false,data.id);
         break;
      }
      extIOLib=g_externalHMILibs->getLib(false);
   }
   wxASSERT_MSG(lib,libname);
   if ((!lib) || (!ret)) returnOK=false;
   else
   {
      if (extLibDataSize>0) // can be 0 if a plug in doesn't stores own data
      {
      // *** load the xustom data of the external libray ***
         extLibData=malloc(extLibDataSize);
         if (extLibData)
         {
            loaded+=FHandle->Read(extLibData,extLibDataSize);
            if (lib->oapc_set_loaded_data)
             lib->oapc_set_loaded_data(m_pluginData,extLibDataSize,(char*)extLibData);
            else returnOK=false;
            free(extLibData);
         }
         else
         {
            if (FHandle->Seek(extLibDataSize,wxFromCurrent)>0) loaded+=(extLibDataSize);
         }
      }
   }
   return loaded;
}



#ifdef ENV_PLAYER
wxUint64 hmiExternalIOLib::getAssignedOutput(wxUint64 input)
{
   if ((lib->getLibCapabilities()) & OAPC_HAS_STANDARD_FLOW_CONFIGURATION)
    return hmiHSlider::getAssignedOutput(input);
   return 0;
}



/**
Sets an digital input and checks if the same thread has set that input before
@param[in] input the flag that specifies the input
@param[in] value the value that has to be set for that input (0 or 1)
@param[in] flowThreadID the Id of the thread that wants to set this value
@return 0 if the same thread already has set a value; the thread has to finish
        in this case to avoid endless loops; 1 if the input value was set correctly
*/
wxByte hmiExternalIOLib::setDigitalInput(FlowConnection *connection,wxByte value,wxUint32 *flowThreadID,flowObject *WXUNUSED(object))
{
   wxInt32  rcode;

   if (g_breakThreads) return 0;
   if (!threadIDOK(flowThreadID,false)) return 0;
   if (!lib->oapc_set_digi_value)
   {
#ifdef ENV_DEBUGGER
      g_debugWin->setDebugInformation(this,DEBUG_STOP_COND_LIB_DOESNT_PROVIDES_INPUT,_T(""));
#endif
      return 0;
   }

   if (value!=0) value=1;

   if ((connection->targetInputNum==1) && ((lib->getLibCapabilities()) & OAPC_HAS_STANDARD_FLOW_CONFIGURATION))
   {
       prevDigiIn1=digiIn1;
       digiIn1=value;
       if ((data.dataFlowFlags & HMI_FLOWFLAG_ON_DIGI_IN1_HI_OUT6) && (value))
        ((HMICanvas*)m_parent)->DoOnSliderScrolled(this,OAPC_NUM_IO6,*flowThreadID,wxGetLocalTimeMillis());
       else if ((data.dataFlowFlags & HMI_FLOWFLAG_ON_DIGI_IN1_LO_OUT6) && (!value))
        ((HMICanvas*)m_parent)->DoOnSliderScrolled(this,OAPC_NUM_IO6,*flowThreadID,wxGetLocalTimeMillis());
       if ((data.dataFlowFlags & HMI_FLOWFLAG_ON_DIGI_IN1_HI_OUT7) && (value))
        ((HMICanvas*)m_parent)->DoOnSliderScrolled(this,OAPC_NUM_IO7,*flowThreadID,wxGetLocalTimeMillis());
       else if ((data.dataFlowFlags & HMI_FLOWFLAG_ON_DIGI_IN1_LO_OUT7) && (!value))
        ((HMICanvas*)m_parent)->DoOnSliderScrolled(this,OAPC_NUM_IO7,*flowThreadID,wxGetLocalTimeMillis());
       return 0; // return with 0 because afterwards no digi thread has to run but a numerical thread
   }

   wxASSERT((connection->targetInputNum>=0) && (connection->targetInputNum<MAX_NUM_IOS));
   rcode=lib->oapc_set_digi_value(m_pluginData,connection->targetInputNum,value);
   digi[connection->targetInputNum]=value;
   if (rcode!=OAPC_OK)
   {
#ifdef ENV_DEBUGGER
      // TODO: call of GUI-functions out of a thread will crash under Linux
      if ((rcode & OAPC_ERROR_MASK)==OAPC_ERROR_NO_SUCH_IO)
       g_debugWin->setDebugInformation(this,DEBUG_STOP_COND_ILLEGAL_IO,_T(""));
      else if ((rcode & OAPC_ERROR_MASK)==OAPC_ERROR_NOT_SUPPORTEDi)
       g_debugWin->setDebugInformation(this,DEBUG_STOP_COND_LIB_DOESNT_PROVIDES_INPUT,_T(""));
      else
       g_debugWin->setDebugInformation(this,DEBUG_STOP_COND_LIB_FAILED,_T(""));
      g_OAPCMessageBox(rcode,lib,m_pluginData,g_mainWin,wxEmptyString);
#endif
      return 0;
   }
   applyData(0);
   return 1;
}



wxByte hmiExternalIOLib::getDigitalOutput(FlowConnection *connection,wxInt32 *rcode,wxUint64 WXUNUSED(lastInput),wxLongLong WXUNUSED(origCreationTime))
{
   oapcMutexLocker locker(m_accessMutex,__FILE__,__LINE__);

   if (g_breakThreads) return 0;
   if (!lib->oapc_get_digi_value)
   {
#ifdef ENV_DEBUGGER
      g_debugWin->setDebugInformation(this,DEBUG_STOP_COND_LIB_DOESNT_PROVIDES_INPUT,_T(""));
#endif
      *rcode=OAPC_ERROR_NOT_SUPPORTEDi;
      return 0;
   }
   prevDigi[connection->sourceOutputNum]=digi[connection->sourceOutputNum];
   wxASSERT((connection->sourceOutputNum>=0) && (connection->sourceOutputNum<MAX_NUM_IOS));
   *rcode=lib->oapc_get_digi_value(m_pluginData,connection->sourceOutputNum,&digi[connection->sourceOutputNum]);
   if (*rcode==OAPC_OK)
   {
      logDataCtrl->logDigi(digi[connection->sourceOutputNum],this);
      return digi[connection->sourceOutputNum];
   }
   return 0;
}



/**
Sets an numerical input and checks if the same thread has set that input before
@param[in] input the flag that specifies the input
@param[in] value the value that has to be set for that input
@param[in] flowThreadID the Id of the thread that wants to set this value
@return 0 if the same thread already has set a value; the thread has to finish
        in this case to avoid endless loops; 1 if the input value was set correctly
*/
wxByte hmiExternalIOLib::setNumInput(FlowConnection *connection,wxFloat64 value,wxUint32 *flowThreadID,flowObject *WXUNUSED(object))
{
   wxInt32  rcode;

   if (g_breakThreads) return 0;
   if (!threadIDOK(flowThreadID,false)) return 0;
   if (!lib->oapc_set_num_value)
   {
#ifdef ENV_DEBUGGER
      g_debugWin->setDebugInformation(this,DEBUG_STOP_COND_LIB_DOESNT_PROVIDES_INPUT,_T(""));
#endif
      return 0;
   }
   if ((connection->targetInputNum<0) || (connection->targetInputNum>=MAX_NUM_IOS))
   {
#ifdef ENV_DEBUGGER
      g_debugWin->setDebugInformation(this,DEBUG_STOP_COND_ILLEGAL_IO,_T(""));
#endif
      return 0;
   }
   num[connection->targetInputNum]=value;
   rcode=lib->oapc_set_num_value(m_pluginData,connection->targetInputNum,value);
   if (rcode!=OAPC_OK)
   {
#ifdef ENV_DEBUGGER
      // TODO: call of GUI-functions out of a thread will crash under Linux
      if ((rcode & OAPC_ERROR_MASK)==OAPC_ERROR_NO_SUCH_IO)
       g_debugWin->setDebugInformation(this,DEBUG_STOP_COND_ILLEGAL_IO,_T(""));
      else if ((rcode & OAPC_ERROR_MASK)==OAPC_ERROR_NOT_SUPPORTEDi)
       g_debugWin->setDebugInformation(this,DEBUG_STOP_COND_LIB_DOESNT_PROVIDES_INPUT,_T(""));
      else
       g_debugWin->setDebugInformation(this,DEBUG_STOP_COND_LIB_FAILED,_T(""));
      g_OAPCMessageBox(rcode,lib,m_pluginData,g_mainWin,wxEmptyString);
#endif
      return 0;
   }
   applyData(0);
   return 1;
}



wxFloat64 hmiExternalIOLib::getNumOutput(FlowConnection *connection,wxInt32 *rcode,wxUint64 WXUNUSED(lastInput),wxLongLong WXUNUSED(origCreationTime))
{
   oapcMutexLocker locker(m_accessMutex,__FILE__,__LINE__);

   if (g_breakThreads) return 0.0;
   if (!lib->oapc_get_num_value)
   {
#ifdef ENV_DEBUGGER
      g_debugWin->setDebugInformation(this,DEBUG_STOP_COND_LIB_DOESNT_PROVIDES_INPUT,_T(""));
#endif
      *rcode=OAPC_ERROR_NOT_SUPPORTEDi;
      return 0.0;
   }

   prevNum[connection->sourceOutputNum]=num[connection->sourceOutputNum];
   wxASSERT((connection->sourceOutputNum>=0) && (connection->sourceOutputNum<MAX_NUM_IOS));
   *rcode=lib->oapc_get_num_value(m_pluginData,connection->sourceOutputNum,&num[connection->sourceOutputNum]);
   if (*rcode!=OAPC_OK) return 0;
   logDataCtrl->logNum(num[connection->sourceOutputNum],this);

   if ((lib->getLibCapabilities()) & OAPC_HAS_STANDARD_FLOW_CONFIGURATION)
   {
      if (connection->sourceOutputNum==6)
      {
         if (data.dataFlowFlags & HMI_FLOWFLAG_ON_CHANGING_OUT6)
         {
            if (prevNum[connection->sourceOutputNum]==num[connection->sourceOutputNum])
            {
               *rcode=OAPC_ERROR_NO_DATA_AVAILABLE;
               return 0;
            }
         }
         else if (data.dataFlowFlags & HMI_FLOWFLAG_ON_CHANGED_OUT6)
         {
            if (prevNum[connection->sourceOutputNum]!=num[connection->sourceOutputNum])
            {
               *rcode=OAPC_ERROR_NO_DATA_AVAILABLE;
               return 0;
            }
         }
         else if (data.dataFlowFlags & HMI_FLOWFLAG_ON_DIGI_IN1_HI_OUT6)
         {
            if (((prevDigiIn1!=0) && (digiIn1!=1)) || (digiIn1==prevDigiIn1))
            {
               *rcode=OAPC_ERROR_NO_DATA_AVAILABLE;
               return 0;
            }
         }
         else if (data.dataFlowFlags & HMI_FLOWFLAG_ON_DIGI_IN1_LO_OUT6)
         {
            if (((prevDigiIn1!=1) && (digiIn1!=0)) || (digiIn1==prevDigiIn1))
            {
               *rcode=OAPC_ERROR_NO_DATA_AVAILABLE;
               return 0;
            }
         }
      }
      else if (connection->sourceOutputNum==7)
      {
         if (data.dataFlowFlags & HMI_FLOWFLAG_ON_CHANGING_OUT7)
         {
            if (prevNum[connection->sourceOutputNum]==num[connection->sourceOutputNum])
            {
               *rcode=OAPC_ERROR_NO_DATA_AVAILABLE;
               return 0;
            }
         }
         else if (data.dataFlowFlags & HMI_FLOWFLAG_ON_CHANGED_OUT7)
         {
            if (prevNum[connection->sourceOutputNum]!=num[connection->sourceOutputNum])
            {
               *rcode=OAPC_ERROR_NO_DATA_AVAILABLE;
               return 0;
            }
         }
         else if (data.dataFlowFlags & HMI_FLOWFLAG_ON_DIGI_IN1_HI_OUT7)
         {
            if (((prevDigiIn1!=0) && (digiIn1!=1)) || (digiIn1==prevDigiIn1))
            {
               *rcode=OAPC_ERROR_NO_DATA_AVAILABLE;
               return 0;
            }
         }
         else if (data.dataFlowFlags & HMI_FLOWFLAG_ON_DIGI_IN1_LO_OUT7)
         {
            if (((prevDigiIn1!=1) && (digiIn1!=0)) || (digiIn1==prevDigiIn1))
            {
               *rcode=OAPC_ERROR_NO_DATA_AVAILABLE;
               return 0;
            }
         }
      }
   }

   digiIn1=1-digiIn1;
   return num[connection->sourceOutputNum];
}



/**
Sets an character input and checks if the same thread has set that input before
@param[in] input the flag that specifies the input
@param[in] value the value that has to be set for that input
@param[in] flowThreadID the Id of the thread that wants to set this value
@return 0 if the same thread already has set a value; the thread has to finish
        in this case to avoid endless loops; 1 if the input value was set correctly
*/
wxByte hmiExternalIOLib::setCharInput(FlowConnection *connection,wxString value,wxUint32 *flowThreadID,flowObject *WXUNUSED(object))
{
   wxInt32 rcode;
   char    c[300+4];

   if (g_breakThreads) return 0;
   if (!threadIDOK(flowThreadID,false)) return 0;
   if (!lib->oapc_set_char_value)
   {
#ifdef ENV_DEBUGGER
      g_debugWin->setDebugInformation(this,DEBUG_STOP_COND_LIB_DOESNT_PROVIDES_INPUT,_T(""));
#endif
      return 0;
   }

   oapc_unicode_stringToCharUTF8(value,c,300);
   wxASSERT((connection->targetInputNum>=0) && (connection->targetInputNum<MAX_NUM_IOS));
   rcode=lib->oapc_set_char_value(m_pluginData,connection->targetInputNum,c);
   if (rcode!=OAPC_OK)
   {
#ifdef ENV_DEBUGGER
      // TODO: call of GUI-functions out of a thread will crash under Linux
      if ((rcode & OAPC_ERROR_MASK)==OAPC_ERROR_NO_SUCH_IO)
       g_debugWin->setDebugInformation(this,DEBUG_STOP_COND_ILLEGAL_IO,_T(""));
      else if ((rcode & OAPC_ERROR_MASK)==OAPC_ERROR_NOT_SUPPORTEDi)
       g_debugWin->setDebugInformation(this,DEBUG_STOP_COND_LIB_DOESNT_PROVIDES_INPUT,_T(""));
      else
       g_debugWin->setDebugInformation(this,DEBUG_STOP_COND_LIB_FAILED,_T(""));
      g_OAPCMessageBox(rcode,lib,m_pluginData,g_mainWin,wxEmptyString);
#endif
      return 0;
   }
   applyData(0);
   return 1;
}



wxString hmiExternalIOLib::getCharOutput(FlowConnection *connection,wxInt32 *rcode,wxUint64 WXUNUSED(lastInput))
{
   wxInt32       length=300;
   char          c[300+4];
   oapcMutexLocker locker(m_accessMutex,__FILE__,__LINE__);

   if (g_breakThreads) return wxEmptyString;
   if (!lib->oapc_get_char_value)
   {
#ifdef ENV_DEBUGGER
      g_debugWin->setDebugInformation(this,DEBUG_STOP_COND_LIB_DOESNT_PROVIDES_INPUT,_T(""));
#endif
      *rcode=OAPC_ERROR_NOT_SUPPORTEDi;
      return _T("");
   }

   prevTxt[connection->sourceOutputNum]=txt[connection->sourceOutputNum];
   wxASSERT((connection->sourceOutputNum>=0) && (connection->sourceOutputNum<MAX_NUM_IOS));
   *rcode=lib->oapc_get_char_value(m_pluginData,connection->sourceOutputNum,length,c);
   if (*rcode!=OAPC_OK) return _T("");
   logDataCtrl->logChar(txt[connection->sourceOutputNum],this);
   oapc_unicode_charToStringUTF8(c,length,&txt[connection->sourceOutputNum]);

   if ((lib->getLibCapabilities()) & OAPC_HAS_STANDARD_FLOW_CONFIGURATION)
   {
      if (connection->sourceOutputNum==6)
      {
         if (data.dataFlowFlags & HMI_FLOWFLAG_ON_CHANGING_OUT6)
         {
            if (prevTxt[connection->sourceOutputNum].Cmp(txt[connection->sourceOutputNum]))
            {
               *rcode=OAPC_ERROR_NO_DATA_AVAILABLE;
               return _T("");
            }
         }
         if (data.dataFlowFlags & HMI_FLOWFLAG_ON_CHANGED_OUT6)
         {
            if (prevTxt[connection->sourceOutputNum].Cmp(txt[connection->sourceOutputNum]))
            {
               *rcode=OAPC_ERROR_NO_DATA_AVAILABLE;
               return _T("");
            }
         }
      }
      else if (connection->sourceOutputNum==7)
      {
         if (data.dataFlowFlags & HMI_FLOWFLAG_ON_CHANGING_OUT7)
         {
            if (prevTxt[connection->sourceOutputNum].Cmp(txt[connection->sourceOutputNum]))
            {
               *rcode=OAPC_ERROR_NO_DATA_AVAILABLE;
               return _T("");
            }
         }
         if (data.dataFlowFlags & HMI_FLOWFLAG_ON_CHANGED_OUT7)
         {
            if (prevTxt[connection->sourceOutputNum].Cmp(txt[connection->sourceOutputNum]))
            {
               *rcode=OAPC_ERROR_NO_DATA_AVAILABLE;
               return _T("");
            }
         }
      }
   }
   return txt[connection->sourceOutputNum];
}



wxByte hmiExternalIOLib::setBinInput(FlowConnection *connection,oapcBinHeadSp &value,wxUint32 *flowThreadID,flowObject *WXUNUSED(object))
{
   wxInt32  rcode;

   if (g_breakThreads) return 0;
   if (!threadIDOK(flowThreadID,false)) return 0;
   if (!lib->oapc_set_bin_value)
   {
#ifdef ENV_DEBUGGER
      g_debugWin->setDebugInformation(this,DEBUG_STOP_COND_LIB_DOESNT_PROVIDES_INPUT,_T(""));
#endif
      return 0;
   }

   wxASSERT((connection->targetInputNum>=0) && (connection->targetInputNum<MAX_NUM_IOS));
   rcode=lib->oapc_set_bin_value(m_pluginData,connection->targetInputNum,value->m_bin);
   if (rcode!=OAPC_OK)
   {
#ifdef ENV_DEBUGGER
      // TODO: call of GUI-functions out of a thread will crash under Linux
      if ((rcode & OAPC_ERROR_MASK)==OAPC_ERROR_NO_SUCH_IO)
       g_debugWin->setDebugInformation(this,DEBUG_STOP_COND_ILLEGAL_IO,_T(""));
      else if ((rcode & OAPC_ERROR_MASK)==OAPC_ERROR_NOT_SUPPORTEDi)
       g_debugWin->setDebugInformation(this,DEBUG_STOP_COND_LIB_DOESNT_PROVIDES_INPUT,_T(""));
      else
       g_debugWin->setDebugInformation(this,DEBUG_STOP_COND_LIB_FAILED,_T(""));
      g_OAPCMessageBox(rcode,lib,m_pluginData,g_mainWin,wxEmptyString);
#endif
      return 0;
   }
   return 1;
}



oapcBinHeadSp hmiExternalIOLib::getBinOutput(FlowConnection *connection,wxInt32 *rcode,wxUint64 WXUNUSED(lastInput))
{
   struct oapc_bin_head *ret=NULL;
   oapcMutexLocker       locker(m_accessMutex,__FILE__,__LINE__);

   if (g_breakThreads) return oapcBinHeadSp();
   if (!lib->oapc_get_bin_value)
   {
#ifdef ENV_DEBUGGER
      g_debugWin->setDebugInformation(this,DEBUG_STOP_COND_LIB_DOESNT_PROVIDES_INPUT,_T(""));
#endif
      *rcode=OAPC_ERROR_NOT_SUPPORTEDi;
      return oapcBinHeadSp();
   }

   wxASSERT((connection->sourceOutputNum>=0) && (connection->sourceOutputNum<MAX_NUM_IOS));
   *rcode=lib->oapc_get_bin_value(m_pluginData,connection->sourceOutputNum,&ret);
   if (*rcode!=OAPC_OK) return oapcBinHeadSp();
   if ((ret->type==OAPC_BIN_TYPE_TEXT) && (ret->subType==OAPC_BIN_SUBTYPE_TEXT_PLAIN) && (((int)ntohl(ret->param2))==-1))
   {
   // we received a debug/error text from the plug-in
#ifdef ENV_DEBUGGER
      wxString errorMessage;

      oapc_unicode_charToStringASCII(&ret->data,ntohl(ret->param1),&errorMessage);
      g_debugWin->setDebugInformation(this,DEBUG_STOP_COND_LIB_RETURNED_ERROR,errorMessage);
#endif
      lib->oapc_release_bin_data(m_pluginData,connection->sourceOutputNum);
      *rcode=OAPC_ERROR;
      return oapcBinHeadSp();
   }
   return oapcBinHeadSp(new oapcBinHead(ret));
}



void hmiExternalIOLib::releaseBinData(FlowConnection *connection)
{
   lib->oapc_release_bin_data(m_pluginData,connection->sourceOutputNum);
}

#endif

#endif //ENV_INT
