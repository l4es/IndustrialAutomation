/******************************************************************************

This file is part of ControlRoom process control/HMI software.

ControlRoom is free software: you can redistribute it and/or modify it under
the terms of the GNU General Public License as published by the Free
Software Foundation, either version 3 of the License, or (at your option) any
later version.

ControlRoom is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more
details.

You should have received a copy of the GNU General Public License along with
ControlRoom. If not, see http://www.gnu.org/licenses/

*******************************************************************************

For different licensing and/or usage of the sources apart from GPL or any other
open source license, please contact us at https://openapc.com/contact.php

*******************************************************************************/

#include <wx/wx.h>

#include "iff.h"
#include "globals.h"
#include "flowISConnect.h"
#include "oapc_libio.h"

#ifdef ENV_EDITOR
#include "DlgConfigflowExternalIOLib.h"
#include "XMLDecoder.h"

static char xmltempl[]="<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"?>\n\
<oapc-config>\n\
 <dialogue>\n\
  <stdpanel>\n\
   <name>Node Input Definitions</name>\n\
   <param>\n\
    <name>nodenamein0</name>\n\
    <text>Node Name 0</text>\n\
    <type>string</type>\n\
    <default>%s</default>\n\
    <min>0</min>\n\
    <max>%d</max>\n\
   </param>\n\
   <param>\n\
    <name>nodein0</name>\n\
    <text>Node Input Number</text>\n\
    <type>integer</type>\n\
    <default>%d</default>\n\
    <min>0</min>\n\
    <max>7</max>\n\
   </param>\n\
   <param>\n\
    <name>nodenamein1</name>\n\
    <text>Node Name 1</text>\n\
    <type>string</type>\n\
    <default>%s</default>\n\
    <min>0</min>\n\
    <max>%d</max>\n\
   </param>\n\
   <param>\n\
    <name>nodein1</name>\n\
    <text>Node Input Number</text>\n\
    <type>integer</type>\n\
    <default>%d</default>\n\
    <min>0</min>\n\
    <max>7</max>\n\
   </param>\n\
   <param>\n\
    <name>nodenamein2</name>\n\
    <text>Node Name 2</text>\n\
    <type>string</type>\n\
    <default>%s</default>\n\
    <min>0</min>\n\
    <max>%d</max>\n\
   </param>\n\
   <param>\n\
    <name>nodein2</name>\n\
    <text>Node Input Number</text>\n\
    <type>integer</type>\n\
    <default>%d</default>\n\
    <min>0</min>\n\
    <max>7</max>\n\
   </param>\n\
   <param>\n\
    <name>nodenamein3</name>\n\
    <text>Node Name 3</text>\n\
    <type>string</type>\n\
    <default>%s</default>\n\
    <min>0</min>\n\
    <max>%d</max>\n\
   </param>\n\
   <param>\n\
    <name>nodein3</name>\n\
    <text>Node Input Number</text>\n\
    <type>integer</type>\n\
    <default>%d</default>\n\
    <min>0</min>\n\
    <max>7</max>\n\
   </param>\n\
   <param>\n\
    <name>nodenamein4</name>\n\
    <text>Node Name 4</text>\n\
    <type>string</type>\n\
    <default>%s</default>\n\
    <min>0</min>\n\
    <max>%d</max>\n\
   </param>\n\
   <param>\n\
    <name>nodein4</name>\n\
    <text>Node Input Number</text>\n\
    <type>integer</type>\n\
    <default>%d</default>\n\
    <min>0</min>\n\
    <max>7</max>\n\
   </param>\n\
   <param>\n\
    <name>nodenamein5</name>\n\
    <text>Node Name 5</text>\n\
    <type>string</type>\n\
    <default>%s</default>\n\
    <min>0</min>\n\
    <max>%d</max>\n\
   </param>\n\
   <param>\n\
    <name>nodein5</name>\n\
    <text>Node Input Number</text>""\n\
    <type>integer</type>\n\
    <default>%d</default>\n\
    <min>0</min>\n\
    <max>7</max>\n\
   </param>\n\
   <param>\n\
    <name>nodenamein6</name>\n\
    <text>Node Name 6</text>\n\
    <type>string</type>\n\
    <default>%s</default>\n\
    <min>0</min>\n\
    <max>%d</max>\n\
   </param>\n\
   <param>\n\
    <name>nodein6</name>\n\
    <text>Node Input Number</text>\n\
    <type>integer</type>\n\
    <default>%d</default>\n\
    <min>0</min>\n\
    <max>7</max>\n\
   </param>\n\
   <param>\n\
    <name>nodenamein7</name>\n\
    <text>Node Name 7</text>\n\
    <type>string</type>\n\
    <default>%s</default>\n\
    <min>0</min>\n\
    <max>%d</max>\n\
   </param>\n\
   <param>\n\
    <name>nodein7</name>\n\
    <text>Node Input Number</text>\n\
    <type>integer</type>\n\
    <default>%d</default>\n\
    <min>0</min>\n\
    <max>7</max>\n\
   </param>\n\
  </stdpanel>\n\
  <stdpanel>\n\
   <name>Node Output Definitions</name>\n\
   <param>\n\
    <name>nodenameout0</name>\n\
    <text>Node Name 0</text>\n\
    <type>string</type>\n\
    <default>%s</default>\n\
    <min>0</min>\n\
    <max>%d</max>\n\
   </param>\n\
   <param>\n\
    <name>nodeout0</name>\n\
    <text>Node Output Number</text>\n\
    <type>integer</type>\n\
    <default>%d</default>\n\
    <min>0</min>\n\
    <max>7</max>\n\
   </param>\n\
   <param>\n\
    <name>nodenameout1</name>\n\
    <text>Node Name 1</text>\n\
    <type>string</type>\n\
    <default>%s</default>\n\
    <min>0</min>\n\
    <max>%d</max>\n\
   </param>\n\
   <param>\n\
    <name>nodeout1</name>\n\
    <text>Node Output Number</text>\n\
    <type>integer</type>\n\
    <default>%d</default>\n\
    <min>0</min>\n\
    <max>7</max>\n\
   </param>\n\
   <param>\n\
    <name>nodenameout2</name>\n\
    <text>Node Name 2</text>\n\
    <type>string</type>\n\
    <default>%s</default>\n\
    <min>0</min>\n\
    <max>%d</max>\n\
   </param>\n\
   <param>\n\
    <name>nodeout2</name>\n\
    <text>Node Output Number</text>\n\
    <type>integer</type>\n\
    <default>%d</default>\n\
    <min>0</min>\n\
    <max>7</max>\n\
   </param>\n\
   <param>\n\
    <name>nodenameout3</name>\n\
    <text>Node Name 3</text>\n\
    <type>string</type>\n\
    <default>%s</default>\n\
    <min>0</min>\n\
    <max>%d</max>\n\
   </param>\n\
   <param>\n\
    <name>nodeout3</name>""\n\
    <text>Node Output Number</text>\n\
    <type>integer</type>\n\
    <default>%d</default>\n\
    <min>0</min>\n\
    <max>7</max>\n\
   </param>\n\
   <param>\n\
    <name>nodenameout4</name>\n\
    <text>Node Name 4</text>\n\
    <type>string</type>\n\
    <default>%s</default>\n\
    <min>0</min>\n\
    <max>%d</max>\n\
   </param>\n\
   <param>\n\
    <name>nodeout4</name>\n\
    <text>Node Output Number</text>\n\
    <type>integer</type>\n\
    <default>%d</default>\n\
    <min>0</min>\n\
    <max>7</max>\n\
   </param>\n\
   <param>\n\
    <name>nodenameout5</name>\n\
    <text>Node Name 5</text>\n\
    <type>string</type>\n\
    <default>%s</default>\n\
    <min>0</min>\n\
    <max>%d</max>\n\
   </param>\n\
   <param>\n\
    <name>nodeout5</name>\n\
    <text>Node Output Number</text>\n\
    <type>integer</type>\n\
    <default>%d</default>\n\
    <min>0</min>\n\
    <max>7</max>\n\
   </param>\n\
   <param>\n\
    <name>nodenameout6</name>\n\
    <text>Node Name 6</text>\n\
    <type>string</type>\n\
    <default>%s</default>\n\
    <min>0</min>\n\
    <max>%d</max>\n\
   </param>\n\
   <param>\n\
    <name>nodeout6</name>\n\
    <text>Node Output Number</text>\n\
    <type>integer</type>\n\
    <default>%d</default>\n\
    <min>0</min>\n\
    <max>7</max>\n\
   </param>\n\
   <param>\n\
    <name>nodenameout7</name>\n\
    <text>Node Name 7</text>\n\
    <type>string</type>\n\
    <default>%s</default>\n\
    <min>0</min>\n\
    <max>%d</max>\n\
   </param>\n\
   <param>\n\
    <name>nodeout7</name>\n\
    <text>Node Output Number</text>\n\
    <type>integer</type>\n\
    <default>%d</default>\n\
    <min>0</min>\n\
    <max>7</max>\n\
   </param>\n\
  </stdpanel>\n\
  <helppanel>\n\
   <in0>D0 - data to be sent to an input of a mapped node</in0>\n\
   <in1>D1 - ...</in1>\n\
   <out0>D0 - data received from an output of a mapped node</out0>\n\
   <out1>D1 - ...</out1>\n\
  </helppanel>\n\
 </dialogue>\n\
</oapc-config>\n";
#endif



flowISConnect::flowISConnect():flowObject(NULL)
{
   wxInt32 i;

   this->data.type=FLOW_TYPE_MISC_ISCONNECT;
   m_flowFlags|=FLAG_ALLOW_MOD_FROM_ISERVER;
   this->data.stdIN= OAPC_DIGI_IO0|OAPC_DIGI_IO1|OAPC_NUM_IO2|OAPC_NUM_IO3|OAPC_CHAR_IO4|OAPC_CHAR_IO5|OAPC_BIN_IO6|OAPC_BIN_IO7;
   this->data.stdOUT=OAPC_DIGI_IO0|OAPC_DIGI_IO1|OAPC_NUM_IO2|OAPC_NUM_IO3|OAPC_CHAR_IO4|OAPC_CHAR_IO5|OAPC_BIN_IO6|OAPC_BIN_IO7;
#ifdef ENV_EDITOR
   for (i=0; i<MAX_NUM_IOS; i++)
   {
      m_ISConnectData.inNode[i][0]=0;
      m_ISConnectData.inNodeNum[i]=i;
      m_ISConnectData.outNode[i][0]=0;
      m_ISConnectData.outNodeNum[i]=i;
   }
   strcpy(m_ISConnectData.inNode[0],"my_nodename");
   strcpy(m_ISConnectData.outNode[0],"my_nodename");
#else
   for (i=0; i<MAX_NUM_IOS; i++)
   {
      m_digiValid[i]=false;
      m_numValid[i]=false;
      m_charValid[i]=false;
      m_bin[i]=oapcBinHeadSp();
   }
#endif
   m_outNodesConverted=false;
}



flowISConnect::~flowISConnect()
{
}



#ifdef ENV_EDITOR
void flowISConnect::doDataFlowDialog(bool WXUNUSED(hideISConfig))
{
   XMLDecoder  xmlDecoder(this,(wxWindow*)g_mainWin,NULL,NULL);
   char       *xmldata=NULL;
   wxString    title;

   (void)asprintf(&xmldata,xmltempl,m_ISConnectData.inNode[0], MAX_NAME_LENGTH,m_ISConnectData.inNodeNum[0],
                                    m_ISConnectData.inNode[1], MAX_NAME_LENGTH,m_ISConnectData.inNodeNum[1],
                                    m_ISConnectData.inNode[2], MAX_NAME_LENGTH,m_ISConnectData.inNodeNum[2],
                                    m_ISConnectData.inNode[3], MAX_NAME_LENGTH,m_ISConnectData.inNodeNum[3],
                                    m_ISConnectData.inNode[4], MAX_NAME_LENGTH,m_ISConnectData.inNodeNum[4],
                                    m_ISConnectData.inNode[5], MAX_NAME_LENGTH,m_ISConnectData.inNodeNum[5],
                                    m_ISConnectData.inNode[6], MAX_NAME_LENGTH,m_ISConnectData.inNodeNum[6],
                                    m_ISConnectData.inNode[7], MAX_NAME_LENGTH,m_ISConnectData.inNodeNum[7],

                                    m_ISConnectData.outNode[0],MAX_NAME_LENGTH,m_ISConnectData.outNodeNum[0],
                                    m_ISConnectData.outNode[1],MAX_NAME_LENGTH,m_ISConnectData.outNodeNum[1],
                                    m_ISConnectData.outNode[2],MAX_NAME_LENGTH,m_ISConnectData.outNodeNum[2],
                                    m_ISConnectData.outNode[3],MAX_NAME_LENGTH,m_ISConnectData.outNodeNum[3],
                                    m_ISConnectData.outNode[4],MAX_NAME_LENGTH,m_ISConnectData.outNodeNum[4],
                                    m_ISConnectData.outNode[5],MAX_NAME_LENGTH,m_ISConnectData.outNodeNum[5],
                                    m_ISConnectData.outNode[6],MAX_NAME_LENGTH,m_ISConnectData.outNodeNum[6],
                                    m_ISConnectData.outNode[7],MAX_NAME_LENGTH,m_ISConnectData.outNodeNum[7]);
   xmlDecoder.setData(xmldata);

   title=name+_T(" ")+_("Definition")+name.Format(_T(" (%d)"),data.id);
   DlgConfigflowExternalIOLib dlg(&name,&xmlDecoder,NULL,data.stdOUT,NULL,title,true);
   dlg.ShowModal();
   if (dlg.returnOK)
   {
      wxString name,value;
      double   d;

      while (dlg.getNextPair(&name,&value,NULL))
      {
         value.ToDouble(&d);
         if (!name.Cmp(_T("nodenamein0")))      oapc_unicode_stringToCharUTF8(value,m_ISConnectData.inNode[0],sizeof(m_ISConnectData.inNode[0]));
         else if (!name.Cmp(_T("nodenamein1"))) oapc_unicode_stringToCharUTF8(value,m_ISConnectData.inNode[1],MAX_NAME_LENGTH);
         else if (!name.Cmp(_T("nodenamein2"))) oapc_unicode_stringToCharUTF8(value,m_ISConnectData.inNode[2],MAX_NAME_LENGTH);
         else if (!name.Cmp(_T("nodenamein3"))) oapc_unicode_stringToCharUTF8(value,m_ISConnectData.inNode[3],MAX_NAME_LENGTH);
         else if (!name.Cmp(_T("nodenamein4"))) oapc_unicode_stringToCharUTF8(value,m_ISConnectData.inNode[4],MAX_NAME_LENGTH);
         else if (!name.Cmp(_T("nodenamein5"))) oapc_unicode_stringToCharUTF8(value,m_ISConnectData.inNode[5],MAX_NAME_LENGTH);
         else if (!name.Cmp(_T("nodenamein6"))) oapc_unicode_stringToCharUTF8(value,m_ISConnectData.inNode[6],MAX_NAME_LENGTH);
         else if (!name.Cmp(_T("nodenamein7"))) oapc_unicode_stringToCharUTF8(value,m_ISConnectData.inNode[7],MAX_NAME_LENGTH);
         else if (!name.Cmp(_T("nodenameout0"))) oapc_unicode_stringToCharUTF8(value,m_ISConnectData.outNode[0],MAX_NAME_LENGTH);
         else if (!name.Cmp(_T("nodenameout1"))) oapc_unicode_stringToCharUTF8(value,m_ISConnectData.outNode[1],MAX_NAME_LENGTH);
         else if (!name.Cmp(_T("nodenameout2"))) oapc_unicode_stringToCharUTF8(value,m_ISConnectData.outNode[2],MAX_NAME_LENGTH);
         else if (!name.Cmp(_T("nodenameout3"))) oapc_unicode_stringToCharUTF8(value,m_ISConnectData.outNode[3],MAX_NAME_LENGTH);
         else if (!name.Cmp(_T("nodenameout4"))) oapc_unicode_stringToCharUTF8(value,m_ISConnectData.outNode[4],MAX_NAME_LENGTH);
         else if (!name.Cmp(_T("nodenameout5"))) oapc_unicode_stringToCharUTF8(value,m_ISConnectData.outNode[5],MAX_NAME_LENGTH);
         else if (!name.Cmp(_T("nodenameout6"))) oapc_unicode_stringToCharUTF8(value,m_ISConnectData.outNode[6],MAX_NAME_LENGTH);
         else if (!name.Cmp(_T("nodenameout7"))) oapc_unicode_stringToCharUTF8(value,m_ISConnectData.outNode[7],MAX_NAME_LENGTH);
         else if (!name.Cmp(_T("nodein0"))) m_ISConnectData.inNodeNum[0]=d;
         else if (!name.Cmp(_T("nodein1"))) m_ISConnectData.inNodeNum[1]=d;
         else if (!name.Cmp(_T("nodein2"))) m_ISConnectData.inNodeNum[2]=d;
         else if (!name.Cmp(_T("nodein3"))) m_ISConnectData.inNodeNum[3]=d;
         else if (!name.Cmp(_T("nodein4"))) m_ISConnectData.inNodeNum[4]=d;
         else if (!name.Cmp(_T("nodein5"))) m_ISConnectData.inNodeNum[5]=d;
         else if (!name.Cmp(_T("nodein6"))) m_ISConnectData.inNodeNum[6]=d;
         else if (!name.Cmp(_T("nodein7"))) m_ISConnectData.inNodeNum[7]=d;
         else if (!name.Cmp(_T("nodeout0"))) m_ISConnectData.outNodeNum[0]=d;
         else if (!name.Cmp(_T("nodeout1"))) m_ISConnectData.outNodeNum[1]=d;
         else if (!name.Cmp(_T("nodeout2"))) m_ISConnectData.outNodeNum[2]=d;
         else if (!name.Cmp(_T("nodeout3"))) m_ISConnectData.outNodeNum[3]=d;
         else if (!name.Cmp(_T("nodeout4"))) m_ISConnectData.outNodeNum[4]=d;
         else if (!name.Cmp(_T("nodeout5"))) m_ISConnectData.outNodeNum[5]=d;
         else if (!name.Cmp(_T("nodeout6"))) m_ISConnectData.outNodeNum[6]=d;
         else if (!name.Cmp(_T("nodeout7"))) m_ISConnectData.outNodeNum[7]=d;
      }
   }
   dlg.Destroy();
   free(xmldata);
}
#endif



bool flowISConnect::containsOutput(wxString name)
{
   wxInt32 i;

   if (!m_outNodesConverted) for (i=0; i<MAX_NUM_IOS; i++)
    oapc_unicode_charToStringUTF8(m_ISConnectData.outNode[i],MAX_NAME_LENGTH,&m_outNode[i]);
   m_outNodesConverted=true;

   for (i=0; i<MAX_NUM_IOS; i++)
    if (!name.Cmp(m_outNode[i])) return true;
   return false;
}



wxString flowISConnect::getDefaultName()
{
   return _("Interlock Server Connection");
}



#ifndef ENV_EDITOR

flowObject *flowISConnect::getTargetObjectInfo(wxString objectName,wxInt32 outputNum,wxUint64 *outputFlag)
{
   wxInt32     i;
   wxUint64    bitmask;
   flowObject *object;

   object=g_objectList.getObject(objectName,true);
   wxASSERT(object);
   if (!object) return NULL;

   for (i=0; i<MAX_NUM_IOS; i++)
   {
      if ((m_ISConnectData.outNodeNum[i]==outputNum) && (!objectName.Cmp(m_outNode[i])))
      {
         bitmask=0x01010101<<i;
         *outputFlag=bitmask & data.stdOUT;
         return object;
      }
   }
   return NULL;
}

/**
Sets an digital input and checks if the same thread has set that input before
@param[in] input the flag that specifies the input
@param[in] value the value that has to be set for that input (0 or 1)
@param[in] flowThreadID the Id of the thread that wants to set this value
@return 0 if the same thread already has set a value; the thread has to finish
        in this case to avoid endless loops; 1 if the input value was set correctly
*/
wxByte flowISConnect::setDigitalInput(FlowConnection *connection,wxByte value,wxUint32 *flowThreadID,flowObject *WXUNUSED(object))
{
   flowObject object(NULL);

   if (g_breakThreads) return 0;
   if (!threadIDOK(flowThreadID,false)) return 0;

   object.m_flowFlags=FLAG_MAP_OUTPUT_TO_ISERVER;
   (void)asprintf(&object.nodeNameIn,"/%s/in",m_ISConnectData.inNode[connection->targetInputNum]);
   if (g_flowPool.sendDigiValueToIS(OAPC_DIGI_IO0 << m_ISConnectData.inNodeNum[connection->targetInputNum],value,&object,false)) return 1;
   return 0;
}



wxByte flowISConnect::getDigitalOutput(FlowConnection *connection,wxInt32 *rcode,wxUint64 WXUNUSED(lastInput),wxLongLong WXUNUSED(origCreationTime))
{
   if (g_breakThreads) return 0;
   if (m_digiValid[connection->sourceOutputNum]) *rcode=OAPC_OK;
   else *rcode=OAPC_ERROR_NO_DATA_AVAILABLE;
   m_digiValid[connection->sourceOutputNum]=false;
   return digi[connection->sourceOutputNum];
}



/**
Sets an numerical input and checks if the same thread has set that input before
@param[in] input the flag that specifies the input
@param[in] value the value that has to be set for that input
@param[in] flowThreadID the Id of the thread that wants to set this value
@return 0 if the same thread already has set a value; the thread has to finish
        in this case to avoid endless loops; 1 if the input value was set correctly
*/
wxByte flowISConnect::setNumInput(FlowConnection *connection,wxFloat64 value,wxUint32 *flowThreadID,flowObject *WXUNUSED(object))
{
   flowObject object(NULL);

   if (g_breakThreads) return 0;
   if (!threadIDOK(flowThreadID,false)) return 0;

   object.m_flowFlags=FLAG_MAP_OUTPUT_TO_ISERVER;
   (void)asprintf(&object.nodeNameIn,"/%s/in",m_ISConnectData.inNode[connection->targetInputNum]);
   if (g_flowPool.sendNumValueToIS(OAPC_NUM_IO0 << m_ISConnectData.inNodeNum[connection->targetInputNum],value,&object,false)) return 1;
   return 0;
}



wxFloat64 flowISConnect::getNumOutput(FlowConnection *connection,wxInt32 *rcode,wxUint64 WXUNUSED(lastInput),wxLongLong WXUNUSED(origCreationTime))
{
   if (g_breakThreads) return 0.0;
   if (m_numValid[connection->sourceOutputNum]) *rcode=OAPC_OK;
   else *rcode=OAPC_ERROR_NO_DATA_AVAILABLE;
   m_numValid[connection->sourceOutputNum]=false;
   return m_num[connection->sourceOutputNum];
}



/**
Sets an character input and checks if the same thread has set that input before
@param[in] input the flag that specifies the input
@param[in] value the value that has to be set for that input
@param[in] flowThreadID the Id of the thread that wants to set this value
@return 0 if the same thread already has set a value; the thread has to finish
        in this case to avoid endless loops; 1 if the input value was set correctly
*/
wxByte flowISConnect::setCharInput(FlowConnection *connection,wxString value,wxUint32 *flowThreadID,flowObject *WXUNUSED(object))
{
   flowObject object(NULL);

   if (g_breakThreads) return 0;
   if (!threadIDOK(flowThreadID,false)) return 0;

   object.m_flowFlags=FLAG_MAP_OUTPUT_TO_ISERVER;
   (void)asprintf(&object.nodeNameIn,"/%s/in",m_ISConnectData.inNode[connection->targetInputNum]);
   if (g_flowPool.sendCharValueToIS(OAPC_CHAR_IO0 << m_ISConnectData.inNodeNum[connection->targetInputNum],value,&object,false)) return 1;
   return 0;
}



wxString flowISConnect::getCharOutput(FlowConnection *connection,wxInt32 *rcode,wxUint64 WXUNUSED(lastInput))
{
   if (g_breakThreads) return wxEmptyString;
   if (m_charValid[connection->sourceOutputNum]) *rcode=OAPC_OK;
   else *rcode=OAPC_ERROR_NO_DATA_AVAILABLE;
   m_charValid[connection->sourceOutputNum]=false;
   return m_txt[connection->sourceOutputNum];
}



wxByte flowISConnect::setBinInput(FlowConnection *connection,oapcBinHeadSp &value,wxUint32 *flowThreadID,flowObject *WXUNUSED(object))
{
   flowObject object(NULL);

   if (g_breakThreads) return 0;
   if (!threadIDOK(flowThreadID,false)) return 0;

   object.m_flowFlags=FLAG_MAP_OUTPUT_TO_ISERVER;
   (void)asprintf(&object.nodeNameIn,"/%s/in",m_ISConnectData.inNode[connection->targetInputNum]);
   if (g_flowPool.sendBinValueToIS(OAPC_BIN_IO0 << m_ISConnectData.inNodeNum[connection->targetInputNum],value,&object,false)) return 1;
   return 0;
}



oapcBinHeadSp flowISConnect::getBinOutput(FlowConnection *connection,wxInt32 *rcode,wxUint64 WXUNUSED(lastInput))
{
   oapcBinHeadSp bin;

   if (g_breakThreads) return oapcBinHeadSp();
   bin=m_bin[connection->sourceOutputNum];
   if (bin)
   {
      *rcode=OAPC_OK;
      m_bin[connection->sourceOutputNum]=oapcBinHeadSp();
      return bin;
   }
   *rcode=OAPC_ERROR_NO_DATA_AVAILABLE;
   return oapcBinHeadSp();
}

#else

wxInt32 flowISConnect::saveDATA(wxFile *FHandle,char chunkName[4],bool isCompiled)
{
   wxInt32                  length;
   struct hmiObjectHead     convHead;
   struct flowISConnectData saveData;
   wxInt32                  len,i;
   wxNode                  *node;
   FlowConnection          *connection;

   if (!FHandle) return 0;
   checkConnections();
   FHandle->Write(chunkName,4);
   if (isCompiled) length=sizeof(struct hmiObjectHead)+sizeof(struct flowISConnectData)+sizeof(struct hmiObjectHead)+(flowList.GetCount()*FLOW_CONNECTION_HEAD_SIZE);
   else length=sizeof(struct hmiObjectHead)+sizeof(struct flowISConnectData)+sizeof(struct hmiObjectHead)+(flowList.GetCount()*sizeof(struct flowConnectionData));
   length=htonl(length);
   FHandle->Write(&length,4);

   convHead.version=htonl(1);
   convHead.size=htonl(sizeof(struct flowISConnectData));
   convHead.reserved1=0;
   convHead.reserved2=0;
   len=FHandle->Write(&convHead,sizeof(struct hmiObjectHead));

   saveData.flowData.id       =htonl(data.id);
   saveData.flowData.version  =htonl(1);
   saveData.flowData.usedFlows=htonl(flowList.GetCount());
   saveData.flowData.maxEdges =htonl(MAX_CONNECTION_EDGES);
   saveData.flowData.flowFlags=htonl(m_flowFlags);
   saveData.flowX             =htonl(getFlowPos().x);
   saveData.flowY             =htonl(getFlowPos().y);
   oapc_unicode_stringToCharUTF16BE(name,saveData.store_name,MAX_NAME_LENGTH);
   for (i=0; i<MAX_NUM_IOS; i++)
   {
      strncpy(saveData.inNode[i],m_ISConnectData.inNode[i],MAX_NAME_LENGTH);
      strncpy(saveData.outNode[i],m_ISConnectData.outNode[i],MAX_NAME_LENGTH);
      saveData.inNodeNum[i]=m_ISConnectData.inNodeNum[i];
      saveData.outNodeNum[i]=m_ISConnectData.outNodeNum[i];
   }
   len+=FHandle->Write(&saveData,sizeof(struct flowISConnectData));

   convHead.version=htonl(1);
   if (isCompiled) convHead.size=htonl(FLOW_CONNECTION_HEAD_SIZE); // size of ONE structure element!
   else convHead.size=htonl(sizeof(struct flowConnectionData)); // size of ONE structure element!
   convHead.reserved1=0;
   convHead.reserved2=0;
   len=FHandle->Write(&convHead,sizeof(struct hmiObjectHead));

   node=flowList.GetFirst();
   while (node)
   {
      connection=(FlowConnection*)node->GetData();
      len+=connection->saveFlow(FHandle,isCompiled);
      node=node->GetNext();
   }
   return len;
}
#endif // ENV_EDITOR



wxInt32 flowISConnect::loadDATA(wxFile *FHandle,wxUint32 WXUNUSED(chunkSize),wxUint32 IDOffset,bool isCompiled)
{
   struct hmiObjectHead     convHead;
   wxInt32                  loaded;

   if (!FHandle) return 0;
   loaded=FHandle->Read(&convHead,sizeof(struct hmiObjectHead));

   convHead.size     =ntohl(convHead.size);
   convHead.version  =ntohl(convHead.version);
   convHead.reserved1=ntohl(convHead.reserved1);
   convHead.reserved2=ntohl(convHead.reserved2);
#ifndef ENV_EDITOR
   if (convHead.version!=1)
   {
      g_fileVersionError();
      return loaded;
   }
#endif

   loaded+=FHandle->Read(&m_ISConnectData,convHead.size);
   data.id                           =ntohl(m_ISConnectData.flowData.id);
   m_ISConnectData.flowData.usedFlows=ntohl(m_ISConnectData.flowData.usedFlows);
   m_ISConnectData.flowData.maxEdges =ntohl(m_ISConnectData.flowData.maxEdges);
   m_flowFlags                       =ntohl(m_ISConnectData.flowData.flowFlags);
   m_ISConnectData.flowX        =     ntohl(m_ISConnectData.flowX);
   m_ISConnectData.flowY        =     ntohl(m_ISConnectData.flowY);
   oapc_unicode_charToStringUTF16BE(m_ISConnectData.store_name,MAX_NAME_LENGTH,&name);
#ifdef ENV_EDITOR
   setFlowPos(NULL,wxPoint(m_ISConnectData.flowX,m_ISConnectData.flowY),1,1);
#else
   // createNodeNames(); no creation of node names, this element does not map to the IS indirectly, it handles them actively with custom node names
#endif
   loaded+=flowObject::loadFlow(FHandle,&m_ISConnectData.flowData,IDOffset,true,isCompiled);
   return loaded;
}


