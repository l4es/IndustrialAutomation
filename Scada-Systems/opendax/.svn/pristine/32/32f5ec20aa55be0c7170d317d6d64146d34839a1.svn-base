/*  OpenDAX - An open source data acquisition and control system
 *  Copyright (c) 2009 Phil Birkelbach
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
 *  Source code file for the library initialization functions.
 */

#include <libdax.h>
#include <dax/libcommon.h>

/* Allocate and initialize the state of the dax_state connection
 * object.  The returned object will need to be passed to 
 * dax_free() when finished. */
dax_state *
dax_init(char *name)
{
    dax_state *ds;
    ds = malloc(sizeof(dax_state));
    if(ds == NULL) return NULL;

    ds->attr_head = NULL;
    ds->L = NULL;
    ds->modulename = strdup(name);
    if(ds->modulename == NULL) return NULL;
    
    ds->msgtimeout = 0;
    ds->sfd = 0;       /* Server's File Descriptor */
    ds->afd = 0;       /* Asynchronous File Descriptor */
    ds->reformat = 0;  /* Flags to show how to reformat the incoming data */
    ds->logflags = 0;
    /* Tag Cache */
    ds->cache_head = NULL;     /* First node in the cache list */
    ds->cache_limit = 0;       /* Total number of nodes that we'll allocate */
    ds->cache_count = 0;       /* How many nodes we actually have */
    /* datatype list */
    ds->datatypes = NULL;
    ds->datatype_size = 0;
    /* Event list array */
    ds->events = malloc(sizeof(event_db));
    if(ds->events == NULL) {
        free(ds->modulename);
        free(ds);
        return NULL;
    }
    ds->event_size = 1;
    ds->event_count = 0;
    /* Logging functions */
    ds->dax_debug = NULL;
    ds->dax_error = NULL;
    ds->dax_log = NULL;
    ds->lock = malloc(sizeof(dax_lock));
    if(ds->lock == NULL) {
        free(ds->modulename);
        free(ds->events);
        free(ds);
        return NULL;
    }
    if(libdax_init_lock(ds->lock)) {
        free(ds->modulename);
        free(ds->events);
        free(ds->lock);
        free(ds);
        return NULL;
    }

    return ds;
}


int
dax_free(dax_state *ds)
{
    libdax_unlock(ds->lock);
    libdax_destroy_lock(ds->lock);
    free(ds->modulename);
    /* TODO: gotta loop through and free the udata in the events. */
    free(ds->events);
    free(ds->lock);
    free(ds);
    return 0;
}
