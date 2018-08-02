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
 #pragma warning (disable: 4248)
#endif


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

#include <list>

#define MAX_XML_SIZE   2500

#pragma pack(8)

#define FLIPFLAG_XY      0x0001
#define FLIPFLAG_MIRRORX 0x0002
#define FLIPFLAG_MIRRORY 0x0004

struct hmiConfigData
{
   wxUint16      length,version;
   wxUint32      m_borderColour[2];
   wxUint32      m_lineColour0[2],m_lineColour100[2];
   unsigned char m_lineSize[2],reserved1,reserved2;
   unsigned int  m_maxvecnum;
   int           uFieldx,uFieldy,uFieldsize;
   unsigned char m_flipflags;
};



struct vecData
{
   double        x,y;
   unsigned char on;
   char          power;
};



struct instData
{
   struct hmiConfigData        config;
   wxPanel                    *m_canvas;
   std::list<struct vecData*> *m_vecList;
   unsigned char               m_digi,m_prevOn;
};



/** the xml data that define the behaviour and user interface of the plugin within the main application */
static char xmltempl[]="<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"?>\
<oapc-config>\
<flowimage>%s</flowimage>\
<dialogue>\
 <helppanel>\
  <in0>SEL - Select element</in0>\
  <in1>RES - Reset all vector data</in1>\
  <in2>LEFT - Field left position</in2>\
  <in3>UPPR - Field upper position</in3>\
  <in4>SIZE - Field size</in4>\
  <in7>CTRL - Control data</in7>\
 </helppanel>\
</dialogue>\
</oapc-config>";



/** the xml data that define the behaviour and HMI configuration panels within the main application */
static char xmlhmitempl[]="<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"?>\
<oapc-config>\
<dialogue>\
 <stdpanel>\
  <name>Configuration</name>\
  <param>\
   <name>maxvecnum</name>\
   <text>Maximum Number of Vectors</text>\
   <type>integer</type>\
   <default>%d</default>\
   <min>10</min>\
   <max>100000</max>\
  </param>\
  <param>\
   <name>fieldx</name>\
   <text>Field left position</text>\
   <type>float</type>\
   <default>%f</default>\
   <min>-2000</min>\
   <max>2000</max>\
   <unit>mm</unit>\
  </param>\
  <param>\
   <name>fieldy</name>\
   <text>Field upper position</text>\
   <type>float</type>\
   <default>%f</default>\
   <min>-2000</min>\
   <max>2000</max>\
   <unit>mm</unit>\
  </param>\
  <param>\
   <name>fieldsize</name>\
   <text>Field size</text>\
   <type>float</type>\
   <default>%f</default>\
   <min>-2000</min>\
   <max>2000</max>\
   <unit>mm</unit>\
  </param>\
  <param>\
   <name>flipxy</name>\
   <text>Swap X and Y</text>\
   <type>checkbox</type>""\
   <default>%d</default>\
  </param>\
  <param>\
   <name>mirrorx</name>\
   <text>Mirror X</text>\
   <type>checkbox</type>\
   <default>%d</default>\
  </param>\
  <param>\
   <name>mirrory</name>\
   <text>Mirror Y</text>\
   <type>checkbox</type>\
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
   <name>lineColour0_0</name>\
   <text>Line 0% Power</text>\
   <type>colorbutton</type>\
   <default>%d</default>\
  </param>\
  <param>\
   <name>lineColour0_1</name>\
   <type>colorbutton</type>\
   <default>%d</default>\
  </param>\
  <param>\
   <name>lineColour100_0</name>\
   <text>Line 100% Power</text>\
   <type>colorbutton</type>\
   <default>%d</default>\
  </param>\
  <param>\
   <name>lineColour100_1</name>\
   <type>colorbutton</type>\
   <default>%d</default>\
  </param>\
  <param>\
   <name>pen0</name>\
   <text>Line Size</text>\
   <type>integer</type>\
   <min>1</min>\
   <max>10</max>\
   <default>%d</default>\
  </param>\
  <param>\
   <name>pen1</name>\
   <text>Line Size</text>\
   <type>integer</type>\
   <min>1</min>\
   <max>10</max>\
   <default>%d</default>\
  </param>\
 </dualpanel>\
</dialogue>\
</oapc-config>\n";

static char                 libname[]="Vector 2D";
static char                 xmlhmidescr[MAX_XML_SIZE+1];
static char                 xmldescr[MAX_XML_SIZE+1];
static char                 flowImage[]="iVBORw0KGgoAAAANSUhEUgAAAGoAAAAyCAMAAACDIr4/AAAAA3NCSVQICAjb4U/gAAAAHlBMVEUEAgTk5vyEgoT8/vwA/////wDEwsQAAAD/////AACIlbjGAAAACXBIWXMAABJ0AAASdAHeZh94AAABYUlEQVRYhdWY0ZKDIAxFUVHM///wsl2BBCEBJHb2Th9ipRzCJUwnxryoxcus27atnzCLrbVCzMms+76H3/yhDuOnN8dyj/2UQszpMB51jbtQv6nERdLYirGQVhgXUD6VuEgaWzEW0grjAsrj0yJ7Y05pXEQdaJG9Mac0LqIWvMjemFMcV0apqIRyn888OQ7lprIYVCktx4nDpPetXrmzrsYdqHh1z2oU5b3o9KqGAgGFZiqiCvtfQYGU1ZhXQBVJT7xC+0tQt3xA9gq9q3lVyipHhS8ebGDFqxwVn7uz4nf8jkqPT7xqQp1NKFSjoyh4DQWtWS3J9jHUdfyu6tL0ChBNFwWnOiq7mCBw56PyUz6OEks4ryftrOgx1ETR2tJE0QtDE1W6BTVPIGXp1NU3UXp3YFX/GDX4n30ANUHvo1C3RB2FOjDKKNyBUUaRDowyCndglFHN3ZUJqNbuyiPUa/oBhM0s679WrtoAAAAASUVORK5CYII=";
static struct hmiConfigData save_config;


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
   return OAPC_HAS_INPUTS|
          OAPC_HAS_XML_CONFIGURATION|           // for the custom HMI configuration panel
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
   return OAPC_HMI_NO_UI_TEXT|OAPC_HMI_NO_FREE_SIZE_RATIO|OAPC_HMI_NO_UI_FG;
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
   return OAPC_DIGI_IO0|OAPC_DIGI_IO1|
          OAPC_NUM_IO2 |OAPC_NUM_IO3 |OAPC_NUM_IO4|
          OAPC_BIN_IO7;
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

   sprintf(xmlhmidescr,xmlhmitempl,data->config.m_maxvecnum,
                                   data->config.uFieldx/1000.0,data->config.uFieldy/1000.0,data->config.uFieldsize/1000.0,
                                   data->config.m_flipflags & FLIPFLAG_XY,data->config.m_flipflags & FLIPFLAG_MIRRORX,data->config.m_flipflags & FLIPFLAG_MIRRORY,
                                   data->config.m_borderColour[0],data->config.m_borderColour[1],
                                   data->config.m_lineColour0[0],data->config.m_lineColour0[1],data->config.m_lineColour100[0],data->config.m_lineColour100[1],
                                   data->config.m_lineSize[0],data->config.m_lineSize[1]);
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
#ifdef _DEBUG
   long l=strlen(xmldescr);
   l=l;
   assert(strlen(xmldescr)<MAX_XML_SIZE);
#endif
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

   if (strcmp(name,"borderColour0")==0)        data->config.m_borderColour[0]=atoi(value);
   else if (strcmp(name,"borderColour1")==0)   data->config.m_borderColour[1]=atoi(value);
   else if (strcmp(name,"lineColour0_0")==0)   data->config.m_lineColour0[0]=atoi(value);
   else if (strcmp(name,"lineColour0_1")==0)   data->config.m_lineColour0[1]=atoi(value);
   else if (strcmp(name,"lineColour100_0")==0) data->config.m_lineColour100[0]=atoi(value);
   else if (strcmp(name,"lineColour100_1")==0) data->config.m_lineColour100[1]=atoi(value);
   else if (strcmp(name,"pen0")==0)            data->config.m_lineSize[0]=atoi(value);
   else if (strcmp(name,"pen1")==0)            data->config.m_lineSize[1]=atoi(value);
   else if (strcmp(name,"maxvecnum")==0)       data->config.m_maxvecnum=atoi(value);
   else if (strcmp(name,"fieldx")==0)          data->config.uFieldx=(int)OAPC_ROUND(oapc_util_atof(value)*1000.0,0);
   else if (strcmp(name,"fieldy")==0)          data->config.uFieldy=(int)OAPC_ROUND(oapc_util_atof(value)*1000.0,0);
   else if (strcmp(name,"fieldsize")==0)       data->config.uFieldsize=(int)OAPC_ROUND(oapc_util_atof(value)*1000.0,0);
   else if (strcmp(name,"flipxy")==0)
   {
      if (atoi(value)) data->config.m_flipflags|=FLIPFLAG_XY;
      else data->config.m_flipflags&=~FLIPFLAG_XY;
   }
   else if (strcmp(name,"mirrorx")==0)
   {
      if (atoi(value)) data->config.m_flipflags|=FLIPFLAG_MIRRORX;
      else data->config.m_flipflags&=~FLIPFLAG_MIRRORX;
   }
   else if (strcmp(name,"mirrory")==0)
   {
      if (atoi(value)) data->config.m_flipflags|=FLIPFLAG_MIRRORY;
      else data->config.m_flipflags&=~FLIPFLAG_MIRRORY;
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
   int              i;

   data=(struct instData*)instanceData;

   *length=sizeof(struct hmiConfigData);
   save_config.version           =htons(data->config.version);
   save_config.length            =htons(data->config.length);
   for (i=0; i<2; i++)
   {
      save_config.m_borderColour[i] =htonl(data->config.m_borderColour[i]);
      save_config.m_lineColour0[i]  =htonl(data->config.m_lineColour0[i]);
      save_config.m_lineColour100[i]=htonl(data->config.m_lineColour100[i]);
      save_config.m_lineSize[i]     =data->config.m_lineSize[i];
   }
   save_config.m_maxvecnum          =htonl(data->config.m_maxvecnum);
   save_config.uFieldx              =htonl(data->config.uFieldx);
   save_config.uFieldy              =htonl(data->config.uFieldy);
   save_config.uFieldsize           =htonl(data->config.uFieldsize);
   save_config.m_flipflags          =data->config.m_flipflags;

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

   if (length>sizeof(struct hmiConfigData)) length=sizeof(struct hmiConfigData);
   memcpy(&save_config,loadedData,length);

   for (i=0; i<2; i++)
   {
      data->config.m_borderColour[i] =ntohl(save_config.m_borderColour[i]);
      data->config.m_lineColour0[i]  =htonl(save_config.m_lineColour0[i]);
      data->config.m_lineColour100[i]=htonl(save_config.m_lineColour100[i]);
      data->config.m_lineSize[i]     =save_config.m_lineSize[i];
   }
   data->config.m_maxvecnum    =ntohl(save_config.m_maxvecnum);
   data->config.uFieldx        =ntohl(save_config.uFieldx);
   data->config.uFieldy        =ntohl(save_config.uFieldy);
   data->config.uFieldsize     =ntohl(save_config.uFieldsize);
   data->config.m_flipflags    =save_config.m_flipflags;
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

   data->config.length=sizeof(struct hmiConfigData);
   data->config.version=1;
   data->config.m_maxvecnum=300;
   data->config.uFieldx=-50*1000;
   data->config.uFieldy=50*1000;
   data->config.uFieldsize=100*1000;
   data->config.m_borderColour[0]=0x808080;
   data->config.m_borderColour[1]=0x808080;
   data->config.m_lineColour0[0]=0xA0A0FF;
   data->config.m_lineColour0[1]=0xA0A0FF;
   data->config.m_lineColour100[0]=0x0000E0;
   data->config.m_lineColour100[1]=0x0000E0;
   data->config.m_lineSize[0]=2;
   data->config.m_lineSize[1]=2;
   return data;
}



/**
This function is called finally, it has to be used to release the instance data structure that was created
during the call of oapc_create_instance()
*/
OAPC_EXT_API void oapc_delete_instance(void* instanceData)
{
/*   struct instData *data;

   data=(struct instData*)instanceData;*/

   if (instanceData) free(instanceData);
}



/**
When this function is called everything has to be initialized in order to perform the required operation
@return a return value/error code that informs the main application if the initialization was done successfully
        or not
*/
OAPC_EXT_API unsigned long oapc_init(void* instanceData)
{
   struct instData *data;

   data=(struct instData*)instanceData;
   data->m_vecList=new std::list<struct vecData*>;
   if (!data->m_vecList)
   {
      oapc_exit(instanceData);
      return OAPC_ERROR_NO_MEMORY;
   }

   return OAPC_OK;
}



static void releaseAllVecData(struct instData *data)
{
   std::list<struct vecData*>::iterator it;

   while (!data->m_vecList->empty())
   {
      it=data->m_vecList->begin();
      free(*it);
      data->m_vecList->pop_front();
   }
}


/**
This function is called before the application unloads everything, it has to be used to deinitialize
everything and to release used resources.
*/
OAPC_EXT_API unsigned long oapc_exit(void* instanceData)
{
   struct instData *data;

   data=(struct instData*)instanceData;

   if (data->m_vecList)
   {
      releaseAllVecData(data);
      delete data->m_vecList;
   }
   data->m_vecList=NULL;
   return OAPC_OK;
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
   struct instData                     *data;
   struct vecData                      *vec;
   std::list<struct vecData*>::iterator it;
   unsigned char                        on=0;
   char                                 power;
   double                               fac,x,y,x2,y2;
   int                                  w,h;

   data=(struct instData*)instanceData;
   data->m_canvas=canvas;

   dc->SetBrush(wxBrush(canvas->GetBackgroundColour()));
   dc->SetPen(wxPen(data->config.m_borderColour[data->m_digi]));
   canvas->GetSize(&w,&h);
   dc->DrawRectangle(0,0,w,h);

   if (!data->m_vecList) return;
   fac=1.0*w/data->config.uFieldsize;

   dc->SetPen(wxPen(data->config.m_lineColour0[data->m_digi],data->config.m_lineSize[data->m_digi]));
   it=data->m_vecList->begin();
   if (it!=data->m_vecList->end())
   {
      vec=*it;
      it++;
      x=vec->x*fac;
      y=vec->y*fac;
      on=vec->on;
      power=vec->power;
      while (it!=data->m_vecList->end())
      {
         vec=*it;
         x2=vec->x*fac;
         y2=vec->y*fac;
         if (power>=0)
         {
            int r,g,b,r0,g0,b0,r100,g100,b100;

            r0= data->config.m_lineColour0[data->m_digi] & 0x0000FF;
            g0=(data->config.m_lineColour0[data->m_digi] & 0x00FF00)>>8;
            b0=(data->config.m_lineColour0[data->m_digi] & 0xFF0000)>>16;
            r100= data->config.m_lineColour100[data->m_digi] & 0x0000FF;
            g100=(data->config.m_lineColour100[data->m_digi] & 0x00FF00)>>8;
            b100=(data->config.m_lineColour100[data->m_digi] & 0xFF0000)>>16;

            r=(((r100-r0)/100.0)*power)+r0;
            g=(((g100-g0)/100.0)*power)+g0;
            b=(((b100-b0)/100.0)*power)+b0;

            dc->SetPen(wxPen(b<<16 | g<<8 | r,data->config.m_lineSize[data->m_digi]));
         }
         if (on) dc->DrawLine(x,y,x2,y2);
         x=x2;
         y=y2;
         on=vec->on;
         power=vec->power;
         it++;
      }
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
   *x=240;
   *y=240;
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
   *foreground=0x0000D0;
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
   if (input==0) data->m_digi=value;
   else if (input==1)
   {
      releaseAllVecData(data);
      if (data->m_canvas) data->m_canvas->Refresh();
   }
   else return OAPC_ERROR_NO_SUCH_IO;
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

   if (input==2) data->config.uFieldx=value;
   else if (input==3) data->config.uFieldy=value;
   else if (input==4) data->config.uFieldsize=value;
   else return OAPC_ERROR_NO_SUCH_IO;

   if (data->m_canvas) data->m_canvas->Refresh();
   return OAPC_OK;
}



OAPC_EXT_API unsigned long  oapc_set_bin_value(void* instanceData,unsigned long input,struct oapc_bin_head *value)
{
   struct instData             *data;
   struct oapc_bin_struct_ctrl *ctrl=NULL;
   struct vecData              *vec;
   int                          i,fieldXOffset,fieldYOffset;
   unsigned short               numCoords;
   unsigned int                 validityFlags;


   if (input!=7) return OAPC_ERROR_NO_SUCH_IO;
   if (value->type!=OAPC_BIN_TYPE_STRUCT) return OAPC_ERROR_INVALID_INPUT;
   data=(struct instData*)instanceData;
   if (value->subType==OAPC_BIN_SUBTYPE_STRUCT_CTRLEND)
   {
      if (data->m_canvas) data->m_canvas->Refresh();
      return OAPC_OK;
   }
   if (value->subType!=OAPC_BIN_SUBTYPE_STRUCT_CTRL) return OAPC_ERROR_INVALID_INPUT;
   ctrl=(struct oapc_bin_struct_ctrl*)&value->data;
   numCoords=ntohs(ctrl->numCoords);
   validityFlags=ntohl(ctrl->validityFlags);
   assert(numCoords>0);
   if (numCoords==0) numCoords=1;

   fieldXOffset=(data->config.uFieldsize/2)+data->config.uFieldx;
   fieldYOffset=(data->config.uFieldsize/2)-data->config.uFieldy;
   for (i=0; i<numCoords; i++)
   {
      vec=(struct vecData*)malloc(sizeof(struct vecData));
      if (!vec) return OAPC_ERROR_NO_MEMORY;
      if (data->config.m_flipflags & FLIPFLAG_XY)
      {
         vec->x=(int)ntohl(ctrl->coord[i].y);
         vec->y=(int)ntohl(ctrl->coord[i].x);
      }
      else
      {
         vec->x=(int)ntohl(ctrl->coord[i].x);
         vec->y=(int)ntohl(ctrl->coord[i].y);
      }
      if (data->config.m_flipflags & FLIPFLAG_MIRRORX)
      {
         vec->x=(vec->x+fieldXOffset)*(-1)-fieldXOffset;
      }
      if ((data->config.m_flipflags & FLIPFLAG_MIRRORY)==0)
      {
         vec->y=(vec->y+fieldYOffset)*(-1)-fieldYOffset;
      }
      vec->x-=data->config.uFieldx;
      vec->y+=data->config.uFieldy;
      if (validityFlags & (TOOLPARAM_VALIDFLAG_ON))
      {
         vec->on=ctrl->on;
         data->m_prevOn=vec->on;
      }
      else vec->on=data->m_prevOn;
      if (validityFlags & (TOOLPARAM_VALIDFLAG_POWER))
      {
         vec->power=OAPC_ROUND((int)ntohl(ctrl->power)/1000.0,0);
         assert(vec->power<=100);
         assert(vec->power>=0);
         if (vec->power<0) vec->power=0;
         else if (vec->power>100) vec->power=100;
      }
      else vec->power=-1;

      data->m_vecList->push_back(vec);
      if (data->m_vecList->size()>data->config.m_maxvecnum)
      {
         std::list<struct vecData*>::iterator it;

         it=data->m_vecList->begin();
         free(*it);
         data->m_vecList->pop_front();
      }
   }

   return OAPC_OK;
}
