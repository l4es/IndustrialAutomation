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
 *  Main source code file for the OpenDAX API regression test module
 */

/* The way the test modules works is by calling a Lua script (usually called
 * daxtest.lua) that in turn calls the run_test() function repeatedly for each
 * test that is desired.  These tests are represented by individual Lua scripts
 * for each test.  If these scripts raise an error the test is considered to
 * have failed.  Otherwise it is assumed that the test passes.  The C code in
 * this module is limited to what is needed to attach to the OpenDAX server and
 * handle all the messages.
 */
#include <daxtest.h>
#include <sys/ipc.h>
#include <sys/msg.h>

/* These are to keep score */
static int _tests_run = 0;
static int _tests_failed = 0;
dax_state *ds;

void
test_start(void) {
    _tests_run++;
}

void
test_fail(void) {
    _tests_failed++;
}

int
tests_run(void)
{
    return _tests_run;
}

int
tests_failed(void)
{
    return _tests_failed;
}


int
main(int argc,char *argv[])
{
    int result=0, flags;
    char *script;
    lua_State *L;
    
    ds = dax_init("daxlua");
    if(ds == NULL) {
        fprintf(stderr, "Unable to Allocate DaxState Object\n");
        return ERR_ALLOC;
    }
    
    dax_log(ds, "Starting module test");
    dax_set_debug_topic(ds, 0xFFFF); /* This should get them all out there */
        
    dax_init_config(ds, "daxtest");
    flags = CFG_CMDLINE | CFG_DAXCONF | CFG_ARG_REQUIRED;
    result += dax_add_attribute(ds, "exitonfail","exitonfail", 'x', flags, "0");
    result += dax_add_attribute(ds, "testscript","testscript", 't', flags, "daxtest.lua");
    
    dax_configure(ds, argc, argv, CFG_CMDLINE | CFG_DAXCONF | CFG_MODCONF);
    
    if(dax_connect(ds))
        dax_fatal(ds, "Unable to register with the server");
    
    script = dax_get_attr(ds, "testscript");

    /* Free the configuration memory once we are done with it */
    dax_free_config(ds);

    L = lua_open();
    /* This adds all of the Lua functions to the lua_State */
    add_test_functions(L);
    daxlua_set_state(L, ds);
    
    /* load and run the configuration file */
    if(luaL_loadfile(L, script)  || lua_pcall(L, 0, 0, 0)) {
        dax_error(ds, "Problem executing configuration file - %s", lua_tostring(L, -1));
        return ERR_GENERIC;
    }
    
    printf("OpenDAX Test Finished, %d tests run, %d tests failed\n", tests_run(), tests_failed());
    dax_disconnect(ds);
    
    return 0;
}
