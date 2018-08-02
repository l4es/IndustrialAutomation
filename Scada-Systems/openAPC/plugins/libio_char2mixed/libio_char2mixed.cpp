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

#define OAPC_CHAR2MIXED_FLAG_USECLOCK 0x0001

struct libio_config
{
   unsigned short version,length;
   int            m_flags;
   char           m_fmtString[MAX_CHAR_SIZE+4];
};


struct instData
{
   struct libio_config config;
   int                 m_callbackID;
   char                m_inString[MAX_CHAR_SIZE+4];
   double              m_retNum;
   unsigned int        ssc_map[7];
   void               *ssc_var[7];
   float               f1,f2,f3,f4;
   char                s5[MAX_CHAR_SIZE+4],s6[MAX_CHAR_SIZE+4],s7[MAX_CHAR_SIZE+4];
   char               *m_retChar;
   char                m_useFmtString[MAX_CHAR_SIZE+4];
};


/** the xml data that define the behaviour and user interface of the plugin within the main application */
static char xmltempl[]="<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"?>\n\
<oapc-config>\n\
 <flowimage>%s</flowimage>\n\
 <dialogue>\n\
  <general>\n\
   <param>\n\
    <name>useclock</name>\n\
    <text>Convert on clock signal</text>\n\
    <type>checkbox</type>\n\
    <default>%d</default>\n\
   </param>\n\
   <param>\n\
    <name>fmtstring</name>\n\
    <text>Format string</text>\n\
    <type>string</type>\n\
    <default>%s</default>\n\
    <min>3</min>\n\
    <max>%d</max>\n\
   </param>\n\
  </general>\n\
  <helppanel>\n\
   <in0>CLK - Convert input data on clock signal</in0>\n\
   <in1>DATA - Input data</in1>\n\
   <out1>D1 - value corresponding to %%f1 out of input data</out1>\n\
   <out2>...</out2>\n\
   <out4>D4 - value corresponding to %%f4 out of input data</out4>\n\
   <out5>D5 - value corresponding to %%s5 out of input data</out5>\n\
   <out6>D6 - value corresponding to %%s6 out of input data</out6>\n\
   <out7>D7 - value corresponding to %%s7 out of input data</out7>\n\
  </helppanel>\n\
 </dialogue>\n\
</oapc-config>\n";
/** Base64-encoded PNG data of an image that is used as icon for the program flow diagram within the main application*/
static char                 flowImage[]="iVBORw0KGgoAAAANSUhEUgAAAGoAAAAyCAMAAACDIr4/AAAAA3NCSVQICAjb4U/gAAAAFVBMVEUEAgT///+EgoT8AvzEwsQAAAAE/vyugFJSAAAACXBIWXMAAA7EAAAOxAGVKw4bAAABK0lEQVRYhe2YwQ6EIAxEQVj//5O3yiIF20hSppe1B+OY6GNSpE1DcIwYY9josh13p0gpVbEwCiqfvFwe5UCoKpajDlvNB9kCmKoostV8kC2AqYoiJ8xHQJi6UJn7yAhTFyp2PhCmGgofdQdGkJU7ijhwd3KukKh6QjmgPIKhHF2ZUHLFkFDmXI0vZxVljuErdLSproSFGVDHIeqDOo9rF1QpDGqurDljb/9KkMO2qMUOj7rKKhzVCjgaxVoFMIo3JVhU1/5AUX2jhUQNLR0QNTaPONStTcWhbg0xEDVbhT3iRb2of0Z1f+CnxAohobZ939toZlKwCY4iBFQO9I02mpkUbIKjCClXtF4+mpkU6UlIKFovH81MivQkxB2oj2YsQkTpoxmLmJoYLoov4HsVWWwB0PYAAAAASUVORK5CYII=";
static char                 libname[]="Character to Mixed";
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
   return OAPC_DIGI_IO0|OAPC_CHAR_IO1;
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
   return OAPC_NUM_IO1|OAPC_NUM_IO2|OAPC_NUM_IO3|OAPC_NUM_IO4|OAPC_CHAR_IO5|OAPC_CHAR_IO6|OAPC_CHAR_IO7;
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

   sprintf(xmldescr,xmltempl,flowImage,((data->config.m_flags & OAPC_CHAR2MIXED_FLAG_USECLOCK)==OAPC_CHAR2MIXED_FLAG_USECLOCK),
                                        data->config.m_fmtString,MAX_CHAR_SIZE);
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
   if (strcmp(name,"fmtstring")==0)        strncpy(data->config.m_fmtString,value,MAX_CHAR_SIZE);
   else if (strcmp(name,"useclock")==0)
   {
      if (atoi(value)!=0) data->config.m_flags|=OAPC_CHAR2MIXED_FLAG_USECLOCK;
      else data->config.m_flags&=~OAPC_CHAR2MIXED_FLAG_USECLOCK;
   }

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
   save_config.version       =htons(1);
   save_config.length        =htons((unsigned short)*length);
   strncpy(save_config.m_fmtString,data->config.m_fmtString,MAX_CHAR_SIZE);
   save_config.m_flags=htonl(data->config.m_flags);
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
   strncpy(data->config.m_fmtString,save_config.m_fmtString,MAX_CHAR_SIZE);
   data->config.m_flags=ntohl(save_config.m_flags);
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

   flags=flags; // removing "unused" warning
   data=(struct instData*)malloc(sizeof(struct instData));
   if (!data) return NULL;
   memset(data,0,sizeof(struct instData));
   sprintf(data->config.m_fmtString,"%%f1 %%f2 %%f3 %%f4 %%s5 %%s6 %%s7");
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
   char            *c1,*c2;
   struct instData *data;
   int              ssc_cnt=0;
   bool             tokenFound;

   data=(struct instData*)instanceData;

   c1=data->config.m_fmtString;
   c2=data->m_useFmtString;
   while (*c1)
   {
      if ((ssc_cnt<7) && (*c1=='%'))
      {
         tokenFound=false;
         if (strstr(c1,"%f1")==c1)
         {
            data->ssc_var[ssc_cnt]=&data->f1;
            data->ssc_map[ssc_cnt]=OAPC_NUM_IO1;
            tokenFound=true;
         }
         else if (strstr(c1,"%f2")==c1)
         {
            data->ssc_var[ssc_cnt]=&data->f2;
            data->ssc_map[ssc_cnt]=OAPC_NUM_IO2;
            tokenFound=true;
         }
         else if (strstr(c1,"%f3")==c1)
         {
            data->ssc_var[ssc_cnt]=&data->f3;
            data->ssc_map[ssc_cnt]=OAPC_NUM_IO3;
            tokenFound=true;
         }
         else if (strstr(c1,"%f4")==c1)
         {
            data->ssc_var[ssc_cnt]=&data->f4;
            data->ssc_map[ssc_cnt]=OAPC_NUM_IO4;
            tokenFound=true;
         }
         else if (strstr(c1,"%s5")==c1)
         {
            data->ssc_var[ssc_cnt]=&data->s5;
            data->ssc_map[ssc_cnt]=OAPC_CHAR_IO5;
            tokenFound=true;
         }
         else if (strstr(c1,"%s6")==c1)
         {
            data->ssc_var[ssc_cnt]=&data->s6;
            data->ssc_map[ssc_cnt]=OAPC_CHAR_IO6;
            tokenFound=true;
         }
         else if (strstr(c1,"%s7")==c1)
         {
            data->ssc_var[ssc_cnt]=&data->s7;
            data->ssc_map[ssc_cnt]=OAPC_CHAR_IO7;
            tokenFound=true;
         }

         if (tokenFound)
         {
            ssc_cnt++;
            *c2=*c1; c2++; c1++; //%
            *c2=*c1; c2++; c1++; //s/f
            c1++;
         }
      }
      *c2=*c1;
      c1++;
      c2++;
   }
   *c2=0;
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



static void convertInput(struct instData *data)
{
   int    ssc_cnt=0,ret;
   float *d;

   ret=sscanf(data->m_inString,data->m_useFmtString,
              data->ssc_var[0],data->ssc_var[1],data->ssc_var[2],data->ssc_var[3],data->ssc_var[4],data->ssc_var[5],data->ssc_var[6]);
   if (ret>0)
   {
      for (ssc_cnt=0; ssc_cnt<7; ssc_cnt++)
      {
         if (data->ssc_map[ssc_cnt]!=0)
         {
            if ((data->ssc_map[ssc_cnt]>=OAPC_NUM_IO1) && (data->ssc_map[ssc_cnt]<=OAPC_NUM_IO4))
            {
               d=(float*)data->ssc_var[ssc_cnt];
               data->m_retNum=*d;
            }
            else if ((data->ssc_map[ssc_cnt]>=OAPC_CHAR_IO5) && (data->ssc_map[ssc_cnt]<=OAPC_CHAR_IO7))
             data->m_retChar=(char*)data->ssc_var[ssc_cnt];
            else assert(0);
            m_oapc_io_callback(data->ssc_map[ssc_cnt],data->m_callbackID);
         }
      }
   }

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

   data=(struct instData*)instanceData;
   if (input==0)
   {
      if (((data->config.m_flags & OAPC_CHAR2MIXED_FLAG_USECLOCK)==OAPC_CHAR2MIXED_FLAG_USECLOCK) && (value!=0))
       convertInput(data);
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
OAPC_EXT_API unsigned long oapc_get_num_value(void* instanceData,unsigned long output,double *value)
{
   struct instData *data;

   data=(struct instData*)instanceData;

   if ((output>=1) && (output<=4)) *value=data->m_retNum;
   else return OAPC_ERROR_NO_SUCH_IO;
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

   if (input==1)
   {
      strncpy(data->m_inString,value,MAX_CHAR_SIZE);
      if ((data->config.m_flags & OAPC_CHAR2MIXED_FLAG_USECLOCK)==0) convertInput(data);
   }
   else return OAPC_ERROR_NO_SUCH_IO; // check for valid IO

   return OAPC_OK;
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
OAPC_EXT_API unsigned long  oapc_get_char_value(void* instanceData,unsigned long output,unsigned long length,char *value)
{
   struct instData *data;

   data=(struct instData*)instanceData;

   if ((output>=5) && (output<=7)) strncpy(value,data->m_retChar,length);
   else return OAPC_ERROR_NO_SUCH_IO;
   return OAPC_OK;
}


