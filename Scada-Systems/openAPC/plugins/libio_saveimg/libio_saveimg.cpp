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
#endif

#ifndef ENV_WINDOWS
#include <arpa/inet.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#define closesocket close
#endif

#include "oapc_libio.h"

#define MAX_XML_SIZE       5000
#define MAX_FILENAME_SIZE   255
#define MAX_BUFSIZE        5000

struct libio_config
{
   unsigned short version,length;
   char           file1[MAX_FILENAME_SIZE+1],file2[MAX_FILENAME_SIZE+1],
                  file3[MAX_FILENAME_SIZE+1],file4[MAX_FILENAME_SIZE+1];
   unsigned char  format[4];
   unsigned int   reserved1,reserved2,reserved3,reserved4;
};



struct instData
{
   struct libio_config  config;
   char                 currFile[4][MAX_FILENAME_SIZE+1];
   char                 txt[4][MAX_BUFSIZE+1];
   int                  m_callbackID;
};



/** the xml data that define the behaviour and user interface of the plugin within the main application */
static char xmltempl[]="<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"?>\
<oapc-config>\
<flowimage>%s</flowimage>\
<dialogue>\
 <general>\
  <param>\
   <name>file1</name>\
   <text>Default Filename 1</text>\
   <type>filesave</type>\
   <default>%s</default>\
   <min>0</min>\
   <max>%d</max>\
   <ffilter>Image files|*.bmp;*.xbm;*.xpm;*.gif;*.png;*.jpg;*.jpeg|All files|*</ffilter>\
  </param>\
  <param>\
   <name>format1</name>\
   <text>File Format 1</text>\
   <type>option</type>\
   <value>PNG</value>\
   <value>GIF</value>\
   <value>PNM</value>\
   <value>XBM</value>\
   <value>XPM</value>\
   <value>BMP</value>\
   <value>JPEG</value>\
   <default>%d</default>\
  </param>\
  <param>\
   <name>file2</name>\
   <text>Default Filename 2</text>\
   <type>filesave</type>\
   <default>%s</default>\
   <min>0</min>\
   <max>%d</max>\
   <ffilter>Image files|*.bmp;*.xbm;*.xpm;*.gif;*.png;*.jpg;*.jpeg|All files|*</ffilter>\
  </param>\
  <param>\
   <name>format2</name>\
   <text>File Format 2</text>\
   <type>option</type>\
   <value>PNG</value>\
   <value>GIF</value>\
   <value>PNM</value>\
   <value>XBM</value>\
   <value>XPM</value>\
   <value>BMP</value>\
   <value>JPEG</value>\
   <default>%d</default>\
  </param>\
  <param>\
   <name>file3</name>\
   <text>Default Filename 3</text>\
   <type>filesave</type>\
   <default>%s</default>\
   <min>0</min>\
   <max>%d</max>\
   <ffilter>Image files|*.bmp;*.xbm;*.xpm;*.gif;*.png;*.jpg;*.jpeg|All files|*</ffilter>\
  </param>\
  <param>\
   <name>format3</name>\
   <text>File Format 3</text>\
   <type>option</type>\
   <value>PNG</value>\
   <value>GIF</value>\
   <value>PNM</value>\
   <value>XBM</value>\
   <value>XPM</value>\
   <value>BMP</value>\
   <value>JPEG</value>\
   <default>%d</default>\
  </param>\
  <param>\
   <name>file4</name>\
   <text>Default Filename 4</text>\
   <type>filesave</type>\
   <default>%s</default>\
   <min>0</min>\
   <max>%d</max>\
   <ffilter>Image files|*.bmp;*.xbm;*.xpm;*.gif;*.png;*.jpg;*.jpeg|All files|*</ffilter>\
  </param>\
  <param>\
   <name>format4</name>\
   <text>File Format 4</text>\
   <type>option</type>\n""\
   <value>PNG</value>\
   <value>GIF</value>\
   <value>PNM</value>\
   <value>XBM</value>\
   <value>XPM</value>\
   <value>BMP</value>\
   <value>JPEG</value>\
   <default>%d</default>\
  </param>\
 </general>\
  <helppanel>\
   <in0>IMG1 - data to be saved</in0>\
   <in1>FILE1 - filename to save the data</in1>\
   <in2>...</in2>\
  </helppanel>\
 </dialogue>\
</oapc-config>\n";
/** Base64-encoded PNG data of an image that is used as icon for the program flow diagram within the main application*/
static char                 flowImage[]="iVBORw0KGgoAAAANSUhEUgAAAGoAAAAyCAMAAACDIr4/AAADAFBMVEUEAgRMmhyEgoSw1ozEwsR0zjQk5myM2kRc6oyU8kTc6sys5oSn8Wl8jszG6K7s7uzMruQEArzkwvRMYozD/WvZ/qqd+0zMhuzc8syMzly89ozI8rDMZvwcnnyEwrTM1siU/mz0/uzam/rU/oTMfuxkwiz8/gQ0/nSE/oyM7lzsvvzx2fnq/cy2uLaU4lDU6rz08fT8AvyM6jyEAgSEkrx81FjM3rzEntwEGpy0wqyM5lx0vkyM6qR8/nTcFvzkr/y0/nS0hsza2drk5uBUqiTUkuzEzrzc/rzUbuzstuyf71mx/lyE0lTRe/zMzsyM/nzcivzU7rzC/pDs8tx0/pzU/nzs/tzMcvyB1jls8pSk9mSUzmz8/vzc/pR0yjTsyvwAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAADQdAOeAAACOklEQVR42u2WbVvaMBSGMyybOEbQiXPuDepEHQXCMnXuza2ZdgOtW2cpw+D+/79YWixNSwPiGr6sD194OLmu+5ycw2kBmKMopSBTrVYz7jfPtFqtwCQWGaIUwIJAGQYVwIKBSSwyRLmJ+Gl4ifAmqcg1iiXip+ElwpukItcoxh6lIcv4KCVIQ5bxUZRLQ5IZoeTLn0DAMUPGzUlgbocSlw+8T5y5LUp8gD8C/uXC59+reY4FDQ+CoCPiyEwoQfMjScSPRV2PyjTFKC7d8OSHuCD+wvWIR0QnpnDYRT270Z3pFK4vcXpxcgjHWCIEEJ4YP48Zqrk2Ui6Xa/SuCNXN6duCTiwkBoUZarXDqd24R2iUFTxEQr8JtlR8rzDGtGlv2vaqr367sU8ICc9GHCpSRWZCk7Cvpj0olQaDTU92efdd4/TxiVKfcd2K6g2hsD1YONzXNK2rlazz89290733pm7OtgPBpCMBCj9cfnt38eDDzs7HcnntdW/r653IwMdWdfOR4FB4+dn29ptvXVaZZhjGk0IBwqlVhTbC2P9ZhEIPfl9cPP/0veKq6wohfSpKeGcTeuVAbZ3VsmSUmCxPCJkJbvYRqeg46kqNGIZmWNZnV51OJVGUz0NF50xVj78crNR+Wf3+Rn4jn29DGSgdFZ866stHx+rRqz81Uvh5v5fNZiFSEkdRD+Wc/VDVI5dVQ6hSWLiUgGLbDkIUIygHFSsJqHrkCTyUomT+mzemFJWiUlSKSlHSUXPTX2EXK+S8FCIiAAAAAElFTkSuQmCC";
static char                 libname[]="Save Image";
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
   return OAPC_HAS_INPUTS|
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
   return OAPC_BIN_IO0|OAPC_CHAR_IO1|OAPC_BIN_IO2|OAPC_CHAR_IO3|
          OAPC_BIN_IO4|OAPC_CHAR_IO5|OAPC_BIN_IO6|OAPC_CHAR_IO7;
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

   sprintf(xmldescr,xmltempl,flowImage,data->config.file1,MAX_FILENAME_SIZE,data->config.format[0],
                                       data->config.file2,MAX_FILENAME_SIZE,data->config.format[1],
                                       data->config.file3,MAX_FILENAME_SIZE,data->config.format[2],
                                       data->config.file4,MAX_FILENAME_SIZE,data->config.format[3]);
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

   if (strcmp(name,"file1")==0)        strncpy(data->config.file1,value,MAX_FILENAME_SIZE);
   else if (strcmp(name,"file2")==0)   strncpy(data->config.file2,value,MAX_FILENAME_SIZE);
   else if (strcmp(name,"file3")==0)   strncpy(data->config.file3,value,MAX_FILENAME_SIZE);
   else if (strcmp(name,"file4")==0)   strncpy(data->config.file4,value,MAX_FILENAME_SIZE);
   else if (strcmp(name,"format1")==0) data->config.format[0]=atoi(value);
   else if (strcmp(name,"format2")==0) data->config.format[1]=atoi(value);
   else if (strcmp(name,"format3")==0) data->config.format[2]=atoi(value);
   else if (strcmp(name,"format4")==0) data->config.format[3]=atoi(value);
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
   strncpy(save_config.file1,data->config.file1,MAX_FILENAME_SIZE);
   strncpy(save_config.file2,data->config.file2,MAX_FILENAME_SIZE);
   strncpy(save_config.file3,data->config.file3,MAX_FILENAME_SIZE);
   strncpy(save_config.file4,data->config.file4,MAX_FILENAME_SIZE);
   for (i=0; i<4; i++) save_config.format[i]=data->config.format[i];
   save_config.reserved1   =htonl(data->config.reserved1);
   save_config.reserved2   =htonl(data->config.reserved2);
   save_config.reserved3   =htonl(data->config.reserved3);
   save_config.reserved4   =htonl(data->config.reserved4);

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
   strncpy(data->config.file1,save_config.file1,MAX_FILENAME_SIZE);
   strncpy(data->config.file2,save_config.file2,MAX_FILENAME_SIZE);
   strncpy(data->config.file3,save_config.file3,MAX_FILENAME_SIZE);
   strncpy(data->config.file4,save_config.file4,MAX_FILENAME_SIZE);
   strncpy(data->currFile[0],save_config.file1,MAX_FILENAME_SIZE);
   strncpy(data->currFile[1],save_config.file2,MAX_FILENAME_SIZE);
   strncpy(data->currFile[2],save_config.file3,MAX_FILENAME_SIZE);
   strncpy(data->currFile[3],save_config.file4,MAX_FILENAME_SIZE);
   for (i=0; i<4; i++) data->config.format[i]=save_config.format[i];
   data->config.reserved1=ntohl(save_config.reserved1);
   data->config.reserved2=ntohl(save_config.reserved2);
   data->config.reserved3=ntohl(save_config.reserved3);
   data->config.reserved4=ntohl(save_config.reserved4);
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
   int              i;

   data=(struct instData*)malloc(sizeof(struct instData));

   data->config.version=1;
   data->config.length=sizeof(struct libio_config);   
   data->config.file1[0]=0;  data->config.file2[0]=0;  data->config.file3[0]=0;  data->config.file4[0]=0;
   data->config.reserved1=0; data->config.reserved2=0; data->config.reserved3=0; data->config.reserved4=0;
   for (i=0; i<4; i++)
   {
      data->config.format[i]=1;
      data->currFile[i][0]=0;
   }
   data->m_callbackID=0;

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
      if (strncmp(data->currFile[idx],value,MAX_FILENAME_SIZE))
       strncpy(data->currFile[idx],value,MAX_FILENAME_SIZE);
      return OAPC_OK;
   }
   else return OAPC_ERROR_NO_SUCH_IO;
}



OAPC_EXT_API unsigned long  oapc_set_bin_value(void* instanceData,unsigned long input,struct oapc_bin_head *value)
{
   struct instData *data;
   wxImage         *img;
   int              idx=input/2;
   wxBitmapType     useFormat;
   wxMBConvUTF8     conv;
   wchar_t          wc[300];
   bool             saved;

   data=(struct instData*)instanceData;

   if (!value) return OAPC_ERROR_INVALID_INPUT;
   if ((value->type!=OAPC_BIN_TYPE_IMAGE) &&
       ((value->subType!=OAPC_BIN_SUBTYPE_IMAGE_RGB24) || (value->subType!=OAPC_BIN_SUBTYPE_IMAGE_GREY8))) return OAPC_ERROR_INVALID_INPUT;
   if (value->subType!=OAPC_BIN_SUBTYPE_IMAGE_GREY8)
   {
      unsigned char *src,*dest;
      int            cnt;

      img=new wxImage(value->param1,value->param2);
      if (!img) return OAPC_ERROR_NO_MEMORY;
      cnt=value->param1*value->param2;
      src=(unsigned char*)&value->data;
      dest=img->GetData();
      do
      {
         *dest=*src; dest++;
         *dest=*src; dest++;
         *dest=*src; dest++;
         src++;
         cnt--;
      }
      while (cnt>0);
   }
   else img=new wxImage(value->param1,value->param2,(unsigned char*)&value->data,true);
   if (!img) return OAPC_ERROR_NO_MEMORY; 
   conv.MB2WC(wc,data->currFile[idx],300);

   if (data->config.format[idx]==1) useFormat=wxBITMAP_TYPE_PNG;
   else if (data->config.format[idx]==2) useFormat=wxBITMAP_TYPE_GIF;
   else if (data->config.format[idx]==3) useFormat=wxBITMAP_TYPE_PNM;
   else if (data->config.format[idx]==4) useFormat=wxBITMAP_TYPE_XBM;
   else if (data->config.format[idx]==5) useFormat=wxBITMAP_TYPE_XPM;
   else if (data->config.format[idx]==6) useFormat=wxBITMAP_TYPE_BMP;
   else /*if (data->config.format[idx]==7)*/ useFormat=wxBITMAP_TYPE_JPEG;
   saved=img->SaveFile(wc,useFormat);
   delete img;
   if (saved) return OAPC_OK;
   else return OAPC_ERROR_RESOURCE;
}



