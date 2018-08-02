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
#include <time.h>
#include <assert.h>

#include <wx/wx.h>
#include <wx/dcbuffer.h>
#include <wx/cursor.h>

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
#include "liboapc.h"


#define MAX_XML_SIZE   2500

#pragma pack(8)

enum symbol_types
{
	UNDEFINED=0,
	SIMPLE,
	VALVE_HORIZ,
	VALVE_VERT,
	PUMP_RIGHT,
	PUMP_LEFT,
	PUMP_UP,
	PUMP_DOWN,
	LAMP,
	HORN,
	SWITCH,
	MOTOR
};



struct hmiConfigData
{
   wxUint16 length,version;
   wxUint32 m_BorderColour[2];
   wxUint32 m_FillColour[2];
   int      m_symbol;
   wxUint8  m_borderSize[2],pad[2];
};



struct instData
{
   struct hmiConfigData  hmiData;
   wxBitmap             *bmOn,*bmOff;
   wxUint8               digi[8]; // we use an array for all possible digital IOs
   wxUint32              m_disBorderColour[2];
   wxUint32              m_disFillColour[2];
   bool                  m_isEnabled,m_isReadOnly;
   int                   m_callbackID;
};


/** the xml data that define the behaviour and user interface of the plugin within the main application */
static char xmltempl[]="<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"?>\
<oapc-config>\
<flowimage>%s</flowimage>\
</oapc-config>";

/** the xml data that define the behaviour and HMI configuration panels within the main application */
static char xmlhmitempl[]="<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"?>\
<oapc-config>\
<dialogue>\
 <stdpanel>\
  <name>Configuration</name>\
  <param>\
   <name>symbol</name>\
   <text>Symbol</text>\
   <type>option</type>\
   <value>Simple Knob</value>\
   <value>Horizontal Valve</value>\
   <value>Vertical Valve</value>\
   <value>Pump (right)</value>\
   <value>Pump (left)</value>\
   <value>Pump (up)</value>\
   <value>Pump (down)</value>\
   <value>Lamp</value>\
   <value>Horn</value>\
   <value>Switch</value>\
   <value>Motor</value>\
   <default>%d</default>\
  </param>\
 </stdpanel>\
 <dualpanel>\
  <name>Colours</name>\
  <param>\
   <name>borderColour0</name>\
   <text>Border</text>\
   <type>colorbutton</type>\
   <default>%d</default>\
  </param>\
  <param>\
   <name>borderColour1</name>\
   <type>colorbutton</type>\
   <default>%d</default>\
  </param>\
  <param>\
   <name>borderSize0</name>\
   <text>Border Size</text>\
   <type>integer</type>\
   <default>%d</default>\
   <min>1</min>\
   <max>20</max>\
  </param>\
  <param>\
   <name>borderSize1</name>\
   <type>integer</type>\
   <default>%d</default>\
   <min>1</min>\
   <max>20</max>\
  </param>\
  <param>\
   <name>fillColour0</name>\
   <text>Symbol</text>\
   <type>colorbutton</type>\
   <default>%d</default>\
  </param>\
  <param>\
   <name>fillColour1</name>\
   <type>colorbutton</type>\
   <default>%d</default>\
  </param>\
 </dualpanel>\
</dialogue>\
</oapc-config>\n";

static char                 libname[]="Symbol Button";
static char                 xmlhmidescr[MAX_XML_SIZE+1];
static char                 xmldescr[MAX_XML_SIZE+1];
static char                 flowImage[]="iVBORw0KGgoAAAANSUhEUgAAAGoAAAAyBAMAAABG0lM+AAAAMFBMVEUEAgSEgoTEwsQEAoTk5vwEgvz8AgT8/vwAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA526+JAAAACXBIWXMAABJ0AAASdAHeZh94AAAAxElEQVR42u3WOw7DIAwGYDdCypqta4fcoDkAjbhBWmVl8kwnrl8KzaMM1HbH8A9M+eQYLASAKMqk6LPvDDF6VWPrG74yp6cSqIvrBUqT29qrsZEoo6o6lnrkuZeVjiM8+yxDWd2g56k0vZEF5dpFQFF1y6WQlP0wsL//cKuFiQFaRl8OIwOkqHUPgwoMkKRSZgfv79HGBYCoZLWYfe0UYw83RTyv7JSJs9F81WJN7z9qumYZ6h11YKXj2LJVfduI3zaivADauaXbIkO+NwAAAABJRU5ErkJggg==";
static struct hmiConfigData saveConfig;
static lib_oapc_io_callback m_oapc_io_callback; // callback function that is used to inform the main function about changes at the IO ports


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
          OAPC_HAS_LOG_TYPE_DIGI|               // to let the main application log the change of data
          OAPC_ACCEPTS_PLAIN_CONFIGURATION|
          OAPC_ACCEPTS_MOUSECLICKS|
          OAPC_ACCEPTS_IO_CALLBACK|
          OAPC_HMICAT_CONTROL|
          OAPC_USERPRIVI_DISABLE|OAPC_USERPRIVI_HIDE;
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
   return OAPC_HMI_NO_UI_TEXT|OAPC_HMI_NO_UI_FG;
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
   return OAPC_DIGI_IO1;
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
   return OAPC_DIGI_IO1;
   // avoid it to submit the same IO number for different data types, that would cause displaying problems for the flow symbol
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

   sprintf(xmlhmidescr,xmlhmitempl,data->hmiData.m_symbol,
                                   data->hmiData.m_BorderColour[0],data->hmiData.m_BorderColour[1],
                                   data->hmiData.m_borderSize[0],data->hmiData.m_borderSize[1],
                                   data->hmiData.m_FillColour[0],  data->hmiData.m_FillColour[1]);
   assert(strlen(xmldescr)<MAX_XML_SIZE);
   return xmlhmidescr;
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

   if (strcmp(name,"symbol")==0)             data->hmiData.m_symbol=atoi(value);
   else if (strcmp(name,"borderColour0")==0) data->hmiData.m_BorderColour[0]=atoi(value);
   else if (strcmp(name,"borderColour1")==0) data->hmiData.m_BorderColour[1]=atoi(value);
   else if (strcmp(name,"borderSize0")==0)   data->hmiData.m_borderSize[0]=atoi(value);
   else if (strcmp(name,"borderSize1")==0)   data->hmiData.m_borderSize[1]=atoi(value);
   else if (strcmp(name,"fillColour0")==0)   data->hmiData.m_FillColour[0]=atoi(value);
   else if (strcmp(name,"fillColour1")==0)   data->hmiData.m_FillColour[1]=atoi(value);
  	if (data->bmOn) delete data->bmOn;
  	if (data->bmOff) delete data->bmOff;
   data->bmOn=NULL;
   data->bmOff=NULL;
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

   *length=sizeof(struct hmiConfigData);
   saveConfig.version          =htons(2);
   saveConfig.length           =htons(*length);
   saveConfig.m_symbol         =htonl(data->hmiData.m_symbol);
   saveConfig.m_BorderColour[0]=htonl(data->hmiData.m_BorderColour[0]); saveConfig.m_BorderColour[1]=htonl(data->hmiData.m_BorderColour[1]);
   saveConfig.m_borderSize[0]  =data->hmiData.m_borderSize[0];          saveConfig.m_borderSize[1]=data->hmiData.m_borderSize[1];
   saveConfig.m_FillColour[0]  =htonl(data->hmiData.m_FillColour[0]);   saveConfig.m_FillColour[1]=htonl(data->hmiData.m_FillColour[1]);
   saveConfig.pad[0]=0;
   saveConfig.pad[1]=0;

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

   saveConfig.version=ntohs(saveConfig.version);
   data->hmiData.m_symbol=ntohl(saveConfig.m_symbol);
   for (i=0; i<2; i++)
   {
      data->hmiData.m_BorderColour[i]=ntohl(saveConfig.m_BorderColour[i]);
      data->hmiData.m_FillColour[i]=ntohl(saveConfig.m_FillColour[i]);
      data->m_disBorderColour[i]=oapc_util_colour2gray(data->hmiData.m_BorderColour[i]);
      data->m_disFillColour[i]=oapc_util_colour2gray(data->hmiData.m_FillColour[i]);
   }
   if (saveConfig.version<2)
   {
      data->hmiData.m_borderSize[0]=2;
      data->hmiData.m_borderSize[1]=2;
   }
   else
   {
      data->hmiData.m_borderSize[0]=saveConfig.m_borderSize[0];
      data->hmiData.m_borderSize[1]=saveConfig.m_borderSize[1];
   }
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

   data->hmiData.length=sizeof(struct hmiConfigData);
   data->hmiData.m_symbol=SIMPLE;
   data->hmiData.m_FillColour[0]=0x0000FF; data->hmiData.m_FillColour[1]=0x00FF00;
   data->hmiData.m_borderSize[0]=2;        data->hmiData.m_borderSize[1]=2;
   data->m_isEnabled=true;
   data->m_isReadOnly=false;
   return data;
}



/**
This function is called finally, it has to be used to release the instance data structure that was created
during the call of oapc_create_instance()
*/
OAPC_EXT_API void oapc_delete_instance(void* instanceData)
{
   struct instData *data;

   data=(struct instData*)instanceData;

   if (data->bmOn) delete data->bmOn;
   if (data->bmOff) delete data->bmOff;

   if (instanceData) free(instanceData);
}



/**
When this function is called everything has to be initialized in order to perform the required operation
@return a return value/error code that informs the main application if the initialization was done successfully
        or not
*/
OAPC_EXT_API unsigned long oapc_init(void* WXUNUSED(instanceData))
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
In case at least one of the OAPC_ACCEPTS_MOUSE... capability flags are set this function is called
whenever the related mouse event happens.
@param[in] event a wxWidgets wxMouseEvent structure that contains al relevant information: the type
           of the event, the current mouse position and others
*/
OAPC_EXT_API void oapc_mouseevent(void* instanceData,wxMouseEvent* event)
{
   struct instData *data;

   data=(struct instData*)instanceData;

   if ((data->m_isEnabled) && (!data->m_isReadOnly) && (event->LeftUp()))
   {
      data->digi[1]=1-data->digi[1];
      m_oapc_io_callback(OAPC_DIGI_IO1,data->m_callbackID);
   }
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
	wxInt32   w,h;
   struct instData *data;

   data=(struct instData*)instanceData;

   if (data->m_isEnabled!=oapc_canvas_get_enabled(canvas))
   {
      delete data->bmOn;  data->bmOn=NULL;
      delete data->bmOff; data->bmOff=NULL;
   }
   data->m_isEnabled=oapc_canvas_get_enabled(canvas);
   data->m_isReadOnly=oapc_canvas_get_readonly(canvas);
   canvas->GetSize(&w,&h);
   if ((!data->bmOn) || (!data->bmOff) ||
       (data->bmOn->GetWidth()!=w) || (data->bmOn->GetHeight()!=h) ||
       (data->bmOff->GetWidth()!=w) || (data->bmOff->GetHeight()!=h))
   {
      int i;

      if (data->bmOn) delete data->bmOn;
      if (data->bmOff) delete data->bmOff;
      data->bmOn=new wxBitmap(w,h);
      data->bmOff=new wxBitmap(w,h);
      if ((!data->bmOn) || (!data->bmOff)) return; // no more memory
#ifdef ENV_LINUX
      w--;
      h--;
#endif
      for (i=0; i<2; i++)
      {
         wxFloat32   fX=100.0/w,fY=100.0/h,xOffset=0,yOffset=0;
         wxMemoryDC *memDC;
         wxPen       borderPen;
         wxBrush     fillBrush;

         if (data->m_isEnabled)
         {
            borderPen.SetColour(wxColour(data->hmiData.m_BorderColour[i]));
            fillBrush.SetColour(wxColour(data->hmiData.m_FillColour[i]));
         }
         else
         {
            borderPen.SetColour(wxColour(data->m_disBorderColour[i]));
            fillBrush.SetColour(wxColour(data->m_disFillColour[i]));
         }
         borderPen.SetWidth(data->hmiData.m_borderSize[i]);

         if (w>h) xOffset=(w-h)/2.0;
         else if (h>w) yOffset=(h-w)/2.0;
         if (i==0) memDC=new wxMemoryDC(*data->bmOff);
         else memDC=new wxMemoryDC(*data->bmOn);
         memDC->SetBackground(wxBrush(canvas->GetBackgroundColour()));
         memDC->Clear();
         memDC->SetBrush(fillBrush);
         memDC->SetPen(borderPen);
         if ((data->hmiData.m_symbol==VALVE_HORIZ) || (data->hmiData.m_symbol==VALVE_VERT))
         {
            wxPoint v1[3],v2[3];
            int     j;
      		
#ifdef ENV_WINDOWS
            h--;
            fY=100.0/h;
#endif
      		if (data->hmiData.m_symbol==VALVE_HORIZ)
      		{
      			v1[0].x=2;  v1[0].y=0;
      			v1[1].x=50; v1[1].y=49;
      			v1[2].x=2;  v1[2].y=99;
      			v2[0].x=99; v2[0].y=0;
      			v2[1].x=50; v2[1].y=49;
      			v2[2].x=99; v2[2].y=99;
      		}
      		else // VALVE_VERT
      		{
      			v1[0].x=0;  v1[0].y=2;
      			v1[1].x=49; v1[1].y=50;
      			v1[2].x=98; v1[2].y=2;
      			v2[0].x=0;  v2[0].y=99;
      			v2[1].x=49; v2[1].y=50;
      			v2[2].x=98; v2[2].y=99;
      		}
      		for (j=0; j<3; j++)
      		{
      			v1[j].x=OAPC_ROUND(v1[j].x/fX,0);
      			v1[j].y=OAPC_ROUND(v1[j].y/fY,0);
      			v2[j].x=OAPC_ROUND(v2[j].x/fX,0);
      			v2[j].y=OAPC_ROUND(v2[j].y/fY,0);
      		}
      		memDC->DrawPolygon(3,v1);
      		memDC->DrawPolygon(3,v2);
      	}
   	   else if (data->hmiData.m_symbol==HORN)
      	{
      		wxPoint v[]={wxPoint(48,37),wxPoint(99,23),wxPoint(84,72),wxPoint(48,59)};
      		int     j;
      		
      		for (j=0; j<4; j++)
      		{
      			v[j].x/=fX; v[j].y/=fY;
      		}
      		memDC->DrawRectangle(1,28/fY, 48/fX,41/fY);
      		memDC->DrawPolygon(4,v);
      	}
      	else if ((data->hmiData.m_symbol==SIMPLE) || 
      	         (data->hmiData.m_symbol==PUMP_LEFT) || (data->hmiData.m_symbol==PUMP_RIGHT) ||
      	         (data->hmiData.m_symbol==PUMP_UP) || (data->hmiData.m_symbol==PUMP_DOWN) ||
      	         (data->hmiData.m_symbol==LAMP) ||
      	         (data->hmiData.m_symbol==MOTOR))
      	{
      		wxInt32 r=(w/2.0)-data->hmiData.m_borderSize[i]+1;
      		
      		if (r>(h/2.0)-data->hmiData.m_borderSize[i]+1)
      		{
      			r=(h/2.0)-data->hmiData.m_borderSize[i]+1;
      			fX=fY;
      		}
      		else fY=fX;
            memDC->DrawCircle(w/2.0,h/2.0,r);
      		if (data->hmiData.m_symbol==PUMP_LEFT)
      		{
      			memDC->DrawLine(3/fX+xOffset+data->hmiData.m_borderSize[i]-1,41/fY+yOffset, 80/fX+xOffset-data->hmiData.m_borderSize[i]+1,11/fY+data->hmiData.m_borderSize[i]-1);
      			memDC->DrawLine(3/fX+xOffset+data->hmiData.m_borderSize[i]-1,59/fY+yOffset, 80/fX+xOffset-data->hmiData.m_borderSize[i]+1,89/fY-data->hmiData.m_borderSize[i]+1);
      		}
      		else if (data->hmiData.m_symbol==PUMP_RIGHT)
      		{
      			memDC->DrawLine(20/fX+xOffset+data->hmiData.m_borderSize[i]-1,11/fY+yOffset+data->hmiData.m_borderSize[i]-1, 97/fX+xOffset-data->hmiData.m_borderSize[i]+1,41/fY);
      			memDC->DrawLine(20/fX+xOffset+data->hmiData.m_borderSize[i]-1,89/fY+yOffset-data->hmiData.m_borderSize[i]+1, 97/fX+xOffset-data->hmiData.m_borderSize[i]+1,59/fY);
      		}
      		else if (data->hmiData.m_symbol==PUMP_UP)
      		{
      			memDC->DrawLine(11/fX+xOffset+data->hmiData.m_borderSize[i]-1,79/fY+yOffset-data->hmiData.m_borderSize[i]+1, 42/fX+xOffset,2/fY+yOffset+data->hmiData.m_borderSize[i]-1);
      			memDC->DrawLine(89/fX+xOffset-data->hmiData.m_borderSize[i]+1,79/fY+yOffset-data->hmiData.m_borderSize[i]+1, 58/fX+xOffset,2/fY+yOffset+data->hmiData.m_borderSize[i]-1);
      		}      		
      		else if (data->hmiData.m_symbol==PUMP_DOWN)
      		{
      			memDC->DrawLine(11/fX+xOffset+data->hmiData.m_borderSize[i]-1,20/fY+yOffset+data->hmiData.m_borderSize[i]-1, 42/fX+xOffset,97/fY+yOffset-data->hmiData.m_borderSize[i]+1);
      			memDC->DrawLine(89/fX+xOffset-data->hmiData.m_borderSize[i]+1,20/fY+yOffset+data->hmiData.m_borderSize[i]-1, 58/fX+xOffset,97/fY+yOffset-data->hmiData.m_borderSize[i]+1);
      		}      		
      		else if (data->hmiData.m_symbol==LAMP)
      		{
      		   double sizeOffset;

      		   sizeOffset=data->hmiData.m_borderSize[i]/2.0;
      			memDC->DrawLine(15/fX+xOffset+sizeOffset,15/fY+yOffset+sizeOffset, 84/fX+xOffset-sizeOffset,84/fY+yOffset-sizeOffset);
      			memDC->DrawLine(15/fX+xOffset+sizeOffset,84/fY+yOffset-sizeOffset, 84/fX+xOffset-sizeOffset,15/fY+yOffset+sizeOffset);
      		}      		
      		else if (data->hmiData.m_symbol==MOTOR)
      		{
      			wxPoint v[]={wxPoint(33,73),wxPoint(33,25),wxPoint(49,51),wxPoint(67,25),wxPoint(67,73)};
      			int j;

         		for (j=0; j<5; j++)
      		   {
      			   v[j].x/=fX;      v[j].y/=fY;
      			   v[j].x+=xOffset; v[j].y+=yOffset;
      		   }
      			memDC->DrawLines(5,v);      			
      		}
      	}
      	else if (data->hmiData.m_symbol==SWITCH)
      	{
      	   memDC->DrawLine(0/fX,60/fY,  10/fX,60/fY);
      	   memDC->DrawLine(10/fX,60/fY, 88/fX,22/fY);
      	   if (i==1) memDC->DrawLine(82/fX,21/fY, 82/fX,60/fY);
      	   memDC->DrawLine(82/fX,60/fY, 99/fX,60/fY);
      	}
   	   delete memDC;
   	}
   }

   if (data->digi[1]) dc->DrawBitmap(*data->bmOn,0,0,false);
   else dc->DrawBitmap(*data->bmOff,0,0,false);
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
Here the default size of the element has to be returned; this size is used when it is created
newly
@param[out] x the default size in x-direction (width)
@param[out] y the default size in y-direction (height)
*/
OAPC_EXT_API void oapc_get_defsize(wxFloat32 *x,wxFloat32 *y)
{
   *x=80;
   *y=40;
}



/**
This function specifies the minimum size that is allowed for this HMI element, no scaling
operation of the main application will set smaller sizes to the related canvas than specified by
this function
@param[out] x the minimum size in x-direction (width)
@param[out] y the minimum size in y-direction (height)
*/
OAPC_EXT_API void oapc_get_minsize(void* WXUNUSED(instanceData),wxFloat32 *x,wxFloat32 *y)
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
OAPC_EXT_API void oapc_get_maxsize(void* WXUNUSED(instanceData),wxFloat32 *x,wxFloat32 *y)
{
   *x=1000;
   *y=1000;
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
   *foreground=0xFFE0E0;
   *background=0xFFFFFF;
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

   if (input!=1) return OAPC_ERROR_NO_SUCH_IO; // check for valid IO
   data->digi[input]=value;
   m_oapc_io_callback(OAPC_DIGI_IO1,data->m_callbackID);
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

   if (output!=1) return OAPC_ERROR_NO_SUCH_IO; // check for valid IO
   *value=data->digi[output];
   return OAPC_OK; // there are no new data available
}




