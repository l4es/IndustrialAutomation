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
 *  Main source code file for the OpenDAX example module
 */

/* NOTES: This is the skeleton module for OpenDAX.  It is the bare 
 * minimum module that shows the functionality of the library and
 * how to call the API function.  There is some error checking
 * that is not included in this file for simplicity.  You should
 * consult the API Reference in the OpenDAX Developer's Guide to
 * see what errors need to be checked for.  There are features of
 * the API that are not illustrated in this module.  This module is
 * meant to be a starting point for new module development not an
 * example of everything that can be done with the API.
 */

#include <skel.h>

void quit_signal(int sig);
static void getout(int exitstatus);

dax_state *ds;
static int _quitsignal;

/* TODO: Event notification logic, creating and manipulating
 * a compound data type.  I might use a separate function to
 * show some of the more advanced features of the API. */

/* main inits and then calls run */
int main(int argc,char *argv[]) {
    struct sigaction sa;
    int flags, result = 0, scan = 0, n;
    char *str, *tagname, *event_tag, *event_type;
    Handle h_full, h_part;
    dax_dint data[5];
    
    /* Set up the signal handlers for controlled exit*/
    memset (&sa, 0, sizeof(struct sigaction));
    sa.sa_handler = &quit_signal;
    sigaction (SIGQUIT, &sa, NULL);
    sigaction (SIGINT, &sa, NULL);
    sigaction (SIGTERM, &sa, NULL);

    /* Create and Initialize the OpenDAX library state object */
    ds = dax_init("skel"); /* Replace 'skel' with your module name */
    if(ds == NULL) {
        /* dax_fatal() logs an errlr and causes a quit 
         * signal to be sent to the module */
        dax_fatal(ds, "Unable to Allocate DaxState Object\n");
    }

    /* Create and initialize the configuration subsystem in the library */
    dax_init_config(ds, "skel");
    /* These flags will be passed to the dax_add_attribute() function.  They
     * basically mean that the following attributes can be set from either the
     * command line or the skel.conf module configuration file and that an
     * argument is required */
    flags = CFG_CMDLINE | CFG_MODCONF | CFG_ARG_REQUIRED;
    result += dax_add_attribute(ds, "tagname","tagname", 't', flags, "skel");
    result += dax_add_attribute(ds, "event_tag","event_tag", 'e', flags, "skel_event");
    /* For these attributes we add the opendax.conf configuration file as a
     * place where the attrubute can be set. No good reason for this other than
     * as an illustration. */
    flags = CFG_CMDLINE | CFG_MODCONF | CFG_DAXCONF | CFG_ARG_REQUIRED;
    result += dax_add_attribute(ds, "event_type","event_type", 'y', flags, "poll");
    /* Execute the configuration */
    dax_configure(ds, argc, argv, CFG_CMDLINE | CFG_DAXCONF);

    /* Get the results of the configuration */
    tagname = strdup(dax_get_attr(ds, "tagname"));
    event_tag = strdup(dax_get_attr(ds, "event_tag"));
    event_type = strdup(dax_get_attr(ds, "event_type"));
    /* Copying all of the strings into local variables might not be
     * the most efficient way of doing this.  The pointers returned
     * from dax_get_attr() function point to the strings inside the
     * configuration system.  Once dax_free_config() is called they
     * would no longer exist but the configuration can be freed at
     * any time or never freed if that makes sense for the module. We
     * did it this way in the skel module for simplicity */
    
    /* Free the configuration data */
    dax_free_config (ds);

    /* Set the logging flags to show all the messages */
    dax_set_debug_topic(ds, LOG_ALL);

    /* Check for OpenDAX and register the module */
    if( dax_connect(ds) ) {
        dax_fatal(ds, "Unable to find OpenDAX");
    }
    
    /* The only thing that skel does right now is manipulate this single
     * five element array of DINT's.  This line creates the tag. h_full is
     * a handle that we can use to read and write the entire array. */
    result = dax_tag_add(ds, &h_full, tagname, DAX_DINT, 5);
    if(result) {
        dax_fatal(ds, "Unable to create tag - %s", tagname);
    }
    /* This function retrieves a handle for the tag.  This is simply an
     * example of how to retrieve a handle for a single element in the array */
    str = malloc(strlen(tagname) + 3);
    if(str == NULL) {
        dax_fatal(ds, "Unable to allocate memory");
    }
    strcpy(str, tagname);
    str = strcat(str, "[2]");

    /* At this point str should be 'tagname[2]\0' */
    result = dax_tag_handle(ds, &h_part, str, 1);
    if(result) {
        dax_fatal(ds, "Unable to retrive Handle for tag - %s", tagname);
    }

    dax_mod_set(ds, MOD_CMD_RUNNING, NULL);

    while(1) {
        /* Check to see if the quit flag is set.  If it is then bail */
        if(_quitsignal) {
            dax_debug(ds, LOG_MAJOR, "Quitting due to signal %d", _quitsignal);
            getout(_quitsignal);
        }
        /* Every ten times though do something with the data */
        if((scan++ % 10) == 0) {
            for(n=0; n<5; n++) {
                data[n] += n;
            }
            /* write the whole tag */
            dax_write_tag(ds, h_full, data);
        }
        /* read the entire array into 'data' */
        result = dax_read_tag(ds, h_full, data);
        /* Just a silly calculation */
        data[2] = data[0] + data[1];
        /* Write the single word back to the server */
        result = dax_write_tag(ds, h_part, &data[2]);
        sleep(1);
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
