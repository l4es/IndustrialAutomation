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
#pragma warning (disable: 4100)
#endif


#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <list>

#ifndef ENV_WINDOWSCE
#include <sys/types.h>
#endif
#include <time.h>

#include <wx/wx.h>
#include <wx/dcbuffer.h>

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
#else
#define snprintf _snprintf
#endif

#include "oapc_libio.h"
#include "images.h"

#define MAX_XML_SIZE   3500

#pragma pack(8)



struct bitmapInfo
{
   wxInt32   w,h;
   wxBitmap *bmHoriz,*bmVert,*bmT[4],*bmElbow[4],*bmHoriz_black,*bmVert_black,*bmT_black[4],*bmElbow_black[4];
} bitmapInfo_t;



struct hmiConfigData
{
   wxUint16 length,version;
   wxByte   pipeType;
};



struct instData
{
   struct hmiConfigData  hmiData;
   char                  m_digi,m_prevDigi;
   wxBitmap             *bmLastUsed;
   wxInt32               w,h;
};



/** the xml data that define the behaviour and HMI configuration panel within the main application */
static char xmlhmitempl[]="<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"?>\n\
<oapc-config>\n\
<dialogue>\n\
 <stdpanel>\n\
  <name>Type</name>\n\
  <param>\n\
   <name>pipeType</name>\n\
   <text>Pipe Type</text>\n\
   <type>option</type>\n\
   <value>Horizontal (Steel)</value>\n\
   <value>Vertical (Steel)</value>\n\
   <value>T-Piece, top (Steel)</value>\n\
   <value>T-Piece, right (Steel)</value>\n\
   <value>T-Piece, bottom (Steel)</value>\n\
   <value>T-Piece, left (Steel)</value>\n\
   <value>Elbow, top-right (Steel)</value>\n\
   <value>Elbow, right-bottom (Steel)</value>\n\
   <value>Elbow, bottom-left (Steel)</value>\n\
   <value>Elbow, left-up (Steel)</value>\n\
   <value>Horizontal (black)</value>\n\
   <value>Vertical (black)</value>\n\
   <value>T-Piece, top (black)</value>\n\
   <value>T-Piece, right (black)</value>\n\
   <value>T-Piece, bottom (black)</value>\n\
   <value>T-Piece, left (black)</value>\n\
   <value>Elbow, top-right (black)</value>\n\
   <value>Elbow, right-bottom (black)</value>\n\
   <value>Elbow, bottom-left (black)</value>\n\
   <value>Elbow, left-up (black)</value>\n\
   <default>%d</default>\n\
  </param>\n\
 </stdpanel>\n\
</dialogue>\n\
</oapc-config>\n";

static char                          libname[]="Pipes";
static char                          xmlhmidescr[MAX_XML_SIZE+1];
static std::list<struct bitmapInfo*> bmInfoList; // list for global image cache
static struct hmiConfigData          saveConfig;



/**
This function has to return the name that is used to display this library within the main application.
The name returned here has to be short and descriptive. It is displayed within the popup menu of the flow
editor, therefore it should not be too long!
@return data pointer to the beginning of an char-array that contains the name, this array has to be available
        globally
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
          OAPC_ACCEPTS_PLAIN_CONFIGURATION|
          OAPC_HMICAT_STATIC|
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
   return OAPC_HMI_NO_UI_RO|    OAPC_HMI_NO_UI_TEXT|OAPC_HMI_NO_UI_DISABLED|
          OAPC_HMI_NO_UI_MINMAX|OAPC_HMI_NO_UI_FG|  OAPC_HMI_NO_UI_FONT;
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
   return  OAPC_DIGI_IO0;
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
   return OAPC_DIGI_IO0;
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

   snprintf(xmlhmidescr,MAX_XML_SIZE,xmlhmitempl,data->hmiData.pipeType);
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

   if (strcmp(name,"pipeType")==0)
   {
      if (data->hmiData.pipeType!=atoi(value)) data->bmLastUsed=NULL;
      data->hmiData.pipeType=atoi(value);
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

   *length=sizeof(struct hmiConfigData);
   saveConfig.version =htons(1);
   saveConfig.length  =htons(*length);
   saveConfig.pipeType=data->hmiData.pipeType;

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

   data=(struct instData*)instanceData;

   if (length>sizeof(struct hmiConfigData)) length=sizeof(struct hmiConfigData);
   memcpy(&saveConfig,loadedData,length);

   data->hmiData.pipeType=saveConfig.pipeType;
}



/**
This function handles all internal data initialisation and has to allocate a memory area where all
data are stored into that are required to operate this Plug-In. This memory area can be used by the
Plug-In freely, it is handed over with every function call so that the Plug-In can access its
values.
@return pointer where the allocated and pre-initialized memory area starts
*/
OAPC_EXT_API void* oapc_create_instance2(unsigned long flags)
{
   flags=flags; // removing "unused" warning

   struct instData *data;

   data=(struct instData*)malloc(sizeof(struct instData));
   data->bmLastUsed=NULL;
   data->m_digi=0;
   data->m_prevDigi=0;
   data->w=0;
   data->h=0;
   data->hmiData.pipeType=1;

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
   wxInt32  w,h;
   wxImage         *image=NULL;

   data=(struct instData*)instanceData;

   canvas->GetClientSize(&w,&h);
   if ((w!=data->w) || (h!=data->h)) data->bmLastUsed=NULL;

   if (!data->bmLastUsed)
   {
      std::list<struct bitmapInfo*>::iterator it;
      struct bitmapInfo                      *bmInfo=NULL;

      for (it=bmInfoList.begin(); it!=bmInfoList.end(); it++)
      {
         if (((*it)->w==w) && ((*it)->h==h)) 
         {
            bmInfo=(*it);
            break;
         }
      }
      
      if (!bmInfo)
      {
         bmInfo=(struct bitmapInfo*)malloc(sizeof(struct bitmapInfo));
         wxASSERT(bmInfo);
         if (!bmInfo) return;
         memset(bmInfo,0,sizeof(struct bitmapInfo));
         bmInfo->w=w;
         bmInfo->h=h;
         bmInfoList.push_back(bmInfo);
      }
      switch (data->hmiData.pipeType)
      {
         case 1: // horizontal
            if (!bmInfo->bmHoriz)
            {
              	wxMemoryInputStream stream(simple,sizeof(simple));
               image=new wxImage(stream,wxBITMAP_TYPE_PNG,-1);
               image->Rescale(image->GetWidth(),h,wxIMAGE_QUALITY_HIGH);
               image->Rescale(w,h);
               bmInfo->bmHoriz=new wxBitmap(*image);
            }
            data->bmLastUsed=bmInfo->bmHoriz;
            break;
         case 2: // vertical
            if (!bmInfo->bmVert)
            {
               wxImage img;

              	wxMemoryInputStream stream(simple,sizeof(simple));
               image=new wxImage(stream,wxBITMAP_TYPE_PNG,-1);
               img=image->Rotate90(false);
               delete image;
               image=new wxImage(img);
               image->Rescale(w,image->GetHeight(),wxIMAGE_QUALITY_HIGH);
               image->Rescale(w,h);
               bmInfo->bmVert=new wxBitmap(*image);
            }
            data->bmLastUsed=bmInfo->bmVert;
            break;
         case 3: // T up
            if (!bmInfo->bmT[0])
            {
              	wxMemoryInputStream stream(t_up,sizeof(t_up));
               image=new wxImage(stream,wxBITMAP_TYPE_PNG,-1);
               image->Rescale(w,h,wxIMAGE_QUALITY_HIGH);
               bmInfo->bmT[0]=new wxBitmap(*image);
            }
            data->bmLastUsed=bmInfo->bmT[0];
            break;
         case 4: // T right
            if (!bmInfo->bmT[1])
            {
              	wxMemoryInputStream stream(t_right,sizeof(t_right));
               image=new wxImage(stream,wxBITMAP_TYPE_PNG,-1);
               image->Rescale(w,h,wxIMAGE_QUALITY_HIGH);
               bmInfo->bmT[1]=new wxBitmap(*image);
            }
            data->bmLastUsed=bmInfo->bmT[1];
            break;
         case 5: // T down
            if (!bmInfo->bmT[2])
            {
              	wxMemoryInputStream stream(t_down,sizeof(t_down/*bumidil*/));
               image=new wxImage(stream,wxBITMAP_TYPE_PNG,-1);
               image->Rescale(w,h,wxIMAGE_QUALITY_HIGH);
               bmInfo->bmT[2]=new wxBitmap(*image);
            }
            data->bmLastUsed=bmInfo->bmT[2];
            break;
         case 6: // T left
            if (!bmInfo->bmT[3])
            {
              	wxMemoryInputStream stream(t_left,sizeof(t_left));
               image=new wxImage(stream,wxBITMAP_TYPE_PNG,-1);
               image->Rescale(w,h,wxIMAGE_QUALITY_HIGH);
               bmInfo->bmT[3]=new wxBitmap(*image);
            }
            data->bmLastUsed=bmInfo->bmT[3];
            break;
         case 8: // elbow right-down_black
            if (!bmInfo->bmElbow[data->hmiData.pipeType-7])
            {
              	wxMemoryInputStream stream(elbow_inner,sizeof(elbow_inner));
               image=new wxImage(stream,wxBITMAP_TYPE_PNG,-1);
               image->Rescale(w,h,wxIMAGE_QUALITY_HIGH);
               bmInfo->bmElbow[data->hmiData.pipeType-7]=new wxBitmap(*image);
            }
            data->bmLastUsed=bmInfo->bmElbow[data->hmiData.pipeType-7];
            break;
         case 7: // elbow up-right
         case 9: // elbow bottom-left
            if (!bmInfo->bmElbow[data->hmiData.pipeType-7])
            {
               wxImage img;

              	wxMemoryInputStream stream(elbow_switch,sizeof(elbow_switch));
               image=new wxImage(stream,wxBITMAP_TYPE_PNG,-1);
               if (data->hmiData.pipeType!=7)
               {
                  if (data->hmiData.pipeType==9)
                  {
                     img=image->Rotate90();
                     delete image;
                     image=new wxImage(img);
                     img=image->Mirror();
                  }
                  delete image;
                  image=new wxImage(img);
               }
               image->Rescale(w,h,wxIMAGE_QUALITY_HIGH);
               bmInfo->bmElbow[data->hmiData.pipeType-7]=new wxBitmap(*image);
            }
            data->bmLastUsed=bmInfo->bmElbow[data->hmiData.pipeType-7];
            break;
         case 10: // elbow left-up
            if (!bmInfo->bmElbow[data->hmiData.pipeType-7])
            {
               wxImage img;

              	wxMemoryInputStream stream(elbow_outer,sizeof(elbow_outer));
               image=new wxImage(stream,wxBITMAP_TYPE_PNG,-1);
               img=image->Rotate90();
               delete image;
               image=new wxImage(img);
               image->Rescale(w,h,wxIMAGE_QUALITY_HIGH);
               bmInfo->bmElbow[data->hmiData.pipeType-7]=new wxBitmap(*image);
            }
            data->bmLastUsed=bmInfo->bmElbow[data->hmiData.pipeType-7];
            break;
         case 11: // horizontal black
            if (!bmInfo->bmHoriz_black)
            {
              	wxMemoryInputStream stream(simple_black,sizeof(simple_black));
               image=new wxImage(stream,wxBITMAP_TYPE_PNG,-1);
               image->Rescale(image->GetWidth(),h,wxIMAGE_QUALITY_HIGH);
               image->Rescale(w,h);
               bmInfo->bmHoriz_black=new wxBitmap(*image);
            }
            data->bmLastUsed=bmInfo->bmHoriz_black;
            break;
         case 12: // vertical_black
            if (!bmInfo->bmVert_black)
            {
               wxImage img;

              	wxMemoryInputStream stream(simple_black,sizeof(simple_black));
               image=new wxImage(stream,wxBITMAP_TYPE_PNG,-1);
               img=image->Rotate90(false);
               delete image;
               image=new wxImage(img);
               image->Rescale(w,image->GetHeight(),wxIMAGE_QUALITY_HIGH);
               image->Rescale(w,h);
               bmInfo->bmVert_black=new wxBitmap(*image);
            }
            data->bmLastUsed=bmInfo->bmVert_black;
            break;
         case 13: // T up
            if (!bmInfo->bmT_black[0])
            {
              	wxMemoryInputStream stream(t_up_black,sizeof(t_up_black));
               image=new wxImage(stream,wxBITMAP_TYPE_PNG,-1);
               image->Rescale(w,h,wxIMAGE_QUALITY_HIGH);
               bmInfo->bmT_black[0]=new wxBitmap(*image);
            }
            data->bmLastUsed=bmInfo->bmT_black[0];
            break;
         case 14: // T right
            if (!bmInfo->bmT_black[1])
            {
              	wxMemoryInputStream stream(t_right_black,sizeof(t_right_black));
               image=new wxImage(stream,wxBITMAP_TYPE_PNG,-1);
               image->Rescale(w,h,wxIMAGE_QUALITY_HIGH);
               bmInfo->bmT_black[1]=new wxBitmap(*image);
            }
            data->bmLastUsed=bmInfo->bmT_black[1];
            break;
         case 15: // T down
            if (!bmInfo->bmT_black[2])
            {
              	wxMemoryInputStream stream(t_down_black,sizeof(t_down_black/*bumidil*/));
               image=new wxImage(stream,wxBITMAP_TYPE_PNG,-1);
               image->Rescale(w,h,wxIMAGE_QUALITY_HIGH);
               bmInfo->bmT_black[2]=new wxBitmap(*image);
            }
            data->bmLastUsed=bmInfo->bmT_black[2];
            break;
         case 16: // T left
            if (!bmInfo->bmT_black[3])
            {
              	wxMemoryInputStream stream(t_left_black,sizeof(t_left_black));
               image=new wxImage(stream,wxBITMAP_TYPE_PNG,-1);
               image->Rescale(w,h,wxIMAGE_QUALITY_HIGH);
               bmInfo->bmT_black[3]=new wxBitmap(*image);
            }
            data->bmLastUsed=bmInfo->bmT_black[3];
            break;
         case 18: // elbow right-down_black
            if (!bmInfo->bmElbow_black[data->hmiData.pipeType-17])
            {
              	wxMemoryInputStream stream(elbow_inner_black,sizeof(elbow_inner_black));
               image=new wxImage(stream,wxBITMAP_TYPE_PNG,-1);
               image->Rescale(w,h,wxIMAGE_QUALITY_HIGH);
               bmInfo->bmElbow_black[data->hmiData.pipeType-17]=new wxBitmap(*image);
            }
            data->bmLastUsed=bmInfo->bmElbow_black[data->hmiData.pipeType-17];
            break;
         case 17: // elbow up-right_black
         case 19: // elbow down-left_black
            if (!bmInfo->bmElbow_black[data->hmiData.pipeType-17])
            {
               wxImage img;

              	wxMemoryInputStream stream(elbow_switch_black,sizeof(elbow_switch_black));
               image=new wxImage(stream,wxBITMAP_TYPE_PNG,-1);
               if (data->hmiData.pipeType!=17)
               {
                  if (data->hmiData.pipeType==19)
                  {
                     img=image->Rotate90();
                     delete image;
                     image=new wxImage(img);
                     img=image->Mirror();
                  }
                  delete image;
                  image=new wxImage(img);
               }
               image->Rescale(w,h,wxIMAGE_QUALITY_HIGH);
               bmInfo->bmElbow_black[data->hmiData.pipeType-17]=new wxBitmap(*image);
            }
            data->bmLastUsed=bmInfo->bmElbow_black[data->hmiData.pipeType-17];
            break;
         case 20: // elbow left-up_black
            if (!bmInfo->bmElbow_black[data->hmiData.pipeType-17])
            {
               wxImage img;

              	wxMemoryInputStream stream(elbow_outer_black,sizeof(elbow_outer_black));
               image=new wxImage(stream,wxBITMAP_TYPE_PNG,-1);
               img=image->Rotate90();
               delete image;
               image=new wxImage(img);
               image->Rescale(w,h,wxIMAGE_QUALITY_HIGH);
               bmInfo->bmElbow_black[data->hmiData.pipeType-17]=new wxBitmap(*image);
            }
            data->bmLastUsed=bmInfo->bmElbow_black[data->hmiData.pipeType-17];
            break;
         default:
            wxASSERT(0);
            break;
      }
      delete image;
      if (data->bmLastUsed) dc->DrawBitmap(*data->bmLastUsed,0,0,true);
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
   *x=20;
   *y=20;
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
   *x=1;
   *y=1;
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
   *x=2000;
   *y=2000;
}



/**
The foreground and background colour of an element are handled by the standard HMI properties dialogue.
This function is called once during first setup of this control and has to return the default colour
values for it. Afterwards these values are stored by the main application, the Plug-In does not have
to take care about them. Within the painting-function (please see above) you can get these colours by
calling canvas->GetBackgroundColour() and canvas->GetForegroundColour()
@param[out] background the default background colour
@param[out] foreground the default foreground colour
*/
OAPC_EXT_API void oapc_get_colours(wxUint32 *background,wxUint32 *foreground)
{
   *foreground=0x000011;
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

   if (input!=0) return OAPC_ERROR_NO_SUCH_IO; // check for valid IO
   data->m_digi=value;
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
   if (data->m_prevDigi!=data->m_digi)
   {
      data->m_prevDigi=data->m_digi;
      *value=data->m_digi;
      return OAPC_OK;
   }
   return OAPC_ERROR_NO_DATA_AVAILABLE;
}


