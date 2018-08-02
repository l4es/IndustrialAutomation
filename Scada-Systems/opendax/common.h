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
 
 
 *  This file contains the global macros, includes and defintions for 
 *  all the files in the entire system.
 */

/* Are we using config.h */
#ifdef HAVE_CONFIG_H
  #include <config.h>
#endif

/* including all this is just easier. */
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <assert.h>
#include <assert.h>
#ifdef HAVE_SYS_TYPES_H
 #include <sys/types.h>
#endif
#ifdef HAVE_STRING_H
 #include <string.h>
#endif
#ifdef HAVE_STRINGS_H
 #include <strings.h>
#endif
#ifdef HAVE_SYS_SELECT_H
 #include <sys/select.h>
 #ifndef FD_COPY
  #define FD_COPY(f, t) (void)(*(t) = *(f))
 #endif
#endif


/* Global definitions. These can be overriden with a -D arguments
 * in the Makefile */
#ifndef PID_FILE_PATH
  #define PID_FILE_PATH "/var/run"
#endif /* !PID_FILE_PATH */

#ifndef ETC_DIR
  #define ETC_DIR "/etc/opendax"
#endif

#define DAX_64_ONES 0xFFFFFFFFFFFFFFFFULL

/* All this silliness is because the different distributions have the libraries
 and header files for lua in different places with different names.
 There has got to be a better way. */
#if defined(HAVE_LUA5_1_LUA_H)
#  include <lua5.1/lua.h>
#elif defined(HAVE_LUA51_LUA_H)
#  include <lua51/lua.h>
#elif defined(HAVE_LUA_LUA_H)
#  include <lua/lua.h>
#elif defined(HAVE_LUA_H)
#  include <lua.h>
#else
#  error "lua.h Not Found"
#endif

#if defined(HAVE_LUA51_LAUXLIB_H)
#  include <lua51/lauxlib.h>
#elif defined(HAVE_LUA5_1_LAUXLIB_H)
#  include <lua5.1/lauxlib.h>
#elif defined(HAVE_LUA_LAUXLIB_H)
#  include <lua/lauxlib.h>
#elif defined(HAVE_LAUXLIB_H)
#  include <lauxlib.h>
#else
#  error "lauxlib.h Not Found"
#endif

#if defined(HAVE_LUA51_LUALIB_H)
#  include <lua51/lualib.h>
#elif defined(HAVE_LUA5_1_LUALIB_H)
#  include <lua5.1/lualib.h>
#elif defined(HAVE_LUA_LUALIB_H)
#  include <lua/lualib.h>
#elif defined(HAVE_LUALIB_H)
#  include <lualib.h>
#else
#  error "lualib.h Not Found"
#endif 


/* These are conditionally compiled debug statements. */
#ifdef DEBUG
# define DAX_DEBUG(x) dax_debug(5, x);
# define DAX_DEBUG2(x, y) dax_debug(5, x, y);
#else
# define DAX_DEBUG(x) 
# define DAX_DEBUG2(x, y) 
#endif

