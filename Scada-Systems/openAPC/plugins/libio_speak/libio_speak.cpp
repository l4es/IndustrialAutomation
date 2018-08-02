/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 3 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write see:                           *
 *               <http://www.gnu.org/licenses/>.                           *
 ***************************************************************************/




#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <assert.h>

#ifndef ENV_WINDOWS
#include <arpa/inet.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#define closesocket close
#else
#include <winsock2.h>
#define MSG_NOSIGNAL 0
#endif

#include "oapc_libio.h"
#include "speak_lib.h"

int DoSpeak(int pitch_adjustment, int speed,int wordgap,char *voicename,char *p_text);

#define MAX_XML_SIZE       8000



struct libio_config
{
   unsigned short version,length;
   int            pitch[2],speed[2],wordgap[2],language[2],gender[2];
};



struct instData
{
   struct libio_config  config;
};


//int DoSpeak(int pitch_adjustment, int speed,int wordgap,char *voicename,char *p_text)

/** the xml data that define the behaviour and user interface of the plugin within the main application */
static char xmltempl[]="<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"?>\n\
<oapc-config>\n\
 <flowimage>%s</flowimage>\n\
 <dialogue>\n\
  <stdpanel>\n\
   <name>Voice 1</name>\n\
   <param>\n\
    <name>pitch1</name>\n\
    <text>Pitch</text>\n\
    <type>integer</type>\n\
    <default>%d</default>\n\
    <min>1</min>\n\
    <max>99</max>\n\
   </param>\n\
   <param>\n\
    <name>speed1</name>\n\
    <text>Speed</text>\n\
    <type>integer</type>\n\
    <default>%d</default>\n\
    <min>80</min>\n\
    <max>300</max>\n\
   </param>\n\
   <param>\n\
    <name>wordgap1</name>\n\
    <text>Word Gap</text>\n\
    <type>integer</type>\n\
    <default>%d</default>\n\
    <min>10</min>\n\
    <max>500</max>\n\
    <unit>msec</unit>\n\
   </param>\n\
   <param>\n\
    <name>gender1</name>\n\
    <text>Gender</text>\n\
    <type>option</type>\n\
    <value>neutral</value>\n\
    <value>female</value>\n\
    <value>male</value>\n\
    <default>%d</default>\n\
   </param>\n\
   <param>\n\
    <name>language1</name>\n\
    <text>Language</text>\n\
    <type>option</type>\n\
    <value>af</value>\n\
    <value>ca</value>\n\
    <value>cs</value>\n\
    <value>cy</value>\n\
    <value>da</value>\n\
    <value>de</value>\n\
    <value>el</value>\n\
    <value>en</value>\n\
    <value>eo</value>\n\
    <value>es</value>\n\
    <value>fi</value>\n\
    <value>fr</value>\n\
    <value>grc</value>\n\
    <value>hbs</value>\n\
    <value>hi</value>\n\
    <value>hu</value>\n\
    <value>hy</value>\n\
    <value>id</value>\n\
    <value>is</value>\n\
    <value>it</value>\n\
    <value>jbo</value>\n\
    <value>ku</value>\n\
    <value>la</value>\n\
    <value>lv</value>\n\
    <value>mk</value>\n\
    <value>nl</value>\n\
    <value>no</value>\n\
    <value>pap</value>\n\
    <value>pl</value>\n\
    <value>pt</value>\n\
    <value>ro</value>\n\
    <value>ru</value>\n\
    <value>sk</value>\n\
    <value>sq</value>\n\
    <value>sv</value>\n\
    <value>sw</value>\n\
    <value>ta</value>\n\
    <value>tr</value>\n\
    <value>vi</value>\n\
    <value>zhy</value>\n\
    <value>zh</value>\n\
    <default>%d</default>\n\
   </param>\n\
  </stdpanel>\n\
  <stdpanel>\n\
   <name>Voice 2</name>\n\
   <param>\n\
    <name>pitch2</name>\n\
    <text>Pitch</text>\n\
    <type>integer</type>\n\
    <default>%d</default>\n""\
    <min>1</min>\n\
    <max>99</max>\n\
   </param>\n\
   <param>\n\
    <name>speed2</name>\n\
    <text>Speed</text>\n\
    <type>integer</type>\n\
    <default>%d</default>\n\
    <min>80</min>\n\
    <max>300</max>\n\
   </param>\n\
   <param>\n\
    <name>wordgap2</name>\n\
    <text>Word Gap</text>\n\
    <type>integer</type>\n\
    <default>%d</default>\n\
    <min>10</min>\n\
    <max>500</max>\n\
    <unit>msec</unit>\n\
   </param>\n\
   <param>\n\
    <name>gender2</name>\n\
    <text>Gender</text>\n\
    <type>option</type>\n\
    <value>neutral</value>\n\
    <value>female</value>\n\
    <value>male</value>\n\
    <default>%d</default>\n\
   </param>\n\
   <param>\n\
    <name>language2</name>\n\
    <text>Language</text>\n\
    <type>option</type>\n\
    <value>af</value>\n\
    <value>ca</value>\n\
    <value>cs</value>\n\
    <value>cy</value>\n\
    <value>da</value>\n\
    <value>de</value>\n\
    <value>el</value>\n\
    <value>en</value>\n\
    <value>eo</value>\n\
    <value>es</value>\n\
    <value>fi</value>\n\
    <value>fr</value>\n\
    <value>grc</value>\n\
    <value>hbs</value>\n\
    <value>hi</value>\n\
    <value>hu</value>\n\
    <value>hy</value>\n\
    <value>id</value>\n\
    <value>is</value>\n\
    <value>it</value>\n\
    <value>jbo</value>\n\
    <value>ku</value>\n\
    <value>la</value>\n\
    <value>lv</value>\n\
    <value>mk</value>\n\
    <value>nl</value>\n\
    <value>no</value>\n\
    <value>pap</value>\n\
    <value>pl</value>\n\
    <value>pt</value>\n\
    <value>ro</value>\n\
    <value>ru</value>\n\
    <value>sk</value>\n\
    <value>sq</value>\n\
    <value>sv</value>\n\
    <value>sw</value>\n\
    <value>ta</value>\n\
    <value>tr</value>\n\
    <value>vi</value>\n\
    <value>zhy</value>\n\
    <value>zh</value>\n\
    <default>%d</default>\n\
   </param>\n\
  </stdpanel>\n\
  <helppanel>\n\
   <in0>text to speak</in0>\n\
   <in4>text to speak</in4>\n\
  </helppanel>\n\
 </dialogue>\n\
</oapc-config>\n";
/** Base64-encoded PNG data of an image that is used as icon for the program flow diagram within the main application*/
static char                langID[][4]={"af","ca","cs","cy","da","de","el","en","eo","es","fi","fr","grc","hbs","hi","hu",
                                     "hy","id","is","it","jbo","ku","la","lv","mk","nl","no","pap","pl","pt","ro","ru",
                                     "sk","sq","sv","sw","ta","tr","vi","zhy","zh"};
static char                flowImage[]="iVBORw0KGgoAAAANSUhEUgAAAGoAAAAyBAMAAABG0lM+AAAAMFBMVEUEAgSMjAiEgoTFw8T8AvzW2AT8/vxnZpBUUgSytM/7/QSnp5thXTbU1vSkpgTc3txrCyUsAAAACXBIWXMAAA7EAAAOxAGVKw4bAAAA0UlEQVR42mNgIAuwpTG5OLGlYQW4ZBKAugxcHMyw68IlA9KVoKKAXRNOGZCuZBZmHLpwyYB0pTHh8BZOGbAuAzNcunDIgHUl4NKESwasi2Qwqmuw68revY10XRWtqyLaSdVVsQoE2knTlR4F1rW0jCRd81ZBwEuSdHVBda2gva7sVTCwjQRdya+gmtaZ0d5fp6C61pCkKweq6xhJujIhTlwxjbQUld4K1BRRRmrqTS9c2l5Gek7JWUNO/poqS4auZJ5rZOjK/jZaso3qGuG6yAIA55v2B6D7j4oAAAAASUVORK5CYII=";
static char                libname[]="Speak";
static char                xmldescr[MAX_XML_SIZE+1];
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
   return OAPC_CHAR_IO0|OAPC_CHAR_IO4;
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

   sprintf(xmldescr,xmltempl,flowImage,data->config.pitch[0],data->config.speed[0],data->config.wordgap[0],data->config.gender[0],data->config.language[0],
                                       data->config.pitch[1],data->config.speed[1],data->config.wordgap[1],data->config.gender[1],data->config.language[1]);
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

   if (strcmp(name,"pitch1")==0)         data->config.pitch[0]=atoi(value);
   else if (strcmp(name,"speed1")==0)    data->config.speed[0]=atoi(value);
   else if (strcmp(name,"wordgap1")==0)  data->config.wordgap[0]=atoi(value);
   else if (strcmp(name,"gender1")==0)   data->config.gender[0]=atoi(value);
   else if (strcmp(name,"language1")==0) data->config.language[0]=atoi(value);
   else if (strcmp(name,"pitch2")==0)    data->config.pitch[1]=atoi(value);
   else if (strcmp(name,"speed2")==0)    data->config.speed[1]=atoi(value);
   else if (strcmp(name,"wordgap2")==0)  data->config.wordgap[1]=atoi(value);
   else if (strcmp(name,"language2")==0) data->config.language[1]=atoi(value);
   else if (strcmp(name,"gender2")==0)   data->config.gender[1]=atoi(value);
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
   save_config.version=htons(data->config.version);
   save_config.length =htons(data->config.length);
   for (i=0; i<2; i++)
   {
      save_config.pitch[i]   =htonl(data->config.pitch[i]);
      save_config.speed[i]   =htonl(data->config.speed[i]);
      save_config.wordgap[i] =htonl(data->config.wordgap[i]);
      save_config.gender[i]  =htonl(data->config.gender[i]);
      save_config.language[i]=htonl(data->config.language[i]);
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
   data->config.version=ntohs(save_config.version);
   data->config.length =ntohs(save_config.length);
   for (i=0; i<2; i++)
   {
      data->config.pitch[i]   =ntohl(save_config.pitch[i]);
      data->config.speed[i]   =ntohl(save_config.speed[i]);
      data->config.wordgap[i] =ntohl(save_config.wordgap[i]);
      data->config.gender[i]  =ntohl(save_config.gender[i]);
      data->config.language[i]=ntohl(save_config.language[i]);
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
   struct instData *data;

   flags=flags; // removing "unused" warning
   data=(struct instData*)malloc(sizeof(struct instData));

   data->config.version=1;
   data->config.length=sizeof(struct libio_config);
   data->config.pitch[0]=45; data->config.speed[0]=100; data->config.wordgap[0]=20; data->config.gender[0]=1; data->config.language[0]=8;
   data->config.pitch[1]=90; data->config.speed[1]=120; data->config.wordgap[1]=0;  data->config.gender[1]=2; data->config.language[1]=8;

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
OAPC_EXT_API unsigned long oapc_init(void*)
{
   return OAPC_OK;
}



/**
This function is called before the application unloads everything, it has to be used to deinitialize
everything and to release used resources.
*/
OAPC_EXT_API unsigned long oapc_exit(void*)
{
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
   int              idx,ret;
   struct instData *data;
   char             langtype[100];

   data=(struct instData*)instanceData;

   if (input==0) idx=0;
   else if (input==4) idx=1;
   else return OAPC_ERROR_NO_SUCH_IO;
   if (data->config.gender[idx]==0) sprintf(langtype,"%s",langID[data->config.language[idx]-1]);
   else if (data->config.gender[idx]==2) sprintf(langtype,"%s+f4",langID[data->config.language[idx]-1]);
   else sprintf(langtype,"%s+m4",langID[data->config.language[idx]-1]);
   ret=DoSpeak(data->config.pitch[idx], data->config.speed[idx], data->config.wordgap[idx]/10,
               langtype,value); // 10 msec
   if (ret==0) return OAPC_OK;
   else return OAPC_ERROR_RESOURCE;
   // ERR_FAILED_TO_LOAD_DATA 
   // define ERR_LOADING_VOICE
}


