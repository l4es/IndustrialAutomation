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
 *  Main source code file for the OpenDAX wireless I/O module.
 */

#include <signal.h>
#include <wio.h>


void quit_signal(int sig);
static void getout(int exitstatus);

dax_state *ds;
static int _quitsignal;

int
sendcommand(int fd, char *cmd)
{
    char str[128];
    int len, result;

    printf("Sending Command [%s]\n", cmd);
    sprintf(str, "%s\r", cmd);
    len = strlen(str)+1;
    write(fd, str, len);
    bzero(str, 128);
    len = 0;
    sleep(1);
    do {
      result = read(fd, str + len, 128);
      len += result;
    } while(result > 0 && len < 128);
    printf("%d: %s\n", len, str);
    return result;
}

int
just_playing(int fd)
{
    char str[128];
    int result, index = 0;
    static int firsttime = 1;
    sleep(1);
    if(firsttime) {
        printf("Doing First Time Stuff\n");
        strcpy(str, "+++");
        write(fd, str, 3);
        sleep(2);
        bzero(str, 128);
        do {
           result = read(fd, str+index, 128);
           printf("read() returned : %d\n", result);
           index += result;
        } while(result > 0);

        printf("%d: %s\n", index, str);
        sleep(3);
        sendcommand(fd, "ATD05");
        sendcommand(fd, "ATD13");
        sendcommand(fd, "ATD23");
        sendcommand(fd, "ATD33");
        sendcommand(fd, "ATD43");
        sendcommand(fd, "ATAC");

        firsttime=0;
    }
    sendcommand(fd, "ATIO1");
    sendcommand(fd, "ATIO0");
    sendcommand(fd, "ATIS");
    return 0;
}


int
main(int argc, char *argv[])
{
    int fd;
    struct sigaction sa;
    int flags, result = 0;
    
    /* Set up the signal handlers for controlled exit*/
    memset (&sa, 0, sizeof(struct sigaction));
    sa.sa_handler = &quit_signal;
    sigaction (SIGQUIT, &sa, NULL);
    sigaction (SIGINT, &sa, NULL);
    sigaction (SIGTERM, &sa, NULL);

    /* Create and Initialize the OpenDAX library state object */
    ds = dax_init("wio"); /* Replace 'skel' with your module name */
    if(ds == NULL) {
        dax_fatal(ds, "Unable to Allocate DaxState Object\n");
    }

    /* Create and initialize the configuration subsystem in the library */
    dax_init_config(ds, "wio");
    flags = CFG_CMDLINE | CFG_MODCONF | CFG_ARG_REQUIRED;
    result += dax_add_attribute(ds, "device","device", 'd', flags, "wio");
    //result += dax_add_attribute(ds, "event_tag","event_tag", 'e', flags, "skel_event");
    dax_configure(ds, argc, argv, CFG_CMDLINE | CFG_MODCONF);

    //dax_free_config (ds);

    /* Set the logging flags to show all the messages */
    dax_set_debug_topic(ds, LOG_ALL);

    /* Check for OpenDAX and register the module */
    if( dax_connect(ds) ) {
        dax_fatal(ds, "Unable to find OpenDAX");
    }
    
//    result = dax_tag_add(ds, &h_full, tagname, DAX_DINT, 5);
//    if(result) {
//        dax_fatal(ds, "Unable to create tag - %s", tagname);
//    }
//    /* This function retrieves a handle for the tag.  This is simply an
//     * example of how to retrieve a handle for a single element in the array */
//    str = malloc(strlen(tagname) + 3);
//    if(str == NULL) {
//        dax_fatal(ds, "Unable to allocate memory");
//    }
//    strcpy(str, tagname);
//    str = strcat(str, "[2]");
//
//    /* At this point str should be 'tagname[2]\0' */
//    result = dax_tag_handle(ds, &h_part, str, 1);
//    if(result) {
//        dax_fatal(ds, "Unable to retrive Handle for tag - %s", tagname);
//    }
    fd = xbee_open_port(dax_get_attr(ds, "device"), B9600);
    while(1) {
        /* Check to see if the quit flag is set.  If it is then bail */
        if(_quitsignal) {
            dax_debug(ds, LOG_MAJOR, "Quitting due to signal %d", _quitsignal);
            getout(_quitsignal);
        }
        sleep(1);
        printf("Give 'er a try\n");
        just_playing(fd);
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

