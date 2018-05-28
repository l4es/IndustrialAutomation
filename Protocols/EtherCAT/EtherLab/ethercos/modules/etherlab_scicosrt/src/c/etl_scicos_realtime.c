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


#include <stdio.h>
#include <machine.h>
#include <math.h>
#include <scicos_block4.h>


#include <stdlib.h>
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
#include <sys/ioctl.h>
#include <sys/types.h>
#include <fcntl.h>
#include <errno.h>
#include <ctype.h>


//Define Clock Source
#define CLOCKID CLOCK_REALTIME

struct RTSYNC{
  int time_old, time_new,steps;
  int orig_scheduling_policy;
  struct sched_param orig_scheduler;
  //Timing values
  struct timespec time_last, time_s;
  struct timespec time_goal;
  double SpareTime;  
  struct timespec acttime,acttime_last;
  struct timespec timerres;
  int cycletime;
  struct timespec cycletimespec;
  long int timediff;
  double process_time_dbl;
  struct timeval process_time;
  long int msr_jiffies;
  long int call_time;
  int overruns;
  uid_t userid;
};

/*Functions for time handling */

double sec_time()
{
  struct timespec mytime;

  clock_gettime(CLOCKID, &mytime);
  return ((double)mytime.tv_sec+((double)mytime.tv_nsec/1000000000.0));
}


long int n_time()
{
  struct timespec mytime;

  clock_gettime(CLOCKID, &mytime);
  return (1000000000*mytime.tv_sec+mytime.tv_nsec);
}

struct timeval my_gettimeval(void)
{
  struct timespec mytime;
  struct timeval result;
  clock_gettime(CLOCKID, &mytime);
  result.tv_sec = mytime.tv_sec;
  result.tv_usec = mytime.tv_nsec / 1000;
  return result;
}


long int mu_only_time()
{
  struct timespec mytime;

  clock_gettime(CLOCKID, &mytime);
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


static int init(scicos_block *block)
{
  struct RTSYNC * comdev = (struct RTSYNC *) malloc(sizeof(struct RTSYNC));
  int retval, ret;
  struct sched_param param,mysched;  
  struct timespec timerres;
  //Get UserId
  comdev->userid = geteuid(); //effective userid
  
  
   if(comdev->userid != 0) //non root
     {
       printf("Run model as normal user!\n");
     }
   
   //Test Timer Resolution
   if(clock_getres(CLOCKID, &timerres) == 0)
     {
       printf("Timer resolution % i s, %i ns\n",timerres.tv_sec, timerres.tv_nsec);
     }
   else
     {
       printf("Timer Source CLOCKID not available!\n Exit Model\n");
    }

   if ((block->ipar[0]==1)&&(comdev->userid == 0)) {
     comdev->orig_scheduling_policy = sched_getscheduler(0);
     sched_getparam(0, &comdev->orig_scheduler);
     param.sched_priority = sched_get_priority_max(SCHED_RR);
     retval=sched_setscheduler(0,SCHED_RR,&param);
     
     
     if (retval==0) {
       printf("Got Real-Time Priority\n");
     }
     else
       printf("Could no set sched priority to %d\n Please start scilab with root rights!\n",param.sched_priority);
   }
   
   clock_gettime(CLOCKID,&comdev->time_last);
   comdev->time_goal = comdev->time_last;
   comdev->time_s=comdev->time_last;
   comdev->cycletime = block->ipar[1]*1000000; //Model specific timecycle in ns 
   comdev->call_time =  comdev->cycletime / 1000;
   comdev->cycletimespec = li2timespec(comdev->cycletime);
   comdev->acttime_last = comdev->time_last;
   
   
   *block->work=(void *)comdev;
   
   return 0;
}


static int inout(scicos_block *block)
{
  struct RTSYNC * comdev = (struct RTSYNC *) (*block->work);
  int wait;
  //outputs
  //time since start
  double *y1 = GetRealOutPortPtrs(block,1);
  //time diff
  double *y2 = GetRealOutPortPtrs(block,2);
  //spared time
  double *y3 = GetRealOutPortPtrs(block,3);


  comdev->process_time_dbl = sec_time();
  comdev->process_time = my_gettimeval(); 
  clock_gettime(CLOCKID,&comdev->acttime);
  comdev->timediff = timespec2li(difftimespec(comdev->time_goal,comdev->acttime));
  comdev->call_time = timespec2li(difftimespec(comdev->acttime_last,comdev->acttime))/1000;
  comdev->acttime_last = comdev->acttime;
  
  if (abs(comdev->timediff)>pow(2,30)) 
    {
      if (sign(timespec2li(comdev->acttime))==-1)
	{
	  wait = 0;//Realtime failed
	  comdev->overruns++;
	}
      else
	{
	  wait=1;
	  comdev->SpareTime=0;//???
	}
    }
  else 
    {
      if (comdev->timediff > comdev->cycletime)
	{
	  wait=0;
	  comdev->overruns++;
	  comdev->SpareTime=0;
	}
      else
	{
	  wait=1; 
	  comdev->SpareTime = (double) 0.000001 * ((double) comdev->timediff);
	}
    }
  //Set new wake up time
  comdev->time_goal=addtime(comdev->time_last,comdev->cycletimespec);
  
  // Trigger Wait or fail
  if (wait==0) 
    {
      printf("Failed at %f2 by %d ns\n",comdev->process_time_dbl,comdev->timediff); 
    }
  else 
    {
      /* wait until next shot */
      if (clock_nanosleep(CLOCKID, TIMER_ABSTIME, &comdev->time_goal, NULL) !=0)
	{
	  printf("Sleep failed\n");
	}
    }//wait if there was no realtime failure
  
  comdev->time_last = comdev->time_goal;
     

  y1[0] = comdev->process_time_dbl;
  //Diff Call Time
  y2[0] = comdev->timediff;
  y3[0] = (double) comdev->SpareTime;      

  return 0;
}



static int end(scicos_block *block)
{
  struct RTSYNC * comdev = (struct RTSYNC *) (*block->work);
  struct sched_param mysched;


 if ((block->ipar[0]==1)&&(comdev->userid==0)) 
	{  
	  sched_setscheduler( 0, comdev->orig_scheduling_policy, &comdev->orig_scheduler );
	  sched_getparam(0,&mysched);
	  printf("priority reset to %i\n",mysched.sched_priority);
	}
  free(comdev);
  return 0;
}



//Parameter 1: 1 -> use RT scheduler and set to high priority (if root)
//             0 -> no RT scheduler
//Parameter 2: Abtastzeit in ms
void etl_scicos_realtime(scicos_block *block,int flag)
{
  if (flag==1){          /* set output */
    inout(block);
  }
  else if (flag==5){     /* termination */ 
    end(block);
  }
  else if (flag ==4){    /* initialisation */
    init(block);
  }
}
