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
 *  source code file for the tag commands OpenDAX Command Line Client  module
 */


#include <daxc.h>

extern dax_state *ds;
extern int quiet_mode;

struct event_item {
    char used;
    dax_event_id event;
};
static struct event_item events[MAX_EVENTS];

static void
_event_callback(void *udata) {
    dax_event_id *id;
    int n;
    id =(dax_event_id *)udata;
    
    /* TODO This is probably bad form.  The event_id structure should be
     * opaque and some common representation should be created. */
    for(n = 0; n < MAX_EVENTS; n++) {
        if(events[n].event.index == id->index && events[n].event.id == id->id) {
            if(!quiet_mode) {
                printf("Event %d Recieved\n", n+1);
            } else {
                printf("%d\n", n+1);
            }
            return;
        }
    }
    fprintf(stderr, "ERROR: Bad Event Received\n");
}

static inline void
_convert_data_str(tag_type datatype, dax_type_union *dest, char *str) {
    
    switch(datatype) {
        case DAX_BYTE:
            dest->dax_byte = (dax_byte)strtol(str, NULL, 0);
            return;
        case DAX_SINT:
            dest->dax_sint = (dax_sint)strtol(str, NULL, 0);
            return;
        case DAX_UINT:
        case DAX_WORD:
            dest->dax_uint = (dax_uint)strtoul(str, NULL, 0);
            return;
        case DAX_INT:
            dest->dax_int = (dax_int)strtol(str, NULL, 0);
            return;
        case DAX_UDINT:
        case DAX_DWORD:
        case DAX_TIME:
            dest->dax_udint = (dax_udint)strtoul(str, NULL, 0);
            return;
        case DAX_DINT:
            dest->dax_dint = (dax_dint)strtol(str, NULL, 0);
            return;
        case DAX_ULINT:
        case DAX_LWORD:
            dest->dax_ulint = (dax_ulint)strtoull(str, NULL, 0);
            return;
        case DAX_LINT:
            dest->dax_lint = (dax_lint)strtoll(str, NULL, 0);
            return;
        case DAX_REAL:
            dest->dax_real = (dax_real)strtod(str, NULL);
            return;
        case DAX_LREAL:
            dest->dax_lreal = (dax_lreal)strtold(str, NULL);
            return;
        default:
            dest->dax_ulint = 0L;
    }
    return;
}


int
event_add(char **tokens, int count)
{
    Handle h;
    int event_type, event_count, result, n;
    dax_type_union value;
    void *ptr;
    
    for(n=0; n < MAX_EVENTS && events[n].used != 0; n++) ;
    if(n == MAX_EVENTS) {
        fprintf(stderr, "ERROR: Maximum Number of stored events is %d\n", MAX_EVENTS);
        return -1;
    }
    if(count < 3) {
        fprintf(stderr, "ERROR: Not enough arguments for EVENT ADD\n");
        return -1;
    }
    event_type = dax_event_string_to_type(tokens[2]);
    if(event_type == 0) {
        fprintf(stderr, "ERROR: Unkown event type %s\n", tokens[2]);
        return -1;
    }
    event_count = strtol(tokens[1], NULL, 0);
    result = dax_tag_handle(ds, &h, tokens[0], event_count);
    if(result == ERR_2BIG) {
        fprintf(stderr, "ERROR: Count %d is too big or invalid\n", event_count);
        return -1;
    } else if(result) {
        fprintf(stderr, "ERROR: Tagname %s not found\n", tokens[0]);
        return -1;
    }
    if(count == 4) { /* This means that a value was give */
        _convert_data_str(h.type, &value, tokens[3]);
        ptr = (void *)&value;
    } else {
        ptr = NULL;
    }
    result = dax_event_add(ds, &h, event_type, ptr, &events[n].event, 
                           _event_callback, (void *)&events[n].event, NULL);
    if(result) {
        fprintf(stderr, "ERROR: Problem adding event to server. Code = %d\n", result);
        return -1;
    }
    events[n].used = 1;
    if(!quiet_mode) printf("OK: Event %d added\n", n+1);
    return result;
}

int
event_del(char **tokens)
{
    int index;
    if(tokens[0] == NULL) {
        fprintf(stderr, "ERROR: Event ID needed\n");
        return -1;
    }
    index = strtol(tokens[0], NULL, 0) - 1;
    if(index < 0 || index > MAX_EVENTS) {
        fprintf(stderr, "ERROR: Event %d, is out of range\n", index);
        return -1;
    }
    return dax_event_del(ds, events[index].event);
}

int
event_wait(char **tokens)
{
    int timeout;
    
    if(tokens[0] == NULL) {
        timeout = 0;
    } else {
        timeout = (int)strtol(tokens[0], NULL, 0);
    }
    dax_event_wait(ds, timeout, NULL);
    return 0;
}

int
event_poll(void)
{
    while(dax_event_poll(ds, NULL) == 0);
    return 0;
}
