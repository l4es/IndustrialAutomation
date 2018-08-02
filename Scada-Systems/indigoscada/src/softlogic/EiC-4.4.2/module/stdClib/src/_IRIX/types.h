#ifndef _SYS_TYPES
#define _SYS_TYPES

#define _need_size_t
#include "sys/stdtypes.h"
#undef _need_size_t


typedef long pid_t;          /* process id variable */
typedef int ssize_t;
typedef unsigned long ino_t;
typedef long off_t;
typedef long uid_t;
typedef long gid_t;
typedef unsigned long dev_t;
typedef unsigned long mode_t;
typedef unsigned long umode_t;
typedef unsigned long nlink_t;

/*typedef int daddr_t; ??*/

#endif
