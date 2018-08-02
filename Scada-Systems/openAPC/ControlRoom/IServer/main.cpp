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

/*
 * main.c
 *
 * Copyright (c) 2008-2011 OAPC Int.
 *
 * This software is not designed or intended for use in on-line control of
 * aircraft, air traffic, aircraft navigation or aircraft communications; or in
 * the design, construction, operation or maintenance of any nuclear
 * facility. Licensee represents and warrants that it will not use or
 * redistribute the Software for such purposes.
 */

#ifdef _MSC_VER
 #pragma warning (disable: 4100)
 #pragma warning (disable: 4127)
 #pragma warning (disable: 4996)
#endif

#if defined (ENV_LINUX) || defined (ENV_QNX)
 #include <sys/socket.h>       /*  socket definitions        */
 #include <sys/types.h>        /*  socket types              */
 #include <arpa/inet.h>        /*  inet (3) funtions         */
 #include <unistd.h>           /*  misc. UNIX functions      */
 #include <pwd.h>
 #include <sched.h>
 #include <sys/select.h>       /* QNX only? */
 #include <errno.h>
 #include <signal.h>
#endif
#ifdef ENV_WINDOWS
 #include <windows.h>
 #ifdef ENV_WINDOWSCE
  #include <winsock2.h>
  #include "../plugins/libio_clock/time_ce.h"

  #define strdup _strdup
  #define EAGAIN 11
  #define time time_ce
  #define time_t time_t_ce
 #else
  #include <errno.h>
 #endif
#endif

#include <stdarg.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>
#include <list>

#ifndef ENV_WINDOWSCE
 #include <time.h>
#endif

#ifndef ENV_QNX
 #include "getopt.h"
#endif

#define EXCLUDE_WX
#include "maindefs.h"
#include "common.h"

#include "oapc_libio.h"
#include "liboapc.h"
#include "platforms.h"
#include "globaldefs.h"


struct list_entry 
{
   int                     sock;                  
   struct oapc_ispace_head recBuffer; 
   int                     readLen;       
   char                   *remoteIP;             // the IP of the remote connection
   struct data_buffer     *data;
   struct oapc_ispace_auth auth;
};



struct data_buffer
{
   int     ios;
   int     readLen[MAX_NUM_IOS],sentLen[MAX_NUM_IOS],nominalLen[MAX_NUM_IOS];
   void   *data[MAX_NUM_IOS];
};



struct data_entry
{
   char                nodeName[MAX_NODENAME_LENGTH+1];
   struct data_buffer *data;
};



char                                *userName=NULL,*authkey=NULL;
static void                         *g_handle=NULL;
static unsigned char                 dSystemExit=0; // set to true by node /system/exit/0
static unsigned short                srcport=18100;
static time_t                        serverStartTime=0;
static const char                   *appName=NULL;
#ifndef ENV_WINDOWSCE
static time_t                        lastBackupUpdateTime=0;
static bool                          isHotBackup=false,startHotBackup=false;
static char                         *redundantIP=NULL;
#endif
static std::list<struct data_entry*> nodeList;
static std::list<struct list_entry*> clientList;

#ifdef ENV_POSIX
static bool              daemonMode=false;
       bool              verbose=false;
#endif

#ifdef ENV_QNX
#define MSG_NOSIGNAL 0
#endif

#ifdef _DEBUG
 static FILE                   *FHandle=NULL;



void writeLog(const char *format,...)
{
   va_list  arglist;
   char     sText[1500];
   void    *time;
   double   sec=0;

   if (!FHandle) return;
   va_start(arglist,format);
   vsprintf(sText,format,arglist);
   va_end(arglist);
   time=oapc_util_get_time();
   if (time)
   {
      sec=oapc_util_get_timeofday(time);
      oapc_util_release_time(time);
   }
#ifdef ENV_WINDOWS
   fprintf(FHandle,"%f\t%s\r\n",sec,sText);
#else
   fprintf(FHandle,"%f\t%s\n",sec,sText);
#endif
}
#endif // _DEBUG



/**
 * This function creates a new list entry and adds a new client to the internal list
 * of connections
 * @param s_sock the socket descriptor of the new client connection
 * @parem remoteIP the remote IP of the client
 * @return the new list_entry when the operation could be finished successfully or an
 *         error code otherwise
 */
struct list_entry *addClient(int s_sock,char *remoteIP)
{
   struct list_entry *n;

   n =(struct list_entry*)malloc(sizeof(struct list_entry));
   if (!n) return NULL;
   memset(n,0,sizeof(struct list_entry));

   n->sock = s_sock;
   if (remoteIP) n->remoteIP=strdup(remoteIP);
   else n->remoteIP=strdup("unknown host");
   n->readLen=0;
   clientList.push_back(n);

   return n;
}



/**
 * Whenever a client closes its connection it has to be removed from the list of active
 * clients and the resources of the related list_entry-structure have to be released.
 * That has to be done by calling this function.
 * @param entry the clients list entry that has to be removed
 * @return OAPC_OK in case the operation could be completed successfully or an error code
 *         otherwise
 */
int releaseClient(struct list_entry *entry)
{
   int                                     i;
   
   if (!entry) return OAPC_ERROR;
   if (entry->sock>0) oapc_tcp_closesocket(entry->sock);
   if (entry->remoteIP) free(entry->remoteIP);
   
   if (entry->data)
   {
   	for (i=0; i<MAX_NUM_IOS; i++)
   	 if (entry->data->data[i]) free(entry->data->data[i]);
   	free(entry->data);
   }
   free(entry);
   return OAPC_OK;
}



/**
 * Remove all these data nodes out of the list that are set by users and not by the
 * system
 */
static void removeNonSysNodes()
{
   std::list<struct data_entry*>::iterator it;
   struct data_entry                      *node;

   if (!nodeList.size()==0) return;
   it=nodeList.begin();
   while (it !=nodeList.end())
   {
      node=*it;
      if (strstr(node->nodeName,S_SYSTEM)==node->nodeName)
      {
         assert(node);
         if (node)
         {
            if (node->data) free(node->data);
            free(node);
         }
         it=nodeList.erase(it);
      }
      else it++;
   }
   return;
}



static struct data_entry *findData(const char *nodeName)
{
   std::list<struct data_entry*>::iterator it;
   struct data_entry                      *node;

   if (nodeList.empty()) return NULL;
   for ( it=nodeList.begin() ; it !=nodeList.end(); it++ )
   {
      node=*it;
   	if (strncmp(node->nodeName,nodeName,MAX_NODENAME_LENGTH)==0) return *it;
   }
   return NULL;
}



static int addData(struct data_buffer *data,const char *nodeName)
{
   struct data_entry *entry;
   
   entry=(struct data_entry*)malloc(sizeof(struct data_entry));
   if (!entry) return OAPC_ERROR_NO_MEMORY;
   memset(entry,0,sizeof(struct data_entry));

   entry->data=data;
   strncpy(entry->nodeName,nodeName,MAX_NODENAME_LENGTH);
   nodeList.push_back(entry);
   return 0;
}



/**
 * This function removes all entries from a given list of connections
 * @param list the list of entries that has to be flushed completely
 */
void freeList()
{
   while (!clientList.empty())
   {
      releaseClient(*clientList.begin());
      clientList.erase(clientList.begin());
   }
}




/**
 * This function us used together with the socet function select(), it fills the file
 * descriptor set that is required by set depending on the available socket connections
 * @parem fds the file descriptor set structure to be filled
 * @param list the list of clients from which the socket information have to be used
 * @return the maximum socket file descriptor number
 */
int fillSet(fd_set *fds)
{
   int max = 0;
   std::list<struct list_entry*>::iterator it;
   struct list_entry                      *lz;

   if (clientList.empty()) return 0;

   for ( it=clientList.begin() ; it!=clientList.end(); it++ )
   {
      lz=*it;
      if (lz->sock > max) max = lz->sock;
      if (lz->sock>0) FD_SET(lz->sock, fds);
   }
   return max;
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
/*struct list_entry *getClient(fd_set *fds,struct list_entry *list)
{
   std::list<struct list_entry*>::iterator it;
   int                                     i=0;
   struct list_entry                      *lz;

   while(!FD_ISSET(i, fds)) i++;
   if (clientList.empty()) return NULL;
   for ( it=clientList.begin() ; it!=clientList.end(); it++ )
   {
      lz=*it;
      if (lz->sock==i) return lz;
   }
   return NULL;
}*/



/** definitions for the command line options */
static char shortopts[] = "vVhabr:";
#ifndef ENV_QNX
static struct option const longopts[] = {
  {"autostart",        no_argument, NULL, 'a'},
  {"version",          no_argument, NULL, 'v'},
#ifdef ENV_LINUX
  {"verbose",          no_argument, NULL, 'V'},
#endif  
  {"help",             no_argument, NULL, 'h'},
//  {"high-availability",no_argument, NULL, 'a'},
  {NULL, no_argument, NULL, 0}
};
#endif



/** definitions for the command line help output */
static char const *const option_help[] = {
  " -a        use the mirrored mode, automatically start the hot backup server",
  " -r <ip>   use the redundant mode with the other Interlock Server at <ip>",
  " -V        print out some debugging information",
  " -v        show version info",
  " -h        show this help",
  0
};



/** print out version information */
static void version(void)
{
   printf ("%s %s %s\n%s\n", FCOMMON_NAME,GLOBAL_VERSION,FCOMMON_URL,FCOMMON_COPYRIGHT);
}



/** print out command line help information */
static void usage (char *pname)
{
   printf ("usage: %s [OPTIONS]\n",pname);
   printf (" --help for more information\n");
   return;
}



/**
 * Get the switches and related values out of the command line parameters
 * @param argc the number of arguments as handed over by main()
 * @param argv the arguments as handed over by main()
 */
static bool getSwitches(int argc,char *argv[])
{
   int                optc;
   char const *const *p;

if (optind == argc) return true; // it is allowed to have no switches
#ifndef ENV_WINDOWS
while ((optc = getopt(argc, argv, shortopts)) != -1)
#else
while ((optc = getopt_long (argc, argv, shortopts, longopts, (int *) 0)) != -1)
#endif
   {
   switch (optc)
      {
#ifndef ENV_WINDOWS
      case 'V':
         verbose=true;
         break;
#endif
#ifndef ENV_WINDOWSCE
      case 'a':
         startHotBackup=true; // start the hot backup server
         break;
      case 'b':
         isHotBackup=true; // this is the hot backup server that runs in background
         break;
      case 'r':
         redundantIP=strdup(optarg);
         break;
#endif
      case 'v':
         version();
         return false;
         break;
      case 'h':
         usage (argv[0]);
         for (p = option_help; *p; p++) printf ("%s\n", *p);
         return false;
         break;
      default:
         usage (argv[0]);
         return false;
         break;
      }
   }
   return true;
}


#ifdef _DEBUG
int retLine;
#endif
/**
 * This function reads from the sockets and tries to fetch data from both connections. It reads
 * data only for a given time and stores them.
 * @param entry the list entry of the client where data have to be read for
 * @param timeout the maximum time in milliseconds that is allowed to be used to receive data from
 *        the clients socket
 * @return true in case a full package could be read successfully or false otherwise
 */
static int readPackets(struct list_entry *entry, long timeout) 
{
   long /*size_t*/   rc;
   long     i;
#ifdef ENV_WINDOWS
   long     err;
#endif

   // data from source client side
#ifdef _DEBUG
   retLine=0;
#endif
   if (entry->readLen==0) memset(&entry->recBuffer,0,sizeof(entry->recBuffer));
   if (entry->readLen<(int)sizeof(entry->recBuffer))
   {
      rc=oapc_tcp_recv(entry->sock,((char*)&entry->recBuffer)+entry->readLen,sizeof(entry->recBuffer)-entry->readLen,NULL,timeout);
//      rc = recv(entry->sock,((char*)&entry->recBuffer)+entry->readLen,sizeof(entry->recBuffer)-entry->readLen,MSG_NOSIGNAL);
      if (rc>0)
      {
         entry->readLen+=rc;
      }
      else
      {
#ifdef ENV_WINDOWS
         err=GetLastError();
         if ((err!=EAGAIN) && (err!=WSAEWOULDBLOCK) && (err!=0))
#else
         if ((errno!=EAGAIN) && (errno!=EINPROGRESS) && (errno!=0)) //|| (errno==ECONNRESET))
#endif
          return -1;
//         ctr+=oapc_thread_sleep(10);
         return 0;
      }
/*      if (ctr>timeout)
      {
#ifdef _DEBUG
         retLine=__LINE__;
#endif
         break;
      }*/
#ifdef _DEBUG
      retLine=__LINE__;
#endif
      return entry->readLen;
   }
   if (entry->data) for (i=0; i<MAX_NUM_IOS; i++)
   {
   	if (entry->data->data[i])
   	{
   		if (entry->data->readLen[i]<entry->data->nominalLen[i])
   	   {
   	      rc=oapc_tcp_recv(entry->sock,((char*)entry->data->data[i])+entry->data->readLen[i],entry->data->nominalLen[i]-entry->data->readLen[i],NULL,timeout);
//            rc = recv(entry->sock,((char*)entry->data->data[i])+entry->data->readLen[i],entry->data->nominalLen[i]-entry->data->readLen[i],MSG_NOSIGNAL);
            if (rc>0)
            {
               entry->data->readLen[i]+=rc;
            }
//            else if (rc==0) return 0;
            else
            {
#ifdef ENV_WINDOWS
               err=GetLastError();
               if ((err!=EAGAIN) && (err!=WSAEWOULDBLOCK) && (err!=0))
#else
               if ((errno!=EAGAIN) && (errno!=EINPROGRESS) && (errno!=0)) //|| (errno==ECONNRESET))
#endif
                return -1;
//               ctr+=oapc_thread_sleep(10);
            }
/*            if (ctr>timeout)
            {
#ifdef _DEBUG
               retLine=__LINE__;
#endif
               break;
            }*/
   	   }
   	}
#ifdef _DEBUG
      retLine=__LINE__;
#endif
   }
   return 1;
}



/**
 * This function has to be called when the application exits, it releases several used
 * buffers
 */
static void freeMem(void)
{
if (userName) free(userName);         userName=NULL;
if (authkey) free(authkey);           authkey=NULL;
}



/**
 * This is the main loop, it is called by main() after several necessary initializations have been done.
 * This main loop accepts new incoming connections, tries to read data from all available connections,
 * parses received data blocks, invokes the required (re)action for the received data and sends a
 * response back to the client.
 */
static bool mainLoop(void* /*hInstance*/)
{
#ifdef ENV_WINDOWSCE
   int                              errno;
#endif
   unsigned long                    on=1;
   int                              list_s;                /*  listening socket          */
   unsigned long                    remoteIP;
   struct in_addr                   in_address;
   int                              c, max,ret,updCnt=0;
   struct list_entry               *client;
   fd_set                           fds;
   std::list<list_entry*>::iterator clientIt;

#ifdef ENV_POSIX
   if (daemonMode)
#endif
   showLog("Starting %s...",FCOMMON_NAME);
   list_s=oapc_tcp_listen_on_port(srcport,"0.0.0.0");
   if (list_s<=0)
   {
      #ifdef ENV_WINDOWS
      errno=GetLastError();
      #endif
      showLog("ERROR: could not create listening socket %s:%d, %d (%d)!","0.0.0.0",srcport,errno,list_s);
      freeMem();
      return false;
   }
   if (setsockopt(list_s,SOL_SOCKET,SO_KEEPALIVE,(char*)&on,sizeof(on))<0)
   {
       #ifdef ENV_WINDOWS
       errno=GetLastError();
       #else
       perror("");
       #endif
      showLog("WARNING: could not set socket options, %d!",errno);
   }
   showLog("INFO: binding to %s:%d","0.0.0.0",srcport);

#ifdef ENV_POSIX
   if (userName)
   {
      struct passwd *pwd;

      pwd=getpwnam(userName);
      if (!pwd) fprintf(stderr,"Error: unknown username %s!\n",optarg);
      else
      {
         if (setgid(pwd->pw_uid)!=0) perror("ERROR: Setting group failed");
         if (setuid(pwd->pw_uid)!=0) perror("ERROR: Setting user failed");
      }
      free(userName);
   }

   if (daemonMode) daemon(0,0);
   #endif
   while ((!dSystemExit) && (!LeaveServer()))
   {
      clientIt=clientList.begin();
      while (clientIt!=clientList.end())
      {
         client=*clientIt;
         if (client->sock<0)
         {
            releaseClient(client);
            clientIt=clientList.erase(clientIt);
         }
         else clientIt++;
      }

      FD_ZERO(&fds);
      max = fillSet(&fds);
      FD_SET(list_s, &fds);
      if (list_s > max) max = list_s;
      select(max + 1, &fds, NULL, NULL, NULL);

      updCnt++;
      if (updCnt>100)
      {
         if (time(NULL)+10>serverStartTime) // update internal information
         {
            struct data_entry *dataEntry;

            dataEntry=findData(S_SYSTEM_UPTIME); // TODO: keep pointer to this block to avoid repeated searching for it
            if ((dataEntry) && (dataEntry->data) && (dataEntry->data->data[0]))
            {
            	double d=(double)(time(NULL)-serverStartTime);
         	
               oapc_util_dbl_to_block(d,(struct oapc_num_value_block*)dataEntry->data->data[0]);
            }
            else assert(0);
            updCnt=0;
         }
      }
   
      if (FD_ISSET(list_s, &fds))
      {
         c=oapc_tcp_accept_connection(list_s, &remoteIP);
         if (c>0)
         {
#ifdef ENV_WINDOWS
            in_address.S_un.S_addr=htonl(remoteIP);
#else
            in_address.s_addr=htonl(remoteIP);
#endif
            oapc_tcp_set_blocking(c,false);
            client=addClient(c,inet_ntoa(in_address));
            showLog("New client accepted from %s",inet_ntoa(in_address));
         }
      }
//      else
      clientIt=clientList.begin();
      while (clientIt!=clientList.end())
      {
         client=*clientIt;
         if (client->sock>0)
         {
            // get the client that sends new data at the moment
   //         if (!client) client = getClient(&fds,&firstEntry);
            if (client)
            {
               ret=readPackets(client,5);
               if (ret<0)
               {
#ifdef _DEBUG
                  writeLog("Connection lost to %s (%d)",client->remoteIP,client->sock);
#endif
                  showLog("Connection lost to %s",client->remoteIP);
                  oapc_tcp_closesocket(client->sock);
                  client->sock=-1;
               }
                  // rec buffer head was received, check and g_handle it
               else
               {
                  if (client->readLen==sizeof(client->recBuffer))
                  {
                     if (client->data) // data block already exists
                     {
                        bool               allReceived=true;
                        int                i,bitmask=0x01010101;
                        struct data_entry *recvData;
                        void              *dataChanged[MAX_NUM_IOS]={NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL};
                        unsigned int       ios=0;

#ifdef _DEBUG
                        writeLog("Full data incl. data block received from %s (%d)",client->remoteIP,client->sock);
#endif
                        for (i=0; i<MAX_NUM_IOS; i++)
                        {
                           if ((client->recBuffer.ios & bitmask) &&
                               (client->data->data[i]) && (client->data->nominalLen[i]>client->data->readLen[i]))
                           {
                              allReceived=false;
                              break;
                           }
                           else if ((client->recBuffer.ios & bitmask & OAPC_BIN_IO_MASK) &&
                                    (client->data->data[i]) &&
                                    (client->data->nominalLen[i]==sizeof(struct oapc_bin_head)-1)) // binary data have to be loaded in three passes: the general head, the binary head and the binary body where the size is known only after loading the binary head
                           {
                              struct oapc_bin_head *binHead;

                              binHead=(struct oapc_bin_head*)client->data->data[i];

                              binHead->version     =ntohl(binHead->version);
                              binHead->unitExponent=ntohs(binHead->unitExponent);
                              binHead->int1        =0; // for application-internal use only, transmission not allowed
                              binHead->param1      =ntohl(binHead->param1);
                              binHead->param2      =ntohl(binHead->param2);
                              binHead->param3      =ntohl(binHead->param3);
                              binHead->sizeData    =ntohl(binHead->sizeData);

                              client->data->nominalLen[i]=sizeof(struct oapc_bin_head)+binHead->sizeData-1;
                              client->data->data[i]=realloc(client->data->data[i],client->data->nominalLen[i]);
                              if (!client->data->data[i])
                              {
                                 //TODO: submit error code to client?
                                 oapc_tcp_closesocket(client->sock);
                                 client->sock=-1;
                                 client=NULL;
                                 break;
                              }
                              if (client->data->nominalLen[i]>client->data->readLen[i])
                              {
                                 allReceived=false;
                                 break;
                              }
                           }
                           bitmask=bitmask<<1;
                        }
                        if ((client) && (allReceived))
                        {
                           showLog("Client %d all received",client->sock);
#ifdef _DEBUG
                           writeLog("Client %d all received (%s)",client->sock,client->recBuffer.nodeName);
#endif
                           recvData=findData(client->recBuffer.nodeName);
                           if (recvData)
                           {
#ifdef _DEBUG
                              writeLog("data block already exists, reusing it");
#endif
                              ios=client->data->ios;
                              for (i=0; i<MAX_NUM_IOS; i++)
                              {
                                 if ((recvData->data->data[i]) && (client->data->data[i]))
                                 {
                                    free(recvData->data->data[i]);
                                    recvData->data->data[i]=client->data->data[i];
                                    dataChanged[i]=client->data->data[i];
                                 }
                                 else if (client->data->data[i])
                                 {
                                    recvData->data->data[i]=client->data->data[i];
                                    dataChanged[i]=client->data->data[i];
                                 }
                              }
                           }
                           else
                           {
#ifdef _DEBUG
                              writeLog("creating new data block for received data");
#endif
                              addData(client->data,client->recBuffer.nodeName);
                              recvData=findData(client->recBuffer.nodeName);
                              assert(recvData);
                              if (recvData)
                              {
                                 ios=client->data->ios;
                                 for (i=0; i<MAX_NUM_IOS; i++)
                                  if (recvData->data->data[i])
                                   dataChanged[i]=recvData->data->data[i];
                              }
                           }
#ifdef _DEBUG
                           writeLog("continuing with data block %p",recvData);
#endif
                           if (recvData)
                           {
                              if (!strncmp(recvData->nodeName,S_SYSTEM_LASTAUTH,MAX_NODENAME_LENGTH))
                              {
                                 // do not forward this information to any other client
                                 client->auth.id=(int)oapc_util_block_to_dbl((struct oapc_num_value_block*)recvData->data->data[0]);
                              }
                              else if (!strncmp(recvData->nodeName,S_SYSTEM_PING,MAX_NODENAME_LENGTH))
                              {
                                 // just do nothing, this is a message sent from clients to check if the connection is still alife
                              }
                              else
                              {
                                 struct list_entry                      *entry;
                                 int                                     ret;
                                 std::list<struct list_entry*>::iterator entryIt;
/*#ifdef _DEBUG
                                 unsigned int                            bitmask=0x01010101;
                                 int                                     cnt;
                                 struct oapc_digi_value_block           *digiValue;
                                 struct oapc_num_value_block            *numValue;
                                 struct oapc_char_value_block           *charValue;
                                 struct oapc_bin_head                   *bin=NULL;

#endif*/

                                 showLog("Broadcasting received data %s from client %d",recvData->nodeName,client->sock);
#ifdef _DEBUG
                                 writeLog("Broadcasting received data %s from client %d",recvData->nodeName,client->sock);
#endif
/*#ifdef _DEBUG
                                 for (cnt=0; cnt<MAX_NUM_IOS; cnt++)
                                 {
                                    if (bitmask & ios & OAPC_DIGI_IO_MASK)
                                    {
                                       digiValue=(struct oapc_digi_value_block*)recvData->data->data[cnt];
                                       printf("D%d %d   \t",cnt,digiValue->digiValue);
                                    }
                                    else if (bitmask & ios & OAPC_NUM_IO_MASK)
                                    {
                                       numValue=(struct oapc_num_value_block*)recvData->data->data[cnt];
                                       printf("N%d %f \t",cnt,oapc_util_block_to_dbl(numValue));
                                    }
                                    else if (bitmask & ios & OAPC_CHAR_IO_MASK)
                                    {
                                       charValue=(struct oapc_char_value_block*)recvData->data->data[cnt];
                                       printf("C%d %s \t",cnt,charValue->charValue);
                                    }
                                    else if (bitmask & ios & OAPC_BIN_IO_MASK)
                                    {
                                       bin=(struct oapc_bin_head*)recvData->data->data[cnt];
                                       printf("B%d %d \t",cnt,bin->sizeData);
                                    }

                                    bitmask=bitmask<<1;
                                 }
                                 printf("\n");
#endif*/
                                 entryIt=clientList.begin();
                                 while (entryIt!=clientList.end())
                                 {
                                    entry=*entryIt;
                                    if (entry->sock!=client->sock) // do not send data to the client that has transmitted the changes
                                    {
                                       oapc_ispace_set_socket(g_handle,entry->sock);
#ifdef _DEBUG
                                       writeLog("sending data to connected client %d",entry->sock);
#endif
                                       ret=oapc_ispace_set_data(g_handle,recvData->nodeName,ios,dataChanged,NULL);
                                       if (ret!=OAPC_OK)
                                       {
                                          oapc_tcp_closesocket(entry->sock);
                                          entry->sock=-1;
                                       }
                                    }
                                    entryIt++;
                                 }

                                 if (!strncmp(recvData->nodeName,S_SYSTEM_EXIT,MAX_NODENAME_LENGTH))
                                 {
                                    struct oapc_digi_value_block *digiValue;

                                    showLog("%s received from client %d",S_SYSTEM_EXIT,client->sock);
                                    digiValue=(struct oapc_digi_value_block*)recvData->data->data[0];
                                    if (digiValue->digiValue==1)
                                    {
                                       removeNonSysNodes();
                                    }
   /*            					   dSystemExit=digiValue->digiValue;
      printf("Exit state: %d\n",dSystemExit);*/
                                 }
      //            				   if ((dSystemExit) && (*dSystemExit)) break; // exit without visiting a - possible blocking - select() again
                              }
                           }
                           client->readLen=0;
                           client->data=NULL;
                        }
                     }
                     else if (client)// head was received but no data from the block
                     {
                        client->recBuffer.version =htonl(client->recBuffer.version);
                        client->recBuffer.length  =htonl(client->recBuffer.length);
                        client->recBuffer.ios     =htonl(client->recBuffer.ios);
                        client->recBuffer.cmd     =htons(client->recBuffer.cmd);
                        client->recBuffer.reserved=htons(client->recBuffer.reserved);
                        if (client->recBuffer.cmd==OAPC_CMD_GET_VALUE)
                        {
                           struct data_entry *sendData;

                           showLog("Client %d command OAPC_CMD_GET_VALUE, IOs 0x%X",client->sock,client->recBuffer.ios);
                           sendData=findData(client->recBuffer.nodeName);
                           oapc_ispace_set_socket(g_handle,client->sock);
                           if (!sendData)
                            oapc_ispace_send_response(g_handle,client->recBuffer.nodeName,OAPC_CMDERR_DOESNT_EXISTS);
                           else
                            oapc_ispace_set_data(g_handle,sendData->nodeName,sendData->data->ios,sendData->data->data,NULL);
                           client->readLen=0;
                        }
                        else if (client->recBuffer.cmd==OAPC_CMD_GET_ALL_VALUES)
                        {
                           int                                     rc;
                           std::list<struct data_entry*>::iterator it;
                           struct data_entry                      *node;

                           showLog("Client %d command OAPC_CMD_GET_ALL_VALUES",client->sock);
                           rc=oapc_ispace_set_socket(g_handle,client->sock);
                           for ( it=nodeList.begin() ; it !=nodeList.end(); it++ )
                           {
                              node=*it;
                              if (rc==OAPC_OK)
                              {
                                 rc=oapc_ispace_set_data(g_handle,node->nodeName,node->data->ios,node->data->data,NULL);
                              }
                              if (rc!=OAPC_OK) showLog("Failed: %d",rc);
                           }
                           client->readLen=0;
                        }
                        else if (client->recBuffer.cmd==OAPC_CMD_SET_VALUE)
                        {
                           int i,bitmask=0x01010101;

                           showLog("Client %d command OAPC_CMD_SET_VALUE, IOs 0x%X",client->sock,client->recBuffer.ios);
                           client->data=(struct data_buffer*)malloc(sizeof(struct data_buffer));
                           if (!client->data)
                           {
                              oapc_tcp_closesocket(client->sock);
                              client->sock=-1;
                              //TODO: submit error code to client?
                           }
                           else
                           {
                              memset(client->data,0,sizeof(struct data_buffer));
                              for (i=0; i<MAX_NUM_IOS; i++)
                              {
                                 if (client->recBuffer.ios & bitmask)
                                 {
                                    if (client->recBuffer.ios & bitmask & OAPC_DIGI_IO_MASK)
                                     client->data->nominalLen[i]=sizeof(struct oapc_digi_value_block);
                                    else if (client->recBuffer.ios & bitmask & OAPC_NUM_IO_MASK)
                                     client->data->nominalLen[i]=sizeof(struct oapc_num_value_block);
                                    else if (client->recBuffer.ios & bitmask & OAPC_CHAR_IO_MASK)
                                     client->data->nominalLen[i]=sizeof(struct oapc_char_value_block);
                                    else if (client->recBuffer.ios & bitmask & OAPC_BIN_IO_MASK)
                                     client->data->nominalLen[i]=sizeof(struct oapc_bin_head)-1;
                                    else assert(0);
                                    client->data->data[i]=malloc(client->data->nominalLen[i]);
                                    if (!client->data->data[i])
                                    {
                                       //TODO: submit error code to client?
                                       oapc_tcp_closesocket(client->sock);
                                       client->sock=-1;
                                    }
                                    client->data->ios=client->recBuffer.ios;
                                 }
                                 bitmask=bitmask<<1;
                              }
                           }
                        }
                        else
                        {
                           showLog("Unknown client %d command 0x%X",client->sock,client->recBuffer.cmd);
                           client->readLen=0;
                           oapc_tcp_closesocket(client->sock);
                           client->sock=-1;
                        }
                     }

      /*struct data_buffer
      {
         int     ios;
         int     readLen[MAX_NUM_IOS],sentLen[MAX_NUM_IOS],nominalLen[MAX_NUM_IOS];
         void   *data[MAX_NUM_IOS];
      };*/

                     // process incoming data
                  }
               }
            }
         }
         clientIt++;
      }
      oapc_thread_sleep(0);
   }
   freeList();
   return true;
}



static void createDefaultNumBlock(const char *nodeName,double value)
{
	struct data_buffer          *dataBuffer;
		
	dataBuffer=(struct data_buffer*)malloc(sizeof(struct data_buffer));
	assert(dataBuffer);
	if (!dataBuffer) return;
	
	memset(dataBuffer,0,sizeof(struct data_buffer));
	dataBuffer->ios=OAPC_NUM_IO0;
	
	dataBuffer->data[0]=malloc(sizeof(double));
   assert(dataBuffer->data[0]);
   if (!dataBuffer->data[0])
   {
   	free(dataBuffer);
   	return;
   }
   memcpy(dataBuffer->data[0],&value,sizeof(double));
   addData(dataBuffer,nodeName);
}



static void createSystemStateBlocks(void)
{
	struct data_buffer           *dataBuffer;
	struct oapc_digi_value_block *digiValue;
	struct oapc_char_value_block *charValue;
	struct oapc_num_value_block  *numValue;
   int                           i,j;
	
	dataBuffer=(struct data_buffer*)malloc(sizeof(struct data_buffer));
	assert(dataBuffer);
	if (!dataBuffer) return;
	
	memset(dataBuffer,0,sizeof(struct data_buffer));
	dataBuffer->ios=OAPC_NUM_IO0;
	
	dataBuffer->data[0]=malloc(sizeof(struct oapc_digi_value_block));
   assert(dataBuffer->data[0]);
   if (!dataBuffer->data[0])
   {
   	free(dataBuffer);
   	return;
   }
   digiValue=(struct oapc_digi_value_block*)dataBuffer->data[0];
   digiValue->digiValue=0;
   addData(dataBuffer,S_SYSTEM_STATE);

   //-------------------------------------------------------------------------
	
	dataBuffer=(struct data_buffer*)malloc(sizeof(struct data_buffer));
	assert(dataBuffer);
	if (!dataBuffer) return;
	
	memset(dataBuffer,0,sizeof(struct data_buffer));
	dataBuffer->ios=OAPC_DIGI_IO0;
	
	dataBuffer->data[0]=malloc(sizeof(struct oapc_digi_value_block));
   assert(dataBuffer->data[0]);
   if (!dataBuffer->data[0])
   {
   	free(dataBuffer);
   	return;
   }
   digiValue=(struct oapc_digi_value_block*)dataBuffer->data[0];
   digiValue->digiValue=0;
   addData(dataBuffer,S_SYSTEM_EXIT);

   //-------------------------------------------------------------------------
	
	dataBuffer=(struct data_buffer*)malloc(sizeof(struct data_buffer));
	assert(dataBuffer);
	if (!dataBuffer) return;
	
	memset(dataBuffer,0,sizeof(struct data_buffer));
	dataBuffer->ios=OAPC_CHAR_IO0|OAPC_CHAR_IO1|OAPC_NUM_IO2;
	
  	dataBuffer->data[0]=malloc(sizeof(struct oapc_char_value_block)); // title
   assert(dataBuffer->data[0]);
   if (!dataBuffer->data[0])
   {
	   free(dataBuffer);
     	return;
   }
   charValue=(struct oapc_char_value_block*)dataBuffer->data[0];
   charValue->charValue[0]=0;

  	dataBuffer->data[1]=malloc(sizeof(struct oapc_char_value_block)); // text
   assert(dataBuffer->data[1]);
   if (!dataBuffer->data[1])
   {
   	free(dataBuffer->data[0]);
	   free(dataBuffer);
     	return;
   }
   charValue=(struct oapc_char_value_block*)dataBuffer->data[1];
   charValue->charValue[1]=0;

	dataBuffer->data[2]=malloc(sizeof(struct oapc_num_value_block)); // type of message
   assert(dataBuffer->data[2]);
   if (!dataBuffer->data[2])
   {
   	free(dataBuffer->data[1]);
   	free(dataBuffer->data[0]);
   	free(dataBuffer);
   	return;
   }
   numValue=(struct oapc_num_value_block*)dataBuffer->data[2];
   numValue->numDivider=0;
   numValue->numValue=0;

   addData(dataBuffer,S_SYSTEM_ERROR);

   //-------------------------------------------------------------------------
	
	dataBuffer=(struct data_buffer*)malloc(sizeof(struct data_buffer));
	assert(dataBuffer);
	if (!dataBuffer) return;
	
	memset(dataBuffer,0,sizeof(struct data_buffer));
	dataBuffer->ios=OAPC_CHAR_IO0|OAPC_CHAR_IO1|OAPC_CHAR_IO2|OAPC_CHAR_IO3;
	
   for (i=0; i<4; i++) // error, warning, info, event
   {
     	dataBuffer->data[i]=malloc(sizeof(struct oapc_char_value_block));
      assert(dataBuffer->data[i]);
      if (!dataBuffer->data[i])
      {
         if (i>0) for (j=0; j<i; j++)
          free(dataBuffer->data[i]);
	      free(dataBuffer);
        	return;
      }
      charValue=(struct oapc_char_value_block*)dataBuffer->data[i];
      charValue->charValue[i]=0;
   }

   addData(dataBuffer,S_SYSTEM_LOGDATA);
}



#ifndef ENV_WINDOWSCE
static void recv_callback(void * /*g_handle*/,char *nodeName,unsigned int cmd,unsigned int ios,void *values[MAX_NUM_IOS])
{
   lastBackupUpdateTime=time(NULL);
   if (cmd!=OAPC_CMD_SET_VALUE) return;
   if (strstr(nodeName,"/system/")!=nodeName)
   {
   	struct data_entry *recvData;
   	int                i;

		recvData=findData(nodeName);
		if (!recvData)
		{
			struct data_buffer *dataBuffer;
			
			dataBuffer=(struct data_buffer*)malloc(sizeof(struct data_buffer));
			if (dataBuffer)
			{
				memset(dataBuffer,0,sizeof(struct data_buffer));
				dataBuffer->ios=ios;
   			addData(dataBuffer,nodeName);
            recvData=findData(nodeName);
			}
		}
      if (recvData)
      {
      	for (i=0; i<MAX_NUM_IOS; i++)
      	{
      		if (recvData->data->data[i]) free(recvData->data->data[i]);
      		recvData->data->ios|=ios;
      		recvData->data->data[i]=values[i];
      	}
      }
      else
      {
      }
   }
   else
   {
      if (!strncmp(nodeName,S_SYSTEM_EXIT,MAX_NODENAME_LENGTH))
      {
         struct oapc_digi_value_block *digiValue;

         digiValue=(struct oapc_digi_value_block*)values[0];
         if (digiValue->digiValue==1)
         {
            showLog("%s received from active server",S_SYSTEM_EXIT);
            removeNonSysNodes();
         }
      }
   }
}


static bool mirrorData(char *host)
{
   g_handle=oapc_ispace_get_instance();
   if (!g_handle) return false;
   oapc_ispace_set_recv_callback(g_handle,&recv_callback);
   if (oapc_ispace_connect(g_handle,host,0,NULL))
   {
      for (;;)
      {
         if (lastBackupUpdateTime+10<time(NULL))
         {
            if (oapc_ispace_request_all_data(g_handle,NULL)!=OAPC_OK) return true;
         }
         else
         {
            if (oapc_ispace_set_value(g_handle,"/system/ping",0,NULL,NULL)!=OAPC_OK) return true;
         }
         oapc_thread_sleep(1250);
      }
   }
   return false;
}
#endif



#ifdef ENV_WINDOWS
#ifdef ENV_WINDOWSCE
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE /*hPrevInstance*/, LPWSTR /*lpCmdLine*/, int /*nCmdShow*/)
#else
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE /*hPrevInstance*/, LPSTR /*lpCmdLine*/, int /*nCmdShow*/)
#endif
{
   WSADATA   wsaData;

#else
int main(int argc,char *argv[])
{
   void*          hInstance=NULL;
#endif
#ifdef ENV_WINDOWS
   int            argc;
   char          *argv[100];
   //unsigned short argv0[300];

   WSAStartup((MAKEWORD(1, 1)), &wsaData);
   argc=splitCmdLine(GetCommandLine(),argv);
//GetModuleFileName(NULL,argv0,300);
   appName="OpenISErver.exe";//argv0;
#else
   setupSignalTraps();
   appName=argv[0];
#endif
   if (!getSwitches(argc,argv)) return -1;
   srand((unsigned int)time(NULL));

#ifdef _DEBUG
 #ifdef ENV_WINDOWS
   FHandle=fopen("C:\\tmp\\iserver.log","a");
 #else
   FHandle=fopen("/tmp/iserver.log","a");
 #endif
   writeLog("=======================================================");
   writeLog("starting mainloop...");
#endif
   #ifdef ENV_POSIX
   if (verbose) printf("Starting mainloop...\n");
   #endif

   serverStartTime=time(NULL);
   createDefaultNumBlock(S_SYSTEM_VERSION,oapc_util_atof(GLOBAL_VERSION));
   createDefaultNumBlock(S_SYSTEM_UPTIME,1.0); // running for 1 second here ;-)
   createSystemStateBlocks();
   createDefaultNumBlock(S_SYSTEM_LASTAUTH,0);

#ifndef ENV_WINDOWSCE
   if ((isHotBackup) && (redundantIP))
   {
      printf("Error: options -r and -a can't be used at the same time!\n");
      return -1;
   }

   if (redundantIP)
   {
      if (!mirrorData(redundantIP)) return -1;
      if (redundantIP) free(redundantIP);
      redundantIP=NULL;
   }
   else
   {
      if (isHotBackup)
      {
   	   oapc_thread_sleep(5000);
         if (!mirrorData(NULL)) return -1;
      }
      if ((startHotBackup) && (!dSystemExit))
      {
   	   char cmd[1000];
   	
      	sprintf(cmd,"%s -a -b &",appName);
      	system(cmd);
      }
   }
#endif

  	g_handle=oapc_ispace_get_instance();
  	if (!g_handle) return -1;
   if ((!dSystemExit) && (mainLoop(hInstance)))
   {
      #ifdef ENV_WINDOWS
      while ((!dSystemExit) && (!LeaveServer()))
      {
         oapc_thread_sleep(100);
      }
      #endif
   }
   #ifdef ENV_WINDOWS
   WSACleanup();
   #endif
   #ifdef ENV_POSIX
   if (verbose) printf("Exiting mainloop...\n");
   #endif

   #ifdef ENV_POSIX
   if (verbose) printf("Releasing buffered data...\n");
   #endif
   showLog("Exiting server!");
#ifdef _DEBUG
   writeLog("Exiting server!");
   if (FHandle) fclose(FHandle);
#endif

   freeMem();
   return 0;
}

