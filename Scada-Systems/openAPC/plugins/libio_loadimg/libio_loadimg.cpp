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
#pragma warning (disable: 4530)
#endif

#ifdef ENV_WINDOWS
#ifdef _DEBUG 
#include <crtdbg.h> 
#endif
#endif

#include <wx/wx.h>

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

#ifndef ENV_WINDOWS
 #include <arpa/inet.h>
 #include <errno.h>
 #include <unistd.h>
 #include <fcntl.h>
 #define closesocket close
#else
 #define snprintf _snprintf
#endif

#include "oapc_libio.h"
#include "liboapc.h"

#define MAX_XML_SIZE       5000
#define MAX_FILENAME_SIZE   255
#define MAX_BUFSIZE        5000

struct libio_config
{
   unsigned short version,length;
   char           file1[MAX_FILENAME_SIZE+1],file2[MAX_FILENAME_SIZE+1],
                  file3[MAX_FILENAME_SIZE+1],file4[MAX_FILENAME_SIZE+1];
   unsigned int   reserved1,reserved2,reserved3,reserved4;
   unsigned char  reserved5,reserved6,reserved7,reserved8;
};



struct client_data
{
	char                  currFile[MAX_FILENAME_SIZE+1];
   struct oapc_bin_head *bin;
	int                   m_callbackID,idx;
   int                   currErr,lastErr;
};


struct instData
{
   struct libio_config   config;
   struct client_data    client[4];
   char                  txt[4][MAX_BUFSIZE+1];
   int                   m_callbackID;
};



/** the xml data that define the behaviour and user interface of the plugin within the main application */
static char xmltempl[]="<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"?>\
<oapc-config>\
 <flowimage>%s</flowimage>\
 <dialogue>\
  <general>\
   <param text=\"Default Filename 1\">\
    <name>file1</name>\
    <type>fileload</type>\
    <default>%s</default>\
    <min>0</min>\
    <max>%d</max>\
    <ffilter>Image files|*.bmp;*.xbm;*.xpm;*.gif;*.png;*.jpg;*.jpeg|All files|*</ffilter>\
   </param>\
   <param text=\"Default Filename 2\">\
    <name>file2</name>\
    <type>fileload</type>\
    <default>%s</default>\
    <min>0</min>\
    <max>%d</max>\
    <ffilter>Image files|*.bmp;*.xbm;*.xpm;*.gif;*.png;*.jpg;*.jpeg|All files|*</ffilter>\
   </param>\
   <param text=\"Default Filename 3\">\
    <name>file3</name>\
    <type>fileload</type>\
    <default>%s</default>\
    <min>0</min>\
    <max>%d</max>\
    <ffilter>Image files|*.bmp;*.xbm;*.xpm;*.gif;*.png;*.jpg;*.jpeg|All files|*</ffilter>\
   </param>\
   <param text=\"Default Filename 4\">\
    <name>file4</name>\
    <type>fileload</type>\
    <default>%s</default>\
    <min>0</min>\
    <max>%d</max>\
    <ffilter>Image files|*.bmp;*.xbm;*.xpm;*.gif;*.png;*.jpg;*.jpeg|All files|*</ffilter>\
   </param>\
  </general>\
  <helppanel>\
   <in0>CLK1 - load predefined file</in0>\
   <in1>FILE1 - set new file and load it</in1>\
   <out0>DATA1 - loaded data</out0>\
   <out1>ERR1 - error code when loading failed</out1>\
  </helppanel>\
 </dialogue>\
</oapc-config>\n";
/** Base64-encoded PNG data of an image that is used as icon for the program flow diagram within the main application*/
static char                 flowImage[]="iVBORw0KGgoAAAANSUhEUgAAAGoAAAAyCAMAAACDIr4/AAADAFBMVEUEAgRMmhyEgoRc6oyw1ozEwsR0zjQk5myM2kSU8kSs5oTc6syn8WmM6qR8jswEArzw8PDMruRMYozoxvjG6K7D/WvZ/qqd+0zMhuy89ozc8syMzlzMZvzI8rAcnnyEwrTM1siU/mz0/uzam/rMfuxkwiz8/gQ0/nSM7lz8Avzsvvzx2fnP/pTq/cy2uLYE/vyU4lCM6jwcHtyEAgSEkrx81FjM3rzEntwEGpyE/oy0wqyM5lx0vkx0/px8/nTcFvzkr/y0/nS0hsza2drk5uBUqiTUkuzEzrzc/rzUbuzstuyf71mx/lyE0lTRe/zMzsyM/nys/ozU6rzcivzU7rzO/ofs8tzU/nzs/tzMcvxs8pSB1jmk9mSUzmz8/vx0yjQAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAhPkBVAAAACXBIWXMAABJ0AAASdAHeZh94AAACrklEQVR42tWYfV/SUBTHb+QwkvKWONMeYTUlmKOWmpZpbeoKdJYRELTe/7tod3AfHdsublbnDz6cz4/xPeeec++FA8A1mu/7oBC8FNC70KnX69TJTBmjlPCtMhYVEIjUyUwZo1AgOIwwENbJSpmggkBwGGEgrJOVMkEFbBJGXg5GKTSMvByM8pkwcnIIKn/DHQgYJufEPcsGD3yQDhWTfiwJRDrxqFkWhA0KJEf4N2rlA5kIr4DyxQqDf6sDqW3aonleTrWyBR86tuPFoGbod4qCawuMPTvZg5dYUQsoi7IC1LcVYqVSqTX87fi2l7yAkrWyQtRyh7F265bjiyyc1ez9bY1RvW6vt4xt1G5tO47D90YUis8KxAdhYWuqarPZDa1X3XjXOn14omxGnoHTlixhk1kUNbe3bRhG31C/np9vbJ1uvfdsL7FWAiqmcJRk3V98O39z587u7odqdeXV8MXRDaHho1EgfZ2IvV58sr7+5nM/yMxwXfdxpQKhd4UOFAvHoODdn/v7Tw++1JD1kUFos9+W3NbckXUpYwZlrAW5LLiqivoDdQjOKu2FL1614ucJ6kJbajiua7jN7kdknU4tRNEDXSqrqfsK3hucadrxp52lxo/uaLRaXi2X2ylqJb2HEerRQHv+4Fg7fBnkVvl+e1gsFiFUpFBAXE4QdQYi1ODsQtMOEasBYa0y92uCws8ko8S2iDzZYbQp7DNyWU3bBrGo9M0ufwt7E1OUglxbCGevfGjpayX3EzbmCyRPCyC/4BnXSu53YKpPzHJZk2dSNbvMn4Dpd6vkFv6vakWHJVk4nMfMYSbTGF3X6fzENE3qyCucRBU8jQk0Oj8JROrIK5xEFTKN0dn5ick68gonEYVMY3R2f"
"mKyjrzCSUTJZxrDeflOYziPmcZcm/0BUkPaZuD48mEAAAAASUVORK5CYII=";
static char                 libname[]="Load Image";
static char                 xmldescr[MAX_XML_SIZE+1];
static struct libio_config  save_config;
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
          OAPC_ACCEPTS_PLAIN_CONFIGURATION|OAPC_ACCEPTS_IO_CALLBACK|
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
   return OAPC_DIGI_IO0|OAPC_CHAR_IO1|OAPC_DIGI_IO2|OAPC_CHAR_IO3|
          OAPC_DIGI_IO4|OAPC_CHAR_IO5|OAPC_DIGI_IO6|OAPC_CHAR_IO7;
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
   return OAPC_BIN_IO0|OAPC_NUM_IO1|OAPC_BIN_IO2|OAPC_NUM_IO3|
          OAPC_BIN_IO4|OAPC_NUM_IO5|OAPC_BIN_IO6|OAPC_NUM_IO7;
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

   snprintf(xmldescr,MAX_XML_SIZE,xmltempl,flowImage,
                                           data->config.file1,MAX_FILENAME_SIZE,
                                           data->config.file2,MAX_FILENAME_SIZE,
                                           data->config.file3,MAX_FILENAME_SIZE,
                                           data->config.file4,MAX_FILENAME_SIZE);
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

   if (strcmp(name,"file1")==0)      strncpy(data->config.file1,value,MAX_FILENAME_SIZE);
   else if (strcmp(name,"file2")==0) strncpy(data->config.file2,value,MAX_FILENAME_SIZE);
   else if (strcmp(name,"file3")==0) strncpy(data->config.file3,value,MAX_FILENAME_SIZE);
   else if (strcmp(name,"file4")==0) strncpy(data->config.file4,value,MAX_FILENAME_SIZE);
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
   strncpy(save_config.file1,data->config.file1,MAX_FILENAME_SIZE);
   strncpy(save_config.file2,data->config.file2,MAX_FILENAME_SIZE);
   strncpy(save_config.file3,data->config.file3,MAX_FILENAME_SIZE);
   strncpy(save_config.file4,data->config.file4,MAX_FILENAME_SIZE);
   save_config.reserved1   =htonl(data->config.reserved1);
   save_config.reserved2   =htonl(data->config.reserved2);
   save_config.reserved3   =htonl(data->config.reserved3);
   save_config.reserved4   =htonl(data->config.reserved4);
   save_config.reserved5   =data->config.reserved5;
   save_config.reserved6   =data->config.reserved6;
   save_config.reserved7   =data->config.reserved7;
   save_config.reserved8   =data->config.reserved8;

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
   strncpy(data->config.file1,save_config.file1,MAX_FILENAME_SIZE);
   strncpy(data->config.file2,save_config.file2,MAX_FILENAME_SIZE);
   strncpy(data->config.file3,save_config.file3,MAX_FILENAME_SIZE);
   strncpy(data->config.file4,save_config.file4,MAX_FILENAME_SIZE);
   strncpy(data->client[0].currFile,save_config.file1,MAX_FILENAME_SIZE);
   strncpy(data->client[1].currFile,save_config.file2,MAX_FILENAME_SIZE);
   strncpy(data->client[2].currFile,save_config.file3,MAX_FILENAME_SIZE);
   strncpy(data->client[3].currFile,save_config.file4,MAX_FILENAME_SIZE);
   data->config.reserved1=ntohl(save_config.reserved1);
   data->config.reserved2=ntohl(save_config.reserved2);
   data->config.reserved3=ntohl(save_config.reserved3);
   data->config.reserved4=ntohl(save_config.reserved4);
   data->config.reserved5=save_config.reserved5;
   data->config.reserved6=save_config.reserved6;
   data->config.reserved7=save_config.reserved7;
   data->config.reserved8=save_config.reserved8;
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

   data->config.version=1;
   data->config.length=sizeof(struct libio_config);

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



static void callErrorCallback(int idx,int callbackID)
{
   if (idx==0) m_oapc_io_callback(OAPC_NUM_IO1,callbackID);
   else if (idx==1) m_oapc_io_callback(OAPC_NUM_IO3,callbackID);
   else if (idx==2) m_oapc_io_callback(OAPC_NUM_IO5,callbackID);
   else if (idx==3) m_oapc_io_callback(OAPC_NUM_IO7,callbackID);
   else assert(0);
}



static void *loadLoop(void *arg)
{
   struct client_data   *client;
   wxImage              *image;
   wxMBConvUTF8          conv;
   wchar_t               wc[300];
   
   client=(struct client_data*)arg;
   conv.MB2WC(wc,client->currFile,300);
   image=new wxImage(wc,wxBITMAP_TYPE_ANY,-1);
   if (!image)
   {
      client->currErr=FILEERROR_NO_MEMORY;
      callErrorCallback(client->idx,client->m_callbackID);
      return NULL;
   }
   if (image->Ok())
   {
      client->currErr=0;
      client->bin=(struct oapc_bin_head*)malloc(sizeof(struct oapc_bin_head)+(3*image->GetWidth()*image->GetHeight()));
      if (!client->bin)
      {
         delete image;
         client->currErr=FILEERROR_NO_MEMORY;
         callErrorCallback(client->idx,client->m_callbackID);
         return NULL;
      }
      memset(client->bin,0,sizeof(struct oapc_bin_head));
      client->bin->version=OAPC_BIN_HEAD_CURR_VERSION;
      client->bin->param1=image->GetWidth();
      client->bin->param2=image->GetHeight();
      client->bin->param3=24;
      client->bin->type=OAPC_BIN_TYPE_IMAGE;
      client->bin->subType=OAPC_BIN_SUBTYPE_IMAGE_RGB24;
      client->bin->sizeData=3*image->GetWidth()*image->GetHeight();
      memcpy(&client->bin->data,image->GetData(),client->bin->sizeData);
      delete image;
   }
   else
   {
      client->currErr=FILEERROR_OTHER;
      delete image;
      callErrorCallback(client->idx,client->m_callbackID);
      return NULL;
   }
   
   if (client->idx==0) m_oapc_io_callback(OAPC_BIN_IO0,client->m_callbackID);
   else if (client->idx==1) m_oapc_io_callback(OAPC_BIN_IO2,client->m_callbackID);
   else if (client->idx==2) m_oapc_io_callback(OAPC_BIN_IO4,client->m_callbackID);
   else if (client->idx==3) m_oapc_io_callback(OAPC_BIN_IO6,client->m_callbackID);
   return NULL;
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
OAPC_EXT_API unsigned long oapc_set_digi_value(void *instanceData,unsigned long input,unsigned char value)
{
   struct instData *data;
   int              idx=input/2;

   data=(struct instData*)instanceData;
   if (value==1)
   {
      if (data->client[idx].bin) return OAPC_ERROR_STILL_IN_PROGRESS;
      data->client[idx].idx=idx;
      data->client[idx].m_callbackID=data->m_callbackID;
      if (!oapc_thread_create(loadLoop,&data->client[idx])) return OAPC_ERROR_NO_MEMORY;
   }
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
   int              idx=output/2;
   struct instData *data;

   data=(struct instData*)instanceData;

   if (data->client[idx].currErr!=data->client[idx].lastErr)
   {
      data->client[idx].lastErr=data->client[idx].currErr;
      *value=(float)data->client[idx].currErr;
      return OAPC_OK;
   }
   return OAPC_ERROR_NO_DATA_AVAILABLE;
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

   data=(struct instData*)instanceData;

   if (input%2==1) // filename
   {
      strncpy(data->client[idx].currFile,value,MAX_FILENAME_SIZE);
	  data->client[idx].currErr=0;
      return oapc_set_digi_value(instanceData,input-1,1);
   }
   else return OAPC_ERROR_NO_SUCH_IO;
}



/**
This function is called by the main application periodically in order to poll the state of the related
output. It has to return the data that are available for that input or - in case there are none available -
the error code OAPC_ERROR_NO_DATA_AVAILABLE to notify the main application, that there is nothing new.
@param[in] output specifies the output where the data are fetched from, here not the OAPC_BIN_IO...-flag is used
           but the plain, 0-based output number
@param[out] value if there are new data available, they have to be stored within that variable, if there are
           no new data, the returned data are ignored by the main application
@return an error code OAPC_ERROR_... in case of an error, OAPC_ERROR_NO_DATA_AVAILABLE in case no new data are
           available or OAPC_OK in case the value could be set
*/
OAPC_EXT_API unsigned long  oapc_get_bin_value(void *instanceData,unsigned long output,struct oapc_bin_head **value)
{
   struct instData      *data;
   int                   idx=output/2;
   
   data=(struct instData*)instanceData;
   
   if (!data->client[idx].bin) return OAPC_ERROR_NO_DATA_AVAILABLE;
   *value=data->client[idx].bin;
   data->client[idx].idx=0; 
   return OAPC_OK;     
}



OAPC_EXT_API void oapc_release_bin_data(void *instanceData,unsigned long output)
{
   int              idx=output/2;
   struct instData *data;

   data=(struct instData*)instanceData;
   	
   free(data->client[idx].bin);
   data->client[idx].bin=NULL;
}

