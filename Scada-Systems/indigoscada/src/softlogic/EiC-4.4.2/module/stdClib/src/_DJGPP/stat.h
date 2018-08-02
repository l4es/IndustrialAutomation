#ifndef SYSSTATH_
#define SYSSTATH_

#define _need_time_t
#include "sys/stdtypes.h"
#undef _need_time_t

struct stat {
  time_t	st_atime;
  time_t	st_ctime;
  dev_t		st_dev;
  gid_t		st_gid;
  ino_t		st_ino;
  mode_t	st_mode;
  time_t	st_mtime;
  nlink_t	st_nlink;
  off_t		st_size;
  off_t		st_blksize;
  uid_t		st_uid;
  dev_t		st_rdev; /* unused */
};

int	chmod(const char *_path, mode_t _mode);
int	fstat(int _fildes, struct stat *_buf);
int	mkdir(const char *_path, mode_t _mode);
int	mkfifo(const char *_path, mode_t _mode);
int	stat(const char *_path, struct stat *_buf);
mode_t	umask(mode_t _cmask);

#define S_ISBLK(m)	(((m) & 0xf000) == 0x1000)
#define S_ISCHR(m)	(((m) & 0xf000) == 0x2000)
#define S_ISDIR(m)	(((m) & 0xf000) == 0x3000)
#define S_ISFIFO(m)	(((m) & 0xf000) == 0x4000)
#define S_ISREG(m)	(((m) & 0xf000) == 0x0000)

#define S_ISUID		0x80000000
#define S_ISGID		0x40000000

#define S_IRUSR		00400
#define S_IRGRP		00040
#define S_IROTH		00004
#define S_IWUSR		00200
#define S_IWGRP		00020
#define S_IWOTH		00002
#define S_IXUSR		00100
#define S_IXGRP		00010
#define S_IXOTH		00001
#define S_IRWXU		00700
#define S_IRWXG		00070
#define S_IRWXO		00007


#endif /*  end SYSSTATH_ */





















