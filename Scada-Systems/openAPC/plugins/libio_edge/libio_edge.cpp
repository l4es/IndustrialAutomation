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
 #include <errno.h>
 #include <unistd.h>
 #include <fcntl.h>
#else
 #include <windows.h>
 typedef int socklen_t;
#endif

#define MAX_XML_SIZE          2000

#include "oapc_libio.h"
#include "liboapc.h"

struct instData
{
	int    m_callbackID;
   char   prevDigi[2];
	double prevNum[2];
};



/** the xml data that define the behaviour and user interface of the plugin within the main application */
static char xmltempl[]="<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"?>\n\
<oapc-config>\n\
 <flowimage>%s</flowimage>\n\
</oapc-config>\n";
/** Base64-encoded PNG data of an image that is used as icon for the program flow diagram within the main application*/
static char                 flowImage[]="iVBORw0KGgoAAAANSUhEUgAAAGoAAAAyBAMAAABG0lM+AAAAMFBMVEUEAgSEgoQE/vzEwsT8/vwAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAADc3AyXAAAA2UlEQVR42u2WQRKDIAxFsXAApuMBWk/AwgOkY+5/ppbYwsehmuKmC1j5yDyJWeA3pmk5Hvjh+LNKmO5VoJcVmOZUK2G6VSFaNPpU2sC1DtFanM21Ei51iBYPuXkdiBVmqGlALIKSCsT6eckMN1tfjgI8YRmDhytg/S7F68vHE1bLDLvVrW5166+twtTAes8Xd6UCwDLva1gB0CFhH/vQPg2JKvFHY6QBBVDKNjSyT9nmAHK2WRzblG0OALLNIAFKBZBtggQoFUC2IQ8JZh8g2ywWEsw+UFuofAKTFGpPMFmgYwAAAABJRU5ErkJggg==";
static char                 libname[]="Rising/Falling Edge";
static char                 xmldescr[MAX_XML_SIZE+1];
static lib_oapc_io_callback m_oapc_io_callback; // callback function that is used to inform the main function about changes at the IO ports



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
          OAPC_ACCEPTS_IO_CALLBACK|
          OAPC_FLOWCAT_LOGIC;
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
   return OAPC_DIGI_IO0|
          OAPC_DIGI_IO2|
          OAPC_NUM_IO4|
          OAPC_NUM_IO6;
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
   return OAPC_DIGI_IO0|
          OAPC_DIGI_IO1|
          OAPC_DIGI_IO2|
          OAPC_DIGI_IO3|
          OAPC_DIGI_IO4|
          OAPC_DIGI_IO5|
          OAPC_DIGI_IO6|
          OAPC_DIGI_IO7;
   // avoid it to submit the same IO number for different data types, that would cause displaying problems for the flow symbol - and it would not make sense
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
   assert(strlen(xmldescr)<MAX_XML_SIZE);
   return xmldescr;
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
   flags=flags; // removing "unused" warning

   struct instData *data;

   data=(struct instData*)malloc(sizeof(struct instData));
   if (!data) return NULL;
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
This function is called by the main application when the library provides an digital input (marked
using the digital input flags OAPC_DIGI_IO...), a connection was edited to this input and a data
flow reaches the input.
@param[in] input specifies the input where the data are send to, here not the OAPC_DIGI_IO...-flag is used
           but the plain, 0-based input number
@param[in] value specifies the value (0 or 1) that is set to that input
@return an error code OAPC_ERROR_... in case of an error or OAPC_OK in case the value could be set
*/
OAPC_EXT_API unsigned long  oapc_set_digi_value(void *instanceData,unsigned long input,unsigned char value)
{
   struct instData *data;

   data=(struct instData*)instanceData;

   if (input==0)
   {
      if (value>data->prevDigi[0])
      {
         m_oapc_io_callback(OAPC_DIGI_IO0,data->m_callbackID);
         m_oapc_io_callback(OAPC_DIGI_IO1,data->m_callbackID);
      }
      data->prevDigi[0]=value;
   }
   else if (input==2)
   {
      if (value<data->prevDigi[1])
      {
         m_oapc_io_callback(OAPC_DIGI_IO2,data->m_callbackID);
         m_oapc_io_callback(OAPC_DIGI_IO3,data->m_callbackID);
      }
      data->prevDigi[1]=value;
   }
   else return OAPC_ERROR_NO_SUCH_IO;
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
OAPC_EXT_API unsigned long  oapc_get_digi_value(void* /*instanceData*/,unsigned long output,unsigned char *value)
{
/*   struct instData *data;

   data=(struct instData*)instanceData;*/

   *value=(unsigned char)(output%2);
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

   if (input==4)
   {
      if (value>data->prevNum[0])
      {
         m_oapc_io_callback(OAPC_DIGI_IO4,data->m_callbackID);
         m_oapc_io_callback(OAPC_DIGI_IO5,data->m_callbackID);
      }
      data->prevNum[0]=value;
   }
   else if (input==6)
   {
      if (value<data->prevNum[1])
      {
         m_oapc_io_callback(OAPC_DIGI_IO6,data->m_callbackID);
         m_oapc_io_callback(OAPC_DIGI_IO7,data->m_callbackID);
      }
      data->prevNum[1]=value;
   }
   else return OAPC_ERROR_NO_SUCH_IO;
   return OAPC_OK;
}



