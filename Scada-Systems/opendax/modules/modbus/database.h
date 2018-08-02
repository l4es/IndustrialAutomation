/* database.h - Modbus (tm) Communication Module for OpenDAX
 * Copyright (C) 2006 Phil Birkelbach
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 * 
 * Source file that contains the tag database handline funcitons.
 */

#ifndef __DATABASE_H
#define __DATABASE_H

#include <common.h>
#include <opendax.h>
#include <modopt.h>
#include <lib/modbus.h>

void setup_command(struct mb_cmd *c, void *userdata, u_int8_t *data, int datasize);
int init_database(void);

#endif
