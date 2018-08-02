#ifndef SYSSTATH_
#define SYSSTATH_

#include "sys/types.h"

#define _need_time_t
#include "sys/stdtypes.h"
#undef _need_time_t

struct  stat {
        dev_t   st_dev;
        ino_t   st_ino;
        mode_t  st_mode;
        short   st_nlink;
        uid_t   st_uid;
        gid_t   st_gid;
        dev_t   st_rdev;
        off_t   st_size;
        time_t  st_atime;
        int     st_spare1;
        time_t  st_mtime;
        int     st_spare2;
        time_t  st_ctime;
        int     st_spare3;
        long    st_blksize;
        long    st_blocks;
        long    st_spare4[2];
};

#define S_IREAD         S_IRUSR /* read permission, owner */
#define S_IWRITE        S_IWUSR /* write permission, owner */
#define S_IEXEC         S_IXUSR /* execute/search permission, owner */

/* POSIX.1 STUFF */

#define S_IRGRP 00040           /* read permission: group */
#define S_IROTH 00004           /* read permission: other */
#define S_IRUSR 00400           /* read permission: owner */
#define S_IRWXG 00070           /* read, write, execute: group */
#define S_IRWXO 00007           /* read, write, execute: other */
#define S_IRWXU 00700           /* read, write, execute: owner */
#define S_ISBLK(mode)   ((mode&0xF000) == 0x6000)
#define S_ISCHR(mode)   ((mode&0xF000) == 0x2000)
#define S_ISDIR(mode)   ((mode&0xF000) == 0x4000)
#define S_ISFIFO(mode)  ((mode&0xF000) == 0x1000)
#define S_ISGID         0x400   /* set group id on execution */
#define S_ISREG(mode)   ((mode&0xF000) == 0x8000)
#define S_ISUID         0x800   /* set user id on execution */
#define S_IWGRP 00020           /* write permission: group */
#define S_IWOTH 00002           /* write permission: other */
#define S_IWUSR 00200           /* write permission: owner */
#define S_IXGRP 00010           /* execute permission: group */
#define S_IXOTH 00001           /* execute permission: other */
#define S_IXUSR 00100           /* execute permission: owner */


/* */

int chmod(const char * path, mode_t mode);   
int fstat(int filedes, struct stat *buf);
int mkdir(const char *path, mode_t mode);
int mkfifo(const char * path, mode_t mode);
int stat(const char *file_name, struct stat *buf);
mode_t umask(mode_t cmask);



#if 0

/* THESE ARE NOT CURRENTLY USED */

   
#define S_ENFMT         0002000 /* enforcement-mode locking */
#define S_IEXEC         0000100 /* execute/search permission, owner */
#define S_IFBLK         _IFBLK
#define S_IFCHR         _IFCHR
#define S_IFDIR         _IFDIR
#define S_IFIFO         _IFIFO
#define S_IFLNK         _IFLNK
#define S_IFMT          _IFMT
#define S_IFREG         _IFREG
#define S_IFSOCK        _IFSOCK
#define S_ISLNK(m)      (((m)&_IFMT) == _IFLNK)
#define S_ISSOCK(m)     (((m)&_IFMT) == _IFSOCK)
#define S_ISVTX         0001000 /* save swapped text even after use */

#define _IFBLK  0060000 /* block special */
#define _IFCHR  0020000 /* character special */
#define _IFDIR  0040000 /* directory */
#define _IFIFO  0010000 /* fifo */
#define _IFLNK  0120000 /* symbolic link */
#define _IFMT   0170000 /* type of file */
#define _IFREG  0100000 /* regular */
#define _IFSOCK 0140000 /* socket */

#endif



#endif






