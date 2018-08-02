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

#include <string.h>
#include <stdio.h>
#include <math.h>
#include <assert.h>

#ifdef ENV_WINDOWS
 #undef _UNICODE
 // #include <winsock2.h>

 /** WinSock 2 extension -- manifest constants for shutdown(), avoids inclusion of whole winsock2.h */
 #define SD_RECEIVE      0x00
 #define SD_SEND         0x01
 #define SD_BOTH         0x02

 #define MSG_NOSIGNAL 0
#else
 #include <sys/socket.h>
 #include <sys/types.h>
 #include <sys/ioctl.h>
 #include <netinet/in.h>
 #include <arpa/inet.h>
 #include <unistd.h>
 #include <netdb.h>
#endif

#ifndef ENV_WINDOWSCE
 #include <fcntl.h>
 #include <errno.h>
#else
 #ifndef EAGIAN
  #define EAGAIN 11 // really? there is no errno.h for WinCE
 #endif
#endif


#ifdef ENV_QNX
 #define MSG_NOSIGNAL 0
#endif
 
#include "liboapc.h"

/**
Receive data from a socket connection
@param[in] sock the identifier of the opened socket connection
@param[in] data the memory area where the received data have to be stored into
@param[in] len the maximum length of data that have to be read
@param[in] termStr an optional termination string; when this value is not NULL and the character
           defined here is received the function returns
@param[in] msecs when this time is exceeded the function returns also if not all data could
           be read; this parameter is valid only in case the socket is non-blocking
*/
OAPC_EXT_API int oapc_tcp_recv(int sock,char *data, int len,const char *termStr,long msecs) 
{
   long /*size_t*/   rc;
   long     ctr=0,readLen=0;
   #ifdef ENV_WINDOWS
   long     err;
   #endif

   // data from source client side
   memset(data,0,len);
   while ((readLen<len) && (ctr<msecs))
   {
      rc = recv(sock,data+readLen,len-readLen,MSG_NOSIGNAL);
      if (rc>0)
      {
         readLen+=rc;
         if (readLen>=len) return readLen;
         if (termStr)
         {
            if (readLen+1<len) data[readLen+1]=0;
            if (strstr(data,termStr))
            {
               return readLen;
            }
         }
         if (readLen>=len)
         {
            return readLen;
         }
      }
      else if ((rc==0) && (readLen<=0))
      {
#ifdef ENV_WINDOWS
         SetLastError(WSAECONNRESET);
#else
         errno=ECONNRESET;
#endif
         return readLen;
      }
      else // rc==-1
      {
#ifdef ENV_WINDOWS
         err=GetLastError();
         if ((err!=EAGAIN) && (err!=WSAEWOULDBLOCK) && (err!=0))
#else
         if ((errno!=EAGAIN) && (errno!=EINPROGRESS) && (errno!=0))
#endif
         {
            return readLen;
         }
         ctr+=10;
         oapc_thread_sleep(10);
      }
   }
   return readLen;
}



/**
Receive data from a socket connection
@param[in] sock the identifier of the opened socket connection
@param[in] msg the data that have to be send
@param[in] len the length of the data
@param[in] flags optional data transmission flags, must be MSG_NOSIGNAL for Linux
@param[in] msecs when this time is exceeded the function returns also if not all data could
           be sent; this parameter is valid only in case the socket is non-blocking
*/
OAPC_EXT_API int oapc_tcp_send(int sock, const char *msg,int len,int msecs)
{
   int    rlen=0;
   int    ctr=0,val;
#ifdef ENV_WINDOWS
   int    errno;
#else

   errno=0;
#endif
   while ((rlen<len) && (ctr<msecs))
   {
#ifdef ENV_LINUX
      val=send(sock,msg+rlen,len-rlen,MSG_NOSIGNAL);
#else
      val=send(sock,msg+rlen,len-rlen,0);
#endif
      if (val>=0) rlen+=val;
      else
      {
#ifndef ENV_WINDOWS
         if (errno==EAGAIN) ctr-=2; // in case of eagain we expect a longer send-timeout
#else
         errno=WSAGetLastError();
         if ((errno==EAGAIN) || (errno==WSAEWOULDBLOCK)) ctr-=2; // in case of eagain we expect a longer send-timeout
#endif
         else if (errno!=0)
         {
            rlen=-1;
            break;
         }
#ifndef ENV_WINDOWS
         errno=0;
#endif
      }
      if (rlen<len)
      {
         oapc_thread_sleep(2);
         ctr+=2;
      }
      if ((rlen==0) && (ctr>msecs/2)) break;
   }
   return rlen;
}



/**
Closes an opened socket connection
@param[in] sock the socket that has to be closed
*/
OAPC_EXT_API void oapc_tcp_closesocket (int sock)
{
   struct linger so_linger;
   int           z;

   oapc_tcp_set_blocking(sock,1);
   so_linger.l_onoff=1;
   so_linger.l_linger=10;
   z=setsockopt(sock,SOL_SOCKET,SO_LINGER,(const char*)&so_linger,sizeof so_linger);

#ifdef ENV_WINDOWS
//   shutdown(sock,SD_SEND);
//   shutdown(sock,SD_RECEIVE);
   closesocket(sock);
#else
//   shutdown(sock,SHUT_WR);
//   shutdown(sock,SHUT_RD);
   if (close (sock)<0) perror("close failed");
#endif
}



/**
Tries to establish a client connection to a (remote) server socket
@param[in] address address of the remote server in style a.b.c.d or www.domain.tld
@param[in] port number to connect with
@return the socket identifier of the established connection or a value <=0 in case of an
        error
*/
OAPC_EXT_API int oapc_tcp_connect_to(const char *address,unsigned short connect_port)
{
   struct sockaddr_in a;
   struct hostent *host;
#ifdef ENV_WINDOWS
   SOCKET          s;
#else
   int             s;
#endif
   unsigned long   nl;

   s = socket (AF_INET, SOCK_STREAM, 0);
   if (s<0)
   {
#ifndef ENV_WINDOWSCE
      perror("Can't create socket");
#endif
      return -1;
   }

   memset (&a, 0, sizeof (a));
   a.sin_port = htons (connect_port);
   a.sin_family = AF_INET;
   a.sin_addr.s_addr =inet_addr(address);
   if (a.sin_addr.s_addr==INADDR_NONE)
   {
      host = gethostbyname (address); // deprecated by posix standard
      if (host)
      {
         memcpy (&nl, host->h_addr, sizeof(unsigned long));
         a.sin_addr.s_addr = nl;
      }
      else
      {
#ifndef ENV_WINDOWSCE
         perror("Getting hostname");
#endif
         oapc_tcp_closesocket((int)s);
         return -1;
      }
   }
   if (connect (s, (struct sockaddr *) &a, sizeof (a)) < 0)
   {
#ifndef ENV_WINDOWSCE
      perror("No connection");
#endif
      oapc_tcp_closesocket((int)s);
      return -1;
   }
   return (int)s;
}



/**
Configures the blocking mode of an opened socket
@param[in] sock identifier of the socket to configure
@param[in] block 1 to set the socket to blocking mode, 0 to set it to non-blocking
*/
OAPC_EXT_API void oapc_tcp_set_blocking(int sock,char block)
{
   int flags;

#ifndef ENV_WINDOWS
   flags=fcntl(sock,F_GETFL, 0);
   if (block) flags&=~O_NONBLOCK;
   else flags|=O_NONBLOCK;
   fcntl(sock,F_SETFL, flags);
#else
   if (block) flags=0;
   else flags=13;
   ioctlsocket(sock,FIONBIO,(unsigned long*)&flags);
#endif
}



/**
This function can be used only with local server sockets, it tries to get and accept an
incoming client connection
@param[in] sock the server socket that has to be checked for an incoming connection
@param[out] the IP of the incoming connection, the value written here is undefined if there
            is no new connection
@return the socket identifier of the new incoming connection or a value <=0 of no new
        connection could be accepted
*/
OAPC_EXT_API int oapc_tcp_accept_connection (int sock, u_long *remote_ip)
{
   #ifdef ENV_WINDOWS
   int                len;
   #else
   socklen_t          len;
   #endif
#ifdef ENV_WINDOWS
   SOCKET             newsock;
#else
   int                newsock;
#endif
   struct sockaddr_in clientaddr;
   unsigned long      ip;

   len = sizeof (clientaddr);
   newsock = accept (sock, (struct sockaddr *) &clientaddr, &len);
   if (newsock >= 0)
   {
      ip = ntohl (clientaddr.sin_addr.s_addr);
      *remote_ip = ip;
      oapc_tcp_set_blocking((int)newsock,false);
      return (int)newsock;
   }
   return -1;
}



/**
Create a local server socket and set it to listen mode for incoming connections
@param[in] port the port number the server socket has to listen at
@param[in] bindToIP the IP the server socket has to be bound at
@return the socket identifier of the new server socket or a value <=0 if it could
        not be created
*/
OAPC_EXT_API int oapc_tcp_listen_on_port(unsigned short port,const char *bindToIP)
{
#ifdef ENV_WINDOWS
   SOCKET             sock;
#else
   int                sock;
#endif
   struct sockaddr_in name;
   int                bindval,listenval;
   unsigned long      op;

   sock = socket(AF_INET, SOCK_STREAM, 0);
   if (sock>0)
   {
      op=1;
      setsockopt(sock,SOL_SOCKET,SO_REUSEADDR,(char *)&op,sizeof(op)); 
      name.sin_family = AF_INET;
      name.sin_port = htons (port);   
      name.sin_addr.s_addr=inet_addr(bindToIP); // htonl (INADDR_ANY);
      bindval = bind(sock, (struct sockaddr *) &name, sizeof (name));
      if (bindval >= 0)
      {
         listenval = listen (sock,3);
         if (listenval>=0)
         {
            oapc_tcp_set_blocking((int)sock,false);
            return (int)sock;
         }
         else
         {
#ifndef ENV_WINDOWSCE
            perror("Can't listen on socket");
#endif
            return -1;
         }
      }
      else
      {
#ifndef ENV_WINDOWSCE
         perror("Can't bind socket to port");
#endif
         return -2;
      }
//      oapc_tcp_closesocket (sock);
   }
   else
   {
#ifndef ENV_WINDOWSCE
      perror("Can't create socket");
#endif
      return -3;
   }
//   return -4;
}
