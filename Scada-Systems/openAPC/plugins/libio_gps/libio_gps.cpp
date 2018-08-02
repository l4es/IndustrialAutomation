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
   unsigned char         m_signalValid;
   double                m_lat,m_lon,m_height,m_speed;
   unsigned int          m_timestamp;
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
 <helppanel>\n\
  <out0>OK - position signal valid</out0>\n\
  <out1>LAT - Degree of latitude</out1>\n\
  <out2>LON - Degree of longitude</out2>\n\
  <out3>HGT - Height</out3>\n\
  <out4>SPD - Speed</out4>\n""\
  <out5>TIME - time (UTC)</out5>\n\
 </helppanel>\n\
</dialogue>\n\
</oapc-config>\n";

#define MAX_XML_SIZE 5500

/** Base64-encoded PNG data of an image that is used as icon for the program flow diagram within the main application*/
static char                 flowImage[]="iVBORw0KGgoAAAANSUhEUgAAAGoAAAAyCAMAAACDIr4/AAADAFBMVEUEAgSEgoQiNIFBSTkE/vxUYzlQXHwhKEmopKAnQadESGLFw8Xo6OgeL204R4drZ12MiqSmloolO5c8XrRXW2LV1NWsqrw2O05+c2t8eoEiJjIsP4BcQlRselQvOnC8trH8/vw8ZtRcisxUcKhPW0dEOzZ5ak4uTqxQZJ45UJmIh5RcerQUFiRtVUt0WmHR0Mw8RlCZnaxYZE6/u79MeuRXVYcsUsAeLFY/R3BSWHKmnJdoZm6trK98eY/c3N2ch3wAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAyHMX5AAAChElEQVR42u2Ye3OiMBDAAbVWs/jgzPG6wvYqPnqnVHMWqjR+/291kau1UKE6gPePOyNkZ0h+m91NNlGSLih4Ibmirqgr6or6PyhwXbgECjgQn4OQqlEckO7ePv57l4GKO70VhffawMXwhiGLFhHzW9NSUDHhwzOWtSke0ZqQ6PaxfwO+DGWhMI3a3AIuht4sfAlf5m2CwEgpqF2vgwNj93Gf94KXwbA9Gti2/orIRrRMB77rABQXndGquxyMnjqqEzwiJVAJinMERzE229HdjaIooWpx9KG0DNw3dw1KsafYr9277nJqO0oYNE2kvIp1BeAS255Ouz//PIw1RVHDpk781EouB8V9L1Lt6f3g18O3SUd4UG9a0YxWgfIpA0UbP99PNuNWx1ZUx7KAAK0AtWUIqtZpjZ/H05uu6ai65Ymk7FeAYhx5R7M1raVphtEdtXVdB85YJWmBfOHMJiJKq0ajUWvPmnJ6WZW1s1Pk/dWPiWPb4XI4rIWeLuIElaA4stmrKiblqOHcq1mWyc9HJW4QUmZfjgQiVUgQPHnWb9nowSkOlDLVT1zpEC2KhLWDoGl5lu7636Hvn4JKXIWkLCOSn9EIRE1k5sY02a4OQ5RRr+KicMKAObIm1KXidCE23t0vp+CfNFxc57Mcjf52zoix9Tlzc44xKZ+lFDwaHukTCygVxZevP4UpJy0kPD46pg06Z6Xsv070ypxVGadb6bRopZM9O25noLIUKeln6QxvHnNg0vZMJTGt02d1uZtI4s+Swko+anfUlvfd5Hq9XkTJRZGY93byJZLoVkT5IgNlPNgkTCym5KLEtA42CROLKV8k+0ebCiu5KPLRpsJK9lZzIfkLPhY9rAn0SHsAAAAASUVORK5CYII=";
static char                 libname[]="GPS (NMEA/SIRF binary)";
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
   return OAPC_HAS_OUTPUTS|
          OAPC_HAS_XML_CONFIGURATION|
          OAPC_ACCEPTS_PLAIN_CONFIGURATION|
          OAPC_ACCEPTS_IO_CALLBACK|
          OAPC_FLOWCAT_IO;
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
   return OAPC_DIGI_IO0|
          OAPC_NUM_IO1|OAPC_NUM_IO2|OAPC_NUM_IO3|OAPC_NUM_IO4|OAPC_NUM_IO5;
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
                             data->config.serialParams.stopbits,data->config.serialParams.flowcontrol
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

   data=(struct instData*)malloc(sizeof(struct instData));
   if (!data) return NULL;
   memset(data,0,sizeof(struct instData));

   data->config.version=1;
   data->config.length=sizeof(struct libio_config);
   
#ifdef ENV_WINDOWS
   strcpy(data->config.serialParams.port,"COM1");
 #else
  #ifdef ENV_LINUX
   strcpy(data->config.serialParams.port,"/dev/ttyUSB0");
  #else
   #ifdef ENV_QNX
    strcpy(data->config.serialParams.port,"/dev/ser1");
   #else
    #error Not implemented!
   #endif
 #endif
#endif

   data->config.serialParams.brate=5;
   data->config.serialParams.databits=4;
   data->config.serialParams.flowcontrol=1;
   data->config.serialParams.parity=1;
   data->config.serialParams.stopbits=1;
   data->m_callbackID=0;
   data->m_running=true;
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



void* NMEALoop( void*  arg )
{
	#define MAX_BUFLEN 100
	
	struct instData *data;
   unsigned char    lastSignalValid=5;
   double           lastLat=-1000,lastLon=-1000,lastHeight=-10000,lastSpeed=-1;
   unsigned int     lastTimestamp=0;
   int              i,h,m,s;
   char             buffer[MAX_BUFLEN+1];
   unsigned char    sum;
   double           f;
//   char            *c;
	
	data=(struct instData*)arg;

	while (data->m_running)
	{
		if (oapc_serial_recv(data->m_hCommPort,&buffer[0],2,NULL,1000)==2)
		{
			if ((buffer[0]=='$') && (buffer[1]=='G') && 
			    (oapc_serial_recv(data->m_hCommPort,&buffer[2],MAX_BUFLEN,"\r\n",1000)>0) && (strstr(buffer,"\r\n")))
			{
				//NMEA-Protocol
				sum=0;
				for (i=1; i<(int)(strlen(buffer)-7); i++)
				 sum^=buffer[i];
				if (sum==strtol((strstr(buffer,"*")+1), (char **)NULL,16))
				{
					if (strstr(buffer,"$GPRMC")==buffer)
					{
			   		// $GPRMC,162614,A,5230.5900,N,01322.3900,E,10.0,90.0,131006,1.2,E,A*13
			         // $GPRMC,HHMMSS,A,BBBB.BBBB,b,LLLLL.LLLL,l,GG.G,RR.R,DDMMYY,M.M,m,F*PP
                  i=atoi(&buffer[7]);
			         h=i/10000;         data->m_timestamp=h*60*60;
			         m=(i-h*10000)/100; data->m_timestamp+=(m*60);
			         s=i-h*10000-m*100; data->m_timestamp+=s;
            
                  f=oapc_util_atof(&buffer[20]);
                  data->m_lat=((int)(f/100.0))+ (f-((int)(f/100.0))*100.0)/60.0;
                  if (buffer[31]=='S') data->m_lat*=-1;

                  f=oapc_util_atof(&buffer[32]);
                  data->m_lon=((int)(f/100.0))+ (f-((int)(f/100.0))*100.0)/60.0;
                  if (buffer[45]=='W') data->m_lon*=-1;
            
                  data->m_speed=oapc_util_atof(&buffer[46]);
/*               c=strstr(buffer,"*"); do not use for valid signal, seems to be incompatible for some devices
               if (c)
               {
                  c--;
                  if (*c=='N') data->m_signalValid=0;
                  else data->m_signalValid=1;
               }*/
               }
   		   	else if (strstr(buffer,"$GPGGA")==buffer)
               {
				   // $GPGGA,HHMMSS.ss,BBBB.BBBB,b,LLLLL.LLLL,l,Q,NN,D.D,H.H,h,G.G,g,A.A,RRRR*PP
                  i=atoi(&buffer[7]);
			         h=i/10000;         data->m_timestamp=h*60*60;
			         m=(i-h*10000)/100; data->m_timestamp+=(m*60);
			         s=i-h*10000-m*100; data->m_timestamp+=s;

                  f=oapc_util_atof(&buffer[18]);
                  data->m_lat=((int)(f/100.0))+ (f-((int)(f/100.0))*100.0)/60.0;
			         if (buffer[27]=='S') data->m_lat*=-1;
               
                  f=oapc_util_atof(&buffer[30]);
                  data->m_lon=((int)(f/100.0))+ (f-((int)(f/100.0))*100.0)/60.0;
                  if (buffer[41]=='W') data->m_lon*=-1;

                  data->m_height=oapc_util_atof(&buffer[52]);

                  if (buffer[43]!='1') data->m_signalValid=0;
                  else data->m_signalValid=1;
               }
				}
				else printf("NMEA checksum error\n");
			}
			else if (((unsigned char)buffer[0]==0xA0) && ((unsigned char)buffer[1]==0xA2)) //SIRF binary protocol
			{
				int sirfLength;
				
		      if (oapc_serial_recv(data->m_hCommPort,&buffer[2],2,NULL,250)==2) // length
		      {
		      	sirfLength=buffer[2]<<8;
		      	sirfLength|=buffer[3];
		      	
   		      if (oapc_serial_recv(data->m_hCommPort,&buffer[4],sirfLength,NULL,250)==sirfLength) // payload
	   	      {
      		      if (oapc_serial_recv(data->m_hCommPort,&buffer[4+sirfLength],4,NULL,250)==4) // checksum and end sequence
      		      {
      		      	if (buffer[4]==41) // Geodetic Navigation Data Out
      		      	{
                        int bits;

                        if ((buffer[7] & 0x03)==0) data->m_signalValid=0;
      		      		else data->m_signalValid=1;
                        
                        data->m_timestamp=buffer[19]*60*60;
                        data->m_timestamp+=buffer[20]*60;
                        data->m_timestamp+=((buffer[21]<<8)|buffer[22])/1000;

                        bits=(((unsigned char)buffer[27]<<24)|((unsigned char)buffer[28]<<16)|
                              ((unsigned char)buffer[29]<<8)|  (unsigned char)buffer[30]);
      		      		data->m_lat=bits/10000000.0;
                        bits=(((unsigned char)buffer[31]<<24)|((unsigned char)buffer[32]<<16)|
                              ((unsigned char)buffer[33]<<8)|  (unsigned char)buffer[34]);
      		      		data->m_lon=bits/10000000.0;
                        bits=(((unsigned char)buffer[35]<<24)|((unsigned char)buffer[36]<<16)|
                              ((unsigned char)buffer[37]<<8)|  (unsigned char)buffer[38]);
      		      		data->m_height=bits/100.0;
                        bits=((unsigned char)buffer[44]<<8)|(unsigned char)buffer[45];
                        data->m_speed=bits/100.0;
      		      	}
      		      }
	   	      }      	
		      }
			}			
	
	      if (data->m_signalValid)
	      {
	         if (data->m_lat!=lastLat)
	         {
	            m_oapc_io_callback(OAPC_NUM_IO1,data->m_callbackID);
	            lastLat=data->m_lat;
	         }
	         if (data->m_lon!=lastLon)
	         {
	            m_oapc_io_callback(OAPC_NUM_IO2,data->m_callbackID);
	            lastLon=data->m_lon;
	         }
	         if (data->m_height!=lastHeight)
	         {
	            m_oapc_io_callback(OAPC_NUM_IO3,data->m_callbackID);
	            lastHeight=data->m_height;
	         }
	         if (data->m_speed!=lastSpeed)
	         {
	            m_oapc_io_callback(OAPC_NUM_IO4,data->m_callbackID);
	            lastSpeed=data->m_speed;
	         }
	         if (data->m_timestamp!=lastTimestamp)
	         {
	            m_oapc_io_callback(OAPC_NUM_IO5,data->m_callbackID);
	            lastTimestamp=data->m_timestamp;
	         }
	      }
	      if (data->m_signalValid!=lastSignalValid)
	      {
	         m_oapc_io_callback(OAPC_DIGI_IO0,data->m_callbackID);
	         lastSignalValid=data->m_signalValid;
	      }
		}
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
   
   if (!oapc_thread_create(NMEALoop,data))
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

   if (output!=0) return OAPC_ERROR_NO_SUCH_IO;
   data=(struct instData*)instanceData;

   *value=data->m_signalValid;
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

   
   data=(struct instData*)instanceData;
   
   if (output==1) *value=data->m_lat;
   else if (output==2) *value=data->m_lon;
   else if (output==3) *value=data->m_height;
   else if (output==4) *value=data->m_speed;
   else if (output==5) *value=data->m_timestamp;
   else return OAPC_ERROR_NO_SUCH_IO;      
   return OAPC_OK;
}


