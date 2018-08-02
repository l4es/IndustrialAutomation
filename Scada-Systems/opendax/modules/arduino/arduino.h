/*  OpenDAX - An open source data acquisition and control system 
 *  Copyright (c) 2010 Phil Birkelbach
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 *
 *  Main header file for the ArduinoIO module
 */

#include <opendax.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <common.h>
#include <sys/time.h>
#include <assert.h>

#ifndef __ARDUINO_H
#define __ARDUINO_H

#define MIN_PIN 2
#define MAX_PIN 54
#define MIN_AI  0
#define MAX_AI  11
#define MAX_NODES 254

/* Flags for the ar_pin.flags member */
#define PIN_FLAG_ENABLED 0x01
#define PIN_FLAG_CREATE  0x02 /* Create Tag */
#define PIN_FLAG_PULLUP  0x04 /* Enable Pullup Resistor */
#define PIN_FLAG_INVERT  0x08 /* The tag will be the compliment of the pin */
#define PIN_FLAG_HIT     0x10 /* For DO and PWM the change event has hit */

struct ar_pin {
    u_int8_t type;
    u_int8_t number;
    u_int8_t flags;
    char *tagname;
    int lastvalue;
    Handle handle;
    dax_event_id event;
    struct ar_pin *next;
};

/* Flags for the ar_analog.flags member */
#define ANG_FLAG_ENABLED 0x01
#define ANG_FLAG_CREATE  0x02 /* Create Tag */

struct ar_analog {
    u_int8_t number;
    u_int8_t reference;
    u_int8_t flags;
    char *tagname;
    Handle handle;
    struct ar_analog *next;
};

#define NODE_FLAG_INHIBITED 0x01

typedef struct ar_node {
    char *name;
    char *address;
    int reconnect;
    u_int8_t flags;
    struct timeval inhibited; /* The time the node was inhibited */
    int failures;
    struct ar_pin *pins;
    struct ar_analog *analogs;
} ar_node;

#define BIT_ISSET(word,bit) ((word) & (bit))
#define BIT_SET(word,bit) ((word) |= (bit))
#define BIT_RESET(word,bit) ((word) &= ~(bit))
#define BIT_TOGGLE(word,bit) ((word) ^= (bit))



#endif