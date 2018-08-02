#ifndef _SYS_TYPES
#define _SYS_TYPES

#define _need_size_t
#include "sys/stdtypes.h"
#undef _need_size_t

typedef int		pid_t;          /* process id */
typedef int		ssize_t;
typedef unsigned	ino_t;		/* inode number */
#ifdef _EiC
typedef long		off_t;		/* file offset */
#else
typedef long long	off_t;
#endif
typedef unsigned	uid_t;		/* user id */
typedef unsigned	gid_t;		/* group id */
typedef unsigned	mode_t;		/* permissions */
typedef unsigned	nlink_t;	/* link count */
typedef unsigned	dev_t;		/* device number */

#endif
