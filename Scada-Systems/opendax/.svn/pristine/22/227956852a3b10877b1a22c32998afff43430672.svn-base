/* modcmds.c - Modbus (tm) Communications Library
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
 * Source file for mb_cmd handling functions
 */

#include <modbus.h>
#include <mblib.h>

/* Sets the command values to some defaults */
static void
initcmd(struct mb_cmd *c)
{
    c->enable = 1;
    c->mode = MB_CONTINUOUS;
    c->node = 0;
    c->function = 0;
    c->m_register = 0;
    c->length = 0;
    c->data = NULL;
    c->datasize = 0;
    c->interval = 0;

    c->icount = 0;
    c->requests = 0;
    c->responses = 0;
    c->timeouts = 0;
    c->crcerrors = 0;
    c->exceptions = 0;
    c->lasterror = 0;
    c->lastcrc = 0;
    c->firstrun = 0;
    c->userdata = NULL;
    c->pre_send = NULL;
    c->post_send = NULL;
    c->send_fail = NULL;
    c->userdata_free = NULL;
    c->next = NULL;
};

/********************/
/* Public Functions */
/********************/


/* Allocates and initializes a master modbus command.  Returns the pointer
 * to the new command or NULL on failure.  If 'port' is not NULL then the
 * new command is added to the port.  The mb_destroy_port() function will
 * take care of deallocating this command in that instance.  Otherwise the
 * caller will have to worry about deallocation using the mb_destroy_cmd()
 * function */
mb_cmd *
mb_new_cmd(mb_port *port)
{
    int result;
    mb_cmd *mcmd;
    
    mcmd = (mb_cmd *)malloc(sizeof(mb_cmd));
    if(mcmd != NULL) {
        initcmd(mcmd);
        if(port != NULL) {
            result = add_cmd(port, mcmd);
            if(result) {
                free(mcmd);
                return NULL;
            }
        }
    }
    return mcmd;    
}

void
mb_destroy_cmd(mb_cmd *cmd) {
    
/* This frees the userdata if it is set.  If there is
 * a userdata_free callback assigned it will call that
 * otherwise it just calls free */    
    if(cmd->userdata != NULL) {
        if(cmd->userdata_free != NULL) {
            cmd->userdata_free(cmd, cmd->userdata);
        } else {
            free(cmd->userdata);
        }
    }
    free(cmd);
}


void
mb_disable_cmd(mb_cmd *cmd) {
    cmd->enable = 0;
}

void
mb_enable_cmd(mb_cmd *cmd) {
    cmd->enable = cmd->mode;
}

int
mb_set_command(mb_cmd *cmd, u_int8_t node, u_int8_t function, u_int16_t reg, u_int16_t length)
{
    u_int8_t *newdata;
    int newsize = 0;
    
    cmd->node = node;
    switch(function) {
        case 1:
        case 2:
        case 3:
        case 4:
        case 5:
        case 6:
        case 15:
        case 16:
            cmd->function = function;
            break;
        default:
            return MB_ERR_FUNCTION; 
    }
    cmd->m_register = reg;
    cmd->length = length;
    /* This will even reallocate the *data area if this command is called again */
    switch(function) {
        case 1:
        case 2:
        case 15:
            /* These are multiple bit function codes */
            newsize = (length - 1) / 8 +1; 
            break;
        case 3:
        case 4:
        case 16:
            /* These are multiple word function codes */
            newsize = length * 2;
            break;
        case 5:
            /* Single bit function code */
            newsize = 1;
        case 6:
            /* Single word function code */
            newsize = 2;
            break;
    }
    newdata = realloc(cmd->data, newsize);
    if(newdata == NULL) {
        return MB_ERR_ALLOC;
    } else {
        cmd->data = newdata;
        cmd->datasize = newsize;
    }
    return 0;
}

int
mb_set_interval(mb_cmd *cmd, int interval)
{
    cmd->interval = interval;
    return 0;
}


void
mb_set_mode(mb_cmd *cmd, unsigned char mode)
{
    if(mode == MB_ONCHANGE)
        cmd->mode = MB_ONCHANGE;
    else
        cmd->mode = MB_CONTINUOUS;
}

void
mb_set_cmd_userdata(mb_cmd *cmd, void *data, void (*userdata_free)(struct mb_cmd *, void *))
{
    cmd->userdata = data;
    if(userdata_free) {
        cmd->userdata_free = userdata_free;
    }
}

/* Returns true (1) if the function code for cmd will read from the node */
int
mb_is_read_cmd(mb_cmd *cmd)
{
    switch(cmd->function) {
        case 1:
        case 2:
        case 3:
        case 4:
            return 1;
        default:
            return 0;
    }

}

/* Returns true (1) if the function code for cmd will write to the node */
int
mb_is_write_cmd(mb_cmd *cmd)
{
    switch(cmd->function) {
        case 5:
        case 6:
        case 15:
        case 16:
            return 1;
        default:
            return 0;
    }
    
}


/* This sets the callback function that will be called just prior to sending a command.
 * The prototype must be...
 * void funcname(mb_cmd *cmd, void *userdata, u_int8_t data, int datasize);
 * The cmd is the command that is about to be sent, userdata is the pointer to the
 * userdata that was assigned by 'mb_set_userdata()', data is the internal command data
 * that would represent the data to be sent/received by the command and datasize is the
 * number of bytes that data occupies. */
void
mb_pre_send_callback(mb_cmd *cmd, void (*pre_send)(struct mb_cmd *, void *, u_int8_t *, int))
{
    cmd->pre_send = pre_send;
}

/* This sets the callback function that will be called just after sending the command.
 * The prototype and description are the same as mb_pre_send_callback(). */
void
mb_post_send_callback(mb_cmd *cmd, void (*post_send)(struct mb_cmd *, void *, u_int8_t *, int))
{
    cmd->post_send = post_send;
}

/* This sets the callback function that will be called if the given command fails.
 * The prototype is the same as the pre/post_send callbacks without the data
 * and datasize parameters */
void
mb_send_fail_callback(mb_cmd *cmd, void (*send_fail)(struct mb_cmd *, void *))
{
    cmd->send_fail = send_fail;
}


/* Set's a user defined deallocation callback for the userdata that is stored in the command.
 * If this function is never called for a command then the system function 'free()' will
 * be used to deallocate the data when the command is destroyed by the system.  Otherwise
 * the library will call this function to have the client program deallocate the memory */
void
mb_userdata_free_callback(mb_cmd *cmd, void (*userdata_free)(struct mb_cmd *, void *))
{
    cmd->userdata_free = userdata_free;
}
    
    
/*********************/
/* Utility Functions */
/*********************/


 
