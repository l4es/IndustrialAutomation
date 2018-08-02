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
 
 *  Source code file for opendax configuration
 */

#include <common.h>
#include <options.h>
#include <module.h>
#include <message.h>
#include <tagbase.h>
#include <func.h>

#include <getopt.h>

static char *_pidfile;
static char *_configfile;
static char *_statustag;
static char *_socketname;
static unsigned int _serverport;
static int _verbosity;
static int _daemonize;
static int _maxstartup;
static int _min_buffers;
static int _start_timeout;  /* module startup tier timeout */


/* Initialize the configuration to NULL or 0 for cleanliness */
static void initconfig(void) {
    int length;
    
    if(!_configfile) {
        length = strlen(ETC_DIR) + strlen("/opendax.conf") +1;
        _configfile = (char *)malloc(sizeof(char) * length);
        if(_configfile) 
            sprintf(_configfile, "%s%s", ETC_DIR, "/opendax.conf");
    }
    _daemonize = -1; /* We set it to negative so we can determine when it's been set */    
    _verbosity = 0;
    _statustag = NULL;
    _pidfile = NULL;
    _maxstartup = 0;
    _min_buffers = 0;
    _socketname = NULL;
    _serverport = 0;
    _start_timeout = 0;
}

/* This function sets the defaults if nothing else has been done 
   to the configuration parameters to this point */
static void
setdefaults(void)
{
    if(_daemonize < 0) _daemonize = DEFAULT_DAEMONIZE;
    if(!_statustag) _statustag = strdup("_status");
    if(!_pidfile) _pidfile = strdup(DEFAULT_PID);
    if(!_min_buffers) _min_buffers = DEFAULT_MIN_BUFFERS;
    if(!_socketname) _socketname = strdup("/tmp/opendax");
    if(!_serverport) _serverport = DEFAULT_PORT;
    if(!_start_timeout) _start_timeout = 3;
}

/* This function parses the command line options and sets
   the proper members of the configuration structure */
static void
parsecommandline(int argc, const char *argv[])
{
    char c;

    static struct option options[] = {
        {"config", required_argument, 0, 'C'},
        {"deamonize", no_argument, 0, 'D'},
        {"socketname", required_argument, 0, 'S'},
        {"serverport", required_argument, 0, 'P'},
        {"start_time", required_argument, 0, 'T'},
        {"version", no_argument, 0, 'V'},
        {"verbose", no_argument, 0, 'v'},
        {0, 0, 0, 0}
    };
      
/* Get the command line arguments */ 
    while ((c = getopt_long (argc, (char * const *)argv, "C:S:VvD",options, NULL)) != -1) {
        switch (c) {
        case 'C':
            _configfile = strdup(optarg);
            break;
        case 'S':
            _socketname = strdup(optarg);
            break;
//        case 'I':
//            if(! inet_aton(optarg, &_serverip)) {
//                xerror("Unknown IP address %s", optarg);
//            }
//            break;
        case 'P':
            _serverport = strtol(optarg, NULL, 0);
            break;
        case 'T':
            _start_timeout = strtol(optarg, NULL, 0);
            break;
        case 'V':
            printf("%s Version %s\n", PACKAGE, VERSION);
            break;
//      case 'v':
//            _verbosity++;
//          break;
        case 'D': 
            _daemonize = 1;
            break;
        case '?':
            printf("Got the big ?\n");
            break;
        case -1:
        case 0:
            break;
        default:
            printf ("?? getopt returned character code 0%o ??\n", c);
        } /* End Switch */
    } /* End While */           
}

/* This is the wrapper for the add module function in the Lua configuration file */
/* The arguments are a table that consist of all the configuration
   parameters of a module.  See the opendax.conf for examples */
static int
_add_module(lua_State *L)
{
    char *name, *path, *arglist;
    unsigned int flags = 0;
    int startup;

    if(!lua_istable(L, -1)) {
        luaL_error(L, "add_module() received an argument that is not a table");
    }

    lua_getfield(L, -1, "name");
    if( !(name = (char *)lua_tostring(L, -1)) ) {
        xerror("No module name given");
        return 0;
    }
    
    lua_getfield(L, -2, "path");
    path = (char *)lua_tostring(L, -1);
    
    lua_getfield(L, -3, "args");
    arglist = (char *)lua_tostring(L, -1);
    
    lua_getfield(L, -4, "startup");
    startup = (int)lua_tonumber(L, -1);
    if(startup > _maxstartup) _maxstartup = startup;
    
    lua_getfield(L, -5, "openpipes");
    if(lua_toboolean(L, -1)) {
        flags = MFLAG_OPENPIPES;
    }
    
    lua_getfield(L, -6, "restart");
    if(lua_toboolean(L, -1)) {
        flags |= MFLAG_RESTART;
    }
    
    lua_getfield(L, -7, "register");
    if(lua_toboolean(L, -1) ) {
        flags |= MFLAG_REGISTER;
    }

    module_add(name, path, arglist, startup, flags);
    
    return 0;
}

static int
readconfigfile(void)
{
    lua_State *L;
    char *string;
    
    xlog(2, "Reading Configuration file %s", _configfile);
    L = lua_open();
    /* We don't open any librarires because we don't really want any
     function calls in the configuration file.  It's just for
     setting a few globals. */
    
    lua_pushcfunction(L, _add_module);
    lua_setglobal(L, "add_module");
    
    luaopen_base(L);
    
    lua_pushstring(L, "opendax");
    lua_setglobal(L, CONFIG_GLOBALNAME);
    
    /* load and run the configuration file */
    if(luaL_loadfile(L, _configfile)  || lua_pcall(L, 0, 0, 0)) {
        xerror("Problem executing configuration file - %s", lua_tostring(L, -1));
        return 1;
    }
    
    /* tell lua to push these variables onto the stack */
    lua_getglobal(L, "daemonize");
    if(_daemonize < 0) { /* negative if not already set */
        _daemonize = lua_toboolean(L, -1);
    }
    lua_pop(L, 1);
    
    lua_getglobal(L, "statustag");
    /* Do I really need to do this???? */
    if(_statustag == NULL) {
        if( (string = (char *)lua_tostring(L, -1)) ) {
            _statustag = strdup(string);
        }
    }
    lua_pop(L, 1);
    
    lua_getglobal(L, "pidfile");
    if(_pidfile == NULL) {
        if( (string = (char *)lua_tostring(L, -1)) ) {
            _pidfile = strdup(string);
        }
    }
    lua_pop(L, 1);
    
    lua_getglobal(L, "min_buffers");
    if(_min_buffers == 0) { /* Make sure we didn't get anything on the commandline */
        _min_buffers = (int)lua_tonumber(L, -1);
    }
    lua_pop(L, 1);

    lua_getglobal(L, "start_timeout");
    if(_start_timeout == 0) {
        _start_timeout = (int)lua_tonumber(L, -1);
    }
    lua_pop(L, 1);

    /* TODO: This needs to be changed to handle the new topic handlers */
    if(_verbosity == 0) { /* Make sure we didn't get anything on the commandline */
        //_verbosity = (int)lua_tonumber(L, 4);
        //set_log_topic(_verbosity);
        set_log_topic(0xFFFFFFFF);    
    }
    
    /* Clean up and get out */
    lua_close(L);
    
    return 0;
}


/* This function should be called from main() to configure the program.
 * After the configurations have been initialized the command line is
 * parsed.  Then the configuration file is read and after that if any
 * parameter has not been set the defaults are used. */
int
opt_configure(int argc, const char *argv[])
{    
    initconfig();
    parsecommandline(argc, argv);
    if(readconfigfile()) {
        xerror("Unable to read configuration running with defaults");
    }
    setdefaults();
    
//    xlog(LOG_CONFIG, "Daemonize set to %d", _daemonize);
//    xlog(LOG_CONFIG, "Status Tagname is set to %s", _statustag);
//    xlog(LOG_CONFIG, "PID File Name set to %s", _pidfile);
    
    return 0;
}
 
int
opt_daemonize(void)
{
    return _daemonize;
}

char *
opt_statustag(void)
{
    return _statustag;
}

char *
opt_pidfile(void)
{
    return _pidfile;
}

int
opt_maxstartup(void)
{
    return _maxstartup;
}

char *
opt_socketname(void)
{
    return _socketname;
}

int
opt_min_buffers(void)
{
    return _min_buffers;
}

int
opt_start_timeout(void)
{
    return _start_timeout;
}
