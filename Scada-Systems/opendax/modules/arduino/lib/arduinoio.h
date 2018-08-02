/*  ArduinoIO - A library for using the Arduino as a general purpose
 *  I/O module.
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
 *  Main header file for the Arduino IO
 */

#ifndef __ARDUINOIO_H
#define __ARDUINOI_H

#define ARIO_ERR_UNIMPLEMENTED -1
#define ARIO_ERR_MALFORMED     -2
#define ARIO_ERR_ARGUMENT      -3
#define ARIO_ERR_INVALID_PIN   -4
#define ARIO_ERR_TIMEOUT       -5
#define ARIO_ERR_WRITE         -6
#define ARIO_ERR_READ          -7
#define ARIO_ERR_ADDRESS       -8;

#define ARIO_PIN_DI  1
#define ARIO_PIN_DO  2
#define ARIO_PIN_PWM 3

#define ARIO_REF_DEFAULT  1
#define ARIO_REF_INTERNAL 2
#define ARIO_REF_EXTERNAL 3

typedef struct ar_network ar_network;

ar_network *ario_init(void);
void ario_free(ar_network *an);
int ario_openport(ar_network *an, char *device, unsigned int baudrate);
int ario_closeport(ar_network *an);
int ario_set_retries(ar_network *an, int retries);
int ario_set_timeout(ar_network *an, int timeout);
int ario_pin_mode(ar_network *an, char *address, int pin, int mode);
int ario_pin_pullup(ar_network *an, char *address, int pin, int pullup);
int ario_ai_enable(ar_network *an, char *address, int ai);
int ario_ai_ref(ar_network *an, char *address, int ai, int ref);

int ario_pin_read(ar_network *an, char *address, int pin);
int ario_pin_write(ar_network *an, char *address, int pin, int val);
int ario_pin_pwm(ar_network *an, char *address, int pin, int val);
int ario_ai_read(ar_network *an, char *address, int ai);



#endif