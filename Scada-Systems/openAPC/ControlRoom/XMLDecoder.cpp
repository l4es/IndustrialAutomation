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
#include <wx/mstream.h>
#include <wx/colordlg.h>
#include <wx/fontdlg.h>
#include <wx/spinctrl.h>
#include <wx/filename.h>
#include <wx/scrolwin.h>

#include "ExternalIOLib.h"

#ifdef ENV_BEAM
 #include "../CNConstruct/cnco_globals.h"
#endif

#include "common/oapcResourceBundle.h"
#include "XMLDecoder.h"
#include "oapcFloatCtrl.h"
#include "flowObject.h"
#include "MainWin.h"
#include "globals.h"

/*BEGIN_EVENT_TABLE(XMLDecoder, wxEvtHandler)
  EVT_BUTTON  (wxID_ANY,XMLDecoder::OnButtonClick)
END_EVENT_TABLE()*/

static signed char base64decode[256]={-2,-2,-2,-2,-2,-2,-2,-2,-2,-2,-2,-2,-2,-2,-2,-2,
                               -2,-2,-2,-2,-2,-2,-2,-2,-2,-2,-2,-2,-2,-2,-2,-2,
                               -2,-2,-2,-2,-2,-2,-2,-2,-2,-2,-2,62,-2,-2,-2,63,
                               52,53,54,55,56,57,58,59,60,61,-2,-2,-2,-1,-2,-2,
                               -2,0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,
                               15,16,17,18,19,20,21,22,23,24,25,-2,-2,-2,-2,-2,
                               -2,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,
                               41,42,43,44,45,46,47,48,49,50,51,-2,-2,-2,-2,-2,
                               -2,-2,-2,-2,-2,-2,-2,-2,-2,-2,-2,-2,-2,-2,-2,-2,
                               -2,-2,-2,-2,-2,-2,-2,-2,-2,-2,-2,-2,-2,-2,-2,-2,
                               -2,-2,-2,-2,-2,-2,-2,-2,-2,-2,-2,-2,-2,-2,-2,-2,
                               -2,-2,-2,-2,-2,-2,-2,-2,-2,-2,-2,-2,-2,-2,-2,-2,
                               -2,-2,-2,-2,-2,-2,-2,-2,-2,-2,-2,-2,-2,-2,-2,-2,
                               -2,-2,-2,-2,-2,-2,-2,-2,-2,-2,-2,-2,-2,-2,-2,-2,
                               -2,-2,-2,-2,-2,-2,-2,-2,-2,-2,-2,-2,-2,-2,-2,-2,
                               -2,-2,-2,-2,-2,-2,-2,-2,-2,-2,-2,-2,-2,-2,-2,-2
};


uiElementDefinition::uiElementDefinition()
                    :wxObject()
                    ,id(wxID_ANY)
                    ,id2(wxID_ANY)
                    ,idRW(wxID_ANY)
                    ,name(_T(""))
                    ,type(TYPE_UNDEFINED)
                    ,type2(TYPE_UNDEFINED)
                    ,intValue(0)
                    ,intValue2(0)
                    ,min(0)
                    ,min2(0)
                    ,max(0)
                    ,max2(0)
                    ,floatValue(0.0)
                    ,floatValue2(0.0)
                    ,stringValue(_T(""))
                    ,stringValue2(_T(""))
                    ,unit(_T(""))
                    ,uiElement(NULL)
                    ,uiElement2(NULL)
                    ,uiText(NULL)
                    ,uiUnit(NULL)
                    ,disabled(0)
                    ,panelNum(0)
                    ,accuracy(4)
                    ,font(NULL)

{
}



uiElementDefinition::~uiElementDefinition()
{
   if (font) delete font;
}



XMLDecoder::XMLDecoder(flowObject *object,wxWindow *parent,ExternalIOLib *extIOLib,void *instData)
           :DlgConfigflowBase(object,parent,wxEmptyString,false)
{
   m_extIOLib=extIOLib;
   m_instData=instData;
   m_dialogueParent=parent;
	m_object=object;
   m_dialogueXMLNode=NULL;
   m_flowBitmap=NULL;
   cycleTimeField=NULL;
   uiElemList=NULL;
   m_cycleMicros=NULL;
   m_parent=NULL;
#ifndef ENV_BEAM
   m_nameField=NULL;
#endif
//   Connect(wxID_OK,wxEVT_COMMAND_BUTTON_CLICKED,wxCommandEventHandler(XMLDecoder::OnButtonClick),NULL,this);
//   Connect(wxID_CANCEL,wxEVT_COMMAND_BUTTON_CLICKED,wxCommandEventHandler(XMLDecoder::OnButtonClick),NULL,this);
}



XMLDecoder::~XMLDecoder()
{
   // if (m_flowBitmap) delete m_flowBitmap; it is deleted by the destructor of flowObject
   if (m_dialogueXMLNode) delete m_dialogueXMLNode;
   cleanElementList();
}



void XMLDecoder::reInit()
{
#ifndef ENV_BEAM
   m_nameField=NULL;
#endif
}



wxBitmap *XMLDecoder::getFlowBitmap()
{
   return m_flowBitmap;
}



char *XMLDecoder::base64Decode(const wxChar *input,size_t len,wxInt32 *outLen)
{
   char   *out =(char*)malloc(len*3/4 + 5);
   char   *result = out;
   char    vals[4];

   *outLen=0;
   while (len > 0)
   {
      if(len < 4)
      {
         free(result);
         return NULL;
      }

      vals[0] = base64decode[(unsigned char)*input]; input++;
      vals[1] = base64decode[(unsigned char)*input]; input++;
      vals[2] = base64decode[(unsigned char)*input]; input++;
      vals[3] = base64decode[(unsigned char)*input]; input++;

      if(vals[0] < 0 || vals[1] < 0 || vals[2] < -1 || vals[3] < -1)
      {
         len -= 4;
         continue;
      }

      (*outLen)++;
      *out++ = vals[0]<<2 | vals[1]>>4;
      (*outLen)++;
      if(vals[2] >= 0) *out++ = ((vals[1]&0x0F)<<4) | (vals[2]>>2);
      else *out++ = 0;

      (*outLen)++;
      if(vals[3] >= 0) *out++ = ((vals[2]&0x03)<<6) | (vals[3]);
      else *out++ = 0;

      len -= 4;
   }
   *out = 0;
   return result;
}



void XMLDecoder::parseXMLDescr(wxXmlNode *node)
{
   wxXmlNode *childNode;

   wxASSERT(node);
   if (!node) return;
   if (node->GetName().CmpNoCase(_T("flowimage"))==0)
   {
      if (!m_flowBitmap)
      {
         char    *img;
         wxImage *decImg;
         wxInt32  outLen;
         wxString nodeContent=node->GetNodeContent();

         img=base64Decode(nodeContent.c_str(),nodeContent.Length(),&outLen);
         assert(outLen>0);
         if (outLen>0)
         {
            wxMemoryInputStream  inStream(img,outLen);
            decImg=new wxImage(inStream,wxBITMAP_TYPE_PNG);
            m_flowBitmap=new wxBitmap(*decImg);
            free(img);
            delete decImg;
         }
      }
   }
   else if (node->GetName().CmpNoCase(_T("dialogue"))==0)
   {
       if (m_dialogueXMLNode) delete m_dialogueXMLNode;
       m_dialogueXMLNode=new wxXmlNode(*node);
   }
   else if (node->GetName().CmpNoCase(_T("oapc-config"))==0)
   {
      childNode=node->GetChildren();
      while (childNode)
      {
         parseXMLDescr(childNode);
         childNode=childNode->GetNext();
      }
   }
//   else wxLogDebug(_T("Unknown node %s"),node->GetName().c_str());
}



void XMLDecoder::setData(char *data)
{
   wxXmlDocument       *xmlDoc;

   if (!data) return;
   wxMemoryInputStream  inStream(data,strlen(data));
   xmlDoc=new wxXmlDocument(inStream);
   parseXMLDescr(xmlDoc->GetRoot());
#ifndef _DEBUG
   delete xmlDoc; // windows CRT problem workaround
#endif
}


void XMLDecoder::setInstanceData(void *instData)
{
   m_instData=instData;
}


#if wxCHECK_VERSION(3,0,0)
#else
void XMLDecoder::toDouble(wxString &tmpString,double *d)
{
   if (!tmpString.ToDouble(d))
   {
      tmpString.Replace(_T("."),_T(","));
      if (!tmpString.ToDouble(d))
      {
         tmpString.Replace(_T(","),_T("."));
         tmpString.ToDouble(d);
      }
   }
}
#endif


bool XMLDecoder::setElemDefDefault(uiElementDefinition *elemDef,wxString &tmpString)
{
   if ((elemDef->type==TYPE_STRING) || (elemDef->type==TYPE_FILELOAD) || (elemDef->type==TYPE_FILESAVE) ||
       (elemDef->type==TYPE_DIRSELECT))
    elemDef->stringValue=tmpString;
   else if ((elemDef->type==TYPE_INT)      || (elemDef->type==TYPE_COLORBUTTON) ||
            (elemDef->type==TYPE_CHECKBOX) || 
            (elemDef->type==TYPE_OPTION))
   {
      long l;
 
      tmpString.ToLong(&l);
      elemDef->intValue=l;
   }
   else if (elemDef->type==TYPE_FLOAT)
   {
      double   d;

/*      if (!tmpString.ToDouble(&d))
      {
         tmpString.Replace(_T("."),_T(","));
         if (!tmpString.ToDouble(&d))
         {
            tmpString.Replace(_T(","),_T("."));
            tmpString.ToDouble(&d);
         }
      }*/
#if wxCHECK_VERSION(3,0,0)
      tmpString.ToCDouble(&d);
#else
      toDouble(tmpString,&d);
#endif
      elemDef->floatValue=(float)d;
   }
   else
   {
      wxASSERT(0);
      wxMessageBox(_("Plug-In error: the default value has to be specified after the type definition,\naborting operation!"),_("Error"),wxICON_ERROR);
      delete elemDef;
      return false;
   }
   return true;
}



bool XMLDecoder::setElemDefType(uiElementDefinition *elemDef,wxString &tmpString,wxPanel &parentPanel,wxComboBox **comboBox,int setNum)
{
   if (tmpString.CmpNoCase(_T("string"))==0)
   {
      if (setNum==1) elemDef->type=TYPE_STRING;
      else elemDef->type2=TYPE_STRING;
   }
   else if (tmpString.CmpNoCase(_T("fileload"))==0)
    elemDef->type=TYPE_FILELOAD;
   else if (tmpString.CmpNoCase(_T("filesave"))==0)
    elemDef->type=TYPE_FILESAVE;
   else if (tmpString.CmpNoCase(_T("dirselect"))==0)
    elemDef->type=TYPE_DIRSELECT;
   else if (tmpString.CmpNoCase(_T("integer"))==0)
   {
      if (setNum==1) elemDef->type=TYPE_INT;
      else elemDef->type2=TYPE_INT;
   }
   else if (tmpString.CmpNoCase(_T("colorbutton"))==0)
    elemDef->type=TYPE_COLORBUTTON;
   else if (tmpString.CmpNoCase(_T("fontbutton"))==0)
    elemDef->type=TYPE_FONTBUTTON;
   else if (tmpString.CmpNoCase(_T("checkbox"))==0)
    elemDef->type=TYPE_CHECKBOX;
   else if (tmpString.CmpNoCase(_T("float"))==0)
   {
      if (setNum==1) elemDef->type=TYPE_FLOAT;
      else elemDef->type2=TYPE_FLOAT;
   }
   else if (tmpString.CmpNoCase(_T("option"))==0)
   {
       elemDef->type=TYPE_OPTION;
       *comboBox=new wxComboBox(&parentPanel,wxID_ANY,_T(""),wxDefaultPosition,wxDefaultSize,0,NULL,wxCB_DROPDOWN|wxCB_READONLY);
   }
   else
   {
//      wxLogDebug(_T("Invalid type: %s"),tmpString.c_str());
      wxMessageBox(_T("\"")+tmpString+_T("\"\n")+_("Plug-In error: invalid type specified for element,\naborting operation!"),_("Error"),wxICON_ERROR);
      delete elemDef;
      return false;
   }
   return true;
}



uiElementDefinition *XMLDecoder::constructUIElements(wxXmlNode *node,wxScrolledWindow &parentPanel,wxSizer &sizer,wxInt32 rowNum,wxByte panelNum)
{
   wxXmlNode           *childNode;
   uiElementDefinition *elemDef;
   wxComboBox          *comboBox=NULL;

   elemDef=new uiElementDefinition(); // TODO: auto_ptr
   wxASSERT(elemDef);
   if (!elemDef)
   {
      wxMessageBox(_("Internal error: not enough memory for data structure,\noperation cancelled!"),_("Error"),wxICON_ERROR);
      return NULL;
   }
   elemDef->panelNum=panelNum;
#if wxCHECK_VERSION(3,0,0)
   node->GetAttribute(wxT("text"),&elemDef->text);
   node->GetAttribute(wxT("name"),&elemDef->name);
#else
   node->GetPropVal(wxT("text"),&elemDef->text);
   node->GetPropVal(wxT("name"),&elemDef->name);
#endif
   childNode=node->GetChildren();
   while (childNode)
   {
      if ((childNode->GetName().CmpNoCase(_T("paramcol"))==0))
      {
         wxString tmpStr;

#if wxCHECK_VERSION(3,0,0)
         childNode->GetAttribute(wxT("name"),&elemDef->name);
         childNode->GetAttribute(wxT("type"),&tmpStr);
#else
         childNode->GetPropVal(wxT("name"),&elemDef->name);
         childNode->GetPropVal(wxT("type"),&tmpStr);
#endif
         if (!setElemDefType(elemDef,tmpStr,parentPanel,&comboBox,1)) return NULL;
#if wxCHECK_VERSION(3,0,0)
         childNode->GetAttribute(wxT("default"), &tmpStr);
#else
         childNode->GetPropVal(wxT("default"),&tmpStr);
#endif
         if (!setElemDefDefault(elemDef,tmpStr)) return NULL;
#if wxCHECK_VERSION(3,0,0)
         if (childNode->GetAttribute(wxT("min"), &tmpStr))
#else
         if (childNode->GetPropVal(wxT("min"),&tmpStr))
#endif
         {
            char tmpC[50+1];

            oapc_unicode_stringToCharASCII(tmpStr,tmpC,50);
            elemDef->min=oapc_util_atof_dot(tmpC);
         }
#if wxCHECK_VERSION(3,0,0)
         if (childNode->GetAttribute(wxT("max"), &tmpStr))
#else
         if (childNode->GetPropVal(wxT("max"),&tmpStr))  
#endif
         {
            char tmpC[50+1];

            oapc_unicode_stringToCharASCII(tmpStr,tmpC,50);
            elemDef->max=oapc_util_atof_dot(tmpC);
         }
#if wxCHECK_VERSION(3,0,0)
         if (childNode->GetAttribute(wxT("unit"), &tmpStr)) elemDef->unit=tmpStr;
#else
         if (childNode->GetPropVal(wxT("unit"),&tmpStr)) elemDef->unit=tmpStr;
#endif
      }
      else if ((childNode->GetName().CmpNoCase(_T("valuecol"))==0))
      {
         wxString tmpStr;

#if wxCHECK_VERSION(3,0,0)
         childNode->GetAttribute(wxT("name"), &elemDef->name2);
         childNode->GetAttribute(wxT("type"), &tmpStr);
#else
         childNode->GetPropVal(wxT("name"),&elemDef->name2);
         childNode->GetPropVal(wxT("type"),&tmpStr);
#endif
         if (!setElemDefType(elemDef,tmpStr,parentPanel,&comboBox,2)) return NULL;
#if wxCHECK_VERSION(3,0,0)
         childNode->GetAttribute(wxT("default"), &tmpStr);
#else
         childNode->GetPropVal(wxT("default"),&tmpStr);
#endif
         if (!setElemDefDefault(elemDef,tmpStr)) return NULL;
#if wxCHECK_VERSION(3,0,0)
         if (childNode->GetAttribute(wxT("min"), &tmpStr))
#else
         if (childNode->GetPropVal(wxT("min"),&tmpStr))
#endif
         {
             double d;
          
#if wxCHECK_VERSION(3,0,0)
             tmpStr.ToCDouble(&d);
#else
             toDouble(tmpStr,&d);
#endif
             elemDef->min2=d;
         }
#if wxCHECK_VERSION(3,0,0)
         if (childNode->GetAttribute(wxT("max"), &tmpStr))
#else
         if (childNode->GetPropVal(wxT("max"),&tmpStr))
#endif
         {
             double d;
          
#if wxCHECK_VERSION(3,0,0)
             tmpStr.ToCDouble(&d);
#else
             toDouble(tmpStr,&d);
#endif
             elemDef->max2=d;
         }
#if wxCHECK_VERSION(3,0,0)
         if (childNode->GetAttribute(wxT("unit"), &tmpStr))
#else
         if (childNode->GetPropVal(wxT("unit"),&tmpStr))
#endif
          elemDef->unit=tmpStr;
      }
      // to be removed in a future version
      else if ((childNode->GetName().CmpNoCase(_T("text"))==0))
       elemDef->text=childNode->GetNodeContent();

      // to be removed in a future version (but more in future than removed <text>
      else if ((childNode->GetName().CmpNoCase(_T("name"))==0))
       elemDef->name=childNode->GetNodeContent();

      else if (((childNode->GetName().CmpNoCase(_T("unit"))==0)) || ((childNode->GetName().CmpNoCase(_T("ffilter"))==0)))
       elemDef->unit=childNode->GetNodeContent();
      else if ((childNode->GetName().CmpNoCase(_T("pointsize"))==0))
      {
         double d;

         if (!elemDef->font) elemDef->font=new wxFont();          
#if wxCHECK_VERSION(3,0,0)
         childNode->GetNodeContent().ToCDouble(&d);
#else
         wxString tmpStr=childNode->GetNodeContent();
         toDouble(tmpStr,&d);
#endif
         elemDef->font->SetPointSize(d);
      }
      else if ((childNode->GetName().CmpNoCase(_T("style"))==0))
      {
         long l;

         if (!elemDef->font) elemDef->font=new wxFont();          
         childNode->GetNodeContent().ToLong(&l);
#if wxCHECK_VERSION(2,9,0)
         elemDef->font->SetStyle((wxFontStyle)l);
#else
         elemDef->font->SetStyle(l);
#endif
      }
      else if ((childNode->GetName().CmpNoCase(_T("weight"))==0))
      {
         long l;

         if (!elemDef->font) elemDef->font=new wxFont();          
         childNode->GetNodeContent().ToLong(&l);
#if wxCHECK_VERSION(2,9,0)
         elemDef->font->SetWeight((wxFontWeight)l);
#else
         elemDef->font->SetWeight(l);
#endif
      }
      else if ((childNode->GetName().CmpNoCase(_T("face"))==0))
      {
         if (!elemDef->font) elemDef->font=new wxFont();
         elemDef->font->SetFaceName(childNode->GetNodeContent());
      }
      else if ((childNode->GetName().CmpNoCase(_T("type"))==0))
      {
         wxString str=childNode->GetNodeContent();

         if (!setElemDefType(elemDef,str,parentPanel,&comboBox,1)) return NULL;
      }
      else if ((childNode->GetName().CmpNoCase(_T("min"))==0))
      {
          double d;
          
#if wxCHECK_VERSION(3,0,0)
          childNode->GetNodeContent().ToCDouble(&d);
#else
          wxString tmpStr=childNode->GetNodeContent();
          toDouble(tmpStr,&d);
#endif
          elemDef->min=d;
      }
      else if ((childNode->GetName().CmpNoCase(_T("max"))==0))
      {
          double d;
          
#if wxCHECK_VERSION(3,0,0)
          childNode->GetNodeContent().ToCDouble(&d);
#else
          wxString tmpStr=childNode->GetNodeContent();
          toDouble(tmpStr,&d);
#endif
          elemDef->max=d;
      }
      else if ((childNode->GetName().CmpNoCase(_T("accuracy"))==0))
      {
          long l;
          
          childNode->GetNodeContent().ToLong(&l);
          elemDef->accuracy=l;
      }
      else if ((childNode->GetName().CmpNoCase(_T("value"))==0))
      {
         wxASSERT(comboBox);
         if (comboBox) comboBox->Append(g_res->getString(childNode->GetNodeContent()));
      }
      else if ((childNode->GetName().CmpNoCase(_T("state"))==0))
      {
         if ((childNode->GetNodeContent().CmpNoCase(_T("disabled"))==0)) elemDef->disabled=1;
//         else wxLogDebug(_T("%s %s"),childNode->GetName().c_str(),childNode->GetNodeContent().c_str());
      }
      else if ((childNode->GetName().CmpNoCase(_T("default"))==0))
      {
         wxString str=childNode->GetNodeContent();

         if (!setElemDefDefault(elemDef,str)) return NULL;
      }
      else if ((childNode->GetName().CmpNoCase(_T("enableon"))==0))
      {
         wxString str=childNode->GetNodeContent();

         if (uiElemList)
         {
            wxNode              *node;
            uiElementDefinition *supElemDef;

            node=uiElemList->GetFirst();
            while (node)
            {
               supElemDef=(uiElementDefinition*)node->GetData();
               if ((supElemDef->name.IsSameAs(str)) && (supElemDef->type==TYPE_CHECKBOX))
               {
                  elemDef->masterEnableList.push_back(supElemDef);
                  supElemDef->slaveEnableList.push_back(elemDef);
                  break;
               }
               node=node->GetNext();
            }
         }
      }
      else if ((childNode->GetName().CmpNoCase(_T("disableon"))==0))
      {
         wxString str=childNode->GetNodeContent();

         if (uiElemList)
         {
            wxNode              *node;
            uiElementDefinition *supElemDef;

            node=uiElemList->GetFirst();
            while (node)
            {
               supElemDef=(uiElementDefinition*)node->GetData();
               if ((supElemDef->name.IsSameAs(str)) && (supElemDef->type==TYPE_CHECKBOX))
               {
                  elemDef->masterDisableList.push_back(supElemDef);
                  supElemDef->slaveDisableList.push_back(elemDef);
                  break;
               }
               node=node->GetNext();
            }
         }
      }
//      else wxLogDebug(_T("%s"),childNode->GetName().c_str());
      childNode=childNode->GetNext();
   }
   if ((rowNum==0) || (rowNum==-1))
   {
      elemDef->uiText=new wxStaticText(&parentPanel,wxID_ANY,g_res->getString(elemDef->text));
      sizer.Add(elemDef->uiText,1,wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL);
   }
   if ((elemDef->type==TYPE_STRING) || (elemDef->type==TYPE_FILELOAD) || (elemDef->type==TYPE_FILESAVE) ||
       (elemDef->type==TYPE_DIRSELECT))
   {
      wxTextCtrl *stringField=new wxTextCtrl(&parentPanel,wxID_ANY,elemDef->stringValue);
      stringField->SetMaxLength(elemDef->max);
      elemDef->uiElement=stringField;
   }
   else if (elemDef->type==TYPE_INT)
   {
      wxSpinCtrl *intField=new wxSpinCtrl(&parentPanel,wxID_ANY,wxEmptyString,wxDefaultPosition,wxDefaultSize,wxSP_ARROW_KEYS|wxTE_LEFT,elemDef->min,elemDef->max,elemDef->intValue);
      intField->SetValidator(wxTextValidator(wxFILTER_NUMERIC));
      elemDef->uiElement=intField;
   }
   else if (elemDef->type==TYPE_FLOAT)
   {
      oapcFloatCtrl *floatField=new oapcFloatCtrl(&parentPanel,wxID_ANY,wxDefaultPosition,wxDefaultSize,wxTE_LEFT|wxTE_PROCESS_ENTER,elemDef->floatValue);
      floatField->Connect(wxEVT_COMMAND_TEXT_ENTER,wxCommandEventHandler(XMLDecoder::FloatCtrlCommandEvent),NULL,this);
      floatField->Connect(wxEVT_KILL_FOCUS,wxFocusEventHandler(XMLDecoder::FloatCtrlFocusEvent),NULL,this);
      floatField->SetAccuracy(elemDef->accuracy);
      floatField->SetValue(elemDef->floatValue); // to let accuracy formatting apply
      elemDef->uiElement=floatField;
   }
   else if (elemDef->type==TYPE_COLORBUTTON)
   {
      wxButton *colButton=new wxButton(&parentPanel,wxID_ANY);
      colButton->Connect(wxEVT_COMMAND_BUTTON_CLICKED,wxCommandEventHandler(XMLDecoder::OnButtonClick),NULL,this);
      colButton->SetBackgroundColour(wxColour(elemDef->intValue));
      elemDef->uiElement=colButton;
   }
   else if (elemDef->type==TYPE_FONTBUTTON)
   {
      wxButton *fontButton=new wxButton(&parentPanel,wxID_ANY,_("AaBbCc123"));
      fontButton->Connect(wxEVT_COMMAND_BUTTON_CLICKED,wxCommandEventHandler(XMLDecoder::OnButtonClick),NULL,this);
      if (elemDef->font) fontButton->SetFont(*elemDef->font);
      elemDef->uiElement=fontButton;
   }
   else if (elemDef->type==TYPE_CHECKBOX)
   {
      wxCheckBox *cBox=new wxCheckBox(&parentPanel,wxID_ANY,_T(" "));
      cBox->Connect(wxEVT_COMMAND_CHECKBOX_CLICKED,wxCommandEventHandler(XMLDecoder::OnCheckBoxCheck),NULL,this);
      if (elemDef->intValue!=0) cBox->SetValue(true);
      elemDef->uiElement=cBox;
   }
   else if (elemDef->type==TYPE_OPTION)
   {
      wxASSERT(comboBox);
      if (comboBox)
      {
         comboBox->Select(elemDef->intValue-1);
         elemDef->uiElement=comboBox;
      }
   }

   if (elemDef->type2==TYPE_INT)
   {
      wxSpinCtrl *intField=new wxSpinCtrl(&parentPanel,wxID_ANY,wxEmptyString,wxDefaultPosition,wxDefaultSize,wxSP_ARROW_KEYS,elemDef->min,elemDef->max,elemDef->intValue);
      intField->SetValidator(wxTextValidator(wxFILTER_NUMERIC));
      elemDef->uiElement2=intField;
   }
   else if (elemDef->type2==TYPE_FLOAT)
   {
      oapcFloatCtrl *floatField=new oapcFloatCtrl(&parentPanel,wxID_ANY,wxDefaultPosition,wxDefaultSize,wxTE_RIGHT,elemDef->floatValue);
      elemDef->uiElement2=floatField;
   }
   
   if (elemDef->uiElement)
   {
      elemDef->id=elemDef->uiElement->GetId();
      if (elemDef->disabled) elemDef->uiElement->Enable(false);
      sizer.Add(elemDef->uiElement,1,wxEXPAND);
      if (elemDef->type==TYPE_FLOAT)
      {
         Connect(elemDef->uiElement->GetId(),wxEVT_COMMAND_TEXT_ENTER,wxCommandEventHandler(XMLDecoder::OnInputChanged));
         Connect(elemDef->uiElement->GetId(),wxEVT_COMMAND_TEXT_UPDATED,wxCommandEventHandler(XMLDecoder::OnInputChanged));
      }
   }

   if (elemDef->uiElement2)
   {
      elemDef->id2=elemDef->uiElement2->GetId();
      sizer.Add(elemDef->uiElement2,1,wxEXPAND);
      if (elemDef->type2==TYPE_FLOAT)
      {
         Connect(elemDef->uiElement2->GetId(),wxEVT_COMMAND_TEXT_ENTER,wxCommandEventHandler(XMLDecoder::OnInputChanged));
         Connect(elemDef->uiElement2->GetId(),wxEVT_COMMAND_TEXT_UPDATED,wxCommandEventHandler(XMLDecoder::OnInputChanged));
      }
   }

   if ((rowNum==1) || (rowNum==-1))
   {
      if ((elemDef->type==TYPE_FILELOAD) || (elemDef->type==TYPE_FILESAVE) || (elemDef->type==TYPE_DIRSELECT)) // add button for filerequester
      {
         wxButton *button=new wxButton(&parentPanel,wxID_ANY,_T("..."),wxDefaultPosition,wxDefaultSize,wxBU_EXACTFIT);
         button->Connect(wxEVT_COMMAND_BUTTON_CLICKED,wxCommandEventHandler(XMLDecoder::OnButtonClick),NULL,this);
         sizer.Add(button,1,wxEXPAND);
         elemDef->id=button->GetId();
      }
      else
      {
         elemDef->uiUnit=new wxStaticText(&parentPanel,wxID_ANY,g_res->getString(elemDef->unit));
         sizer.Add(elemDef->uiUnit,1,wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL);
      }
   }

   uiElemList->Append(elemDef);
   return elemDef;
}


void XMLDecoder::FloatCtrlCommandEvent(wxCommandEvent &event)
{
   wxNode              *node;
   uiElementDefinition *elemDef;

   if (!uiElemList) return;
   node=uiElemList->GetFirst();
   while (node)
   {
      elemDef=(uiElementDefinition*)node->GetData();
      if (elemDef->id==event.GetId())
      {
         oapcFloatCtrl *ctrl;

         ctrl=(oapcFloatCtrl*)elemDef->uiElement;
         ctrl->GetValue(elemDef->min,elemDef->max);
         return;
      }
      node=node->GetNext();
   }
}


void XMLDecoder::FloatCtrlFocusEvent(wxFocusEvent &event)
{
   wxCommandEvent    e;

   e.SetId(event.GetId());
   FloatCtrlCommandEvent(e);
   event.Skip(true);
}


void XMLDecoder::setScrollParams(wxScrolledWindow *rootPanel,wxSizer *fSizer)
{
   rootPanel->EnableScrolling(false,true);
//   rootPanel->ShowScrollbars(wxSHOW_SB_NEVER,wxSHOW_SB_DEFAULT);
   rootPanel->SetScrollRate(0,10);
   rootPanel->SetMinSize(fSizer->Fit(rootPanel));
   rootPanel->SetMaxSize(fSizer->Fit(rootPanel));
}


wxScrolledWindow *XMLDecoder::constructUIBasicPanel(wxXmlNode *node,wxUint64 stdOUT,wxUint32 cycleMicros,wxString *panelName,wxByte panelNum)
{
   wxXmlNode    *childNode;
   wxStaticText *text;

   *panelName=_("Basic");
   // create the basic panel and its sizer
   wxScrolledWindow *rootPanel=new wxScrolledWindow(m_book,wxID_ANY,wxDefaultPosition,wxDefaultSize,0);
//   wxPanel *rootPanel=new wxPanel(m_book,wxID_ANY);
   wxFlexGridSizer *fSizer=new wxFlexGridSizer(3,4,4);
   rootPanel->SetSizer(fSizer);

   /* Name of the element *******************/
#ifndef ENV_BEAM
   text=new wxStaticText(rootPanel,wxID_ANY,_("Name")+_T(":"));
   fSizer->Add(text,0,wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL);

   m_nameField=new oapcNoSpaceCtrl(rootPanel,wxID_ANY,*m_objectName);
   fSizer->Add(m_nameField,1,wxEXPAND);

   text=new wxStaticText(rootPanel,wxID_ANY,_(""));
   fSizer->Add(text,0,wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL);
#endif
   /* pollingtime for incoming **************/
   if ((stdOUT!=0) && (cycleMicros>0))
   {
      text=new wxStaticText(rootPanel,wxID_ANY,_("Input Poll Cycle Time")+_T(":"));
      fSizer->Add(text,0,wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL);

      cycleTimeField=new wxSpinCtrl(rootPanel,wxID_ANY,wxEmptyString,wxDefaultPosition,wxDefaultSize,wxSP_ARROW_KEYS,25,60000,cycleMicros/1000);
      cycleTimeField->SetValidator(wxTextValidator(wxFILTER_NUMERIC));
      fSizer->Add(cycleTimeField,1,wxEXPAND);

      uiElementDefinition *elemDef=new uiElementDefinition();
      elemDef->uiElement=cycleTimeField;
      elemDef->type=TYPE_INT;
      elemDef->name=_T(OAPC_ID_CYCLETIME);
      uiElemList->Append(elemDef);

      text=new wxStaticText(rootPanel,wxID_ANY,_("msec"));
      fSizer->Add(text,0,wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL);
   }


#ifndef ENV_BEAM
   addStdElements(fSizer,rootPanel,false,true);
#endif
   if (node)
   {
      childNode=node->GetChildren();
      while (childNode)
      {
         if ((childNode->GetName().CmpNoCase(_T("param"))==0))
         {
            if (!constructUIElements(childNode,*rootPanel,*fSizer,-1,panelNum)) break;
         }
//         else wxLogDebug(_T("%s"),childNode->GetName().c_str());
         childNode=childNode->GetNext();
      }
   }
   fSizer->AddGrowableCol(1,10);
#ifdef ENV_BEAM
   if (m_extIOLib->m_beamID==GPL_BEAMID)
   {
      wxStaticText *text=new wxStaticText(rootPanel,wxID_ANY,_T("Open Source:"));
      fSizer->Add(text,1,wxEXPAND);
      text=new wxStaticText(rootPanel,wxID_ANY,_T("Free plug-in, licensed under GPL"));
      fSizer->Add(text,1,wxEXPAND);
   }
#endif
   setScrollParams(rootPanel,fSizer);
   return rootPanel;
}



wxScrolledWindow *XMLDecoder::constructUIStdPanel(wxXmlNode *node,wxString *panelName,wxWindow *parent,wxByte panelNum)
{
   wxXmlNode *childNode;
   wxInt32    cnt;

   // create the basic panel and its sizer
   wxScrolledWindow *rootPanel=new wxScrolledWindow(parent,wxID_ANY,wxDefaultPosition,wxDefaultSize,0);
   //wxPanel *rootPanel=new wxPanel(parent,wxID_ANY);
   wxFlexGridSizer *fSizer=new wxFlexGridSizer(3,4,4);
   rootPanel->SetSizer(fSizer);

   childNode=node->GetChildren();
   cnt=0;
   while (childNode)
   {
      if ((childNode->GetName().CmpNoCase(_T("param"))==0))
      {
         if (!constructUIElements(childNode,*rootPanel,*fSizer,-1,panelNum)) break;
         cnt++;
      }
      else if ((childNode->GetName().CmpNoCase(_T("name"))==0))
      {
         *panelName=g_res->getString(childNode->GetNodeContent());
      }
//      else wxLogDebug(_T("%s"),childNode->GetName().c_str());
      childNode=childNode->GetNext();
   }
   fSizer->AddGrowableCol(1,10);
#ifdef ENV_BEAM
   if (m_extIOLib->m_beamID==GPL_BEAMID)
   {
      wxStaticText *text=new wxStaticText(rootPanel,wxID_ANY,_T("Open Source:"));
      fSizer->Add(text,1,wxEXPAND);
      text=new wxStaticText(rootPanel,wxID_ANY,_T("Free plug-in, licensed under GPL"));
      fSizer->Add(text,1,wxEXPAND);
   }
#endif
   setScrollParams(rootPanel,fSizer);
   return rootPanel;
}


#ifndef ENV_BEAM
wxScrolledWindow *XMLDecoder::constructUIHelpPanel(wxXmlNode *node,wxWindow *parent)
{
   wxXmlNode *childNode;
   wxByte     i;
   wxString   name;

   wxScrolledWindow *rootPanel=new wxScrolledWindow(parent,wxID_ANY,wxDefaultPosition,wxDefaultSize,0);
//   wxPanel *rootPanel=new wxPanel(parent);
   wxFlexGridSizer *fSizer=new wxFlexGridSizer(2,4,4);
   rootPanel->SetSizer(fSizer);

   wxStaticText *text=new wxStaticText(rootPanel,wxID_ANY,_("IO"));
   wxFont f =text->GetFont();  
#if wxCHECK_VERSION(2,9,0)
   f.SetWeight(wxFONTWEIGHT_BOLD);
#else
   f.SetWeight(wxBOLD); 
#endif
   text->SetFont(f);
   fSizer->Add(text,1,wxEXPAND|wxALIGN_LEFT);

   text=new wxStaticText(rootPanel,wxID_ANY,_("Description"));
   f =text->GetFont();  
#if wxCHECK_VERSION(2,9,0)
   f.SetWeight(wxFONTWEIGHT_BOLD);
#else
   f.SetWeight(wxBOLD); 
#endif
   text->SetFont(f);
   fSizer->Add(text,1,wxEXPAND|wxALIGN_LEFT);

   childNode=node->GetChildren();
   while (childNode)
   {
      for (i=0; i<MAX_NUM_IOS; i++)
      {
         name=wxString::Format(_T("IN%d"),i);
         if ((childNode->GetName().CmpNoCase(name)==0))
         {
            fSizer->Add(new wxStaticText(rootPanel,wxID_ANY,name),1,wxEXPAND|wxALIGN_LEFT);
            fSizer->Add(new wxStaticText(rootPanel,wxID_ANY,g_res->getString(childNode->GetNodeContent())),wxEXPAND|wxALIGN_LEFT);
         }
      }
      for (i=0; i<MAX_NUM_IOS; i++)
      {
         name=wxString::Format(_T("OUT%d"),i);
         if ((childNode->GetName().CmpNoCase(name)==0))
         {
            fSizer->Add(new wxStaticText(rootPanel,wxID_ANY,name),1,wxEXPAND|wxALIGN_LEFT);
            fSizer->Add(new wxStaticText(rootPanel,wxID_ANY,g_res->getString(childNode->GetNodeContent())),wxEXPAND|wxALIGN_LEFT);
         }
      }
      childNode=childNode->GetNext();
   }
#ifdef ENV_BEAM
   if (m_extIOLib->m_beamID==GPL_BEAMID)
   {
      wxStaticText *text=new wxStaticText(rootPanel,wxID_ANY,_T("Open Source:"));
      fSizer->Add(text,1,wxEXPAND);
      text=new wxStaticText(rootPanel,wxID_ANY,_T("Free plug-in, licensed under GPL"));
      fSizer->Add(text,1,wxEXPAND);
   }
#endif
   setScrollParams(rootPanel,fSizer);
   return rootPanel;
}
#endif //ENV_BEAM


wxScrolledWindow *XMLDecoder::constructUIDualPanel(wxXmlNode *node,wxString *panelName,wxWindow *parent,wxByte panelNum)
{
   wxXmlNode *childNode;
   wxInt32    cnt;

   // create the basic panel and its sizer
   wxScrolledWindow *rootPanel=new wxScrolledWindow(parent,wxID_ANY,wxDefaultPosition,wxDefaultSize,0);
//   wxPanel *rootPanel=new wxPanel(parent,wxID_ANY);
   wxFlexGridSizer *fSizer=new wxFlexGridSizer(4,4,4);
   rootPanel->SetSizer(fSizer);

   wxStaticText *text=new wxStaticText(rootPanel,wxID_ANY,_("Digital IN 0")+_T(":"));
   wxFont f =text->GetFont();  
#if wxCHECK_VERSION(2,9,0)
   f.SetWeight(wxFONTWEIGHT_BOLD);
#else
   f.SetWeight(wxBOLD); 
#endif
   text->SetFont(f);
   fSizer->Add(text,0,0);

   text=new wxStaticText(rootPanel,wxID_ANY,_("LOW State"));
   f =text->GetFont();  
#if wxCHECK_VERSION(2,9,0)
   f.SetWeight(wxFONTWEIGHT_BOLD);
#else
   f.SetWeight(wxBOLD); 
#endif
   text->SetFont(f);
   fSizer->Add(text,1,wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL);

   text=new wxStaticText(rootPanel,wxID_ANY,_("HIGH State"));
   f =text->GetFont();  
#if wxCHECK_VERSION(2,9,0)
   f.SetWeight(wxFONTWEIGHT_BOLD);
#else
   f.SetWeight(wxBOLD);
#endif
   text->SetFont(f);
   fSizer->Add(text,1,wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL);

   fSizer->Add(new wxStaticText(rootPanel,wxID_ANY,_T("")),0,0);

   childNode=node->GetChildren();
   cnt=0;
   while (childNode)
   {
      if ((childNode->GetName().CmpNoCase(_T("param"))==0))
      {
         if (!constructUIElements(childNode,*rootPanel,*fSizer,cnt%2,panelNum)) break;
         cnt++;
      }
      else if ((childNode->GetName().CmpNoCase(_T("name"))==0))
      {
         *panelName=g_res->getString(childNode->GetNodeContent());
      }
//      else wxLogDebug(_T("%s"),childNode->GetName().c_str());
      childNode=childNode->GetNext();
   }
   fSizer->AddGrowableCol(1,5);
   fSizer->AddGrowableCol(2,5);
   setScrollParams(rootPanel,fSizer);
   return rootPanel;
}



wxScrolledWindow *XMLDecoder::constructUIParamPanel(wxXmlNode *node,wxWindow *parent,wxByte panelNum)
{
   wxXmlNode           *childNode;
   wxInt32              cnt;
   uiElementDefinition *elemDef;

   // create the basic panel and its sizer
   wxScrolledWindow *rootPanel=new wxScrolledWindow(parent,wxID_ANY,wxDefaultPosition,wxDefaultSize,0);
   //wxPanel *rootPanel=new wxPanel(parent,wxID_ANY);
   wxFlexGridSizer *fSizer=new wxFlexGridSizer(5,4,4);
   rootPanel->SetSizer(fSizer);

   childNode=node->GetChildren();
   cnt=0;
   while (childNode)
   {
      if ((childNode->GetName().CmpNoCase(_T("param"))==0))
      {
         elemDef=constructUIElements(childNode,*rootPanel,*fSizer,-1,panelNum);
         if (!elemDef) break;

         wxButton *readButton=new wxButton(rootPanel,wxID_ANY,_("Read Value"),wxDefaultPosition,wxDefaultSize,wxBU_EXACTFIT);
         readButton->Connect(wxEVT_COMMAND_BUTTON_CLICKED,wxCommandEventHandler(XMLDecoder::OnButtonClick),NULL,this);
         fSizer->Add(readButton,1,wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL);
         elemDef->idRW=readButton->GetId();

         cnt++;
      }
//      else wxLogDebug(_T("%s"),childNode->GetName().c_str());
      childNode=childNode->GetNext();
   }
   fSizer->AddGrowableCol(1,5);
   fSizer->AddGrowableCol(2,5);
   setScrollParams(rootPanel,fSizer);
   return rootPanel;
}



void XMLDecoder::cleanElementList()
{
   wxNode   *node;
   wxObject *object;

   if (!uiElemList) return;
   node=uiElemList->GetFirst();
   while (node)
   {
      object=node->GetData();
      uiElemList->DeleteNode(node);
      delete object;
      node=uiElemList->GetFirst();
   }
   delete uiElemList;
}



void XMLDecoder::updateWinSize()
{
   wxInt32 y=m_bSizer->Fit(m_parent).y;
   wxInt32 x=m_bSizer->Fit(m_parent).x;

   if (y<200) y=200;
   if (x<260) x=260;
   m_parent->SetSize(x,y);
   m_parent->Center();
}



void XMLDecoder::constructUI(wxDialog *parent,wxString *objectName,bool *returnOK,wxUint64 stdOUT,wxUint32 *cycleMicros,bool hideISConfig)
{
   wxScrolledWindow *panel;
   wxString          panelName;
   wxInt32           i;
   bool              morePanelsAvailable; 

#ifndef ENV_BEAM
   m_hideISConfig=hideISConfig;
#else
   hideISConfig=hideISConfig;
#endif
   cleanElementList();
   uiElemList=new wxList();
   m_objectName=objectName;
   m_cycleMicros=cycleMicros;
   m_returnOK=returnOK;
   m_parent=parent;
   // create and add the main book
   m_bSizer=new wxBoxSizer(wxVERTICAL);
   parent->SetSizer(m_bSizer);
   m_book=new wxNotebook(parent,wxID_ANY);
   m_bSizer->Add(m_book,1,wxEXPAND);

   // create the standard dialogue buttons
   g_createDialogButtons(parent,m_bSizer,wxOK|wxCANCEL);
   i=0;
   do
   {
      panel=NULL;
      panel=getConfigPanel(&morePanelsAvailable,&panelName,m_book,i,stdOUT,cycleMicros);
      if (panel)
      {
         panel->SetLabel(panelName);
         m_book->AddPage(panel,panelName,i==0);
         panel->Layout();
         panel->AdjustScrollbars();
      }
      if (morePanelsAvailable) i++;
   }
   while (morePanelsAvailable);
#ifndef ENV_BEAM
   if (!m_nameField)
   {
      if (cycleMicros) panel=constructUIBasicPanel(NULL,0,*cycleMicros,&panelName,0);
      else panel=constructUIBasicPanel(NULL,0,0,&panelName,0);
      panel->SetLabel(panelName);
      m_book->InsertPage(0,panel,panelName,false);
      panel->Layout();
   }
#endif
   if (uiElemList)
   {
      wxNode              *elemNode;
      uiElementDefinition *elemDef;

      elemNode=uiElemList->GetFirst();
      while (elemNode)
      {
         elemDef=(uiElementDefinition*)elemNode->GetData();
         checkEnableDependencies(elemDef);
         checkDisableDependencies(elemDef);
         elemNode=elemNode->GetNext();
      }
   }

   updateWinSize();
}



wxScrolledWindow *XMLDecoder::getConfigPanel(bool *morePanelsAvailable,wxString *name,wxWindow *parent,wxInt32 num)
{
   if (num==0)
   {
      cleanElementList();
      uiElemList=new wxList();
   }
   return getConfigPanel(morePanelsAvailable,name,parent,num,0,NULL);
}



wxScrolledWindow *XMLDecoder::getConfigPanel(bool *morePanelsAvailable,wxString *name,wxWindow *parent,wxInt32 num,wxUint64 stdOUT,wxUint32 *cycleMicros)
{
   wxXmlNode        *childNode;
   wxByte            panelNum;
   wxScrolledWindow *panel;

   *morePanelsAvailable=false;
   if (!m_dialogueXMLNode) return NULL;
   childNode=m_dialogueXMLNode->GetChildren();
   panelNum=num+1;
   while (childNode)
   {
      if (!childNode) return NULL;
      panel=NULL;
      if (num==0) 
      {
         wxString nodeName;

#if wxCHECK_VERSION(3,0,0)
         childNode->GetAttribute(wxT("text"), name); // text to be shown with an element
#else
         childNode->GetPropVal(wxT("text"),name); // text to be shown with an element
#endif
         *name=g_res->getString(*name);
         nodeName=childNode->GetName();
         if ((nodeName.CmpNoCase(_T("general"))==0))
          panel=constructUIBasicPanel(childNode,stdOUT,*cycleMicros,name,panelNum);
         else if (
                  ((childNode->GetName().CmpNoCase(_T("stdpanel"))==0))
#ifndef ENV_BEAM
                  || ((childNode->GetName().CmpNoCase(_T("oapcstdpanel"))==0))
#endif
                  )
          panel=constructUIStdPanel(childNode,name,parent,panelNum);
         else if ((childNode->GetName().CmpNoCase(_T("dualpanel"))==0)) panel=constructUIDualPanel(childNode,name,parent,panelNum);
         else if ((childNode->GetName().CmpNoCase(_T("parampanel"))==0)) panel=constructUIParamPanel(childNode,parent,panelNum);
#ifndef ENV_BEAM
         else if (
                  ((childNode->GetName().CmpNoCase(_T("helppanel"))==0))
                  || ((childNode->GetName().CmpNoCase(_T("oapchelppanel"))==0))
                 )
         {
            panel=constructUIHelpPanel(childNode,parent);
            *name=_("Description");
         }
#endif
         *morePanelsAvailable=true;
         return panel;
      }
      num--;
      childNode=childNode->GetNext();
   }
   return NULL;
}



void XMLDecoder::OnInputChanged(wxCommandEvent &event)
{
   wxNode *node =uiElemList->GetFirst();
   while (node)
   {
      uiElementDefinition *current =(uiElementDefinition*)node->GetData();
      if (current->id==event.GetId())
      {
          if (current->type==TYPE_FLOAT)
          {
             oapcFloatCtrl *floatField;

             floatField=(oapcFloatCtrl*)current->uiElement;
             floatField->SetValue(floatField->GetValue(current->min,current->max));
         }
      }
      else if (current->id2==event.GetId())
      {
          if (current->type2==TYPE_FLOAT)
          {
             oapcFloatCtrl *floatField;

             floatField=(oapcFloatCtrl*)current->uiElement2;
             floatField->SetValue(floatField->GetValue(current->min,current->max));
         }
      }
      node = node->GetNext();
   }
}



wxByte XMLDecoder::getNextPair(wxString *name,wxString *value,wxFont *font)
{
   static wxNode* node=NULL;
   
   if (!node) node=uiElemList->GetFirst();
   else node = node->GetNext();
   if (!node) return 0;
   
   if (font) font->SetPointSize(0);
   uiElementDefinition *current =(uiElementDefinition*)node->GetData();
   *name=current->name;
   if (current->type==TYPE_FLOAT)
   {
      wxFloat32 f;
      char      testC[100+4];

      value->Printf(_T("%f"),current->floatValue);
      oapc_unicode_stringToCharASCII(*value,testC,100);
      f=atof(testC);
      if (f!=current->floatValue)
      {
         value->Replace(_T(","),_T("."));
         value->Printf(_T("%f"),current->floatValue);
         oapc_unicode_stringToCharASCII(*value,testC,100);
         f=atof(testC);
         if (f!=current->floatValue)
         {
            value->Replace(_T("."),_T(","));
         }
      }
   }
   else if ((current->type==TYPE_INT) || (current->type==TYPE_COLORBUTTON) || (current->type==TYPE_OPTION)) value->Printf(_T("%d"),current->intValue);
   else if ((current->type==TYPE_CHECKBOX))
   {
      if (((wxCheckBox*)current->uiElement)->GetValue()) *value=_T("1");
      else *value=_T("0");
   }
   else if (current->type==TYPE_FONTBUTTON)
   {
      if (font) *font=*current->font;
   }
   else *value=current->stringValue;
   return 1;
}


/**
 * Builds a description of a UI element consisting of its panel name and the
 * text of the element itself
 */
wxString XMLDecoder::getUIDescription(uiElementDefinition *def)
{
   wxString txt;

   txt=_T(" \"");
   if ((def->uiElement))
   {
      txt=txt+((wxPanel*)def->uiElement->GetParent())->GetLabel();
      txt=txt+_T(", ");
   }
   txt=txt+g_res->getString(def->text)+_T("\":\n");
   return txt;
}


bool XMLDecoder::getValuesFromUI()
{
   wxNode *node =uiElemList->GetFirst();
   while (node)
   {
      uiElementDefinition *current =(uiElementDefinition*)node->GetData();
      if ((current->type==TYPE_STRING) || (current->type==TYPE_FILELOAD) || (current->type==TYPE_FILESAVE) || 
          (current->type==TYPE_DIRSELECT))
      {
         wxTextCtrl *stringField;
         wxString    s;

         stringField=(wxTextCtrl*)current->uiElement;
         s=stringField->GetValue();
         if (s.Length()<current->min)
         {
            wxMessageBox(_("Illegal data entered for")+getUIDescription(current)+_("Text is too short!"),_("Error"),wxICON_ERROR);
            return false;
         }
         current->stringValue=stringField->GetValue();
      }
      else if (current->type==TYPE_FLOAT)
      {
         wxFloat32       d;
         oapcFloatCtrl *floatField;

         floatField=(oapcFloatCtrl*)current->uiElement;
         d=floatField->GetValueUnlimited();
         if ((d<current->min) || (d>current->max))
         {
            wxString s;

            s.Printf(_T("%f .. %f!"),current->min,current->max);
            wxMessageBox(_("Illegal data entered for")+getUIDescription(current)+_("Number has to be within range")+_T(" ")+s,_("Error"),wxICON_ERROR);
            return false;
         }
         current->floatValue=d;
      }
      else if (current->type2==TYPE_FLOAT)
      {
         wxFloat32      d;
         oapcFloatCtrl *floatField;

         floatField=(oapcFloatCtrl*)current->uiElement2;
         d=floatField->GetValueUnlimited();
         if ((d<current->min) || (d>current->max))
         {
            wxString s;

            s.Printf(_T("%f .. %f!"),current->min2,current->max2);
            wxMessageBox(_("Illegal data entered for")+getUIDescription(current)+_("Number has to be within range")+_T(" ")+s,_("Error"),wxICON_ERROR);
            return false;
         }
         current->floatValue2=d;
      }
      else if (current->type==TYPE_INT)
      {
         wxSpinCtrl *intField;

         intField=(wxSpinCtrl*)current->uiElement;
         current->intValue=intField->GetValue();
      }
      else if (current->type2==TYPE_INT)
      {
         wxSpinCtrl *intField;

         intField=(wxSpinCtrl*)current->uiElement2;
         current->intValue2=intField->GetValue();
      }
      else if (current->type==TYPE_COLORBUTTON)
      {
         wxButton *colButton;
         wxColour  col;

         colButton=(wxButton*)current->uiElement;
         col=colButton->GetBackgroundColour();
         current->intValue=col.Red()|col.Green()<<8|col.Blue()<<16;
      }
      else if (current->type==TYPE_FONTBUTTON)
      {
         wxButton *fontButton;

         fontButton=(wxButton*)current->uiElement;
         *current->font=fontButton->GetFont();
      }
      else if (current->type==TYPE_OPTION)
      {
         wxComboBox *comboBox;

         comboBox=(wxComboBox*)current->uiElement;
         current->intValue=comboBox->GetSelection()+1;
         
      }
      node = node->GetNext();
   }
   return true;
}


void XMLDecoder::checkEnableDependencies(uiElementDefinition *elemDef)
{
   wxUint32 m,s;

   for (s=0; s<elemDef->slaveEnableList.size(); s++)
   {
      uiElementDefinition *slaveElemDef;
      bool                 enableSlave=true;

      slaveElemDef=elemDef->slaveEnableList[s];
      for (m=0; m<slaveElemDef->masterEnableList.size(); m++)
      {
         if ((slaveElemDef->masterEnableList[m]->type==TYPE_CHECKBOX) &&
             (!((wxCheckBox*)slaveElemDef->masterEnableList[m]->uiElement)->IsChecked())) enableSlave=false;
      }
      if (slaveElemDef->uiElement)  slaveElemDef->uiElement->Enable(enableSlave);
      if (slaveElemDef->uiElement2) slaveElemDef->uiElement2->Enable(enableSlave);
      if (slaveElemDef->uiText)     slaveElemDef->uiText->Enable(enableSlave);
      if (slaveElemDef->uiUnit)     slaveElemDef->uiUnit->Enable(enableSlave);
   }
}


void XMLDecoder::checkDisableDependencies(uiElementDefinition *elemDef)
{
   wxUint32 m,s;

   for (s=0; s<elemDef->slaveDisableList.size(); s++)
   {
      uiElementDefinition *slaveElemDef;
      bool                 disableSlave=true;

      slaveElemDef=elemDef->slaveDisableList[s];
      for (m=0; m<slaveElemDef->masterDisableList.size(); m++)
      {
         if ((slaveElemDef->masterDisableList[m]->type==TYPE_CHECKBOX) &&
             (!((wxCheckBox*)slaveElemDef->masterDisableList[m]->uiElement)->IsChecked())) disableSlave=false;
      }
      if (slaveElemDef->uiElement)  slaveElemDef->uiElement->Enable(!disableSlave);
      if (slaveElemDef->uiElement2) slaveElemDef->uiElement2->Enable(!disableSlave);
      if (slaveElemDef->uiText)     slaveElemDef->uiText->Enable(!disableSlave);
      if (slaveElemDef->uiUnit)     slaveElemDef->uiUnit->Enable(!disableSlave);
   }
}


void XMLDecoder::OnCheckBoxCheck(wxCommandEvent &event)
{
   wxNode              *node;
   uiElementDefinition *elemDef;

   if (!uiElemList) return;
   node=uiElemList->GetFirst();
   while (node)
   {
      elemDef=(uiElementDefinition*)node->GetData();
      if (elemDef->id==event.GetId())
      {
         checkEnableDependencies(elemDef);
         checkDisableDependencies(elemDef);
         break;
      }
      node=node->GetNext();
   }
}



void XMLDecoder::sendPanelParams()
{
   if (!m_extIOLib) return;
   if ((m_extIOLib->getLibCapabilities() & OAPC_ACCEPTS_PLAIN_CONFIGURATION) &&
       ((m_extIOLib->oapc_set_config_data) || (m_extIOLib->oapc_set_config_data_const)))
   {
      wxString name,value;
      wxFont   font;

      while (getNextPair(&name,&value,&font))
      {
         char c1[50+4],c2[2000+4];

         if (font.GetPointSize()>0)
         {
            oapc_unicode_stringToCharASCII(name+_T("_pointsize"),c1,50);
            value.Printf(_T("%d"),font.GetPointSize());
            oapc_unicode_stringToCharASCII(value,c2,2000);
            if (m_extIOLib->oapc_set_config_data_const) m_extIOLib->oapc_set_config_data_const(m_instData,c1,c2);
            else m_extIOLib->oapc_set_config_data(m_instData,c1,c2);

            oapc_unicode_stringToCharASCII(name+_T("_style"),c1,50);
            value.Printf(_T("%d"),font.GetStyle());
            oapc_unicode_stringToCharASCII(value,c2,2000);
            if (m_extIOLib->oapc_set_config_data_const) m_extIOLib->oapc_set_config_data_const(m_instData,c1,c2);
            else m_extIOLib->oapc_set_config_data(m_instData,c1,c2);

            oapc_unicode_stringToCharASCII(name+_T("_weight"),c1,50);
            value.Printf(_T("%d"),font.GetWeight());
            oapc_unicode_stringToCharASCII(value,c2,2000);
            if (m_extIOLib->oapc_set_config_data_const) m_extIOLib->oapc_set_config_data_const(m_instData,c1,c2);
            else m_extIOLib->oapc_set_config_data(m_instData,c1,c2);

            oapc_unicode_stringToCharASCII(name+_T("_facename"),c1,50);
            oapc_unicode_stringToCharASCII(value,c2,2000);
            if (m_extIOLib->oapc_set_config_data_const) m_extIOLib->oapc_set_config_data_const(m_instData,c1,c2);
            else m_extIOLib->oapc_set_config_data(m_instData,c1,c2);
         }
         else
         {
            oapc_unicode_stringToCharASCII(name,c1,50);
            oapc_unicode_stringToCharASCII(value,c2,2000);
            if (m_extIOLib->oapc_set_config_data_const) m_extIOLib->oapc_set_config_data_const(m_instData,c1,c2);
            else m_extIOLib->oapc_set_config_data(m_instData,c1,c2);
         }
      }
   }
}



void XMLDecoder::OnButtonClick(wxCommandEvent &event)
{
   static bool evtLocked=false;

   if (evtLocked) return;
   evtLocked=true;
   if (event.GetId()==wxID_OK)
   {
      if (!getValuesFromUI()) 
      {
         evtLocked=false;
         return;
      }
      if (cycleTimeField) *m_cycleMicros=cycleTimeField->GetValue()*1000;
#ifndef ENV_BEAM
      *m_objectName=m_nameField->GetValue();
      
      wxASSERT(m_object);
      if (m_object)
      {
         if (mapOutputCB)
         {
            if (mapOutputCB->GetValue()) m_object->m_flowFlags|=FLAG_MAP_OUTPUT_TO_ISERVER;
            else m_object->m_flowFlags&=~(FLAG_MAP_OUTPUT_TO_ISERVER);
         }
         if (ioModCB)
         {
            if (ioModCB->GetValue()) m_object->m_flowFlags|=FLAG_ALLOW_MOD_FROM_ISERVER;
            else m_object->m_flowFlags&=~(FLAG_ALLOW_MOD_FROM_ISERVER);
         }
      }
#endif
      g_isSaved=false;
      *m_returnOK=1;
      m_parent->EndModal(0);
      sendPanelParams();
   }
   else if (event.GetId()==wxID_CANCEL)
   {
      *m_returnOK=0;
      m_parent->EndModal(0);
   }
   else
   {
      wxColourData colour;
      wxFontData   font;
      wxButton    *colButton;
      wxNode *node =uiElemList->GetFirst();

      while (node)
      {
         uiElementDefinition *current =(uiElementDefinition*)node->GetData();
         if (current->id==event.GetId())
         {
            if (current->type==TYPE_COLORBUTTON)
            {
               colButton=(wxButton*)current->uiElement;
               colour.SetColour(colButton->GetBackgroundColour());
               g_colourDialog->Create(NULL,&colour);
               g_colourDialog->Centre();
               g_colourDialog->SetTitle(_("Choose the background colour"));
               if (g_colourDialog->ShowModal() == wxID_OK)
               {
                  colour=g_colourDialog->GetColourData();
                  colButton->SetBackgroundColour(colour.GetColour());
               }
            }
            else if (current->type==TYPE_FONTBUTTON)
            {
               wxFontData   font;

               colButton=(wxButton*)current->uiElement;
               font.SetInitialFont(colButton->GetFont());
               font.EnableEffects(false);

               wxFontDialog *dialog = new wxFontDialog(m_dialogueParent,font);
               if (dialog->ShowModal() == wxID_OK)
               {
                  font=dialog->GetFontData();          
                  colButton->SetFont(font.GetChosenFont());          
               }
               updateWinSize();
               dialog->Destroy();
            }
            else if ((current->type==TYPE_FILELOAD) || (current->type==TYPE_FILESAVE))
            {
               wxFileDialog* fileDialog;

               if (current->unit.Length()<=0) current->unit=_T("*.*");
               if (current->type==TYPE_FILELOAD)
                fileDialog=new wxFileDialog(NULL,_("Load file"),wxEmptyString,((wxTextCtrl*)current->uiElement)->GetValue(),current->unit,wxFD_OPEN|wxFD_FILE_MUST_EXIST|wxFD_PREVIEW, wxDefaultPosition);
               else
                fileDialog=new wxFileDialog(NULL,_("Save file"),wxEmptyString,((wxTextCtrl*)current->uiElement)->GetValue(),current->unit,wxFD_SAVE|wxFD_OVERWRITE_PROMPT, wxDefaultPosition);
               fileDialog->Centre();
               if (fileDialog->ShowModal()==wxID_OK)
                ((wxTextCtrl*)current->uiElement)->SetValue(fileDialog->GetPath());
               delete fileDialog;
            }
            else if (current->type==TYPE_DIRSELECT)
            {
               wxDirDialog *dirDialog;
               
               dirDialog=new wxDirDialog(NULL,_("Select directory"),((wxTextCtrl*)current->uiElement)->GetValue());
               dirDialog->Centre();
               if (dirDialog->ShowModal()==wxID_OK)
                ((wxTextCtrl*)current->uiElement)->SetValue(dirDialog->GetPath());
               delete dirDialog;
            }
         }
         else if (current->idRW==event.GetId())
         {
            int   ret;

            getValuesFromUI();
            sendPanelParams();
            ret=m_extIOLib->oapc_init(m_instData);
            if ((ret==OAPC_OK) && (current->type2!=TYPE_UNDEFINED) && (m_extIOLib->oapc_read_pvalue))
            {
               double param=0,value=0;

               if (current->type==TYPE_INT) param=current->intValue;
               else if (current->type==TYPE_FLOAT) param=current->floatValue;
               if (current->type2==TYPE_INT) value=current->intValue2;
               else if (current->type2==TYPE_FLOAT) value=current->floatValue2;
               ret=m_extIOLib->oapc_read_pvalue(m_instData,param,&value);
               if (ret==OAPC_OK)
               {
                  if (current->type2==TYPE_INT)
                  {
                     wxSpinCtrl *intField;

                     intField=(wxSpinCtrl*)current->uiElement2;
                     intField->SetValue(value);
                  }
                  else if (current->type2==TYPE_FLOAT)
                  {
                     oapcFloatCtrl *floatField;

                     floatField=(oapcFloatCtrl*)current->uiElement2;
                     floatField->SetValue(value);
                  }
               }
               else g_OAPCMessageBox(ret,m_extIOLib,m_instData,this,wxEmptyString);
            }
            else g_OAPCMessageBox(ret,m_extIOLib,m_instData,this,wxEmptyString);
            m_extIOLib->oapc_exit(m_instData);
         }
         node = node->GetNext();
      }
   }
   evtLocked=false;
}


