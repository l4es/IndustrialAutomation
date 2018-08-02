/*  OpenDAX - An open source data acquisition and control system 
 *  Copyright (c) 1997 Phil Birkelbach
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
 
 * This is the header file for the tagname database handling routines
 */

#include <dax/daxtypes.h>
#include <dax/libcommon.h>
#include <opendax.h>

#ifndef __TAGBASE_H
#define __TAGBASE_H

/* This stuff may be better to belong in the configuration */

/* This is the initial size of the tagname list array */
#ifndef DAX_TAGLIST_SIZE
 #define DAX_TAGLIST_SIZE 1024
#endif

/* This is the amount that the tagname list array will grow when
 * the size is exceeded. */
#ifndef DAX_TAGLIST_INC
 #define DAX_TAGLIST_INC 1024
#endif

/* The initial size of the database */
#ifndef DAX_DATABASE_SIZE
 #define DAX_DATABASE_SIZE 1024
#endif

/* This is the increment by which the database will grow when
   the size is exceeded */
#ifndef DAX_DATABASE_INC
 #define DAX_DATABASE_INC 1024
#endif

#ifndef DAX_DATATYPE_SIZE
# define DAX_DATATYPE_SIZE 10
#endif

/* Define Handles for _status register points */
/* TODO: These should probably go away in lieu of making the _status tag a cdt */
#define STATUS_SIZE   4
#define STAT_MSG_RCVD 0
#define STAT_MSG_SENT 32
#define STAT_DB_SIZE  64
#define STAT_TAG_CNT 96

typedef struct dax_event_t {
    int id;              /* Unique identifier for this event definition */

    int byte;            /* The byte offset where the data block starts */
    unsigned char bit;   /* The bit offset */
    int count;           /* The number of items represented by the handle */
    u_int32_t size;      /* The total size of the data block in bytes */
    tag_type datatype;   /* The datatype of the block */
    int eventtype;       /* The type of event */
    void *data;          /* Data given by module */
    void *test;          /* Internal data, depends on event type */
    dax_module *notify;   /* List of every module to be notified of this event */
    struct dax_event_t *next;
} _dax_event;

/* This is the internal structure for the tag array. */
typedef struct {
    tag_type type;
    unsigned int count;
    char *name;
    int nextevent;
    _dax_event *events;
    char *data;
} _dax_tag_db;

typedef struct {
    /* TODO: Name's size is no longer fixed, should it be?
             It still is in the library.  Let's leave it for now?? */
    char *name;
    int tag_idx;
} _dax_tag_index;

/* Tag Database Handling Functions */
void initialize_tagbase(void);
tag_index tag_add(char *name, tag_type type, unsigned int count);
int tag_del(char *name);
int tag_get_name(char *, dax_tag *);
int tag_get_index(int, dax_tag *);
long int tag_get_count(void);
int tag_get_size(tag_index idx);


int tag_read(tag_index handle, int offset, void *data, int size);
int tag_write(tag_index handle, int offset, void *data, int size);
int tag_mask_write(tag_index handle, int offset, void *data, void *mask, int size);

/* Custom DataType functions */
tag_type cdt_create(char *str, int *error);
unsigned int cdt_get_type(char *name);
char *cdt_get_name(unsigned int type);
int type_size(tag_type type);
int serialize_datatype(tag_type type, char **str);

/* The event stuff is defined in events.c */
void event_check(tag_index idx, int offset, int size);
int event_add(Handle h, int event_type, void *data, dax_module *module);
int event_del(int index, int id, dax_module *module);
int events_cleanup(dax_module *module);

#define DAX_DIAG
#ifdef DAX_DIAG
/* Diagnostic functions: should not be compiled in production */
void diag_list_tags(void);

#endif /* DAX_DIAG */

#endif /* !__TAGBASE_H */
