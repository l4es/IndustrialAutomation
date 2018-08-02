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

 * This file contains the messaging code for the OpenDAX core
 */

/* TODO: Remove all normal C datatypes from the messages.  These could change
 * size on different architectures.  Should use the dax_* datatypes or the
 * _t datatypes instead */

#include <message.h>
#include <func.h>
#include <module.h>
#include <tagbase.h>
#include <options.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/un.h>
#include <string.h>

#define ASYNC 0
#define RESPONSE 1
#define ERROR 2

#ifndef FD_COPY
# define FD_COPY(x,y) memcpy((y),(x),sizeof(fd_set))
#endif

/* These are the listening sockets for the local UNIX domain
 *  socket and the remote TCP socekt. */
static fd_set _listenfdset;
/* This is the set of all the sockets, both listening and conencted
 * it is used in the select() call in msg_receive() */
static fd_set _fdset;
static int _maxfd;

/* This array holds the functions for each message command */
#define NUM_COMMANDS 16
int (*cmd_arr[NUM_COMMANDS])(dax_message *) = {NULL};

/* Macro to check whether or not the command 'x' is valid */
#define CHECK_COMMAND(x) (((x) < 0 || (x) > (NUM_COMMANDS-1)) ? 1 : 0)

int msg_mod_register(dax_message *msg);
int msg_tag_add(dax_message *msg);
int msg_tag_del(dax_message *msg);
int msg_tag_get(dax_message *msg);
int msg_tag_list(dax_message *msg);
int msg_tag_read(dax_message *msg);
int msg_tag_write(dax_message *msg);
int msg_tag_mask_write(dax_message *msg);
int msg_mod_get(dax_message *msg);
int msg_mod_set(dax_message *msg);
int msg_evnt_add(dax_message *msg);
int msg_evnt_del(dax_message *msg);
int msg_evnt_get(dax_message *msg);
int msg_evnt_mod(dax_message *msg);
int msg_cdt_create(dax_message *msg);
int msg_cdt_get(dax_message *msg);


/* Generic message sending function.  If response is MSG_ERROR then it is assumed that 
 * an error is being sent to the module.  In that case payload should point to a 
 * single int that indicates the error */
static int
_message_send(int fd, int command, void *payload, size_t size, int response)
{
    int result;
    char buff[DAX_MSGMAX];
    
    ((u_int32_t *)buff)[0] = htonl(size + MSG_HDR_SIZE);
    if(response == RESPONSE) {
        ((u_int32_t *)buff)[1] = htonl(command | MSG_RESPONSE);
    } else if(response == ERROR) {
        xlog(LOG_MSGERR, "Returning Error %d to Module", *(int *)payload);
        ((u_int32_t *)buff)[1] = htonl(command | MSG_ERROR);
    } else {
        ((u_int32_t *)buff)[1] = htonl(command);         
    }
    /* Bounds check so we don't seg fault */
    if(size > (DAX_MSGMAX - MSG_HDR_SIZE)) {
        return ERR_2BIG;
    }
    memcpy(&buff[MSG_HDR_SIZE], payload, size);
    result = xwrite(fd, buff, size + MSG_HDR_SIZE);
    if(result < 0) {
        xerror("_message_send: %s", strerror(errno));
        return ERR_MSG_SEND;
    }
    return 0;    
}

/* Sets up the local UNIX domain socket for listening. */
static int
_msg_setup_local_socket(void)
{
    struct sockaddr_un addr;
    int fd;
    
    fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if(fd < 0) {
        xfatal("Unable to create local socket - %s", strerror(errno));
    }
    
    bzero(&addr, sizeof(addr));
    strncpy(addr.sun_path, opt_socketname(), sizeof(addr.sun_path));
    addr.sun_family = AF_LOCAL;
    unlink(addr.sun_path); /* Delete the socket if it exists on the filesystem */
    if(bind(fd, (const struct sockaddr *)&addr, sizeof(addr))) {
        xfatal("Unable to bind to local socket: %s", addr.sun_path);
    }
    
    if(listen(fd, 5) < 0) {
        xfatal("Unable to listen for some reason");
    }
    FD_SET(fd, &_listenfdset);
    msg_add_fd(fd);
    
    xlog(LOG_COMM, "Listening on local socket - %d", fd);
    return 0;
}

/* Sets up the remote TCP socket for listening. The arguments
 * are the ipaddress and the port to listen on.  They should be
 * in host order (will be converted within this function) */
static int
_msg_setup_remote_socket(in_addr_t ipaddress, in_port_t ipport)
{
    struct sockaddr_in addr;
    int fd;
    
    fd = socket(AF_INET, SOCK_STREAM, 0);
    if(fd < 0) {
        xfatal("Unable to create remote socket - %s", strerror(errno));
    }
    
    bzero(&addr, sizeof(addr));
    
    /* TODO: Change this to the configuration stuff */
    addr.sin_family = AF_INET;
    addr.sin_port = htons(ipport);
    addr.sin_addr.s_addr = htonl(ipaddress);
    
    if(bind(fd, (const struct sockaddr *)&addr, sizeof(addr))) {
        xfatal("Unable to bind remote socket - %s", strerror(errno));
    }
    
    if(listen(fd, 5) < 0) {
        xfatal("Unable to listen on remote socket - %s", strerror(errno));
    }
    /* set the _remotefd set here.  This gives us a simpler way to
     * determine if the fd that we get from the select() call in
     * msg_receive() is a listening socket or not */
    /* TODO: Does this need to be an fd_set?? */
    FD_SET(fd, &_listenfdset);
    msg_add_fd(fd);
    
    xlog(LOG_COMM, "Listening on remote socket - %d", fd);
    return 0;    
}

/* Creates and sets up the local message socket for the program.
   It's a fatal error if we cannot create the local socket.  The
   remote socket creation is allowed to fail without exiting.  This
   also sets up the array of command function pointers */
int
msg_setup(void)
{
    _maxfd = 0;
    FD_ZERO(&_fdset);
    FD_ZERO(&_listenfdset);
    
    /* TODO: These should be called based on configuration options
     * for now we'll just listen on the local domain socket and bind
     * to all interfaces on the default port. */
    _msg_setup_local_socket();
    _msg_setup_remote_socket(INADDR_ANY, DEFAULT_PORT);
    
    buff_initialize(); /* This initializes the communications buffers */
    
    /* The functions are added to an array of function pointers with their
        messsage type used as the index.  This makes it really easy to call
        the handler functions from the messaging thread. */
    cmd_arr[MSG_MOD_REG]    = &msg_mod_register;
    cmd_arr[MSG_TAG_ADD]    = &msg_tag_add;
    cmd_arr[MSG_TAG_DEL]    = &msg_tag_del;
    cmd_arr[MSG_TAG_GET]    = &msg_tag_get;
    cmd_arr[MSG_TAG_LIST]   = &msg_tag_list;
    cmd_arr[MSG_TAG_READ]   = &msg_tag_read;
    cmd_arr[MSG_TAG_WRITE]  = &msg_tag_write;
    cmd_arr[MSG_TAG_MWRITE] = &msg_tag_mask_write;
    cmd_arr[MSG_MOD_GET]    = &msg_mod_get;
    cmd_arr[MSG_MOD_SET]    = &msg_mod_set;
    cmd_arr[MSG_EVNT_ADD]   = &msg_evnt_add;
    cmd_arr[MSG_EVNT_DEL]   = &msg_evnt_del;
    cmd_arr[MSG_EVNT_GET]   = &msg_evnt_get;
    cmd_arr[MSG_EVNT_MOD]   = &msg_evnt_mod;
    cmd_arr[MSG_CDT_CREATE] = &msg_cdt_create;
    cmd_arr[MSG_CDT_GET]    = &msg_cdt_get;
    
    return 0;
}

/* This destroys the queue if it was created */
void
msg_destroy(void)
{
    unlink(opt_socketname());
    xlog(LOG_MAJOR, "Resources being destroyed");
}

/* These two functions are wrappers to deal with adding and deleting
   file descriptors to the global _fdset and dealing with _maxfd */
void
msg_add_fd(int fd)
{
    FD_SET(fd, &_fdset);
    if(fd > _maxfd) _maxfd = fd;
}

void
msg_del_fd(int fd)
{
    int n, tmpfd = 0;
    
    FD_CLR(fd, &_fdset);
    
    /* If it's the largest one then we need to refigure _maxfd */
    if(fd == _maxfd) {
        for(n = 0; n <= _maxfd; n++) {
            if(FD_ISSET(n, &_fdset)) {
                tmpfd = n;
            }
        }
        _maxfd = tmpfd;
    }
    close(fd); /* Just to make sure */
    buff_free(fd);
}

/* This function blocks waiting for a message to be received.  Once a message
 * is retrieved from the system the proper handling function is called */
int
msg_receive(void)
{
    fd_set tmpset;
    struct timeval tm;
    struct sockaddr_un addr;
    int result, fd, n;
    socklen_t len = 0;
    
    FD_ZERO(&tmpset);
    FD_COPY(&_fdset, &tmpset);
    tm.tv_sec = 1; /* TODO: this should be configuration */
    tm.tv_usec = 0;
    
    result = select(_maxfd + 1, &tmpset, NULL, NULL, &tm);
    
    if(result < 0) {
        /* Ignore interruption by signal */
        if(errno != EINTR) {
            /* TODO: Deal with these errors */
            xerror("msg_receive select error: %s", strerror(errno));
            return ERR_MSG_RECV;
        }
    } else if(result == 0) { /* Timeout */
        buff_freeall(); /* this erases all of the _buffer nodes */
        return 0;
    } else {
        for(n = 0; n <= _maxfd; n++) {
            if(FD_ISSET(n, &tmpset)) {
                if(FD_ISSET(n, &_listenfdset)) { /* This is a listening socket */
                    fd = accept(n, (struct sockaddr *)&addr, &len);
                    if(fd < 0) {
                        /* TODO: Need to handle these errors */
                        xerror("Error Accepting socket: %s", strerror(errno));
                    } else {
                        xlog(LOG_COMM, "Accepted socket on fd %d", n);
                        msg_add_fd(fd);
                    }
                } else {
                    result = buff_read(n);
                    if(result == ERR_NO_SOCKET) { /* This is the end of file */
                        /* TODO: unregister module?? */
                        xlog(LOG_COMM, "Connection Closed for fd %d", n);
                        msg_del_fd(n);
                    } else if(result < 0) {
                        return result; /* Pass the error up */
                    }
                }
            }
        }
    }
    return 0;
}

/* This handles each message.  It extracts out the dax_message structure
 * and then calls the proper message handling function.  This message will
 * unmarshal the header but it is up to the individual wrapper function to
 * unmarshal the data portion of the message if need be. */
int
msg_dispatcher(int fd, unsigned char *buff)
{
    dax_message message;
    
    /* The first four bytes are the size and the size is always
     * sent in network order */
    message.size = ntohl(*(u_int32_t *)buff) - MSG_HDR_SIZE;
    /* The next four bytes are the DAX command also sent in network
     * byte order. */
    message.command = ntohl(*(u_int32_t *)&buff[4]);
    //--printf("We've received message : command = %d, size = %d\n", message.command, message.size);
    
    if(CHECK_COMMAND(message.command)) return ERR_MSG_BAD;
    message.fd = fd;
    memcpy(message.data, &buff[8], message.size);
    buff_free(fd);
    /* Now call the function to deal with it */
    return (*cmd_arr[message.command])(&message);
}

/* The rest of the functions in this file are wrappers for other functions
 * in the server.  These each correspond to a messagre command.  They are
 * called from the cmd_arr with the command number used as the index.
 */

/* This message is the module registration/unregistration function.  This
 * tells the server which module is on which socket fd and the modules name.
 * When this message is called with a zero size it's an unregister
 * message.  Otherwise the first four bytes are the PID of the calling module
 * and then the rest is the module name. */
int
msg_mod_register(dax_message *msg)
{
    pid_t pid;
    int flags, result;
    char buff[DAX_MSGMAX];
    dax_module *mod;
    
    if(msg->size > 0) {
        pid = ntohl(*((u_int32_t *)&msg->data[0]));
        flags = ntohl(*((u_int32_t *)&msg->data[4]));
        
        /* Is this the initial registration of the synchronous socket */
        if(flags & CONNECT_SYNC) {
            xlog(LOG_MSG, "Registering Module %s fd = %d", &msg->data[8], msg->fd);
            /* TODO: Need to check for errors there */
            mod = module_register(&msg->data[MSG_HDR_SIZE], pid, msg->fd);
            if(!mod) {
                result = ERR_NOTFOUND;
                _message_send(msg->fd, MSG_MOD_REG, &result, sizeof(result) , ERROR);
                return result;
            } else {
                /* This puts the test data into the buffer for sending. */
                *((u_int16_t *)&buff[0]) = REG_TEST_INT;    /* 16 bit test data */
                *((u_int32_t *)&buff[2]) = REG_TEST_DINT;   /* 32 bit integer test data */
                *((u_int64_t *)&buff[6]) = REG_TEST_LINT;   /* 64 bit integer test data */
                *((float *)&buff[14])    = REG_TEST_REAL;   /* 32 bit float test data */
                *((double *)&buff[18])   = REG_TEST_LREAL;  /* 64 bit float test data */
                strncpy(&buff[26], mod->name, DAX_MSGMAX - 26 - 1);

                _message_send(msg->fd, MSG_MOD_REG, buff, 26 + strlen(mod->name) + 1, RESPONSE);
            }
        /* Is this the asynchronous event socket registration */
        } else if(flags & CONNECT_EVENT) {
            xlog(LOG_MSG, "Event Socket Registration for Module %s fd = %d", &msg->data[8], msg->fd);
            mod = event_register(pid, msg->fd);
            result = ERR_NOTFOUND;
            if(!mod) {
                _message_send(msg->fd, MSG_MOD_REG, &result, sizeof(result) , ERROR);
            } else {
                _message_send(msg->fd, MSG_MOD_REG, NULL, 0, RESPONSE);    
            }
        } else { /* If the flags are bad send error */
            result = ERR_MSG_BAD;
            _message_send(msg->fd, MSG_MOD_REG, &result, sizeof(result) , ERROR);
        }
    } else {
        xlog(4, "Unregistering Module fd = %d", msg->fd);
        module_unregister(msg->fd);
        _message_send(msg->fd, MSG_MOD_REG, buff, 0, 1);
    }
    return 0;
}

/* Message wrapper function for adding a tag.  The first four bytes are
 * the type of tag, the next four bytes are the count and the remainder
 * of the message is the tag name */
int
msg_tag_add(dax_message *msg)
{
    tag_index idx;
    u_int32_t type;
    u_int32_t count;
        
    type = *((u_int32_t *)&msg->data[0]);
    count = *((u_int32_t *)&msg->data[4]);
    
    xlog(LOG_MSG | LOG_VERBOSE, "Tag Add Message for '%s' from module %d, type 0x%X, count %d", &msg->data[8], msg->fd, type, count);
    
    idx = tag_add(&msg->data[8], type, count);
    
    if(idx >= 0) {
        _message_send(msg->fd, MSG_TAG_ADD, &idx, sizeof(tag_index), RESPONSE);
    } else {
        _message_send(msg->fd, MSG_TAG_ADD, &idx, sizeof(tag_index), ERROR);
    }
    return 0;
}

/* TODO: Make this function do something */
int
msg_tag_del(dax_message *msg)
{
    return 0;
}



int
msg_tag_get(dax_message *msg)
{
    int result, index, size;
    dax_tag tag;
    char *buff;
    
    if(msg->data[0] == TAG_GET_INDEX) { /* Is it a string or index */
        index = *((tag_index *)&msg->data[1]); /* cast void * -> handle_t * then indirect */
        result = tag_get_index(index, &tag); /* get the tag */
        xlog(LOG_MSG | LOG_VERBOSE, "Tag Get Message from %d for index 0x%X", msg->fd, index);
    } else { /* A name was passed */
        ((char *)msg->data)[DAX_TAGNAME_SIZE + 1] = 0x00; /* Add a NULL to avoid trouble */
        /* Get the tag by it's name */
        result = tag_get_name((char *)&msg->data[1], &tag);
        xlog(LOG_MSG | LOG_VERBOSE, "Tag Get Message from %d for name '%s'", msg->fd, (char *)msg->data);
    }
    size = strlen(tag.name) + 13;
    buff = alloca(size);
    if(buff == NULL) result = ERR_ALLOC;
    if(!result) {
        *((u_int32_t *)&buff[0]) = tag.idx;
        *((u_int32_t *)&buff[4]) = tag.type;
        *((u_int32_t *)&buff[8]) = tag.count;
        strcpy(&buff[12], tag.name);
        _message_send(msg->fd, MSG_TAG_GET, buff, size, RESPONSE);
        xlog(LOG_MSG | LOG_VERBOSE, "Returning tag - '%s':0x%X to module %d",tag.name, tag.idx, msg->fd);
    } else {
        _message_send(msg->fd, MSG_TAG_GET, &result, sizeof(result), ERROR);
        xlog(LOG_MSG, "Bad tag query for MSG_TAG_GET");
    }
    return 0;
}

int
msg_tag_list(dax_message *msg)
{
    xlog(LOG_MSG | LOG_VERBOSE, "Tag List Message from %d", msg->fd);
    return 0;
}

/* The first part of the payload of the message is the handle
 * of the tag that we want to read and the next part is the size
 * of the buffer that we want to read */
int
msg_tag_read(dax_message *msg)
{
    char data[MSG_DATA_SIZE];
    tag_index index;
    int result, offset;
    int size;
     
    index = *((tag_index *)&msg->data[0]);
    offset = *((int *)&msg->data[4]);
    size = *((int *)&msg->data[8]);
    
    xlog(LOG_MSG | LOG_VERBOSE, "Tag Read Message from module %d, index %d, offset %d, size %d", msg->fd, index, offset, size);
    
    result = tag_read(index, offset, &data, size);
    if(result) {
        _message_send(msg->fd, MSG_TAG_READ, &result, sizeof(result), ERROR);
    } else {
        _message_send(msg->fd, MSG_TAG_READ, &data, size, RESPONSE);
    }
    return 0;
}

/* Generic write message */
int
msg_tag_write(dax_message *msg)
{
    tag_index handle;
    int result, offset;
    void *data;
    size_t size;
    	
    size = msg->size - sizeof(tag_index) - sizeof(int);
    handle = *((tag_index *)&msg->data[0]);
    offset = *((int *)&msg->data[4]);
    data = &msg->data[8];

    xlog(LOG_MSG | LOG_VERBOSE, "Tag Write Message from module %d, index %d, offset %d, size %d", msg->fd, handle, offset, size);

    result = tag_write(handle, offset, data, size);
    if(result) {
        _message_send(msg->fd, MSG_TAG_WRITE, &result, sizeof(result), ERROR);
        xlog(LOG_ERROR, "Unable to write tag 0x%X with size %d",handle, size);
    } else {
        _message_send(msg->fd, MSG_TAG_WRITE, NULL, 0, RESPONSE);
    }    
    return 0;
}

/* Generic write with bit mask */
int
msg_tag_mask_write(dax_message *msg)
{
    tag_index handle;
    int result, offset;
    void *data, *mask;
    size_t size;
    
    size = (msg->size - sizeof(tag_index) - sizeof(int)) / 2;
    handle = *((tag_index *)&msg->data[0]);
    offset = *((int *)&msg->data[4]);
    data = &msg->data[8];
    mask = &msg->data[8 + size];
    
    xlog(LOG_MSG | LOG_VERBOSE, "Tag Masked Write Message from module %d, index %d, offset %d, size %d", msg->fd, handle, offset, size);
    
    result = tag_mask_write(handle, offset, data, mask, size);
    if(result) {
        _message_send(msg->fd, MSG_TAG_MWRITE, &result, sizeof(result), ERROR);
        xerror("Unable to write tag 0x%X with size %d: result %d", handle, size, result);
    } else {
        _message_send(msg->fd, MSG_TAG_MWRITE, NULL, 0, RESPONSE);
    }    
    return 0;
}


int
msg_mod_get(dax_message *msg)
{
    xlog(LOG_MSG | LOG_VERBOSE, "Get Module Parameter Message from %d", msg->fd);
    return 0;
}

int
msg_mod_set(dax_message *msg)
{
    int result;
    u_int8_t cmd;

    xlog(LOG_MSG | LOG_VERBOSE, "Set Module Parameter Message from %d", msg->fd);

    cmd = msg->data[0];

    if(cmd == MOD_CMD_RUNNING) {
        xlog(LOG_MSG | LOG_VERBOSE, "Set Module Running Flag from %d", msg->fd);
        /* Set the flag */
        result = module_set_running(msg->fd);;
    } else {
        result = ERR_ARG;
    }

    if(result) {
        _message_send(msg->fd, MSG_MOD_SET, &result, sizeof(result), ERROR);
        xerror("Stupid Error - %d", result);
    } else {
        _message_send(msg->fd, MSG_MOD_SET, NULL, 0, RESPONSE);
    }
    return 0;
}

int
msg_evnt_add(dax_message *msg)
{
    Handle h;
    void *data = NULL;
    dax_dint event_type;
    dax_module *module;
    dax_dint event_id = -1;
    
    
    module = module_find_fd(msg->fd);
    if( module != NULL ) {
        memcpy(&h.index, msg->data, 4);
        memcpy(&h.byte, &msg->data[4], 4);
        memcpy(&h.count, &msg->data[8], 4);
        memcpy(&h.type, &msg->data[12], 4);
        memcpy(&event_type, &msg->data[16], 4);
        memcpy(&h.size, &msg->data[20], 4);
        h.bit = msg->data[24];
        data = (void *)&msg->data[25];
//        fprintf(stderr, "Event Handle Index = %d\n",h.index);
//        fprintf(stderr, "Event Handle Count = %d\n",h.count);
//        fprintf(stderr, "Event Handle Datatype = 0x%X\n",h.type);
//        fprintf(stderr, "Event Handle Size = %d\n",h.size);
//        fprintf(stderr, "Event Handle Byte = %d\n",h.byte);
//        fprintf(stderr, "Event Handle Bit = %d\n",h.bit);
//        fprintf(stderr, "Event Type = %d\n", event_type);
        xlog(LOG_MSG | LOG_VERBOSE, "Add Event Message from %d - Index = %d, Count = %d, Type = %d", msg->fd, h.index, h.count, event_type);
        event_id = event_add(h, event_type, data, module);
    }
    
    if(event_id < 0) { /* Send Error */
        _message_send(msg->fd, MSG_EVNT_ADD, &event_id, 0, ERROR);    
    } else {
        _message_send(msg->fd, MSG_EVNT_ADD, &event_id, sizeof(dax_dint), RESPONSE);
    }
    return 0;
}

int
msg_evnt_del(dax_message *msg)
{
    tag_index idx;
    u_int32_t id;
    int result;
    dax_module *module;
       
    idx = *((u_int32_t *)&msg->data[0]);
    id = *((u_int32_t *)&msg->data[4]);
    module = module_find_fd(msg->fd);

    xlog(LOG_MSG | LOG_VERBOSE, "Event Delete Message from %d", msg->fd);
      
    result = event_del(idx, id, module);
    
    if(idx >= 0) {
        _message_send(msg->fd, MSG_EVNT_DEL, &idx, 8, RESPONSE);
    } else {
        _message_send(msg->fd, MSG_EVNT_DEL, &result, sizeof(result), ERROR);
    }
    return 0;
}

int
msg_evnt_get(dax_message *msg)
{
    return 0;
}

int
msg_evnt_mod(dax_message *msg)
{
    return 0;
}


int
msg_cdt_create(dax_message *msg)
{
    int result;
    tag_type type;
    
    msg->data[MSG_DATA_SIZE-1] = '\0'; /* Just to be safe */
    type = cdt_create(msg->data, &result);
    xlog(LOG_MSG | LOG_VERBOSE, "Create CDT message name = '%s' type = 0x%X", msg->data, type);
    
    if(result < 0) { /* Send Error */
        _message_send(msg->fd, MSG_CDT_CREATE, &result, sizeof(int), ERROR);    
    } else {
        _message_send(msg->fd, MSG_CDT_CREATE, &type, sizeof(tag_type), RESPONSE);
    }
    return 0;
}


int
msg_cdt_get(dax_message *msg)
{
    int result, size;
    unsigned char subcommand;
    tag_type cdt_type;
    char type[DAX_TAGNAME_SIZE + 1];
    char *str = NULL, *data = NULL;
    
    result = 0;  /* result will stay zero if we don't have any errors */
    subcommand = msg->data[0];
    if(subcommand == CDT_GET_TYPE) {
        cdt_type = *((tag_type *)&msg->data[1]);
        xlog(LOG_MSG | LOG_VERBOSE, "CDT Get for type 0x%X Message from Module %d", cdt_type, msg->fd);
    } else {
        strncpy(type, &msg->data[1], DAX_TAGNAME_SIZE);
        type[DAX_TAGNAME_SIZE] = '\0';  /* Just to be sure */
        cdt_type = cdt_get_type(type);
        xlog(LOG_MSG | LOG_VERBOSE, "CDT Get for type %s Message from Module %d", type, msg->fd);
    }
    
    size = serialize_datatype(cdt_type, &str);
    
    /* TODO: !! We need to figure out how to deal with a string that is longer than one
     * message can hold.  For now we are going to return error. */
    if(size > (int)(MSG_DATA_SIZE - 4)) {
        result = ERR_2BIG;
    } else if(size < 0) {
        result = size;
    } else {
        data = malloc(size + 4);
        if(data == NULL) {
            result = ERR_ALLOC;
        } else {
            *((tag_type *)data) = cdt_type;
            memcpy(&(data[4]), str, size);
        }
    }
    if(result) {
        _message_send(msg->fd, MSG_CDT_GET, &result, sizeof(int), ERROR);
    } else {
        _message_send(msg->fd, MSG_CDT_GET, data, size + 4, RESPONSE);
    }
    
    if(data) free(data);
    if(str) free(str); /* Allocated by serialize_datatype() */
    return 0;
}
