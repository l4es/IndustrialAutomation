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
#include <assert.h>
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
#include "liboapc.h"

#define MAX_XML_SIZE   3500

#pragma pack(8)



struct hmiConfigData
{
   wxUint16 length,version;
   wxUint32 m_startColour[2],m_endColour[2],m_borderColour[2];
   wxUint8  m_borderSize[2],m_flowDir,m_waveNum;
   wxInt32  m_mFlowSpeed[2];
};



struct instData
{
   struct hmiConfigData  hmiData;
   char                  m_digi,m_prevDigi;
   wxFloat64             m_num,m_prevNum;
   wxPanel              *m_canvas;
   wxBitmap             *bm[2];
   wxMemoryDC           *dc[2];
   wxInt32               w,h;
   double                offset;
   bool                  m_running;
};



/** the xml data that define the behaviour and HMI configuration panel within the main application */
static char xmlhmitempl[]="<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"?>\
<oapc-config>\
<dialogue>\
 <dualpanel text=\"Colours\">\
  <param text=\"Start Colour\">\
   <name>startColour0</name>\
   <type>colorbutton</type>\
   <default>%d</default>\
  </param>\
  <param>\
   <name>startColour1</name>\
   <type>colorbutton</type>\
   <default>%d</default>\
  </param>\
  <param text=\"End Colour\">\
   <name>endColour0</name>\
   <type>colorbutton</type>\
   <default>%d</default>\
  </param>\
  <param>\
   <name>endColour1</name>\
   <type>colorbutton</type>\
   <default>%d</default>\
  </param>\
  <param text=\"Border Colour\">\
   <name>borderColour0</name>\
   <type>colorbutton</type>\
   <default>%d</default>\
  </param>\
  <param>\
   <name>borderColour1</name>\
   <type>colorbutton</type>\
   <default>%d</default>\
  </param>\
  <param text=\"Border Size\">\
   <name>borderSize0</name>\
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
  <param text=\"Flow Speed\">\
   <name>flowSpeed0</name>\
   <type>float</type>\
   <default>%f</default>\
   <min>-100</min>\
   <max>100</max>\
   <unit>px/sec</unit>\
  </param>\
  <param>\
   <name>flowSpeed1</name>\
   <type>float</type>\
   <default>%f</default>\
   <min>-100</min>\
   <max>100</max>\
  </param>\
 </dualpanel>\
 <stdpanel text=\"Configuration\">\
  <param text=\"Flow Direction\">\
   <name>flowDir</name>\
   <type>option</type>\
   <value>Horizontal</value>\
   <value>Vertical</value>\
   <default>%d</default>\
  </param>\
  <param text=\"Number of Waves\">\
   <name>waveNum</name>\
   <type>integer</type>\
   <default>%d</default>\
   <min>1</min>\
   <max>20</max>\
  </param>\
 </stdpanel>\
</dialogue>\
</oapc-config>\n";

static char                          libname[]="Flow Indicator";
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
   return OAPC_DIGI_IO0|OAPC_NUM_IO7;
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

   snprintf(xmlhmidescr,MAX_XML_SIZE,xmlhmitempl,data->hmiData.m_startColour[0],data->hmiData.m_startColour[1],
                                                 data->hmiData.m_endColour[0],data->hmiData.m_endColour[1],
                                                 data->hmiData.m_borderColour[0],data->hmiData.m_borderColour[1],
                                                 data->hmiData.m_borderSize[0],data->hmiData.m_borderSize[1],
                                                 data->hmiData.m_mFlowSpeed[0]/1000.0,data->hmiData.m_mFlowSpeed[1]/1000.0,
                                                 data->hmiData.m_flowDir,data->hmiData.m_waveNum);
   assert(strlen(xmlhmidescr)<MAX_XML_SIZE);
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

   if (strcmp(name,"startColour0")==0) data->hmiData.m_startColour[0]=atoi(value);
   else if (strcmp(name,"startColour1")==0) data->hmiData.m_startColour[1]=atoi(value);
   else if (strcmp(name,"endColour0")==0) data->hmiData.m_endColour[0]=atoi(value);
   else if (strcmp(name,"endColour1")==0) data->hmiData.m_endColour[1]=atoi(value);
   else if (strcmp(name,"borderColour0")==0) data->hmiData.m_borderColour[0]=atoi(value);
   else if (strcmp(name,"borderColour1")==0) data->hmiData.m_borderColour[1]=atoi(value);
   else if (strcmp(name,"borderSize0")==0) data->hmiData.m_borderSize[0]=atoi(value);
   else if (strcmp(name,"borderSize1")==0) data->hmiData.m_borderSize[1]=atoi(value);
   else if (strcmp(name,"flowSpeed0")==0) data->hmiData.m_mFlowSpeed[0]=OAPC_ROUND(oapc_util_atof(value)*1000.0,0);
   else if (strcmp(name,"flowSpeed1")==0) data->hmiData.m_mFlowSpeed[1]=OAPC_ROUND(oapc_util_atof(value)*1000.0,0);
   else if (strcmp(name,"flowDir")==0) data->hmiData.m_flowDir=atoi(value);
   else if (strcmp(name,"waveNum")==0) data->hmiData.m_waveNum=atoi(value);
   data->w=0; // force recalculation of bitmap
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
   saveConfig.version          =htons(1);
   saveConfig.length           =htons(*length);
   saveConfig.m_startColour[0]=htonl(data->hmiData.m_startColour[0]);   saveConfig.m_startColour[1]=htonl(data->hmiData.m_startColour[1]);
   saveConfig.m_endColour[0]=htonl(data->hmiData.m_endColour[0]);       saveConfig.m_endColour[1]=htonl(data->hmiData.m_endColour[1]);
   saveConfig.m_borderColour[0]=htonl(data->hmiData.m_borderColour[0]); saveConfig.m_borderColour[1]=htonl(data->hmiData.m_borderColour[1]);
   saveConfig.m_borderSize[0]=data->hmiData.m_borderSize[0];            saveConfig.m_borderSize[1]=data->hmiData.m_borderSize[1];
   saveConfig.m_mFlowSpeed[0]=htonl(data->hmiData.m_mFlowSpeed[0]);     saveConfig.m_mFlowSpeed[1]=htonl(data->hmiData.m_mFlowSpeed[1]);
   saveConfig.m_flowDir=data->hmiData.m_flowDir;
   saveConfig.m_waveNum=data->hmiData.m_waveNum;

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

   data->hmiData.m_startColour[0]=ntohl(saveConfig.m_startColour[0]);   data->hmiData.m_startColour[1]=ntohl(saveConfig.m_startColour[1]);
   data->hmiData.m_endColour[0]=ntohl(saveConfig.m_endColour[0]);       data->hmiData.m_endColour[1]=ntohl(saveConfig.m_endColour[1]);
   data->hmiData.m_borderColour[0]=ntohl(saveConfig.m_borderColour[0]); data->hmiData.m_borderColour[1]=ntohl(saveConfig.m_borderColour[1]);
   data->hmiData.m_borderSize[0]=saveConfig.m_borderSize[0];            data->hmiData.m_borderSize[1]=saveConfig.m_borderSize[1];
   data->hmiData.m_mFlowSpeed[0]=ntohl(saveConfig.m_mFlowSpeed[0]);     data->hmiData.m_mFlowSpeed[1]=ntohl(saveConfig.m_mFlowSpeed[1]);
   data->hmiData.m_flowDir=saveConfig.m_flowDir;
   data->hmiData.m_waveNum=saveConfig.m_waveNum;
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
   data->hmiData.m_startColour[0]=0xA00000;  data->hmiData.m_startColour[1]=0x0000A0;
   data->hmiData.m_endColour[0]=0xFFE0E0;    data->hmiData.m_endColour[1]=0xE0E0FF;
   data->hmiData.m_borderColour[0]=0x000000; data->hmiData.m_borderColour[1]=0x000000;
   data->hmiData.m_borderSize[0]=1;          data->hmiData.m_borderSize[1]=1;
   data->hmiData.m_mFlowSpeed[0]=15*1000;    data->hmiData.m_mFlowSpeed[1]=40*1000;
   data->hmiData.m_flowDir=1;
   data->hmiData.m_waveNum=1;
   data->m_num=0;
   data->m_prevNum=0;
   data->m_digi=0;
   data->m_prevDigi=0;
   data->bm[0]=NULL;                         data->bm[1]=NULL;
   data->dc[0]=NULL;                         data->dc[1]=NULL;
   data->m_canvas=NULL;
   data->w=0;
   data->h=0;
   data->offset=0;
   data->m_running=true;

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



void* FlowLoop( void*  arg )
{
   struct instData *data;

   data=(struct instData*)arg;

   while (data->m_running)
   {
      if (data->m_canvas)
      {
         data->offset-=(data->hmiData.m_mFlowSpeed[(int)data->m_digi]/1000.0)/20.0;
         if (data->hmiData.m_flowDir==1) // horizontal flowdir
         {
            while (((int)data->offset)>=data->w) data->offset-=data->w;
            while (((int)data->offset)<0) data->offset+=data->w;
         }
         else
         {
            while (((int)data->offset)>=data->h) data->offset-=data->h;
            while (((int)data->offset)<0) data->offset+=data->h;
         }
         wxMutexGuiEnter();
         data->m_canvas->Refresh();
         wxMutexGuiLeave();
      }
      oapc_util_thread_sleep(50);
   }
   data->m_running=true;
   return NULL;
}



OAPC_EXT_API unsigned long oapc_init(void* instanceData)
{
   struct instData *data;

   data=(struct instData*)instanceData;

   if (!oapc_thread_create(FlowLoop,data))
   {
      oapc_exit(instanceData);
      return OAPC_ERROR_NO_MEMORY;
   }
   return OAPC_OK;
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
   data->m_running=false;
   for (i=0; i<200; i++)
   {
      if (data->m_running) break; // thread has finished
      oapc_util_thread_sleep(10);
   }
   if (data->dc[0]) delete data->dc[0];
   if (data->dc[1]) delete data->dc[1];
   if (data->bm[0]) delete data->bm[0];
   if (data->bm[1]) delete data->bm[1];

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
   struct instData *data;
   wxInt32          w,h,i;

   data=(struct instData*)instanceData;

   canvas->GetClientSize(&w,&h);
   if ((w!=data->w) || (h!=data->h))
   {
      if (data->dc[0]) delete data->dc[0];
      if (data->dc[1]) delete data->dc[1];
      if (data->bm[0]) delete data->bm[0];
      if (data->bm[1]) delete data->bm[1];
      data->bm[0]=NULL;
      data->bm[1]=NULL;
   }

   for (i=0; i<2; i++) if (!data->bm[i])
   {
      wxBitmap *bm;
      wxImage   img;
      wxPen    *pen;
      wxBrush  *brush;
      int       bmSize,j,posCnt=0;
      
      bmSize=(data->hmiData.m_waveNum*5);
      if (data->hmiData.m_flowDir==1) // horizontal flow
      {
         bm=new wxBitmap(bmSize,1);
         img=bm->ConvertToImage();
         for (j=0; j<data->hmiData.m_waveNum; j++)
         {
            img.SetRGB(posCnt,0,data->hmiData.m_startColour[i] & 0xFF,
                               (data->hmiData.m_startColour[i] >> 8) & 0xFF,
                               (data->hmiData.m_startColour[i] >> 16) & 0xFF);
            posCnt++;
            img.SetRGB(posCnt,0,data->hmiData.m_startColour[i] & 0xFF,
                               (data->hmiData.m_startColour[i] >> 8) & 0xFF,
                               (data->hmiData.m_startColour[i] >> 16) & 0xFF);
            posCnt++;
            img.SetRGB(posCnt,0,data->hmiData.m_endColour[i] & 0xFF,
                               (data->hmiData.m_endColour[i] >> 8) & 0xFF,
                               (data->hmiData.m_endColour[i] >> 16) & 0xFF);
            posCnt++;
            img.SetRGB(posCnt,0,data->hmiData.m_endColour[i] & 0xFF,
                               (data->hmiData.m_endColour[i] >> 8) & 0xFF,
                               (data->hmiData.m_endColour[i] >> 16) & 0xFF);
            posCnt++;
            img.SetRGB(posCnt,0,data->hmiData.m_startColour[i] & 0xFF,
                          (data->hmiData.m_startColour[i] >> 8) & 0xFF,
                          (data->hmiData.m_startColour[i] >> 16) & 0xFF);
            posCnt++;
         }
      }
      else
      {
         bm=new wxBitmap(1,bmSize);
         img=bm->ConvertToImage();
         for (j=0; j<data->hmiData.m_waveNum; j++)
         {
            img.SetRGB(0,posCnt,data->hmiData.m_startColour[i] & 0xFF,
                               (data->hmiData.m_startColour[i] >> 8) & 0xFF,
                               (data->hmiData.m_startColour[i] >> 16) & 0xFF);
            posCnt++;
            img.SetRGB(0,posCnt,data->hmiData.m_startColour[i] & 0xFF,
                               (data->hmiData.m_startColour[i] >> 8) & 0xFF,
                               (data->hmiData.m_startColour[i] >> 16) & 0xFF);
            posCnt++;
            img.SetRGB(0,posCnt,data->hmiData.m_endColour[i] & 0xFF,
                               (data->hmiData.m_endColour[i] >> 8) & 0xFF,
                               (data->hmiData.m_endColour[i] >> 16) & 0xFF);
            posCnt++;
            img.SetRGB(0,posCnt,data->hmiData.m_endColour[i] & 0xFF,
                               (data->hmiData.m_endColour[i] >> 8) & 0xFF,
                               (data->hmiData.m_endColour[i] >> 16) & 0xFF);
            posCnt++;
            img.SetRGB(0,posCnt,data->hmiData.m_startColour[i] & 0xFF,
                               (data->hmiData.m_startColour[i] >> 8) & 0xFF,
                               (data->hmiData.m_startColour[i] >> 16) & 0xFF);
            posCnt++;
         }
      }
      img.Rescale(w,h,wxIMAGE_QUALITY_HIGH);
      data->bm[i]=new wxBitmap(img);
      data->dc[i]=new wxMemoryDC();
      data->dc[i]->SelectObject(*data->bm[i]);
      pen=new wxPen(data->hmiData.m_borderColour[i]);
      brush=new wxBrush(data->hmiData.m_borderColour[i]);
      data->dc[i]->SetPen(*pen);
      data->dc[i]->SetBrush(*brush);
      if (data->hmiData.m_flowDir==1) // horizontal flow
      {
         data->dc[i]->DrawRectangle(0,0,w,data->hmiData.m_borderSize[i]);
         data->dc[i]->DrawRectangle(0,h-data->hmiData.m_borderSize[i],w,data->hmiData.m_borderSize[i]);
      }
      else
      {
         data->dc[i]->DrawRectangle(0,0,data->hmiData.m_borderSize[i],h);
         data->dc[i]->DrawRectangle(w-data->hmiData.m_borderSize[i],0,data->hmiData.m_borderSize[i],h);
      }
      delete brush;
      delete pen;
      delete bm;
   }
   data->w=w;
   data->h=h;
   data->m_canvas=canvas;
   if (data->offset==0)
    dc->Blit(0,0,             data->w,data->h,data->dc[(int)data->m_digi],0,0,wxCOPY,false,-1,-1);
//    dc->DrawBitmap(*data->bm[(int)data->m_digi],0,0,true);
   else
   {
      int offset=(int)data->offset;

      if (data->hmiData.m_flowDir==1) // horizontal flow
      {
         dc->Blit(0,0,             data->w-offset,data->h,data->dc[(int)data->m_digi],offset,0,wxCOPY,false,-1,-1);
         dc->Blit(data->w-offset,0,offset,data->h,      data->dc[(int)data->m_digi],0,0,     wxCOPY,false,-1,-1);
      }
      else
      {
         dc->Blit(0,0,             data->w,data->h-offset,data->dc[(int)data->m_digi],0,offset,wxCOPY,false,-1,-1);
         dc->Blit(0,data->h-offset,data->w,offset,        data->dc[(int)data->m_digi],0,0,     wxCOPY,false,-1,-1);
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
   *x=40;
   *y=10;
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
   *x=4;
   *y=4;
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
   *x=1000;
   *y=1000;
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
   data->m_num=value;
   data->hmiData.m_mFlowSpeed[(int)data->m_digi]=(wxInt32)OAPC_ROUND(value*1000.0,0);
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

   if ((output!=6) && (output!=7)) return OAPC_ERROR_NO_SUCH_IO;
   *value=data->m_num;
   return OAPC_OK;
}



