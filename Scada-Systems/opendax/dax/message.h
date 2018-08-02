/*  OpenDAX - An open source data acquisition and control system
 *  Copyright (c) 2007 Phil Birkelbach
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

 *  Header file for the messaging code
 */
 
#ifndef __MESSAGE_H
#define __MESSAGE_H

#include <common.h>
#include <dax/daxtypes.h>
#include <dax/libcommon.h>
#include <opendax.h>

/* message.c functions */
int msg_setup(void);
void msg_destroy(void);
int msg_receive(void);
void msg_add_fd(int);
void msg_del_fd(int);
int msg_dispatcher(int, unsigned char *);


/* buffer.c functions */
int buff_initialize(void);
int buff_read(int fd);
void buff_wipe(void);
void buff_free(int);
void buff_freeall(void);


#endif /* !__MESSAGE_H */
