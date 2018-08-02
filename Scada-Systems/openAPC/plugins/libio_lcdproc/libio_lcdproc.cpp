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
 #define closesocket close
#else
 #include <windows.h>
 #define MSG_NOSIGNAL 0
 typedef int socklen_t;
#endif

#ifdef ENV_QNX
 #define MSG_NOSIGNAL 0

 #include <sys/types.h>
 #include <sys/socket.h>
 #include <arpa/inet.h>
#endif


#include "oapc_libio.h"
#include "liboapc.h"

#define MAX_IP_SIZE       128
#define REC_BUF_SIZE     4096
#define MAX_XML_SIZE     5000
#define MAX_LCDLINE_SIZE  200

// backlight definitions
#define LCDPROC_FLAG_BL_OFF   1
#define LCDPROC_FLAG_BL_ON    2
#define LCDPROC_FLAG_BL_BLINK 3
#define LCDPROC_FLAG_BL_FLASH 4
#define LCDPROC_MASK_BL       7

#define LCDPROC_TYPE_TEXT 1
#define LCDPROC_TYPE_BAR  2


#pragma pack(8)

struct libio_config
{
   unsigned short    version,length;
   char              ip[MAX_IP_SIZE];
   unsigned short    port;
   int               flags;
   int               splitPos;        // position where the screen is splitted
   int               typeLeft[4],typeRight[4];
   int               scrollSpeed;
};

struct instData
{
   struct libio_config   config;
   int                   sock;
   int                   w,h;
};



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
  </general>\n\
  <stdpanel>\n\
   <name>Display Parameters</name>\n\
   <param>\n\
    <name>backlight</name>\n\
    <text>Backlight</text>\n\
    <type>option</type>\n\
    <value>Off</value>\n\
    <value>On</value>\n\
    <value>Blink</value>\n\
    <value>Flash</value>\n\
    <default>%d</default>\n\
   </param>\n\
   <param>\n\
    <name>splitpos</name>\n\
    <text>Horizontal Split Position</text>\n\
    <type>integer</type>\n\
    <default>%d</default>\n\
    <min>0</min>\n\
    <max>%d</max>\n\
   </param>\n\
   <param>\n\
    <name>scrollspeed</name>\n\
    <text>Scrolling Speed</text>\n\
    <type>integer</type>\n\
    <default>%d</default>\n\
    <min>0</min>\n\
    <max>30</max>\n\
   </param>\n\
   <param>\n\
    <name>line1</name>\n\
    <text>Line 1</text>\n\
    <type>option</type>\n\
    <value>Text left / Bargraph right</value>\n\
    <value>Bargraph left / Text Right</value>\n\
    <default>%d</default>\n\
   </param>\n\
   <param>\n\
    <name>line2</name>\n\
    <text>Line 2</text>\n\
    <type>option</type>\n\
    <value>Text left / Bargraph right</value>\n\
    <value>Bargraph left / Text Right</value>\n\
    <default>%d</default>\n\
   </param>\n\
   <param>\n\
    <name>line3</name>\n\
    <text>Line 3</text>\n\
    <type>option</type>\n\
    <value>Text left / Bargraph right</value>\n\
    <value>Bargraph left / Text Right</value>\n\
    <default>%d</default>\n\
   </param>\n\
   <param>\n\
    <name>line4</name>\n\
    <text>Line 4</text>\n\
    <type>option</type>\n\
    <value>Text left / Bargraph right</value>\n\
    <value>Bargraph left / Text Right</value>\n\
    <default>%d</default>\n\
   </param>\n\
  </stdpanel>\n\
  <helppanel>""\n\
   <in0>LN1 - text to be displayed in line 1</in0>\n\
   <in1>LN1 - bargraph value to be displayed in line 1</in1>\n\
   <in2>LN2 - text to be displayed in line 2</in2>\n\
   <in3>LN2 - bargraph value to be displayed in line 2</in3>\n\
   <in4>...</in4>\n\
  </helppanel>\n\
 </dialogue>\n\
</oapc-config>\n";
/** Base64-encoded PNG data of an image that is used as icon for the program flow diagram within the main application*/
static char                 flowImage[]="iVBORw0KGgoAAAANSUhEUgAAAGoAAAAyCAMAAACDIr4/AAADAFBMVEUEAgQEhAT8AvwHRQeEgoQSIg4EugQFZQUQFQoEogQHNQcE0AQEWwUTHBIEdAQElwQErQQE/vzEwsT8/vwEyAQE3gQELgQHPgUPDAwcFRQETgQEbgQUMhQUKhQEjgQEfgQAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAB80yh8AAACGElEQVR42u2XYW/iMAyGG7o6mKSxAfcYzV3y///l3HYdTKJ3ogrTfaiRKJZRntfJGyOq6gfjfD5Xb7vd7m34NCbH4/GWFKtMqFOlxeo0FU+VFm9JscqEGoTMMkYh90mpyidKhcwyRiH3SanKJ0rZXzJelcyo003Gq5IZdb6T8aLkC/X6mB1Y3TG/JYOmhWQdarn9anw9Stailr9QGrW8gdV5KVnd1bKBCtviL2e1bJjiDizW779VFtvZb2f+aM3iG7hG/vsY1ynatm3+/L7U3gaTiEFE3BhPndWSiOt1v2+bQ+2jIdTFh4VlCneLZ85qEdUS6PrdXUwkAAaFUkooT6GW+21AF3fAjkk6DBHF1nUAyZdLZAFf15bk2a4eRwO6EuSIOZMz2UTyIficDRlvySOZ2LMUQbVM7JAYAyKaxDH7TNH6mDD63Ks5GBHKoDCxI3Iug/bmQ8qX3uahIzA+9Ia152JdjaiuMwDBhpCo7+uQfCQxNipqjQOXUAaFCDADcdTtYq+9oVVUsNErSqQQquHkLdvI3qvtUM3okwmYI0GIsU8sOYgUcqDTYwcEQeiAu04ycTDoDxdDZHRyJL3ZhbrSa+U6vcTDPR4egMLInNTvzCbGRFimq/d982uYSjjPjGFY6KQQ1jkl+q7jAsqclc7a+c/Mvjn0Vi2oGwbDZNI3NzygkC3K/zRuqA21oTbUhtpQ/wXqx+IDJPtV9TTzmHUAAAAASUVORK5CYII=";
static char                 libname[]="LCDproc Client";
static char                 xmldescr[MAX_XML_SIZE+1];
static struct libio_config  save_config;



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
   return OAPC_CHAR_IO0|OAPC_NUM_IO1|OAPC_CHAR_IO2|OAPC_NUM_IO3|
          OAPC_CHAR_IO4|OAPC_NUM_IO5|OAPC_CHAR_IO6|OAPC_NUM_IO7;
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

   sprintf(xmldescr,xmltempl,flowImage,data->config.ip,MAX_IP_SIZE,
                                       data->config.port,
                                       data->config.flags & LCDPROC_MASK_BL,
                                       data->config.splitPos,MAX_LCDLINE_SIZE,
                                       data->config.scrollSpeed,
                                       data->config.typeLeft[0],data->config.typeLeft[1],
                                       data->config.typeLeft[2],data->config.typeLeft[3]);
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
   int              i;

   data=(struct instData*)instanceData;

   if (strcmp(name,"ip")==0)               strncpy(data->config.ip,value,MAX_IP_SIZE);
   else if (strcmp(name,"port")==0)        data->config.port=(unsigned short)atoi(value);
   else if (strcmp(name,"backlight")==0)
   {
   	data->config.flags&=~LCDPROC_MASK_BL;
   	data->config.flags|=(unsigned short)atoi(value);
   }
   else if (strcmp(name,"splitpos")==0)    data->config.splitPos=(unsigned short)atoi(value);
   else if (strcmp(name,"scrollspeed")==0) data->config.scrollSpeed=(unsigned short)atoi(value);
   else if (strcmp(name,"line1")==0)       data->config.typeLeft[0]=(unsigned short)atoi(value);
   else if (strcmp(name,"line2")==0)       data->config.typeLeft[1]=(unsigned short)atoi(value);
   else if (strcmp(name,"line3")==0)       data->config.typeLeft[2]=(unsigned short)atoi(value);
   else if (strcmp(name,"line4")==0)       data->config.typeLeft[3]=(unsigned short)atoi(value);
   for (i=0; i<4; i++) data->config.typeRight[i]=3-data->config.typeLeft[3];
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
   int              i;

   data=(struct instData*)instanceData;

   *length=sizeof(struct libio_config);
   save_config.version    =htons(data->config.version);
   save_config.length     =htons(data->config.length);
   strncpy(save_config.ip,data->config.ip,MAX_IP_SIZE);
   save_config.port       =htons(data->config.port);
   save_config.flags      =htonl(data->config.flags);
   save_config.splitPos   =htonl(data->config.splitPos);
   save_config.scrollSpeed=htonl(data->config.scrollSpeed);
   for (i=0; i<4; i++)
   {
      save_config.typeLeft[i]=htonl(data->config.typeLeft[i]);
      save_config.typeRight[i]=htonl(data->config.typeRight[i]);
   }
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
   int              i;

   data=(struct instData*)instanceData;

   if (length>sizeof(struct libio_config)) length=sizeof(struct libio_config);
   memcpy(&save_config,loadedData,length);
   data->config.version    =ntohs(save_config.version);
   data->config.length     =ntohs(save_config.length);
   strncpy(data->config.ip,save_config.ip,MAX_IP_SIZE);
   data->config.port       =ntohs(save_config.port);
   data->config.flags      =ntohl(save_config.flags);
   data->config.splitPos   =ntohl(save_config.splitPos);
   data->config.scrollSpeed=ntohl(save_config.scrollSpeed);
   for (i=0; i<4; i++)
   {
      data->config.typeLeft[i]=ntohl(save_config.typeLeft[i]);
      data->config.typeRight[i]=ntohl(save_config.typeRight[i]);
   }
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
   flags=flags; // removing "unused" warning

   struct instData *data;

   data=(struct instData*)malloc(sizeof(struct instData));

   memset(data,0,sizeof(struct instData));
   data->config.version=1;
   data->config.length=sizeof(struct libio_config);
   strcpy(data->config.ip,"192.168.1.1");
   data->config.port=13666;
   data->config.flags=LCDPROC_FLAG_BL_ON;
   data->config.splitPos=10;
   data->config.scrollSpeed=3;
   data->config.typeLeft[0]=LCDPROC_TYPE_TEXT; data->config.typeRight[0]=LCDPROC_TYPE_BAR;  
   data->config.typeLeft[1]=LCDPROC_TYPE_TEXT; data->config.typeRight[1]=LCDPROC_TYPE_BAR;  
   data->config.typeLeft[2]=LCDPROC_TYPE_TEXT; data->config.typeRight[2]=LCDPROC_TYPE_BAR;  
   data->config.typeLeft[3]=LCDPROC_TYPE_TEXT; data->config.typeRight[3]=LCDPROC_TYPE_BAR;  
   
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



static int send_request(const int sock, const char *txt)
{
   if (oapc_tcp_send(sock,txt,(int)strlen(txt),750)<(int)strlen(txt))
   {
       perror("send() failed");
       return OAPC_ERROR_SEND_DATA;
   }
   return OAPC_OK;
}



static int parse_response(struct instData *data,char *response)
{
   char *c;
   
   c=strstr(response,"protocol ");
   if (c)
   {
      c+=9;
      if (strstr("0.3",c)==0) printf("Protocol OK\n");
      else
      { 
         printf("Unknown protocol %s\n",c);
         return OAPC_ERROR_PROTOCOL;
      }
   }
   
   c=strstr(response,"wid ");
   if (c)
   {
      c+=4;
      data->w=atoi(c);
   }
   c=strstr(response,"hgt ");
   if (c)
   {
      c+=4;
      data->h=atoi(c);
   }
   printf("Display: %dx%d\n",data->w,data->h);
   return OAPC_OK;
}



/**
When this function is called everything has to be initialized in order to perform the required operation
@return a return value/error code that informs the main application if the initialization was done successfully
        or not
*/
OAPC_EXT_API unsigned long oapc_init(void* instanceData)
{
   int              ret,i;
   char             response[REC_BUF_SIZE];
   struct instData *data;
   
   data=(struct instData*)instanceData;

#ifdef ENV_WINDOWS
   WSADATA wsa;

   WSAStartup(MAKEWORD(2,0),&wsa);
#endif
   data->sock=oapc_tcp_connect_to(data->config.ip,data->config.port);
   if (data->sock<=0) return OAPC_ERROR_CONNECTION;
   // set socket to non-blocking in order to avoid deadlocks when sending fails
   oapc_tcp_set_blocking(data->sock,false);

   if (send_request(data->sock,"hello\n")!=OAPC_OK)
   {
   	oapc_exit(data);
   	return OAPC_ERROR_SEND_DATA;
   }
   oapc_tcp_recv(data->sock,response,REC_BUF_SIZE-1,"\n",750);
   ret=parse_response(data,response);
   if (ret!=OAPC_OK)
   {
   	oapc_exit(data);
   	return ret;
   }

   if ((data->config.flags & LCDPROC_MASK_BL)==LCDPROC_FLAG_BL_OFF)        send_request(data->sock,"backlight off\n");
   else if ((data->config.flags & LCDPROC_MASK_BL)==LCDPROC_FLAG_BL_ON)    send_request(data->sock,"backlight on\n");
   else if ((data->config.flags & LCDPROC_MASK_BL)==LCDPROC_FLAG_BL_BLINK) send_request(data->sock,"backlight blink\n");
   else if ((data->config.flags & LCDPROC_MASK_BL)==LCDPROC_FLAG_BL_FLASH) send_request(data->sock,"backlight flash\n");
    
   send_request(data->sock,"client_set name {oapc-lib}\n");
   oapc_tcp_recv(data->sock,response,REC_BUF_SIZE-1,"\n",200);
   send_request(data->sock,"screen_add oapc-scr\n");
   oapc_tcp_recv(data->sock,response,REC_BUF_SIZE-1,"\n",200);
   send_request(data->sock,"screen_set oapc-scr name {OAPC} priority foreground heartbeat off\n");
   oapc_tcp_recv(data->sock,response,REC_BUF_SIZE-1,"\n",200);
   for (i=0; i<4; i++)
   {
      if (data->config.splitPos>0)
      {
         if (data->config.typeLeft[i]==LCDPROC_TYPE_TEXT)
         {
            sprintf(response,"widget_add oapc-scr lineL%d scroller\n",i+1);
            send_request(data->sock,response);
         }
         else // LCDPROC_TYPE_BAR
         {
            sprintf(response,"widget_add oapc-scr barL%d hbar\n",i+1);
            send_request(data->sock,response);
         }
         oapc_tcp_recv(data->sock,response,REC_BUF_SIZE-1,"\n",200);
      }
      if (data->config.splitPos<data->w)
      {
         if (data->config.typeRight[i]==LCDPROC_TYPE_TEXT)
         {
            sprintf(response,"widget_add oapc-scr lineR%d scroller\n",i+1);
            send_request(data->sock,response);
         }
         else // LCDPROC_TYPE_BAR
         {
            sprintf(response,"widget_add oapc-scr barR%d hbar\n",i+1);
            send_request(data->sock,response);
         }
         oapc_tcp_recv(data->sock,response,REC_BUF_SIZE-1,"\n",200);
      }
   }
   oapc_tcp_recv(data->sock,response,REC_BUF_SIZE-1,"\n",200);
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

   send_request(data->sock,"screen_del oapc-scr\n");
   send_request(data->sock,"client_del oapc-lib\n");

   oapc_tcp_closesocket(data->sock);
#ifdef ENV_WINDOWS
   WSACleanup();
#endif
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
   int              idx=input/2;
   char             lcdProcCmd[REC_BUF_SIZE];

   data=(struct instData*)instanceData;

   if (data->config.splitPos>0)
   {
      if (data->config.typeLeft[idx]==LCDPROC_TYPE_BAR)
      {
         sprintf(lcdProcCmd,"widget_set oapc-scr barL%d 1 %d %d\n",idx+1,idx+1,(int)(value*(data->config.splitPos+2)/data->w));
         send_request(data->sock,lcdProcCmd);
      }
   }
   if (data->config.splitPos<data->w)
   {
      if (data->config.typeRight[idx]==LCDPROC_TYPE_BAR)
      {
         sprintf(lcdProcCmd,"widget_set oapc-scr barR%d %d %d %d\n",idx+1,data->config.splitPos+1,idx+1,(int)(value*(data->w-(data->config.splitPos-2))/data->w));
         send_request(data->sock,lcdProcCmd);
      }
   }
   oapc_tcp_recv(data->sock,lcdProcCmd,REC_BUF_SIZE-1,"\n",1000);

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
   struct instData *data;
   int              idx=input/2;
   char             lcdProcCmd[REC_BUF_SIZE];

   data=(struct instData*)instanceData;

   if (data->config.splitPos>0)
   {
      if (data->config.typeLeft[idx]==LCDPROC_TYPE_TEXT)
      {
         sprintf(lcdProcCmd,"widget_set oapc-scr lineL%d 1 %d %d 1 h %d {%s}\n",idx+1,idx+1,data->config.splitPos,data->config.scrollSpeed,value);
         send_request(data->sock,lcdProcCmd);
      }
   }
   if (data->config.splitPos<data->w)
   {
      if (data->config.typeRight[idx]==LCDPROC_TYPE_TEXT)
      {
         sprintf(lcdProcCmd,"widget_set oapc-scr lineR%d %d %d %d 1 h %d {%s}\n",idx+1,data->config.splitPos+1,idx+1,data->w,data->config.scrollSpeed,value);
         send_request(data->sock,lcdProcCmd);
      }
   }
   oapc_tcp_recv(data->sock,lcdProcCmd,REC_BUF_SIZE-1,"\n",1000);

   return OAPC_OK;
}

