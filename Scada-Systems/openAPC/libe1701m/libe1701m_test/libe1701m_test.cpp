#include <stdio.h>
#include <stdlib.h>

#include "../libe1701m.h"
#include "liboapc.h"

static int cardNum=-1;

#define USE_AXIS_FLAG E1701M_AXIS_3
#define USE_AXIS_NUM  3

static int moveToPosition(int pos,char releaseTrigger)
{
   int    ret;
   long   currPos;
   double spd;

   ret=E1701M_move_abs(cardNum,USE_AXIS_FLAG,pos);
   if (ret!=E1701M_OK) return ret;

   if (releaseTrigger)
   {
      oapc_thread_sleep(1500);
      E1701M_release_trigger_point(cardNum,USE_AXIS_FLAG); // release the trigger point by software to start movement
   }

   // wait until movement has started
   for (;;)
   {
      if ((E1701M_get_axis_state(cardNum,USE_AXIS_NUM) & E1701M_AXIS_STATE_MOVING)==E1701M_AXIS_STATE_MOVING) break;
      oapc_thread_sleep(10);
   }
   // wait until movement has finished
   for (;;)
   {
      if ((E1701M_get_axis_state(cardNum,USE_AXIS_NUM) & E1701M_AXIS_STATE_MOVING)==0) break;
      if (E1701M_get_axis_pos2(cardNum,USE_AXIS_NUM,&currPos)==E1701M_OK)
      {
         spd=E1701M_get_axis_speed(cardNum,USE_AXIS_NUM);
         printf("\t%d \t%.2f          \r",currPos,spd);
      }
      oapc_thread_sleep(10);
   }    
   return E1701M_OK;
}


static int doReferencing()
{
   int ret;

   ret=E1701M_reference(cardNum,USE_AXIS_FLAG,E1701M_REFSTEP_1_ENTER_N|E1701M_REFSTEP_2_LEAVE_P,0x01,300.0,150.0,0,0);
   if (ret!=E1701M_OK) return ret;

   // wait until referencing has started
   for (;;)
   {
      if ((E1701M_get_axis_state(cardNum,USE_AXIS_NUM) & E1701M_AXIS_STATE_REFERENCING)==E1701M_AXIS_STATE_REFERENCING) break;
      oapc_thread_sleep(10);
   }
   // wait until movement and referencing has finished
   for (;;)
   {
      if ((E1701M_get_axis_state(cardNum,USE_AXIS_NUM) & (E1701M_AXIS_STATE_MOVING|E1701M_AXIS_STATE_REFERENCING))==0) break;
      oapc_thread_sleep(10);
   }

   // specify value of referenced position
   ret=E1701M_set_pos(cardNum,USE_AXIS_FLAG,0);
   if (ret!=E1701M_OK) return ret;

   // wait until value was written before continuing with some movements
   for (;;)
   {
       if ((E1701M_get_axis_state(cardNum,USE_AXIS_NUM) & E1701M_AXIS_STATE_SETPOS)==E1701M_AXIS_STATE_SETPOS) break;
       oapc_thread_sleep(10);
   }

   return E1701M_OK;
}


static int moveSequence()
{
   int    ret;
   long   currPos;
   double spd;

   ret=E1701M_move_abs(cardNum,USE_AXIS_FLAG,3000);
   if (ret!=E1701M_OK) return ret;
   ret=E1701M_delay(cardNum,USE_AXIS_FLAG,1.0);
   if (ret!=E1701M_OK) return ret;
   ret=E1701M_move_abs(cardNum,USE_AXIS_FLAG,0);
   if (ret!=E1701M_OK) return ret;

   // wait until movement sequence has started
   for (;;)
   {
      if ((E1701M_get_axis_state(cardNum,USE_AXIS_NUM) & E1701M_AXIS_STATE_MOVING)==E1701M_AXIS_STATE_MOVING) break;
      oapc_thread_sleep(10);
   }
   // wait until movement sequence has finished
   for (;;)
   {
      if ((E1701M_get_axis_state(cardNum,USE_AXIS_NUM) & E1701M_AXIS_STATE_MOVING)==0) break;
      if (E1701M_get_axis_pos2(cardNum,USE_AXIS_NUM,&currPos)==E1701M_OK)
      {
         spd=E1701M_get_axis_speed(cardNum,USE_AXIS_NUM);      
         printf("\t%d \t%.2f          \r",currPos,spd);
      }
      oapc_thread_sleep(10);
   }
   return E1701M_OK;
}


int main(int argc, char *argv[])
{
   int       ret,mode=0;
#ifdef ENV_WINDOWS
   WSADATA   wsaData;

   WSAStartup(MAKEWORD(1,1),&wsaData);
#endif

   if (argc==2) mode=atoi(argv[1]);
   if ((mode<1) || (mode>5))
   {
      printf("Usage: libe1701m_test [mode]\r\n");
      printf("       mode=1 - simple motion\r\n");
      printf("       mode=2 - motion sequence with delay\r\n");
      printf("       mode=3 - motion after external trigger\r\n");
      printf("       mode=4 - referencing\r\n");
      printf("       mode=5 - endless motion loop\r\n");
      return -1;
   }
   cardNum=E1701M_set_connection("192.168.2.254");
   if (cardNum>0)
   {
//      E1701M_set_password(cardNum,"123456"); // used only in case of Ethernet connection; requires the password "123456" to be set in card configuration file
      ret=E1701M_open_connection(cardNum);
      if (ret==E1701M_OK)
      {
         unsigned int cnt=0;

         ret=E1701M_set_accels(cardNum,USE_AXIS_FLAG,10.0,7.5,250.0);
         ret=E1701M_set_limits(cardNum,USE_AXIS_FLAG,-10000,20000,1250);
         ret=E1701M_set_speed(cardNum,USE_AXIS_FLAG,400.0);
         ret=E1701M_set_pos(cardNum,USE_AXIS_FLAG,0); // set current position to 0

         if (mode==1)
         {
            moveToPosition(1000,0);
            moveToPosition(-1000,0);
         }
         else if (mode==2) moveSequence();
         else if (mode==3) 
         {
            E1701M_set_trigger_point(cardNum,USE_AXIS_FLAG,0x01); // set external trigger point
            moveToPosition(3000,1);
         }
         else if (mode==4) doReferencing();
         else while (mode==5)
         {
            cnt++;
            printf("\r\n%d:\r\n",cnt);
            ret=E1701M_set_speed(cardNum,USE_AXIS_FLAG,400.0);
            moveToPosition(1000,0);
            ret=E1701M_set_speed(cardNum,USE_AXIS_FLAG,600.0);
            moveToPosition(-1000,0);
            moveSequence();
         }
         E1701M_close(cardNum);
      }
      else printf("ERROR: opening connection failed with error %d\r\n",ret);
   }
   else printf("ERROR: Could not initialise!\r\n");
   return 0;
}

