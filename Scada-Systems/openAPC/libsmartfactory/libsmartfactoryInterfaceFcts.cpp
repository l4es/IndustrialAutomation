/********************************************************************************************

This program and source file is free software: you can redistribute it and/or modify it under
the terms of the GNU General Public License as published by the Free Software Foundation,
either version 3 of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with this program.
If not, see <http://www.gnu.org/licenses/>.

*********************************************************************************************/

#ifndef ENV_USE_PUGIXML
 #include <wx/wx.h>
 #include <wx/sstream.h>
 #include <wx/mstream.h>
 #include <wx/string.h>
 #include <wx/xml/xml.h>
#else
 #include "pugiconfig.hpp"
 #include "pugixml.hpp"
 #include <sstream>
#endif

#ifdef ENV_WINDOWS
 #define snprintf _snprintf
#endif

#include <queue>
#include <list>
#include <assert.h>

#include "liboapc.h"
#include "libsmartfactory.h"

#ifndef ENV_USE_PUGIXML
extern char *convertMessage(const wxXmlDocument *xmlDoc);
#else
extern char *convertMessage(const pugi::xml_document *xmlDoc);
#endif

struct if_inst_data
{
   int                                        m_sock;
   void                                      *m_parameterMutex,*m_clientSocksMutex;
   bool                                       m_acceptLoopRunning,m_timerLoopRunning;
   enum SmartState                            m_state,m_prevState;
   int                                        m_lastProcessValue[4];
   time_t                                     m_lastTransmission;
   wxString                                  *m_machineID;
   wxString                                  *m_traceDataBoard,*m_traceDataProduct,*m_projectName,*m_errorText;
   std::queue<std::pair<wxString,wxString> > *m_traceData;
   std::list<int>                            *m_clientSocks;
};


struct if_send_data
{
   struct if_inst_data *instData;
   char                *message;
};


SF_EXT_API int sf_if_wx_set_machine_identifier(void* instance,const wxString *machineIdentifier)
{
   struct if_inst_data *instData;

   if (!instance) return OAPC_ERROR_INVALID_INPUT;
   instData=(struct if_inst_data*)instance;

   oapc_thread_mutex_lock(instData->m_parameterMutex);
   *instData->m_machineID=*machineIdentifier;
   oapc_thread_mutex_unlock(instData->m_parameterMutex);
   return OAPC_OK;
}



SF_EXT_API void *sf_if_create_instance(const char *machineIdentifier)
{
#ifndef ENV_USE_PUGIXML
   wxString mid;

   oapc_unicode_charToStringASCII(machineIdentifier,strlen(machineIdentifier),&mid);
#else
   std::string mid(machineIdentifier);
#endif
   return sf_if_wx_create_instance(&mid);
}


static void flushTraceList(struct if_inst_data *instData)
{
   while (instData->m_traceData->size()>0)
    instData->m_traceData->pop();
   *instData->m_traceDataBoard=wxEmptyString;
   *instData->m_traceDataProduct=wxEmptyString;
}


SF_EXT_API void *sf_if_wx_create_instance(const wxString *machineIdentifier)
{
   struct if_inst_data *instData;

   instData=(struct if_inst_data*)malloc(sizeof(struct if_inst_data));
   if (!instData) return NULL;
   memset(instData,0,sizeof(struct if_inst_data));

   instData->m_clientSocksMutex=NULL;
   instData->m_parameterMutex=oapc_thread_mutex_create();
   if (!instData->m_parameterMutex)
   {
      sf_if_delete_instance(instData);
      return NULL;
   }

   instData->m_traceDataBoard=new wxString();
   if (!instData->m_traceDataBoard)
   {
      sf_if_delete_instance(instData);
      return NULL;
   }
   
   instData->m_traceDataProduct=new wxString();
   if (!instData->m_traceDataProduct)
   {
      sf_if_delete_instance(instData);
      return NULL;
   }

   instData->m_projectName=new wxString();
   if (!instData->m_projectName)
   {
      sf_if_delete_instance(instData);
      return NULL;
   }

   instData->m_errorText=new wxString();
   if (!instData->m_errorText)
   {
      sf_if_delete_instance(instData);
      return NULL;
   }

   instData->m_machineID=new wxString();
   if (!instData->m_machineID)
   {
      sf_if_delete_instance(instData);
      return NULL;
   }

   instData->m_traceData=new std::queue<std::pair<wxString, wxString>>();
   if (!instData->m_traceData)
   {
      sf_if_delete_instance(instData);
      return NULL;
   }

   instData->m_clientSocks=new std::list<int>();
   if (!instData->m_clientSocks)
   {
      sf_if_delete_instance(instData);
      return NULL;
   }

   instData->m_acceptLoopRunning=false;
   instData->m_state=eIDLE;
   instData->m_prevState=eUNKNOWN;
   flushTraceList(instData);
   instData->m_lastProcessValue[0]=0xFFFFFFFF;
   instData->m_lastProcessValue[1]=0xFFFFFFFF;
   instData->m_lastProcessValue[2]=0xFFFFFFFF;
   instData->m_lastProcessValue[3]=0xFFFFFFFF;
   sf_if_wx_set_machine_identifier(instData,machineIdentifier);
   return instData;
}


#ifndef ENV_USE_PUGIXML
static wxXmlNode *createIfRootNode(struct if_inst_data *instData, wxXmlDocument &xmlDoc)
{
    wxDateTime time=wxDateTime::Now();
    wxXmlNode *root=new wxXmlNode(NULL, wxXML_ELEMENT_NODE, _T("BCMsg"));

    oapc_thread_mutex_lock(instData->m_parameterMutex);
#if wxCHECK_VERSION(3,0,0)
    root->AddAttribute(new wxXmlAttribute(_T("Time"), time.Format(_T("%Y-%m-%dT%H:%M:%S")))); // ISO 8601 time format, local time
    root->AddAttribute(new wxXmlAttribute(_T("ID"),*instData->m_machineID));
#else
    root->AddProperty(new wxXmlProperty(_T("Time"), time.Format(_T("%Y-%m-%dT%H:%M:%S")), NULL)); // ISO 8601 time format, local time
    root->AddProperty(new wxXmlProperty(_T("ID"),*instData->m_machineID, NULL));
#endif
    oapc_thread_mutex_unlock(instData->m_parameterMutex);
    xmlDoc.SetRoot(root);
    return root;
}
#else //ENV_USE_PUGIXML
static pugi::xml_node createIfRootNode(struct if_inst_data *instData, pugi::xml_document &xmlDoc)
{
   time_t                currTime=time(NULL);
   char                  fmtTime[200+1];
   struct tm            *timeinfo;
   pugi::xml_node        declarationNode=xmlDoc.append_child(pugi::node_declaration);
   pugi::xml_node        root=xmlDoc.append_child("BCMsg");

   declarationNode.append_attribute("version")="1.0";
   declarationNode.append_attribute("encoding")="UTF-8";
   declarationNode.append_attribute("standalone")="yes";

   timeinfo=localtime(&currTime);
   strftime(fmtTime,200,"%Y-%m-%dT%H:%M:%S",timeinfo);

   oapc_thread_mutex_lock(instData->m_parameterMutex);
   root.append_attribute("Time")=fmtTime;
   root.append_attribute("ID")=instData->m_machineID->c_str();
   oapc_thread_mutex_unlock(instData->m_parameterMutex);

   return root;
}
#endif //ENV_USE_PUGIXML


static char *getStateMessage(struct if_inst_data *instData)
{
   wxString        stateText;
   wxXmlDocument   xmlDoc;
#ifndef ENV_USE_PUGIXML
   wxXmlNode      *root=createIfRootNode(instData,xmlDoc);

   wxXmlNode* dataNode=new wxXmlNode(root,wxXML_ELEMENT_NODE,_T("Data"));
#else
   wxXmlNode       root=createIfRootNode(instData,xmlDoc);

   wxXmlNode dataNode=root.append_child("Data");
#endif
   switch (instData->m_state)
   {
      case eIDLE:
         stateText=_T("idle");
         break;
      case eREADY:
         stateText=_T("ready");
         break;
      case eMARKING:
         stateText=_T("marking");
         break;
      case eERROR:
         stateText=_T("error");
         break;
      case ePAUSED:
         stateText=_T("paused");
         break;
      default:
         assert(0);
         stateText=_T("unknown");
         break;
   }
#ifndef ENV_USE_PUGIXML
   dataNode->AddAttribute(_T("State"),stateText);
   if (instData->m_state==eERROR) dataNode->AddAttribute(_T("Text"),*instData->m_errorText);
#else
   dataNode.append_attribute("State")=stateText.c_str();
   dataNode.append_attribute("Text")= instData->m_errorText->c_str();
#endif
   return convertMessage(&xmlDoc);
}


static char *getProjectNameMessage(struct if_inst_data *instData)
{
   wxXmlDocument xmlDoc;
   wxString      stateText;
#ifndef ENV_USE_PUGIXML
   wxXmlNode    *root=createIfRootNode(instData,xmlDoc);

   wxXmlNode* dataNode=new wxXmlNode(root,wxXML_ELEMENT_NODE,_T("Data"));
   dataNode->AddAttribute(_T("Project"),*instData->m_projectName);
#else
   wxXmlNode     root=createIfRootNode(instData,xmlDoc);

   wxXmlNode dataNode=root.append_child("Data");
   dataNode.append_attribute("Project")= instData->m_projectName->c_str();
#endif
   return convertMessage(&xmlDoc);
}


static void *acceptLoop(void *instance)
{
   int           c;
   unsigned long ip;
   struct if_inst_data *instData;

   if (!instance) return NULL;
   instData=(struct if_inst_data*)instance;

   while (instData->m_acceptLoopRunning)
   {
      oapc_thread_sleep(0);

      c=oapc_tcp_accept_connection(instData->m_sock,&ip);
      if (c>0)
      {
         char *message;

         oapc_tcp_set_blocking(c,false);
         oapc_thread_mutex_lock(instData->m_clientSocksMutex);
         
         message=getStateMessage(instData);
         if (oapc_tcp_send(c,message,strlen(message),500) > 0)
         {
            if (instData->m_projectName->length()>0)
            {
               message=getProjectNameMessage(instData);
               if (oapc_tcp_send(c, message, strlen(message),500) > 0)
                instData->m_clientSocks->push_back(c);
               else
                oapc_tcp_closesocket(c);
            }
            else
             instData->m_clientSocks->push_back(c);
         }
         else
          oapc_tcp_closesocket(c);

         oapc_thread_mutex_unlock(instData->m_clientSocksMutex);
         c=-1;
      }
   }
   instData->m_acceptLoopRunning=true;
   return NULL;
}


static void *sendLoop(void *data)
{
   int                      len;
   struct if_send_data     *sendData;
   std::list<int>::iterator it;

   sendData=(struct if_send_data*)data;
   if (!sendData) return NULL;
   len=strlen(sendData->message);

   oapc_thread_mutex_lock(sendData->instData->m_clientSocksMutex);
   for (it=sendData->instData->m_clientSocks->begin(); it!=sendData->instData->m_clientSocks->end(); it++)
   {
      if (*it>0)
      {
         if (oapc_tcp_send(*it,sendData->message,len,500)<=0)
         {
            oapc_tcp_closesocket(*it);
            *it=0;
         }
      }
   }
   oapc_thread_mutex_unlock(sendData->instData->m_clientSocksMutex);
   free(sendData->message);
   sendData->instData->m_lastTransmission=time(NULL);
   free(sendData);
   return NULL;
}



static void *timerLoop(void *instance)
{
   struct if_inst_data *instData;

   if (!instance) return NULL;
   instData=(struct if_inst_data*)instance;

   while (instData->m_timerLoopRunning)
   {
      oapc_thread_sleep(1000);
      time_t currentTime=time(NULL);

      if (currentTime- instData->m_lastTransmission>60)
      {
         wxXmlDocument        xmlDoc;
         struct if_send_data *sendData;

         createIfRootNode(instData,xmlDoc);
/*         wxDateTime time=wxDateTime::Now();
         wxXmlNode* root=new wxXmlNode(NULL,wxXML_ELEMENT_NODE,_T("BCMsg"));
         xmlDoc.SetRoot(root);*/
         sendData=(struct if_send_data*)malloc(sizeof(struct if_send_data));
         if (!sendData)
         {
            assert(0);
         }
         else
         {
            sendData->message=convertMessage(&xmlDoc);
            sendData->instData=instData;
            oapc_thread_create(sendLoop,sendData);
         }
      }
   }
   instData->m_timerLoopRunning=true;
   return NULL;
}



SF_EXT_API int sf_if_open_connections(void* instance)
{
   struct if_inst_data *instData;

   if (!instance) return OAPC_ERROR_INVALID_INPUT;
   instData=(struct if_inst_data*)instance;

   instData->m_lastTransmission=time(NULL);
   instData->m_clientSocksMutex=oapc_thread_mutex_create();
   if (!instData->m_clientSocksMutex) return OAPC_ERROR_NO_MEMORY;
   instData->m_sock=oapc_tcp_listen_on_port(11355,"0.0.0.0");
   if (instData->m_sock<=0) return OAPC_ERROR_RESOURCE;
   oapc_tcp_set_blocking(instData->m_sock,true);
   instData->m_acceptLoopRunning=true;
   if (!oapc_thread_create(acceptLoop,instance))
    return OAPC_ERROR_NO_MEMORY;
   instData->m_timerLoopRunning=true;
   if (!oapc_thread_create(timerLoop,instance))
    return OAPC_ERROR_NO_MEMORY;
   return OAPC_OK;

}


SF_EXT_API int sf_if_delete_instance(void* instance)
{
   int                  i;
   struct if_inst_data *instData;

   if (!instance) return OAPC_ERROR_INVALID_INPUT;
   instData=(struct if_inst_data*)instance;

   if (instData->m_acceptLoopRunning)
   {
      instData->m_timerLoopRunning=false;
      instData->m_acceptLoopRunning=false;
      oapc_tcp_closesocket(instData->m_sock);
      for (i=0; i<20; i++)
      {
         if ((instData->m_acceptLoopRunning) && (instData->m_timerLoopRunning)) break;
         oapc_thread_sleep(100);
      }
      std::list<int>::iterator it;

      for (it=instData->m_clientSocks->begin(); it!=instData->m_clientSocks->end(); it++)
      {
         oapc_tcp_closesocket(*it);
      }
   }

   if (instData->m_clientSocksMutex) oapc_thread_mutex_release(instData->m_clientSocksMutex);
   if (instData->m_parameterMutex)   oapc_thread_mutex_release(instData->m_parameterMutex);
   if (instData->m_traceDataBoard)   delete instData->m_traceDataBoard;
   if (instData->m_traceDataProduct) delete instData->m_traceDataProduct;
   if (instData->m_projectName)      delete instData->m_projectName;
   if (instData->m_errorText)        delete instData->m_errorText;
   if (instData->m_machineID)        delete instData->m_machineID;
   if (instData->m_traceData)        delete instData->m_traceData;
   if (instData->m_clientSocks)      delete instData->m_clientSocks;
   return OAPC_OK;
}


static void cleanupSocketList(struct if_inst_data *instData)
{
   std::list<int>::iterator it;

   oapc_thread_mutex_lock(instData->m_clientSocksMutex);
   if (instData->m_clientSocks->size()>0)
    for (it=instData->m_clientSocks->begin(); it!=instData->m_clientSocks->end(); it++)
   {
      if (*it==0)
      {
         instData->m_clientSocks->erase(it);
         it=instData->m_clientSocks->begin();
      }
   }
   oapc_thread_mutex_unlock(instData->m_clientSocksMutex);
}


static bool hasClients(struct if_inst_data *instData)
{
   oapc_thread_mutex_lock(instData->m_clientSocksMutex);
   if (instData->m_clientSocks->empty())
   {
      oapc_thread_mutex_unlock(instData->m_clientSocksMutex);
      return false;
   }
   oapc_thread_mutex_unlock(instData->m_clientSocksMutex);
   return true;
}


int sf_if_set_state(void* instance, const enum SmartState state)
{
   struct if_inst_data *instData;
   struct if_send_data *sendData;

   if (!instance) return OAPC_ERROR_INVALID_INPUT;
   instData=(struct if_inst_data*)instance;
   if (instData->m_sock==0) return OAPC_ERROR_RESOURCE;

   if (instData->m_state==eERROR)
   {
      instData->m_prevState=state;
      return OAPC_OK;
   }
   if (state==instData->m_state) return OAPC_OK;

   instData->m_state=state;

   if (!hasClients(instData)) return OAPC_OK;

   sendData=(struct if_send_data*)malloc(sizeof(struct if_send_data));
   if (!sendData) return OAPC_ERROR_NO_MEMORY;

   flushTraceList(instData);

   sendData->message=getStateMessage(instData);
   sendData->instData=instData;
   cleanupSocketList(instData);
   oapc_thread_create(sendLoop,sendData);
   return OAPC_OK;
}


SF_EXT_API int sf_if_wx_set_product_identifier(void* instance, const wxString *productIdentifier)
{
   struct if_inst_data *instData;
   struct if_send_data *sendData;

   if (!instance) return OAPC_ERROR_INVALID_INPUT;
   instData=(struct if_inst_data*)instance;
   if (instData->m_sock==0) return OAPC_ERROR_RESOURCE;

   if ((instData->m_projectName->length()>0) && 
       (productIdentifier->compare(*instData->m_projectName)==0)) return OAPC_OK;

   *instData->m_projectName=*productIdentifier;

   if (!hasClients(instData)) return OAPC_OK;

   sendData=(struct if_send_data*)malloc(sizeof(struct if_send_data));
   if (!sendData) return OAPC_ERROR_NO_MEMORY;

   sendData->message=getProjectNameMessage(instData);
   sendData->instData=instData;
   cleanupSocketList(instData);
   oapc_thread_create(sendLoop,sendData);
   return OAPC_OK;
}


SF_EXT_API int   sf_if_wx_set_error(void* instance,const wxString *errorText)
{
   struct if_inst_data *instData;
   struct if_send_data *sendData;

   if (!instance) return OAPC_ERROR_INVALID_INPUT;
   instData=(struct if_inst_data*)instance;
   if (instData->m_sock==0) return OAPC_ERROR_RESOURCE;

   if ((instData->m_errorText->length()>0) && (errorText->compare(*instData->m_errorText)==0)) return OAPC_OK;
   if ((instData->m_errorText->empty()) && (errorText->empty())) return OAPC_OK;

   *instData->m_errorText=*errorText;
   if ((instData->m_errorText->empty()) && (instData->m_prevState!=eUNKNOWN)) instData->m_state= instData->m_prevState;
   else if ((!instData->m_errorText->empty()) && (instData->m_state!=eERROR))
   {
      instData->m_prevState=instData->m_state;
      instData->m_state=eERROR;
   }

   if (!hasClients(instData)) return OAPC_OK;

   sendData=(struct if_send_data*)malloc(sizeof(struct if_send_data));
   if (!sendData) return OAPC_ERROR_NO_MEMORY;

   sendData->message=getStateMessage(instData);
   sendData->instData=instData;
   cleanupSocketList(instData);
   oapc_thread_create(sendLoop,sendData);
   return OAPC_OK;
}


static char *getPartsMessage(struct if_inst_data *instData,const unsigned int parts)
{
   wxXmlDocument xmlDoc;
   wxString      stateText;
#ifndef ENV_USE_PUGIXML
   wxXmlNode    *root=createIfRootNode(instData,xmlDoc);

   wxXmlNode* dataNode=new wxXmlNode(root,wxXML_ELEMENT_NODE,_T("Data"));
   dataNode->AddAttribute(_T("Parts"),wxString::Format(_T("%d"),parts));
   if (!instData->m_traceDataProduct->empty())
    dataNode->AddAttribute(_T("ProdID"),*instData->m_traceDataProduct);
   if (!instData->m_traceDataBoard->empty())
	dataNode->AddAttribute(_T("ID"),*instData->m_traceDataBoard);
   while (instData->m_traceData->size()>0)
   {
       wxXmlNode* traceNode=new wxXmlNode(dataNode,wxXML_ELEMENT_NODE,_T("Trace"));

       traceNode->AddAttribute(_T("Name"), instData->m_traceData->front().first);
       if (instData->m_traceData->front().second.length()>0)
        traceNode->AddAttribute(_T("ID"), instData->m_traceData->front().second);

       instData->m_traceData->pop();
   }
#else
   wxXmlNode     root=createIfRootNode(instData,xmlDoc);
   char          txt[200+1];
   wxXmlNode     dataNode=root.append_child("Data");

   snprintf(txt,200,"%d",parts);
   dataNode.append_attribute("Parts")=txt;

   if (!instData->m_traceDataProduct->empty())
    dataNode.append_attribute("ProdID")=instData->m_traceDataProduct->c_str();
   if (!instData->m_traceDataBoard->empty())
    dataNode.append_attribute("ID")=instData->m_traceDataBoard->c_str();
   while (instData->m_traceData->size()>0)
   {
       wxXmlNode* traceNode=&dataNode.append_child("Trace");

       traceNode->append_attribute("Name")= instData->m_traceData->front().first.c_str();
       if (instData->m_traceData->front().second.length()>0)
        traceNode->append_attribute("ID")= instData->m_traceData->front().second.c_str();

       instData->m_traceData->pop();
   }

#endif

   return convertMessage(&xmlDoc);
}


SF_EXT_API int sf_if_set_parts(void* instance, const unsigned int parts)
{
   struct if_inst_data *instData;
   struct if_send_data *sendData;

   if (!instance) return OAPC_ERROR_INVALID_INPUT;
   instData=(struct if_inst_data*)instance;
   if (instData->m_sock==0) return OAPC_ERROR_RESOURCE;

   if (!hasClients(instData)) return OAPC_OK;

   sendData=(struct if_send_data*)malloc(sizeof(struct if_send_data));
   if (!sendData) return OAPC_ERROR_NO_MEMORY;

   sendData->message=getPartsMessage(instData,parts);
   sendData->instData=instData;
   cleanupSocketList(instData);
   oapc_thread_create(sendLoop,sendData);
   flushTraceList(instData);
   return OAPC_OK;
}


static char *getSlicesMessage(struct if_inst_data *instData,const unsigned int maxSlices,const unsigned int currSlice)
{
   wxXmlDocument xmlDoc;
   wxString      stateText;
#ifndef ENV_USE_PUGIXML
   wxXmlNode    *root=createIfRootNode(instData,xmlDoc);

   wxXmlNode* dataNode=new wxXmlNode(root,wxXML_ELEMENT_NODE,_T("Data"));
   dataNode->AddAttribute(_T("MaxSlices"),wxString::Format(_T("%d"),maxSlices));
   dataNode->AddAttribute(_T("CurrSlice"),wxString::Format(_T("%d"),currSlice));
#else
   wxXmlNode     root=createIfRootNode(instData,xmlDoc);
   char          txt[200+1];
   wxXmlNode     dataNode=root.append_child("Data");

   snprintf(txt,200,"%d",maxSlices);
   dataNode.append_attribute("MaxSlices")=txt;
   snprintf(txt,200,"%d",currSlice);
   dataNode.append_attribute("CurrSlice")=txt;
#endif

   return convertMessage(&xmlDoc);
}


SF_EXT_API int sf_if_set_slices(void* instance, const unsigned int maxSlices, const unsigned int currSlice)
{
   struct if_inst_data *instData;
   struct if_send_data *sendData;

   if (!instance) return OAPC_ERROR_INVALID_INPUT;
   instData=(struct if_inst_data*)instance;
   if (instData->m_sock==0) return OAPC_ERROR_RESOURCE;

   if (!hasClients(instData)) return OAPC_OK;

   sendData=(struct if_send_data*)malloc(sizeof(struct if_send_data));
   if (!sendData) return OAPC_ERROR_NO_MEMORY;

   sendData->message=getSlicesMessage(instData,maxSlices,currSlice);
   sendData->instData=instData;
   cleanupSocketList(instData);
   oapc_thread_create(sendLoop,sendData);
   return OAPC_OK;
}


static char *getNotificationMessage(struct if_inst_data *instData,const wxString *msg,const long severity,const wxString *description,const wxString *src)
{
   wxXmlDocument xmlDoc;
   wxString      priorityText,priorityValue;
#ifndef ENV_USE_PUGIXML
   wxXmlNode    *root=createIfRootNode(instData,xmlDoc);
   wxXmlNode    *dataNode=new wxXmlNode(root,wxXML_ELEMENT_NODE,_T("Msg"));

   dataNode->AddAttribute(_T("Src"),*src);
   dataNode->AddAttribute(_T("Text"),*msg);
#else
   wxXmlNode     root=createIfRootNode(instData,xmlDoc);
   wxXmlNode     dataNode=root.append_child("Msg");

   dataNode.append_attribute("Src")=src->c_str();
   dataNode.append_attribute("Text")=msg->c_str();
#endif
   switch (severity)
   {
      case 1:
         priorityText=_T("Fatal error");
         priorityValue=_T("1");
         break;
      case 2:
         priorityText=_T("Error");
         priorityValue=_T("2");
         break;
      case 3:
         priorityText=_T("Warning");
         priorityValue=_T("3");
         break;
      case 4:
         priorityText=_T("Info");
         priorityValue=_T("4");
         break;
      default:
      {
         priorityText=_T("Other");
#ifndef ENV_USE_PUGIXML
         priorityValue=wxString::Format(_T("%d"),severity);
#else
         std::ostringstream stringStream;
         stringStream << severity;
         priorityValue = stringStream.str();
#endif
         break;
      }
   }
#ifndef ENV_USE_PUGIXML
   dataNode->AddAttribute(_T("Prio"),priorityValue);
   dataNode->AddAttribute(_T("PrioText"),priorityText);
   dataNode->AddAttribute(_T("Desc"),*description);
#else
   dataNode.append_attribute("Prio")=priorityValue.c_str();
   dataNode.append_attribute("PrioText")=priorityText.c_str();
   dataNode.append_attribute("Desc")=description->c_str();
#endif
   return convertMessage(&xmlDoc);
}


SF_EXT_API int sf_if_wx_set_hermes_notification(void* instance,const long notificationCode,const long severity,const wxString *description,const wxString *src)
{
   struct if_inst_data *instData;
   struct if_send_data *sendData;
   wxString             msg;

   if (!instance) return OAPC_ERROR_INVALID_INPUT;
   instData=(struct if_inst_data*)instance;
   if (instData->m_sock==0) return OAPC_ERROR_RESOURCE;

   if (!hasClients(instData)) return OAPC_OK;

   sendData=(struct if_send_data*)malloc(sizeof(struct if_send_data));
   if (!sendData) return OAPC_ERROR_NO_MEMORY;

   msg=_T("Hermes notification");
   switch (notificationCode)
   {
      case 1:
         msg=msg+_T(", protocol error");
         break;
      case 2:
         msg=msg+_T(", connection refused because of an established connection");
         break;
      case 3:
         msg=msg+_T(", connection reset because of changed configuration");
         break;
      case 4:
         msg=msg+_T(", configuration error");
         break;
      case 5:
         msg=msg+_T(", machine shutdown");
         break;
      default:
         break;
   }
   sendData->message=getNotificationMessage(instData,&msg,severity,description,src);
   sendData->instData=instData;
   cleanupSocketList(instData);
   oapc_thread_create(sendLoop,sendData);
   return OAPC_OK;
}



static char *getPControlInfoDataMessage(struct if_inst_data *instData,const int idx,const double value,const wxString *name,const wxString *unit)
{
   wxXmlDocument xmlDoc;
   wxString      text;
#ifndef ENV_USE_PUGIXML
   wxXmlNode    *root=createIfRootNode(instData,xmlDoc);
   wxXmlNode    *dataNode=new wxXmlNode(root,wxXML_ELEMENT_NODE,_T("Process"));
   
   dataNode->AddAttribute(_T("Name"),*name);
   dataNode->AddAttribute(_T("Unit"),*unit);
   dataNode->AddAttribute(_T("Value"),wxString::Format(_T("%f"),value));
   dataNode->AddAttribute(_T("Idx"), wxString::Format(_T("%d"),idx));
#else
   wxXmlNode     root=createIfRootNode(instData,xmlDoc);
   char          txt[200+1];
   wxXmlNode     dataNode=root.append_child("Process");

   dataNode.append_attribute("Name")=name->c_str();
   dataNode.append_attribute("Unit")=unit->c_str();
   snprintf(txt,200,"%d",value);
   dataNode.append_attribute("Value")=txt;
   snprintf(txt,200,"%d",idx);
   dataNode.append_attribute("Idx")=txt;
#endif


   return convertMessage(&xmlDoc);
}


SF_EXT_API int sf_if_wx_set_process_info_data(void* instance,const unsigned int idx,const double value,const wxString *name,const wxString *unit)
{
   struct if_inst_data *instData;
   struct if_send_data *sendData;

   if (idx>=4)
   {
      assert(0);
      return OAPC_ERROR_INVALID_INPUT;
   }
   if (!instance) return OAPC_ERROR_INVALID_INPUT;
   instData=(struct if_inst_data*)instance;
   if (instData->m_sock==0) return OAPC_ERROR_RESOURCE;
   if ((instData->m_lastProcessValue[idx]==(int)OAPC_ROUND(value*1000.0,0))) return OAPC_OK;

   if (!hasClients(instData)) return OAPC_OK;

   sendData=(struct if_send_data*)malloc(sizeof(struct if_send_data));
   if (!sendData) return OAPC_ERROR_NO_MEMORY;

   sendData->message=getPControlInfoDataMessage(instData,idx,value,name,unit);
   sendData->instData=instData;
   cleanupSocketList(instData);
   oapc_thread_create(sendLoop,sendData);
   instData->m_lastProcessValue[idx]=(int)OAPC_ROUND(value*1000.0,0);
   return OAPC_OK;
}


SF_EXT_API int sf_if_wx_append_trace_data(void* instance, const wxString *name, const wxString *data)
{
   struct if_inst_data *instData;

   if (!instance) return OAPC_ERROR_INVALID_INPUT;
   instData=(struct if_inst_data*)instance;

   instData->m_traceData->push(std::make_pair(*name,*data));
   return OAPC_OK;
}


SF_EXT_API int sf_if_wx_set_hermes_trace_info(void* instance,const wxString *boardID,const wxString *productID)
{
   struct if_inst_data *instData;

   if (!instance) return OAPC_ERROR_INVALID_INPUT;
   instData=(struct if_inst_data*)instance;

   *instData->m_traceDataBoard=*boardID;
   *instData->m_traceDataProduct=*productID;
   return OAPC_OK;
}

