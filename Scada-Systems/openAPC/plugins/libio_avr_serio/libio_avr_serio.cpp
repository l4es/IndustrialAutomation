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
#include <math.h>
#include <ctype.h>
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
 #include <stdio_ext.h>
#endif

#ifdef ENV_QNX
 #include <errno.h>
 #include <unistd.h>
 #include <fcntl.h>
 #include <arpa/inet.h>
 #include <termios.h>
 #include <unistd.h>
 #include <devctl.h>
 #include <sys/types.h>
 #include <sys/dcmd_chr.h>
#endif

#ifdef ENV_WINDOWS
#include <windows.h>
#endif

#ifdef ENV_WINDOWSCE
#include <winsock2.h>
#endif

#include "oapc_libio.h"
#include "liboapc.h"

#define MAX_CMD_LEN      63



struct libio_config
{
   unsigned short       version,length;
   struct serial_params serialParams;
   int                  pollDelay;
};



struct instData
{
   struct libio_config   config;
   unsigned char         DO[8];                     // Digital outputs
   unsigned char         DI[4];                     // Digital input data
   int                   AI[4];                     // Analogue input data
#ifdef ENV_WINDOWS
   HANDLE                m_hCommPort;
#else
   int                   m_hCommPort;
#endif
   int                   m_callbackID;
   bool                  m_running;
};



/** the xml data that define the behaviour and user interface of the plugin within the main application */
static char xmltempl[]="<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"?>\n\
<oapc-config>\n\
<flowimage>%s</flowimage>\n\
<dialogue>\n\
 <general>\n\
  <param>\n\
   <name>port</name>\n\
   <text>Interface</text>\n\
   <type>string</type>\n\
   <default>%s</default>\n\
   <min>4</min>\n\
   <max>12</max>\n\
  </param>\n\
  <param>\n\
   <name>brate</name>\n\
   <text>Data Rate</text>\n\
   <unit>bps</unit>\
   <type>option</type>\n\
   <value>110</value>\n\
   <value>300</value>\n\
   <value>1200</value>\n\
   <value>2400</value>\n\
   <value>4800</value>\n\
   <value>9600</value>\n\
   <value>19200</value>\n\
   <value>38400</value>\n\
   <value>57600</value>\n\
   <value>115200</value>\n\
   <value>230400</value>\n\
   <default>%d</default>\n\
  </param>\n\
  <param>\n\
   <name>databits</name>\n\
   <text>Data Bits</text>\n\
   <type>option</type>\n\
   <value>5</value>\n\
   <value>6</value>\n\
   <value>7</value>\n\
   <value>8</value>\n\
   <default>%d</default>\n\
  </param>\n\
  <param>\n\
   <name>parity</name>\n\
   <text>Parity</text>\n\
   <type>option</type>\n\
   <value>None</value>\n\
   <value>Even</value>\n\
   <value>Odd</value>\n\
   <default>%d</default>\n\
  </param>\n\
  <param>\n\
   <name>stopbits</name>\n\
   <text>Stop Bits</text>\n\
   <type>option</type>\n\
   <value>1</value>\n\
   <value>1.5</value>\n\
   <value>2</value>\n\
   <default>%d</default>\n\
  </param>\n\
  <param>\n\
   <name>flowcontrol</name>\n\
   <text>Flow Control</text>\n\
   <type>option</type>\n\
   <value>None</value>\n\
   <value>Xon / Xoff</value>\n\
   <value>CTS / RTS</value>\n\
   <default>%d</default>\n\
  </param>\n\
 </general>\n\
 <stdpanel>\n\
  <name>Configuration</name>\n\
  <param>\n\
   <name>polldelay</name>\n\
   <text>Poll Delay</text>\n\
   <type>integer</type>\n\
   <default>%d</default>\n\
   <min>25</min>\n\
   <max>1000</max>\n\
   <unit>msec</unit>\n\
  </param>\n\
 </stdpanel>\n\
 <helppanel>\n\
  <in0>SET - digital output 1</in0>\n\
  <in1>SET - digital output 2</in1>\n\
  <in2>SET - digital output 3</in2>\n\
  <in3>...</in3>\n\
  <out0>GET - digital input 1</out0>\n\
  <out1>GET - digital input 2</out1>\n\
  <out2>GET - digital input 3</out2>\n\
  <out3>GET - digital input 4</out3>\n\
  <out4>GET - analogue input 1</out4>\n""\
  <out5>GET - analogue input 2</out5>\n\
  <out6>GET - analogue input 3</out6>\n\
  <out7>GET - analogue input 4</out7>\n\
 </helppanel>\n\
</dialogue>\n\
</oapc-config>\n";

#define MAX_XML_SIZE 5500

/** Base64-encoded PNG data of an image that is used as icon for the program flow diagram within the main application*/
static char                 flowImage[]="iVBORw0KGgoAAAANSUhEUgAAAGoAAAAyCAMAAACDIr4/AAADAFBMVEUEAgQwilgwSDUE/vxJZ3Ekaz/ELiQfJRtXirGHTTkyPEKgxq5UJiDu6+dFSEeflYksNCGwxti0raZ0aFBUonSCgoQyWDpWVklwbmi818BYaFRJRTdus9woKyf8/vxkHhTEwsRGcIKpo5Vvm7RwinDATkRXmIIUVpRFOR/Z29dqob95d1lGnHA8dVQVGRTchnxsuoxcYkB7em+saFA/PTIxNS1ZW1XBwrS7ua9clpyCu9xcmmxHjWd4WkRHfYSkoqQAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAACn7ivrAAACYUlEQVR42u2Yj3PaIBTHabTDxhGzWI2K0KhrZ6xhawrROe3+//9qxJhInN5kBne37Z3n8T0e7/P4kRAA4IpGKQUd+ddJSwZFhgq2xSCrNCUyVMrO0zAmdijJztMwJnYoyS7SMCVyVLBPw5TIUVRJw5AoUOYtX4FAZQI1LaCWVbcDr1NuRYBiWZQ6rIQvCVounnIDR92ODSCg5dyVGKp3KgA9VgUOw/2hufpXUasPlIYhMo9CKGg0ve+95QIZRvWRtNhxSIK9sB+aQy3x+Hkctl2Ls837lu8vTKFQh/EB5NPp9HkQxw9rC3pHvAJQvFTLWgOFEiaEcF9c7GO3y/Dz62Nr+flnUGpBjlK0BspnmAAoRLJJelwI/HL3brN5DFFwQNqGCjKUqjVQsQMJFg5hFrNczLGL44/Ejywcq0GCHTFDqVoHlTAChQMhIVO8SQTnjDBIIONNtO/UHinLJX0+qv/tNhowKIPLGRO98esdSw0yYvmLJlVXQlbKfvSw5pxe9ec36+7brG7jlCWmDxZhQJKEWPYrRknY/c3X1bBRd+wtzIoiizuJ8FCMKh3ArTWf3mar4aTuOCkL3kbCsT3U6lW8LArYZDicjWxbLoiIE9mncbvqxb6HjSbD2WxEmGsJMVZJVT3CJVhj9UmOn3BbgddGlb+YDobR5t2uP1gse8jUflXA1t2neWT91q6lveE35/P1PaLXQMnn7AuiV0L9/zj7Jeq844Fum2MnkXOPB5ptTpxEzjke6La5+lztL0sMiktuY2q1mo645DZGxtARF93G1LTERbcxNS3x197GXM1+AHViDRc1E0z5AAAAAElFTkSuQmCC";
static char                 libname[]="AVR Net-IO Serial Interface";
static char                 xmldescr[MAX_XML_SIZE+1];
static lib_oapc_io_callback m_oapc_io_callback; // callback function that is used to inform the main function about changes at the IO ports
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
   return OAPC_HAS_INPUTS|OAPC_HAS_OUTPUTS|
          OAPC_HAS_XML_CONFIGURATION|
          OAPC_ACCEPTS_PLAIN_CONFIGURATION|
          OAPC_ACCEPTS_IO_CALLBACK|
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
   return OAPC_DIGI_IO0|OAPC_DIGI_IO1|OAPC_DIGI_IO2|OAPC_DIGI_IO3|
          OAPC_DIGI_IO4|OAPC_DIGI_IO5|OAPC_DIGI_IO6|OAPC_DIGI_IO7;
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
   return OAPC_DIGI_IO0|OAPC_DIGI_IO1|OAPC_DIGI_IO2|OAPC_DIGI_IO3|
          OAPC_NUM_IO4| OAPC_NUM_IO5| OAPC_NUM_IO6| OAPC_NUM_IO7;
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
                             data->config.pollDelay
                             );
#ifdef _DEBUG
   assert(strlen(xmldescr)<MAX_XML_SIZE);
#endif
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
   else if (strcmp(name,"polldelay")==0)   data->config.pollDelay=(unsigned short)atoi(value);
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
   save_config.version    =htons(data->config.version);
   save_config.length     =htons(data->config.length);
   strncpy(save_config.serialParams.port,data->config.serialParams.port,MAX_TTY_SIZE);
   save_config.serialParams.brate      =htons(data->config.serialParams.brate);
   save_config.serialParams.parity     =htons(data->config.serialParams.parity);
   save_config.serialParams.databits   =htons(data->config.serialParams.databits);
   save_config.serialParams.flowcontrol=htons(data->config.serialParams.flowcontrol);
   save_config.serialParams.stopbits   =htons(data->config.serialParams.stopbits);
   save_config.pollDelay               =htonl(data->config.pollDelay);
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
   data->config.pollDelay               =ntohl(save_config.pollDelay);
}



/**
When the capability flag OAPC_ACCEPTS_IO_CALLBACK is set, the main application no longer cyclically polls
the outputs of a Plug-In and the related parameter within the flow configuration dialogue is turned off.
Instead of this the main application hands over a function pointer to a callback and an ID. Whenever something
changes within the scope of this Plug-In that influences the output state of it, the Plug-In jumps into
that callback function to notify the main application about the new output state. The callback function 
"oapc_io_callback" expects two parameters. The first one "outputs" expects the Or-concatenated flags of
the outputs that have changed and the second one "callbackID" expects the ID that is handed over here to
identify the Plug-In. For a typedef of the callback function oapc_io_callback() that is called by the Plug-In
please refer to oapc_libio.h.<BR><BR>
Here the main application hands over a pointer to the callback function and a unique callback ID. Both have
to be stored for later use
@param[in] oapc_io_callback the callback function that has to be called whenever something changes at the
           outputs of this Plug-In
@param[in] callbackID a unique ID that identifies this Plug-In and that has to be used when the function
           oapc_io_callback is called
*/
OAPC_EXT_API void oapc_set_io_callback(void* instanceData,lib_oapc_io_callback oapc_io_callback,unsigned long callbackID)
{
   struct instData *data;

   data=(struct instData*)instanceData;

   m_oapc_io_callback=oapc_io_callback;
   data->m_callbackID=callbackID;
}



/**
When this function is called everything has to be initialized in order to perform the required operation
@return a return value/error code that informs the main application if the initialization was done successfully
        or not
*/
OAPC_EXT_API void* oapc_create_instance2(unsigned long flags)
{
   flags=flags; // removing "unused" warning

   struct instData *data;
   int              i;

   data=(struct instData*)malloc(sizeof(struct instData));
   if (!data) return NULL;
   memset(data,0,sizeof(struct instData));

   data->config.version=1;
   data->config.length=sizeof(struct libio_config);
   
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
   data->config.serialParams.parity=1;
   data->config.serialParams.stopbits=1;
   data->config.pollDelay=250;
   data->m_callbackID=0;
   data->m_running=true;
   for (i=0; i<8; i++) data->DO[i]=2;
   for (i=0; i<4; i++)
   {
   	data->DI[i]=2;
   	data->AI[i]=1000000000;
   }
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



void* AVRLoop( void*  arg )
{
	#define MAX_BUFLEN 100
	
	struct instData *data;
   int              i;
   char             lastDO[8]={2,2,2,2,2,2,2,2},lastDI[4]={2,2,2,2};
   int              lastAI[4]={1000000000,1000000000,1000000000,1000000000};
   char             sendBuf[MAX_BUFLEN+1];
	
	data=(struct instData*)arg;


	while (data->m_running)
	{
		for (i=0; i<8; i++) if (lastDO[i]!=data->DO[i])
		{
			sprintf(sendBuf,"SETPORT %d.%d\r\n",i+1,data->DO[i]);
			oapc_serial_send(data->m_hCommPort,sendBuf,(int)strlen(sendBuf),250);
#ifdef ENV_WINDOWS
         FlushFileBuffers(data->m_hCommPort);
#else
         tcflush(data->m_hCommPort, TCIOFLUSH );
#endif
			if ((oapc_serial_recv(data->m_hCommPort,sendBuf,MAX_BUFLEN,"\n",250)>0) &&
				 (strstr(sendBuf,"\n")))
		    lastDO[i]=data->DO[i];
		}
      for (i=0; i<4; i++)
      {
  			sprintf(sendBuf,"GETPORT %d\r\n",i+1);
 			oapc_serial_send(data->m_hCommPort,sendBuf,(int)strlen(sendBuf),250);
#ifdef ENV_WINDOWS
         FlushFileBuffers(data->m_hCommPort);
#else
         tcflush(data->m_hCommPort, TCIOFLUSH );
#endif
  			if ((oapc_serial_recv(data->m_hCommPort,sendBuf,MAX_BUFLEN,"\n",250)>0) &&
       		 (strstr(sendBuf,"\n")))
         {
         	data->DI[i]=(unsigned char)atoi(sendBuf);
	         if (lastDI[i]!=data->DI[i])
	         {
	         	if (i==0) m_oapc_io_callback(OAPC_DIGI_IO0,data->m_callbackID);
	         	else if (i==1) m_oapc_io_callback(OAPC_DIGI_IO1,data->m_callbackID);
	         	else if (i==2) m_oapc_io_callback(OAPC_DIGI_IO2,data->m_callbackID);
	         	else if (i==3) m_oapc_io_callback(OAPC_DIGI_IO3,data->m_callbackID);
	         }
        	}
  			sprintf(sendBuf,"GETADC %d\r\n",i+1);
 			oapc_serial_send(data->m_hCommPort,sendBuf,(int)strlen(sendBuf),250);
#ifdef ENV_WINDOWS
         FlushFileBuffers(data->m_hCommPort);
#else
         tcflush(data->m_hCommPort, TCIOFLUSH );
#endif
  			if ((oapc_serial_recv(data->m_hCommPort,sendBuf,MAX_BUFLEN,"\n",250)>0) &&
       		 (strstr(sendBuf,"\n")))
         {
         	data->AI[i]=atoi(sendBuf);
	         if (lastAI[i]!=data->AI[i])
	         {
	         	if (i==0) m_oapc_io_callback(OAPC_NUM_IO4,data->m_callbackID);
	         	else if (i==1) m_oapc_io_callback(OAPC_NUM_IO5,data->m_callbackID);
	         	else if (i==2) m_oapc_io_callback(OAPC_NUM_IO6,data->m_callbackID);
	         	else if (i==3) m_oapc_io_callback(OAPC_NUM_IO7,data->m_callbackID);
               lastAI[i]=data->AI[i];
	         }
         }
      }
      oapc_thread_sleep(data->config.pollDelay);
	}
	data->m_running=true;
	return NULL;
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
   	printf("Opening serial interface failed\n");
   	oapc_exit(instanceData);
   	return ret;
   }
   
   if (!oapc_thread_create(AVRLoop,data))
   {
   	printf("create thread failed\n");
      oapc_exit(instanceData);
      return OAPC_ERROR_NO_MEMORY;
   }
   return OAPC_OK;
}



/**
This function is called before the application unloads everything, it has to be used to deinitialize
everything and to release used resources.
*/
OAPC_EXT_API unsigned long oapc_exit(void* instanceData)
{
   int              ctr=0;
   struct instData *data;

   data=(struct instData*)instanceData;

   data->m_running=false;
   while ((!data->m_running) && (ctr<10)) // wait for thread to finish
   {
      oapc_thread_sleep(250);
      ctr++;
   }
   oapc_serial_port_close(&data->m_hCommPort);
#ifdef ENV_WINDOWS
   data->m_hCommPort=NULL;
#else
   data->m_hCommPort=0;
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
   data->DO[input]=value;
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
   struct instData *data;

   if (output>3) return OAPC_ERROR_NO_SUCH_IO;
   data=(struct instData*)instanceData;

   *value=data->DI[output];
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
OAPC_EXT_API unsigned long  oapc_get_num_value(void* instanceData,unsigned long output,double* value)
{
   struct instData *data;

   if (output<4) return OAPC_ERROR_NO_SUCH_IO;
   data=(struct instData*)instanceData;

   *value=data->AI[output-4];      
   return OAPC_OK;
}


