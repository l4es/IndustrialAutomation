/* Modbus (tm) Communications Module for OpenDAX
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
 */

#ifndef __OPTIONS_H
#define __OPTIONS_H

#include <opendax.h>
#include <lib/modbus.h>


#ifndef DEFAULT_DEVICE
 #define DEFAULT_DEVICE "/dev/serial"
#endif

/* Beginning number of port to allocate */
#ifndef DEFAULT_PORTS
  #define DEFAULT_PORTS 16
#endif

#define SERIAL_PORT  1
#define NETWORK_PORT 2

struct Config {
    int portcount;   /* Number of ports that are assigned */
    int portsize;    /* Number of ports that are allocated */
    pthread_t *threads; /* The port threads */
    mb_port **ports; /* Pointer to an array of ports */
};

typedef struct cmd_temp_data {
    char *tagname;
    int index;
    u_int8_t function;
    u_int16_t length;
} cmd_temp_data;

/* This is a structure of information that we attach to the mb_port userdata
 * field that helps us keep track of information that our module needs */
typedef struct port_ud_item {
    char *mbreg;        /* OpenDAX Tagname */
    Handle h;           /* OpenDAX Handle */
    dax_event_id event; /* Event identification */
} port_ud_item;

#define HOLD_REG 0
#define INPUT_REG 1
#define COIL_REG 2
#define DISC_REG 3

typedef struct port_userdata {
    port_ud_item reg[4];
} port_userdata;


int modbus_configure(int, const char **);
int getbaudrate(int);

#endif
