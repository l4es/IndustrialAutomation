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

//#include "libe1803.h"
#include "libe1803inter.h"

E1803_API short n_load_z_table(unsigned short n, double a, double b, double c)
{
    return 0;
};
E1803_API short load_z_table(double a, double b, double c)
{
    return 0;
};
E1803_API void n_set_defocus_list(unsigned short n, short value)
{
};
E1803_API void set_defocus_list(short value)
{
};
E1803_API void n_set_offset_list(unsigned short n, short xoffset, short yoffset)
{
};
E1803_API void set_offset_list(short xoffset, short yoffset)
{
};
E1803_API void n_laser_on_list(unsigned short n, unsigned short value)
{
};
E1803_API void laser_on_list(unsigned short value)
{
};
E1803_API void n_set_list_jump(unsigned short n, unsigned short position)
{
};
E1803_API void set_list_jump(unsigned short position)
{
};
E1803_API void n_set_input_pointer(unsigned short n, unsigned short pointer)
{
};
E1803_API void set_input_pointer(unsigned short pointer)
{
};
E1803_API void n_list_call(unsigned short n, unsigned short position)
{
};
E1803_API void list_call(unsigned short position)
{
};
E1803_API void n_list_return(unsigned short n)
{
};
E1803_API void list_return(void)
{
};
E1803_API void n_z_out_list(unsigned short n, short z)
{
};
E1803_API void z_out_list(short z)
{
};
E1803_API void n_timed_jump_abs(unsigned short n, short x, short y, double time)
{
};
E1803_API void timed_jump_abs(short x, short y, double time)
{
};
E1803_API void n_timed_mark_abs(unsigned short n, short x, short y, double time)
{
};
E1803_API void timed_mark_abs(short x, short y, double time)
{
};
E1803_API void n_timed_jump_rel(unsigned short n, short dx, short dy, double time)
{
};
E1803_API void timed_jump_rel(short dx, short dy, double time)
{
};
E1803_API void n_timed_mark_rel(unsigned short n, short dx, short dy, double time)
{
};
E1803_API void timed_mark_rel(short dx, short dy, double time)
{
};
E1803_API void n_set_fly_rot(unsigned short n, double resolution)
{
};
E1803_API void set_fly_rot(double resolution)
{
};
E1803_API void n_fly_return(unsigned short n, short x, short y)
{
};
E1803_API void fly_return(short x, short y)
{
};
E1803_API void n_calculate_fly(unsigned short n, unsigned short direction, double distance)
{
};
E1803_API void calculate_fly(unsigned short direction, double distance)
{
};
E1803_API void n_select_cor_table_list(unsigned short n, unsigned short heada, unsigned short headb)
{
};
E1803_API void select_cor_table_list(unsigned short heada, unsigned short headb)
{
};
E1803_API void n_set_wait(unsigned short n, unsigned short value)
{
};
E1803_API void set_wait(unsigned short value)
{
};
E1803_API void n_simulate_ext_start(unsigned short n, short delay, short encoder)
{
};
E1803_API void simulate_ext_start(short delay, short encoder)
{
};
E1803_API void n_set_pixel_line(unsigned short n, unsigned short pixelmode, unsigned short pixelperiod, double dx, double dy)
{
};
E1803_API void set_pixel_line(unsigned short pixelmode, unsigned short pixelperiod, double dx, double dy)
{
};
E1803_API void n_set_pixel(unsigned short n, unsigned short pulswidth, unsigned short davalue, unsigned short adchannel)
{
};
E1803_API void set_pixel(unsigned short pulswidth, unsigned short davalue, unsigned short adchannel)
{
};
E1803_API void n_set_extstartpos_list(unsigned short n, unsigned short position)
{
};
E1803_API void set_extstartpos_list(unsigned short position)
{
};
E1803_API void n_laser_signal_on_list(unsigned short n)
{
};
E1803_API void laser_signal_on_list(void)
{
};
E1803_API void n_laser_signal_off_list(unsigned short n)
{
};
E1803_API void laser_signal_off_list(void)
{
};
E1803_API void n_set_io_cond_list(unsigned short n, unsigned short mask_1, unsigned short mask_0, unsigned short mask_set)
{
};
E1803_API void set_io_cond_list(unsigned short mask_1, unsigned short mask_0, unsigned short mask_set)
{
};
E1803_API void n_clear_io_cond_list(unsigned short n, unsigned short mask_1, unsigned short mask_0, unsigned short mask_clear)
{
};
E1803_API void clear_io_cond_list(unsigned short mask_1, unsigned short mask_0, unsigned short mask_clear)
{
};
E1803_API void n_list_jump_cond(unsigned short n, unsigned short mask_1, unsigned short mask_0, unsigned short position)
{
};
E1803_API void list_jump_cond(unsigned short mask_1, unsigned short mask_0, unsigned short position)
{
};
E1803_API void n_list_call_cond(unsigned short n, unsigned short mask_1, unsigned short mask_0, unsigned short position)
{
};
E1803_API void list_call_cond(unsigned short mask_1, unsigned short mask_0, unsigned short position)
{
};
E1803_API void n_save_and_restart_timer(unsigned short n)
{
};
E1803_API void save_and_restart_timer(void)
{
};
E1803_API void n_set_ext_start_delay_list(unsigned short n, short delay, short encoder)
{
};
E1803_API void set_ext_start_delay_list(short delay, short encoder)
{
};
E1803_API void n_set_trigger(unsigned short n, unsigned short sampleperiod, unsigned short channel1, unsigned short channel2)
{
};
E1803_API void set_trigger(unsigned short sampleperiod, unsigned short signal1, unsigned short signal2)
{
};
E1803_API void n_arc_rel(unsigned short n, short dx, short dy, double angle)
{
};
E1803_API void arc_rel(short dx, short dy, double angle)
{
};
E1803_API void n_arc_abs(unsigned short n, short x, short y, double angle)
{
};
E1803_API void arc_abs(short x, short y, double angle)
{
};
E1803_API void drilling(short pulsewidth, short relencoderdelay)
{
};
E1803_API void regulation(void)
{
};
E1803_API void flyline(short encoderdelay)
{
};
E1803_API unsigned short n_get_input_pointer(unsigned short n)
{
    return 0;
};
E1803_API unsigned short get_input_pointer(void)
{
    return 0;
};

E1803_API unsigned short n_get_marking_info(unsigned short n)
{
    return 0;
};
E1803_API unsigned short get_marking_info(void)
{
    return 0;
};
E1803_API void n_auto_change_pos(unsigned short n, unsigned short start)
{
};
E1803_API void auto_change_pos(unsigned short start)
{
};
E1803_API void aut_change(void)
{
};
E1803_API void n_start_loop(unsigned short n)
{
};
E1803_API void start_loop(void)
{
};
E1803_API void n_quit_loop(unsigned short n)
{
};
E1803_API void quit_loop(void)
{
};
E1803_API void n_write_da_2(unsigned short n, unsigned short value)
{
};
E1803_API void write_da_2(unsigned short value)
{
};
E1803_API void n_set_max_counts(unsigned short n, long counts)
{
};
E1803_API void set_max_counts(long counts)
{
};
E1803_API void n_set_offset(unsigned short n, short xoffset, short yoffset)
{
};
E1803_API void set_offset(short xoffset, short yoffset)
{
};
E1803_API void n_disable_laser(unsigned short n)
{
};
E1803_API void disable_laser(void)
{
};
E1803_API void n_enable_laser(unsigned short n)
{
};
E1803_API void enable_laser(void)
{
};
E1803_API void n_stop_list(unsigned short n)
{
};
E1803_API void stop_list(void)
{
};
E1803_API void n_restart_list(unsigned short n)
{
};
E1803_API void restart_list(void)
{
};
E1803_API void n_get_xyz_pos(unsigned short n, short *x, short *y, short *z)
{
};
E1803_API void get_xyz_pos(short *x, short *y, short *z)
{
};
E1803_API void n_get_xy_pos(unsigned short n, short *x, short *y)
{
};
E1803_API void get_xy_pos(short *x, short *y)
{
};
E1803_API void n_select_list(unsigned short n, unsigned short list_2)
{
};
E1803_API void select_list(unsigned short list_2)
{
};
E1803_API void n_z_out(unsigned short n, short z)
{
};
E1803_API void z_out(short z)
{
};
E1803_API void n_laser_signal_on(unsigned short n)
{
};
E1803_API void laser_signal_on(void)
{
};
E1803_API void n_laser_signal_off(unsigned short n)
{
};
E1803_API void laser_signal_off(void)
{
};
E1803_API void n_set_delay_mode(unsigned short n, unsigned short varpoly, unsigned short directmove3d, unsigned short edgelevel, unsigned short minjumpdelay, unsigned short jumplengthlimit)
{
};
E1803_API void set_delay_mode(unsigned short varpoly, unsigned short directmove3d, unsigned short edgelevel, unsigned short minjumpdelay, unsigned short jumplengthlimit)
{
};
E1803_API void n_set_piso_control(unsigned short n, unsigned short l1, unsigned short l2)
{
};
E1803_API void set_piso_control(unsigned short l1, unsigned short l2)
{
};
E1803_API void n_select_status(unsigned short n, unsigned short mode)
{
};
E1803_API void select_status(unsigned short mode)
{
};
E1803_API void n_get_encoder(unsigned short n, short *zx, short *zy)
{
};
E1803_API void get_encoder(short *zx, short *zy)
{
};
E1803_API void n_select_cor_table(unsigned short n, unsigned short heada, unsigned short headb)
{
};
E1803_API void select_cor_table(unsigned short heada, unsigned short headb)
{
};
E1803_API void n_execute_at_pointer(unsigned short n, unsigned short position)
{
};
E1803_API void execute_at_pointer(unsigned short position)
{
};
E1803_API unsigned short n_get_head_status(unsigned short n, unsigned short head)
{
    return 0;
};
E1803_API unsigned short get_head_status(unsigned short head)
{
    return 0;
};
E1803_API void n_simulate_encoder(unsigned short n, unsigned short channel)
{
};
E1803_API void simulate_encoder(unsigned short channel)
{
};
E1803_API void n_set_hi(unsigned short n, double galvogainx, double galvogainy, short galvooffsetx, short galvooffsety, short head)
{
};
E1803_API void set_hi(double galvogainx, double galvogainy, short galvooffsetx, short galvooffsety, short head)
{
};
E1803_API void n_release_wait(unsigned short n)
{
};
E1803_API void release_wait(void)
{
};
E1803_API unsigned short n_get_wait_status(unsigned short n)
{
    return 0;
};
E1803_API unsigned short get_wait_status(void)
{
    return 0;
};
E1803_API void n_set_ext_start_delay(unsigned short n, short delay, short encoder)
{
};
E1803_API void set_ext_start_delay(short delay, short encoder)
{
};
E1803_API void n_home_position(unsigned short n, short xhome, short yhome)
{
};
E1803_API void home_position(short xhome, short yhome)
{
};
E1803_API void n_set_rot_center(unsigned short n, long center_x, long center_y)
{
};
E1803_API void set_rot_center(long center_x, long center_y)
{
};
E1803_API unsigned short n_read_ad_x(unsigned short n, unsigned short x)
{
    return 0;
};
E1803_API unsigned short read_ad_x(unsigned short x)
{
    return 0;
};
E1803_API unsigned short n_read_pixel_ad(unsigned short n, unsigned short pos)
{
    return 0;
};
E1803_API unsigned short read_pixel_ad(unsigned short pos)
{
    return 0;
};
E1803_API short n_get_z_distance(unsigned short n, short x, short y, short z)
{
    return 0;
};
E1803_API short get_z_distance(short x, short y, short z)
{
    return 0;
};
E1803_API double n_get_time(unsigned short n)
{
    return 0;
};
E1803_API double get_time(void)
{
    return 0;
};
E1803_API void n_set_defocus(unsigned short n, short value)
{
};
E1803_API void set_defocus(short value)
{
};
E1803_API void n_set_softstart_mode(unsigned short n, unsigned short mode, unsigned short number, unsigned short restartdelay)
{
};
E1803_API void set_softstart_mode(unsigned short mode, unsigned short number, unsigned short resetdelay)
{
};
E1803_API void n_set_softstart_level(unsigned short n, unsigned short index, unsigned short level)
{
};
E1803_API void set_softstart_level(unsigned short index, unsigned short level)
{
};
E1803_API void n_control_command(unsigned short n, unsigned short head, unsigned short axis, unsigned short data)
{
};
E1803_API void control_command(unsigned short head, unsigned short axis, unsigned short data)
{
};
E1803_API short load_cor(const char* filename)
{
    return 0;
};
E1803_API short load_pro(const char* filename)
{
    return 0;
};
E1803_API unsigned short n_get_serial_number(unsigned short n)
{
    return 0;
};
E1803_API unsigned short get_serial_number(void)
{
    return 0;
};
E1803_API long n_get_serial_number_32(unsigned short n)
{
    return 0;
};
E1803_API long get_serial_number_32(void)
{
    return 0;
};
E1803_API void get_hi_data(unsigned short *x1, unsigned short *x2, unsigned short *y1, unsigned short *y2)
{
};
E1803_API short n_auto_cal(unsigned short n, unsigned short head, unsigned short command)
{
    return 0;
};
E1803_API short auto_cal(unsigned short head, unsigned short command)
{
    return 0;
};
E1803_API unsigned short n_get_list_space(unsigned short n)
{
    return 0;
};
E1803_API unsigned short get_list_space(void)
{
    return 0;
};
E1803_API short teachin(const char* filename, short xin, short yin, short zin, double ll0, short *xout, short *yout, short *zout)
{
    return 0;
};
E1803_API short n_get_value(unsigned short n, unsigned short signal)
{
    return 0;
};
E1803_API short get_value(unsigned short signal)
{
    return 0;
};
E1803_API void set_duty_cycle_table(unsigned short index, unsigned short dutycycle)
{
};
E1803_API void n_move_to(unsigned short n, unsigned short position)
{
};
E1803_API void move_to(unsigned short position)
{
};
E1803_API short getmemory(unsigned short adr)
{
    return 0;
};
E1803_API void n_get_waveform(unsigned short n, unsigned short channel, unsigned short istop, signed short *memptr)
{
};
E1803_API void get_waveform(unsigned short channel, unsigned short istop, signed short *memptr)
{
};
E1803_API void n_measurement_status(unsigned short n, unsigned short *busy, unsigned short *position)
{
};
E1803_API void measurement_status(unsigned short *busy, unsigned short *position)
{
};
E1803_API short n_load_varpolydelay(unsigned short n, const char* stbfilename, unsigned short tableno)
{
    return 0;
};
E1803_API short load_varpolydelay(const char* stbfilename, unsigned short tableno)
{
    return 0;
};
E1803_API void n_write_da_2_list(unsigned short n, unsigned short value)
{
};
E1803_API void write_da_2_list(unsigned short value)
{
};
