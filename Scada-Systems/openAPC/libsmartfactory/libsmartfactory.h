/********************************************************************************************

This program and source file is free software: you can redistribute it and/or modify it under
the terms of the GNU General Public License as published by the Free Software Foundation,
either version 3 of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with this program.
If not, see <http://www.gnu.org/licenses/>.

*********************************************************************************************/

#ifndef LIBSF_H
#define LIBSF_H

#pragma pack(8)


#ifdef ENV_USE_PUGIXML
 #include <string>

 #define wxString      std::string
 #define wxEmptyString ""
 #define wxXmlDocument pugi::xml_document
 #define wxXmlNode     pugi::xml_node
 #define _T
#endif

#ifdef ENV_LINUX
 #define SF_EXT_API_DEF
 #ifdef SF_EXT_EXPORTS
  #define SF_EXT_API __attribute__ ((visibility ("default")))
 #else
  #define SF_EXT_API
 #endif
#else

 #ifdef ENV_QNX
  #define SF_EXT_API
  #define SF_EXT_API_DEF
 #else

  #ifdef ENV_WINDOWS
   #ifdef SF_EXT_EXPORTS
    #define SF_EXT_API __declspec(dllexport)
    #define SF_EXT_API_DEF
   #else
    #define SF_EXT_API
    #ifndef SF_EXT_API_DEF
     #define SF_EXT_API_DEF __declspec(dllimport) // allowed only in header so a separate definition is used
    #endif
   #endif
  #else
   #ifdef ENV_EMBEDDED
    #define SF_EXT_API
    #define SF_EXT_API_DEF
   #else
    #error None of the environment switches ENV_LINUX, ENV_EMBEDDED or ENV_WINDOWS is defined!
   #endif
  #endif
 #endif
#endif

#ifndef __cplusplus
 #ifndef ENV_EMBEDDED
  #define false 0
  #define true  1
 #endif
#endif

#ifdef ENV_WINDOWS
 #include <windows.h>
#else

#endif

#ifndef _DEBUG
 #undef wxDEBUG_LEVEL
 #ifndef wxDEBUG_LEVEL
  #define wxDEBUG_LEVEL 0
 #endif
#endif

#ifdef ENV_LINUX
 #include <stdint.h>
#endif

/** Hermes-related definitions **********************************************************************************/

enum hermes_prev_state
{
   ePREV_OFFLINE,
   ePREV_WAITING_SERVICE_DESCRIPTION,
   ePREV_IDLE,
   ePREV_REVOKE_BOARD_AVAILABLE,
   ePREV_BOARD_AVAILABLE,
   ePREV_TRANSPORT_FINISHED
};


enum hermes_prev_command
{
   ePREV_CMD_NONE,
   ePREV_CMD_START_TRANSPORT,
   ePREV_CMD_STOP_TRANSPORT,
   ePREV_CMD_REVOKE_MACHINE_READY,
   ePREV_CMD_SET_MACHINE_READY
};

typedef void(*hermes_prev_state_callback)(const enum hermes_prev_state newState,const bool error,const void* custData);

enum hermes_next_state
{
   eNEXT_OFFLINE,
   eNEXT_IDLE,
   eNEXT_MACHINE_READY,
   eNEXT_REVOKE_MACHINE_READY,
   eNEXT_START_TRANSPORT,
   eNEXT_STOP_TRANSPORT
};


enum hermes_next_command
{
   eNEXT_CMD_NONE,
   eNEXT_CMD_BOARD_AVAILABLE,
   eNEXT_CMD_REVOKE_BOARD_AVAILABLE,
   eNEXT_CMD_TRANSPORT_FINISHED
};

typedef void(*hermes_next_state_callback)(const enum hermes_next_state newState,const bool error,const void* custData);

typedef void(*hermes_wx_notification_callback)(const long notificationCode, const long severity, const wxString *description, const wxString *src,const void* custData);

typedef void(*hermes_log_callback)(const char *logTxt,const void* custData);

enum hermes_level
{
   eHERMES_1_0, // the basic hermes standard which has to be supported at least
};

/** Smart Interface related definitions *************************************************************************/

enum SmartState
{
   eIDLE,
   eREADY,
   eMARKING,
   eERROR,
   ePAUSED,
   eUNKNOWN
};


#ifdef __cplusplus
extern "C" 
{
#endif

#ifndef ENV_USE_PUGIXML
   SF_EXT_API_DEF SF_EXT_API void *sf_hermes_wx_create_instance(const wxString *machineIdentifier,const enum hermes_level level);
#endif
   SF_EXT_API_DEF SF_EXT_API void *sf_hermes_create_instance(const char *machineIdentifier,const enum hermes_level level);
   SF_EXT_API_DEF SF_EXT_API int   sf_hermes_open_connections(void* instance,const unsigned short prevMachinePort,const char* prevMachineIP,const unsigned short nextMachinePort,const char remoteConfigEnabled);
   SF_EXT_API_DEF SF_EXT_API int   sf_hermes_delete_instance(void* instance);
#ifndef ENV_USE_PUGIXML
   SF_EXT_API_DEF SF_EXT_API int   sf_hermes_wx_set_product_identifier(void* instance,const wxString *productIdentifier);
   SF_EXT_API_DEF SF_EXT_API int   sf_hermes_wx_set_machine_identifier(void* instance,const wxString *machineIdentifier);
#endif

   SF_EXT_API_DEF SF_EXT_API int   sf_hermes_set_next_BoardAvailable(void* instance);
   SF_EXT_API_DEF SF_EXT_API int   sf_hermes_set_next_RevokeBoardAvailable(void* instance);
   SF_EXT_API_DEF SF_EXT_API int   sf_hermes_set_next_TransportFinished(void* instance);
   SF_EXT_API_DEF SF_EXT_API int   sf_hermes_set_prev_StartTransport(void* instance);
   SF_EXT_API_DEF SF_EXT_API int   sf_hermes_set_prev_StopTransport(void* instance);
   SF_EXT_API_DEF SF_EXT_API int   sf_hermes_set_prev_MachineReady(void* instance);
   SF_EXT_API_DEF SF_EXT_API int   sf_hermes_set_prev_RevokeMachineReady(void* instance);

   SF_EXT_API_DEF SF_EXT_API int   sf_hermes_set_prev_state_callback(void* instance,const hermes_prev_state_callback fct,void *data);
   SF_EXT_API_DEF SF_EXT_API int   sf_hermes_set_next_state_callback(void* instance,const hermes_next_state_callback fct,void *data);
#ifndef ENV_USE_PUGIXML
   SF_EXT_API_DEF SF_EXT_API int   sf_hermes_wx_set_notification_callback(void* instance,const hermes_wx_notification_callback fct,void *data);
#endif
   SF_EXT_API_DEF SF_EXT_API int   sf_hermes_set_log_callback(void* instance,const hermes_log_callback fct, void* data);

   SF_EXT_API_DEF SF_EXT_API enum hermes_prev_state sf_hermes_get_prev_state(void* instance);
   SF_EXT_API_DEF SF_EXT_API enum hermes_next_state sf_hermes_get_next_state(void* instance);
#ifndef ENV_USE_PUGIXML
   SF_EXT_API_DEF SF_EXT_API int                    sf_hermes_wx_get_curr_board_info(void* instance,wxString *BoardId,wxString *ProductTypeId);

#endif
   SF_EXT_API_DEF SF_EXT_API void *sf_if_wx_create_instance(const wxString *machineIdentifier);
   SF_EXT_API_DEF SF_EXT_API void *sf_if_create_instance(const char *machineIdentifier);
   SF_EXT_API_DEF SF_EXT_API int   sf_if_open_connections(void* instance);
   SF_EXT_API_DEF SF_EXT_API int   sf_if_delete_instance(void* instance);
#ifndef ENV_USE_PUGIXML
   SF_EXT_API_DEF SF_EXT_API int   sf_if_wx_set_machine_identifier(void* instance,const wxString *machineIdentifier);
   SF_EXT_API_DEF SF_EXT_API int   sf_if_wx_set_product_identifier(void* instance, const wxString *productIdentifier);
   SF_EXT_API_DEF SF_EXT_API int   sf_if_wx_set_error(void* instance, const wxString *errorText);
   SF_EXT_API_DEF SF_EXT_API int   sf_if_wx_set_hermes_notification(void* instance,const long notificationCode,const long severity,const wxString *description,const wxString *src);
   SF_EXT_API_DEF SF_EXT_API int   sf_if_wx_set_process_info_data(void* instance,const unsigned int idx,const double value,const wxString *name,const wxString *unit);
   SF_EXT_API_DEF SF_EXT_API int   sf_if_wx_append_trace_data(void* instance,const wxString *name,const wxString *data);
   SF_EXT_API_DEF SF_EXT_API int   sf_if_wx_set_hermes_trace_info(void* instance,const wxString *boardID,const wxString *productID);
#endif
   SF_EXT_API_DEF SF_EXT_API int   sf_if_set_state(void* instance,const enum SmartState state);
   SF_EXT_API_DEF SF_EXT_API int   sf_if_set_parts(void* instance,const unsigned int parts);
   SF_EXT_API_DEF SF_EXT_API int   sf_if_set_slices(void* instance,const unsigned int maxSlices,const unsigned int currSlice);


   SF_EXT_API_DEF SF_EXT_API void *sf_mqtt_wx_create_instance(const wxString *machineIdentifier);
   SF_EXT_API_DEF SF_EXT_API void *sf_mqtt_create_instance(const char *machineIdentifier);
   SF_EXT_API_DEF SF_EXT_API int   sf_mqtt_open_connection(void* instance,const unsigned short port,const char* IP);
   SF_EXT_API_DEF SF_EXT_API int   sf_mqtt_delete_instance(void* instance);
   SF_EXT_API_DEF SF_EXT_API int   sf_mqtt_wx_publish(void* instance,const wxString *topic,const wxString *pay);
   SF_EXT_API_DEF SF_EXT_API int   sf_mqtt_publish(void* instance,const char *topic,const char *pay);


#ifdef __cplusplus
}
#endif

#endif
