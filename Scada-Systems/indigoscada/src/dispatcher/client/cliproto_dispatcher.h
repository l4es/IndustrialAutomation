//-< CLIPROTO_DISPATCHER.H DERIVAED FROM CLIPROTO>H>----------------------------------------------------*--------*
// GigaBASE                  Version 1.0         (c) 1999  GARRET    *     ?  *
// (Post Relational Database Management System)                      *   /\|  *
//                                                                   *  /  \  *
//                          Created:     13-Jan-2000  K.A. Knizhnik  * / [] \ *
//                          Last update: 13-Jan-2000  K.A. Knizhnik  * GARRET *
//-------------------------------------------------------------------*--------*
// Client-server dispatcher communication protocol
//-------------------------------------------------------------------*--------*

#ifndef __CLIPROTO_H__
#define __CLIPROTO_H__

BEGIN_GIGABASE_NAMESPACE

enum cli_commands_dispatcher {
    cli_cmd_close_session,
    cli_cmd_login
};

static const int sizeof_type[] = {
    sizeof(cli_oid_t_dispatcher),
    sizeof(cli_bool_t_dispatcher),
    sizeof(cli_int1_t_dispatcher),
    sizeof(cli_int2_t_dispatcher),
    sizeof(cli_int4_t_dispatcher),
    sizeof(cli_int8_t_dispatcher),
    sizeof(cli_real4_t_dispatcher),
    sizeof(cli_real8_t_dispatcher),
    sizeof(cli_real8_t_dispatcher), // cli_decimal
    sizeof(char*), // cli_asciiz, 
    sizeof(char*), // cli_pasciiz,
    sizeof(char*), // cli_cstring,
    sizeof(cli_array_t_dispatcher), // cli_array_of_oid,
    sizeof(cli_array_t_dispatcher), // cli_array_of_bool,
    sizeof(cli_array_t_dispatcher), // cli_array_of_int1,
    sizeof(cli_array_t_dispatcher), // cli_array_of_int2,
    sizeof(cli_array_t_dispatcher), // cli_array_of_int4,
    sizeof(cli_array_t_dispatcher), // cli_array_of_int8,
    sizeof(cli_array_t_dispatcher), // cli_array_of_real4,
    sizeof(cli_array_t_dispatcher), // cli_array_of_real8, 
    sizeof(cli_array_t_dispatcher), // cli_array_of_decimal, 
    sizeof(cli_array_t_dispatcher), // cli_array_of_string,
    0, // cli_any,
    sizeof(cli_int8_t_dispatcher), // cli_datetime,
    sizeof(cli_int4_t_dispatcher), // cli_autoincrement,
    0  // cli_unknown
};

union cli_field_alignment_dispatcher {
    struct { char n; cli_oid_t_dispatcher v;  }  _cli_oid_t_dispatcher;
    struct { char n; cli_bool_t_dispatcher v;  } _cli_bool_t_dispatcher;
    struct { char n; cli_int1_t_dispatcher v;  } _cli_int1_t_dispatcher;
    struct { char n; cli_int2_t_dispatcher v;  } _cli_int2_t_dispatcher;
    struct { char n; cli_int4_t_dispatcher v;  } _cli_int4_t_dispatcher;
    struct { char n; cli_int8_t_dispatcher v;  } _cli_int8_t_dispatcher;
    struct { char n; cli_real4_t_dispatcher v; } _cli_real4_t_dispatcher;
    struct { char n; cli_real8_t_dispatcher v; } _cli_real8_t_dispatcher;
	struct { char n; cli_array_t_dispatcher v; } _cli_array_t_dispatcher;
    struct { char n; char*       v; } _cli_asciiz_t_dispatcher;
    struct { char n; cli_cstring_t_dispatcher v; } _cli_cstring_t_dispatcher;
};

#define CLI_ALIGNMENT(type) \
        (((char *)&(((union cli_field_alignment_dispatcher*)0)->_##type.v)) - ((char *)&(((union cli_field_alignment_dispatcher*)0)->_##type.n)))

static const int alignof_type[] = { 
    CLI_ALIGNMENT(cli_oid_t_dispatcher), 
    CLI_ALIGNMENT(cli_bool_t_dispatcher), 
    CLI_ALIGNMENT(cli_int1_t_dispatcher), 
    CLI_ALIGNMENT(cli_int2_t_dispatcher), 
    CLI_ALIGNMENT(cli_int4_t_dispatcher), 
    CLI_ALIGNMENT(cli_int8_t_dispatcher), 
    CLI_ALIGNMENT(cli_real4_t_dispatcher), 
    CLI_ALIGNMENT(cli_real8_t_dispatcher),
    CLI_ALIGNMENT(cli_real8_t_dispatcher),
    CLI_ALIGNMENT(cli_asciiz_t_dispatcher),
    CLI_ALIGNMENT(cli_asciiz_t_dispatcher),
    CLI_ALIGNMENT(cli_cstring_t_dispatcher),
    CLI_ALIGNMENT(cli_array_t_dispatcher), // cli_array_of_oid,
    CLI_ALIGNMENT(cli_array_t_dispatcher), // cli_array_of_bool,
    CLI_ALIGNMENT(cli_array_t_dispatcher), // cli_array_of_int1,
    CLI_ALIGNMENT(cli_array_t_dispatcher), // cli_array_of_int2,
    CLI_ALIGNMENT(cli_array_t_dispatcher), // cli_array_of_int4,
    CLI_ALIGNMENT(cli_array_t_dispatcher), // cli_array_of_int8,
    CLI_ALIGNMENT(cli_array_t_dispatcher), // cli_array_of_real4,
    CLI_ALIGNMENT(cli_array_t_dispatcher), // cli_array_of_real8, 
    CLI_ALIGNMENT(cli_array_t_dispatcher), // cli_array_of_decimal, 
    CLI_ALIGNMENT(cli_array_t_dispatcher), // cli_array_of_string,
    0, // cli_any,
    CLI_ALIGNMENT(cli_int8_t_dispatcher), // cli_datetime,
    CLI_ALIGNMENT(cli_int4_t_dispatcher), // cli_autoincrement,
    0  // cli_unknown
};


#if defined(__FreeBSD__)
#include <sys/param.h>
#include <netinet/in.h>
#define USE_HTON_NTOH
#elif defined(__linux__)
//
// At Linux inline assembly declarations of ntohl, htonl... are available
//
#include <netinet/in.h>
#define USE_HTON_NTOH
#else
#if defined(_WIN32) && _M_IX86 >= 400 && !defined(__BCPLUSPLUS__) && !defined(__MINGW32__)
#pragma warning(disable:4035) // disable "no return" warning
#ifdef __BORLANDC__
static
#else
inline 
#endif
int swap_bytes_in_dword(int val) {
    __asm {
          mov eax, val
          bswap eax
    }
}
#ifdef __BORLANDC__
static
#else
inline 
#endif
short swap_bytes_in_word(short val) {
    __asm {
          mov ax, val
          xchg al,ah
    }
}
#pragma warning(default:4035)
#define ntohl(w) swap_bytes_in_dword(w)
#define htonl(w) swap_bytes_in_dword(w)
#define ntohs(w) swap_bytes_in_word(w)
#define htons(w) swap_bytes_in_word(w)

#define USE_HTON_NTOH
#endif
#endif




inline char* pack2(char* dst, int2 val) {
    *dst++ = char(val >> 8);
    *dst++ = char(val);
    return dst;
}

inline char* pack2(char* dst, char const* src) {
    return pack2(dst, *(int2*)src);
}

inline void pack2(int2& val) {
#if BYTE_ORDER != BIG_ENDIAN
#ifdef USE_HTON_NTOH
    val = htons(val);
#else
    pack2((char*)&val, val);
#endif
#endif
}


inline char* pack4(char* dst, int4 val) {
    *dst++ = char(val >> 24);
    *dst++ = char(val >> 16);
    *dst++ = char(val >> 8);
    *dst++ = char(val);
    return dst;
}

inline char* pack4(char* dst, char const* src) {
    return pack4(dst, *(int4*)src);
}

inline void pack4(int4& val) {
#if BYTE_ORDER != BIG_ENDIAN
#ifdef USE_HTON_NTOH
    val = htonl(val);
#else
    pack4((char*)&val, val);
#endif
#endif
}


inline char* pack8(char* dst, char const* src) {
#if BYTE_ORDER == BIG_ENDIAN
    return pack4( pack4(dst, src), src + 4);
#else
    return pack4( pack4(dst, src + 4), src);
#endif
}

inline char* pack8(char* dst, db_int8 val) {
    return pack8(dst, (char*)&val);
}

inline char* pack_oid(char* dst, cli_oid_t_dispatcher oid)
{
    return (sizeof(oid) == 4) ? pack4(dst, oid) : pack8(dst, (char*)&oid);
}

#ifdef UNICODE
inline char* pack_str(char* dst, char_t const* src) { 
    char_t ch;
    do {
        ch = *src++;
        *dst++ = (char)(ch >> 8);
        *dst++ = (char)ch;
    } while (ch != '\0');
    return dst;
}
inline char* pack_str(char* dst, char_t const* src, int n) { 
    char_t ch;
    while (--n >= 0) { 
        ch = *src++;
        *dst++ = (char)(ch >> 8);
        *dst++ = (char)ch;
    }
    return dst;
}
#else
inline char* pack_str(char* dst, char const* src) { 
    while ((*dst++ = *src++) != '\0');
    return dst;
}
inline char* pack_str(char* dst, char const* src, int n) { 
    while (--n >= 0) { 
        *dst++ = *src++;
    }
    return dst;
}
#endif

inline int2 unpack2(char const* src) {
    nat1* s = (nat1*)src;
    return (s[0] << 8) + s[1];
}

inline char* unpack2(char* dst, char* src) {
    *(int2*)dst = unpack2(src);
    return src + 2;
}

inline void  unpack2(int2& val) {
#if BYTE_ORDER != BIG_ENDIAN
#ifdef USE_HTON_NTOH
    val = ntohs(val);
#else
    val = unpack2((char*)&val);
#endif
#endif
}


inline int4  unpack4(char const* src) {
    nat1* s = (nat1*)src;
    return (((((s[0] << 8) + s[1]) << 8) + s[2]) << 8) + s[3];
}

inline char* unpack4(char* dst, char* src) {
    *(int4*)dst = unpack4(src);
    return src + 4;
}

inline void unpack4(int4& val) {
#if BYTE_ORDER != BIG_ENDIAN
#ifdef USE_HTON_NTOH
    val = ntohl(val);
#else
    val = unpack4((char*)&val);
#endif
#endif
}

inline char* unpack8(char* dst, char* src) {
#if BYTE_ORDER == BIG_ENDIAN
    *(int4*)dst = unpack4(src);
    *((int4*)dst+1) = unpack4(src+4);
#else
    *(int4*)dst = unpack4(src+4);
    *((int4*)dst+1) = unpack4(src);
#endif
    return src + 8;
}

inline db_int8 unpack8(char* src) {
    db_int8 val;
    unpack8((char*)&val, src);
    return val;
}

inline cli_oid_t_dispatcher unpack_oid(char* src)
{
    cli_oid_t_dispatcher oid;
    if (sizeof(oid) == 4) {
        oid = unpack4(src);
    } else {
        unpack8((char*)&oid, src);
    }
    return oid;
}

#ifdef UNICODE
inline char* skip_str(char* p) {
    while (p[0] != 0 || p[1] != 0) {
        p += 2;
    }
    return p + 2;
}
inline char* unpack_str(char_t* dst, char* src) {
    char_t ch;
    do {
        ch = (src[0] << 8) | (src[1] & 0xFF);
        src += sizeof(char_t);
        *dst++ = ch;
    } while (ch != '\0');
    return src;
}
inline char* unpack_str(char_t* dst, char* src, int n) {
    char_t ch;
    while (--n >= 0) { 
        ch = (src[0] << 8) | (src[1] & 0xFF);
        src += sizeof(char_t);
        *dst++ = ch;
    } 
    return src;
}
inline char_t unpack_char(char const* p) { 
    return (p[0] << 8) | (p[1] & 0xFF);
}
#else
inline char* skip_str(char* p) {
    while (*p++ != 0);
    return p;
}
inline char* unpack_str(char* dst, char* src) {
    while ((*dst++ = *src++) != '\0');
    return src;
}
inline char* unpack_str(char* dst, char* src, int n) {
    while (--n >= 0) {
        *dst++ = *src++;
    }
    return src;
}
inline char_t unpack_char(char const* p) {
    return *p;
}
#endif

struct cli_request_dispatcher {
    int4 length;
    int4 cmd;
    int4 stmt_id;
#ifdef SECURE_SERVER
    int4 sig;
#endif

    void pack() {
#ifdef SECURE_SERVER
        int4 i, s = length + cmd + stmt_id; //APA patch change type of i and s from i to int4
        char *p = (char *)&length + sizeof(cli_request_dispatcher);
        for (i = 0; i < length - (int4)sizeof(cli_request_dispatcher); i++, p++) { //APA patch added (int4)
            s += (*p << 7) + (*p << 3) + i;
        }
        sig = s;
#endif
        pack4(length);
        pack4(cmd);
        pack4(stmt_id);
#ifdef SECURE_SERVER
        pack4(sig);
#endif
    }

    void unpack() {
        unpack4(length);
        unpack4(cmd);
        unpack4(stmt_id);
#ifdef SECURE_SERVER
        unpack4(sig);
#endif
    }
};

END_GIGABASE_NAMESPACE

#endif
