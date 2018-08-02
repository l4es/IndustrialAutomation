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

/*
This file partially bases on OpenLaserShowController.h which is (c) by 2009
Chris Favreau
*/


#ifndef E1701_INTER_H
#define E1701_INTER_H

// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the E1701_EXPORTS
// symbol defined on the command line. This symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// E1701_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
#ifdef ENV_LINUX
 #ifdef E1701_EXPORTS
  #define E1701_API __attribute ((visibility ("default")))
 #else
  #define E1701_API
 #endif
#else
 #ifdef E1701_EXPORTS
  #define E1701_API __declspec(dllexport)
 #else
  #define E1701_API __declspec(dllimport)
 #endif
#endif


struct point_xy_rgbi 
{
   unsigned short x,y;
   unsigned char  r,g,b,i;
};


struct point_xy_rgbia 
{
   unsigned short x,y;
   unsigned short r,g,b,i;
   unsigned short a1,a2;
};

#define GET_STATUS_READY	1
#define GET_STATUS_BUSY		2


#define SC_SCI_OK                      1
#define	SC_SCI_ERROR                  -1
#define	SC_SCI_ERROR_NOT_INITIALIZED (SC_SCI_ERROR-0x301)


// ***** OLSC v2 API constants and definitions ********************************************************************
// Shutter State
#define LASER_SHOW_DEVICE_SHUTTER_STATE_ON		0
#define LASER_SHOW_DEVICE_SHUTTER_STATE_OFF		1

// Errors
#define OLSC_ERROR_SUCCESS						1
#define OLSC_ERROR_NONE							0
#define OLSC_ERROR_INVALID_PARAMETER			-1
#define OLSC_ERROR_FAILED						-2

typedef void(*POLSCCALLBACK)(int device, int status);

struct LASER_SHOW_DEVICE_CAPABILITIES
{
   // Device Name
   char name[1024];
   // Device Version
   int	version_major;
   int	version_minor;
   // Min Speed/Max Speed
   int min_speed;
   int max_speed;
   // Min Frame Size/Max Frame Size
   int min_frame_size;
   int max_frame_size;
   // DMX In/Out?
   bool has_dmx_in;
   bool has_dmx_out;
   // TTL In/Out?
   bool has_ttl_in;
   bool has_ttl_out;
   // X/Y Resolution (8 to 16 bits)
   int	xy_resolution;
   // Color Resolution (1 to 16 bits)
   int color_resolution;
   // Uses callbacks?
   bool uses_callbacks;
};

struct LASER_SHOW_DEVICE_POINT
{
   short x;
   short y;
   short r;
   short g;
   short b;
   short i;
};

struct LASER_SHOW_DEVICE_FRAME
{
   int display_speed;
   int point_count;
   struct LASER_SHOW_DEVICE_POINT *points;
};
// ***** end of OLSC v2 API constants and definitions **************************************************************


extern "C"
{
   // ***** E1701 easy interface connection function ***************************************************************************
   E1701_API unsigned char E1701_set_connection(const char *address);
   // ***** E1701 easy interface connection function ***************************************************************************

   // ***** RTC4 compatibility functions ***************************************************************************************
   E1701_API short getmemory(unsigned short adr);
   E1701_API void n_get_waveform(unsigned short n, unsigned short channel, unsigned short istop, signed short *memptr);
   E1701_API void get_waveform(unsigned short channel, unsigned short istop, signed short *memptr);
   E1701_API void n_measurement_status(unsigned short n, unsigned short *busy, unsigned short *position);
   E1701_API void measurement_status(unsigned short *busy, unsigned short *position);
   E1701_API short n_load_varpolydelay(unsigned short n, const char* stbfilename, unsigned short tableno);
   E1701_API short load_varpolydelay(const char* stbfilename, unsigned short tableno);
   E1701_API short n_load_program_file(unsigned short n, const char* name);
   E1701_API short load_program_file(const char* name);
   E1701_API short n_load_correction_file(unsigned short n, const char* filename, short cortable, double kx, double ky, double phi, double xoffset, double yoffset);
   E1701_API short load_correction_file(const char* filename, short cortable, double kx, double ky, double phi, double xoffset, double yoffset);
   E1701_API short n_load_z_table(unsigned short n, double a, double b, double c);
   E1701_API short load_z_table(double a, double b, double c);
   E1701_API void n_list_nop(unsigned short n);
   E1701_API void list_nop(void);
   E1701_API void n_set_end_of_list(unsigned short n);
   E1701_API void set_end_of_list(void);
   E1701_API void n_jump_abs_3d(unsigned short n, short x, short y, short z);
   E1701_API void jump_abs_3d(short x, short y, short z);
   E1701_API void n_jump_abs(unsigned short n, short x, short y);
   E1701_API void jump_abs(short x, short y);
   E1701_API void n_mark_abs_3d(unsigned short n, short x, short y, short z);
   E1701_API void mark_abs_3d(short x, short y, short z);
   E1701_API void n_mark_abs(unsigned short n, short x, short y);
   E1701_API void mark_abs(short x, short y);
   E1701_API void n_jump_rel_3d(unsigned short n, short dx, short dy, short dz);
   E1701_API void jump_rel_3d(short dx, short dy, short dz);
   E1701_API void n_jump_rel(unsigned short n, short dx, short dy);
   E1701_API void jump_rel(short dx, short dy);
   E1701_API void n_mark_rel_3d(unsigned short n, short dx, short dy, short dz);
   E1701_API void mark_rel_3d(short dx, short dy, short dz);
   E1701_API void n_mark_rel(unsigned short n, short dx, short dy);
   E1701_API void mark_rel(short dx, short dy);
   E1701_API void n_write_8bit_port_list(unsigned short n, unsigned short value);
   E1701_API void write_8bit_port_list(unsigned short value);
   E1701_API void n_write_da_1_list(unsigned short n, unsigned short value);
   E1701_API void write_da_1_list(unsigned short value);
   E1701_API void n_write_da_2_list(unsigned short n, unsigned short value);
   E1701_API void write_da_2_list(unsigned short value);
   E1701_API void n_set_matrix_list(unsigned short n, unsigned short i, unsigned short j, double mij);
   E1701_API void set_matrix_list(unsigned short i, unsigned short j, double mij);
   E1701_API void n_set_defocus_list(unsigned short n, short value);
   E1701_API void set_defocus_list(short value);
   E1701_API void n_set_control_mode_list(unsigned short n, unsigned short mode);
   E1701_API void set_control_mode_list(unsigned short mode);
   E1701_API void n_set_offset_list(unsigned short n, short xoffset, short yoffset);
   E1701_API void set_offset_list(short xoffset, short yoffset);
   E1701_API void n_long_delay(unsigned short n, unsigned short value);
   E1701_API void long_delay(unsigned short value);
   E1701_API void n_laser_on_list(unsigned short n, unsigned short value);
   E1701_API void laser_on_list(unsigned short value);
   E1701_API void n_set_jump_speed(unsigned short n, double speed);
   E1701_API void set_jump_speed(double speed);
   E1701_API void n_set_mark_speed(unsigned short n, double speed);
   E1701_API void set_mark_speed(double speed);
   E1701_API void n_set_laser_delays(unsigned short n, short ondelay, short offdelay);
   E1701_API void set_laser_delays(short ondelay, short offdelay);
   E1701_API void n_set_scanner_delays(unsigned short n, unsigned short jumpdelay, unsigned short markdelay, unsigned short polydelay);
   E1701_API void set_scanner_delays(unsigned short jumpdelay, unsigned short markdelay, unsigned short polydelay);
   E1701_API void n_set_list_jump(unsigned short n, unsigned short position);
   E1701_API void set_list_jump(unsigned short position);
   E1701_API void n_set_input_pointer(unsigned short n, unsigned short pointer);
   E1701_API void set_input_pointer(unsigned short pointer);
   E1701_API void n_list_call(unsigned short n, unsigned short position);
   E1701_API void list_call(unsigned short position);
   E1701_API void n_list_return(unsigned short n);
   E1701_API void list_return(void);
   E1701_API void n_z_out_list(unsigned short n, short z);
   E1701_API void z_out_list(short z);
   E1701_API void n_set_standby_list(unsigned short n, unsigned short half_period, unsigned short pulse);
   E1701_API void set_standby_list(unsigned short half_period, unsigned short pulse);
   E1701_API void n_timed_jump_abs(unsigned short n, short x, short y, double time);
   E1701_API void timed_jump_abs(short x, short y, double time);
   E1701_API void n_timed_mark_abs(unsigned short n, short x, short y, double time);
   E1701_API void timed_mark_abs(short x, short y, double time);
   E1701_API void n_timed_jump_rel(unsigned short n, short dx, short dy, double time);
   E1701_API void timed_jump_rel(short dx, short dy, double time);
   E1701_API void n_timed_mark_rel(unsigned short n, short dx, short dy, double time);
   E1701_API void timed_mark_rel(short dx, short dy, double time);
   E1701_API void n_set_laser_timing(unsigned short n, unsigned short halfperiod, unsigned short pulse1, unsigned short pulse2, unsigned short timebase);
   E1701_API void set_laser_timing(unsigned short halfperiod, unsigned short pulse1, unsigned short pulse2, unsigned short timebase);
   E1701_API void n_set_wobbel_xy(unsigned short n, unsigned short long_wob, unsigned short trans_wob, double frequency);
   E1701_API void set_wobbel_xy(unsigned short long_wob, unsigned short trans_wob, double frequency);
   E1701_API void n_set_wobbel(unsigned short n, unsigned short amplitude, double frequency);
   E1701_API void set_wobbel(unsigned short amplitude, double frequency);
   E1701_API void n_set_fly_x(unsigned short n, double kx);
   E1701_API void set_fly_x(double kx);
   E1701_API void n_set_fly_y(unsigned short n, double ky);
   E1701_API void set_fly_y(double ky);
   E1701_API void n_set_fly_rot(unsigned short n, double resolution);
   E1701_API void set_fly_rot(double resolution);
   E1701_API void n_fly_return(unsigned short n, short x, short y);
   E1701_API void fly_return(short x, short y);
   E1701_API void n_calculate_fly(unsigned short n, unsigned short direction, double distance);
   E1701_API void calculate_fly(unsigned short direction, double distance);
   E1701_API void n_write_io_port_list(unsigned short n, unsigned short value);
   E1701_API void write_io_port_list(unsigned short value);
   E1701_API void n_select_cor_table_list(unsigned short n, unsigned short heada, unsigned short headb);
   E1701_API void select_cor_table_list(unsigned short heada, unsigned short headb);
   E1701_API void n_set_wait(unsigned short n, unsigned short value);
   E1701_API void set_wait(unsigned short value);
   E1701_API void n_simulate_ext_start(unsigned short n, short delay, short encoder);
   E1701_API void simulate_ext_start(short delay, short encoder);
   E1701_API void n_write_da_x_list(unsigned short n, unsigned short x, unsigned short value);
   E1701_API void write_da_x_list(unsigned short x, unsigned short value);
   E1701_API void n_set_pixel_line(unsigned short n, unsigned short pixelmode, unsigned short pixelperiod, double dx, double dy);
   E1701_API void set_pixel_line(unsigned short pixelmode, unsigned short pixelperiod, double dx, double dy);
   E1701_API void n_set_pixel(unsigned short n, unsigned short pulswidth, unsigned short davalue, unsigned short adchannel);
   E1701_API void set_pixel(unsigned short pulswidth, unsigned short davalue, unsigned short adchannel);
   E1701_API void n_set_extstartpos_list(unsigned short n, unsigned short position);
   E1701_API void set_extstartpos_list(unsigned short position);
   E1701_API void n_laser_signal_on_list(unsigned short n);
   E1701_API void laser_signal_on_list(void);
   E1701_API void n_laser_signal_off_list(unsigned short n);
   E1701_API void laser_signal_off_list(void);
   E1701_API void n_set_firstpulse_killer_list(unsigned short n, unsigned short fpk);
   E1701_API void set_firstpulse_killer_list(unsigned short fpk);
   E1701_API void n_set_io_cond_list(unsigned short n, unsigned short mask_1, unsigned short mask_0, unsigned short mask_set);
   E1701_API void set_io_cond_list(unsigned short mask_1, unsigned short mask_0, unsigned short mask_set);
   E1701_API void n_clear_io_cond_list(unsigned short n, unsigned short mask_1, unsigned short mask_0, unsigned short mask_clear);
   E1701_API void clear_io_cond_list(unsigned short mask_1, unsigned short mask_0, unsigned short mask_clear);
   E1701_API void n_list_jump_cond(unsigned short n, unsigned short mask_1, unsigned short mask_0, unsigned short position);
   E1701_API void list_jump_cond(unsigned short mask_1, unsigned short mask_0, unsigned short position);
   E1701_API void n_list_call_cond(unsigned short n, unsigned short mask_1, unsigned short mask_0, unsigned short position);
   E1701_API void list_call_cond(unsigned short mask_1, unsigned short mask_0, unsigned short position);
   E1701_API void n_save_and_restart_timer(unsigned short n);
   E1701_API void save_and_restart_timer(void);
   E1701_API void n_set_ext_start_delay_list(unsigned short n, short delay, short encoder);
   E1701_API void set_ext_start_delay_list(short delay, short encoder);
   E1701_API void n_set_trigger(unsigned short n, unsigned short sampleperiod, unsigned short channel1, unsigned short channel2);
   E1701_API void set_trigger(unsigned short sampleperiod, unsigned short signal1, unsigned short signal2);
   E1701_API void n_arc_rel(unsigned short n, short dx, short dy, double angle);
   E1701_API void arc_rel(short dx, short dy, double angle);
   E1701_API void n_arc_abs(unsigned short n, short x, short y, double angle);
   E1701_API void arc_abs(short x, short y, double angle);
   E1701_API void drilling(short pulsewidth, short relencoderdelay);
   E1701_API void regulation(void);
   E1701_API void flyline(short encoderdelay);
   E1701_API unsigned short n_get_input_pointer(unsigned short n);
   E1701_API unsigned short get_input_pointer(void);
   E1701_API void select_rtc(unsigned short cardno);
   E1701_API unsigned short rtc4_count_cards(void);
   E1701_API void n_get_status(unsigned short n, unsigned short *busy, unsigned short *position);
   E1701_API void get_status(unsigned short *busy, unsigned short *position);
   E1701_API unsigned short n_read_status(unsigned short n);
   E1701_API unsigned short read_status(void);
   E1701_API unsigned short n_get_startstop_info(unsigned short n);
   E1701_API unsigned short get_startstop_info(void);
   E1701_API unsigned short n_get_marking_info(unsigned short n);
   E1701_API unsigned short get_marking_info(void);
   E1701_API unsigned short get_dll_version(void);
   E1701_API void n_set_start_list_1(unsigned short n);
   E1701_API void set_start_list_1(void);
   E1701_API void n_set_start_list_2(unsigned short n);
   E1701_API void set_start_list_2(void);
   E1701_API void n_set_start_list(unsigned short n, unsigned short listno);
   E1701_API void set_start_list(unsigned short listno);
   E1701_API void n_execute_list_1(unsigned short n);
   E1701_API void execute_list_1(void);
   E1701_API void n_execute_list_2(unsigned short n);
   E1701_API void execute_list_2(void);
   E1701_API void n_execute_list(unsigned short n, unsigned short listno);
   E1701_API void execute_list(unsigned short listno);
   E1701_API void n_write_8bit_port(unsigned short n, unsigned short value);
   E1701_API void write_8bit_port(unsigned short value);
   E1701_API void n_write_io_port(unsigned short n, unsigned short value);
   E1701_API void write_io_port(unsigned short value);
   E1701_API void n_auto_change(unsigned short n);
   E1701_API void auto_change(void);
   E1701_API void n_auto_change_pos(unsigned short n, unsigned short start);
   E1701_API void auto_change_pos(unsigned short start);
   E1701_API void aut_change(void);
   E1701_API void n_start_loop(unsigned short n);
   E1701_API void start_loop(void);
   E1701_API void n_quit_loop(unsigned short n);
   E1701_API void quit_loop(void);
   E1701_API void n_set_list_mode(unsigned short n, unsigned short mode);
   E1701_API void set_list_mode(unsigned short mode);
   E1701_API void n_stop_execution(unsigned short n);
   E1701_API void stop_execution(void);
   E1701_API unsigned short n_read_io_port(unsigned short n);
   E1701_API unsigned short read_io_port(void);
   E1701_API void n_write_da_1(unsigned short n, unsigned short value);
   E1701_API void write_da_1(unsigned short value);
   E1701_API void n_write_da_2(unsigned short n, unsigned short value);
   E1701_API void write_da_2(unsigned short value);
   E1701_API void n_set_max_counts(unsigned short n, long counts);
   E1701_API void set_max_counts(long counts);
   E1701_API long n_get_counts(unsigned short n);
   E1701_API long get_counts(void);
   E1701_API void n_set_matrix(unsigned short n, double m11, double m12, double m21, double m22);
   E1701_API void set_matrix(double m11, double m12, double m21, double m22);
   E1701_API void n_set_offset(unsigned short n, short xoffset, short yoffset);
   E1701_API void set_offset(short xoffset, short yoffset);
   E1701_API void n_goto_xyz(unsigned short n, short x, short y, short z);
   E1701_API void goto_xyz(short x, short y, short z);
   E1701_API void n_goto_xy(unsigned short n, short x, short y);
   E1701_API void goto_xy(short x, short y);
   E1701_API unsigned short n_get_hex_version(unsigned short n);
   E1701_API unsigned short get_hex_version(void);
   E1701_API void n_disable_laser(unsigned short n);
   E1701_API void disable_laser(void);
   E1701_API void n_enable_laser(unsigned short n);
   E1701_API void enable_laser(void);
   E1701_API void n_stop_list(unsigned short n);
   E1701_API void stop_list(void);
   E1701_API void n_restart_list(unsigned short n);
   E1701_API void restart_list(void);
   E1701_API void n_get_xyz_pos(unsigned short n, short *x, short *y, short *z);
   E1701_API void get_xyz_pos(short *x, short *y, short *z);
   E1701_API void n_get_xy_pos(unsigned short n, short *x, short *y);
   E1701_API void get_xy_pos(short *x, short *y);
   E1701_API void n_select_list(unsigned short n, unsigned short list_2);
   E1701_API void select_list(unsigned short list_2);
   E1701_API void n_z_out(unsigned short n, short z);
   E1701_API void z_out(short z);
   E1701_API void n_set_firstpulse_killer(unsigned short n, unsigned short fpk);
   E1701_API void set_firstpulse_killer(unsigned short fpk);
   E1701_API void n_set_standby(unsigned short n, unsigned short half_period, unsigned short pulse);
   E1701_API void set_standby(unsigned short half_period, unsigned short pulse);
   E1701_API void n_laser_signal_on(unsigned short n);
   E1701_API void laser_signal_on(void);
   E1701_API void n_laser_signal_off(unsigned short n);
   E1701_API void laser_signal_off(void);
   E1701_API void n_set_delay_mode(unsigned short n, unsigned short varpoly, unsigned short directmove3d, unsigned short edgelevel, unsigned short minjumpdelay, unsigned short jumplengthlimit);
   E1701_API void set_delay_mode(unsigned short varpoly, unsigned short directmove3d, unsigned short edgelevel, unsigned short minjumpdelay, unsigned short jumplengthlimit);
   E1701_API void n_set_piso_control(unsigned short n, unsigned short l1, unsigned short l2);
   E1701_API void set_piso_control(unsigned short l1, unsigned short l2);
   E1701_API void n_select_status(unsigned short n, unsigned short mode);
   E1701_API void select_status(unsigned short mode);
   E1701_API void n_get_encoder(unsigned short n, short *zx, short *zy);
   E1701_API void get_encoder(short *zx, short *zy);
   E1701_API void n_select_cor_table(unsigned short n, unsigned short heada, unsigned short headb);
   E1701_API void select_cor_table(unsigned short heada, unsigned short headb);
   E1701_API void n_execute_at_pointer(unsigned short n, unsigned short position);
   E1701_API void execute_at_pointer(unsigned short position);
   E1701_API unsigned short n_get_head_status(unsigned short n, unsigned short head);
   E1701_API unsigned short get_head_status(unsigned short head);
   E1701_API void n_simulate_encoder(unsigned short n, unsigned short channel);
   E1701_API void simulate_encoder(unsigned short channel);
   E1701_API void n_set_hi(unsigned short n, double galvogainx, double galvogainy, short galvooffsetx, short galvooffsety, short head);
   E1701_API void set_hi(double galvogainx, double galvogainy, short galvooffsetx, short galvooffsety, short head);
   E1701_API void n_release_wait(unsigned short n);
   E1701_API void release_wait(void);
   E1701_API unsigned short n_get_wait_status(unsigned short n);
   E1701_API unsigned short get_wait_status(void);
   E1701_API void n_set_control_mode(unsigned short n, unsigned short mode);
   E1701_API void set_control_mode(unsigned short mode);
   E1701_API void n_set_laser_mode(unsigned short n, unsigned short mode);
   E1701_API void set_laser_mode(unsigned short mode);
   E1701_API void n_set_ext_start_delay(unsigned short n, short delay, short encoder);
   E1701_API void set_ext_start_delay(short delay, short encoder);
   E1701_API void n_home_position(unsigned short n, short xhome, short yhome);
   E1701_API void home_position(short xhome, short yhome);
   E1701_API void n_set_rot_center(unsigned short n, long center_x, long center_y);
   E1701_API void set_rot_center(long center_x, long center_y);
   E1701_API void n_dsp_start(unsigned short n);
   E1701_API void dsp_start(void);
   E1701_API void n_write_da_x(unsigned short n, unsigned short x, unsigned short value);
   E1701_API void write_da_x(unsigned short x, unsigned short value);
   E1701_API unsigned short n_read_ad_x(unsigned short n, unsigned short x);
   E1701_API unsigned short read_ad_x(unsigned short x);
   E1701_API unsigned short n_read_pixel_ad(unsigned short n, unsigned short pos);
   E1701_API unsigned short read_pixel_ad(unsigned short pos);
   E1701_API short n_get_z_distance(unsigned short n, short x, short y, short z);
   E1701_API short get_z_distance(short x, short y, short z);
   E1701_API unsigned short n_get_io_status(unsigned short n);
   E1701_API unsigned short get_io_status(void);
   E1701_API double n_get_time(unsigned short n);
   E1701_API double get_time(void);
   E1701_API void n_set_defocus(unsigned short n, short value);
   E1701_API void set_defocus(short value);
   E1701_API void n_set_softstart_mode(unsigned short n, unsigned short mode, unsigned short number, unsigned short restartdelay);
   E1701_API void set_softstart_mode(unsigned short mode, unsigned short number, unsigned short resetdelay);
   E1701_API void n_set_softstart_level(unsigned short n, unsigned short index, unsigned short level);
   E1701_API void set_softstart_level(unsigned short index, unsigned short level);
   E1701_API void n_control_command(unsigned short n, unsigned short head, unsigned short axis, unsigned short data);
   E1701_API void control_command(unsigned short head, unsigned short axis, unsigned short data);
   E1701_API short load_cor(const char* filename);
   E1701_API short load_pro(const char* filename);
   E1701_API unsigned short n_get_serial_number(unsigned short n);
   E1701_API unsigned short get_serial_number(void);
   E1701_API long n_get_serial_number_32(unsigned short n);
   E1701_API long get_serial_number_32(void);
   E1701_API unsigned short n_get_rtc_version(unsigned short n);
   E1701_API unsigned short get_rtc_version(void);
   E1701_API void get_hi_data(unsigned short *x1, unsigned short *x2, unsigned short *y1, unsigned short *y2);
   E1701_API short n_auto_cal(unsigned short n, unsigned short head, unsigned short command);
   E1701_API short auto_cal(unsigned short head, unsigned short command);
   E1701_API unsigned short n_get_list_space(unsigned short n);
   E1701_API unsigned short get_list_space(void);
   E1701_API short teachin(const char* filename, short xin, short yin, short zin, double ll0, short *xout, short *yout, short *zout);
   E1701_API short n_get_value(unsigned short n, unsigned short signal);
   E1701_API short get_value(unsigned short signal);
   E1701_API void n_set_io_bit(unsigned short n, unsigned short mask1);
   E1701_API void set_io_bit(unsigned short mask1);
   E1701_API void n_clear_io_bit(unsigned short n, unsigned short mask0);
   E1701_API void clear_io_bit(unsigned short mask0);
   E1701_API void set_duty_cycle_table(unsigned short index, unsigned short dutycycle);
   E1701_API void n_move_to(unsigned short n, unsigned short position);
   E1701_API void move_to(unsigned short position);
   // ***** end of RTC4 compatibility functions ********************************************************************************

#ifdef ENV_WINDOWS // SCI interface is Windows-only
   // ***** USC1/2 compatibility functions *************************************************************************************
   // rename E1701inter.DLL to sc_optic.dll for direct usage
   E1701_API long ScSCIInit(void);
   E1701_API long ScSCIInitInterface(void);
   E1701_API long ScSCISetCardType(const char *CardName);
   E1701_API long ScSCIInitOptic(void);
   E1701_API void ScSCIExit(void);
   E1701_API long ScSCISetContinuousMode(long Mode);
   E1701_API long ScSCIGetContinuousMode(long *Mode);
   E1701_API long ScSCIFlush(void);
   E1701_API long ScSCIEnableHeadSelect(int Enable);
   E1701_API long ScSCIMoveAbs(double X,double Y);
   E1701_API long ScSCIMoveAbs3D(double X,double Y,double Z);
   E1701_API long ScSCIGetHardwareState(long *state);
   E1701_API long ScSCIGetCurrentHead(long *headID);
   E1701_API long ScSCISetCurrentHead(long headID);
   E1701_API long ScSCIDevicePixelLine(float *PixelLine, long PixelCount, double dx, double dy, double dz);
   E1701_API long ScSCIRasterPixelLine(float *PixelLine, long PixelCount, double PixelPeriod);
   E1701_API long ScSCIRasterStart(double origin_x, double origin_y, double dir_x, double dir_y, double pixel_step, double line_step,double acceleration_time, double deceleration_time, double line_offset,double jump_speed, double wait_delay_0, double limit_value, long mode_flags);
   E1701_API long ScSCIRasterEnd();
   E1701_API long ScSCIGetDeviceName(long NameId,char *Name);
   E1701_API long ScSCIGetDeviceCaps(long CapsId,long *Caps);
   E1701_API long ScSCIGetDeviceData(long DataId, long DataLength, unsigned char *Data);
   E1701_API long ScSCISetDeviceData(long DataId, long DataLength, unsigned char *Data);
   E1701_API long ScSCIGetDeviceDelayD(long DelayId, double *Value);
   E1701_API long ScSCISetDeviceDelayD(long DelayId, double Value);
   E1701_API long ScSCIGetDeviceDelay(long DelayId, long *Value);
   E1701_API long ScSCISetDeviceDelay(long DelayId, double Value);
   E1701_API long ScSCIGetDeviceSpeed(long SpeedId,double *Value);
   E1701_API long ScSCISetDeviceSpeed(long SpeedId,double Value);
   E1701_API long ScSCIGetDeviceTimer(long TimerId,double *Value);
   E1701_API long ScSCISetDeviceTimer(long TimerId,double Value); 
   E1701_API long ScSCIGetDeviceOperationMode(long *Value);
   E1701_API long ScSCISetDeviceOperationMode(long Value);
   E1701_API long ScSCIGetDevicePortValue(long PortId,long *Value);
   E1701_API long ScSCISetDevicePortValue(long PortId,long Value);
   E1701_API long ScSCISetExecute(long Value);
   E1701_API long ScSCIGetExecute(long *Value);
   E1701_API long ScSCISetExternalTrigger(long Value);
   E1701_API long ScSCIGetExternalTrigger(long *Value);
   E1701_API long ScSCISetExternalTriggerCount(long Count);
   E1701_API long ScSCIGetExternalTriggerCount(long *Count);
   E1701_API long ScSCISetEnableHead(long Value);
   E1701_API long ScSCIGetEnableHead(long *Value);
   E1701_API long ScSCISetField(double XMin, double YMin, double XMax, double YMax);
   E1701_API long ScSCISetZField(double ZMin, double ZMax);
   E1701_API long ScSCIGetField(double *XMin, double *YMin, double *XMax, double *YMax);
   E1701_API long ScSCIGetZField(double *ZMin, double *ZMax);
   E1701_API long ScSCIGetGain(double *GainX, double *GainY);
   E1701_API long ScSCIGetZGain(double *GainZ);
   E1701_API long ScSCISetGain(double GainX, double GainY);
   E1701_API long ScSCISetZGain(double GainZ);
   E1701_API long ScSCIGetHomePosition(double *HomeX, double *HomeY);
   E1701_API long ScSCIGetZHomePosition(double *HomeZ);
   E1701_API long ScSCISetHomePosition(double HomeX, double HomeY);
   E1701_API long ScSCISetZHomePosition(double HomeZ);
   E1701_API long ScSCIGetOffset(double *OffsetX, double *OffsetY);
   E1701_API long ScSCIGetZOffset(double *OffsetZ);
   E1701_API long ScSCISetOffset(double OffsetX, double OffsetY);
   E1701_API long ScSCISetZOffset(double OffsetZ);
   E1701_API long ScSCISetWorkingArea(double XMin, double YMin, double XMax, double YMax);
   E1701_API long ScSCISetZWorkingArea(double ZMin, double ZMax);
   E1701_API long ScSCIGetWorkingArea(double *XMin, double *YMin, double *XMax, double *YMax);
   E1701_API long ScSCIGetZWorkingArea(double *ZMin, double *ZMax);
   E1701_API long ScSCIGetHomeJump(long *Value);
   E1701_API long ScSCISetHomeJump(long Value);
   E1701_API long ScSCIIsExposureEnd(long *Value);
   E1701_API long ScSCIMaxExternalTriggerCount(long Count);
   E1701_API long ScSCIResetExternalTriggerCount(void);
   E1701_API long ScSCISetMoveLaserState(long Value);
   E1701_API long ScSCIGetMoveLaserState(long *Value);
   E1701_API long ScSCISetDeviceEnableFlags(long FlagsId, long Flags);
   E1701_API long ScSCIGetDeviceEnableFlags(long FlagsId, long *Flags);
   E1701_API long ScSCISetDevicePath(long path_id,const char *path);
   E1701_API long ScSCIGetDevicePath(long PathId,char *path);
   E1701_API long ScSCISetDeviceMiscValue(long MiscId,long Value);
   E1701_API long ScSCIGetDeviceMiscValue(long MiscId,long *Value);
   E1701_API long ScSCISetDeviceMiscValueD(long MiscId,double Value);
   E1701_API long ScSCIGetDeviceMiscValueD(long MiscId,double *Value);
   E1701_API long ScSCIGetHeadCount(unsigned long *Count);
   E1701_API long ScSCISetHeadCount(unsigned long Count);
   E1701_API long ScSCIStreamInfo(long ident,long i_1,long i_2);
   E1701_API long ScSCIMarkLine(double x0, double y0, double x1, double y1);
   E1701_API long ScSCIMarkLine3D(double x0,double y0,double z0,double x1,double y1,double z1);
   E1701_API long ScSCIGetSpeed(double *Speed);
   E1701_API long ScSCISetSpeed(double Speed);
   E1701_API long ScSCIGetStyleSet(unsigned long *Style);
   E1701_API long ScSCISetStyleSet(unsigned long Style);
   E1701_API long ScSCISetLoopMode(long Mode);
   E1701_API long ScSCIGetLoopMode(long *Mode);
   E1701_API long ScSCISetLoop(long Count);
   E1701_API long ScSCIGetLoop(long *Count);
   E1701_API long ScSCISetMessageWindow(long hwnd);
   E1701_API long ScSCISetAxisState(long state);
   E1701_API long ScSCIGetAxisState(long *state);
   E1701_API long ScSCISaveSettings(const char *FileName);
   E1701_API long ScSCILoadSettings(const char *FileName);
   E1701_API long ScSCIEditSettings(void);
   E1701_API long ScSCISetRotAngle(double Angle); 
   E1701_API long ScSCIGetRotAngle(double *Angle);
   E1701_API long ScSCIUpdateDeviceStyle(void);
   E1701_API long ScSCIGetInterfaceVersion(unsigned long *Version); 
   E1701_API long ScSCIGetDebugMode(unsigned long *Flags);
   E1701_API long ScSCISetDebugMode(unsigned long Flags);
   E1701_API long ScSCIGetIdentString(char *Ident);
   E1701_API long ScSCIGetDeviceMapLaserPort(long *Port);
   E1701_API long ScSCISetDeviceMapLaserPort(long Port);
   E1701_API long ScSCIGetUSCInfoLong(unsigned long offset, long *value);
   // ***** end of USC1/2 compatibility functions ******************************************************************************
#endif // ENV_WINDOWS -- SCI interface is Windows-only

   // ***** EasyLase compatibility functions ***********************************************************************************
   // rename E1701inter.DLL to EasyLase.DLL for direct usage
   E1701_API int  EasyLaseGetCardNum(void);
   E1701_API bool EasyLaseWriteFrame(const int *card,const struct point_xy_rgbi *data,int size,unsigned short pps);
   E1701_API bool EasyLaseWriteFrameNR(const int *card,const struct point_xy_rgbi *data,int size,unsigned short pps,unsigned short rep);
   E1701_API int  EasyLaseGetLastError(const int *card);
   E1701_API int  EasyLaseGetStatus(const int *card);
   E1701_API bool EasyLaseStop(const int *card);
   E1701_API bool EasyLaseClose(void);
   E1701_API bool EasyLaseWriteDMX(const int *card,unsigned char *data);
   E1701_API bool EasyLaseGetDMX(const int *card,unsigned char *data);
   E1701_API bool EasyLaseDMXOut(const int *card,unsigned char *data,unsigned short addr,unsigned short chanCnt);
   E1701_API bool EasyLaseDMXIn(const int *card,unsigned char *data,unsigned short addr,unsigned short chanCnt);
   E1701_API bool EasyLaseWriteTTL(const int *card,unsigned short digiValue);
   E1701_API bool EasyLaseGetDebugInfo(const int *card,void * data,unsigned short count);
   E1701_API int  EasyLaseSend(const int *card,const struct point_xy_rgbi *data,int size,unsigned short kpps);
   E1701_API int  EasyLaseWriteFrameUncompressed(const int *card,const struct point_xy_rgbi *data,int size,unsigned short pps);
   E1701_API int  EasyLaseReConnect();
   // ***** end of EasyLase compatibility functions ****************************************************************************

   // ***** EtherDream compatibility functions *********************************************************************************
   // rename E1701inter.DLL to EtherDream.DLL for direct usage
   E1701_API int  EtherDreamGetCardNum(void);
   E1701_API void EtherDreamGetDeviceName(const int *card,char *buf,int max);
   E1701_API bool EtherDreamOpenDevice(const int *card);
   E1701_API bool EtherDreamWriteFrame(const int *card,const struct point_xy_rgbia *data,int size,unsigned short pps,unsigned short rep);
   E1701_API int  EtherDreamGetStatus(const int *card);
   E1701_API bool EtherDreamWriteDMX(const int *card,int uni,const unsigned char *data);
   E1701_API bool EtherDreamReadDMX(const int *card,int uni,unsigned char *data);
   E1701_API bool EtherDreamStop(const int *card);
   E1701_API bool EtherDreamCloseDevice(const int *card);
   E1701_API bool EtherDreamClose(void);
   // ***** end of EtherDream compatibility functions **************************************************************************

   // ***** EzAudDac compatibility functions ***********************************************************************************
   // rename E1701inter.DLL to EZAuDAC.DLL for direct usage
   E1701_API int  EzAudDacGetCardNum(void);
   E1701_API bool EzAudDacWriteFrame(const int *card,const struct point_xy_rgbia *data,int size,unsigned short pps);
   E1701_API bool EzAudDacWriteFrameNR(const int *card,const struct point_xy_rgbia *data,int size,unsigned short pps,unsigned short rep);
   E1701_API int  EzAudDacGetStatus(const int *card);
   E1701_API bool EzAudDacStop(const int *card);
   E1701_API bool EzAudDacClose(void);
   // ***** end of EzAudDac compatibility functions ****************************************************************************

   // ***** OLSC v2 API functions **********************************************************************************************
   // rename E1701inter.DLL to applications OLSC DLL name for direct usage
   E1701_API int OLSC_GetAPIVersion(void);
   E1701_API int OLSC_GetInterfaceName(char *pString);
   E1701_API int OLSC_Initialize(void);
   E1701_API int OLSC_Shutdown(void);
   E1701_API int OLSC_GetDeviceCount(void);
   E1701_API int OLSC_GetDeviceCapabilities(int device_number, struct LASER_SHOW_DEVICE_CAPABILITIES &device_capabilities);
   E1701_API int OLSC_GetLastErrorNumber(int device_number, int &number, char *string_pointer, int string_length);
   E1701_API int OLSC_SetConfigFile(char *pFullPathString);									// NEW IN V2
   E1701_API int OLSC_ShowConfigWindow(void);												// NEW IN V2
   E1701_API int OLSC_Play(int device_number);
   E1701_API int OLSC_Pause(int device_number);
   E1701_API int OLSC_Shutter(int device_number, int state);
   E1701_API int OLSC_WriteFrame(int device_number, struct LASER_SHOW_DEVICE_FRAME frame);
   E1701_API int OLSC_WritePoint(int device_number, struct LASER_SHOW_DEVICE_POINT point);	// NEW IN V2
   E1701_API int OLSC_WriteBufferedFrame(int device_number);									// NEW IN V2
   E1701_API int OLSC_GetStatus(int device_number, int &status);
   E1701_API int OLSC_SetCallback(int device_number, POLSCCALLBACK pCallback);				// NEW IN V2
   E1701_API int OLSC_WriteDMX(int device_number, int start_address, unsigned char *data_pointer, int length);
   E1701_API int OLSC_ReadDMX(int device_number, int start_address, unsigned char *data_pointer, int length);
   E1701_API int OLSC_WriteTTL(int device_number, int data);
   E1701_API int OLSC_ReadTTL(int device_number, int &data);
   // ***** end of OLSC v2 API functions ***************************************************************************************


   // ***** GCode access functions *********************************************************************************************
   E1701_API int E1701_GCODE_set_script(const char *script, int length);
   // ***** end of GCode access functions **************************************************************************************
};

#endif //E1701_INTER_H
