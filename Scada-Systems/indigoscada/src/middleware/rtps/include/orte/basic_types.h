#ifndef BASIC_TYPES_H
#define BASIC_TYPES_H 1


enum {
	CORBA_FALSE = 0,
	CORBA_TRUE  = 1
};

typedef int16_t   CORBA_short;
typedef int32_t   CORBA_long;
typedef uint16_t  CORBA_unsigned_short;
typedef uint32_t  CORBA_unsigned_long;
typedef float     CORBA_float;
typedef double    CORBA_double;
typedef int8_t    CORBA_char;
typedef int16_t   CORBA_wchar;
typedef uint8_t   CORBA_boolean;
typedef uint8_t   CORBA_octet;
typedef double    CORBA_long_double;

/*
 * Bad hack, oh well
 */

typedef CORBA_char  *CORBA_string;
typedef CORBA_wchar *CORBA_wstring;

#endif
