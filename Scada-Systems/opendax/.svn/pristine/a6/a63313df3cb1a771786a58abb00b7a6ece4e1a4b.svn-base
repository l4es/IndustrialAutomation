/* modlib.h - Modbus (tm) Communications Library
 * Copyright (C) 2009 Phil Birkelbach
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
 * This is the private header file for the modbus library functions
 */
 
#ifndef __MODLIB_H
#define __MODLIB_H

/* Are we using config.h */
#ifdef HAVE_CONFIG_H
 #include <config.h>
#endif

#include <stdio.h>
#include <stdlib.h>

#ifdef HAVE_SYS_SOCKET_H
 #include <sys/socket.h>
#endif
#ifdef HAVE_SYS_PARAM_H
 #include <sys/param.h>
#endif
#ifdef HAVE_STRING_H
 #include <string.h>
#endif
#ifdef HAVE_STRINGS_H
 #include <strings.h>
#endif
#ifdef HAVE_SYS_SELECT_H
 #include <sys/select.h>
 #ifndef FD_COPY
  #define FD_COPY(f, t) (void)(*(t) = *(f))
 #endif
#endif

#include <unistd.h>
#include <termios.h>
#include <errno.h>
#include <assert.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/uio.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <pthread.h>

#include <modbus.h>

#ifdef __BIG_ENDIAN__ /* PPC Endianness */
# define __MB_BIG_ENDIAN
#endif

#ifdef __BYTE_ORDER /* x86 Endianness */
# if __BYTE_ORDER == __BIG_ENDIAN
#   define __MB_BIG_ENDIAN
# endif
#endif

/* Used to insert a 16bit value into the modbus buffer 
 another endian machine would simply use memcpy() */
#ifndef __MB_BIG_ENDIAN
# define COPYWORD(OUT,IN) swab((const void *)(IN),(void *)(OUT),(ssize_t)2)
#else
# define COPYWORD(OUT,IN) memcpy((OUT),(IN),2);
#endif

/* This defines whether or not we are going to build a
 * thread safe system.  If the user is not going to need
 * thread safety this would be undefined and then not
 * compiled into the library */
#define __MB_THREAD_SAFE 1
#ifdef __MB_THREAD_SAFE
#  define mb_mutex_init(mutex) __mb_mutex_init((mutex))
#  define mb_mutex_lock(port,mutex) __mb_mutex_lock((port),(mutex))
#  define mb_mutex_unlock(port,mutex) __mb_mutex_unlock((port),(mutex))

typedef pthread_mutex_t _mb_mutex_t;

/* These are defined in mbutil.c */
int __mb_mutex_init(_mb_mutex_t *);
int __mb_mutex_lock(mb_port *, _mb_mutex_t *);
int __mb_mutex_unlock(mb_port *, _mb_mutex_t *);

#else
#  define mb_mutex_init(port)
#  define mb_mutex_lock(port,mutex)
#  define mb_mutex_unlock(port,mutex)
#endif   

#define MB_FRAME_LEN 255

#ifndef MAX_RETRIES
#  define MAX_RETRIES 100
#endif

/* Device Port Types */
#define MB_SERIAL  0
#define MB_NETWORK 1

/* Maximum size of the receive buffer */
#define MB_BUFF_SIZE 150

/* This is used in the port for client connections for the TCP Server */
struct client_buffer {
	int fd;                /* File descriptor of the socket */
	int buffindex;         /* index where the next character will be placed */
	unsigned char buff[MB_BUFF_SIZE];   /* data buffer */
	struct client_buffer *next;
};

/* Internal struct that defines a single Modbus(tm) Port */
struct mb_port {
    char *name;               /* Port name if needed : Maybe we don't need this */
    unsigned int flags;       /* Port Attribute Flags */
    char *device;             /* device filename of the serial port */
    unsigned char enable;     /* 0=Pause, 1=Run */
    unsigned char type;       /* 0=Master, 1=Slave */
    unsigned char devtype;    /* 0=serial, 1=network */
    unsigned char protocol;   /* [Only RTU is implemented so far] */
    u_int8_t slaveid;         /* Slave ID 1-247 (Slave Only) */
    int baudrate;
    short databits;
    short stopbits;
    short parity;             /* 0=NONE, 1=EVEN OR 2=ODD */
    char ipaddress[16];
    unsigned int bindport;    /* IP port to bind to */
    unsigned char socket;     /* either UDP_SOCK or TCP_SOCK */
    
    int delay;       /* Intercommand delay */
    int frame;       /* Interbyte timeout */
    int retries;     /* Number of retries to try */
    int scanrate;    /* Scanrate in mSeconds */
    int timeout;     /* Response timeout */
    int maxattempts; /* Number of failed attempts to allow before closing and exiting the port */
    
    u_int16_t *holdreg;       /* database index for holding registers (slave only) */
    unsigned int holdsize;    /* size of the internal holding register bank */
    u_int16_t *inputreg;      /* database index for input registers (slave only) */
    unsigned int inputsize;   /* size of the internal input register bank */
    u_int16_t *coilreg;       /* database index for coils (slave only) */
    unsigned int coilsize;    /* size of the internal bank of coils in 16-bit registers */
    u_int16_t *discreg;       /* discrete input register */
    unsigned int discsize;    /* size of the internal bank of coils */
#ifdef __MB_THREAD_SAFE
    _mb_mutex_t hold_mutex;  /* mutexes used to lock the above register areas when needed */
    _mb_mutex_t input_mutex;
    _mb_mutex_t coil_mutex;
    _mb_mutex_t disc_mutex;
    _mb_mutex_t ctrl_mutex;  /* used to lock control bits */
#endif    
    fd_set fdset;
    int maxfd;
    struct client_buffer *buff_head; /* Head of a linked list of client connection buffers */
    
    struct mb_cmd *commands;  /* Linked list of Modbus commands */
    int fd;                   /* File descriptor to the port */
    int ctrl_flags;
    int dienow;
    unsigned int attempt;        /* Attempt counter */
    unsigned char running;       /* Flag to indicate the port is running */
    unsigned char inhibit;       /* When set the port will not be started */
    unsigned int inhibit_time;   /* Number of seconds before the port will be retried */
    unsigned int inhibit_temp;
    void *userdata;
    /* These are callback function pointers for the port message data */
    void (*out_callback)(struct mb_port *port, u_int8_t *buff, unsigned int);
    void (*in_callback)(struct mb_port *port, u_int8_t *buff, unsigned int);
    void (*slave_read)(struct mb_port *port, int reg, int index, int size, void *userdata);
    void (*slave_write)(struct mb_port *port, int reg, int index, int size, void *userdata);
    void (*userdata_free)(struct mb_port *port, void *userdata);
};

struct mb_cmd {
    unsigned char enable;    /* 0=disable, 1=enable */
    unsigned char mode;      /* MB_CONTINUOUS, MB_ONCHANGE */
    u_int8_t node;           /* Modbus device ID */
    u_int8_t function;       /* Function Code */
    u_int16_t m_register;    /* Modbus Register */
    u_int16_t length;        /* length of modbus data */
    unsigned int interval;   /* number of port scans between messages */
    u_int8_t *data;          /* pointer to the actual modbus data that this command refers */
    int datasize;            /* size of the *data memory area */
    unsigned int icount;     /* number of intervals passed */
    unsigned int requests;   /* total number of times this command has been sent */
    unsigned int responses;  /* number of valid modbus responses (exceptions included) */
    unsigned int timeouts;   /* number of times this command has timed out */
    unsigned int crcerrors;  /* number of checksum errors */
    unsigned int exceptions; /* number of modbus exceptions recieved from slave */
    u_int8_t lasterror;      /* last error on command */
    u_int16_t lastcrc;       /* used to determine if a conditional message should be sent */
    unsigned char firstrun;  /* Indicates that this command has been sent once */
    void *userdata;          /* Data that can be assigned by the user.  Use free function callback */
    void (*pre_send)(struct mb_cmd *cmd, void *userdata, u_int8_t *data, int size);
    void (*post_send)(struct mb_cmd *cmd, void *userdata, u_int8_t *data, int size);
    void (*send_fail)(struct mb_cmd *cmd, void *userdata);
    void (*userdata_free)(struct mb_cmd *cmd, void *userdata); /* Callback to free userdata */
    struct mb_cmd* next;
};

/* Port Functions - defined in modports.c */
int add_cmd(mb_port *p, mb_cmd *mc);


/* Command Functions - defined in modcmds.c */


/* TCP Server Functions - defined in mbserver.c */
int server_loop(mb_port *port);

/* Protocol Functions - defined in modbus.c */
int create_response(mb_port * port, unsigned char *buff, int size);

/* Utility Functions - defined in modutil.c */
u_int16_t crc16(unsigned char *msg, unsigned short length);
int crc16check(u_int8_t *buff, int length);

#ifdef DEBUG
 #define DEBUGMSG(x) debug(x)
 #define DEBUGMSG2(x,y) debug(x,y)
void debug(char *message, ...);
#else
 #define DEBUGMSG(x)
 #define DEBUGMSG2(x,y)
#endif

#endif /* __MODLIB_H */
