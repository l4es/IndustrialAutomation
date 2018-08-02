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
#else
 #include <winsock2.h>
#endif

#ifdef ENV_LINUX
 #include <arpa/inet.h>
#endif

#define MAX_XML_SIZE  4000
#define MAX_CHAR_SIZE  200

#include "oapc_libio.h"
#include "liboapc.h"

struct libio_config
{
   unsigned short version,length;
   unsigned char  inputseparator,out[MAX_NUM_IOS];
};


struct instData
{
   struct libio_config config;
   int                 m_callbackID;
   char                separator[2];
   double              retNum[MAX_NUM_IOS];
};


/** the xml data that define the behaviour and user interface of the plugin within the main application */
static char xmltempl[]="<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"?>\n\
<oapc-config>\n\
 <flowimage>%s</flowimage>\n\
 <dialogue>\n\
  <general>\n\
   <param>\n\
    <name>inputseparator</name>\n\
    <text>Column Separator</text>\n\
    <type>option</type>\n\
    <value>Tabulator</value>\n\
    <value>Comma (,)</value>\n\
    <value>Semicolon (;)</value>\n\
    <value>Colon (:)</value>\n\
    <value>Space ( )</value>\n\
    <default>%d</default>\n\
   </param>\n\
   <param>\n\
    <name>out0</name>\n\
    <text>Out 0 from CSV field</text>\n\
    <type>integer</type>\n\
    <default>%d</default>\n\
    <min>1</min>\n\
    <max>100</max>\n\
   </param>\n\
   <param>\n\
    <name>out1</name>\n\
    <text>Out 1 from CSV field</text>\n\
    <type>integer</type>\n\
    <default>%d</default>\n\
    <min>1</min>\n\
    <max>100</max>\n\
   </param>\n\
   <param>\n\
    <name>out2</name>\n\
    <text>Out 2 from CSV field</text>\n\
    <type>integer</type>\n\
    <default>%d</default>\n\
    <min>1</min>\n\
    <max>100</max>\n\
   </param>\n\
   <param>\n\
    <name>out3</name>\n\
    <text>Out 3 from CSV field</text>\n\
    <type>integer</type>\n\
    <default>%d</default>\n\
    <min>1</min>\n\
    <max>100</max>\n\
   </param>\n\
   <param>\n\
    <name>out4</name>\n\
    <text>Out 4 from CSV field</text>\n\
    <type>integer</type>\n\
    <default>%d</default>\n\
    <min>1</min>\n\
    <max>100</max>\n\
   </param>\n\
   <param>\n\
    <name>out5</name>\n\
    <text>Out 5 from CSV field</text>\n\
    <type>integer</type>\n\
    <default>%d</default>\n\
    <min>1</min>\n\
    <max>100</max>\n\
   </param>\n\
   <param>\n\
    <name>out6</name>\n\
    <text>Out 6 from CSV field</text>\n\
    <type>integer</type>\n\
    <default>%d</default>\n\
    <min>1</min>\n\
    <max>100</max>\n\
   </param>\n\
   <param>\n\
    <name>out7</name>\n\
    <text>Out 7 from CSV field</text>\n\
    <type>integer</type>\n\
    <default>%d</default>\n\
    <min>1</min>\n\
    <max>100</max>\n\
   </param>\n\
  </general>\n\
  <helppanel>\n\
   <in0>CSV - Single CSV line to be decoded</in0>\n\
   <out0>D0 - value out of CSV field</out0>\n\
   <out1>...</out1>\n\
   <out7>D7 - value out of CSV field</out7>\n\
  </helppanel>\n\
 </dialogue>\n\
</oapc-config>\n";
/** Base64-encoded PNG data of an image that is used as icon for the program flow diagram within the main application*/
static char                 flowImage[]="iVBORw0KGgoAAAANSUhEUgAAAGoAAAAyCAMAAACDIr4/AAAAA3NCSVQICAjb4U/gAAAAGFBMVEUEAgT8/vyEgoT8Avz////EwsQAAAAA//+lzPMaAAAACXBIWXMAAA7EAAAOxAGVKw4bAAABCklEQVRYhe3YwQ6DMAgGYFBL3/+Nh660WiVZ0v7sIqdx+sRWyCAKDGamZV3XZf/FvGFCkYNKpBSlCGovqxQFp7SsUhSc0rJKUXgqWVF4iq0oNGUZNAolIlGUhsRRm4RRUVUBz6reQfwN7CkZfYHPE+OZkrFrQV2eXOooayKlfc6lRs/qSu0d1X+BM6s6encI9Z0S/lnJ4A3sJZ+ad1Y2+fBUnbFwqk1zhxoP6iU0dZIcamsxRJ0lLHWRoNRVQlKdBKR6CUj10m2C8TzKm8LTqd+e5qVe6t9UTJQVyfmjQyW2jck52398VGLbGM2ti6GSuo2pjwFL6jYmt9YMSu7bGFhy38bAkoc5A4sPx+gh82y6iZ4AAAAASUVORK5CYII=";
static char                 libname[]="CSV to Number";
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
          OAPC_HAS_INPUTS|
          OAPC_HAS_XML_CONFIGURATION|
          OAPC_ACCEPTS_PLAIN_CONFIGURATION|
          OAPC_ACCEPTS_IO_CALLBACK|
          OAPC_FLOWCAT_CONVERSION;
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
   return OAPC_CHAR_IO0;
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
   return OAPC_NUM_IO0|OAPC_NUM_IO1|OAPC_NUM_IO2|OAPC_NUM_IO3|OAPC_NUM_IO4|OAPC_NUM_IO5|OAPC_NUM_IO6|OAPC_NUM_IO7;
   // avoid it to submit the same IO number for different data types, that would cause displaying problems for the flow symbol - and it would not make sense
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

   sprintf(xmldescr,xmltempl,flowImage,data->config.inputseparator,
                                       data->config.out[0],data->config.out[1],data->config.out[2],data->config.out[3],
                                       data->config.out[4],data->config.out[5],data->config.out[6],data->config.out[7]);
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

   if (strcmp(name,"inputseparator")==0) data->config.inputseparator=(unsigned char)atoi(value);
   else if (strcmp(name,"out0")==0)      data->config.out[0]=(unsigned char)atoi(value);
   else if (strcmp(name,"out1")==0)      data->config.out[1]=(unsigned char)atoi(value);
   else if (strcmp(name,"out2")==0)      data->config.out[2]=(unsigned char)atoi(value);
   else if (strcmp(name,"out3")==0)      data->config.out[3]=(unsigned char)atoi(value);
   else if (strcmp(name,"out4")==0)      data->config.out[4]=(unsigned char)atoi(value);
   else if (strcmp(name,"out5")==0)      data->config.out[5]=(unsigned char)atoi(value);
   else if (strcmp(name,"out6")==0)      data->config.out[6]=(unsigned char)atoi(value);
   else if (strcmp(name,"out7")==0)      data->config.out[7]=(unsigned char)atoi(value);
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
   save_config.version       =htons(1);
   save_config.length        =htons((unsigned short)*length);
   save_config.inputseparator=data->config.inputseparator;
   for (i=0; i<MAX_NUM_IOS; i++)
    save_config.out[i]=data->config.out[i];
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
   data->config.version       =ntohs(save_config.version);
   data->config.length        =ntohs(save_config.length);
   data->config.inputseparator=save_config.inputseparator;
   for (i=0; i<MAX_NUM_IOS; i++)
    data->config.out[i]=save_config.out[i];
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
   int              i;

   flags=flags; // removing "unused" warning
   data=(struct instData*)malloc(sizeof(struct instData));
   if (!data) return NULL;
   memset(data,0,sizeof(struct instData));
   for (i=0; i<MAX_NUM_IOS; i++) data->config.out[i]=(unsigned char)(i+1);
   data->config.inputseparator=3;
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

   data->separator[1]=0;
   if (data->config.inputseparator==1) data->separator[0]='\t';
   else if (data->config.inputseparator==2) data->separator[0]=',';
   else if (data->config.inputseparator==3) data->separator[0]=';';
   else if (data->config.inputseparator==4) data->separator[0]=':';
   else if (data->config.inputseparator==5) data->separator[0]=' ';

   return OAPC_OK;
}



/**
This function is called before the application unloads everything, it has to be used to deinitialize
everything and to release used resources.
*/
OAPC_EXT_API unsigned long oapc_exit(void* /*instanceData*/)
{
/*   struct instData *data;

   data=(struct instData*)instanceData;*/

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
OAPC_EXT_API unsigned long oapc_get_num_value(void* instanceData,unsigned long output,double *value)
{
   struct instData *data;

   data=(struct instData*)instanceData;

   *value=data->retNum[output];
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
   int              fieldCnt=0,i,outCnt=0;
   char            *c,*s;

   data=(struct instData*)instanceData;

   if (input!=0) return OAPC_ERROR_NO_SUCH_IO; // check for valid IO
   c=value;
   while (*c)
   {
      while ((*c==' ') || (*c=='\t')) c++;
      if (*c==data->separator[0]) // empty field
      {
         fieldCnt++;
         c++;
      }
      else
      {
         s=c;
         while ((*c!=data->separator[0]) && (*c!=0)) c++; // find next separator
         if (*c!=0)
         {
            *c=0;
            c++;

            for (i=0; i<MAX_NUM_IOS; i++) if (data->config.out[i]==fieldCnt+1)
            {
               if ((s[0]=='0') || (s[0]=='1') || (s[0]=='2') || (s[0]=='3') || (s[0]=='4') ||
                   (s[0]=='5') || (s[0]=='6') || (s[0]=='7') || (s[0]=='8') || (s[0]=='9') ||
                   (s[0]=='-') || (s[0]=='.') || (s[0]==',') || (s[0]=='+'))
               {
                  data->retNum[i]=oapc_util_atof(s);
                  switch (i)
                  {
                     case 0:
                        m_oapc_io_callback(OAPC_NUM_IO0,data->m_callbackID);
                        break;
                     case 1:
                        m_oapc_io_callback(OAPC_NUM_IO1,data->m_callbackID);
                        break;
                     case 2:
                        m_oapc_io_callback(OAPC_NUM_IO2,data->m_callbackID);
                        break;
                     case 3:
                        m_oapc_io_callback(OAPC_NUM_IO3,data->m_callbackID);
                        break;
                     case 4:
                        m_oapc_io_callback(OAPC_NUM_IO4,data->m_callbackID);
                        break;
                     case 5:
                        m_oapc_io_callback(OAPC_NUM_IO5,data->m_callbackID);
                        break;
                     case 6:
                        m_oapc_io_callback(OAPC_NUM_IO6,data->m_callbackID);
                        break;
                     case 7:
                        m_oapc_io_callback(OAPC_NUM_IO7,data->m_callbackID);
                        break;
                     default:
                        assert(0);
                        break;
                  }
               }
               outCnt++;
               if (outCnt>=MAX_NUM_IOS) return OAPC_OK;
            }
            fieldCnt++;
         }
      }
   }

   return OAPC_OK;
}




