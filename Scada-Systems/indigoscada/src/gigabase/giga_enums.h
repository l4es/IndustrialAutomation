///APA added to gigabase distribution in server dir version 3.77
//This file shares importants tokens enums between server and client part
#ifndef GIGA_ENUMS_H
#define GIGA_ENUMS_H

BEGIN_GIGABASE_NAMESPACE

#ifndef __COMPILER_H__
//Keep apdated with///////enum tokens///////in compiler.h in GigaBASE server dir/////////////////////////////
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
    tkn_all,
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
    tkn_last_token
};

#endif //__COMPILER_H__

///////enum SubSqlTokens///////from subsql.h of gigabase distribution/////////////////////////////

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
    tkn_of,
    tkn_off,
    tkn_on,
    tkn_open,
    tkn_real4,
    tkn_real8,
    tkn_rectangle,
    tkn_reference,
    tkn_restore,
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
	tkn_insert_blob,//APA
	tkn_get_blob,//APA
	tkn_delete_blob,//APA
	tkn_close  //APA
};

#ifndef __CLASS_H__

//copied from class.h in GigaBASE server dir
/**
 * Types of field index
 */
enum dbIndexType {
    HASHED  = 1, // hash table
    INDEXED = 2, // B-tree
    CASE_INSENSITIVE = 4, // Index is case insensetive

    DB_FIELD_CASCADE_DELETE = 8,   // Used by OWNER macro, do not set it explicitly
    UNIQUE = 16, // should be used in conjunction with HASHED or INDEXED - unique constraint 

    AUTOINCREMENT = 32, // field is assigned automaticall incremented value
    OPTIMIZE_DUPLICATES = 64,    // index with lot of duplicate key values

    DB_FIELD_INHERITED_MASK = ~(HASHED|INDEXED)
};

#endif //__CLASS_H__


static struct {
	char_t* name;
	int     tag;
    } keywords[] = {

#ifndef __COMPILER_H__
	//from compiler.cpp  of gigabase distribution
        {STRLITERAL("all"),     tkn_all},
        {STRLITERAL("abs"),     tkn_abs},
        {STRLITERAL("and"),     tkn_and},
        {STRLITERAL("area"),    tkn_area},
        {STRLITERAL("asc"),     tkn_asc},
        {STRLITERAL("between"), tkn_between},
        {STRLITERAL("by"),      tkn_by},
        {STRLITERAL("current"), tkn_current},
        {STRLITERAL("desc"),    tkn_desc},
        {STRLITERAL("escape"),  tkn_escape},
        {STRLITERAL("exists"),  tkn_exists},
        {STRLITERAL("first"),   tkn_first},
        {STRLITERAL("false"),   tkn_false},
        {STRLITERAL("follow"),  tkn_follow},
        {STRLITERAL("from"),    tkn_from},
        {STRLITERAL("in"),      tkn_in},
        {STRLITERAL("insert"),  tkn_insert},
        {STRLITERAL("into"),    tkn_into},
        {STRLITERAL("is"),      tkn_is},
        {STRLITERAL("integer"), tkn_integer},
        {STRLITERAL("last"),    tkn_last},
        {STRLITERAL("length"),  tkn_length},
        {STRLITERAL("like"),    tkn_like},
        {STRLITERAL("limit"),   tkn_limit},
        {STRLITERAL("lower"),   tkn_lower},
        {STRLITERAL("not"),     tkn_not},
        {STRLITERAL("null"),    tkn_null},
        {STRLITERAL("or"),      tkn_or},
        {STRLITERAL("order"),   tkn_order},
        {STRLITERAL("overlaps"),tkn_overlaps},
        {STRLITERAL("real"),    tkn_real},
        {STRLITERAL("select"),  tkn_select},
        {STRLITERAL("start"),   tkn_start},
        {STRLITERAL("string"),  tkn_string},
        {STRLITERAL("table"),   tkn_table},
        {STRLITERAL("true"),    tkn_true},
        {STRLITERAL("upper"),   tkn_upper},
        {STRLITERAL("where"),   tkn_where},
#endif //__COMPILER_H__

	//from subsql.cpp of gigabase distribution
        {STRLITERAL("alter"),   tkn_alter},
        {STRLITERAL("array"),   tkn_array},
        {STRLITERAL("autocommit"),   tkn_autocommit},
        {STRLITERAL("autoincrement"),tkn_autoincrement},
        {STRLITERAL("backup"),  tkn_backup},
        {STRLITERAL("bool"),    tkn_bool},
        {STRLITERAL("commit"),  tkn_commit},
        {STRLITERAL("compactify"),tkn_compactify},
        {STRLITERAL("count"),   tkn_count},
        {STRLITERAL("create"),  tkn_create},
        {STRLITERAL("delete"),  tkn_delete},
        {STRLITERAL("describe"),tkn_describe},
        {STRLITERAL("drop"),    tkn_drop},
        {STRLITERAL("exit"),    tkn_exit},
        {STRLITERAL("export"),  tkn_export},
        {STRLITERAL("hash"),    tkn_hash},
        {STRLITERAL("help"),    tkn_help},
        {STRLITERAL("http"),    tkn_http},        
        {STRLITERAL("import"),  tkn_import},
        {STRLITERAL("index"),   tkn_index},
        {STRLITERAL("inverse"), tkn_inverse},
        {STRLITERAL("int1"),    tkn_int1},
        {STRLITERAL("int2"),    tkn_int2},
        {STRLITERAL("int4"),    tkn_int4},
        {STRLITERAL("int8"),    tkn_int8},
        {STRLITERAL("of"),      tkn_of},
        {STRLITERAL("off"),     tkn_off},
        {STRLITERAL("on"),      tkn_on},
        {STRLITERAL("open"),    tkn_open},
        {STRLITERAL("reference"),tkn_reference},
        {STRLITERAL("real4"),   tkn_real4},
        {STRLITERAL("real8"),   tkn_real8},
        {STRLITERAL("rectangle"), tkn_rectangle},
        {STRLITERAL("restore"), tkn_restore},
        {STRLITERAL("rollback"),tkn_rollback},
        {STRLITERAL("server"),  tkn_server},
        {STRLITERAL("set"),     tkn_set},
        {STRLITERAL("start"),   tkn_start},
        {STRLITERAL("stop"),    tkn_stop},
        {STRLITERAL("show"),    tkn_show},
        {STRLITERAL("to"),      tkn_to},
        {STRLITERAL("update"),  tkn_update},
        {STRLITERAL("values"),  tkn_values},
        {STRLITERAL("version"), tkn_version},

		{STRLITERAL("insertblob"),tkn_insert_blob},//APA
		{STRLITERAL("getblob"),   tkn_get_blob},//APA
		{STRLITERAL("deleteblob"),tkn_delete_blob},//APA
		{STRLITERAL("close"),     tkn_close}//APA
};

END_GIGABASE_NAMESPACE

#endif