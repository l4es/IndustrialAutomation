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

 * This file contains some of the misc functions for the library
 */

#include <libdax.h>
#include <common.h>
#include <stdarg.h>
#include <signal.h>

/* These functions handle the logging and error callback function */

/* Callback functions. */
static void (*_dax_debug)(const char *output) = NULL;
static void (*_dax_error)(const char *output) = NULL;
static void (*_dax_log)(const char *output) = NULL;

void
dax_set_debug_topic(dax_state *ds, u_int32_t topic)
{
    ds->logflags = topic;
    dax_debug(ds, LOG_MAJOR, "Log Topics Set to %d", ds->logflags);
}

/* Function for modules to set the debug message callback */
void
dax_set_debug(dax_state *ds, void (*debug)(const char *format))
{
    _dax_debug = debug;
}

/* Function for modules to set the error message callback */
void
dax_set_error(dax_state *ds, void (*error)(const char *format))
{
    _dax_error = error;
}

/* Function for modules to override the dax_log function */
void
dax_set_log(dax_state *ds, void (*log)(const char *format))
{
    _dax_log = log;
}

/* TODO: Make these function allocate the memory at run time so that
 * we don't have this limitation */
#ifndef DEBUG_STRING_SIZE
  #define DEBUG_STRING_SIZE 80
#endif

/* Function for use inside the library for sending debug messages.
 * If the level is lower than the global _verbosity level then it will
 * print the message.  Otherwise do nothing */
void
dax_debug(dax_state *ds, int level, const char *format, ...)
{
    char output[DEBUG_STRING_SIZE];
    va_list val;
    
    /* check if the callback has been set and _verbosity is set high enough */
    if(level & ds->logflags) {
        va_start(val, format);
        if(_dax_debug) {
            vsnprintf(output, DEBUG_STRING_SIZE, format, val);
            _dax_debug(output);
        } else {
            vprintf(format, val);
            printf("\n");
        }
        va_end(val);
    }
}

/* Prints an error message if the callback has been set otherwise
   sends the string to stderr. */
void
dax_error(dax_state *ds, const char *format, ...)
{
    char output[DEBUG_STRING_SIZE];
    va_list val;
    va_start(val, format);
    
    /* Check whether the callback has been set */
    if(_dax_error) {
        vsnprintf(output, DEBUG_STRING_SIZE, format, val);
        _dax_error(format);
    } else {
        vfprintf(stderr, format, val);
        printf("\n");
    }
    va_end(val);
}

/* This function would be for logging events within the module */
/* TODO: At some point this may send a message to opendax right
   now it either calls the callback or if none is given prints to stdout */
void
dax_log(dax_state *ds, const char *format, ...)
{
    char output[DEBUG_STRING_SIZE];
    va_list val;
    va_start(val, format);
    
    if(_dax_log) {
        vsnprintf(output, DEBUG_STRING_SIZE, format, val);
        _dax_log(format);
    } else {
        vprintf(format, val);
        printf("\n");
    }
    va_end(val);
}

void
dax_fatal(dax_state *ds, const char *format, ...)
{
    char output[DEBUG_STRING_SIZE];
    va_list val;
    va_start(val, format);
    
    /* Check whether the callback has been set */
    if(_dax_error) {
        vsnprintf(output, DEBUG_STRING_SIZE, format, val);
        _dax_error(format);
    } else {
        vfprintf(stderr, format, val);
        fprintf(stderr, "\n");
    }
    va_end(val);
    kill(getpid(), SIGQUIT);
}

#ifdef USE_PTHREAD_LOCK

inline int
libdax_lock(dax_lock *lock) {
    int result;
    result = pthread_mutex_lock(lock);
    if(result) return ERR_GENERIC;
    return 0;
}

inline int
libdax_unlock(dax_lock *lock) {
    int result;
    result = pthread_mutex_unlock(lock);
    if(result) return ERR_GENERIC;
    return 0;
}

inline int
libdax_init_lock(dax_lock *lock) {
    int result;
    result = pthread_mutex_init(lock, NULL);
    if(result) return ERR_GENERIC;
    return 0;
}

inline int
libdax_destroy_lock(dax_lock *lock) {
    int result;
    result = pthread_mutex_destroy(lock);
    if(result) return ERR_GENERIC;
    return 0;
}

/* If no locking mechanisms are #defined then we just set the functions
 * to return nothing and hope that the optimizer will eliminate them */
#else

inline int
libdax_lock(dax_lock *lock) {
    return 0;
}

inline int
libdax_unlock(dax_lock *lock) {
    return 0;
}

inline int
libdax_init_lock(dax_lock *lock) {
    return 0;
}

inline int
libdax_destroy_lock(dax_lock *lock) {
    return 0;
}

#endif
