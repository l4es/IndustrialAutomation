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

#include <string.h>
#include <assert.h>
#include <stdlib.h>

#include "libe1803.h"
#include "libe1803inter.h"
#include "liboapc.h"



unsigned char E1701_set_connection(const char *address)
{
    return E1803_set_connection(address);
}


unsigned char E1701_set_connection_(const char *address)
{
    return E1803_set_connection_(address);
}


void E1701_set_password(const unsigned char n, const char *ethPwd)
{
    E1803_set_password(n, ethPwd);
}


int E1701_set_filepath(unsigned char n, const char *fname, unsigned int mode)
{
    return E1803_set_filepath(n, fname, mode);
}


int E1701_set_debug_logfile(const unsigned char n, const char *path, const unsigned char flags)
{
    return E1803_set_debug_logfile(n, path, flags);
}


void E1701_close(unsigned char n)
{
    E1803_close(n);
}


int E1701_load_correction(unsigned char n, const char* filename, unsigned char tableNum)
{
    return E1803_load_correction(n, filename, tableNum);
}


int E1701_switch_correction(unsigned char n, unsigned char tableNum)
{
    return E1803_switch_correction(n, tableNum);
}


int E1701_set_xy_correction2(const unsigned char n, const double gainX, const double gainY, const double rot, const int offsetX, const int offsetY, const double slantX, const double slantY)
{
    return E1803_set_xy_correction(n, 0, gainX, gainY, rot, offsetX, offsetY, slantX, slantY);
}


int E1701_set_z_correction(const unsigned char n, const unsigned int h, const double xy_to_z_ratio, const int res)
{
    return E1803_set_z_correction(n, h, xy_to_z_ratio, res);
}


int E1701_tune(const unsigned char n, const unsigned int tuneFlags)
{
    return E1803_tune(n, tuneFlags);
}


int E1701_set_speeds(unsigned char n, double jumpspeed, double markspeed)
{
    return E1803_set_speeds(n, jumpspeed, markspeed);
}


int E1701_set_laser_delays(unsigned char n, double ondelay, double offdelay)
{
    return E1803_set_laser_delays(n, ondelay, offdelay);
}


int E1701_set_laser_mode(unsigned char n, unsigned int mode)
{
    return E1803_set_laser_mode(n, mode);
}


int E1701_set_laser(const unsigned char n, const unsigned int flags, const char on)
{
    return E1803_set_laser(n, flags, on);
}


int E1701_set_wobble(unsigned char n, unsigned int x, unsigned int y, double freq)
{
    return E1803_set_wobble(n, x, y, freq);
}


int E1701_set_scanner_delays(unsigned char n, double jumpdelay, double markdelay, double polydelay)
{
    return E1803_set_scanner_delays(n, 0, jumpdelay, markdelay, polydelay);
}


int E1701_jump_abs(unsigned char n, int x, int y, int z)
{
    return E1803_jump_abs(n, x, y, z);
}


int E1701_mark_abs(unsigned char n, int x, int y, int z)
{
    return E1803_mark_abs(n, x, y, z);
}


int E1701_set_pixelmode(const unsigned char n, const unsigned int mode, const double powerThres, const unsigned int res)
{
    return E1803_set_pixelmode(n, mode, powerThres, res);
}


int E1701_mark_pixelline(const unsigned char n, int x, int y, int z, const int pixWidth, const int pixHeight, const int pixDepth, unsigned int pixNum, const double *pixels, E1701_power_callback power_callback, void *userData)
{
    return E1803_mark_pixelline(n, x, y, z, pixWidth, pixHeight, pixDepth, pixNum, pixels, power_callback, userData);
}


int E1701_set_pos(unsigned char n, int x, int y, int z, unsigned char laserOn)
{
    return E1803_set_pos(n, x, y, z, laserOn);
}


int E1701_set_matrix(unsigned char n, double m11, double m12, double m21, double m22)
{
    return E1803_set_matrix(n, m11, m12, m21, m22);
}


int E1701_set_trigger_point(unsigned char n)
{
    return E1803_set_trigger_point(n);
}


int E1701_release_trigger_point(unsigned char n)
{
    return E1803_release_trigger_point(n);
}


int E1701_execute(unsigned char n)
{
    return E1803_execute(n);
}


int E1701_stop_execution(unsigned char n)
{
    return E1803_stop_execution(n);
}


int E1701_halt_execution(unsigned char n, unsigned char halt)
{
    return E1803_halt_execution(n, halt);
}


int E1701_delay(unsigned char n, double delay)
{
    return E1803_delay(n, delay);
}


int E1701_dynamic_data(unsigned char n, struct oapc_bin_struct_dyn_data *dynData)
{
    return E1803_dynamic_data(n, dynData);
}


unsigned int E1701_get_startstop_state(unsigned char n)
{
    return E1803_get_startstop_state(n);
}


unsigned int  E1701_get_card_state(unsigned char n)
{
    return E1803_get_card_state(n);
}


unsigned int  E1701_get_card_info(unsigned char n)
{
    return E1803_get_card_info(n);
}


int E1701_set_laser_timing(unsigned char n, double frequency, double pulse)
{
    return E1803_set_laser_timing(n, frequency, pulse);
}


int E1701_set_laserb(const unsigned char n, const double frequency, const double pulse)
{
    return E1701_ERROR_NOT_SUPPORTED;
}


int E1701_set_standby(unsigned char n, double frequency, double pulse)
{
    return E1803_set_standby(n, frequency, pulse);
}


int E1701_set_fpk(unsigned char n, double fpk, double yag3QTime)
{
    return E1803_set_fpk(n, fpk, yag3QTime);
}


int E1701_set_sky_params(unsigned char n, double angle, unsigned int fadeIn, unsigned int fadeOut)
{
    return E1701_ERROR_NOT_SUPPORTED;
}


int E1701_get_free_space(unsigned char n, int buffer)
{
    return E1803_get_free_space(n, buffer);
}


void E1701_get_version(unsigned char n, unsigned short *hwVersion, unsigned short *fwVersion)
{
    return E1803_get_version(n, hwVersion, fwVersion);
}


int E1701_get_library_version()
{
    return E1803_get_library_version();
}


int E1701_write(unsigned char n, unsigned int flags, unsigned int value)
{
    return E1803_write(n, flags, value);
}


int E1701_lp8_write2(unsigned char n, unsigned int flags, unsigned char value)
{
    return E1803_lp8_write(n, flags, value);
}


int E1701_lp8_write_latch(unsigned char n, unsigned char on, double delay1, unsigned char value, double delay2, double delay3)
{
    return E1803_lp8_write_latch(n, on, delay1, value, delay2, delay3);
}


int E1701_lp8_a0_2(unsigned char n, unsigned int flags, unsigned char value)
{
    return E1701_ERROR_NOT_SUPPORTED;
}


int E1701_lp8_write_mo(unsigned char n, unsigned char on)
{
    return E1803_lp8_write_mo(n, on);
}

int E1701_digi_write2(unsigned char n, unsigned int flags, unsigned int value, unsigned int mask)
{
    return E1803_digi_write(n, flags, value, mask);
}


unsigned int  E1701_digi_read(unsigned char n)
{
    return E1803_digi_read(n);
}


int E1701_digi_wait(unsigned char n, unsigned long value, unsigned long mask)
{
    return E1803_digi_wait(n, value, mask);
}


int E1701_digi_set_motf(unsigned char n, double motfX, double motfY)
{
    return E1803_digi_set_motf(n, motfX, motfY);
}


int E1701_digi_set_motf_sim(unsigned char n, double motfX, double motfY)
{
    return E1803_digi_set_motf_sim(n, motfX, motfY);
}


int E1701_digi_wait_motf(const unsigned char n, const unsigned int flags, const double dist)
{
    return E1803_digi_wait_motf(n, flags, dist);
}


int E1701_digi_set_mip_output(unsigned char n, unsigned int value, unsigned int flags)
{
    return E1803_digi_set_mip_output(n, value, flags);
}


int E1701_digi_set_wet_output(const unsigned char n, const unsigned int value, const unsigned int flags)
{
    return E1803_digi_set_wet_output(n, value, flags);
}


int E1701_ana_a123(const unsigned char n, const unsigned short r, const unsigned short g, const unsigned short b)
{
    return E1701_ERROR_NOT_SUPPORTED;
}


unsigned int  E1701_send_data(unsigned char n, unsigned int flags, const char *sendData, unsigned int length, unsigned int *sentLength)
{
    E1803_send_data(n, flags, sendData, length, sentLength);
    return *sentLength;
}


unsigned int  E1701_recv_data(unsigned char n, unsigned int flags, char *recvData, unsigned int maxLength)
{
    E1803_recv_data(n, flags, recvData, maxLength);
    return *recvData;
}