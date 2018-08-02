/*  ArduinoIO - A library for using the Arduino as a general purpose
 *  I/O module.
 *  Copyright (c) 2010 Phil Birkelbach
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
 *  Main header file for the Arduino IO
 */

#include <arduinoio.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <termios.h>
#include <fcntl.h>
#include <sys/select.h>
#include <errno.h>

#define MAX_MSG_SIZE 12

struct ar_network {
    int fd;
    int retries;
    int timeout;
};

static int
_getbaudrate(unsigned int b_in)
{
    switch(b_in) {
        case 300:
            return B300;
        case 600:
            return B600;
        case 1200:
            return B1200;
        case 1800:
            return B1800;
        case 2400:
            return B2400;
        case 4800:
            return B4800;
        case 9600:
            return B9600;
        case 19200:
            return B19200;
        case 38400:
            return B38400;
        case 57600:
            return B57600;
#ifdef B76800 /* Not a common baudrate */
        case 76800:
            return B76800;
#endif
        case 115200:
            return B115200;
        default:
            return 0;
    }
}

static int
_send_msg(ar_network *an, char *msg, int length, char *buff)
{
    struct timeval tval;
    fd_set fds;
    int result, tryagain = 0, try = 0, done = 0, index;
    //int n;
    
    if(length > MAX_MSG_SIZE) return -1;

    do {
        /* TODO: Delay should probably be configurable */
        usleep(10000);
        result = write(an->fd, msg, length);
        if(result < 0) {
            return ARIO_ERR_WRITE;
        }
        //fprintf(stderr, "<");
        //for(n = 0; n < length-1; n++) fprintf(stderr, "%c", msg[n]);
        //fprintf(stderr, "> ");
        index = 0;
        do {
            tryagain = 0;
            FD_ZERO(&fds);
            FD_SET(an->fd, &fds);
            tval.tv_sec = an->timeout / 1000;
            tval.tv_usec = (an->timeout % 1000) * 1000;
            result = select(an->fd + 1, &fds, NULL, NULL, &tval);
            /* This is a Timeout */
            if(result == 0) {
                if(try >= an->retries) {
                    return ARIO_ERR_TIMEOUT;
                } else {
                    try++;
                    tryagain = 1;
                    break;
                }
            } else if(result == EINTR) {
                return -1;
            } else {
                /* It's simpler just to read one character at a time */
                //fprintf(stderr, "[");
                while( (result = read(an->fd, &buff[index], 1)) != 0) {
                    /* If read() returns an error we should bail */
                    if(result < 0) return result;
                    if(buff[index] == '\r') {
                        if(buff[0] != '%') index = 0;
                        if(index > 3 && buff[3] != '&') index = 0;
                        if(index > MAX_MSG_SIZE) index = 0; 
                        if(index > 3 && buff[index] == '\r') {
                            //fprintf(stderr, "]\n");
                            return 0;
                        }
                    //} else {
                        //fprintf(stderr, "%c", buff[index]);
                    }
                    index += 1;
                }
                
            }
        } while(!done);
    } while(tryagain);

    return result;
}

static int
_get_ack(char *buff)
{
    if(buff[4] == 'O' && buff[5] == 'K') {
        return 0;
    } else {
        return (buff[5] - '0') * -1;
    }
}

static int
_get_response(char *buff)
{
    if(buff[4] == 'E') {
        return (buff[5] - '0') * -1;
    } else {
        return strtol(&buff[4], NULL, 16);
    }
}

ar_network *
ario_init(void)
{
    ar_network *an;
    an = malloc(sizeof(ar_network));
    if(an != NULL) {
        an->retries = 3;
        an->timeout = 1000;
    }
    return an;
}

void
ario_free(ar_network *an)
{
    free(an);
}


/* Open and set up the serial port */
int
ario_openport(ar_network *an, char *device, unsigned int baudrate)
{
    int fd, br;
    struct termios options;

    /* the port is opened RW and reads will not block */
    fd = open(device, O_RDWR | O_NOCTTY | O_NONBLOCK);

    if(fd == -1)  {
        return(-1);
    } else  {
        fcntl(fd, F_SETFL, 0);
        tcgetattr(fd, &options);
        /* Set the baudrate */
        br = _getbaudrate(baudrate);
        if(br == 0) return -1;
        cfsetispeed(&options, br);
        cfsetospeed(&options, br);
        options.c_cflag |= (CLOCAL | CREAD);
        /* Set the parity to NONE*/
        options.c_cflag &= ~PARENB;
        options.c_cflag &= ~CSTOPB;
        /* Set databits */
        options.c_cflag &= ~CSIZE;
        options.c_cflag |= CS8;    
        options.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
        options.c_iflag &= ~(IXON | IXOFF | IXANY | ICRNL);
        options.c_oflag &= ~OPOST;
        options.c_cc[VMIN] = 0;
        options.c_cc[VTIME] = 0;
        /* TODO: Should check for errors here */
        tcsetattr(fd, TCSANOW, &options);
    } 
    an->fd = fd;
    return fd;
}

int
ario_closeport(ar_network *an)
{
    return close(an->fd);
}

int
ario_set_retries(ar_network *an, int retries)
{
    an->retries = retries;
    return 0;
}

int
ario_set_timeout(ar_network *an, int timeout)
{
    an->timeout = timeout;
    return 0;
}

int
ario_pin_mode(ar_network *an, char *address, int pin, int mode)
{
    int result, next;
    char buff[MAX_MSG_SIZE];
    char msg[MAX_MSG_SIZE];
    //"$AA&CD3I\r"
    msg[0] = '$';
    msg[1] = address[0];
    msg[2] = address[1];
    msg[3] = '&';
    msg[4] = 'C';
    msg[5] = 'D';
    if(pin > 9) {
        msg[6] = (pin / 10) + '0';
        msg[7] = (pin % 10) + '0';
        next = 8;
    } else {
        msg[6] = pin + '0';
        next = 7;
    }
    if(mode == ARIO_PIN_DI) {
        msg[next++] = 'I';
    } else if(mode == ARIO_PIN_DO || mode == ARIO_PIN_PWM) {
        msg[next++] = 'O';
    } else {
        return ARIO_ERR_ARGUMENT;
    }
    msg[next] = '\r';
    result = _send_msg(an, msg, next + 1, buff);
    if(result < 0) return result;
    if(buff[1] != address[0] || buff[2] != address[1]) return ARIO_ERR_ADDRESS;
    result = _get_ack(buff);
    return result;
}

int
ario_pin_pullup(ar_network *an, char *address, int pin, int pullup)
{
    int result, next;
    char buff[MAX_MSG_SIZE];
    char msg[MAX_MSG_SIZE];
    //"$AA&CD3R1\r"
    msg[0] = '$';
    msg[1] = address[0];
    msg[2] = address[1];
    msg[3] = '&';
    msg[4] = 'C';
    msg[5] = 'D';
    if(pin > 9) {
        msg[6] = (pin / 10) + '0';
        msg[7] = (pin % 10) + '0';
        next = 8;
    } else {
        msg[6] = pin + '0';
        next = 7;
    }
    msg[next++] = 'R';
    if(pullup) {
        msg[next++] = '1';
    } else {
        msg[next++] = '0';
    }
    msg[next] = '\r';
    result = _send_msg(an, msg, next + 1, buff);
    if(result < 0) return result;
    if(buff[1] != address[0] || buff[2] != address[1]) return ARIO_ERR_ADDRESS;
    result = _get_ack(buff);
    return result;
}

int
ario_ai_enable(ar_network *an, char *address, int ai)
{
    return 0;
}

int
ario_ai_ref(ar_network *an, char *address, int ai, int ref)
{
    return 0;
}

int
ario_pin_read(ar_network *an, char *address, int pin)
{
    int result, next;
    char buff[MAX_MSG_SIZE];
    char msg[MAX_MSG_SIZE];
    //"$AA&RD3\r"
    msg[0] = '$';
    msg[1] = address[0];
    msg[2] = address[1];
    msg[3] = '&';
    msg[4] = 'R';
    msg[5] = 'D';
    if(pin > 9) {
        msg[6] = (pin / 10) + '0';
        msg[7] = (pin % 10) + '0';
        next = 8;
    } else {
        msg[6] = pin + '0';
        next = 7;
    }
    msg[next] = '\r';
    result = _send_msg(an, msg, next + 1, buff);
    if(result < 0) return result;
    if(buff[1] != address[0] || buff[2] != address[1]) return ARIO_ERR_ADDRESS;
    result = _get_response(buff);
    return result;
}

int
ario_pin_write(ar_network *an, char *address, int pin, int val)
{
    int result, next;
    char buff[MAX_MSG_SIZE];
    char msg[MAX_MSG_SIZE];
    //"$AA&WD3,1\r"
    msg[0] = '$';
    msg[1] = address[0];
    msg[2] = address[1];
    msg[3] = '&';
    msg[4] = 'W';
    msg[5] = 'D';
    if(pin > 9) {
        msg[6] = (pin / 10) + '0';
        msg[7] = (pin % 10) + '0';
        next = 8;
    } else {
        msg[6] = pin + '0';
        next = 7;
    }
    msg[next++] = ',';
    if(val) {
        msg[next++] = '1';
    } else {
        msg[next++] = '0';
    }
    msg[next] = '\r';
    result = _send_msg(an, msg, next+1, buff);
    if(result < 0) return result;
    if(buff[1] != address[0] || buff[2] != address[1]) return ARIO_ERR_ADDRESS;
    result = _get_ack(buff);
    return result;
}

int
ario_pin_pwm(ar_network *an, char *address, int pin, int val)
{
    return 0;
}

int
ario_ai_read(ar_network *an, char *address, int ai)
{
    return 0;
}
