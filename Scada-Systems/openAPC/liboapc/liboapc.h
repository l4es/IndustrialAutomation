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

#ifndef LIBOAPC_H
#define LIBOAPC_H

#pragma pack(8)

#include <wchar.h>

#ifdef ENV_LINUX
 #define OAPC_EXT_API_DEF
 #ifdef OAPC_EXT_EXPORTS
  #define OAPC_EXT_API __attribute__ ((visibility ("default")))
 #else
  #define OAPC_EXT_API
 #endif
#else

 #ifdef ENV_QNX
  #define OAPC_EXT_API
  #define OAPC_EXT_API_DEF
 #else

  #ifdef ENV_WINDOWS
   #ifdef OAPC_EXT_EXPORTS
    #define OAPC_EXT_API __declspec(dllexport)
    #define OAPC_EXT_API_DEF
   #else
    #define OAPC_EXT_API
    #ifndef OAPC_EXT_API_DEF
     #define OAPC_EXT_API_DEF __declspec(dllimport) // allowed only in header so a separate definition is used
    #endif
   #endif
  #else
   #ifdef ENV_EMBEDDED
    #define OAPC_EXT_API
    #define OAPC_EXT_API_DEF
   #else
    #error None of the environment switches ENV_LINUX, ENV_QNX, ENV_EMBEDDED or ENV_WINDOWS is defined!
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

 #define int64_t __int64
 #define uint64_t unsigned __int64
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

#ifdef ENV_QNX
 #include <stdint.h>
#endif

#ifndef ENV_WINDOWSCE
#include <time.h>
#endif
#include "oapc_libio.h"

#if !defined(ENV_EMBEDDED) || defined(ENV_FWTEST)

#define OAPC_CANVAS_IDENTIFIER 0xDEADBEEF
#define OAPC_CANVAS_READONLY   0x00000001
#define OAPC_CANVAS_ENABLED    0x00000002

struct oapc_canvas_userdata
{
   unsigned int identifier;
   unsigned int flags;
};



/**
 * This structure is used by the oapc_util_rb_xxx() functions and contains all ring buffer
 * related information
 */
struct oapc_util_rb_data
{
   int    head,tail,elements;
   void **buffer;
};



/**
 * This data structure is used to store a numerical information into. To avoid
 * non-portable floating point numbers here two values are stored that both together
 * form the data: the numValue has to be divided with numDivider in order to get
 * the (floating point) value (e.g.: myFloat=numValue/(float)numDivider;)
 */
struct oapc_num_value_block
{
   int numValue;     // the numerical value
   short numDivider; // the power of ten to divide by
   short reserved;
};


#define MAX_SEND_TIMEOUT 500

#define MAX_TTY_SIZE    100


/**
This is a standard data structure that keeps configuration information for
the serial interface. Here "port" is the (platform-dependent) name of the serial
port that has to be used. All the other parameters are index values that point
to the related real values, the index values are taken directly from the combo
boxes of the related configuration panel.
*/
struct serial_params
{
   char           port[MAX_TTY_SIZE];
   unsigned short brate,databits,parity,stopbits,flowcontrol;
};

#define MAX_TEXT_LEN 240        /** the maximum length of characters */

// definitions and constants for the common interface functionalities *************

#define OAPC_IFACE_TYPE_STATE_NEW_CONNECTION    1 /** a new client has connected to the interface */
#define OAPC_IFACE_TYPE_STATE_CONNECTION_CLOSED 2 /** a client has closed its connection to the interface */
#define OAPC_IFACE_TYPE_STATE_CONNECTION_ERROR  3 /** an error occurred for a client */
//#define OAPC_IFACE_TYPE_STATE_DATA_RECEIVED   4 /** this state does no apply for the C/C++ implementation, here the following type flags are used directly instead */
#define OAPC_IFACE_TYPE_DIGI                    5
#define OAPC_IFACE_TYPE_NUM                     6
#define OAPC_IFACE_TYPE_CHAR                    7
#define OAPC_IFACE_TYPE_BIN                     8


// end of definitions and constants for the common interface functionalities ******


// definitions and constants for the interlock space functionalities **************

#define MAX_NODENAME_LENGTH 127 /** This constant defines the maximal length a nodes name may have; a nodes
                                    name consists of "/element_name/dir" where "element_name" is the name of
                                    the element within the main application and "dir" is either "in" or "out"
                                    depending on the fact if this node stores the input data or the output
                                    data of that element */

#define OAPC_CMD_SET_VALUE        0x0001 // set the value(s) followed to this structure; this is used in both directions to set new values to the server and to set new values to a connected client
#define OAPC_CMD_GET_VALUE        0x0002 // request a value, no data are following here; this is used by a client only, the server doesn't requests data
#define OAPC_CMD_GET_ALL_VALUES   0x0003 // request all available values, no data are following here; this is used by a client only, the server doesn't requests data

#define OAPC_CMDERR_DOESNT_EXISTS 0x0100 // a requested value does not exist; this error may be returned as response to OAPC_CMD_GET_VALUE



struct oapc_ispace_head
{
   unsigned int   version,length;
   unsigned int   ios;
   unsigned short cmd,reserved;
   char           nodeName[MAX_NODENAME_LENGTH+4];
};



/**
 * This data structure is used to store a digital information
 */
struct oapc_digi_value_block
{
   unsigned char digiValue;   // the digital value
};



/**
 * This data structure is used to store a character information
 */
struct oapc_char_value_block
{
   char charValue[MAX_TEXT_LEN+4];
};


/**
 * This structure is for future use and can be ignored at the moment
 */
struct oapc_ispace_auth
{
   unsigned int id;
};

#define OAPC_ISERVER_AUTH_ID_MAIN 0x80000000

#if !defined(ENV_EMBEDDED) || defined (ENV_FWTEST)
/**
 * This is the type definition for the callback function that has to be used for receiving data
 * from the Interlock Server. This callback function is called every time something is received from
 * the server. Such receptions can occur for different reasons:
 * <UL>
 * <LI>a client changed something within the interlock data space, here the new values are
 *     transmitted to all other clients that may be interested in these changed data
 * <LI>the own client requested a specific data block by calling oapc_ispace_request_data(), here
 *     the callback function is called either with the data that belong to this data block or with no
 *     data and an error code when this specific block doesn't exists
 * <LI>the own client requested a complete update by calling oapc_ispace_request_all_data(), in this
 *     case the callback function is called for every data block that exists within the Interlock
 *     Server
 * </UL>
 * @param[in] handle identifies the connection where the new data arrive at; this parameter would give
 *            the possibility to check where data are received at when more interlock server connections
 *            are used from within the same application, nevertheless it is highly recommended to use an
 *            own callback for every used connection 
 * @param[in] nodeName the name of the data block the following data belong to
 * @param[in] cmd specifies using which command the data have been sent or which reason for an error
 *            occured: OAPC_CMD_SET_VALUE is specified here in case a new or requested data block
 *            was received, OAPC_CMDERR_DOESNT_EXISTS is given when a requested data block could
 *            not be found; in last case "ios" and all values are set to 0
 * @param[in] ios the IO flags that sepcify which fields of the follwing values-array contain which
 *            kind of data
 * @param[in] values the data itself
 */
#ifdef ENV_FWTEST
 #ifndef MAX_NUM_IOS
  #define MAX_NUM_IOS 8
 #endif
#endif


#ifndef ENV_EMBEDDED
 typedef void (*lib_oapc_ispace_callback)(void *handle,char *nodeName,unsigned int cmd,unsigned int ios,void *values[MAX_NUM_IOS]); /** type definition for the callback function that is provided by the application in order to get informed about changes in interlock space */
 typedef void (*lib_oapc_iface_callback)(int type,char *cmd,unsigned char digi,float num,char *str,struct oapc_bin_head *bin,int socket); /** type definition for the callback function that is provided by the application in order to get informed about received data */
#endif // ENV_EMBEDDED

// end of definitions and constants for the interlock space functionalities ******


#ifdef __cplusplus
 class wxPanel;  // forward declarations
 class wxString;
#else
 typedef void wxPanel;  // fake definition
 typedef int  wxString; // fake definition
 #ifndef ENV_FWTEST
  typedef char bool;
 #endif
 #ifdef ENV_MLO
  typedef char bool;
 #endif
#endif

#ifdef __cplusplus
extern "C" 
{
#endif
   OAPC_EXT_API_DEF OAPC_EXT_API int  oapc_tcp_connect_to(const char *address,unsigned short connect_port);
   OAPC_EXT_API_DEF OAPC_EXT_API void oapc_tcp_set_blocking(int sock,char block);
   OAPC_EXT_API_DEF OAPC_EXT_API int  oapc_tcp_send(int sock, const char *msg,int len,int msecs);
   OAPC_EXT_API_DEF OAPC_EXT_API int  oapc_tcp_recv(int sock,char *data, int len,const char *termStr,long msecs); 
   OAPC_EXT_API_DEF OAPC_EXT_API void oapc_tcp_closesocket(int sock);
   OAPC_EXT_API_DEF OAPC_EXT_API int  oapc_tcp_listen_on_port(unsigned short port, const char *bindToIP);
   OAPC_EXT_API_DEF OAPC_EXT_API int  oapc_tcp_accept_connection(int sock, unsigned long * remote_ip);

   OAPC_EXT_API_DEF OAPC_EXT_API void                  oapc_util_dbl_to_block(double inValue,struct oapc_num_value_block *outBlock);
   OAPC_EXT_API_DEF OAPC_EXT_API double                oapc_util_block_to_dbl(struct oapc_num_value_block *inBlock);
   OAPC_EXT_API_DEF OAPC_EXT_API bool                  oapc_util_to_unicode(char* str,wchar_t* out,int outSize);
   OAPC_EXT_API_DEF OAPC_EXT_API int64_t               oapc_util_htonll(int64_t host_longlong);
   OAPC_EXT_API_DEF OAPC_EXT_API int64_t               oapc_util_ntohll(int64_t net_longlong);
   OAPC_EXT_API_DEF OAPC_EXT_API unsigned int          oapc_util_colour2gray(unsigned int colour);
   OAPC_EXT_API_DEF OAPC_EXT_API double                oapc_util_atof(const char *c);
   OAPC_EXT_API_DEF OAPC_EXT_API double                oapc_util_atof_dot(const char *c);
   OAPC_EXT_API_DEF OAPC_EXT_API void*                 oapc_util_get_time();
   OAPC_EXT_API_DEF OAPC_EXT_API double                oapc_util_diff_time(void *time1,void *time2);
   OAPC_EXT_API_DEF OAPC_EXT_API void                  oapc_util_release_time(void *time1);
   OAPC_EXT_API_DEF OAPC_EXT_API double                oapc_util_get_timeofday(void *time1);
   OAPC_EXT_API_DEF OAPC_EXT_API struct oapc_bin_head *oapc_util_alloc_bin_data(unsigned char type,unsigned char subType,unsigned char compression,int sizeData);
   OAPC_EXT_API_DEF OAPC_EXT_API struct oapc_bin_head *oapc_util_duplicate_bin_data(const struct oapc_bin_head *bin_in);
   OAPC_EXT_API_DEF OAPC_EXT_API void                  oapc_util_release_bin_data(struct oapc_bin_head *bin);
   OAPC_EXT_API_DEF OAPC_EXT_API int                   oapc_util_rb_alloc(struct oapc_util_rb_data *buffer,int elements);
   OAPC_EXT_API_DEF OAPC_EXT_API int                   oapc_util_rb_release(struct oapc_util_rb_data *buffer);
   OAPC_EXT_API_DEF OAPC_EXT_API int                   oapc_util_rb_push(struct oapc_util_rb_data *buffer,void *data);
   OAPC_EXT_API_DEF OAPC_EXT_API void                 *oapc_util_rb_front(struct oapc_util_rb_data *buffer);
   OAPC_EXT_API_DEF OAPC_EXT_API int                   oapc_util_rb_pop(struct oapc_util_rb_data *buffer);
   OAPC_EXT_API_DEF OAPC_EXT_API bool                  oapc_util_rb_empty(struct oapc_util_rb_data *buffer);
   OAPC_EXT_API_DEF OAPC_EXT_API bool                  oapc_util_rb_full(struct oapc_util_rb_data *buffer);
   OAPC_EXT_API_DEF OAPC_EXT_API int                   oapc_util_check_maskbit(struct oapc_bin_head *bin,int x,int y);

   OAPC_EXT_API_DEF OAPC_EXT_API void *oapc_thread_create(void *(*start_routine)(void*), void *arg);
   OAPC_EXT_API_DEF OAPC_EXT_API bool  oapc_thread_set_prio(const void *handle,const char prio);
   OAPC_EXT_API_DEF OAPC_EXT_API void  oapc_thread_release(void *handle);
   OAPC_EXT_API_DEF OAPC_EXT_API int   oapc_thread_sleep(int msecs);
   OAPC_EXT_API_DEF OAPC_EXT_API bool  oapc_util_create_thread(void *(*start_routine)(void*), void *arg); // do not use any longer, will be removed in future versions!
   OAPC_EXT_API_DEF OAPC_EXT_API int   oapc_util_thread_sleep(int msecs); // do not use any longer, will be removed in future versions!
   OAPC_EXT_API_DEF OAPC_EXT_API bool  oapc_util_thread_set_prio(const unsigned char prio); // do not use any longer, will be removed in future versions!
   OAPC_EXT_API_DEF OAPC_EXT_API void *oapc_thread_mutex_create(void);
   OAPC_EXT_API_DEF OAPC_EXT_API void  oapc_thread_mutex_lock(void *handle);
   OAPC_EXT_API_DEF OAPC_EXT_API void  oapc_thread_mutex_unlock(void *handle);
   OAPC_EXT_API_DEF OAPC_EXT_API void  oapc_thread_mutex_release(void *handle);
   OAPC_EXT_API_DEF OAPC_EXT_API bool  oapc_thread_timer_start(void (*start_routine)(void*,int),int time,void *data,int timerID);
   OAPC_EXT_API_DEF OAPC_EXT_API void *oapc_thread_signal_create(void);
   OAPC_EXT_API_DEF OAPC_EXT_API int   oapc_thread_signal_send(void *handle);
   OAPC_EXT_API_DEF OAPC_EXT_API int   oapc_thread_signal_wait(void *handle,int msecs);
   OAPC_EXT_API_DEF OAPC_EXT_API void  oapc_thread_signal_release(void *handle);

   OAPC_EXT_API_DEF OAPC_EXT_API void *oapc_dlib_load(const char *name);
   OAPC_EXT_API_DEF OAPC_EXT_API void *oapc_dlib_get_symbol(void *handle,const char *name);
   OAPC_EXT_API_DEF OAPC_EXT_API void  oapc_dlib_release(void *handle);

#ifdef ENV_WINDOWS
   OAPC_EXT_API_DEF OAPC_EXT_API int  oapc_serial_port_open(struct serial_params *serialParams,HANDLE *fd);
   OAPC_EXT_API_DEF OAPC_EXT_API int  oapc_serial_usb_port_open(const char *portname,HANDLE *fd);
   OAPC_EXT_API_DEF OAPC_EXT_API int  oapc_serial_recv_data(HANDLE fd,char *data,int toLoad,long msecs);
   OAPC_EXT_API_DEF OAPC_EXT_API int  oapc_serial_recv(HANDLE fd,char *data,int len,const char *termStr,long msecs);
   OAPC_EXT_API_DEF OAPC_EXT_API int  oapc_serial_send(HANDLE fd,const char *msg, int len,int msecs);
   OAPC_EXT_API_DEF OAPC_EXT_API int  oapc_serial_send_data(HANDLE fd,const char *msg, int len,int msecs); // do not use any longer, will be removed in future versions!
   OAPC_EXT_API_DEF OAPC_EXT_API void oapc_serial_port_close(HANDLE *fd);
#else
   OAPC_EXT_API_DEF OAPC_EXT_API int  oapc_serial_port_open(struct serial_params *serialParams,int *fd);
   OAPC_EXT_API_DEF OAPC_EXT_API int  oapc_serial_usb_port_open(const char *portname,int *fd);
   OAPC_EXT_API_DEF OAPC_EXT_API int  oapc_serial_recv_data(int fd,char *data,int len,long timeout);
   OAPC_EXT_API_DEF OAPC_EXT_API int  oapc_serial_recv(int fd,char *data,int len,const char *termStr,long msecs);
   OAPC_EXT_API_DEF OAPC_EXT_API int  oapc_serial_send(int fd,const char *msg, int len,int msecs);
   OAPC_EXT_API_DEF OAPC_EXT_API int  oapc_serial_send_data(int fd,const char *msg, int len,int msecs); // do not use any longer, will be removed in future versions!
   OAPC_EXT_API_DEF OAPC_EXT_API void oapc_serial_port_close(int *fd);
#endif

#ifdef ENV_WINDOWS
   OAPC_EXT_API_DEF OAPC_EXT_API int  oapc_parallel_port_open(const char *portname,HANDLE *fd); // do not use any longer, will be removed in future versions!
   OAPC_EXT_API_DEF OAPC_EXT_API int  oapc_parallel_send(HANDLE fd,const char *msg,int len,int msecs); // do not use any longer, will be removed in future versions!
   OAPC_EXT_API_DEF OAPC_EXT_API void oapc_parallel_port_close(HANDLE *fd); // do not use any longer, will be removed in future versions!
#else
   OAPC_EXT_API_DEF OAPC_EXT_API int  oapc_parallel_port_open(const char *portname,int *fd); // do not use any longer, will be removed in future versions!
   OAPC_EXT_API_DEF OAPC_EXT_API int  oapc_parallel_send(int fd,const char *msg,int len,int msecs); // do not use any longer, will be removed in future versions!
   OAPC_EXT_API_DEF OAPC_EXT_API void oapc_parallel_port_close(int *fd); // do not use any longer, will be removed in future versions!
#endif

#ifndef ENV_EMBEDDED
   OAPC_EXT_API_DEF OAPC_EXT_API int oapc_iface_set_recv_callback(lib_oapc_iface_callback oapc_iface_callback);
   OAPC_EXT_API_DEF OAPC_EXT_API int oapc_iface_init(const char *host,unsigned short port);
   OAPC_EXT_API_DEF OAPC_EXT_API int oapc_iface_set_authentication(char *uname,char *pwd);
   OAPC_EXT_API_DEF OAPC_EXT_API int oapc_iface_send_digi(const char *cmd,unsigned char digi,int socket);
   OAPC_EXT_API_DEF OAPC_EXT_API int oapc_iface_send_num(const char *cmd,float num,int socket);
   OAPC_EXT_API_DEF OAPC_EXT_API int oapc_iface_send_char(const char *cmd,char *str,int socket);
   OAPC_EXT_API_DEF OAPC_EXT_API int oapc_iface_send_bin(const char *cmd,struct oapc_bin_head *bin,int socket);
   OAPC_EXT_API_DEF OAPC_EXT_API int oapc_iface_exit(void);   

   OAPC_EXT_API_DEF OAPC_EXT_API bool oapc_canvas_get_readonly(wxPanel *canvas);
   OAPC_EXT_API_DEF OAPC_EXT_API void oapc_canvas_set_readonly(wxPanel *canvas,bool readonly);
   OAPC_EXT_API_DEF OAPC_EXT_API bool oapc_canvas_get_enabled(wxPanel *canvas);
   OAPC_EXT_API_DEF OAPC_EXT_API void oapc_canvas_set_enabled(wxPanel *canvas,bool enable);
/* internal library functions, do not use them within a HMI plug-in! ****/
   OAPC_EXT_API_DEF OAPC_EXT_API void oapc_canvas_release_data(wxPanel *canvas);
/* end of internal library functions, do not use! ***********************/

   OAPC_EXT_API_DEF OAPC_EXT_API void oapc_unicode_charToStringUTF16BE(const char *c,int len,wxString *result);
   OAPC_EXT_API_DEF OAPC_EXT_API void oapc_unicode_charToStringUTF8(const char *c,int len,wxString *result);
   OAPC_EXT_API_DEF OAPC_EXT_API void oapc_unicode_charToStringASCII(const char *c,int len,wxString *result);
   OAPC_EXT_API_DEF OAPC_EXT_API void oapc_unicode_stringToCharUTF16BE(wxString s,char *c,int len);
   OAPC_EXT_API_DEF OAPC_EXT_API void oapc_unicode_stringToCharUTF8(wxString s,char *c,int len);
   OAPC_EXT_API_DEF OAPC_EXT_API void oapc_unicode_stringToCharASCII(wxString s,char *c,int len);
   OAPC_EXT_API_DEF OAPC_EXT_API void oapc_unicode_utf16BEToASCII(char *utf,int len);

   OAPC_EXT_API_DEF OAPC_EXT_API void oapc_path_split(wxString *path,wxString *dir,wxString *file,wxString extension);

   OAPC_EXT_API_DEF OAPC_EXT_API void *oapc_ispace_get_instance();
   OAPC_EXT_API_DEF OAPC_EXT_API int   oapc_ispace_set_recv_callback(void *handle,lib_oapc_ispace_callback oapc_ispace_callback);
   OAPC_EXT_API_DEF OAPC_EXT_API int   oapc_ispace_connect(void *handle,const char *host,unsigned short port,struct oapc_ispace_auth *auth);
   OAPC_EXT_API_DEF OAPC_EXT_API int   oapc_ispace_set_data(void *handle,const char *nodeName,unsigned int ios,void *values[MAX_NUM_IOS],struct oapc_ispace_auth *auth);
   OAPC_EXT_API_DEF OAPC_EXT_API int   oapc_ispace_set_value(void *handle,const char *nodeName,unsigned int io,void *value,struct oapc_ispace_auth *auth);
   OAPC_EXT_API_DEF OAPC_EXT_API int   oapc_ispace_request(void *handle,const char *nodeName,unsigned short cmd,struct oapc_ispace_auth *auth);
   OAPC_EXT_API_DEF OAPC_EXT_API int   oapc_ispace_request_data(void *handle,const char *nodeName,struct oapc_ispace_auth *auth);
   OAPC_EXT_API_DEF OAPC_EXT_API int   oapc_ispace_request_all_data(void *handle,struct oapc_ispace_auth *auth);
   OAPC_EXT_API_DEF OAPC_EXT_API int   oapc_ispace_disconnect(void *handle);   
#endif // ENV_EMBEDDED

/* internal library functions, do not use them within a client application! ****/
   OAPC_EXT_API_DEF OAPC_EXT_API int oapc_ispace_set_socket(void *handle,int newSock);
   OAPC_EXT_API_DEF OAPC_EXT_API int oapc_ispace_send_response(void *handle,const char *nodeName,unsigned int rcode);
/* end of internal library functions, do not use! ******************************/

#ifdef __cplusplus
}
#endif

#endif //ENV_EMBEDDED

#ifdef _DEBUG
extern void show_backtrace(void);
#endif

#endif // ENV_EMBEDDED

#endif
