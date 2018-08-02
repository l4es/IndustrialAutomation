///APA added to fastdb distribution in server dir. Fasdb version ....
//This file shares importants tokens enums between server and client part
#ifndef FASDB_ENUMS_H
#define FASDB_ENUMS_H

BEGIN_FASTDB_NAMESPACE

#ifndef __COMPILER_H__
//Keep updated with///////enum tokens///////in compiler.h in FastDB server dir/////////////////////////////
enum tokens { 
    tkn_ident,
    tkn_lpar,
    tkn_rpar,
    tkn_lbr,
    tkn_rbr,
    tkn_dot,
    tkn_comma,
    tkn_power,
    tkn_iconst,
    tkn_sconst,
    tkn_fconst,
    tkn_add,
    tkn_sub,
    tkn_mul,
    tkn_div,
    tkn_and,
    tkn_or,
    tkn_not,
    tkn_null,
    tkn_neg,
    tkn_eq,
    tkn_ne,
    tkn_gt,
    tkn_ge,
    tkn_lt,
    tkn_le,
    tkn_between,
    tkn_escape,
    tkn_exists,
    tkn_like,
    tkn_limit,
    tkn_in,
    tkn_length,
    tkn_lower,
    tkn_upper,
    tkn_abs,
    tkn_area,
    tkn_is,
    tkn_integer,
    tkn_real,
    tkn_string,
    tkn_wstring,
    tkn_first,
    tkn_last,
    tkn_current,
    tkn_var,
    tkn_col,
    tkn_true,
    tkn_false,
    tkn_where, 
    tkn_follow,
    tkn_start,
    tkn_from,
    tkn_order,
    tkn_overlaps,
    tkn_by,
    tkn_asc,
    tkn_desc, 
    tkn_eof,
    tkn_insert, 
    tkn_into, 
    tkn_select, 
    tkn_table,
    tkn_error,
    tkn_all, 
    tkn_match,
    tkn_last_token
};    

#endif //__COMPILER_H__

///////enum SubSqlTokens///////from subsql.h of fastdb distribution/////////////////////////////

enum SubSqlTokens { 
    tkn_alter = tkn_last_token,
    tkn_array,
    tkn_autoincrement,
    tkn_autocommit,
    tkn_backup,
    tkn_bool,
    tkn_commit,
    tkn_compactify,
    tkn_count,
    tkn_create,
    tkn_delete,
    tkn_describe,
    tkn_drop,
    tkn_exit,
    tkn_export,
    tkn_hash,
    tkn_help,
    tkn_http,
    tkn_import,
    tkn_index,
    tkn_int1,
    tkn_int2,
    tkn_int4,
    tkn_int8,
    tkn_inverse,
    tkn_memory, 
    tkn_of,
    tkn_off,
    tkn_on,
    tkn_open,
    tkn_profile, 
    tkn_real4, 
    tkn_real8, 
    tkn_rectangle,
    tkn_reference,
    tkn_rename,
    tkn_rollback,
    tkn_server,
    tkn_set,
    tkn_stop,
    tkn_semi,
    tkn_show,
    tkn_to, 
    tkn_update,
    tkn_values,
    tkn_version,
	//tkn_insert_blob,//APA
	//tkn_get_blob,//APA
	//tkn_delete_blob,//APA
	tkn_close  //APA
};


#ifndef __CLASS_H__

//copied from class.h in FastDB server dir
/**
 * Types of field index
 */
enum dbIndexType { 
    HASHED  = 1,                   // hash table
    INDEXED = 2,                   // T-tree

    DB_FIELD_CASCADE_DELETE = 8,   // Used by OWNER macro, do not set it explicitly

    AUTOINCREMENT = 16,            // field is assigned automaticall incremented value

    DB_TIMESTAMP = 256,            // field is used as timestamp (this flag is used by CLI to perfrom proper mapping,
                                   // it is not used by C++ API)
    DB_FIELD_INHERITED_MASK = ~(HASHED|INDEXED)
};

#endif //__CLASS_H__

//from compiler.cpp in FastDB server dir

    static struct { 
        char* name;
        int   tag;
    } keywords[] = { 

#ifndef __COMPILER_H__
        {"all",     tkn_all},
        {"abs",     tkn_abs},
        {"and",     tkn_and},
#ifndef AREA_IS_IDENTIFIER
        {"area",    tkn_area},
#endif
        {"asc",     tkn_asc},
        {"between", tkn_between},
        {"by",      tkn_by},
        {"current", tkn_current},
        {"desc",    tkn_desc},
        {"escape",  tkn_escape},
        {"exists",  tkn_exists},
        {"first",   tkn_first},
        {"false",   tkn_false},
        {"follow",  tkn_follow},
        {"from",    tkn_from},
        {"in",      tkn_in},
        {"is",      tkn_is},
        {"integer", tkn_integer},
        {"insert",  tkn_insert},
        {"into",    tkn_into},
        {"last",    tkn_last},
        {"length",  tkn_length},
        {"like",    tkn_like},
        {"limit",   tkn_limit},
        {"lower",   tkn_lower},
#ifdef USE_REGEX
        {"match",   tkn_match},
#endif
        {"not",     tkn_not},
        {"null",    tkn_null},
        {"or",      tkn_or},
        {"order",   tkn_order},
        {"overlaps",tkn_overlaps},
        {"real",    tkn_real},
        {"select",  tkn_select},
        {"start",   tkn_start},
        {"string",  tkn_string},
        {"table",   tkn_table},
        {"true",    tkn_true},
        {"upper",   tkn_upper},
        {"where",   tkn_where},
        {"wstring", tkn_wstring},
#endif //__COMPILER_H__

	//from subsql.cpp of fastdb distribution
        {"alter",   tkn_alter},
        {"array",   tkn_array},
        {"autocommit",   tkn_autocommit},
        {"autoincrement",tkn_autoincrement},
        {"backup",  tkn_backup},
        {"bool",    tkn_bool},
        {"commit",  tkn_commit},
        {"compactify",tkn_compactify},
        {"count",   tkn_count},
        {"create",  tkn_create},
        {"delete",  tkn_delete},
        {"describe",tkn_describe},
        {"drop",    tkn_drop},
        {"exit",    tkn_exit},
        {"export",  tkn_export},
        {"hash",    tkn_hash},
        {"help",    tkn_help},
        {"http",    tkn_http},        
        {"import",  tkn_import},
        {"index",   tkn_index},
        {"inverse", tkn_inverse},
        {"int1",    tkn_int1},
        {"int2",    tkn_int2},
        {"int4",    tkn_int4},
        {"int8",    tkn_int8},
        {"of",      tkn_of},
        {"off",     tkn_off},
        {"on",      tkn_on},
        {"open",    tkn_open},
        {"reference",tkn_reference},
        {"real4",   tkn_real4},
        {"real8",   tkn_real8},
        {"rectangle", tkn_rectangle},
        {"rollback",tkn_rollback},
        {"server",  tkn_server},
        {"set",     tkn_set},
        {"start",   tkn_start},
        {"stop",    tkn_stop},
        {"show",    tkn_show},
        {"to",      tkn_to},
        {"update",  tkn_update},
        {"values",  tkn_values},
        {"version", tkn_version},
        {"memory",  tkn_memory}, //only in fastdb
        {"profile", tkn_profile}, //only in fastdb        
        {"rename",  tkn_rename}, //only in fastdb

//		{"insertblob",tkn_insert_blob},//APA
//		{"getblob",   tkn_get_blob},//APA
//		{"deleteblob",tkn_delete_blob},//APA
		{"close",     tkn_close}//APA
};

END_FASTDB_NAMESPACE

#endif