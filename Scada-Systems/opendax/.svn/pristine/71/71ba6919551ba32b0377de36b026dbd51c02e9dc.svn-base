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
 
 * This header contains all of the type definitions that are common between the
 * opendax server and the library.  For definitions that are private to the server
 * see daxtypes.h.  For definitions that are private to the library see libdax.h.
 */

#ifndef __LIBCOMMON_H
#define __LIBCOMMON_H

#include <opendax.h>
#include <sys/types.h>
#include <assert.h>

/* Message functions */
#define MSG_MOD_REG    0x0000 /* Register the module with the server */
#define MSG_TAG_ADD    0x0001 /* Add a tag */
#define MSG_TAG_DEL    0x0002 /* Delete a tag from the database */
#define MSG_TAG_GET    0x0003 /* Get the handle, type, size etc for the tag */
#define MSG_TAG_LIST   0x0004 /* Retrieve a list of all the tagnames */
#define MSG_TAG_READ   0x0005 /* Read the value of a tag */
#define MSG_TAG_WRITE  0x0006 /* Write the value to a tag */
#define MSG_TAG_MWRITE 0x0007 /* Masked Write */
#define MSG_MOD_GET    0x0008 /* Get the module parameters */
#define MSG_MOD_SET    0x0009 /* set the module parameters */
#define MSG_EVNT_ADD   0x000A /* Add an event to the taglist */
#define MSG_EVNT_DEL   0x000B /* Delete an event */
#define MSG_EVNT_GET   0x000C /* Get an event definition */
#define MSG_EVNT_MOD   0x000D /* Get an event definition */
#define MSG_CDT_CREATE 0x000E /* Create a Custom Datatype */
#define MSG_CDT_GET    0x000F /* Get the definition of a Custom Datatype */
/* More to come */

#define MSG_RESPONSE   0x1000000LL /* Flag for defining a response message */
#define MSG_ERROR      0x2000000LL /* Flag for defining an error message */

/* These are flags for the registration command */
#define CONNECT_SYNC  0x01 /* Used to identify the synchronous socket during registration */
#define CONNECT_EVENT 0x02 /* Identifies the asynchronous event socket during registration */

/* These are the values that the registration system uses to 
   determine whether or not the module will have to reformat
   the data because of different machine architectures. */
/* TODO: I need to spend some time on these so that I know that
   they will actually be good tests. Integers are okay but I may
   want to put some thought into the floats.  Got to make sure
   that none of the bytes are the same or they can be mixed up and
   still work?? */
#define REG_TEST_INT    0xBCDE
#define REG_TEST_DINT   0x56789ABC
#define REG_TEST_LINT   0x123456789ABCDEF0LL
#define REG_TEST_REAL   3.14159265
#define REG_TEST_LREAL  -58765463.8766677

/* Subcommands for the MSG_TAG_GET command */
#define TAG_GET_NAME    0x01 /* Retrieve the tag by name */
#define TAG_GET_INDEX   0x02 /* Retrieve the tag by it's index */

/* Subcommands for the MSG_CDT_GET command */
#define CDT_GET_NAME    0x01 /* Retrieve the type by name */
#define CDT_GET_TYPE    0x02 /* Retrieve the type by it's type */

/* Some Macros for manipulating CDT types */
#define CDT_TO_INDEX(TYPE) (TYPE & ~DAX_CUSTOM)
#define CDT_TO_TYPE(INDEX) (INDEX | DAX_CUSTOM)

/* Maximum size allowed for a single message */
#ifndef DAX_MSGMAX
#  define DAX_MSGMAX 4096
#endif

/* Event messages are a fixed size */
#ifndef EVENT_MSGSIZE
#  define EVENT_MSGSIZE 25
#endif

/* This defines the size of the message minus the actual data */
#define MSG_HDR_SIZE (sizeof(u_int32_t) + sizeof(u_int32_t))
#define MSG_DATA_SIZE (DAX_MSGMAX - MSG_HDR_SIZE)
#define MSG_TAG_DATA_SIZE (MSG_DATA_SIZE - sizeof(tag_idx_t))

/* This is a full sized message.  It's the largest message allowed to be sent */
struct dax_message{
    /* Message Header Stuff.  Changes here should be reflected in the 
     * MSG_HDR_SIZE definition above */
    u_int32_t size;     /* size of the data sent */
    u_int32_t command;  /* Which function to call */
    /* Main data payload */
    char data[MSG_DATA_SIZE];
    /* The following stuff isn't in the socket message */
    int fd;             /* We'll use the fd to identify the module*/
};

#define CONFIG_GLOBALNAME "calling_module"

typedef struct dax_message dax_message;

#endif /* ! __LIBCOMMON_H */
