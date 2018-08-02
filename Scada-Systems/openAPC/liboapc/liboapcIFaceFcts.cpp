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

#ifdef _MSC_VER
#pragma warning (disable: 4018)
#pragma warning (disable: 4127)
#endif


#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <math.h>
#ifndef ENV_WINDOWSCE
 #include <errno.h>
#else
#endif

#ifdef ENV_WINDOWS
 #undef _UNICODE
 #ifdef ENV_WINDOWSCE
//  #include <winsock2.h>
 #endif
#else
 #define _strdup strdup
#endif

#ifndef ENV_WINDOWS
 #include <sys/io.h>
 #include <sys/socket.h>
 #include <arpa/inet.h>
 #include <errno.h>
 #include <unistd.h>
 #include <fcntl.h>
#else
/* #ifndef ENV_WINDOWSCE 
//  #include <pthread_win.h>
 #else
  #include <winsock2.h>
 #endif*/
#endif

#ifdef ENV_LINUX
 #include <termios.h>
 #include <unistd.h>
#endif

#ifdef ENV_QNX
#include <sys/select.h>
#define MSG_NOSIGNAL 0
#endif

#include "oapc_libio.h"
#include "liboapc.h"

#define MAX_BUF_LEN MAX_TEXT_LEN+20


/**
 * This structure is used once for every connected client, it stores all relevant
 * information including the received data
 */
struct list_entry 
{
   struct list_entry      *next;                 // the next structure in the list
   int                     sock;                  
   char                    recBuffer[MAX_BUF_LEN+1]; 
   int                     readLen;       
   char                   *remoteIP;             // the IP of the remote connection
   char                    cmdData[MAX_TEXT_LEN+1];
   float                   numData;
   char                    charData[MAX_TEXT_LEN+1];
   struct oapc_bin_head   *binData;
   unsigned                digiData:1;
   unsigned                cmdReceived:1;
   unsigned                digiReceived:1;
   unsigned                numReceived:1;
   unsigned                charReceived:1;
   unsigned                binReceived:1;
};


static int                     list_s=-1; // the local server socket
static char                    m_iface_recv_running=0;
static char                   *m_uname=NULL,*m_pwd=NULL;
static lib_oapc_iface_callback m_oapc_iface_callback=NULL;
static struct list_entry       firstEntry={NULL,0,"",0,NULL,"",0.0,"",NULL,0,0,0,0,0};
#ifndef ENV_WINDOWS
static pthread_mutex_t         m_client_mutex;
#else
static CRITICAL_SECTION        m_client_mutex;
#endif



/**
 * This function has to be called before the interface is initialized: here a callback function
 * has to e specified where state information and received data are handed over during
 * communicating with connected clients.
 * @param[in] oapc_iface_callack the callack function of type li_oapc_iface_callack to receive
 *            communication data with
 * @return OAPC_OK in case the callack function could e registered successfully
 */
OAPC_EXT_API int oapc_iface_set_recv_callback(lib_oapc_iface_callback oapc_iface_callback)
{
	m_oapc_iface_callback=oapc_iface_callback;
	return OAPC_OK;
}



/**
 * Whenever a client closes its connection it has to be removed from the list of active
 * clients and the resources of the related list_entry-structure have to be released.
 * That has to be done by calling this function.
 * @param list the list of entries where the client has to be removed from
 * @param entry the clients list entry that has to be removed
 * @return 0 in case the operation could be completed successfully or an error code
 *         otherwise
 */
int removeClient(struct list_entry *list,struct list_entry *entry)
{
   struct list_entry *lz, *lst = NULL;
   
   if (!entry) return 0;

   if (!list->next) return OAPC_ERROR_RESOURCE;
   for (lz = list->next; lz; lz = lz->next)
   {
      if (lz->sock == entry->sock) break;
      lst = lz;
   }
   if (!lz) return OAPC_ERROR_RESOURCE;
   if (lst) lst->next = lz->next;
   else list->next = lz->next;
   if (lz->sock) oapc_tcp_closesocket(lz->sock);
   if (lz->remoteIP) free(lz->remoteIP);
   
   free(lz);
   return 0;
}



OAPC_EXT_API int oapc_iface_exit(void)
{
	m_iface_recv_running=0;
	while (firstEntry.next) removeClient(&firstEntry,firstEntry.next);

	if (m_uname) free(m_uname);
	if (m_pwd) free(m_pwd);
	m_uname=NULL;
	m_pwd=NULL;

	if (list_s>0) oapc_tcp_closesocket(list_s);
	list_s=-1;
#ifdef ENV_WINDOWS // cause a thread switch
   Sleep(10);
#else
   sleep(10);
#endif
#ifndef ENV_WINDOWS
   pthread_mutex_destroy(&m_client_mutex);    
#else
   DeleteCriticalSection(&m_client_mutex);
#endif

#ifdef ENV_WINDOWS
   WSACleanup();
#endif
	return OAPC_OK;
}



OAPC_EXT_API int oapc_iface_set_authentication(char *uname,char *pwd)
{
	if (m_uname) free(m_uname);
	if (m_pwd) free(m_pwd);
	m_uname=NULL;
	m_pwd=NULL;
	if ((!uname) || (!pwd)) return OAPC_ERROR_RESOURCE;
	m_uname=_strdup(uname);
	m_pwd=_strdup(pwd);
	
	return OAPC_OK;
}



/**
 * This function us used together with the socet function select(), it fills the file
 * descriptor set that is required by set depending on the available socket connections
 * @parem fds the file descriptor set structure to be filled
 * @param list the list of clients from which the socket information have to be used
 * @return the maximum socket file descriptor number
 */
static int fillSet(fd_set *fds, struct list_entry *list)
{
   int max = 0;
   struct list_entry *lz;

   for (lz = list->next; lz; lz = lz->next)
   {
      if (lz->sock>max) max = lz->sock;
      if (lz->sock>0) FD_SET(lz->sock, fds);
   }
   return max;
}



/**
 * This function creates a new list_entry and adds a new client to the internal list
 * of connections
 * @param list the list where the new entry has to be added
 * @param s_sock the socket descriptor of the new client connection
 * @param t_sock the socket descriptor of the forwarded connection
 * @parem remoteIP the remote IP of the client
 * @return 0 when the operation could be finished successfully or an error code otherwise
 */
static struct list_entry *addClient(struct list_entry *list, int s_sock,char *remoteIP)
{
   struct list_entry *n;

   n = (struct list_entry*)malloc(sizeof(struct list_entry));
   if (!n) return NULL;
   memset(n,0,sizeof(struct list_entry));
   n->sock = s_sock;
   if (remoteIP) n->remoteIP=_strdup(remoteIP);
   else n->remoteIP=_strdup("unknown host");
   n->readLen=0;
   n->next = list->next;
   list->next= n;
   return n;
}



/**
 * This function can be used after a call to the socekt function select(), it retrieves
 * that client out of a given list where a event occured that was watched by select().
 * @param fds the file descriptor set structure that was modified by select()
 * @param the related list of client conmnections that belongs to the preceding caqll of
 *        select and that was used by a former call to fillSet()
 * @return the client connection that caused select() to leave its waiting state or NULL
 *        in case there is no client socket that fits to the result
 */
static struct list_entry *getClient(fd_set *fds,struct list_entry *list)
{
   int                i=0;
   struct list_entry *lz;

   while(!FD_ISSET(i, fds)) i++;
   lz=list->next;
   while (lz)
   {
      if (lz->sock==i) return lz;
      lz=lz->next;
   }
   return NULL;
}



/**
 * This function reads from the sockets and tries to fetch data from both connections. It reads
 * data only for a given time and stores them.
 * @param entry the list entry of the client where data have to be read for
 * @param timeout the maximum time in milliseconds that is allowed to be used to receive data from
 *        the clients socket
 * @return true in case a full package could be read successfully or false otherwise
 */
static bool readPackets(struct list_entry *entry, long timeout) 
{
   long /*size_t*/   rc;
   long     ctr=0;
   #ifdef ENV_WINDOWS
   long     err;
   #endif

   // data from source client side
   while (entry->readLen<=MAX_BUF_LEN)
   {
      rc = oapc_tcp_recv(entry->sock,((char*)&entry->recBuffer)+entry->readLen,1,NULL,500);   
      if (rc>0)
      {
         entry->readLen+=rc;
         if ((entry->recBuffer[entry->readLen-1]=='\r') || (entry->recBuffer[entry->readLen-1]=='\n'))
         {
         	entry->recBuffer[entry->readLen-1]=0;
         	entry->readLen=(int)strlen(entry->recBuffer);
         	return true;
         } 
      }
      else if (rc==0) return false;
      else
      {
#ifdef ENV_WINDOWS
         err=GetLastError();
         if ((err!=EAGAIN) && (err!=WSAEWOULDBLOCK) && (err!=0))
#else
         if ((errno!=EAGAIN) && (errno!=EINPROGRESS) && (errno!=0))
#endif
         {
         	entry->readLen=-1;
            return false;
         }
         ctr+=10;
#ifdef ENV_WINDOWS
         Sleep(10);
#else
         usleep(10000);
#endif
      }
      if (ctr>timeout) break;
   }
   return false;
}



static void *sock_thread(void *arg)
{
   int                c, max;
	fd_set             fds;
   unsigned long      remoteIP;
   struct list_entry *client;
   struct in_addr     in_address;
   unsigned char      doAdd=0;

   arg=arg;
   while (m_iface_recv_running)
   {
      FD_ZERO(&fds);
      max = fillSet(&fds, &firstEntry);
      FD_SET(list_s, &fds);
      if (list_s > max) max = list_s;
      select(max + 1, &fds, NULL, NULL, NULL);
   
      if (FD_ISSET(list_s, &fds))
      {
         c=oapc_tcp_accept_connection(list_s, &remoteIP);
         if (c>0)
         {
         	char uname[MAX_TEXT_LEN],pwd[MAX_TEXT_LEN];
         	int  mode;
         	
         	{
         		struct list_entry tmpClient={NULL,c,"",0,NULL,"",0.0,"",NULL,0,0,0,0,0};

               doAdd=0;         		
         		if (readPackets(&tmpClient,500))
         		{
                  tmpClient.readLen=0;
         			if (strstr(tmpClient.recBuffer,"UNAME "))
         			{
         				strcpy(uname,tmpClient.recBuffer+6);
               		if (readPackets(&tmpClient,400))
               		{
                        tmpClient.readLen=0;
         	       		if (strstr(tmpClient.recBuffer,"PWD "))
         			      {
               				strcpy(pwd,tmpClient.recBuffer+4);
         				      if ((m_uname) && (m_pwd) &&
                               ((strncmp(uname,m_uname,MAX_TEXT_LEN)) || (strncmp(pwd,m_pwd,MAX_TEXT_LEN))))
         				      {
         				      	strcpy(tmpClient.recBuffer,"FAIL authentication error\n");
		                        oapc_tcp_send(c,tmpClient.recBuffer,(int)strlen(tmpClient.recBuffer),500);
		                        oapc_tcp_closesocket(c);
         				      }
         				      else
         				      {
                        		if (readPackets(&tmpClient,300))
                         		{
                                 tmpClient.readLen=0;
                  	       		if (strstr(tmpClient.recBuffer,"MODE "))
                  			      {
                  			      	mode=atoi(tmpClient.recBuffer+5);
                  			      	if (mode!=1)
                  			      	{
                   				      	strcpy(tmpClient.recBuffer,"FAIL wrong mode\n");
		                                 oapc_tcp_send(c,tmpClient.recBuffer,(int)strlen(tmpClient.recBuffer),500);
		                                 oapc_tcp_closesocket(c);
                  			      	}
                  			      	else doAdd=1;
                  			      }
                        			else printf("Wrong format on incoming data (MODE not found)");
                         		}	      	
         				      }
               			}
               			else printf("Wrong format on incoming data (PWD not found)");
         	      	}
         			}
         			else printf("Wrong format on incoming data (UNAME not found)");
         		}
         	}
         	if ((!m_uname) || (!m_pwd)) doAdd=1;
         	

         	if (doAdd)
         	{
#ifndef ENV_WINDOWS
               pthread_mutex_lock(&m_client_mutex);
#else
               EnterCriticalSection(&m_client_mutex);
#endif
#ifdef ENV_WINDOWS
               in_address.S_un.S_addr=remoteIP;
#else
               in_address.s_addr=remoteIP;
#endif
               client=addClient(&firstEntry,c,inet_ntoa(in_address));
#ifndef ENV_WINDOWS
               pthread_mutex_unlock(&m_client_mutex);
#else
               LeaveCriticalSection(&m_client_mutex);
#endif
     		   	m_oapc_iface_callback(OAPC_IFACE_TYPE_STATE_NEW_CONNECTION,0,0,0,NULL,NULL,client->sock);
         	}
         }
      }
      else
      {
      	unsigned char mutexUnlocked=0;

#ifndef ENV_WINDOWS
         pthread_mutex_lock(&m_client_mutex);
#else
         EnterCriticalSection(&m_client_mutex);
#endif
         // get the client that sends new data at the moment
         client = getClient(&fds,&firstEntry);
         if (client)
         {
            if (!readPackets(client,5))
            {
            	if (client->readLen<0)
            	{
       		   	m_oapc_iface_callback(OAPC_IFACE_TYPE_STATE_CONNECTION_CLOSED,0,0,0,NULL,NULL,client->sock);
            		removeClient(&firstEntry, client);
            	}
            	// else: only partially received
            }
            else if (client->readLen>0)
            {            	
            	if (strstr(client->recBuffer,"CMD ")==client->recBuffer)
            	{
            		strncpy(client->cmdData,client->recBuffer+4,MAX_TEXT_LEN);
            		client->cmdReceived=1;
            		client->readLen=0;
            	}
            	else if (strstr(client->recBuffer,"DIGI ")==client->recBuffer)
            	{
            		if (atoi(client->recBuffer+5)!=0) client->digiData=1;
            		else client->digiData=0;
            		client->digiReceived=1;
            		client->readLen=0;
            	}
            	else if (strstr(client->recBuffer,"NUM ")==client->recBuffer)
            	{
            		client->numData=(float)oapc_util_atof(client->recBuffer+4);
            		client->numReceived=1;
            		client->readLen=0;
            	}
            	else if (strstr(client->recBuffer,"CHAR ")==client->recBuffer)
            	{
            		strncpy(client->charData,client->recBuffer+5,MAX_TEXT_LEN);
            		client->charReceived=1;
            		client->readLen=0;
            	}
            	if (client->cmdReceived)
            	{
            		if (client->digiReceived)
            		{
            			client->cmdReceived=0;
            			client->digiReceived=0;
#ifndef ENV_WINDOWS
                     pthread_mutex_unlock(&m_client_mutex);
#else
                     LeaveCriticalSection(&m_client_mutex);
#endif
                     mutexUnlocked=1;
            			m_oapc_iface_callback(OAPC_IFACE_TYPE_DIGI,client->cmdData,client->digiData,0.0,NULL,NULL,client->sock);
            		}
            		else if (client->numReceived)
            		{
            			client->cmdReceived=0;
            			client->numReceived=0;
#ifndef ENV_WINDOWS
                     pthread_mutex_unlock(&m_client_mutex);
#else
                     LeaveCriticalSection(&m_client_mutex);
#endif
                     mutexUnlocked=1;
            			m_oapc_iface_callback(OAPC_IFACE_TYPE_NUM,client->cmdData,0,client->numData,NULL,NULL,client->sock);
            		}
            		else if (client->charReceived)
            		{
            			client->cmdReceived=0;
            			client->charReceived=0;
#ifndef ENV_WINDOWS
                     pthread_mutex_unlock(&m_client_mutex);
#else
                     LeaveCriticalSection(&m_client_mutex);
#endif
                     mutexUnlocked=1;
            			m_oapc_iface_callback(OAPC_IFACE_TYPE_CHAR,client->cmdData,0,0.0,client->charData,NULL,client->sock);
            		}
            	}
	         }
         }
         if (!mutexUnlocked)
#ifndef ENV_WINDOWS
          pthread_mutex_unlock(&m_client_mutex);
#else
          LeaveCriticalSection(&m_client_mutex);
#endif

      }
	}
	return NULL;
}



OAPC_EXT_API int oapc_iface_send_digi(const char *cmd,unsigned char digi,int socket)
{
	if (!socket)
	{
      struct list_entry *lz,*curr;

#ifndef ENV_WINDOWS
      pthread_mutex_lock(&m_client_mutex);
#else
      EnterCriticalSection(&m_client_mutex);
#endif
      lz = firstEntry.next;
      while (lz)
      {
         if (oapc_iface_send_digi(cmd,digi,lz->sock)==OAPC_OK) lz=lz->next;
         else
         {
         	curr=lz;
         	lz=lz->next;
         	removeClient(&firstEntry,curr);
         }
      }
#ifndef ENV_WINDOWS
      pthread_mutex_unlock(&m_client_mutex);
#else
      LeaveCriticalSection(&m_client_mutex);
#endif
	}
	else
	{
		char buffer[MAX_BUF_LEN+1];
		
		sprintf(buffer,"CMD %s\n",cmd);
		if (oapc_tcp_send(socket,buffer,(int)strlen(buffer),500)<0) return OAPC_ERROR_CONNECTION;
      if (!digi) strcpy(buffer,"DIGI 0\n");
      else strcpy(buffer,"DIGI 1\n");
		if (oapc_tcp_send(socket,buffer,(int)strlen(buffer),500)<0) return OAPC_ERROR_CONNECTION;
	}
	return OAPC_OK;
}



OAPC_EXT_API int oapc_iface_send_num(const char *cmd,float num,int socket)
{
	if (!socket)
	{
      struct list_entry *lz,*curr;

#ifndef ENV_WINDOWS
      pthread_mutex_lock(&m_client_mutex);
#else
      EnterCriticalSection(&m_client_mutex);
#endif
      lz = firstEntry.next;
      while (lz)
      {
         if (oapc_iface_send_num(cmd,num,lz->sock)==OAPC_OK) lz=lz->next;
         else
         {
         	curr=lz;
         	lz=lz->next;
         	removeClient(&firstEntry,curr);
         }
      }
#ifndef ENV_WINDOWS
      pthread_mutex_unlock(&m_client_mutex);
#else
      LeaveCriticalSection(&m_client_mutex);
#endif
	}
	else
	{
		char buffer[MAX_BUF_LEN+1];
		
		sprintf(buffer,"CMD %s\n",cmd);
		if (oapc_tcp_send(socket,buffer,(int)strlen(buffer),500)<0) return OAPC_ERROR_CONNECTION;
		sprintf(buffer,"NUM %f\n",num);
		if (oapc_tcp_send(socket,buffer,(int)strlen(buffer),500)<0) return OAPC_ERROR_CONNECTION;
	}
	return OAPC_OK;
}



OAPC_EXT_API int oapc_iface_send_char(const char *cmd,char *str,int socket)
{
	if (!socket)
	{
      struct list_entry *lz,*curr;

#ifndef ENV_WINDOWS
      pthread_mutex_lock(&m_client_mutex);
#else
      EnterCriticalSection(&m_client_mutex);
#endif
      lz = firstEntry.next;
      while (lz)
      {
         if (oapc_iface_send_char(cmd,str,lz->sock)==OAPC_OK) lz=lz->next;
         else
         {
         	curr=lz;
         	lz=lz->next;
         	removeClient(&firstEntry,curr);
         }
      }
#ifndef ENV_WINDOWS
      pthread_mutex_unlock(&m_client_mutex);
#else
      LeaveCriticalSection(&m_client_mutex);
#endif
	}
	else
	{
		char buffer[MAX_BUF_LEN+1];
		
		sprintf(buffer,"CMD %s\n",cmd);
		if (oapc_tcp_send(socket,buffer,(int)strlen(buffer),500)<0) return OAPC_ERROR_CONNECTION;
		sprintf(buffer,"CHAR %s\n",str);
		if (oapc_tcp_send(socket,buffer,(int)strlen(buffer),500)<0) return OAPC_ERROR_CONNECTION;
	}
	return OAPC_OK;
}



OAPC_EXT_API int oapc_iface_init(const char *host,unsigned short port)
{
   unsigned long      on=1;
#ifdef ENV_WINDOWS
   WSADATA            wsaData;

   WSAStartup((MAKEWORD(1, 1)), &wsaData);
#endif
	if ((!m_oapc_iface_callback) || (m_iface_recv_running)) return OAPC_ERROR_RESOURCE;
	if (port==0) port=1810;
   if (host==NULL) list_s=oapc_tcp_listen_on_port(port,"0.0.0.0");
   else list_s=oapc_tcp_listen_on_port(port,host);
   if (list_s<=0) return OAPC_ERROR_RESOURCE;
   if (setsockopt(list_s,SOL_SOCKET,SO_KEEPALIVE,(char*)&on,sizeof(on))<0)
   {
   	oapc_iface_exit();
   	return OAPC_ERROR_RESOURCE;
   }
   m_iface_recv_running=1;
	if (!oapc_thread_create(&sock_thread,NULL))
	{
   	oapc_iface_exit();
		return OAPC_ERROR_NO_MEMORY;
	}
#ifndef ENV_WINDOWS
   pthread_mutex_init(&m_client_mutex,NULL);    
#else
   InitializeCriticalSection(&m_client_mutex);
#endif
	return OAPC_OK;
}
