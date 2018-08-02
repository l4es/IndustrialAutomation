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

 *  Header file for the Lua script interpreter
 */

#ifndef __DAXLUA_H
#define __DAXLUA_H

#include <opendax.h>
#include <common.h>
#include <../../lib/lua/libdaxlua.h>


/* This defines the starting number of scripts in the array */
#define NUM_SCRIPTS 8
#define DEFAULT_RATE 1000

/* These are the modes for the scripts globals. */
#define MODE_READ   0x01
#define MODE_WRITE  0x02
#define MODE_STATIC 0X04

/* This is the representation of a custom Lua global
   if the mode is static then the tagname will be written
   to the Lua registry otherwise it's either read, written
   or both to the OpenDAX server as a tag. */
typedef struct Global_t {
    char *name;
    unsigned char mode;
    Handle handle;
    int ref;
    struct Global_t *next;
} global_t;

/* Contains all the information to identify a script */
typedef struct Script_t {
    char enable;
    char trigger;
    pthread_t thread;
    pthread_cond_t condition;
    pthread_mutex_t mutex;
    char *name;
    char *filename;
    int func_ref;
    char *event_tagname;
    int event_count;
    int event_type;
    lua_Number event_value;
    unsigned char firstrun;
    long rate;
    long lastscan;
    long executions;
    global_t *globals;
} script_t;

/* options.c - Configuration functions */
int configure(int argc, char *argv[]);
char *get_init(void);
int get_scriptcount(void);
script_t *get_script(int index);
script_t *get_script_name(char *name);


int get_verbosity(void);

/* luaif.c - Lua Interface functions */
int daxlua_init(void);
int setup_interpreter(lua_State *L);
int fetch_tag(lua_State *L, Handle h);
int send_tag(lua_State *L, Handle h);
void tag_dax_to_lua(lua_State *L, Handle h, void* data);
int tag_lua_to_dax(lua_State *L, Handle h, void* data, void *mask);

#endif /* !__DAXLUA_H */
