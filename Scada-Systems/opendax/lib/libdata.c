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

 * This file contains some of the database handling code for the library
 */

#include <libdax.h>
#include <dax/libcommon.h>

/* Tag Cache Handling Code
 * The tag cache is a doubly linked circular list
 * the head pointer is fixed at the top of the list
 * tag searched for will bubble up one item in the list
 * each time it's found.  This will make the top of the list
 * the most searched for tags and the bottom the lesser used tags.
 */

/* TODO: I need a way to invalidate the tag cache so that changes
   to the tag can be put into the tag cache */

int
init_tag_cache(dax_state *ds)
{
    ds->cache_head = NULL;
    ds->cache_limit = strtol(dax_get_attr(ds, "cachesize"), NULL, 0);
    ds->cache_count = 0;
    return 0;
}

/****** FOR TESTING ONLY ********
static void
print_cache(void)
{
    int n;
    tag_cnode *this;
    this = _cache_head;
    printf("_cache_head->handle = {%d}\n", _cache_head->handle);
    for(n=0; n<_cache_count; n++) {
        printf("{%d} - %s\t\t", this->handle, this->name);
        printf("this->prev = {%d}", this->prev->handle);
        printf(" : this->next = {%d}\n", this->next->handle);
        this = this->next;
    }
    printf("\n{ count = %d, head = %p }\n", _cache_count, _cache_head);
    //printf("{ head->handle = %d, tail->handle = %d head->prev = %p, tail->next = %p}\n", 
    //       _cache_head->handle, _cache_tail->handle, _cache_head->prev, _cache_tail->next);
}
*/
 
/* This function assigns the data to *tag and bubbles
   this up one node in the list */
static inline void
_cache_hit(dax_state *ds, tag_cnode *this, dax_tag *tag)
{
    tag_cnode *before, *after;
    
    /* Store the return values in tag */
    strcpy(tag->name, this->name);
    tag->idx = this->idx;
    tag->type = this->type;
    tag->count = this->count;
    
    /* Bubble up:
     'after' is set to the node that we swap with
     'before' is set to the node that will be before
     us after the swap.  The special case is when our
     node will become the first one, then we move the head. */
    if(this != ds->cache_head) { /* Do nothing if we are already at the top */
        /* If there are only two then we do it the easy way */
        if(ds->cache_count == 2) {
            ds->cache_head = ds->cache_head->next;
        } else {
            if(this == ds->cache_head->next) ds->cache_head = this;
            after = this->prev;
            before = after->prev;
            before->next = this;
            after->prev = this;
            after->next = this->next;
            this->next->prev = after;
            this->next = after;
            this->prev = before;
        }
    }
    //--print_cache();
}

/* Used to check if a tag with the given handle is in the 
 * cache.  Returns a pointer to the tag if found and
 * returns ERR_NOTFOUND otherwise */
int
check_cache_index(dax_state *ds, tag_index idx, dax_tag *tag)
{
    tag_cnode *this;
    
    if(ds->cache_head != NULL) {
        this = ds->cache_head;
    } else {
        return ERR_NOTFOUND;
    }
    if(ds->cache_head->idx != idx) {
        this = this->next;
        
        while(this != ds->cache_head && this->idx != idx) {
            this = this->next;
        }
        if(this == ds->cache_head) {
            return ERR_NOTFOUND;
        }
    }
    
    _cache_hit(ds, this, tag);
    
    return 0;
}

/* Used to check if a tag with the given name is in the cache
 * If it is found then a pointer to the tag is returned and
 * ERR_NOTFOUND otherwise */
int
check_cache_name(dax_state *ds, char *name, dax_tag *tag)
{
    tag_cnode *this;
    
    if(ds->cache_head != NULL) {
        this = ds->cache_head;
    } else {
        return ERR_NOTFOUND;
    }
    if( strcmp(ds->cache_head->name, name) ) {
        this = this->next;
        
        while(this != ds->cache_head && strcmp(this->name, name)) {
            this = this->next;
        }
        if(this == ds->cache_head) {
            return ERR_NOTFOUND;
        }
    }
    
    _cache_hit(ds, this, tag);
    
    return 0;
}


/* Adds a tag to the cache */
int
cache_tag_add(dax_state *ds, dax_tag *tag)
{
    tag_cnode *new;
    
    if(ds->cache_head == NULL) { /* First one */
        //--printf("adding {%d} to the beginning\n", tag->handle);
        new = malloc(sizeof(tag_cnode));
        if(new) {
            new->next = new;
            new->prev = new;
            ds->cache_head = new;
            ds->cache_count++;
        } else {
            return ERR_ALLOC;
        }
    } else if(ds->cache_count < ds->cache_limit) { /* Add to end */
        //--printf("adding {%d} to the end\n", tag->handle);
        new = malloc(sizeof(tag_cnode));
        if(new) {
            new->next = ds->cache_head;
            new->prev = ds->cache_head->prev;
            ds->cache_head->prev->next = new;
            ds->cache_head->prev = new;
            ds->cache_count++;
        } else {
            return ERR_ALLOC;
        }
    } else {
        //--printf("Just putting {%d}  last\n", tag->handle);
        new = ds->cache_head->prev;
    }
    strcpy(new->name, tag->name);
    new->idx = tag->idx;
    new->type = tag->type;
    new->count = tag->count;
    //--print_cache();
    
    return 0;
}

/* This function deletes the tag in the cache given by 'tagname'
 * It returns 0 on success and ERR_NOTFOUND if the tag is not in the cache */
int
cache_tag_del(dax_state *ds, char *tagname) {
    tag_cnode *this, *head;
    
    /* Search for the tag */
    if(ds->cache_head != NULL) {
        head = ds->cache_head;
        this = head;
    } else {
        return ERR_NOTFOUND;
    }
    if( strcmp(head->name, tagname) ) {
        this = this->next;
        
        while(this != head && strcmp(this->name, tagname)) {
            this = this->next;
        }
        if(this == head) {
            return ERR_NOTFOUND;
        }
    }
    
    /* If we get this far the tag was found and 'this' should point to
     * the tag we want to delete */ 
    if(this->next == this) { /* Last module */
        head = NULL;
    } else {
        /* disconnect module */
        (this->next)->prev = this->prev;
        (this->prev)->next = this->next;
        /* don't want current to be pointing to the one we are deleting */
        if(head == this) {
            head = this->next;
        }
    }
    ds->cache_count--;
    /* free allocated memory */
    free(this);
    return 0;
}


/* Type specific reading and writing functions.  These should be the most common
 * methods to read and write tags to the sever.*/

/* This is a recursive function that traverses the *data and makes the proper
 * data conversions based on the data type. */
static inline int
_read_format(dax_state *ds, tag_type type, int count, void *data, int offset)
{
    int n, pos, result;
    char *newdata;
    datatype *dtype = NULL;
    cdt_member *this = NULL;
    
    newdata = (char *)data + offset;
    if(IS_CUSTOM(type)) {
        /* iterate through the list */
        dtype = get_cdt_pointer(ds, type, NULL);
        pos = offset;
        if(dtype != NULL) {
            this = dtype->members;
        } else {
            return ERR_NOTFOUND;
        }
        while(this != NULL) {
            result = _read_format(ds, this->type, this->count, data, pos);
            if(result) return result;
            if(IS_CUSTOM(this->type)) {
                pos += (dax_get_typesize(ds, this->type) * this->count);
            } else {
                /* This gets the size in bits */
                pos += TYPESIZE(this->type) * this->count / 8;
            }
            this = this->next;
        }
    } else {
        switch(type) {
            case DAX_BOOL:
            case DAX_BYTE:
            case DAX_SINT:
                /* Since there are no conversions for byte level tags we do nothing */
                break;
            case DAX_WORD:
            case DAX_UINT:
                for(n = 0; n < count; n++) {
                    ((dax_uint *)newdata)[n] = stom_uint(((dax_uint *)newdata)[n]);
                }
                break;
            case DAX_INT:
                for(n = 0; n < count; n++) {
                    ((dax_int *)newdata)[n] = stom_int(((dax_int *)newdata)[n]);
                }
                break;
            case DAX_DWORD:
            case DAX_UDINT:
            case DAX_TIME:
                for(n = 0; n < count; n++) {
                    ((dax_udint *)newdata)[n] = stom_udint(((dax_udint *)newdata)[n]);
                }
                break;
            case DAX_DINT:
                for(n = 0; n < count; n++) {
                    ((dax_dint *)newdata)[n] = stom_dint(((dax_dint *)newdata)[n]);
                }
                break;
            case DAX_REAL:
                for(n = 0; n < count; n++) {
                    ((dax_real *)newdata)[n] = stom_real(((dax_real *)newdata)[n]);
                }
                break;
            case DAX_LWORD:
            case DAX_ULINT:
                for(n = 0; n < count; n++) {
                    ((dax_ulint *)newdata)[n] = stom_ulint(((dax_ulint *)newdata)[n]);
                }
                break;
            case DAX_LINT:
                for(n = 0; n < count; n++) {
                    ((dax_lint *)newdata)[n] = stom_lint(((dax_lint *)newdata)[n]);
                }
                break;
            case DAX_LREAL:
                for(n = 0; n < count; n++) {
                    ((dax_lreal *)newdata)[n] = stom_lreal(((dax_lreal *)newdata)[n]);
                }
                break;
            default:
                return ERR_ARG;
                break;
        }    
    }
    return 0;
}


int
dax_read_tag(dax_state *ds, Handle handle, void *data)
{
    int result, n, i;
    u_int8_t *newdata;
    
    result = dax_read(ds, handle.index, handle.byte, data, handle.size);
    if(result) return result;
    
    /* The only time that the bit index should be greater than 0 is if
     * the tag datatype is BOOL.  If not the bytes should be aligned.
     * If there is a bit index then we need to 'realign' the bits so that
     * the bits that the handle point to start at the top of the *data buffer */
    if(handle.type == DAX_BOOL) {
        i = handle.bit;
        newdata = malloc(handle.size);
        if(newdata == NULL) return ERR_ALLOC;
        bzero(newdata, handle.size);
        for(n = 0; n < handle.count; n++) {
            if( (0x01 << (i % 8)) & ((u_int8_t *)data)[i / 8] ) {
                ((u_int8_t *)newdata)[n / 8] |= (1 << (n % 8));
            }
            i++;
        }
        memcpy(data, newdata, handle.size);
        free(newdata);
    } else {
        libdax_lock(ds->lock);
        result = _read_format(ds, handle.type, handle.count, data, 0);
        libdax_unlock(ds->lock);
        return result;
    }
    return 0;
}


/* This reformats the information in *data to prepare it to be written
 * to the server by changing the byte ordering and number format if
 * necessary to match the server */
static inline int
_write_format(dax_state *ds, tag_type type, int count, void *data, int offset)
{
    int n, pos, result;
    char *newdata;
    datatype *dtype = NULL;
    cdt_member *this = NULL;
    
    newdata = (char *)data + offset;

    if(IS_CUSTOM(type)) {
        /* iterate through the list */
        dtype = get_cdt_pointer(ds, type, NULL);
        pos = offset;
        if(dtype != NULL) {
            this = dtype->members;
        } else {
            return ERR_NOTFOUND;
        }
        while(this != NULL) {
            result = _write_format(ds, this->type, this->count, newdata, pos);
            if(result) return result;
            if(IS_CUSTOM(this->type)) {
                pos += (dax_get_typesize(ds, this->type) * this->count);
            } else {
                /* This gets the size in bits */
                pos += TYPESIZE(this->type) * this->count / 8;
            }
            this = this->next;
        }
    } else {
        switch(type) {
            case DAX_BOOL:
            case DAX_BYTE:
            case DAX_SINT:
                break;
            case DAX_WORD:
            case DAX_UINT:
                for(n = 0; n < count; n++) {
                    ((dax_uint *)newdata)[n] = mtos_uint(((dax_uint *)newdata)[n]);
                }
                break;
            case DAX_INT:
                for(n = 0; n < count; n++) {
                    ((dax_int *)newdata)[n] = mtos_int(((dax_int *)newdata)[n]);
                }
                break;
            case DAX_DWORD:
            case DAX_UDINT:
            case DAX_TIME:
                for(n = 0; n < count; n++) {
                    ((dax_udint *)newdata)[n] = mtos_udint(((dax_udint *)newdata)[n]);
                }
                break;
            case DAX_DINT:
                for(n = 0; n < count; n++) {
                    ((dax_dint *)newdata)[n] = mtos_dint(((dax_dint *)newdata)[n]);
                }
                break;
            case DAX_REAL:
                for(n = 0; n < count; n++) {
                    ((dax_real *)newdata)[n] = mtos_real(((dax_real *)newdata)[n]);
                }
                break;
            case DAX_LWORD:
            case DAX_ULINT:
                for(n = 0; n < count; n++) {
                    ((dax_ulint *)newdata)[n] = mtos_ulint(((dax_ulint *)newdata)[n]);
                }
                break;
            case DAX_LINT:
                for(n = 0; n < count; n++) {
                    ((dax_lint *)newdata)[n] = mtos_lint(((dax_lint *)newdata)[n]);
                }
                break;
            case DAX_LREAL:
                for(n = 0; n < count; n++) {
                    ((dax_lreal *)newdata)[n] = mtos_lreal(((dax_lreal *)newdata)[n]);
                }
                break;
            default:
                return ERR_ARG;
                break;
        }
    }
    return 0;
}


int
dax_write_tag(dax_state *ds, Handle handle, void *data)
{
    int i, n, result = 0;
    u_int8_t *mask, *newdata;
    
    if(handle.type == DAX_BOOL && handle.bit > 0) {
        mask = malloc(handle.size);
        if(mask == NULL) return ERR_ALLOC;
        newdata = malloc(handle.size);
        if(newdata == NULL) {
            free(mask);
            return ERR_ALLOC;
        }
        bzero(mask, handle.size);
        bzero(newdata, handle.size);
            
        i = handle.bit % 8;
        for(n = 0; n < handle.count; n++) {
            if( (0x01 << (n % 8)) & ((u_int8_t *)data)[n / 8] ) {
                ((u_int8_t *)newdata)[i / 8] |= (1 << (i % 8));
            }
            mask[i / 8] |= (1 << (i % 8));
            i++;
        }
        result = dax_mask(ds, handle.index, handle.byte, newdata, mask, handle.size);
        free(newdata);
        free(mask);
    } else {
        libdax_lock(ds->lock);
        result =  _write_format(ds, handle.type, handle.count, data, 0);
        if(result) {
            libdax_unlock(ds->lock);
            return result;
        }
        /* Unlock here because dax_write() has it's own locking */
        libdax_unlock(ds->lock);
        result = dax_write(ds, handle.index, handle.byte, data, handle.size);
    }
    return result;
}


int
dax_mask_tag(dax_state *ds, Handle handle, void *data, void *mask)
{
    int i, n, result = 0;
    u_int8_t *newmask = NULL, *newdata;

    if(handle.type == DAX_BOOL && handle.bit > 0) {
        newmask = malloc(handle.size);
        if(newmask == NULL) return ERR_ALLOC;
        newdata = malloc(handle.size);
        if(newdata == NULL) {
            free(newmask);
            return ERR_ALLOC;
        }
        bzero(newmask, handle.size);
        bzero(newdata, handle.size);

        i = handle.bit % 8;
        for(n = 0; n < handle.count; n++) {
            if( (0x01 << (n % 8)) & ((u_int8_t *)data)[n / 8] ) {
                ((u_int8_t *)newdata)[i / 8] |= (1 << (i % 8));
            }
            newmask[i / 8] |= (1 << (i % 8));
            i++;
        }
        result = dax_mask(ds, handle.index, handle.byte, newdata, newmask, handle.size);
        free(newmask);
        free(newdata);
    } else {
        libdax_lock(ds->lock);
        result =  _write_format(ds, handle.type, handle.count, data, 0);
        if(result) {
            libdax_unlock(ds->lock);
            return result;
        }
        /* Unlock here because dax_mask() has it's own locking */
        libdax_unlock(ds->lock);
        result = dax_mask(ds, handle.index, handle.byte, data, mask, handle.size);
    }
    return result;
}
