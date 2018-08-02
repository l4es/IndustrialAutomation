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
 #include <linux/joystick.h>
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



struct instData
{
   int                   m_callbackID;
   unsigned char         m_digi[2];
   double                m_num[2];
   char                  m_char[2][MAX_TXT_SIZE+4];
   struct oapc_bin_head *m_bin[2];
};


/** the xml data that define the behaviour and user interface of the plugin within the main application */
static char xmltempl[]="<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"?>\n\
<oapc-config>\n\
<flowimage>%s</flowimage>\n\
</oapc-config>\n";
/** Base64-encoded PNG data of an image that is used as icon for the program flow diagram within the main application*/
static char                        flowImage[]="iVBORw0KGgoAAAANSUhEUgAAAGoAAAAyCAMAAACDIr4/AAAAA3NCSVQICAjb4U/gAAAAG1BMVEUEAgT8/vyCgob///8E/vz/AP///wDBw8cAAACQizKKAAAACXBIWXMAAA7EAAAOxAGVKw4bAAAAq0lEQVRYhe3YwRKDIAxF0cRA8P+/uGEAxWKmLoRuXlae6cjFZUO0cJiZNhHZ8tOIEIKDGKMDVR1RUonsdErlxy/Y6Q7sdAd2+oiSyl/SrjEguIgu9AY1ZV/SrjEguIgu9AY1Ze3jGrPQUum8xiy0FO/czQX0DD/feZB6C4tTkmeXbl4GUkghhRRSSCGFFFJIIYXUifWp/k/QRNQVybEsmQhsY7CN+fs2Ztl8AL+vNANL5P2eAAAAAElFTkSuQmCC";
static char                        libname[]="Data Dummy";
static char                        xmldescr[MAX_XML_SIZE+1];
static lib_oapc_io_callback        m_oapc_io_callback; // callback function that is used to inform the main function about changes at the IO ports



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
          OAPC_ACCEPTS_PLAIN_CONFIGURATION|OAPC_ACCEPTS_IO_CALLBACK|
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
   return OAPC_DIGI_IO0|OAPC_DIGI_IO1|OAPC_NUM_IO2|OAPC_NUM_IO3|OAPC_CHAR_IO4|OAPC_CHAR_IO5|OAPC_BIN_IO6|OAPC_BIN_IO7;
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
   return OAPC_DIGI_IO0|OAPC_DIGI_IO1|OAPC_NUM_IO2|OAPC_NUM_IO3|OAPC_CHAR_IO4|OAPC_CHAR_IO5|OAPC_BIN_IO6|OAPC_BIN_IO7;
   // avoid it to submit the same IO number for different data types, that would cause displaying problems for the flow symbol
}



/**
When the OAPC_HAS_XML_CONFIGURATION capability flag was set this function is called to retrieve an XML
structure that describes which configuration information have to be displayed within the main application.
@param[out] data pointer to the beginning of an char-array that contains the XML data
@return the length of the XML structure or 0 in case of an error
*/
OAPC_EXT_API char *oapc_get_config_data(void* /*instanceData*/)
{
/*   struct instData *data;

   data=(struct instData*)instanceData;*/

   sprintf(xmldescr,xmltempl,flowImage);
#ifdef _DEBUG
   long l=strlen(xmldescr);
   l=l;
   assert(strlen(xmldescr)<MAX_XML_SIZE);
#endif
   return xmldescr;
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
   memset(data,0,sizeof(struct instData));

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
OAPC_EXT_API unsigned long oapc_init(void* /*instanceData*/)
{
/*   struct instData *data;

   data=(struct instData*)instanceData;*/
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



OAPC_EXT_API unsigned long  oapc_set_digi_value(void* instanceData,unsigned long input,unsigned char value)
{
   struct instData           *data;

   data=(struct instData*)instanceData;   
   if (input==0)
   {
      data->m_digi[0]=value;
      m_oapc_io_callback(OAPC_DIGI_IO0,data->m_callbackID);
   }
   else if (input==1)
   {
      data->m_digi[1]=value;
      m_oapc_io_callback(OAPC_DIGI_IO1,data->m_callbackID);
   }
   else return OAPC_ERROR_NO_SUCH_IO;   
   return OAPC_OK;
}



OAPC_EXT_API unsigned long  oapc_get_digi_value(void* instanceData,unsigned long output,unsigned char *value)
{
   struct instData *data;

   data=(struct instData*)instanceData;
   if (output==0) *value=data->m_digi[0];
   else if (output==1) *value=data->m_digi[1];
   else return OAPC_ERROR_NO_SUCH_IO;   
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
   struct instData           *data;

   data=(struct instData*)instanceData;   
   if (input==2)
   {
      data->m_num[0]=value;
      m_oapc_io_callback(OAPC_NUM_IO2,data->m_callbackID);
   }
   else if (input==3)
   {
      data->m_num[1]=value;
      m_oapc_io_callback(OAPC_NUM_IO3,data->m_callbackID);
   }
   else return OAPC_ERROR_NO_SUCH_IO;   
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
   struct instData *data;

   data=(struct instData*)instanceData;
   if (output==2) *value=data->m_num[0];
   else if (output==3) *value=data->m_num[1];
   else return OAPC_ERROR_NO_SUCH_IO;   
   return OAPC_OK;
}



OAPC_EXT_API unsigned long  oapc_set_char_value(void* instanceData,unsigned long input,char *value)
{
   struct instData           *data;

   data=(struct instData*)instanceData;   
   if (input==4)
   {
      strncpy(data->m_char[0],value,MAX_TXT_SIZE);
      m_oapc_io_callback(OAPC_CHAR_IO4,data->m_callbackID);
   }
   else if (input==5)
   {
      strncpy(data->m_char[1],value,MAX_TXT_SIZE);
      m_oapc_io_callback(OAPC_CHAR_IO5,data->m_callbackID);
   }
   else return OAPC_ERROR_NO_SUCH_IO;   
   return OAPC_OK;
}



OAPC_EXT_API unsigned long  oapc_get_char_value(void* instanceData,unsigned long output,unsigned long length,char *value)
{
   struct instData *data;

   data=(struct instData*)instanceData;
   if (length>MAX_TXT_SIZE) length=MAX_TXT_SIZE;
   if (output==4) strncpy(value,data->m_char[0],length);
   else if (output==5) strncpy(value,data->m_char[1],length);
   else return OAPC_ERROR_NO_SUCH_IO;   
   return OAPC_OK;
}



OAPC_EXT_API unsigned long  oapc_set_bin_value(void* instanceData,unsigned long input,struct oapc_bin_head *value)
{
   struct instData           *data;

   data=(struct instData*)instanceData;   
   if (input==6)
   {
      data->m_bin[0]=(struct oapc_bin_head*)malloc(sizeof(struct oapc_bin_head)+value->sizeData);
      if (!data->m_bin[0]) return OAPC_ERROR_NO_MEMORY;
      memcpy(data->m_bin[0],value,sizeof(struct oapc_bin_head)+value->sizeData-1);
      m_oapc_io_callback(OAPC_BIN_IO6,data->m_callbackID);
   }
   else if (input==7)
   {
      data->m_bin[1]=(struct oapc_bin_head*)malloc(sizeof(struct oapc_bin_head)+value->sizeData);
      if (!data->m_bin[1]) return OAPC_ERROR_NO_MEMORY;
      memcpy(data->m_bin[1],value,sizeof(struct oapc_bin_head)+value->sizeData-1);
      m_oapc_io_callback(OAPC_BIN_IO7,data->m_callbackID);
   }
   else return OAPC_ERROR_NO_SUCH_IO;   
   return OAPC_OK;
}



OAPC_EXT_API unsigned long  oapc_get_bin_value(void* instanceData,unsigned long output,struct oapc_bin_head **value)
{
   struct instData *data;

   data=(struct instData*)instanceData;
   if (output==6)
   {
      if (!data->m_bin[0]) return OAPC_ERROR_NO_DATA_AVAILABLE;
      *value=data->m_bin[0];
   }
   else if (output==7)
   {
      if (!data->m_bin[1]) return OAPC_ERROR_NO_DATA_AVAILABLE;
      *value=data->m_bin[1];
   }
   else return OAPC_ERROR_NO_SUCH_IO;   
   return OAPC_OK;
}



OAPC_EXT_API void oapc_release_bin_data(void *instanceData,unsigned long output)
{
   struct instData *data;

   data=(struct instData*)instanceData;
   	
   if (output==6) 
   {
      free(data->m_bin[0]);
	   data->m_bin[0]=NULL;
   }
   else if (output==7)
   {
      free(data->m_bin[1]);
	   data->m_bin[1]=NULL;
   }
}
