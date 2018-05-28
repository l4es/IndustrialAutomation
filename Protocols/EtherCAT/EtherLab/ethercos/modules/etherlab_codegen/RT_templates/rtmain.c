/******************************************************************************
 *
 *  Copyright (C) 2008-2009  Andreas Stewering-Bone, Ingenieurgemeinschaft IgH
 *
 *  This file is part of the IgH EtherLAB Scicos Toolbox.
 *  
 *  The IgH EtherLAB Scicos Toolbox is free software; you can
 *  redistribute it and/or modify it under the terms of the GNU Lesser General
 *  Public License as published by the Free Software Foundation; version 2.1
 *  of the License.
 *
 *  The IgH EtherLAB Scicos Toolbox is distributed in the hope that
 *  it will be useful, but WITHOUT ANY WARRANTY; without even the implied
 *  warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with the IgH EtherLAB Scicos Toolbox. If not, see
 *  <http://www.gnu.org/licenses/>.
 *  
 *  ---
 *  
 *  The license mentioned above concerns the source code only. Using the
 *  EtherCAT technology and brand is only permitted in compliance with the
 *  industrial property and similar rights of Beckhoff Automation GmbH.
 *
 *****************************************************************************/
 
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <sched.h>
#include <malloc.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <getopt.h>
#include <sys/mman.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <linux/rtc.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <fcntl.h>
#include <errno.h>
#include <math.h>
#include <ctype.h>
//MSR Preincludes
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <pthread.h>

//MSR-Includes
#include "rt_lib/include/msr_lists.h"
#include "rt_lib/include/msr_main.h"
#include "rt_lib/include/msr_reg.h"



//#define DEBUG

#define ETHERLAB_VERSION "0.0.2"


//MSR-MACROS
//max count of connected clients
#define MAXFD 20


#define MAX_SAFE_STACK (5*1024*1024) /* 5Mb, The maximum stack size which is
                                   guranteed safe to access without
                                   faulting */

#define PREGETMEM (10*1024*1024) //10Mb for MSR Main Loop



//Define Clock Source
#define CLOCKID CLOCK_REALTIME

//Comedi
#define MAX_COMEDI_DEVICES      11
#define MAX_COMEDI_COUNTERS      8

void *ComediDev[MAX_COMEDI_DEVICES];
int ComediDev_InUse[MAX_COMEDI_DEVICES] = {0};
int ComediDev_AIInUse[MAX_COMEDI_DEVICES] = {0};
int ComediDev_AOInUse[MAX_COMEDI_DEVICES] = {0};
int ComediDev_DIOInUse[MAX_COMEDI_DEVICES] = {0};
int ComediDev_CounterInUse[MAX_COMEDI_DEVICES][MAX_COMEDI_COUNTERS] = {{0}};


//Model macros
#define XNAME(x,y)  x##y
#define NAME(x,y)   XNAME(x,y)




//Model Functions
int NAME(MODEL,_init)(void);
int NAME(MODEL,_isr)(double);
int NAME(MODEL,_end )(void);

//Model help functions
double NAME(MODEL,_get_tsamp)(void);
double NAME(MODEL,_get_tsamp_delay)(void);



//global variables
int CpuMap;
double FinalTime;
int stackinc;
int priority;
int verbose;
int DoSoft;
long int cycletime;
int networkport;

//Timing values
struct timespec time_goal;
struct timespec time_last;
struct timespec time_relwakeup;
double SpareTime;  
struct timespec acttime,acttime_last;
struct timespec timerres;
struct timespec cycletimespec;
long int timediff;
double process_time_dbl;
struct timeval process_time;
long int msr_jiffies;
long int exec_time;
long int call_time;
double model_t0;
double model_t;
int overruns;
int max_overruns;


/* determine if the C library supports Priority Inheritance mutexes */
//#if defined(_POSIX_THREAD_PRIO_INHERIT) && _POSIX_THREAD_PRIO_INHERIT != -1
//#define HAVE_PI_MUTEX 1
//#else
//#define HAVE_PI_MUTEX 0
//#endif

//int use_pi_mutex = HAVE_PI_MUTEX;

//Threading utilities
//pthread_mutex_t mutex_state;
//pthread_mutexattr_t mutex_attr;
int msr_thread_end; // Flag for thread control

//CPU-Set 
cpu_set_t cpu_mask;


struct option options[] = {
  { "usage",      0, 0, 'u' },
  { "verbose",    0, 0, 'v' },
  { "soft",       0, 0, 's' },
  { "priority",   1, 0, 'p' },
  { "finaltime",  1, 0, 'f' },
  { "cpumap",     1, 0, 'c' },
  { "stack",      1, 0, 'm' },
  { "networkport",1, 0, 'n' },
  { "maxoveruns" ,1, 0, 'o' },
  { "runcycle",   1, 0, 'r' }
};

//syslog utils
/*#define exit_if(expr) \
  if(expr) { \
  syslog(LOG_WARNING, "exit_if() %s: %d: %s: Error %s\n", \
  __FILE__, __LINE__, __PRETTY_FUNCTION__, strerror(errno)); \
  exit(1); \
  }*/

//Scicos Time function
double get_scicos_time(void)
{
  return(model_t);
}

//Scicos Block Number Funtion
int get_block_number(void)
{
  return 0;
}

//Dummy End Scicos Sim
void end_scicos_sim(void)
{
  return;
}

/*Functions for time handling */

double sec_time(struct timespec mytime)
{
  return ((double)mytime.tv_sec+((double)mytime.tv_nsec/1000000000.0));
}


long int n_time()
{
  struct timespec mytime;

  clock_gettime(CLOCKID, &mytime);
  return (1000000000*mytime.tv_sec+mytime.tv_nsec);
}

struct timeval my_gettimeval(struct timespec mytime)
{
  struct timeval result;
  result.tv_sec = mytime.tv_sec;
  result.tv_usec = mytime.tv_nsec / 1000;
  return result;
}


long int mu_only_time(struct timespec mytime)
{
  return mytime.tv_nsec / 1000;
}

int sign(int a)
{
  if (a>=0)
    return 1;
  else
    return -1;
}

long int timeval2li(struct timeval t)
{

  long int result;
  result = (long int)t.tv_sec * 1000000 + t.tv_usec;
  return result;

}/* timeval2li */

long int timespec2li(struct timespec t)
{

  long int result;
  result = (long int)t.tv_sec * 1000000000 + t.tv_nsec;
  return result;

}/* timeval2li */

struct timespec li2timespec(long int t)
{

  struct timespec result;
  result.tv_sec = t / 1000000000;
  result.tv_nsec = t % 1000000000;
  return result;

} /* li2timespec */



/*Functions for time handling */


struct timespec addtime(struct timespec time1, struct timespec time2)
{
  struct timespec result;
  if((time1.tv_nsec + time2.tv_nsec) >= 1e9)
    {
      result.tv_sec = time1.tv_sec + time2.tv_sec + 1;
      result.tv_nsec = time1.tv_nsec +time2.tv_nsec -1e9;
    }
  else
    {
      result.tv_sec = time1.tv_sec + time2.tv_sec ;
      result.tv_nsec = time1.tv_nsec +time2.tv_nsec;
    }
  return result;
}




struct timespec difftimespec(struct timespec start, struct timespec end)
{
  struct timespec temp;
  if ((end.tv_nsec-start.tv_nsec)<0) {
    temp.tv_sec = end.tv_sec-start.tv_sec-1;
    temp.tv_nsec = 1000000000+end.tv_nsec-start.tv_nsec;
  } else {
    temp.tv_sec = end.tv_sec-start.tv_sec;
    temp.tv_nsec = end.tv_nsec-start.tv_nsec;
  }
  return temp;
}


void preget_mem(void) {
  int pagesize;
  int i;
  char * buffer;
  int size = PREGETMEM + stackinc;
  pagesize = sysconf(_SC_PAGESIZE);
  buffer = malloc(size);
  for(i=0;i< size; i+=pagesize)
    {
      buffer[i]=0;
    }
  free(buffer);
}


static void endme(int dummy)
{
  signal(SIGINT, endme);
  signal(SIGTERM, endme);
  msr_thread_end = 1;
}

void exit_on_error(void)
{
  endme(0);
}

void print_usage(void)
{
  fputs(
	("\nUsage:  'RT-model-name' [OPTIONS]\n"
	 "\n"
	 "OPTIONS:\n"
	 "  -u, --usage\n"
	 "      print usage\n"
	 "  -v, --verbose\n"
	 "      verbose output\n"
	 "  -V, --version\n"
         "      print rtmain version\n"
	 "  -s, --soft\n"
	 "      run RT-model in soft mode \n"
	 "  -p <priority>, --priority <priority>\n"
	 "      set the priority at which the RT-model's highest priority task will run (default 0)\n"
	 "  -f <finaltime>, --finaltime <finaltime>\n"
	 "      set the final time (default infinite)\n"
	 "  -c <cpumap>, --cpumap <cpumap>\n"
	 "      (1 << cpunum) on which the RT-model runs (default: all cpus)\n"
	 "  -m <stack>, --stack <stack>\n"
	 "      set a guaranteed stack size extension (default 30000)\n"
	 "  -n <port>, --networkport <port>\n"
	 "      set the MSR-Networkport (default 2345)\n"
	 "  -o <number>, --maxoverruns <number>\n"
	 "      set max allowed time overruns (default 10)\n"
	 "  -r <num>, --runcycle <num>\n"
	 "      set the model cycle time in mues (default modelspecific)\n"
	 "\n")
	,stderr);
  exit(0);
}

//Get Process Pagefaults
void show_new_pagefault_count(void)
{
  static int major_flt = 0, minor_flt = 0;
  struct rusage usage;

  getrusage(RUSAGE_SELF,&usage);

  printf("Pagefaults Major %ld (Sum %ld), Minor %ld (Sum %ld) \n",usage.ru_majflt-major_flt, major_flt,usage.ru_minflt-minor_flt, minor_flt);
  major_flt = usage.ru_majflt;
  minor_flt = usage.ru_minflt;
}


//CPUSet Handling
int set_cpu_affinity(cpu_set_t *cpu_set, int num_proc)
{
	int status, i;
	cpu_set_t current_mask, new_mask;
	if(num_proc == 0)//Failure, no valid cpu selected
	  {
	    printf("set_cpu_affinity: No valid schedulable CPU selected!\n");
	    return -1;
	  }

	if ((num_proc > 0 )&&(num_proc >= sizeof(cpu_set_t) * 8)) {
	  printf("set_cpu_affinity: No valid schedulable CPU selected!\n");
	  return -1;
	}



	if (num_proc < 0) //Default, Limit Process to ther first selectable CPU
	  {
	    /* Now set our CPU affinity to only run on one processor */
	    status = sched_getaffinity(0, sizeof(cpu_set_t), &current_mask);
	    if (status) {
	      printf("set_cpu_affinity: getting CPU affinity mask: 0x%x\n", status);
	      return -1;
	    }
	    //for (i = 0; i < sizeof(cpu_set_t) * 8; i++) {
	    //  if (CPU_ISSET(i, &current_mask))
	    //	break;
	    //}
	    //if (i >= sizeof(cpu_set_t) * 8) {
	    //  printf("set_cpu_affinity: No schedulable CPU found!\n");
	    //  return -1; 
	    //}
	    //num_proc = i;
	    new_mask = current_mask;
	  }
	if(num_proc > 0) //CPU selected by user
	  {
	    CPU_ZERO(&new_mask);
	    CPU_SET(num_proc, &new_mask);
	  }
	else
	  {
	    //Change nothing, take all available cpus
	  }
#ifdef DEBUG
	printf("set_cpu_affinity: using processr %d\n", num_proc);
#endif
	*cpu_set = new_mask;
	return 0;
}

//Set Thread Scheduler Option
pthread_attr_t set_thread_rt_opt(int priority, int policy, cpu_set_t *mask)
{
  pthread_attr_t result;
  struct sched_param thread_param;
  //Configure Thread Parameter
  if(pthread_attr_init(&result) !=0)
    {
      printf("Thread Attribut init failed\n");
      exit(-1);
    }
  if(pthread_attr_setinheritsched(&result, PTHREAD_EXPLICIT_SCHED)!=0)
    {
      printf("Setting explicit scheduling inheritance failed\n");
      exit(-1);
    }
  if(pthread_attr_setschedpolicy(&result, policy) !=0)
    {
      printf("Thread Attribut Policy failed\n");
      exit(-1);
    }
  if((priority <= sched_get_priority_max(policy))&&(priority >= sched_get_priority_min(policy)))
    {
      thread_param.sched_priority = priority; 
    }
  else
    {
      printf("Priority value not valid, select highest priority\n");
      thread_param.sched_priority = sched_get_priority_max(policy); //highest 
    }
  if(pthread_attr_setschedparam(&result, &thread_param) !=0)
    {
      printf("Thread Attribut Priority failed\n");
      exit(-1);
    }
  if(pthread_attr_setstacksize(&result, MAX_SAFE_STACK + stackinc))
    {
      printf("Thread Attribut Stacksize failed\n");
      exit(-1);
    }
  // Setaffinity not supported in Debian Etch
  //  if(pthread_attr_setaffinity_np(&result, sizeof(cpu_set_t), mask));
  //{
  //  printf("Setting affinity attribute failed\n");
  //  exit(-1);
  //}
  return result;
}

//MSR_SERVER

int msr_server_init(int port)
/* Server (listen) Port oeffnen - nur einmal ausfuehren
 * in port: TCP Server Portnummer
 * return: Socket Filedescriptor zum Verbindungsaufbau vom Client
 */
{
  int listen_fd;
  int ret;
  struct sockaddr_in sock;
  int yes = 1;

  listen_fd = socket(PF_INET, SOCK_STREAM, 0);
  if(listen_fd <0 ){
    fprintf(stderr, "-> Unable to open socket.\n");
    exit(-1);
  }

  /* vermeide "Error Address already in use" Fehlermeldung */
  ret = setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));


  if(ret <0 ){
    fprintf(stderr, "-> Unable to set socket options.\n");
    exit(-1);
  }

  memset((char *) &sock, 0, sizeof(sock));
  sock.sin_family = AF_INET;
  sock.sin_addr.s_addr = htonl(INADDR_ANY);
  sock.sin_port = htons(port);

  ret = bind(listen_fd, (struct sockaddr *) &sock, sizeof(sock));
  if(ret != 0 ){
    fprintf(stderr, "-> Unable to bind socket.\n");
    exit(-1);
  }

  ret = listen(listen_fd, 5);
  if(ret <0 ){
    fprintf(stderr, "-> Unable to set up socket listening.\n");
    exit(-1);
  }

  return listen_fd;
}



int msr_server_clientaccept(int listen_fd)
{
  int fd;
  struct sockaddr_in sock;
  socklen_t socklen;

  socklen = sizeof(sock);
  fd = accept(listen_fd, (struct sockaddr *) &sock, &socklen);
  if(fd < 0){
    fprintf(stderr, "-> No valid client connect.\n");
    return fd;
  }
#ifdef DEBUG
  printf("MSR-Client connected\n");
#endif
  return fd;
}


void msr_server_loop(int listen_fd, int do_soft)
{
  //MSR Client FD
  fd_set readfds, the_state;
  int maxfd; //actual count of MSR Clients
  struct msr_dev *dev[listen_fd+MAXFD];
  int valid_clients[listen_fd+MAXFD];
  int rfd;
  struct sched_param schedparam;
  int i, ret, buflen;
  struct timeval waittime;
  struct timespec sleeptime;       /* Timeout for select() */
  sleeptime.tv_sec = 0;
  sleeptime.tv_nsec = 50000000; //20Hz
  waittime.tv_sec = 0;
  waittime.tv_usec = 20000; //50Hz

#ifdef DEBUG
  printf("MSR-Server start\n");
  printf("Listen FD =  %i \n",listen_fd);
#endif

  if(do_soft == 0) //Set higher priority for MSR-Server
    {
      memset(&schedparam, 0, sizeof(schedparam));
      schedparam.sched_priority = sched_get_priority_max(SCHED_OTHER); //maximum for scheduler 
      if(sched_setscheduler(0, SCHED_OTHER, &schedparam) < 0)
	{
	  printf("Can't set Process prioity\n Please run as Root\n");
	}
      else
	{
#ifdef DEBUG
	  printf("Set MSR-Server priority\n");
#endif
	}
    }

  //Reset FD Handler
  FD_ZERO(&the_state);
  FD_SET(listen_fd,&the_state);
  maxfd = listen_fd;

  for(i=0;i<(listen_fd+MAXFD);i++)
    {
      valid_clients[i] = 0;
    }

  //endless loop
  for(;;)
    {
      readfds = the_state;
      ret = select(maxfd+1, &readfds, NULL, NULL, &waittime);
      if ((ret == -1) && (errno == EINTR))
	{
	  /* ignore signals */
	  continue;
	}
      
      if(ret <0)
	{
	  exit(-1);
	  //exit_if(ret < 0);
	}
      else
	{
	  if (FD_ISSET(listen_fd, &readfds)) // New Client 	      
	    {
	      /* Check TCP Server LISTEN Port (Client connect)*/
	      rfd = msr_server_clientaccept(listen_fd);
	      if (rfd >= 0) 
		{
		  if (rfd >= (listen_fd+MAXFD))
		    {
#ifdef DEBUG
		      printf("Too many MSR Clients \n");
#endif
		      close(rfd);
		      continue;
		    }
		  else
		    {
		      /* add Client fd */
		      dev[rfd] = msr_open(rfd,rfd);  
		      FD_SET(rfd, &the_state);   
		      valid_clients[rfd] = 1;
#ifdef DEBUG
		      printf("Add MSR Client No %i \n",rfd);
#endif
		      if (rfd > maxfd)
			{
			  maxfd = rfd;
			}
		    }
		}
	    }

	  /* Check TCP Server CONNECT Ports (Clients communication)*/
	  for (rfd = listen_fd+1; rfd <= maxfd; rfd++) 
	    {

	      if (FD_ISSET(rfd, &readfds)) 
		{
#ifdef DEBUG
		  printf("Read MSR Client %i\n",readfds.fds_bits[rfd]);
#endif
		  //Lock Mutex
		  //pthread_mutex_lock(&mutex_state);
		  buflen = msr_read(dev[rfd]);
		  //Unlock Mutex
		  //pthread_mutex_unlock(&mutex_state);

#ifdef DEBUG
		  printf("Buflen =  %i\n",buflen);
#endif
		  if (buflen <= 0)
		    {
#ifdef DEBUG
		      printf("Close MSR Connection %i\n",rfd);
#endif
		      /* End of TCP Connection */
		      msr_close(dev[rfd]);
		      close(rfd);
		      FD_CLR(rfd, &the_state); 
		      valid_clients[rfd] = 0;
		    }
		}
	      if(valid_clients[rfd])
		{
		  while(msr_write(dev[rfd])!=0){}; // Send until send buffer empty
		}
	    }
	}
      if(msr_thread_end == 1)
	{
	  for (rfd = listen_fd+1; rfd <= maxfd; rfd++) 
	    {
	      if(valid_clients[rfd])
		{
		  msr_close(dev[rfd]);
		  close(rfd);
		  FD_CLR(rfd, &the_state); 
		  valid_clients[rfd] = 0;
		  
		}
	    }
	  break;
	}  
      nanosleep(&sleeptime,NULL); //Wait
    }
}





void model_exec_thread(void)
/* Main Model Loop*/
{
  char wait=1;

  //Workaround for old Linux Systems, in the future CPU-Mask is set by thread Attributes
  if(!DoSoft)
    {
      if(sched_setaffinity(0, sizeof(cpu_set_t), &cpu_mask))
      {
	printf("set_cpu_affinity: Setting CPU affinity mask failed\n");
	msr_thread_end = 1;
      }
    }
  

#ifdef DEBUG
  printf("Start Model Cycling Mode\n");
#endif
  clock_gettime(CLOCKID,&time_goal);
  time_last = time_goal;
  call_time =  cycletime / 1000;
  cycletimespec = li2timespec(cycletime);
  acttime_last = time_goal;
  process_time_dbl = model_t0;
  while(1) {

    //Lock Mutex
    //if(pthread_mutex_trylock(&mutex_state)==EBUSY)
    //  {
    //#ifdef DEBUG
    //	printf("Locking failed! Is ignored!\n");
    //#endif
    //  }


    
    clock_gettime(CLOCKID,&acttime);
    process_time_dbl = sec_time(acttime);
    process_time = my_gettimeval(acttime); 
    model_t = model_t+(double)cycletime*1e-9;
    
    
    //Call Model ISR
    exec_time = mu_only_time(acttime);
    NAME(MODEL,_isr)(model_t);
    clock_gettime(CLOCKID,&acttime);
    exec_time = mu_only_time(acttime) - exec_time;
    if (exec_time < 0)
      {
	exec_time = 0;
      }


    timediff = timespec2li(difftimespec(time_goal,acttime));
    msr_jiffies = timediff /1000;
    call_time = timespec2li(difftimespec(acttime_last,acttime))/1000;
    acttime_last = acttime;

    //Call MSR-Stuff
    msr_write_kanal_list();

    //Unlock Mutex
    //if(pthread_mutex_unlock(&mutex_state)==EPERM)
    //  {
    //	printf("Cannot unlock Mutex!\n");
    //  }
#ifdef DEBUG
    printf("Last Time s %i ns %i\n",time_goal.tv_sec,time_goal.tv_nsec);
    printf("Actual Time s %i ns %i\n",acttime.tv_sec,acttime.tv_nsec);
    printf("Call difftime %i ns\n",timediff);
    printf("Call Cyletime %i mus\n",call_time);
    printf("Execution Time %i mus\n",exec_time);
    printf("Model Time %f s\n",model_t);
#endif
    if (abs(timediff)>pow(2,30)) //overrun detected 
      {
	if (sign(timespec2li(acttime))==-1)
	  {
	    printf("Clock unstable at %f by %d ns\n",model_t,timediff); 
	    printf("Syncing Time.\n"); 
	    wait = 0;//Realtime failed
	    overruns++;
	    clock_gettime(CLOCKID,&time_last); //Resync Call Time
	  }
	else
	  {
	    wait=1;
	    SpareTime=0;//???
	  }
      }
    else 
      {
	if (abs(timediff) > cycletime)
	  {
	    wait=0;
	    overruns++;
	    SpareTime=0;
	  }
	else
	  {
	    /*if (timediff > 0)
	      {
		printf("Clock unstable at %f by %d ns\n",model_t,timediff); 
		printf("Syncing Time.\n"); 
		wait = 1;
		overruns++;
		clock_gettime(CLOCKID,&time_last); //Resync Call Time
	      }
	    else
	    {*/
	    wait=1; 
	    SpareTime = (double) 0.000001 * ((double) timediff);
		//}
	  }
      }
    //Set new wake up time
    time_goal=addtime(time_last,cycletimespec);
    time_relwakeup = difftimespec(acttime,time_goal);
#ifdef DEBUG
    printf("Last Time s %i ns %i\n",time_last.tv_sec,time_last.tv_nsec);
    printf("Actual Time absolut s %i ns %i\n",acttime.tv_sec,acttime.tv_nsec); 
    printf("Wake up Time absolut s %i ns %i\n",time_goal.tv_sec,time_goal.tv_nsec); 
    printf("Wake up Time s %i ns %i\n",time_relwakeup.tv_sec,time_relwakeup.tv_nsec); 
#endif   
    // Trigger Wait or fail
    if (wait==0) 
      {
	printf("Failed at %f2 by %d ns\n",model_t,timediff); 
      }
    else 
      {
        /* wait until next shot */
	if (clock_nanosleep(CLOCKID,0, &time_relwakeup, NULL) !=0)
	  {
	    printf("Sleep failed\n");
	  }
      }//wait if there was no realtime failure
     
     
    time_last = time_goal;



      if((FinalTime > 0.0 )&&(FinalTime <= (process_time_dbl - model_t0)))
	{
	  printf("Final Model Time reached!\n");
	  msr_thread_end = 1;
	}
      
      if(overruns >= max_overruns)
	{
	  printf("Max Model Overruns reached!\n");
	  msr_thread_end = 1;
	}


      if(msr_thread_end)
	{
	  break;
	}
  }
  pthread_exit(NULL);
}




int main(int argc, char *argv[])
{
  extern char *optarg;
  int c, donotrun = 0, priority = sched_get_priority_max(SCHED_RR);
  uid_t userid;

  //Status Feedback for Mutex Attributes
  //int status;

  stackinc = 0;

  //RT-Thread
  pthread_t rt_thread;
  pthread_attr_t thread_attr_rt;
  //MSR-SERVER
  int listen_fd;


  //Get UserId
  userid = geteuid(); //effective userid

  //Signal handling
  signal(SIGINT, endme);
  signal(SIGTERM, endme);
  signal(SIGTERM, endme);
  signal(SIGKILL, endme);
  signal(SIGPIPE,SIG_IGN); //Fix for Process kill by SIGPIPE
  
  //Preset Option values
  DoSoft = 0;
  CpuMap = -1; //Default, set RT-Process on all cpus
  networkport = 2345;
  msr_thread_end = 0;
  max_overruns = 10; //max allowed time overruns (default value)

  cycletime = (int)round(NAME(MODEL,_get_tsamp)()*1000000000.0); //Model specific timecycle in ns 
  //Options work
  do {
    c = getopt_long(argc, argv, "euvVsp:c:f:m:n:o:r:", options, NULL);
    switch (c) {
    case 'c':
      if ((CpuMap = atoi(optarg)) < 0) {
	fprintf(stderr, "-> Invalid CPU map.\n");
	donotrun = 1;
      } 
      break;
    case 'f':
      if (strstr(optarg, "inf")) {
	FinalTime = 0.0;
      } else if ((FinalTime = atof(optarg)) <= 0.0) {
	fprintf(stderr, "-> Invalid final time.\n");
	donotrun = 1;
      }
      break;
    case 'm':
      if ((stackinc = atoi(optarg)) < 0 ) {
	fprintf(stderr, "-> Invalid stack expansion.\n");
	donotrun = 1;
      }
      break;
    case 'n':
      if ((networkport = atoi(optarg)) < 1000 ) {
	fprintf(stderr, "-> Invalid Network Port.\n");
	donotrun = 1;
      }
      break;
    case 'o':
      if ((max_overruns = atoi(optarg)) < 0 ) {
	fprintf(stderr, "-> Invalid max overruns count.\n");
	donotrun = 1;
      }
      break;
    case 'p':
      if (((priority = atoi(optarg)) <= 0) && (priority > 50)) {
	fprintf(stderr, "-> Invalid priority value.\n");
	donotrun = 1;
      }
      break;
    case 'r':
      if (((cycletime = atoi(optarg)) < 1) || (cycletime >= 1000000)) 
	{
	  fprintf(stderr, "-> Invalid runcycle value.\n");
	  donotrun = 1;
	}
      else
	{
	  cycletime = cycletime*1000; //Calc from mues to ns
	}
      break;
    case 's':
      DoSoft = 1;
      printf("Do not use realtime priority \n");
      break;
    case 'u':
      print_usage();
      break;
    case 'V':
      fprintf(stderr, "EtherLAB-Scicos Version %s.\n", ETHERLAB_VERSION);
      fprintf(stderr, "Copyright Andreas Stewering-Bone, Igh Essen, Germany, 2009.\n");
      fprintf(stderr, "Contact: ab@igh-essen.com\n");
      return 0;
      break;
    case 'v':
      verbose = 1;
      break;
    default:
      if (c >= 0) {
	donotrun = 1;
      }
      break;
    }
  } while (c >= 0);

  if(userid != 0) //non root
    {
      DoSoft = 1;
      printf("Run model as normal user!\n");
    }

  //Test Timer Resolution
  if(clock_getres(CLOCKID, &timerres) == 0)
    {
#ifdef DEBUG
      printf("Timer resolution % i s, %i ns\n",timerres.tv_sec, timerres.tv_nsec);
#endif
      if(cycletime < 50*timerres.tv_nsec) // 2 percent tolerance for measurement
	{
	  printf("============================================================\n");
	  printf("Timerresolution is bad, be careful for measurement systems. \n");
	  printf("Timerresolution = %i ns \n",timerres.tv_nsec);
	  printf("============================================================\n");
	}
      
      if(cycletime < timerres.tv_nsec)
	{
	  cycletime = timerres.tv_nsec*2;
	  printf("Fix Cycletime to 2*Timerresolution = %i ns \n",cycletime);
	}
    }
  else
    {
      printf("Timer Source CLOCKID not available!\n Exit Model\n");
      donotrun = 1;
    }




  if (verbose) {
    printf("\nTarget settings\n");
    printf("===============\n");
    printf("  Cycletime         : %i in [ns]\n",cycletime);
    printf("  Realtime Priority : %s\n",DoSoft?"no":"yes");
    if (FinalTime > 0) {
      printf("  Finaltime         : %f [s]\n", FinalTime);
    } else {
      printf("  Finaltime         : RUN FOREVER\n");
    }
    printf("  maximal Overruns  : %i\n",max_overruns);
    if(!DoSoft)
      {
	printf("  Set userdefined CPU map : %s\n\n", (CpuMap >=0)?"Yes":"No");
      }
    sleep(5);

  }



  if (donotrun) {
    printf("ABORTED BECAUSE OF EXECUTION OPTIONS ERRORS.\n");
    return 1;
  }


  //Init MSR
  msr_rtlib_init(1,1.0e9/(double)cycletime,10,NULL);

  
  //INIT
  clock_gettime(CLOCKID,&acttime);
  process_time_dbl = sec_time(acttime);
  process_time = my_gettimeval(acttime); 
  exec_time = 0;
  overruns = 0;
  model_t0 = sec_time(acttime);
  model_t = 0.0;
  NAME(MODEL,_init)();
  




  //INIT Channels and Parameter
  msr_reg_kanal("/Time","s",&process_time_dbl,TDBL);
  msr_reg_kanal("/Time/sec","s",&process_time.tv_sec,TULINT);
  msr_reg_kanal("/Time/usec","us",&process_time.tv_usec,TULINT);
  msr_reg_kanal("/Linuxtime","s",&process_time_dbl,TDBL);
  //msr_reg_kanal("/Trigger","",&msr_trigger,TINT);*/ 
  
  /* dann die Taskinfo */
  msr_reg_kanal("/Taskinfo/Jiffies","us",&msr_jiffies,TLINT);
  msr_reg_kanal("/Taskinfo/Controller_Execution_Time","us",&exec_time,TINT);
  msr_reg_kanal("/Taskinfo/Controller_Call_Time","us",&call_time,TLINT); 
  msr_reg_kanal("/Taskinfo/Overruns","-",&overruns,TINT); 


  /* Lock memory */
  if(mlockall(MCL_CURRENT|MCL_FUTURE) == -1) 
    {
      perror("mlockall failed");
      exit(-2);
    }
  
  //Turn off malloc trimming
  mallopt(M_TRIM_THRESHOLD,-1);
  
  //Do not use, ethercat use mmap
  //Turn off mmap usage
  mallopt(M_MMAP_MAX,0);
 
  //Preget Memory
  preget_mem();


  //Init Mutex Attributes
  /* initialize default attributes for the mutex */
  //status = pthread_mutexattr_init(&mutex_attr);
  //if (status) {
  //  printf("Main: Initializing mutex attribute: 0x%x\n", status);
  //  return 1;
  //}

  //#ifdef HAVE_PI_MUTEX  
  //Set Priority Inherited Flags
  // To be fixed for old systems
  //#ifdef PTHREAD_PRIO_INHERIT
  //if (use_pi_mutex) {
    /* set priority inheritance attribute for mutex */
  //  status = pthread_mutexattr_setprotocol(&mutex_attr, 
  //					   PTHREAD_PRIO_INHERIT);
  //  if (status) {
  //    printf("Main: Setting mutex attribute policy: 0x%x\n", status);
  //    exit(-1);
  //  }
  //}
  //#endif
  //#ifdef DEBUG
  //	printf("Main: Priority Inheritance turned %s\n", use_pi_mutex ? "on" : "off");
  //#endif
  //#endif
  //Init Mutex
  //status = pthread_mutex_init(&mutex_state,&mutex_attr);
  //if (status) {
  //  printf("Main: Initializing mutex : 0x%x\n", status);
  //  exit(-1);
  //}

  //Setup CPUSet
  /* set up CPU affinity so we only use one processor */
  if (set_cpu_affinity(&cpu_mask,CpuMap))
    exit(-1);

  if(!DoSoft) 
    {
      thread_attr_rt = set_thread_rt_opt(priority, SCHED_RR,&cpu_mask);
    }

  //Wait for MSR Thread creation
  sleep(2);
  //Create RT Thread
  if(!DoSoft)
    {
      if(pthread_create(&rt_thread, &thread_attr_rt, model_exec_thread,NULL)!=0)
	{
	  printf("Can not create RT-Thread. Exiting!\n");
	  exit(0);
	}
    }
  else
    {
      if(pthread_create(&rt_thread, NULL, model_exec_thread,NULL)!=0)
	{
	  printf("Can not create RT-Thread. Exiting!\n");
	  exit(0);
	}
    }


  //Init MSR-Server
  listen_fd  = msr_server_init(networkport);
  //endless loop
  msr_server_loop(listen_fd,DoSoft);

  //END
  printf("Wait for Thread end.\n");
  sleep(2);
  //Destroy Mutex
  //pthread_mutex_destroy(&mutex_state);
  msr_cleanup();
  NAME(MODEL,_end)();
  printf("Model end reached\n");
  return 0;
}

