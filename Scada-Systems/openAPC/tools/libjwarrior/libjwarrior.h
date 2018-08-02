/**
 * libjwarrior - library for easy data access of JoyWarrior accelerometers
 * Copyright (C) 2010-2011 OpenAPC Inc. openapc(at)gmail.com
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * This software is not designed or intended for use in on-line control of
 * aircraft, air traffic, aircraft navigation or aircraft communications; or in
 * the design, construction, operation or maintenance of any nuclear
 * facility. Licensee represents and warrants that it will not use or
 * redistribute the Software for such purposes.
 */


#ifndef LIBJWARRIOR_H
#define LIBJWARRIOR_H

#define LJW_MAX_G_1   1
#define LJW_MAX_G_1_5 2
#define LJW_MAX_G_2   3
#define LJW_MAX_G_3   4
#define LJW_MAX_G_4   5
#define LJW_MAX_G_8   6
#define LJW_MAX_G_16  7

#define LJW_SPEED_X 1
#define LJW_SPEED_Y 2
#define LJW_SPEED_Z 3

#ifdef __cplusplus
extern "C"
{
#endif

int ljw_open(const char *device,int maxG);
int ljw_get_data(double *accX,double *accY,double *accZ,double *spdX,double *spdY,double *spdZ);
int ljw_correct_speed(int speed,double value);
int ljw_close();

#ifdef __cplusplus
}
#endif

#endif
