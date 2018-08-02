/*  OpenDAX - An open source data acquisition and control system
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

 * This is the main source file for the daxlua library.
 * The daxlua library contains OpenDAX API bindings for
 * the Lua programming language.
 */

#include <libdaxlua.h>

static dax_state *ds;

/* This is just a convienience since I need to pass multiple pieces
 * of data back from the cdt_iter callback. */
struct iter_udata {
    lua_State *L;
    void *data;
    void *mask;
    int error;
};

/* This function figures out what type of data the tag is and translates
 * buff appropriately and pushes the value onto the lua stack */
static inline void
_read_to_stack(lua_State *L, tag_type type, void *buff)
{
    switch (type) {
        /* Each number has to be cast to the right datatype then dereferenced
           and then cast to double for pushing into the Lua stack */
        case DAX_BYTE:
            lua_pushnumber(L, (double)*((dax_byte *)buff));
            break;
        case DAX_SINT:
            lua_pushnumber(L, (double)*((dax_sint *)buff));
            break;
        case DAX_WORD:
        case DAX_UINT:
            lua_pushnumber(L, (double)*((dax_uint *)buff));
            break;
        case DAX_INT:
            lua_pushnumber(L, (double)*((dax_int *)buff));
            break;
        case DAX_DWORD:
        case DAX_UDINT:
        case DAX_TIME:
            lua_pushnumber(L, (double)*((dax_udint *)buff));
            break;
        case DAX_DINT:
            lua_pushnumber(L, (double)*((dax_dint *)buff));
            break;
        case DAX_REAL:
            lua_pushnumber(L, (double)*((dax_real *)buff));
            break;
        case DAX_LWORD:
        case DAX_ULINT:
            lua_pushnumber(L, (double)*((dax_ulint *)buff));
            break;
        case DAX_LINT:
            lua_pushnumber(L, (double)*((dax_lint *)buff));
            break;
        case DAX_LREAL:
            lua_pushnumber(L, *((dax_lreal *)buff));
            break;
    }
}


static void
_push_base_datatype(lua_State *L, cdt_iter tag, void *data)
{
    int n, bit;
    
    /* We have to treat Booleans differently */
    if(tag.type == DAX_BOOL) {
        /* Check to see if it's an array */
        if(tag.count > 1 ) {
            lua_createtable(L, tag.count, 0);
            for(n = 0; n < tag.count ; n++) {
                bit = n + tag.bit;
                if(((u_int8_t *)data)[bit/8] & (1 << (bit % 8))) { /* If *buff bit is set */
                    lua_pushboolean(L, 1);
                } else {  /* If the bit in the buffer is not set */
                    lua_pushboolean(L, 0);
                }
                lua_rawseti(L, -2, n + 1);
            }
        } else {
            if(((u_int8_t *)data)[tag.bit/8] & (1 << (tag.bit % 8))) { /* If *buff bit is set */
                lua_pushboolean(L, 1);
            } else {  /* If the bit in the buffer is not set */
                lua_pushboolean(L, 0);
            }
        }
    } else { /* Not a boolean */
        /* Push the data up to the lua interpreter stack */
        if(tag.count > 1) { /* We need to return a table */
            lua_createtable(L, tag.count, 0);
            for(n = 0; n < tag.count ; n++) {
                _read_to_stack(L, tag.type, data + (TYPESIZE(tag.type) / 8) * n);
                lua_rawseti(L, -2, n + 1); /* Lua likes 1 indexed arrays */
            }
        } else { /* It's a single value */
            _read_to_stack(L, tag.type, data);
        }
    }
}

/* When the dax_cdt_iter() function that refers to this function returns
 * there should be a table at the top of the stack that represents the
 * contents of the member given by 'member' */
static void
_read_callback(cdt_iter member, void *udata)
{
    lua_State *L = ((struct iter_udata *)udata)->L;
    unsigned char *data = ((struct iter_udata *)udata)->data;
    int offset, n;
    struct iter_udata newdata;

    lua_pushstring(L, member.name);
            
    if(IS_CUSTOM(member.type)) {
        lua_newtable(L);
        newdata.L = L;
        newdata.mask = NULL;
        newdata.error = 0;
        
        if(member.count > 1) {
            for(n = 0;n < member.count; n++) {
                lua_newtable(L);
                offset = member.byte + (n * dax_get_typesize(ds, member.type));
                newdata.data = data + offset;
                dax_cdt_iter(ds, member.type, &newdata , _read_callback);
                lua_rawseti(L, -2, n + 1);
            }
        } else {
            newdata.data = data + member.byte;
            dax_cdt_iter(ds, member.type, &newdata, _read_callback);
        }
    } else {
        _push_base_datatype(L, member, data + member.byte);
    }
    lua_rawset(L, -3);
}

/* This is the top level function for taking the data that is is in *data,
 * iterating through the tag give by handle 'h' and storing that information
 * into a Lua variable on the top of the Lua stack. */
static void
_send_tag_to_lua(lua_State *L, Handle h, void *data)
{
    cdt_iter tag;
    struct iter_udata udata;
    int offset, n;
    
    udata.L = L;
    udata.data = data;
    udata.mask = NULL;
    udata.error = 0;

    if(IS_CUSTOM(h.type)) {
        lua_newtable(L);
        
        if(h.count > 1) {
            for(n = 0; n < h.count; n++) {
                lua_newtable(L);    
                offset = n * dax_get_typesize(ds, h.type);
                udata.data = (char *)data + offset;
                dax_cdt_iter(ds, h.type, &udata, _read_callback);
                lua_rawseti(L, -2, n+1);
            }
        } else {
            dax_cdt_iter(ds, h.type, &udata, _read_callback);
        }
    } else {
        tag.count = h.count;
        tag.type = h.type;
        tag.byte = 0;
        tag.bit = 0;
        _push_base_datatype(L, tag, data);
    } 
}

/* Here begin the tag writing functions.  */

/* Takes the Lua value at the top of the stack, converts it and places it
 * in the proper place in data.  The mask is set as well.  */
static inline void
_write_from_stack(lua_State *L, unsigned int type, void *data, void *mask, int index)
{
    lua_Integer x;
    
    assert(mask != NULL);
    switch (type) {
        case DAX_BYTE:
            x = lua_tointeger(L, -1) % 256;
            ((dax_byte *)data)[index] = x;
            ((dax_byte *)mask)[index] = 0xFF;
            break;
        case DAX_SINT:
            x = lua_tointeger(L, -1) % 256;
            ((dax_sint *)data)[index] = x;
            ((dax_sint *)mask)[index] = 0xFF;
            break;
        case DAX_WORD:
        case DAX_UINT:
            x = lua_tointeger(L, -1);
            ((dax_uint *)data)[index] = x;
            ((dax_uint *)mask)[index] = 0xFFFF;
            break;
        case DAX_INT:
            x = lua_tointeger(L, -1);
            ((dax_int *)data)[index] = x;
            ((dax_int *)mask)[index] = 0xFFFF;
            break;
        case DAX_DWORD:
        case DAX_UDINT:
        case DAX_TIME:
            x = lua_tointeger(L, -1);
            ((dax_udint *)data)[index] = x;
            ((dax_udint *)mask)[index] = 0xFFFFFFFF;
            break;
        case DAX_DINT:
            x = lua_tointeger(L, -1);
            ((dax_dint *)data)[index] = x;
            ((dax_dint *)mask)[index] = 0xFFFFFFFF;
            break;
        case DAX_REAL:
            ((dax_real *)data)[index] = (dax_real)lua_tonumber(L, -1);
            ((u_int32_t *)mask)[index] = 0xFFFFFFFF;
            break;
        case DAX_LWORD:
        case DAX_ULINT:
            x = lua_tointeger(L, -1);
            ((dax_ulint *)data)[index] = x;
            ((dax_ulint *)mask)[index] = DAX_64_ONES;
            break;
        case DAX_LINT:
            x = lua_tointeger(L, -1);
            ((dax_lint *)data)[index] = x;
            ((dax_lint *)mask)[index] = DAX_64_ONES;
            break;
        case DAX_LREAL:
            ((dax_lreal *)data)[index] = lua_tonumber(L, -1);
            ((u_int64_t *)mask)[index] = DAX_64_ONES;
            break;
    }
}


/* This function reads the variable from the top of the Lua stack
   and sends it to the opendax tag given by *tagname */
int
_pop_base_datatype(lua_State *L, cdt_iter tag, void *data, void *mask)
{
    int n, bit;
    
    //printf("_pop_base_datatype() called with *data = %p\n", data);
    if(tag.count > 1) { /* The tag is an array */
        /* Check that the second parameter is a table */
        if( ! lua_istable(L, -1) ) {
            lua_pushfstring(L, "Table needed to set - %s", tag.name);
            return -1;
        }
        /* We're just searching for indexes in the table.  Anything
         * other than numerical indexes in the table don't count */
        for(n = 0; n < tag.count; n++) {
            lua_rawgeti(L, -1, n + 1);
            if(! lua_isnil(L, -1)) {
                if(tag.type == DAX_BOOL) {
                    /* Handle the boolean */
                    bit = n + tag.bit;
                    if(lua_toboolean(L, -1)) {
                        ((u_int8_t *)data)[bit/8] |= (1 << (bit % 8));
                    } else {  /* If the bit in the buffer is not set */
                        ((u_int8_t *)data)[bit/8] &= ~(1 << (bit % 8));
                    }
                    ((u_int8_t *)mask)[bit/8] |= (1 << (bit % 8));
                } else {
                    /* Handle the non-boolean */
                    _write_from_stack(L, tag.type, data, mask, n);
                }
            }
            lua_pop(L, 1);
        }
    } else { /* Retrieved tag is a single point */
        if(tag.type == DAX_BOOL) {
            bit = tag.bit;
//            printf("tag.bit = %d \n", tag.bit);
//            printf("Before Data[%d] = 0x%X\n", bit/8, ((u_int8_t *)data)[bit/8]);
//            printf("Before Mask[%d] = 0x%X\n", bit/8, ((u_int8_t *)mask)[bit/8]);
            if(lua_toboolean(L, -1)) {
//                printf("set to TRUE\n");
                ((u_int8_t *)data)[bit/8] |= (1 << (bit % 8));
            } else {  /* If the bit in the buffer is not set */
//                printf("set to FALSE\n");
                ((u_int8_t *)data)[bit/8] &= ~(1 << (bit % 8));
            }
            ((u_int8_t *)mask)[bit/8] |= (1 << (bit % 8));
//            printf("After Data[%d] = 0x%X\n", bit/8, ((u_int8_t *)data)[bit/8]);
//            printf("After Mask[%d] = 0x%X\n", bit/8, ((u_int8_t *)mask)[bit/8]);
            
        } else {
            _write_from_stack(L, tag.type, data, mask, 0);
        }
    }
    return 0;
}

/* This is the recursive callback function that is passed to
 * dax_cdt_iter() to handle the compound data types.  If the
 * type has nested cdt's then this function will call itself */
static void
_write_callback(cdt_iter member, void *udata)
{
    struct iter_udata newdata;
    
    lua_State *L = ((struct iter_udata *)udata)->L;
    unsigned char *data = ((struct iter_udata *)udata)->data;
    unsigned char *mask = ((struct iter_udata *)udata)->mask;
    int offset, n, result = 0;
    
    if(IS_CUSTOM(member.type)) {
        newdata.L = L;
        newdata.error = 0;
        lua_pushstring(L, member.name);
        lua_rawget(L, -2);
        if(member.count > 1) {
            for(n = 0;n < member.count; n++) {
                offset = member.byte + (n * dax_get_typesize(ds, member.type));
                newdata.data = (char *)data + offset;
                newdata.mask = (char *)mask + offset;
                if( ! lua_istable(L, -1) ) {
                    lua_pushfstring(L, "Table needed to set - %s", member.name);
                    ((struct iter_udata *)udata)->error = -1;
                    return;
                }
                lua_rawgeti(L, -1, n+1);
                if(! lua_isnil(L, -1)) {
                    if( ! lua_istable(L, -1) ) {
                        lua_pop(L, 1);
                        lua_pushfstring(L, "Table needed to set - %s", member.name);
                        ((struct iter_udata *)udata)->error = -1;
                        return;
                    }
                    dax_cdt_iter(ds, member.type, &newdata , _write_callback);
                }
                lua_pop(L, 1);
                if(newdata.error) {
                    ((struct iter_udata *)udata)->error = newdata.error;
                    return;
                }
            }
        } else {
            newdata.data = (char *)data + member.byte;
            newdata.mask = (char *)mask + member.byte;
            if(! lua_isnil(L, -1)) {
                if( ! lua_istable(L, -1) ) {
                    lua_pushfstring(L, "Table needed to set - %s", member.name);
                    ((struct iter_udata *)udata)->error = -1;
                    return;
                }
                dax_cdt_iter(ds, member.type, &newdata, _write_callback);
            }
            if(newdata.error) {
                ((struct iter_udata *)udata)->error = newdata.error;
                return;
            }
        }
        lua_pop(L, 1);
    } else {
        lua_pushstring(L, member.name);
        lua_rawget(L, -2);
        if(! lua_isnil(L, -1)) {
            result = _pop_base_datatype(L, member, data + member.byte, mask + member.byte);
        }
        lua_pop(L, 1);
    }
    if(result) {
        ((struct iter_udata *)udata)->error = result;
    }
}

/* This function takes care of the top level of the tag.  If the tag
 * is a simple base datatype tag then the _pop_base_datatype() function
 * is called directly and write is complete.  If the tag is a compound
 * datatype then the top level is taken care of and then it's turned
 * over to the recursive write_callback() function through the
 * cdt iterator. */
static int
_get_tag_from_lua(lua_State *L, Handle h, void* data, void *mask){
    cdt_iter tag;
    struct iter_udata udata;
    int n, offset;
    
    if(IS_CUSTOM(h.type)) {
        udata.L = L;
        udata.error = 0;
        if(h.count > 1) {
            for(n = 0; n < h.count; n++) {
                offset = n * dax_get_typesize(ds, h.type);
                udata.data = (char *)data + offset;
                udata.mask = (char *)mask + offset;
                if( ! lua_istable(L, -1) ) {
                    lua_pushfstring(L, "Table needed to set - %s", tag.name);
                    return -1;
                }
                lua_rawgeti(L, -1, n+1);
                if(! lua_isnil(L, -1)) {
                    if( ! lua_istable(L, -1) ) {
                        lua_pop(L, 1);
                        lua_pushstring(L, "Table needed to set tag");
                        return -1;
                    }
                    dax_cdt_iter(ds, h.type, &udata , _write_callback);
                }
                lua_pop(L, 1);
                if(udata.error) {
                    return udata.error;
                }
            }
        } else {
            udata.data = data;
            udata.mask = mask;
            if(! lua_isnil(L, -1)) {
                if( ! lua_istable(L, -1) ) {
                    lua_pushstring(L, "Table needed to set Tag");
                    return -1;
                }
                dax_cdt_iter(ds, h.type, &udata, _write_callback);
            }
            if(udata.error) {
                return udata.error;
            }
        }
    } else {
        tag.count = h.count;
        tag.type = h.type;
        tag.byte = 0;
        tag.bit = 0;
        return _pop_base_datatype(L, tag, data, mask);
    }
    return 0;
}

/* These are the functions that get exported */

static int
_dax_init(lua_State *L)
{
    char *modulename;
    modulename = (char *)lua_tostring(L, 1);
    /* TODO: Transfer the Lua 'arg' table to these variables */
    int argc = 1;
    char *argv[] = {modulename};
    
    /* Create and Initialize the OpenDAX library state object */
    ds = dax_init(modulename);
    if(ds == NULL) {
        luaL_error(L, "Unable to allocate memory for dax_state object");
    }

    /* Create and initialize the configuration subsystem in the library */
    dax_init_config(ds, modulename);
    /* We don't really do any custom configuration at this point */
    
    /* Execute the configuration */
    dax_configure(ds, argc, argv, CFG_CMDLINE | CFG_DAXCONF);

    /* Free the configuration data */
    dax_free_config (ds);

    /* Check for OpenDAX and register the module */
    if( dax_connect(ds) ) {
        luaL_error(L, "Unable to find OpenDAX Server");
    }

    return 0;
}

static int
_dax_free(lua_State *L)
{
    if( dax_disconnect(ds) ) {
        luaL_error(L, "Problem Unregistering from the server");
    }
    dax_free(ds);
    return 0;
}

/* Wrapper function for the cdt creation functions */
/* The Lua function should be passed two arguments.  The
 * first is the name of the datatype and the second is
 * a table of tables that represent all the members of the
 * compound datatype.  It should be formatted like this...
 * members = {{"Name", "DataType", count},
 *            {"AnotherNmae", "DataType", count}} */
static int
_cdt_create(lua_State *L)
{
    int count, n = 1, result;
    dax_cdt *cdt;
    tag_type type;
    char *name, *typename, *cdt_name;
    
    if(ds == NULL) {
        luaL_error(L, "OpenDAX is not initialized");
    }
    if(lua_gettop(L) != 2) {
        luaL_error(L, "Wrong number of arguments to cdt_create()");
    }
    cdt_name = (char *)lua_tostring(L, 1);
    cdt = dax_cdt_new(cdt_name, NULL);
    
    if(cdt == 0) {
        luaL_error(L, "Unable to create datatype %s", lua_tostring(L, 1));
    }
    
    if(! lua_istable(L, 2) ) {
        luaL_error(L, "Should pass a table to tag_handle_test()");
    }
    
    while(1) {
        /* Get the table from the argument */
        lua_rawgeti(L, 2, n++);
        if(lua_isnil(L, -1)) break;
        lua_rawgeti(L, 3, 1);
        name = (char *)lua_tostring(L, -1);
        lua_rawgeti(L, 3, 2);
        typename = (char *)lua_tostring(L, -1);
        lua_rawgeti(L, 3, 3);
        count = lua_tointeger(L, -1);
        lua_pop(L, 4);
        
        result = dax_cdt_member(ds, cdt, name, dax_string_to_type(ds, typename), count);
        if(result) {
            dax_cdt_free(cdt);
            luaL_error(L, "Unable to add member %s", name);
        }
    }
    result = dax_cdt_create(ds, cdt, &type);
    if(result) {
        dax_cdt_free(cdt);
        luaL_error(L, "Unable to create datatype %s", cdt_name);
    }
    
    lua_pushinteger(L, type);
    return 1;
}


/* Adds a tag to the dax tagbase.  Three arguments...
    First - tagname
    Second - tag type
    Third - count
    Returns the handle on success
    Raises an error on failure
*/
static int
_tag_add(lua_State *L)
{
    int result;
    tag_type type;
    
    if(ds == NULL) {
        luaL_error(L, "OpenDAX is not initialized");
    }
    if(lua_gettop(L) != 3) {
        luaL_error(L, "wrong number of arguments to tag_add()");
    }
    if(lua_isnumber(L, 2)) {
        type = lua_tointeger(L, 2);
    } else {
        type = dax_string_to_type(ds, (char *)lua_tostring(L, 2));
        if(type == 0) {
            luaL_error(L, "Can't get type '%s'", (char *)lua_tostring(L, 2));      
        }
    }

    result = dax_tag_add(ds, NULL, (char *)lua_tostring(L,1), type, lua_tointeger(L, 3));
    if(result) luaL_error(L, "Unable to add tag '%s'", (char *)lua_tostring(L,1));
    return 0;
}

/* Wrapper for the two tag retrieving functions */
static int
_tag_get(lua_State *L)
{
    int result;
    dax_tag tag;

    if(ds == NULL) {
        luaL_error(L, "OpeOnDAX is not initialized");
    }
    if(lua_isnumber(L, 1)) {
        result = dax_tag_byindex(ds, &tag, (tag_index)lua_tointeger(L, 1));
    } else {
        result = dax_tag_byname(ds, &tag, (char *)lua_tostring(L, 1));
    }
    if(result != 0) {
        luaL_error(L, "Can't get tag '%s'", (char *)lua_tostring(L, 2));      
    }
    lua_pushstring(L, tag.name);
    lua_pushstring(L, dax_type_to_string(ds, tag.type));
    lua_pushinteger(L, tag.count);
    return 3;
}


/* These are the main data transfer functions.  These are wrappers for
 * the dax_read/write/mask function. */
static int
_tag_read(lua_State *L) {
    char *name;
    int count, result;
    Handle h;
    void *data;
    
    if(ds == NULL) {
        luaL_error(L, "OpenDAX is not initialized");
    }
    if(lua_gettop(L) != 2) {
        luaL_error(L, "Wrong number of arguments passed to tag_read()");
    }
    name = (char *)lua_tostring(L, 1);
    count = lua_tointeger(L, 2);
//    printf("_tag_read() Getting Handle for %s with count of %d\n", name, count);

    result = dax_tag_handle(ds, &h, name, count);
    if(result) {
        luaL_error(L, "dax_tag_handle() returned %d", result);
    }
//    printf("h.index = %d\n", h.index);
//    printf("h.byte = %d\n", h.byte);
//    printf("h.bit = %d\n", h.bit);
//    printf("h.size = %d\n", h.size);
//    printf("h.count = %d\n", h.count);
//    printf("h.type = %s\n", dax_type_to_string(h.type));
    
    data = malloc(h.size);
    if(data == NULL) {
        luaL_error(L, "tag_read() unable to allocate data area");
    }
    
    result = dax_read_tag(ds, h, data);

    if(result) {
        free(data);
        luaL_error(L, "dax_read_tag() returned %d", result);
    }
    /* This function figures all the tag data out and pushes the right
     * thing onto the top of the Lua stack */
    _send_tag_to_lua(L, h, data);
    
    free(data);
    return 1;
}

static int
_tag_write(lua_State *L) {
    char *name;
    char q = 0;
    int result, n;
    Handle h;
    void *data, *mask;
    
    if(ds == NULL) {
        luaL_error(L, "OpenDAX is not initialized");
    }
    if(lua_gettop(L) != 2) {
        luaL_error(L, "Wrong number of arguments passed to tag_write()");
    }
    name = (char *)lua_tostring(L, 1);
    //printf("Getting Handle for %s\n", name);
    result = dax_tag_handle(ds, &h, name, 0);
//    printf("h.index = %d\n", h.index);
//    printf("h.byte = %d\n", h.byte);
//    printf("h.bit = %d\n", h.bit);
//    printf("h.size = %d\n", h.size);
//    printf("h.count = %d\n", h.count);
//    printf("h.type = %s\n", dax_type_to_string(h.type));
    if(result) {
        luaL_error(L, "dax_tag_handle() returned %d", result);
    }
    
    data = malloc(h.size);
    if(data == NULL) {
        luaL_error(L, "tag_write() unable to allocate data area");
    }

    mask = malloc(h.size);
    if(mask == NULL) {
        free(data);
        luaL_error(L, "tag_write() unable to allocate mask memory");
    }
    bzero(mask, h.size);
    
    result = _get_tag_from_lua(L, h, data, mask);
    if(result) {
        free(data);
        free(mask);
        lua_error(L); /* The error message should already be on top of the stack */
    }
    
    /* This checks the mask to determine which function to use
     * to write the data to the server */
    /* TODO: Might want to scan through and find the beginning and end of
     * any changed data and send less through the socket.  */
    for(n = 0; n < h.size && q == 0; n++) {
        if( ((unsigned char *)mask)[n] != 0xFF) {
            q = 1;
        }
    }
    if(q) {
        //printf("call dax_mask_tag()\n");
        result = dax_mask_tag(ds, h, data, mask);
    } else {
        //printf("call dax_write_tag()\n");
        result = dax_write_tag(ds, h, data);
    }
    
    if(result) {
        free(data);
        free(mask);
        luaL_error(L, "dax_write/mask_tag() returned %d", result);
    }

    free(data);
    free(mask);
    return 1;
}

/* This structure is used to store the indexes in the registry where the
 * callback function and callback data are stored.  A pointer to one of
 * these is what we send to the OpenDAX library to use as the callback
 * for the C part of this deal. */
typedef struct event_ref_data {
    lua_State *L;
    int function;
    int data;
} event_ref_data;

/* This function should be passed a pointer to the event_ref_data structure
 * that represents the lua_State, the Lua function and the argument that we
 * want to pass to the Lua function and then calls the Lua function */
static void
_event_callback(void *data) {
    event_ref_data *rdata;
    rdata = (event_ref_data *)data;
    lua_rawgeti(rdata->L, LUA_REGISTRYINDEX, rdata->function);
    lua_rawgeti(rdata->L, LUA_REGISTRYINDEX, rdata->data);
    lua_call(rdata->L, 1, 0);
}

/* Takes a string as an argument and returns the event type that the
 * string represents.  Returns ERR_ARG if it can't find a match */
static int
_get_event_type(char *str) {
    if(strcasecmp(str, "WRITE") == 0) {
        return EVENT_WRITE;
    } else if(strcasecmp(str, "CHANGE") == 0) {
        return EVENT_CHANGE;
    } else if(strcasecmp(str, "DEADBAND") == 0) {
        return EVENT_DEADBAND;
    } else if(strcasecmp(str, "SET") == 0) {
        return EVENT_SET;
    } else if(strcasecmp(str, "RESET") == 0) {
        return EVENT_RESET;
    } else if(strcasecmp(str, "EQUAL") == 0) {
        return EVENT_EQUAL;
    } else if(strcasecmp(str, "GREATER") == 0) {
        return EVENT_GREATER;
    } else if(strcasecmp(str, "LESS") == 0) {
        return EVENT_LESS;
    } else {
        return ERR_ARG;
    }
}

/* Converts the lua_Number that we would get off of the Lua stack into
 * the proper datatype for DAX.  The pointer that is returned in **data
 * will wind up pointing to a static member within this function.  This
 * is not a general use function for this reason and it is not re-entrant
 * or thread safe. */
static inline void
_convert_lua_number(tag_type datatype, void **data, lua_Number x) {
    static dax_type_union u1;
    
    switch(datatype) {
        case DAX_BYTE:
            u1.dax_byte = (dax_byte)x;
            *data = &u1.dax_byte;
            return;
        case DAX_SINT:
            u1.dax_sint = (dax_sint)x;
            *data = &u1.dax_sint;
            return;
        case DAX_UINT:
        case DAX_WORD:
            u1.dax_uint = (dax_uint)x;
            *data = &u1.dax_uint;
            return;
        case DAX_INT:
            u1.dax_int = (dax_int)x;
            *data = &u1.dax_int;
            return;
        case DAX_UDINT:
        case DAX_DWORD:
        case DAX_TIME:
            u1.dax_udint = (dax_udint)x;
            *data = &u1.dax_udint;
            return;
        case DAX_DINT:
            u1.dax_dint = (dax_dint)x;
            *data = &u1.dax_dint;
            return;
        case DAX_ULINT:
        case DAX_LWORD:
            u1.dax_ulint = (dax_ulint)x;
            *data = &u1.dax_ulint;
            return;
        case DAX_LINT:
            u1.dax_lint = (dax_lint)x;
            *data = &u1.dax_lint;
            return;
        case DAX_REAL:
            u1.dax_real = (dax_real)x;
            *data = &u1.dax_real;
            return;
        case DAX_LREAL:
            u1.dax_lreal = (dax_lreal)x;
            *data = &u1.dax_lreal;
            return;
        default:
            *data = NULL;
    }
    return;
}

/* Used to add an event to the server's event list.  The arguments are...
 * 1 - string - tagname
 * 2 - number - count
 * 3 - string - event type
 * 4 - number - event data
 * 5 - function - callback function
 * 6 - ** - callback data
 * The function returns a table that can be used in other functions.  The
 * Lua script should probably not mess with the members of this table. */ 
static int
_event_add(lua_State *L) {
    char *str;
    int count, type, result;
    lua_Number number;
    Handle h;
    event_ref_data *edata;
    dax_event_id id;
    void *data;

    if(lua_gettop(L) != 6) {
        luaL_error(L, "Wrong number of arguments passed to event_add()");
    }
    if(lua_isnil(L, 5) || lua_tonumber(L, 5) == 0) {
        
    } else {
        if(!lua_isfunction(L, 5)) {
            luaL_error(L, "Argument 5 to event_add() should be a function");
        }
    }
    edata = malloc(sizeof(event_ref_data));
    if(edata == NULL) {
        luaL_error(L, "Unable to allocate memory");
    }
    /* Get tagname and count, then determine the handle */
    str = (char *)lua_tostring(L, 1);
    count = lua_tonumber(L, 2);
    result = dax_tag_handle(ds, &h, str, count);
    if(result) {
        free(edata);
        luaL_error(L, "%s with count %d is not a valid tag", str, count);
    }
    /* Get the event type and any number */
    str = (char *)lua_tostring(L, 3);
    type = _get_event_type(str);
    if(type < 0) {
        free(edata);
        luaL_error(L, "%s is not a valid event type", str);
    }
    /* Get the data that we need for < > = and deadband events. */
    number = lua_tonumber(L, 4);
    _convert_lua_number(h.type, &data, number);
    /* The data that was passed is actually at the top of the stack */
    edata->data = luaL_ref(L, LUA_REGISTRYINDEX); /* Also pops the value */
    /* Now the function should be at the top of the stack */
    edata->function = luaL_ref(L, LUA_REGISTRYINDEX);
    edata->L = L; /* We'll need this later */
    result = dax_event_add(ds, &h, type, data, &id, _event_callback, edata, NULL);
    if(result) {
        free(edata);
        luaL_error(L, "Unable to add event to server - result = %d", result);
    }
    lua_createtable(L, 2, 0);
    lua_pushinteger(L, id.index);
    lua_rawseti(L, -2, 1);
    lua_pushinteger(L, id.id);
    lua_rawseti(L, -2, 2);
    return 1;
}

/* Used to delete an event from the server.  The arguement is a single
 * table that would have been returned from _add_event().  It returns
 * nothing */
static int
_event_del(lua_State *L) {
    dax_event_id id;

    if(!lua_istable(L, 1)) {
        luaL_error(L, "Wrong type of argument passed to event_del()");
    }
    lua_rawgeti(L, 1, 1); /* Get the index from the table */
    id.index = lua_tointeger(L, -1);
    lua_rawgeti(L, 1, 2); /* Get the id from the table */
    id.id = lua_tointeger(L, -1);
    
    dax_event_del(ds, id);
    return 0;
}

/* Wrapper for dax_event_wait().  It takes a single argument that is
 * the timeout in milliseconds.  It returns 0 on timeout and 1 if
 * it dispatches an event */
static int
_event_wait(lua_State *L) {
    int result;
    int timeout;

    if(lua_gettop(L) != 1) {
        luaL_error(L, "Wrong number of arguments passed to event_select()");
    }
    timeout = lua_tonumber(L, 1);
    result = dax_event_wait(ds, timeout, NULL);

    if(result == ERR_TIMEOUT) {
        lua_pushnumber(L, 0);
    } else if (result == 0) {
        lua_pushnumber(L, 1);
    } else {
        luaL_error(L, "Problem with dax_event_select() call");
    }
    return 1;
}

/* Wrapper for dax_event_poll() no argument  It returns 0 on if there are
 * no events and 1 if it dispatches an event */
static int
_event_poll(lua_State *L) {
    int result;

    result = dax_event_poll(ds, NULL);

    if(result == ERR_NOTFOUND) {
        lua_pushnumber(L, 0);
    } else if (result == 0) {
        lua_pushnumber(L, 1);
    } else {
        luaL_error(L, "Problem with dax_event_poll() returned %d", result);
    }
    return 1;
}


/* This is used by C program / modules that would like to take care of all
 * the allocation, initialization and configuration of their dax_state
 * objects.  It is very critical for that C function not to lose track of
 * this dax_state object because this simply resets the libraries global
 * variable. */
int
daxlua_set_state(lua_State *L, dax_state *new_ds) {
    ds = new_ds;
    return 0;
}

/* This array defines the functions that can be exported to a Lua script */
static const struct luaL_Reg daxlib[] = {
    {"dax_init", _dax_init},
    {"dax_free", _dax_free},
    {"cdt_create", _cdt_create},
    {"tag_add", _tag_add},
    {"tag_get", _tag_get},
    {"tag_read", _tag_read},
    {"tag_write", _tag_write},
    {"event_add", _event_add},
    {"event_del", _event_del},
    {"event_wait", _event_wait},
    {"event_poll", _event_poll},
    {NULL, NULL}  /* sentinel */
};

/* This registers all of the functions that are defined in the above array
 * to the Lua script given by L.  It places them in a table named 'dax' and
 * leaves this table on the top of the stack.  This is the function that you
 *  would pass to the 'package.loadlib()' function in the Lua script.  
 * C program modules should probably use daxlua_register_function(). */
/* TODO: Add a table, "tags" to "dax" that would be empty but utilize the
 * metamethods __index and __newindex to read and write the tags */
int
luaopen_dax (lua_State *L)
{
    char *modulename = (char *)lua_tostring(L, 1);
    luaL_register(L, modulename, daxlib);
    return 1;
}


/* This function is used by C program modules to register the individual
 * functions that it wants to be available to the lua_State. */
int
daxlua_register_function(lua_State *L, char *function_name) {
    int n = 0;

    if(strcmp(function_name, "all") == 0) {
        while(daxlib[n].name != NULL) {
            lua_pushcfunction(L, daxlib[n].func);
            lua_setglobal(L, daxlib[n].name);
            n++;
        }
    } else {
        while(daxlib[n].name != NULL) {
            if(strcmp(function_name, daxlib[n].name) == 0) {
                lua_pushcfunction(L, daxlib[n].func);
                lua_setglobal(L, daxlib[n].name);
            }
            n++;
        }
    }
    return 0;
}



