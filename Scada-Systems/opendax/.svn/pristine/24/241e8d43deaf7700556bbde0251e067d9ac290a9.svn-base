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
 *  This file contains the wrapper functions and lua definitions for the
 *  opendax daxtest module.
 */

#include <daxtest.h>

extern dax_state *ds;

/* This function runs a test script.  If the script raises an 
 * error this function increments the fail count.  It returns
 * nothing */
static int
_run_test(lua_State *L)
{
    char *script, *desc;
    
    if(lua_gettop(L) != 2) {
        luaL_error(L, "Wrong number of arguments to run_test()");
    }
    
    script = (char *)lua_tostring(L, -2);
    desc = (char *)lua_tostring(L, -1);
    test_start();
    printf("Starting - %s\n", desc);
        
    /* load and run the test script */
    if(luaL_loadfile(L, script)) {
        /* Here the error is allowed to propagate up and kill the whole thing */
        dax_error(ds, "Problem loading script - %s", lua_tostring(L, -1));
    }
    if(lua_pcall(L, 0, 0, 0)) {
        test_fail();
        printf("%s\n", lua_tostring(L, -1));
        printf("FAILED - %s\n", desc);
    } else {
        printf("PASSED - %s\n", desc);
    }
    return 0;
}


/* Adds a certain number of randomly generated tags that start
 * with the given name.  
 * Lua Call : add_random_tags(int count, string name) */
static int
_add_random_tags(lua_State *L)
{
    int count;
    char *name;
    
    if(lua_gettop(L) != 2) {
        luaL_error(L, "wrong number of arguments to add_random_tags()");
    }
    
    if(! lua_isnumber(L, 1)) {
        luaL_error(L, "argument to add_random_tags() is not a number");
    }
    count = lua_tointeger(L, 1);
    name = (char *)lua_tostring(L, 2);
    if(add_random_tags(count, name)) {
        luaL_error(L, "Failed add_random_tags(%d, %s)\n", count, name);
    }
    return 0;
}

/* This test is used to determine if the dax_tag_handle() will return
 * the correct information in the tag handle.  The function takes 8
 * arguments.  See the handles.lua script for details. */
static int
_handle_test(lua_State *L)
{
    Handle h;
    int final = 0;
    const char *name, *type;
    int count, result, byte, bit, rcount, size, test;
    
    if(lua_gettop(L) != 8) {
        luaL_error(L, "wrong number of arguments to handle_test()");
    }
    name = lua_tostring(L, 1);
    count = lua_tointeger(L, 2);
    byte = lua_tointeger(L, 3);
    bit = lua_tointeger(L, 4);
    rcount = lua_tointeger(L, 5);
    size = lua_tointeger(L, 6);
    type = lua_tostring(L, 7);
    test = lua_tointeger(L, 8);
    
    result = dax_tag_handle(ds, &h, (char *)name, count);
    if(test == FAIL && result == 0) { /* We should fail */
        printf("Handle Test: %s : %d Should have Failed\n", name, count);
        final = 1;
    } else if(test == PASS) {
        if(result) {
            printf("Handle Test: dax_tag_handle failed with code %d\n", result);
            final = 1;
        } else { 
            if(h.byte != byte) {
                printf("Handle Test: %s : %d - Byte offset does not match (%d != %d)\n", name, count, h.byte, byte);
                final = 1;
            }
            if(h.bit != bit) {
                printf("Handle Test: %s : %d - Bit offset does not match (%d != %d)\n", name, count, h.bit, bit);
                final = 1;
            }
            if(h.count != rcount) {
                printf("Handle Test: %s : %d - Item count does not match (%d != %d)\n", name, count, h.count, rcount);
                final = 1;
            }
            if(h.size != size) {
                printf("Handle Test: %s : %d - Size does not match (%d != %d)\n", name, count, h.size, size);
                final = 1;
            }
            if(h.type != dax_string_to_type(ds, (char *)type)) {
                printf("Handle Test: %s : %d - Datatype does not match\n", name, count);
                final = 1;
            }
        }
    }
    lua_pop(L, 8);        

    if(final) {
        luaL_error(L, "Handle Test Failed");
    }
    return 0;
}

/*** LAZY PROGRAMMER TESTS *****************************************
 * This is a temporary place for development of tests.  It puts
 * these tests within the normal testing framework but allows
 * a developer a way to test new library or server features without
 * having to create a formal test.
 *******************************************************************/
static void
_event_callback(void *data) {
    fprintf(stderr, "Got Callback: data = %d\n", *(int *)data);
}

static int
_lazy_test(lua_State *L)
{
    int result, n;
    static dax_dint test[10], data;
    
    Handle h;
    dax_event_id id[10];
    

    for(n = 0; n < 10; n++) {
        test[n] = n;
    }
    data = 25;
    result = dax_tag_add(ds, NULL, "LazyTag", DAX_INT, 10);
    result = dax_tag_handle(ds, &h, "LazyTag[5]", 3);
    result = dax_event_add(ds, &h, EVENT_EQUAL, &data, &id[0], _event_callback, &test[0], NULL);
    dax_write_tag(ds, h, &test[5]);
    dax_event_poll(ds, NULL);
    test[5] = 25;
    dax_write_tag(ds, h, &test[5]);
    dax_event_poll(ds, NULL);
    dax_write_tag(ds, h, &test[5]);
    dax_event_poll(ds, NULL);
    result = dax_tag_handle(ds, &h, "LazyTag[6]", 1);
    dax_write_tag(ds, h, &test[5]);
    dax_event_poll(ds, NULL);
    return 0;
}

/**END OF LAZY PROGRAMMER TESTS**************************************/

/* Adds the functions to the Lua State */
void
add_test_functions(lua_State *L)
{
    /* This adds the functions that we need from the daxlua library */
    daxlua_register_function(L,"cdt_create");
    daxlua_register_function(L,"tag_add");
    daxlua_register_function(L,"tag_get");
    daxlua_register_function(L,"tag_read");
    daxlua_register_function(L,"tag_write");
    daxlua_register_function(L,"event_add");
    daxlua_register_function(L,"event_del");
    daxlua_register_function(L,"event_select");
    daxlua_register_function(L,"event_poll");


    /* These are the functions that only make sense in this module */
    lua_pushcfunction(L, _run_test);
    lua_setglobal(L, "run_test");

    lua_pushcfunction(L, _add_random_tags);
    lua_setglobal(L, "add_random_tags");

    lua_pushcfunction(L, _handle_test);
    lua_setglobal(L, "handle_test");
    
    lua_pushcfunction(L, _lazy_test);
    lua_setglobal(L, "lazy_test");

    /* These are the standard Lua libraries that we need */
    luaopen_base(L);
    luaopen_table(L);
    luaopen_string(L);
    luaopen_math(L);
}
