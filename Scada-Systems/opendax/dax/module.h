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
 */


#ifndef __MODULE_H
#define __MODULE_H

#include <common.h>
#include <dax/daxtypes.h>

#define MSTATE_STARTED      0x01 /* Module has been started */
#define MSTATE_WAITING      0x02 /* Waiting for restart */
#define MSTATE_CHILD        0x04 /* Module was started by this program */
#define MSTATE_REGISTERED   0x08 /* Is the module registered */
#define MSTATE_RUNNING      0x10 /* Module is running */

typedef struct {
    pid_t pid;
    int status;
} dead_module;

/* Module List Handling Functions */
dax_module *module_add(char *name, char *path, char *arglist, int startup, unsigned int flags);
int module_del(dax_module *mod);

/* This function is to initialize module stuff that needs to happen
 * before the communications is set up. */
void initialize_module(void);

/* Module runtime functions */
void module_start_all(void);
int module_set_running(int fd);
pid_t module_start (dax_module *mod);
int module_stop(dax_module *mod);
dax_module *module_register(char *name, pid_t pid, int fd);
dax_module *event_register(pid_t pid, int fd);
void module_unregister(pid_t pid);
dax_module *module_find_fd(int fd);


#ifdef DEBUG
void print_modules(void);
#endif

/* module maintanance */
void module_scan(void);

void module_dmq_add(pid_t, int);

#endif /* !__MODULE_H */
