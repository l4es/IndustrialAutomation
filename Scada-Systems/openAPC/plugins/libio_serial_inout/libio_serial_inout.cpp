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
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#ifndef ENV_WINDOWSCE
#include <sys/types.h>
#else
#include "Winsock2.h"
#endif

#ifdef ENV_LINUX
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <termios.h>
#include <unistd.h>
#endif

#ifdef ENV_QNX
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <termios.h>
#include <unistd.h>
#endif

#ifdef ENV_WINDOWS
#include <windows.h>
#endif

#include "oapc_libio.h"
#include "liboapc.h"

#define MAX_XML_SIZE   3500
#define MAX_CMDVAL_SIZE 250
#define MAX_AUTH_SIZE    20
#define MAX_SENDBUF_SIZE MAX_CMDVAL_SIZE+MAX_CMDVAL_SIZE+MAX_AUTH_SIZE+MAX_AUTH_SIZE+3



struct libio_config
{
   unsigned short version,length;
   struct serial_params serialParams;
   char           uname[MAX_AUTH_SIZE],pwd[MAX_AUTH_SIZE];
   unsigned short mode;
   // version 2
   unsigned char  termChar,res1,res2,res3;
};



struct cmd_value_pair
{
   unsigned char  cmd_set,val_char_set,val_num_set,val_digi_set;
   char           cmd[MAX_CMDVAL_SIZE];
   char           val_char[MAX_CMDVAL_SIZE];
   double         val_num;
   char           val_digi;
};



struct client_dataset
{
   unsigned int   loaded;
   unsigned short mode;
   char           buffer[MAX_SENDBUF_SIZE];
   char           uname[MAX_AUTH_SIZE],pwd[MAX_AUTH_SIZE];

   unsigned char  val_char_set,val_cmd_set,val_num_set,val_digi_set;
   char           val_char[MAX_CMDVAL_SIZE],val_cmd[MAX_CMDVAL_SIZE];
   float          val_num;
   char           val_digi;
   char           is_valid;
};



struct instData
{
   struct libio_config   config;
#ifdef ENV_WINDOWS
   HANDLE                m_hCommPort;
#else
   int                   m_hCommPort;
#endif
   struct cmd_value_pair cmdValuePair;
   struct client_dataset client;
   char                  termChar[10];
};



/** the xml data that define the behaviour and user interface of the plugin within the main application */
static char xmltempl[]="<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"?>\
<oapc-config>\
<flowimage>%s</flowimage>\
<dialogue>\
 <general>\
  <param text=\"Interface\">\
   <name>port</name>\
   <type>string</type>\
   <default>%s</default>\
   <min>4</min>\
   <max>12</max>\
  </param>\
  <param text=\"Data Rate\">\
   <name>brate</name>\
   <unit>bps</unit>\
   <type>option</type>\
   <value>110</value>\
   <value>300</value>\
   <value>1200</value>\
   <value>2400</value>\
   <value>4800</value>\
   <value>9600</value>\
   <value>19200</value>\
   <value>38400</value>\
   <value>57600</value>\
   <value>115200</value>\
   <value>230400</value>\
   <default>%d</default>\
  </param>\
  <param text=\"Data Bits\">\
   <name>databits</name>\
   <type>option</type>\
   <value>5</value>\
   <value>6</value>\
   <value>7</value>\
   <value>8</value>\
   <default>%d</default>\
  </param>\
  <param text=\"Parity\">\
   <name>parity</name>\
   <type>option</type>\
   <value>None</value>\
   <value>Even</value>\
   <value>Odd</value>\
   <default>%d</default>\
  </param>\
  <param text=\"Stop Bits\">\
   <name>stopbits</name>\
   <type>option</type>\
   <value>1</value>\
   <value>1.5</value>\
   <value>2</value>\
   <default>%d</default>\
  </param>\
  <param text=\"Flow Control\">\
   <name>flowcontrol</name>\
   <type>option</type>\
   <value>None</value>\
   <value>Xon / Xoff</value>\
   <value>CTS / RTS</value>\
   <default>%d</default>\
  </param>\
  <param text=\"Username\">\
   <name>uname</name>\
   <type>string</type>\
   <default>%s</default>\
   <min>0</min>\
   <max>%d</max>\
  </param>\
  <param text=\"Password\">\
   <name>pwd</name>\
   <type>string</type>\
   <default>%s</default>\
   <min>0</min>\
   <max>%d</max>\
  </param>\
  <param text=\"Mode\">\
   <name>mode</name>\
   <type>option</type>\
   <value>Command/Value - transmit only pairs</value>\
   <value>Plain - transmit everything separately</value>\
   <value>Raw - transmit separately and unformatted</value>\
   <value>Raw/E5AK checksum - append XOR-checksum</value>\
   <default>%d</default>\
  </param>\
  <param text=\"Termination Character\">\
   <name>termchar</name>\
   <type>option</type>\
   <value>None</value>\
   <value>CR - \\r - 0x0D</value>""\
   <value>LF - \\n - 0x0A</value>\
   <value>CRLF - \\r\\n - 0x0D0A</value>\
   <default>%d</default>\
  </param>\
 </general>\
  <helppanel>\
   <in0>CMD - command value</in0>\
   <in1>DATA - payload data</in1>\
   <in2>DATA - payload data</in2>\
   <in3>DATA - payload data</in3>\
   <out0>CMD - command value</out0>\
   <out1>DATA - payload data</out1>\
   <out2>DATA - payload data</out2>\
   <out3>DATA - payload data</out3>\
  </helppanel>\
 </dialogue>\n""\
</oapc-config>\n";
/** Base64-encoded PNG data of an image that is used as icon for the program flow diagram within the main application*/
static char                  flowImage[]="iVBORw0KGgoAAAANSUhEUgAAAGoAAAAyBAMAAABG0lM+AAAAMFBMVEUEAgT8/gSEgoQE/vz8AvzEwsT8/vwAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAACvX5NoAAAA/klEQVR42u2WwQ6CMAyGK0rP6hNoJJ6NvAAxPMBOnBuTlTeQ13dtEzm4Ic6jlITuJ3x/RxlkAFmBXLQ3ZKYzH5i5qK8iqlbFbh/Ee1CgXNt0zB65DBdcfRGxaVXstl2Kokp8ubiLL51M3FQcD5yi/EZ82ZH4+rWJRsWqTFJc6OTJfCMiTjmdvDffiIhTZGPzjYko9XUs1N9QwyOD6sMxGS4sZULLsykHUAqleaSGYQrygAQdoeW53aBQwCGh5bm15E4qCS2PtaZbCHoihJcwqs+ipmslZvjhuRLd+BCJzme+5awVlbd6lz/AQqUo3aqEEcgnFxFxatnb/Lq3yYon06+bji+nn8UAAAAASUVORK5CYII=";
static char                  libname[]="Serial Interface";
static char                  xmldescr[MAX_XML_SIZE+1],sendbuf[MAX_SENDBUF_SIZE];
static struct libio_config   save_config;



/**
This function is used in command/value mode and checks if already a complete pair of command and value was received.
If yes both values are transmitted via network and the used structure cmdValuePair is initialized in order to collect the
next pair of command and value.
This function is not part of the librarys external interface.
*/
static void check_cmd_value_pair(struct instData *data)
{
   if ((data->cmdValuePair.cmd_set) && ((data->cmdValuePair.val_char_set) || 
       (data->cmdValuePair.val_num_set) || (data->cmdValuePair.val_digi_set)))
   {
       if (data->cmdValuePair.val_char_set)
       {
          sprintf(sendbuf,"CHAR %s \"%s\"\n",data->cmdValuePair.cmd,data->cmdValuePair.val_char);
          data->cmdValuePair.val_char_set=0;
       }
       else if (data->cmdValuePair.val_num_set)
       {
          sprintf(sendbuf,"NUM %s %f\n",data->cmdValuePair.cmd,data->cmdValuePair.val_num);
          data->cmdValuePair.val_num_set=0;
       }
       else if (data->cmdValuePair.val_digi_set)
       {
          sprintf(sendbuf,"DIGI %s %d\n",data->cmdValuePair.cmd,data->cmdValuePair.val_digi);
          data->cmdValuePair.val_digi_set=0;
       }
       data->cmdValuePair.cmd_set=0;
       oapc_serial_send_data(data->m_hCommPort,sendbuf,(int)strlen(sendbuf),MAX_SEND_TIMEOUT);
   }
}



/**
This function checks if there are new incoming data. They are polled for new data.
This function is not part of the external library interface.
*/
static int check_client(struct instData *data)
{
   int                len;
   char              *id,*cr;

   if (!data->client.is_valid) return OAPC_ERROR;
   data->client.loaded+=oapc_serial_recv_data(data->m_hCommPort,data->client.buffer+data->client.loaded,sizeof(data->client.buffer)-data->client.loaded,10);
   cr=strstr(data->client.buffer,"\n");
   if (!cr) cr=strstr(data->client.buffer,"\r");
   if (cr)
   {
      if (data->client.mode==0) // no mode information detected, waiting for MODE tag
      {
         *cr=0;
         len=(int)strlen(data->client.buffer)+1;
         id=strstr(data->client.buffer,"UNAME ");
         if (id) 
         {
            strncpy(data->client.uname,id+6,MAX_AUTH_SIZE);
            memmove(data->client.buffer,cr+1,sizeof(data->client.buffer)-len);
            data->client.loaded-=len;
         }
         else
         {
            id=strstr(data->client.buffer,"PWD ");
            if (id) 
            {
               strncpy(data->client.uname,id+4,MAX_AUTH_SIZE);
               memmove(data->client.buffer,cr+1,sizeof(data->client.buffer)-len);
               data->client.loaded-=len;
               if ((strncmp(data->client.pwd,data->config.pwd,MAX_AUTH_SIZE)) || 
                   (strncmp(data->client.uname,data->config.uname,MAX_AUTH_SIZE)))
               {
                  oapc_serial_send_data(data->m_hCommPort,"FAIL authentication error\n",28,100);
                  data->client.is_valid=0;
                  return OAPC_ERROR_AUTHENTICATION;
               }
            }
            else
            {
               id=strstr(data->client.buffer,"MODE ");
               if (id) 
               {
                  data->client.mode=(unsigned short)atoi(id+5);
                  memmove(data->client.buffer,cr+1,sizeof(data->client.buffer)-len);
                  data->client.loaded-=len;
               }
            }
         }
      }
      else if (data->client.mode==2) // plain data mode
      {
         len=(int)strlen(data->client.buffer)+1;
         if (!data->client.val_num_set)
         {
            id=strstr(data->client.buffer,"NUM ");
            if (id==data->client.buffer)
            {
               data->client.val_num=(float)oapc_util_atof(id+4);
               data->client.val_num_set=1;
               memmove(data->client.buffer,cr+1,sizeof(data->client.buffer)-len);
               data->client.loaded-=len;
            }
         }
         if (!data->client.val_digi_set)
         {
            id=strstr(data->client.buffer,"DIGI ");
            if (id==data->client.buffer)
            {
               if (atoi(id+5)!=0) data->client.val_digi=1;
               else data->client.val_digi=0;
               data->client.val_digi_set=1;
               memmove(data->client.buffer,cr+1,sizeof(data->client.buffer)-len);
               data->client.loaded-=len;
            }
         }
         if (!data->client.val_char_set)
         {
            id=strstr(data->client.buffer,"CHAR ");
            if (id==data->client.buffer)
            {
               *cr=0;
               if (strlen(id)>5) strncpy(data->client.val_char,id+5,MAX_CMDVAL_SIZE);
               else data->client.val_char[0]=0;
               data->client.val_char_set=1;
               memmove(data->client.buffer,cr+1,sizeof(data->client.buffer)-len);
               data->client.loaded-=len;
            }
         }
         if (!data->client.val_cmd_set)
         {
            id=strstr(data->client.buffer,"CMD ");
            if (id==data->client.buffer)
            {
               *cr=0;
               if (strlen(id)>4) strncpy(data->client.val_cmd,id+4,MAX_CMDVAL_SIZE);
               else data->client.val_cmd[0]=0;
               data->client.val_cmd_set=1;
               memmove(data->client.buffer,cr+1,sizeof(data->client.buffer)-len);
               data->client.loaded-=len;
            }
         }
      }
      else if ((data->config.mode>=3) && (data->client.val_cmd_set==0))
      {
         len=(int)strlen(data->client.buffer)+1;
         *cr=0;
         strncpy(data->client.val_cmd,data->client.buffer,MAX_CMDVAL_SIZE);
         data->client.val_cmd_set=1;
//         memmove(data->client.buffer,cr+1,sizeof(data->client.buffer)-len);
         data->client.loaded=0; //-=len;
      }
   }
   return OAPC_OK;
}



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
   return OAPC_HAS_INPUTS|OAPC_HAS_OUTPUTS|
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
          OAPC_CHAR_IO3;
   // avoid it to submit the same IO number for different data types, that would cause displaying problems for the flow symbol
}



/**
When the OAPC_HAS_OUTPUTS flag is set, the application calls this function to get the configuration
for the outputs. Here "output" is defined from the libraries point of view, means data that are sent from
the library to the application are output data
@return or-concatenated OAPC_..._IO...-flags that describe which outputs and data types are used or 0 in case
        of an error
*/
OAPC_EXT_API unsigned long oapc_get_output_flags(void)
{
   return OAPC_CHAR_IO0|
          OAPC_DIGI_IO1|
          OAPC_NUM_IO2|
          OAPC_CHAR_IO3;
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

   sprintf(xmldescr,xmltempl,flowImage,
                             data->config.serialParams.port,data->config.serialParams.brate,
                             data->config.serialParams.databits,data->config.serialParams.parity,
                             data->config.serialParams.stopbits,data->config.serialParams.flowcontrol,
                             data->config.uname,MAX_AUTH_SIZE,data->config.pwd,MAX_AUTH_SIZE,
                             data->config.mode,data->config.termChar);
   assert(strlen(xmldescr)<MAX_XML_SIZE);
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

   if (strcmp(name,"port")==0)             strncpy(data->config.serialParams.port,value,MAX_TTY_SIZE);
   else if (strcmp(name,"brate")==0)       data->config.serialParams.brate=(unsigned short)atoi(value);
   else if (strcmp(name,"parity")==0)      data->config.serialParams.parity=(unsigned short)atoi(value);
   else if (strcmp(name,"databits")==0)    data->config.serialParams.databits=(unsigned short)atoi(value);
   else if (strcmp(name,"flowcontrol")==0) data->config.serialParams.flowcontrol=(unsigned short)atoi(value);
   else if (strcmp(name,"stopbits")==0)    data->config.serialParams.stopbits=(unsigned short)atoi(value);
   else if (strcmp(name,"uname")==0)       strncpy(data->config.uname,value,MAX_AUTH_SIZE);
   else if (strcmp(name,"pwd")==0)         strncpy(data->config.pwd,value,MAX_AUTH_SIZE);
   else if (strcmp(name,"mode")==0)        data->config.mode=(unsigned short)atoi(value);
   else if (strcmp(name,"termchar")==0)    data->config.termChar=(unsigned char)atoi(value);
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

   data=(struct instData*)instanceData;

   *length=sizeof(struct libio_config);
   save_config.version    =htons(2);
   save_config.length     =htons(sizeof(struct libio_config));
   strncpy(save_config.serialParams.port,data->config.serialParams.port,MAX_TTY_SIZE);
   save_config.serialParams.brate      =htons(data->config.serialParams.brate);
   save_config.serialParams.parity     =htons(data->config.serialParams.parity);
   save_config.serialParams.databits   =htons(data->config.serialParams.databits);
   save_config.serialParams.flowcontrol=htons(data->config.serialParams.flowcontrol);
   save_config.serialParams.stopbits   =htons(data->config.serialParams.stopbits);
   strncpy(save_config.uname,data->config.uname,MAX_AUTH_SIZE);
   strncpy(save_config.pwd,data->config.pwd,MAX_AUTH_SIZE);
   save_config.mode       =htons(data->config.mode);
   save_config.termChar   =data->config.termChar;
   save_config.res1       =0;
   save_config.res2       =0;
   save_config.res3       =0;

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
   data->config.version    =ntohs(save_config.version);
   data->config.length     =ntohs(save_config.length);
   strncpy(data->config.serialParams.port,save_config.serialParams.port,MAX_TTY_SIZE);
   data->config.serialParams.brate      =ntohs(save_config.serialParams.brate);
   data->config.serialParams.parity     =ntohs(save_config.serialParams.parity);
   data->config.serialParams.databits   =ntohs(save_config.serialParams.databits);
   data->config.serialParams.flowcontrol=ntohs(save_config.serialParams.flowcontrol);
   data->config.serialParams.stopbits   =ntohs(save_config.serialParams.stopbits);
   strncpy(data->config.uname,save_config.uname,MAX_AUTH_SIZE);
   strncpy(data->config.pwd,save_config.pwd,MAX_AUTH_SIZE);
   data->config.mode       =ntohs(save_config.mode);
   if (data->config.version>=2)
   {
      data->config.termChar=save_config.termChar;
      data->config.res1=0;
      data->config.res2=0;
      data->config.res3=0;
   }
   else data->config.termChar=3;
}



/**
When this function is called everything has to be initialized in order to perform the required operation
@return a return value/error code that informs the main application if the initialization was done successfully
        or not
*/
OAPC_EXT_API void* oapc_create_instance2(unsigned long flags)
{
   struct instData *data;

   flags=flags; // removing "unused" warning
   data=(struct instData*)malloc(sizeof(struct instData));
  
#ifdef ENV_WINDOWS
   strcpy(data->config.serialParams.port,"COM1");
 #else
  #ifdef ENV_LINUX
   strcpy(data->config.serialParams.port,"/dev/ttyS0");
  #else
   #ifdef ENV_QNX
    strcpy(data->config.serialParams.port,"/dev/ser1");
   #else
    #error Not implemented!
   #endif
 #endif
#endif

   data->config.serialParams.brate=6;
   data->config.serialParams.databits=4;
   data->config.serialParams.flowcontrol=1;
   data->config.mode=1;
   data->config.serialParams.parity=1;
   data->config.serialParams.stopbits=1;
   data->config.pwd[0]=0;
   data->config.uname[0]=0;
   data->config.termChar=3;

   memset(&data->cmdValuePair,0,sizeof(struct cmd_value_pair));
   memset(&data->client,0,sizeof(struct client_dataset));
   data->client.is_valid=1;
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
   int              ret;

   data=(struct instData*)instanceData;

   ret=oapc_serial_port_open(&data->config.serialParams,&data->m_hCommPort);
   if (ret!=OAPC_OK)
   {
   	oapc_exit(instanceData);
   	return ret;
   }
   if (data->config.mode!=3)
    sprintf(sendbuf,"UNAME %s\nPWD %s\nMODE %d\n",data->config.uname,data->config.pwd,data->config.mode);
   else
    data->client.mode=3;
   if (oapc_serial_send_data(data->m_hCommPort,sendbuf,(int)strlen(sendbuf),500)<(int)strlen(sendbuf))
   {
      oapc_exit(instanceData);
      return OAPC_ERROR_CONNECTION;
   }
   if (data->config.termChar==1) strcpy(data->termChar,"");
   else if (data->config.termChar==2) strcpy(data->termChar,"\r");
   else if (data->config.termChar==3) strcpy(data->termChar,"\n");
   else if (data->config.termChar==4) strcpy(data->termChar,"\r\n");
   else assert(0);
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
   oapc_serial_port_close(&data->m_hCommPort);
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
      sprintf(sendbuf,"DIGI %d%s",value,data->termChar);
      oapc_serial_send_data(data->m_hCommPort,sendbuf,(int)strlen(sendbuf),MAX_SEND_TIMEOUT);
   }
   return OAPC_OK;
}



/**
This function is called by the main application periodically in order to poll the state of the related
output. It has to return the data that are available for that input or - in case there are none available -
the error code OAPC_ERROR_NO_DATA_AVAILABLE to notify the main application, that there is nothing new.
@param[in] output specifies the output where the data are fetched from, here not the OAPC_DIGI_IO...-flag is used
           but the plain, 0-based output number
@param[out] value if there are new data available, they are stored within that variable, if there are no new
           data, the returned data are ignored by the main application
@return an error code OAPC_ERROR_... in case of an error, OAPC_ERROR_NO_DATA_AVAILABLE in case no new data are
           available or OAPC_OK in case the value could be set
*/
OAPC_EXT_API unsigned long  oapc_get_digi_value(void* instanceData,unsigned long output,unsigned char *value)
{
   int i;
   struct instData *data;

   if (output!=1) return OAPC_ERROR_NO_SUCH_IO;
   data=(struct instData*)instanceData;

   i=check_client(data);
   if (i!=OAPC_OK) return i;
   if (data->client.val_digi_set)
   {
      *value=data->client.val_digi;
      data->client.val_digi_set=0;
      return OAPC_OK;
   }
   return OAPC_ERROR_NO_DATA_AVAILABLE; // this library does not support inputs
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
      sprintf(sendbuf,"NUM %f%s",value,data->termChar);
      oapc_serial_send_data(data->m_hCommPort,sendbuf,(int)strlen(sendbuf),MAX_SEND_TIMEOUT);
   }
   return OAPC_OK;
}



/**
This function is called by the main application periodically in order to poll the state of the related
output. It has to return the data that are available for that input or - in case there are none available -
the error code OAPC_ERROR_NO_DATA_AVAILABLE to notify the main application, that there is nothing new.
@param[in] output specifies the output where the data are fetched from, here not the OAPC_NUM_IO...-flag is used
           but the plain, 0-based output number
@param[out] value if there are new data available, they are stored within that variable, if there are no new
           data, the returned data are ignored by the main application
@return an error code OAPC_ERROR_... in case of an error, OAPC_ERROR_NO_DATA_AVAILABLE in case no new data are
           available or OAPC_OK in case the value could be set
*/
OAPC_EXT_API unsigned long  oapc_get_num_value(void* instanceData,unsigned long output,double *value)
{
   int              i;
   struct instData *data;

   if (output!=2) return OAPC_ERROR_NO_SUCH_IO;
   data=(struct instData*)instanceData;

   i=check_client(data);
   if (i!=OAPC_OK) return i;
   if (data->client.val_num_set)
   {
      *value=data->client.val_num;
      data->client.val_num_set=0;
      return OAPC_OK;
   }
   return OAPC_ERROR_NO_DATA_AVAILABLE; // this library does not support inputs
}


static char getE5AKChecksum(char *value)
{
   char *c;
   char  sum=0;

   c=value;
   while (*c)
   {
      sum=(char)(sum^(*c));
      c++;
   }
   return sum;
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
   else if (data->config.mode==3)
   {
      buffer=(char*)malloc(strlen(value)+10);
      if (!buffer) return OAPC_ERROR_RESOURCE;
      sprintf(buffer,"%s%s",value,data->termChar);
      oapc_serial_send_data(data->m_hCommPort,buffer,(int)strlen(buffer),MAX_SEND_TIMEOUT);
      free((void*)buffer);
   }
   else if (data->config.mode==4)
   {
      buffer=(char*)malloc(strlen(value)+10);
      if (!buffer) return OAPC_ERROR_RESOURCE;
      sprintf(buffer,"%s%c*%s",value,getE5AKChecksum(value),data->termChar);
      oapc_serial_send_data(data->m_hCommPort,buffer,(int)(strlen(value)+strlen(data->termChar)+2),MAX_SEND_TIMEOUT);
      free((void*)buffer);
   }
   else
   {
      buffer=(char*)malloc(strlen(value)+10);
      if (!buffer) return OAPC_ERROR_RESOURCE;
      if (input==3) sprintf(buffer,"CHAR %s%s",value,data->termChar);
      else sprintf(buffer,"CMD %s%s",value,data->termChar);
      oapc_serial_send_data(data->m_hCommPort,buffer,(int)strlen(buffer),MAX_SEND_TIMEOUT);
      free((void*)buffer);
   }
   return OAPC_OK;
}



/**
This function is called by the main application periodically in order to poll the state of the related
output. It has to return the data that are available for that input or - in case there are none available -
the error code OAPC_ERROR_NO_DATA_AVAILABLE to notify the main application, that there is nothing new.
@param[in] output specifies the output where the data are fetched from, here not the OAPC_CHAR_IO...-flag is used
           but the plain, 0-based output number
@param[out] value if there are new data available, they are stored within that variable, if there are no new
           data, the returned data are ignored by the main application
@return an error code OAPC_ERROR_... in case of an error, OAPC_ERROR_NO_DATA_AVAILABLE in case no new data are
           available or OAPC_OK in case the value could be set
*/
OAPC_EXT_API unsigned long  oapc_get_char_value(void* instanceData,unsigned long output,unsigned long length,char *value)
{
   int              i;
   struct instData *data;

   data=(struct instData*)instanceData;

   i=check_client(data);
   if (i!=OAPC_OK) return i;
   if ((output!=0) && (output!=3)) return OAPC_ERROR_NO_SUCH_IO; // check for valid IO
   if ((output==3) && (data->client.val_char_set))
   {
      if (length>MAX_CMDVAL_SIZE) length=MAX_CMDVAL_SIZE;
      strncpy(value,data->client.val_char,length);
      data->client.val_char_set=0;
      return OAPC_OK;
   }
   else if ((output==0) && (data->client.val_cmd_set))
   {
      if (length>MAX_CMDVAL_SIZE) length=MAX_CMDVAL_SIZE;
      strncpy(value,data->client.val_cmd,length);
      data->client.val_cmd_set=0;
      return OAPC_OK;
   }
   return OAPC_ERROR_NO_DATA_AVAILABLE; // this library does not support inputs
}


