/*
 *  $Id: ortemanager.c,v 0.0.0.1        2003/10/07
 *
 *  DEBUG:  section                     Manager
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

#ifndef CONFIG_ORTE_RT
//global variables
ORTEDomain          *d;
ORTEDomainProp      dp;
int32_t             opt,domain=ORTE_DEFAULT_DOMAIN;
Boolean             orteDaemon=ORTE_FALSE;
Boolean             orteWinService=ORTE_FALSE;
ORTEDomainAppEvents *events=NULL;

//event system
Boolean
onMgrAppRemoteNew(const struct ORTEAppInfo *appInfo, void *param) {
  printf("%s 0x%x-0x%x was accepted\n",
         (appInfo->appId & 0x3)==MANAGER ? "manager" : "application",
         appInfo->hostId,appInfo->appId);
  return ORTE_TRUE;
}

void
onMgrAppDelete(const struct ORTEAppInfo *appInfo, void *param) {
  printf("%s 0x%x-0x%x was deleted\n",
         (appInfo->appId & 0x3)==MANAGER ? "manager" : "application",
         appInfo->hostId,appInfo->appId);
}

#ifdef _WIN32
//Windows service support
void serviceDispatchTable(void);  //forward declaration
void removeService(void);         //forward declaration
void installService(void);        //forward declaration
int managerInit(void) {
  d=ORTEDomainMgrCreate(domain,&dp,events,ORTE_TRUE);
  if (d==NULL) return -1;
  return 0;
}
int managerStart(void) {
  ORTEDomainStart(d,ORTE_TRUE,ORTE_FALSE,ORTE_FALSE,ORTE_FALSE,ORTE_TRUE);
  return 0;
}
int managerStop(void) {
  ORTEDomainMgrDestroy(d);
  return 0;
}
#endif

#ifdef CONFIG_ORTE_UNIX
//Unix daemon support
pthread_mutex_t     mutex; //for wake up
pthread_cond_t	    cond; //for wake up
int		    cvalue;
void sig_usr(int signo) {
  if ((signo==SIGTERM) || (signo==SIGINT)) {
    pthread_mutex_lock(&mutex);
    cvalue=1;
    pthread_cond_signal(&cond);
    pthread_mutex_unlock(&mutex);
  }
}
void waitForEndingCommand(void) {
  pthread_mutex_init(&mutex, NULL);
  pthread_cond_init(&cond, NULL);
  cvalue=0;
  signal(SIGTERM,sig_usr);
  signal(SIGINT,sig_usr);
  pthread_mutex_lock(&mutex);
  while(cvalue==0)
    pthread_cond_wait(&cond,&mutex);
  pthread_mutex_unlock(&mutex);
  pthread_mutex_destroy(&mutex);
  pthread_cond_destroy(&cond);
}
static int daemonInit(void) {
  pid_t pid;

  if ((pid = fork()) < 0) {
    return -1;
  } else
    if (pid != 0) {
      exit(0);	/* parent vanishes */
    }
  /* child process */
  setsid();
  umask(0);
  close(0);
  close(1);
  close(2);
  return 0;
}
#endif

static void usage(void) {
  printf("usage: ortemanager <parameters> \n");
  printf("  -p, --peer <IPAdd:IPAdd:...>  possible locations of fellow managers\n");
  printf("  -k, --key  <IPAdd:IPAdd:...>  manualy assigned manager's keys\n");
  printf("  -d, --domain <domain>         working manager domain\n");
  printf("  -v, --verbosity <level>       set verbosity level SECTION, up to LEVEL:...\n");
  printf("      examples: ORTEManager -v 51.7:32.5 sections 51 and 32\n");
  printf("                ORTEManager -v ALL.7     all sections up to level 7\n");
  printf("  -R, --refresh <s>             refresh period in second(s)\n");
  printf("  -P, --purge <s>               purge time in second(s)\n");
  printf("  -I, --metaMulticast <IPAdd>   use multicast IPAddr for metatraffic comm.\n");
  printf("  -t, --timetolive <number>     time-to-live for multicast packets\n");
  printf("  -L, --listen <IPAdd>          IP address to listen on\n");
#ifdef CONFIG_ORTE_UNIX
  printf("  -D, --daemon                  start program like daemon\n");
#endif
  printf("  -E, --expiration <s>          expiration time of manager in second(s)\n");
  printf("  -e, --events                  register event system\n");
  printf("  -l, --logfile <filename>      set log file name\n");
  printf("  -V, --version                 show version\n");
#ifdef _WIN32
  printf("  -i, --install_service         install service into service manager on Windows\n");
  printf("  -r, --remove_service          remove service from service manager on Windows\n");
#endif
  printf("  -h, --help                    this usage screen\n");
}

int main(int argc,char *argv[]) {
#if defined HAVE_GETOPT_LONG || defined HAVE_GETOPT_LONG_ORTE
  static struct option long_opts[] = {
    { "peer",1,0, 'p' },
    { "key",1,0, 'k' },
    { "domain",1,0, 'd' },
    { "verbosity",1,0, 'v' },
    { "refresh",1,0, 'R' },
    { "purge",1,0, 'P' },
    { "metaMulticast",1,0, 'I' },
    { "timetolive",1,0, 't' },
    { "listen",1,0, 'L' },
#ifdef CONFIG_ORTE_UNIX
    { "daemon",1,0, 'D' },
#endif
    { "expiration",1,0, 'E' },
    { "events",0,0, 'e' },
    { "logfile",1,0, 'l' },
    { "version",0,0, 'V' },
    { "install_service",0,0, 'i' },
    { "remove_service",0,0, 'r' },
    { "help",  0, 0, 'h' },
    { 0, 0, 0, 0}
  };
#endif

  ORTEInit();
  ORTEDomainPropDefaultGet(&dp);

#if defined HAVE_GETOPT_LONG || defined HAVE_GETOPT_LONG_ORTE
  while ((opt = getopt_long(argc, argv, "k:p:d:v:R:E:P:I:t:L:l:VhDesir",&long_opts[0], NULL)) != EOF) {
#else
  while ((opt = getopt(argc, argv, "k:p:d:v:R:E:P:I:t:L:l:VhDesir")) != EOF) {
#endif
    switch (opt) {
      case 'p':
        dp.mgrs=optarg;
        break;
      case 'k':
        dp.keys=optarg;
        break;
      case 'd':
        domain=strtol(optarg,NULL,0);
        break;
      case 'v':
        ORTEVerbositySetOptions(optarg);
        break;
      case 'R':
        NtpTimeAssembFromMs(dp.baseProp.refreshPeriod,strtol(optarg,NULL,0),0);
        break;
      case 'P':
        NtpTimeAssembFromMs(dp.baseProp.purgeTime,strtol(optarg,NULL,0),0);
        break;
      case 'I':
        dp.multicast.enabled=ORTE_TRUE;
        dp.multicast.ipAddress=StringToIPAddress(optarg);
        break;
      case 'L':
        dp.listen=StringToIPAddress(optarg);
        break;
      case 't':
        dp.multicast.ttl=strtol(optarg,NULL,0);
        break;
      case 'E':
        NtpTimeAssembFromMs(dp.baseProp.expirationTime,strtol(optarg,NULL,0),0);
        break;
      case 'e':
        events=(ORTEDomainAppEvents*)malloc(sizeof(ORTEDomainAppEvents));
        ORTEDomainInitEvents(events);
        events->onMgrNew=onMgrAppRemoteNew;
        events->onAppRemoteNew=onMgrAppRemoteNew;
        events->onMgrDelete=onMgrAppDelete;
        events->onAppDelete=onMgrAppDelete;
        break;
      case 'l':
        ORTEVerbositySetLogFile(optarg);
        break;
      case 'V':
        printf("Open Real-Time Ethernet (%s).\n",dp.version);
        exit(0);
        break;
      case 'D':
        orteDaemon=ORTE_TRUE;
        break;
      #ifdef _WIN32
      case 's':
        serviceDispatchTable();
        exit(0);
        break;
      case 'i':
        installService();
        orteWinService=ORTE_TRUE;
        break;
      case 'r':
        removeService();
        exit(0);
        break;
      #endif
      case 'h':
      default:
        usage();
        exit(opt == 'h' ? 0 : 1);
    }
  }
  
  if (orteWinService) { 
    exit(0);
  }
  
  d=ORTEDomainMgrCreate(domain,&dp,events,ORTE_TRUE);
  if (!d)
    exit(1);

  #ifdef CONFIG_ORTE_UNIX
  if (orteDaemon)
    daemonInit();
  #endif

  ORTEDomainStart(d,ORTE_TRUE,ORTE_FALSE,ORTE_FALSE,ORTE_FALSE,ORTE_TRUE);
  #ifndef CONFIG_ORTE_UNIX
     while(1) ORTESleepMs(1000);
  #endif

  #ifdef CONFIG_ORTE_UNIX
  waitForEndingCommand();
  ORTEDomainMgrDestroy(d);
  if (events) 
    free(events);
  #endif

  exit(0);
}
#else
char *verbosity="";
MODULE_PARM(verbosity,"1s");
MODULE_PARM_DESC(verbosity,"set verbosity level SECTION, up to LEVEL:...");
char *peer="";
MODULE_PARM(peer,"1s");
MODULE_PARM_DESC(peer,"possible locations of fellow managers");
MODULE_LICENSE("GPL");
ORTEDomain *d=NULL;
pthread_t  thread;

void *
domainInit(void *arg) {
  ORTEDomainProp dp;

  ORTEDomainPropDefaultGet(&dp);
  ORTEVerbositySetOptions(verbosity);
  dp.mgrs=peer;
  d=ORTEDomainMgrCreate(ORTE_DEFAULT_DOMAIN,&dp,NULL,ORTE_TRUE);
  return arg;
}

void *
domainDestroy(void *arg) {
  if (!d) return NULL;
  ORTEDomainMgrDestroy(d);
  return arg;
}

int
init_module(void) {
  ORTEInit();
  pthread_create(&thread,NULL,&domainInit,NULL);  //allocate resources in RT 
  pthread_join(thread,NULL);
  if (d)
    ORTEDomainStart(d,ORTE_TRUE,ORTE_FALSE,ORTE_FALSE,ORTE_FALSE,ORTE_TRUE); //manager start
  return 0;
}
void
cleanup_module(void) {
  if (!d) return;
  pthread_create(&thread,NULL,&domainDestroy,NULL);
  pthread_join(thread,NULL);
}
#endif
