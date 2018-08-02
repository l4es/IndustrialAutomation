#ifndef _SYS_TYPES
#define _SYS_TYPES


#define _need_size_t
#include "sys/stdtypes.h"
#undef _need_size_t

#if 0 /* used for libc5 */

typedef int pid_t;          /* process id variable */
typedef int ssize_t;
typedef unsigned long ino_t;
typedef long off_t;
typedef unsigned short uid_t;
typedef unsigned short gid_t;
typedef unsigned short dev_t;
typedef unsigned short mode_t;
typedef unsigned short umode_t;
typedef unsigned short nlink_t;

/*typedef int daddr_t; ??*/

#else /* libc6 definitions */

typedef int pid_t;          /* process id variable */
typedef int ssize_t;
typedef unsigned long ino_t;
typedef long off_t;
typedef unsigned uid_t;
typedef unsigned gid_t;
typedef unsigned mode_t;
typedef unsigned umode_t;
typedef unsigned nlink_t;


#ifdef __GNUC__

typedef unsigned long long int __u_quad_t;

#else

typedef struct
  {
    long int __val[2];
  } __u_quad_t;

#endif

typedef __u_quad_t dev_t;

#endif 


#endif


