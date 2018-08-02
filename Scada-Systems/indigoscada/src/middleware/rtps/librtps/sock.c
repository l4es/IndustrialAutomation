/*
 *  $Id: sock.c,v 0.0.0.1               2003/08/21 
 *
 *  DEBUG:  section 6                   Socket 
 *
 *  -------------------------------------------------------------------  
 *                                ORTE                                 
 *                      Open Real-Time Ethernet                       
 *                                                                    
 *                      Copyright (C) 2001-2006                       
 *  Department of Control Engineering FEE CTU Prague, Czech Republic  
 *                      http://dce.felk.cvut.cz                       
 *                      http://www.ocera.org                          
 *                                                                    
 *  Author: 		 Petr Smolik	petr.smolik@wo.cz             
 *  Advisor: 		 Pavel Pisa                                   
 *  Project Responsible: Zdenek Hanzalek                              
 *  --------------------------------------------------------------------
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *  
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *  
 */ 

#include "orte_all.h"

/*********************************************************************/
int
sock_start(void) {
#if defined(SOCK_BSD) || defined (SOCK_RTLWIP)
  return 0;
#elif defined (SOCK_WIN)
  WORD wVersionRequested;
  WSADATA wsaData;
  #ifdef SOCK_WIN_PHARLAP
    wVersionRequested = MAKEWORD(1, 1);
  #else
    wVersionRequested = MAKEWORD(2, 0);
  #endif
  return WSAStartup(wVersionRequested, &wsaData);
#endif
}

/*********************************************************************/
inline void
sock_finish(void) {
#if defined(SOCK_WIN)
  WSACleanup();
#endif
}

/*********************************************************************/
int
sock_init_udp(sock_t *sock) {
  sock->fd = socket(AF_INET, SOCK_DGRAM, 0);
  if (sock->fd < 0) return -1;
  return 0;
}

/*********************************************************************/
inline void
sock_cleanup(sock_t *sock) {
#if defined(SOCK_BSD)
  close(sock->fd);
#elif defined(SOCK_RTLWIP)
  close_socket_np(sock->fd);
#elif defined(SOCK_WIN)
  closesocket(sock->fd);
#endif
}

/*********************************************************************/
inline int
sock_setsockopt(sock_t *sock,int level,int optname,const char *optval, int optlen) {
  if (setsockopt(sock->fd, level, optname,(void *)optval, optlen)) {
    sock_cleanup(sock);
    return -1;
  }
  return 0;
}

#ifdef _MSC_VER
#define socklen_t unsigned int
#endif

/*********************************************************************/
inline int
sock_getsockopt(sock_t *sock,int level,int optname,char *optval, int *optlen) {
  if (getsockopt(sock->fd, level, optname,(void *)optval, (socklen_t *)optlen)) {
    sock_cleanup(sock);
    return -1;
  }
  return 0;
}

/*********************************************************************/
int
sock_bind(sock_t *sock,uint16_t port, IPAddress listen) {
  struct sockaddr_in name;
  int size;

  name.sin_family = AF_INET;
  name.sin_port = htons(port);
  name.sin_addr.s_addr = htonl(listen);
  if (bind(sock->fd, 
          #ifndef CONFIG_ORTE_RTL_ONETD 
            (struct sockaddr *)
          #endif
          &name, sizeof(name)) < 0) {
    sock_cleanup(sock);
    return -1;
  }
  size = sizeof(name);
  if (getsockname(sock->fd,
         #ifndef CONFIG_ORTE_RTL_ONETD 
  	   (struct sockaddr *)
         #endif
	 &name, 
         #ifndef CONFIG_ORTE_RTL_ONETD 
           (socklen_t *)&size
         #else
	   size 
         #endif
	 ) < 0) {
    sock_cleanup(sock);
    return -1;
  }
  sock->port=ntohs(name.sin_port);
  return 0;
}

/*********************************************************************/
inline int
sock_recvfrom(sock_t *sock, void *buf, int max_len,struct sockaddr_in *des,int des_len) {
  return recvfrom(sock->fd, buf, max_len, 0,
    #ifndef CONFIG_ORTE_RTL_ONETD 
      (struct sockaddr*)
    #endif
    des,(socklen_t *)&des_len);
}

/*********************************************************************/
inline int
sock_sendto(sock_t *sock, void *buf, int len,struct sockaddr_in *des,int des_len) {
  return sendto(sock->fd, buf, len, 0,
    #ifndef CONFIG_ORTE_RTL_ONETD 
      (struct sockaddr*)
    #endif
    des,des_len);
}

/*********************************************************************/
inline int
sock_ioctl(sock_t *sock, long cmd, unsigned long *arg) {
  return ioctl(sock->fd, cmd, arg);
}

/*********************************************************************/
int
sock_get_local_interfaces(sock_t *sock,ORTEIFProp *IFProp,char *IFCount) {
#if defined(SOCK_BSD)
  struct ifconf           ifc;
  char                    buf[MAX_INTERFACES*sizeof(struct ifreq)];
  char                    *ptr;

  ifc.ifc_len = sizeof(buf);
  ifc.ifc_buf = buf;
  *IFCount=0;
  if (ioctl(sock->fd, SIOCGIFCONF, &ifc) < 0) return -1;
  for (ptr = buf; ptr < (buf + ifc.ifc_len);ptr += sizeof(struct ifreq)) {
    struct ifreq*     ifr = (struct ifreq*) ptr;
    struct sockaddr   addr;
    memcpy(&addr, &ifr->ifr_addr, sizeof(addr));
    ioctl(sock->fd, SIOCGIFFLAGS, ifr);
    if ((ifr->ifr_flags & IFF_UP) && !(ifr->ifr_flags & IFF_LOOPBACK)) {
      (*IFCount)++;
      IFProp->ifFlags=ifr->ifr_flags;
      IFProp->ipAddress=ntohl(((struct sockaddr_in*)&addr)->sin_addr.s_addr);
      IFProp++;
    }
  }
  return 0;
#elif defined(SOCK_RTLWIP)
  /* loopback iface is recognized if it has this address */
  char ip_address [] = "127.0.0.1";
  struct in_addr loopaddr;
  int i;

  *IFCount=0;
  if (inet_aton(ip_address, &loopaddr) != 0) return -1;
  
  for (i = 0; i < NIC_TABLE_SIZE; i++) {
    if (nic_table [i].nic_struct != NULL) {
      if (nic_table[i].ipad.s_addr != loopaddr.s_addr) {
	(*IFCount)++;
	IFProp->ifFlags=0; //RT-Linux doesn't flags
	IFProp->ipAddress=ntohl(nic_table[i].ipad.s_addr);
	IFProp++;
      }
    }
  }
  return 0;
#elif defined(SOCK_WIN_PHARLAP)
  DEVHANDLE hDev;
  EK_TCPIPCFG *pCfg;
  union {
    EK_TCPETHSTATUS eth;
    EK_TCPSLIPSTATUS slip;
    EK_TCPPPPSTATUS ppp;
  } status;
  *IFCount = 0;
  hDev = NULL;

  while (hDev = EtsTCPIterateDeviceList(hDev)) {
    pCfg = EtsTCPGetDeviceCfg(hDev);

    if (pCfg->nwIPAddress == 0x0100007F) // 127.0.0.1 localhost
      continue;

    status.eth.length = sizeof(EK_TCPETHSTATUS);
    EtsTCPGetDeviceStatus(hDev, &status);
    if (status.eth.DevStatus.Flags & ETS_TCP_DEV_ONLINE) {
      IFProp->ifFlags = IFF_UP;
      IFProp->ipAddress = ntohl(pCfg->nwIPAddress);
      (*IFCount)++;
    }
  }
  return 0;
#elif defined(SOCK_WIN)
  INTERFACE_INFO      InterfaceList[MAX_INTERFACES];
  struct sockaddr_in* pAddress;
  unsigned long       len,i;

  *IFCount=0;
  if (WSAIoctl(sock->fd,SIO_GET_INTERFACE_LIST,NULL,0,
               InterfaceList, sizeof(InterfaceList),
               &len, NULL, NULL)==SOCKET_ERROR) return -1;
  len=len/sizeof(INTERFACE_INFO);
  for(i=0;i<len;i++) {
    long  nFlags;
    pAddress = (struct sockaddr_in*)&(InterfaceList[i].iiAddress);
    nFlags = InterfaceList[i].iiFlags;
    if ((nFlags & IFF_UP) && !(nFlags & IFF_LOOPBACK)) {
      IFProp->ifFlags=nFlags;
      IFProp->ipAddress=ntohl(pAddress->sin_addr.s_addr);
      IFProp++;
      (*IFCount)++;
    }
  }
  return 0;
#endif
}


