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
 #include "../ExternalIOLib.cpp"
#else //ENV_INT

#include <wx/wx.h>
#include <wx/dynlib.h>

#ifndef ENV_WINDOWSCE
 #include <errno.h>
#endif

#ifndef ENV_BEAM
 #include "globals.h"
#else
 #include "../CNConstruct/cnco_globals.h"
#endif
#include "../OpenAPC/common/oapcResourceBundle.h"
#include "oapc_libio.h"
#include "hmiObject.h"
#include "ExternalIOLib.h"
#include "globals.h"
#include "common/oapcResourceBundle.h"

#ifdef ENV_WINDOWS
 #define snprintf _snprintf
#endif



ExternalIOLib::ExternalIOLib()
{
    SDBG
}



ExternalIOLib::ExternalIOLib(const wxString& libname,wxUint32 type,wxWindow *parentSplash)
#ifdef ENV_BEAM
              : m_beamID(0)
#endif
{
   char        *cname;
   char         lname[200+4],libname_c[200+4];
   wchar_t      buf[200];
   wxMBConvUTF8 conv;

   SDBG
   isValid=0;
   this->libname=libname;
   m_capabilities=0xFFFFFFFF;
   oapc_unicode_stringToCharUTF16BE(libname,libname_c,200);
   oapc_unicode_utf16BEToASCII(libname_c,200);
   snprintf(lname,200,"%s%s",LIB_PATH,libname_c);
#ifdef ENV_BEAM
 #ifdef _DEBUG
  #ifdef ENV_WINDOWS
   snprintf(lname,200,"..\\OpenAPC\\%s",libname_c);
  #endif
 #endif
#endif
   m_dynLib=oapc_dlib_load(lname);
   if (!m_dynLib)
   {
#ifndef ENV_HPLAYER
      if (parentSplash) parentSplash->Hide();
      wxMessageBox(libname+_T(":\n")+_("Could not load external Plug-In!"),_("Warning"),wxICON_WARNING|wxOK,const_cast<wxWindow*>(parentSplash));
#else
      printf("%s: Could not load external Plug-In!",lname);
#endif
      return;
   }
#ifdef ENV_EDITOR
   uiID=wxID_ANY;
   foldUiID=wxID_ANY;

   /* optional functions */
   oapc_get_save_data=   (lib_oapc_get_save_data)            oapc_dlib_get_symbol(m_dynLib,"oapc_get_save_data");
#endif
   /* optional functions */
   oapc_set_loaded_data=    (lib_oapc_set_loaded_data)        oapc_dlib_get_symbol(m_dynLib,"oapc_set_loaded_data");
   oapc_get_config_data=    (lib_oapc_get_config_data)        oapc_dlib_get_symbol(m_dynLib,"oapc_get_config_data");
   oapc_set_config_data =   (lib_oapc_set_config_data)        oapc_dlib_get_symbol(m_dynLib,"oapc_set_config_data");
   oapc_set_config_data_const=(lib_oapc_set_config_data_const)oapc_dlib_get_symbol(m_dynLib,"oapc_set_config_data");

   /* necessary functions */
   oapc_get_name        =(lib_oapc_get_name)        oapc_dlib_get_symbol(m_dynLib,"oapc_get_name");
   oapc_get_capabilities=(lib_oapc_get_capabilities)oapc_dlib_get_symbol(m_dynLib,"oapc_get_capabilities");
   oapc_get_input_flags =(lib_oapc_get_input_flags) oapc_dlib_get_symbol(m_dynLib,"oapc_get_input_flags");
   oapc_get_output_flags=(lib_oapc_get_output_flags)oapc_dlib_get_symbol(m_dynLib,"oapc_get_output_flags");
   oapc_create_instance= (lib_oapc_create_instance) oapc_dlib_get_symbol(m_dynLib,"oapc_create_instance");
   oapc_create_instance2=(lib_oapc_create_instance2)oapc_dlib_get_symbol(m_dynLib,"oapc_create_instance2");
   oapc_delete_instance= (lib_oapc_delete_instance) oapc_dlib_get_symbol(m_dynLib,"oapc_delete_instance");
   oapc_init=            (lib_oapc_init)            oapc_dlib_get_symbol(m_dynLib,"oapc_init");
   oapc_exit=            (lib_oapc_exit)            oapc_dlib_get_symbol(m_dynLib,"oapc_exit");
   oapc_read_pvalue=     (lib_oapc_read_pvalue)     oapc_dlib_get_symbol(m_dynLib,"oapc_read_pvalue");

#ifndef ENV_PLUGGER
   if (type==HMI_TYPE_MASK)
   {
      /* optional functions */
      oapc_get_hmi_config_data=(lib_oapc_get_hmi_config_data)oapc_dlib_get_symbol(m_dynLib,"oapc_get_hmi_config_data");

      /* necessary functions */
      oapc_set_numminmax=   (lib_oapc_set_numminmax)   oapc_dlib_get_symbol(m_dynLib,"oapc_set_numminmax");
      oapc_get_no_ui_flags =(lib_oapc_get_no_ui_flags) oapc_dlib_get_symbol(m_dynLib,"oapc_get_no_ui_flags");
      oapc_paint=           (lib_oapc_paint)           oapc_dlib_get_symbol(m_dynLib,"oapc_paint");

#ifdef ENV_EDITOR
      oapc_get_defsize=     (lib_oapc_get_defsize)     oapc_dlib_get_symbol(m_dynLib,"oapc_get_defsize");
      oapc_get_minsize=     (lib_oapc_get_minsize)     oapc_dlib_get_symbol(m_dynLib,"oapc_get_minsize");
      oapc_get_maxsize=     (lib_oapc_get_maxsize)     oapc_dlib_get_symbol(m_dynLib,"oapc_get_maxsize");
      oapc_get_numminmax=   (lib_oapc_get_numminmax)   oapc_dlib_get_symbol(m_dynLib,"oapc_get_numminmax");
      oapc_get_colours=     (lib_oapc_get_colours)     oapc_dlib_get_symbol(m_dynLib,"oapc_get_colours");
#endif
   }
#else
   type=type;
#endif

   if ((!oapc_create_instance2) && (!oapc_delete_instance))
   {
#ifndef ENV_HPLAYER
         wxMessageBox(libname+_T(":\n")+_("Could not load required symbols, plugin seems to be corrupt!"),_("Warning"),wxICON_WARNING|wxOK);
#else
         printf("%s: Could not load required symbols, plugin seems to be corrupt!",lname);
#endif
         return;
   }

#ifdef ENV_PLAYER
   if ((oapc_get_capabilities) && ((oapc_get_capabilities()) & OAPC_ACCEPTS_IO_CALLBACK))
   {
      oapc_set_io_callback= (lib_oapc_set_io_callback) oapc_dlib_get_symbol(m_dynLib,"oapc_set_io_callback");
      if (!oapc_set_io_callback) 
      {
#ifndef ENV_HPLAYER
         wxMessageBox(libname+_T(":\n")+_("Could not load required symbols, plugin seems to be corrupt!"),_("Warning"),wxICON_WARNING|wxOK);
#else
         printf("%s: Could not load required symbols, plugin seems to be corrupt!",lname);
#endif
         return;
      }
   }   
   else oapc_set_io_callback=NULL;
   parentSplash=parentSplash;
   
   oapc_set_digi_value=  (lib_oapc_set_digi_value)  oapc_dlib_get_symbol(m_dynLib,"oapc_set_digi_value");
   oapc_get_digi_value=  (lib_oapc_get_digi_value)  oapc_dlib_get_symbol(m_dynLib,"oapc_get_digi_value");
   oapc_set_num_value=   (lib_oapc_set_num_value)   oapc_dlib_get_symbol(m_dynLib,"oapc_set_num_value");
   oapc_get_num_value=   (lib_oapc_get_num_value)   oapc_dlib_get_symbol(m_dynLib,"oapc_get_num_value");
   oapc_set_char_value=  (lib_oapc_set_char_value)  oapc_dlib_get_symbol(m_dynLib,"oapc_set_char_value");
   oapc_get_char_value=  (lib_oapc_get_char_value)  oapc_dlib_get_symbol(m_dynLib,"oapc_get_char_value");
   oapc_set_bin_value=   (lib_oapc_set_bin_value)   oapc_dlib_get_symbol(m_dynLib,"oapc_set_bin_value");
   oapc_get_bin_value=   (lib_oapc_get_bin_value)   oapc_dlib_get_symbol(m_dynLib,"oapc_get_bin_value");
   oapc_release_bin_data=(lib_oapc_release_bin_data)oapc_dlib_get_symbol(m_dynLib,"oapc_release_bin_data");
#endif   

   m_internalType=0;
#ifdef ENV_BEAM
   oapc_get_beam_id=         (lib_oapc_get_beam_id)         oapc_dlib_get_symbol(m_dynLib,"oapc_get_beam_id");
   oapc_wait_stop=           (lib_oapc_wait_stop)           oapc_dlib_get_symbol(m_dynLib,"oapc_wait_stop");
   oapc_get_config_info_data=(lib_oapc_get_config_info_data)oapc_dlib_get_symbol(m_dynLib,"oapc_get_config_info_data");
   if (!oapc_get_config_info_data) return;
   else
   {
      struct config_info cfgInfo;
      void              *instData;
      int                ret;

      instData=oapc_create_instance2(OAPC_INSTANCE_MINIMUM_INIT);
      if (!instData) return;

      ret=oapc_get_config_info_data(instData,&cfgInfo);
      oapc_delete_instance(instData);
      if (cfgInfo.configType==INT_CONFIG_TYPE_SCANNERCONTROLLER)
      {
         if (!oapc_get_beam_id) return;
         m_beamID=oapc_get_beam_id();
         if (License::checkBeamID(m_beamID)!=OAPC_OK) return;
      }
      if (ret!=OAPC_OK) return;
      m_internalType=cfgInfo.configType;
   }
#endif
   
   if ((oapc_get_name) && (oapc_get_capabilities))
   {
      cname=oapc_get_name();
      if (strlen(cname)>0)
      {
         conv.MB2WC(buf,cname,sizeof(buf));
#ifndef ENV_HPLAYER
         name=g_res->getString(buf);
#else
         name=wxString(buf);
#endif
#ifdef ENV_BEAM
         if (m_beamID==GPL_BEAMID)
          name.append(_T(" (GPL'ed)"));
#endif
      }
//      getLibCapabilities();
   }
   else
   {
      wxASSERT(1);
   }
   isValid=1;
}



ExternalIOLib::~ExternalIOLib()
{
    SDBG
   if (m_dynLib) oapc_dlib_release(m_dynLib);
   m_dynLib=NULL;
}



wxUint32 ExternalIOLib::getLibCapabilities()
{
   SDBG
   if (m_capabilities!=0xFFFFFFFF) return m_capabilities;
   m_capabilities=oapc_get_capabilities();
   return m_capabilities;
}


wxUint32 ExternalIOLib::getLibInputFlags()
{
    SDBG
    return oapc_get_input_flags();
}



wxUint32 ExternalIOLib::getLibOutputFlags()
{
    SDBG
    return oapc_get_output_flags();
}


bool ExternalIOLib::compare(wxString findname)
{
   int pos;

   if (m_libnameStripped.length()==0)
   {
      pos=libname.Find('.',true);
      m_libnameStripped=libname.Mid(0,pos);
   }
   pos=findname.Find('.',true);
   findname=findname.Mid(0,pos);
#ifdef ENV_WINDOWS
   findname.Replace(_T("/"),_T("\\"),true);
#else
   findname.Replace(_T("\\"),_T("/"),true);
#endif
   return (m_libnameStripped.Cmp(findname)==0);
}

#endif //ENV_INT

