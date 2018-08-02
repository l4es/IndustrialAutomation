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

#ifdef ENV_WINDOWS
 #define snprintf _snprintf
#endif

#include <string.h>

#include "liboapc.h"
#include "libsmartfactory.h"
#include "mqtt/MQTTPacket/src/MQTTPacket.h"
#include "mqtt/MQTTPacket/src/MQTTPublish.h"

#define MAX_BUFLEN 500

struct mqtt_inst_data
{
   int                                       m_sock;
   wxString                                 *m_machineID;
   unsigned short                            port;
   char                                      IP[200+1];
};



SF_EXT_API void *sf_mqtt_wx_create_instance(const wxString *machineIdentifier)
{
   struct mqtt_inst_data *instData;

   instData=(struct mqtt_inst_data*)malloc(sizeof(struct mqtt_inst_data));
   if (!instData) return NULL;
   memset(instData,0,sizeof(struct mqtt_inst_data));

   instData->m_machineID=new wxString();
   if (!instData->m_machineID)
   {
      sf_if_delete_instance(instData);
      return NULL;
   }
   *instData->m_machineID=*machineIdentifier;

   return instData;
}



SF_EXT_API void *sf_mqtt_create_instance(const char *machineIdentifier)
{
#ifndef ENV_USE_PUGIXML
   wxString mid;

   oapc_unicode_charToStringASCII(machineIdentifier,strlen(machineIdentifier),&mid);
#else
   std::string mid(machineIdentifier);
#endif
   return sf_if_wx_create_instance(&mid);
}


static int open_connection(struct mqtt_inst_data *instData)
{
   instData->m_sock=oapc_tcp_connect_to(instData->IP,instData->port);
   if (instData->m_sock<=0) return OAPC_ERROR_CONNECTION;
   oapc_tcp_set_blocking(instData->m_sock,0);
   return OAPC_OK;
}


SF_EXT_API_DEF SF_EXT_API int   sf_mqtt_open_connection(void* instance,const unsigned short port,const char* IP)
{
   struct mqtt_inst_data   *instData;
   int                      rc;

   if (!instance) return OAPC_ERROR_INVALID_INPUT;
   instData=(struct mqtt_inst_data*)instance;
   instData->port=port;
   strncpy(instData->IP,IP,200);
   rc=open_connection(instData);
   if (rc!=OAPC_OK) return rc;
   // just try if the connection really exists in order to early throw an error
   oapc_tcp_closesocket(instData->m_sock);
   return OAPC_OK;
}


SF_EXT_API int sf_mqtt_wx_publish(void* instance,const wxString *topicData,const wxString *payData)
{
   struct mqtt_inst_data   *instData;
   MQTTPacket_connectData   data=MQTTPacket_connectData_initializer;
   int                      rc=0,payloadlen,len;
   unsigned char            buf[MAX_BUFLEN+1];
   MQTTString               topic=MQTTString_initializer;
   char                     payload[200+1];
#ifndef ENV_USE_PUGIXML
   char                     id[200+1];
#endif

   if (!instance) return OAPC_ERROR_INVALID_INPUT;
   instData=(struct mqtt_inst_data*)instance;
   rc=open_connection(instData);
   if (rc!=OAPC_OK) return rc;

#ifndef ENV_USE_PUGIXML
   oapc_unicode_stringToCharASCII(*instData->m_machineID,id,200);
   data.clientID.cstring=id;
#else
   data.clientID.cstring=(char*)instData->m_machineID->c_str();
#endif
//   data.keepAliveInterval = 20;
   data.cleansession=1;
   len = MQTTSerialize_connect(buf,MAX_BUFLEN,&data); /* 1 */

#ifndef ENV_USE_PUGIXML
   oapc_unicode_stringToCharASCII(*topicData,id,200);
   topic.cstring=id;
#else
   topic.cstring=(char*)topicData->c_str();
#endif

#ifndef ENV_USE_PUGIXML
   oapc_unicode_stringToCharASCII(*payData,payload,200);
#else
   strncpy(payload,payData->c_str(),200);
#endif
   payloadlen=strlen(payload);

   len+=MQTTSerialize_publish(buf+len,MAX_BUFLEN-len,0,3,0,0,topic,(unsigned char*)payload,payloadlen); /* 2 */

   len+=MQTTSerialize_disconnect(buf+len,MAX_BUFLEN-len); /* 3 */

   rc=oapc_tcp_send(instData->m_sock,(const char*)buf,len,200);
   oapc_tcp_closesocket(instData->m_sock);
   if (rc<len) return OAPC_ERROR_SEND_DATA;
   return OAPC_OK;
}



SF_EXT_API int sf_mqtt_publish(void* instance,const char *topicData,const char *payload)
{
#ifndef ENV_USE_PUGIXML
   wxString mid,pay;

   oapc_unicode_charToStringASCII(topicData,strlen(topicData),&mid);
   oapc_unicode_charToStringASCII(payload,strlen(pay),&mid);
#else
   std::string mid(topicData);
   std::string pay(payload);
#endif
   return sf_mqtt_wx_publish(instance,&mid,&pay);
}


SF_EXT_API_DEF SF_EXT_API int   sf_mqtt_delete_instance(void* instance)
{
   struct mqtt_inst_data *instData;

   if (!instance) return OAPC_ERROR_INVALID_INPUT;
   instData=(struct mqtt_inst_data*)instance;
   if (instData->m_machineID)      delete instData->m_machineID;
   free(instance);
   return OAPC_OK;
}

