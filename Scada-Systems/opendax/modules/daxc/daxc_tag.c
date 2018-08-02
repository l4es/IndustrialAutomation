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
 *  source code file for the tag commands OpenDAX Command Line Client  module
 */


#include <daxc.h>

extern dax_state *ds;
extern int quiet_mode;

static inline void
show_tag(int n, dax_tag temp_tag)
{
    /* Print the name */
    if( n >= 0 ) printf("[%3d] ", n);
    printf("%-32s %s", temp_tag.name, dax_type_to_string(ds, temp_tag.type));
    /* Output the array size of it's greater than 1 */
    if(temp_tag.count > 1) {
        printf("[%d]", temp_tag.count);
    }
    printf("\n");
}


/* Adds a tag to the dax tag database. */
int
tag_add(char **tokens)
{
    int count, result;
    tag_type type;
    Handle handle;
    const char usage[] = "Usage: add type count\n";
    
    /* Make sure that name is not NULL and we get the tagname */
    if( tokens[0] == NULL) {
        fprintf(stderr, "ERROR: No tagname given\n");
        fprintf(stderr, usage);
        return 1;
    }
    
    if( tokens[1] ) {
        type = dax_string_to_type(ds, tokens[1]);
        if( type < 0 ) {
            fprintf(stderr, "ERROR: Invalid Type '%s' Given\n", tokens[1]);
            return 1;
        }
    } else {
        fprintf(stderr, "ERROR: No Type Given\n");
        fprintf(stderr, usage);
        return 1;
    }
        
    if( tokens[2] ) {
        count = strtol(tokens[2], NULL, 0);
        if( count == 0 ) {
            fprintf(stderr, "ERROR: Invalid Count Given\n");
        }
    } else {
        fprintf(stderr, "ERROR: No Count Given\n");
        fprintf(stderr, usage);
        return 1;
    }
    
    result = dax_tag_add(ds, &handle, tokens[0], type, count);
    if(result == 0) {
        if(!quiet_mode) printf("Tag Added at index %d\n", handle.index);
    } else {
        /* TODO: Print descriptive error message here */
        printf("OPPS Can't add tag???\n");
    }
    return 0;
}

/* TAG LIST command function 
 * If we have no arguments then we list all the tags
 * If we have a single argument then we see if it's a tagname
 * or a number.  If a tagname then we list that tag if it's a number
 * then we check for a second argument.  If we have two numbers then
 * we list from the first to the first + second if not then we list
 * the next X tags and increment lastindex. */

/* TODO: If we allow tags to be deleted then gaps will appear in the list.
 * We should deal with these appropriately */
int
list_tags(char **tokens)
{
    dax_tag temp_tag;
    static int lastindex;
    char *arg[] = {NULL, NULL};
    char *end_ptr;
    int n, argcount, start, count;
    
    argcount = 0;
    
    arg[0] = strtok(NULL, " ");
    
    if(arg[0]) {
        start = strtol(arg[0], &end_ptr, 0);
        /* If arg[0] is text then it's a tagname instead of an index */
        if(end_ptr == arg[0]) {
            if( dax_tag_byname(ds, &temp_tag, arg[0]) ) {
                fprintf(stderr, "ERROR: Unknown Tagname %s\n", arg[0]);
                return 1;
            } else {
                show_tag(-1, temp_tag);
            }
            /* arg[0] should be an integer here and start is the first one */
        } else {
            arg[1] = strtok(NULL, " ");
            /* List tags from start to start + count */
            if(arg[1]) {
                count = strtol(arg[1], &end_ptr, 0);
                for(n = start; n < (start + count); n++) {
                    if(dax_tag_byindex(ds, &temp_tag, n)) {
                        printf("No More Tags To List\n");
                        return 1;
                    } else {
                        show_tag(n, temp_tag);
                    }
                }
            } else {
                /* List the next 'start' amount of tags */
                for(n = lastindex; n < (start + lastindex); n++) {
                    if(dax_tag_byindex(ds, &temp_tag, n)) {
                        printf("No More Tags To List\n");
                        lastindex = 0;
                        return 1;
                    } else {
                        show_tag(n, temp_tag);
                    }
                }
                lastindex += start;
            }
        }
    } else {
        /* List all tags */
        n = 0;
        while( !dax_tag_byindex(ds, &temp_tag, n) ) {
            show_tag(n, temp_tag);
            n++;
        }
    }
    return 0;
}


/* Used to printf a dax tag value. */
static void
dax_to_string(tag_type type, void *buff, int index)
{
    switch (type) {
     /* Each number has to be cast to the right datatype then dereferenced */
        case DAX_BOOL:
            if((0x01 << (index % 8)) & ((u_int8_t *)buff)[index / 8]) {
                printf("1\n");
            } else {
                printf("0\n");
            }
            break;
        case DAX_BYTE:
        case DAX_SINT:
            printf("%hhd\n", ((dax_sint *)buff)[index]);
            break;
        case DAX_WORD:
        case DAX_UINT:
            printf("%d\n", ((dax_uint *)buff)[index]);
            break;
        case DAX_INT:
            printf("%hd\n", ((dax_int *)buff)[index]);
            break;
        case DAX_DWORD:
        case DAX_UDINT:
        case DAX_TIME:
            printf("%d\n", ((dax_udint *)buff)[index]);
            break;
        case DAX_DINT:
            printf("%d\n", ((dax_dint *)buff)[index]);
            break;
        case DAX_REAL:
            printf("%g\n", ((dax_real *)buff)[index]);
            break;
        case DAX_LWORD:
        case DAX_ULINT:
            printf("%lld\n", ((dax_ulint *)buff)[index]);
            break;
        case DAX_LINT:
            printf("%lld\n", ((dax_lint *)buff)[index]);
            break;
        case DAX_LREAL:
            printf("%g\n", ((dax_lreal *)buff)[index]);
            break;
    }
}

/* This function figures out how to format the data from the string given
 * by *val and places the result in *buff.  If *mask is NULL it is ignored */
/* TODO: Check for overflow */
static void
string_to_dax(char *val, tag_type type, void *buff, void *mask, int index)
{   
    long temp;

    switch (type) {
        case DAX_BOOL:
            temp = strtol(val, NULL, 0);
            if(temp == 0) {
                ((u_int8_t *)buff)[index / 8] &= ~(0x01 << (index % 8));
            } else {
                ((u_int8_t *)buff)[index / 8] |= (0x01 << (index % 8));
            }
            if(mask) {
                ((u_int8_t *)mask)[index / 8] |= (0x01 << (index % 8));
            }
            break;
        case DAX_BYTE:
        case DAX_SINT:
            ((dax_sint *)buff)[index] = (dax_sint)strtol(val, NULL, 0);
            if(mask) ((dax_sint *)mask)[index] = 0xFF;
            break;
        case DAX_WORD:
        case DAX_UINT:
            ((dax_uint *)buff)[index] = (dax_uint)strtol(val, NULL, 0);
            if(mask) ((dax_uint *)mask)[index] = 0xFFFF;
            break;
        case DAX_INT:
            ((dax_int *)buff)[index] = (dax_int)strtol(val, NULL, 0);
            if(mask) ((dax_int *)mask)[index] = 0xFFFF;
            break;
        case DAX_DWORD:
        case DAX_UDINT:
        case DAX_TIME:
            ((dax_udint *)buff)[index] = (dax_udint)strtol(val, NULL, 0);
            if(mask) ((dax_udint *)mask)[index] = 0xFFFFFFFF;
            break;
        case DAX_DINT:
            ((dax_dint *)buff)[index] = (dax_dint)strtol(val, NULL, 0);
            if(mask) ((dax_dint *)mask)[index] = 0xFFFFFFFF;
            break;
        case DAX_REAL:
            ((dax_real *)buff)[index] = strtof(val, NULL);
            if(mask) ((u_int32_t *)mask)[index] = 0xFFFFFFFF;
            break;
        case DAX_LWORD:
        case DAX_ULINT:
            ((dax_ulint *)buff)[index] = (dax_ulint)strtol(val, NULL, 0);
            if(mask) ((dax_ulint *)mask)[index] = DAX_64_ONES;
            break;
        case DAX_LINT:
            ((dax_lint *)buff)[index] = (dax_lint)strtol(val, NULL, 0);
            if(mask) ((dax_lint *)mask)[index] = DAX_64_ONES;
            break;
        case DAX_LREAL:
            ((dax_lreal *)buff)[index] = strtod(val, NULL);
            if(mask) ((u_int64_t *)mask)[index] = DAX_64_ONES;
            break;
    }
}


/* This is the tag read function.  One or two parameters can be given. The
 * first parameter is the tagname and the second if given is the number of
 * points to be returned */
int
tag_read(char **tokens)
{
    Handle handle;
    int result, n, count = 0;
    char *name;
    void *buff;

    if(tokens[0]) {
        name = tokens[0];
        if(tokens[1]) {
            count = (int)strtoul(tokens[1], NULL, 0);
        }
    } else {
        fprintf(stderr, "ERROR: No Tagname Given\n");
        return ERR_NOTFOUND;
    }
    
    result = dax_tag_handle(ds, &handle, name, count);
    if(result) {
        /* TODO: More descriptive error messages here based on result */
        fprintf(stderr, "ERROR: %s Not a Valid Tag\n", tokens[0]);
        return ERR_ARG;
    }
    
    buff = malloc(handle.size);
    if(buff == NULL) {
        fprintf(stderr, "ERROR: Unable to Allocate Memory\n");
        return ERR_ALLOC;
    }
    
    result = dax_read_tag(ds, handle, buff);
    if(result == 0) {
        if(IS_CUSTOM(handle.type)) {
            /* Print Custom Stuff Here */
            fprintf(stderr, "ERROR: Given Tagname is a compound data type\n");
        } else {
            for(n = 0; n < handle.count; n++) {
                dax_to_string(handle.type, buff, n);
            }
        }
    } else {
        fprintf(stderr, "ERROR: Unable to Read Tag %s\n", tokens[0]);
    }
    free(buff);
    return result;
}

/* This is the tag write function.  It is pretty lame at the moment.  It
 * takes a tagname as the first argument and then as many arguments after that
 * as needed to represent the data to add.*/
int
tag_write(char **tokens, int tcount) {
    Handle handle;
    int result, n, points, bit, byte;
    long temp;
    char *name;
    void *buff, *mask;
    
    if(tokens[0]) {
        name = tokens[0];
    } else {
        fprintf(stderr, "ERROR: No Tagname Given\n");
        return ERR_NOTFOUND;
    }
    /* TODO: Should probably get a count based on how many tokens we have */ 
    result = dax_tag_handle(ds, &handle, name, 0);
    if(result) {
        /* TODO: More descriptive error messages here based on result */
        fprintf(stderr, "ERROR: %s Not a Valid Tag\n", tokens[0]);
        return ERR_ARG;
    }

    buff = malloc(handle.size);
    mask = malloc(handle.size);
    if(buff == NULL || mask == NULL) {
        if(buff) free(buff);
        if(mask) free(mask);
        fprintf(stderr, "ERROR: Unable to Allocate Memory\n");
        return ERR_ALLOC;
    }
    bzero(buff, handle.size);
    bzero(mask, handle.size);
    
    points = tcount - 1;
    if(handle.count < points) {
        points = handle.count;
    }
    /* TODO: I might want to add the ability to skip points with a '-'
     * I'd have to search for any '-' and then use dax_mask_tag() instead. */
        
    for(n = 0; n < points; n++) {
        string_to_dax(tokens[n + 1], handle.type, buff, mask, n);
    }
    /* TODO: Check if the mask is all 1s and if so just use the dax_write_tag() function */
    result = dax_mask_tag(ds, handle, buff, mask);
                
    if(result) {
        fprintf(stderr, "ERROR: Unable to Write to tag %s\n", name);
    }
    free(buff);
    free(mask);
    return 0;
}

/* adds a compound datatype to the server.
 * Usage:
 * >cdt typename mem1name mem1type mem1count mem2name mem2type mem2count ...
 */
int
cdt_add(char **tokens, int tcount)
{
    dax_cdt *type;
    tag_type memtype;
    int count, n;
    int result;
    
    if(tcount < 4 || (tcount - 1) % 3 != 0) {
        fprintf(stderr, "ERROR: Wrong number of arguments\n");
        return ERR_ARG;
    }
    type = dax_cdt_new(tokens[0], &result);
    
    if(type == 0) {
        fprintf(stderr, "ERROR: Unable to create type %s\n", tokens[0]);
        return result;
    }
    for(n = 1; n < tcount - 2; n += 3) {
        memtype = dax_string_to_type(ds, tokens[n+1]);
        count = strtol(tokens[n+2], NULL, 0);
        result = dax_cdt_member(ds, type, tokens[n], memtype, count);
    
        if(result) {
            fprintf(stderr, "ERROR: Unable to add member %s\n", tokens[n]);
        }
    }
  
    result = dax_cdt_create(ds, type, NULL);
    if(result) {
        fprintf(stderr, "ERROR: Unable to create type %s\n", tokens[0]);
    }
    return 0;
}

static void
_print_cdt_callback(cdt_iter iter, void *udata)
{
    if(udata == NULL) {
        printf("%s\n", iter.name);
    } else {
        printf("%-32s  %s", iter.name, dax_type_to_string(ds, iter.type));
        if(iter.count > 0) {
            printf("[%d]", iter.count);
        }
        printf("\n");
    }
}


int
list_types(char **tokens)
{
    tag_type type;
    
    if(tokens[0] == NULL) {
        dax_cdt_iter(ds, 0, NULL, _print_cdt_callback);
    } else {
        type = dax_string_to_type(ds, tokens[0]);
        dax_cdt_iter(ds, type, &type, _print_cdt_callback);
    }
    return 0;
}
