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
#endif

#ifdef ENV_LINUX
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <time.h>
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

#ifdef ENV_WINDOWSCE
#include <winsock2.h>
#endif

#include "oapc_libio.h"
#include "liboapc.h"

#define MAX_XML_SIZE    1500
#define MAX_TXT_SIZE     250


struct libio_config
{
   unsigned short version,length;
   int            numRange;
};



struct instData
{
   struct libio_config   config;
   int                   m_callbackID;
   unsigned char         m_digi;
   double                m_num;
};


static char xmltempl[]="<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"?>\n\
<oapc-config>\n\
<flowimage>%s</flowimage>\n\
<dialogue>\n\
 <general>\n\
  <param>\n\
   <name>numrange</name>\n\
   <text>Numeric Range</text>\n\
   <type>integer</type>\n\
   <default>%d</default>\n\
   <min>2</min>\n\
   <max>10000</max>\n\
  </param>\n\
 </general>\n\
 <helppanel>\n\
  <in0>CLK - generate random digital value</in0>\n\
  <in1>CLK - generate random numeric value</in1>\n\
  <out0>RND - random digital value</out0>\n\
  <out1>RND - random numeric value</out1>\n\
 </helppanel>\n\
</dialogue>\n\
</oapc-config>\n";

static char                 flowImage[]="iVBORw0KGgoAAAANSUhEUgAAAGoAAAAyCAMAAACDIr4/AAAAA3NCSVQICAjb4U/gAAAAFVBMVEUEAgT8/vyCgob////Bw8cAAAAE/vzB5Y3sAAAACXBIWXMAAA7EAAAOxAGVKw4bAAABI0lEQVRYhe2YyxbCMAhEEyn5/082Kc8sPWai1eJC6SL3DENppZSNUWstDyJ6jF9zQuuin3uijtKTcggqJQDUUGKicoJAdSUmKicIVFfiolICQR0hKiUQVOWawhIEauGZ34LCc27U+6BLopiYx1cPks+ZIVADIkinkhERqPNoYyoOggpJgqVUwaVecZzsFcR45UKEqCiIV1YvKR/PTq2+r1hbQkHaIgCUVWvWg/GKXA6rWdqTy1EqSzvd7l6YquhxE4fySjsvpIFQKobNoDSdlqO0C6zl5QrIK46j2a54LC8g5XYHTov09PDnI8grn7h5xoM60AZTyIoKXvQ15i9Rv/ki/ZG/crEsmRMEat7GtNZ2bWM6ats2pu3bxrR7G/MCals8AQSqOGkaTMsoAAAAAElFTkSuQmCC";
static char                 libname[]="Random Generator";
static char                 xmldescr[MAX_XML_SIZE+1];
static lib_oapc_io_callback m_oapc_io_callback;
static struct libio_config  save_config;



OAPC_EXT_API char *oapc_get_name(void)
{
   return libname;
}



OAPC_EXT_API unsigned long oapc_get_capabilities(void)
{
   return OAPC_HAS_INPUTS|OAPC_HAS_OUTPUTS|
          OAPC_HAS_XML_CONFIGURATION|
          OAPC_ACCEPTS_PLAIN_CONFIGURATION|OAPC_ACCEPTS_IO_CALLBACK|
          OAPC_FLOWCAT_DATA;
}



OAPC_EXT_API unsigned long oapc_get_input_flags(void)
{
   return OAPC_DIGI_IO0|OAPC_DIGI_IO1;
}



OAPC_EXT_API unsigned long oapc_get_output_flags(void)
{
   return OAPC_DIGI_IO0|OAPC_NUM_IO1;
}



OAPC_EXT_API char *oapc_get_config_data(void* instanceData)
{
   struct instData *data;

   data=(struct instData*)instanceData;

   sprintf(xmldescr,xmltempl,flowImage,data->config.numRange);
#ifdef _DEBUG
   long l=strlen(xmldescr);
   l=l;
   assert(strlen(xmldescr)<MAX_XML_SIZE);
#endif
   return xmldescr;
}



OAPC_EXT_API void oapc_set_config_data(void* instanceData,const char *name,const char *value)
{
   struct instData *data;

   data=(struct instData*)instanceData;

   if (strcmp(name,"numrange")==0) data->config.numRange=atoi(value);
}



OAPC_EXT_API char *oapc_get_save_data(void* instanceData,unsigned long *length)
{
   struct instData *data;

   data=(struct instData*)instanceData;

   *length=sizeof(struct libio_config);
   save_config.version =htons(1);
   save_config.length  =htons((unsigned short)*length);
   save_config.numRange=htonl(data->config.numRange);

   return (char*)&save_config;
}



OAPC_EXT_API void oapc_set_loaded_data(void* instanceData,unsigned long length,char *loadedData)
{
   struct instData *data;

   data=(struct instData*)instanceData;

   if (length>sizeof(struct libio_config)) length=sizeof(struct libio_config);
   memcpy(&save_config,loadedData,length);
   data->config.version =ntohs(save_config.version);
   data->config.length  =ntohs(save_config.length);
   if ((data->config.version!=1) || (data->config.length!=sizeof(struct libio_config)))
   {
      // do conversion from earlier versions here
   }
   data->config.numRange=ntohl(save_config.numRange);
}



OAPC_EXT_API void* oapc_create_instance2(unsigned long /*flags*/)
{
   struct instData *data;

   data=(struct instData*)malloc(sizeof(struct instData));
   if (!data) return NULL;
   memset(data,0,sizeof(struct instData));

   data->config.numRange=500; // set default values

   return data;
}



OAPC_EXT_API void oapc_delete_instance(void* instanceData)
{
   if (instanceData) free(instanceData);
}



OAPC_EXT_API unsigned long oapc_init(void* instanceData)
{
#ifndef ENV_WINDOWSCE
   srand((unsigned int)time(NULL));
#else
   srand(GetTickCount());
#endif
   return OAPC_OK;
}



OAPC_EXT_API unsigned long oapc_exit(void* instanceData)
{
/*   struct instData *data;

   data=(struct instData*)instanceData; // to be used when releasing device resources */
   return OAPC_OK;
}



OAPC_EXT_API void oapc_set_io_callback(void* instanceData,lib_oapc_io_callback oapc_io_callback,unsigned long callbackID)
{
   struct instData *data;

   data=(struct instData*)instanceData;

   m_oapc_io_callback=oapc_io_callback;
   data->m_callbackID=callbackID;
}



OAPC_EXT_API unsigned long  oapc_set_digi_value(void* instanceData,unsigned long input,unsigned char value)
{
   struct instData           *data;

   data=(struct instData*)instanceData;   
   if (value!=1) return OAPC_OK; // react only on high signals
   if (input==0)
   {
      if (rand()>RAND_MAX/2) data->m_digi=1;
      else data->m_digi=0;
      m_oapc_io_callback(OAPC_DIGI_IO0,data->m_callbackID);
   }
   else if (input==1)
   {
      double factor;

      factor=(1.0*data->config.numRange)/RAND_MAX;
      data->m_num=(int)(rand()*factor);
      m_oapc_io_callback(OAPC_NUM_IO1,data->m_callbackID);
   }
   else return OAPC_ERROR_NO_SUCH_IO;   
   return OAPC_OK;
}



OAPC_EXT_API unsigned long  oapc_get_digi_value(void* instanceData,unsigned long output,unsigned char *value)
{
   struct instData *data;

   data=(struct instData*)instanceData;
   if (output==0) *value=data->m_digi;
   else return OAPC_ERROR_NO_SUCH_IO;   
   return OAPC_OK;
}



OAPC_EXT_API unsigned long  oapc_get_num_value(void* instanceData,unsigned long output,double *value)
{
   struct instData *data;

   data=(struct instData*)instanceData;
   if (output==1) *value=data->m_num;
   else return OAPC_ERROR_NO_SUCH_IO;   
   return OAPC_OK;
}



