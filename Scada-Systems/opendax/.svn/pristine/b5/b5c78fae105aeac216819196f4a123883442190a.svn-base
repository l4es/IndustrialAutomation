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

 * This file contains the libdax functions that send messages to the server
 */
 
#include <libdax.h>
#include <dax/libcommon.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <math.h>


/* These are the generic message functions.  They simply send the message of
 * the type given by command, attach the payload.  The payloads size should be
 * given in bytes */
static int
_message_send(dax_state *ds, int command, void *payload, size_t size)
{
    int result;
    char buff[DAX_MSGMAX];
    
    /* We always send the size and command in network order */
    ((u_int32_t *)buff)[0] = htonl(size + MSG_HDR_SIZE);
    ((u_int32_t *)buff)[1] = htonl(command);
    memcpy(&buff[MSG_HDR_SIZE], payload, size);

    /* TODO: We need to set some kind of timeout here.  This could block
       forever if something goes wrong.  It may be a signal or something too. */
    result = write(ds->sfd, buff, size + MSG_HDR_SIZE);
    
    if(result < 0) {
    /* TODO: Should we handle the case when this returns due to a signal */
        dax_error(ds, "_message_send: %s", strerror(errno));
        return ERR_MSG_SEND;
    }
    return 0;
}

/* This function waits for a message with the given command to come in. If
   a message of another command comes in it will send that message out to
   an asynchronous command handler.  This is due to a race condition that could
   happen if the server puts a message in the queue after we send a request but
   before we retrieve the result. */
/* TODO: Do a better job of explaining the functionality of this function */
static int
_message_recv(dax_state *ds, int command, void *payload, int *size, int response)
{
    char buff[DAX_MSGMAX];
    int index, done, msg_size, result;
    done = index = msg_size = 0;
    
    while( index < msg_size || index < MSG_HDR_SIZE) {
        result = read(ds->sfd, &buff[index], DAX_MSGMAX);
        /*****TESTING STUFF******/
//        printf("_message_recv() returned %d\n", result);
//        for(done = 0; done < result; done ++) {
//            printf("0x%02X[%c] " , (unsigned char)buff[done], (unsigned char)buff[done]);
//        } printf("\n");
        
        if(result < 0) {
            if(errno == EWOULDBLOCK) {
                dax_debug(ds, LOG_COMM, "_message_recv Timed out");
                return ERR_TIMEOUT;
            } else {
                dax_debug(ds, LOG_COMM, "_message_recv failed: %s", strerror(errno));
                return ERR_MSG_RECV;
            }
        } else if(result == 0) {
            printf("TODO: I don't know what to do with read returning 0\n");
            return ERR_GENERIC;
        } else {
            index += result;
        }
        
        if(index >= MSG_HDR_SIZE) {
            msg_size = ntohl(*(u_int32_t *)buff);
            if(msg_size > DAX_MSGMAX) {
                dax_debug(ds, LOG_COMM, "_message_recv message size is too big");
                return ERR_MSG_BAD;
            }
        }
    }
    /* This gets the command out of the buffer */
    result = ntohl(*((u_int32_t *)&buff[4]));
    
    /* Test if the error flag is set and then return the error code */
    if(result == (command | MSG_ERROR)) {
        return stom_dint((*(int32_t *)&buff[8]));
    } else if(result == (command | (response ? MSG_RESPONSE : 0))) {
        if(size) {
            if((msg_size - MSG_HDR_SIZE) > *size) {
                printf("Why do we think it's too big. msg_size = %d, *size = %d\n", msg_size, *size);
                return ERR_2BIG;
            } else {
                memcpy(payload, &buff[MSG_HDR_SIZE], msg_size - MSG_HDR_SIZE);
                *size = msg_size - MSG_HDR_SIZE; /* size is value result */
            }
        }
    } else { /* This is not the command we wanted */
        printf("TODO: Whoa we got the wrong command\n");
    }
    return 0;
}

/* Connect to the server.  If the "server" attribute is local we
 * connect via LOCAL domain socket called out in "socketname" else
 * we connect to the server at IP address "serverip" on port "serverport".
 * returns a positive file descriptor on sucess and a negative error
 * code on failure. */
static int
_get_connection(dax_state *ds)
{
    int fd, len;
    struct sockaddr_un addr_un;
    struct sockaddr_in addr_in;
    struct timeval tv;
    
    if( ! strcasecmp("local", dax_get_attr(ds, "server") )) {
        //printf("...... Connecting to local socket - %s\n", dax_get_attr("socketname"));
        /* create a UNIX domain stream socket */
        if ((fd = socket(AF_UNIX, SOCK_STREAM, 0)) < 0) {
            dax_error(ds, "Unable to create local socket - %s", strerror(errno));
            return ERR_NO_SOCKET;
        }
        /* fill socket address structure with our address */
        memset(&addr_un, 0, sizeof(addr_un));
        addr_un.sun_family = AF_UNIX;
        strncpy(addr_un.sun_path, dax_get_attr(ds, "socketname"), sizeof(addr_un.sun_path));

        len = offsetof(struct sockaddr_un, sun_path) + strlen(addr_un.sun_path);
        if (connect(fd, (struct sockaddr *)&addr_un, len) < 0) {
            dax_error(ds, "Unable to connect to local socket - %s", strerror(errno));
            return ERR_NO_SOCKET;
        } else {
            dax_debug(ds, LOG_COMM, "Connected to Local Server fd = %d", fd);
        }
    } else { /* We are supposed to connect over the network */
        //printf("...... Connecting over the network\n");
        /* create an IPv4 TCP socket */
        if ((fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
            dax_error(ds, "Unable to create remote socket - %s", strerror(errno));
            return ERR_NO_SOCKET;
        }
        memset(&addr_in, 0, sizeof(addr_in));
        addr_in.sin_family = AF_INET;
        addr_in.sin_port = htons(strtol(dax_get_attr(ds, "serverport"), NULL, 0));
        inet_pton(AF_INET, dax_get_attr(ds, "serverip"), &addr_in.sin_addr);
        
        if (connect(fd, (struct sockaddr *)&addr_in, sizeof(addr_in)) < 0) {
            dax_error(ds, "Unable to connect to remote socket - %s", strerror(errno));
            return ERR_NO_SOCKET;
        } else {
            dax_debug(ds, LOG_COMM, "Connected to Network Server fd = %d", fd);
        }
    }
    tv.tv_sec = opt_get_msgtimeout(ds) / 1000;
    tv.tv_usec = (opt_get_msgtimeout(ds) % 1000) * 1000;
    
    len = setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));
    if(len) {
        dax_error(ds, "Problem setting socket timeout - s", strerror(errno));
    }
    return fd;
     
}

#define CON_HDR_SIZE 8

static int
_mod_connect(dax_state *ds, char *name)
{
    int result, len;
    char buff[DAX_MSGMAX];
    
/* TODO: Boundary check that a name that is longer than data size will
   be handled correctly. */
    len = strlen(name) + 1;
    if(len > (MSG_DATA_SIZE - CON_HDR_SIZE)) {
        len = MSG_DATA_SIZE - CON_HDR_SIZE;
        name[len] = '\0';
    }
    
    /* For registration we send the data in network order no matter what */
    *((u_int32_t *)&buff[0]) = htonl(getpid());       /* 32 bits for the PID */
    *((u_int32_t *)&buff[4]) = htonl(CONNECT_SYNC);  /* registration flags */
    strcpy(&buff[CON_HDR_SIZE], name);                /* The rest is the name */

    if((result = _message_send(ds, MSG_MOD_REG, buff, CON_HDR_SIZE + len)))
        return result;
    len = DAX_MSGMAX;
    if((result = _message_recv(ds, MSG_MOD_REG, buff, &len, 1)))
        return result;

    /* Here we check to see if the data that we got in the registration message is in the same
       format as we use here on the client module. This should be offloaded to a separate
       function that can determine what needs to be done to the incoming and outgoing data to
       get it to match with the server */
    if( (*((u_int16_t *)&buff[0]) != REG_TEST_INT) ||     
        (*((u_int32_t *)&buff[2]) != REG_TEST_DINT) || 
        (*((u_int64_t *)&buff[6]) != REG_TEST_LINT)) {   
        /* TODO: right now this is just to show error.  We need to determine if we can
           get the right data from the server by some means. */
        ds->reformat = REF_INT_SWAP;
    } else {
        ds->reformat = 0; /* this is redunant, already done in dax_init() */
    }
    /* There has got to be a better way to compare that we are getting good floating point numbers */
    if( fabs(*((float *)&buff[14]) - REG_TEST_REAL) / REG_TEST_REAL   > 0.0000001 || 
        fabs(*((double *)&buff[18]) - REG_TEST_LREAL) / REG_TEST_REAL > 0.0000001) {   
        ds->reformat |= REF_FLT_SWAP;
    }
    /* TODO: Need to store my name somewhere ??? */
    /* TODO: returning _reformat is only good until we figure out how to reformat the
     * messages. Then we should return 0.  Right now since there isn't any reformating
     * of messages being done we consider it an error and return that so that the module
     * won't try to communicate */
    return ds->reformat;
}

static int
_event_connect(dax_state *ds)
{
    int result, len, tmpfd;
    char buff[DAX_MSGMAX];
    
    *((u_int32_t *)&buff[0]) = htonl(getpid());       /* 32 bits for the PID */
    *((u_int32_t *)&buff[4]) = htonl(CONNECT_EVENT); /* registration flags */
    
    /* This is to trick the _message_send into sending on the new connection
     * instead of the existing one. */
    tmpfd = ds->sfd;
    ds->sfd = ds->afd;
    /* For registration we pack the data no matter what */
    if((result = _message_send(ds, MSG_MOD_REG, buff, CON_HDR_SIZE))) {
        ds->sfd = tmpfd;
        return result;
    }
    len = DAX_MSGMAX;
    result = _message_recv(ds, MSG_MOD_REG, buff, &len, 1);
    ds->sfd = tmpfd;
    return result;
}

/* Setup the module data structures and send registration message
 * to the server.  *name is the name that we want to give our 
 * module */
int
dax_connect(dax_state *ds)
{
    int fd, result;
    
    libdax_lock(ds->lock);
    dax_debug(ds, LOG_COMM, "Sending registration for name - %s", ds->modulename);
    
    /* This is the connection that we used for all the functional
     * request / response messages. */
    fd = _get_connection(ds);
    if(fd > 0) {
        ds->sfd = fd;
    } else {
        return fd;
    }
    
    result = _mod_connect(ds, ds->modulename);
    if(result) {
        libdax_unlock(ds->lock);
        return result;
    }
    
    /* This will be the event connection.  This socket receives
     * asynchronous messages that are generated in the server */
    fd = _get_connection(ds);
    if(fd > 0) {
        ds->afd = fd;
    } else {
        libdax_unlock(ds->lock);
        return fd;
    }
    result = _event_connect(ds); 
    if(result) {
        libdax_unlock(ds->lock);
        return result;
    }
    init_tag_cache(ds);
    
    libdax_unlock(ds->lock);
    return 0;
}

int
dax_disconnect(dax_state *ds)
{
    int len, result = -1;
    libdax_lock(ds->lock);

    if(ds->sfd) {
        result = _message_send(ds, MSG_MOD_REG, NULL, 0);
        if(! result ) {
            len = 0;
            result = _message_recv(ds, MSG_MOD_REG, NULL, &len, 1);
        }
        close(ds->sfd);
        ds->sfd = 0;
        close(ds->afd);
        ds->afd = 0;
    }
    libdax_unlock(ds->lock);
    return result;
}

/* Not yet implemented */
int
dax_mod_get(dax_state *ds, char *modname)
{
    return 0;
}

/* This is a generic function for setting module parameters.  Right now it is only
 * capable of setting the RUNNING flag */
int
dax_mod_set(dax_state *ds, u_int8_t cmd, void *param)
{
    int result, size;
    char buff[1];  /* So far this is as big as we need */

    libdax_lock(ds->lock);
    buff[0] = cmd;
    if(cmd == MOD_CMD_RUNNING) {
        size = 1;
    } else {
        return ERR_ARG;
    }
        /* Send the message to the server.  Add 2 to the size for the subcommand and the NULL */
    result = _message_send(ds, MSG_MOD_SET, buff, size);
    if(result) {
        dax_error(ds, "Can't send MSG_MOD_SET message");
        libdax_unlock(ds->lock);
        return result;
    }
    size = 0;
    result = _message_recv(ds, MSG_MOD_SET, buff, &size, 1);
    if(result) {
        dax_error(ds, "Problem receiving message MSG_MOD_SET : result = %d", result);
        libdax_unlock(ds->lock);
        return result;
    }
    libdax_unlock(ds->lock);
    return 0;
}

/* Sends a message to dax to add a tag.  The payload is basically the
 * tagname without the handle. */

int
dax_tag_add(dax_state *ds, Handle *h, char *name, tag_type type, int count)
{
    int size, result;
    dax_tag tag;
    char buff[DAX_TAGNAME_SIZE + 8 + 1];
    
    if(count == 0) return ERR_ARG;
    if(name) {
        if((size = strlen(name)) > DAX_TAGNAME_SIZE) {
            return ERR_2BIG;
        }
        /* Add the 8 bytes for type and count to one byte for NULL */
        size += 9;
        /* TODO Need to do some more error checking here */
        *((u_int32_t *)&buff[0]) = mtos_udint(type);
        *((u_int32_t *)&buff[4]) = mtos_udint(count);
        
        strcpy(&buff[8], name);
    } else {
        return ERR_TAG_BAD;
    }
    libdax_lock(ds->lock);

    result = _message_send(ds, MSG_TAG_ADD, buff, size);
    if(result) { 
        libdax_unlock(ds->lock);
        return ERR_MSG_SEND;
    }
    
    size = 4; /* we just need the handle */
    result = _message_recv(ds, MSG_TAG_ADD, buff, &size, 1);

    if(result == 0) {
        if(h != NULL) {
            h->index = *(tag_index *)buff;
            h->byte = 0;
            h->bit = 0;
            h->type = type;
            h->count = count;
            if(type == DAX_BOOL) {
                h->size = (count - 1)/8 +1;
            } else {
                h->size = count * dax_get_typesize(ds, type);
            }
        }
        strcpy(tag.name, name);
        tag.idx = *(tag_index *)buff;
        tag.type = type;
        tag.count = count;
        /* Just in case this call modifies the tag */
        cache_tag_del(ds, name);
        cache_tag_add(ds, &tag);
    }
    libdax_unlock(ds->lock);
    return result;
}


/* These tag name getting routines will have to be rewritten when we get
the custom data types going.  Returns zero on success. */
/* TODO: Need to clean up this function.  There is stuff I don't think I want in here */ 
int
dax_tag_byname(dax_state *ds, dax_tag *tag, char *name)
{
    int result, size;
    char *buff;
        
    if(name == NULL) return ERR_ARG;
    
    if((size = strlen(name)) > DAX_TAGNAME_SIZE) return ERR_2BIG;
    
    libdax_lock(ds->lock);
    if(check_cache_name(ds, name, tag)) {
        /* We make buff big enough for the outgoing message and the incoming
           response message which would have 3 additional int32s */
        buff = malloc(size + 14);
        if(buff == NULL) return ERR_ALLOC;
        buff[0] = TAG_GET_NAME;
        strcpy(&buff[1], name);
        /* Send the message to the server.  Add 2 to the size for the subcommand and the NULL */
        result = _message_send(ds, MSG_TAG_GET, buff, size + 2);
        if(result) {
            dax_error(ds, "Can't send MSG_TAG_GET message");
            free(buff);
            libdax_unlock(ds->lock);
            return result;
        }
        size += 14; /* This makes room for the type, count and handle */
        
        result = _message_recv(ds, MSG_TAG_GET, buff, &size, 1);
        if(result) {
            dax_error(ds, "Problem receiving message MSG_TAG_GET : result = %d", result);
            free(buff);
            libdax_unlock(ds->lock);
            return result;
        }
        tag->idx = stom_dint( *((int *)&buff[0]) );
        tag->type = stom_udint(*((u_int32_t *)&buff[4]));
        tag->count = stom_udint(*((u_int32_t *)&buff[8]));
        buff[size - 1] = '\0'; /* Just to make sure */
        strcpy(tag->name, &buff[12]);
        cache_tag_add(ds, tag);
        free(buff);
    }
    libdax_unlock(ds->lock);
    return 0;
}

/* Retrieves the tag by handle.  */
int
dax_tag_byindex(dax_state *ds, dax_tag *tag, tag_index handle)
{
    int result, size;
    char buff[DAX_TAGNAME_SIZE + 13];

    libdax_lock(ds->lock);
    if(check_cache_index(ds, handle, tag)) {
        buff[0] = TAG_GET_INDEX;
        *((tag_index *)&buff[1]) = handle;
        result = _message_send(ds, MSG_TAG_GET, buff, sizeof(tag_index) + 1);
        if(result) {
            dax_error(ds, "Can't send MSG_TAG_GET message");
            libdax_unlock(ds->lock);
            return result;
        }
        /* Maximum size of buffer, the 13 is the NULL plus three integers */
        size = DAX_TAGNAME_SIZE + 13;
        result = _message_recv(ds, MSG_TAG_GET, buff, &size, 1);
        if(result) {
            //dax_error("Unable to retrieve tag for handle %d", handle);
            libdax_unlock(ds->lock);
            return result;
        }
        tag->idx = stom_dint(*((int32_t *)&buff[0]));
        tag->type = stom_dint(*((int32_t *)&buff[4]));
        tag->count = stom_dint(*((int32_t *)&buff[8]));
        buff[DAX_TAGNAME_SIZE + 12] = '\0'; /* Just to be safe */
        strcpy(tag->name, &buff[12]);
        /* Add the tag to the tag cache */
        cache_tag_add(ds, tag);
    }
    libdax_unlock(ds->lock);
    return 0;
}
 
/* The following three functions are the core of the data handling
 * system in Dax.  They are the raw reading and writing functions.
 * 'handle' is the handle of the tag as returned by the dax_tag_add()
 * function, 'offset' is the byte offset into the data area of the tag
 * 'data' is a pointer to a data area where the data will be written
 * and 'size' is the number of bytes to read. If data isn't allocated
 * then bad things will happen.  The data will come out of here exactly
 * like it appears in the server.  It is up to the module to convert
 * the data to the module's number format. */
int
dax_read(dax_state *ds, tag_index idx, int offset, void *data, size_t size)
{
    int n, count, m_size, sendsize;
    int result = 0;
    int buff[3];
    
    /* This calculates the amount of data that we can send with a single message
        It subtracts a handle_t from the data size for use as the tag handle.*/
    m_size = MSG_DATA_SIZE;
    count = ((size - 1) / m_size) + 1;
    for(n = 0; n < count; n++) {
        if(n == (count - 1)) { /* Last Packet */
            sendsize = size % m_size; /* What's left over */
        } else {
            sendsize = m_size;
        }
        buff[0] = mtos_dint(idx);
        buff[1] = mtos_dint(offset + n * m_size);
        buff[2] = mtos_dint(sendsize);

        libdax_lock(ds->lock);
        result = _message_send(ds, MSG_TAG_READ, (void *)buff, sizeof(buff));
        if(result) {
            libdax_unlock(ds->lock);
            return result;
        }
        result = _message_recv(ds, MSG_TAG_READ, &((char *)data)[m_size * n], &sendsize, 1);
        
        if(result) {
            libdax_unlock(ds->lock);
            return result;
        }
    }
    libdax_unlock(ds->lock);
    return 0;
}

/* This is a type neutral way to just write bytes to the data table.
 * It is assumed that the data is already in the servers number format.
 * size is the total number of bytes to send, the offset is the byte
 * offset into the data area of the tag.
 */
int
dax_write(dax_state *ds, tag_index idx, int offset, void *data, size_t size)
{
    size_t n, count, m_size, sendsize;
    int result;
    char buff[MSG_DATA_SIZE];
    
    /* This calculates the amount of data that we can send with a single message
       It subtracts a handle_t from the data size for use as the tag handle and
       an int, because we'll send the handle and the offset.*/
    m_size = MSG_DATA_SIZE - sizeof(tag_index) - sizeof(int);
    /* count is the number of messages that we will have to send to transport this data. */
    count = ( (size - 1) / m_size ) + 1;
    for(n = 0; n < count; n++) {
        if(n == (count - 1)) { /* Last Packet */
            sendsize = size % m_size; /* What's left over */
        } else {
            sendsize = m_size;
        }
        /* Write the data to the message buffer */
        *((tag_index *)&buff[0]) = mtos_dint(idx);
        *((int *)&buff[4]) = mtos_dint(offset + n * m_size);
        memcpy(&buff[8], data + (m_size * n), sendsize);

        libdax_lock(ds->lock);
        result = _message_send(ds, MSG_TAG_WRITE, buff, sendsize + sizeof(tag_index) + sizeof(int));
        if(result) {
            libdax_unlock(ds->lock);
            return result;
        }
        result = _message_recv(ds, MSG_TAG_WRITE, buff, 0, 1);
        if(result) {
            libdax_unlock(ds->lock);
            return result;
        }
    }
    libdax_unlock(ds->lock);
    return 0;
}

/* Same as the dax_write() function except that only bits that are in *mask
 * will be changed. */
int
dax_mask(dax_state *ds, tag_index idx, int offset, void *data, void *mask, size_t size)
{
    size_t n, count, m_size, sendsize;
    u_int8_t buff[MSG_DATA_SIZE];
    int result;

    /* This calculates the amount of data that we can send with a single message
       It subtracts a handle_t from the data size for use as the tag handle.*/
    m_size = (MSG_DATA_SIZE - sizeof(tag_index) - sizeof(int)) / 2;
    count=((size - 1) / m_size) + 1;
    for(n = 0; n < count; n++) {
        if(n == (count - 1)) { /* Last Packet */
            sendsize = (size % m_size); /* What's left over */
        } else {
            sendsize = m_size;
        }
        /* Write the data to the message buffer */
        *((tag_index *)&buff[0]) = mtos_dint(idx);
        *((int *)&buff[4]) = mtos_dint(offset + n * m_size);
        memcpy(&buff[8], data + (m_size * n), sendsize);
        memcpy(&buff[8 + sendsize], mask + (m_size * n), sendsize);

        libdax_lock(ds->lock);
        result = _message_send(ds, MSG_TAG_MWRITE, buff, sendsize * 2 + sizeof(tag_index) + sizeof(int));
        if(result) {
            libdax_unlock(ds->lock);
            return result;
        }
        result = _message_recv(ds, MSG_TAG_MWRITE, buff, 0, 1);
        if(result) {
            libdax_unlock(ds->lock);
            return result;
        }
    }
    libdax_unlock(ds->lock);
    return 0;
}

int
dax_event_add(dax_state *ds, Handle *h, int event_type, void *data,
              dax_event_id *id, void (*callback)(void *udata),
              void *udata, void (*free_callback)(void *udata))
{
    int test;
    dax_dint result;
    dax_dint temp;
    dax_udint u_temp;
    int size;
    dax_event_id eid;
    char buff[MSG_DATA_SIZE];

    temp = mtos_dint(h->index);      /* Index */
    memcpy(buff, &temp, 4);
    temp = mtos_dint(h->byte);       /* Byte ofset */
    memcpy(&buff[4], &temp, 4);
    temp = mtos_dint(h->count);      /* Tag Count */
    memcpy(&buff[8], &temp, 4);
    temp = mtos_dint(h->type);       /* Datatype */
    memcpy(&buff[12], &temp, 4);
    temp = mtos_dint(event_type);    /* Event Type */
    memcpy(&buff[16], &temp, 4);
    u_temp = mtos_udint(h->size);    /* Size in Bytes */
    memcpy(&buff[20], &u_temp, 4);
    buff[24]=h->bit;                 /* Bit offset */
    
    if(data != NULL) {
        mtos_generic(h->type, &buff[25], data);
        size = 25 + TYPESIZE(h->type) / 8;
    } else {
        size = 25;
    }
    
    libdax_lock(ds->lock);
    if(_message_send(ds, MSG_EVNT_ADD, buff, size)) {
        libdax_unlock(ds->lock);
        return ERR_MSG_SEND;
    } else {
        test = _message_recv(ds, MSG_EVNT_ADD, &result, &size, 1);
        if(test) {
            libdax_unlock(ds->lock);
            return test;
        } else {
            if(id != NULL) {
                id->id = result;
                id->index = h->index;
            }
            eid.id = result;
            eid.index = h->index;
            result = add_event(ds, eid, udata, callback, free_callback);
            if(result) {
                libdax_unlock(ds->lock);
                return result;
            }
        }
    }
    libdax_unlock(ds->lock);
    return 0;
}

int
dax_event_del(dax_state *ds, dax_event_id id)
{
    int test, size;
    dax_dint result;
    dax_dint temp;
    char buff[MSG_DATA_SIZE];

    temp = mtos_dint(id.index);      /* Tag Index */
    memcpy(buff, &temp, 4);
    temp = mtos_dint(id.id);       /* Event ID */
    memcpy(&buff[4], &temp, 4);
    size = 8;
    
    libdax_lock(ds->lock);
    if(_message_send(ds, MSG_EVNT_DEL, buff, size)) {
        libdax_unlock(ds->lock);
        return ERR_MSG_SEND;
    } else {
        test = _message_recv(ds, MSG_EVNT_DEL, &result, &size, 1);
        if(test) {
            libdax_unlock(ds->lock);
            return test;
        } else {
            del_event(ds, id);
        }
    }
    libdax_unlock(ds->lock);
    return result;

    
    return 0;
}

int
dax_event_get(dax_state *ds, dax_event_id id)
{
    
    return 0;
}

int
dax_event_mod(dax_state *ds, dax_event_id id, Handle *h, int event_type, void *data,
              void (*callback)(void *udata), void *udata)
{
    
    return 0;
}

/* write the datatype to the server, and free() it */
/* TODO: There is an arbitrary limit to the size that a compound
 * datatype can be. If the desription string is larger than can
 * be sent in one message this will fail. */
int
dax_cdt_create(dax_state *ds, dax_cdt *cdt, tag_type *type)
{
    int size = 0, result;
    cdt_member *this;
    char test[DAX_TAGNAME_SIZE + 1];
    char buff[MSG_DATA_SIZE], rbuff[10];
    
    if(cdt->name == NULL || cdt->members == NULL) return ERR_EMPTY;
    
    /* The first thing we do is figure out how big it
     * will all be. */
    size = strlen(cdt->name);
    this = cdt->members;

    while(this != NULL) {
        size += strlen(this->name);
        size += strlen(dax_type_to_string(ds, this->type));
        snprintf(test, DAX_TAGNAME_SIZE + 1, "%d", this->count);
        size += strlen(test);
        size += 3; /* This is for the ':' and the two commas */
        this = this->next;
    }
    size += 1; /* For Trailing NULL */
    
    if(size > MSG_DATA_SIZE) return ERR_2BIG;
    
    /* Now build the string */
    strncpy(buff, cdt->name, size - 1);
    this = cdt->members;

    while(this != NULL) {
        strncat(buff, ":", size - 1);
        strncat(buff, this->name, size - 1);
        strncat(buff, ",", size);
        strncat(buff, dax_type_to_string(ds, this->type), size - 1);
        strncat(buff, ",", size - 1);
        snprintf(test, DAX_TAGNAME_SIZE + 1, "%d", this->count);
        strncat(buff, test, size - 1);

        this = this->next;
    }

    libdax_lock(ds->lock);
    result = _message_send(ds, MSG_CDT_CREATE, buff, size);
    
    if(result) { 
        libdax_unlock(ds->lock);
        return result;
    }
    
    size = 10;
    result = _message_recv(ds, MSG_CDT_CREATE, rbuff, &size, 1);
    
    if(result == 0) {
        if(type != NULL) {
            *type = stom_udint(*((tag_type *)rbuff));
        }
        result = add_cdt_to_cache(ds, stom_udint(*((tag_type *)rbuff)), buff);
        dax_cdt_free(cdt);
    }
    libdax_unlock(ds->lock);
    return result;
}

/* This function retrieves the serialized string definition
 * from the server and puts the definition into list of
 * datatypes so that the rest of the library can access them.
 * Either type or name should be passed.  If name is passed
 * we will retrieve the cdt from the server by name and by
 * the type if NULL is passed for name.
 * 
 * If this function returns true then it can be assumed that
 * the type is in the cache and can then be retrieved there.
 */
int
dax_cdt_get(dax_state *ds, tag_type cdt_type, char *name)
{
    int result, size;
    char buff[MSG_DATA_SIZE];
    tag_type type;
    
    size = sizeof(tag_type);
    if(name != NULL) {
        buff[0] = CDT_GET_NAME;
        size = strlen(name) + 1;
        strncpy(&(buff[1]), name, size); /* Pass the cdt name in this subcommand */
        size++; /* Add one for the sub command */
    } else {
        buff[0] = CDT_GET_TYPE;  /* Put the subcommand in the first byte */
        *((u_int32_t *)&buff[1]) = mtos_udint(cdt_type); /* type in the next four */
        size = 5;
    }

    libdax_lock(ds->lock);
    result = _message_send(ds, MSG_CDT_GET, buff, size);
    
    if(result) { 
        libdax_unlock(ds->lock);
        return ERR_MSG_SEND;
    }
    
    size = MSG_DATA_SIZE;
    result = _message_recv(ds, MSG_CDT_GET, buff, &size, 1);
    if(result == 0) {
        type = stom_udint(*((tag_type *)buff));
        result = add_cdt_to_cache(ds, type, &(buff[4]));
    }
    libdax_unlock(ds->lock);
    return result;
}
