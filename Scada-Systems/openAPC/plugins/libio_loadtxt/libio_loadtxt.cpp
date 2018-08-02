/********************************************************************************************

These sources are distributed in the hope that they will be useful, but WITHOUT ANY WARRANTY;
without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. All
the information given here, within the interface descriptions and within the specification
are subject to change without notice. Errors and omissions excepted.

These sources demonstrate the usage of the OpenAPC Plug-In programming interface. They can be
used freely according to the OpenAPC Dual License: As long as the sources and the resulting
applications/libraries/Plug-Ins are used together with the OpenAPC software, they are
licensed as Freeware. When you use them outside the OpenAPC software they are licensed under
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



struct instData
{
   struct libio_config  config;
   FILE                *FHandle[4];
   char                 currFile[4][MAX_FILENAME_SIZE+1];
   int                  currErr[4],lastErr[4];
   char                 dataAvailable[4];
   char                 txt[4][MAX_BUFSIZE+1];
};



/** the xml data that define the behaviour and user interface of the plugin within the main application */
static char xmltempl[]={"<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"?>\n\
<oapc-config>\n\
 <flowimage>%s</flowimage>\n\
 <dialogue>\n\
  <general>\n\
   <param>\n\
    <name>file1</name>\n\
    <text>Default Filename 1</text>\n\
    <type>fileload</type>\n\
    <default>%s</default>\n\
    <min>0</min>\n\
    <max>%d</max>\n\
    <ffilter>Text files|*.txt;*.log|All files|*</ffilter>\n\
   </param>\n\
   <param>\n\
    <name>file2</name>\n\
    <text>Default Filename 2</text>\n\
    <type>fileload</type>\n\
    <default>%s</default>\n\
    <min>0</min>\n\
    <max>%d</max>\n\
    <ffilter>Text files|*.txt;*.log|All files|*</ffilter>\n\
   </param>\n\
   <param>\n\
    <name>file3</name>\n\
    <text>Default Filename 3</text>\n\
    <type>fileload</type>\n\
    <default>%s</default>\n\
    <min>0</min>\n\
    <max>%d</max>\n\
    <ffilter>Text files|*.txt;*.log|All files|*</ffilter>\n\
   </param>\n\
   <param>\n\
    <name>file4</name>\n\
    <text>Default Filename 4</text>\n\
    <type>fileload</type>\n\
    <default>%s</default>\n\
    <min>0</min>\n\
    <max>%d</max>\n\
    <ffilter>Text files|*.txt;*.log|All files|*</ffilter>\n\
   </param>\n\
  </general>\n\
  <helppanel>\n\
   <in0>CLK1 - load a single line of current file</in0>\n\
   <in1>FILE1 - set new file for loading</in1>\n\
   <out0>DATA1 - loaded data</out0>\n\
   <out1>ERR1 - error code when loading failed</out1>\n\
  </helppanel>\n\
 </dialogue>\n\
 </oapc-config>\n"};
/** Base64-encoded PNG data of an image that is used as icon for the program flow diagram within the main application*/
static char                flowImage[]="iVBORw0KGgoAAAANSUhEUgAAAGoAAAAyCAMAAACDIr4/AAADAFBMVEUEAgSEgoQE/vwcHrRMSqTEwsTo6OiLjfz8AvwkIrlnabH8/vy0ssQFA+PZ2NqYlvV0dvykorxEQvQ8Pu6cnvyEAgSIiLB2eKoMCtRcXvRMSswUFvS0svS0trTp6fwEAvQAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA5Xp0mAAAB6UlEQVR42tWYiZLCIAyGsbsUlbV7KOoelvd/y6VVCOkKDUed2cx4ZH7rF5JAO2HsgSalZCvzthq+jc56vQanmnJF8fErv4qcGRGcasoVNQRiwxgD8Z1ayg1lArFhjIH4Ti3lhjJsF8ZSjkVxCGMpx6KkF8ZCjkMtb7YDmcdETuxayTJQkeVHSSwDlZMQclCTBEovgYn/kboqlp+ZB7RF7qpK7SCEMB9t23bdjxB9FJXRhB5Ja/0m+7Z7OR4vX1rve1ICc1DCoERvlnQ67/mzvse6l8DkWikPtdUfI0qL0KpK+ltZlCHxNFTiqhSguo5fzCuKKuhEZU1/bzabp8HOUVS4JVg8tQpQ2Igo+tGhClHo6IjmVhWisiplUJ9g+ywUase/hQOU4GCnnUO5HM2iZgvnUK8XMA+V8GwxWzi7rwIouE1VONnjKHqtppvs3u/jKHvNPGqSwPDJHkoguVaMduab+5V4B9vu9CG5A1Puwr4dZPG+Sg+NXqvpvsogZe2rjGep+rWq8XA2OQKTU+iuITU7K3nuddckbuF/VSsYltRwkOfNYW7TGDRMaZomMGYhKUgCxU5j0DDFiIExC0lBEihuGoOGKU1wzEJSkOQUN41Bw5QmOGYhKUhyyjLTGOQtO41BnjeNeZj9Avd9zPGFqibuAAAAAElFTkSuQmCC";
static char                libname[]="Load Text";
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
   return OAPC_HAS_INPUTS|OAPC_HAS_OUTPUTS|
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
   return OAPC_CHAR_IO0|OAPC_NUM_IO1|OAPC_CHAR_IO2|OAPC_NUM_IO3|
          OAPC_CHAR_IO4|OAPC_NUM_IO5|OAPC_CHAR_IO6|OAPC_NUM_IO7;
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

   sprintf(xmldescr,xmltempl,flowImage,data->config.file1,MAX_FILENAME_SIZE,
                                       data->config.file2,MAX_FILENAME_SIZE,
                                       data->config.file3,MAX_FILENAME_SIZE,
                                       data->config.file4,MAX_FILENAME_SIZE);
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
   save_config.version  =htons(data->config.version);
   save_config.length   =htons(data->config.length);
   strncpy(save_config.file1,data->config.file1,MAX_FILENAME_SIZE);
   strncpy(save_config.file2,data->config.file2,MAX_FILENAME_SIZE);
   strncpy(save_config.file3,data->config.file3,MAX_FILENAME_SIZE);
   strncpy(save_config.file4,data->config.file4,MAX_FILENAME_SIZE);
   save_config.reserved1=htonl(data->config.reserved1);
   save_config.reserved2=htonl(data->config.reserved2);
   save_config.reserved3=htonl(data->config.reserved3);
   save_config.reserved4=htonl(data->config.reserved4);
   save_config.reserved5=data->config.reserved5;
   save_config.reserved6=data->config.reserved6;
   save_config.reserved7=data->config.reserved7;
   save_config.reserved8=data->config.reserved8;

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
   data->config.version  =ntohs(save_config.version);
   data->config.length   =ntohs(save_config.length);
   strncpy(data->config.file1,save_config.file1,MAX_FILENAME_SIZE);
   strncpy(data->config.file2,save_config.file2,MAX_FILENAME_SIZE);
   strncpy(data->config.file3,save_config.file3,MAX_FILENAME_SIZE);
   strncpy(data->config.file4,save_config.file4,MAX_FILENAME_SIZE);
   strncpy(data->currFile[0],save_config.file1,MAX_FILENAME_SIZE);
   strncpy(data->currFile[1],save_config.file2,MAX_FILENAME_SIZE);
   strncpy(data->currFile[2],save_config.file3,MAX_FILENAME_SIZE);
   strncpy(data->currFile[3],save_config.file4,MAX_FILENAME_SIZE);
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
   int              i;

   data=(struct instData*)malloc(sizeof(struct instData));

   data->config.version=1;
   data->config.length=sizeof(struct libio_config);
   data->config.file1[0]=0;  data->config.file2[0]=0;  data->config.file3[0]=0;  data->config.file4[0]=0;
   data->config.reserved1=0; data->config.reserved2=0; data->config.reserved3=0; data->config.reserved4=0;
   data->config.reserved5=0; data->config.reserved6=0; data->config.reserved7=0; data->config.reserved8=0;
   for (i=0; i<4; i++)
   {
      data->currFile[i][0]=0;
      data->FHandle[i]=NULL;
      data->currErr[i]=0;
      data->lastErr[i]=0;
      data->dataAvailable[i]=0;
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



/**
This function is called before the application unloads everything, it has to be used to deinitialize
everything and to release used resources.
*/
OAPC_EXT_API unsigned long oapc_exit(void* instanceData)
{
   struct instData *data;
   int              i;

   data=(struct instData*)instanceData;
   for (i=0; i<4; i++)
    if (data->FHandle[i])
     fclose(data->FHandle[i]);
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
OAPC_EXT_API unsigned long  oapc_set_digi_value(void *instanceData,unsigned long input,unsigned char value)
{
   struct instData *data;
   int              idx=input/2;

   data=(struct instData*)instanceData;
   if (((input==0) || (input==2) || (input==4) || (input==6)) && (value==1))
   {
      if (!data->FHandle[idx])
      {
         if (strlen(data->currFile[idx])<1)
         {
            data->currErr[idx]=FILEERROR_NO_FILENAME;
            return OAPC_OK;
         }
         data->FHandle[idx]=fopen(data->currFile[idx],"r");
         if (!data->FHandle[idx]) data->currErr[idx]=FILEERROR_NOT_OPENED;
      }
      if (data->FHandle[idx])
      {
         if (fgets(data->txt[idx],MAX_BUFSIZE,data->FHandle[idx]))
         {
             data->dataAvailable[idx]=1;
             if (strlen(data->txt[idx])>0)
             {
                while (data->txt[idx][strlen(data->txt[idx])-1]=='\n') data->txt[idx][strlen(data->txt[idx])-1]=0;
                while (data->txt[idx][strlen(data->txt[idx])-1]=='\r') data->txt[idx][strlen(data->txt[idx])-1]=0;
             }
         }
         else
          data->currErr[idx]=FILEERROR_EOF;
         return OAPC_OK;
      }   
   }
   return OAPC_ERROR_NO_SUCH_IO;
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
OAPC_EXT_API unsigned long oapc_get_num_value(void* instanceData,unsigned long output,double* value)
{
   int              idx=output/2;
   struct instData *data;

   data=(struct instData*)instanceData;

   if (data->currErr[idx]!=data->lastErr[idx])
   {
      data->lastErr[idx]=data->currErr[idx];
      *value=(float)data->currErr[idx];
      return OAPC_OK;
   }

   return OAPC_ERROR_NO_DATA_AVAILABLE; // this library does not support outputs
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
//      if (strncmp(data->currFile[idx],value,MAX_FILENAME_SIZE)) reset line counter every time a file is set
      {
         strncpy(data->currFile[idx],value,MAX_FILENAME_SIZE);
         if (data->FHandle[idx]) fclose(data->FHandle[idx]);
         data->FHandle[idx]=NULL;
		 data->currErr[idx]=0;
      }
      return OAPC_OK;
   }
   else return OAPC_ERROR_NO_SUCH_IO;
}



/**
This function is called by the main application periodically in order to poll the state of the related
output. It has to return the data that are available for that input or - in case there are none available -
the error code OAPC_ERROR_NO_DATA_AVAILABLE to notify the main application, that there is nothing new.
@param[in] output specifies the output where the data are fetched from, here not the OAPC_CHAR_IO...-flag is used
           but the plain, 0-based output number
@param[out] value if there are new data available, they are stored within that variable, if there are no new
           data, the returned data are ignored by the main application
@return an error code OAPC_ERROR_... in case of an error, OAPC_ERROR_NO_DATA_AVAILABLE in case no new data are
           available or OAPC_OK in case the value could be set
*/
OAPC_EXT_API unsigned long  oapc_get_char_value(void *instanceData,unsigned long output,unsigned long length,char* value)
{
   struct instData *data;
   int              idx=output/2;

   data=(struct instData*)instanceData;
   if (data->dataAvailable[idx])
   {
      strncpy(value,data->txt[idx],length);
      data->dataAvailable[idx]=0;
      return OAPC_OK;
   }
   return OAPC_ERROR_NO_DATA_AVAILABLE; // this library does not support outputs
}


