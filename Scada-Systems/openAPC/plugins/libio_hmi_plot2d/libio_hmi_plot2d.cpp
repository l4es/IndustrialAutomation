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
#pragma warning (disable: 4018)
#pragma warning (disable: 4100)
#pragma warning (disable: 4663)
#pragma warning (disable: 4284)
#endif



#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <assert.h>

#include <vector>

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
#endif

#include "oapc_libio.h"
#include "mathplot.h"


#define MAX_XML_SIZE   7500

#pragma pack(8)


#define HMI_FLAG_SCI_NUMBERS 0x00000001
#define HMI_FLAG_LOG_DISPLAY 0x00000002

struct hmiConfigData
{
   unsigned short length,version;
   unsigned char  m_inilayers,m_maxlayers,m_orientation,m_mode,m_skipmode;
   unsigned int   m_maxlength;
   unsigned int   m_colour[6][2];
   unsigned char  m_pen[6][2];
   unsigned char  m_style[6][2];
   // version 2
   unsigned int   m_flags;
};



struct instData
{
   struct hmiConfigData  hmiData;
   mpWindow             *m_plot;
   mpLayer              *m_layer[6];
   mpScaleX             *xaxis;
   mpScaleY             *yaxis;
   double                m_minVal,m_maxVal;
   char                  digi;
   bool                  resetLayout;
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
 <stdpanel text=\"Configuration\">\
  <param text=\"Initial Graphs\">\
   <name>inilayers</name>\
   <type>integer</type>\
   <min>1</min>\
   <max>6</max>\
   <default>%d</default>\
  </param>\
  <param text=\"Maximum Graphs\">\
   <name>maxlayers</name>\
   <type>integer</type>\
   <min>1</min>\
   <max>6</max>\
   <default>%d</default>\
  </param>\
  <param text=\"Direction\">\
   <name>mode</name>\
   <type>option</type>\
   <value>X</value>\
   <value>Y</value>\
   <value>X/Y combined</value>\
   <default>%d</default>\
  </param>\
  <param text=\"Maximum Graph Length\">\
   <name>maxlength</name>\
   <type>integer</type>\
   <min>100</min>\
   <max>1000000</max>\
   <default>%d</default>\
  </param>\
  <param text=\"Data Skip Mode\">\
   <name>skipmode</name>\
   <type>option</type>\
   <value>Delete at beginning</value>\
   <value>Reset everything</value>\
   <value>Overwrite from beginning</value>\
   <default>%d</default>\
  </param>\
  <param text=\"Scientific Notation\">\
   <name>scinumbers</name>\
   <type>checkbox</type>""\
   <default>%d</default>\
  </param>\
  <param text=\"Logarithmic Y Axis\">\
   <name>logdisplay</name>\
   <type>checkbox</type>""\
   <default>%d</default>\
  </param>\
 </stdpanel>\
 <dualpanel text=\"\">\
  <name>Graphs</name>\
  <param text=\"Colour 1\">\
   <name>layer1colour0</name>\
   <type>colorbutton</type>\
   <default>%d</default>\
  </param>\
  <param>\
   <name>layer1colour1</name>\
   <type>colorbutton</type>\
   <default>%d</default>\
  </param>\
  <param text=\"Line Size 1\">\
   <name>layer1pen0</name>\
   <type>integer</type>\
   <min>1</min>\
   <max>10</max>\
   <default>%d</default>\
  </param>\
  <param>\
   <name>layer1pen1</name>\
   <type>integer</type>\
   <min>1</min>\
   <max>10</max>\
   <default>%d</default>\
  </param>\
  <param text=\"Line Style 1\">\
   <name>layer1style0</name>\
   <type>option</type>\
   <value>Simple</value>\
   <value>Dotted</value>\
   <value>Short Dashes</value>\
   <value>Dots and dashes</value>\
   <default>%d</default>\
  </param>\
  <param>\
   <name>layer1style1</name>\
   <type>option</type>\
   <value>Simple</value>\
   <value>Dotted</value>\
   <value>Short Dashes</value>\
   <value>Dots and dashes</value>\n""\
   <default>%d</default>\
  </param>\
  <param text=\"Colour 2\">\
   <name>layer2colour0</name>\
   <type>colorbutton</type>\
   <default>%d</default>\
  </param>\
  <param>\
   <name>layer2colour1</name>\
   <type>colorbutton</type>\
   <default>%d</default>\
  </param>\
  <param text=\"Line Size 2\">\
   <name>layer2pen0</name>\
   <type>integer</type>\
   <min>1</min>\
   <max>10</max>\
   <default>%d</default>\
  </param>\
  <param>\
   <name>layer2pen1</name>\
   <type>integer</type>\
   <min>1</min>\
   <max>10</max>\
   <default>%d</default>\
  </param>\
  <param text=\"Line Style 2\">\
   <name>layer2style0</name>\
   <type>option</type>\
   <value>Simple</value>\
   <value>Dotted</value>\
   <value>Short Dashes</value>\
   <value>Dots and dashes</value>\
   <default>%d</default>\
  </param>\
  <param>\
   <name>layer2style1</name>\
   <type>option</type>\
   <value>Simple</value>\
   <value>Dotted</value>\
   <value>Short Dashes</value>\
   <value>Dots and dashes</value>\
   <default>%d</default>\
  </param>\
  <param text=\"Colour 3\">\
   <name>layer3colour0</name>\
   <type>colorbutton</type>\
   <default>%d</default>\
  </param>\
  <param>\
   <name>layer3colour3</name>\
   <type>colorbutton</type>\
   <default>%d</default>\
  </param>\
  <param text=\"Line Size 3\">\
   <name>layer3pen0</name>\
   <type>integer</type>\
   <min>1</min>\
   <max>10</max>\
   <default>%d</default>\
  </param>\
  <param>\
   <name>layer3pen1</name>\
   <type>integer</type>\
   <min>1</min>\
   <max>10</max>\
   <default>%d</default>\
  </param>\
  <param text=\"Line Style 3\">\
   <name>layer3style0</name>\
   <type>option</type>\
   <value>Simple</value>\
   <value>Dotted</value>\
   <value>Short Dashes</value>\
   <value>Dots and dashes</value>\
   <default>%d</default>\
  </param>\
  <param>\
   <name>layer3style1</name>\
   <type>option</type>\
   <value>Simple</value>\
   <value>Dotted</value>\
   <value>Short Dashes</value>\
   <value>Dots and dashes</value>\
   <default>%d</default>\
  </param>\
  <param text=\"Colour A\">\
   <name>layer4colour0</name>\n""\
   <type>colorbutton</type>\
   <default>%d</default>\
  </param>\
  <param>\
   <name>layer4colour1</name>\
   <type>colorbutton</type>\
   <default>%d</default>\
  </param>\
  <param text=\"Line Size A\">\
   <name>layer4pen0</name>\
   <type>integer</type>\
   <min>1</min>\
   <max>10</max>\
   <default>%d</default>\
  </param>\
  <param>\
   <name>layer4pen1</name>\
   <type>integer</type>\
   <min>1</min>\n""\
   <max>10</max>\
   <default>%d</default>\
  </param>\
  <param text=\"Line Style A\">\
   <name>layer4style0</name>\
   <type>option</type>\
   <value>Simple</value>\
   <value>Dotted</value>\
   <value>Short Dashes</value>\
   <value>Dots and dashes</value>\
   <default>%d</default>\
  </param>\
  <param>\
   <name>layer4style1</name>\
   <type>option</type>\
   <value>Simple</value>\
   <value>Dotted</value>\
   <value>Short Dashes</value>\
   <value>Dots and dashes</value>\
   <default>%d</default>\
  </param>\
  <param text=\"Colour B\">\
   <name>layer5colour0</name>\
   <type>colorbutton</type>\
   <default>%d</default>\
  </param>\
  <param>\
   <name>layer5colour1</name>\
   <type>colorbutton</type>\
   <default>%d</default>\
  </param>\
  <param text=\"Line Size B\">\
   <name>layer5pen0</name>\
   <type>integer</type>\
   <min>1</min>\
   <max>10</max>\
   <default>%d</default>\
  </param>\
  <param>\
   <name>layer5pen1</name>\
   <type>integer</type>\
   <min>1</min>\
   <max>10</max>\
   <default>%d</default>\
  </param>\
  <param text=\"Line Style B\">\
   <name>layer5style0</name>\
   <type>option</type>\
   <value>Simple</value>\
   <value>Dotted</value>\
   <value>Short Dashes</value>\
   <value>Dots and dashes</value>\
   <default>%d</default>\
  </param>\
  <param>\
   <name>layer5style1</name>\
   <type>option</type>\
   <value>Simple</value>\
   <value>Dotted</value>\
   <value>Short Dashes</value>\
   <value>Dots and dashes</value>\
   <default>%d</default>\
  </param>\
  <param text=\"Colour C\">\
   <name>layer6colour0</name>\
   <type>colorbutton</type>\
   <default>%d</default>\
  </param>\
  <param>\
   <name>layer6colour1</name>\
   <type>colorbutton</type>\
   <default>%d</default>\
  </param>\
  <param text=\"Line Size C\">\
   <name>layer6pen0</name>\
   <type>integer</type>\
   <min>1</min>\
   <max>10</max>\
   <default>%d</default>\
  </param>\
  <param>\
   <name>layer6pen1</name>\
   <type>integer</type>\
   <min>1</min>\
   <max>10</max>\
   <default>%d</default>\
  </param>\
  <param text=\"Line Style C\">\
   <name>layer6style0</name>\n""\
   <type>option</type>\
   <value>Simple</value>\
   <value>Dotted</value>\
   <value>Short Dashes</value>\
   <value>Dots and dashes</value>\
   <default>%d</default>\
  </param>\
  <param>\
   <name>layer6style1</name>\
   <type>option</type>\
   <value>Simple</value>\
   <value>Dotted</value>\
   <value>Short Dashes</value>\
   <value>Dots and dashes</value>\
   <default>%d</default>\
  </param>\
 </dualpanel>\
</dialogue>\
</oapc-config>\n";

static char                 libname[]="2D Plotter";
static char                 xmlhmidescr[MAX_XML_SIZE+1];
static char                 xmldescr[MAX_XML_SIZE+1];
static char                 flowImage[]="iVBORw0KGgoAAAANSUhEUgAAAGoAAAAyBAMAAABG0lM+AAAAMFBMVEUEAgQEggSEgoQE/vwEAoTEwsT8AoQEgvz8AgQEAvzk5vz8/vwAAAAAAAAAAAAAAACfenqhAAABhUlEQVR42u3UvU6EQBAAYIKxWSsaY7KN0fIaI69gdw21D7AND2BiZ4ehsr5mG5qp5AoC2Zdzd3b2hPA30Jk4ye3dcPPN/kCIol0hIDZnAaAf03sAiNPn9URblZnvV4DqKr22/2Xp03rilD64VhC7VrYXI3GqunGtIHOtbC9G4hTErhVobMVKUGWuFVTYipWg0ngRsBUrQbU5ekpHfvRfvr8QvSKfCBB9JajQJ1iite5ZSoYq6i+WrQYh8Lr2J+CLwtojmJ2Lfxp4O+BlPt7n1dHMxpSiFR7Poag7MRTFRY1QOaXofgU1Qq1hqBEqJ5V/ooLqQnViQ7r1cfZFqk5obFmn4RdYyzBjyVdJQIVJOAoXmNA5FIUdJVPVREq/WIY6uT1ZQcTPzFD1XU9gyDXVtR+S9/QO1BRaU+1Jmu3qMwmFjbr8yldU+0WVSuVG5QhV3uTLym5KKYMfN4lRNOWiqmXza/yc+fq+bqlo02k8zMWB897YNpfZo97mY0FtfGP/qz+hdsUPr4r19iTQeE0AAAAASUVORK5CYII=";
static struct hmiConfigData saveConfig;


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
   return OAPC_HMI_NO_UI_RO|OAPC_HMI_NO_UI_DISABLED;
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
   return OAPC_DIGI_IO0|
          OAPC_NUM_IO1|
          OAPC_NUM_IO2|OAPC_NUM_IO3|OAPC_NUM_IO4|OAPC_NUM_IO5|OAPC_NUM_IO6|OAPC_NUM_IO7;
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

   sprintf(xmlhmidescr,xmlhmitempl,data->hmiData.m_inilayers,data->hmiData.m_maxlayers,data->hmiData.m_mode,data->hmiData.m_maxlength,data->hmiData.m_skipmode,
                                   ((data->hmiData.m_flags & HMI_FLAG_SCI_NUMBERS)==HMI_FLAG_SCI_NUMBERS),((data->hmiData.m_flags & HMI_FLAG_LOG_DISPLAY)==HMI_FLAG_LOG_DISPLAY),
                                   data->hmiData.m_colour[0][0],data->hmiData.m_colour[0][1],data->hmiData.m_pen[0][0],data->hmiData.m_pen[0][1],data->hmiData.m_style[0][0],data->hmiData.m_style[0][1],
                                   data->hmiData.m_colour[1][0],data->hmiData.m_colour[1][1],data->hmiData.m_pen[1][0],data->hmiData.m_pen[1][1],data->hmiData.m_style[1][0],data->hmiData.m_style[1][1],
                                   data->hmiData.m_colour[2][0],data->hmiData.m_colour[2][1],data->hmiData.m_pen[2][0],data->hmiData.m_pen[2][1],data->hmiData.m_style[2][0],data->hmiData.m_style[2][1],
                                   data->hmiData.m_colour[3][0],data->hmiData.m_colour[3][1],data->hmiData.m_pen[3][0],data->hmiData.m_pen[3][1],data->hmiData.m_style[3][0],data->hmiData.m_style[3][1],
                                   data->hmiData.m_colour[4][0],data->hmiData.m_colour[4][1],data->hmiData.m_pen[4][0],data->hmiData.m_pen[4][1],data->hmiData.m_style[4][0],data->hmiData.m_style[4][1],
                                   data->hmiData.m_colour[5][0],data->hmiData.m_colour[5][1],data->hmiData.m_pen[5][0],data->hmiData.m_pen[5][1],data->hmiData.m_style[5][0],data->hmiData.m_style[5][1]
                                   );
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

   if (strcmp(name,"inilayers")==0)             data->hmiData.m_inilayers=atoi(value);
   else if (strcmp(name,"maxlayers")==0)        data->hmiData.m_maxlayers=atoi(value);
   else if (strcmp(name,"mode")==0)             data->hmiData.m_mode=atoi(value);
   else if (strcmp(name,"maxlength")==0)        data->hmiData.m_maxlength=atoi(value);
   else if (strcmp(name,"skipmode")==0)         data->hmiData.m_skipmode=atoi(value);
   else if (strcmp(name,"scinumbers")==0)
   {
      if (atoi(value)==0) data->hmiData.m_flags&=~HMI_FLAG_SCI_NUMBERS;
      else data->hmiData.m_flags|=HMI_FLAG_SCI_NUMBERS;
   }
   else if (strcmp(name,"logdisplay")==0)
   {
      if (atoi(value)==0) data->hmiData.m_flags&=~HMI_FLAG_LOG_DISPLAY;
      else data->hmiData.m_flags|=HMI_FLAG_LOG_DISPLAY;
   }

   else if (strcmp(name,"layer1colour0")==0)    data->hmiData.m_colour[0][0]=atoi(value);
   else if (strcmp(name,"layer1colour1")==0)    data->hmiData.m_colour[0][1]=atoi(value);
   else if (strcmp(name,"layer2colour0")==0)    data->hmiData.m_colour[1][0]=atoi(value);
   else if (strcmp(name,"layer2colour1")==0)    data->hmiData.m_colour[1][1]=atoi(value);
   else if (strcmp(name,"layer3colour0")==0)    data->hmiData.m_colour[2][0]=atoi(value);
   else if (strcmp(name,"layer3colour1")==0)    data->hmiData.m_colour[2][1]=atoi(value);
   else if (strcmp(name,"layer4colour0")==0)    data->hmiData.m_colour[3][0]=atoi(value);
   else if (strcmp(name,"layer4colour1")==0)    data->hmiData.m_colour[3][1]=atoi(value);
   else if (strcmp(name,"layer5colour0")==0)    data->hmiData.m_colour[4][0]=atoi(value);
   else if (strcmp(name,"layer5colour1")==0)    data->hmiData.m_colour[4][1]=atoi(value);
   else if (strcmp(name,"layer6colour0")==0)    data->hmiData.m_colour[5][0]=atoi(value);
   else if (strcmp(name,"layer6colour1")==0)    data->hmiData.m_colour[5][1]=atoi(value);

   else if (strcmp(name,"layer1pen0")==0)       data->hmiData.m_pen[0][0]=atoi(value);
   else if (strcmp(name,"layer1pen1")==0)       data->hmiData.m_pen[0][1]=atoi(value);
   else if (strcmp(name,"layer2pen0")==0)       data->hmiData.m_pen[1][0]=atoi(value);
   else if (strcmp(name,"layer2pen1")==0)       data->hmiData.m_pen[1][1]=atoi(value);
   else if (strcmp(name,"layer3pen0")==0)       data->hmiData.m_pen[2][0]=atoi(value);
   else if (strcmp(name,"layer3pen1")==0)       data->hmiData.m_pen[2][1]=atoi(value);
   else if (strcmp(name,"layer4pen0")==0)       data->hmiData.m_pen[3][0]=atoi(value);
   else if (strcmp(name,"layer4pen1")==0)       data->hmiData.m_pen[3][1]=atoi(value);
   else if (strcmp(name,"layer5pen0")==0)       data->hmiData.m_pen[4][0]=atoi(value);
   else if (strcmp(name,"layer5pen1")==0)       data->hmiData.m_pen[4][1]=atoi(value);
   else if (strcmp(name,"layer6pen0")==0)       data->hmiData.m_pen[5][0]=atoi(value);
   else if (strcmp(name,"layer6pen1")==0)       data->hmiData.m_pen[5][1]=atoi(value);

   else if (strcmp(name,"layer1style0")==0)     data->hmiData.m_style[0][0]=atoi(value);
   else if (strcmp(name,"layer1style1")==0)     data->hmiData.m_style[0][1]=atoi(value);
   else if (strcmp(name,"layer2style0")==0)     data->hmiData.m_style[1][0]=atoi(value);
   else if (strcmp(name,"layer2style1")==0)     data->hmiData.m_style[1][1]=atoi(value);
   else if (strcmp(name,"layer3style0")==0)     data->hmiData.m_style[2][0]=atoi(value);
   else if (strcmp(name,"layer3style1")==0)     data->hmiData.m_style[2][1]=atoi(value);
   else if (strcmp(name,"layer4style0")==0)     data->hmiData.m_style[3][0]=atoi(value);
   else if (strcmp(name,"layer4style1")==0)     data->hmiData.m_style[3][1]=atoi(value);
   else if (strcmp(name,"layer5style0")==0)     data->hmiData.m_style[4][0]=atoi(value);
   else if (strcmp(name,"layer5style1")==0)     data->hmiData.m_style[4][1]=atoi(value);
   else if (strcmp(name,"layer6style0")==0)     data->hmiData.m_style[5][0]=atoi(value);
   else if (strcmp(name,"layer6style1")==0)     data->hmiData.m_style[5][1]=atoi(value);

   if (data->hmiData.m_mode==3) data->hmiData.m_maxlayers=3;
   if (data->hmiData.m_inilayers>data->hmiData.m_maxlayers) data->hmiData.m_inilayers=data->hmiData.m_maxlayers;
   data->resetLayout=true;
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
   saveConfig.version    =htons(2);
   saveConfig.length     =htons((unsigned short)*length);
   saveConfig.m_inilayers=data->hmiData.m_inilayers;
   saveConfig.m_maxlayers=data->hmiData.m_maxlayers;
   saveConfig.m_mode     =data->hmiData.m_mode;
   saveConfig.m_maxlength=htonl(data->hmiData.m_maxlength);
   saveConfig.m_skipmode =data->hmiData.m_skipmode;
   for (i=0; i<6; i++)
   {
      saveConfig.m_colour[i][0]=htonl(data->hmiData.m_colour[i][0]);
      saveConfig.m_colour[i][1]=htonl(data->hmiData.m_colour[i][1]);

      saveConfig.m_pen[i][0]=data->hmiData.m_pen[i][0];
      saveConfig.m_pen[i][1]=data->hmiData.m_pen[i][1];

      saveConfig.m_style[i][0]=data->hmiData.m_style[i][0];
      saveConfig.m_style[i][1]=data->hmiData.m_style[i][1];
   }
   // version 2
   saveConfig.m_flags=htonl(data->hmiData.m_flags);

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

   data->hmiData.version    =ntohs(saveConfig.version);
   data->hmiData.m_inilayers=saveConfig.m_inilayers;
   data->hmiData.m_maxlayers=saveConfig.m_maxlayers;
   data->hmiData.m_mode     =saveConfig.m_mode;
   data->hmiData.m_maxlength=ntohl(saveConfig.m_maxlength);
   data->hmiData.m_skipmode =saveConfig.m_skipmode;

   for (i=0; i<6; i++)
   {
      data->hmiData.m_colour[i][0]=ntohl(saveConfig.m_colour[i][0]);
      data->hmiData.m_colour[i][1]=ntohl(saveConfig.m_colour[i][1]);

      data->hmiData.m_pen[i][0]=saveConfig.m_pen[i][0];
      data->hmiData.m_pen[i][1]=saveConfig.m_pen[i][1];

      data->hmiData.m_style[i][0]=saveConfig.m_style[i][0];
      data->hmiData.m_style[i][1]=saveConfig.m_style[i][1];
   }
   if (data->hmiData.version>=2) data->hmiData.m_flags=ntohl(saveConfig.m_flags);
   else data->hmiData.m_flags=0;
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
   if (!data) return NULL;
   memset(data,0,sizeof(struct instData));

   data->digi=0;
   data->hmiData.m_inilayers=6;
   data->hmiData.m_maxlayers=6;
   data->hmiData.m_mode=1;
   data->hmiData.m_maxlength=1000;
   data->hmiData.m_skipmode=1;
   data->hmiData.m_colour[0][1]=0xFF0000; data->hmiData.m_colour[0][0]=0xA00000;
   data->hmiData.m_colour[1][1]=0x00FF00; data->hmiData.m_colour[1][0]=0x00A000;
   data->hmiData.m_colour[2][1]=0x0000FF; data->hmiData.m_colour[2][0]=0x0000A0;
   data->hmiData.m_colour[3][1]=0xFF00FF; data->hmiData.m_colour[3][0]=0xA000A0;
   data->hmiData.m_colour[4][1]=0xFFFF00; data->hmiData.m_colour[4][0]=0xA0A000;
   data->hmiData.m_colour[5][1]=0x00FFFF; data->hmiData.m_colour[5][0]=0x00A0A0;
   for (i=0; i<6; i++)
   {
      data->hmiData.m_pen[i][1]=2;
      data->hmiData.m_pen[i][0]=1;
   }
   data->hmiData.m_style[0][1]=1; data->hmiData.m_style[0][0]=1;
   data->hmiData.m_style[1][1]=3; data->hmiData.m_style[1][0]=3;
   data->hmiData.m_style[2][1]=4; data->hmiData.m_style[2][0]=4;
   data->hmiData.m_style[3][1]=1; data->hmiData.m_style[3][0]=1;
   data->hmiData.m_style[4][1]=3; data->hmiData.m_style[4][0]=3;
   data->hmiData.m_style[5][1]=4; data->hmiData.m_style[5][0]=4;
   data->hmiData.m_orientation=1;

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



class Plot2D : public mpFXYVector //mpFX
{
private:
   std::vector<double> m_valY,m_valX;
   unsigned int        m_cntX,m_cntY,m_cnt,m_offset;
   double              m_lastX,m_lastY;

public:
   double              m_minVal,m_maxVal;
   double              m_size;
   char                m_mode,m_skipmode;
   unsigned int        m_maxlength;

   Plot2D(bool logDisplay) : mpFXYVector(wxEmptyString,mpALIGN_NE,logDisplay)
   {
      m_cntX=0;
      m_cntY=0;
      m_cnt=0;
      m_offset=0;
      m_minVal=0;
      m_maxVal=0;
      m_mode=0;
      m_size=100;
      m_lastX=0;
      m_lastY=0;
      SetDrawOutsideMargins(true);
   }


   
   virtual double GetMinY()
   {
      if ((m_mode==1) || (m_mode==3))
      {
         return m_minVal;
      }
      else
      {
         return 0.0;
      }
   }



   virtual double GetMaxY()
   {
      if ((m_mode==1) || (m_mode==3))
      {
         return m_maxVal;
      }
      else
      {
         if (m_cnt<m_size) return m_size;
         return m_cnt;
      }
   }



   virtual double GetMinX()
   {
      if (m_mode==1)
      {
         return 0.0;
      }
      else
      {
         return m_minVal;
      }
   }



   virtual double GetMaxX()
   {
      if (m_mode==1)
      {
         if (m_cnt<m_size) return m_size;
         return m_cnt;
      }
      else
      {
         return m_maxVal;
      }
   }



   void ResetData()
   {
      m_valX.clear();
      m_valY.clear();
      m_cnt=0;
      m_cntX=0;
      m_cntY=0;
   }



   void AddValue(double value,bool lowerGraph)
   {
      if (m_cnt>=m_maxlength)
      {
         if (m_skipmode==1)
         {
            m_valX.erase(m_valX.begin());
            m_valY.erase(m_valY.begin());
            m_cnt--;
            m_cntX--;
            m_cntY--;
            m_offset++;
         }
         else if (m_skipmode==3)
         {
            if ((m_cntX>=m_maxlength) || (m_cntY>=m_maxlength))
            {
               m_cntX=0;
               m_cntY=0;
            }
         }
         else ResetData();
      }
      if (value<m_minVal) m_minVal=value;
      else if (value>m_maxVal) m_maxVal=value;
      if (m_mode==1)
      {
         if (m_cntY<m_cnt) m_valY[m_cntY]=value;
         else
         {
            m_valY.push_back(value);
            m_valX.push_back(m_cnt+m_offset);
            m_cnt++;
         }
         m_cntY++;
      }
      else if (m_mode==2)
      {
         if (m_cntX<m_cnt) m_valX[m_cntX]=value;
         else
         {
            m_valY.push_back(m_cnt+m_offset);
            m_valX.push_back(value);
            m_cnt++;
         }
         m_cntX++;
      }
      else if (m_mode==3)
      {
         if (lowerGraph)
         {
            if (m_cntY<m_cnt) m_valY[m_cntY]=value;
            else
            {
               m_valX.push_back(m_lastX);
               m_valY.push_back(value);
               m_lastY=value;
               m_cnt++;
            }
            m_cntY++;
         }
         else
         {
            if (m_cntX<m_cnt) m_valX[m_cntX]=value;
            else
            {
               m_valX.push_back(value);
               m_valY.push_back(m_lastY);
               m_lastX=value;
               m_cnt++;
            }
            m_cntX++;
         }
      }
      SetData(m_valX,m_valY);
   }

};




/**
This function is relevant only to HMI controls that provide or handle numeric values, it specifies
the allowed range the numeric value can handle.
@param[out] minValue the minimum allowed value that can be handled by this control, this value can't
            be smaller than -2100000000
@param[out] maxValue the maximum allowed value that can be handled by this control, this value can't
            be bigger than 2100000000
*/
OAPC_EXT_API void oapc_set_numminmax(void* instanceData,wxFloat32 minValue,wxFloat32 maxValue)
{
   struct instData *data;

   data=(struct instData*)instanceData;

   if ((data->m_minVal!=minValue) || (data->m_maxVal!=maxValue))
   {
      data->m_minVal=minValue;
      data->m_maxVal=maxValue;
      data->resetLayout=true;
   }
}



static void setGraphStyle(struct instData *data, int i)
{
   wxUint32 style=wxSOLID;

   if (data->hmiData.m_style[i][(int)data->digi]==2) data->m_layer[i]->SetContinuity(false);
   else
   {
      data->m_layer[i]->SetContinuity(true);
      if (data->hmiData.m_style[i][(int)data->digi]==3) style=wxSHORT_DASH;
      else if (data->hmiData.m_style[i][(int)data->digi]==4) style=wxDOT_DASH;
   }
   data->m_layer[i]->SetPen(wxPen(wxColour(data->hmiData.m_colour[i][(int)data->digi]),data->hmiData.m_pen[i][(int)data->digi],style));
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
   wxInt32          w,h,i;
   struct instData *data;

   data=(struct instData*)instanceData;

   canvas->GetSize(&w,&h);
   if (data->resetLayout)
   {
      delete data->m_plot;
      data->m_plot=NULL;
      data->resetLayout=false;
   }

   if (!data->m_plot)
   {
      bool logDisplay;

      wxFont font=canvas->GetFont();

      logDisplay=((data->hmiData.m_flags & HMI_FLAG_LOG_DISPLAY)==HMI_FLAG_LOG_DISPLAY);

      if ((data->hmiData.m_flags & HMI_FLAG_SCI_NUMBERS)==HMI_FLAG_SCI_NUMBERS)
      {
         data->xaxis = new mpScaleX(wxT("X"), mpALIGN_BORDER_BOTTOM, true,mpX_SCIENTIFIC);
         data->yaxis = new mpScaleY(wxT("Y"), mpALIGN_LEFT, true,mpY_SCIENTIFIC,logDisplay);
      }
      else
      {
         data->xaxis = new mpScaleX(wxT("X"), mpALIGN_BORDER_BOTTOM, true,mpX_NORMAL);
         data->yaxis = new mpScaleY(wxT("Y"), mpALIGN_LEFT, true,mpY_NORMAL,logDisplay);
      }

      data->xaxis->SetFont(font);
      data->yaxis->SetFont(font);

      data->m_plot = new mpWindow(canvas, -1, wxPoint(0,0), wxSize(100,100), wxSUNKEN_BORDER,data->m_minVal,data->m_maxVal);
      data->m_plot->SetMargins(5,5,10,10);
      data->m_plot->SetFont(font);

      for (i=0; i< data->hmiData.m_maxlayers; i++)
      {
         data->m_layer[i]=new Plot2D(logDisplay);

         setGraphStyle(data,i);

         ((Plot2D*)data->m_layer[i])->m_minVal=data->m_minVal;
         ((Plot2D*)data->m_layer[i])->m_maxVal=data->m_maxVal;
         ((Plot2D*)data->m_layer[i])->m_mode=data->hmiData.m_mode;
         ((Plot2D*)data->m_layer[i])->m_maxlength=data->hmiData.m_maxlength;
         ((Plot2D*)data->m_layer[i])->m_skipmode=data->hmiData.m_skipmode;
         data->m_plot->AddLayer(data->m_layer[i]);
      }
      if (data->hmiData.m_inilayers>data->hmiData.m_maxlayers) for (i=data->hmiData.m_inilayers; i<data->hmiData.m_maxlayers; i++)
       data->m_plot->SetLayerVisible(i,false);
      data->m_plot->AddLayer(data->xaxis);
      data->m_plot->AddLayer(data->yaxis);
      data->m_plot->EnableDoubleBuffer(true);
      data->m_plot->SetMPScrollbars(true);
      data->m_plot->Fit();
   }
   else
   {
      wxInt32 ow,oh;

      data->m_plot->GetSize(&ow,&oh);
      if ((w!=ow) || (h!=oh))
      {
         data->m_plot->SetSize(w,h);
         for (i=0; i< data->hmiData.m_maxlayers; i++)
         {
            if (data->hmiData.m_mode==1) ((Plot2D*)data->m_layer[i])->m_size=w;
            else ((Plot2D*)data->m_layer[i])->m_size=h;
         }
      }
   }
   data->xaxis->SetPen(canvas->GetForegroundColour());
   data->yaxis->SetPen(canvas->GetForegroundColour());
   data->m_plot->SetBackgroundColour(canvas->GetBackgroundColour());
}




/**
Here the default size of the element has to be returned; this size is used when it is created
newly
@param[out] x the default size in x-direction (width)
@param[out] y the default size in y-direction (height)
*/
OAPC_EXT_API void oapc_get_defsize(wxFloat32 *x,wxFloat32 *y)
{
   *x=200;
   *y=140;
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
   *x=50;
   *y=50;
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
   *foreground=0x110000;
   *background=0xFFF0F0;
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
   int              i;

   data=(struct instData*)instanceData;

   if (input!=1) return OAPC_ERROR_NO_SUCH_IO; // check for valid IO
   data->digi=value;
   for (i=0; i<data->hmiData.m_maxlayers; i++) setGraphStyle(data,i);
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
   if (input==1) // command
   {
      int layer=(((int)value)/10)-1,cmd=((int)value) % 10;

      if ((layer<0) || (layer>=6)) return OAPC_ERROR_INVALID_INPUT;

      switch (cmd)
      {
         case 0:
            data->m_plot->SetLayerVisible(layer,false);
            break;
         case 1:
            data->m_plot->SetLayerVisible(layer,true);
            break;
         case 2:
            ((Plot2D*)data->m_layer[layer])->ResetData();
            break;
         default:
            return OAPC_ERROR_INVALID_INPUT;
      }
   }
   else
   {
      if (data->hmiData.m_mode==3)
      {
         if (input<5)
         {
            if (!data->m_layer[input-2]) return OAPC_ERROR;
            ((Plot2D*)data->m_layer[input-2])->AddValue(value,true);
         }
         else
         {
            if (!data->m_layer[input-5]) return OAPC_ERROR;
            ((Plot2D*)data->m_layer[input-5])->AddValue(value,false);
         }
      }
      else
      {
         if (!data->m_layer[input-2]) return OAPC_ERROR;
         ((Plot2D*)data->m_layer[input-2])->AddValue(value,true);
      }
   }
   return OAPC_OK;
}


