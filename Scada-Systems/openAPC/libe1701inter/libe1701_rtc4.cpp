/******************************************************************************

This file is part of E1701inter shared library

E1701inter is free software: you can redistribute it and/or modify it under the
terms of the GNU General Public License as published by the Free Software
Foundation, either version 3 of the License, or (at your option) any later
version.

E1701inter is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with
E1701inter. If not, see <http://www.gnu.org/licenses/>.

E1701inter linking exception: Linking this library dynamically with other
modules is making a combined work based on this library. Thus, the terms and
conditions of the GNU General Public License cover the whole combination. As a
special exception, the copyright holders of this library give you permission
to link this library with independent modules to produce an executable,
regardless of the license terms of these independent modules, and to copy and
distribute the resulting executable under terms of your choice, provided that
you also meet, for each linked independent module, the terms and conditions of
the license of that module. An independent module is a module which is not
derived from or based on this library. If you modify this library, you may
extend this exception to your version of the library, but you are not
obliged to do so. If you do not wish to do so, delete this exception statement
from your version.

*******************************************************************************/

#include <string.h>
#include <assert.h>
#include <stdlib.h>

#include "libe1701.h"
#include "libe1701inter.h"
#include "liboapc.h"

//#include <queue>

static unsigned char  m_currListNo[E1701_MAX_HEAD_NUM+1]={0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1};
static int            m_lastX[E1701_MAX_HEAD_NUM+1]=     {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
static int            m_lastY[E1701_MAX_HEAD_NUM+1]=     {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
static int            m_lastZ[E1701_MAX_HEAD_NUM+1]=     {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
static unsigned short m_lastA2[E1701_MAX_HEAD_NUM+1]=     {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
static unsigned short m_lastA3[E1701_MAX_HEAD_NUM+1]=     {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
static unsigned short m_lastA4[E1701_MAX_HEAD_NUM+1]=     {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};


E1701_API unsigned char E1701_set_connection(const char *address)
{
   return E1701_set_connection_(address);
}


E1701_API short n_load_program_file(unsigned short n, const char* name)
{
   if (n>=E1701_MAX_HEAD_NUM) return E1701_ERROR_INVALID_CARD;
   return E1701_load_correction((unsigned char)n,NULL,0);
};

E1701_API short n_load_correction_file(unsigned short n, const char* filename, short cortable, double kx, double ky, double phi, double xoffset, double yoffset)
{
   int ret;

   if (n>=E1701_MAX_HEAD_NUM) return E1701_ERROR_INVALID_CARD;
   ret=E1701_load_correction((unsigned char)n,filename,0);
   if (ret==E1701_OK) ret=E1701_set_xy_correction((unsigned char)n,kx,ky,phi,(int)OAPC_ROUND(xoffset*1024,0),(int)OAPC_ROUND(yoffset*1024,0));
   return ret;
};

E1701_API void n_set_jump_speed(unsigned short n, double speed)
{
   if (n>=E1701_MAX_HEAD_NUM) return;
   E1701_set_speeds((unsigned char)n,speed,-1.0);
};

E1701_API void n_set_mark_speed(unsigned short n, double speed)
{
   if (n>=E1701_MAX_HEAD_NUM) return;
   E1701_set_speeds((unsigned char)n,-1.0,speed);
};

E1701_API void n_set_laser_delays(unsigned short n, short ondelay, short offdelay)
{
   E1701_set_laser_delays((unsigned char)n,ondelay,offdelay);
};


E1701_API void n_set_scanner_delays(unsigned short n, unsigned short jumpdelay, unsigned short markdelay, unsigned short polydelay)
{
   E1701_set_scanner_delays((unsigned char)n,jumpdelay*10.0,markdelay*10.0,polydelay*10.0);
};


E1701_API void n_set_laser_mode(unsigned short n, unsigned short mode)
{
   if (n>=E1701_MAX_HEAD_NUM) return;
   switch (mode)
   {
      case 0:
         E1701_set_laser_mode((unsigned char)n,E1701_LASERMODE_CO2);
         break;
      case 1:
         E1701_set_laser_mode((unsigned char)n,E1701_LASERMODE_YAG1);
         break;
      case 2:
         E1701_set_laser_mode((unsigned char)n,E1701_LASERMODE_YAG2);
         break;
      case 3:
         E1701_set_laser_mode((unsigned char)n,E1701_LASERMODE_YAG3);
         break;
      case 4:
         E1701_set_laser_mode((unsigned char)n,E1701_LASERMODE_CRF);
         break;
      default:
         assert(0);
         break;
   }
};

E1701_API void n_dsp_start(unsigned short n)
{
   if (n>=E1701_MAX_HEAD_NUM) return;
   E1701_close((unsigned char)n);
   m_lastX[n]=0; m_lastY[n]=0; m_lastZ[n]=0;
   m_lastA2[n]=0; m_lastA3[n]=0; m_lastA4[n]=0;
};


E1701_API void n_jump_abs_3d(unsigned short n, short x, short y, short z)
{
   E1701_jump_abs((unsigned char)n,x<<10,y<<10,z<<10);
   m_lastX[n]=x;
   m_lastY[n]=y;
   m_lastZ[n]=z;
};


E1701_API void n_jump_abs(unsigned short n, short x, short y)
{
   E1701_jump_abs((unsigned char)n,x<<10,y<<10,0);
   m_lastX[n]=x;
   m_lastY[n]=y;
};


E1701_API void n_mark_abs_3d(unsigned short n, short x, short y, short z)
{
   E1701_mark_abs((unsigned char)n,x<<10,y<<10,z<<10);
   m_lastX[n]=x;
   m_lastY[n]=y;
   m_lastZ[n]=z;
};


E1701_API void n_mark_abs(unsigned short n, short x, short y)
{
   E1701_mark_abs((unsigned char)n,x<<10,y<<10,0);
   m_lastX[n]=x;
   m_lastY[n]=y;
};


E1701_API void n_set_matrix(unsigned short n, double m11, double m12, double m21, double m22)
{
   E1701_set_matrix((unsigned char)n,m11,m12,m21,m22);
};

E1701_API void n_set_matrix_list(unsigned short n, unsigned short i, unsigned short j, double mij)
{
   if (n>=E1701_MAX_HEAD_NUM) return;
   if ((i==1) && (j==1)) E1701_set_matrix((unsigned char)n,mij,-10000000,-10000000,-10000000);
   else if ((i==1) && (j==2)) E1701_set_matrix((unsigned char)n,-10000000,mij,-10000000,-10000000);
   else if ((i==2) && (j==1)) E1701_set_matrix((unsigned char)n,-10000000,-10000000,mij,-10000000);
   else if ((i==2) && (j==2)) E1701_set_matrix((unsigned char)n,-10000000,-10000000,-10000000,mij);
   else return;
};

E1701_API void n_set_start_list(unsigned short n, unsigned short listno)
{
   if (n>=E1701_MAX_HEAD_NUM) return;
   m_currListNo[n]=(unsigned char)listno;
};

E1701_API void n_execute_list(unsigned short n, unsigned short listno)
{
   E1701_execute((unsigned char)n);
};

E1701_API void n_set_end_of_list(unsigned short n)
{
   E1701_execute((unsigned char)n);
};

E1701_API void n_auto_change(unsigned short n)
{
   if (n>=E1701_MAX_HEAD_NUM) return;
   m_currListNo[n]=3-m_currListNo[n];
};


E1701_API unsigned short n_read_status(unsigned short n)
{
   unsigned short status=0;
   unsigned int   card_state=0;

   if (n>=E1701_MAX_HEAD_NUM) return 0xFFFF;

   card_state=E1701_get_card_state((unsigned char)n);

   if (m_currListNo[n]==1) status=0x0001; // list 1 is to be filled
   else status=0x0002; // list 2 is to be filled

   if (m_currListNo[n]==1) status=0x0080; // list 1 is to be filled so list 2 is closed
   else status=0x0040;  // list 2 is to be filled so list 1 is closed

   if ((card_state & (E1701_CSTATE_MARKING|E1701_CSTATE_PROCESSING))!=0)
   {
      if (m_currListNo[n]==1) status|=0x0020; // list 1 is to be filled so mark list 2 as busy
      else status|=0x0010; // list 2 is to be filled so mark list 1 as busy
   }

   status|=0xFF00;

   return status;
};


E1701_API void n_get_status(unsigned short n, unsigned short *busy, unsigned short *position)
{
   if (m_currListNo[n]==1) *position=7999;
   else *position=3999;
   *busy=((E1701_get_card_state((unsigned char)n) & (E1701_CSTATE_MARKING|E1701_CSTATE_PROCESSING))!=0);
};


E1701_API void n_stop_execution(unsigned short n)
{
   E1701_stop_execution((unsigned char)n);
};


E1701_API void n_jump_rel_3d(unsigned short n,short dx,short dy,short dz)
{
    if (n>=E1701_MAX_HEAD_NUM) return;
    E1701_jump_abs((unsigned char)n,m_lastX[n]+(dx<<10),m_lastY[n]+(dy<<10),m_lastZ[n]+(dz<<10));
    m_lastX[n]=dx<<10;
    m_lastY[n]=dy<<10;
    m_lastZ[n]=dz<<10;
};


E1701_API void n_jump_rel(unsigned short n, short dx, short dy)
{
    if (n>=E1701_MAX_HEAD_NUM) return;
    E1701_jump_abs((unsigned char)n,m_lastX[n]+(dx<<10),m_lastY[n]+(dy<<10),m_lastZ[n]);
    m_lastX[n]=dx<<10;
    m_lastY[n]=dy<<10;
};


E1701_API void n_mark_rel_3d(unsigned short n, short dx, short dy, short dz)
{
    if (n>=E1701_MAX_HEAD_NUM) return;
    E1701_mark_abs((unsigned char)n,m_lastX[n]+(dx<<10),m_lastY[n]+(dy<<10),m_lastZ[n]+(dz<<10));
    m_lastX[n]=dx<<10;
    m_lastY[n]=dy<<10;
    m_lastZ[n]=dz<<10;
};


E1701_API void n_mark_rel(unsigned short n, short dx, short dy)
{
   if (n>=E1701_MAX_HEAD_NUM) return;
   E1701_mark_abs((unsigned char)n,m_lastX[n]+(dx<<10),m_lastY[n]+(dy<<10),m_lastZ[n]);
   m_lastX[n]=dx<<10;
   m_lastY[n]=dy<<10;
};


E1701_API void n_long_delay(unsigned short n, unsigned short value)
{
   if (n>=E1701_MAX_HEAD_NUM) return;
   E1701_delay((unsigned char)n,value*10);
};


E1701_API void n_list_nop(unsigned short n)
{
   if (n>=E1701_MAX_HEAD_NUM) return;
   E1701_delay((unsigned char)n,1);
};

E1701_API void n_set_laser_timing(unsigned short n, unsigned short halfperiod, unsigned short pulse1, unsigned short pulse2, unsigned short timebase)
{
   double frequency,pulse;

   if (n>=E1701_MAX_HEAD_NUM) return;
   if (timebase==0)
   {
       frequency=(1.0/(halfperiod/1000000.0))/2.0; // divide by two because incoming value is halfperiod, not period
       pulse=pulse1;
   }
   else
   {
       frequency=(1.0/((halfperiod/8.0)/1000000.0))/2.0; // divide by two because incoming value is halfperiod, not period
       pulse=pulse1/8.0;
   }
   E1701_set_laser_timing((unsigned char)n,frequency,pulse);
};


E1701_API void n_set_standby_list(unsigned short n, unsigned short halfperiod, unsigned short in_pulse)
{
   double frequency,pulse;

   if (n>=E1701_MAX_HEAD_NUM) return;
   frequency=(1.0/((halfperiod/8.0)/1000000.0))/2.0; // divide by two because incoming value is halfperiod, not period
   pulse=in_pulse/8.0;
   E1701_set_standby((unsigned char)n,frequency,pulse);
};

E1701_API void n_write_8bit_port_list(unsigned short n, unsigned short value)
{
   if (n>=E1701_MAX_HEAD_NUM) return;
   E1701_lp8_write((unsigned char)n,(unsigned char)value);
};

E1701_API void n_write_da_1_list(unsigned short n, unsigned short value)
{
   if (n>=E1701_MAX_HEAD_NUM) return;
   E1701_lp8_a0((unsigned char)n,(unsigned char)value);
};

E1701_API void n_write_da_x_list(unsigned short n, unsigned short x, unsigned short value)
{
    switch (x)
    {
    case 1:
       n_write_da_1_list(n,value);
       return;
    case 2:
       m_lastA2[n]=value;
       break;
    case 3:
       m_lastA3[n]=value;
       break;
    case 4:
       m_lastA4[n]=value;
       break;
    default:
       assert(0);
       return;
    }
    E1701_ana_a123((unsigned char)n,m_lastA2[n],m_lastA3[n],m_lastA4[n]);
};

// fpk: 1/8 usec
E1701_API void n_set_firstpulse_killer_list(unsigned short n, unsigned short fpk)
{
   if (n>=E1701_MAX_HEAD_NUM) return;
   E1701_set_fpk((unsigned char)n,fpk/8.0,10.0); // setting 10 usec yag3QTime, is used in YAG3 mode only
};

E1701_API void n_set_list_mode(unsigned short /*n*/, unsigned short /*mode*/)
{
    // no functionality needed for e1701
};

E1701_API void set_list_mode(unsigned short /*mode*/)
{
    // no functionality needed for e1701
};


E1701_API unsigned short n_read_io_port(unsigned short n)
{
   return E1701_digi_read((unsigned char)n);
};

