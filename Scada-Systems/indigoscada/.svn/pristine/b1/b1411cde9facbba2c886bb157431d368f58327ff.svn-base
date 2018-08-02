#ifndef SYSSTATH_
#define SYSSTATH_

#define _need_time_t
#include "sys/stdtypes.h"
#undef _need_time_t

#include "sys/time.h"

struct stat {
        dev_t		st_dev;		/* inode's device */
        ino_t		st_ino;		/* inode's number */
        mode_t		st_mode;	/* inode protection mode */
        nlink_t		st_nlink;	/* number of hard links */
        uid_t		st_uid;		/* user ID of the file's owner */
        gid_t		st_gid;		/* group ID of the file's group */
        dev_t		st_rdev;	/* device type */
	struct timespec	st_atimespec;	/* time of last access */
	struct timespec	st_mtimespec;	/* time of last data modification */
	struct timespec	st_ctimespec;	/* time of last file status change */
#if !defined(_EiC) || defined(__LP64__)
	off_t		st_size;	/* file size, in bytes */
	off_t		st_blocks;	/* blocks allocated for file */
#else
# if __LITTLE_ENDIAN__
	off_t		st_size;
	int		__pad0;
	int		st_blocks;
	int		__pad1;
# else
	int		__pad0;
	off_t		st_size;
	int		__pad1;
	int		st_blocks;
#endif
#endif
	unsigned	st_blksize;	/* optimal blocksize for I/O */
	unsigned	st_flags;	/* user defined flags for file */
	unsigned	st_gen;		/* file generation number */
};

#define st_atime	st_atimespec.tv_sec
#define st_atimensec	st_atimespec.tv_nsec
#define st_mtime	st_mtimespec.tv_sec
#define st_mtimensec	st_mtimespec.tv_nsec
#define st_ctime	st_ctimespec.tv_sec
#define st_ctimensec	st_ctimespec.tv_nsec

/* file type */
#define S_IFMT		0170000		/* type of file mask */
#define S_IFIFO		0010000		/* named pipe (fifo) */
#define S_IFCHR		0020000		/* character special */
#define S_IFDIR		0040000		/* directory */
#define S_IFBLK		0060000		/* block special */
#define S_IFREG		0100000		/* regular */
#define S_IFLNK		0120000		/* symbolic link */

/* file mode bits */
#define S_IRWXU		0000700		/* RWX mask for owner */
#define S_IRUSR		0000400		/* R for owner */
#define S_IWUSR		0000200		/* W for owner */
#define S_IXUSR		0000100		/* X for owner */
#define S_IRWXG		0000070		/* RWX mask for group */
#define S_IRGRP		0000040		/* R for group */
#define S_IWGRP		0000020		/* W for group */
#define S_IXGRP		0000010		/* X for group */
#define S_IRWXO		0000007		/* RWX mask for other */
#define S_IROTH		0000004		/* R for other */
#define S_IWOTH		0000002		/* W for other */
#define S_IXOTH		0000001		/* X for other */
#define S_ISUID		0004000		/* set user id on execution */
#define S_ISGID		0002000		/* set group id on execution */
#define S_ISVTX		0001000		/* save sapped text even after use */


#define S_ISFIFO(m)	(((m) & 0170000) == 0010000)	/* fifo */
#define S_ISCHR(m)	(((m) & 0170000) == 0020000)	/* char special */
#define S_ISDIR(m)	(((m) & 0170000) == 0040000)	/* directory */
#define S_ISBLK(m)	(((m) & 0170000) == 0060000)	/* block special */
#define S_ISREG(m)	(((m) & 0170000) == 0100000)	/* regular file */
#define S_ISLNK(m)	(((m) & 0170000) == 0120000)	/* symbolic link */

#define S_TYPEISMQ(buf)		(0)
#define S_TYPEISSEM(buf)	(0)
#define S_TYPEISSHM(buf)	(0)

int	chmod(const char *, mode_t);
int	fchmod(int, mode_t);
int	fstat(int, struct stat *);
int	lstat(const char *, struct stat *);
int	mkdir(const char *, mode_t);
int	mkfifo(const char *, mode_t);
int	mknod(const char *, mode_t, dev_t);
int	stat(const char *, struct stat *);
mode_t	umask(mode_t);


#endif
