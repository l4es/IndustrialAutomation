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
 *  Main source code file for the OpenDAX Arduino I/O module
 */

#include <arduino.h>
#include <lib/arduinoio.h>

void quit_signal(int sig);
static void getout(int exitstatus);

dax_state *ds; /* DaxState Object */
static ar_network *_an; /* Arduino IO Network Object */
static int _quitsignal;
/* Configuration */
static char *_device;
static unsigned int _baudrate;
static int _retries;
static int _timeout;    /* Message Timeout (mSec) */
static int _rate;       /* Polling Rate (mSec) */
static int _maxfail;    /* Maximum number of failures of a node before shutdown */
static int _reconnect;  /* Time (sec) before a reconnect is tried on a failed node */
static int _connected;  /* Port is connected */

static ar_node _nodes[MAX_NODES];
static int _node_count;

static void
_add_pin(u_int8_t number, u_int8_t type, u_int8_t flags, char *tagname)
{
    struct ar_pin *new_pin, *this;
    if(tagname == NULL) {
        dax_debug(ds, LOG_ERROR, "No tagname given for pin %d", number);
        return;
    }
    if(type == 0) {
        dax_debug(ds, LOG_ERROR, "Unknown type given for pin %d", number);
        return;
    }
    new_pin = malloc(sizeof(struct ar_pin));
    if(new_pin == NULL) {
        dax_fatal(ds, "Unable to allocate memory");
    }
    new_pin->type = type;
    new_pin->number = number;
    new_pin->flags = flags;
    new_pin->tagname = strdup(tagname);
    new_pin->next = NULL;
    
    this = _nodes[_node_count].pins;
    if(this == NULL) {
        _nodes[_node_count].pins = new_pin;
    } else {
        while(this != NULL) {
            if(this->next == NULL) {
                this->next = new_pin;
                return;
            }
            this = this->next;
        }
    }
}

static void
_add_analog(u_int8_t number, u_int8_t reference, char *tagname)
{
    struct ar_analog *new_analog, *this;
    if(tagname == NULL) {
        dax_debug(ds, LOG_ERROR, "No tagname given for analog %d", number);
        return;
    }
    if(reference == 0) {
        dax_debug(ds, LOG_ERROR, "Unknown reference given for analog %d", number);
        return;
    }
    new_analog = malloc(sizeof(struct ar_analog));
    if(new_analog == NULL) {
        dax_fatal(ds, "Unable to allocate memory");
    }
    new_analog->reference = reference;
    new_analog->number = number;
    new_analog->tagname = strdup(tagname);
    new_analog->next = NULL;
    
    this = _nodes[_node_count].analogs;
    if(this == NULL) {
        _nodes[_node_count].analogs = new_analog;
    } else {
        while(this != NULL) {
            if(this->next == NULL) {
                this->next = new_analog;
                return;
            }
            this = this->next;
        }
    }

}

/* Four arguments are passed to this function.
 * name - string for the name of the node
 * address - 2 character string that represents the address of the node
 * pins - a table array of tables that define the pins
 * analogs - a table array of tables that define the analog points
 */
static int
_add_node(lua_State *L)
{
    u_int8_t type, flags, n;
    const char *str;
    
    if(_node_count >= MAX_NODES) {
        luaL_error(L, "Maximum number of nodes has been exceeded");
    }
    
    /* Check the arguments */
    if(lua_gettop(L) != 4) {
        luaL_error(L, "Wrong number of arguments passed to add_node()");
    }
    if(!lua_istable(L, 3)) {
        luaL_error(L, "add_node() argument 3 must be a table");
    }
    if(!lua_istable(L, 4)) {
        luaL_error(L, "add_node() argument 4 must be a table");
    }
    
    str = lua_tostring(L, 1);
    _nodes[_node_count].name = strdup(str);
    
    str = lua_tostring(L, 2);
    _nodes[_node_count].address = strdup(str);

    for(n = MIN_PIN; n < MAX_PIN; n++) {
        flags = 0;  /* Reset the Flags */
        lua_rawgeti(L, 3, n);
        if(lua_istable(L, -1)) {
            lua_pushstring(L, "type");
            lua_rawget(L, -2);
            str = lua_tostring(L, -1);
            type = ARIO_PIN_DI;
            if(str != NULL) {
                if(strcasecmp("DI", str)==0) {
                    type = ARIO_PIN_DI;
                } else if(strcasecmp("DO", str)==0) {
                    type = ARIO_PIN_DO;
                } else if(strcasecmp("PWM", str)==0) {
                    type = ARIO_PIN_PWM;
                }
            }
            lua_pop(L, 1);
            lua_pushstring(L, "pullup");
            lua_rawget(L, -2);
            if(lua_toboolean(L, -1)) BIT_SET(flags, PIN_FLAG_PULLUP);
            lua_pop(L, 1);
            lua_pushstring(L, "create");
            lua_rawget(L, -2);
            if(lua_toboolean(L, -1)) BIT_SET(flags, PIN_FLAG_CREATE);
            lua_pop(L, 1);
            lua_pushstring(L, "invert");
            lua_rawget(L, -2);
            if(lua_toboolean(L, -1)) BIT_SET(flags, PIN_FLAG_INVERT);
            lua_pop(L, 1);
            
            lua_pushstring(L, "tagname");
            lua_rawget(L, -2);
            _add_pin(n, type, flags, (char *)lua_tostring(L, -1));
            lua_pop(L, 1);
        }
        lua_pop(L, 1);
    }
    for(n = MIN_AI; n < MAX_AI; n++) {
        lua_rawgeti(L, 4, n);
        if(lua_istable(L, -1)) {
            lua_pushstring(L, "reference");
            lua_rawget(L, -2);
            str = lua_tostring(L, -1);
            type = ARIO_REF_DEFAULT;
            if(str != NULL) {
                if(strcasecmp("DEFAULT", str)==0) {
                    type = ARIO_REF_DEFAULT;
                } else if(strcasecmp("INTERNAL", str)==0) {
                    type = ARIO_REF_INTERNAL;
                } else if(strcasecmp("EXTERNAL", str)==0) {
                    type = ARIO_REF_EXTERNAL;
                    fprintf(stderr, "Pine %d set as external\n", n);
                }
            }
            lua_pop(L, 1);
            lua_pushstring(L, "tagname");
            lua_rawget(L, -2);
            _add_analog(n, type, (char *)lua_tostring(L, -1));
            lua_pop(L, 1);
        }
        lua_pop(L, 1);
    }
    _node_count++;

    //lua_getfield(L, -1, "name");
    //name = (char *)lua_tostring(L, -1);
    
    //lua_pop(L, 1);
    return 0;
}

static int
_run_config(int argc, char *argv[])
{
    int flags, result;
    /* Create and initialize the configuration subsystem in the library */
    dax_init_config(ds, "arduino");
    flags = CFG_CMDLINE | CFG_MODCONF | CFG_ARG_REQUIRED;
    result = 0;
    result += dax_add_attribute(ds, "device","device", 'd', flags, "/dev/ttyS0");
    result += dax_add_attribute(ds, "baudrate","baudrate", 'b', flags, "9600");
    result += dax_add_attribute(ds, "retries","retries", 'r', flags, "3");
    result += dax_add_attribute(ds, "timeout","timeout", 't', flags, "500");
    result += dax_add_attribute(ds, "poll_rate","poll_rate", 'p', flags, "1000");
    result += dax_add_attribute(ds, "max_fail","max_fail", 'f', flags, "20");
    result += dax_add_attribute(ds, "reconnect","reconnect", 'n', flags, "120");
    if(result > 0) {
        fprintf(stderr, "Problem adding attributes - %d\n", result);
    }
    dax_set_luafunction(ds, (void *)_add_node, "add_node");
    /* Execute the configuration */
    dax_configure(ds, argc, argv, CFG_CMDLINE | CFG_DAXCONF | CFG_MODCONF);

    /* Get the results of the configuration */
    _device = strdup(dax_get_attr(ds, "device"));
    _baudrate = strtol(dax_get_attr(ds, "baudrate"), NULL, 0);
    _retries = strtol(dax_get_attr(ds, "retries"), NULL, 0);
    _timeout = strtol(dax_get_attr(ds, "timeout"), NULL, 0);
    _rate = strtol(dax_get_attr(ds, "poll_rate"), NULL, 0);
    _maxfail = strtol(dax_get_attr(ds, "max_fail"), NULL, 0);
    _reconnect = strtol(dax_get_attr(ds, "reconnect"), NULL, 0);
    /* Free the configuration data */
    dax_free_config (ds);
    return 0;
}

static void
_print_config(void)
{
    int n;
    struct ar_pin *pin;
    struct ar_analog *analog;
    fprintf(stderr, "device = %s\n", _device);
    fprintf(stderr, "baudrate = %d\n", _baudrate);
    fprintf(stderr, "retries = %d\n", _retries);
    fprintf(stderr, "timeout = %d\n", _timeout);
    fprintf(stderr, "poll_rate = %d\n", _rate);
    fprintf(stderr, "max_fail = %d\n", _maxfail);
    fprintf(stderr, "reconnect = %d\n", _reconnect);
      
    for(n = 0; n < _node_count; n++) {
        fprintf(stderr, "Node[%d] name = %s : address = %s\n", n, _nodes[n].name, _nodes[n].address);
        pin = _nodes[n].pins;
        while(pin != NULL) {
            fprintf(stderr, " Pin[%d] type = %d : tagname = %s ", pin->number, pin->type, pin->tagname);
            fprintf(stderr, ": pullup = %d ", BIT_ISSET(pin->flags, PIN_FLAG_PULLUP) ? 1 : 0);
            fprintf(stderr, ": create = %d ", BIT_ISSET(pin->flags, PIN_FLAG_CREATE) ? 1 : 0);
            fprintf(stderr, ": invert = %d\n", BIT_ISSET(pin->flags, PIN_FLAG_INVERT) ? 1 : 0);
            pin = pin->next;
        }
        analog = _nodes[n].analogs;
        while(analog != NULL) {
            fprintf(stderr, " Analog[%d] tagname = %s : reference = %d\n", analog->number, analog->tagname, analog->reference);
            analog = analog->next;
        }
    }
}

/* This is the callback that is assigned to a change event for
 * the tag associated with the pin.  It simply sets the HIT flag */
static void
_pin_event_callback(void *udata)
{
    struct ar_pin *pin;
    pin = (struct ar_pin *)udata;
    BIT_SET(pin->flags, PIN_FLAG_HIT);
}

inline static int
_inhibit_node(int node)
{
    struct ar_pin *pthis;
    struct ar_analog *athis;
    
    /* Sets the time that this node was inhibited */
    gettimeofday(&_nodes[node].inhibited, NULL);
    pthis = _nodes[node].pins;
    while(pthis != NULL) {
        BIT_RESET(pthis->flags, PIN_FLAG_ENABLED);
        if(pthis->type == ARIO_PIN_DO || pthis->type == ARIO_PIN_PWM) {
            dax_event_del(ds, pthis->event);
        }
        pthis = pthis->next;
    }
    athis = _nodes[node].analogs;
    while(athis != NULL) {
        BIT_RESET(athis->flags, PIN_FLAG_ENABLED);
        athis = athis->next;
    }

    return 0;
}

inline static int
_init_pin(int node, struct ar_pin *pin)
{
    int type, result, temp;

    dax_debug(ds, LOG_MINOR, "Initializing Pin:%d Tagname = %s", pin->number, pin->tagname);
    result = ario_pin_mode(_an, _nodes[node].address, pin->number, pin->type);
    if(result) {
        dax_debug(ds, LOG_ERROR, "Problem Setting Mode return result; for Node:%s Pin:%d Result = %d",
                 _nodes[node].address, pin->number, result);
        return result;
    }
    if(pin->type == ARIO_PIN_DI) {
        if(BIT_ISSET(pin->flags, PIN_FLAG_PULLUP)) {
            temp = 1;
        } else {
            temp = 0;
        }
        result = ario_pin_pullup(_an, _nodes[node].address, pin->number, temp);
    }
    if(result) {
        dax_debug(ds, LOG_ERROR, "Problem Setting Pullup for Node:%s Pin:%d Result = %d",
                 _nodes[node].address, pin->number, result);
        
        return result;
    }
    
    if(BIT_ISSET(pin->flags, PIN_FLAG_CREATE)) {
        if(pin->type == ARIO_PIN_PWM) {
            type = DAX_SINT;
        } else {
            type = DAX_BOOL;
        }
        /* TODO: Deal with making arrays here */
        dax_tag_add(ds, NULL, pin->tagname, type, 1);
    }
    result = dax_tag_handle(ds, &pin->handle, pin->tagname, 0);
    if(result) return result;
    /* If it's an output type pin we need an event */
    if(pin->type == ARIO_PIN_DO || pin->type == ARIO_PIN_PWM) {
        result = dax_event_add(ds, &pin->handle, EVENT_CHANGE, NULL, 
                               &pin->event, _pin_event_callback,
                               (void *)pin, NULL);
        if(result) return result;
    }
    BIT_SET(pin->flags, PIN_FLAG_ENABLED);
    BIT_SET(pin->flags, PIN_FLAG_HIT); /* This will force a write */
    return 0;
}

inline static int
_init_analog(int node, struct ar_analog *analog)
{
    return -1;
}

inline static int
_init_node(int node)
{
    struct ar_pin *pthis;
    struct ar_analog *athis;
    int good_pin_count = 0;
    int good_analog_count = 0;
    int result;

    dax_debug(ds, LOG_MINOR, "Initializing Node:%d", node);

    _nodes[node].failures = 0;
    pthis = _nodes[node].pins;
    while(pthis != NULL) {
        result = _init_pin(node, pthis);
        if(result == 0) good_pin_count++;
        pthis = pthis->next;
    }
    athis = _nodes[node].analogs;
    while(athis != NULL) {
        result = _init_analog(node, athis);
        if(result == 0) good_analog_count++;
        athis = athis->next;
    }
    if(good_pin_count > 0 || good_analog_count > 0) {
        return 0;
    } else {
        return -1;
    }
}

inline static int
_exec_node(int node)
{
    struct ar_pin *pthis;
    struct ar_analog *athis;
    int value, temp, result;

    pthis = _nodes[node].pins;
    while(pthis != NULL) {
        if(BIT_ISSET(pthis->flags, PIN_FLAG_ENABLED)) {
            if(pthis->type == ARIO_PIN_DI) {
                value = ario_pin_read(_an, _nodes[node].address, pthis->number);
                if( value < 0) {
                    _nodes[node].failures++;
                    dax_debug(ds, LOG_ERROR, "Unable to write to Node:%s Pin:%d Result=%d",
                              _nodes[node].address, pthis->number, value);
                } else {
                    if(value != pthis->lastvalue) {
                        temp = 0;
                        if(value) temp = 0x01;
                        if(BIT_ISSET(pthis->flags, PIN_FLAG_INVERT)) {
                            BIT_TOGGLE(temp, 0x01);
                        }
                        dax_write_tag(ds, pthis->handle, &temp);
                        pthis->lastvalue = value;
                    }
                }
            } else if(pthis->type == ARIO_PIN_DO) {
                if(BIT_ISSET(pthis->flags, PIN_FLAG_HIT)) {
                    BIT_RESET(pthis->flags, PIN_FLAG_HIT);
                    result = dax_read_tag(ds, pthis->handle, &temp);
                    if(result == 0) {
                        if(BIT_ISSET(pthis->flags, PIN_FLAG_INVERT)) {
                            BIT_TOGGLE(temp, 0x01);
                        }
                        result = ario_pin_write(_an, _nodes[node].address, pthis->number, temp);
                        if(result < 0) {
                            _nodes[node].failures++;
                            dax_debug(ds, LOG_ERROR, "Unable to write to Node:%s Pin:%d Result = %d",
                                      _nodes[node].address, pthis->number, result);
                            
                        }
                    }
                }
            } else if(pthis->type == ARIO_PIN_PWM) {
                ;//ario_pin_pwm(ar_network *an, char *address, int pin, int val);
            } else {
                assert(0); /* One of the above must be true */
            }
        }
        if(_nodes[node].failures > _maxfail) {
            _inhibit_node(node);
            return -1;
        }
        pthis = pthis->next;
    }
    athis = _nodes[node].analogs;
    while(athis != NULL) {
        BIT_RESET(athis->flags, PIN_FLAG_ENABLED);

        if(_nodes[node].failures > _maxfail) {
            _inhibit_node(node);
            return -1;
        }
        athis = athis->next;
    }
    return 0;
}

/* main inits and then calls run */
int main(int argc,char *argv[])
{
    struct sigaction sa;
    int result, node, good_nodes, running = 0;
    struct timeval start, end, now;
    long time_spent;
    /* Set up the signal handlers for controlled exit*/
    memset (&sa, 0, sizeof(struct sigaction));
    sa.sa_handler = &quit_signal;
    sigaction (SIGQUIT, &sa, NULL);
    sigaction (SIGINT, &sa, NULL);
    sigaction (SIGTERM, &sa, NULL);

    /* Create and Initialize the OpenDAX library state object */
    ds = dax_init("arduino");
    if(ds == NULL) {
        /* dax_fatal() logs an error and causes a quit 
         * signal to be sent to the module */
        dax_fatal(ds, "Unable to Allocate DaxState Object\n");
    }

    /* Set the logging flags to show all the messages */
    dax_set_debug_topic(ds, LOG_ALL);

    _run_config(argc, argv);

    /* Check for OpenDAX and register the module */
    if( dax_connect(ds) ) {
        dax_fatal(ds, "Unable to find OpenDAX");
    }
    
    _print_config();
    
    _an = ario_init();
    if(_an == NULL) dax_fatal(ds, "Unable to allocate Arduino Network Object");

    while(1) {
        if( ! _connected) {
            dax_debug(ds, LOG_COMM, "Attempting to connect to device %s", _device);
            result = ario_openport(_an, _device, _baudrate);
            if(result < 0) {
                dax_error(ds, "Unable to open Arduino Network on device %s", _device);
                sleep(_reconnect);
            } else {
                dax_debug(ds, LOG_COMM, "Connected to device %s", _device);
                _connected = 1;
                for(node = 0; node < _node_count; node++) {
                    _init_node(node);
                }
            }
        }
        if(_connected) {
            if(running == 0) { /* We'll only do this once */
                dax_mod_set(ds, MOD_CMD_RUNNING, NULL);
                running = 1;
            }
            gettimeofday(&start, NULL);
            good_nodes = 0;
            for(node = 0; node < _node_count; node++) {
                /* Check to see if the node is inhibited.  If it is then check
                 * to see if enough time has passed (reconnect seconds) to try
                 * to un-inhibit the node */
                if(BIT_ISSET(_nodes[node].flags, NODE_FLAG_INHIBITED)) {
                    gettimeofday(&now, NULL);
                    if(now.tv_sec > _nodes[node].inhibited.tv_sec + _nodes[node].reconnect) {
                        result = _init_node(node);
                        if(result) {
                            _inhibit_node(node);
                        } else {
                            BIT_RESET(_nodes[node].flags, NODE_FLAG_INHIBITED);
                        }
                    }
                }
                /* This is not really the same as an else from above because the above
                 * if() block may unset the INHIBITED flag */
                if( ! BIT_ISSET(_nodes[node].flags, NODE_FLAG_INHIBITED)) {
                    if(_exec_node(node) == 0) {
                        good_nodes++;
                    }
                }
            }
            if(good_nodes == 0) {
                dax_debug(ds, LOG_COMM, "Port down, disconnecting from %s", _device);
                ario_closeport(_an);
                _connected = 0;
                sleep(_reconnect);
            } else {
                gettimeofday(&end, NULL);
                time_spent = (end.tv_sec-start.tv_sec)*1000 + (end.tv_usec/1000 - start.tv_usec/1000);
                /* If it takes longer than the scan rate then just go again instead of sleeping */
                result = 0;
                if(time_spent < _rate) {
                    result = dax_event_wait(ds, _rate - time_spent, NULL);
                }
                /* Poll for the rest once the time has expired */
                if(result == 0) {
                    while(! dax_event_poll(ds, NULL));
                }
            }
        }
        /* Check to see if the quit flag is set.  If it is then bail */
        if(_quitsignal) {
            dax_debug(ds, LOG_MAJOR, "Quitting due to signal %d", _quitsignal);
            getout(_quitsignal);
        }
    }
    
 /* This is just to make the compiler happy */
    return(0);
}


/* Signal handler */
void
quit_signal(int sig)
{
    _quitsignal = sig;
}

/* We call this function to exit the program */
static void
getout(int exitstatus)
{
    dax_disconnect(ds);
    exit(exitstatus);
}
