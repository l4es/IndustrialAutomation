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
 #define CONST const
 #include <windows.h>
 #include <ceddk.h>
#endif

#ifndef ENV_WINDOWS
 #include <sys/io.h>
 #include <arpa/inet.h>
 #include <errno.h>
 #include <unistd.h>
 #include <fcntl.h>
#else
 #include <winsock2.h>
 #define snprintf _snprintf
#endif

#ifdef ENV_LINUX
 #include <termios.h>
 #include <unistd.h>
#endif

#ifdef ENV_QNX
 #include <hw/inout.h>
 #include <sys/mman.h>
#endif

#include "oapc_libio.h"
#include "liboapc.h"

#define MAX_XML_SIZE   2500

struct instData
{
#ifdef ENV_LINUX
   FILE *digiInFH[MAX_NUM_IOS];
   FILE *digiOutFH[4]; // output 1,2, led red,green
   void *threadHandle;
   bool  running;
   char  digiIn[MAX_NUM_IOS];
#endif
   int   m_callbackID;
};


/** the xml data that define the behaviour and user interface of the plugin within the main application */
static char xmltempl[]="<?xml version=\"1.0\" encoding=\"UTF-8\"?>\
<oapc-config>\
<flowimage>%s</flowimage>\
 <dialogue>\
  <helppanel>\
   <in0>1 - output 1</in0>\
   <in1>2 - output 2</in1>\
   <in6>RD - red LED</in6>\
   <in7>GN - green LED</in7>\
  </helppanel>\
 </dialogue>\
</oapc-config>";
/** Base64-encoded PNG data of an image that is used as icon for the program flow diagram within the main application*/
static char                 flowImage[]="iVBORw0KGgoAAAANSUhEUgAAAGoAAAAyCAMAAACDIr4/AAADAFBMVEUEAgQE/gQ8PjyEgoTEwsRcXlykpqTk5uTsHiQkJiRMTkxsbmy0trT09vSUlpQUFhRERkTU0tRkZmSur67y8fI0MjRWV1YMDgyMjozc3tzMyswsLix0dnS8vrz8/vycnpwcHhw8OjxEQkRkYmSsqqzs6uxUUlR0cnS8urz8+vycmpxMSkxsamwAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAABNYX/gAAAACXBIWXMAAA7EAAAOxAGVKw4bAAACXklEQVR42u2Y7XaiQAyGXy2I0wpKEavCZv1YXK3e//U1GUCZHjisA3X/mHMqCU7zzJtJ6TDAA42I8MIfL+J9D2ysIVuOcrTr5F8agY01ZMtRwr7pMAJLWTXZChSzbzqMwFJWTbYCxWxUZ9VJVEO2EuVUdTjdRDVkK1Fk6Ogoqj7bFfXzVnYgqkx01WUIuLZDeTUEoxsK4/H41uyDwQBNBeyKEtKVJaScVY8iPAjVef0fh7pjrXrr6/oOfNzf1RNVMS8rHD+bF948O+nrKcvsUcCO6AhsiVIgpOzAv/wxkWe3Yi/x2Aum7C2Iwpiv04kt6gNDIk6RiBvTStLzz4k+maj0/YN+lr7LhLSFlqgNqwolP00kyQZY8b9yhJ4WylRaMtPJorVkXfrvgGuJSljVDGco2rEoxn2SwzxyAV+UKUkmVaRAF9HXQq1QW8wIm5FO6QhAW0DlJs/jkp71yBmw5gYBz8gO9YrFBcs1VCCz5UXbJOdlnkIlbkQUAakeqbS6yL6Ae+x4sAOlcCQuIqS/T5JC12kuzbDTTqxVTWFuUe5A/eWxe0bJqpC0gBqNXKX1HSbB9iIVwzlVI2mWacrShrbNzglYyJo/o6JKbDPSXZnfTLWzkmYVS3xblOPuef/tufu3PF4O48NveTR4l3jr/pJbf2bx4lh89xo9H7dP1BP1bc9p7HWb3jb+aZtfJmjdSJtB9S3i3mF1byLNOch0G1G1w+rfRKjKJWO6Zu1RuxJmoXD6T1vO22HJDwZtpzE9Bm2nMT0Gracx/QWtpzH9Be2nMb0F7acxvQWPPHj8An0+9PfBwig+AAAAAElFTkSuQmCC";
static char                 libname[]="Weecoboard 4M IO";
static char                 xmldescr[MAX_XML_SIZE+1];
static lib_oapc_io_callback m_oapc_io_callback; // callback function that is used to inform the main function about changes at the IO ports
#ifdef ENV_LINUX
static const char          *inPath[MAX_NUM_IOS]={"/sys/kernel/lgw4m-8di/in0_value","/sys/kernel/lgw4m-8di/in1_value",
                                                 "/sys/kernel/lgw4m-8di/in2_value","/sys/kernel/lgw4m-8di/in3_value",
                                                 "/sys/kernel/lgw4m-8di/in4_value","/sys/kernel/lgw4m-8di/in5_value",
                                                 "/sys/kernel/lgw4m-8di/in6_value","/sys/kernel/lgw4m-8di/in7_value"};
#endif

/**
This function has to return the name that is used to display this library within the main application.
The name returned here has to be short and descriptive. It is displayed within the popup menu of the flow
editor, therefore it should not be too long!
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
   return OAPC_DIGI_IO0|OAPC_DIGI_IO1|OAPC_DIGI_IO6|OAPC_DIGI_IO7;
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
   return OAPC_DIGI_IO0|OAPC_DIGI_IO1|OAPC_DIGI_IO2|OAPC_DIGI_IO3|OAPC_DIGI_IO4|OAPC_DIGI_IO5|OAPC_DIGI_IO6|OAPC_DIGI_IO7;
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


#ifdef ENV_LINUX
void *pollLoop(void *arg)
{
   struct instData *data;
   int              i;
   char             raw[1+4];
   char             value;
   bool             valueChanged;

   data=(struct instData*)arg;

   data->running=true;
   raw[1]=0;
   while (data->running)
   {
      valueChanged=false;
      for (i=0; i<MAX_NUM_IOS; i++)
      {
         if (!data->digiInFH[i]) data->digiInFH[i]=fopen(inPath[i],"rt");
         if (data->digiInFH[i])
         {
             fread(raw,1,1,data->digiInFH[i]);
             fclose(data->digiInFH[i]);
             data->digiInFH[i]=NULL;
             if (raw[0]=='0') value=0;
             else if (raw[0]=='1') value=1;
             else continue;
             if (value!=data->digiIn[i])
             {
                valueChanged=true;
                data->digiIn[i]=value;
                switch (i)
                {
                   default:
                      assert(0);
                   case 0:
                      m_oapc_io_callback(OAPC_DIGI_IO0,data->m_callbackID);
                      break;
                   case 1:
                      m_oapc_io_callback(OAPC_DIGI_IO1,data->m_callbackID);
                      break;
                   case 2:
                      m_oapc_io_callback(OAPC_DIGI_IO2,data->m_callbackID);
                      break;
                   case 3:
                      m_oapc_io_callback(OAPC_DIGI_IO3,data->m_callbackID);
                      break;
                   case 4:
                      m_oapc_io_callback(OAPC_DIGI_IO4,data->m_callbackID);
                      break;
                   case 5:
                      m_oapc_io_callback(OAPC_DIGI_IO5,data->m_callbackID);
                      break;
                   case 6:
                      m_oapc_io_callback(OAPC_DIGI_IO6,data->m_callbackID);
                      break;
                   case 7:
                      m_oapc_io_callback(OAPC_DIGI_IO7,data->m_callbackID);
                      break;
                }
             }
         }   
      }
      if (valueChanged) oapc_thread_sleep(100);
      else oapc_thread_sleep(200);
   }
   data->running=true;
   return NULL;
}
#endif


/**
When this function is called everything has to be initialized in order to perform the required operation
@return a return value/error code that informs the main application if the initialization was done successfully
        or not
*/
OAPC_EXT_API unsigned long oapc_init(void* instanceData)
{
#ifdef ENV_LINUX
   struct instData *data;
   int              i;
   FILE            *FHandle;

   data=(struct instData*)instanceData;

   for (i=0; i<MAX_NUM_IOS; i++)
   {
      data->digiInFH[i]=fopen(inPath[i],"rt");
      if (!data->digiInFH[i]) 
      {
         printf("Error accessing %s\n",inPath[i]);
         return OAPC_ERROR_RESOURCE;
      }
   }

   FHandle=fopen("/sys/class/leds/led_green/trigger","wt");
   if (!FHandle) 
   {
      printf("Error accessing /sys/class/leds/led_green/trigger\n");
      return OAPC_ERROR_RESOURCE;
   }
   fprintf(FHandle,"backlight\n");
   fclose(FHandle);

   FHandle=fopen("/sys/class/leds/led_orange/trigger","wt");
   if (!FHandle)
   {
      printf("Error accessing /sys/class/leds/led_orange/trigger\n");
      return OAPC_ERROR_RESOURCE;
   }
   fprintf(FHandle,"backlight\n");
   fclose(FHandle);

   data->digiOutFH[0]=fopen("/sys/class/leds/output1/brightness","wt");
   if (!data->digiOutFH[0])
   {
      printf("Error accessing /sys/class/leds/output1/brightness\n");
      return OAPC_ERROR_RESOURCE;
   }
   data->digiOutFH[1]=fopen("/sys/class/leds/output2/brightness","wt");
   if (!data->digiOutFH[1])
   {
      printf("Error accessing /sys/class/leds/output2/brightness\n");
      return OAPC_ERROR_RESOURCE;
   }
   data->digiOutFH[2]=fopen("/sys/class/leds/led_orange/brightness","wt");
   if (!data->digiOutFH[2])
   {
      printf("Error accessing /sys/class/leds/led_orange/brightness\n");
      return OAPC_ERROR_RESOURCE;
   }
   data->digiOutFH[3]=fopen("/sys/class/leds/led_green/brightness","wt");
   if (!data->digiOutFH[3])
   {
      printf("Error accessing /sys/class/leds/led_green/brightness\n");
      return OAPC_ERROR_RESOURCE;
   }

   data->threadHandle=oapc_thread_create(pollLoop,instanceData);
   if (!data->threadHandle) return OAPC_ERROR_NO_MEMORY;

   return OAPC_OK;
#else
   instanceData=instanceData;
   return OAPC_ERROR_DEVICE;
#endif
}



/**
This function is called before the application unloads everything, it has to be used to deinitialize
everything and to release used resources.
*/
OAPC_EXT_API unsigned long oapc_exit(void *instanceData)
{
#ifdef ENV_LINUX
   struct instData *data;
   int              i;

   data=(struct instData*)instanceData;

   if (data->running)
   {
       data->running=false;
       i=0;
       while ((!data->running) && (i<10)) // wait for thread to finish
       {
           oapc_thread_sleep(250);
           i++;
       }
       data->running=false;
   }
   if (data->threadHandle) oapc_thread_release(data->threadHandle);

   for (i=0; i<8; i++)
    if (data->digiInFH[i]) fclose(data->digiInFH[i]);
   for (i=0; i<4; i++)
    if (data->digiOutFH[i]) fclose(data->digiOutFH[i]);
#else
   instanceData=instanceData;
#endif
   return OAPC_OK;
}


#ifdef ENV_LINUX
static void writeDigiOut(struct instData *data,int idx, unsigned char value)
{
   if (value)
   {
      if (idx<2) fprintf(data->digiOutFH[idx],"1");
      else fprintf(data->digiOutFH[idx],"255");
   }
   else fprintf(data->digiOutFH[idx],"0");
   fflush(data->digiOutFH[idx]);
}
#endif


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
#ifdef ENV_LINUX
   struct instData *data;

   data=(struct instData*)instanceData;
   
   if (input==0) writeDigiOut(data,0,value);
   else if (input==1) writeDigiOut(data,1,value);
   else if (input==6) writeDigiOut(data,2,value);
   else if (input==7) writeDigiOut(data,3,value);
   else return OAPC_ERROR_NO_SUCH_IO;
   return OAPC_OK;
#else
   instanceData=instanceData;
   input=input;
   value=value;
   return OAPC_ERROR;
#endif
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
OAPC_EXT_API unsigned long  oapc_get_digi_value(void* instanceData,unsigned long output,unsigned char *value)
{
#ifdef ENV_LINUX
   struct instData *data;
  
   data=(struct instData*)instanceData;
   *value=data->digiIn[output];
   return OAPC_OK;
#else
   instanceData=instanceData;
   output=output;
   value=value;
   return OAPC_ERROR;
#endif
}


