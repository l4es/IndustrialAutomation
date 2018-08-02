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


/**
 * This file contains the functionality for accessing the Interlock Server out of own
 * applications. It provides functions that don't have to be used externally (marked as
 * internal function) and other functions that have to be used by external applications.<BR><BR>
 * 
 * The functions described here handle connecting to the server, transferring data to it,
 * receiving data from it and perform some decoding/service tasks.<BR><BR>
 * 
 * The handled data consist of a member "ios" that describes how much and which data blocks will
 * follow, here for IO types OAPC_DIGI_IOx a struct oapc_digi_value_block will follow, for IO
 * types OAPC_NUM_IOx a double will follow that is converted internally, for IO types OAPC_CHAR_IOx
 * a struct oapc_char_value_block will follow,
 * for IO types OAPC_BIN_IOx a struct oapc_bin_head (define in oapc_libio.h) will follow that
 * is converted to and from network byte order. If "ios" is equal to 0 no data are attached.<BR>
 * "cmd" uses the OAPC_CMD_xxx constants to inform what has to be done exactly.
 */


#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <math.h>

#ifdef ENV_WINDOWS
 #undef _UNICODE
 #ifdef ENV_WINDOWSCE
//  #include "Winsock2.h"
 #endif
#endif

#ifndef ENV_WINDOWS
 #include <sys/io.h>
 #include <arpa/inet.h>
 #include <errno.h>
 #include <unistd.h>
 #include <fcntl.h>
#else
/* #ifdef ENV_WINDOWSCE
  #include <winsock2.h>
 #endif*/
#endif

#ifdef ENV_LINUX
#include <termios.h>
#include <unistd.h>
#endif

#include "liboapc.h"

struct ispace_conn_info
{
	int                      m_sock;
	char                     m_ispace_recv_running,m_used;
	lib_oapc_ispace_callback m_oapc_ispace_callback;
};

#define MAX_ISPACE_CONNECTIONS 25

/*static*/ struct ispace_conn_info  m_cInfo[MAX_ISPACE_CONNECTIONS]={{-1,0,0,NULL},{-1,0,0,NULL},{-1,0,0,NULL},{-1,0,0,NULL},{-1,0,0,NULL},
                                                                 {-1,0,0,NULL},{-1,0,0,NULL},{-1,0,0,NULL},{-1,0,0,NULL},{-1,0,0,NULL},
                                                                 {-1,0,0,NULL},{-1,0,0,NULL},{-1,0,0,NULL},{-1,0,0,NULL},{-1,0,0,NULL},
                                                                 {-1,0,0,NULL},{-1,0,0,NULL},{-1,0,0,NULL},{-1,0,0,NULL},{-1,0,0,NULL},
                                                                 {-1,0,0,NULL},{-1,0,0,NULL},{-1,0,0,NULL},{-1,0,0,NULL},{-1,0,0,NULL}};



/**
 * This is an INTERNAL FUNCTION that receives a full set of data. Here first the
 * head is received from the socket and - depending on the structure member "ios" -
 * the data that may follow the head.
 * @param[out] recHead the structure where the head data have to be stored into
 *             after conversion to host byteorder
 * @param[out] values an pointer array that points to memory areas containing
 *             the received data
 * @return OAPC_OK in case all data could be received or an error code otherwise;
 *             in case of an error the socket connection is closed by this function
 *             and has to be re-established by the main application
 */
int oapc_ispace_recv_data(void *handle,struct oapc_ispace_head *recHead,void *values[MAX_NUM_IOS])
{
	int                      i,j,bitmask,ret;
	char                     recvError;
   struct ispace_conn_info *cInfo;
#ifdef ENV_WINDOWS
   int                      err;
#endif
   
   if (!handle) return OAPC_ERROR_RESOURCE;
   cInfo=(struct ispace_conn_info*)handle;
   ret=oapc_tcp_recv(cInfo->m_sock,((char*)recHead),sizeof(struct oapc_ispace_head),NULL,10000);
#ifdef _DEBUG
/*   unsigned char *c;
   pthread_t     id = pthread_self();


   c=(unsigned char*)recHead;
   for (i=0; i<(int)sizeof(struct oapc_ispace_head); i++)
   {
      if (((*c>='0') && (*c<='9')) || ((*c>='a') && (*c<='z')) || ((*c>='A') && (*c<='Z')) || (*c=='/') || (*c=='_')) printf("%c",*c);
      else printf("0x%X ",*c);
      c++;
   }
   printf("\n");

   if (ntohl(recHead->version)!=1)
   {
      show_backtrace();
      assert(0);
   }*/
#endif
   if (ret==0)
   {
#ifdef ENV_WINDOWS
      err=GetLastError();
      if (err!=WSAECONNRESET)
#else
      if (errno!=ECONNRESET)
#endif
       return OAPC_ERROR_NO_DATA_AVAILABLE;
      else return OAPC_ERROR_RECV_DATA;
   }
   else if (ret<(int)sizeof(struct oapc_ispace_head))   
   {
      oapc_ispace_disconnect(handle);
     	return OAPC_ERROR_CONNECTION;
   }      
   recHead->length  =ntohl(recHead->length);
   recHead->version =ntohl(recHead->version);
   recHead->ios     =ntohl(recHead->ios);
   recHead->cmd     =ntohs(recHead->cmd);
   recHead->reserved=ntohs(recHead->reserved);

   recvError=0;
   bitmask=0x01010101;
   for (i=0; i<MAX_NUM_IOS; i++)
   {
  		values[i]=NULL;
 	   if (recHead->ios & bitmask)
     	{
     		if (recHead->ios & bitmask & OAPC_DIGI_IO_MASK)
     		{
  	   		values[i]=malloc(sizeof(struct oapc_digi_value_block));
  		   	if (!values[i])
             recvError=1;
  			   else
     			{
               if (oapc_tcp_recv(cInfo->m_sock,((char*)values[i]),sizeof(struct oapc_digi_value_block),NULL,10000)<(int)sizeof(struct oapc_digi_value_block))
                recvError=1;
  	   		}
  		   }
         else if (recHead->ios & bitmask & OAPC_NUM_IO_MASK)
     		{
  		   	values[i]=malloc(sizeof(struct oapc_num_value_block));
  			   if (!values[i])
             recvError=1;
     			else
     			{
               ret=oapc_tcp_recv(cInfo->m_sock,((char*)values[i]),sizeof(struct oapc_num_value_block),NULL,10000);
               if (ret<(int)sizeof(struct oapc_num_value_block))
                recvError=1;
     			}
     		}
  	   	else if (recHead->ios & bitmask & OAPC_CHAR_IO_MASK)
  		   {
     			values[i]=malloc(sizeof(struct oapc_char_value_block));
     			if (!values[i])
             recvError=1;
     			else
  	   		{
               if (oapc_tcp_recv(cInfo->m_sock,((char*)values[i]),sizeof(struct oapc_char_value_block),NULL,10000)<(int)sizeof(struct oapc_char_value_block))
                recvError=1;
     			}
     		}
  	   	else if (recHead->ios & bitmask & OAPC_BIN_IO_MASK)
  		   {
  		   	struct oapc_bin_head binHead;
  		   	char                *dataPos;
  		   	
            if (oapc_tcp_recv(cInfo->m_sock,((char*)&binHead),sizeof(struct oapc_bin_head)-1,NULL,10000)<(int)sizeof(struct oapc_bin_head)-1)
             recvError=1;
            else
            {
               binHead.version     =ntohl(binHead.version); 
               binHead.unitExponent=ntohs(binHead.unitExponent);
               binHead.int1        =0; // for application-internal use only, transmission not allowed
               binHead.param1      =ntohl(binHead.param1);
               binHead.param2      =ntohl(binHead.param2);
               binHead.param3      =ntohl(binHead.param3);
               binHead.sizeData    =ntohl(binHead.sizeData);
        			values[i]=malloc(sizeof(struct oapc_bin_head)+binHead.sizeData);
      			if (!values[i])
                recvError=1;
      			else
  	      		{
  	      			memcpy(values[i],&binHead,sizeof(struct oapc_bin_head));
  	      			dataPos=&((struct oapc_bin_head*)values[i])->data;
                  if (oapc_tcp_recv(cInfo->m_sock,dataPos,binHead.sizeData,NULL,10000)<binHead.sizeData)
                   recvError=1;
  	      		}
  	   	  	}            
     		}
     		if (recvError)
         {
           	if (i>0)
        	   for (j=i-i; j>=0; j--) free(values[j]);
           	oapc_ispace_disconnect(handle);
           	return OAPC_ERROR_CONNECTION;
         }         
  	   }
      bitmask=bitmask<<1;
   }
   return OAPC_OK;
}



/**
 * This is an INTERNAL FUNCTION that handles reception of data from a socket and sends the
 * result to the callback that was set before the connection was established. When receiving
 * of data fails due to a connection problem this function exits.
 */
static void *recv_thread(void *arg)
{
   void                        *values[MAX_NUM_IOS];
   int                          i,ret;
   struct oapc_ispace_head      recHead;
   struct ispace_conn_info     *cInfo;

   cInfo=(struct ispace_conn_info*)arg;
   while (cInfo->m_ispace_recv_running)
   {
      if (cInfo->m_sock<=0) oapc_thread_sleep(100);
      else
      {
         memset(&recHead,0,sizeof(struct oapc_ispace_head));
         ret=oapc_ispace_recv_data(arg,&recHead,values);
         if (ret==OAPC_OK)
         {
            cInfo->m_oapc_ispace_callback(arg,recHead.nodeName,recHead.cmd,recHead.ios,values);
            for (i=0; i<MAX_NUM_IOS; i++)
             if (values[i]) free(values[i]); // release the received data after submission to the client
         }
         else if (ret!=OAPC_ERROR_NO_DATA_AVAILABLE)
         {
            oapc_tcp_closesocket(cInfo->m_sock);
            cInfo->m_sock=-1;
            return NULL;
         }
      }
      oapc_thread_sleep(1); // task switch
   }
   cInfo->m_ispace_recv_running=1;
   return NULL;
}



/**
 * This function has to be called first to obtain instance resources for a new interlock space
 * connection. This function is not thread safe and returns the new instance handler or
 * NULL in case there are no more available.
 */
OAPC_EXT_API void *oapc_ispace_get_instance()
{
	int i;
	
	for (i=0; i<MAX_ISPACE_CONNECTIONS; i++)
	{
		if (!m_cInfo[i].m_used)
		{
			m_cInfo[i].m_used=1;
			return &m_cInfo[i];
		}
	}
	return NULL;
}


/**
 * This function has to be called before a connection to an Interlock Server is established,
 * here a callback function has to be set that receives data sent from the server to the
 * application. ATTENTION: This function has to be called exactly once before connecting
 * to the server, it is not possible to change the callback function later!
 * @param[in] oapc_ispace_callback the callback function that will be called when data have
 *            been received
 * @return OAPC_OK when the callback function could be installed successfully or an error
 *            code otherwise
 */
int oapc_ispace_set_recv_callback(void *handle,lib_oapc_ispace_callback oapc_ispace_callback)
{
   struct ispace_conn_info *cInfo;
   
   if (!handle) return OAPC_ERROR_RESOURCE;
   cInfo=(struct ispace_conn_info*)handle;
   cInfo->m_oapc_ispace_callback=oapc_ispace_callback;
	cInfo->m_ispace_recv_running=1;
	if (!oapc_thread_create(&recv_thread,handle))
	{
   	cInfo->m_ispace_recv_running=0;
		return OAPC_ERROR_NO_MEMORY;
	}
	return OAPC_OK;
}



/**
 * This function tries to connect to an existing Interlock Server.
 * @param[in] host the host name or IP to connect with, when this parameter is
 *            NULL the library connects to the localhost
 * @param[in] port the port of the server to connect with, when this value is set
 *            to 0 the default port is used
 * @param[in] auth reserved for future use, set to NULL
 * @return OAPC_OK when the connection could be established successfully,
 *            OAPC_ERROR_RESOURCE in case no callback function was defined to
 *            receive answers from the server or OAPC_ERROR_CONNECTION when the
 *            server could not be contacted
 */
OAPC_EXT_API int oapc_ispace_connect(void *handle,const char *host,unsigned short port,struct oapc_ispace_auth *auth)
{
   struct ispace_conn_info *cInfo;
   
   if (!handle) return OAPC_ERROR_RESOURCE;
   cInfo=(struct ispace_conn_info*)handle;
   auth=auth;
   if ((!cInfo->m_oapc_ispace_callback) || (!cInfo->m_ispace_recv_running)) return OAPC_ERROR_RESOURCE;
   if (port==0) port=18100;
   if (host==NULL) cInfo->m_sock=oapc_tcp_connect_to("127.0.0.1",port);
   else cInfo->m_sock=oapc_tcp_connect_to(host,port);
   if (cInfo->m_sock<=0)
   {
      oapc_ispace_disconnect(handle);
      return OAPC_ERROR_CONNECTION;
   }
   oapc_tcp_set_blocking(cInfo->m_sock,0);
   if (auth)
   {
      struct oapc_num_value_block numValueBlock;
   		
   	oapc_util_dbl_to_block(auth->id,&numValueBlock);
      return oapc_ispace_set_value(handle,"/system/lastauth",OAPC_NUM_IO0,&numValueBlock,auth);
   }
   return OAPC_OK;
}



/**
 * This function is for INTERNAL USE ONLY, it defines a different socket to communicate
 * with a server; this socket needs to belong to an already established connection.
 * @param newSock the new socket to use for all sending operations; the callback can't
 *        be used when a socket is set this way.
 */
OAPC_EXT_API int oapc_ispace_set_socket(void *handle,int newSock)
{
   struct ispace_conn_info *cInfo;
   
   if (!handle) return OAPC_ERROR_RESOURCE;
   cInfo=(struct ispace_conn_info*)handle;
	cInfo->m_ispace_recv_running=0;
	cInfo->m_sock=newSock;
	return OAPC_OK;
}



OAPC_EXT_API int oapc_ispace_request(void *handle,const char *nodeName,unsigned short cmd,struct oapc_ispace_auth *auth)
{
	struct oapc_ispace_head  head;
   int                      rc;
   struct ispace_conn_info *cInfo;
   
   if (!handle) return OAPC_ERROR_RESOURCE;
   cInfo=(struct ispace_conn_info*)handle;   
   auth=auth;
	if (cInfo->m_sock<=0) return OAPC_ERROR_RESOURCE;
	
	head.version =ntohl(1);
	head.length  =ntohl(sizeof(struct oapc_ispace_head));
	head.ios     =ntohl(0);
	head.cmd     =ntohs(cmd);
	head.reserved=ntohs(0);
	strncpy(head.nodeName,nodeName,MAX_NODENAME_LENGTH);
	rc=oapc_tcp_send(cInfo->m_sock,(char*)&head,sizeof(struct oapc_ispace_head),1500);
	if (rc<(int)sizeof(struct oapc_ispace_head))
	{
		oapc_ispace_disconnect(handle);
		return OAPC_ERROR_CONNECTION;
	}
	return OAPC_OK;
}



/**
 * This function requests a specific data block from the server. When the requested data exists the
 * result will be received internally and forwarded to the callback function.
 * @param[in] the node name of the requested data block, the same name is provided within the
 *            callback function as soon as the related data are received. When the requested
 *            data block could not be found a specific answer is sent to the callback function, in
 *            this case no data are provided but the "cmd" parameter is set to OAPC_CMDERR_DOESNT_EXISTS
 * @param[in] auth reserved for future use, set to NULL
 * @return OAPC_OK when the request could be transmitted successfully or an error code otherwise;
 *            in case of an error the socket connection is closed by this function and has to be
 *            re-established by the main application
 */
OAPC_EXT_API int oapc_ispace_request_data(void *handle,const char *nodeName,struct oapc_ispace_auth *auth)
{
   return oapc_ispace_request(handle,nodeName,OAPC_CMD_GET_VALUE,auth);
}



/**
 * This function requests all available data blocks from the server. The result will be received
 * internally and forwarded to the callback function. The data blocks itself can be identified
 * by the node name that is handed over to the callback function.
 * @param[in] auth reserved for future use, set to NULL
 * @return OAPC_OK when the request could be transmitted successfully or an error code otherwise;
 *            in case of an error the socket connection is closed by this function and has to be
 *            re-established by the main application
 */
OAPC_EXT_API int oapc_ispace_request_all_data(void *handle,struct oapc_ispace_auth *auth)
{
   return oapc_ispace_request(handle,"",OAPC_CMD_GET_ALL_VALUES,auth);
}



/**
 * This is an INTERNAL FUNCTION, it sends a response to a connected socket that does not
 * contain any data but an error information within the "cmd" structure member.
 * @param[in] nodeName the name of the data block this response is sent for
 * @param[in] rcode a constant of type OAPC_CMDERR_xxx
 */
OAPC_EXT_API int oapc_ispace_send_response(void *handle,const char*nodeName,unsigned int rcode)
{
	struct oapc_ispace_head head;
	int                     rc;
   struct ispace_conn_info *cInfo;
   
   if (!handle) return OAPC_ERROR_RESOURCE;
   cInfo=(struct ispace_conn_info*)handle;
	if (cInfo->m_sock<=0) return OAPC_ERROR_RESOURCE;
	
	head.version =ntohl(1);
	head.length  =ntohl(sizeof(struct oapc_ispace_head));
	head.ios     =ntohl(0);
	head.cmd     =ntohs((unsigned short)rcode);
	head.reserved=ntohs(0);
	strncpy(head.nodeName,nodeName,MAX_NODENAME_LENGTH);
	rc=oapc_tcp_send(cInfo->m_sock,(char*)&head,sizeof(struct oapc_ispace_head),1500);
	if (rc<(int)sizeof(struct oapc_ispace_head))
	{
		oapc_ispace_disconnect(handle);
		return OAPC_ERROR_CONNECTION;
	}
	return OAPC_OK;
}



/**
 * This function has to be called by a client application whenever modified data have to be transmitted
 * to the server. When a data block already exists not all data have to be transmitted but only the
 * modified ones, the server will change only these fields, that are sent, the other ones are left
 * untouched. That means that no data can be deleted from the server.
 * @param[in] nodeName the name of the data block to be transmitted
 * @param[in] ios the IOs that have to be sent, the fields of this value have to be filled with
 *            or-concatenated flags of type OAPC_xxx_IOx and have to correspond with the data
 *            provided in the values array
 * @param[in] values the data to be sent to the server
 * @param[in] auth reserved for future use, set to NULL
 * @return OAPC_OK when the request could be transmitted successfully or an error code otherwise;
 *            in case of an error the socket connection is closed by this function and has to be
 *            re-established by the main application;
 *            when an error OAPC_ERROR_NO_DATA_AVAILABLE is returned the parameters provided to this
 *            function have been invalid: in this case at least one IO was specified within the
 *            parameter "ios" where no data have been set to the corresponding "values" index
 */
OAPC_EXT_API int oapc_ispace_set_data(void *handle,const char *nodeName,unsigned int ios,void *values[MAX_NUM_IOS],struct oapc_ispace_auth *auth)
{
	struct oapc_ispace_head  head;
	int                      bitmask=0x01010101,i,rc;
   struct ispace_conn_info *cInfo;
   
   if (!handle)
    return OAPC_ERROR_RESOURCE;
   cInfo=(struct ispace_conn_info*)handle;
   auth=auth;
	if (cInfo->m_sock<=0)
    return OAPC_ERROR_RESOURCE;
	
	memset(&head,0,sizeof(struct oapc_ispace_head));
	head.version =ntohl(1);
	head.length  =ntohl(sizeof(struct oapc_ispace_head));
	head.ios     =ntohl(ios);
	head.cmd     =ntohs(OAPC_CMD_SET_VALUE);
	head.reserved=ntohs(0);
	strncpy(head.nodeName,nodeName,MAX_NODENAME_LENGTH);
	rc=oapc_tcp_send(cInfo->m_sock,(char*)&head,sizeof(struct oapc_ispace_head),1500);
//printf("liboapc: sending head, %d bytes, %s, 0x%X\n",rc,head.nodeName,head.ios);
	if (rc<(int)sizeof(struct oapc_ispace_head))
	{
		oapc_ispace_disconnect(handle);
		return OAPC_ERROR_CONNECTION;
	}
	for (i=0; i<MAX_NUM_IOS; i++)
	{
		if ((ios & bitmask)!=0)
		{
			if (!values[i]) // client error, a IO flag was set for an IO where no data are provided
			{
	         oapc_ispace_disconnect(handle);
            return OAPC_ERROR_NO_DATA_AVAILABLE;
			}
         if (ios & bitmask & OAPC_DIGI_IO_MASK)
         {
            rc=oapc_tcp_send(cInfo->m_sock,(char*)values[i],sizeof(struct oapc_digi_value_block),1500);
            if (rc<(int)sizeof(struct oapc_digi_value_block))
	         {		
	         	oapc_ispace_disconnect(handle);
         		return OAPC_ERROR_CONNECTION;
	         }
         }
         else if (ios & bitmask & OAPC_NUM_IO_MASK)
         {
            rc=oapc_tcp_send(cInfo->m_sock,(char*)values[i],sizeof(struct oapc_num_value_block),1500);
//printf("liboapc: sending body, %d bytes\n",rc);
            if (rc<(int)sizeof(struct oapc_num_value_block))
	         {		
	         	oapc_ispace_disconnect(handle);
         		return OAPC_ERROR_CONNECTION;
	         }
         }
         else if (ios & bitmask & OAPC_CHAR_IO_MASK)
         {
            rc=oapc_tcp_send(cInfo->m_sock,(char*)values[i],sizeof(struct oapc_char_value_block),1500);
            if (rc<(int)sizeof(struct oapc_char_value_block))
	         {		
	         	oapc_ispace_disconnect(handle);
         		return OAPC_ERROR_CONNECTION;
	         }
         }			
         else if (ios & bitmask & OAPC_BIN_IO_MASK)
         {
         	struct oapc_bin_head binHead;
         	char                *dataPos;

         	memcpy(&binHead,values[i],sizeof(struct oapc_bin_head));
            binHead.version     =htonl(((struct oapc_bin_head*)values[i])->version); 
            binHead.unitExponent=htons(((struct oapc_bin_head*)values[i])->unitExponent);
            binHead.int1        =0; // for application-internal use only, has not to be transmitted
            binHead.param1      =htonl(((struct oapc_bin_head*)values[i])->param1);
            binHead.param2      =htonl(((struct oapc_bin_head*)values[i])->param2);
            binHead.param3      =htonl(((struct oapc_bin_head*)values[i])->param3);
            binHead.sizeData    =htonl(((struct oapc_bin_head*)values[i])->sizeData);
            // send heads minus the first byte that already belongs to the data
            rc=oapc_tcp_send(cInfo->m_sock,(char*)&binHead,sizeof(struct oapc_bin_head)-1,1500);
            if (rc<(int)sizeof(struct oapc_bin_head)-1)
	         {		
	         	oapc_ispace_disconnect(handle);
         		return OAPC_ERROR_CONNECTION;
	         }
	         // send the data
	         dataPos=&((struct oapc_bin_head*)values[i])->data;
            rc=oapc_tcp_send(cInfo->m_sock,dataPos,((struct oapc_bin_head*)values[i])->sizeData,1500);
            if (rc<((struct oapc_bin_head*)values[i])->sizeData)
	         {		
	         	oapc_ispace_disconnect(handle);
         		return OAPC_ERROR_CONNECTION;
	         }
         }			
         else assert(0);
		}
	   bitmask=bitmask<<1;
	}	
	return OAPC_OK;	
}



/**
 * This function has to be called by a client application whenever a single value has to be transmitted
 * to the server.
 * @param[in] nodeName the name of the data block to be transmitted
 * @param[in] io the IO flag of the value to be sent, this parameter expects exactly one OAPC_xxx_IOx
 *            flag
 * @param[in] value the value to be sent to the server
 * @param[in] auth reserved for future use, set to NULL
 * @return OAPC_OK when the request could be transmitted successfully or an error code otherwise;
 *            in case of an error the socket connection is closed by this function and has to be
 *            re-established by the main application;
 *            when an error OAPC_ERROR_NO_DATA_AVAILABLE is returned the parameters provided to this
 *            function have been invalid: in this case at least one IO was specified within the
 *            parameter "ios" where no data have been set to the corresponding "values" index
 */
OAPC_EXT_API int oapc_ispace_set_value(void *handle,const char *nodeName,unsigned int io,void *value,struct oapc_ispace_auth *auth)
{
   void         *values[MAX_NUM_IOS]={NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL};
   int           i;
   unsigned long bitmask=0x01010101;
   
   for (i=0; i<MAX_NUM_IOS; i++)
   {
      if (bitmask & io) values[i]=value;
      else values[i]=NULL;
      bitmask=bitmask<<1;
   }
   return oapc_ispace_set_data(handle,nodeName,io,values,auth);
}



/**
 * This function closes a connection to an Interlock Server and releases all related
 * resources including the callback function. So before the next connection attempt the callback
 * has to be installed again.
 * @return OAPC_OK
 */
OAPC_EXT_API int oapc_ispace_disconnect(void *handle)
{
   struct ispace_conn_info *cInfo;
   int                      i;
   
   if (!handle) return OAPC_ERROR_RESOURCE;
   cInfo=(struct ispace_conn_info*)handle;
	cInfo->m_ispace_recv_running=0;
	if (cInfo->m_sock>0) oapc_tcp_closesocket(cInfo->m_sock);
	cInfo->m_sock=-1;
	cInfo->m_used=0;
   for (i=0; i<100; i++)
   {
	   oapc_thread_sleep(10); // cause a thread switch
      if (cInfo->m_ispace_recv_running) break; // thread was finished so no chance the callback is called once again
   }
	return OAPC_OK;
}
