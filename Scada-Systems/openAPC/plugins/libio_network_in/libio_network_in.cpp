/********************************************************************************************

These sources are distributed in the hope that they will be useful, but WITHOUT ANY WARRANTY;
without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. All
the information given here, within the interface descriptions and within the specification
are subject to change without notice. Errors and omissions excepted.

These sources demonstrate the usage of the OpenAPC Plug-In programming interface. They can be
used freely according to the OpenAPC Dual License: As long as the sources and the resulting
applications/libraries/Plug-Ins are used together with the OpenAPC software, they are
licensed as freeware. When you use them outside the OpenAPC software they are licensed under
the terms of the GNU General Public License.

For more information please refer to http://www.openapc.com/oapc_license.php

*********************************************************************************************/



#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#ifndef ENV_WINDOWSCE
#include <sys/types.h>
#endif

#ifndef ENV_WINDOWS
 #include <arpa/inet.h>
 #include <errno.h>
 #include <unistd.h>
 #include <fcntl.h>
#else
 #include <winsock2.h>
 #define snprintf _snprintf
#endif

#define MAX_XML_SIZE   4000

#include "oapc_libio.h"
#include "network_common.h"
#include "liboapc.h"


/** the xml data that define the behaviour and user interface of the plugin within the main application */
static char xmltempl[]="<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"?>\n\
<oapc-config>\n\
 <flowimage>%s</flowimage>\n\
 <dialogue>\n\
  <general>\n\
   <param>\n\
    <name>ip</name>\n\
    <text>IP</text>\n\
    <type>string</type>\n\
    <default>%s</default>\n\
    <min>3</min>\n\
    <max>%d</max>\n\
   </param>\n\
   <param>\n\
    <name>port</name>\n\
    <text>Port</text>\n\
    <type>integer</type>\n\
    <default>%d</default>\n\
    <min>1</min>\n\
    <max>65535</max>\n\
   </param>\n\
   <param>\n\
    <name>uname</name>\n\
    <text>Username</text>\n\
    <type>string</type>\n\
    <default>%s</default>\n\
    <min>0</min>\n\
    <max>%d</max>\n\
   </param>\n\
   <param>\n\
    <name>pwd</name>\n\
    <text>Password</text>\n\
    <type>string</type>\n\
    <default>%s</default>\n\
    <min>0</min>\n\
    <max>%d</max>\n\
   </param>\n\
   <param>\n\
    <name>mode</name>\n\
    <text>Mode</text>\n\
    <type>option</type>\n\
    <value>Command/Value - transmit only pairs</value>\n\
    <value>Plain - transmit everything separately</value>\n\
    <default>%d</default>\n\
   </param>\n\
  </general>\n\
  <helppanel>\n\
   <in0>CMD - command value</in0>\n\
   <in1>DATA - payload data</in1>\n\
   <in2>DATA - payload data</in2>\n\
   <in3>DATA - payload data</in3>\n\
   <in4>DATA - payload data</in4>\n\
  </helppanel>\n\
 </dialogue>\n\
</oapc-config>\n";
/** Base64-encoded PNG data of an image that is used as icon for the program flow diagram within the main application*/
static char                flowImage[]="iVBORw0KGgoAAAANSUhEUgAAAGoAAAAyBAMAAABG0lM+AAAAMFBMVEUEAgT8/gSEgoQE/vz8AvzEwsT8/vwAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAACvX5NoAAAA8ElEQVR42u2UTQrCMBCFY2yztp6gorgWe4FScoBs7DoImfsfwWaiUOlEk8EqSGfVF/q9+ckQIVihQOpOAdgj1AAgm7MXB42i2g5iGnagjG57AKegHA5Mc/Ki0CiqTR+j7MH7grx6X7sPokOxqyFGucL7grHe162DaFGsyigFEou3wZcQNGWweBd8CUFTNnwHX0qQVHYglU3i5GOFzEEJwemLNQ1mLpXwmw/yll9RlyFMdq4pldAXlQtYud43RkwjbTcUa3sVazeyKDt5CuejnqpTWX19jXrc8OimU6j7No226j8o1jQ++84v1EL9nGLFDZa4qMClnYm7AAAAAElFTkSuQmCC";
static char                libname[]="Network Transmitter";
static char                xmldescr[MAX_XML_SIZE+1],sendbuf[MAX_SENDBUF_SIZE];
static struct libio_config save_config;



/**
This function has to return the name that is used to display this library within the main application.
The name returned here has to be short and descriptive
@param[out] data pointer to the beginning of an char-array that contains the name
@return the length of the name structure or 0 in case of an error
*/
OAPC_EXT_API char *oapc_get_name(void)
{
   return libname;
}



/**
This function returns a set of OAPC_HAS_... flags that describe the general capabilities of this
library. These flags can be taken from oapc_libio.h
@return or-concatenated flags that describe the libraries capabilities
*/
OAPC_EXT_API unsigned long oapc_get_capabilities(void)
{
   return OAPC_HAS_INPUTS|
          OAPC_HAS_XML_CONFIGURATION|
          OAPC_ACCEPTS_PLAIN_CONFIGURATION|
          OAPC_FLOWCAT_IO;
}



/**
When the OAPC_HAS_INPUTS flag is set, the application calls this function to get the configuration
for the inputs. Here "input" is defined from the libraries point of view, means data that are sent from
the application to the library are input data
@return or-concatenated OAPC_..._IO...-flags that describe which inputs and data types are used or 0 in
        case of an error
*/
OAPC_EXT_API unsigned long oapc_get_input_flags(void)
{
   return OAPC_CHAR_IO0|
          OAPC_DIGI_IO1|
          OAPC_NUM_IO2|
          OAPC_CHAR_IO3|
          OAPC_BIN_IO4;
   // avoid it to submit the same IO number for different data types, that would cause displaying problems for the flow symbol
}



/**
When the OAPC_HAS_XML_CONFIGURATION capability flag was set this function is called to retrieve an XML
structure that describes which configuration information have to be displayed within the main application.
@param[out] data pointer to the beginning of an char-array that contains the XML data
@return the length of the XML structure or 0 in case of an error
*/
OAPC_EXT_API char *oapc_get_config_data(void* instanceData)
{
   struct instData *data;

   data=(struct instData*)instanceData;

   snprintf(xmldescr,MAX_XML_SIZE,xmltempl,flowImage,data->config.ip,MAX_IP_SIZE,data->config.port,
                             data->config.uname,MAX_AUTH_SIZE,data->config.pwd,MAX_AUTH_SIZE,data->config.mode);
   return xmldescr;
}



/**
When the OAPC_ACCEPTS_PLAIN_CONFIGURATION capability flag was set this function is called for every configurable parameter
to return the value that was set within the application.
@param[in] name the name of the parameter according to the unique identifier that was set within the <name></name> field of the
           XML config file
@param[in] value the value that was configured for this parameter; in case it is not a string but a number it has to be converted,
           the representation sent by the application is always a string
*/
OAPC_EXT_API void oapc_set_config_data(void* instanceData,const char *name,const char *value)
{
   struct instData *data;

   data=(struct instData*)instanceData;
   set_config_data(data,name,value);
}



/**
This function delivers the data that are stored within the project file by the main application. It is
recommended to put two fields "version" and "length" into the data structure that is handed over here.
So when the data to be saved need to be changed it is easy to recognize which version of the data structure
is used, possible conversions can be done easier in oapc_set_loaded_data().<BR>
PLEASE NOTE: In order to keep the resulting project files compatible with all possible platforms the
             application is running at you have to store all values in network byteorder
@param[out] length the total effective length of the data that are returned by this function
@return the data that have to be saved
*/
OAPC_EXT_API char *oapc_get_save_data(void* instanceData,unsigned long *length)
{
   struct instData *data;

   *length=sizeof(struct libio_config);
   data=(struct instData*)instanceData;
   convert_to_save_byteorder(&save_config,&data->config);

   return (char*)&save_config;
}



/**
This function receives the data that have been stored within the project file by the main application. It is
recommended to check if the returned data are really what the library expects. To do that two fields
"version" and "length" within the saved data structure should be checked.<BR>
PLEASE NOTE: In order to keep the resulting project files compatible with all possible platforms the
             application is running at you have to convert all the values that have been stored in network
             byteorder back to the local byteorder
@param[in] length the total effective length of the data that are handed over to this function
@param[in] data the configuration data that are loaded for this library
*/
OAPC_EXT_API void oapc_set_loaded_data(void* instanceData,unsigned long length,char *loadedData)
{
   struct instData *data;

   data=(struct instData*)instanceData;

   if (length>sizeof(struct libio_config)) length=sizeof(struct libio_config);
   memcpy(&save_config,loadedData,length);
   convert_from_save_byteorder(&data->config,&save_config);
}



/**
This function handles all internal data initialisation and has to allocate a memory area where all
data are stored into that are required to operate this Plug-In. This memory area can be used by the
Plug-In freely, it is handed over with every function call so that the Plug-In cann access its
values. The memory area itself is released by the main application, therefore it has to be allocated
using malloc().
@return pointer where the allocated and pre-initialized memory area starts
*/
OAPC_EXT_API void* oapc_create_instance2(unsigned long flags)
{
   struct instData *data;

   flags=flags; // removing "unused" warning
   data=(struct instData*)malloc(sizeof(struct instData));
   init_instancedata(data,1820);
   return data;
}



/**
This function is called finally, it has to be used to release the instance data structure that was created
during the call of oapc_create_instance()
*/
OAPC_EXT_API void oapc_delete_instance(void* instanceData)
{
   if (instanceData) free(instanceData);
}



/**
When this function is called everything has to be initialized in order to perform the required operation
@return a return value/error code that informs the main application if the initialization was done successfully
        or not
*/
OAPC_EXT_API unsigned long oapc_init(void* instanceData)
{
   struct instData *data;

   data=(struct instData*)instanceData;

#ifdef ENV_WINDOWS
   WSADATA wsa;

   WSAStartup(MAKEWORD(2,0),&wsa);
#endif
   data->client[0].sock=oapc_tcp_connect_to(data->config.ip,data->config.port);
   if (data->client[0].sock<=0) return OAPC_ERROR_CONNECTION;

   // set socket to non-blocking in order to avoid deadlocks when sending fails
   oapc_tcp_set_blocking(data->client[0].sock,false);

   if (data->config.mode!=3) 
   {
      sprintf(sendbuf,"UNAME %s\nPWD %s\nMODE %d\n",data->config.uname,data->config.pwd,data->config.mode);
      if (oapc_tcp_send(data->client[0].sock,sendbuf,(int)strlen(sendbuf),500)<(int)strlen(sendbuf))
      {
         // pjbu7zhg
         oapc_exit(instanceData);
         return OAPC_ERROR_CONNECTION;
      }
   }
   return OAPC_OK;
}



/**
This function is called before the application unloads everything, it has to be used to deinitialize
everything and to release used resources.
*/
OAPC_EXT_API unsigned long oapc_exit(void* instanceData)
{
   struct instData *data;

   data=(struct instData*)instanceData;
   oapc_tcp_closesocket(data->client[0].sock);
#ifdef ENV_WINDOWS
   WSACleanup();
#endif
   return OAPC_OK;
}



/**
This function is called by the main application when the library provides an digital input (marked
using the digital input flags OAPC_DIGI_IO...), a connection was edited to this input and a data
flow reaches the input.
@param[in] input specifies the input where the data are send to, here not the OAPC_DIGI_IO...-flag is used
           but the plain, 0-based input number
@param[in] value specifies the value (0 or 1) that is set to that input
@return an error code OAPC_ERROR_... in case of an error or OAPC_OK in case the value could be set
*/
OAPC_EXT_API unsigned long  oapc_set_digi_value(void* instanceData,unsigned long input,unsigned char value)
{
   struct instData *data;

   data=(struct instData*)instanceData;

   if (input!=1) return OAPC_ERROR_NO_SUCH_IO; // check for valid IO
   if (data->config.mode==1)
   {
      data->cmdValuePair.val_num_set=0;
      data->cmdValuePair.val_char_set=0;
      data->cmdValuePair.val_digi_set=1;
      data->cmdValuePair.val_digi=value;
      check_cmd_value_pair(data);
   }
   else
   {
      sprintf(sendbuf,"DIGI %d\n",value);
      oapc_tcp_send(data->client[0].sock,sendbuf,(int)strlen(sendbuf),MAX_SEND_TIMEOUT);
   }
   return OAPC_OK;
}



/**
This function is called by the main application when the library provides an numerical input (marked
using the digital input flags OAPC_NUM_IO...), a connection was edited to this input and a data
flow reaches the input.
@param[in] input specifies the input where the data are send to, here not the OAPC_NUM_IO...-flag is used
           but the plain, 0-based input number
@param[in] value specifies the numerical floating-point value that is set to that input
@return an error code OAPC_ERROR_... in case of an error or OAPC_OK in case the value could be set
*/
OAPC_EXT_API unsigned long  oapc_set_num_value(void* instanceData,unsigned long input,double value)
{
   struct instData *data;

   data=(struct instData*)instanceData;

   if (input!=2) return OAPC_ERROR_NO_SUCH_IO; // check for valid IO
   if (data->config.mode==1)
   {
      data->cmdValuePair.val_num_set=1;
      data->cmdValuePair.val_char_set=0;
      data->cmdValuePair.val_digi_set=0;
      data->cmdValuePair.val_num=value;
      check_cmd_value_pair(data);
   }
   else
   {
      sprintf(sendbuf,"NUM %f\n",value);
      oapc_tcp_send(data->client[0].sock,sendbuf,(int)strlen(sendbuf),MAX_SEND_TIMEOUT);
   }
   return OAPC_OK;
}



/**
This function is called by the main application when the library provides an character input (marked
using the digital input flags OAPC_CHAR_IO...), a connection was edited to this input and a data
flow reaches the input.
@param[in] input specifies the input where the data are send to, here not the OAPC_CHAR_IO...-flag is used
           but a plain, 0-based input number
@param[in] value specifies the string that is set to that input
@return an error code OAPC_ERROR_... in case of an error or OAPC_OK in case the value could be set
*/
OAPC_EXT_API unsigned long  oapc_set_char_value(void* instanceData,unsigned long input,char *value)
{
   // here we allocate the send buffer dynamically because we do not know how big the string is
   char            *buffer;
   struct instData *data;

   data=(struct instData*)instanceData;

   if ((input!=0) && (input!=3)) return OAPC_ERROR_NO_SUCH_IO; // check for valid IO
   if (data->config.mode==1)
   {
      if (input==0) // command
      {
         data->cmdValuePair.cmd_set=1;
         strncpy(data->cmdValuePair.cmd,value,MAX_CMDVAL_SIZE);
         check_cmd_value_pair(data);
      }
      else // char value
      {
         data->cmdValuePair.val_num_set=0;
         data->cmdValuePair.val_char_set=1;
         data->cmdValuePair.val_digi_set=0;
         strncpy(data->cmdValuePair.val_char,value,MAX_CMDVAL_SIZE);
         check_cmd_value_pair(data);
      }
   }
   else
   {
      buffer=(char*)malloc(strlen(value)+10);
      if (!buffer) return OAPC_ERROR_RESOURCE;
      if (input==3) sprintf(buffer,"CHAR %s\n",value);
      else sprintf(buffer,"CMD %s\n",value);
      oapc_tcp_send(data->client[0].sock,buffer,(int)strlen(buffer),MAX_SEND_TIMEOUT);
      free((void*)buffer);
   }
   return OAPC_OK;
}



/**
This function is called by the main application when the library provides a binary data input (marked
using the digital input flags OAPC_BIN_IO...), a connection was edited to this input and a data
flow reaches the input.
@param[in] input specifies the input where the data are send to, here not the OAPC_BIN_IO...-flag is used
           but a plain, 0-based input number
@param[in] value pointer to the binary data; PLEASE NOTE: it is not allowed to change these data, they
           are a reference only that has to be copied before modification
@return an error code OAPC_ERROR_... in case of an error or OAPC_OK in case the value could be set
*/
OAPC_EXT_API unsigned long  oapc_set_bin_value(void* instanceData,unsigned long input,struct oapc_bin_head *value)
{
   struct instData     *data;
   struct oapc_bin_head binHead;

   data=(struct instData*)instanceData;

   if (input!=4) return OAPC_ERROR_NO_SUCH_IO; // check for valid IO
   if (data->config.mode==1)
   {
      if (data->client[0].val_bin) free(data->client[0].val_bin); // thats an erroneous condition: new binary data have been sent before the preceding ones could be submitted
      data->cmdValuePair.val_bin=(struct oapc_bin_head*)malloc(sizeof(struct oapc_bin_head)+value->sizeData);
      if (!data->cmdValuePair.val_bin) return OAPC_ERROR_NO_MEMORY;
      memcpy(data->client[0].val_bin,value,sizeof(struct oapc_bin_head)+value->sizeData);
      data->cmdValuePair.val_num_set=0;
      data->cmdValuePair.val_char_set=0;
      data->cmdValuePair.val_digi_set=0;
      data->cmdValuePair.val_bin_set=1;
      check_cmd_value_pair(data);
   }
   else
   {
      sprintf(sendbuf,"BIN\n");
      if (oapc_tcp_send(data->client[0].sock,sendbuf,(int)strlen(sendbuf),MAX_SEND_TIMEOUT)<=0) return OAPC_ERROR_SEND_DATA;
      convert_bin_to_network_byteorder(&binHead,value);
      if (oapc_tcp_send(data->client[0].sock,(char*)&binHead,sizeof(struct oapc_bin_head)-1,MAX_SEND_TIMEOUT)<=0) return OAPC_ERROR_SEND_DATA;
      if (oapc_tcp_send(data->client[0].sock,(char*)&value->data,value->sizeData,MAX_SEND_TIMEOUT*100)<=0) return OAPC_ERROR_SEND_DATA;
   }
   return OAPC_OK;
}

