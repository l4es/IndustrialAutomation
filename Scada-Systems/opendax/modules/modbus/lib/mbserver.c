/* mbserver.c - Modbus (tm) Communications Library
 * Copyright (C) 2010 Phil Birkelbach
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 * 
 * Source file for TCP Server functionality
 */

#include <modbus.h>
#include <mblib.h>

/* These two functions are wrappers to deal with adding and deleting
   file descriptors to the global _fdset and dealing with _maxfd */
static void
_add_fd(mb_port *port, int fd)
{
    FD_SET(fd, &(port->fdset));
    if(fd > port->maxfd) port->maxfd = fd;
}

static void
_del_fd(mb_port *port, int fd)
{
    int n, tmpfd = 0;
    
    FD_CLR(fd, &(port->fdset));
    
    /* If it's the largest one then we need to re-figure _maxfd */
    if(fd == port->maxfd) {
        for(n = 0; n <= port->maxfd; n++) {
            if(FD_ISSET(n, &(port->fdset))) {
                tmpfd = n;
            }
        }
        port->maxfd = tmpfd;
    }
}

static int
_add_connection(mb_port *port, int fd)
{
    struct client_buffer *new;
    
    DEBUGMSG2("_add_connection() - Adding connection on fd %d", fd);
    new = malloc(sizeof(struct client_buffer));
    if(new == NULL) return MB_ERR_ALLOC;
    
    new->fd = fd;
    new->buffindex = 0;
    
    if(port->buff_head == NULL) {
        /* If it's the first one just put it on top */
        new->next = NULL;
    } else {
        /* Just jam the thing onto the top of the list */
        new->next = port->buff_head;
    }
    port->buff_head = new;
    _add_fd(port, fd);
    return 0;
}

/* Loops through the client buffer linked list of the port and sets
 * the index to 0, effectively delteing the buffers. */
static int
_clear_buffers(mb_port *port)
{
    struct client_buffer *this;
    
    this = port->buff_head;
    
    while(this != NULL) {
        this->buffindex = 0;
        this = this->next;
    }
    return 0;
}

static int
_del_connection(mb_port *port, int fd)
{
    
    close(fd);
    _del_fd(port, fd);
    return 0;
}

static struct client_buffer *
_get_buff_ptr(mb_port *port, int fd)
{
    struct client_buffer *this;
    
    this = port->buff_head;
    while(this != NULL) {
        if(this->fd == fd) {
            return this;
        }
        this = this->next;
    }
    return NULL;
}

static int
_mb_read(mb_port *port, int fd)
{   
    int result, size;
    unsigned char buff[100];
    struct client_buffer *cc;
    u_int16_t msgsize;
    
    size = 100;
    result = read(fd, buff, size);
    if(result < 0) {
        DEBUGMSG2("Unable to read data from socket %d", fd);
        return MB_ERR_RECV_FAIL;
    } if(result == 0) { /* EOF means the other guy is closed */
        DEBUGMSG2("Received EOF on socket %d", fd);
        return MB_ERR_NO_SOCKET;
    }
    
    cc = _get_buff_ptr(port, fd);
    assert(cc != NULL); /* If we get this far cc should exist in the port */
    
    /* Check that we haven't received too big of a message */
    if((result + cc->buffindex) > MB_BUFF_SIZE) {
        return MB_ERR_OVERFLOW;
    }
    memcpy(&(cc->buff[cc->buffindex]), buff, result); /* Copy the new data to the buffer */
    cc->buffindex += result;
    if(cc->buffindex > 5) {
        COPYWORD(&msgsize, (u_int16_t *)&cc->buff[4]); /* Get the Modbus Message size */
        if(cc->buffindex >= (msgsize + 6)) {
            if(port->in_callback) {
                port->in_callback(port, cc->buff, cc->buffindex);
            }

            result = create_response(port, &(cc->buff[6]), MB_BUFF_SIZE - 6);
            if(result > 0) { /* We have a response */
                msgsize = result;
                COPYWORD(&(cc->buff[4]), &msgsize);
                if(port->out_callback) {
                    port->out_callback(port, cc->buff, result + 6);
                }
                write(cc->fd, cc->buff, result + 6);
                cc->buffindex = 0;
            } else if(result < 0) {
                fprintf(stderr, "Error Code Returned %d\n", result);
                return result;
            }
        }
    }
    return 0;
}

/* Open a socket to listen */
static int
_server_listen(mb_port *port)
{
    struct sockaddr_in addr;
    int fd;
    
    fd = socket(AF_INET, SOCK_STREAM, 0);
    if(fd < 0) {
        return -1;
    }

    bzero(&addr, sizeof(addr));

    addr.sin_family = AF_INET;
    addr.sin_port = htons(port->bindport);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);

    if(bind(fd, (const struct sockaddr *)&addr, sizeof(addr))) {
        return -1;
    }

    if(listen(fd, 5) < 0) {
        return -1;
    }
    /* We store this fd so that we know what socket we are listening on */
    port->fd = fd;
    _add_fd(port, fd);

    return 0;
}

/* This function blocks waiting for a message to be received.  Once a message
 * is retrieved from the system the proper handling function is called */
int
_receive(mb_port *port)
{
    fd_set tmpset;
    struct timeval tm;
    struct sockaddr_in addr;
    int result, fd, n;
    socklen_t len = 0;

    FD_ZERO(&tmpset);
    FD_COPY(&(port->fdset), &tmpset);
    tm.tv_sec = 1; /* TODO: this should be configuration */
    tm.tv_usec = 0;

    result = select(port->maxfd + 1, &tmpset, NULL, NULL, &tm);

    if(result < 0) {
        /* Ignore interruption by signal */
        if(errno == EINTR) {
            ; /* TODO: check to see if we should die here */
        } else {
            /* TODO: Deal with these errors */
            DEBUGMSG2("_receive() - select error: %s", strerror(errno));
            return MB_ERR_RECV_FAIL;
        }
    } else if(result == 0) { /* Timeout */
//        DEBUGMSG("_receive() - Timeout");
        _clear_buffers(port); /* this erases all of the _buffer nodes */
        return 0;
    } else {
        for(n = 0; n <= port->maxfd; n++) {
            if(FD_ISSET(n, &tmpset)) {
                if(n == port->fd) { /* This is a listening socket */
                    fd = accept(n, (struct sockaddr *)&addr, &len);
                    if(fd < 0) {
                        /* TODO: Need to handle these communication errors */
                        DEBUGMSG2("_recieve() - Error Accepting socket: %s", strerror(errno));
                    } else {
                        DEBUGMSG2("_recieve() - Accepted socket on fd %d", n);
                        _add_connection(port, fd);
                    }
                } else {
                    result = _mb_read(port, n);
                    if(result == MB_ERR_NO_SOCKET) { /* This is the end of file */
                        DEBUGMSG2("Connection Closed for fd %d", n);
                        _del_connection(port, n);
                    } else if(result < 0) {
                        return result; /* Pass the error up */
                    }
                }
            }
        }
    }
    return 0;
}


int
server_loop(mb_port *port)
{
    int result;

    result = _server_listen(port);
    if(result) {
        DEBUGMSG2("Failed to listen on port - %s", strerror(errno));
        return result;
    } else {
        DEBUGMSG2("Listening on file descriptor %d", port->fd);
    }
    
    while(1) {
        result = _receive(port);
        if(result) {
            if(result == MB_ERR_OVERFLOW) {
                DEBUGMSG("Buffer Overflow Attempt");
            } else {
                return result;
            }
        }
    }
    return -1; /* Can never get here */
}