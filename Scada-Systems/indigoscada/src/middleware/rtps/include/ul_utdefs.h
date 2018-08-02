/*******************************************************************
  uLan Utilities Library - C library of basic reusable constructions

  ul_utdefs.h	- common defines used in uLan utilities library

 *******************************************************************/


#ifndef _UL_UTDEFS_H
#define _UL_UTDEFS_H

#ifdef __cplusplus
extern "C" {
#endif

#if defined(_WIN32)&&defined(_MSC_VER)&&!defined(inline)
#define inline _inline
#endif

#ifndef UL_OFFSETOF
/* offset of structure field */
#define UL_OFFSETOF(_type,_member) \
                ((size_t)&(((_type*)0)->_member))
#endif /*UL_OFFSET*/

#ifndef UL_CONTAINEROF
#ifdef  __GNUC__
#define UL_CONTAINEROF(_ptr, _type, _member) ({ \
        const typeof( ((_type *)0)->_member ) *__mptr = (_ptr); \
        (_type *)( (char *)__mptr - UL_OFFSETOF(_type,_member) );})
#else /*!__GNUC__*/
#define UL_CONTAINEROF(_ptr, _type, _member) \
        ((_type *)( (char *)_ptr - UL_OFFSETOF(_type,_member)))
#endif /*__GNUC__*/
#endif /*UL_CONTAINEROF*/

#ifndef ul_cyclic_gt
#define ul_cyclic_gt(x,y) \
	((sizeof(x)>=sizeof(long long))&&(sizeof(y)>=sizeof(long long))? \
		(long long)((long long)(x)-(long long)(y))>0: \
	 (sizeof(x)>=sizeof(long))&&(sizeof(y)>=sizeof(long))? \
	 	(long)((long)(x)-(long)(y))>0: /* x,y casts to suppress warnings only*/ \
	 (sizeof(x)>=sizeof(int))&&(sizeof(y)>=sizeof(int))?(int)((x)-(y))>0: \
	 (sizeof(x)>=sizeof(short))&&(sizeof(y)>=sizeof(short))?(short)((x)-(y))>0: \
	 (signed char)((x)-(y))>0 \
	)
#endif /*ul_cyclic_gt*/

#ifndef ul_cyclic_ge
#define ul_cyclic_ge(x,y) \
	((sizeof(x)>=sizeof(long long))&&(sizeof(y)>=sizeof(long long))? \
		(long long)((long long)(x)-(long long)(y))>=0: \
	 (sizeof(x)>=sizeof(long))&&(sizeof(y)>=sizeof(long))? \
	 	(long)((long)(x)-(long)(y))>=0: /* x,y casts to suppress warnings only*/ \
	 (sizeof(x)>=sizeof(int))&&(sizeof(y)>=sizeof(int))?(int)((x)-(y))>=0: \
	 (sizeof(x)>=sizeof(short))&&(sizeof(y)>=sizeof(short))?(short)((x)-(y))>=0: \
	 (signed char)((x)-(y))>=0 \
	)
#endif /*ul_cyclic_ge*/

/* GNUC neat features */

#ifdef	__GNUC__
#ifndef UL_ATTR_UNUSED
#define UL_ATTR_PRINTF( format_idx, arg_idx )	\
  __attribute__((format (printf, format_idx, arg_idx)))
#define UL_ATTR_SCANF( format_idx, arg_idx )	\
  __attribute__((format (scanf, format_idx, arg_idx)))
#define UL_ATTR_FORMAT( arg_idx )		\
  __attribute__((format_arg (arg_idx)))
#define UL_ATTR_NORETURN			\
  __attribute__((noreturn))
#define UL_ATTR_CONST				\
  __attribute__((const))
#define	UL_ATTR_UNUSED				\
  __attribute__((unused))
#endif  /*UL_ATTR_UNUSED*/
#else	/* !__GNUC__ */
#ifndef UL_ATTR_UNUSED
#define UL_ATTR_PRINTF( format_idx, arg_idx )
#define UL_ATTR_SCANF( format_idx, arg_idx )
#define UL_ATTR_FORMAT( arg_idx )
#define UL_ATTR_NORETURN
#define UL_ATTR_CONST
#define UL_ATTR_UNUSED
#endif  /*UL_ATTR_UNUSED*/
#endif	/* !__GNUC__ */

#ifdef __cplusplus
} /* extern "C"*/
#endif

#endif /* _UL_UTDEFS_H */
