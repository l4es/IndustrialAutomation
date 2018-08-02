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

 * This is the header file for the daxlua library.  The daxlua library contains
 * OpenDAX API bindings for the Lua programming language.
 */

#ifndef LIBDAXLUA_H
#define LIBDAXLUA_H 1

#include <opendax.h>
#include <common.h>

int luaopen_daxlib (lua_State *L);
int daxlua_register_function(lua_State *L, char *function_name);
int daxlua_set_state(lua_State *L, dax_state *new_ds);

#endif