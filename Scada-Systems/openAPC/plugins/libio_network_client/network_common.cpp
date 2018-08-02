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



#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#ifndef ENV_WINDOWS
 #include <sys/socket.h>
 #include <arpa/inet.h>
 #include <unistd.h>
#else
 #include <winsock2.h>
#endif

#include "oapc_libio.h"
#include "liboapc.h"
#include "network_common.h"

static char sendbuf[MAX_SENDBUF_SIZE];



bool set_config_data(struct instData* data,const char *name,const char *value)
{
   if (strcmp(name,"ip")==0)         strncpy(data->config.ip,value,MAX_IP_SIZE);
   else if (strcmp(name,"port")==0)  data->config.port=(unsigned short)atoi(value);
   else if (strcmp(name,"incoming")==0)
   {
      data->config.incoming=(unsigned short)atoi(value);
      if (data->config.incoming>MAX_INCOMING_CONNECTIONS) data->config.incoming=MAX_INCOMING_CONNECTIONS;
   }
   else if (strcmp(name,"uname")==0) strncpy(data->config.uname,value,MAX_AUTH_SIZE);
   else if (strcmp(name,"pwd")==0)   strncpy(data->config.pwd,value,MAX_AUTH_SIZE);
   else if (strcmp(name,"mode")==0)  data->config.mode=(unsigned short)atoi(value);
   else return false;
   return true;
}



void convert_to_save_byteorder(struct libio_config *save_config,struct libio_config *current_config)
{
   save_config->version =htons(current_config->version);
   save_config->length  =htons(current_config->length);
   strncpy(save_config->ip,current_config->ip,MAX_IP_SIZE);
   save_config->port    =htons(current_config->port);
   save_config->incoming=htons(current_config->incoming);
   strncpy(save_config->uname,current_config->uname,MAX_AUTH_SIZE);
   strncpy(save_config->pwd,current_config->pwd,MAX_AUTH_SIZE);
   save_config->mode    =htons(current_config->mode);
}



void convert_from_save_byteorder(struct libio_config *current_config,struct libio_config *save_config)
{
   current_config->version =ntohs(save_config->version);
   current_config->length  =ntohs(save_config->length);
   strncpy(current_config->ip,save_config->ip,MAX_IP_SIZE);
   current_config->port    =ntohs(save_config->port);
   current_config->incoming=ntohs(save_config->incoming);
   strncpy(current_config->uname,save_config->uname,MAX_AUTH_SIZE);
   strncpy(current_config->pwd,save_config->pwd,MAX_AUTH_SIZE);
   current_config->mode    =ntohs(save_config->mode);
}



void init_instancedata(struct instData *data,unsigned short port)
{
   memset(data,0,sizeof(struct instData));
   data->config.version=1;
   data->config.length=sizeof(struct libio_config);
   strcpy(data->config.ip,"192.168.1.1");
   data->config.port=port;
   data->config.uname[0]=0;
   data->config.pwd[0]=0;
   data->config.mode=1;
   data->client[0].sock=-1;
   data->sock=-1;
   data->config.incoming=3;
   memset(&data->cmdValuePair,0,sizeof(struct cmd_value_pair));
   memset(&data->client,0,sizeof(struct client_dataset));
   memset(&data->cust,0,sizeof(struct cust_instance_data));
}



/**
This function checks if there are new incoming clients. If the number of the client does not
exceed the limit it is accepted, elsewhere an error message is sent and the connection is
closed. Additionally the successfully connected clients are polled for new data.
this function is not part of the external library interface.
*/
int check_clients(struct instData *data,struct client_dataset *client,bool checkAuth)
{
   char              *id,*cr;

   cr=strstr(client->buffer,"\n"); // TODO: implement resynchronization mechanism in case a reception error occured for binary data
   if ((!cr) || (client->loaded==0))
   {
      if (oapc_tcp_recv(client->sock,((char*)&client->buffer)+client->loaded,sizeof(client->buffer)-client->loaded,"\n",50)<=0)
       return OAPC_ERROR_CONNECTION;
   }
   if (!cr) cr=strstr(client->buffer,"\n");
   if (cr)
   {
      // for plug-ins supporting server functionality
      if ((checkAuth) && (client->mode==0)) // no mode information detected, waiting for MODE tag
      {
         *cr=0;
         id=strstr(client->buffer,"UNAME ");
         if (id) 
         {
            strncpy(client->uname,id+6,MAX_AUTH_SIZE);
            client->loaded=0;
            client->buffer[0]=0;
         }
         else
         {
            id=strstr(client->buffer,"PWD ");
            if (id) 
            {
               strncpy(client->uname,id+4,MAX_AUTH_SIZE);
               client->loaded=0;
               if ((strncmp(client->pwd,data->config.pwd,MAX_AUTH_SIZE)) || (strncmp(client->uname,data->config.uname,MAX_AUTH_SIZE)))
               {
                  oapc_tcp_send(client->sock,"FAIL authentication error\n",28,100);
                  oapc_tcp_closesocket(client->sock);
                  client->sock=-1;

                  return OAPC_ERROR_AUTHENTICATION;
               }
            }
            else
            {
               id=strstr(client->buffer,"MODE ");
               if (id) 
               {
                  client->mode=(unsigned short)atoi(id+5);
                  client->loaded=0;
                  client->buffer[0]=0;
               }
            }
         }
      }
      // end of server plug in functions
      else if ((client->mode==2) || (client->mode==1)) // plain data mode
      {
         if (!client->val_num_set)
         {
            id=strstr(client->buffer,"NUM ");
            if (id==client->buffer)
            {
               client->val_num=(float)oapc_util_atof(id+4);
               client->val_num_set=1;
               client->loaded=0;
               client->buffer[0]=0;
            }
         }
         if (!client->val_digi_set)
         {
            id=strstr(client->buffer,"DIGI ");
            if (id==client->buffer)
            {
               if (atoi(id+5)!=0) client->val_digi=1;
               else client->val_digi=0;
               client->val_digi_set=1;
               client->loaded=0;
               client->buffer[0]=0;
            }
         }
         if (!client->val_char_set)
         {
            id=strstr(client->buffer,"CHAR ");
            if (id==client->buffer)
            {
               *cr=0;
               if (strlen(id)>5) strncpy(client->val_char,id+5,MAX_CMDVAL_SIZE);
               else client->val_char[0]=0;
               client->val_char_set=1;
               client->loaded=0;
               client->buffer[0]=0;
            }
         }
         if (!client->val_bin_set)
         {
            id=strstr(client->buffer,"BIN");
            if (id==client->buffer)
            {
               struct oapc_bin_head binHead;

               if (oapc_tcp_recv(data->client[0].sock,(char*)&binHead,sizeof(struct oapc_bin_head)-1,NULL,MAX_RECV_TIMEOUT)<(int)sizeof(struct oapc_bin_head)-1)
               {
                  client->lastBinError=OAPC_ERROR_RECV_DATA;
                  return OAPC_OK;
               }
               convert_bin_to_host_byteorder(&binHead,&binHead);
               if (binHead.sizeData<=0)
               {
                  client->lastBinError=OAPC_ERROR_RECV_DATA;
                  return OAPC_OK;
               }               
               client->val_bin=(struct oapc_bin_head*)malloc(sizeof(struct oapc_bin_head)+binHead.sizeData);
               if (!client->val_bin)
               {
                  client->lastBinError=OAPC_ERROR_NO_MEMORY;
                  return OAPC_OK;
               }               
               memcpy(client->val_bin,&binHead,sizeof(struct oapc_bin_head)-1);
               if (oapc_tcp_recv(data->client[0].sock,(char*)&client->val_bin->data,binHead.sizeData,NULL,MAX_RECV_TIMEOUT)<binHead.sizeData)
               {
                  free(client->val_bin);
                  client->val_bin=NULL;
                  client->lastBinError=OAPC_ERROR_RECV_DATA;
                  return OAPC_OK;
               }
               client->val_bin_set=1;
               client->loaded=0;
               client->buffer[0]=0;
            }
         }
         if (!client->val_cmd_set)
         {
            id=strstr(client->buffer,"CMD ");
            if (id==client->buffer)
            {
               *cr=0;
               if (strlen(id)>4) strncpy(client->val_cmd,id+4,MAX_CMDVAL_SIZE);
               else client->val_cmd[0]=0;
               client->val_cmd_set=1;
               client->loaded=0;
               client->buffer[0]=0;
            }
         }
      }
      else if ((client->mode==3) && (client->val_cmd_set==0))
      {
         *cr=0;
         strncpy(client->val_cmd,client->buffer,MAX_CMDVAL_SIZE);
         client->val_cmd_set=1;
         client->loaded=0;
         client->buffer[0]=0;
      }
   }
   return OAPC_OK;
}



/**
This function is used in command/value mode and checks if already a complete pair of command and value was received.
If yes both values are transmitted via network and the used structure cmdValuePair is initialized in order to collect the
next pair of command and value.
This function is not part of the librarys external interface.
*/
void check_cmd_value_pair(struct instData *data)
{
   if ((data->cmdValuePair.cmd_set) && ((data->cmdValuePair.val_char_set) || 
       (data->cmdValuePair.val_num_set) || (data->cmdValuePair.val_digi_set)))
   {
      sprintf(sendbuf,"CMD %s\n",data->cmdValuePair.cmd);
      oapc_tcp_send(data->client[0].sock,sendbuf,(int)strlen(sendbuf),MAX_SEND_TIMEOUT);
      if (data->cmdValuePair.val_char_set)
      {
         sprintf(sendbuf,"CHAR %s\n",data->cmdValuePair.val_char);
         data->cmdValuePair.val_char_set=0;
      }
      else if (data->cmdValuePair.val_num_set)
      {
         sprintf(sendbuf,"NUM %f\n",data->cmdValuePair.val_num);
         data->cmdValuePair.val_num_set=0;
      }
      else if (data->cmdValuePair.val_digi_set)
      {
         sprintf(sendbuf,"DIGI %d\n",data->cmdValuePair.val_digi);
         data->cmdValuePair.val_digi_set=0;
      }
      else if (data->cmdValuePair.val_bin_set)
      {
         oapc_tcp_send(data->client[0].sock,sendbuf,(int)strlen(sendbuf),MAX_SEND_TIMEOUT);
         sprintf(sendbuf,"BIN\n");
      }
      data->cmdValuePair.cmd_set=0;
      oapc_tcp_send(data->client[0].sock,sendbuf,(int)strlen(sendbuf),MAX_SEND_TIMEOUT);
      if (data->cmdValuePair.val_bin_set)
      {
         convert_bin_to_network_byteorder(data->client[0].val_bin,data->client[0].val_bin);
         oapc_tcp_send(data->client[0].sock,(char*)data->client[0].val_bin,sizeof(struct oapc_bin_head)-1,MAX_SEND_TIMEOUT);
         oapc_tcp_send(data->client[0].sock,(char*)&data->client[0].val_bin->data,data->client[0].val_bin->sizeData,MAX_SEND_TIMEOUT*100);
         free(data->client[0].val_bin);
         data->cmdValuePair.val_bin_set=0;
         data->client[0].val_bin=NULL;
      }
   }
}



void convert_bin_to_network_byteorder(struct oapc_bin_head *net,struct oapc_bin_head *host)
{
   net->version     =ntohl(host->version);
   net->unitExponent=ntohs(host->unitExponent);
   net->int1        =ntohs(host->int1);
   net->param1      =ntohl(host->param1);
   net->param2      =ntohl(host->param2);
   net->param3      =ntohl(host->param3);
   net->sizeData    =ntohl(host->sizeData);
   net->type        =host->type;
   net->subType     =host->subType;
   net->compression =host->compression;
   net->unit        =host->unit;
}



void convert_bin_to_host_byteorder(struct oapc_bin_head *host,struct oapc_bin_head *net)
{
   host->version     =htonl(net->version);
   host->unitExponent=htons(net->unitExponent);
   host->int1        =htons(net->int1);
   host->param1      =htonl(net->param1);
   host->param2      =htonl(net->param2);
   host->param3      =htonl(net->param3);
   host->sizeData    =htonl(net->sizeData);
   host->type        =net->type;
   host->subType     =net->subType;
   host->compression =net->compression;
   host->unit        =net->unit;
}
