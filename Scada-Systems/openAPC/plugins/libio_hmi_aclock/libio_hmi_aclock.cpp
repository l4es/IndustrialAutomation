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



/********************************************************************************************

This control was inspired by:

// Name:        wxAnalogSaat.cpp
// Purpose:     Analog clock
// Author:      emarti, Murat Özdemir  e-mail: dtemarti<add>gmail<dot>com
// Created:     15.04.2006
// Copyright:   (c) emarti
// Licence:     wxWindows license

*********************************************************************************************/


#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifndef ENV_WINDOWSCE
#include <sys/types.h>
#endif
#include <time.h>

#include <wx/wx.h>
#include <wx/dcbuffer.h>
#include "wx/mstream.h"

#if defined (ENV_LINUX) || defined (ENV_QNX)
#include <sys/io.h>
#include <arpa/inet.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#else
#include <wx/msw/private.h>

#ifdef ENV_WINDOWSCE
#include <winsock2.h>
#endif
#endif

#ifdef ENV_LINUX
#include <termios.h>
#include <unistd.h>
#endif

#include "oapc_libio.h"
#include "images.h"



#define MAX_XML_SIZE   3500

#pragma pack(8)

struct hmiConfigData
{
   wxUint16 length,version;
   wxUint32 m_hourColour[2],m_minColour[2],m_secColour[2];
   wxUint32 m_hourSize[2],m_minSize[2],m_secSize[2];
   wxUint8  m_type;
};



struct instData
{
   struct hmiConfigData  hmiData;
   wxBitmap             *m_bitmap;
   wxFloat64             cx,cy;
   wxFloat32             xpre;
   wxUint8               digi,prevDigi; // digi IO 0
   wxFloat64             num,prevNum;   // num IO 7
};



/** the xml data that define the behaviour and HMI configuration panels within the main application */
static char xmlhmitempl[]="<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"?>\n\
<oapc-config>\n\
<dialogue>\n\
 <dualpanel>\n\
  <name>Colours</name>\n\
  <param>\n\
   <name>hourColour0</name>\n\
   <text>Hour Hand</text>\n\
   <type>colorbutton</type>\n\
   <default>%d</default>\n\
  </param>\n\
  <param>\n\
   <name>hourColour1</name>\n\
   <type>colorbutton</type>\n\
   <default>%d</default>\n\
  </param>\n\
  <param>\n\
   <name>hourSize0</name>\n\
   <text>Size</text>\n\
   <type>integer</type>\n\
   <default>%d</default>\n\
   <min>1</min>\n\
   <max>6</max>\n\
  </param>\n\
  <param>\n\
   <name>hourSize1</name>\n\
   <type>integer</type>\n\
   <default>%d</default>\n\
   <min>1</min>\n\
   <max>6</max>\n\
  </param>\n\
  <param>\n\
   <name>minColour0</name>\n\
   <text>Minute Hand</text>\n\
   <type>colorbutton</type>\n\
   <default>%d</default>\n\
  </param>\n\
  <param>\n\
   <name>minColour1</name>\n\
   <type>colorbutton</type>\n\
   <default>%d</default>\n\
  </param>\n\
  <param>\n\
   <name>minSize0</name>\n\
   <text>Size</text>\n\
   <type>integer</type>\n\
   <default>%d</default>\n\
   <min>1</min>\n\
   <max>6</max>\n\
  </param>\n\
  <param>\n\
   <name>minSize1</name>\n\
   <type>integer</type>\n\
   <default>%d</default>\n\
   <min>1</min>\n\
   <max>6</max>\n\
  </param>\n\
  <param>\n\
   <name>secColour0</name>\n\
   <text>Second Hand</text>\n\
   <type>colorbutton</type>\n\
   <default>%d</default>\n\
  </param>\n\
  <param>\n\
   <name>secColour1</name>\n\
   <type>colorbutton</type>\n\
   <default>%d</default>\n\
  </param>\n\
  <param>\n\
   <name>secSize0</name>\n\
   <text>Size</text>\n\
   <type>integer</type>\n\
   <default>%d</default>\n\
   <min>1</min>\n\
   <max>6</max>\n\
  </param>\n\
  <param>\n\
   <name>secSize1</name>\n\
   <type>integer</type>\n\
   <default>%d</default>\n\
   <min>1</min>\n\
   <max>6</max>\n\
  </param>\n\
  <param>\n\
   <name>type</name>\n\
   <text>Type</text>\n\
   <type>option</type>\n\
   <value>Plain Clock</value>\n\
   <value>Plain clock w/o Minutes</value>\n\
   <value>Glow clock w/o Minutes</value>\n\
   <default>%d</default>\n\
  </param>\n\
 </dualpanel>\n\
</dialogue>\n\
</oapc-config>\n";

static char                  libname[]="Analogue Clock";
static char                  xmlhmidescr[MAX_XML_SIZE+1];

static struct hmiConfigData  saveConfig;



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
          OAPC_HAS_XML_CONFIGURATION|           // for the custom HMI configuration panel
          OAPC_HAS_STANDARD_FLOW_CONFIGURATION| // for the standard Flow configuration panel
          OAPC_HAS_LOG_TYPE_FLOATNUM|           // to let the main application log the change of data
          OAPC_ACCEPTS_PLAIN_CONFIGURATION|
          OAPC_HMICAT_DISPLAY|
          OAPC_USERPRIVI_HIDE;
}



/**
This function returns a set of OAPC_HMI_NO_... flags that modify the general configuration dialogue
within the HMI Editor. These flags decide which parts of that dialogue have NOT to be accessible and
can be taken from oapc_libio.h
@return or-concatenated flags that describe which user interface properties are not supported by this
        type of HMI object
*/
OAPC_EXT_API unsigned long oapc_get_no_ui_flags(void)
{
   return OAPC_HMI_NO_UI_RO|OAPC_HMI_NO_UI_TEXT|OAPC_HMI_NO_UI_FG|OAPC_HMI_NO_UI_MINMAX;
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
   return  OAPC_DIGI_IO0|OAPC_NUM_IO7;
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
   return OAPC_DIGI_IO0|OAPC_NUM_IO7;
   // avoid it to submit the same IO number for different data types, that would cause displaying problems for the flow symbol
}



/**
When the OAPC_HAS_XML_CONFIGURATION capability flag was set this function is called to retrieve an XML
structure that describes which configuration information have to be displayed within the main application.
When there are no extended flow configuration possibilities but the flag was set in order to provide
extended HMI possibilities (please see function below) this function has to return NULL.
@return an char-array that contains the XML data; this char array has to be released only when the 
        library is unloaded!
*/
OAPC_EXT_API char *oapc_get_config_data(void* WXUNUSED(instanceData))
{
   // here we're using a standard configuration dialogue so no XML document is returned for the custom flow dialogue
   return NULL;
}



/**
This function has to return a XML structure that describes additional configuration possibilities beside the standard
HMI properties. Within that XML structure several panels can be configured that are displayed as additional tab panes
when the user is editing the configuration of an element within the HMI editor. This function is called only when the
OAPC_HAS_XML_CONFIGURATION flag is set. When there are no extended HMI configuration possibilities but the flag was
set in order to provide extended flow possibilities (please see function above) this function has to return NULL.
@return an char-array that contains the XML data; this char array has to be released only when the 
        library is unloaded!
*/
OAPC_EXT_API char *oapc_get_hmi_config_data(void* instanceData)
{
   struct instData *data;

   data=(struct instData*)instanceData;

   sprintf(xmlhmidescr,xmlhmitempl,data->hmiData.m_hourColour[0],data->hmiData.m_hourColour[1],data->hmiData.m_hourSize[0],data->hmiData.m_hourSize[1],
                                   data->hmiData.m_minColour[0], data->hmiData.m_minColour[1], data->hmiData.m_minSize[0], data->hmiData.m_minSize[1],
                                   data->hmiData.m_secColour[0], data->hmiData.m_secColour[1], data->hmiData.m_secSize[0], data->hmiData.m_secSize[1],
                                   data->hmiData.m_type);
   return xmlhmidescr;
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

   if (strcmp(name,"hourColour0")==0)       data->hmiData.m_hourColour[0]=atoi(value);
   else if (strcmp(name,"hourColour1")==0)  data->hmiData.m_hourColour[1]=atoi(value);
   else if (strcmp(name,"hourSize0")==0)    data->hmiData.m_hourSize[0]=atoi(value);
   else if (strcmp(name,"hourSize1")==0)    data->hmiData.m_hourSize[1]=atoi(value);
   else if (strcmp(name,"minColour0")==0)   data->hmiData.m_minColour[0]=atoi(value);
   else if (strcmp(name,"minColour1")==0)   data->hmiData.m_minColour[1]=atoi(value);
   else if (strcmp(name,"minSize0")==0)     data->hmiData.m_minSize[0]=atoi(value);
   else if (strcmp(name,"minSize1")==0)     data->hmiData.m_minSize[1]=atoi(value);
   else if (strcmp(name,"secColour0")==0)   data->hmiData.m_secColour[0]=atoi(value);
   else if (strcmp(name,"secColour1")==0)   data->hmiData.m_secColour[1]=atoi(value);
   else if (strcmp(name,"secSize0")==0)     data->hmiData.m_secSize[0]=atoi(value);
   else if (strcmp(name,"secSize1")==0)     data->hmiData.m_secSize[1]=atoi(value);
   else if (strcmp(name,"type")==0)         data->hmiData.m_type=atoi(value);
   if (data->m_bitmap) delete data->m_bitmap;
   data->m_bitmap=NULL;
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

   *length=sizeof(struct hmiConfigData);
   saveConfig.version          =htons(data->hmiData.version);
   saveConfig.length           =htons(data->hmiData.length);
   for (i=0; i<2; i++)
   {
      saveConfig.m_hourColour[i]=htonl(data->hmiData.m_hourColour[i]);  
      saveConfig.m_hourSize[i]  =htonl(data->hmiData.m_hourSize[i]);  
      saveConfig.m_minColour[i] =htonl(data->hmiData.m_minColour[i]);  
      saveConfig.m_minSize[i]   =htonl(data->hmiData.m_minSize[i]);  
      saveConfig.m_secColour[i] =htonl(data->hmiData.m_secColour[i]);  
      saveConfig.m_secSize[i]   =htonl(data->hmiData.m_secSize[i]);  
   }
   saveConfig.m_type=data->hmiData.m_type;

   return (char*)&saveConfig;
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

   if (length>sizeof(struct hmiConfigData)) length=sizeof(struct hmiConfigData);
   memcpy(&saveConfig,loadedData,length);

   for (i=0; i<2; i++)
   {
      data->hmiData.m_hourColour[i]=ntohl(saveConfig.m_hourColour[i]);
      data->hmiData.m_hourSize[i]  =ntohl(saveConfig.m_hourSize[i]);
      data->hmiData.m_minColour[i]=ntohl(saveConfig.m_minColour[i]);
      data->hmiData.m_minSize[i]  =ntohl(saveConfig.m_minSize[i]);
      data->hmiData.m_secColour[i]=ntohl(saveConfig.m_secColour[i]);
      data->hmiData.m_secSize[i]  =ntohl(saveConfig.m_secSize[i]);
   }
   data->hmiData.m_type=saveConfig.m_type;
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

   data->m_bitmap=NULL;
   data->hmiData.length=sizeof(struct hmiConfigData);
   data->hmiData.version=1;
   data->hmiData.m_type=1;
   for (i=0; i<2; i++)
   {
      data->hmiData.m_hourColour[i]=0x252525;
      data->hmiData.m_hourSize[i]=3;
      data->hmiData.m_minColour[i] =0x252525;
      data->hmiData.m_minSize[i]=2;
      data->hmiData.m_secColour[i] =0x0000FF;
      data->hmiData.m_secSize[i]=1;
   }
   data->digi=0;   data->prevDigi=0;
   data->num=3750; data->prevNum=0;
   data->cx=0;     data->cy=0;
   data->xpre=0;

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

   data->num=0;

   return OAPC_OK;
}



static int modulal(int val)
{
   do{ 
      val +=360;
   }while(val<0);
   return val;
}

static int kactane(int x, int y)
{
   int times=0;
   do{ 
      x-=y;
      times++;
   }while(x>=y);
   return times;
}

static double sm_angle(int m)
{
   // Values of m must be minute or second. 
   return ((modulal((90 - (m * 6))) * M_PI) / 180);
}

static double h_angle(int m, int n)
{
   // Values of m must be hour, values of n must be minute.
   return ((modulal((90 - (m * 30) - (kactane(n, 6) * 3))) * M_PI) / 180);
}



/**
This function is called by the application whenever the element has to be repainted. That may happen
because a new value was set that has to be displayed or because the element was hidden and has to be
redrawn now. So the Plug-In does not take care about the states and conditions under which a repaint
is necessary, this function is called automatically.
@param[in] dc the wxWidgets drawing context structure that has to be used for painting the HMI element
@param[in] canvas the wxWindow element that was created by the main application in order to allocate
           a private area of defined size for the Plug-In to draw within
*/
OAPC_EXT_API void oapc_paint(void* instanceData,wxAutoBufferedPaintDC *dc,wxPanel *canvas)
{
   struct instData *data;
   wxFloat32        x,y,z;

   data=(struct instData*)instanceData;

   if ((!data->m_bitmap) || (canvas->GetSize().x!=data->m_bitmap->GetWidth()))
   {
      wxImage              img;
      wxMemoryInputStream *stream;

      if (data->m_bitmap) delete data->m_bitmap;
      if (data->hmiData.m_type==1) stream=new wxMemoryInputStream(clock1,sizeof(clock1));
      else if (data->hmiData.m_type==2) stream=new wxMemoryInputStream(clock2,sizeof(clock2));
      else stream=new wxMemoryInputStream(clock3,sizeof(clock3));
	   img=wxImage(*stream, wxBITMAP_TYPE_PNG, -1 );

      img.Rescale(canvas->GetSize().x,canvas->GetSize().x,wxIMAGE_QUALITY_HIGH);
      data->m_bitmap =  new wxBitmap(img);
      data->cx=(canvas->GetSize().x+4.0)/2.0;
      data->cy=(canvas->GetSize().x)/2.0;
      delete stream;
   }
   
   if ((data->m_bitmap) && (data->m_bitmap->Ok()) && (data->num>0))
   { 
      wxDateTime zmn;
      wxPen      pen;

      zmn.Set((time_t)data->num);
	
   	x = sm_angle(zmn.GetSecond());
   	y = sm_angle(zmn.GetMinute());
	   z = h_angle(zmn.GetHour(),zmn.GetMinute());
		
		if (data->hmiData.m_type==3) pen.SetColour(*wxBLACK);
      else pen.SetColour(*wxWHITE);
		// clear pre-now
		dc->SetPen(pen);
		dc->DrawLine(data->cx, data->cy, (int)(data->cx + 0.8 * data->cx * cos(data->xpre)), (int)(data->cy - 0.8 * data->cy * sin(data->xpre))); // pre-second hand
		dc->DrawBitmap(wxBitmap(*data->m_bitmap),0,0,true);
		// draw now
		pen.SetColour(wxColour(data->hmiData.m_hourColour[data->digi]));
      pen.SetWidth(data->hmiData.m_hourSize[data->digi]);
		dc->SetPen(pen);
		dc->DrawLine(data->cx, data->cy, (int)(data->cx + 0.62 * data->cx * cos(z)), (int)(data->cy - 0.62  * data->cy * sin(z))); // hour hand
		pen.SetColour(wxColour(data->hmiData.m_minColour[data->digi]));
      pen.SetWidth(data->hmiData.m_minSize[data->digi]);
		dc->SetPen(pen);
		dc->DrawLine(data->cx, data->cy, (int)(data->cx + 0.8 * data->cx * cos(y)), (int)(data->cy - 0.8  * data->cy * sin(y))); // minute hand
		pen.SetColour(wxColour(data->hmiData.m_secColour[data->digi]));
      pen.SetWidth(data->hmiData.m_secSize[data->digi]);
		dc->SetPen(pen);
		dc->DrawLine(data->cx, data->cy, (int)(data->cx + 0.8 * data->cx * cos(x)), (int)(data->cy - 0.8 * data->cy * sin(x))); // second hand
      data->xpre=x;
   } 
}



/**
Here the default size of the element has to be returned; this size is used when it is created
newly
@param[out] x the default size in x-direction (width)
@param[out] y the default size in y-direction (height)
*/
OAPC_EXT_API void oapc_get_defsize(wxFloat32 *x,wxFloat32 *y)
{
   *x=100;
   *y=100;
}



/**
This function specifies the minimum size that is allowed for this HMI element, no scaling
operation of the main application will set smaller sizes to the related canvas than specified by
this function
@param[out] x the minimum size in x-direction (width)
@param[out] y the minimum size in y-direction (height)
*/
OAPC_EXT_API void oapc_get_minsize(void*,wxFloat32 *x,wxFloat32 *y)
{
   *x=20;
   *y=20;
}



/**
This function specifies the maximum size that is allowed for this HMI element, no scaling
operation of the main application will set bigger sizes to the related canvas than specified by
this function
@param[out] x the maximum size in x-direction (width)
@param[out] y the maximum size in y-direction (height)
*/
OAPC_EXT_API void oapc_get_maxsize(void*,wxFloat32 *x,wxFloat32 *y)
{
   *x=300;
   *y=300;
}




/**
The foreground and background colour of an element are handled by the standard HMI properties dialogue.
This function is called once during first setup of this control and has to return the default colour
values for it. Afterwards these values are stored by the main application, the Plug-In does not to
take care about them. Within the painting-function (please see above) you can get these colours by
calling canvas->GetBackgroundColour() and canvas->GetForegroundColour()
@param[out] background the default background colour
@param[out] foreground the default foreground colour
*/
OAPC_EXT_API void oapc_get_colours(wxUint32 *background,wxUint32 *foreground)
{
   *foreground=0xFF8080;
   *background=0x000000;
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

   if (input!=0) return OAPC_ERROR_NO_SUCH_IO; // check for valid IO
   data->digi=value;
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
OAPC_EXT_API unsigned long  oapc_get_digi_value(void* instanceData,unsigned long output,unsigned char *value)
{
   struct instData *data;

   data=(struct instData*)instanceData;

   if (output!=0) return OAPC_ERROR_NO_SUCH_IO; // check for valid IO
   if (data->prevDigi!=data->digi)
   {
      data->prevDigi=data->digi;
      *value=data->digi;
      return OAPC_OK;
   }
   return OAPC_ERROR_NO_DATA_AVAILABLE;
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

   if (input!=7) return OAPC_ERROR_NO_SUCH_IO;
   data->num=value;
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

   if (output!=7) return OAPC_ERROR_NO_SUCH_IO;
   if (data->num!=data->prevNum)
   {
      data->prevNum=data->num;
      *value=data->num;
      return OAPC_OK;
   }
   return OAPC_ERROR_NO_DATA_AVAILABLE;
}



