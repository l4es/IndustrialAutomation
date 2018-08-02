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

 * This file contains the message buffering code for the OpenDAX server
 */

#include <message.h>
#include <func.h>
#include <module.h>
#include <tagbase.h>
#include <options.h>

#include <sys/socket.h>
#include <sys/un.h>
#include <arpa/inet.h>
#include <string.h>

/* Notes:
 Okay here is how all this works.  To keep from having to have a buffer for
 each module and to keep from having some arbitrary amount of buffers for 
 new modules we are going to share some buffers.  Each buffer has a file
 descriptor associated with it.  When the socket code decides that it needs
 to read information from a socket it will use these functions to find a buffer
 that is associated with the file descriptor.  If there isn't one associated
 with the file descriptor then it tries to find a free one.  If it can't 
 find a free one then it'll create a new one.  There is a minimum number
 of buffers that will be kept allocated all the time to keep from calling
 malloc() and free() too much.
 
 There will be quite a few denial of service attacks that can be done here
 and I'll have to figure out a way to keep things limping along if some
 socket starts sending data to gum up the works.
*/


typedef struct dax_BuffNode {
    int fd;
    int index; /* Index of the next available char in the buffer */
    unsigned char buffer[DAX_MSGMAX];
    struct dax_BuffNode *next;
} dax_buffnode;

/* This is the head of the data buffer list */
static dax_buffnode *_buffer;

/* Allocate and initialize a buffer node */
static dax_buffnode *
_new_buffnode(void)
{
    dax_buffnode *node;
    
    node = malloc(sizeof(dax_buffnode));
    if(node == NULL) return NULL;
    node->fd = 0;
    node->index = 0;
    node->next = NULL;
    
    return node;
}

/* Create the initial linked list of buffer nodes */
int
buff_initialize(void)
{
    int n, count;
    dax_buffnode *node, *last;
    
    count = opt_min_buffers();
    last = NULL;
    
    for(n = 0; n < count; n++) {
        node = _new_buffnode();
        if(node == NULL) {
            xfatal("Unable to allocate all of the communication buffers");
        }
        if(n == 0) {
            _buffer = node;
        } else {
            last->next = node;
        }
        last = node;
    }
    return 0;
}   

/* Return the index of the buffer node that is either presently
   assigned to the fd or if there isn't one a free one */
static dax_buffnode *
find_buff_slot(int fd)
{
    dax_buffnode *node, *firstfree, *result;
    
    node = _buffer;
    firstfree = NULL;
    
    while(node != NULL) {
        if(node->fd == fd) /* found it */
            return node;
        else if(node->fd == 0 && firstfree == NULL) 
            firstfree = node;  /* store a free one for later */
        node = node->next;
    }
    /* couldn't find one that matched the fd so
     * if we have one that was free return it */
    if(firstfree != NULL) {
        result = firstfree;
    } else {
        result = _new_buffnode();
        if(result != NULL) {
            node = _buffer;
            /* double linked list would eliminate this ??? */
            while(node->next != NULL) node = node->next;
            node->next = result;
        }
    }
    if(result) result->fd = fd;
    return result;
}

int
buff_read(int fd)
{
    dax_buffnode *node;
    size_t result;
    u_int32_t size;
    
    node = find_buff_slot(fd);
    
    /* If we can't get a buffer then return error */
    if(node == NULL) return ERR_ALLOC;
    
    /* We don't want to read too much now do we */
    size = DAX_MSGMAX - node->index;
    result = read(fd, &node->buffer[node->index], size);
    //--Problem with xread() see func.c
    //--result = xread(fd, &node->buffer[node->index], size);
    
    if(result < 0) {
        xerror("Unable to read data from socket %d", fd);
        return ERR_MSG_RECV;
    } if(result == 0) { /* EOF means the other guy is closed */
        xlog(LOG_COMM | LOG_VERBOSE, "Received EOF on socket %d", fd);
        return ERR_NO_SOCKET;
    }
    
    node->index += result;
    
    /* Check the size and let the caller know how it turns out. */
    /* First four bytes of a message should always be the size of
       the message and it should be in network byte order */
    size = ntohl(*(u_int32_t *)node->buffer);
    if(node->index < (size - 1)) {
        return ERR_MSG_BAD;
    } else if(node->index >= (size - 1)) {
        return msg_dispatcher(fd, node->buffer);
    }else if(size > DAX_MSGMAX) {
        return ERR_2BIG;
    }
    return 0;
}
    
/* TODO: Check boundary conditions where min_buffers = 0 or 1.  Shouldn't
   be able to equal 0 but try to break it. */

/* This frees the message buffer associated with 'fd' */
void
buff_free(int fd)
{
    dax_buffnode *node;
    node = _buffer;
    
    while(node != NULL) {
        if(node->fd == fd) {
            node->fd = 0;
            node->index = 0;
            return;
        }
        node = node->next;
    }
}

/* This function essentially marks all of the buffers as free.  If there are
 * more than min_buffers it'll free() the last one.  Kindof a poor boy
 * garbage collection. */
void
buff_freeall(void)
{
    int n;
    dax_buffnode *node, *last;
    
    n = 0;
    node = _buffer;
    last = NULL;
    
    while(node != NULL) {
        node->fd = 0;
        node->index = 0;
        n++;
        /* If we are the last node, there are more than min_buffers nodes in the list
           and last is not NULL then free the last node */
        if(node->next == NULL && n > opt_min_buffers() && last != NULL) {
            free(node);
            node = NULL;
            last->next = NULL;
        } else {
            last = node;
            node = node->next;
        }
    }
}
