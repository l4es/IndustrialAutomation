/*
 *                         IndigoSCADA
 *
 *   This software and documentation are Copyright 2002 to 2009 Enscada 
 *   Limited and its licensees. All rights reserved. See file:
 *
 *                     $HOME/LICENSE 
 *
 *   for full copyright notice and license terms. 
 *
 */
#include "fifo.h"
#include "fifoc.h"

FIFO_DLL_ENTRY fifo_h fifo_open(char const* name, size_t max_size, p_call_exit_handler f_log_arg)
{
    fifo_obj* fifo = new fifo_obj();

    if(fifo->open(name, max_size, f_log_arg))
	{
        return (fifo_h)fifo;
    }

	fprintf(stderr, "Failed to create queue buffer: %i\n", max_size);
	fflush(stderr);

	f_log_arg(0, NULL, "Failed to create queue buffer\n");

    delete fifo;
    return NULL;
}
        
FIFO_DLL_ENTRY void fifo_put(fifo_h hnd, char* message, int length)
{
    if(hnd)
    {
        ((fifo_obj*)hnd)->put(message, length);
    }
}

FIFO_DLL_ENTRY int fifo_get(fifo_h hnd, char* buf, int buf_size, unsigned msec)
{
    if(hnd)
    {
        return ((fifo_obj*)hnd)->get(buf, buf_size, msec);
    }
    else
    {
        return -1;
    }
}

FIFO_DLL_ENTRY void fifo_close(fifo_h hnd)
{
    if(hnd)
    {
        ((fifo_obj*)hnd)->close();
        delete (fifo_obj*)hnd;
    }
}
