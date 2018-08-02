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

#ifdef _MSC_VER
#pragma warning (disable: 4100)
#pragma warning (disable: 4245)
#pragma warning (disable: 4505)
#endif


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
#include <sys/ioctl.h>
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
 #define snprintf _snprintf
#endif

#include "oapc_libio.h"
#include "liboapc.h" // contains sleep function

#define MAX_XML_SIZE   4000

struct instData
{
   int                   m_callbackID;
   double                m_outVal[6];
};



/** the xml data that define the behaviour and user interface of the plugin within the main application */
static char xmltempl[]="<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"?>\n\
<oapc-config>\n\
<flowimage>%s</flowimage>\n\
<dialogue>\n\
 <helppanel>\n\
  <in7>POSCORR - Position correction data</in7>\n\
  <out0>X - X position correction</out0>\n\
  <out1>Y - Y position correction</out1>\n\
  <out2>Z - Z position correction</out2>\n\
  <out3>XANG - X angle correction</out3>\n\
  <out4>YANG - Y angle correction</out4>\n\
  <out5>ZANG - Z angle correction</out5>\n\
 </helppanel>\n\
</dialogue>\n""\
</oapc-config>\n";
/** Base64-encoded PNG data of an image that is used as icon for the program flow diagram within the main application*/
static char                 flowImage[]="iVBORw0KGgoAAAANSUhEUgAAAGoAAAAyBAMAAABG0lM+AAAAMFBMVEUEAgT8/gSEgoQE/vzEwsT8/vwAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAACigH0vAAAACXBIWXMAAA7EAAAOxAGVKw4bAAABWElEQVR42t2WTXLDIAyFVcfaNz2BO7lAJ74AG++1sO5/lRokXJkMELSsVxHxx9PPgzGA60Eeeuj7vjCNUvvHfR6nePpCBxU+NwdFCzuoffZQjC6KPRSRp4foyjC66Z/UdTkfb9eVXwk4UhdmL2EixqiwYQqGMjxs66DCxr0MEfJsMhVPSK8biGlfopM6pLoZomTDJxWluhSBzPfMMErxaOeTVLcucQ8B6MtJSjKkhpYsk/4WKaGwUZdlUaW61DVDldK+vusNlep2Q9dFa55NsU1HoThK0t0M1fTGhbJnue0Nk+GLa/w3m/GoMSz+Te1lkklLi5BFJFMRNm423RLMkcrKUKcuWgxtrXwfjtXlu+czJeM7nAoy0UpQUNP6jP891iV+TlSDggrrz2G0/bZGk9aDcsqPtNX0TA2oBgW139JWIe7bCMoeTpg/WrgVFJQcVT3m9cBSrucXanHq2qGHv5oAAAAASUVORK5CYII=";
static char                 libname[]="Position Correction to Number";
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
   return OAPC_HAS_INPUTS|OAPC_HAS_OUTPUTS|
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
   return OAPC_BIN_IO7;
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
   return OAPC_NUM_IO0|OAPC_NUM_IO1|OAPC_NUM_IO2|OAPC_NUM_IO3|OAPC_NUM_IO4|OAPC_NUM_IO5;
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

   snprintf(xmldescr,MAX_XML_SIZE,xmltempl,flowImage);
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
//   struct instData *data;

//   data=(struct instData*)instanceData;

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
OAPC_EXT_API unsigned long  oapc_get_num_value(void* instanceData,unsigned long output,double* value)
{
   struct instData *data;
   
   if (output>6) return OAPC_ERROR_NO_SUCH_IO;
   data=(struct instData*)instanceData;
   *value=data->m_outVal[output];
   return OAPC_OK;
}



OAPC_EXT_API unsigned long  oapc_set_bin_value(void* instanceData,unsigned long input,struct oapc_bin_head *value)
{
   struct instData                 *data;
   struct oapc_bin_struct_pos_corr *posCorr;

   if (input!=7) return OAPC_ERROR_NO_SUCH_IO;
   if (value->type!=OAPC_BIN_TYPE_STRUCT) return OAPC_ERROR_INVALID_INPUT;
   if (value->subType!=OAPC_BIN_SUBTYPE_STRUCT_POS_CORR) return OAPC_ERROR_INVALID_INPUT; // nothing to do in case of an end signal

   data=(struct instData*)instanceData;

   posCorr=(struct oapc_bin_struct_pos_corr*)&value->data;
   data->m_outVal[0]=posCorr->uPosX/1000.0;
   data->m_outVal[1]=posCorr->uPosY/1000.0;
   data->m_outVal[2]=posCorr->uPosZ/1000.0;
   data->m_outVal[3]=posCorr->mXAngle/1000.0;
   data->m_outVal[4]=posCorr->mYAngle/1000.0;
   data->m_outVal[5]=posCorr->mZAngle/1000.0;
   m_oapc_io_callback(OAPC_NUM_IO0,data->m_callbackID);
   m_oapc_io_callback(OAPC_NUM_IO1,data->m_callbackID);
   m_oapc_io_callback(OAPC_NUM_IO2,data->m_callbackID);
   m_oapc_io_callback(OAPC_NUM_IO3,data->m_callbackID);
   m_oapc_io_callback(OAPC_NUM_IO4,data->m_callbackID);
   m_oapc_io_callback(OAPC_NUM_IO5,data->m_callbackID);

   return OAPC_OK;
}


