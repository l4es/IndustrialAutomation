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

#ifdef ENV_WINDOWS
#ifdef _DEBUG 
#include <crtdbg.h> 
#endif
#endif

#include <wx/wx.h>

#include "wxSMTP.h"

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
#ifndef ENV_WINDOWSCE
#endif
#endif

#include "oapc_libio.h"

#define MAX_XML_SIZE        5000
#define MAX_BUTTONNAME_SIZE   31
#define MAX_NAME_SIZE         31
#define MAX_TEXT_SIZE        511

struct libio_config
{
   unsigned short version,length;
   unsigned int   reserved1;
   char           defaultMsgText[MAX_TEXT_SIZE+1];
   char           defaultMsgSubj[MAX_NAME_SIZE+1];
   char           defaultMsgTo[MAX_NAME_SIZE+1];
   char           mailserver[MAX_NAME_SIZE+1];
};



struct instData
{
   struct libio_config  config;
   char                 msgText[MAX_TEXT_SIZE+1];
   char                 msgSubj[MAX_NAME_SIZE+1];
   char                 msgTo[MAX_NAME_SIZE+1];
};



/** the xml data that define the behaviour and user interface of the plugin within the main application */
static char xmltempl[]="<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"?>\
<oapc-config>\
<flowimage>%s</flowimage>\
<dialogue>\
 <general>\
  <param>\
   <name>mailserver</name>\
   <text>Mailserver</text>\
   <type>string</type>\
   <default>%s</default>\
   <min>3</min>\
   <max>%d</max>\
  </param>\
  <param>\
   <name>to</name>\
   <text>To</text>\
   <type>string</type>\
   <default>%s</default>\
   <min>3</min>\
   <max>%d</max>\
  </param>\
  <param>\
   <name>subject</name>\
   <text>Subject</text>\
   <type>string</type>\
   <default>%s</default>\
   <min>3</min>\
   <max>%d</max>\
  </param>\
  <param>\
   <name>text</name>\
   <text>Text</text>\
   <type>string</type>\
   <default>%s</default>\
   <min>3</min>\
   <max>%d</max>\
  </param>\
 </general>\
 <helppanel>\
  <in0>SEND - send the mail</in0>\
  <in1>SUB - set a subject dynamically</in1>\
  <in2>BDY - set a mail body text dynamically</in2>\
 </helppanel>\
</dialogue>\
</oapc-config>\n";


/** Base64-encoded PNG data of an image that is used as icon for the program flow diagram within the main application*/
static char                 flowImage[]="iVBORw0KGgoAAAANSUhEUgAAAGoAAAAyCAMAAACDIr4/AAADAFBMVEUEAgTcnkyEgoTEwsT8Avyc3twESkwEAky8/ty8/ry8/vxMntz63px0AkxMAgT8/gQESpycSgRMnrwEdry8dgT8vnR0vvyc3rxMAnQEAnSc3vz8/tz8/rz8/vx0AgTc/vwAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAgnq46AAABpklEQVR42u2W63KCMBCFF6GlBRQpl6oQfP+37C4hkCjgcrN/khnUkxnz7cmeMAF44xBCwAE/DvSrEZ7ncUQcx0pwhkS5Dc+VUy7gghyBKCX4KLLVF4jF88QsUwqFtvoCsXiemGVKobBErcAlgo9y9QKXCD5KGAUuEXzU/qPrlY4ErV6zEhCwFgX6ojDBWu9qAmUYXrPfCgXaIoYwXMEaly/LfIocbO1qh8AOnytdbnYcurCP7Rs8CrHbBpooWOHxdSyMIlhrOvSXUP4qhOPfps7Vmp2KTil+VvX5LsrkGtDzgILxt8UsblUXzXeZhK1GJg8FDFZ0+knA/62B9isVOcDnKc2vARmUJsdiYewgB5WDf6MHly2TghbPIKyOOBeKXHZtdixGc4BucEmERJcgwxzgDKLkfPH0ul2ehKbzLSC6fLWqhWYqgIO9mjuo82UH+G6Vk5oB3ARFzVG46viRUOPOaIrmqjodeDEtRuWyTTLXWVjVGEBAM2Yqtr9b5Ko36oDteI1xpA/Nzn43pjKh89Gl4T8vZxZlURZlURZlURbVX17fNP4Ay/AKSaaqVpgAAAAASUVORK5CYII=";
static char                 libname[]="E-Mail Notification";
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
          OAPC_FLOWCAT_DATA;
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
   return OAPC_DIGI_IO0|OAPC_CHAR_IO1|OAPC_CHAR_IO2|OAPC_CHAR_IO3;
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

   sprintf(xmldescr,xmltempl,flowImage,data->config.mailserver,MAX_NAME_SIZE,
                                       data->config.defaultMsgTo,MAX_NAME_SIZE,
                                       data->config.defaultMsgSubj,MAX_NAME_SIZE,
                                       data->config.defaultMsgText,MAX_TEXT_SIZE);
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

   if (strcmp(name,"to")==0)           strncpy(data->config.defaultMsgTo,value,MAX_NAME_SIZE);
   else if (strcmp(name,"subject")==0) strncpy(data->config.defaultMsgSubj,value,MAX_NAME_SIZE);
   else if (strcmp(name,"text")==0)    strncpy(data->config.defaultMsgText,value,MAX_TEXT_SIZE);
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
   save_config.version=htons(data->config.version);
   save_config.length =htons(data->config.length);
   strncpy(save_config.defaultMsgSubj,data->config.defaultMsgSubj,MAX_NAME_SIZE);
   strncpy(save_config.defaultMsgTo,  data->config.defaultMsgTo,  MAX_NAME_SIZE);
   strncpy(save_config.defaultMsgText,data->config.defaultMsgText,MAX_TEXT_SIZE);
   strncpy(save_config.mailserver,    data->config.mailserver,MAX_NAME_SIZE);

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
   data->config.version=ntohs(save_config.version);
   data->config.length =ntohs(save_config.length);
   strncpy(data->config.defaultMsgSubj,save_config.defaultMsgSubj,MAX_NAME_SIZE);
   strncpy(data->config.defaultMsgTo,  save_config.defaultMsgTo,  MAX_NAME_SIZE);
   strncpy(data->config.defaultMsgText,save_config.defaultMsgText,MAX_TEXT_SIZE);
   strncpy(data->config.mailserver,    save_config.mailserver,    MAX_NAME_SIZE);
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
   strcpy(data->config.mailserver,"127.0.0.1");
   strcpy(data->config.defaultMsgSubj,"OpenAPC Notification");
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
OAPC_EXT_API unsigned long oapc_init(void *instanceData)
{
   struct instData *data;

   data=(struct instData*)instanceData;
   strncpy(data->msgSubj,data->config.defaultMsgSubj,MAX_NAME_SIZE);
   strncpy(data->msgTo,  data->config.defaultMsgTo,  MAX_NAME_SIZE);
   strncpy(data->msgText,data->config.defaultMsgText,MAX_TEXT_SIZE);
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
   wxMBConvUTF8     conv;
   wchar_t          wc[MAX_TEXT_SIZE+1];
   wxString         str_mailserver,str_subj,str_text,str_to;

   data=(struct instData*)instanceData;

   if (input!=0) return OAPC_ERROR_NO_SUCH_IO; // check for valid IO
   if (value==0) return OAPC_OK;
   conv.MB2WC(wc,data->config.mailserver,MAX_TEXT_SIZE);
   str_mailserver=wc;
   conv.MB2WC(wc,data->msgSubj,MAX_TEXT_SIZE);
   str_subj=wc;
   conv.MB2WC(wc,data->msgText,MAX_TEXT_SIZE);
   str_text=wc;
   conv.MB2WC(wc,data->msgTo,MAX_TEXT_SIZE);
   str_to=wc;

   wxSMTP *smtp = new wxSMTP(NULL);
   smtp->SetHost(str_mailserver);
   wxEmailMessage *msg = new wxEmailMessage(str_subj,str_text,str_to);
   msg->AddTo(str_to);
   smtp->Send(msg);

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

   data=(struct instData*)instanceData;

   if (input==1) strncpy(data->msgSubj,value,MAX_NAME_SIZE);
   else if (input==2) strncpy(data->msgText,value,MAX_TEXT_SIZE);
   else if (input==3) strncpy(data->msgTo,value,MAX_NAME_SIZE);
   else return OAPC_ERROR_NO_SUCH_IO;

   return OAPC_OK;
}



