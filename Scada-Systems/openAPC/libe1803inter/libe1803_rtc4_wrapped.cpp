/******************************************************************************

This file is part of E1803inter shared library

E1803inter is free software: you can redistribute it and/or modify it under the
terms of the GNU General Public License as published by the Free Software
Foundation, either version 3 of the License, or (at your option) any later
version.

E1803inter is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with
E1803inter. If not, see <http://www.gnu.org/licenses/>.

E1803inter linking exception: Linking this library dynamically with other
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

#include "libe1803.h"
#include "libe1803inter.h"


static unsigned short currHead=1;
static unsigned short lastDigiOut[E1803_MAX_HEAD_NUM+1]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
static double         flyX[E1803_MAX_HEAD_NUM+1]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
static double         flyY[E1803_MAX_HEAD_NUM+1]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

E1803_API void select_rtc(unsigned short cardno)
{
   if (cardno>=E1803_MAX_HEAD_NUM) return;
   currHead=cardno;
};

E1803_API unsigned short rtc4_count_cards(void)
{
   return E1803_MAX_HEAD_NUM;
};

E1803_API short load_program_file(const char* name)
{
    return n_load_program_file(currHead,name);
};

E1803_API short load_correction_file(const char* filename, short cortable, double kx, double ky, double phi, double xoffset, double yoffset)
{
    return n_load_correction_file(currHead,filename,cortable,kx,ky,phi,xoffset,yoffset);
};

E1803_API void dsp_start(void)
{
    n_dsp_start(currHead);
};

E1803_API void set_laser_mode(unsigned short mode)
{
    n_set_laser_mode(currHead,mode);
};

E1803_API void set_laser_delays(short ondelay, short offdelay)
{
    n_set_laser_delays(currHead,ondelay,offdelay);
};

E1803_API void set_scanner_delays(unsigned short jumpdelay, unsigned short markdelay, unsigned short polydelay)
{
    n_set_scanner_delays(currHead,jumpdelay,markdelay,polydelay);
};

E1803_API void set_jump_speed(double speed)
{
    n_set_jump_speed(currHead,speed);
};

E1803_API void set_mark_speed(double speed)
{
    n_set_mark_speed(currHead,speed);
};

E1803_API void jump_abs_3d(short x, short y, short z)
{
   n_jump_abs_3d(currHead,x,y,z);
};

E1803_API void n_goto_xyz(unsigned short n, short x, short y, short z)
{
   n_jump_abs_3d(n,x,y,z);
};

E1803_API void goto_xyz(short x, short y, short z)
{
   n_jump_abs_3d(currHead,x,y,z);
};

E1803_API void jump_abs(short x, short y)
{
   n_jump_abs(currHead,x,y);
};

E1803_API void n_goto_xy(unsigned short n, short x, short y)
{
   n_jump_abs(n,x,y);
};

E1803_API void goto_xy(short x, short y)
{
   n_jump_abs(currHead,x,y);
};

E1803_API void mark_abs_3d(short x, short y, short z)
{
   n_mark_abs_3d(currHead,x,y,z);
};

E1803_API void mark_abs(short x, short y)
{
   n_mark_abs(currHead,x,y);
};

E1803_API void set_matrix(double m11, double m12, double m21, double m22)
{
   n_set_matrix(currHead,m11,m12,m21,m22);
};

E1803_API void set_matrix_list(unsigned short i, unsigned short j, double mij)
{
   n_set_matrix_list(currHead,i,j,mij);
};

E1803_API void set_start_list_1(void)
{
    n_set_start_list(currHead,1);
};

E1803_API void set_start_list_2(void)
{
    n_set_start_list(currHead,2);
};

E1803_API void set_start_list(unsigned short listno)
{
    n_set_start_list(currHead,listno);
};

E1803_API void n_set_start_list_1(unsigned short n)
{
    n_set_start_list(n,1);
};

E1803_API void n_set_start_list_2(unsigned short n)
{
    n_set_start_list(n,2);
};

E1803_API void n_execute_list_1(unsigned short n)
{
    n_execute_list(n,1);
};

E1803_API void execute_list_1(void)
{
    n_execute_list(currHead,1);
};

E1803_API void n_execute_list_2(unsigned short n)
{
    n_execute_list(n,2);
};

E1803_API void execute_list_2(void)
{
    n_execute_list(currHead,2);
};

E1803_API void execute_list(unsigned short listno)
{
    n_execute_list(currHead,listno);
};

E1803_API void auto_change(void)
{
    n_auto_change(currHead);
};

E1803_API unsigned short read_status(void)
{
   return n_read_status(currHead);
};

E1803_API void stop_execution(void)
{
    n_stop_execution(currHead);
};

E1803_API void jump_rel_3d(short dx, short dy, short dz)
{
    n_jump_rel_3d(currHead,dx,dy,dz);
};

E1803_API void jump_rel(short dx, short dy)
{
    n_jump_rel(currHead,dx,dy);
};

E1803_API void mark_rel_3d(short dx, short dy, short dz)
{
    n_mark_rel_3d(currHead,dx,dy,dz);
};

E1803_API void mark_rel(short dx, short dy)
{
    n_mark_rel(currHead,dx,dy);
};

E1803_API void long_delay(unsigned short value)
{
    n_long_delay(currHead,value);
};

E1803_API void set_laser_timing(unsigned short halfperiod, unsigned short pulse1, unsigned short pulse2, unsigned short timebase)
{
   n_set_laser_timing(currHead,halfperiod,pulse1,pulse2,timebase);
};

E1803_API void set_standby_list(unsigned short half_period, unsigned short pulse)
{
    n_set_standby_list(currHead,half_period,pulse);
};

E1803_API void n_set_standby(unsigned short n, unsigned short half_period, unsigned short pulse)
{
    n_set_standby_list(n,half_period,pulse);
};

E1803_API void set_standby(unsigned short half_period, unsigned short pulse)
{
   n_set_standby_list(currHead,half_period,pulse);
};

E1803_API void list_nop(void)
{
   n_list_nop(currHead);
};

E1803_API void write_8bit_port_list(unsigned short value)
{
   n_write_8bit_port_list(currHead,value);
};

E1803_API void n_write_8bit_port(unsigned short n, unsigned short value)
{
   n_write_8bit_port_list(n,value);
};

E1803_API void write_8bit_port(unsigned short value)
{
   n_write_8bit_port_list(currHead,value);
};

E1803_API void write_da_x_list(unsigned short x, unsigned short value)
{
   n_write_da_x_list(currHead,x,value);
};

E1803_API void write_da_1_list(unsigned short value)
{
   n_write_da_1_list(currHead,value);
};

E1803_API void n_write_da_x(unsigned short n, unsigned short x, unsigned short value)
{
   if (x==1) n_write_da_1_list(n,value);
};

E1803_API void write_da_x(unsigned short x, unsigned short value)
{
   if (x==1) n_write_da_1_list(currHead,value);
};

E1803_API void n_write_da_1(unsigned short n, unsigned short value)
{
   n_write_da_1_list(n,value);
};

E1803_API void write_da_1(unsigned short value)
{
   n_write_da_1_list(currHead,value);
};

E1803_API void set_firstpulse_killer_list(unsigned short fpk)
{
    n_set_firstpulse_killer_list(currHead,fpk);
};

E1803_API void n_set_firstpulse_killer(unsigned short n, unsigned short fpk)
{
    n_set_firstpulse_killer_list(n,fpk);
};

E1803_API void set_firstpulse_killer(unsigned short fpk)
{
    n_set_firstpulse_killer_list(currHead,fpk);
};

E1803_API void set_end_of_list(void)
{
	n_set_end_of_list(currHead);
};

E1803_API void n_set_wobbel_xy(unsigned short n, unsigned short long_wob, unsigned short trans_wob, double frequency)
{
   E1803_set_wobble((unsigned char)n,long_wob,trans_wob,frequency);
};

E1803_API void set_wobbel_xy(unsigned short long_wob, unsigned short trans_wob, double frequency)
{
   n_set_wobbel_xy(currHead,long_wob,trans_wob,frequency);
};

E1803_API void n_set_wobbel(unsigned short n, unsigned short amplitude, double frequency)
{
   n_set_wobbel_xy(n,amplitude,amplitude,frequency);
};

E1803_API void set_wobbel(unsigned short amplitude, double frequency)
{
   n_set_wobbel(currHead,amplitude,frequency);
};

E1803_API void n_set_control_mode_list(unsigned short n, unsigned short mode)
{
   if (mode==0x01) E1803_set_trigger_point((unsigned char)n);
};

E1803_API void set_control_mode_list(unsigned short mode)
{
   n_set_control_mode_list(currHead,mode);
};

E1803_API void n_set_control_mode(unsigned short n, unsigned short mode)
{
   n_set_control_mode_list(n,mode);
};

E1803_API void set_control_mode(unsigned short mode)
{
   set_control_mode_list(mode);
};

E1803_API unsigned short get_dll_version(void)
{
    return E1803_DLL_VERSION;
};

E1803_API unsigned short n_get_rtc_version(unsigned short n)
{
    unsigned short hwVersion,fwVersion;

    E1803_get_version((unsigned char)n,&hwVersion,&fwVersion);

    return fwVersion;
};

E1803_API unsigned short get_rtc_version(void)
{
    return n_get_rtc_version(currHead);
};

E1803_API unsigned short n_get_hex_version(unsigned short n)
{
    return n_get_rtc_version(n);
};

E1803_API unsigned short get_hex_version(void)
{
    return n_get_rtc_version(currHead);
};

E1803_API unsigned short n_get_startstop_info(unsigned short n)
{
   return (unsigned short)(E1803_get_startstop_state((unsigned char)n) & 0x0000FFFF);
};

E1803_API unsigned short get_startstop_info(void)
{
   return n_get_startstop_info(currHead);
};

E1803_API void n_write_io_port_list(unsigned short n, unsigned short value)
{
   lastDigiOut[n]=value;
   E1803_digi_write((unsigned char)n,E1803_COMMAND_FLAG_STREAM,lastDigiOut[n],0xFF);
};

E1803_API void write_io_port_list(unsigned short value)
{
   n_write_io_port_list(currHead,value);
};

E1803_API void n_write_io_port(unsigned short n, unsigned short value)
{
   n_write_io_port_list(n,value);
};

E1803_API void write_io_port(unsigned short value)
{
   n_write_io_port_list(currHead,value);
};

E1803_API void n_set_io_bit(unsigned short n, unsigned short mask1)
{
   lastDigiOut[n]|=mask1;
   E1803_digi_write((unsigned char)n,E1803_COMMAND_FLAG_STREAM,lastDigiOut[n],0xFF);
};

E1803_API void set_io_bit(unsigned short mask1)
{
   n_set_io_bit(currHead,mask1);
};

E1803_API void n_clear_io_bit(unsigned short n, unsigned short mask0)
{
   lastDigiOut[n]&=~mask0;
   E1803_digi_write((unsigned char)n,E1803_COMMAND_FLAG_STREAM,lastDigiOut[n],0xFF);
};

E1803_API void clear_io_bit(unsigned short mask0)
{
   n_clear_io_bit(currHead,mask0);
};

E1803_API unsigned short n_get_io_status(unsigned short n)
{
   return lastDigiOut[n];
};

E1803_API unsigned short get_io_status(void)
{
   return n_get_io_status(currHead);
};


E1803_API unsigned short read_io_port(void)
{
   return n_read_io_port(currHead);
};

E1803_API long n_get_counts(unsigned short n)
{
   return 0x7FFFFFFF;
};

E1803_API long get_counts(void)
{
   return 0x7FFFFFFF;
};

E1803_API void n_set_fly_x(unsigned short n, double kx)
{
   flyX[n]=kx;
   E1803_digi_set_motf((unsigned char)n,flyX[n],flyY[n]);
};

E1803_API void set_fly_x(double kx)
{
   n_set_fly_x(currHead,kx);
};

E1803_API void n_set_fly_y(unsigned short n, double ky)
{
   flyY[n]=ky;
   E1803_digi_set_motf((unsigned char)n,flyX[n],flyY[n]);
};

E1803_API void set_fly_y(double ky)
{
   n_set_fly_y(currHead,ky);
};

E1803_API void get_status(unsigned short *busy, unsigned short *position)
{
   n_get_status(currHead,busy,position);
};
