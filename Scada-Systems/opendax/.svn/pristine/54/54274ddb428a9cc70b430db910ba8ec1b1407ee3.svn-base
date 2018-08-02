/* modbus.c - Modbus (tm) Communications Library
 * Copyright (C) 2006 Phil Birkelbach
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
 * Source code file that contains all of the modbus protocol stuff.
 */


#include <mblib.h>
#include <modbus.h>

int master_loop(mb_port *);

/* Calculates the difference between the two times */
inline unsigned long long
timediff(struct timeval oldtime,struct timeval newtime)
{
    return (newtime.tv_sec - oldtime.tv_sec) * 1000 + 
           (newtime.tv_usec / 1000)-(oldtime.tv_usec / 1000);
};


/* Opens the port passed in m_port and starts the loop that
 * will handle the port */
int
mb_run_port(struct mb_port *m_port)
{
    /* If the port is not already open */
    if(!m_port->fd) {
        mb_open_port(m_port);
    }
    /* If the port is still not open then we inhibit the port and
     * let the _loop() functions deal with it */
    if(m_port->fd == 0) {
        m_port->inhibit = 1;
    }
    printf("mb_run_port() - Port type = %d\n", m_port->type);
            
    if(m_port->type == MB_MASTER) {
        printf("mb_run_port() - Calling master_loop() for %s\n", m_port->name);
        return master_loop(m_port);
    } else if(m_port->type == MB_SLAVE) {
        if(m_port->protocol == MB_TCP) {
            printf("mb_run_port() - Start the TCP Server Loop\n");
            return server_loop(m_port);
        } else {
            printf("mb_run_port() - Should run slave here\n");
            return MB_ERR_PROTOCOL;
        }
        /* TODO: start slave thread */
    } else {
        return MB_ERR_PORTTYPE;
    }
    return 0;
}

int
mb_scan_port(mb_port *mp)
{
    int cmds_sent = 0;
    int result;
    mb_cmd *mc;
    
    mc = mp->commands;
    while(mc != NULL && mp->inhibit == 0) {
        /* Only if the command is enabled and the interval counter is over */
        if(mc->enable && (++mc->icount >= mc->interval)) { 
            mc->icount = 0;
            if(mp->maxattempts) {
                mp->attempt++;
                DEBUGMSG2("Incrementing attempt - %d", mp->attempt);
            }
            result = mb_send_command(mp, mc);
            if( result > 0 ) {
                mp->attempt = 0; /* Good response, reset counter */
                cmds_sent++;
                if(mp->delay > 0) usleep(mp->delay * 1000);
            } else if( result == 0 ) mp->maxattempts--; /* Conditional command that was not sent */
            
            if((mp->maxattempts && mp->attempt >= mp->maxattempts) || mp->dienow) {
                mp->inhibit_temp = 0;
                mp->inhibit = 1;
            }

        }
        mc = mc->next; /* get next command from the linked list */
    } /* End of while for sending commands */
    return cmds_sent;
}



/* This is the primary event loop for a Modbus master.  It calls the functions
   to send the request and recieve the responses.  It also takes care of the
   retries and the counters. */
int
master_loop(mb_port *mp)
{
    long time_spent;
    int result;
    struct mb_cmd *mc;
    struct timeval start, end;
    unsigned char bail = 0;
    
    mp->running = 1; /* Tells the world that we are going */
    mp->attempt = 0;
    mp->dienow = 0;
   
    /* If enable goes negative we bail at the next scan */
    while(1) {
        gettimeofday(&start, NULL);
        if(mp->enable && !mp->inhibit) { /* If enable=0 then pause for the scanrate and try again. */
            mc = mp->commands;
            while(mc != NULL && !bail) {
                /* Only if the command is enabled and the interval counter is over */
                if(mc->enable && (++mc->icount >= mc->interval)) { 
                    mc->icount = 0;
                    if(mp->maxattempts) {
                        mp->attempt++;
                        //DEBUGMSG2("Incrementing attempt - %d", mp->attempt);
                    }
                    if( mb_send_command(mp, mc) > 0 )
                        mp->attempt = 0; /* Good response, reset counter */
                    if((mp->maxattempts && mp->attempt >= mp->maxattempts) || mp->dienow) {
                        bail = 1;
                        mp->inhibit_temp = 0;
                        mp->inhibit = 1;
                    }
                }
                if(mp->delay > 0) usleep(mp->delay * 1000);
                mc = mc->next; /* get next command from the linked list */
            } /* End of while for sending commands */
        }
        if(mp->inhibit) {
            bail = 0;
            mb_close_port(mp);
            if(mp->inhibit_time) {
                sleep(mp->inhibit_time);
                result = mb_open_port(mp);
                if(result == 0) mp->inhibit = 0;
            } else {
                return MB_ERR_PORTFAIL;
            }    
        }
        /* This calculates the length of time that it took to send the messages on this port
           and then subtracts that time from the port's scanrate and calls usleep to hold
           for the right amount of time.  */
        gettimeofday(&end, NULL);
        time_spent = (end.tv_sec-start.tv_sec)*1000 + (end.tv_usec/1000 - start.tv_usec/1000);
        /* If it takes longer than the scanrate then just go again instead of sleeping */
        if(time_spent < mp->scanrate)
            usleep((mp->scanrate - time_spent) * 1000);
    }
    /* Close the port */
    mb_close_port(mp); 
    mp->dienow = 0;
    mp->running = 0;
    return MB_ERR_PORTFAIL; 
}

/* This function formulates and sends the modbus master request */
static int
sendRTUrequest(mb_port *mp, mb_cmd *cmd)
{
    u_int8_t buff[MB_FRAME_LEN], length;
    u_int16_t crc, temp;
    
    /* build the request message */
    buff[0]=cmd->node;
    buff[1]=cmd->function;
    
    switch (cmd->function) {
        case 1:
        case 2:
        case 3:
        case 4:
            COPYWORD(&buff[2], &cmd->m_register);
            COPYWORD(&buff[4], &cmd->length);
            length = 6;
            break;
        case 5:
            temp = *cmd->data;
            if(cmd->enable == MB_CONTINUOUS || (temp != cmd->lastcrc) || !cmd->firstrun ) {
                COPYWORD(&buff[2], &cmd->m_register);
                if(temp) buff[4] = 0xff;
                else     buff[4] = 0x00;
                buff[5] = 0x00;
                cmd->firstrun = 1;
                cmd->lastcrc = temp;
                length = 6;
                break;
            } else {
                return 0;
            }
            break;
        case 6:
            temp = *cmd->data;
            /* If the command is contiunous go, if conditional then 
             check the last checksum against the current datatable[] */
            if(cmd->enable == MB_CONTINUOUS || (temp != cmd->lastcrc)) {
                COPYWORD(&buff[2], &cmd->m_register);
                COPYWORD(&buff[4], &temp);
                cmd->lastcrc = temp; /* Since it's a single just store the word */
                length = 6;
                break;
            } else {
                return 0;
            }
        /* TODO: Add the rest of the function codes */
        default:
            break;
    }
    crc = crc16(buff, length);
    COPYWORD(&buff[length], &crc);
    /* Send Request */
    cmd->requests++; /* Increment the request counter */
    tcflush(mp->fd, TCIOFLUSH);
    /* Send the buffer to the callback routine. */
    if(mp->out_callback) {
        mp->out_callback(mp, buff, length + 2);
    }

    return write(mp->fd, buff, length + 2);
}

/* This function waits for one interbyte timeout (wait) period and then
 * checks to see if there is data on the port.  If there is no data and
 * we still have not received any data then it compares the current time
 * against the time the loop was started to see about the timeout.  If there
 * is data then it is written into the buffer.  If there is no data on the
 * read() and we have received some data already then the full message should
 * have been received and the function exits. 
 
 * Returns 0 on timeout
 * Returns -1 on CRC fail
 * Returns the length of the message on success */
static int
getRTUresponse(u_int8_t *buff, mb_port *mp)
{
    unsigned int buffptr = 0;
    struct timeval oldtime, thistime;
    int result;
    
    gettimeofday(&oldtime, NULL);
    
    while(1) {
        usleep(mp->frame * 1000);
        result = read(mp->fd, &buff[buffptr], MB_FRAME_LEN);
        if(result > 0) { /* Get some data */
            buffptr += result; // TODO: WE NEED A BOUNDS CHECK HERE.  Seg Fault Commin'
        } else { /* Message is finished, good or bad */
            if(buffptr > 0) { /* Is there any data in buffer? */
                
                if(mp->in_callback) {
                    mp->in_callback(mp, buff, buffptr);
                }
                /* Check the checksum here. */
                result = crc16check(buff, buffptr);
                
                if(!result) return -1;
                else return buffptr;
                
            } else { /* No data in the buffer */
                gettimeofday(&thistime,NULL); 
                if(timediff(oldtime, thistime) > mp->timeout) {
                    return 0;
                }
            }
        }
    }
    return 0; /* Should never get here */
}

static int
sendASCIIrequest(mb_port *mp, mb_cmd *cmd)
{
    return 0;
}

static int
getASCIIresponse(u_int8_t *buff, mb_port *mp)
{
    return 0;
}

/* This function takes the message buffer and the current command and
 * determines what to do with the message.  It may write data to the 
 * datatable or just return if the message is an acknowledge of a write.
 * This function is protocol indifferent so the checksums should be
 * checked before getting here.  This function assumes that *buff looks
 * like an RTU message so the ASCII & TCP response functions should translate
 * the their responses into RTUish messages */
/* TODO: There is all kinds of buffer overflow potential here.  It should all be checked */
static int
handleresponse(u_int8_t *buff, mb_cmd *cmd)
{
    int n;
    
    cmd->responses++;
    if(buff[1] >= 0x80) 
        return buff[2];
    if(buff[0] != cmd->node)
        return ME_WRONG_DEVICE;
    if(buff[1] != cmd->function)
        return ME_WRONG_FUNCTION;
    /* If we get this far the message should be good */
    switch (cmd->function) {
        case 1:
        case 2:
            memcpy(cmd->data, &buff[3], buff[2]);
            break;
        case 3:
        case 4:
            /* TODO: There may be times when we get more data than cmd->length but how to deal with that? */
            for(n = 0; n < (buff[2] / 2); n++) {
                COPYWORD(&((u_int16_t *)cmd->data)[n], &buff[(n * 2) + 3]);
            }
            break;
        case 5:
        case 6:
            COPYWORD(cmd->data, &buff[2]);
            break;
        default:
            break;
    }
    return 0;
}


/* External function to send a Modbus commaond (mc) to port (mp).  The function
 * sets some function pointers to the functions that handle the port protocol and
 * then uses those functions generically.  The retry loop tries the command for the
 * configured number of times and if successful returns 0.  If not, an error code
 * is returned. mb_buff should be at least MB_FRAME_LEN in length */

int
mb_send_command(mb_port *mp, mb_cmd *mc)
{
    u_int8_t buff[MB_FRAME_LEN]; /* Modbus Frame buffer */
    int try = 1;
    int result, msglen;
    static int (*sendrequest)(struct mb_port *, struct mb_cmd *) = NULL;
    static int (*getresponse)(u_int8_t *,struct mb_port *) = NULL;
    
  /* This sets up the function pointers so we don't have to constantly check
   *  which protocol we are using for communication.  From this point on the
   *  code is generic for RTU or ASCII */
    if(mp->protocol == MB_RTU) {
        sendrequest = sendRTUrequest;
        getresponse = getRTUresponse;
    } else if(mp->protocol == MB_ASCII) {
        sendrequest = sendASCIIrequest;
        getresponse = getASCIIresponse;
    } else {
        return -1;
    }
    
    if(mc->pre_send != NULL) {
        mc->pre_send(mc, mc->userdata, mc->data, mc->datasize);
        /* This is in case the pre_send() callback disables the command */
        if(mc->enable == 0) return 0;
    }
    do { /* retry loop */
        result = sendrequest(mp, mc);
        if(result > 0) {
            msglen = getresponse(buff, mp);
        } else if(result == 0) {
            /* Should be 0 when a conditional command simply doesn't run */
            return result;
        } else {
            return -1;
        }
        
        if(msglen > 0) {
            result = handleresponse(buff,mc); /* Returns 0 on success + on failure */
            if(result > 0) {
                if(mc->send_fail != NULL) {
                    mc->send_fail(mc, mc->userdata);
                }
                mc->exceptions++;
                mc->lasterror = result | ME_EXCEPTION;
                DEBUGMSG2("Exception Received - %d", result);
            } else { /* Everything is good */
                if(mc->post_send != NULL) {
                    mc->post_send(mc, mc->userdata, mc->data, mc->datasize);
                }
                mc->lasterror = 0;
            }
            return msglen; /* We got some kind of message so no sense in retrying */
        } else if(msglen == 0) {
            if(mc->send_fail != NULL) {
                mc->send_fail(mc, mc->userdata);
            }
            DEBUGMSG("Timeout");
            mc->timeouts++;
            mc->lasterror = ME_TIMEOUT;
        } else {
            /* Checksum failed in response */
            if(mc->send_fail != NULL) {
                mc->send_fail(mc, mc->userdata);
            }
            DEBUGMSG("Checksum");
            mc->crcerrors++;
            mc->lasterror = ME_CHECKSUM;
        }
    } while(try++ <= mp->retries);
    /* After all the retries get out with error */
    /* TODO: Should set error code?? */
    if(mc->send_fail != NULL) {
        mc->send_fail(mc, mc->userdata);
    }
    return 0 - mc->lasterror;
}

static int
_create_exception(unsigned char *buff, u_int16_t exception)
{
    buff[1] |= ME_EXCEPTION;
    buff[2] = exception;
    return 3;
}

/* Handles responses for function codes 1 and 2 */
static int
_read_bits_response(mb_port *port, unsigned char *buff, int size, int mbreg)
{
    int n, bit, word, buffbit, buffbyte;
    unsigned int regsize;
    u_int16_t index, count, *reg;

    COPYWORD(&index, (u_int16_t *)&buff[2]); /* Starting Address */
    COPYWORD(&count, (u_int16_t *)&buff[4]); /* Number of words/coils */
    if(port->slave_read) { /* Call the callback function if it has been set */
        port->slave_read(port, mbreg, index, count, port->userdata);
    }
    if(mbreg == MB_REG_COIL) {
        reg = port->coilreg;
        regsize = port->coilsize;
    } else {
        reg = port->discreg;
        regsize = port->discsize;
    }
    
    if(((count - 1)/8+1) > (size - 3)) { /* Make sure we have enough room */
        return MB_ERR_OVERFLOW;
    }
    if((index + count) > regsize) {
        return _create_exception(buff, ME_BAD_ADDRESS);
    }
    buff[2] = (count - 1)/8+1;

    bit = index % 16;
    word = index / 16;
    buffbit = 0;
    buffbyte = 3;
    for(n = 0; n < count; n++) {
        if(reg[word] & (0x01 << bit)) {
            buff[buffbyte] |= (0x01 << buffbit);
        } else {
            buff[buffbyte] &= ~(0x01 << buffbit);
        }
        buffbit++;
        if(buffbit == 8) {
            buffbit = 0; buffbyte++;
        }
        bit++;
        if(bit == 16) {
            bit = 0; word++;
        }
    }
    return (count - 1)/8+4;
}

/* Handles responses for function codes 3 and 4 */
static int
_read_words_response(mb_port *port, unsigned char *buff, int size, int mbreg)
{
    int n;
    unsigned int regsize;
    u_int16_t index, count, *reg;

    COPYWORD(&index, (u_int16_t *)&buff[2]); /* Starting Address */
    COPYWORD(&count, (u_int16_t *)&buff[4]); /* Number of words/coils */
    if(port->slave_read) { /* Call the callback function if it has been set */
        port->slave_read(port, mbreg, index, count, port->userdata);
    }
    if(mbreg == MB_REG_HOLDING) {
        reg = port->holdreg;
        regsize = port->holdsize;
    } else {
        reg = port->inputreg;
        regsize = port->inputsize;
    }

    if((count * 2) > (size - 3)) { /* Make sure we have enough room */
        return MB_ERR_OVERFLOW;
    }
    if((index + count) > regsize) {
        return _create_exception(buff, ME_BAD_ADDRESS);
    }
    buff[2] = count * 2;
    for(n = 0; n < count; n++) {
        COPYWORD(&buff[3+(n*2)], &reg[index+n]);
    }
    return (count * 2) + 3;
}

/* Creates a generic slave response.  buff should point to a buffer that
 * looks like an RTU request without the checksum.  This function will generate
 * an RTU response message, and write that back into buff.  size is the total
 * size that we can write into buff.  Returns positive number of bytes written, zero
 * if no bytes written and negative error code if there is a problem. */
int
create_response(mb_port *port, unsigned char *buff, int size)
{
    u_int8_t node, function;
    u_int16_t index, value, count;
    int word, bit, n;
    node = buff[0]; /* Node Number */
    function = buff[1]; /* Modbus Function Code */
    
    /* If we're TCP then node doesn't matter yet.  Other wise return 0 if
     * this message isn't for us. */
    if(port->protocol != MB_TCP) {
        if(node != port->slaveid) return 0;
    }
    switch(function) {
        case 1: /* Read Coils */
            return _read_bits_response(port, buff, size, MB_REG_COIL);
        case 2: /* Read Discrete Inputs */
            return _read_bits_response(port, buff, size, MB_REG_DISC);
        case 3: /* Read Holding Registers */
            return _read_words_response(port, buff, size, MB_REG_HOLDING);
        case 4: /* Read Input Registers */
            return _read_words_response(port, buff, size, MB_REG_INPUT);
        case 5: /* Write Single Coil */
            COPYWORD(&index, (u_int16_t *)&buff[2]); /* Starting Address */
            COPYWORD(&value, (u_int16_t *)&buff[4]); /* Value */
            if(index >= port->coilsize) {
                return _create_exception(buff, ME_BAD_ADDRESS);
            }
            if(value) {
                port->coilreg[index / 16] |= (0x01 << (index % 16));
            } else {
                port->coilreg[index / 16] &= ~(0x01 << (index % 16));
            }
            if(port->slave_write) { /* Call the callback function if it has been set */
                port->slave_write(port, MB_REG_COIL, index, 1, port->userdata);
            }
            return 6;
        case 6: /* Write Single Register */
            COPYWORD(&index, (u_int16_t *)&buff[2]); /* Starting Address */
            COPYWORD(&value, (u_int16_t *)&buff[4]); /* Value */
            if(index >= port->holdsize) {
                return _create_exception(buff, ME_BAD_ADDRESS);
            }
            port->holdreg[index] = value;
            if(port->slave_write) { /* Call the callback function if it has been set */
                port->slave_write(port, MB_REG_HOLDING, index, 1, port->userdata);
            }
            return 6;
        case 8:
            return size / 2;
        case 15: /* Write Multiple Coils */
            COPYWORD(&index, (u_int16_t *)&buff[2]); /* Starting Address */
            COPYWORD(&count, (u_int16_t *)&buff[4]); /* Value */
            if((index + count) > port->coilsize) {
                return _create_exception(buff, ME_BAD_ADDRESS);
            }
            word = index / 16;
            bit = index % 16;
            for(n = 0; n < count; n++) {
                fprintf(stderr, "Buff[%d] = 0x%X\n", 7 +n/8, buff[7+n/8]);
                if(buff[7 + n/8] & (0x01 << (n%8))) {
                    fprintf(stderr, "Setting Coil %d to TRUE\n", index + n);
                    port->coilreg[word] |= (0x01 << bit);
                } else {
                    fprintf(stderr, "Setting Coil %d to FALSE\n", index + n);
                    port->coilreg[word] &= ~(0x01 << bit);
                }
                bit++;
                if(bit == 16) { bit = 0; word++; }
            }
            if(port->slave_write) { /* Call the callback function if it has been set */
                port->slave_write(port, MB_REG_COIL, index, count, port->userdata);
            }
            return 6;
        case 16: /* Write Multiple Registers */
            COPYWORD(&index, (u_int16_t *)&buff[2]); /* Starting Address */
            COPYWORD(&count, (u_int16_t *)&buff[4]); /* Value */
            if((index + count) > port->holdsize) {
                return _create_exception(buff, ME_BAD_ADDRESS);
            }
            for(n = 0; n < count; n++) {
                COPYWORD(&port->holdreg[index + n], &buff[7 + (n*2)]);
            }
            if(port->slave_write) { /* Call the callback function if it has been set */
                port->slave_write(port, MB_REG_HOLDING, index, count, port->userdata);
            }
            return 6;
        default:
            break;
    }
    
    return 0;
}
