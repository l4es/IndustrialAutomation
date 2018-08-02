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

#include <options.h>
#include <message.h>
#include <tagbase.h>
#include <common.h>
#include <module.h>
#include <func.h>
#include <pthread.h>
#include <syslog.h>
#include <signal.h>
#include <sys/wait.h>

static int quitflag = 0;

static void messagethread(void);
void child_signal(int);
void quit_signal(int);
void catch_signal(int);

int
main(int argc, const char *argv[])
{
    struct sigaction sa;
    pthread_t message_thread;
	int result;
    
    /* Set up the signal handlers */
    memset (&sa, 0, sizeof(struct sigaction));
    sa.sa_handler = &quit_signal;
    sigaction (SIGQUIT, &sa, NULL);
    sigaction (SIGINT, &sa, NULL);
    sigaction (SIGTERM, &sa, NULL);
    
    sa.sa_handler = &catch_signal;
    sigaction(SIGHUP, &sa, NULL);
    sigaction(SIGPIPE, &sa, NULL);

    sa.sa_handler = &child_signal;
    sigaction (SIGCHLD, &sa, NULL);
    
    //set_log_topic(LOG_MAJOR); /*TODO: Needs to be configuration */
    set_log_topic(-1); /*TODO: Needs to be configuration */
    
    /* Read configuration from defaults, file and command line */
    opt_configure(argc, argv);
    /* Go to the background */
    if(opt_daemonize()) {
        if(daemonize("OpenDAX")) {
            xerror("Unable to go to the background");
        }
    }
    
    result = msg_setup();    /* This creates and sets up the message sockets */
    if(result) xerror("msg_setup() returned %d", result);
    initialize_tagbase(); /* initialize the tag name database */
    initialize_module();  /* initialize module stuff */
    /* Start the message handling thread */
    if(pthread_create(&message_thread, NULL, (void *)&messagethread, NULL)) {
        xfatal("Unable to create message thread");
    }
        
    module_start_all(); /* Start all the modules */
    
    /* DO TESTING STUFF HERE */

    /* END TESTING STUFF */
    
    xlog(LOG_MAJOR, "OpenDAX started");
    
    while(1) { /* Main loop */
        /* TODO: This might could be some kind of condition
           variable or signal thing instead of just the sleep(). */
        module_scan();
        sleep(1);
        /* If the quit flag is set then we clean up and get out */
        if(quitflag) {
            /* TODO: Need to kill the message_thread */
            /* TODO: Should stop all running modules */
            kill(0, SIGTERM); /* ...this'll do for now */
            /* TODO: Should verify that all the children are dead.  If not
             then send a SIGKILL signal */
            msg_destroy(); /* Destroy the message queue */
            exit(-1);
        }
    }
}

/* This is the main message handling thread.  It should never return. */
static void
messagethread(void)
{
    while(1) {
        if(msg_receive()) {
            sleep(1);
        }
    }
}

/* Clean up any child modules that have shut down */
void
child_signal(int sig)
{
    int status;
    pid_t pid;

    do {
        pid = waitpid(-1, &status, WNOHANG);
        if(pid > 0) { 
            module_dmq_add(pid, status);
        }
    } while(pid > 0);
}

/* this handles shutting down of the server */
/* TODO: There's the easy way out and then there is the hard way out.
 * I need to figure out which is which and then act appropriately.
 */
void
quit_signal(int sig)
{
    xlog(LOG_MAJOR, "Quitting due to signal %d", sig);
    quitflag = 1;
}

/* TODO: May need to so something with signals like SIGPIPE etc */
void
catch_signal(int sig)
{
    xlog(LOG_MINOR, "Received signal %d", sig);    
}
