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

#include <assert.h>
#include <string.h>
#include <stdarg.h>

#include "liboapc.h"
#include "libsmartfactory.h"

#define MAX_NAME_LENGTH 128

#define TYPE_CHECK_ALIVE            "CheckAlive"
#define TYPE_SERVICE_DESCRIPTION    "ServiceDescription"
#define TYPE_NOTIFICATION           "Notification"
#define TYPE_BOARD_AVAILABLE        "BoardAvailable"
#define TYPE_REVOKE_BOARD_AVAILABLE "RevokeBoardAvailable"
#define TYPE_MACHINE_READY          "MachineReady"
#define TYPE_REVOKE_MACHINE_READY   "RevokeMachineReady"
#define TYPE_START_TRANSPORT        "StartTransport"
#define TYPE_STOP_TRANSPORT         "StopTransport"
#define TYPE_TRANSPORT_FINISHED     "TransportFinished"

#define TYPE_SET_CONFIGURATION      "SetConfiguration"
#define TYPE_GET_CONFIGURATION      "GetConfiguration"
#define TYPE_CURRENT_CONFIGURATION  "CurrentConfiguration"


struct board_data
{
   wxString BoardId,BoardIdCreatedBy;
   long     FailedBoard;
   wxString ProductTypeId;
   long     FlippedBoard;
   wxString TopBarcode,BottomBarcode;
   wxString Length,Width,Thickness,ConveyorSpeed,TopClearanceHeight,BottomClearanceHeight;
   //=====================================================================================
   long     TransferState;
};

struct hermes_inst_data
{
   bool                            m_acceptLoopRunning,m_toPrevLoopRunning,m_toNextLoopRunning,m_toConfigLoopRunning;
   int                             m_toNextSSock,m_toNextCSock;
   int                             m_toConfigSSock;
   time_t                          m_lastTransmission;
   wxString                       *m_machineID,*m_productID;
   struct board_data              *m_nextBoard,*m_currBoard;
   enum hermes_prev_state          m_prevState;
   hermes_prev_state_callback      m_prev_state_callback;
   void                           *m_prev_state_data;
   enum hermes_prev_command        m_prevCommand;
   enum hermes_next_state          m_nextState;
   hermes_next_state_callback      m_next_state_callback;
   void                           *m_next_state_data;
   hermes_wx_notification_callback m_notification_callback;
   void                           *m_notification_data;
   hermes_log_callback             m_log_callback;
   void                           *m_log_data;
   enum hermes_next_command        m_nextCommand;
   bool                            m_isShutdown;
   void                           *m_parameterMutex;
   enum hermes_level               m_level;

   long                            m_prevMachinePort,m_nextMachinePort;
   char                            m_prevMachineIP[MAX_NAME_LENGTH+1];
};


static wxString create_uuid()
{
   //[0-9a-f]{8}-[0-9a-f]{4}-[0-9a-f]{4}-[0-9a-f]{4}-[0-9a-f]{12}
   wxString s;

#ifndef ENV_USE_PUGIXML
   s.Printf(_T("%x%x%x%x%x%x%x%x-%x%x%x%x-%x%x%x%x-%x%x%x%x-%x%x%x%x%x%x%x%x%x%x"),
       rand() & 0x0F,rand() & 0x0F,rand() & 0x0F,rand() & 0x0F,rand() & 0x0F,rand() & 0x0F,rand() & 0x0F,rand() & 0x0F,
       rand() & 0x0F,rand() & 0x0F,rand() & 0x0F,rand() & 0x0F,
       rand() & 0x0F,rand() & 0x0F,rand() & 0x0F,rand() & 0x0F,
       rand() & 0x0F,rand() & 0x0F,rand() & 0x0F,rand() & 0x0F,
       rand() & 0x0F,rand() & 0x0F,rand() & 0x0F,rand() & 0x0F,rand() & 0x0F,rand() & 0x0F,rand() & 0x0F,rand() & 0x0F,rand() & 0x0F,rand() & 0x0F);
   s.Printf(_T("%s%x%x"),s.c_str(),rand() & 0x0F,rand() & 0x0F);
#else
   std::ostringstream stringStream;
   stringStream << (rand() & 0x0F) <<  (rand() & 0x0F) << (rand() & 0x0F) << (rand() & 0x0F) << (rand() & 0x0F) << (rand() & 0x0F) << (rand() & 0x0F) << (rand() & 0x0F) << "-" <<
                   (rand() & 0x0F) << (rand() & 0x0F) << (rand() & 0x0F) << (rand() & 0x0F) << "-" <<
                   (rand() & 0x0F) << (rand() & 0x0F) << (rand() & 0x0F) << (rand() & 0x0F) << "-" <<
                   (rand() & 0x0F) << (rand() & 0x0F) << (rand() & 0x0F) << (rand() & 0x0F) << "-" <<
                   (rand() & 0x0F) << (rand() & 0x0F) << (rand() & 0x0F) << (rand() & 0x0F) << (rand() & 0x0F) << (rand() & 0x0F) << (rand() & 0x0F) << (rand() & 0x0F) << (rand() & 0x0F) << (rand() & 0x0F) << (rand() & 0x0F) << (rand() & 0x0F);
   s=stringStream.str();
#endif
   return s;
}


#ifndef ENV_USE_PUGIXML
static wxXmlNode *createHermesRootNode(struct hermes_inst_data *instData,wxXmlDocument &xmlDoc)
{
   wxDateTime time=wxDateTime::Now();
   wxXmlNode* root=new wxXmlNode(NULL,wxXML_ELEMENT_NODE,_T("Hermes"));

   oapc_thread_mutex_lock(instData->m_parameterMutex);
#if wxCHECK_VERSION(3,0,0)
   root->AddAttribute(new wxXmlAttribute(_T("TimeStamp"),time.Format(_T("%Y-%m-%dT%H:%M:%S")))); // ISO 8601 time format, local time
#else
   root->AddProperty(new wxXmlProperty(_T("TimeStamp"),time.Format(_T("%Y-%m-%dT%H:%M:%S")),NULL)); // ISO 8601 time format, local time
#endif
   oapc_thread_mutex_unlock(instData->m_parameterMutex);
   xmlDoc.SetRoot(root);
   return root;
}
#else // ENV_USE_PUGIXML
static wxXmlNode createHermesRootNode(struct hermes_inst_data *instData,wxXmlDocument &xmlDoc)
{
   time_t                currTime=time(NULL);
   char                  fmtTime[200+1];
   struct tm            *timeinfo;
   pugi::xml_node        declarationNode=xmlDoc.append_child(pugi::node_declaration);
   pugi::xml_node        root=xmlDoc.append_child("Hermes");

   declarationNode.append_attribute("version")="1.0";
   declarationNode.append_attribute("encoding")="UTF-8";
   declarationNode.append_attribute("standalone")="yes";

   timeinfo=localtime(&currTime);
   strftime(fmtTime,200,"%Y-%m-%dT%H:%M:%S",timeinfo);

   oapc_thread_mutex_lock(instData->m_parameterMutex);
   root.append_attribute("TimeStamp")=fmtTime;
   oapc_thread_mutex_unlock(instData->m_parameterMutex);

   return root;
}


struct xml_string_writer: pugi::xml_writer
{
    std::string result;

    virtual void write(const void* data, size_t size)
    {
        result.append(static_cast<const char*>(data), size);
    }
};
#endif // ENV_USE_PUGIXML


char *convertMessage(const wxXmlDocument *xmlDoc)
{
   char *message;

#ifndef ENV_USE_PUGIXML
   wxStringOutputStream stream;
   xmlDoc->Save(stream);
   message=(char*)malloc((stream.GetString().Length()+1)*4);
   if (!message) return NULL;
   oapc_unicode_stringToCharUTF8(stream.GetString(),message,(stream.GetString().Length()+1)*4);
#else
   xml_string_writer writer;
   
   xmlDoc->print(writer);
   message=(char*)malloc((writer.result.length()+1)*4);
   memcpy(message,writer.result.c_str(),writer.result.length());
   if (!message) return NULL;
#endif
   return message;
}



static void sendMessage(struct hermes_inst_data *instData,const int s,const wxXmlDocument *xmlDoc)
{
   char *message;

   message=convertMessage(xmlDoc);
   oapc_tcp_send(s,message,strlen(message),1000);
   free(message);
   instData->m_lastTransmission=time(NULL);
}


static void sendServiceDescription(struct hermes_inst_data *instData,const int s)
{
   wxXmlDocument xmlDoc;
#ifndef ENV_USE_PUGIXML
   wxXmlNode    *root=createHermesRootNode(instData,xmlDoc);
   wxXmlNode    *serviceDescNode=new wxXmlNode(root,wxXML_ELEMENT_NODE,_T(TYPE_SERVICE_DESCRIPTION));

   oapc_thread_mutex_lock(instData->m_parameterMutex);
   serviceDescNode->AddAttribute(_T("MachineId"),*instData->m_machineID);
   oapc_thread_mutex_unlock(instData->m_parameterMutex);
   serviceDescNode->AddAttribute(_T("LaneId"),   _T("1"));
   serviceDescNode->AddAttribute(_T("Version"),  _T("1.0"));
#else
   wxXmlNode     root=createHermesRootNode(instData,xmlDoc);
   wxXmlNode     serviceDescNode=root.append_child(TYPE_SERVICE_DESCRIPTION);

   oapc_thread_mutex_lock(instData->m_parameterMutex);
   serviceDescNode.append_attribute("MachineId")=instData->m_machineID->c_str();
   oapc_thread_mutex_unlock(instData->m_parameterMutex);
   serviceDescNode.append_attribute("LaneId")="1";
   serviceDescNode.append_attribute("Version")="1.0";
#endif
   sendMessage(instData,s,&xmlDoc);
}


static void sendNotification(struct hermes_inst_data *instData,const int s,const int code,const int severity,const wxString msg)
{
   wxXmlDocument xmlDoc;
#ifndef ENV_USE_PUGIXML
   wxXmlNode    *root=createHermesRootNode(instData,xmlDoc);
   wxXmlNode    *notifyNode=new wxXmlNode(root,wxXML_ELEMENT_NODE,_T(TYPE_NOTIFICATION));
   wxString      val;

   val=wxString::Format(_T("%d"),code);
   notifyNode->AddAttribute(_T("NotificationCode"),val);
   val=wxString::Format(_T("%d"),severity);
   notifyNode->AddAttribute(_T("Severity"),        val);
   notifyNode->AddAttribute(_T("Description"),     msg);
#else
   wxXmlNode     root=createHermesRootNode(instData,xmlDoc);
   wxXmlNode     notifyNode=root.append_child(TYPE_NOTIFICATION);
   char          val[200+1];

   snprintf(val,200,"%d",code);
   notifyNode.append_attribute("NotificationCode")=val;
   snprintf(val,200,"%d",severity);
   notifyNode.append_attribute("Severity")=val;
   notifyNode.append_attribute("Description")=msg.c_str();
#endif
   sendMessage(instData,s,&xmlDoc);
}


static void sendShutdown(struct hermes_inst_data *instData,const int s)
{
   if (instData->m_isShutdown)
    sendNotification(instData,s,5,4,_T("Service stopped by application/closedown"));
   else
    sendNotification(instData,s,3,3,_T("Service stopped due to change of configuration"));
}


static void decodeNotification(struct hermes_inst_data *instData,const wxXmlNode *childNode,long *notificationCode,long *severity,wxString *description,const wxString src)
{
   wxString val;

#ifndef ENV_USE_PUGIXML
 #if wxCHECK_VERSION(3,0,0)
   childNode->GetAttribute(_T("NotificationCode"),&val); val.ToLong(notificationCode,10);
   childNode->GetAttribute(_T("Severity"),&val);         val.ToLong(severity,10);
   childNode->GetAttribute(_T("Description"),description);
 #else
   childNode->GetPropVal(_T("NotificationCode"),&val); val.ToLong(notificationCode,10);
   childNode->GetPropVal(_T("Severity"),&val);         val.ToLong(severity,10);
   childNode->GetPropVal(_T("Description"),description);
 #endif
#else
   pugi::xml_attribute attr;

   if (attr=childNode->attribute("NotificationCode")) *notificationCode=attr.as_int();
   if (attr=childNode->attribute("Severity"))         *severity=attr.as_int();
   if (attr=childNode->attribute("Description"))      *description=std::string(attr.as_string());
#endif
   oapc_thread_mutex_lock(instData->m_parameterMutex);
   if (instData->m_notification_callback)
    instData->m_notification_callback(*notificationCode,*severity,description,&src, instData->m_notification_data);
   oapc_thread_mutex_unlock(instData->m_parameterMutex);
}


static void newPrevState(struct hermes_inst_data *instData,const enum hermes_prev_state prevState,const bool error)
{
   instData->m_prevState=prevState;
   oapc_thread_mutex_lock(instData->m_parameterMutex);
   if (instData->m_prev_state_callback)
    instData->m_prev_state_callback(prevState,error, instData->m_prev_state_data);
   oapc_thread_mutex_unlock(instData->m_parameterMutex);
}


static void newNextState(struct hermes_inst_data *instData,const enum hermes_next_state nextState,const bool error)
{
   instData->m_nextState=nextState;
   oapc_thread_mutex_lock(instData->m_parameterMutex);
   if (instData->m_next_state_callback)
    instData->m_next_state_callback(nextState,error, instData->m_next_state_data);
   oapc_thread_mutex_unlock(instData->m_parameterMutex);
}


#define MAX_RECV_BUFFER 1000


static void uilog(struct hermes_inst_data *instData,const char *format, ...)
{
   va_list arglist;
   char    logTxt[MAX_NAME_LENGTH+1];

   va_start(arglist,format);
   vsnprintf(logTxt,MAX_NAME_LENGTH,format,arglist);
   va_end(arglist);
   oapc_thread_mutex_lock(instData->m_parameterMutex);
   if (instData->m_log_callback)
    instData->m_log_callback(logTxt, instData->m_log_data);
   oapc_thread_mutex_unlock(instData->m_parameterMutex);
}


static void *toNextLoop(void *data)
{
   char                  recvBuffer[MAX_RECV_BUFFER];
   int                   recvLen;
   std::string           recvData;
   struct hermes_inst_data     *instData;

   instData=(struct hermes_inst_data*)data;
   newNextState(instData,eNEXT_IDLE,false);
   while ((instData->m_toPrevLoopRunning) && (instData->m_toNextCSock>=0))
   {
      if (instData->m_nextCommand!=eNEXT_CMD_NONE)
      {
         switch (instData->m_nextCommand)
         {
            case eNEXT_CMD_TRANSPORT_FINISHED:
            {
               wxXmlDocument xmlDoc;
#ifndef ENV_USE_PUGIXML
               wxXmlNode    *root=createHermesRootNode(instData,xmlDoc);
               wxXmlNode    *subNode=new wxXmlNode(root,wxXML_ELEMENT_NODE,_T(TYPE_TRANSPORT_FINISHED));

               subNode->AddAttribute(_T("BoardId"),instData->m_currBoard->BoardId);
               subNode->AddAttribute(_T("TransferState"),_T("3"));
#else
               wxXmlNode     root=createHermesRootNode(instData,xmlDoc);
               wxXmlNode     subNode=root.append_child(TYPE_TRANSPORT_FINISHED);

               subNode.append_attribute("BoardId")=instData->m_currBoard->BoardId.c_str();
               subNode.append_attribute("TransferState")="3";
#endif
			   uilog(instData,"Hermes cmd to next: %s",TYPE_TRANSPORT_FINISHED);
               sendMessage(instData,instData->m_toNextCSock,&xmlDoc);
               break;
            }
	        case eNEXT_CMD_BOARD_AVAILABLE:
            {
			   uilog(instData,"Hermes cmd to next: %s",TYPE_BOARD_AVAILABLE);
               if (instData->m_prevState==ePREV_OFFLINE)
               {
                  instData->m_currBoard->BoardId=create_uuid();
                  oapc_thread_mutex_lock(instData->m_parameterMutex);
				  instData->m_currBoard->BoardIdCreatedBy=*instData->m_machineID;
				  instData->m_currBoard->ProductTypeId=*instData->m_productID;
                  oapc_thread_mutex_unlock(instData->m_parameterMutex);
				  instData->m_currBoard->FailedBoard=1;
				  instData->m_currBoard->FlippedBoard=1;
				  instData->m_currBoard->TopBarcode=wxEmptyString;
				  instData->m_currBoard->BottomBarcode=wxEmptyString;
				  instData->m_currBoard->Length=wxEmptyString;
				  instData->m_currBoard->Width=wxEmptyString;
				  instData->m_currBoard->Thickness=wxEmptyString;
				  instData->m_currBoard->ConveyorSpeed=wxEmptyString;
				  instData->m_currBoard->TopClearanceHeight=wxEmptyString;
				  instData->m_currBoard->BottomClearanceHeight=wxEmptyString;
               }
			   instData->m_currBoard->TransferState=3;

               wxXmlDocument xmlDoc;
#ifndef ENV_USE_PUGIXML
               wxXmlNode    *root=createHermesRootNode(instData,xmlDoc);
               wxXmlNode    *subNode=new wxXmlNode(root,wxXML_ELEMENT_NODE,_T(TYPE_BOARD_AVAILABLE));

               subNode->AddAttribute(_T("BoardId"),         instData->m_currBoard->BoardId);
               subNode->AddAttribute(_T("BoardIdCreatedBy"),instData->m_currBoard->BoardIdCreatedBy);
               subNode->AddAttribute(_T("ProductTypeId"),   instData->m_currBoard->ProductTypeId);
               subNode->AddAttribute(_T("FailedBoard"),     wxString::Format(_T("%d"),instData->m_currBoard->FailedBoard));
               subNode->AddAttribute(_T("FlippedBoard"),    wxString::Format(_T("%d"),instData->m_currBoard->FlippedBoard));

               if (instData->m_currBoard->TopBarcode.Length()>0)
                subNode->AddAttribute(_T("TopBarcode"),           instData->m_currBoard->TopBarcode);
               if (instData->m_currBoard->BottomBarcode.Length()>0)
                subNode->AddAttribute(_T("BottomBarcode"),        instData->m_currBoard->BottomBarcode);
               if (instData->m_currBoard->Length.Length()>0)
                subNode->AddAttribute(_T("Length"),               instData->m_currBoard->Length);
               if (instData->m_currBoard->Width.Length()>0)
                subNode->AddAttribute(_T("Width"),                instData->m_currBoard->Width);
               if (instData->m_currBoard->Thickness.Length()>0)
                subNode->AddAttribute(_T("Thickness"),            instData->m_currBoard->Thickness);
               if (instData->m_currBoard->ConveyorSpeed.Length()>0)
                subNode->AddAttribute(_T("ConveyorSpeed"),        instData->m_currBoard->ConveyorSpeed);
               if (instData->m_currBoard->TopClearanceHeight.Length()>0)
                subNode->AddAttribute(_T("TopClearanceHeight"),   instData->m_currBoard->TopClearanceHeight);
               if (instData->m_currBoard->BottomClearanceHeight.Length()>0)
                subNode->AddAttribute(_T("BottomClearanceHeight"),instData->m_currBoard->BottomClearanceHeight);
#else
               wxXmlNode    root=createHermesRootNode(instData,xmlDoc);
               wxXmlNode    subNode=root.append_child(TYPE_BOARD_AVAILABLE);
               char          val[200+1];


               subNode.append_attribute("BoardId")=instData->m_currBoard->BoardId.c_str();
               subNode.append_attribute("BoardIdCreatedBy")=instData->m_currBoard->BoardIdCreatedBy.c_str();
               subNode.append_attribute("ProductTypeId")=instData->m_currBoard->ProductTypeId.c_str();
               snprintf(val,200,"%d",instData->m_currBoard->FailedBoard);
               subNode.append_attribute("FailedBoard")=val;
               snprintf(val,200,"%d",instData->m_currBoard->FlippedBoard);
               subNode.append_attribute("FlippedBoard")=val;

               if (instData->m_currBoard->TopBarcode.length()>0)
                subNode.append_attribute("TopBarcode")=instData->m_currBoard->TopBarcode.c_str();
               if (instData->m_currBoard->BottomBarcode.length()>0)
                subNode.append_attribute("BottomBarcode")=instData->m_currBoard->BottomBarcode.c_str();
               if (instData->m_currBoard->Length.length()>0)
                subNode.append_attribute("Length")=instData->m_currBoard->Length.c_str();
               if (instData->m_currBoard->Width.length()>0)
                subNode.append_attribute("Width")=instData->m_currBoard->Width.c_str();
               if (instData->m_currBoard->Thickness.length()>0)
                subNode.append_attribute("Thickness")=instData->m_currBoard->Thickness.c_str();
               if (instData->m_currBoard->ConveyorSpeed.length()>0)
                subNode.append_attribute("ConveyorSpeed")=instData->m_currBoard->ConveyorSpeed.c_str();
               if (instData->m_currBoard->TopClearanceHeight.length()>0)
                subNode.append_attribute("TopClearanceHeight")=instData->m_currBoard->TopClearanceHeight.c_str();
               if (instData->m_currBoard->BottomClearanceHeight.length()>0)
                subNode.append_attribute("BottomClearanceHeight")=instData->m_currBoard->BottomClearanceHeight.c_str();
#endif
               sendMessage(instData,instData->m_toNextCSock,&xmlDoc);
               break;
            }
			case eNEXT_CMD_REVOKE_BOARD_AVAILABLE:
			{
			   uilog(instData,"Hermes cmd to next: %s",TYPE_REVOKE_BOARD_AVAILABLE);

               wxXmlDocument xmlDoc;
#ifndef ENV_USE_PUGIXML
               wxXmlNode    *root=createHermesRootNode(instData,xmlDoc);
               new wxXmlNode(root,wxXML_ELEMENT_NODE,_T(TYPE_REVOKE_BOARD_AVAILABLE));
#else
               wxXmlNode     root=createHermesRootNode(instData,xmlDoc);
               root.append_child(TYPE_REVOKE_BOARD_AVAILABLE);
#endif
			   sendMessage(instData,instData->m_toNextCSock,&xmlDoc);
               break;
			}
			default:
               assert(0);
         }

		 instData->m_nextCommand=eNEXT_CMD_NONE;
      }

      recvLen=oapc_tcp_recv(instData->m_toNextCSock,recvBuffer,MAX_RECV_BUFFER,NULL,100);
      if (recvLen>0)
      {
         size_t endPos;

         recvData.append(recvBuffer,recvLen);

         endPos=recvData.find("/Hermes>");
         if (endPos!=std::string::npos)
         {
            size_t startPos;

            startPos=recvData.find("<Hermes");
            if (startPos==std::string::npos)
            {
               assert(0);
               continue;
            }
#ifndef ENV_USE_PUGIXML
            wxMemoryInputStream *memInStream=new wxMemoryInputStream(recvData.data(),endPos+8);
            if (memInStream)
#else
            wxXmlDocument xmlDoc;
            pugi::xml_parse_result result=xmlDoc.load_buffer(recvData.data(),endPos+8);
            if (result.status==pugi::status_ok)
#endif
            {
#ifndef ENV_USE_PUGIXML
               wxXmlDocument xmlDoc(*memInStream);
               wxXmlNode *rootNode=xmlDoc.GetRoot();

               if ((rootNode) && (rootNode->GetName().CmpNoCase(_T("hermes"))==0))
#else
               wxXmlNode rootNode=xmlDoc.child("Hermes");

               if (!rootNode.empty())
#endif
               {
                  wxString   val;
#ifndef ENV_USE_PUGIXML
                  wxXmlNode *childNode=rootNode->GetChildren();

                  if (childNode)
#endif
                  {
#ifndef ENV_USE_PUGIXML
                     if (childNode->GetName().Cmp(_T(TYPE_MACHINE_READY))==0)
#else
                     if (!rootNode.child(TYPE_MACHINE_READY).empty())
#endif
                     {
                        uilog(instData,"Hermes cmd from next: %s",TYPE_MACHINE_READY);
                        newNextState(instData,eNEXT_MACHINE_READY,false);
                     }
#ifndef ENV_USE_PUGIXML
                     else if (childNode->GetName().Cmp(_T(TYPE_REVOKE_MACHINE_READY))==0)
#else
                     if (!rootNode.child(TYPE_REVOKE_MACHINE_READY).empty())
#endif
                     {
                        uilog(instData,"Hermes cmd from next: %s",TYPE_REVOKE_MACHINE_READY);
                        newNextState(instData,eNEXT_REVOKE_MACHINE_READY,false);
                     }
#ifndef ENV_USE_PUGIXML
                     else if (childNode->GetName().Cmp(_(TYPE_START_TRANSPORT))==0)
#else
                     if (!rootNode.child(TYPE_START_TRANSPORT).empty())
#endif
                     {
                        wxString boardId;

                        uilog(instData,"Hermes cmd from next: %s",TYPE_START_TRANSPORT);
#ifndef ENV_USE_PUGIXML
 #if wxCHECK_VERSION(3,0,0)
                        childNode->GetAttribute(_T("BoardId"),&boardId);
 #else
                        childNode->GetPropVal(_T("BoardId"),&boardId);
 #endif
#else
                        wxXmlNode childNode=rootNode.child(TYPE_START_TRANSPORT);

                        pugi::xml_attribute attr=childNode.attribute("BoardId");
                        if (!attr.empty())
                         boardId=std::string(attr.as_string());
#endif
                        newNextState(instData,eNEXT_START_TRANSPORT,boardId.compare(instData->m_currBoard->BoardId)!=0);
                     }
#ifndef ENV_USE_PUGIXML
                     else if (childNode->GetName().Cmp(_T(TYPE_STOP_TRANSPORT))==0)
#else
                     if (!rootNode.child(TYPE_STOP_TRANSPORT).empty())
#endif
                     {
                        wxString boardId;

                        uilog(instData,"Hermes cmd from next: %s",TYPE_STOP_TRANSPORT);
#ifndef ENV_USE_PUGIXML
 #if wxCHECK_VERSION(3,0,0)
                        childNode->GetAttribute(_T("BoardId"),&boardId);
                        childNode->GetAttribute(_T("TransferState"),&val); val.ToLong(&instData->m_currBoard->TransferState,10);
 #else
                        childNode->GetPropVal(_T("BoardId"),&boardId);
                        childNode->GetPropVal(_T("TransferState"),&val); val.ToLong(&instData->m_currBoard->TransferState,10);
 #endif
#else
                        wxXmlNode childNode=rootNode.child(TYPE_STOP_TRANSPORT);

                        pugi::xml_attribute attr=childNode.attribute("BoardId");
                        if (!attr.empty())
                         boardId=std::string(attr.as_string());

                        if (attr=childNode.attribute("TransferState"))
                         instData->m_currBoard->TransferState=attr.as_int();
#endif
                        newNextState(instData,eNEXT_STOP_TRANSPORT,(boardId.compare(instData->m_currBoard->BoardId)!=0) || (instData->m_currBoard->TransferState!=3));
                     }
#ifndef ENV_USE_PUGIXML
                     else if (childNode->GetName().Cmp(_T(TYPE_NOTIFICATION))==0)
#else
                     if (!rootNode.child(TYPE_NOTIFICATION).empty())
#endif
                     {
                        long     notificationCode,severity;
                        wxString description;
#ifndef ENV_USE_PUGIXML

                        decodeNotification(instData,childNode,&notificationCode,&severity,&description,_T("Next machine"));
#else
                        wxXmlNode childNode=rootNode.child(TYPE_NOTIFICATION);
                        decodeNotification(instData,&childNode,&notificationCode,&severity,&description,_T("Next machine"));
#endif

                        uilog(instData,"Hermes cmd from next: %s",TYPE_NOTIFICATION);
                        switch (notificationCode)
                        {
                           case 1: // protocol error
                              assert(0);
                              break;
                           case 2: // Connection refused because of an established connection
                           case 3: // Connection reset because of changed configuration
                           case 5: // Machine shutdown
                              newNextState(instData,eNEXT_IDLE,false);
                              oapc_tcp_closesocket(instData->m_toNextCSock);
							  instData->m_toNextCSock=0;
                              break;
                           case 4: // Configuration error
                              assert(0);
                              break;
                           default:
                              assert(0);
                        }
                     }
#ifndef ENV_USE_PUGIXML
                     else if (childNode->GetName().Cmp(_T(TYPE_SERVICE_DESCRIPTION))==0)
#else
                     if (!rootNode.child(TYPE_SERVICE_DESCRIPTION).empty())
#endif
                     {
                        uilog(instData,"Hermes cmd from next: %s",TYPE_SERVICE_DESCRIPTION);
                        sendServiceDescription(instData,instData->m_toNextCSock);
                     }
#ifndef ENV_USE_PUGIXML
                     else if (childNode->GetName().Cmp(_T(TYPE_CHECK_ALIVE))==0)
#else
                     if (!rootNode.child(TYPE_CHECK_ALIVE).empty())
#endif
                     {
                         // no need to react on this
                     }
                     else assert(0);
                  }
#ifndef ENV_USE_PUGIXML
                  else assert(0);
#endif
               }
               else assert(0);
#ifndef ENV_USE_PUGIXML
               delete memInStream;
#endif
            }
            else assert(0);
            recvData.erase(0,endPos+8);
            while ((recvData.length()>0) && ((recvData.substr(0,1)=="\r") || (recvData.substr(0,1)=="\n")))
             recvData.erase(0,1); 
            if (instData->m_toNextCSock==0)
             break;
         }
      }
   }
   if (instData->m_toNextCSock>0) // send shutdown notification
   {
      sendShutdown(instData,instData->m_toNextCSock);
   }
   newNextState(instData,eNEXT_OFFLINE,false);
   instData->m_toNextLoopRunning=true;
   return NULL;
}


static void *toPrevLoop(void *data)
{
   int                   toPrevCSock=0;
   char                  ip[MAX_NAME_LENGTH*2];
   char                  recvBuffer[MAX_RECV_BUFFER];
   int                   recvLen;
   std::string           recvData;
   struct hermes_inst_data     *instData;

   instData=(struct hermes_inst_data*)data;
   memcpy(ip,instData->m_prevMachineIP,MAX_NAME_LENGTH*2);
#ifndef ENV_USE_PUGIXML
   oapc_unicode_utf16BEToASCII(ip,MAX_NAME_LENGTH);
#endif
   while (instData->m_toPrevLoopRunning)
   {
      toPrevCSock=oapc_tcp_connect_to(ip,(unsigned short)instData->m_prevMachinePort);
      if ((toPrevCSock>0) && (instData->m_toPrevLoopRunning))
      {
         newPrevState(instData,ePREV_WAITING_SERVICE_DESCRIPTION,false);
         sendServiceDescription(instData,toPrevCSock);

         oapc_tcp_set_blocking(toPrevCSock,0);

         while ((instData->m_toPrevLoopRunning) && (toPrevCSock>=0))
         {
            if (instData->m_prevState!=ePREV_WAITING_SERVICE_DESCRIPTION)
            {
               if (instData->m_prevCommand!=ePREV_CMD_NONE)
               {
                  switch (instData->m_prevCommand)
                  {
					 case ePREV_CMD_SET_MACHINE_READY:
					 {
                        wxXmlDocument xmlDoc;
#ifndef ENV_USE_PUGIXML
                        wxXmlNode    *root=createHermesRootNode(instData,xmlDoc);
                        wxXmlNode    *readyNode=new wxXmlNode(root,wxXML_ELEMENT_NODE,_T(TYPE_MACHINE_READY));

                        readyNode->AddAttribute(_T("FailedBoard"),_T("1")); // accept any good board, TODO: implement handling to drop and pass through failed boards
#else
                        wxXmlNode    root=createHermesRootNode(instData,xmlDoc);
                        wxXmlNode    readyNode=root.append_child(TYPE_MACHINE_READY);

                        readyNode.append_attribute("FailedBoard")="1"; // accept any good board, TODO: implement handling to drop and pass through failed boards
#endif
                        uilog(instData,"Hermes cmd to prev: %s",TYPE_MACHINE_READY);
                        sendMessage(instData,toPrevCSock,&xmlDoc);
						break;
					 }
					 case ePREV_CMD_REVOKE_MACHINE_READY:
					 {
                        wxXmlDocument xmlDoc;
#ifndef ENV_USE_PUGIXML
                        wxXmlNode    *root=createHermesRootNode(instData,xmlDoc);
     
                        new wxXmlNode(root,wxXML_ELEMENT_NODE,_T(TYPE_REVOKE_MACHINE_READY));
#else
                        wxXmlNode    root=createHermesRootNode(instData,xmlDoc);
     
                        root.append_child(TYPE_REVOKE_MACHINE_READY);
#endif
                        uilog(instData,"Hermes cmd to prev: %s",TYPE_REVOKE_MACHINE_READY);
                        sendMessage(instData,toPrevCSock,&xmlDoc);
						break;
					 }
                     case ePREV_CMD_START_TRANSPORT:
                     {
                        wxXmlDocument xmlDoc;
#ifndef ENV_USE_PUGIXML
                        wxXmlNode    *root=createHermesRootNode(instData,xmlDoc);
                        wxXmlNode    *readyNode=new wxXmlNode(root,wxXML_ELEMENT_NODE,_T(TYPE_START_TRANSPORT));

                        readyNode->AddAttribute(_T("BoardId"),instData->m_nextBoard->BoardId);
                        // readyNode->AddAttribute(_T("ConveyorSpeed"),); //optional
#else
                        wxXmlNode    root=createHermesRootNode(instData,xmlDoc);
                        wxXmlNode    readyNode=root.append_child(TYPE_START_TRANSPORT);

                        readyNode.append_attribute("BoardId")=instData->m_nextBoard->BoardId.c_str();
                        // readyNode->AddAttribute(_T("ConveyorSpeed"),); //optional
#endif
                        uilog(instData,"Hermes cmd to prev: %s",TYPE_START_TRANSPORT);
                        sendMessage(instData,toPrevCSock,&xmlDoc);
                        break;
                     }
                     case ePREV_CMD_STOP_TRANSPORT:
                     {
                        wxXmlDocument xmlDoc;
#ifndef ENV_USE_PUGIXML
                        wxXmlNode    *root=createHermesRootNode(instData,xmlDoc);
                        wxXmlNode    *readyNode=new wxXmlNode(root,wxXML_ELEMENT_NODE,_T(TYPE_STOP_TRANSPORT));

                        readyNode->AddAttribute(_T("BoardId"),instData->m_currBoard->BoardId);
                        readyNode->AddAttribute(_T("TransferState"),_T("3"));
#else
                        wxXmlNode    root=createHermesRootNode(instData,xmlDoc);
                        wxXmlNode    readyNode=root.append_child(TYPE_STOP_TRANSPORT);

                        readyNode.append_attribute("BoardId")=instData->m_currBoard->BoardId.c_str();
                        readyNode.append_attribute("TransferState")="3";
#endif
                        uilog(instData,"Hermes cmd to prev: %s",TYPE_STOP_TRANSPORT);
                        sendMessage(instData,toPrevCSock,&xmlDoc);
                        break;
                     }
                     default:
                        assert(0);
                        break;
                  }
				  instData->m_prevCommand=ePREV_CMD_NONE;
               }
            }

            recvLen=oapc_tcp_recv(toPrevCSock,recvBuffer,MAX_RECV_BUFFER,NULL,100);
            if (recvLen>0)
            {
               size_t endPos;

               recvData.append(recvBuffer,recvLen);

               endPos=recvData.find("/Hermes>");
               if (endPos!=std::string::npos)
               {
                  size_t startPos;

                  startPos=recvData.find("<Hermes");
                  if (startPos==std::string::npos)
                  {
                     assert(0);
                     continue;
                  }

#ifndef ENV_USE_PUGIXML
                  wxMemoryInputStream *memInStream=new wxMemoryInputStream(recvData.data(),endPos+8);
                  if (memInStream)
#else
                  wxXmlDocument xmlDoc;
                  pugi::xml_parse_result result=xmlDoc.load_buffer(recvData.data(),endPos+8);
                  if (result.status==pugi::status_ok)
#endif
                  {
#ifndef ENV_USE_PUGIXML
                     wxXmlDocument xmlDoc(*memInStream);
                     wxXmlNode *rootNode=xmlDoc.GetRoot();
                     if ((rootNode) && (rootNode->GetName().CmpNoCase(_T("hermes"))==0))
#else
                     wxXmlNode rootNode=xmlDoc.child("Hermes");

                     if (!rootNode.empty())
#endif
                     {
#ifndef ENV_USE_PUGIXML
                        wxString   val;
                        wxXmlNode *childNode=rootNode->GetChildren();

                        if (childNode)
#endif
                        {
#ifndef ENV_USE_PUGIXML
                           if (childNode->GetName().Cmp(_T(TYPE_BOARD_AVAILABLE))==0)
#else
                           if (!rootNode.child(TYPE_BOARD_AVAILABLE).empty())
#endif
                           {
                              uilog(instData,"Hermes cmd from prev: %s",TYPE_BOARD_AVAILABLE);
                              newPrevState(instData,ePREV_BOARD_AVAILABLE,false);
#ifndef ENV_USE_PUGIXML
 #if wxCHECK_VERSION(3,0,0)
                              childNode->GetAttribute(_T("BoardId"),         &instData->m_nextBoard->BoardId);
                              childNode->GetAttribute(_T("BoardIdCreatedBy"),&instData->m_nextBoard->BoardIdCreatedBy);
                              childNode->GetAttribute(_T("FailedBoard"),&val);          val.ToLong(&instData->m_nextBoard->FailedBoard,10);
                              childNode->GetAttribute(_T("ProductTypeId"),   &instData->m_nextBoard->ProductTypeId);
                              childNode->GetAttribute(_T("FlippedBoard"),&val);          val.ToLong(&instData->m_nextBoard->FlippedBoard,10);
                              childNode->GetAttribute(_T("TopBarcode"),      &instData->m_nextBoard->TopBarcode);
                              childNode->GetAttribute(_T("BottomBarcode"),   &instData->m_nextBoard->BottomBarcode);
                              childNode->GetAttribute(_T("Length"),          &instData->m_nextBoard->Length);
                              childNode->GetAttribute(_T("Width"),           &instData->m_nextBoard->Width);
                              childNode->GetAttribute(_T("Thickness"),       &instData->m_nextBoard->Thickness);
                              childNode->GetAttribute(_T("ConveyorSpeed"),   &instData->m_nextBoard->ConveyorSpeed);
                              childNode->GetAttribute(_T("TopClearanceHeight"),   &instData->m_nextBoard->TopClearanceHeight);
                              childNode->GetAttribute(_T("BottomClearanceHeight"),&instData->m_nextBoard->BottomClearanceHeight);
 #else
                              childNode->GetPropVal(_T("BoardId"),         &instData->m_nextBoard->BoardId);
                              childNode->GetPropVal(_T("BoardIdCreatedBy"),&instData->m_nextBoard->BoardIdCreatedBy);
                              childNode->GetPropVal(_T("FailedBoard"),&val);           val.ToLong(&instData->m_nextBoard->FailedBoard,10);
                              childNode->GetPropVal(_T("ProductTypeId"),   &instData->m_nextBoard->ProductTypeId);
                              childNode->GetPropVal(_T("FlippedBoard"),&val);          val.ToLong(&instData->m_nextBoard->FlippedBoard,10);
                              childNode->GetPropVal(_T("TopBarcode"),      &instData->m_nextBoard->TopBarcode);
                              childNode->GetPropVal(_T("BottomBarcode"),   &instData->m_nextBoard->BottomBarcode);
                              childNode->GetPropVal(_T("Length"),          &instData->m_nextBoard->Length);
                              childNode->GetPropVal(_T("Width"),           &instData->m_nextBoard->Width);
                              childNode->GetPropVal(_T("Thickness"),       &instData->m_nextBoard->Thickness);
                              childNode->GetPropVal(_T("ConveyorSpeed"),   &instData->m_nextBoard->ConveyorSpeed);
                              childNode->GetPropVal(_T("TopClearanceHeight"),   &instData->m_nextBoard->TopClearanceHeight);
                              childNode->GetPropVal(_T("BottomClearanceHeight"),&instData->m_nextBoard->BottomClearanceHeight);
 #endif
#else
                              wxXmlNode childNode=rootNode.child(TYPE_START_TRANSPORT);
                              pugi::xml_attribute attr;

                              if (attr=childNode.attribute("BoardId"))
                               instData->m_nextBoard->BoardId=std::string(attr.as_string());

                              if (attr=childNode.attribute("BoardIdCreatedBy"))
                               instData->m_nextBoard->BoardIdCreatedBy=std::string(attr.as_string());

                              if (attr=childNode.attribute("FailedBoard"))
                               instData->m_nextBoard->FailedBoard=attr.as_int(-1);

                              if (attr=childNode.attribute("ProductTypeId"))
                               instData->m_nextBoard->ProductTypeId=std::string(attr.as_string());

                              if (attr=childNode.attribute("FlippedBoard"))
                               instData->m_nextBoard->FlippedBoard=attr.as_int();

                              if (attr=childNode.attribute("TopBarcode"))
                               instData->m_nextBoard->TopBarcode=std::string(attr.as_string());

                              if (attr=childNode.attribute("BottomBarcode"))
                               instData->m_nextBoard->BottomBarcode=std::string(attr.as_string());

                              if (attr=childNode.attribute("Length"))
                               instData->m_nextBoard->Length=std::string(attr.as_string());

                              if (attr=childNode.attribute("Width"))
                               instData->m_nextBoard->Width=std::string(attr.as_string());

                              if (attr=childNode.attribute("Thickness"))
                               instData->m_nextBoard->Thickness=std::string(attr.as_string());

                              if (attr=childNode.attribute("ConveyorSpeed"))
                               instData->m_nextBoard->ConveyorSpeed=std::string(attr.as_string());

                              if (attr=childNode.attribute("TopClearanceHeight"))
                               instData->m_nextBoard->TopClearanceHeight=std::string(attr.as_string());

                              if (attr = childNode.attribute("BottomClearanceHeight"))
                               instData->m_nextBoard->BottomClearanceHeight=std::string(attr.as_string());
#endif
                           }
#ifndef ENV_USE_PUGIXML
                           else if (childNode->GetName().Cmp(_T(TYPE_NOTIFICATION)) == 0)
#else
                           else if (!rootNode.child(TYPE_NOTIFICATION).empty())
#endif
                           {
                              long     notificationCode,severity;
                              wxString description;
#ifndef ENV_USE_PUGIXML

                              decodeNotification(instData,childNode,&notificationCode,&severity,&description,_T("Previous machine"));
#else
                              wxXmlNode childNode=rootNode.child(TYPE_NOTIFICATION);

                              decodeNotification(instData,&childNode,&notificationCode,&severity,&description,_T("Previous machine"));
#endif
                              uilog(instData,"Hermes cmd from prev: %s",TYPE_NOTIFICATION);
                              switch (notificationCode)
                              {
                                 case 1: // protocol error
                                    assert(0);
                                    break;
                                 case 2: // Connection refused because of an established connection
                                 case 3: // Connection reset because of changed configuration
                                 case 5: // Machine shutdown
                                    newPrevState(instData,ePREV_IDLE,false);
                                    oapc_tcp_closesocket(toPrevCSock);
                                    toPrevCSock=0;
                                    break;
                                 case 4: // Configuration error
                                    assert(0);
                                    break;
                                 default:
                                    assert(0);
                              }
                           }
#ifndef ENV_USE_PUGIXML
                           else if (childNode->GetName().Cmp(_T(TYPE_TRANSPORT_FINISHED))==0)
#else
                           else if (!rootNode.child(TYPE_TRANSPORT_FINISHED).empty())
#endif
                           {
                              wxString boardId;

                              uilog(instData,"Hermes cmd from prev: %s",TYPE_TRANSPORT_FINISHED);
#ifndef ENV_USE_PUGIXML
 #if wxCHECK_VERSION(3,0,0)
                              childNode->GetAttribute(_T("BoardId"),&boardId);
                              childNode->GetAttribute(_T("TransferState"),&val); val.ToLong(&instData->m_nextBoard->TransferState,10);
 #else
                              childNode->GetPropVal(_T("BoardId"),  &boardId);
                              childNode->GetPropVal(_T("TransferState"),&val); val.ToLong(&instData->m_nextBoard->TransferState,10);
 #endif
#else // ENV_USE_PUGIXML
                              wxXmlNode childNode=rootNode.child(TYPE_TRANSPORT_FINISHED);

                              pugi::xml_attribute attr=childNode.attribute("BoardId");
                              if (!attr.empty())
                               boardId=std::string(attr.as_string());

                              attr=childNode.attribute("TransferState");
                              if (!attr.empty())
                               instData->m_nextBoard->TransferState=attr.as_int();

#endif // ENV_USE_PUGIXML
                              newPrevState(instData,ePREV_TRANSPORT_FINISHED,(instData->m_nextBoard->TransferState!=3) || (boardId.compare(instData->m_nextBoard->BoardId)!=0));
                           }
#ifndef ENV_USE_PUGIXML
                           else if (childNode->GetName().Cmp(_T(TYPE_REVOKE_BOARD_AVAILABLE))==0)
#else
                           else if (!rootNode.child(TYPE_REVOKE_BOARD_AVAILABLE).empty())
#endif
                           {
                              uilog(instData,"Hermes cmd from prev: %s",TYPE_REVOKE_BOARD_AVAILABLE);
                              newPrevState(instData,ePREV_REVOKE_BOARD_AVAILABLE,false);
                           }
#ifndef ENV_USE_PUGIXML
                           else if (childNode->GetName().Cmp(_T(TYPE_CHECK_ALIVE))==0)
#else
                           else if (!rootNode.child(TYPE_CHECK_ALIVE).empty())
#endif
                           {
                              // no need to react on this
                           }
#ifndef ENV_USE_PUGIXML
                           else if (childNode->GetName().Cmp(_T(TYPE_SERVICE_DESCRIPTION))==0)
#else
                           else if (!rootNode.child(TYPE_SERVICE_DESCRIPTION).empty())
#endif
                           {
                              // nothing to do here since we do not support more than Hermes 1.0
                              uilog(instData,"Hermes cmd from prev: %s",TYPE_SERVICE_DESCRIPTION);
                              newPrevState(instData,ePREV_IDLE,false);
                           }
                           else assert(0);
                        }
#ifndef ENV_USE_PUGIXML
                        else assert(0);
#endif
                     }
                     else assert(0);
#ifndef ENV_USE_PUGIXML
                     delete memInStream;
#endif
                  }
                  else assert(0);
                  recvData.erase(0,endPos+8);
                  while ((recvData.length()>0) && ((recvData.substr(0,1)=="\r") || (recvData.substr(0,1)=="\n")))
                   recvData.erase(0,1); 
                  if (toPrevCSock==0)
                   break;
               }
            }

            time_t currentTime=time(NULL);

            if (currentTime-instData->m_lastTransmission>60)
            {
               wxXmlDocument xmlDoc;
               char         *message;
#ifndef ENV_USE_PUGIXML
               wxXmlNode    *root=createHermesRootNode(instData,xmlDoc);

               new wxXmlNode(root,wxXML_ELEMENT_NODE,_T(TYPE_CHECK_ALIVE));
#else
               wxXmlNode     root=createHermesRootNode(instData,xmlDoc);

               root.append_child(TYPE_CHECK_ALIVE);
#endif
               message=convertMessage(&xmlDoc);
               if (oapc_tcp_send(toPrevCSock,message,strlen(message),1000)<=0)
               {
                  oapc_tcp_closesocket(toPrevCSock);
                  toPrevCSock=0;
               }
               free(message);
			   instData->m_lastTransmission=currentTime;
            }

         }
      }
      else
      {
         newPrevState(instData,ePREV_OFFLINE,true);
         oapc_thread_sleep(2000);
      }
   }
   if (toPrevCSock>0) // send shutdown notification
   {
      sendShutdown(instData,toPrevCSock);
   }
   instData->m_toPrevLoopRunning=true;
   return NULL;
}


static void *hermesAcceptLoop(void *data)
{
   int               c;
   unsigned long     ip;
   struct hermes_inst_data *instData;

   instData=(struct hermes_inst_data*)data;
   while (instData->m_acceptLoopRunning)
   {
      oapc_thread_sleep(0);

      c=oapc_tcp_accept_connection(instData->m_toNextSSock,&ip);
      if (c>0)
      {
         oapc_tcp_set_blocking(c,0);
         if (instData->m_toNextCSock>0)
         {
            wxXmlDocument xmlDoc;
#ifndef ENV_USE_PUGIXML
            wxXmlNode    *root=createHermesRootNode(instData,xmlDoc);
            wxXmlNode    *notifyNode=new wxXmlNode(root,wxXML_ELEMENT_NODE,_T(TYPE_NOTIFICATION));

#if wxCHECK_VERSION(3,0,0)
            notifyNode->AddAttribute(_T("NotificationCode"),_T("2"));
            notifyNode->AddAttribute(_T("Severity"),        _T("1"));
            notifyNode->AddAttribute(_T("Description"),     _T("Already connected with a machine"));
#else
            notifyNode->AddProperty(_T("NotificationCode"),_T("2"));
            notifyNode->AddProperty(_T("Severity"),        _T("1"));
            notifyNode->AddProperty(_T("Description"),     _T("Already connected with a machine"));
#endif
#else
            wxXmlNode     root=createHermesRootNode(instData,xmlDoc);
            wxXmlNode     notifyNode=root.append_child(TYPE_NOTIFICATION);

            notifyNode.append_attribute("NotificationCode")="2";
            notifyNode.append_attribute("Severity")        ="1";
            notifyNode.append_attribute("Description")     ="Already connected with a machine";
#endif
            sendMessage(instData,c,&xmlDoc);
            sendShutdown(instData,c);
            oapc_tcp_closesocket(c);
         }
         else
         {
            instData->m_toNextLoopRunning=true;
			instData->m_toNextCSock=c;
            oapc_thread_create(toNextLoop,data);
         }
         c=-1;
      }
   }
   instData->m_acceptLoopRunning=true;
   return NULL;
}


static int openMachineConnections(struct hermes_inst_data *instData)
{
   instData->m_lastTransmission=time(NULL);
   instData->m_toPrevLoopRunning=true;
   if (!oapc_thread_create(toPrevLoop,instData))
    return OAPC_ERROR_NO_MEMORY;

   {
      instData->m_toNextSSock=oapc_tcp_listen_on_port((unsigned short)instData->m_nextMachinePort,"0.0.0.0");
      if (instData->m_toNextSSock<=0) return OAPC_ERROR_RESOURCE;
      oapc_tcp_set_blocking(instData->m_toNextSSock,true);
	  instData->m_acceptLoopRunning=true;
      if (!oapc_thread_create(hermesAcceptLoop,instData))
       return OAPC_ERROR_NO_MEMORY;
   }
   return OAPC_OK;
}


static void closeMachineConnections(struct hermes_inst_data *instData,const bool isShutdown)
{
   int i=0;

   instData->m_isShutdown=isShutdown;
   instData->m_acceptLoopRunning = false;
   instData->m_toPrevLoopRunning=false;
   if (instData->m_toNextLoopRunning) instData->m_toNextLoopRunning=false; // the thread for connection to next machine may not be started, so no need to wait for it
   else instData->m_toNextLoopRunning=true;
   oapc_tcp_closesocket(instData->m_toNextSSock);
   for (i=0; i<50; i++)
   {
      if ((instData->m_acceptLoopRunning) &&
          (instData->m_toPrevLoopRunning) &&
          (instData->m_toNextLoopRunning)) break;
      oapc_thread_sleep(100);
   }
   instData->m_isShutdown=true;
}


static void handleConfigConnection(int s,const void *data)
{
   char              recvBuffer[MAX_RECV_BUFFER];
   int               recvLen;
   std::string       recvData;
   struct hermes_inst_data *instData;

   instData=(struct hermes_inst_data*)data;
   newNextState(instData,eNEXT_IDLE,false);
   while ((instData->m_toConfigLoopRunning) && (s>=0))
   {
      recvLen=oapc_tcp_recv(s,recvBuffer,MAX_RECV_BUFFER,NULL,100);
      if (recvLen>0)
      {
         size_t endPos;

         recvData.append(recvBuffer,recvLen);

         endPos=recvData.find("/Hermes>");
         if (endPos!=std::string::npos)
         {
            size_t startPos;

            startPos=recvData.find("<Hermes");
            if (startPos==std::string::npos)
            {
               assert(0);
               continue;
            }
#ifndef ENV_USE_PUGIXML
            wxMemoryInputStream *memInStream=new wxMemoryInputStream(recvData.data(),endPos+8);
            if (memInStream)
#else
            wxXmlDocument xmlDoc;
            pugi::xml_parse_result result=xmlDoc.load_buffer(recvData.data(),endPos+8);
            if (result.status==pugi::status_ok)
#endif
            {
#ifndef ENV_USE_PUGIXML
               wxXmlDocument xmlDoc(*memInStream);
               wxXmlNode *rootNode=xmlDoc.GetRoot();
               if ((rootNode) && (rootNode->GetName().CmpNoCase(_T("hermes"))==0))
#else
               wxXmlNode rootNode=xmlDoc.child("Hermes");

               if (!rootNode.empty())
#endif
               {
#ifndef ENV_USE_PUGIXML
                  wxString   val;
                  wxXmlNode *childNode=rootNode->GetChildren();

                  if (childNode)
#endif
                  {
#ifndef ENV_USE_PUGIXML
                     if (childNode->GetName().Cmp(_T(TYPE_SET_CONFIGURATION))==0)
#else
                     if (!rootNode.child(TYPE_SET_CONFIGURATION).empty())
#endif
                     {
                        wxString MachineId;
                        // apply new configuration here

                        uilog(instData,"Hermes cmd from config: %s",TYPE_SET_CONFIGURATION);
#ifndef ENV_USE_PUGIXML
 #if wxCHECK_VERSION(3,0,0)
                        childNode->GetAttribute(_T("MachineId"),&MachineId);
 #else
                        childNode->GetPropVal(_T("MachineId"),  &MachineId);
 #endif
#else
                        wxXmlNode childNode = rootNode.child(TYPE_SET_CONFIGURATION);

                        pugi::xml_attribute attr=childNode.attribute("MachineId");
                        if (!attr.empty())
                         MachineId=std::string(attr.as_string());
#endif
                        oapc_thread_mutex_lock(instData->m_parameterMutex);
                        if (MachineId.compare(*instData->m_machineID)!=0)
                        {
                           oapc_thread_mutex_unlock(instData->m_parameterMutex);
                           sendNotification(instData,s,4,2,_T("Machine identifier does not match"));
                        }
                        else
                        {
                           oapc_thread_mutex_unlock(instData->m_parameterMutex);
#ifndef ENV_USE_PUGIXML
                           wxXmlNode *nextNode;

                           nextNode=childNode->GetNext();
                           while (nextNode)
                           {
                              if (nextNode->GetName().Cmp(_T("UpstreamConfigurations"))==0)
                              {
 #if wxCHECK_VERSION(3,0,0)
                                 childNode->GetAttribute(_T("HostAddress"),&val); oapc_unicode_stringToCharUTF16BE(val,instData->m_prevMachineIP,MAX_NAME_LENGTH);
                                 childNode->GetAttribute(_T("Port"),       &val); val.ToLong(&instData->m_prevMachinePort,10);
 #else
                                 childNode->GetPropVal(_T("HostAddress"),  &val); oapc_unicode_stringToCharUTF16BE(val,instData->m_prevMachineIP,MAX_NAME_LENGTH);
                                 childNode->GetPropVal(_T("Port"),         &val); val.ToLong(&instData->m_prevMachinePort,10);
 #endif
                              }
                              else if (nextNode->GetName().Cmp(_T("DownstreamConfigurations"))==0)
                              {
 #if wxCHECK_VERSION(3,0,0)
                                 childNode->GetAttribute(_T("Port"),       &val); val.ToLong(&instData->m_nextMachinePort,10);
 #else
                                 childNode->GetPropVal(_T("Port"),         &val); val.ToLong(&instData->m_nextMachinePort,10);
 #endif
                              }

                              nextNode=childNode->GetNext();
                           }
#else // ENV_USE_PUGIXML
                           wxXmlNode nextNode=childNode.child("UpstreamConfigurations");
                           if (!nextNode.empty())
                           {
                              pugi::xml_attribute attr=nextNode.attribute("HostAddress");
                              if (!attr.empty())
                               strncpy(instData->m_prevMachineIP,attr.as_string(),MAX_NAME_LENGTH);

                              attr=nextNode.attribute("Port");
                              if (!attr.empty())
                               instData->m_prevMachinePort=attr.as_int();
                           }
                           nextNode=childNode.child("DownstreamConfigurations");
                           if (!nextNode.empty())
                           {
                              pugi::xml_attribute attr=nextNode.attribute("Port");
                              if (!attr.empty())
                               instData->m_nextMachinePort=attr.as_int();
                           }
#endif // ENV_USE_PUGIXML
                           closeMachineConnections(instData,false);
                           if (openMachineConnections(instData)!=OAPC_OK)
                           {
                              sendNotification(instData,s,4,1,_T("New connection could not be applied"));
                           }
                        }
                     }
#ifndef ENV_USE_PUGIXML
                     else if (childNode->GetName().Cmp(_T(TYPE_GET_CONFIGURATION))==0)
#else
                     if (!rootNode.child(TYPE_GET_CONFIGURATION).empty())
#endif
                     {
                        // send current configuration
                        wxXmlDocument xmlDoc;
#ifndef ENV_USE_PUGIXML
                        wxXmlNode    *root=createHermesRootNode(instData,xmlDoc);
                        wxXmlNode    *subNode=new wxXmlNode(root,wxXML_ELEMENT_NODE,_T(TYPE_CURRENT_CONFIGURATION));
                        wxXmlNode    *upCfgNode=new wxXmlNode(subNode,wxXML_ELEMENT_NODE,_T("UpstreamConfigurations"));
                        wxXmlNode    *downCfgNode=new wxXmlNode(subNode,wxXML_ELEMENT_NODE,_T("DownstreamConfigurations"));
                        wxString      val;

                        uilog(instData,"Hermes cmd from config: %s",TYPE_GET_CONFIGURATION);
                        oapc_thread_mutex_lock(instData->m_parameterMutex);
                        subNode->AddAttribute(_T("MachineId"),*instData->m_machineID);
                        oapc_thread_mutex_unlock(instData->m_parameterMutex);

                        upCfgNode->AddAttribute(_T("UpstreamLaneId"),_T("1"));
                        oapc_unicode_charToStringUTF16BE(instData->m_prevMachineIP,MAX_NAME_LENGTH,&val);
                        upCfgNode->AddAttribute(_T("HostAddress"),val);
                        val=wxString::Format(_T("%d"), instData->m_prevMachinePort);
                        upCfgNode->AddAttribute(_T("Port"),val);

                        downCfgNode->AddAttribute(_T("DownstreamLaneId"),_T("1"));
                        val=wxString::Format(_T("%d"), instData->m_nextMachinePort);
                        downCfgNode->AddAttribute(_T("Port"),val);
#else
                        wxXmlNode    root=createHermesRootNode(instData,xmlDoc);
                        wxXmlNode    subNode=root.append_child(TYPE_CURRENT_CONFIGURATION);
                        wxXmlNode    upCfgNode=subNode.append_child("UpstreamConfigurations");
                        wxXmlNode    downCfgNode=subNode.append_child("DownstreamConfigurations");
                        char         val[MAX_NAME_LENGTH+1];

                        oapc_thread_mutex_lock(instData->m_parameterMutex);
                        subNode.append_attribute("MachineId")=instData->m_machineID->c_str();
                        oapc_thread_mutex_unlock(instData->m_parameterMutex);

                        upCfgNode.append_attribute("UpstreamLaneId")="1";
                        strncpy(val,instData->m_prevMachineIP,MAX_NAME_LENGTH);
#ifndef ENV_USE_PUGIXML
                        oapc_unicode_utf16BEToASCII(val,MAX_NAME_LENGTH);
#endif
                        upCfgNode.append_attribute("HostAddress")=val;
                        snprintf(val,MAX_NAME_LENGTH,"%d",instData->m_prevMachinePort);
                        upCfgNode.append_attribute("Port")=val;

                        downCfgNode.append_attribute("DownstreamLaneId")="1";
                        snprintf(val,MAX_NAME_LENGTH,"%d",instData->m_nextMachinePort);
                        downCfgNode.append_attribute("Port")=val;
#endif
                        uilog(instData,"Hermes cmd from config: %s",TYPE_GET_CONFIGURATION);
                        sendMessage(instData,s,&xmlDoc);
                        break;
                     }
#ifndef ENV_USE_PUGIXML
                     else if (childNode->GetName().Cmp(_T(TYPE_NOTIFICATION))==0)
#else
                     if (!rootNode.child(TYPE_NOTIFICATION).empty())
#endif
                     {
                        long     notificationCode,severity;
                        wxString description;

                        uilog(instData,"Hermes cmd from config: %s",TYPE_NOTIFICATION);
#ifndef ENV_USE_PUGIXML
                        decodeNotification(instData,childNode,&notificationCode,&severity,&description,_T("Configuration supervisor"));
#else
                        wxXmlNode childNode=rootNode.child(TYPE_NOTIFICATION);
                        decodeNotification(instData,&childNode,&notificationCode,&severity,&description,_T("Configuration supervisor"));
#endif
                        switch (notificationCode)
                        {
                           case 1: // protocol error
                              assert(0);
                              break;
                           case 2: // Connection refused because of an established connection
                           case 3: // Connection reset because of changed configuration
                           case 5: // Machine shutdown
                              oapc_tcp_closesocket(s);
                              s=0;
                              break;
                           case 4: // Configuration error
                              assert(0);
                              break;
                           default:
                              assert(0);
                        }
                     }
#ifndef ENV_USE_PUGIXML
                     else if (childNode->GetName().Cmp(_T(TYPE_CHECK_ALIVE))==0)
#else
                     if (!rootNode.child(TYPE_CHECK_ALIVE).empty())
#endif
                     {
                         // no need to react on this
                     }
                     else assert(0);
                  }
#ifndef ENV_USE_PUGIXML
                  else assert(0);
#endif
               }
               else assert(0);
#ifndef ENV_USE_PUGIXML
               delete memInStream;
#endif
            }
            else assert(0);
            recvData.erase(0,endPos+8);
            while ((recvData.length()>0) && ((recvData.substr(0,1)=="\r") || (recvData.substr(0,1)=="\n")))
             recvData.erase(0,1); 
            if (s==0)
             break;
         }
      }
   }
   if (s) // send shutdown notification
   {
      sendShutdown(instData,s);
   }
   instData->m_toConfigLoopRunning=true;
}


static void *toConfigLoop(void *data)
{
   int           c;
   unsigned long ip;
   struct hermes_inst_data *instData;

   instData=(struct hermes_inst_data *)data;
   instData->m_toConfigSSock=oapc_tcp_listen_on_port(1248,"0.0.0.0");
   if (instData->m_toConfigSSock<=0) return NULL;
   oapc_tcp_set_blocking(instData->m_toConfigSSock,true);

   while (instData->m_toConfigLoopRunning)
   {
      oapc_thread_sleep(0);

      c=oapc_tcp_accept_connection(instData->m_toConfigSSock,&ip);
      if (c>0)
      {
         handleConfigConnection(c,data);
         oapc_tcp_closesocket(c);
         c=-1;
      }
   }
   instData->m_toConfigLoopRunning=true;
   return NULL;
}


static void setMachineIdentifier(struct hermes_inst_data *instance,const wxString *machineIdentifier)
{
   oapc_thread_mutex_lock(instance->m_parameterMutex);
   *instance->m_machineID=*machineIdentifier;
   oapc_thread_mutex_unlock(instance->m_parameterMutex);
}


/**
Create a new Hermes communication instance, the returned instance resource has to be used with all following
sf_hermes_-function calls and needs to be released by calling sf_hermes_delete_instance() at the end
@param machineIdentifier: unique identifier for local machine, this value has to be given in UTF-16 format
@return instance identifier to be used for one specific Hermes instance
*/
SF_EXT_API void *sf_hermes_wx_create_instance(const wxString *machineIdentifier,const enum hermes_level level)
{
   struct hermes_inst_data *instData;

   instData=(struct hermes_inst_data*)malloc(sizeof(struct hermes_inst_data));
   if (!instData) return NULL;
   memset(instData,0,sizeof(struct hermes_inst_data));

   instData->m_parameterMutex=oapc_thread_mutex_create();
   instData->m_machineID=new wxString();
   instData->m_productID=new wxString();
   instData->m_currBoard=new board_data();
   instData->m_nextBoard=new board_data();
   if ((!instData->m_parameterMutex) || 
	   (!instData->m_machineID) ||
	   (!instData->m_productID) ||
	   (!instData->m_currBoard) ||
	   (!instData->m_nextBoard))
   {
	  if (instData->m_parameterMutex) oapc_thread_mutex_release(instData->m_parameterMutex);
	  if (instData->m_machineID) delete instData->m_machineID;
	  if (instData->m_productID) delete instData->m_productID;
	  if (instData->m_currBoard) delete instData->m_currBoard;
	  if (instData->m_nextBoard) delete instData->m_nextBoard;
	  free(instData);
	  return NULL;
   }
   instData->m_level=level;
   instData->m_toNextSSock=0;
   instData->m_toNextCSock=0;
   instData->m_toConfigSSock=0;
   instData->m_prevState=ePREV_OFFLINE;
   instData->m_prevCommand=ePREV_CMD_NONE;
   instData->m_nextState=eNEXT_OFFLINE;
   instData->m_nextCommand=eNEXT_CMD_NONE;
   setMachineIdentifier(instData,machineIdentifier);
   instData->m_prev_state_callback=NULL; instData->m_next_state_callback=NULL; instData->m_notification_callback=NULL; instData->m_log_callback=NULL;
   instData->m_prev_state_data=NULL;     instData->m_next_state_data=NULL;     instData->m_notification_data=NULL;     instData->m_log_data=NULL;
   instData->m_acceptLoopRunning=false;
   instData->m_toPrevLoopRunning=false;
   instData->m_toNextLoopRunning=false;
   instData->m_toConfigLoopRunning=false;
   instData->m_isShutdown=true;
   srand((unsigned int)time(NULL));
   return instData;
}


SF_EXT_API void *sf_hermes_create_instance(const char *machineIdentifier,const enum hermes_level level)
{
#ifndef ENV_USE_PUGIXML
   wxString mid;

   oapc_unicode_charToStringASCII(machineIdentifier,strlen(machineIdentifier),&mid);
#else
   std::string mid(machineIdentifier);
#endif
   return sf_hermes_wx_create_instance(&mid,level);
}


/**
Open new Hermes connections according to the given parameters
@param[in] prevMachinePort port number for the client socket connection to the previous machine
@param[in] prevMachineIP IP in style xxx.xxx.xxx.xxx for the client socket connection to the previous machine
@param[in] nextMachinePort port number of the server socket the next machine has to connect with, here the IP is
           always 0.0.0.0, means all incoming connections are accepted
@param[in] remoteConfigEnabled when this parameter is set to 1 the function to change the local configuration is
           enabled: the related server socket for incoming connection is created and modifications of the local
		   Hermes parameters are accepted
@return OAPC_OK or an error code in case this function failed
*/
SF_EXT_API int sf_hermes_open_connections(void *instance,const unsigned short prevMachinePort,const char* prevMachineIP,
	                                                     const unsigned short nextMachinePort,
     	                                                 const char remoteConfigEnabled)
{
   int               ret;
   struct hermes_inst_data *instData;

   if (!instance) return OAPC_ERROR_INVALID_INPUT;
   instData=(struct hermes_inst_data*)instance;
   strncpy(instData->m_prevMachineIP,prevMachineIP,MAX_NAME_LENGTH);
   instData->m_prevMachinePort=prevMachinePort;
   instData->m_nextMachinePort=nextMachinePort;

   ret=openMachineConnections(instData);
   if (ret!=OAPC_OK) return ret;

   if (remoteConfigEnabled)
   {
      instData->m_toConfigLoopRunning=true;
      if (!oapc_thread_create(toConfigLoop,instance))
       return OAPC_ERROR_NO_MEMORY;
   }
   return OAPC_OK;
}


/**
Removes an existing Hermes instance, closes all related connections and releases all related
resources; after this functions has been called the instance handle is invalud and can't be used
for any further calls to sf_hermes_-functions
@param[in] instance the instance resource created with sf_hermes_wx_create_instance()
@return OAPC_OK or an error code in case this function failed
*/
SF_EXT_API int sf_hermes_delete_instance(void* instance)
{
   int i;

   struct hermes_inst_data *instData;

   if (!instance) return OAPC_ERROR_INVALID_INPUT;
   instData=(struct hermes_inst_data*)instance;
   if (instData->m_toPrevLoopRunning)
   {
      instData->m_toConfigLoopRunning=false;
      closeMachineConnections(instData,true);
      oapc_tcp_closesocket(instData->m_toConfigSSock);
      for (i=0; i<50; i++)
      {
         if (instData->m_toConfigLoopRunning) break;
         oapc_thread_sleep(100);
      }
   }
   if (instData->m_parameterMutex) oapc_thread_mutex_release(instData->m_parameterMutex);
   if (instData->m_machineID)      delete instData->m_machineID;
   if (instData->m_productID)      delete instData->m_productID;
   if (instData->m_currBoard)      delete instData->m_currBoard;
   if (instData->m_nextBoard)      delete instData->m_nextBoard;
   free(instance);
   return OAPC_OK;
}


/**
Signal the next machine a board is available
@param[in] instance the instance resource created with sf_hermes_wx_create_instance()
@return OAPC_OK or an error code in case this function failed
*/
SF_EXT_API int sf_hermes_set_next_BoardAvailable(void* instance)
{
   struct hermes_inst_data *instData;

   instData=(struct hermes_inst_data*)instance;
   instData->m_nextCommand=eNEXT_CMD_BOARD_AVAILABLE;
   return OAPC_OK;
}


/**
Signal the next machine a board is no longer available
@param[in] instance the instance resource created with sf_hermes_wx_create_instance()
@return OAPC_OK or an error code in case this function failed
*/
SF_EXT_API int sf_hermes_set_next_RevokeBoardAvailable(void* instance)
{
   struct hermes_inst_data *instData;

   instData=(struct hermes_inst_data*)instance;
   instData->m_nextCommand=eNEXT_CMD_REVOKE_BOARD_AVAILABLE;
   return OAPC_OK;
}


/**
Signal the next machine a transport was finished
@param[in] instance the instance resource created with sf_hermes_wx_create_instance()
@return OAPC_OK or an error code in case this function failed
*/
SF_EXT_API int sf_hermes_set_next_TransportFinished(void* instance)
{
   struct hermes_inst_data *instData;

   instData=(struct hermes_inst_data*)instance;
   instData->m_nextCommand=eNEXT_CMD_TRANSPORT_FINISHED;
   return OAPC_OK;
}


/**
Signal the previous machine to start transport
@param[in] instance the instance resource created with sf_hermes_wx_create_instance()
@return OAPC_OK or an error code in case this function failed
*/
SF_EXT_API int sf_hermes_set_prev_StartTransport(void* instance)
{
   struct hermes_inst_data *instData;

   instData=(struct hermes_inst_data*)instance;
   instData->m_prevCommand=ePREV_CMD_START_TRANSPORT;
   return OAPC_OK;
}


/**
Signal the previous machine to stop transport
@param[in] instance the instance resource created with sf_hermes_wx_create_instance()
@return OAPC_OK or an error code in case this function failed
*/
SF_EXT_API int sf_hermes_set_prev_StopTransport(void* instance)
{
   struct hermes_inst_data *instData;

   instData=(struct hermes_inst_data*)instance;
   *instData->m_currBoard=*instData->m_nextBoard;
   instData->m_prevCommand=ePREV_CMD_STOP_TRANSPORT;
   return OAPC_OK;
}


/**
Get information from the board which currently has arrived; this function has to be called
AFTER sf_hermes_set_prev_StopTransport(), elsewhere the returned data are undefined
*/
SF_EXT_API int sf_hermes_wx_get_curr_board_info(void* instance,wxString *BoardId,wxString *ProductTypeId)
{
   struct hermes_inst_data *instData;

   instData=(struct hermes_inst_data*)instance;
   *BoardId=instData->m_currBoard->BoardId;
   *ProductTypeId=instData->m_currBoard->ProductTypeId;
   return OAPC_OK;
}


/**
Signal the previous machine that the local machine is ready to accept a board
@param[in] instance the instance resource created with sf_hermes_wx_create_instance()
@return OAPC_OK or an error code in case this function failed
*/
SF_EXT_API int sf_hermes_set_prev_MachineReady(void* instance)
{
   struct hermes_inst_data *instData;

   instData=(struct hermes_inst_data*)instance;
   instData->m_prevCommand=ePREV_CMD_SET_MACHINE_READY;
   return OAPC_OK;
}


/**
Signal the previous machine that the local machine is no longer ready to accept a board
@param[in] instance the instance resource created with sf_hermes_wx_create_instance()
@return OAPC_OK or an error code in case this function failed
*/
SF_EXT_API int sf_hermes_set_prev_RevokeMachineReady(void* instance)
{
   struct hermes_inst_data *instData;

   instData=(struct hermes_inst_data*)instance;
   instData->m_prevCommand=ePREV_CMD_REVOKE_MACHINE_READY;
   return OAPC_OK;
}


/**
Set a new product identifier, this value is used only in case no previous machine exists which
already provides product identification data in the provided board structure
@param[in] instance the instance resource created with sf_hermes_wx_create_instance()
@return OAPC_OK or an error code in case this function failed
*/
SF_EXT_API int sf_hermes_wx_set_product_identifier(void* instance,const wxString *productIdentifier)
{
   struct hermes_inst_data *instData;

   instData=(struct hermes_inst_data*)instance;
   oapc_thread_mutex_lock(instData->m_parameterMutex);
   *instData->m_productID=*productIdentifier;
   oapc_thread_mutex_unlock(instData->m_parameterMutex);
   return OAPC_OK;
}


/**
Set an identifier for the local machine which is used in several data strucutres to uniquely specify
this local machine
@param[in] instance the instance resource created with sf_hermes_wx_create_instance()
@return OAPC_OK or an error code in case this function failed
*/
SF_EXT_API int sf_hermes_wx_set_machine_identifier(void* instance,const wxString *machineIdentifier)
{
   struct hermes_inst_data *instData;

   instData=(struct hermes_inst_data*)instance;
   oapc_thread_mutex_lock(instData->m_parameterMutex);
   *instData->m_machineID=*machineIdentifier;
   oapc_thread_mutex_unlock(instData->m_parameterMutex);
   return OAPC_OK;
}


/**
Specify a callback function which is used whenever the state of the previous machine changes
@param[in] instance the instance resource created with sf_hermes_wx_create_instance()
@return OAPC_OK or an error code in case this function failed
*/
SF_EXT_API int sf_hermes_set_prev_state_callback(void* instance,const hermes_prev_state_callback fct,void *data)
{
   struct hermes_inst_data *instData;

   instData=(struct hermes_inst_data*)instance;
   oapc_thread_mutex_lock(instData->m_parameterMutex);
   instData->m_prev_state_callback=fct;
   instData->m_prev_state_data=data;
   oapc_thread_mutex_unlock(instData->m_parameterMutex);
   return OAPC_OK;
}


/**
Specify a callback function which is used whenever the state of the next machine changes
@param[in] instance the instance resource created with sf_hermes_wx_create_instance()
@return OAPC_OK or an error code in case this function failed
*/
SF_EXT_API int sf_hermes_set_next_state_callback(void* instance,const hermes_next_state_callback fct, void *data)
{
   struct hermes_inst_data *instData;

   instData=(struct hermes_inst_data*)instance;
   oapc_thread_mutex_lock(instData->m_parameterMutex);
   instData->m_next_state_callback=fct;
   instData->m_next_state_data=data;
   oapc_thread_mutex_unlock(instData->m_parameterMutex);
   return OAPC_OK;
}


/**
Specify a callback function which is used whenever a notification arrives at one of the possible
connections (previous machine, next machine or configuration socket)
@param[in] instance the instance resource created with sf_hermes_wx_create_instance()
@return OAPC_OK or an error code in case this function failed
*/
SF_EXT_API int sf_hermes_wx_set_notification_callback(void* instance,const hermes_wx_notification_callback fct, void *data)
{
   struct hermes_inst_data *instData;

   instData=(struct hermes_inst_data*)instance;
   oapc_thread_mutex_lock(instData->m_parameterMutex);
   instData->m_notification_callback=fct;
   instData->m_notification_data=data;
   oapc_thread_mutex_unlock(instData->m_parameterMutex);
   return OAPC_OK;
}


/**
Specify a callback function which is used whenever an event happens which is interesting for logging purposes
@param[in] instance the instance resource created with sf_hermes_wx_create_instance()
@return OAPC_OK or an error code in case this function failed
*/
SF_EXT_API int sf_hermes_set_log_callback(void* instance,const hermes_log_callback fct,void *data)
{
   struct hermes_inst_data *instData;

   instData=(struct hermes_inst_data*)instance;
   oapc_thread_mutex_lock(instData->m_parameterMutex);
   instData->m_log_callback=fct;
   instData->m_log_data=data;
   oapc_thread_mutex_unlock(instData->m_parameterMutex);
   return OAPC_OK;
}


/**
Retrieve the current state of the previous machine
@param[in] instance the instance resource created with sf_hermes_wx_create_instance()
@return OAPC_OK or an error code in case this function failed
*/
SF_EXT_API enum hermes_prev_state sf_hermes_get_prev_state(void* instance)
{
   struct hermes_inst_data *instData;

   instData=(struct hermes_inst_data*)instance;
   return instData->m_prevState;
}


/**
Retrieve the current state of the next machine
@param[in] instance the instance resource created with sf_hermes_wx_create_instance()
@return OAPC_OK or an error code in case this function failed
*/
SF_EXT_API enum hermes_next_state sf_hermes_get_next_state(void* instance)
{
   struct hermes_inst_data *instData;

   instData=(struct hermes_inst_data*)instance;
   return instData->m_nextState;
}

