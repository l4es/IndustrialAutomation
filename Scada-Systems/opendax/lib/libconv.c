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
 
 * This file contains libdax data conversion functions.  These functions handle
 * the required conversions of data to be sent and received from the server.
 */

#include <libdax.h>
#include <dax/libcommon.h>

/* TODO: All of these functions need to be written.  Right now we just
   assume that all is good and that we don't need any of this. */
/* TODO: Might also be nice to have some that will handle a whole buffer
   of data instead of just a single point */

/* 16 Bit Conversion Functions */
int16_t
mtos_int(int16_t x)
{
    return x;
}

u_int16_t
mtos_uint(u_int16_t x)
{
    return x;
}

int16_t
stom_int(int16_t x)
{
    return x;
}

u_int16_t
stom_uint(u_int16_t x)
{
    return x;
}

/* 32 Bit conversion Functions */
int32_t
mtos_dint(int32_t x)
{
    return x;
}

u_int32_t
mtos_udint(u_int32_t x)
{
    return x;
}

float
mtos_real(float x)
{
    return x;
}

int32_t
stom_dint(int32_t x)
{
    return x;
}

u_int32_t
stom_udint(u_int32_t x)
{
    return x;
}

float
stom_real(float x)
{
    return x;
}

dax_lint
mtos_lint(dax_lint x)
{
    return x;
}

dax_ulint
mtos_ulint(dax_ulint x)
{
    return x;
}

dax_lreal
mtos_lreal(dax_lreal x)
{
    return x;
}

dax_lint
stom_lint(dax_lint x)
{
    return x;
}

dax_ulint
stom_ulint(dax_ulint x)
{
    return x;
}

dax_lreal
stom_lreal(dax_lreal x)
{
    return x;
}

/* This is a generic module to server function.  It looks at the
 * datatype that is passed to it and converts the data found at src
 * and places it in *dst. If successful it returns 0 and a negative
 * error otherwise. */
int
mtos_generic(tag_type type, void *dst, void *src) {
    switch (type) {
        case DAX_BOOL:
        case DAX_BYTE:
        case DAX_SINT:
            *(dax_byte *)dst = *(dax_byte *)src;
            break;
        case DAX_WORD:
            *(dax_word *)dst = mtos_word(*(dax_word *)src);
            break;
        case DAX_INT:
            *(dax_int *)dst = mtos_int(*(dax_int *)src);
            break;
        case DAX_UINT:
            *(dax_uint *)dst = mtos_uint(*(dax_uint *)src);
            break;
        case DAX_DWORD:
            *(dax_dword *)dst = mtos_dword(*(dax_dword *)src);
            break;
        case DAX_DINT:
            *(dax_dint *)dst = mtos_dint(*(dax_dint *)src);
            break;
        case DAX_UDINT:
            *(dax_udint *)dst = mtos_udint(*(dax_udint *)src);
            break;
        case DAX_TIME:
            *(dax_time *)dst = mtos_time(*(dax_time *)src);
            break;
        case DAX_REAL:
            *(dax_real *)dst = mtos_real(*(dax_real *)src);
            break;
        case DAX_LWORD:
            *(dax_lword *)dst = mtos_lword(*(dax_lword *)src);
            break;
        case DAX_LINT:
            *(dax_lint *)dst = mtos_lint(*(dax_lint *)src);
            break;
        case DAX_ULINT:
            *(dax_ulint *)dst = mtos_ulint(*(dax_ulint *)src);
            break;
        case DAX_LREAL:
            *(dax_lreal *)dst = mtos_lreal(*(dax_lreal *)src);
            break;
        default:
            return ERR_ARG;
    }
    return 0;
}

/* This is a generic server to module function.  It looks at the
 * datatype that is passed to it and converts the data found at src
 * and places it in *dst. If successful it returns 0 and a negative
 * error otherwise. */
int
stom_generic(tag_type type, void *dst, void *src) {
    switch (type) {
        case DAX_BOOL:
        case DAX_BYTE:
        case DAX_SINT:
            *(dax_byte *)dst = *(dax_byte *)src;
            break;
        case DAX_WORD:
            *(dax_word *)dst = stom_word(*(dax_word *)src);
            break;
        case DAX_INT:
            *(dax_int *)dst = stom_int(*(dax_int *)src);
            break;
        case DAX_UINT:
            *(dax_uint *)dst = stom_uint(*(dax_uint *)src);
            break;
        case DAX_DWORD:
            *(dax_dword *)dst = stom_dword(*(dax_dword *)src);
            break;
        case DAX_DINT:
            *(dax_dint *)dst = stom_dint(*(dax_dint *)src);
            break;
        case DAX_UDINT:
            *(dax_udint *)dst = stom_udint(*(dax_udint *)src);
            break;
        case DAX_TIME:
            *(dax_time *)dst = stom_time(*(dax_time *)src);
            break;
        case DAX_REAL:
            *(dax_real *)dst = stom_real(*(dax_real *)src);
            break;
        case DAX_LWORD:
            *(dax_lword *)dst = stom_lword(*(dax_lword *)src);
            break;
        case DAX_LINT:
            *(dax_lint *)dst = stom_lint(*(dax_lint *)src);
            break;
        case DAX_ULINT:
            *(dax_ulint *)dst = stom_ulint(*(dax_ulint *)src);
            break;
        case DAX_LREAL:
            *(dax_lreal *)dst = stom_lreal(*(dax_lreal *)src);
            break;
        default:
            return ERR_ARG;
    }
    return 0;
}