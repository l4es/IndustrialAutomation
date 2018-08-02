#ifdef ENV_INT
#include "../ExternalIOLib.h"
#else

#ifndef EXTERNALIOLIB_H
#define EXTERNALIOLIB_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <wx/dynlib.h>
#include <wx/xml/xml.h>
#include <wx/dcbuffer.h>
#include <wx/panel.h>

#include "oapc_libio.h"

typedef char*          (*lib_oapc_get_name)(void);
typedef unsigned long  (*lib_oapc_get_capabilities)(void);
typedef unsigned long  (*lib_oapc_get_input_flags) (void);
typedef unsigned long  (*lib_oapc_get_output_flags)(void);
typedef char          *(*lib_oapc_get_config_data) (void* instanceData);
typedef void           (*lib_oapc_set_config_data) (void* instanceData,char *name,char *value);
typedef void           (*lib_oapc_set_config_data_const) (void* instanceData,const char *name,const char *value);
typedef void           (*lib_oapc_set_loaded_data) (void* instanceData,unsigned long length,char *data);
typedef void*          (*lib_oapc_create_instance) (void);
typedef void*          (*lib_oapc_create_instance2)(unsigned long flags);
typedef unsigned long  (*lib_oapc_init)            (void* instanceData);
typedef unsigned long  (*lib_oapc_exit)            (void* instanceData);
typedef unsigned long  (*lib_oapc_read_pvalue)     (void* instanceData,double param,double *value);
typedef void           (*lib_oapc_delete_instance) (void* instanceData);
typedef unsigned long  (*lib_oapc_get_no_ui_flags)      (void);
typedef char          *(*lib_oapc_get_hmi_config_data)  (void* instanceData);
typedef void           (*lib_oapc_paint)                (void* instanceData,wxAutoBufferedPaintDC *dc,wxPanel *canvas);
typedef void           (*lib_oapc_mouseevent)           (void* instanceData,wxMouseEvent* event);
typedef void           (*lib_oapc_get_numminmax)        (void* instanceData,wxFloat32 *minValue,wxFloat32 *maxValue);
typedef void           (*lib_oapc_set_numminmax)        (void* instanceData,wxFloat32 minValue,wxFloat32 maxValue);

#ifdef ENV_BEAM
typedef unsigned long  (*lib_oapc_get_config_info_data)(void *instanceData,struct config_info *fillStruct);
typedef unsigned long  (*lib_oapc_get_beam_id)();
typedef unsigned long  (*lib_oapc_wait_stop)(void *instanceData,const unsigned long flags);
#endif

#ifdef ENV_PLAYER
typedef void           (*lib_oapc_set_io_callback) (void* instanceData,lib_oapc_io_callback oapc_io_callback,unsigned long callbackID);
typedef unsigned long  (*lib_oapc_set_digi_value)  (void* instanceData,unsigned long input,unsigned char value);
typedef unsigned long  (*lib_oapc_get_digi_value)  (void* instanceData,unsigned long output,unsigned char *value);
typedef unsigned long  (*lib_oapc_set_num_value)   (void* instanceData,unsigned long input,double value);
typedef unsigned long  (*lib_oapc_get_num_value)   (void* instanceData,unsigned long output,double *value);
typedef unsigned long  (*lib_oapc_set_char_value)  (void* instanceData,unsigned long input,char *value);
typedef unsigned long  (*lib_oapc_get_char_value)  (void* instanceData,unsigned long output,unsigned long length,char *value);
typedef unsigned long  (*lib_oapc_set_bin_value)   (void* instanceData,unsigned long input,struct oapc_bin_head *value);
typedef unsigned long  (*lib_oapc_get_bin_value)   (void* instanceData,unsigned long output,struct oapc_bin_head **value);
typedef unsigned long  (*lib_oapc_release_bin_data)(void* instanceData,unsigned long output);
#endif

#ifdef ENV_EDITOR
typedef char          *(*lib_oapc_get_save_data)   (void* instanceData,unsigned long *length);
typedef void           (*lib_oapc_get_defsize)     (wxFloat32 *x,wxFloat32 *y);
typedef void           (*lib_oapc_get_minsize)     (void* instanceData,wxFloat32 *x,wxFloat32 *y);
typedef void           (*lib_oapc_get_maxsize)     (void* instanceData,wxFloat32 *x,wxFloat32 *y);
typedef void           (*lib_oapc_get_colours)     (wxUint32 *background,wxUint32 *foreground);
#endif


class ExternalIOLib : public wxObject
{
public:
   ExternalIOLib();
   ExternalIOLib(const wxString& name,wxUint32 type,wxWindow *parentSplash);
   virtual ~ExternalIOLib();

   wxByte       isValid;
   wxString     libname;
   void         *m_dynLib;
   wxString      name;
   wxUint32      m_internalType;
#ifdef ENV_BEAM
   unsigned long m_beamID;
#endif

   wxUint32           getLibCapabilities();
   wxUint32           getLibInputFlags();
   wxUint32           getLibOutputFlags();
   bool               compare(wxString libname);

   lib_oapc_get_name              oapc_get_name;
   lib_oapc_get_capabilities      oapc_get_capabilities;
   lib_oapc_get_no_ui_flags       oapc_get_no_ui_flags;
   lib_oapc_get_input_flags       oapc_get_input_flags;
   lib_oapc_get_output_flags      oapc_get_output_flags;
   lib_oapc_get_hmi_config_data   oapc_get_hmi_config_data;
   lib_oapc_get_config_data       oapc_get_config_data;
   lib_oapc_set_config_data_const oapc_set_config_data_const;
   lib_oapc_set_config_data       oapc_set_config_data; // deprecated
   lib_oapc_set_loaded_data       oapc_set_loaded_data;
   lib_oapc_create_instance       oapc_create_instance;
   lib_oapc_create_instance2      oapc_create_instance2;
   lib_oapc_delete_instance       oapc_delete_instance;
   lib_oapc_init                  oapc_init;
   lib_oapc_exit                  oapc_exit;
   lib_oapc_read_pvalue           oapc_read_pvalue;
   lib_oapc_paint                 oapc_paint; // public function is called from the wxWindows object that will hold the external graphical data
   lib_oapc_set_numminmax         oapc_set_numminmax;
#ifdef ENV_BEAM
   lib_oapc_get_config_info_data  oapc_get_config_info_data;
   lib_oapc_get_beam_id           oapc_get_beam_id;
   lib_oapc_wait_stop             oapc_wait_stop;
#endif
#ifdef ENV_PLAYER
   lib_oapc_set_digi_value        oapc_set_digi_value;
   lib_oapc_get_digi_value        oapc_get_digi_value;
   lib_oapc_set_num_value         oapc_set_num_value;
   lib_oapc_get_num_value         oapc_get_num_value;
   lib_oapc_set_char_value        oapc_set_char_value;
   lib_oapc_get_char_value        oapc_get_char_value;
   lib_oapc_set_bin_value         oapc_set_bin_value;
   lib_oapc_get_bin_value         oapc_get_bin_value;
   lib_oapc_release_bin_data      oapc_release_bin_data;
   lib_oapc_set_io_callback       oapc_set_io_callback;
#endif


#ifdef ENV_EDITOR
   lib_oapc_get_save_data        oapc_get_save_data;
   lib_oapc_get_defsize          oapc_get_defsize;
   lib_oapc_get_minsize          oapc_get_minsize;
   lib_oapc_get_maxsize          oapc_get_maxsize;
   lib_oapc_get_colours          oapc_get_colours;
   lib_oapc_get_numminmax        oapc_get_numminmax;
#endif


#ifdef ENV_EDITOR
   wxInt32                       uiID,foldUiID;
#endif

private:
   wxUint32 m_capabilities;
   wxString m_libnameStripped;

};

#endif

#endif //ENV_INT