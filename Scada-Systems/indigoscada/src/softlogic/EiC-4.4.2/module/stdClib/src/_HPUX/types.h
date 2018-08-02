#ifndef _SYS_TYPES
#define _SYS_TYPES

#define _need_size_t
#include "sys/stdtypes.h"
#undef _need_size_t


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

#endif
